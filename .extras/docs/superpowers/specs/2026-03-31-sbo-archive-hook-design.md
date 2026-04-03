# SBo Submission Archive Hook — Design Spec

**Date:** 2026-03-31

---

## Goal

A `post-commit` git hook that automatically offers to create a SBo-ready submission
archive whenever a package's `.SlackBuild` file is added or modified in a commit.

---

## Detection

Uses `git diff-tree --no-commit-id -r --name-status HEAD` to list files changed in
the current commit. Filters for entries with status `A` (added) or `M` (modified)
whose filename matches `*.SlackBuild` at exactly one directory depth
(e.g. `hugo/hugo.SlackBuild`). Each match yields one package name (the parent directory).

Multiple packages in a single commit are handled sequentially.

---

## Per-Package Flow

1. Print header: `==> Package: <name>`
2. List all files inside `<repo-root>/<pkg>/` recursively, shown as relative paths
3. Prompt: `Create SBo archive for '<pkg>'? [y/N]`
   - Input is read from `/dev/tty` so the prompt works correctly inside a git hook
4. On `y`/`Y`: create the archive (see below)
5. On anything else: skip silently and continue to the next package

---

## Archive

- **Location:** `SBo/<pkg>.tar.gz` (inside repo root, not under version control)
- **Structure:** single top-level directory `<pkg>/` containing all package files flat
- **Command:** `tar -czf "$SBO_DIR/$pkg.tar.gz" -C "$REPO_ROOT" "$pkg"`
- `SBo/` is created automatically if it does not exist
- Repeated archives overwrite the previous one (no version suffix — matches SBo naming conventions)

---

## Output Directory

`SBo/` lives at the repo root and is added to `.gitignore` so it is never committed.

---

## Files Changed

| File | Change |
|---|---|
| `.git/hooks/post-commit` | New file (created, made executable) |
| `.gitignore` | Append `SBo/` entry |

---

## Out of Scope

- Verifying checksums or linting (handled by existing pre-commit hook and manual workflow)
- Uploading or submitting to SBo directly
- Tracking which archives have already been submitted
