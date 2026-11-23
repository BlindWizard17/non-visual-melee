# 02 The fork version and the changelog

Status: resolved (2026-11-23)
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

## Comments

### 2026-11-23, implementation (agent)

- **The version file** is `src/pc/a11y/VERSION`, `0.0.0`. `a11y.cmake` reads it with `CMAKE_CONFIGURE_DEPENDS`, as the root `CMakeLists.txt` reads `VERSION`, and stops on anything but `N.N.N` (no `v`, no suffix) with `a11y: src/pc/a11y/VERSION holds "<value>", not a fork version MAJOR.MINOR.PATCH such as 0.1.0 (no v, no suffix).` Checked with `v0.1`. The configure logs `a11y: Non-Visual Melee <version>, build label "<label>"`.
- **The build label's contract**, for issue 04: `MELEE_A11Y_BUILD_LABEL`, read at configure. Unset or empty gives "local build"; `none` gives no label (the tag build sets this, so an empty value, which a runner may drop, never stands for "unlabelled"); anything else is the label as written. Letters, digits, spaces and `. _ -` only, so it needs no escaping into a C string; anything else stops the configure. `a11y.yml` sets `dev build <short hash>` in a "Label the build" step of the Windows and both Mac jobs (`$GITHUB_ENV`), each step's comment saying a tag build is to set `none` instead: issue 04 makes that step choose `none` on an `a11y-v*` tag. Changing the variable needs a re-configure, as `MELEE_VERSION` does; CI always configures afresh.
- **One place**: `fork_version.hpp`. `a11y::build_versions()` gives a `BuildVersions` with `fork` (the bare version, the one to compare), `label` (empty when unlabelled) and `app` (`pc::get_app_version()`, as written, `v0.4.2`). `labelled_fork_version()` gives "0.1.0 (local build)", and `version_line()` the log line. The two defines are set on `fork_version.cpp` alone, so a new version or label rebuilds one file. Issue 05's shown text ("0.1.0 (Melee-PC v0.4.2)") is a formatter for it to add beside these, from the same struct.
- **The startup log line** is written first thing in `pc_a11y_init`, through `pc_log_line` directly, so neither the switches nor the speech log switch (`MELEE_A11Y_LOG`) silence it: `[a11y] Non-Visual Melee 0.0.0 (local build), Melee-PC v0.4.2`, the run's first `[a11y]` line, in the launcher's runs too. No new hook.
- **Tests**: `fork_version` (`test_fork_version.cpp`): the shown forms with and without a label, and this build's fork version against the version file, whose path is its argument.
- **`CHANGELOG.md`** has a title, a line saying what it is, and Unreleased with `### New` holding the first beta's summary: six bullets from the status lines and `ACCESSIBILITY.md`, then the guide's full URL on `master`. **The maintainer's read of the summary is pending**: the one "Done when" left for them, before the release (issue 07 at the latest).
- **Docs**: CLAUDE.md's "Docs" has the changelog rule (a line or two under New, Changed or Fixed, internal changes nothing) and the full-URL rule; "Online compatibility" has the fork version beside the app version. The `base-merge` skill adds Unreleased lines in the merge commit, always one under Changed when `VERSION` moves, with an example. The `verify` skill names the version line under "Bounded runs" and the configure's refusal under "Build, style and unit tests".
- **Verification**: build passes; the unit tests pass but the known `launcher_data`; `python tools/check_style.py` passes; a bounded `title` run's log opens with the version line; the menus level passes, 38 of 38. A CI build's label is seen first on the next push to `dev` (its log's configure line, and a run's log).
