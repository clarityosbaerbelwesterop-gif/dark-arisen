# Animation + Presentation source completion boundary

**Date:** 2026-08-29  
**Integration tranche:** `feature/content-alpha-production` / PR #13 → `develop`  
**Operator direction:** pursue the Rockstar-quality path; merge this tranche after Animation and Presentation source work is coherent.  
**Evidence status:** source contracts and native tests/validators exist. UE 5.5 compile, Unreal Automation, imported binary assets, Sequencer review and runtime acceptance are not claimed while matching self-hosted runners/editor evidence are unavailable.

## What this boundary means

This is a **source-production merge boundary**, not an Alpha-complete claim and not a declaration that animation or cinematic binary assets are finished.

The purpose of the boundary is to put the authored rules, finite production lists, gameplay ownership and fail-closed validations on `develop` before the next content tranche continues. It deliberately refuses the shortcut of inventing absent assets, final-act material or tuning values merely to make a checklist look complete.

## Animation source state

The native animation production authority now records and validates:

- all eleven specifically named animation requirements from `animation system.md`;
- all five weapon weight classes with their authored startup/recovery timings: Light 9/14, Medium 13/20, Heavy 19/31, Great 26/44 and Polearm 15/24 frames;
- the global minimum eight-frame readable attack tell and locked six-frame deflection window;
- non-cancellable attack recovery as a production law;
- the four deterioration layers: Winded, Hurt, Bad and Failing;
- Katana clean-set behavior while Jake is wounded;
- posture readability, wounded locomotion, traversal effort, world interaction, people carrying, horse gait transitions, horse body-language mood and Jake's permanently worse rider layer;
- no false `bAssetAuthored` or asset-path claim before an imported/reviewed Unreal asset actually exists.

`CoreLoopTuning.h` now statically asserts that every weapon-class startup preserves the eight-frame readability floor and that the deflection window remains exactly six frames.

Previously hard-coded wound presentation multipliers are now exposed as `EditDefaultsOnly` tuning with explicit `DESIGN-GAP` comments because the governing documents specify qualitative deterioration but do not lock those numeric movement/camera amplitudes. The qualitative requirements remain fixed: Winded breathes, Hurt favours a side, Bad limps/stagger-runs, Failing cannot sprint and drags the weapon. Camera instability remains driven by deterioration only.

## Presentation source state

The presentation authority keeps the canonical separation between:

- **19 authored cinematic slots**;
- **22 protected playable moments that must never become cutscenes**;
- **5 inserts**;
- **6 slow pushes**.

Control ownership is no longer flattened into a generic Sequencer lock:

- The Grove uses an anchored camera while player movement remains available, then returns to free camera.
- The Emergence is the six-second anchored wide with movement available and no flythrough.
- The Voice From Behind is only a brief authored takeover for the turn, then returns to free camera.
- The Arrow is exactly a 1.5-second brief takeover, camera static, with the existing music continuing unchanged; the thirty seconds after it remain protected gameplay.
- The Real Letter owns only the eight-second pillow slow push; reading is player-paced and remains gameplay.
- Every boss death, Ibarra in the garden, Liberation aftermaths and the other Section-6 moments remain protected from cinematic conversion.

The existing `UCameraStateComponent` already distinguishes Free / Anchored / Authored behavior, with Anchored refusing look input while preserving movement. No additional camera seizure was invented where source timing is not explicit.

## Presentation gaps intentionally left open

The following are not filled with invented material:

1. Cutscene slots #15–#19 remain unresolved because `cutscene catalog.md` delegates them to a Crimson Armada source that is not present under the referenced source identity in the repository.
2. The P1 aggregate-count discrepancy remains explicit: the local #9 entry says P1 while the later summary omits #9 and still states its own total. Narrower local authority wins for #9; the aggregate conflict is not silently rewritten.
3. No Sequencer, performance-capture, facial-animation or final imported animation asset is claimed without actual Unreal evidence.

## World-quality work included in the same tranche

The Rockstar-path continuation also moved the world layer forward instead of jumping directly to packaging:

- regional population production contracts now encode source-authored density ranges, demographic shares, regional daily rhythms, Jake-reaction rules, chapter evolution, purpose-driven routines and child-protection requirements;
- Region 06 remains population-by-absence rather than receiving generic crowd filler;
- fauna/flora production locks the Phase-4 corpus scale: 42 land animals + 6 legendary variants, 52 birds + 4 legendary, 65 sea species, 45 small-creature types + 3 legendary, and at least 60 plants/herbs + 4 legendary;
- regional ecological/audio identities are explicit and generic/random wildlife substitution is prohibited;
- The Patriarch, The Keeper Below and The Canopy Jaguar are explicit source-backed production anchors rather than generic spawn variants;
- runtime crowd/creature/plant assets remain open evidence rather than false source assertions.

## Native source gates added/strengthened

`DarkArisenOps validate-world` now includes the population/fauna production files, counts, named ecological anchors and a source scan against random/generic authored-world generation paths.

`DarkArisenOps validate-presentation` now requires the explicit animation readability/tuning contracts and the special presentation control-ownership boundaries.

Native Unreal Automation specs cover the new world-production contracts, all weight-class animation timings/readability requirements and the presentation ownership cases. These tests are **not reported as passed** until they actually execute on the approved UE 5.5 runners.

## What remains after this merge

The next tranche must continue from the merged `develop` head and keep the same quality bar:

- actual `.umap` world/settlement/dungeon production and authored placements;
- full fauna identity transcription and actual creature/plant/AI/audio/animation assets;
- remaining source-backed mission, reward, boss and encounter integration without touching the 275 unauthored quest identities unless approved design appears;
- actual animation assets, capture/keyframe work and AnimGraph/Montage integration;
- actual Sequencer/camera/facial-performance assets for the resolved cinematic material;
- final-act #15–#19 only when an approved governing source exists;
- then audio/music/credits, dialogue lock/casting/voices, save/platform integration and finally real UE 5.5 compile/automation/performance/package/playthrough evidence.

This boundary is intentionally strict: **source can define what must be built; it cannot pretend the thing has been built in Unreal.**
