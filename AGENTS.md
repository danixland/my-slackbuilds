# SlackBuilds Repository

Personal collection of SlackBuild scripts maintained by Danilo, compatible with
[SlackBuilds.org (SBo)](https://slackbuilds.org) conventions.

---

## Core Rules

1. **Ask before acting.** If anything about the task is ambiguous — target version,
   which package, whether to commit — stop and ask. Do not infer intent and proceed.
2. **Use available skills.** For git operations, commits, PRs, and any task covered
   by a skill, invoke the relevant skill. Do not improvise a workflow that a skill
   already defines.
3. **One package per task.** Never modify multiple packages in a single operation
   unless explicitly instructed.
4. **Never skip lint.** Every change must pass `sbolint` before committing. No
   exceptions.
5. **Never commit without being asked.** Complete all file edits and verification
   steps, then wait for explicit instruction to commit.

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

Run `sbofixinfo` from inside the package directory:

```bash
cd <package-name> && sbofixinfo
```

If `sbofixinfo` reports no changes (common when the checksum is a placeholder rather
than a stale real value), use the two-pass `sbodl` procedure instead:

```bash
# Pass 1 — downloads the source; fails because MD5SUM is wrong/placeholder
cd <package-name> && sbodl

# Compute the real checksum from the downloaded file
md5sum <package-name>-<version>.tar.gz   # adjust filename as needed

# Update MD5SUM in the .info file with the value from the command above

# Pass 2 — verifies the checksum; must report "md5sum matches OK"
cd <package-name> && sbodl
```

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
4. Run `sbofixinfo`, then `sbodl` (two-pass if needed), then `sbolint`.
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
| `pre-commit-sbolint` | Git pre-commit hook — blocks commits that fail `sbolint` |

---

## Version Tracking: nvchecker.toml

Every package in this repo must have an entry in `.extras/nvchecker.toml`.
Example for a GitHub-hosted package:

```toml
[package-name]
source = "github"
github = "owner/repo"
use_max_tag = true
```

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

### Git hook setup

Both hooks are tracked in the `.extras/hooks/` directory. Install them after cloning:

```bash
cp .extras/hooks/pre-commit .git/hooks/pre-commit
cp .extras/hooks/post-commit .git/hooks/post-commit
chmod +x .git/hooks/pre-commit .git/hooks/post-commit
```

| Hook | Trigger | Purpose |
|------|---------|---------|
| `pre-commit` | Before every commit | Runs `sbolint` on staged packages; blocks commit on errors. Also checks for staged source archives: symlinks are auto-removed, real files block the commit. |
| `post-commit` | After every commit | Offers to create a `SBo/<pkg>.tar.gz` archive for submission |

The pre-commit hook runs `sbolint` automatically. If it blocks the commit, fix
the reported errors and retry — do not bypass with `SBOLINT=no` unless the user
explicitly instructs it.

The post-commit hook may prompt to create an SBo archive. This is interactive and
may fail in non-TTY environments — that failure is harmless and can be ignored.

Commit conventions:
- One commit per package add or update.
- Message format:
  - Add: `<package-name>: add version X.Y.Z`
  - Update: `<package-name>: update to X.Y.Z`
  - Fix: `<package-name>: fix <short description>`

---

## What Requires User Confirmation

Stop and ask before doing any of the following:

- Committing or pushing changes
- Modifying files in more than one package directory
- Deleting any file
- Bypassing the pre-commit hook (`SBOLINT=no`)
- Adding or removing entries in `.extras/nvchecker.toml`
- Any action not covered by the workflows above

---

## Running Test Builds

The general rule is: **never run builds directly.** Building is the user's
responsibility.

The one exception is the `test-build-slackbuild` skill, which builds the package
in a Docker container via `sbo-dockerbuild` without touching the host system.
Use it to verify that a SlackBuild completes successfully, especially for new
packages or after significant changes.

---

## Findings

Repo-specific gotchas discovered during maintenance. Append new entries here
so they are not rediscovered. Each entry names the package, the symptom, the
cause, and the action.

Each entry carries a `Found` date. Recheck stale entries (roughly >6 months
old) against current `sbolint`/`sbopkglint`/`test-build` behavior before
trusting them, since upstream tooling fixes can retire a false positive.

### claude-code-bin: do not strip the bun binary

- Found: 2026-08-15
- Symptom: `sbopkglint` 20-arch fails with "ELF object(s) not stripped" on
  `usr/bin/claude`.
- Cause: the binary is bun-generated; stripping breaks it at runtime.
- Action: leave it unstripped. The `sbopkglint` failure is expected; do not
  add a strip pass to the SlackBuild.

### claude-desktop-bin: OVMF symlinks flagged as broken

- Found: 2026-08-15
- Symptom: `sbopkglint` 05-basic-sanity fails with "broken relative symlinks"
  for `/usr/share/OVMF/OVMF_{CODE,VARS}{,_4M}.fd`.
- Cause: the symlinks point at `edk2-ovmf-x64` files provided by the
  `edk2-ovmf` REQUIRES dep, which is not installed in the lint sandbox.
- Action: keep the symlinks, they are needed at runtime. The failure is a
  false positive.

### llama.cpp-vulkan: sbolint false flag on tag archive URLs

- Found: 2026-08-15
- Symptom: `sbolint` errors "github commit URL needs full 40-digit commit
  hash in filename" for the `archive/b<N>/llama.cpp-b<N>.tar.gz` DOWNLOAD.
- Cause: `sbolint` misidentifies the short build tag (e.g. `b10437`) as a
  commit hash.
- Action: known false flag and the documented exception to Core Rule 4. Run
  the normal checks, then commit with `SBOLINT=no`. Do not switch to full
  commit hashes.

### nvchecker: test-build needs --local-deps

- Found: 2026-08-15
- Symptom: `test-build --dry-run nvchecker` reports `UNMET-DEP` for
  `python3-awesomeversion` and `python3-structlog`.
- Cause: those deps are siblings in this repo, not in the SBo tree.
- Action: run `test-build --local-deps --dry-run nvchecker` first, then
  `test-build --local-deps --yes nvchecker`.

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
