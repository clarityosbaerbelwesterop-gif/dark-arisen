# Pre-main-quest readiness register

**Status:** source/readiness checkpoint, not Alpha acceptance.  
**Branch:** `feature/pre-runner-asset-production` / PR #18.  
**Purpose:** make the remaining work finite and explicit before main-story implementation begins.

## What this register means

A row marked source-ready means the repository has enough current authority for the stated **source/readiness purpose**. It does not mean the corresponding Unreal asset exists, compiles, runs at 60 fps or has been playtested.

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

A short finite list is intentionally used instead of a vague completion percentage. It separates **missing authored decisions** from **later production/evidence work**, so a missing mesh or runner result cannot be mistaken for missing story design and vice versa.

### A — Content/authorship decisions that remain genuinely missing

These are not implementation bugs. Current design authority does not provide the individual identity or final story fact, so they stay empty until authored deliberately.

1. **129 Turns** without individual authored identities.
2. **146 Standing variants** without individual authored identities.
3. **20 Minor/Tier-A dungeons** without individual authored identities.
4. **1 additional named Region-06 dungeon identity** unresolved.
5. **5 final-act cutscene identities** unresolved because the current final-act/main-story sequence is not fully authored.
6. **Main-story mission authority** beyond the Phase-11 chapter scaffold: finite mission IDs, exact triggers, participants, routes/actions, outcomes, persistence and presentation ownership are not consistently resolved.
7. **Current Ethan/Draven final-act authority**: the old captured/betrayal/boss branch conflicts with Phase 11 and cannot be restored implicitly.

The Turn + Standing mission identity gap remains exactly **275**.

### B — Art/physical-design authority gaps

These do not prevent the already-authored gameplay identities from existing, but a final visual asset would currently require choosing visual canon that the source has not supplied.

1. **9 current Tier-1 complete physical look sheets**.
2. **1 La Liberación exterior silhouette package**: final hull class/dimensions/mast-sail plan/colors/figurehead remain unlocked.
3. **Jake primary costume sheet**.
4. **Elowen complete physical sheet**.
5. **Current-story Ethan/Draven visual sheets**, tied to the story-authority conflict above.
6. Variable/hidden creature and flora cases that deliberately cannot collapse into one fixed final geometry.
7. Unique physical appearance sheets for the nine State Treasure documents where only identity/origin/outcome are authored.

### C — Later production/runtime evidence, not missing canon

Once the authored decisions above exist, these become the actual Alpha-production gates rather than reasons to invent more design:

- real modeled/rigged/materialed assets;
- production `.umap` / `.uasset` levels and placements;
- animation retargeting, AnimSequences, Montages, AnimNotifies and AnimGraphs;
- Sequencer implementation for resolved cinematics;
- AI, navmesh, collision and save/load integration;
- UE 5.5 Windows/Linux compile and Unreal Automation;
- measured 60-fps acceptance;
- same-commit Windows/Linux Shipping ZIPs and checksums;
- complete start-to-credits runs;
- private Arcware-style full-game exploration/playthrough;
- the improvement tranche based on observed play quality.

## Higgsfield boundary

Higgsfield is used only for:

- animation motion reference;
- body/facial-performance reference for already-authored beats;
- cinematic/cutscene previs for already-resolved identities.

Exactly **37** source-grounded briefs are in that queue: 23 animation/performance + 14 resolved cinematics.

It is **not** the queue for characters, boss looks, worlds, dungeons, fauna, flora, ship looks, props or general 3D.

No Higgsfield job currently exists for those 37 briefs. The tested Seedance path is plan-blocked before job creation, and no provider upgrade or purchase is authorised.

## 3D boundary

`ThreeDAssetReadinessCatalog` currently tracks 140 records:

- 105 candidate-geometry-ready;
- 14 reference-only;
- 21 blocked.

That is a modeling-readiness result, not a claim that 105 meshes exist.

## What is actually exhausted before main-story decisions

All currently source-backed pre-main production families represented in this register have been reconciled to their present authority boundary. That means the next useful **authorship** work is the main story or one of the explicit A/B decisions above; it does not mean the full game or Alpha is content-complete.

## Next boundary

The next major content family is the main story. We do **not** implement it from chapter summaries alone. See `Docs/MAIN_QUEST_AUTHORITY_GAP.md` for the exact decisions required before a main-story implementation tranche can begin.
