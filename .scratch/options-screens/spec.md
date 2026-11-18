# Options screens: speech

Status: agreed 2026-11-18 (design conversation with the maintainer)

The maintainer agreed: one feature with an issue per screen; Rumble follows every controller; Rumble's names list is built now, ahead of Name Entry's speech; Rumble's standing bottom line is said as the game shows it; Language says "Japanese" for 日本語, warns when Japanese is hovered, and says the change once the menu reloads; Erase Data gives no feedback the game does not give, and the game's erase sound is the only confirmation of an erase. The wording of announcements is a first proposal for the play test to judge.

## Goal

A blind player can use the three Options screens that still say "No speech yet.": Rumble, Language and Erase Data. They can switch rumble per controller and per name, switch the game's language knowing what that does to the game's own text, and erase saved data knowing which data and which choice of the prompt they are on.

How the game's menus run is in `docs/a11y/native-menus.md`; the menu reader's rules are ADR-0003 and the main menu tree spec (`.scratch/main-menu-tree/spec.md`, "Leaf screens with a reader"), which this one follows unless it says otherwise. Vocabulary is `CONTEXT.md`: leaf screen, row, value, description, player label.

## Scope

In:

- **Rumble** (`mnvibration.c`, `cur_menu` 19): the four controller panels and the names list.
- **Language** (`mnlanguage.c`, `cur_menu` 23): the two choices and the change.
- **Erase Data** (`mndatadel.c`, `cur_menu` 24): the six rows and both prompts.
- **A save with names** for testing Rumble's names list: a `make_save.py` edit.

Out:

- **Making Japanese mode good.** After a switch to Japanese the game's own text (descriptions, prompts, unlock notices) is Japanese and the fork's words stay English. The fork says what it can decode and logs the rest; nothing more.
- **Name Entry's speech**, a screen of its own.
- **Erase Data's YES/NO highlight.** In the base port the drawn highlight does not match the choice: both boxes lit while No is chosen, NO lit while Yes is chosen (runs on 2026-11-18). A fix helps every player, so it is not this feature's; the fork speaks the game's state, never the drawing.

## What the game gives us

From reading the three files and runs with screenshots on 2026-11-18. Address-style names are the decomp's.

### All three

- `pc_a11y_menu_frame` already polls these screens: each says its name and "No speech yet." today.
- None keeps its cursor in `mn_804A04F0.hovered_selection`; each keeps its own state.
- Each ignores input for a few frames after opening (`mn_804D6BC8.cooldown`); Rumble also until its intro timer passes.

### Rumble

- State is `MnVibrationData`, the user data of the global `mnVibration_804D6C28`: `x0[1]` the names list's cursor row (0 to 7, the visible rows), `x0[2 + port]` each controller's place (0 its panel, 1 the names list), `x6[port]` whether the controller is connected (updated live as controllers come and go), `scroll_offset` the first name shown.
- Each controller acts alone: A on its panel switches its rumble (`GetRumbleSettingOfPort`, `gmMainLib_SetRumbleEnabled`), and turning it on buzzes the controller. Right takes it into the names list and Left back, only when there are names (`GetNameCount()`); with none, Right does nothing.
- The names list has one cursor shared by every controller in it. Up and Down move it, scrolling past eight names, without wrapping; A switches the hovered name's rumble (`GetPersistentNameData(slot)->rumble_enabled`). The name is `namedata`, Shift-JIS, decoded as the player label decodes a name tag (`game_text.hpp`).
- The bottom line is game text 0xA7, "Player name entry must precede Rumble setting.", drawn always, with or without names. It is drawn directly, not through `Menu_InitCenterText`.
- Each panel is labelled "P1" to "P4"; a disconnected controller's panel shows a red cross and no value.
- B from any controller goes back to Options.

### Language

- State is the screen's `Menu` user data, `cursor` 0 Japanese, 1 English, and `unk1` the language in use; the gobj (`mn_gobj`) is static to the file, so the fork cannot reach it without a hook.
- Left and Right toggle the cursor. A on the language in use does nothing, no sound. A on the other saves it (`lbLang_SetSavedLanguage`) and reloads the menu scene (`mn_80229860(GM_MENU)`) on Options with Language hovered, in the new language.
- The bottom line is game text 0xBF, "Choose a language and press the A Button to confirm.", through `Menu_InitCenterText`, so `pc_a11y_menu_center_text` reports it.
- The title is drawn in Japanese (`menu_names.cpp` already says "Language"); the choices are pictures, 日本語 and "English".

### Erase Data

- State is the user data of the global `mnDataDel_804D6C68` (`MnDataDelGObjUserData`): `x0` the row, 0 to 5; `x1` the prompt, 0 none, 1 "Erase?", 2 "Are you sure?"; `x2` the prompt's choice, 0 No, 1 Yes; `x3[row]` set once the row has been erased this visit. `WarnCmnData` is the same memory under other names.
- Rows, top to bottom: Hidden Characters, Hidden Stages, 1-P High Score, VS. Mode Scores, Trophy Collection, Erase All. Up and Down wrap. Each row's description is game text by number from `mnDataDel_803EF8C8` (static: 0xC0 to 0xC4, and 0xC6 for Erase All), drawn without `Menu_InitCenterText`.
- A opens "Erase?" (game text 318) on No. Left or Right switch; A on Yes erases with the erase sound, A on No or B closes. On Erase All, Yes opens "Are you sure?" (319), again on No, and Yes there erases everything.
- An erased row is marked `x3` until the screen closes; reopening clears every mark. An erased row looks like any other until hovered, where its bar is dim instead of bright; A on it does nothing, silently. Erase All marks every row.
- The erase is real: on PC it reaches the save on the memory card file.

