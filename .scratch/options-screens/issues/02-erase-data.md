# 02 Erase Data

Status: resolved (2026-11-18)
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

## Comments

### 2026-11-18, implementation (agent)

Implemented in `src/pc/a11y/erase_data_reader.cpp` on issue 01's `leaf_reader.hpp`, polled from the menu snapshot with no new hook.

- **Snapshot.** `A11yMenuState.erase` (`A11yEraseData` in `game_access.h`), filled by `erase_data_state` in `game_access.c` only while `cur_menu` is Erase Data, all zero elsewhere: the row, the prompt, its choice, the erased marks, the row's description and the prompt's text. `MnDataDelGObjUserData` is declared only in `mndatadel.c`, so `game_access.c` mirrors its first nine bytes (`a11y_erase_data_head`) and declares the global `mnDataDel_804D6C68`, and keeps a copy of the static `mnDataDel_803EF8C8`. The gobj is freed some frames after the screen closes without the global being cleared, which is why it is read only while the screen is up; it is made in the same call that sets `cur_menu` to 24, so the arrival frame already sees it.
- **Reader.** `EraseDataReader`, a member of `MenuSpeech` with its case in `leaf_reader()`. A prompt changing (opening, or Erase All's second) wins over a choice changing, which wins over a row changing; a prompt closing says nothing, and so does the erase mark appearing on the hovered row, so the erase sound alone confirms it. The prompt's keys are said on the first prompt of each opening of the screen. A row outside 0 to 5 says "Unknown row N", logged once.
- **Wording.** As the spec, except that a line with nothing after the choice ends without a full stop, as `add_sentence` joins parts: "Erase? No", "Are you sure? No". The game's descriptions: "Erase data which unlocked hidden characters.", "... hidden stages.", "Erase the 1-P Mode data.", "Erase the VS. Mode melee scores.", "Erase trophy collection data.", "Erase all game data."; Options' description of Erase Data is "Erase melee records, etc.". All decode with no gaps.
- **Tests.** Eleven in `test_menu_speech.cpp`, with an `erase_frame` fixture helper: the opening, moves with both wraps, every row's name, the prompt's choices, closing by No, B and an erase, an erased row hovered and A on it, opening on an erased row, Erase All through both prompts and its marks, No on either of Erase All's prompts, the prompt's keys again after reopening, and gaps logged.
- **Drive script.** `tools/a11y/erase_data.drive`, no card, in `run_level.py`'s menus set with a count of five prompt openings, so A on the erased row opening one fails it. Every step matched on the first run.
- **Verification.** Menus level: build with the "Rollback sections" line, style, unit tests (39 of 40; `launcher_data` failed on both runs at its disc inspection assert, in the launcher's disc code, which this change does not touch), `run_level.py menus` 36 of 36 passed.
- **Docs.** `docs/a11y/native-menus.md`, "Erase Data" under "The Options screens in detail" (state, words, prompts, marks, the highlight finding); `CLAUDE.md`'s main menu tree line; `ACCESSIBILITY.md` an Erase Data line under Options; the `verify` skill the new script.
- **Left open.** The play test (05). `game_access.c` changed, which a Mac build compiles differently; the CI run on a topic branch waits for the maintainer's leave to push.
