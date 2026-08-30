# Higgsfield animation / cinematic previs matrix

**Branch:** `feature/pre-runner-asset-production`  
**Evidence level:** planning / `RequirementOnly` unless real provider evidence is recorded.  
**Operator direction:** Higgsfield is used for **animation, performance and cinematic/cutscene previs**. It is not the canonical character-look, boss-look, world-look, dungeon-look, fauna/flora, ship, prop or general 3D owner.

| Family | Grounded identities | Higgsfield state | External media | Unreal state | Blocked / omitted |
| --- | ---: | --- | --- | --- | --- |
| Named animation requirements | 11 | Briefed | None | None | — |
| System animation requirements | 12 | Briefed | None | None | exact clip/blend/retarget tuning remains UE production work |
| Resolved cutscenes | 14 | Briefed | None | None | 5 final-act cutscene identities unresolved |
| Static character/world/dungeon/boss/ship/fauna/flora/prop looks | 0 | Deliberately outside Higgsfield provider catalog | None | Source-readiness tracked separately | use native visual/3D readiness catalogs |
| 3D meshes/rigged meshes | 0 | Not assigned to Higgsfield | None | None | no verified connected 3D action; future 3D pipeline remains separate |
| Voice shipping assets | 0 | Not a Higgsfield responsibility | None | None | AI Voice Generator is late-stage and separately gated |

## Finite Higgsfield total

`FExternalAssetProductionCatalog` expects exactly **37** current Higgsfield briefs:

- 11 named animation requirements;
- 12 system animation/performance requirements;
- 14 resolved cinematic/cutscene identities.

That is the entire provider-facing Higgsfield corpus at this stage.

A future operator instruction may expand the provider scope, but a model/provider cannot expand it by itself.

## What Higgsfield is for

### Animation / performance reference

Higgsfield may help establish motion, body mechanics, timing feel, performance staging and camera-readable action for requirements that are already authored.

The output remains reference. It is **not** automatically:

- a retargeted animation;
- an AnimSequence;
- an AnimMontage;
- an AnimGraph state;
- an AnimNotify;
- root-motion ownership;
- a final facial rig performance;
- runtime timing evidence.

Locked Dark Arisen timing survives the provider output, not the other way around: readable attack tells, six-frame deflection, committed recovery, wound layers, horse gait ownership and the Crystal Katana clean-set exception remain source authority.

### Cinematic / cutscene previs

Higgsfield may produce camera/performance previs only for the **14 cutscenes whose identities are currently resolved**. Control ownership, camera law, music law and playable-vs-cinematic boundaries remain in `PresentationProductionCatalog`.

The five unresolved final-act slots are excluded. A good-looking generated ending is still invented canon.

## What Higgsfield is not for in this tranche

The following source work remains valuable but is deliberately outside the provider catalog:

- major character look sheets;
- current Tier-1 physical looks;
- legacy/deep-dive boss visual sheets;
- dungeon environment kits;
- regions / Highmoore landmarks;
- La Liberación interior/exterior modeling;
- fauna and flora;
- State Treasure appearance;
- general 3D modeling.

Those areas are tracked in native source-readiness catalogs such as `CharacterVisualProductionCatalog`, `Tier1BossVisualReadinessCatalog`, `BossVisualProductionCatalog`, `ShipVisualProductionCatalog`, `FaunaVisualProductionCatalog`, `FloraVisualProductionCatalog` and `ThreeDAssetReadinessCatalog`. They do not need to be routed through Higgsfield.

## Explicit no-generation boundaries

Do not generate simply to close counts:

- 129 unauthored Turn identities;
- 146 unauthored Standing variants;
- 20 unauthored minor-dungeon identities;
- unresolved named Region 06 dungeon slot;
- 5 unresolved final-act cutscene identities;
- current Phase-11 Ethan/Draven story-conflicted visual material;
- any unresolved main-story trigger, actor, mission sequence or outcome.

Turn/Standing identity gap remains **275**.

## Provider state

Connected Higgsfield preflight on 2026-08-30 remains:

- Free plan;
- 10 credits;
- Unlimited false;
- two Seedance 2.0 Mini test submissions rejected before job creation with `Requires basic plan or higher`;
- zero credits consumed by those attempts;
- no purchase or plan upgrade authorised.

Therefore the 37 briefs are prepared, but no paid/plan-gated job is started automatically.

## Promotion law

`RequirementOnly` → `PrevisGenerated` → `ImportedForReview` → `RuntimeAccepted`.

A generated video can never skip directly to an Unreal or Alpha acceptance state.
