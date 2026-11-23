# 06 The release skill

Status: ready-for-agent
Type: task
Blocked by: 04

Spec: `.scratch/versioned-releases/spec.md`, "The release skill" and "Branches". Write it with the `writing-for-agents` skill.

## What to do

`.claude/skills/release/SKILL.md`, invoked by the maintainer as `/release`.

- **A release**:
  1. Check that `dev` is clean and pushed, and that `CHANGELOG.md`'s Unreleased section has entries.
  2. Ask the maintainer for a minor or a patch bump, and show the version that gives.
  3. Rename Unreleased to `<version> (<date>)`, add the "Plays online with Melee-PC <base version>." line from `VERSION`, and bump `src/pc/a11y/VERSION`. Commit on `dev`.
  4. Fast-forward `master` to `dev` (`--ff-only`), tag `a11y-v<version>`, and push `dev`, `master` and the tag. Invoking the skill is the maintainer's leave to push these three; nothing else is pushed.
  5. Watch the tag's run (`gh run watch --exit-status`).
  6. Check the draft: its title, its notes as `gh release view` prints them, one Windows zip, and the zip's listing with `readme.html` and `changelog.html` (downloaded to the scratchpad, never run). Show the notes to the maintainer.
  7. Publish the draft on the maintainer's yes and give the release's link.
  8. Say when `dev`'s `VERSION` differs from the release's base version, which happens after a base merge.
- **A hotfix**: the same steps from `master`. The fix is committed there or merged from a short branch, and its changelog section is written directly under the new patch version. The skill ends by merging `master` into `dev` and pushing `dev`.
- **On failure**:
  - A failed check or build leaves a tag with no release, or a draft. The skill says what failed.
  - It deletes the draft and the tag (local and remote) only with the maintainer's yes, then fixes the problem on `dev` and starts again.
- **Not part of it**: verify. The skill says once that the maintainer may ask for it.
- **CLAUDE.md** names the skill in "Remotes and branches".

## Done when

The skill reads clearly start to end, and every command in it has been checked against `git` and `gh` help or a dry run (`git merge --ff-only` on a scratch branch, `gh release view ci-latest`). Its first real run is issue 07.
