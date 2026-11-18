# 01 Language

Status: ready-for-agent
Type: task

Spec: `.scratch/options-screens/spec.md`. Read it first, then `src/pc/a11y/menu_speech.cpp` and `menu_names.cpp` (the leaf screen readers this extends).

## Machinery

- **Hook**: one line in `mnLanguage_8024BFE0` (`mnlanguage.c`), before the cooldown check, handing the fork the screen's cursor (0 Japanese, 1 English) each frame; declared in `a11y_hooks.h`. `mnlanguage.c` gets the header through `inlines.h`. The next `pc_a11y_menu_frame` reads it into the menu snapshot.
- **The bottom line** is the centre text (0xBF), which `pc_a11y_menu_center_text` already reports.
- **The change**: find how the menu's reload looks to the hooks (spec, open question 1) and say "Language changed to …" once, before the Options opening.
- **Japanese text**: see what the decoder makes of the Options description after the switch (spec, open question 2); gaps logged once, nothing more.

## Speech in this slice

Wording is in the spec, "Speech: Language". The screen loses its "No speech yet." notice.

## Done when

- `cmake --build build` passes, and the unit tests pass, with tests for the opening, both moves, the Japanese warning, A on the language in use, and the change.
- A drive script opens Language, moves both ways, switches to Japanese, waits on the change line, switches back to English and waits again. No card, or `--card` with a card of the run's own.
- `python tools/check_style.py` passes.
- The `verify` skill updated; the primer records the screen's state and the reload.
