# 06 The release skill

Status: resolved (2026-11-23)
Type: task
Blocked by: 04

Spec: `.scratch/versioned-releases/spec.md`, "The release skill" and "Branches". Write it with the `writing-for-agents` skill.

## What to do

`.claude/skills/release/SKILL.md`, invoked by the maintainer as `/release`.

- **A release**:
  1. Check that `dev` is clean and pushed, and that `CHANGELOG.md`'s Unreleased section has entries.
  2. Ask the maintainer for a minor or a patch bump, and show the version that gives.
  3. Rename Unreleased to `<version> (<date>)`, add the "Plays online with Melee-PC <base version>." line from `VERSION`, and bump `src/pc/a11y/FORK_VERSION`. Commit on `dev`.
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

## Comments

### 2026-11-23, implementation (agent)

- **The skill** is `.claude/skills/release/SKILL.md`, user-invoked (`disable-model-invocation: true`), in three parts: "A release" in seven steps, "A hotfix" as the release's steps with their differences plus the merge into `dev` and the base version comparison, and "When it fails". It says once, at the start, that the maintainer may ask for `verify` first.
- **The pre-flight** is `release_notes.py a11y-v<version> --commit master --branch master --notes <scratch>/notes-<version>.md`, after the local fast-forward and before the tag. `--branch master` because the script's default, `origin/master`, holds the release commit only after the push; locally the master check is then trivial, and the tag build makes it for real against `origin/master`. The script reads the version files and the changelog from the working tree, which at that point is the release commit. A refusal comes before any push: `master` is put back with `git branch -f master origin/master` (step 1 checked they were level) and the release commit amended on `dev`. `--notes` rather than a redirect: on Windows Python's standard output writes CRLF, and the notes file is diffed against the draft's body (`diff -B`, which `gh` writes with LF and one trailing blank line more).
- **The push** is `git push --atomic origin dev master a11y-v<version>`, so the tag never reaches GitHub without `master` holding its commit, which the tag build's master check would refuse. The tag is annotated (`git tag -a`), as the base port's `v*` tags are; the master check's `git merge-base` peels a tag either way.
- **No empty Unreleased after a release**, a choice beside the spec's shape ("An `## Unreleased` section at the top"): the release renames the section and adds none, so the release's `changelog.html` and `master`'s `CHANGELOG.md` open with the release, not an empty heading a screen reader lands on first. The script would accept either (it skips an empty Unreleased). CLAUDE.md's "Docs" rule now says the first change a player meets after a release adds `## Unreleased` back at the top; a hotfix's merge into `dev` keeps `dev`'s Unreleased above the hotfix's section.
- **The watch** is `gh run watch <id> --exit-status --compact --interval 60`, in the background: the last tag-like runs (dispatches) took about twenty minutes, longer than a foreground command may run. The run is found by `gh run list --workflow a11y.yml --branch a11y-v<version> --event push`.
- **Publishing** is `gh release edit --draft=false --latest`, checked by `gh api repos/ohylli/non-visual-melee/releases/latest --jq .tag_name` naming the tag, since `releases/latest` is the link `ACCESSIBILITY.md` gives.
- **Failures**: the skill tells apart a failed check (the `release_notes: refused` lines in `--log-failed`), a failed Windows build and a failed "Draft the release", re-runs a failure that is not the code's with `gh run rerun --failed` (deleting only a draft, keeping the tag, when the draft job had created one), and otherwise deletes the draft with its tag (`gh release delete --cleanup-tag`) or the bare tag (`git push origin --delete refs/tags/...`) on the maintainer's yes, then fixes in new commits and runs again from step 4 with the same version and section.
- **Checked against help and dry runs**, nothing pushed, tagged or released: `git push -h` (`--atomic`, `--delete`), `git tag -h` (`-a`, `-m`, `-d`), `git merge -h` (`--ff-only`); `gh` 2.83.2's help for `run watch` (`--compact`, `--exit-status`, `--interval`), `run list` (`--workflow`, `--branch`, `--event`, `--json`), `run view` (`--log-failed`), `run rerun` (`--failed`), `release view` (its `--json` fields: `name`, `isDraft`, `isPrerelease`, `assets`, `body`, `url`), `release edit` (`--draft`, `--latest`), `release delete` (`--cleanup-tag`, `--yes`) and `release download` (`-p`, `-D`); `gh release view ci-latest` with the step 6 `--json ... --jq` and plain; `gh run list --workflow a11y.yml`; `gh api .../releases/latest` (404 today: no full release yet); `git merge --ff-only dev` from `master` in a scratch worktree (fast-forwarded `2accaab2..cdef7afb`; worktree and branch removed); the pre-flight on a scratch copy of `CHANGELOG.md` with Unreleased renamed as step 3 does it (`## 0.0.0 (2026-11-23)`, the Plays online line), which passed every check with `--commit master --branch master` and refused `--commit dev` against `origin/master`; `python -m zipfile -l` for the listing. `python tools/check_style.py` passes. No code changed, so no build or game run.

Left open, for issue 07:

- The parts only a real tag can show: the run's `headBranch` being the tag (so `--branch` finds it), `gh release view`, `download` and `delete` reaching a draft by its tag, and the draft's body matching the pre-flight's notes under `diff -B`.
- `dev` is 7 commits ahead of `origin/dev`: step 1 stops until the maintainer pushes it.
- The maintainer's read of the first beta's summary in `CHANGELOG.md` (issue 02's open "Done when") belongs before step 3.
