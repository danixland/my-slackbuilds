# claude-desktop-bin SlackBuild Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Repackage Anthropic's official `claude-desktop` Debian `.deb` into a Slackware `.txz` package named `claude-desktop-bin`.

**Architecture:** Binary repack. Extract the deb's `data.tar.xz` into `$PKG`, drop the Debian maintainer scripts (apt-repo, AppArmor), reproduce only the chrome-sandbox setuid, and package with `makepkg`. Same pattern as the existing `megasync-bin` package in this repo.

**Tech Stack:** bash SlackBuild, `ar`/`tar` for deb extraction, `makepkg`, sbo-maintainer-tools (sbolint/sbodl/sbopkglint), nvchecker.

**Spec:** `.extras/docs/superpowers/specs/2026-07-09-claude-desktop-bin-slackbuild-design.md`

**Note on "tests":** this is a packaging repo, not a code repo. Verification is `sbolint` / `sbodl` / `sbopkglint` / build / launch, not a unit-test framework. Each task's verification step names the exact command and expected result.

---

## File Structure

All under a new `claude-desktop-bin/` package directory, plus repo-level edits:

```
claude-desktop-bin/
  claude-desktop-bin.SlackBuild   # build script (repack logic)
  claude-desktop-bin.info         # metadata: 1 amd64 deb DOWNLOAD + MD5SUM
  claude-desktop-bin.nvchecker    # cmd stanza, shipped into /usr/doc
  README                          # description, notes, NVCHECKER section
  slack-desc                      # 11-line package description
  doinst.sh                       # desktop-db, mime-db, icon-cache, sandbox chmod
.extras/nvchecker.toml            # add [claude-desktop-bin] stanza
~/.config/nvchecker/nvchecker.toml# add same stanza (local, not committed)
README.md                         # add package-table row
```

Package facts (from spec):
- `PRGNAM=claude-desktop-bin`, `SRCNAM=claude-desktop`, `VERSION=1.18286.2`
- x86_64 only, `REQUIRES=""`
- Source: `https://downloads.claude.ai/claude-desktop/apt/stable/pool/main/c/claude-desktop/claude-desktop_1.18286.2_amd64.deb`
- MD5SUM: `853168e12c721d6bc6ce9566bbd5348e`

---

### Task 1: Create the package directory and .info file

**Files:**
- Create: `claude-desktop-bin/claude-desktop-bin.info`

- [ ] **Step 1: Create the directory and .info file**

```bash
mkdir -p claude-desktop-bin
```

Create `claude-desktop-bin/claude-desktop-bin.info`:

```
PRGNAM="claude-desktop-bin"
VERSION="1.18286.2"
HOMEPAGE="https://claude.ai/download"
DOWNLOAD="https://downloads.claude.ai/claude-desktop/apt/stable/pool/main/c/claude-desktop/claude-desktop_1.18286.2_amd64.deb"
MD5SUM="853168e12c721d6bc6ce9566bbd5348e"
DOWNLOAD_x86_64=""
MD5SUM_x86_64=""
REQUIRES=""
MAINTAINER="danix"
EMAIL="danix@danix.xyz"
```

- [ ] **Step 2: Verify the download + checksum with sbodl**

Run:
```bash
cd claude-desktop-bin && sbodl ; cd ..
```
Expected: downloads the deb and reports `md5sum matches OK`. (~138 MB download.)
If it reports a mismatch, the repo has published a newer version: re-fetch the
latest `Version:`/`MD5sum:`/`Filename:` from
`https://downloads.claude.ai/claude-desktop/apt/stable/dists/stable/main/binary-amd64/Packages`,
update `VERSION`, `MD5SUM`, and the `DOWNLOAD` filename, then re-run.

- [ ] **Step 3: Remove the symlink sbodl created**

`sbodl` symlinks the downloaded deb into the package dir. It must not be committed.

Run:
```bash
find claude-desktop-bin -type l -delete
```
Expected: no output. The real deb stays in sbodl's cache; the symlink is gone.

---

### Task 2: Write the SlackBuild script

**Files:**
- Create: `claude-desktop-bin/claude-desktop-bin.SlackBuild`

- [ ] **Step 1: Write the SlackBuild**

Create `claude-desktop-bin/claude-desktop-bin.SlackBuild`:

