# megasync-bin SlackBuild — Design

Date: 2026-07-06
Package: `megasync-bin`
Target: personal SBo-compatible repo (not SBo submission — binary repack)
Arch: x86_64 only (upstream ships no 32-bit)

## Goal

Package MEGA's official desktop client (MEGAsync) for Slackware by repacking
the upstream prebuilt Debian package, plus the dolphin and thunar file-manager
integration plugins, into a single Slackware package.

Building from source is impractical: upstream uses CMake + Qt5.15 + a MEGA SDK
git submodule + Microsoft vcpkg, which fetches and compiles ~30 deps at build
time (network-heavy, offline-illegal, hours-long Qt/C++ compile). MEGA ships
prebuilt binaries for deb/rpm distros; the binary links against system Qt5 and
bundles its own ffmpeg, so it drops cleanly onto stock Slackware.

## Naming

- `PRGNAM=megasync-bin` — avoids clash with the old SBo source `megasync`
  (SBo still has 4.5.3.0, last touched 2021). The `-bin` suffix follows the
  repo's existing pattern (claude-code-bin, kitty-bin, opencode-bin).
- Installed binary stays `/usr/bin/megasync` (upstream name preserved).

## Source

Versionless Debian 13 URLs (always serve latest; MD5SUM drifts by design,
re-checksum on nvchecker alert):

```
https://mega.nz/linux/repo/Debian_13/amd64/megasync-Debian_13_amd64.deb
https://mega.nz/linux/repo/Debian_13/amd64/dolphin-megasync-Debian_13_amd64.deb
https://mega.nz/linux/repo/Debian_13/amd64/thunar-megasync-Debian_13_amd64.deb
```

`.info` DOWNLOAD lists all three (space/backslash-separated), with three
matching MD5SUM entries in the same order. Current checksums:

```
megasync-Debian_13_amd64.deb          c2f596ea2043da9613104a47934957d8
dolphin-megasync-Debian_13_amd64.deb  0917cb242b0f388f1ab01f6d44b76891
thunar-megasync-Debian_13_amd64.deb   5270041c4b5b23990d522390fafa551e
```

VERSION = `6.4.0.2` (upstream release tag `v6.4.0.2_Linux`; the repo package
metadata calls it 6.4.0, the integration plugins lag at 6.2.0 — VERSION tracks
the main app).

## Dependencies

`REQUIRES=""`. Verified on stock Slackware (this machine, Qt5 5.15.19):

- Main binary: links system Qt5 (Core/Widgets/Quick/Gui/Qml/Network/Svg/DBus/
  QuickWidgets/X11Extras), libxcb; bundles ffmpeg in `/opt/megasync/lib`
  (RUNPATH). All QtQuick QML runtime modules present. `ldd` fully resolves.
- dolphin plugin: needs KF5 (libKF5ConfigCore/CoreAddons/KIOCore/KIOWidgets/
  WidgetsAddons) — present in stock Slackware. Loads only when dolphin runs.
- thunar plugin: needs thunarx-3 + glib — present in stock Slackware. Loads
  only when Thunar runs.

Plugins for an absent file manager are dead `.so` files: harmless, unused,
never loaded by megasync itself. So bundling all three in one package is safe
regardless of which file managers the user has installed.

## Package layout (from main deb, paths as-is unless noted)

```
/usr/bin/megasync
/usr/bin/mega-desktop-app-gfxworker
/opt/megasync/lib/libav*.so.*  libsw*.so.*      # bundled ffmpeg, keep RUNPATH
/usr/share/megasync/resources/*.rcc
/usr/share/applications/megasync.desktop         # upstream ships it
/usr/share/icons/hicolor/**/apps/mega.png
/usr/share/icons/hicolor/scalable/status/*.svg
/usr/share/icons/ubuntu-mono-dark/status/24/*.svg
/etc/sysctl.d/99-megasync-inotify-limit.conf     # ship as .new
/etc/udev/rules.d/99-megasync-udev.rules         # ship as .new
```

Integration plugins — remap Debian multiarch libdir to Slackware `/usr/lib64`:

```
dolphin:
  /usr/lib/x86_64-linux-gnu/qt5/plugins/kf5/overlayicon/megasync-overlay-plugin.so
  /usr/lib/x86_64-linux-gnu/qt5/plugins/kf5/kfileitemaction/megasync-plugin.so
    -> /usr/lib64/qt5/plugins/kf5/...
  /usr/share/icons/hicolor/**/emblems/mega-dolphin-*.png
thunar:
  /usr/lib/x86_64-linux-gnu/thunarx-3/libMEGAShellExtThunar.so
    -> /usr/lib64/thunarx-3/libMEGAShellExtThunar.so
```

## SlackBuild flow

1. Standard SBo header: set `PRGNAM=megasync-bin`, `SRCNAM=megasync`,
   `VERSION`, `ARCH`/`SLKCFLAGS`/`LIBDIRSUFFIX` detection (x86_64 only —
   `exit 1` on other arch).
2. Extract each of the three debs: `ar x <deb>` then `tar xf data.tar.*`
   into `$PKG` (or a staging dir then copy). Debs have no versioned top dir.
3. Relocate integration plugin `.so` files from
   `usr/lib/x86_64-linux-gnu/` to `usr/lib${LIBDIRSUFFIX}/`.
4. Rename the two `/etc` config files to `.new`.
5. Strip binaries and bundled libs (upstream does not discourage it).
   Preserve the `/opt/megasync/lib` RUNPATH on the main binary.
6. Install docs to `/usr/doc/$PRGNAM-$VERSION/` (README, plus upstream
   copyright). Drop the Debian `changelog.Debian.gz` and `copyright`
   from `/usr/share/doc/megasync`.
7. `slack-desc` (11-line), `$PKG/install/slack-desc`.
8. `find -L . ... chown/chmod` cleanup block.
9. `makepkg -l y -c n`.

## Repo integration

- `nvchecker.toml`: track the GitHub release tag.
  Tags look like `v6.4.0.2_Linux`; strip the `v` prefix and `_Linux`
  suffix to get `6.4.0.2` with a regex transform:
  ```toml
  [megasync-bin]
  source = "github"
  github = "meganz/MEGAsync"
  use_max_tag = true
  prefix = "v"
  from_pattern = '_Linux$'
  to_pattern = ''
  ```
  (Verify against `nvchecker` output when wiring it up; adjust the pattern
  if upstream tag format differs.)
- README version table: add a `megasync-bin` row.

## Testing

1. `sbolint` clean.
2. `sbodl` verifies all three MD5SUMs (will pass only until MEGA rebuilds).
3. Build with the SlackBuild; `sbopkglint` clean.
4. Install; run `megasync --version` / launch; confirm binary resolves
   (`ldd`), tray app starts.
5. Confirmed against -current (VM) and 15.0 as usual.

## Known gotchas

- Versionless download URL means MD5SUM goes stale on every upstream
  rebuild, not just version bumps. Expected for this `-bin`; re-checksum
  when nvchecker flags a new tag.
- Integration plugins version-lag (6.2.0) behind the main app (6.4.0) in
  upstream's repo; nothing we control.
- Not SBo-submittable (binary repack + versionless rolling source).
