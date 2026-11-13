# 02 The sounds: the preparation script and loading from resources

Status: resolved (2026-11-13)
Type: task
Blocked by: none

Spec: `.scratch/items/spec.md` ("The sounds"). Read it first, then `docs/a11y/cues.md` ("Sounds", "The engine's rules" for the lead-in) and `cue_sounds.hpp`.

## What to build

- **`tools/a11y/prepare_sounds.py`**, run with `uv run`, its dependencies (numpy, soundfile) declared in the script's own metadata block so uv installs them. It takes an original file by path (WAV or FLAC, any rate, any channel count) and an output name, and writes `resources/a11y/<name>.wav`, mono, 48 kHz:
  - silence trimmed at both ends below a threshold;
  - the end faded over a few milliseconds, so the trim never clicks;
  - normalized by peak to just under full scale;
  - no lead-in: the engine adds it.
  The thresholds and levels are constants in the script. `--help` says what it does.
- **Its report**: the result's length, and for a pulsed cue the fastest interval at which a pulse plays whole at a given lowest pitch (`--lowest-pitch`, in octaves down), so the maintainer knows which paces the engine will cut (issue 01).
- **Loading.** `cue_sounds` reads a sound from a file through `ma_decoder`, asked for mono 48 kHz floats, then shapes it as the other sounds are (the lead-in). The path is `SDL_GetBasePath()` plus `resources/a11y/`, as the base port finds its own resources. A missing or unreadable file gives an empty sound: the cue stays silent, with one log line naming the file and why; nothing else fails.
- **Placeholders.** Until the maintainer's files arrive, `resources/a11y/item_appearance.wav` and `resources/a11y/item_position.wav` are placeholders run through the script: short, plainly different sounds, the second short enough to pulse. The maintainer's files replace them by the same script run; the issue that brings them notes the command.
- **The package.** Check that `cmake --build build` copies `resources/a11y/` beside `melee.exe` and that `tools/package_windows.sh` takes it into the Windows package, with no base port file touched.

## Done when

- The script runs with `uv run` on a test file of each kind (stereo, 44.1 kHz, leading and trailing silence) and its report is right; it refuses a missing input with a clear message.
- A unit test loads a small WAV through the new reader into a `Sound` of the right length at 48 kHz, and a missing file gives an empty sound and a log line.
- `build/resources/a11y/` holds both files after a build.
- The `verify` skill names the script and how to check it; `cmake --build build`, the unit tests and `python tools/check_style.py` pass.

## Comments

### 2026-11-13, implementation (agent)

Implemented; the script prepares a cue's sound from an original, the engine loads it from `resources/a11y/`, and two placeholder files stand in for the maintainer's.