```bash
#!/bin/bash

# Slackware build script for claude-desktop (binary repack)

# Copyright 2026 danix <danix@danix.xyz>
# All rights reserved.
#
# Redistribution and use of this script, with or without modification, is
# permitted provided that the following conditions are met:
#
# 1. Redistributions of this script must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
#  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
#  EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


cd $(dirname $0) ; CWD=$(pwd)

PRGNAM=claude-desktop-bin
SRCNAM=claude-desktop
VERSION=${VERSION:-1.18286.2}
BUILD=${BUILD:-1}
TAG=${TAG:-_SBo}
PKGTYPE=${PKGTYPE:-tgz}

if [ -z "$ARCH" ]; then
  case "$( uname -m )" in
    *) ARCH=$( uname -m ) ;;
  esac
fi

if [ ! -z "${PRINT_PACKAGE_NAME}" ]; then
  echo "$PRGNAM-$VERSION-$ARCH-$BUILD$TAG.$PKGTYPE"
  exit 0
fi

# Upstream ships x86_64 only.
if [ "$ARCH" != "x86_64" ]; then
  echo "$ARCH is not supported: Claude Desktop is x86_64 only."
  exit 1
fi

TMP=${TMP:-/tmp/SBo}
PKG=$TMP/package-$PRGNAM
OUTPUT=${OUTPUT:-/tmp}

set -e

rm -rf $PKG
mkdir -p $TMP $PKG $OUTPUT

# Extract the upstream .deb (an ar archive holding a data.tar.xz payload with
# the real file tree) into $PKG.  The Debian control scripts (postinst/postrm:
# apt-repo registration + AppArmor userns profile) are intentionally NOT
# carried over -- neither applies to Slackware.
( cd $TMP && ar x $CWD/${SRCNAM}_${VERSION}_amd64.deb data.tar.xz \
  && tar xf data.tar.xz -C $PKG && rm -f data.tar.xz )

# Drop Debian-only cruft.
rm -rf $PKG/usr/share/lintian

# Defensive: the deb ships its bundled chromium libs mode 0755 already, but
# guard against a future deb regressing to 0644 (the "ELF libs 0644" Debian
# convention that trips sbopkglint's 20-arch test).  Do NOT strip: these are
# prebuilt Electron/Chromium binaries and upstream ships them unstripped.
find $PKG/usr/lib/$SRCNAM -maxdepth 1 -name '*.so*' -type f -print0 \
  | xargs -0 -r chmod 755

# Docs: keep the upstream copyright, add the SlackBuild and nvchecker stanza.
mkdir -p $PKG/usr/doc/$PRGNAM-$VERSION
if [ -f $PKG/usr/share/doc/$SRCNAM/copyright ]; then
  cp -a $PKG/usr/share/doc/$SRCNAM/copyright $PKG/usr/doc/$PRGNAM-$VERSION/
fi
rm -rf $PKG/usr/share/doc
cat $CWD/README > $PKG/usr/doc/$PRGNAM-$VERSION/README
cat $CWD/$PRGNAM.SlackBuild > $PKG/usr/doc/$PRGNAM-$VERSION/$PRGNAM.SlackBuild
cat $CWD/$PRGNAM.nvchecker > $PKG/usr/doc/$PRGNAM-$VERSION/$PRGNAM.nvchecker

mkdir -p $PKG/install
cat $CWD/slack-desc > $PKG/install/slack-desc
cat $CWD/doinst.sh > $PKG/install/doinst.sh

chown -R root:root $PKG
find -L $PKG \
  \( -perm 777 -o -perm 775 -o -perm 750 -o -perm 711 -o -perm 555 \
   -o -perm 511 \) -exec chmod 755 {} \+ -o \
  \( -perm 666 -o -perm 664 -o -perm 640 -o -perm 600 -o -perm 444 \
   -o -perm 440 -o -perm 400 \) -exec chmod 644 {} \+

# chrome-sandbox needs setuid root (4755) for Chromium's SUID sandbox.  Set it
# LAST, after the find -L perm-normalize above would otherwise strip it.
chmod 4755 $PKG/usr/lib/$SRCNAM/chrome-sandbox

cd $PKG
/sbin/makepkg -l y -c n $OUTPUT/$PRGNAM-$VERSION-$ARCH-$BUILD$TAG.$PKGTYPE
```

- [ ] **Step 2: Make it executable and syntax-check**

Run:
```bash
chmod +x claude-desktop-bin/claude-desktop-bin.SlackBuild
bash -n claude-desktop-bin/claude-desktop-bin.SlackBuild && echo "syntax OK"
```
Expected: `syntax OK`.

---

### Task 3: Write the nvchecker stanza

**Files:**
- Create: `claude-desktop-bin/claude-desktop-bin.nvchecker`

- [ ] **Step 1: Write the nvchecker file**

Create `claude-desktop-bin/claude-desktop-bin.nvchecker`:

