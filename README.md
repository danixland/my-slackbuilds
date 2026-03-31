# my-slackbuilds

Personal collection of SlackBuild scripts by danix, following
[SlackBuilds.org (SBo)](https://slackbuilds.org) conventions.

Primarily targeting **Slackware64-current**.

This repository serves two purposes:

- **New packages** — SlackBuilds for programs not yet available on SBo
- **Updated packages** — SlackBuilds for programs already on SBo but whose official build lags behind the latest upstream release

---

## Repository Structure

Each package lives in its own top-level directory:

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
| discord | ✅ | not tested | [discord](https://slackbuilds.org/repository/15.0/network/discord/) | 0.0.130 | 0.0.131 |
| hstr | ✅ | not tested | [hstr](https://slackbuilds.org/repository/15.0/system/hstr/) | 3.1 | 3.2 |
| hugo | ✅ | not tested | [hugo](https://slackbuilds.org/repository/15.0/development/hugo/) | 0.159.1 | 0.159.1 |
| kitty-bin | ✅ | not tested | ❌ | 0.46.2 | 0.46.2 |
| llama.cpp-vulkan | ✅ | not tested | ❌ | b8589 | b8590 |
| obsidian | ✅ | not tested | [obsidian](https://slackbuilds.org/repository/15.0/development/obsidian/) | 1.12.7 | 1.12.7 |
| qarma | ✅ | not tested | ❌ | 1.0.0 | 1.1.0 |
| opencode-bin| ✅ | not tested | ❌ | 1.3.9 | 1.3.9 |
| syncthing | ✅ | not tested | [syncthing](https://slackbuilds.org/repository/15.0/network/syncthing/) | 2.0.15 | 2.0.15 |

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
sbodl <package-name>/

# Log in as root, then build the package
cd <package-name>
bash <package-name>.SlackBuild

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
