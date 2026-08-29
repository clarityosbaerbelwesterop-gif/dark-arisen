# Higgsfield game-asset production lane

**Status:** source/provenance pipeline only; no provider output is a UE asset by default.  
**Provider state checked:** 2026-08-30.  
**Workspace state:** Free plan, 10 credits, Unlimited false. Two Seedance 2.0 Mini 4 s / 480p submission attempts were rejected before job creation with `Requires basic plan or higher`; zero credits consumed.

## Purpose

Higgsfield is the upstream visual and motion-production assistant for Dark Arisen. It may help turn already-authored design into reference media and candidate assets. It is not design authority and it does not replace Unreal Engine as the eventual integration/runtime/package authority.

The target production uses are:

- character look/reference exploration from approved character sources;
- region, settlement and landmark reference images;
- named dungeon visual-language and unforgettable-image reference work;
- physical prop/treasure reference work;
- combat, wound, traversal, interaction and mounted motion previs;
- facial/performance reference for already-authored beats;
- cinematic/cutscene previs for resolved PresentationProductionCatalog identities;
- candidate 3D/rig/motion output only if the connected provider later exposes a concrete supported model/action and its cost/rights preflight passes.

## Current connected capability boundary

Connected model discovery currently returned useful image and video families, including Nano Banana Pro, FLUX.2 Max, Soul, Seedream, Seedance, WAN and Hunyuan. A 3D-model search did not return a usable connected 3D generation model/action.

Therefore the repository must not currently claim:

- that Higgsfield has generated a GLB for this project;
- that a provider mesh is rigged;
- that a provider motion clip is importable;
- that a generated reference already has an Unreal skeleton, collision, materials or LODs.

If a supported 3D action appears later, it must enter the same evidence chain as every other external output.

## Evidence chain

1. `RequirementOnly`
   - Canon/source requirement exists.
   - No provider media exists.

2. `PrevisGenerated`
   - An external reference image/video exists.
   - It is still non-shipping reference material.

3. `CandidateAssetGenerated`
   - A real candidate mesh/audio/motion artifact exists.
   - It has not yet passed Unreal import review.

4. `ImportedForReview`
   - The candidate has actually been imported into the UE project.
   - Required material/skeleton/collision/retarget/Sequencer settings have been inspected.

5. `RuntimeAccepted`
   - The exact imported asset has passed the relevant UE 5.5 runtime/automation/performance acceptance on the exact commit.

No provider may jump evidence states.

## Animation lane

Higgsfield motion/video is valuable for pose language, silhouette, weight, rhythm, physical intent and performance reference. It cannot by itself prove Dark Arisen animation timing.

The UE-side production must still own:

- AnimMontage structure;
- AnimNotify contact timing;
- root motion/locomotion integration;
- skeleton and retarget mapping;
- animation graph transitions;
- exact 8-frame minimum attack tell;
- exact 6-frame deflection window;
- non-cancellable attack recovery;
- wound-layer gameplay state;
- horse gait-state ownership;
- Crystal Katana wound-animation exception.

A good Higgsfield clip can be the motion target. It is not the timing authority.

## Cinematic lane

Only the fourteen resolved cutscene identities may receive cutscene previs today. The five unresolved final-act cutscene slots remain blocked.

Previs must preserve:

- camera rules from `PresentationProductionCatalog`;
- control ownership;
- no reactive music;
- no unauthorized slow motion;
- protected playable moments remaining playable;
- authored duration where the source provides one;
- no invented dialogue or emotional reaction shot.

Sequencer integration is separate Unreal work.

## Dungeon/world lane

Provider briefs may be built for:

- the forty currently grounded named dungeon production profiles;
- the eight authored world-region identities;
- the twelve currently grounded Highmoore world anchors;
- source-backed props and State Treasures.

They may not be built for:

- twenty unauthored minor-dungeon identities;
- the unresolved additional named Region 06 slot;
- invented coordinates;
- invented settlement/landmark identities;
- quest-marker/minimap presentation;
- generic fantasy filler substituted for missing authored design.

Dungeon briefs preserve the source Access/Discovery, Puzzle, Hazard, Unforgettable Image, Boss/Bottom and Reward reads. A withheld element stays withheld.

## Character lane

Character generation begins only after the relevant character/NPC source has been read and converted into a source-backed visual brief. A provider prompt may not invent a canonical face, age, clothing, scars, body type, ethnicity, equipment or costume detail merely because an image model needs specificity.

If the source does not define a visual fact, it remains omitted or is explicitly marked as a design gap. Consistency work should then use an approved reference image once one exists.

## Cost, privacy and rights

- No provider upgrade or credit purchase is automatic.
- No billable generation is started without explicit operator approval.
- Existing free credits do not bypass a plan restriction.
- Provider job IDs and stable output references may be recorded; secrets, signed upload URLs and credentials may not.
- Preview generation does not prove commercial/shipping rights.
- Rights/cost clearance is a separate recorded gate before shipping use.

## Relationship to Game Studios / Kimi

Game Studios/Kimi may review a source-derived brief for omissions, contradictions and production clarity. It may not rewrite canon or fill a missing identity. The correct output for missing information is `DESIGN-GAP`, not an invented solution.

## Relationship to Unreal and the later Alpha

The current production objective is to complete as much authored source/asset/previs work as possible before bringing the UE runner/editor path online. Later, Unreal remains necessary for actual integration, gameplay ownership, levels, AI/navmesh, animation graphs, Sequencer, save/load, runtime validation, performance and the Windows/Linux Shipping Alpha packages.

After those two same-commit Alpha packages exist, the planned private Arcware-style browser playthrough is the acceptance/quality loop: complete and explore the game end-to-end, record issues, then perform a focused improvement tranche. The streaming playthrough does not replace native Windows/Linux acceptance evidence.
