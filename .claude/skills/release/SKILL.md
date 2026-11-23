---
name: release
description: Cut a Non-Visual Melee release from dev, or a hotfix from master: the changelog and version, the tag and push, the tag's CI run, and the draft release checked and published.
disable-model-invocation: true
---

A release fast-forwards `master` to `dev`, tags it `a11y-v<version>` and pushes. The tag's run of `.github/workflows/a11y.yml` checks the tag, builds the Windows zip and creates the release as a **draft**, invisible to players; this skill checks the draft and publishes it on the maintainer's yes (ADR-0011, `CLAUDE.md`'s "Remotes and branches"). `<version>` is the fork version, `src/pc/a11y/FORK_VERSION`; the base version is the first line of the root `VERSION`. `<scratch>` is a folder of the session's scratchpad.

Invoking the skill is the maintainer's leave to push `dev`, `master` and the release's tag, and nothing else. Deleting anything on GitHub waits for their yes.

Verify is not part of a release. Say once, as you start, that the maintainer may ask for the `verify` skill before the release goes ahead.

## A release

### 1. Check dev

```
git fetch origin
git status --porcelain
git rev-parse dev origin/dev master origin/master
git merge-base --is-ancestor master dev
```

Ready when: the current branch is `dev` with nothing in `git status --porcelain`; `dev` equals `origin/dev` and `master` equals `origin/master`; the ancestor check exits 0; and `CHANGELOG.md`'s `## Unreleased` holds entries under `### New`, `### Changed` or `### Fixed`. Otherwise stop and say which failed. An unpushed `dev` is pushed by the maintainer, or with their leave. A failed ancestor check means `master` holds a hotfix not yet merged into `dev`, which the hotfix's step 8 does.

### 2. Ask for the bump

Ask for a minor bump (new features) or a patch bump (fixes alone), showing the version each gives from `FORK_VERSION` (from 0.1.0: minor 0.2.0, patch 0.1.1) and the base version it will play online with. A major bump only when the maintainer asks for it; from 1.0.0 the release's title drops "beta".

### 3. Commit the release on dev

- `CHANGELOG.md`: the line `## Unreleased` becomes `## <version> (<YYYY-MM-DD>)`, today's date, followed by a blank line, `Plays online with Melee-PC <base version>.` and a blank line, then the `###` sections as they were. No `## Unreleased` stays above it, so the release's `changelog.html` and `master`'s `CHANGELOG.md` open with the release itself; the next change a player meets adds the heading back (`CLAUDE.md`, "Docs").
- `src/pc/a11y/FORK_VERSION`: the new version, alone on its line.
- Commit both on `dev`: `A11y: Non-Visual Melee <version>`.

### 4. Fast-forward master, tag and push

```
git switch master
git merge --ff-only dev
python tools/a11y/release_notes.py a11y-v<version> --commit master --branch master --notes <scratch>/notes-<version>.md
git tag -a a11y-v<version> -m "Non-Visual Melee <version>" master
git push --atomic origin dev master a11y-v<version>
git switch dev
```

`release_notes.py` is the **pre-flight**: the tag build's own checks (its `--help` names each), run before anything is pushed. It passes when its last line is `passed every check: "<title>"`. It reads `FORK_VERSION`, `VERSION` and `CHANGELOG.md` from the working tree, which here is the release commit. `--branch master` names the local branch: the script's default, `origin/master`, holds the release commit only after the push, and the tag build checks against `origin/master` then. Locally the check confirms only that the commit to tag is `master`'s.

A refusal (`release_notes: refused, <check>: <why>`) comes before any push. Put `master` back with `git switch dev` and `git branch -f master origin/master`, fix the problem on `dev` (amending the unpushed release commit), and run this step again.

`--atomic` pushes the three refs together or none of them, so the tag never reaches GitHub without `master` holding its commit. The push to `dev` also rebuilds `ci-latest`, in a run of its own.

### 5. Watch the tag's run

```
gh run list --workflow a11y.yml --branch a11y-v<version> --event push --limit 1 --json databaseId,status
gh run watch <databaseId> --exit-status --compact --interval 60
```

The run is listed within seconds of the push; list again until it is. The watch lasts as long as the Windows build, about twenty minutes, so run it in the background and wait for its exit. Exit 0 means it passed: `gh run view <databaseId>` shows "Check the release", "Windows (x86_64)" and "Draft the release" passed, the macOS jobs and "Publish ci-latest" skipped. Any other exit: "When it fails".

### 6. Check the draft

```
gh release view a11y-v<version> --json name,isDraft,isPrerelease,assets --jq '{name,isDraft,isPrerelease,assets:[.assets[].name]}'
gh release view a11y-v<version> --json body --jq .body > <scratch>/draft-<version>.md
diff -B <scratch>/notes-<version>.md <scratch>/draft-<version>.md
gh release download a11y-v<version> -p NonVisualMelee-Windows-x86_64.zip -D <scratch>/release-<version>
python -m zipfile -l <scratch>/release-<version>/NonVisualMelee-Windows-x86_64.zip
```

The draft is right when:

- its name is `Non-Visual Melee <version> beta`, `isDraft` is true, `isPrerelease` false, and its one asset is `NonVisualMelee-Windows-x86_64.zip`;
- `diff -B` prints nothing: the notes are the pre-flight's;
- the zip's listing holds `melee-windows-x86_64/melee.exe` with `readme.html` and `changelog.html` beside it. The zip is listed, never extracted or run.

Then show the maintainer the notes as `gh release view a11y-v<version>` prints them, say what was checked, and ask whether to publish.

### 7. Publish

On the maintainer's yes:

```
gh release edit a11y-v<version> --draft=false --latest
gh api repos/ohylli/non-visual-melee/releases/latest --jq .tag_name
gh release view a11y-v<version> --json url --jq .url
```

Published when the second command prints `a11y-v<version>`: `releases/latest`, the link `ACCESSIBILITY.md` gives players, reaches the release. Give the maintainer the release's link, and remove `<scratch>/release-<version>`.

A no leaves the draft as it is, seen by no player: ask what should change, and go on as "When it fails" says for a wrong draft.

## A hotfix

A fix that cannot wait for `dev`, released as a patch from `master`. Before the skill starts, the fix is committed on `master`, or merged into it from a short branch made from `master`. The steps are the release's, with these differences:

1. **Check master**: the current branch is `master`, the working tree clean, `git merge-base --is-ancestor origin/master master` exits 0, and `git log --oneline origin/master..master` lists the fix, which you show the maintainer. `dev` equals `origin/dev`, since step 8 merges into it.
2. **The bump** is a patch, shown without asking.
3. **The changelog section is written directly** on `master`: a new `## <version> (<YYYY-MM-DD>)` section above the last release's, with the "Plays online with" line and the fix in a player's words under `### Fixed`. `FORK_VERSION` is bumped and both are committed on `master`.
4. **No fast-forward**: the pre-flight, the tag and `git push --atomic origin master a11y-v<version>`; `dev` is not pushed yet. A refusal is fixed by amending the release commit on `master`.
5. to 7. As a release.
8. **Merge master into dev** and push it: `git switch dev`, `git merge master`, `git push origin dev`. `CHANGELOG.md` conflicts when `dev`'s Unreleased holds entries: keep `## Unreleased` with them first, then the hotfix's section, then the older releases. Other conflicts follow the `resolving-merge-conflicts` skill.
9. **Compare the base versions**: `git show dev:VERSION` against `git show a11y-v<version>:VERSION`. They differ when `dev` holds a base merge no release has shipped: say so, since fork players pair online with the release's base version until the next release from `dev`.

## When it fails

Say which job failed and quote its error. A run never replaces a release, draft or published, so a retry needs the failed one gone.

- **"Check the release" failed**: `gh run view <databaseId> --log-failed | grep "release_notes: refused"` names each failed check. Nothing was built; the tag is on GitHub with no release.
- **"Windows (x86_64)" failed**: no draft exists; `gh run view <databaseId> --log-failed` has the error. A failure that is not the code's (a runner never acquired, a download timing out) is re-run with `gh run rerun <databaseId> --failed`, then watched again (step 5), with nothing deleted.
- **"Draft the release" failed**: when `gh release view a11y-v<version>` finds a draft, it was created before the failure, and a re-run is refused until the draft is deleted. When the failure is not the code's, delete the draft alone (`gh release delete a11y-v<version> --yes`, which keeps the tag), then `gh run rerun <databaseId> --failed`.
- **The draft is wrong** (step 6), or the maintainer declined it.

To start again, ask the maintainer's yes for each removal, naming it:

- a draft and its tag: `gh release delete a11y-v<version> --cleanup-tag --yes`, then `git tag -d a11y-v<version>`;
- a tag with no release: `git push origin --delete refs/tags/a11y-v<version>`, then `git tag -d a11y-v<version>`.

Then fix the problem on `dev` (a hotfix: on `master`) in new commits, since the release commit is already pushed, and run the release again from step 4. The version keeps its number and its section, as no release of it was published: a fix players meet goes into that section, and its date becomes today's if the day has changed. Until then `master` on GitHub stands ahead of the last release.

A published release is never deleted or replaced: a broken one is fixed by the next patch, as a hotfix.
