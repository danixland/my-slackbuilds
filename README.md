# my-slackbuilds

Personal collection of SlackBuild scripts by danix, following
[SlackBuilds.org (SBo)](https://slackbuilds.org) conventions.

Primarily targeting **Slackware64-current**.

This repository serves two purposes:

- **New packages** — SlackBuilds for programs not yet available on SBo
- **Updated packages** — SlackBuilds for programs already on SBo but whose official build lags behind the latest upstream release

---

## Repository Structure

Each package lives in its own top-level subfolder:

```
<package-name>/
├── <package-name>.SlackBuild   # Main build script
├── <package-name>.info         # Metadata (version, checksums, URLs)
├── README                      # Description and usage notes
├── slack-desc                  # Package description (11-line format)
└── <package-name>.desktop      # (optional) Desktop entry for GUI apps
```

---

## Packages

| Package | -current | 15.0 | SBo | Version | Latest |
|---------|----------|------|-----|---------|--------|
| hstr | ✅ | not tested | [hstr](https://slackbuilds.org/repository/15.0/system/hstr/) | 3.1 | 3.2 |
| kitty-bin | ✅ | not tested | ❌ | 0.46.2 | 0.46.2 |
| llama.cpp-vulkan | ✅ | not tested | ❌ | b8661 | b8661 |
| qarma | ✅ | not tested | ❌ | 1.1.1 | 1.1.1 |
| opencode-bin | ✅ | not tested | ❌ | 1.4.0 | 1.4.0 |

---

## Usage

### Prerequisites

- Slackware64-current
- Root access (required to run `.SlackBuild` scripts)
- [`sbo-maintainer-tools`](https://slackware.uk/~urchlay/repos/sbo-maintainer-tools) (optional, for linting and source downloads)

### Building a package

```bash
# Clone the repository
git clone https://github.com/danix/my-slackbuilds.git
cd my-slackbuilds

# Fix any .info issues automatically
cd <package-name> && sbofixinfo

# Download the source and verify checksums
cd <package-name> && sbodl

# Lint the script and metadata
cd <package-name> && sbolint

# Build the package
cd <package-name> && sudo bash <package-name>.SlackBuild

# Install the resulting package
installpkg /tmp/<package-name>-*.t?z
```

Check each package's `README` for dependencies and any special build instructions.

---

## Git Hooks

Two hooks are included in `hooks/`. Install them after cloning:

```bash
cp hooks/pre-commit .git/hooks/pre-commit
cp hooks/post-commit .git/hooks/post-commit
chmod +x .git/hooks/pre-commit .git/hooks/post-commit
```

| Hook | Purpose |
|------|---------|
| `pre-commit` | Runs [`sbolint`](https://slackware.uk/~urchlay/repos/sbo-maintainer-tools) on staged packages before each commit |
| `post-commit` | After each commit, offers to create a `SBo/<pkg>.tar.gz` archive ready for submission to SlackBuilds.org |

---

## License

GPL-2.0 — see [LICENSE](LICENSE).
