# 07 The first release, 0.1.0

Status: ready-for-human
Type: task
Blocked by: 05, 06

Spec: `.scratch/versioned-releases/spec.md`. The maintainer runs `/release` with a minor bump, from `0.0.0` to `0.1.0`, and checks the result by ear.

## What to try

- **Before the release**, download `ci-latest`'s Windows zip and keep it. It is labelled a dev build at `0.0.0`, and is used below to hear the update notice.
- **The draft's notes**, as the skill shows them: is the header's "Plays online with" line clear? Are the requirements right? Do the headings work in NVDA's heading navigation once published?
- **`readme.html` and `changelog.html`** from the release zip, opened in a browser: do they read well by heading, or does pandoc's page need its contents list or style changed (the spec's open question 1)?
- **The launcher** of the release zip: does it say the version as you want it heard ("0.1.0 (Melee-PC v0.4.2)"), and "Non-Visual Melee is up to date"?
- **The update notice**: open the launcher of the kept `0.0.0` zip, with the update check on. Does it say "Update available: 0.1.0", and does Open in browser reach the release page?
- **The link** in `ACCESSIBILITY.md` (`releases/latest`) reaches 0.1.0, not `ci-latest`.

## Done when

The release is published, and the maintainer's findings are recorded below, each fixed or filed as its own issue.

## Comments
