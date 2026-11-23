# 02 The fork version and the changelog

Status: ready-for-agent
Type: task

Spec: `.scratch/versioned-releases/spec.md`, "Versions" and "CHANGELOG.md". ADR-0011.

## What to do

- **The version file**: `src/pc/a11y/VERSION` holding `0.0.0`.
  - `a11y.cmake` reads it into a define for fork code, re-configuring when it changes, as the root `CMakeLists.txt` does with `VERSION`.
  - It stops the configure on a value that is not `N.N.N`.
- **The build label**: an environment variable CI sets, read at configure time.
  - A tag build is unlabelled, other CI builds say "dev build" and the short hash, and a local build says "local build".
  - Setting it in `a11y.yml` belongs here. The tag build's own setting comes with issue 04.
- **One place in fork code gives both versions** and the label, for the log line here and the launcher in issue 05.
- **The startup log line**: one `[a11y]` line naming the fork version with its label and the app version, such as "Non-Visual Melee 0.1.0 (local build), Melee-PC v0.4.2". It is written whether or not accessibility is on.
- **`CHANGELOG.md`** at the repo root, in the spec's shape.
  - It has an Unreleased section holding a short summary of the first beta: what is accessible, from CLAUDE.md's status lines and `ACCESSIBILITY.md`, in players' words, pointing to `ACCESSIBILITY.md` by full URL. Keep it short, since the maintainer reads it before the release.
- **CLAUDE.md**:
  - "Docs": a change a player meets adds its line under Unreleased in `CHANGELOG.md`, beside keeping `ACCESSIBILITY.md` in step.
  - "Docs": `ACCESSIBILITY.md` and `CHANGELOG.md` link only by full URL.
  - "Online compatibility": the fork version is the fork's own, beside the base port's unchanged app version.
- **The `base-merge` skill**: a merge that moves `VERSION` adds a line under Unreleased, and so does one whose base port changes a player would notice.
- **The `verify` skill**: the new log line where it lists what a bounded run shows.

## Done when

- `cmake --build build` passes, and a bounded run's log has the version line with "local build".
- Changing `src/pc/a11y/VERSION` to a bad value stops the configure with a clear message.
- `CHANGELOG.md` exists with its Unreleased section, and the maintainer has read the first beta's summary.
- `python tools/check_style.py` passes.
