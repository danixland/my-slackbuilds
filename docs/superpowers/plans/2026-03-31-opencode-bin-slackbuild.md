# opencode-bin SlackBuild Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Create a complete `opencode-bin` SlackBuild package that repackages the official pre-built opencode binary for Slackware.

**Architecture:** Binary-repackaging pattern — download upstream tarball, install the static binary to `/usr/bin/opencode`, install docs, run `makepkg`. Follows the same structure as `kitty-bin` and `hugo` in this repo. Supports `x86_64` and `aarch64`.

**Tech Stack:** Bash, SBo packaging tools (`sbolint`, `sbofixinfo`, `sbodl`)

---

## File Map

| File | Action | Responsibility |
|------|--------|----------------|
| `opencode-bin/opencode-bin.SlackBuild` | Create | Main build script |
| `opencode-bin/opencode-bin.info` | Create | Package metadata and download URLs |
| `opencode-bin/README` | Create | Package description and usage notes |
| `opencode-bin/slack-desc` | Create | 11-line package description for pkgtool |
| `nvchecker.toml` | Modify | Add upstream version tracking entry |

---

### Task 1: Create the package files

**Files:**
- Create: `opencode-bin/opencode-bin.SlackBuild`
- Create: `opencode-bin/opencode-bin.info`
- Create: `opencode-bin/README`
- Create: `opencode-bin/slack-desc`

- [ ] **Step 1: Create the directory**

```bash
mkdir -p /home/danix/Programming/GIT/GITHUB/my-slackbuilds/opencode-bin
```

- [ ] **Step 2: Create opencode-bin.SlackBuild**

Create `/home/danix/Programming/GIT/GITHUB/my-slackbuilds/opencode-bin/opencode-bin.SlackBuild` with this exact content:

```bash
#!/bin/bash

# Slackware build script for opencode-bin

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

PRGNAM=opencode-bin
VERSION=${VERSION:-0.0.55}
BUILD=${BUILD:-1}
TAG=${TAG:-_SBo}
PKGTYPE=${PKGTYPE:-tgz}

if [ -z "$ARCH" ]; then
  case "$( uname -m )" in
    arm*) ARCH=arm ;;
       *) ARCH=$( uname -m ) ;;
  esac
fi

if [ ! -z "${PRINT_PACKAGE_NAME}" ]; then
  echo "$PRGNAM-$VERSION-$ARCH-$BUILD$TAG.$PKGTYPE"
  exit 0
fi

TMP=${TMP:-/tmp/SBo}
PKG=$TMP/package-$PRGNAM
OUTPUT=${OUTPUT:-/tmp}

if [ "$ARCH" = "x86_64" ]; then
  SRCARCH="x86_64"
elif [ "$ARCH" = "aarch64" ]; then
  SRCARCH="arm64"
else
  echo "$ARCH is not supported."
  exit 1
fi

set -e

rm -rf $PKG
mkdir -p $TMP $PKG $OUTPUT
cd $TMP
rm -rf $PRGNAM-$VERSION
mkdir -p $PRGNAM-$VERSION
tar xvf $CWD/opencode-linux-$SRCARCH.tar.gz -C $PRGNAM-$VERSION
cd $PRGNAM-$VERSION
chown -R root:root .
find -L . \
 \( -perm 777 -o -perm 775 -o -perm 750 -o -perm 711 -o -perm 555 \
  -o -perm 511 \) -exec chmod 755 {} \; -o \
 \( -perm 666 -o -perm 664 -o -perm 640 -o -perm 600 -o -perm 444 \
  -o -perm 440 -o -perm 400 \) -exec chmod 644 {} \;

mkdir -p $PKG/usr/bin
install -m 0755 opencode $PKG/usr/bin/opencode

mkdir -p $PKG/usr/doc/$PRGNAM-$VERSION
cp -a LICENSE README.md $PKG/usr/doc/$PRGNAM-$VERSION
cat $CWD/$PRGNAM.SlackBuild > $PKG/usr/doc/$PRGNAM-$VERSION/$PRGNAM.SlackBuild

mkdir -p $PKG/install
cat $CWD/slack-desc > $PKG/install/slack-desc

cd $PKG
/sbin/makepkg -l y -c n $OUTPUT/$PRGNAM-$VERSION-$ARCH-$BUILD$TAG.$PKGTYPE
```

Make it executable:
```bash
chmod +x /home/danix/Programming/GIT/GITHUB/my-slackbuilds/opencode-bin/opencode-bin.SlackBuild
```

