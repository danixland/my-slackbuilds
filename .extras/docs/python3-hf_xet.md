# Adding python3-hf_xet as a separate SlackBuild

Plan for packaging the `hf_xet` Rust/PyO3 extension that accelerates
`huggingface_hub` with Xet storage (chunk-based deduplication). Research only,
nothing has been built or committed yet.

Written 2026-08-11. All upstream versions and checksums were verified on that
date. Re-verify before acting if a new hf-xet release has shipped.

---

## Conclusions up front

**Package it as `python3-hf_xet`, a separate package.** It is an optional
accelerator, not a hard runtime dependency of `huggingface_hub`, so it should
not be added to `python3-hf_xet`'s REQUIRES. Without it, `huggingface_hub`
falls back to regular HTTP download and prints a warning, which is the
behaviour that motivated this plan (see `TODO-AND-NOTES.md`).

**The SBo name is free.** Searched slackbuilds.org for both `hf_xet` and
`hf-xet`: no results.

**Build system is maturin (PyO3).** SBo-stable already has `python3-maturin`
1.14.1, which satisfies `maturin>=1.7,<2.0`. That package REQUIRES `rust-opt`
(currently 1.97.1), which provides `/opt/rust/bin/cargo` and `rustc`. Rust
edition 2024 is used, which needs rustc 1.85+, so 1.97.1 is fine.

**The closest SBo precedent is `python3-orjson`** — same stack: Rust/PyO3
extension built with maturin, same SlackBuild pattern (PATH and
LD_LIBRARY_PATH for rust-opt, `python3 -m build --no-isolation`, `python3 -m
installer`). The new script should follow it closely.

**The sdist does not vendor cargo dependencies.** The PyPI sdist
(`hf_xet-1.6.0.tar.gz`, 920 KB) contains the workspace source (the `hf_xet`
crate plus sibling path-dependency crates: `xet_pkg`, `xet_runtime`,
`xet_client`, `xet_core_structures`, `xet_data`) and `Cargo.lock` (148 KB),
but not the crates.io dependencies. Building from sdist requires network
access to fetch from crates.io. This is the main decision point, see below.

---

## What hf_xet is

`hf-xet` enables `huggingface_hub` to use Xet storage for uploading and
downloading from the HF Hub. Xet storage provides chunk-based deduplication,
efficient storage/retrieval with local disk caching, and backwards
compatibility with Git LFS. The library is not meant to be used directly, it
is imported by `huggingface_hub` when available.

As of `huggingface_hub` 0.32.0, `hf_xet` is installed automatically as a
dependency. On this system, where `python3-huggingface_hub` is built from
source without it, the CLI prints:

```
Xet Storage is enabled for this repo, but the 'hf_xet' package is not
installed. Falling back to regular HTTP download. For better performance,
install the package with: `pip install huggingface_hub[hf_xet]` or
`pip install hf_xet`
```

That warning is the reason this package is wanted.

### Upstream facts (verified 2026-08-11)

| Item | Value |
|---|---|
| PyPI name | `hf-xet` |
| Import name | `hf_xet` |
| Latest version | `1.6.0` |
| License | Apache-2.0 |
| requires_python | `>=3.8` |
| Build system | maturin (`>=1.7,<2.0`) |
| Rust edition | 2024 (needs rustc 1.85+) |
| PyO3 version | 0.29, abi3-py38 (one wheel for all CPython 3.8+) |
| Runtime deps | none (only `pytest` for the `tests` extra) |
| Homepage | `https://github.com/huggingface/xet-core` |
| sdist | `hf_xet-1.6.0.tar.gz`, 920,527 bytes, md5 `bf1ab4c595afa39ed1391294dc1d9f01` |
| sdist URL | `https://files.pythonhosted.org/packages/source/h/hf-xet/hf_xet-1.6.0.tar.gz` |

### Workspace structure

`hf_xet` lives in the `huggingface/xet-core` monorepo, excluded from the main
workspace but depending on sibling crates via path deps:

