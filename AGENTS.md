# SlackBuilds Repository

Personal collection of SlackBuild scripts maintained by Danilo, compatible with
[SlackBuilds.org (SBo)](https://slackbuilds.org) conventions.

**Maintainer workspace:** if this repo is checked out inside the maintainer
workspace (a parent directory holding `AGENTS.md`, `sweep.sh` and `hooks/`),
read `../AGENTS.md` first. It holds the operational workflow: git hooks, the
upstream sweep, test builds, and delivery to SBo.

---

## Repository Layout

Each package lives in its own top-level subfolder:

```
<package-name>/
├── <package-name>.SlackBuild   # Main build script
├── <package-name>.info         # Metadata (version, checksums, download URL)
├── README                      # Description and usage notes
├── slack-desc                  # 11-line package description
├── <package-name>.desktop      # (optional) Desktop entry for GUI apps
└── [...]                       # Other optional files
```

Maintainer tooling (git hooks, the upstream sweep, test builds) is kept
outside this repo, in the maintainer's private workspace.

Root-level files you may need to edit:

```
.extras/nvchecker.toml   # Upstream version tracking — one entry per package
```

### `.extras/` holds all non-package repo files

Anything that is not a SlackBuild package must live under `.extras/`, never at
the repo root. In particular, design docs and specs go in
`.extras/docs/`, NOT a top-level `docs/`.

---

## Mandatory Workflow: Updating a Package Version

Follow these steps in order. Do not skip or reorder them.

### Step 1 — Update version strings

Edit **both** of the following files, changing the old version to the new one:

- `<package-name>/<package-name>.SlackBuild` — change `VERSION=${VERSION:-<old>}` to the new value
- `<package-name>/<package-name>.info` — change `VERSION=`, `DOWNLOAD=`, and `MD5SUM=`

For the `DOWNLOAD` field, substitute the new version into the URL pattern.
Set `MD5SUM` to a placeholder (e.g. `"placeholder"`) — it will be fixed in the next step.

### Step 2 — Fix the checksum

`sbofixinfo` does not fix checksums, it only normalizes `.info` formatting.
The checksum loop is `sbodl`:

```bash
# Pass 1: downloads the source and prints
#   WARN: md5sum doesn't match ... got: <new>
cd <package-name> && sbodl

# Put the got: value into MD5SUM in the .info file

# Pass 2: must report "md5sum matches OK"
cd <package-name> && sbodl
```

For a PyPI bump the hash-path `DOWNLOAD` URL also changes: take the sdist URL
and md5 from `https://pypi.org/pypi/<pkg>/<ver>/json` and set both fields,
then run `sbodl`.

Do not proceed past this step until `sbodl` reports `md5sum matches OK`.

### Step 3 — Lint

```bash
cd <package-name> && sbolint
```

`sbolint` must report no errors. Fix any issues before continuing.

### Step 4 — Report and wait

Present a summary of all changes made and wait for the user to instruct you to commit.

---

## Mandatory Workflow: Adding a New Package

Before creating any files, ask the user for:

- The exact `PRGNAM` (package name)
- The upstream source URL and version
- Any non-SBo runtime dependencies

Also check the package name is free on SlackBuilds.org: search
https://slackbuilds.org/result/?search=<name> (or browse the category).
If the name is taken by an unrelated project, pick a distinct name (e.g.
gitea-cli instead of tea, whose slot is a text editor) and keep the upstream
binary name via a SRCNAM variable in the SlackBuild.

Then proceed:

1. Create the package directory with all required files:
   `<prgnam>.SlackBuild`, `<prgnam>.info`, `README`, `slack-desc`
2. Follow the SlackBuild scripting rules below exactly.
3. Add an entry for the package in `.extras/nvchecker.toml`.
4. Run `sbofixinfo`, then `sbodl` (two-pass, see above), then `sbolint`.
5. Report results and wait for commit instruction.

---

## Templates

Templates live in `~/Templates/SlackBuilds/sbo/` (a git clone of the SBo
templates). **Always start from these templates** when authoring a NEW
SlackBuild, or when taking over maintenance of an existing script. Read the
matching template first; do not copy an existing package from this repo and
rename it — an old package can carry drift the current template has since fixed.

Pick by build system:

| Template | Use for |
|----------|---------|
| `sbo/python-template.SlackBuild` | Python packages (setup.py and pyproject-only paths) |
| `sbo/autotools-template.SlackBuild` | `./configure && make` |
| `sbo/cmake-template.SlackBuild` | CMake |
| `sbo/meson-template.SlackBuild` | Meson |
| `sbo/perl-template.SlackBuild` | Perl modules |
| `sbo/rubygem-template.SlackBuild` | Ruby gems |
| `sbo/haskell-template.SlackBuild` | Haskell |
| `sbo/template.info`, `sbo/slack-desc`, `sbo/README`, `sbo/doinst.sh`, `sbo/douninst.sh` | the other package files |

Strip the template's instructional comment blocks and unused branches (e.g. the
python2/setup.py path, the man-page gzip block when upstream ships no man pages)
before shipping the script.

