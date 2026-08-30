# 3D asset readiness matrix

**Branch:** `feature/pre-runner-asset-production`  
**Evidence level:** source-readiness only. No generated 3D candidate or Unreal import is claimed.  
**Provider fact:** the connected Higgsfield discovery has not exposed a verified usable 3D-generation action/model, and current operator direction does not use Higgsfield as the general 3D/look-development owner.

## Why this matrix exists

A design source can be detailed enough for future modeling without implying that a mesh exists. This register therefore separates:

- `CandidateGeometryReady` — current source is sufficient to construct a non-shipping 3D candidate without choosing missing canon;
- `ReferenceOnly` — source can guide design/reference work but one complete/final mesh would collapse an authored ambiguity or choose an unlocked visual fact;
- `Blocked` — current authority explicitly prevents full candidate production.

All three states remain below actual model production, Unreal import and runtime acceptance.

## Current finite register

| Family | Records | Candidate geometry ready | Reference-only | Blocked |
| --- | ---: | ---: | ---: | ---: |
| Major characters | 9 | 5 | 1 | 3 |
| Legacy/deep-dive boss visuals | 21 | 19 | 0 | 2 |
| Current Tier-1 bosses — The Nine Who Hold | 9 | 0 | 0 | 9 |
| La Liberación | 6 | 5 | 0 | 1 |
| Fauna | 24 | 16 | 3 | 5 |
| Flora | 9 | 7 | 1 | 1 |
| State Treasures + current named unique reward | 10 | 1 | 9 | 0 |
| Grounded named-dungeon modular kits | 40 | 40 | 0 | 0 |
| Highmoore named-anchor modular kits | 12 | 12 | 0 | 0 |
| **Total** | **140** | **105** | **14** | **21** |

Native authority: `Source/DarkArisen/Production/ThreeDAssetReadinessCatalog.h/.cpp`.

Automation contract: `Source/DarkArisen/Tests/ThreeDAssetReadinessSpec.cpp`.

## Important boundaries

### Characters

The five core crew members Mira, Big Tom, Ines, Father Salvio and Esteban have enough physical + wardrobe source for non-shipping rigged candidates. Jake's body/proportion reference is grounded, but his complete canonical primary costume is not; his full rig remains `ReferenceOnly`. Elowen is blocked by the missing physical sheet. Ethan and Draven remain blocked by the Phase-11 story-authority conflict recorded in `Docs/DesignAuthority.md`.

### Bosses

Nineteen older/deep-dive boss visuals are detailed enough for non-shipping sculpt/rig candidates in their own categories. They do **not** define the current Tier-1 count or silently restore superseded story branches.

The authoritative current Tier-1 set is the Nine Who Hold. All nine gameplay identities are tracked, but all nine remain full-rig `Blocked` because their current Phase-7 sources lock role, weapons, arena and performance without supplying complete physical look sheets. A future art pass must not borrow unrelated legacy boss faces to close those nine gaps.

### La Liberación

Five deck/interior kits are source-ready as modular environment candidates. The exterior hull is blocked because the current source does not lock final hull class, overall dimensions, mast/sail plan, color scheme or figurehead. No provider/modeler may choose those details merely to make the ship look finished.

### Fauna

Most individually described legendary animals can become non-shipping creature-mesh candidates. Three records remain `ReferenceOnly` for complete 3D:

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

## Higgsfield boundary

Higgsfield is not the owner of the 140 records above. Current operator direction reserves Higgsfield for animation/performance/cinematic previs. These 3D records remain source packages for a later verified modeler/3D pipeline and Unreal import stage.

## Evidence ladder

For every future 3D record:

`Source readiness` → `model/provider candidate generated` → `Unreal ImportedForReview` → `RuntimeAccepted`.

No source-ready record currently skips the first arrow. No generated candidate may be called a final game asset before real Unreal-side review.