```toml
# Claude Desktop has no github/pypi release feed; the source of truth is the
# Anthropic apt repo Packages index. nvchecker has no native apt/deb source,
# so use the cmd source to parse the highest Version: out of the index.
[claude-desktop-bin]
source = "cmd"
cmd = "curl -fsSL https://downloads.claude.ai/claude-desktop/apt/stable/dists/stable/main/binary-amd64/Packages | awk '/^Version:/{print $2}' | sort -V | tail -1"
```

- [ ] **Step 2: Verify the command resolves a version**

Run:
```bash
curl -fsSL https://downloads.claude.ai/claude-desktop/apt/stable/dists/stable/main/binary-amd64/Packages | awk '/^Version:/{print $2}' | sort -V | tail -1
```
Expected: a dotted version string, `1.18286.2` or newer.

---

### Task 4: Write slack-desc

**Files:**
- Create: `claude-desktop-bin/slack-desc`

- [ ] **Step 1: Write slack-desc**

Create `claude-desktop-bin/slack-desc` (the `|` under the `:` must line up on the
`handy-ruler` line; exactly 11 description lines):

```
# HOW TO EDIT THIS FILE:
# The "handy ruler" below makes it easier to edit a package description.
# Line up the first '|' above the ':' following the base package name, and
# the '|' on the right side marks the last column you can put a character in.
# You must make exactly 11 lines for the formatting to be correct.  It's also
# customary to leave one space after the ':' except on otherwise blank lines.

                  |-----handy-ruler------------------------------------------------------|
claude-desktop-bin: claude-desktop-bin (official desktop client for Claude.ai)
claude-desktop-bin:
claude-desktop-bin: Claude Desktop is Anthropic's Electron desktop application for
claude-desktop-bin: Claude.ai, with support for the claude:// URL scheme, quick chat
claude-desktop-bin: and Claude Code session launchers.
claude-desktop-bin:
claude-desktop-bin: This is a binary repack of Anthropic's official Debian package.
claude-desktop-bin: It bundles its own Chromium runtime and is x86_64 only.
claude-desktop-bin:
claude-desktop-bin:
claude-desktop-bin: Homepage: https://claude.ai/download
```

- [ ] **Step 2: Verify line count and prefix**

Run:
```bash
grep -c '^claude-desktop-bin:' claude-desktop-bin/slack-desc
```
Expected: `11`.

---

### Task 5: Write doinst.sh

**Files:**
- Create: `claude-desktop-bin/doinst.sh`

- [ ] **Step 1: Write doinst.sh**

Create `claude-desktop-bin/doinst.sh`. The chrome-sandbox setuid is shipped in
the package (Task 2); this line is a belt-and-suspenders re-assert in case
`makepkg` did not preserve the setuid bit (relative path, no leading slash, so
`--root`/`$ROOT` installs stay correct):

```bash
if [ -x /usr/bin/update-desktop-database ]; then
  /usr/bin/update-desktop-database -q usr/share/applications >/dev/null 2>&1
fi

if [ -x /usr/bin/update-mime-database ]; then
  /usr/bin/update-mime-database usr/share/mime >/dev/null 2>&1
fi

if [ -e usr/share/icons/hicolor/icon-theme.cache ]; then
  if [ -x /usr/bin/gtk-update-icon-cache ]; then
    /usr/bin/gtk-update-icon-cache -f -q usr/share/icons/hicolor >/dev/null 2>&1
  fi
fi

# Chromium's SUID sandbox helper must be setuid root.  Re-assert in case the
# package did not preserve the 4755 bit.
chmod 4755 usr/lib/claude-desktop/chrome-sandbox 2>/dev/null
```

- [ ] **Step 2: Syntax-check**

Run:
```bash
bash -n claude-desktop-bin/doinst.sh && echo "syntax OK"
```
Expected: `syntax OK`.

---

### Task 6: Write README

**Files:**
- Create: `claude-desktop-bin/README`

- [ ] **Step 1: Write README**

Create `claude-desktop-bin/README` (SBo READMEs are plain text, wrapped ~72 cols):