---

## SlackBuild Scripting Rules

- Base all scripts on the SBo template: https://slackbuilds.org/templates/
- Use `set -e` (abort on error).
- Honor `$TMP`, `$BUILD`, `$TAG`, `$OUTPUT`; provide defaults if unset.
- Detect `$ARCH` and set `SLKCFLAGS` and `LIBDIRSUFFIX` accordingly.
- Strip binaries and libraries (unless upstream explicitly discourages it).
- Install docs to `/usr/doc/$PRGNAM-$VERSION/`.
- Always include the `find -L` + `chown`/`chmod` cleanup block before packaging.
- Copy repo files into `$PKG` with `cat src > dest`, never `cp`. `cat` writes
  through a fresh destination so the build's umask/root ownership sets the
  perms; `cp` bleeds the git working-tree mode/ownership into the package.
  Applies to `slack-desc`, `doinst.sh`, and any file staged from `$CWD`
  (SlackBuild, README, `.nvchecker`) into `$PKG`.
- Build the package with `makepkg -l y -c n`.

### `.info` file — required fields

```
PRGNAM="..."
VERSION="..."
HOMEPAGE="..."
DOWNLOAD="..."
MD5SUM="..."
DOWNLOAD_x86_64=""
MD5SUM_x86_64=""
REQUIRES=""
MAINTAINER="danix"
EMAIL="danix@danix.xyz"
```

- `REQUIRES=""` if no SBo dependencies; otherwise space-separated SBo package names.
- Checksums must match the exact source archive — verify with `sbodl`.
- `MD5SUM` may be replaced by `SHA256SUM` where preferred.

### `slack-desc` rules

- Exactly 11 lines, each prefixed with `package-name: `.
- Line 1: `package-name: package-name (short one-liner description)`
- Lines 2–11: prose description; blank lines use `package-name:` with nothing after the colon.
- Handy ruler line must be included (but not shipped).

---

## Tooling: sbo-maintainer-tools

Source: https://slackware.uk/~urchlay/repos/sbo-maintainer-tools

| Tool | Purpose |
|------|---------|
| `sbolint` | Lint `.SlackBuild`, `README`, `.info`, `slack-desc` |
| `sbopkglint` | Lint the built package |
| `sbofixinfo` | Auto-fix common `.info` file issues |
| `sbodl` | Download sources and verify `MD5SUM`/`SHA256SUM` from `.info` |

Run `sbolint` on every changed package before committing. A `pre-commit`
wrapper that does it automatically is available from
https://slackware.uk/~urchlay/repos/sbostuff.

---

## Version Tracking: nvchecker.toml

Every package in this repo must have an entry in `.extras/nvchecker.toml`.
Example for a GitHub-hosted package:

```toml
[package-name]
source = "github"
github = "owner/repo"
use_latest_release = true
```

Prefer `use_latest_release` (hits `releases/latest`, lenient) over
`use_max_tag` (hits `git/refs/tags`, tightly rate-limited); use `use_max_tag`
only for repos that tag but don't cut Releases.

The file has no `[__config__]`. Do not run it as-is against GitHub: the
anonymous API budget (60 req/h) runs out and later stanzas 403. Run it from a
throwaway copy with a `[__config__]` that adds a GitHub token keyfile, and
probe single stanzas with `nvchecker -c <cfg> -e <name>`.

When adding or updating a package, verify the entry exists and is correct.

