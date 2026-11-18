# 04 Rumble: the names list

Status: resolved (2026-11-18)
Type: task
Blocked by: 03

Spec: `.scratch/options-screens/spec.md`. Read it first, and the comments of issue 03.

## Machinery

- **Snapshot**: the names list's cursor row (`x0[1]`), `scroll_offset`, `GetNameCount()`, and for the hovered name its `namedata` and `rumble_enabled`. Find whether the row maps straight to a name tag slot (spec, open question 3).
- **Names** are decoded as the player label decodes a name tag (`game_text.hpp`).
- **A save with names**: a `make_save.py` edit that writes name tags, more than eight so the list scrolls, with mixed rumble settings, into the fresh save (spec, open question 4). If that proves harder than typing them, a drive script makes them through Name Entry's keyboard instead; say which. `test_make_save.py` covers the edit.

## Speech in this slice

Wording is in the spec, "Speech: Rumble": a controller entering and leaving the list, moving, scrolling, switching a name's rumble, and the hint's "right for the names list" once names exist.

## Done when

- `cmake --build build` passes, and the unit tests pass, with tests for entering and leaving the list, moving and scrolling both ways, both ends saying nothing, switching a name, a name with no words in it, and the hint with names.
- A drive script on the save with names (`--card`) enters the list, moves down past the eighth name and back, switches a name's rumble and back, and leaves the list.
- `python tools/check_style.py` passes.
- The `verify` skill updated with the save edit and the script; `CLAUDE.md`'s status line for the Options screens, `ACCESSIBILITY.md`, and the main menu tree's line no longer listing these screens among those with "No speech yet."

## Comments

### 2026-11-18, implementation (agent)

Implemented in `src/pc/a11y/rumble_reader.cpp`, on issue 03's snapshot, with no new hook.

- **Snapshot.** `A11yRumble` (`game_access.h`) gains `name_count` (`GetNameCount()`), `names_row` (`x0[1]`), `names_scroll` (`scroll_offset`), `name_slot` (the hovered name tag slot, `scroll_offset + row`, or -1), `name` (its eight bytes of Shift-JIS through `copy_name_tag`, with a terminator) and `name_rumble` (its `rumble_enabled`), filled by `rumble_state` in `game_access.c` beside the panels; `game_access.c` now includes `mnname.h`.
- **Open question 3, the slot.** The row maps straight to a slot, `scroll_offset + row`, with no compacting at read time, because the save has no gaps: Name Entry writes a new name into the slot numbered by the count of names (`mnName_MainInput`, `mnName_CountValid`) and `DeleteName` moves the later names down. A run on a save with ten names confirmed it: every name the speech gave matched the drawn list, scrolled to its end (screenshot during the run). Down's scroll has a wrap to 0 in the code that its own guard keeps from running, so nothing wraps either way.
- **Reader.** Per controller in port order, a connection change wins, then a change of place, then a rumble change. Entering says "Names" and the hovered name with its setting, once a frame however many controllers enter; leaving says the panel. After the controllers, the names list's one cursor: a new slot (a move or a scroll) says the name, the same slot with a new setting says "On" or "Off"; a frame that already said the name on entering says no more. At either end the game changes nothing, and nothing is said. The opening's keys gain ", right for the names list" when `name_count` is above 0. A controller already in the list at an opening (only after `forget()`) adds "Names" and the hovered name after the panels; the spec did not say.
- **Names.** Decoded with `decode_shift_jis` and spoken with `spoken_text`, as the player label does, but without its "P" and port fallback: a name with no words says "Name" and its slot counted from 1 ("Name 3: On"), the list's place, logged once as `[a11y] menu 19 name 3: no words, said as its slot`, and a decoding gap as `[a11y] menu 19 name 2: "{sjis 889F}", unknown glyphs left out`. The name keyboard writes full-width capitals, which the decoder folds to ASCII: "ANNA", which a screen reader may spell or read as a word by its own rules.
- **Open question 4, the save.** Writing the name tags proved simple, so `make_save.py names` edits the fresh save; no Name Entry drive script. The name tags are block ids 2 to 8, a bank of 19 `NameTagData` (0x1A4 bytes) each; the name at 0x198, full-width Shift-JIS, `rumble_enabled` at 0x1A1. The edit writes ANNA, BEN, CARL, DORA, ED, FAY, GUS, HAL, IVY and JO into slots 0 to 9, rumble on and off by turns from ANNA's on, empties every later slot and leaves the other fields as the fresh save has them (cleared, as `CreateNameAtIndex` leaves a new name). `make_save.py` gained `edit_block` (any block id; `edit_save` is block 1 on it), `saved_names`, and `show` lists the names; `names` takes `--out`, `--fresh` and `--play`, as `challenger` does. Four tests in `test_make_save.py`: the encoding and its refusals, the edit (names, rumble, an old name's bytes cleared, a later slot emptied, other fields and blocks untouched, checksums), and refusals (no name tag blocks, more names than slots).
- **Tests.** Nine more in `test_menu_speech.cpp`, the fixture's `Panel` with a place and `rumble_frame` with a `NamesList`: the opening's hint with names, entering and leaving, moving and scrolling down and up with both ends silent, switching a name, a name with no words (and one empty), full-width Shift-JIS, a second controller joining the list with a third switching its own rumble in the same frame, and an opening after `forget()` with a controller in the list.
- **Drive script.** `tools/a11y/rumble_names.drive`, `--card` on the names save, in `run_level.py`'s menus set through `SAVE_EDIT` (the save made first, as the challenger's), with counts of eighteen name lines and two switches. Every step matched on the first run.
- **Verification.** Menus level: build with the "Rollback sections" line, style (one clang-format fix in the tests), unit tests (39 of 40; `launcher_data` failed at its known spot), `run_level.py menus` 38 of 38 passed on the first run. `make_save.py names --play --dry-run` prints the launch; it also writes the names save into `build/play`'s card, as any `--play` does.
- **Docs.** `docs/a11y/native-menus.md`: the name tag blocks under "The save file", the names list under Rumble; `CLAUDE.md`: an Options screens status line, the main menu tree's line pointing to it, the names save among the drivers' notes; `ACCESSIBILITY.md` Rumble's names list; the `verify` skill the new script and the save.
- **For the play test (05).** `python tools/a11y/make_save.py names --play` writes the save with ten names into `build/play` and starts the game there with a copy of your accessibility settings; Options, Rumble, then Right. Name Entry itself is still unread, so names made by hand need sighted help or this save.
- **Left open.** The play test (05). `game_access.c` changed, which a Mac build compiles differently; the CI run on a topic branch waits for the maintainer's leave to push.