```
xet-core/
├── Cargo.toml          # workspace, excludes hf_xet
├── Cargo.lock          # 148 KB, shared across the workspace
├── hf_xet/             # the PyO3 bindings (this package)
│   ├── Cargo.toml      # path deps to ../xet_pkg, ../xet_runtime, ../xet_client
│   └── pyproject.toml  # maturin build backend
├── xet_pkg/            # consolidated package, depends on the rest
├── xet_runtime/        # async runtime, config, logging
├── xet_client/         # HTTP client for CAS and Hub backend
├── xet_core_structures/ # MerkleHash, metadata shards, shared structs
└── xet_data/           # chunking, dedup, file reconstruction
```

The maturin sdist includes the `hf_xet` crate and the sibling path-dep crates,
so it is self-contained at the workspace level. It is not self-contained at
the crates.io level.

---

## Decision: online build vs vendored build

This is the only real design choice. Everything else follows the `python3-orjson`
template.

### Option A: online build (recommended)

Do not set `CARGO_NET_OFFLINE=true`. Cargo fetches dependencies from crates.io
at build time. The build needs network access, which the Docker test-build
container and the buildsystem VM both have.

Pros:
- Simplest script, fewest moving parts.
- No vendored tarball to generate, host, or keep in sync across version bumps.
- Matches how most Rust Python extensions are built outside SBo.

Cons:
- Not SBo-conventional. SBo packages are expected to build offline.
- If submitting to SBo later, a vendored tarball would need to be added.

### Option B: vendored build

Generate a vendored cargo deps tarball via `cargo vendor`, host it (the SBo
maintainer `fourtysixandtwo` hosts on `slackware.uk`), and add it as a second
`DOWNLOAD`/`MD5SUM` entry. Set `CARGO_NET_OFFLINE=true` and point
`CARGO_HOME` at the vendored directory.

Pros:
- SBo-conventional offline build.
- No network dependency at build time.

Cons:
- The vendored tarball would be roughly 50-200 MB (the dep tree includes
  `reqwest`, `tokio`, `pyo3`, `git2`, `blake3`, `hyper`, etc.).
- Needs hosting somewhere.
- Every version bump requires regenerating and re-hosting the tarball.
- `python3-maturin` ships a `mkvendored.sh` script that could be used as a
  reference, but the workflow is still manual.

### Recommendation

Start with option A (online build). It gets the package working with the
least effort. If submitting to SBo becomes the goal, switch to option B at
that point, the script change is small (add the second `DOWNLOAD`, set
`CARGO_NET_OFFLINE=true`, point `CARGO_HOME` at the vendor dir).

---

## Plan

### Step 1: create the package files

Create `python3-hf_xet/` with the standard four files:

```
python3-hf_xet/
├── python3-hf_xet.SlackBuild
├── python3-hf_xet.info
├── README
└── slack-desc
```

#### SlackBuild

Base the script on `python3-orjson` (`/var/lib/sbopkg/SBo-stable/python/python3-orjson/python3-orjson.SlackBuild`),
which is the closest analog. Key points:

- `PRGNAM=python3-hf_xet`, `SRCNAM=hf_xet` (the PyPI sdist name).
- Standard ARCH detection, SLKCFLAGS, LIBDIRSUFFIX.
- Extract: `tar xvf $CWD/$SRCNAM-$VERSION.tar.gz`.
- Set up rust-opt:
  ```sh
  export PATH="/opt/rust/bin:$PATH"
  export LD_LIBRARY_PATH="/opt/rust/lib$LIBDIRSUFFIX${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
  ```
- `export MATURIN_NO_INSTALL_RUST=1` — prevents maturin from auto-installing
  rust if rust-opt is missing.
- `export CARGO_HOME=$(pwd)/.cargo` — keeps cargo writes inside `$TMP`.
- Do **not** set `CARGO_NET_OFFLINE=true` (online build, option A).
- Build and install:
  ```sh
  python3 -m build --wheel --no-isolation
  python3 -m installer --destdir "$PKG" dist/*.whl
  ```
- Strip, docs (`LICENSE README*`), slack-desc, makepkg.

#### .info

