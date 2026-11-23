# 04 The versioned release workflow

Status: resolved (2026-11-23)
Type: task
Blocked by: 03

Spec: `.scratch/versioned-releases/spec.md`, "The workflow". ADR-0011.

## What to do

- **Trigger**: pushed tags matching `a11y-v*`.
- **`MELEE_VERSION`** is set from the root `VERSION` file in every build job, for every trigger, so a tag build's app version is the base port's and never the tag's name.
- **The checks**, before any build. On any failure the run stops and says which check failed:
  - The tag is `a11y-v` plus `src/pc/a11y/FORK_VERSION`.
  - The first version section of `CHANGELOG.md` is that version.
  - The tagged commit is on `origin/master`.
- **The builds**: the Windows job alone, unlabelled (issue 02's build label). The Mac jobs are skipped on tags.
- **The notes**, in the spec's order:
  1. The header: "Plays online with" the fork and base versions, the requirements, and the link to `ACCESSIBILITY.md` at the tag.
  2. The version's changelog section without its heading.
  3. The link to `CHANGELOG.md` at the tag.
- **Where the logic lives**: the checks and the notes go in a small script, such as `tools/a11y/release_notes.py`, which runs the same locally. Its `--help` says what it checks and prints. Avoid a long inline shell step.
- **The release**:
  - It is created as a draft titled "Non-Visual Melee <version> beta", not a prerelease, with the Windows zip.
  - A run for a tag whose release already exists, draft or published, fails.
- **The header comment** of `a11y.yml` describes the tag build.
- **CLAUDE.md**'s CI paragraph says what a tag does.

## Done when

- The script, run locally against a `CHANGELOG.md` with a test version section, prints the notes and refuses each mismatch. Test the mismatches through its arguments, with no tag pushed.
- A dispatch on a topic branch still builds all three zips and leaves `ci-latest` and the releases alone.
- The tag path itself runs first for real in issue 07, as a draft, which no player sees.
- `python tools/check_style.py` passes.

## Comments

### 2026-11-23, implementation (agent)

- **`tools/a11y/release_notes.py TAG`** holds the checks and the notes; `--help` says each. Its refusals are one line per failed check, all of them reported at once, `release_notes: refused, <tag|changelog|master|release>: <why>`, exit 1, no notes written. `--commit` (default `HEAD`), `--branch` (default `origin/master`), `--repo` (default `ohylli/non-visual-melee`, the links' repository too), `--changelog FILE` (a test changelog in place of the root's), `--notes FILE` (else standard output) and `--outputs FILE` (`version=` and `title=` lines, for `$GITHUB_OUTPUT`). Its test, `tools/a11y/test_release_notes.py` (ctest `release_notes`), runs `main` through its arguments on scratch trees, git and gh faked: the notes' parts and order, the title, each refusal.
- **The checks beyond the issue's three**, each part of the changelog check: an Unreleased section with entries above the version's refuses (an empty one is skipped), since at a release it becomes the version's section, and entries left there would be shipped but missing from the notes; the version heading must be `## <version> (YYYY-MM-DD)`; the section must open with `Plays online with Melee-PC <VERSION>.`, the root `VERSION`'s, so the notes' header and the changelog never disagree; and it must have changes below that line. `FORK_VERSION` must be a whole `MAJOR.MINOR.PATCH`.
- **The release check** is `gh release view TAG --repo ... --json isDraft`: exit 0 is a release already there, named draft or published; "release not found" on stderr is none; anything else (no auth, no network) refuses with gh's words rather than passing. GitHub lists drafts only to a token with push access, so the check job has `contents: write`.
- **The notes**: "Plays online with Non-Visual Melee <version> and Melee-PC <base>.", then a paragraph with the requirements (Windows 10 or 11, 64-bit; a screen reader such as NVDA; the player's own disc image, USA revision 2, no game data included) and the guide's link at the tag; `## Changes` holding the section's body with its own `###` headings; `## Full changelog` with the link to `CHANGELOG.md` at the tag. The `##` headings put the changelog's `###` under a level of their own, so the page reads by heading as title, Changes, New/Changed/Fixed, Full changelog. **The section's own "Plays online with Melee-PC" line is left out of the notes**, which the spec did not say: the header says the same with the fork version added, and a screen reader would read it twice.
- **`a11y.yml`**: `push.tags: ['a11y-v*']` beside `dev`. "Check the release" (`release-check`) runs on a tag alone, before any build, with the full history (`fetch-depth: 0`) so `origin/master` is there, and prints the notes in its log. The Windows job needs it and runs when it passed or was skipped, so a dev push or dispatch is not held up. The macOS jobs skip tags. "Draft the release" (`release`) needs both, checks out the full history again, runs the same command (so a re-run of that job alone, or a release made by hand meanwhile, is refused), and runs `gh release create TAG --draft --verify-tag --title <title> --notes-file notes.md dist/NonVisualMelee-Windows-x86_64.zip`, not a prerelease. A dispatch on a tag (`--ref a11y-v<version>`) takes the same path as its push. The header comment describes it.
- **Versions**: the Windows and Mac jobs' "Label the build" became "Set the version and the build label": `MELEE_VERSION` from `head -n 1 VERSION` on every trigger, and the label `none` when `GITHUB_REF` is `refs/tags/a11y-v*`, else `dev build <short hash>` (the Mac step always the latter, as tags never reach it). Without `MELEE_VERSION` the root `CMakeLists.txt` takes a tag's name, and `a11y-v0.1.0` would also fail its version pattern.
- **Docs**: CLAUDE.md's "Remotes and branches" has a line for the tags (`a11y-v<version>` on `master`, apart from the base port's `v*` tags), and its CI paragraph what a tag does. The `verify` skill's unit tests line names `release_notes`, and a "Release workflow" item says how to check a change to it. `ACCESSIBILITY.md`'s Online play says each release names the melee-pc version it plays with at the top of its notes. No line under Unreleased: the release notes are new, but the first beta's section is a summary.
- **Verification**: `python tools/check_style.py` passes; the build passes and the unit tests pass 43 of 44, all but the known `launcher_data`, `release_notes` among them. No game code changed, so no game run. The script run locally on a copy of `CHANGELOG.md` with a `## 0.0.0 (2026-11-23)` section and `--commit origin/master` printed the notes and `passed every check: "Non-Visual Melee 0.0.0 beta"`; it refused `a11y-v0.0.1` (tag), the real `CHANGELOG.md` (Unreleased has entries), `HEAD` with `dev` ahead of `origin/master` (master), `--branch origin/nothing` (git's own error), and `ci-latest` (tag, and `already has a published release`).

- **CI**: dispatch run 45154960029 on `ci/release-workflow` (at `787b2045`, since deleted) passed: "Check the release" and "Draft the release" skipped, as was "Publish ci-latest", and all three build jobs passed, keeping the three zips as artifacts; `ci-latest` stayed at its 09:18 build of `2accaab2` and the repository has no other release. The Windows job's log shows `MELEE_VERSION: v0.4.2` and the label `dev build 787b204`. The arm64 job first sat queued for 15 minutes and was cancelled by GitHub ("not acquired by Runner", macOS arm64 capacity), and passed on a re-run of that job alone.
- **Left open**: the tag path's first real run is issue 07's, as a draft. The release-check job's `--commit "${GITHUB_SHA}"` and `gh release view` against drafts with the job's token are the parts only that run can show; a failure there leaves a tag with no release, which the release skill reports.
