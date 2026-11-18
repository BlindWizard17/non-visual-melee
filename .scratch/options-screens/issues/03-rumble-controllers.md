# 03 Rumble: the controller panels

Status: resolved (2026-11-18)
Type: task
Blocked by: 02

Spec: `.scratch/options-screens/spec.md`. Read it first, and the comments of issues 01 and 02.

## Machinery

- **Snapshot**: `game_access.c` reads `mnVibration_804D6C28`'s user data while `cur_menu` is 19: each controller's place (`x0[2 + port]`) and connection (`x6[port]`), and each controller's rumble (`GetRumbleSettingOfPort`). The names list's part of the snapshot is issue 04's.
- **Every controller** is followed; a line about a panel names it "P1" to "P4".
- **The bottom line** is game text 0xA7, decoded by number; the screen draws it without `Menu_InitCenterText`.

## Speech in this slice

Wording is in the spec, "Speech: Rumble": the opening, switching a controller's rumble, and controllers coming and going. With no names, Right says nothing, as the game does nothing. The screen loses its "No speech yet." notice.

## Done when

- `cmake --build build` passes, and the unit tests pass, with tests for the opening with one controller and with several (disconnected ones left out), switching each controller, a controller connecting and disconnecting, and the hint without names.
- A drive script opens Rumble with the default save (no names), switches P1 off and on again, and waits on each line. A second controller is checked by the unit tests alone if the drivers cannot reach one; say which.
- `python tools/check_style.py` passes.
- The `verify` skill updated; the primer records the screen's state.

## Comments

### 2026-11-18, implementation (agent)

Implemented in `src/pc/a11y/rumble_reader.cpp` on issue 01's `leaf_reader.hpp`, polled from the menu snapshot with no new hook.

- **Snapshot.** `A11yMenuState.rumble` (`A11yRumble` in `game_access.h`), filled by `rumble_state` in `game_access.c` only while `cur_menu` is Rumble, all zero elsewhere: per controller (`A11yRumblePort`) whether it is plugged in (`x6[port]`), its place (`in_names`, from `x0[2 + port]`) and its rumble (`GetRumbleSettingOfPort`), and the bottom line, SdMenu 0xA7 through `menu_string()`. `MnVibrationData` is declared only in `mnvibration.c`, so `game_access.c` mirrors its first eleven bytes (`a11y_rumble_head`, the scroll offset included) and declares the global `mnVibration_804D6C28`. B frees the gobj at once and leaves the global as it was, so it is read only while the screen is up, as Erase Data's; the gobj is made in the call that sets `cur_menu` to 19, so the arrival frame sees it. The names list's fields go into `A11yRumble` beside the ports.
- **Finding.** The decomp's comment on `MnVibrationData.x0` calls `x0[2..5]` a per-port rumble toggle; the input code shows it is each controller's place, 0 its panel and 1 the names list. The rumble setting is in the save. `x6` is set as the screen opens and updated by `mnVibration_Think`, which only starts once the intro has played, so a controller plugged in during the intro is said a moment later.
- **Reader.** `RumbleReader`, a member of `MenuSpeech` with its case in `leaf_reader()`. The opening says each plugged-in controller's panel by port order, the keys ("A to turn on or off.") and the bottom line; a frame says, per controller in port order, joined into one announcement, a connection change ("P2: Off" with its current value, "P2 disconnected") or, while plugged in, a rumble change ("P1: Off"). A disconnected controller's setting changing says nothing. The place (`in_names`) is in the snapshot but the reader ignores it: entering and leaving the names list, and the hint's "right for the names list", are issue 04's. With no names Right does nothing in the game and nothing is said.
- **Wording.** As the spec. A new no-card save has rumble on for P1.
- **Tests.** Nine in `test_menu_speech.cpp`, with a `rumble_frame` fixture helper taking each port's connection and rumble: the opening with one controller (the hint without names), with several and the disconnected left out, with none, switching each of four controllers, two switching in one frame, connecting and disconnecting (and a disconnected controller's setting changing silently), unchanged frames silent, reopening, and a missing bottom line logged once. The tree's "No speech yet." test moved from Rumble to VS. Records.
- **Second controller.** The drivers reach port 1 alone: in a drive run its virtual pad is plugged in and ports 2 to 4 show the red cross. A second controller, its switching, plugging in and unplugging, is checked by the unit tests alone. The drive script's opening wait allows extra panels in case a real controller is plugged in on another port.
- **Drive script.** `tools/a11y/rumble.drive`, no card: the opening, A twice (Off, On), Right with no names, A twice again, B back to Options. In `run_level.py`'s menus set, with a count of four P1 lines (Right taking P1 off its panel would leave A silent and fail it) and the old "Rumble. No speech yet." absent. Every step matched on the first run. `tree_walk.drive` now opens VS. Records from Melee Records as its leaf screen without a reader.
- **Verification.** Menus level: build with the "Rollback sections" line, style, unit tests (39 of 40; `launcher_data` failed at its known spot), `run_level.py menus` 37 of 37 passed on the first run.
- **Docs.** `docs/a11y/native-menus.md`, "Rumble" under "The Options screens in detail" (state, controllers, input, words); `CLAUDE.md`'s main menu tree line, with the names list waiting for 04; `ACCESSIBILITY.md` Rumble in the Options line and a Rumble line saying the names list is not read yet, for 04 to replace; the `verify` skill the new script and `tree_walk.drive`'s new leaf; `menu_speech.hpp`'s header comment.
- **For 04.** Add the names list's fields to `A11yRumble` (cursor row `x0[1]`, `scroll_offset`, `GetNameCount()`, the hovered name's `namedata` and `rumble_enabled`); `a11y_rumble_head` already mirrors `names_row` and `scroll_offset`. `RumbleReader::frame` handles only panels today: a controller's `in_names` changing should be checked before its rumble, and the opening should treat a controller already in the list (possible only after `forget()`). The hint gains ", right for the names list" when names exist. `rumble.drive`'s COUNT of P1 lines assumes no names; the names script is a separate `--card` run.
- **Left open.** The names list (04), the play test (05). `game_access.c` changed, which a Mac build compiles differently; the CI run on a topic branch waits for the maintainer's leave to push.
