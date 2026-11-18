# 02 Erase Data

Status: ready-for-agent
Type: task
Blocked by: 01

Spec: `.scratch/options-screens/spec.md`. Read it first, and issue 01's comments.

## Machinery

- **Snapshot**: `game_access.c` reads `mnDataDel_804D6C68`'s user data while `cur_menu` is 24: the row (`x0`), the prompt (`x1`), its choice (`x2`, 1 Yes) and the erased marks (`x3`). The speech follows these, never the drawn highlight, which in the base port does not match the choice.
- **Descriptions**: a fork copy of `mnDataDel_803EF8C8` (static in `mndatadel.c`; a comment naming the original), decoded by number through `menu_string()`. The prompts' words are game text 318 and 319.
- **Row names**: a fork table, as drawn (spec, "Speech: Erase Data").

## Speech in this slice

Wording is in the spec, "Speech: Erase Data". The screen loses its "No speech yet." notice.

## Done when

- `cmake --build build` passes, and the unit tests pass, with tests for the opening, moves with the wrap, every row's name, an erased row, A on an erased row, both prompts with their choices, the hint on the first prompt of a visit only and again after reopening, closing by each way, and Erase All marking every row.
- A drive script opens Erase Data, walks the rows, opens a prompt and backs out with No and with B, erases one row and hovers it again, and takes Erase All through both prompts. It runs with no card, the default, or `--card` with a card of its own, never the maintainer's.
- `python tools/check_style.py` passes.
- The `verify` skill updated; the primer records the screen's state, the prompts, the erased marks and the highlight finding.
