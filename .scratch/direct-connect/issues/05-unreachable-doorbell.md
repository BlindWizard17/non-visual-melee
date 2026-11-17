# 05 Calls to some codes fail: crawler nodes swallow the doorbell

Status: needs-triage
Type: task

Parked on 2026-11-17 by the maintainer: recorded so the work can be picked up later, not started.

## What happened

On 2026-11-16 the maintainer played a sighted friend over the internet by Direct Connect: the maintainer on Windows, the friend on a Mac build of the fork. Every call from the maintainer to the friend connected within seconds. Every call from the friend to the maintainer failed: two tries, about four minutes in all, with the maintainer waiting the whole time.

The logs were `build/melee-pc_connect_success.log` (Windows) and `build/melee-pc_connect_success_mac.log` (Mac). Neither is committed, and they may be gone.

## Diagnosis

The cause looks like the callee's code, not who calls or the Mac build.

- How players meet: the waiting player posts their address to the DHT (the BitTorrent network's shared lookup table) under a key made from their own code's suffix. The caller searches the same key (`pc_dht_topic` in `src/pc/net_dht.c`, the doorbell in `src/pc/net_match.c`). The caller also searches a pair key made from both codes, but a player who is waiting does not.
- At the friend's key (`TJSWBBCW`), a waiting Mac saw its own posted address come back within about 17 seconds. On a later call it even saw addresses the maintainer had posted 20 minutes before. One ordinary home computer (188.156.114.44) was among the computers posted to in every session.
- At the maintainer's key (`LATG5MC5`), both machines posted many times, partly to the same computers. Neither ever found the other's address, and the maintainer's machine never found its own.
- The computers at that key were mostly in a few server address ranges (169.197.143.x, 216.152.149.x, 134.195.198.x, 147.135.7.63, 162.251.63.79), each answering from many ports. That fits Sybil crawlers: one operator running many fake DHT identities, typically torrent indexers that take a post to learn the key and never hand it back.
- 31.200.249.x answered at both keys with its own address and never replied to the game's hello: probably a monitoring honeypot, harmless noise.
- A later test (2026-11-16, the maintainer waiting with nobody calling) found the address did come back at `LATG5MC5`, but only after about 66 seconds. It came back right after the game first posted to a computer it had not used before (212.129.74.255). So the key is not dead: it relies on one or two honest computers that come and go among the crawlers.

## Why the library lets the crawlers in

The vendored jech/dht (`extern/dht/dht.c`) has no BEP 42 support (a node's identity bound to its IP address) and no limit on entries per IP address:

- A search keeps the 14 computers closest to the key (`SEARCH_NODES`) and drops a duplicate only when its identity matches (`insert_search_node`). It then posts to the closest eight that answered (`search_step`).
- One IP address took two of those places in the same search in both logs: 134.195.198.230 on ports 62401 and 61035, and 216.152.149.244 on ports 60854 and 63393.

## Options

1. **Pair keys while waiting (fork side).** While waiting, also wait at the pair key for each recent contact (`pc_dht_add_topic`, up to `EXTRA_TOPICS`). A caller already searches its pair key, so this adds a second meeting place in another part of the network, and pairing with base port builds still works. It helps only friends who have played before.
2. **One entry per IP address in a search (`extern/dht`).** In `insert_search_node`, keep at most one entry per IP address, so one crawler IP can't take several of the eight places. This is a base port file and helps every player, so it would go in its own commit as a fix, not as a footprint change. It changes only which DHT computers are asked, not what the two games send each other. It doesn't stop a crawler spread over a whole address range.
3. **Several doorbell keys per code (base port protocol).** One crowded key could then no longer block a player. Both sides would need the change, so it's for the base port. Raise it there only if the maintainer asks.

Options 1 and 2 work together.

## Until then

Whoever has a reliable doorbell calls. When a call to someone fails, try the other direction, and give a call at least a couple of minutes before giving up.

## How to check

A run with the player waiting and nobody calling shows whether their own address comes back: a `dht: candidate discovered` line with their own public address, timed from `lobby: direct connect waiting as`. Compare how soon it comes back before and after a change, at a known bad key. The check needs the maintainer's identity, so the same doorbell key, in the run's settings folder.

## Comments
