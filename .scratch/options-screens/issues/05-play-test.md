# 05 Play test by ear

Status: ready-for-human
Type: task
Blocked by: 04

The maintainer listens with NVDA and judges the wording of `.scratch/options-screens/spec.md`.

## What to try

- **Rumble.** With a controller that rumbles: switch it off and on. Is "P1: On" clear when only one controller is connected? Plug a second controller in and out. With names on the card (`make_save.py`'s edit, issue 04), enter the names list, scroll it and switch a name. Is the bottom line, said at every opening, worth its length?
- **Language.** Hover Japanese and listen to the warning. Switch to Japanese and back: is "Language changed to …" in the right place, and how do the Japanese descriptions come out?
- **Erase Data.** Use a test folder's card, never your own (such as the folder `make_save.py`'s `--play` sets up). Walk the rows, open a prompt and back out, erase a row, hover it again, and take Erase All through both prompts. Is the game's erase sound enough on its own, and is "erased" on a hovered erased row right?

## Done when

The maintainer's findings are recorded below, each either fixed or filed as its own issue, and `CLAUDE.md`'s status line says the Options screens were play-tested.

## Comments

### 2026-11-18, code review (agent)

The review of issues 01 to 04 left these for the maintainer, most pressing first:

- **Which controller entered the names list.** With two or more controllers plugged in, P2 pressing Right says "Names. ANNA: On" with no label, while P2 leaving says "P2: On" (`rumble_reader.cpp`, `entering`). Options: leave it, as the spec's wording does; or put the panel's label first when more than one controller is plugged in ("P2: Names. ANNA: On"). Recommended: the label with two or more, to hear with two controllers in the play test.
- **The names list's keys.** Nothing on Rumble says that Up and Down move through the names and Left goes back; only `ACCESSIBILITY.md` does. Options: leave it; or say the keys on the first entry of a visit, as Erase Data does with its prompt's keys. Recommended: judge by ear first; add them if entering the list leaves you unsure.
- **Other readers' font glyph names in Japanese.** `MenuText` now gives the decoder no glyph names while the game is in Japanese, but the unlock notices (`prize_font_glyphs`, `unlock_speech.cpp`) and results (`results_speech.hpp`) still pass the English table's, and their tables are Japanese too after the switch, so a kanji at 0x4000 or 0x4002 would be said as é or II. Options: one helper in `game_text` that picks the glyph names by the saved language, used by all three readers; or a separate issue for Japanese mode, which the spec leaves out. Recommended: the helper, as its own small change outside this feature.
- **`MenuText`'s name and home.** It lives in `leaf_reader.hpp` though `MenuSpeech` owns it, beside an unrelated `menu_text.hpp`; its `description()` also speaks prompts and bottom lines and logs them as "description". Options: move it into `menu_text.hpp` and rename the method (`spoken`), touching the four readers; or leave it. Recommended: the move, at the next change to these readers.