```
Claude Desktop is Anthropic's official Electron desktop application for
Claude.ai. It bundles its own Chromium runtime, registers the claude://
URL scheme handler, and provides desktop actions for starting a new chat
or a new Claude Code session.

This is a binary repack of Anthropic's official Debian package pulled from
the Anthropic apt repository (https://downloads.claude.ai/claude-desktop).
No build from source is performed. The package is x86_64 only and installs
to roughly 480 MB.

The Debian package's apt-repository registration and AppArmor userns
profile are Debian/Ubuntu specific and are not reproduced here. Chromium's
SUID sandbox helper (chrome-sandbox) is shipped setuid root so the sandbox
works without --no-sandbox.

There are no SBo dependencies: the Debian runtime dependencies (gtk3, nss,
notify, atspi, drm, gbm, secret, xtst, xdg-utils, xdg-desktop-portal) are
all satisfied by a stock Slackware install, and Chromium's own libraries
are bundled inside the package.


NVCHECKER
---------
An nvchecker configuration file is installed alongside this README at:

  /usr/doc/claude-desktop-bin-VERSION/claude-desktop-bin.nvchecker

nvchecker (https://github.com/lilydjwg/nvchecker) is a tool for checking
new versions of software. Copy or include this file in your nvchecker
configuration to be notified when Anthropic publishes a new Claude Desktop
release.
```

- [ ] **Step 2: Verify no line exceeds 80 columns**

Run:
```bash
awk 'length > 80 {print FILENAME":"NR": "length" cols"}' claude-desktop-bin/README
```
Expected: no output.

---

### Task 7: Lint the sources

**Files:**
- All of `claude-desktop-bin/`

- [ ] **Step 1: Run sbolint**

Run:
```bash
cd claude-desktop-bin && sbolint ; cd ..
```
Expected: no errors. If sbolint flags the `.nvchecker` file as an unexpected
extra file, that is acceptable (r8125 and UrbanTerror ship the same and pass);
fix any real errors it reports about the SlackBuild, .info, README, or slack-desc.

---

### Task 8: Build the package

**Files:**
- Produces: `/tmp/claude-desktop-bin-1.18286.2-x86_64-1_SBo.tgz`

- [ ] **Step 1: Build**

