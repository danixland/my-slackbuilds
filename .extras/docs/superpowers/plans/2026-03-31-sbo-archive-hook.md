# SBo Archive Hook Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a `post-commit` git hook that detects added/updated packages and offers to create a SBo-ready submission archive.

**Architecture:** A single bash script at `.git/hooks/post-commit` inspects the HEAD commit via `git diff-tree`, identifies touched `.SlackBuild` files, lists the package contents, prompts the user interactively, and creates a `tar.gz` archive under `SBo/`. The `SBo/` directory is excluded from version control via `.gitignore`.

**Tech Stack:** Bash, standard POSIX tools (`tar`, `find`, `git diff-tree`)

---

## File Map

| File | Action | Responsibility |
|---|---|---|
| `.git/hooks/post-commit` | Create | Hook entry point — detection, prompt, archive creation |
| `.gitignore` | Modify | Exclude `SBo/` from version control |

---

### Task 1: Add SBo/ to .gitignore

**Files:**
- Modify: `.gitignore`

- [ ] **Step 1: Append the SBo/ entry**

Open `.gitignore` (currently contains only `.claude`) and add:

```
SBo/
```

Final `.gitignore`:
```
.claude
SBo/
```

- [ ] **Step 2: Verify it is ignored**

```bash
mkdir -p SBo
git status
```

Expected: `SBo/` does NOT appear in untracked files (git ignores it).

```bash
rmdir SBo
```

- [ ] **Step 3: Commit**

```bash
git add .gitignore
git commit -m "gitignore: exclude SBo/ output directory"
```

---

### Task 2: Write the post-commit hook

**Files:**
- Create: `.git/hooks/post-commit`

- [ ] **Step 1: Create the hook file**

Create `.git/hooks/post-commit` with the following content:

```bash
#!/bin/bash
# post-commit hook: create SBo submission archive for added/updated packages

REPO_ROOT=$(git rev-parse --show-toplevel)
SBO_DIR="$REPO_ROOT/SBo"

# Find packages whose .SlackBuild was added (A) or modified (M) in this commit.
# git diff-tree output format: <status>\t<file>
# We only want files exactly one directory deep, e.g. hugo/hugo.SlackBuild.
PACKAGES=()
while IFS=$'\t' read -r status file; do
    [[ "$status" != "A" && "$status" != "M" ]] && continue
    dir=$(dirname "$file")
    base=$(basename "$file")
    [[ "$dir" == "." ]] && continue       # skip root-level files
    [[ "$dir" == *"/"* ]] && continue     # skip files deeper than one level
    [[ "$base" == *.SlackBuild ]] || continue
    PACKAGES+=("$dir")
done < <(git diff-tree --no-commit-id -r --name-status HEAD)

[[ ${#PACKAGES[@]} -eq 0 ]] && exit 0

mkdir -p "$SBO_DIR"

for pkg in "${PACKAGES[@]}"; do
    echo ""
    echo "==> Package: $pkg"
    echo ""
    echo "Files to be archived:"
    echo "---------------------"
    find "$REPO_ROOT/$pkg" -type f | sort | sed "s|$REPO_ROOT/||"
    echo ""
    printf "Create SBo archive for '%s'? [y/N] " "$pkg"
    read -r answer </dev/tty
    case "$answer" in
        [yY]|[yY][eE][sS])
            tar -czf "$SBO_DIR/$pkg.tar.gz" -C "$REPO_ROOT" "$pkg"
            echo "  -> Archive created: SBo/$pkg.tar.gz"
            ;;
        *)
            echo "  -> Skipped."
            ;;
    esac
done

exit 0
```

- [ ] **Step 2: Make it executable**

```bash
chmod +x .git/hooks/post-commit
```

- [ ] **Step 3: Verify the hook is executable**

```bash
ls -l .git/hooks/post-commit
```

Expected: `-rwxr-xr-x` permissions.

---

### Task 3: Test the hook end-to-end

- [ ] **Step 1: Simulate a commit that touches a .SlackBuild file**

Make a trivial change to an existing package's `.SlackBuild` (e.g. add a blank line), stage and commit:

```bash
echo "" >> kitty-bin/kitty-bin.SlackBuild
git add kitty-bin/kitty-bin.SlackBuild
git commit -m "kitty-bin: test hook trigger"
```

Expected output after the commit message appears:

```
==> Package: kitty-bin

Files to be archived:
---------------------
kitty-bin/doinst.sh
kitty-bin/kitty-bin.SlackBuild
kitty-bin/kitty-bin.info
kitty-bin/README
kitty-bin/slack-desc

Create SBo archive for 'kitty-bin'? [y/N]
```

- [ ] **Step 2: Answer y and verify the archive**

Type `y` and press Enter.

Expected:
```
  -> Archive created: SBo/kitty-bin.tar.gz
```

Verify the archive contents:

```bash
tar -tzf SBo/kitty-bin.tar.gz
```

Expected: all files listed under a single `kitty-bin/` top-level directory:
```
kitty-bin/
kitty-bin/doinst.sh
kitty-bin/kitty-bin.SlackBuild
kitty-bin/kitty-bin.info
kitty-bin/README
kitty-bin/slack-desc
```

- [ ] **Step 3: Verify SBo/ is not tracked by git**

```bash
git status
```

Expected: `SBo/` does not appear.

- [ ] **Step 4: Revert the test change**

```bash
git revert HEAD --no-edit
```

- [ ] **Step 5: Clean up test archive**

```bash
rm SBo/kitty-bin.tar.gz
```
