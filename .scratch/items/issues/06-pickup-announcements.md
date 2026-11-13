# 06 Pickup announcements

Status: resolved (2026-11-13)
Type: task
Blocked by: 03

Spec: `.scratch/items/spec.md` ("Pickup announcements", open question 1 as answered in issue 03). Read it first, then `.scratch/match-announcements/spec.md` ("Who is announced, in which voice", "Player labels") and `match_announcer.cpp`, whose labels and match start and end this reuses.

## What to build

- **Takes.** From the snapshot's per-slot hands and touch marks (issue 03), each tick from GO to GAME! or TIME!, every item a player took: picked up, caught after a throw, a Starman or Mushroom touched, and the same item picked up again after a drop or throw. A healing item used up on the tick it was picked up is still a take.
- **Who and which voice.** Yours in your voice; everyone else's in the opponents' voice, teammates' only with the Teammates switch on; with no "you", everyone's in the opponents' voice. Only players the match announcements would announce (a HUD panel).
- **The words.** The item's name, after the player label where `MatchAnnouncer` uses one, its labels fixed at GO: "Ray Gun", "P3, Ray Gun", "teammate P2, Food". Share the label with the match announcements rather than computing it again.
- **The subject.** A range of subjects of its own, one per player, so a pickup replaces only an older pickup about the same player and never a damage or stock line, nor they it (`speech.hpp`, the subjects).
- **The switch.** **Item pickups**, in Match announcement settings after Item appearances, on by default, under the Match announcements switch.

## Done when

- Unit tests with hand-written snapshots: a pickup by you and by a CPU, each in its voice; labels with one opponent and with three others; a teammate with the switch on and off; a catch; a touch item; a re-pickup after a throw; a healing item used up at once; nothing at or after GAME!; a pickup line not replacing a waiting damage line about the same player, nor the reverse; the switch off.
- A drive run in Training: an item spawned and picked up is said in your voice.
- The menu scripts read the new row, their step counts updated.
- `cmake --build build` passes, as do the unit tests and `python tools/check_style.py`; the `verify` skill's quick and menus levels.

## Comments

### 2026-11-13, implementation (agent)

Implemented; every take of an item by a player with a HUD panel is said in the player's voice, the item's name after the match announcements' label, from GO to GAME! or TIME!, under the new Item pickups row.

- **Where things live.** `item_takes.hpp/.cpp`: `ItemTakes`, one player's take rule from issue 03 (hand takes by serial set, then the tally's rise beyond them per kind, a fall counting nothing), the first call after `forget()` taking the baseline. `MatchAnnouncer` owns one per slot and says the takes in `pickups()`, after the damage and stocks of the tick, so the labels, voices, Teammates rule and GO-to-GAME! stages are the ones the damage and stock lines use, not computed again. No new hook: it runs in `pc_a11y_match_tick` with the HUD snapshot the announcer already reads.
- **Following.** Every present slot is followed each running tick, whatever the switches, so turning a switch on says only later takes. The baseline is set at GO (what each holds and the tally so far), again at a rollback that takes GAME! back (the announcer's own restart), and for a slot that was absent (its hands unfilled, serial 0 from the memset) on its first present tick. `scene_entered` forgets them all.
- **The words.** `pickup_line`: the label, then the item's name joined by `add_part`: "Ray Gun", "P3, Ray Gun", "teammate CPU 2, Food", "CPU 1, Food" with no "you". Two takes by one player on one tick (never seen) are one line, "Food, Ray Gun". A kind with no name says "Unknown item", logged once.
- **The subject.** `speech.hpp` gains `kPickupSubjects` (100) and `pickup_subject(slot)`, a range of its own; the voices' queueing is unchanged.
- **The row.** Item pickups (`match_announcements.item_pickups`), a switch on by default, after Item appearances: "Says each item a player takes, by name: yours in your voice, everyone else's in the opponents' voice." Two descriptions changed with it: Match announcements ("... your damage, stocks and pickups in your voice, ...") and Teammates ("Says your teammates' damage, stocks and pickups in a team match, ...").
- **Tests**, in `match_announcer` (now linking `item_takes.cpp` and `item_names.cpp`), written first and seen failing against a stub: yours and a CPU's each in its voice; labels with one opponent, three others (CPU, port, name tag) and no "you"; a teammate with Teammates on and off; a catch by another player and by the thrower; a touch item; a re-pickup; a healing item used up at once, and one into the second hand said once; the Bunny Hood moving hands, what is held at GO and a slot coming back, no takes; a rollback's fall; nothing before GO, on GAME!'s tick or after; the subject (a pickup waiting beside a damage line about the same player, the damage line replaced by a newer one and by a stock line, the pickup by a newer pickup, neither by the other); the switches and a player without a panel; the row in the rows test. The fixture's players now start with empty hands (serial -1). Two mutations (hand takes not subtracted from the tally's rise; the last tick's hands ignored) each failed. `launcher_speech`, `port_menu_speech` and `test_rmlui_page.hpp` updated for the row and the description.
- **The drive run.** `match_items.drive` presses A until `voice yours[^:]*: "Food"` (the take is logged before the item's position cue's stop on the same tick, since the announcer runs before the cues), then waits for the stop; `run_level.py` counts the pickup line once. Seen: `[a11y] voice yours (off): "Food"`.
- **A CPU's pickup in VS**, seen in a probe through character select (Fox and two CPUs, default rules, random stage, `--muted`): `[a11y] voice opponents: "CPU 2, Capsule"`, labelled, a Capsule the CPU took; no `refused` line. The existing VS scripts met item switch spawns but no take in their runs.
- **Verification**, quick and menus levels: build with the "Rollback sections" line; style; unit tests 39 of 40, `launcher_data` the known failure (it passed on one run); `run_level.py menus` with the seven match cue scripts, `match_items`, the five match announcement scripts and `results_cpu4`, 47 of 48 at first, `match_items` failing on its new wait's order, then passing alone after the fix; `match_items`, `launcher_a11y`, `port_menu_a11y`, `port_menu_voices` and `match_damage` again on the final build, 5 of 5. Not run online: the takes only read the HUD snapshot, and issue 07 has the online run.
- **Scripts.** `launcher_a11y.drive`, `port_menu_a11y.drive` and `port_menu_voices.drive` read Item pickups after Item appearances, and the first two the changed descriptions; Match announcement settings is now 12 rows.
- **Docs.** `docs/a11y/speech.md`, the pickups under "What speaks through them" and their subject under "Queueing"; `docs/a11y/gameplay-state.md`, "How the fork sees a take", the reader and its baselines; `ACCESSIBILITY.md`, the pickups in "Match announcements"; `CLAUDE.md`'s Accessibility settings line names the row; the `verify` skill: the `match_announcer` suite's pickups, `launcher_a11y.drive`'s row, `match_items.drive`'s pickup line and count, and the VS scripts' pickup lines.

**For issue 07.** Expected lines: your take `[a11y] voice yours: "<item>"` (`(off)` with speech off), another's `[a11y] voice opponents: "<label>, <item>"`, or the name alone with one opponent; each followed by its `plays` line when the voices render (`--muted`). In VS a CPU's take comes once the CPU reaches an item, so a script waiting for one needs a long timeout or a script that brings the item to it; a Capsule taken by a CPU was seen. The online run should show the same lines on each copy for its own "you".

**Left open.** A Metal Box broken by an attack is said as a take of the attacker's (the tally counts it), as issue 03 expected; for the maintainer to judge in the play test. The words, and a tick's takes joined in one line, are first proposals.
