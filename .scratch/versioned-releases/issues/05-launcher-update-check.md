# 05 The launcher follows fork releases

Status: resolved (2026-11-23)
Type: task
Blocked by: 02

Spec: `.scratch/versioned-releases/spec.md`, "The launcher's update check". ADR-0011 and ADR-0007, on why the fork edits the launcher's files.

## What to do

- **Fork code** (`src/pc/a11y/`):
  - It picks the release to offer from the release list's JSON: tags `a11y-v<version>` only, newer than the fork version, with a zip for this platform.
  - It gives the repository's API path, the release page's URL, the zip's name, the version to show and the messages.
  - It gives the shown version text, the fork version with the label and the base version.
  - It has unit tests on canned release lists: `ci-latest` skipped, a base port tag skipped, a release without this platform's zip skipped, a draft skipped (the API lists drafts only to the owner, but test it), older, equal and newer versions, and no release at all.
- **The base port's lines**:
  - `updater.cpp` and `launcher.cpp` call that code where they now use the base port's repository, release page, asset names, version and messages, and where the launcher and port menu show the app version (`app-version`, `check-status`, `menu-version`, `port-update-status`, and the update card's tag).
  - Each line is one call, declared in `a11y_hooks.h` and marked `a11y`.
  - Keep the count small: where one call can replace a block's result, prefer it to several.
- **A developer switch** that reads the release list from a file instead of the network. A drive run can then hear "Update available" from a canned list without touching GitHub. `drive.py --launcher` gains a way to leave the update check on for such a run.
- **Docs**:
  - CLAUDE.md's Launcher status line names the update check.
  - The `verify` skill gets the switch and the drive script.
  - `docs/a11y/rmlui-screens.md` gets a short note on the update check's wiring, beside "The wiring in the base port's files".
  - `ACCESSIBILITY.md` says the launcher's update check tells of new Non-Visual Melee releases.

## Done when

- `cmake --build build` and the unit tests pass.
- A drive run with `--launcher`, the update check on and a canned list holding a newer fork release says "Update available" with the version. The status line names Non-Visual Melee.
- With a list holding only base port releases and `ci-latest`, the run says it is up to date with the fork version.
- `grep -rn a11y src/pc/updater.cpp src/pc/launcher.cpp` shows each new line marked.
- `python tools/check_style.py` passes.

## Comments

### 2026-11-23, implementation (agent)

