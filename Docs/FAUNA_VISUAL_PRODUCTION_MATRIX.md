# Fauna visual production matrix

**Branch:** `feature/pre-runner-asset-production`  
**Evidence level:** source authority / `RequirementOnly` only.  
**Rule:** Phase-4 target counts are not permission to invent missing species, variants or final models.

## Current bounded visual set

| Family | Indexed | Provider-ready | Provider-blocked | Notes |
| --- | ---: | ---: | ---: | --- |
| Individually described legendary fauna | 19 | 18 | 1 | Final Wolf is save-history dependent and cannot have one static canonical appearance |
| Highmoore MVP fauna | 5 | 1 | 4 | Red deer is species-bounded; grouse, hare, fox and Fell Wolf lack a locked exact visual variant |
| **Total** | **24** | **19** | **5** | No external media or UE asset is claimed |

Native authority: `Source/DarkArisen/Production/FaunaVisualProductionCatalog.h/.cpp`.

Automation contract: `Source/DarkArisen/Tests/FaunaVisualProductionSpec.cpp`.

## Legendary creature set

The fauna catalog indexes source-defined visual identities for:

- The Patriarch;
- Ghost Reindeer;
- The Ash-Shadow;
- The Emperor's Stag;
- Old Iron-Back;
- Memory Stag;
- Threshold Fox;
- Final Wolf;
- Scarred Leviathan;
- Bone Ghost;
- Kraken;
- Deep One;
- Emperor of the Wind;
- Old Messenger;
- Silver Albatross;
- Jungle Phoenix;
- Blood Moth;
- Ancient Tortoise;
- Phoenix Spider.

The Ghost Ship is deliberately not counted as fauna. Plant/tree legendaries belong to flora/world production rather than this catalog.

## Fail-closed visual laws

### Final Wolf

Its appearance changes to wolves killed in the individual player's save. A single provider-generated face/coat would destroy the authored rule. It remains provider-blocked until the eventual runtime representation can consume player-history evidence.

### Deep One

The source never reveals the full creature. Provider reference work may show only the authored fragments: a massive tentacle, a single eye and an impossibly large shadow. No final size, limb count or body plan may be generated.

### Highmoore

Highmoore's newer authority requires grouse, hare, red deer, fox and Fell Wolf as MVP ecology. It does **not** lock exact species/coat/silhouette variants for grouse, hare, fox or Fell Wolf, so those four remain provider-blocked rather than becoming generic AI animal designs. Red deer may receive a natural species-level ecology reference only; it is not a legendary or individually named animal.

## Phase-4 scale boundary

`docs/design/fauna_overview.md` contains broad production targets such as 40+ mammal species, 60+ fish species and roughly 280 total species. Those are scale targets. The current tranche does not synthesize missing animals to reach the number. Additional fauna enters the production matrix only when its source record is individually grounded enough to preserve visual identity without invention.

## Evidence boundary

All 24 entries are source records only. Provider-ready means a bounded concept/reference request can be prepared. It does **not** mean:

- a Higgsfield generation exists;
- a 3D mesh exists;
- a rig or animation exists;
- Unreal import exists;
- AI/spawn/ecosystem runtime exists;
- performance or Alpha acceptance exists.

Those remain later evidence gates.
