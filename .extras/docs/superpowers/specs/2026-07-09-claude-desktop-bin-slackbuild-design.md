# claude-desktop-bin SlackBuild — Design Spec

Date: 2026-07-09
Package: `claude-desktop-bin`
Type: binary repack of the official Anthropic Debian package

---

## Summary

Repackage Anthropic's official `claude-desktop` Debian `.deb` into a Slackware
`.txz`. Claude Desktop is the Electron desktop client for Claude.ai. Anthropic
publishes an official apt repository at `https://downloads.claude.ai/claude-desktop/apt/stable`
carrying prebuilt `amd64` and `arm64` `.deb` packages; this SlackBuild consumes
the `amd64` deb and re-lays its payload into a Slackware package. Build from
source is rejected: Anthropic ships an official signed binary, and rebuilding
the Electron/Chromium app offline is slow and fragile with no gain.

## Package facts

- `PRGNAM=claude-desktop-bin`
- `VERSION=1.18286.2` (latest amd64 in the apt repo at spec time)
- `SRCNAM=claude-desktop` (deb filename + installed `/usr/bin/claude-desktop`
  keep the upstream name; only the SlackBuild package name gets the `-bin`
  suffix, matching this repo's convention: megasync-bin, kitty-bin,
  opencode-bin, claude-code-bin)
- `ARCH`: **x86_64 only**. The apt repo also serves `arm64`, but this repo
  targets x86_64; the SlackBuild exits with an error on any other arch.
- `REQUIRES=""`. The Debian `Depends` (libgtk-3-0, libnss3, libnotify4,
  libatspi, libdrm, libgbm, libsecret, libxtst, xdg-utils, xdg-desktop-portal,
  libc6) are all satisfied by stock Slackware. Chromium's own libs
  (libEGL, libGLESv2, libffmpeg, libvulkan, libvk_swiftshader) are bundled
  inside the payload. No SBo dependencies.
- Name `claude-desktop-bin` is free on SlackBuilds.org (only `claude-code*`
  exists there). Does not clash with the unrelated `claude-code-bin` already in
  this repo.

## Source

- URL: `https://downloads.claude.ai/claude-desktop/apt/stable/pool/main/c/claude-desktop/claude-desktop_1.18286.2_amd64.deb`
- MD5SUM: `853168e12c721d6bc6ce9566bbd5348e`
- Size: ~138 MB deb, ~480 MB installed.

The download path is **versioned** (`pool/main/c/claude-desktop/claude-desktop_<version>_amd64.deb`),
so a given version's file is immutable and the MD5SUM is stable. This is unlike
megasync-bin's versionless URL: the checksum does not drift here.

## Payload layout (from the deb `data.tar.xz`)

```
usr/bin/claude-desktop                       -> ../lib/claude-desktop/claude-desktop (symlink)
usr/lib/claude-desktop/                       Electron app + bundled chromium
  claude-desktop                              main ELF binary (~208 MB)
  chrome-sandbox                              SUID sandbox helper (4755 in deb)
  chrome_crashpad_handler
  libEGL.so libGLESv2.so libffmpeg.so
  libvulkan.so.1 libvk_swiftshader.so
  *.pak  icudtl.dat  *.bin  locales/  resources/  version
  LICENSES.chromium.html  vk_swiftshader_icd.json
usr/share/applications/claude-desktop.desktop
usr/share/icons/hicolor/{16,32,48,128,256}x*/apps/claude-desktop.png
usr/share/doc/claude-desktop/copyright
usr/share/lintian/...                         (Debian-only, dropped)
```

The upstream `.desktop` is kept as-is: `Exec=claude-desktop %U`, registers the
`x-scheme-handler/claude` MIME handler and `NewChat` / `NewCode` desktop
actions. No Slackware-specific `.desktop` is written.

## What is discarded from the deb

The Debian maintainer scripts (`control.tar.xz`: postinst, postrm) are **not**
carried over. They do three Debian-specific things, none of which apply to
Slackware:

1. **apt repo registration** — writes the Anthropic apt source + keyring so
   `apt upgrade` tracks updates. Irrelevant on Slackware.
2. **AppArmor userns profile** — works around
   `kernel.apparmor_restrict_unprivileged_userns=1` on Ubuntu 24.04+, which
   blocks `CLONE_NEWUSER` for unconfined processes. Slackware does not set this
   restriction, so Chromium's userns/SUID sandbox works without the profile.
3. **chrome-sandbox setuid** — the only piece we must reproduce (see below).

`usr/share/lintian/` is dropped.

## chrome-sandbox setuid