Run (needs the deb present; sbodl in Task 1 fetched it — if the build cannot
find `claude-desktop_1.18286.2_amd64.deb` in the package dir, symlink or copy it
from sbodl's cache into `claude-desktop-bin/` first):
```bash
cd claude-desktop-bin && sudo bash claude-desktop-bin.SlackBuild ; cd ..
```
Expected: ends with `Slackware package .../claude-desktop-bin-1.18286.2-x86_64-1_SBo.tgz created.`

- [ ] **Step 2: Confirm chrome-sandbox setuid in the built package**

Run:
```bash
tar tvf /tmp/claude-desktop-bin-1.18286.2-x86_64-1_SBo.tgz \
  | grep chrome-sandbox
```
Expected: mode shows setuid, e.g. `-rwsr-xr-x root/root`. Also check whether
`makepkg` moved the bit into `install/doinst.sh` (some versions do); note which
happened for the commit message.

- [ ] **Step 3: Remove any symlink left in the package dir before staging**

Run:
```bash
find claude-desktop-bin -type l -delete
```
Expected: no output.

---

### Task 9: Lint the built package

**Files:**
- `/tmp/claude-desktop-bin-1.18286.2-x86_64-1_SBo.tgz`

- [ ] **Step 1: Run sbopkglint**

Run:
```bash
cd claude-desktop-bin && sbopkglint /tmp/claude-desktop-bin-1.18286.2-x86_64-1_SBo.tgz ; cd ..
```
Expected: green. A warning about the setuid `chrome-sandbox` is expected and
acceptable (it is required for the Chromium sandbox); any hard error about
non-executable ELF (the 20-arch test) means the `chmod 755` guard in the
SlackBuild needs to cover more files — investigate and fix.

---

### Task 10: Install and smoke-test on -current

**Files:** none

- [ ] **Step 1: Install**

Run:
```bash
sudo /sbin/upgradepkg --install-new /tmp/claude-desktop-bin-1.18286.2-x86_64-1_SBo.tgz
```
Expected: installs cleanly, doinst.sh runs.

- [ ] **Step 2: Confirm installed sandbox perms and launch**

Run:
```bash
ls -l /usr/lib/claude-desktop/chrome-sandbox
```
Expected: `-rwsr-xr-x 1 root root ...` (setuid preserved after install).

Then launch (from a graphical session):
```bash
claude-desktop
```
Expected: the app window opens with the sandbox enabled (no `--no-sandbox`
needed). Close it after confirming.

---

### Task 11: Add nvchecker stanza to the repo config

**Files:**
- Modify: `.extras/nvchecker.toml`
- Modify: `~/.config/nvchecker/nvchecker.toml` (local, not committed)

- [ ] **Step 1: Append the stanza to both configs**

Append to `.extras/nvchecker.toml`:

```toml
[claude-desktop-bin]
source = "cmd"
cmd = "curl -fsSL https://downloads.claude.ai/claude-desktop/apt/stable/dists/stable/main/binary-amd64/Packages | awk '/^Version:/{print $2}' | sort -V | tail -1"
```

Append the identical stanza to `~/.config/nvchecker/nvchecker.toml`.

- [ ] **Step 2: Verify nvchecker resolves the entry**

Run:
```bash
nvchecker -c .extras/nvchecker.toml 2>&1 | grep claude-desktop-bin
```
Expected: a line showing `claude-desktop-bin` with the current upstream version
(`1.18286.2` or newer). No error.

---

### Task 12: Add the package to the README table

**Files:**
- Modify: `README.md` (after the `megasync-bin` row, line ~63)

- [ ] **Step 1: Insert the table row**

After the `megasync-bin` row in the package table, add:

```
| claude-desktop-bin | ✅ | not tested | ❌ | 1.18286.2 | 1.18286.2 |
```

(Update the `-current` / `15.0` cells to ✅ once each target is actually built
and tested per Tasks 8–10; leave `not tested` for a target you have not run.)

- [ ] **Step 2: Verify the row renders (column count)**

Run:
```bash
grep 'claude-desktop-bin' README.md
```
Expected: one row with 6 `|`-separated cells matching the table header.

---

### Task 13: Commit

**Files:** all of the above.

- [ ] **Step 1: Stage and commit the package**

The pre-commit hook runs sbolint automatically. `.nvchecker` and the deb must
not be staged as real files (the hook blocks staged source archives; symlinks
are auto-removed).

Run:
```bash
git add claude-desktop-bin/ .extras/nvchecker.toml README.md
git status   # confirm no .deb and no symlink is staged
git commit -m "claude-desktop-bin: add version 1.18286.2

Co-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>"
```
Expected: pre-commit sbolint passes, commit is created and GPG-signed.

- [ ] **Step 2: Verify the commit is signed**

Run:
```bash
git log --format='%h %G? %s' -1
```
Expected: `<hash> G claude-desktop-bin: add version 1.18286.2`.

- [ ] **Step 3: Push**

Run:
```bash
git push
```
Expected: pushes to all three remotes (slackware_forge, github, danix_git).

---

### Task 14: Save a memory note

**Files:**
- Create: `~/.claude/projects/-home-danix-Programming-GIT-GITHUB-my-slackbuilds/memory/claude-desktop-bin-package.md`
- Modify: the memory `MEMORY.md` index

- [ ] **Step 1: Write the memory**

Record the non-obvious facts: official Anthropic apt-repo deb (versioned URL,
so md5 is stable, unlike megasync-bin's versionless URL); PRGNAM has `-bin`
suffix but SRCNAM keeps `claude-desktop`; Debian apt-repo/AppArmor scripts
dropped; chrome-sandbox shipped setuid 4755; nvchecker uses a `cmd` source
parsing the apt `Packages` index; nvchecker stanza shipped in-package like
r8125/UrbanTerror. Link `[[deb-repack-elf-perms]]`.

Add a one-line pointer to `MEMORY.md`.

---

## Self-Review

**Spec coverage:**
- PRGNAM/SRCNAM/VERSION/x86_64/REQUIRES → Task 1, 2. ✓
- Versioned URL, stable md5 → Task 1, memory Task 14. ✓
- Drop Debian control scripts (apt/AppArmor) → Task 2 (comment + no carry-over). ✓
- Drop lintian → Task 2. ✓
- No stripping, defensive chmod 755 on libs → Task 2. ✓
- chrome-sandbox setuid in SlackBuild, last after find -L → Task 2; verify → Task 8; doinst fallback → Task 5. ✓
- Docs via cat, not cp → Task 2. ✓
- doinst hooks (desktop/mime/icon) → Task 5. ✓
- nvchecker cmd stanza, shipped in-package, both configs → Task 3, 11. ✓
- README + NVCHECKER section → Task 6. ✓
- slack-desc → Task 4. ✓
- README.md table row → Task 12. ✓
- Testing (sbolint/sbodl/sbopkglint/build/launch) → Tasks 1, 7, 8, 9, 10. ✓
- Memory note → Task 14. ✓

**Placeholder scan:** no TBD/TODO; all file contents are literal. ✓

**Type/name consistency:** `PRGNAM=claude-desktop-bin` and `SRCNAM=claude-desktop`
used consistently; deb filename `claude-desktop_1.18286.2_amd64.deb`; sandbox
path `usr/lib/claude-desktop/chrome-sandbox` identical in SlackBuild, doinst.sh,
and verification steps. ✓
