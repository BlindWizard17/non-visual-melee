# 01 The dev branch

Status: resolved (2026-11-23)
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

## Comments

### 2026-11-23, implementation (agent)

Committed on `master`, which was then pushed and `dev` created from it and pushed, with the maintainer's leave.

- **`a11y.yml`.** The push trigger names `dev`, the publish job's condition `refs/heads/dev`, and `ci-latest`'s notes open "The newest build of dev". The header comment lists the three triggers as they stand (a push to `dev` builds and publishes, a push to `master` builds nothing, a dispatch builds and keeps run artifacts); issue 04 adds the tag build to that list. A dispatch on `dev` still leaves `ci-latest` alone, since the publish job asks for a push.
- **`CLAUDE.md`.** "Remotes and branches" has a line for `dev` (everyday work, topic branches, base merges) and one for `master` (moves only at a release by `git merge --ff-only dev`, GitHub's default branch, hotfixes merged back into `dev`); the line about adding `dev` after a first alpha is gone. The base merge line merges into `dev` and says a merge that moves `VERSION` leaves the last fork release paired with the old base release until the next one; its "before it reaches `master`" now says `upstream/master`, as it meant the base port's branch. The CI paragraph names `dev` and says a push to `master` builds nothing. The footprint paragraph's `git log upstream/master..master` became `..dev`, since fork commits to base port files land on `dev` first. "Online compatibility" named no branch, so it is unchanged; the fork version beside the app version is issue 02's.
- **Skills.** `base-merge` merges into `dev` and every survey diff compares against `dev`; `code-review`'s branch diff is `git diff dev...<branch>`; `verify` says a dispatched run, `dev`'s included, leaves `ci-latest` alone. `grep -rn "master" CLAUDE.md .claude/skills` now shows only `master` as the releasing branch, `upstream/master` and "base port master", and the master volume and master switches.
- **Other docs.** `docs/a11y`, `docs/agents`, `CONTEXT.md` and `ACCESSIBILITY.md` name no fork branch; their `master` hits are the master volume and switches, and `netplay.md`'s `upstream/master`.
- **Tags** are left out of "Remotes and branches" until the tag build exists (issue 04 or 06).
- **Verification.** No code changed, so no game build or run: `python tools/check_style.py` passes. This file's commit is the one pushed, so the CI checks come after it: the push to `master` is to start no A11y run (`gh run list`), and the push to `dev` is to publish `ci-latest` at `dev`'s commit (`gh release view ci-latest`). Their result is in the implementer's report; a failure would have reopened this issue.