---

## Git Operations

**Use the commit skill for all commits.** Do not run `git commit` manually.

**Remove symlinks before staging.** `sbodl` creates symlinks in the package
directory pointing to downloaded source archives. These must never be committed
to git. Before any `git add`, run from the repo root:

```bash
find . -type l -delete
```

Commit conventions:
- One commit per package add or update.
- Message format:
  - Add: `<package-name>: add version X.Y.Z`
  - Update: `<package-name>: update to X.Y.Z`
  - Fix: `<package-name>: fix <short description>`

---

## Findings

Repo-specific gotchas discovered during maintenance. Append new entries here
so they are not rediscovered. Each entry names the package, the symptom, the
cause, and the action.

Each entry carries a `Found` date. Recheck stale entries (roughly >6 months
old) against current `sbolint`/`sbopkglint`/`test-build` behavior before
trusting them, since upstream tooling fixes can retire a false positive.

### claude-desktop-bin: OVMF symlinks flagged as broken

- Found: 2026-08-15
- Symptom: `sbopkglint` 05-basic-sanity fails with "broken relative symlinks"
  for `/usr/share/OVMF/OVMF_{CODE,VARS}{,_4M}.fd`.
- Cause: the symlinks point at `edk2-ovmf-x64` files provided by the
  `edk2-ovmf` REQUIRES dep, which is not installed in the lint sandbox.
- Action: keep the symlinks, they are needed at runtime. The failure is a
  false positive.

### llama.cpp-vulkan: sbolint false flag on tag archive URLs (RETIRED)

- Found: 2026-08-15
- Retired: 2026-09-07
- Symptom: `sbolint` errored "github commit URL needs full 40-digit commit
  hash in filename" for the `archive/b<N>/llama.cpp-b<N>.tar.gz` DOWNLOAD.
- Cause: `sbolint` misidentified the short build tag (e.g. `b10437`) as a
  commit hash.
- Action: no longer applies. Upstream switched to versioned releases, so the
  DOWNLOAD is now `archive/refs/tags/v<X.Y.Z>/...` and `sbolint` passes
  clean. The `SBOLINT=no` exception to the lint-before-commit rule is withdrawn: commit
  normally. Kept here so the old exception is not reintroduced from memory.

### test-build needs --local-deps for in-repo sibling deps

- Found: 2026-08-15 (nvchecker), also seen 2026-09-07 (piper-tts)
- Symptom: `test-build` reports `UNMET-DEP` and refuses to build, e.g.
  `python3-awesomeversion`/`python3-structlog` for nvchecker, or
  `python3-onnxruntime`/`python3-pathvalidate` for piper-tts.
- Cause: those deps are siblings in this repo, not in the SBo tree, so the
  default tree search cannot resolve them.
- Action: pass `--local-deps` for any package whose REQUIRES names another
  package in this repo. This is not an override case; do NOT add a `drop:`
  rule. Check REQUIRES against the repo's own dirs before building.

### opencode-bin: sbodl verifies a stale cached file

- Found: 2026-08-20
- Symptom: after bumping the version, `sbodl` reports an md5 identical to
  the previous release, or a `matches OK` for a tarball that must have
  changed.
- Cause: the sbodl download cache is keyed by filename, and opencode's
  tarball has no version in its name (`opencode-linux-x64.tar.gz`). The
  cache held the old release under that name, so sbodl verified it without
  downloading.
- Action: run `sbodl -f` to force a re-download when the cached file may be
  stale, then verify the reported md5 differs from the previous release
  before updating `.info`. Use `-f` only when a filename-versionless source
  could hide a stale cache entry (e.g. after a version bump); do NOT use it
  for every `sbodl` call. If `-f` is not available, delete the entry under
  `~/sbodl-cache/` and re-run plain `sbodl`.

### opencode-bin: do not strip the bun binary

- Found: 2026-08-25
- Symptom: `sbopkglint` 20-arch fails with "ELF object(s) not stripped" on
  `usr/bin/opencode`.
- Cause: the binary is bun-generated; stripping breaks it at runtime. The
  SlackBuild intentionally skips stripping (see the comment in the script).
- Action: leave it unstripped. The `sbopkglint` failure is expected; do not
  add a strip pass to the SlackBuild.
