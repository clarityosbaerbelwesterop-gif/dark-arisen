# Pre-main-quest readiness register

**Status:** source/readiness checkpoint, not Alpha acceptance.  
**Branch:** `feature/pre-runner-asset-production` / PR #18.  
**Purpose:** make the remaining work small and explicit before main-story implementation begins.

## What this register means

A row marked source-ready means the repository now has enough current authority for the stated **source/readiness purpose**. It does not mean the corresponding Unreal asset exists, compiles, runs at 60 fps or has been playtested.

The native counterpart is `Source/DarkArisen/Production/PreMainQuestReadinessCatalog.h/.cpp`.

## Current closure table

| Family | Registered | Production-ready | Reference-only | Explicit gaps | Boundary |
| --- | ---: | ---: | ---: | ---: | --- |
| Dungeon identities | 61 | 40 | 0 | 21 | 20 minor + 1 named Region-06 identity remain unauthored; Crystal Caves is separate |
| Named Threads | 17 | 17 | 0 | 0 | owners exist; individual sub-gaps stay fail-closed |
| Turns | 132 | 3 | 0 | 129 | no generated filler |
| Standing variants | 147 | 1 | 0 | 146 | only San Telmo Bell individually complete |
| Tier-1 gameplay identities | 9 | 9 | 0 | 0 | the Nine Who Hold |
| Tier-1 full physical look sheets | 9 | 0 | 0 | 9 | current sources do not lock complete physical sheets |
| World regions | 8 | 8 | 0 | 0 | final maps/placement remain UE work |
| Highmoore anchors | 12 | 12 | 0 | 0 | no invented coordinates |
| State Treasures + named unique reward | 10 | 10 | 0 | 0 | physical prop appearance can still be incomplete |
| Major-character full-geometry readiness | 9 | 5 | 1 | 3 | Jake reference-only; Elowen + Ethan/Draven blocked for different authority reasons |
| La Liberación geometry readiness | 6 | 5 | 0 | 1 | exterior silhouette not locked |
| Fauna geometry readiness | 24 | 16 | 3 | 5 | variable/hidden/under-specified identities preserved |
| Flora geometry readiness | 9 | 7 | 1 | 1 | Infinite Branch variable; Ethan's Grove story-blocked |
| Cross-family 3D source readiness | 140 | 105 | 14 | 21 | no generated meshes claimed |
| Animation requirements | 23 | 23 | 0 | 0 | actual Unreal animation assets still absent/unverified |
| Cutscene identities | 19 | 14 | 0 | 5 | final-act slots 15-19 unresolved |
| Higgsfield motion/cinematic briefs | 37 | 37 | 0 | 0 | RequirementOnly; no provider media claimed |

## The small closure list

This is the best way to keep the remaining authored work honest: a short list of finite gaps rather than one vague percentage.

### Authorship gaps that are deliberately **not** being fabricated

1. **129 Turns** without individual authored identities.
2. **146 Standing variants** without individual authored identities.
3. **20 Minor/Tier-A dungeons** without individual authored identities.
4. **1 additional named Region-06 dungeon identity** unresolved.
5. **9 current Tier-1 complete physical look sheets** unresolved.
6. **1 La Liberación exterior silhouette package** unresolved.
7. **5 final-act cutscene identities** unresolved.
8. Character/art special cases: Jake primary costume incomplete; Elowen physical sheet incomplete; Ethan/Draven current-story visual identity blocked until the Phase-11 conflict is resolved.

The Turn + Standing mission identity gap remains exactly **275**.

## Higgsfield boundary

Higgsfield has been reduced to the work it is actually wanted for:

- animation motion reference;
- body/performance reference;
- cinematic/cutscene previs.

Exactly **37** source-grounded briefs are in that queue: 23 animation/performance + 14 resolved cinematics.

It is **not** the queue for characters, boss looks, worlds, dungeons, fauna, flora, ship looks, props or general 3D.

## 3D boundary

`ThreeDAssetReadinessCatalog` currently tracks 140 records:

- 105 candidate-geometry-ready;
- 14 reference-only;
- 21 blocked.

That is a modeling-readiness result, not a claim that 105 meshes exist.

## Runtime boundary

Still open and unclaimed:

- UE 5.5 Windows/Linux compile;
- Unreal Automation execution;
- real `.umap` / `.uasset` production evidence;
- retarget/Montage/Notify/Sequencer integration;
- navigation/collision/AI runtime;
- 60-fps evidence;
- Windows/Linux Shipping ZIPs;
- full start-to-credits playthroughs;
- private streaming acceptance.

These become relevant after the authored/source/content path and main-story authority are closed.

## Next boundary

The next content family is the main story. We do **not** implement it from chapter summaries alone. See `Docs/MAIN_QUEST_AUTHORITY_GAP.md` for the exact reason the current tranche stops there.
