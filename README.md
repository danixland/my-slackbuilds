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
| discord | ✅ | not tested | ✅ [discord](https://slackbuilds.org/repository/15.0/network/discord/) | 1.0.159 | 1.0.159 |
| llama.cpp-vulkan | ✅ | not tested | ❌ | 0.5.0 | 0.5.0 |
| qarma | ✅ | not tested | ❌ | 1.1.1 | 1.1.1 |
| claude-code-bin | ✅ | not tested | ❌ | 2.1.282 | 2.1.282 |
| kvantum-qt6 | ✅ | not tested | ❌ | 1.1.8 | 1.1.8 |
| kvantum-qt5 | ✅ | not tested | ✅ [kvantum-qt5](https://slackbuilds.org/repository/15.0/system/kvantum-qt5/) | 1.1.8 | 1.1.8 |
| gitleaks | ✅ | not tested | ❌ | 8.30.1 | 8.30.1 |
| mutagen | ✅ | not tested | ✅ [mutagen](https://slackbuilds.org/repository/15.0/development/mutagen/) | 1.48.1 | 1.48.1 |
| solvespace | ✅ | not tested | ❌ | 3.2 | 3.2 |
| gamescope | ✅ | not tested | ✅ [gamescope](https://slackbuilds.org/repository/15.0/system/gamescope/) | 3.16.30 | 3.16.30 |
| nvchecker | ✅ | not tested | ❌ | 2.22 | 2.22 |
| python3-structlog | ✅ | not tested | ❌ | 26.1.0 | 26.1.0 |
| python3-platformdirs | ✅ | not tested | ✅ [python3-platformdirs](https://slackbuilds.org/repository/15.0/python/python3-platformdirs/) | 4.11.13 | 4.11.13 |
| python3-awesomeversion | ✅ | not tested | ❌ | 25.8.0 | 25.8.0 |
| python3-fsspec | ✅ | not tested | ❌ | 2026.9.0 | 2026.9.0 |
| python3-packaging | ✅ | not tested | ❌ | 26.3 | 26.3 |
| python3-huggingface_hub | ✅ | not tested | ❌ | 2.0.0 | 2.0.0 |
| click | ✅ | not tested | ✅ [click](https://slackbuilds.org/repository/15.0/python/click/) | 8.5.0 | 8.5.0 |
| playwright-cli | ✅ | not tested | ❌ | 0.1.21 | 0.1.21 |
| firefly-cli | ✅ | not tested | ❌ | 0.5.0 | 0.5.0 |
| gitea-cli | ✅ | ✅ | ❌ | 0.16.0 | 0.16.0 |
| megasync-bin | ✅ | ✅ | ❌ | 6.6.2.0 | 6.6.2.0 |
| claude-desktop-bin | ✅ | ✅ | ❌ | 2.7032.0 | 2.7032.0 |
| hyprsunset-qt | ✅ | not tested | ❌ | 0.1.1 | 0.1.1 |
| python3-pathvalidate | ✅ | not tested | ❌ | 3.3.1 | 3.3.1 |
| python3-onnxruntime | ✅ | not tested | ❌ | 1.30.0 | 1.30.0 |
| piper-tts | ✅ | not tested | ❌ | 1.8.0 | 1.8.0 |
| edge-tts | ✅ | not tested | ❌ | 7.2.8 | 7.2.8 |
| qtmaildir | ✅ | not tested | ❌ | 0.29.0 | 0.29.0 |
| typora-bin | ✅ | not tested | ❌ | 1.14.9 | 1.14.10 |
| quickshell | ✅ | ❌ | ❌ | 0.3.1 | 0.3.1 |
| pcsx2 | ✅ | not tested | ❌ | 2.8.2 | 2.8.2 |
| kitty | ✅ | not tested | ✅ [kitty](https://slackbuilds.org/repository/15.0/system/kitty/) | 0.49.1 | 0.49.1 |
| openvino | ✅ | not tested | ✅ [openvino](https://slackbuilds.org/repository/15.0/libraries/openvino/) | 2026.4.0 | 2026.4.0 |
| python3-httpcore2 | ✅ | ❌ | ❌ | 2.13.1 | 2.13.1 |
| python3-httpx2 | ✅ | ❌ | ❌ | 2.13.1 | 2.13.1 |
| python3-truststore | ✅ | ❌ | ❌ | 0.10.4 | 0.10.4 |
| slang-bin | ✅ | not tested | ❌ | 2026.18.2 | 2026.18.2 |
| stable-diffusion.cpp-vulkan | ✅ | not tested | ❌ | 913 | 913 |

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
git clone https://github.com/danixland/my-slackbuilds.git
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

## Contributing

Run [`sbolint`](https://slackware.uk/~urchlay/repos/sbo-maintainer-tools) on
every changed package before sending a patch or pull request, and never commit
downloaded source archives or the symlinks `sbodl` leaves in package
directories.

---

## License

GPL-2.0 — see [LICENSE](LICENSE).

## Development Approach

This project is developed using AI-assisted tools. Code is generated with the help of AI based on human-provided specifications, design decisions, and iterative feedback.

All contributions are reviewed, tested, and curated by the maintainer before being included in the codebase. AI is used as a productivity and exploration tool, while human oversight remains central to all decisions.

The goal is to combine the flexibility of AI-assisted development with standard open-source practices such as transparency, review, and accountability.
