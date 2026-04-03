# README Packages Table — Design Spec

**Date:** 2026-03-31

---

## Goal

Add a static "Packages" section to `README.md` listing all packages with their
build status, SBo presence, repo version, and latest upstream version.

---

## Table Structure

Placed between "Repository Structure" and "Usage" in `README.md`.

| Column | Content |
|--------|---------|
| **Package** | Plain package name |
| **-current** | ✅ / ❌ / `not tested` |
| **15.0** | ✅ / ❌ / `not tested` |
| **SBo** | `[name](https://slackbuilds.org/...)` or ❌ |
| **Version** | Version from `.info` file |
| **Latest** | Latest upstream version (fetched 2026-03-31) |

---

## Package Data

| Package | -current | 15.0 | SBo | Version | Latest |
|---------|----------|------|-----|---------|--------|
| discord | not tested | not tested | [discord](https://slackbuilds.org/repository/15.0/network/discord/) | 0.0.130 | 0.0.131 |
| hstr | not tested | not tested | [hstr](https://slackbuilds.org/repository/15.0/system/hstr/) | 3.1 | 3.2 |
| hugo | not tested | not tested | [hugo](https://slackbuilds.org/repository/15.0/development/hugo/) | 0.159.1 | 0.159.1 |
| kitty-bin | not tested | not tested | ❌ | 0.46.2 | 0.46.2 |
| llama.cpp-vulkan | not tested | not tested | ❌ | b8581 | b8590 |
| obsidian | not tested | not tested | [obsidian](https://slackbuilds.org/repository/15.0/development/obsidian/) | 1.12.7 | 1.12.7 |
| qarma | not tested | not tested | ❌ | 1.0.0 | 1.1.0 |
| syncthing | not tested | not tested | [syncthing](https://slackbuilds.org/repository/15.0/network/syncthing/) | 2.0.15 | 2.0.15 |

Notes:
- `kitty-bin` SBo page returned 404 — marked ❌, verify manually
- `llama.cpp-vulkan` and `qarma` have no SBo entry
- Latest versions fetched via web search on 2026-03-31; re-run search when updating

---

## Maintenance

Update the table manually when:
- A build is tested → replace `not tested` with ✅ or ❌
- A package is bumped → update Version and Latest columns
- A package is submitted to SBo → replace ❌ with a link
