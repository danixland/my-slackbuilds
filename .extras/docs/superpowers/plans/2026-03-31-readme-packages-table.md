# README Packages Table Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a "Packages" section to `README.md` with a table listing all 8 packages, their build status, SBo presence, repo version, and latest upstream version.

**Architecture:** Single static Markdown table inserted into `README.md` between the "Repository Structure" and "Usage" sections. No scripts or automation — maintained manually.

**Tech Stack:** Markdown

---

## File Map

| File | Action | Responsibility |
|------|--------|----------------|
| `README.md` | Modify | Insert "Packages" section with table |

---

### Task 1: Add Packages table to README.md

**Files:**
- Modify: `README.md`

- [ ] **Step 1: Insert the Packages section**

Open `README.md`. After the closing `---` of the "Repository Structure" section (after the `nvchecker.toml` line) and before the `## Usage` heading, insert the following block exactly:

```markdown
## Packages

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

---
```

- [ ] **Step 2: Verify the section is in the right place**

Read `README.md` and confirm the order is:
1. `## Repository Structure`
2. `## Packages` ← new section
3. `## Usage`

- [ ] **Step 3: Commit**

```bash
git add README.md
git commit -m "README: add packages table with build status and version info"
```
