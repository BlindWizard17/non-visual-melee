# 01 The dev branch

Status: ready-for-agent
Type: task

Spec: `.scratch/versioned-releases/spec.md`, "Branches" and "The workflow". ADR-0011.

## What to do

- **`a11y.yml`**:
  - It triggers on pushes to `dev` instead of `master`.
  - The publish job's condition names `refs/heads/dev`.
  - `ci-latest`'s notes say "The newest build of dev".
  - The header comment says what a push to `dev`, a push to `master` (nothing) and a dispatch do. The tag build comes in issue 04.
- **CLAUDE.md**:
  - "Remotes and branches": `dev` holds everyday work, topic branches and base merges. `master` moves only at a release, by fast-forward, and stays GitHub's default branch. Hotfixes go on `master` and are merged back into `dev`. The line saying a `dev` branch is added after a first alpha goes.
  - The CI paragraph and "Online compatibility" name `dev` where they name the branch `ci-latest` builds or the merged `master`.
- **The `base-merge` skill** merges `upstream/master` into `dev`, and its diffs compare against `dev`.
- **The `code-review` skill**'s branch diff is against `dev`.
- **The `verify` skill**: where it says a run off `master` leaves `ci-latest` alone, it says `dev`.
- **The other docs**: search `docs/a11y`, `docs/agents` and `CONTEXT.md` for `master` as the branch name (most hits are master volume and master switches, which stay).
- **The branch**: commit these on `master`, then create `dev` from it and push both, with the maintainer's leave to push. GitHub's settings need no change.

## Done when

- `dev` exists on `origin`, level with `master`.
- After a push to `dev`, the A11y run publishes `ci-latest` at `dev`'s commit (`gh release view ci-latest`). A push to `master` starts no run.
- The docs and skills above name `dev` as the working branch, and `grep -rn "master" CLAUDE.md .claude/skills` shows only the branch that releases and base port names.
