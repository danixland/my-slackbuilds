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
| hstr | ✅ | not tested | ✅ [hstr](https://slackbuilds.org/repository/15.0/system/hstr/) | 3.2 | 3.2 |
| discord | ✅ | not tested | ✅ [discord](https://slackbuilds.org/repository/15.0/network/discord/) | 1.0.145 | 1.0.145 |
| kitty-bin | ✅ | not tested | ❌ | 0.47.4 | 0.47.4 |
| llama.cpp-vulkan | ✅ | not tested | ❌ | b9871 | b9871 |
| qarma | ✅ | not tested | ❌ | 1.1.1 | 1.1.1 |
| opencode-bin | ✅ | not tested | ❌ | 1.17.13 | 1.17.13 |
| claude-code-bin | ✅ | not tested | ❌ | 2.1.140 | 2.1.140 |
| kvantum-qt6 | ✅ | not tested | ❌ | 1.1.8 | 1.1.8 |
| gitleaks | ✅ | not tested | ❌ | 8.30.1 | 8.30.1 |
| mutagen | ✅ | not tested | ✅ [mutagen](https://slackbuilds.org/repository/15.0/development/mutagen/) | 1.47.0 | 1.47.0 |
| solvespace | ✅ | not tested | ❌ | 3.2 | 3.2 |
| gamescope | ✅ | not tested | ✅ [gamescope](https://slackbuilds.org/repository/15.0/system/gamescope/) | 3.16.24 | 3.16.24 |
| nvchecker | ✅ | not tested | ❌ | 2.20 | 2.20 |
| python3-structlog | ✅ | not tested | ❌ | 26.1.0 | 26.1.0 |
| python3-platformdirs | ✅ | not tested | ✅ [python3-platformdirs](https://slackbuilds.org/repository/15.0/python/python3-platformdirs/) | 4.10.0 | 4.10.0 |
| python3-awesomeversion | ✅ | not tested | ❌ | 25.8.0 | 25.8.0 |
| python3-fsspec | ✅ | not tested | ❌ | 2026.6.0 | 2026.6.0 |
| python3-packaging | ✅ | not tested | ❌ | 26.2 | 26.2 |
| python3-annotated-doc | ✅ | not tested | ❌ | 0.0.4 | 0.0.4 |
| python3-typer | ✅ | not tested | ❌ | 0.26.8 | 0.26.8 |
| python3-huggingface_hub | ✅ | not tested | ❌ | 1.22.0 | 1.22.0 |
| click | ✅ | not tested | ✅ [click](https://slackbuilds.org/repository/15.0/python/click/) | 8.4.2 | 8.4.2 |
| playwright-cli | ✅ | not tested | ❌ | 0.1.15 | 0.1.15 |
| r8125 | ✅ | not tested | ✅ [r8125](https://slackbuilds.org/repository/15.0/network/r8125/) | 9.018.00 | 9.018.00 |
| firefly-cli | ✅ | not tested | ❌ | 0.4.1 | 0.4.1 |
| gitea-cli | ✅ | ✅ | ❌ | 0.14.2 | 0.14.2 |
| UrbanTerror | ✅ | ✅ | ✅ [UrbanTerror](https://slackbuilds.org/repository/15.0/games/UrbanTerror/) | 4.3.4 | 4.3.4 |
| megasync-bin | ✅ | ✅ | ❌ | 6.4.0.2 | 6.4.0.2 |

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

## Development Approach

This project is developed using AI-assisted tools. Code is generated with the help of AI based on human-provided specifications, design decisions, and iterative feedback.

All contributions are reviewed, tested, and curated by the maintainer before being included in the codebase. AI is used as a productivity and exploration tool, while human oversight remains central to all decisions.

The goal is to combine the flexibility of AI-assisted development with standard open-source practices such as transparency, review, and accountability.