- [ ] **Step 3: Create opencode-bin.info**

Create `/home/danix/Programming/GIT/GITHUB/my-slackbuilds/opencode-bin/opencode-bin.info` with this exact content:

```
PRGNAM="opencode-bin"
VERSION="0.0.55"
HOMEPAGE="https://github.com/opencode-ai/opencode"
DOWNLOAD="UNSUPPORTED"
MD5SUM=""
DOWNLOAD_x86_64="https://github.com/opencode-ai/opencode/releases/download/v0.0.55/opencode-linux-x86_64.tar.gz"
MD5SUM_x86_64=""
REQUIRES=""
MAINTAINER="danix"
EMAIL="danix@danix.xyz"
```

- [ ] **Step 4: Create README**

Create `/home/danix/Programming/GIT/GITHUB/my-slackbuilds/opencode-bin/README` with this exact content:

```
opencode-bin is a pre-built binary repackage of opencode, a powerful AI
coding agent built for the terminal. It supports multiple LLM providers
including OpenAI, Anthropic, Google Gemini, and others.

No compilation is required; the official upstream binary is statically
linked and has no runtime library dependencies.

To use opencode, you need an API key for at least one supported LLM
provider. Set the appropriate environment variable (e.g. OPENAI_API_KEY,
ANTHROPIC_API_KEY) or configure it via opencode's interactive setup.

Homepage: https://github.com/opencode-ai/opencode

NOTE FOR aarch64 USERS: The aarch64 tarball is not listed in the .info
file (not a standard SBo field). Download it manually before building:
  wget https://github.com/opencode-ai/opencode/releases/download/v0.0.55/opencode-linux-arm64.tar.gz
```

- [ ] **Step 5: Create slack-desc**

Create `/home/danix/Programming/GIT/GITHUB/my-slackbuilds/opencode-bin/slack-desc` with this exact content:

```
# HOW TO EDIT THIS FILE:
# The "handy ruler" below makes it easier to edit a package description.
# Line up the first '|' above the ':' following the base package name, and
# the '|' on the right side marks the last column you can put a character in.
# You must make exactly 11 lines for the formatting to be correct.  It's also
# customary to leave one blank line after the name.

             |-----handy-ruler------------------------------------------------------|
opencode-bin: opencode-bin (AI coding agent for the terminal)
opencode-bin:
opencode-bin: opencode is a powerful AI coding agent built for the terminal.
opencode-bin: It supports multiple LLM providers including OpenAI, Anthropic,
opencode-bin: Google Gemini, and others via simple configuration.
opencode-bin:
opencode-bin: This package ships the official pre-built binary. No compilation
opencode-bin: is required. API keys for a supported LLM provider are needed
opencode-bin: at runtime.
opencode-bin:
opencode-bin: Homepage: https://github.com/opencode-ai/opencode
```

---

### Task 2: Fix checksums, download source, and lint

**Files:**
- Modify: `opencode-bin/opencode-bin.info` (checksums filled in by sbofixinfo)

- [ ] **Step 1: Run sbofixinfo to normalise the .info file**

```bash
cd /home/danix/Programming/GIT/GITHUB/my-slackbuilds
sbofixinfo opencode-bin/
```

Expected: no errors; `MD5SUM_x86_64` (or `SHA256SUM_x86_64`) populated.

- [ ] **Step 2: Download source and verify checksum**

```bash
sbodl opencode-bin/
```

Expected: downloads `opencode-linux-x86_64.tar.gz` into `opencode-bin/` and reports checksum OK.

- [ ] **Step 3: Run sbolint**

```bash
sbolint opencode-bin/
```

Expected: no errors or warnings. If any are reported, fix them before continuing.

---

### Task 3: Add nvchecker entry and commit

**Files:**
- Modify: `nvchecker.toml`

- [ ] **Step 1: Add the opencode-bin entry to nvchecker.toml**

Open `/home/danix/Programming/GIT/GITHUB/my-slackbuilds/nvchecker.toml` and append:

```toml
[opencode-bin]
source = "github"
github = "opencode-ai/opencode"
use_latest_release = true
```

- [ ] **Step 2: Commit everything**

```bash
cd /home/danix/Programming/GIT/GITHUB/my-slackbuilds
git add opencode-bin/ nvchecker.toml
git commit -m "opencode-bin: add version 0.0.55"
```
