# Agent Instructions — SlackBuilds Repository

This file governs how AI agents must behave in this repository.
**Read it in full before taking any action.**

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
nvchecker.toml   # Upstream version tracking — one entry per package
```

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

Then proceed:

1. Create the package directory with all required files:
   `<prgnam>.SlackBuild`, `<prgnam>.info`, `README`, `slack-desc`
2. Follow the SlackBuild scripting rules below exactly.
3. Add an entry for the package in `nvchecker.toml`.
4. Run `sbofixinfo`, then `sbodl` (two-pass if needed), then `sbolint`.
5. Report results and wait for commit instruction.

---

## SlackBuild Scripting Rules

- Base all scripts on the SBo template: https://slackbuilds.org/templates/
- Use `set -e` (abort on error).
- Honor `$TMP`, `$BUILD`, `$TAG`, `$OUTPUT`; provide defaults if unset.
- Detect `$ARCH` and set `SLKCFLAGS` and `LIBDIRSUFFIX` accordingly.
- Strip binaries and libraries (unless upstream explicitly discourages it).
- Install docs to `/usr/doc/$PRGNAM-$VERSION/`.
- Always include the `find -L` + `chown`/`chmod` cleanup block before packaging.
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

### `slack-desc` rules

- Exactly 11 lines, each prefixed with `package-name: `.
- Line 1: `package-name: package-name (short one-liner description)`
- Lines 2–11: prose description; blank lines use `package-name:` with nothing after the colon.
- Do not include the ruler line in the committed file.

---

## Version Tracking: nvchecker.toml

Every package in this repo must have an entry in `nvchecker.toml`.
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

Commit conventions:
- One commit per package add or update.
- Message format:
  - Add: `<package-name>: add version X.Y.Z`
  - Update: `<package-name>: update to X.Y.Z`
  - Fix: `<package-name>: fix <short description>`

The pre-commit hook runs `sbolint` automatically. If it blocks the commit, fix
the reported errors and retry — do not bypass with `SBOLINT=no` unless the user
explicitly instructs it.

The post-commit hook may prompt to create an SBo archive. This is interactive and
may fail in non-TTY environments — that failure is harmless and can be ignored.

---

## What Requires User Confirmation

Stop and ask before doing any of the following:

- Committing or pushing changes
- Modifying files in more than one package directory
- Deleting any file
- Bypassing the pre-commit hook (`SBOLINT=no`)
- Adding or removing entries in `nvchecker.toml`
- Any action not covered by the workflows above
