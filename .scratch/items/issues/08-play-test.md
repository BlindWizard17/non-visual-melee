# 08 Play test by ear

Status: ready-for-human
Type: task
Blocked by: 07

The maintainer plays matches with items on, with NVDA, and judges `.scratch/items/spec.md`. The maintainer's own sound files go in first, through `tools/a11y/prepare_sounds.py`, if they have not already.

## What to try

- **Appearances.** A match against a CPU with items at High or Very High. Is the name alone enough? Does the appearance cue place the item well enough to head for it, in the air where it comes in? Does a Party Ball's burst read as one line?
- **The item's position cue.** Find the nearest item by ear. Is the steady pace right beside your position cue and the opponent's? Is the within-reach pace clearly different, and does facing away making it slow help or confuse? Does the cue jumping to a newly appeared nearer item get in the way?
- **The frame.** Items relative to you on and off. Which is easier for finding an item?
- **Pickups.** Your pickups and the CPU's, in a 1v1 and a free-for-all of four. Are the labels quick enough? Do pickups crowd out the damage lines?
- **Containers.** Is hearing a Crate or Barrel appear and taken useful, or noise?
- **Left out for now.** Peach's pulls, Pokémon and stage hazards: is their absence felt?
- **The sounds.** Do the levels sit right against the other cues, and does the cut at the within-reach pace sound right?
- **Online**, if convenient.

## Done when

The maintainer's findings are recorded below, each either fixed or filed as its own issue, and CLAUDE.md's status line says items were play-tested.

## Comments

### 2026-11-13, code review (agent)

The review of 61f40c0a..90ead041 fixed what was plainly the agent's in its own commit ("A11y: items, review fixes"). These are left for the maintainer, most severe first, each with the agent's recommendation:

- **Within reach in the air.** `match_items` (`game_access.c`) asks the game's test for light and heavy items (flags 3) on every tick, airborne or not. In the air the test uses the air box, but A there is an aerial: the game's own air catch is Z, for light items only (`ftCo_Attack100.c` passes 1). So a short hop near a Crate, or falling past a Ray Gun, quickens the cue, while `ACCESSIBILITY.md` says the quick pace means pressing A would take the item. Options: in the air ask for light items alone and say in `ACCESSIBILITY.md` that Z catches them there; or count nothing as within reach in the air; or keep it. Recommended: the first, the game's own air catch.
- **The item followed is not always the one within reach.** The cue follows the loose item nearest your feet, and only that item can be within reach; the game's answer is the item nearest its pickup box in front of you. With one item just behind you and another in front, the cue pulses steady from the one behind though A would take the one in front; and when the game's answer is an item the fork leaves out (a turnip, a Link bomb), nothing is within reach. Options: follow the within-reach item first when there is one (`ItemPosition::tick`, or the snapshot's order), or keep following the nearest. Recommended: the within-reach item first, so the quick pace always means A takes something.
- **Online, a rollback can shift serials.** Every item made, projectiles included, takes the next serial from one counter (`it_804D6D10`). A rollback that corrects a peer's predicted shot or a container broken on a guessed frame can give a common item made inside the window another serial on the corrected timeline: the same item is then said and chimed twice, or a real new item lands on a serial already seen and is never said; the hands' comparison can likewise say a pickup twice. Options: key the seen set on serial and kind (catches a reused serial, still drops a true re-creation), match a vanished item to a new one of the same kind and place on the tick after a re-run, or keep it until an online play test hears it. Recommended: serial and kind, a small change in `NewItems`.
- **Items beyond the snapshot's 32 are said late.** The spec asks that a list longer than the snapshot still notices every new serial; `NewItems` sees only the 32 nearest the snapshot keeps, so an item that comes in far away while the list is full is said and chimed when it first comes among the nearest 32, from where it is then. The game allows 40 common items and 100 Food at once. Options: the C side reports every serial (an uncapped list beside the 32), or keep it as a known limit. Recommended: keep it, noted in `cues.md`, unless the play test meets a burst that big.
- **Nana's re-pickups are not said.** The hands come from each slot's main fighter alone; the tally still catches Nana's first holds, so only her re-pickups and catches of an item already held are missed. Recommended: keep it as a known gap, noted in `gameplay-state.md`.
- **The relative values are registered twice, and the item frame holds two values that are not shared.** `add_opponent_values` and `add_item_values` (`match_cues.cpp`) register the same four distances with the same ranges, building `RelativeMapping` two ways; `ItemValues` holds the appearance cue's pitch range (which `ItemPositionValues::mapping` overrides) and the position cue's crossing margin. A shared `RelativeValues` and moving those two values to their cues' groups would change `a11y.ini` keys. Recommended: once the play test has settled the tuning values.

### 2026-11-13, maintainer's decisions on the review

The maintainer fixed findings 1 to 3 as the review recommended, kept 4 to 6 as they are, and chose no credits file for the cue sounds.

- **1, within reach in the air: fixed** (0175a91f). The snapshot asks the game's reach test as the press would: on the ground light and heavy items, as A's pickup; in the air light items alone, as Z's air catch (`ftCo_800D7100`). It also asks nothing in the air while you hold an item, as that catch does, since Z then throws the item held. Still a read only, no random draw. `ACCESSIBILITY.md` says A picks up on the ground and Z catches in the air.
- **2, the item followed: fixed** (05a9ee09). `ItemPosition::tick` gives the first voice the loose item within reach, else the nearest; the snapshot's list stays in order of distance, as the appearances read it, and with several voices the rest follow nearest first. The switch only moves the voice, as a new nearest item does. The second case stays: when the game's answer is an item the fork leaves out (a turnip, a Link bomb), or one not loose (a thrown item flying in front of you), no item the cue follows is within reach and the nearest pulses at the steady pace. That is right by the rule the fix serves: the quick pace means the button takes the item heard, and here it would take another. Hearing that some other item is in reach would need the fork's items to include character items, which the spec leaves out.
- **3, rollback-shifted serials: fixed** (ba5756b6). `NewItems` keys its seen set on serial and kind, so a serial reused by another kind is said and chimed; GAME!'s ticks still mark nothing seen. The hands' comparison in `item_takes.cpp` takes the same key: a first hold on such a serial the tally already caught, so the key adds only a re-pickup. Neither key helps the same item on a shifted serial, which is still said again (an appearance, or a pickup with no tally rise, which the hands cannot tell from a re-pickup), nor a new item of the same kind on a serial seen, still missed. Both wait for an online play test that hears them.
- **4, items beyond the snapshot's 32; 5, Nana's re-pickups; 6, the relative values registered twice: kept** as they are.
- **CREDITS.txt: removed** (3e31b1f2). The maintainer chose no credits file; `cues.md` says the files are placeholders made by code until the maintainer's recordings arrive. Issues 02 and 07 still mention it as written then.

Verified at the quick, menus and online levels with `MELEE_A11Y_CUES=1`, the online level against the fork and against the base port (`--base-b ../melee-pc-base/build`): every run passed, `match_items`, `match_items_vs` and `match_items_net` among them; the unit tests passed but the known `launcher_data`.

New for the play test: in the air the item's position cue quickens only for a light item with your hands empty, for Z; and an item in front of you within reach takes the cue from one nearer behind you.
