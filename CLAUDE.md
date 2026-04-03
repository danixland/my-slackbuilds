# SlackBuilds Repository

Personal collection of SlackBuild scripts maintained by Danilo, compatible with
[SlackBuilds.org (SBo)](https://slackbuilds.org) conventions.

---

## Repo Structure

Each package lives in its own top-level subfolder:

```
<package-name>/
├── <package-name>.SlackBuild   # Main build script
├── <package-name>.info         # Metadata (version, checksums, URLs)
├── README                      # Description and usage notes
├── slack-desc                  # Package description (11-line format)
├── <package-name>.desktop      # (optional) Desktop entry for GUI apps
└── [...]                       # other optional files
```

Version tracking is handled by a single repo-level file:

```
nvchecker.toml   # nvchecker config listing all packages
```

---

## SlackBuild Scripting Guidelines

- Follow the [SBo template](https://slackbuilds.org/templates/) as the base for all scripts
- Use `set -e` to abort on errors
- Honor `$TMP`, `$BUILD`, `$TAG`, `$OUTPUT` variables; default values must be set if unset
- Use `$ARCH` detection with proper `SLKCFLAGS` and `LIBDIRSUFFIX`
- Strip binaries and libraries unless upstream explicitly discourages it
- Install docs to `/usr/doc/$PRGNAM-$VERSION/`
- Always include `find -L` + `chown`/`chmod` cleanup block before packaging
- Use `makepkg -l y -c n` to create the final package

### `.info` file

Must contain:
```
PRGNAM, VERSION, HOMEPAGE, DOWNLOAD, MD5SUM (or SHA256SUM), REQUIRES, MAINTAINER, EMAIL
```
- Checksums must match the exact source archive
- Use `REQUIRES=""` if no SBo dependencies; list space-separated SBo package names otherwise

### `slack-desc`

- Exactly 11 lines in the `package-name: description` format
- First line: `package-name: package-name (short one-liner)`
- Lines 2–11: description, leave blank lines as `package-name:`
- Handy ruler line must be included (but not shipped)

---

## Version Tracking: nvchecker

A single `nvchecker.toml` at the repo root tracks upstream versions for all packages. Each package defines its own source type (GitHub releases, PyPI, etc.).

Example entry for a GitHub-hosted package:

```toml
[package-name]
source = "github"
github = "owner/repo"
use_max_tag = true
```

To check for new upstream versions:

```bash
nvchecker -c nvchecker.toml
```

When a new version is detected, update `VERSION` in both `.SlackBuild` and `.info`, then run `sbofixinfo` to refresh checksums.

**Every new package added to this repo must have a corresponding entry in `nvchecker.toml`.**

---

## Tooling: sbo-maintainer-tools

Source: https://slackware.uk/~urchlay/repos/sbo-maintainer-tools

### Tools used

| Tool | Purpose |
|------|---------|
| `sbolint` | Lint `.SlackBuild`, `README`, `.info`, `slack-desc` |
| `sbopkglint` | Lint the built package |
| `sbofixinfo` | Auto-fix common `.info` file issues |
| `sbodl` | Download sources and verify `MD5SUM`/`SHA256SUM` from `.info` |
| `pre-commit-sbolint` | Git pre-commit hook — blocks commits that fail `sbolint` |

### Git hook setup

Both hooks are tracked in the `hooks/` directory. Install them after cloning:

```bash
cp hooks/pre-commit .git/hooks/pre-commit
cp hooks/post-commit .git/hooks/post-commit
chmod +x .git/hooks/pre-commit .git/hooks/post-commit
```

| Hook | Trigger | Purpose |
|------|---------|---------|
| `pre-commit` | Before every commit | Runs `sbolint` on staged packages; blocks commit on errors |
| `post-commit` | After every commit | Offers to create a `SBo/<pkg>.tar.gz` archive for submission |

To bypass the pre-commit lint check in exceptional cases:
```bash
SBOLINT=no git commit -m'Message here'
```

### Workflow per package

```bash
# 1. Fix any .info issues automatically
cd <package-name> && sbofixinfo

# 2. Download sources and verify checksums
# NOTE: when updating to a new version, sbodl will download the source but fail
# because the .info file still has the old (or placeholder) MD5SUM. In that case:
#   a) compute the checksum manually: md5sum <source-file>
#   b) update MD5SUM in the .info file
#   c) run sbodl again — it should now report "md5sum matches OK"
cd <package-name> && sbodl

# 3. Lint the script and metadata
cd <package-name> && sbolint

# 4. Build the package
cd <package-name> && sudo bash <package-name>.SlackBuild

# 5. Lint the built package
cd <package-name> && sbopkglint

# 6. Add an entry for the package in the repo-level nvchecker.toml

# 7. Remove symlinks created by sbodl before staging
# sbodl creates symlinks in the package directory pointing to downloaded sources.
# These must never be committed to git.
find . -type l -delete

# 8. Commit (pre-commit hook runs sbolint automatically)
git add <package-name>/ nvchecker.toml
git commit -m'<package-name>: add version X.Y.Z'
```

---

## SBo Submission Guidelines

Before submitting or updating a package on SlackBuilds.org:

1. **Pass all lint checks** — `sbolint` and `sbopkglint` must report no errors
2. **Test the build** on a clean Slackware installation (or VM snapshot)
3. **Verify checksums** via `sbodl` before committing
4. **Check `README`** includes all non-SBo dependencies and any special build notes
5. **Follow the SBo submission guidelines**: https://slackbuilds.org/guidelines/
6. **Bump `VERSION`** in both `.SlackBuild` and `.info` when updating
7. **Run `sbofixinfo`** after every version bump to catch stale checksums/URLs

---

## Commit Conventions

- One commit per package add/update
- Commit message format: `<package-name>: add version X.Y.Z` or `<package-name>: update to X.Y.Z`
- For fixes: `<package-name>: fix <short description>`

---

## Maintainer

danix