- **Fork code.** `update_check.cpp` picks the release and gives the words; `update_hooks.cpp` hands them to the base port's files. The pick reads the list's JSON with a small parser of its own (the base port's, in `updater.cpp`'s anonymous namespace, drops the `draft` flag and unicode escapes) and takes the newest release that is tagged `a11y-v` plus a whole `MAJOR.MINOR.PATCH`, newer than `build_versions().fork` by number, neither a draft nor a prerelease, and holding `NonVisualMelee-<Windows|macOS>-<x86_64|arm64>.zip`. A body that is not a JSON array is a failure, with GitHub's own message when it has one: its rate limit is an object, which the base port's check took as "up to date". `fork_version.hpp` gained `shown_version()`.
- **Prereleases are skipped too**, beyond the spec's list: the release workflow publishes full releases, and a prerelease would be one the maintainer kept from `releases/latest` on purpose, so the launcher does not push it either. `ci-latest` is a prerelease but is skipped by its tag already.
- **The words.** Status lines: "Non-Visual Melee is up to date (0.1.0)." and "Update available: Non-Visual Melee 0.1.1", the same in the launcher's Settings, the port menu and the updater's message; the port menu's short "Up to date (v0.4.2)" took the long form, which fits its row. The banner: its tag "0.1.1" (the reader says "Update available: 0.1.1"), the release's own title ("Non-Visual Melee 0.1.1 beta"), and "A newer version of Non-Visual Melee is available (0.1.1)." before the base port's Windows instructions. A failure keeps the base port's "Could not check for updates: <reason>".
- **The base port's lines**, each one call declared at the end of `a11y_hooks.h` and marked `a11y`. `updater.cpp`: the include; `bool ok = pc_a11y_update_list_file(body)` and `ok = ok || ...` on the WinHTTP and curl fetches, which name the fork's repository through `pc_a11y_update_api_path()` and `pc_a11y_update_api_url()`; `return pc_a11y_update_result(body, g_updater_state);` before the base port's choice, which stays below, unreached and unchanged rather than deleted, so a base merge conflicts only where upstream changes these lines; `pc_a11y_update_releases_page()` as the browser's fallback. `launcher.cpp`: nine lines, the two status lines' update and up-to-date branches, `app-version`, `check-status`'s "Current version: ", `menu-version`, and the banner's tag and description. `grep -n a11y src/pc/updater.cpp src/pc/launcher.cpp` shows each. The base port's updater test compiles `updater.cpp`, so `a11y.cmake` adds the fork's side and a silent `pc_log_line` (`test_updater_log.cpp`) to it.
- **Spec open question 2, `SemVer`.** It would parse `0.1.0` once `a11y-` is stripped (it takes an optional `v`), but it is lenient: `1` parses as 1.0.0, trailing text is ignored, and a suffix orders as a prerelease. A fork tag must be a whole `MAJOR.MINOR.PATCH`, as `a11y.cmake` holds `FORK_VERSION` to, so the fork parses and compares the three numbers itself; `SemVer` and `select_best_asset` are bypassed, not bent.
- **Spec open question 3, the label.** The shown version puts the label first in the brackets: "0.1.0 (local build, Melee-PC v0.4.2)", "0.1.0 (dev build 1a2b3c4, Melee-PC v0.4.2)", and "0.1.0 (Melee-PC v0.4.2)" on a release. It is shown in the launcher's title bar, the port menu's title bar and Settings' first "Current version: " line. Screenshots of both title bars at the launcher's window size show it on one line with room to spare, enough for a dev build's longer label. No reader says the title bars, and "Current version: " lasts only until Settings first refreshes, so a player hears the fork version from the status lines alone; see "Left open".
- **The developer switch** `MELEE_A11Y_UPDATE_LIST` names a release list file read in place of GitHub's; missing or empty, the check fails with "no release list in <path>". Each check logs `[a11y] update check (canned list): <message>`. `drive.py --update-list FILE` sets it and writes `check_updates 1` into the run's `launcher.cfg`; nothing leaves the check on without a list, so no run reaches the network. The canned lists are in `tools/a11y/update_lists/`: `newer.json` (ci-latest, a draft, a release with only the Mac zips, `a11y-v99.0.0`, `a11y-v0.0.0`, two base port releases) and `base_only.json` (ci-latest and two base port releases).
- **Tests.** `update_check` (`test_update_check.cpp`): ci-latest, base port tags, near-miss tags, a release without this platform's zip, a draft, a prerelease, older, equal and newer versions, numeric order, no release, unreadable lists and GitHub's refusal, escapes; then the hand-offs filling the updater's state from a canned file. `fork_version` covers `shown_version`. Mutating the version comparison or the draft check fails the suite.
- **Drive scripts**, both in the menus level: `launcher_update.drive` (newer.json: the banner's three lines, its buttons, Settings' Updates row and Check now) and `launcher_up_to_date.drive` (base_only.json: no banner, which `run_level.py` checks, the Updates row, Check now, and the port menu's Check for updates row).
- **Verification.** Build passes; unit tests pass but the known `launcher_data`; `python tools/check_style.py` passes; both scripts pass alone; the menus level, 40 scripts at the default eight at once, passed 25 with both new ones, and the 15 others failed on lost presses, surface errors and a slow launcher (the flakes the `verify` skill names, `training_menu`'s ten numbers among them); all 15 passed on an unchanged rerun, four at once.
- **Docs.** CLAUDE.md's Launcher line, the `verify` skill (`--update-list`, the switch, its log line, both scripts), `docs/a11y/rmlui-screens.md` ("The update check" and its wiring, after the Accessibility category), `ACCESSIBILITY.md` (an "Update check" feature) and a sentence in `CHANGELOG.md`'s first-beta summary.

Left open:

- **The base port's version is never heard.** The spec puts it beside the fork version where the app version was shown, and those places are visual. If a player should hear which Melee-PC release they pair with (the release notes say it too), the up-to-date line could carry the shown version: "Non-Visual Melee is up to date (0.1.0, Melee-PC v0.4.2)." For the maintainer to judge in issue 07.
- **Base port quirks kept:** the port menu's update row keeps "Checking for updates..." until its next refresh (as `port_menu.drive`'s comment says), and its description is always "Up to date". Neither is the fork's to change here.
- The update check is not yet heard by ear; issue 07 has the maintainer hear it with a kept `0.0.0` zip.
