# Higgsfield animation / cinematic production lane

**Status:** source/provenance pipeline only; no provider output is a UE asset by default.  
**Provider state checked:** 2026-08-30.  
**Workspace state:** Free plan, 10 credits, Unlimited false. Two Seedance 2.0 Mini 4 s / 480p submission attempts were rejected before job creation with `Requires basic plan or higher`; zero credits consumed.

## Purpose

Higgsfield is the upstream **motion, performance and cinematic-previsualisation assistant** for Dark Arisen. It is deliberately not the project's general look-development or 3D-production owner.

Current approved production uses are:

- combat and weapon-motion previs;
- wound, traversal, carry and interaction motion reference;
- mounted/horse gait and rider-body reference where the animation authority already defines the requirement;
- facial/body-performance reference for already-authored beats;
- cinematic/cutscene previs for resolved `PresentationProductionCatalog` identities.

Current non-uses in this tranche:

- canonical character faces/costumes;
- boss faces/body sheets;
- dungeon/world/settlement look generation;
- ship look generation;
- flora/fauna look generation;
- prop/treasure look generation;
- general 3D/GLB production;
- dialogue writing;
- missing main-story content.

Those families remain in the native Dark Arisen source/readiness catalogs and later 3D/art pipeline.

## Current finite provider scope

Exactly **37** source-derived Higgsfield briefs are permitted today:

- 11 named animation requirements;
- 12 system animation/performance requirements;
- 14 resolved cutscene identities.

The five unresolved final-act cutscenes are excluded.

## Evidence chain

1. `RequirementOnly`
   - Canon/source requirement exists.
   - No provider media exists.

2. `PrevisGenerated`
   - An external motion/performance/cinematic reference exists.
   - It remains non-shipping reference material.

3. `ImportedForReview`
   - Real UE animation/Sequencer work based on the reference exists.
   - Required skeleton/retarget/Montage/Notify/Sequencer ownership has been inspected.

4. `RuntimeAccepted`
   - The exact integrated result has passed the relevant UE 5.5 runtime/automation/performance acceptance on the exact commit.

No provider may jump evidence states.

## Animation lane

Higgsfield video may communicate pose language, silhouette, weight, rhythm, contact intent and body performance. It cannot override Dark Arisen's timing law.

UE-side production still owns:

- AnimSequence/AnimMontage construction;
- AnimNotify contact timing;
- root motion and locomotion integration;
- skeleton and retarget mapping;
- animation graph transitions;
- exact authored weapon startup/recovery values;
- minimum readable attack tells where locked;
- exact six-frame deflection window;
- non-cancellable attack recovery;
- wound-layer gameplay state;
- horse gait-state ownership;
- carry/interaction ownership;
- Crystal Katana clean-set exception.

A visually excellent provider clip is rejected if it erases weight, commitment, readability or source timing.

## Cinematic lane

Only the fourteen resolved cutscene identities may receive cinematic previs today.

Previs must preserve:

- camera rules from `PresentationProductionCatalog`;
- player-control ownership;
- nineteen-total cutscene boundary without inventing missing identities;
- twenty-two protected playable moments remaining playable;
- no reactive music;
- no unauthorized slow motion;
- authored duration where the source provides one;
- no invented dialogue;
- no extra reaction shots inserted simply to increase drama.

Sequencer integration is later Unreal work.

## NON_AI_SLOP review

Every generated motion/cinematic candidate is reviewed against `Docs/NON_AI_SLOP_STANDARD.md`.

Reject by default:

- floaty or centre-of-mass-free motion;
- sliding feet;
- generic mocap gestures repeated across characters;
- over-performed idle/head/breath motion;
- camera orbits and drone moves with no narrative reason;
- speed ramps used as excitement punctuation;
- constant shallow depth of field;
- unnecessary cuts;
- spectacle that converts a protected gameplay beat into a movie.

Provider output is reference, not authority.

## Cost / plan boundary

- No provider upgrade or credit purchase is automatic.
- No billable generation is started without explicit operator approval.
- Existing free credits do not bypass a plan restriction.
- Two test submissions already proved that the useful Seedance path currently requires a higher plan before job creation.
- No purchase or upgrade was performed.
- No credits were consumed by those rejected submissions.

Therefore production remains at `RequirementOnly` until a permitted provider path exists or the operator separately approves the required provider cost/change.

## Relationship to 3D / visual production

The repository still contains detailed source-readiness catalogs for characters, bosses, ships, dungeons, Highmoore, fauna, flora and props. They are intentional and useful. They are **not** Higgsfield submission queues.

`ThreeDAssetReadinessCatalog` separates what is ready for a future modeler/3D pipeline from what remains reference-only or blocked. Current connected provider discovery does not prove a usable 3D action, and the operator has not assigned Higgsfield that responsibility.

## Relationship to Game Studios / Kimi

Game Studios/Kimi may review motion/cinematic briefs for omissions, contradictions and production clarity. It may not create missing canon or expand Higgsfield into visual families that are outside the current operator-approved scope.

## Relationship to the later Alpha

The current objective is to exhaust authored source/content/asset-readiness work before depending on UE runner/editor availability. Later, Unreal is still required for actual integration, gameplay ownership, levels, AI/navmesh, animation graphs, Sequencer, save/load, runtime validation, performance and Windows/Linux Shipping Alpha packages.

After both same-commit Alpha packages exist, the planned private Arcware-style complete playthrough/exploration becomes the quality loop. That playthrough evaluates the real game; it does not replace native build/runtime evidence.