## Decisions

### How the fork attaches

- **Rumble and Erase Data are polled** from the existing `pc_a11y_menu_frame` through new snapshots in `game_access.c`, read from the two global gobjs. No new hook.
- **Language gets one hook**, declared in `a11y_hooks.h`, one line in `mnlanguage.c` that hands the fork the cursor each frame (before the cooldown check, so it runs every frame the screen is up). The next menu frame reads it.
- **Read only.** The readers write nothing. Speech stays gated by `pc_net_resim()`; these screens are offline only, but the gate costs nothing.
- **The existing reader model is extended or bypassed as the implementer finds best.** `Reader` keys rows by the hovered entry or the centre text; these screens need their own state. Small per-screen readers beside `menu_speech.cpp`, as the memory card prompts have, are fine.

### Speech: Rumble

- **Every controller is followed.** Each line about a panel names it as the screen does, "P1" to "P4".
- **Opening**: title, each connected controller's panel, the hint, the bottom line. "Rumble. P1: On. A to turn on or off, right for the names list. Player name entry must precede Rumble setting." Disconnected controllers are left out. "right for the names list" only when there are names.
- **Switching a controller's rumble**: "P1: Off".
- **A controller entering the names list**: "Names. Ann: On." **Leaving it**: "P1: On".
- **Moving in the names list**: "Bob: Off". Nothing at either end, as the game gives nothing.
- **Switching a name's rumble**: "On".
- **A controller coming or going**: "P2: Off" as it connects, "P2 disconnected" as it goes.
- **Right with no names**: nothing, as the game gives nothing.
- Names spoken as decoded; a name with no words in it says "Name" and its slot number, logged.

### Speech: Language

- **Opening**: "Language. English. Left and right to choose. Choose a language and press the A Button to confirm." The last sentence is the game's bottom line.
- **Moving**: "Japanese. The game's own text will be in Japanese." and "English".
- **A on the language in use**: nothing.
- **The change**: once the menu has reloaded, "Language changed to Japanese." before the Options opening the menu tree says ("Options. Language. ..."), whose description is now the game's Japanese text, said as the decoder can.

### Speech: Erase Data

- **Opening**: "Erase Data. Hidden Characters. Up and down to move, A to erase. Erase data which unlocked hidden characters."
- **Moving**: row and description. "Hidden Stages. Erase data which unlocked hidden stages."
- **An erased row**: the screen marks it only by its dim bar when hovered, so the row says "erased" there: "Hidden Characters: erased. Erase data which unlocked hidden characters." A on it says nothing.
- **The prompt opening**: its text and the choice, with the hint the first time a prompt opens in a visit. "Erase? No. Left and right to choose, A to confirm." The second: "Are you sure? No."
- **Moving in a prompt**: "Yes", "No".
- **Closing a prompt**, by erasing, No or B: nothing; the game's sounds tell it.
- Row names as drawn: Hidden Characters, Hidden Stages, 1-P High Score, VS. Mode Scores, Trophy Collection, Erase All.

### Testing

- **Unit tests** for each reader: openings, moves, values, the names list with scrolling, connecting and disconnecting, the Japanese warning, the change, erased rows, both prompts, the hint once per visit, gaps.
- **Drive scripts** under `tools/a11y/`: one per screen or one for all three. They run with no card (the default) or `--card` with a card of their own, never the maintainer's: Erase Data's scripts may erase and Language's may switch to Japanese and back.
- **A save with names**: `make_save.py` gains an edit that writes name tags (with their rumble settings) into the fresh save, for a `--card` run of the names list. If writing name tags proves harder than typing them, a drive script may make them through Name Entry's keyboard instead; say which.

### Docs

- `docs/a11y/native-menus.md`: a section on the three screens' state, Erase Data's prompts and erased marks, the highlight finding, and Language's reload.
- `CLAUDE.md`: an "Accessibility status" line; the main menu tree line no longer lists these among screens with "No speech yet."
- `ACCESSIBILITY.md`: the three screens.
- The `verify` skill: new scripts, changed expected lines, the save edit.

## Open questions

For the implementer to find out:

1. How the reload after a language change looks to the hooks: whether `pc_a11y_scene_entered` and the menu reader see an arrival, and where the fork notices the change (the saved language before and after is one way).
2. What the decoder makes of Japanese descriptions; enough to log gaps once, not to fix them.
3. Whether the names list's row maps straight to a name tag slot (`scroll_offset + row`), as `mnVibration_GetNameSlot` suggests, or names are compacted.
4. The name tag layout in the save, for the `make_save.py` edit (`native-menus.md`, "The save file").

## Issues

1. `issues/01-language.md`: Language, with its hook.
2. `issues/02-erase-data.md`: Erase Data. Blocked by 01.
3. `issues/03-rumble-controllers.md`: Rumble's controller panels. Blocked by 02.
4. `issues/04-rumble-names.md`: Rumble's names list and the save with names. Blocked by 03.
5. `issues/05-play-test.md`: the maintainer, by ear. Blocked by 04.

Docs are part of the issue they belong to. The issues are chained because they share `menu_speech.cpp`, `menu_names.cpp` and `game_access.c`.
