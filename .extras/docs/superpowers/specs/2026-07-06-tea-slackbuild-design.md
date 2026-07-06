# SlackBuild: tea (gitea CLI client)

Date: 2026-07-06
Maintainer: danix

## Goal

Package `tea` (the Gitea command-line client) as a SlackBuild built from source,
for the personal repo. Upstream: https://gitea.com/gitea/tea

## Target: personal repo only

Network build (modules fetched at build time via GOPROXY). This is NOT
SBo-legal (SBo forbids network access during build and would require a vendored
source tarball). Decision: personal repo only, do not submit to SlackBuilds.org.

## Scope

Single package `tea`, version 0.14.2, x86_64, built via `go build`.

## Files

```
tea/
├── tea.SlackBuild
├── tea.info
├── README
└── slack-desc
```

Plus one entry in `.extras/nvchecker.toml`.

## Build script

Clone `gitleaks/gitleaks.SlackBuild` (existing Go template in this repo) verbatim,
with these changes:

- `PRGNAM=tea`, `VERSION=0.14.2`
- The gitea archive extracts to a top dir named `tea/`, NOT `tea-0.14.2/`.
  So: `rm -rf $PRGNAM`, `tar xvf .../$PRGNAM-$VERSION.tar.gz`, `cd $PRGNAM`.
- Version stamp in ldflags (tea injects its version via a version package):
  `LIB_LDFLAGS='-linkmode=external -s -w -X "gitea.dev/tea/modules/version.Version=$VERSION"'`
- Build: `go build -ldflags="$LIB_LDFLAGS" -o "$PKG"/usr/bin/tea .`
- Docs installed: `LICENSE README.md CONTRIBUTING.md`

Everything else (GOPATH sandbox, CGO flags, GOFLAGS, strip, makepkg) unchanged
from gitleaks.

## Environment facts

- **No `vendor/` dir** in the tarball. Build fetches modules over the network via
  GOPROXY (`https://proxy.golang.org,direct`). Build host needs internet.
- **Requires Go 1.26.** Confirmed: Go 1.26.4 present on this machine and the
  buildsystem VM. (Note: buildsystem also has a gccgo 1.18 at `/usr/bin/go` —
  ignore it, the real toolchain is used.)
- Module path is `gitea.dev/tea`; `main.go` is at repo root, so `go build .`.
- Without the version ldflag, `tea --version` reports blank/dev.

## .info

```
PRGNAM="tea"
VERSION="0.14.2"
HOMEPAGE="https://gitea.com/gitea/tea"
DOWNLOAD="https://gitea.com/gitea/tea/archive/v0.14.2.tar.gz"
MD5SUM="b1f37f2cc43763a3a0c05f060b485768"
DOWNLOAD_x86_64=""
MD5SUM_x86_64=""
REQUIRES=""
MAINTAINER="danix"
EMAIL="danix@danix.xyz"
```

REQUIRES empty: static-ish Go binary, no SBo runtime deps.

## slack-desc

11-line format. First line `tea: tea (Gitea command line tool)`. Body describes
tea: interact with Gitea/Forgejo servers from the CLI (issues, PRs, releases,
repos, etc).

## nvchecker

```toml
[tea]
source = "gitea"
gitea = "gitea/tea"
use_max_tag = true
```

No `host` key (only needed for self-hosted instances; default is gitea.com).

## Verification

1. `sbolint` clean on the 4 files.
2. `sbodl` reports md5sum matches OK.
3. Build on buildsystem VM (Go 1.26 + network). `sbopkglint` clean.
4. `tea --version` on installed binary shows `0.14.2`, not blank.

## Out of scope

- Multi-arch (arm/aarch64). x86_64 only.
- Prebuilt-binary variant (`tea-bin`).
- SBo submission (needs a vendored source tarball; not done here).
