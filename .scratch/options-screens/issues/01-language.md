# 01 Language

Status: resolved (2026-11-18)
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

## Comments

### 2026-11-18, implementation (agent)

Implemented in `src/pc/a11y/language_reader.cpp` on the new `leaf_reader.hpp`, with the `pc_a11y_language_frame` hook in `mnlanguage.c`.

- **The reader shape, for 02 and 03.** `leaf_reader.hpp` holds `LeafReader`, an interface of two calls, `opening(state)` (what follows the screen's name; resets the reader's memory of the screen) and `frame(state)` (what changed since the last frame, or nothing), and `MenuText`, the menu scene's game text as spoken lines with the one `LogOnce` the menu reader and the leaf readers share (`description(bytes, state, where)`). `MenuSpeech` owns one reader per screen as a member (`m_language`), finds it by `MenuKind` in `leaf_reader()`, says the screen's name from `menu_names.cpp` and hands over the rest; arrival, `forget()` and leaving stay `MenuSpeech`'s. Erase Data and Rumble add a class each beside `language_reader.cpp`, a member, a case in `leaf_reader()`, and the line in `a11y.cmake`'s two lists (the game's sources and `menu_speech_test`'s). A screen's words live in its reader's file; `menu_names.cpp` keeps only the screen's name.
- **Snapshots.** The screen's state goes into `A11yMenuState` (`game_access.h`), filled in `a11y_game_menu_state`; Language's two fields are `language_japanese` (the saved language) and `language_cursor_japanese`. A bool for "Japanese" keeps a zeroed test snapshot English, so the existing tests needed no change. Erase Data's and Rumble's state read from their global gobjs fit as a sub-struct each, filled only while `cur_menu` is theirs.
- **Tests.** In `test_menu_speech.cpp`, through `MenuSpeech` as the game drives it: a `Fixture` helper per screen (`language_frame`) building the snapshot, and the existing `frame()` with a `japanese` argument. Eight tests: the opening, both moves with the warning, A on the language in use, the change before Options' opening both ways, Japanese mode's opening without the warning, no change line on the first frame, the Japanese table without the English font glyphs, and a missing centre text logged.
- **Open question 1, the reload.** A on the other language saves it and ends the menu scene with `cur_menu` set to Options and Language hovered; the scene starts again at once (`boot scene: game mode 1`, `state 0 scene 1` in a drive transcript), `pc_a11y_scene_entered` runs and says nothing for the menu scene, and the next menu frame is an arrival on Options. Nothing in the menu's state marks the reload, so `LanguageReader::change` compares the saved language (`lbLang_GetSavedLanguage`) with the last menu frame's, kept across `forget()`, and the menu reader says "Language changed to Japanese." in the same announcement before Options' opening, about 0.2 s after the press. The first frame ever seen says no change, so a save already in Japanese is quiet at startup.
- **Open question 2, Japanese text.** After the switch the menu scene loads `SdMenu.dat`. Its kana and the atlas's 23 kanji decode; its other kanji are its own font glyphs from 0x4000, which the fork cannot name. Options' description of Language, 現在日本語設定です, comes out as "本です" (logged `[a11y] menu 4 entry 4: description "{glyph 4088}...", unknown glyphs left out`; drive.py's console output shows the kana and kanji as `?`, the transcript holds them as UTF-8), and Language's bottom line as an A and a run of kana. The English table's glyph names (é at 0x4000, II at 0x4002) would misname the Japanese table's, so `MenuText` gives the decoder no font glyphs while the game is in Japanese; every font glyph is then left out and logged once. Other readers (character select's, the unlock notices') still hand over their English tables' glyphs and were not looked at in Japanese.
- **Hook timing.** The screen's input function runs after `pc_a11y_menu_frame`, so each menu frame takes the report of the frame before and clears it (`take_language_cursor` in `hooks.cpp`, taken even while speech is gated, like Random Stage's refusal); with no report, as on the arrival frame, the snapshot uses the saved language, where the screen opens its cursor. A report never outlives its visit.
- **Wording choice.** The warning is said on Japanese only while the game is in English: in Japanese mode Japanese is the language in use, A on it does nothing, and "will be in Japanese" would be wrong. The spec did not say; the play test (05) can judge.
- **Docs.** `docs/a11y/native-menus.md` gains "The Options screens in detail" with a "Language" section (state, the hook, the reload, Japanese text) for 02 and 03 to add theirs beside; `CLAUDE.md`'s main menu tree line names Language with its hook, waiting for 05; `ACCESSIBILITY.md` an Options line with the Japanese caveat; the `verify` skill the new script.
- **Drive script.** `tools/a11y/language.drive`, no card: the opening, A on English, both moves, the switch to Japanese and its change line, Language in Japanese, the switch back and its line, B back to Options. In `run_level.py`'s menus set, with a count of one line opening with Options' Language entry (only the return by B), which an A on the language in use that reloaded the menu would break.
- **Verification.** Menus level: build with the "Rollback sections" line, style, unit tests (40 of 40, `launcher_data` passing this time), and `run_level.py menus`, 35 of 35 passed on the first run.
- **Left open.** The play test (05). Japanese mode's other readers, out of scope.
