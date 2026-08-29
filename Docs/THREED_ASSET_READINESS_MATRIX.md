# 3D asset readiness matrix

**Branch:** `feature/pre-runner-asset-production`  
**Evidence level:** source-readiness only. No generated 3D candidate or Unreal import is claimed.  
**Provider fact:** the connected Higgsfield discovery has not exposed a verified usable 3D-generation action/model.

## Why this matrix exists

A good concept image is not automatically enough to produce final geometry. This register separates three source states:

- `CandidateGeometryReady` — current source is sufficient to construct a non-shipping 3D candidate without choosing missing canon;
- `ReferenceOnly` — visual exploration is useful but one complete/final mesh would collapse an authored ambiguity or choose an unlocked visual fact;
- `Blocked` — current authority explicitly prevents 3D candidate production.

All three remain below actual provider generation, Unreal import and runtime acceptance.

## Current finite register

| Family | Records | Candidate geometry ready | Reference-only | Blocked |
| --- | ---: | ---: | ---: | ---: |
| Major characters | 9 | 5 | 1 | 3 |
| Legacy/deep-dive boss visuals | 21 | 19 | 0 | 2 |
| La Liberacion | 6 | 5 | 0 | 1 |
| Fauna | 24 | 16 | 3 | 5 |
| Flora | 9 | 7 | 1 | 1 |
| State Treasures + current named unique reward | 10 | 1 | 9 | 0 |
| Grounded named-dungeon modular kits | 40 | 40 | 0 | 0 |
| Highmoore named-anchor modular kits | 12 | 12 | 0 | 0 |
| **Total** | **131** | **105** | **14** | **12** |

Native authority: `Source/DarkArisen/Production/ThreeDAssetReadinessCatalog.h/.cpp`.

Automation contract: `Source/DarkArisen/Tests/ThreeDAssetReadinessSpec.cpp`.

## Important examples

### Characters

The five core crew members Mira, Big Tom, Ines, Father Salvio and Esteban have enough physical + wardrobe source for non-shipping rigged candidates. Jake's body/proportion reference is grounded, but his complete canonical primary costume is not; his full rig remains `ReferenceOnly`. Elowen is blocked by the missing physical sheet. Ethan and Draven remain blocked by current Phase-11 story authority.

### Boss visuals

Nineteen deep-dive boss visuals are detailed enough for non-shipping sculpt/rig candidates. That does **not** make their visual catalog the current boss-count/gameplay register. Ethan and Draven remain blocked. Final boss/category selection remains governed by the current boss sources and `Docs/M7_TIER1_BOSS_REGISTER.md`.

### La Liberacion

Five deck/interior kits are source-ready as modular environment candidates. The exterior hull is blocked because the current source does not lock final hull class, overall dimensions, mast/sail plan, color scheme or figurehead. A provider must not invent the exterior and make it canon by momentum.

### Fauna

Most individually described legendary animals can become non-shipping creature-mesh candidates. Three visually useful records remain `ReferenceOnly` for complete 3D:

- Memory Stag — antlers/face are deliberately variable;
- Threshold Fox — tail count deliberately varies;
- Deep One — the full body is never revealed.

Final Wolf remains `Blocked` because its appearance depends on the individual save's hunting history. Four under-specified Highmoore animal variants remain blocked as recorded in the fauna matrix.

### Flora

Infinite Branch is `ReferenceOnly` for full geometry because its scale and silhouette change between viewings. Ethan's Grove is blocked pending current story authority. The other seven individually grounded flora/tree identities can proceed to non-shipping vegetation/static candidates while preserving runtime-only phenomena separately.

### Props

The Crystal Katana has a dedicated physical sheet and is geometry-ready as a weapon candidate. The nine State Treasures are authored as named physical documents with fixed origins, but their cover/binding/paper/wear/layout appearance is not locked; they stay `ReferenceOnly` for unique final prop geometry/materials.

### Dungeons and Highmoore

Forty grounded dungeon records and twelve Highmoore anchors can drive **modular architecture/prop/material kit candidates**. That is intentionally not the same thing as a level. This matrix does not create room coordinates, final layout, encounter placement, navmesh, lighting, World Partition/streaming cells or `.umap` evidence.

## Evidence ladder

For every future 3D record:

`Source readiness` → `provider/modeler candidate generated` → `Unreal ImportedForReview` → `RuntimeAccepted`.

No source-ready record currently skips the first arrow. The connected provider 3D-action flag remains false until a real supported action/model is discovered and verified without an unauthorized purchase or plan upgrade.
