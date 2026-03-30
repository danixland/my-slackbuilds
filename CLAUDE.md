# SlackBuilds Repository

Personal collection of SlackBuild scripts maintained by Danilo, compatible with
[SlackBuilds.org (SBo)](https://slackbuilds.org) conventions.

---

## Repo Structure

Each package lives in its own top-level directory named after the package:

```
<package-name>/
├── <package-name>.SlackBuild   # Main build script
├── <package-name>.info         # Metadata (version, checksums, URLs)
├── README                      # Description and usage notes
├── slack-desc                  # Package description (11-line format)
└── <package-name>.desktop      # (optional) Desktop entry for GUI apps
```

No category subdirectories — all packages are at the repo root.

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

Install the pre-commit hook into this repo:

```bash
cp /usr/doc/sbo-maintainer-tools-0.9.3/pre-commit-sbolint .git/hooks/pre-commit
chmod +x .git/hooks/pre-commit
```

To bypass it in exceptional cases:
```bash
SBOLINT=no git commit -m'Message here'
```

### Workflow per package

```bash
# 1. Fix any .info issues automatically
sbofixinfo <package-name>/

# 2. Download sources and verify checksums
sbodl <package-name>/

# 3. Lint the script and metadata
sbolint <package-name>/

# 4. Build the package
cd <package-name> && sudo bash <package-name>.SlackBuild

# 5. Lint the built package
sbopkglint /tmp/<package-name>-*.t?z

# 6. Commit (pre-commit hook runs sbolint automatically)
git add <package-name>/
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
