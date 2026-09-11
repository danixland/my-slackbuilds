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
| discord | ✅ | not tested | ✅ [discord](https://slackbuilds.org/repository/15.0/network/discord/) | 1.0.156 | 1.0.156 |
| kitty-bin | ✅ | not tested | ❌ | 0.48.2 | 0.48.2 |
| llama.cpp-vulkan | ✅ | not tested | ❌ | 0.4.0 | 0.4.0 |
| qarma | ✅ | not tested | ❌ | 1.1.1 | 1.1.1 |
| opencode-bin | ✅ | not tested | ❌ | 1.18.29 | 1.18.29 |
| claude-code-bin | ✅ | not tested | ❌ | 2.1.263 | 2.1.263 |
| kvantum-qt6 | ✅ | not tested | ❌ | 1.1.8 | 1.1.8 |
| kvantum-qt5 | ✅ | not tested | ✅ [kvantum-qt5](https://slackbuilds.org/repository/15.0/system/kvantum-qt5/) | 1.1.8 | 1.1.8 |
| gitleaks | ✅ | not tested | ❌ | 8.30.1 | 8.30.1 |
| mutagen | ✅ | not tested | ✅ [mutagen](https://slackbuilds.org/repository/15.0/development/mutagen/) | 1.48.1 | 1.48.1 |
| solvespace | ✅ | not tested | ❌ | 3.2 | 3.2 |
| gamescope | ✅ | not tested | ✅ [gamescope](https://slackbuilds.org/repository/15.0/system/gamescope/) | 3.16.28 | 3.16.28 |
| nvchecker | ✅ | not tested | ❌ | 2.22 | 2.22 |
| python3-structlog | ✅ | not tested | ❌ | 26.1.0 | 26.1.0 |
| python3-platformdirs | ✅ | not tested | ✅ [python3-platformdirs](https://slackbuilds.org/repository/15.0/python/python3-platformdirs/) | 4.11.7 | 4.11.7 |
| python3-awesomeversion | ✅ | not tested | ❌ | 25.8.0 | 25.8.0 |
| python3-fsspec | ✅ | not tested | ❌ | 2026.7.0 | 2026.7.0 |
| python3-packaging | ✅ | not tested | ❌ | 26.3 | 26.3 |
| python3-annotated-doc | ✅ | not tested | ❌ | 0.0.5 | 0.0.5 |
| python3-typer | ✅ | not tested | ❌ | 0.27.2 | 0.27.2 |
| python3-huggingface_hub | ✅ | not tested | ❌ | 1.30.0 | 1.30.0 |
| click | ✅ | not tested | ✅ [click](https://slackbuilds.org/repository/15.0/python/click/) | 8.5.0 | 8.5.0 |
| playwright-cli | ✅ | not tested | ❌ | 0.1.19 | 0.1.19 |
| firefly-cli | ✅ | not tested | ❌ | 0.5.0 | 0.5.0 |
| gitea-cli | ✅ | ✅ | ❌ | 0.15.1 | 0.15.1 |
| megasync-bin | ✅ | ✅ | ❌ | 6.5.1.0 | 6.5.1.0 |
| claude-desktop-bin | ✅ | ✅ | ❌ | 1.46388.2 | 1.46388.2 |
| hyprsunset-qt | ✅ | not tested | ❌ | 0.1.1 | 0.1.1 |
| python3-pathvalidate | ✅ | not tested | ❌ | 3.3.1 | 3.3.1 |
| python3-onnxruntime | ✅ | not tested | ❌ | 1.29.0 | 1.29.0 |
| piper-tts | ✅ | not tested | ❌ | 1.8.0 | 1.8.0 |
| edge-tts | ✅ | not tested | ❌ | 7.2.8 | 7.2.8 |
| qtmaildir | ✅ | not tested | ❌ | 0.28.0 | 0.28.0 |
| typora-bin | ✅ | not tested | ❌ | 1.14.9 | 1.14.9 |
| quickshell | ✅ | ❌ | ❌ | 0.3.1 | 0.3.1 |

> **Note on `quickshell`:** Slackware 15.0 ships Qt5 only, so this package is
> **Slackware64-current only**. It also relies on private Qt APIs and must be
> rebuilt after every Qt update, or it will crash from ABI mismatches.

> **Note on `kvantum-qt5`:** the official SBo build lags several releases behind
> upstream (1.1.2 vs 1.1.8), so this repo ships an updated build. It provides the
> Qt5 style plugin only (`libkvantum.so`); the kvantummanager GUI and bundled
> themes come from the `kvantum-qt6` package, which upstream now builds Qt6-only.

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

# Download the source and verify checksums
# After a version bump the first run fails with "md5sum doesn't match ...
# got: <new>"; set that value in MD5SUM in the .info file, then re-run until
# it reports "md5sum matches OK"
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

Two hooks are included in `.extras/hooks/`. Install them after cloning:

```bash
cp .extras/hooks/pre-commit .git/hooks/pre-commit
cp .extras/hooks/post-commit .git/hooks/post-commit
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
