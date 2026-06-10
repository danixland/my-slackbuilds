![](.extras/assets/tiles-03-1024x819.jpg)

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
| hstr | ✅ | not tested | [hstr](https://slackbuilds.org/repository/15.0/system/hstr/) | 3.2 | 3.2 |
| discord | ✅ | not tested | [discord](https://slackbuilds.org/repository/15.0/network/discord/) | 1.0.137 | 0.0.135 |
| kitty-bin | ✅ | not tested | ❌ | 0.46.2 | 0.46.2 |
| llama.cpp-vulkan | ✅ | not tested | ❌ | b8989 | b8989 |
| qarma | ✅ | not tested | ❌ | 1.1.1 | 1.1.1 |
| opencode-bin | ✅ | not tested | ❌ | 1.14.30 | 1.14.30 |
| claude-code-bin | ✅ | not tested | ❌ | 2.1.140 | 2.1.140 |
| kvantum-qt6 | ✅ | not tested | ❌ | 1.1.6 | 1.1.6 |
| gitleaks | ✅ | not tested | ❌ | 8.30.1 | 8.30.1 |
| mutagen | ✅ | not tested | ✅ | 1.47.0 | 1.47.0 |
| solvespace | ✅ | not tested | ❌ | 3.2 | 3.2 |
| gamescope | ✅ | not tested | ❌ | 3.16.23 | 3.16.23 |
| nvchecker | ✅ | not tested | ❌ | 2.20 | 2.20 |
| python3-structlog | ✅ | not tested | ❌ | 25.5.0 | 25.5.0 |
| python3-platformdirs | ✅ | not tested | ❌ | 4.9.6 | 4.9.6 |
| python3-awesomeversion | ✅ | not tested | ❌ | 25.8.0 | 25.8.0 |
| python3-fsspec | ✅ | not tested | ❌ | 2026.4.0 | 2026.4.0 |
| python3-packaging | ✅ | not tested | ❌ | 26.2 | 26.2 |
| python3-annotated-doc | ✅ | not tested | ❌ | 0.0.4 | 0.0.4 |
| python3-typer | ✅ | not tested | ❌ | 0.25.1 | 0.25.1 |
| python3-huggingface_hub | ✅ | not tested | ❌ | 1.18.0 | 1.18.0 |
| click | ✅ | not tested | ✅ | 8.4.1 | 8.4.1 |
| btop | ✅ | not tested | ✅ | 1.4.6 | 1.4.6 |

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
| `pre-commit` | Runs [`sbolint`](https://slackware.uk/~urchlay/repos/sbo-maintainer-tools) on staged packages before each commit. Also guards against staged source archives: symlinks are auto-removed silently, real archive files block the commit and list the offenders. |
| `post-commit` | After each commit, offers to create a `SBo/<pkg>.tar.gz` archive ready for submission to SlackBuilds.org |

---

## License

GPL-2.0 — see [LICENSE](LICENSE).
