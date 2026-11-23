# 05 The launcher follows fork releases

Status: ready-for-agent
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