```
PRGNAM="python3-hf_xet"
VERSION="1.6.0"
HOMEPAGE="https://github.com/huggingface/xet-core"
DOWNLOAD="https://files.pythonhosted.org/packages/source/h/hf-xet/hf_xet-1.6.0.tar.gz"
MD5SUM="bf1ab4c595afa39ed1391294dc1d9f01"
DOWNLOAD_x86_64=""
MD5SUM_x86_64=""
REQUIRES="python3-maturin"
MAINTAINER="danix"
EMAIL="danix@danix.xyz"
```

`REQUIRES="python3-maturin"` because maturin is the build backend. `rust-opt`
is pulled in transitively through `python3-maturin`'s own REQUIRES. No runtime
REQUIRES, `hf_xet` is a standalone extension module.

#### slack-desc

11 lines, `python3-hf_xet:` prefix. Short description: "Rust extension for
Hugging Face Hub Xet storage (dedup and fast transfers)."

#### README

Standard SBo README: what it is, what it does, that it is an optional
accelerator for `huggingface_hub`, build deps (rust-opt, python3-maturin).

### Step 2: add nvchecker entry

Append to `.extras/nvchecker.toml`:

```toml
[python3-hf_xet]
source = "pypi"
pypi = "hf-xet"
```

### Step 3: verify and lint

```sh
cd python3-hf_xet && sbofixinfo
cd python3-hf_xet && sbodl
cd python3-hf_xet && sbolint
```

`sbodl` must report `md5sum matches OK`. If the checksum is stale, use the
two-pass `sbodl` procedure from AGENTS.md.

### Step 4: test build

Use the `test-build-slackbuild` skill. Run both trees since this package is
SBo-submittable:

```sh
test-build python3-hf_xet            # -current
test-build --stable python3-hf_xet   # 15.0
```

Watch for:
- rust-opt version on -stable (if 15.0's rust-opt is older than 1.85, edition
  2024 will fail to compile).
- Network access in the Docker container (cargo needs to fetch from
  crates.io for the online build).
- The build is slow (Rust compilation with LTO, `codegen-units = 1`).

### Step 5: report and wait

Present a summary of all changes and test results. Wait for commit
instruction.

---

## Known problem areas

**-stable rust-opt age.** The `Cargo.toml` uses `edition = "2024"`, which
requires rustc 1.85+. If SBo-stable's `rust-opt` is older than 1.85, the
build will fail on 15.0. SBo-stable currently ships rust-opt 1.97.1 (verified
2026-08-11 in `/var/lib/sbopkg/SBo-stable/development/rust-opt/rust-opt.info`),
which is fine, but this should be re-checked at build time.

**Build time.** The release profile uses `lto = true` and
`codegen-units = 1`, plus the dep tree is large (reqwest, tokio, pyo3, git2).
Expect a multi-minute compile even on fast hardware. The Docker test-build
timeout may need to be generous.

**No runtime deps, but a transitive build-toolchain dep.** `python3-maturin`
REQUIRES `rust-opt` and `python3-setuptools-rust-opt`. Anyone building
`python3-hf_xet` needs the full Rust toolchain installed. This is expected
for any maturin-built package and is not a problem specific to hf_xet.

**sdist does not include vendored crates.** See the decision section above.
The online build is the pragmatic choice for this repo. If SBo submission is
the goal, vendoring must be added.

**`huggingface_hub` integration is automatic.** Once `hf_xet` is installed,
`huggingface_hub` detects it and uses it. No configuration needed. The
warning in `TODO-AND-NOTES.md` disappears.

---

## What NOT to change

**Do not add `python3-hf_xet` to `python3-huggingface_hub`'s REQUIRES.** It
is optional. Forcing it would pull in the entire Rust toolchain as a build
dependency of `huggingface_hub`, which is unreasonable for users who only
want the Python API.

**Do not use the prebuilt wheels.** PyPI ships manylinux wheels, but SBo
policy is to build from source. The sdist exists and is buildable.

---

## Status

Updated 2026-08-11.

Research complete. No files created yet. The package is ready to be built
when the user decides to proceed. The only open decision is online vs
vendored build (recommended: online).
