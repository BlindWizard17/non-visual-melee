# 03 Rumble: the controller panels

Status: ready-for-agent
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