Chromium's SUID sandbox helper must be owned root:root and mode `4755` for the
sandbox to initialize; without it Claude Desktop fails to start unless launched
with `--no-sandbox` (which weakens security).

**Decision:** set `chmod 4755` on `chrome-sandbox` in the SlackBuild itself,
inside `$PKG` before `makepkg`, so the package ships the setuid bit directly.
This tests two things at build time:

- whether `sbopkglint` flags the shipped setuid binary, and
- whether `makepkg` auto-extracts the setuid bit into an `install/doinst.sh`
  `chmod` line (some makepkg versions do this for non-0755 modes).

If linting objects or makepkg does not preserve the bit, fall back to a
`chmod 4755 usr/lib/claude-desktop/chrome-sandbox` line in `doinst.sh` (relative
path, no leading slash, so `--root`/`$ROOT` installs stay correct). The fallback
is a known-good path; the in-SlackBuild approach is tried first to learn
makepkg/lint behavior.

## SlackBuild flow

Base: SBo template + the megasync-bin binary-repack pattern in this repo.

1. `set -e`; honor `$TMP $OUTPUT $PKG $TAG`; ARCH guard (exit on non-x86_64).
2. Unpack the deb: `ar x $CWD/claude-desktop_${VERSION}_amd64.deb`, then
   `tar xf data.tar.xz -C $PKG`.
3. Remove Debian-only cruft: `usr/share/lintian`.
4. **No stripping.** These are prebuilt Electron/Chromium binaries; stripping
   risks breaking them and upstream ships them unstripped. Skip the strip pass.
   (This also means the "ELF libs are 0644" megasync gotcha does not bite: the
   deb already ships its libs 0755. A defensive `chmod 755` over the bundled
   `*.so` files is still applied in case a future deb regresses.)
5. `chmod 4755 usr/lib/claude-desktop/chrome-sandbox` (setuid, per above).
6. Install docs: `usr/doc/$PRGNAM-$VERSION/`, moving the upstream `copyright`
   there and adding the `.SlackBuild` script. Remove the Debian
   `usr/share/doc/claude-desktop` dir after copying.
7. Standard `find -L $PKG` chown/chmod cleanup block (root:root, sane dir/file
   modes) — applied **before** the chrome-sandbox chmod so it does not clobber
   the setuid bit. Order matters: perm-normalize first, setuid last.
8. Install `slack-desc` to `install/`, `doinst.sh` to `install/`.
9. `makepkg -l y -c n`.

## doinst.sh

Trimmed from the SBo template. Keep only the hooks the payload needs:

- `update-desktop-database` (has a `.desktop`)
- `update-mime-database` (registers `x-scheme-handler/claude`)
- `gtk-update-icon-cache` (ships hicolor icons)

No `config()` block (no config files shipped). The chrome-sandbox `chmod`
line is added here only as the fallback if step 5 above does not survive
`makepkg`/lint.

## nvchecker

Anthropic publishes no GitHub releases for the desktop app; the source of truth
is the apt repo `Packages` index. nvchecker has no native apt/deb source, so use
the `cmd` source to parse it:

```toml
[claude-desktop-bin]
source = "cmd"
cmd = "curl -fsSL https://downloads.claude.ai/claude-desktop/apt/stable/dists/stable/main/binary-amd64/Packages | awk '/^Version:/{print $2}' | sort -V | tail -1"
```

`awk` extracts every `Version:` value from the index; `sort -V | tail -1`
returns the highest. Verified to resolve to `1.18286.2` at spec time. Added to
both `.extras/nvchecker.toml` and `~/.config/nvchecker/nvchecker.toml`.

## Files produced

```
claude-desktop-bin/
  claude-desktop-bin.SlackBuild
  claude-desktop-bin.info    # single amd64 deb DOWNLOAD + MD5SUM, REQUIRES=""
  README
  slack-desc                 # official Anthropic Electron client, x86_64 only
  doinst.sh                  # desktop-db, mime-db, icon-cache (+ sandbox fallback)
```

Plus: nvchecker entry (both configs), README package-table row, a memory note.

## README notes

- Official Anthropic build (not a community port), Electron/Chromium based.
- x86_64 only; ~480 MB installed.
- chrome-sandbox ships setuid root for Chromium's sandbox.
- No SBo dependencies; all libs are stock Slackware or bundled.
- Registers the `claude://` URL scheme handler.

## Testing

- `sbolint` clean on the sources.
- Build on -current (buildsystem VM) and 15.0 (sbo-batch-test overlay).
- `sbopkglint` on the built package — note whether it flags the setuid
  chrome-sandbox, and whether makepkg moved the bit into doinst.sh.
- Launch on -current, confirm the app starts with the sandbox enabled (no
  `--no-sandbox`).