- **The script.** `tools/a11y/prepare_sounds.py ORIGINAL NAME [--lowest-pitch OCTAVES] [--out-dir DIR]`, run with `uv run`, numpy and soundfile in its metadata block. Channels averaged, resampled to 48 kHz by its own Kaiser-windowed sinc (no scipy, so the dependencies stay the two the spec names), trimmed at both ends below -50 dB under the file's own peak (relative, so a quiet recording trims as a loud one does), the end faded over 5 ms with a raised cosine, the peak normalized to 0.95 (the generated cues' amplitude), written 16-bit. A missing original, one soundfile cannot read, one of silence alone and a name with a path in it are refused with argparse's exit 2 and a message. `--out-dir` is for the tests.
- **Its report**: the length in seconds and frames, the length with the engine's 12 ms lead-in, the silence trimmed at each end, and "A pulse plays whole at intervals of X s or longer at its lowest pitch, N octaves down", X being the length with the lead-in divided by 2^-N, as issue 01's engine reckons it. The lead-in is a constant in the script beside `kLeadInSeconds`, now in `cue_sounds.hpp`, each naming the other.
- **Its test**, `tools/a11y/test_prepare_sounds.py`, also run with `uv run` and left out of ctest, which has no uv: a stereo 44.1 kHz WAV with 0.2 s of silence before and 0.3 s after a 1 kHz tone, louder on the right (mono, 48 kHz, 0.1 s, peak 0.95, last sample silent, still 1 kHz, the report's lengths, trims and pace at an octave down); a mono FLAC at 22.05 kHz; the four refusals. The frequency is measured away from the ends: the resampler's ringing before an abrupt digital onset is kept by the trim (it is above -50 dB) and adds zero crossings. A real recording rarely starts that abruptly.
- **Loading.** `load_sound(path)` in `cue_sounds.cpp`: the file read into memory through a `std::filesystem::path` made from the UTF-8 text, then `ma_decoder_init_memory` asked for mono f32 at 48 kHz, read to its end, then `lead_in(kLeadInSeconds)`. Reading it ourselves rather than `ma_decoder_init_file` matters on Windows: miniaudio's `fopen` takes the system code page, so an install folder with letters outside ASCII would not open. Failures give an empty sound and one line, `[a11y] cue sound <path> not loaded: no such file`, `cannot open it`, `cannot decode (<ma_result_description>)` or `no sound in it`. The folder is `cue_sound_folder()`, declared in `cue_output.hpp` and defined in `cue_output.cpp`, the engine's SDL side, so `cue_sounds.cpp` and its tests need no SDL: `SDL_GetBasePath()` plus `resources/a11y/`. On a Mac package that is `Contents/Resources/resources/a11y/`, where `package_macos.sh` puts `resources/`; not checked on a Mac.
- **Engine tests** (`test_cue_engine.cpp`): a 24 kHz stereo 16-bit WAV written by hand, named `ääni.wav`, loads to 0.1 s at 48 kHz after the lead-in's silence, its level kept; a missing file and a text file each give an empty sound and exactly one line. A mutation (no lead-in) fails the first.
- **Placeholders.** Originals made by a throwaway numpy snippet, each with 0.1 s of silence around it at 44.1 kHz, then run through the script: `item_appearance.wav`, a rising two-note chime (C6 then G6, each with its second and third harmonics, a bell's decay), 0.320 s; `item_position.wav`, a falling blip (1100 to 650 Hz with a little of its octave, 2 ms attack, fast decay), 0.045 s, 0.057 s with the lead-in, so it plays whole at 0.114 s or slower an octave down. Neither has been heard.
- **The package.** `cmake --build build` copies `resources/` beside `melee.exe` (`launcher_resources` in the root `CMakeLists.txt`), and `build/resources/a11y/` holds both files; `tools/package_windows.sh` copies all of `resources/` into the package. No base port file touched.
- **Open question 6, the credits.** Answered yes: `resources/a11y/CREDITS.txt` names each file's source and licence and ships with them, as the base port's `FONT-LICENSE.txt` ships beside its fonts; a licence that asks for attribution is met where the files go. Today it names the two placeholders, made by the fork, GPL-3.0-or-later. The issue bringing the maintainer's files updates it.
- **Verification**, quick level: build with the "Rollback sections" line; style; unit tests 37 of 38, `launcher_data` the known failure; `uv run tools/a11y/test_prepare_sounds.py` passed; a bounded `title` run as expected. With a temporary probe in `CueSystem::open` (not committed), a `title` run loaded both files from `buildesources/a11y/` (15934 and 2734 frames, each the file plus 576 frames of lead-in) and logged `cue sound ...nothere.wav not loaded: no such file`. No match script was run: nothing in the game loads a file yet, and the generated sounds' code is unchanged.
- **Docs.** `docs/a11y/cues.md`, "Sounds" with a new "Sounds from files"; the `verify` skill's `cue_engine` suite and a bullet on the sound preparation test and the missing file's lines; CLAUDE.md's cue engine status line.
- **For issue 04 and 05.** In `CueSystem::open`, `load_sound(cue_sound_folder() + "item_appearance.wav")` (and `item_position.wav`), handed to `MatchCues`, or the folder handed to it so its tests can pass their own; the lead-in is already there, so the spec's sound goes straight into the `VoiceSpec`. A missing file gives an empty sound, which `add_voice` refuses (`cue <voice> refused: no sound`, -1; `OneShotPool::add` then returns false with no voices, and `trigger` -1): the drivers must take -1 quietly. The maintainer's files replace the placeholders by `uv run tools/a11y/prepare_sounds.py <original> item_appearance` and `uv run tools/a11y/prepare_sounds.py <original> item_position --lowest-pitch <octaves>`, the octaves the item's position cue's pitch range reaches down, with `CREDITS.txt` updated.
- **Left open.** The thresholds (-50 dB, 5 ms, 0.95) are first proposals; the placeholders are plain stand-ins for the play test to judge, if the maintainer's files are late.
