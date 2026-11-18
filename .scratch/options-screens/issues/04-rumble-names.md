# 04 Rumble: the names list

Status: ready-for-agent
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
