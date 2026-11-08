# Dark Arisen: ten milestones to Alpha (O3DE, 2026-11)

**Baseline:** `main` after PR #61, which adds world terrain, vegetation, region architecture, and the foliage and lava shaders. The state of each system is in `Docs/O3DE_MIGRATION.md`.

**Definition of Alpha:** unchanged from `Docs/ALPHA_DELIVERY_CHECKLIST.md`. The whole main story must be playable from Harlow to the Chapter 10 credits, in packaged Windows and Linux builds, with a measured 60 fps, and approved by the owner after a full playthrough. Nothing here lowers that bar.

**Rule for every milestone:** a milestone is done only when its source exists *and* its evidence exists. Evidence means CI, a probe, a render, or a runtime capture, as the milestone states. Work that only exists in source stays PARTIAL.

The milestones are ordered by dependency. Milestones in the same column can run in parallel.

| # | Milestone | Depends on | Owner decision needed |
|---|-----------|------------|------------------------|
| A1 | World complete | — | none |
| A2 | Cast in motion | — | skin look sign-off |
| A3 | Combat and encounters | A2 | deflect posture penalty (open item 4) |
| A4 | Creatures and bosses | A2 | Keeper and Maw boss look |
| A5 | New regions: Ashenmoor, Quiet Coast, The Maw | A1 | chapter placement of Ashenmoor and the Quiet Coast |
| A6 | Missions in depth | A1, A2 | none (the contracts are authority) |
| A7 | Physical world map and navigation | A1 | minimap ruling (open item 5); default is no minimap |
| A8 | Audio, voice and lip sync | A2, A6 | voice production: actors, ElevenLabs, or local TTS |
| A9 | Runtime proof and performance | A1–A4 | a GPU host (no purchase without approval) |
| A10 | Alpha candidate | all | final playthrough and approval |

## A1 — World complete

- **Goal:** every one of the 34 missions stands in a finished place.
- **Deliverables:**
  - Rexa town and harbour on terrain, with streets, houses and the market.
  - Ship decks dressed: planking, rigging, gear.
  - Shoreline foam and shallow water on the ocean, using a baked shore depth per level.
  - Ground cover that sways (split it out of the terrain mesh).
  - The Driftwood and Moran levels moved onto the shared terrain path.
- **Exit criteria:**
  - `materialize --check` is byte-identical on GCC, Clang and an FMA build.
  - The story probe still reaches 27/27 missions and 93/93 actors on ground.
  - There is a preview render of every level.

## A2 — Cast in motion

- **Goal:** the people move and look right in Atom.
- **Deliverables:**
  - EMotionFX actors with anim graphs for Jake, the boarders and the world set, driven by the motion library and the gameplay state.
  - Skin on the engine's `Skin.materialtype`.
  - Hair cards with cutout.
  - LODs (three per actor).
  - Detail normal maps.
- **Exit criteria:**
  - Actor and anim graph assets build in the Asset Processor.
  - The anim graph state machine covers every state in `JakeAnimationManifest.json`.
  - There is a render of every cast entry.

## A3 — Combat and encounters

- **Goal:** combat feels authored, not greybox.
- **Deliverables:**
  - Attack, deflect, dodge, hit and death clips timed on `AlphaCombatTiming.json` and consumed by Core `Combat` and `EnemyBrain`.
  - Contact through animation events, exactly once.
  - Lock-on camera.
  - Encounter layouts on the new terrain.
  - The duels: Draven, Herrera, Isabel Cruz.
- **Exit criteria:**
  - Combat and EnemyBrain tests pass.
  - The level probe resolves every duel.
  - A five-minute encounter capture (needs A9 for the frame rate).

## A4 — Creatures and bosses

- **Goal:** the non-human threats exist.
- **Deliverables:** the Cenote Keeper, sea creatures for the naval chapters, and The Maw's boss. Each is built through the native mesh pipeline with Blender preview, and each has rig, motion and behaviour states in Core.
- **Exit criteria:** models and motion pass `materialize --check`, behaviour tests pass, and there are preview renders.

## A5 — New regions: Ashenmoor, The Quiet Coast, The Maw

- **Goal:** the remaining geography of the world progression map (`MORAN_OPENING_WORLD_AUTHORITY_2026_09.md` §6).
- **Deliverables:**
  - New biomes on the terrain path:
    - Ashenmoor: volcanic, with ash and lava (`DarkArisenLava`).
    - The Quiet Coast: cold fjord, with villages and a lighthouse chain.
    - The Maw: post-story only; it never blocks the credits.
- **Exit criteria:** levels materialise, the probe reaches their entries, and there are preview renders.
- **Limits:** the main chapter order is not changed without an owner ruling.

## A6 — Missions in depth

- **Goal:** the 34 missions play as stories, not as a line of triggers.
- **Deliverables:**
  - Dialogue from the contracts, with choices and their consequences in `LivingWorld` and the reputation model.
  - Journal entries.
  - Side quests from `ContentSource/SideQuests`.
  - Mission-specific set pieces: chases, boarding, sieges.
- **Exit criteria:**
  - Core tests for every branch.
  - The story probe walks every resolution path.
  - The journal and save stay compatible.

## A7 — Physical world map and navigation

- **Goal:** the player finds the way without a HUD marker.
- **Deliverables:**
  - A held paper map drawn from the materialised geography: islands, sea lanes, discovered places.
  - Chart tables on the ship and in Koa's post.
  - Local directions from people.
- **Exit criteria:** map rendering is deterministic, and the map shows only discovered places (a test).
- **Limits:** no minimap unless the owner rules otherwise.

## A8 — Audio, voice and lip sync

- **Goal:** the world sounds and the people speak.
- **Deliverables:**
  - Ambience per biome, the sea, footsteps per surface, combat.
  - Music cues, including the non-reactive credits cue.
  - Voice lines for the contracts' dialogue.
  - Lip sync from the 15 visemes in `FaceShapes.json`.
- **Exit criteria:**
  - Every line has audio and a viseme track.
  - The credits attribute every source.
- **Decision needed:**
  - Who voices the cast.
  - Options are human actors, ElevenLabs (paid; needs approval and a key), or a free local TTS (Apache-2.0, for example Kokoro) as a placeholder.
  - Nothing is bought without approval.

## A9 — Runtime proof and performance

- **Goal:** the game runs in the Editor and the GameLauncher, not only in probes.
- **Deliverables:**
  - The Asset Processor builds every asset.
  - The GameLauncher loads every level.
  - Frame-time capture on the target GPU.
  - Graphics presets consumed by Atom.
- **Exit criteria:** 60 fps floor in every benchmark level, with no runtime errors in the logs.
- **Limits:**
  - "DO NOT CLAIM FULL RUNTIME EVIDENCE UNTIL GAMELAUNCHER ACTUALLY RUNS."
  - A GPU host needs the owner's approval of any cost.

## A10 — Alpha candidate

- **Goal:** the deliverable of `ALPHA_DELIVERY_CHECKLIST.md`.
- **Deliverables:**
  - Windows and Linux Shipping packages.
  - Credits and the complete attribution: third party, tools, music.
  - Checksums.
  - A source archive.
- **Exit criteria:** the owner completes the playthrough and approves.

## Order of work in this branch

1. A1 and A7 need no owner decision, so they go first: shoreline, sway, Rexa, the map.
2. A2, A3 and A6 follow in source.
3. A8 waits on the voice decision.
4. A9 waits on a GPU host.
