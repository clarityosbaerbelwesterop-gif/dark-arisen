# Engineering handover

**Updated:** 2026-08-29  
**Integrated baseline:** `develop` at `d9f268ddf4dee30160d4f50e520fbba241d50adf` after PR #15  
**Current source tranche:** `feature/world-rewards-encounters-production` / PR #16 → `develop`  
**Merge direction:** operator explicitly authorized review and merge of this tranche when coherent.  
**Runtime state:** UE 5.5 Windows/Linux compile, Unreal Automation, runtime, performance, packaging and playthrough gates remain deferred/unverified while matching self-hosted runners are offline/queued. Queued is not passed.

## Operator direction

Take the **Rockstar path**, not the shortest path. Continue building the actual game with authored specificity, physical-world readability, restrained presentation and technical integrity. Do not manufacture content merely to close counts or make an Alpha look closer.

Executable project implementation remains native C++. Required data/config/document/build formats remain their native formats. Do not invent missing canon; preserve `DESIGN-GAP` where approved source does not answer a production fact.

## Integrated history

- PR #7: M3 ship + M4 systems source closure.
- PR #8: M5 Colonial War + M6 Highmoore source closure.
- PR #9: M7/M8 source contracts + native-C++ project-operations migration.
- CI/governance repair tranche integrated on `develop` at `6e1620b...`.
- PR #15: Rockstar-path authored dungeon/world/population/fauna, all 17 Thread owners, fail-closed mission gaps, Tier-1 boss constraints, Animation + Presentation source boundary. Merge commit `d9f268ddf4dee30160d4f50e520fbba241d50adf`.
- Detailed prior boundaries: `Docs/M3_M4_SOURCE_COMPLETION.md`, `Docs/M5_M6_SOURCE_COMPLETION.md`, `Docs/M7_M8_SOURCE_PLAN.md`, `Docs/ANIMATION_PRESENTATION_SOURCE_COMPLETION.md`.

## Current PR #16 — World + Rewards + Encounter production

Detailed boundary: `Docs/WORLD_REWARDS_ENCOUNTERS_SOURCE_COMPLETION.md`.

### Treasure / physical evidence

- The five canonical treasure classes are explicit: Coin, Material, Document, Map, Object.
- Approximate corpus mix remains 15/30/20/15/20 percent without assigning missing item identities just to hit a percentage.
- All nine state treasures are now source-grounded documents with exact castle origins from `treasure system.md`:
  1. The Conquest Archives, 1651 — La Ciudadela
  2. The Labor Ledgers — Fuerte Esperanza
  3. The 1846 Patrol Reports — Fuerte San Rafael
  4. The Bribe Ledgers — Castillo Dorado
  5. Vega's Eleven Years — Puesto del Norte
  6. The Master Ledger — Coventry House
  7. Sterling's Correspondence — The Sterling Bastion
  8. The Four Thousand Contracts — Ashcroft Hall
  9. Thorne's Dispatches — Fort Resolute
- Sterling's taken-versus-voluntary-handover distinction remains source-specific.
- Approximately forty living-community artifacts remain the sell / keep / return corpus; the exact missing identities are not fabricated.
- Sixteen buried-hoard chains remain locked as twelve archipelago + four Highmoore; unnamed chains remain gaps.
- `UTreasureStateComponent` persists one-time sell/keep/return decisions. Returning a counted artifact yields the authored +5 network-strength delta for the actual Alliance/network owner to apply.
- The cumulative return number has no UI/public getter. Only the hidden return-bias boolean is exposed for the existing Harbour That Was First access evidence.
- Random loot tables, rarity colours, treasure-found counters and returned-count UI remain prohibited.

### Siege / reward integration

- `UCastleSiegeComponent::RecordStateTreasureRecovered` now resolves against the nine authored state treasures.
- A real treasure at the wrong castle fails closed; an arbitrary ID cannot become canon.
- This is source/runtime-state integration only. Physical document assets, castle placement and acquisition presentation are still real Unreal production work.

### Travel / reachability

- `FAuthoredTravelProductionCatalog` locks six source-backed travel modes without moving actors itself:
  - physical archipelago sea passage;
  - feared-water guide/trust route while still sailing physically;
  - first Crystal Caves passage, 90–120 real minutes;
  - repeat Crystal Caves passage, 4–6 real minutes;
  - Highmoore stable relocation moves the horse only;
  - within-settlement movement remains walking.
- No water instant travel, map-click movement, compass, mini-map or player dot.
- No pairwise sea-route time/distance table is invented where the design corpus does not provide one.
- Actual sea lanes, streaming links, stable actors and level traversal remain asset/runtime work.

### Highmoore world production

- Twelve current named anchors are source-grounded: Arion, Belos, Ashby Under Moor, Lynnwater, Halford Cross, Stonecroft, The Mere Villages, Coldbeck, Dunford, Wynne's Cross, Halford Cross Abbey and The Drover's Rest.
- Source scale remains: at least six castles visible from the emergence shelf; 8–10 minutes on foot down the switchback; roughly 25 minutes end-to-end at a canter.
- No coordinates are invented. No generic lesser-House castle is substituted. No question-mark discovery markers.
- Runtime placement flags/paths remain empty until reviewed Unreal assets exist.

### Alpha source reconciliation

- Readiness tracking expands from fourteen to sixteen families.
- State treasures: source 9/9, production assets 0/9.
- Highmoore named world anchors: source 12/12, production placements 0/12.
- Source completeness therefore improves without self-certifying production or runtime completion.

### Native source gates

- `DarkArisenOps validate-rewards`: exact treasure/state-treasure laws, hidden return-state contract, castle-origin matching, forbidden generic loot scan and native tests.
- `DarkArisenOps validate-world`: existing region/population/fauna coverage plus travel and Highmoore production contracts/tests.
- `DarkArisenOps validate-alpha-content`: sixteen-family readiness reconciliation.
- Unreal Automation source specs cover treasure decisions, castle-specific state treasures, travel law, Crystal timings, horse-only relocation and Highmoore anchors.
- None of these tests are claimed as executed until UE 5.5 runners actually run them.

## Existing major content state that remains authoritative

- 61-site dungeon structure is finite/non-radiant; forty grounded named regional/cross-network sites plus Crystal Caves separate; twenty Minor/Tier-A slots remain unfilled rather than receiving fabricated content.
- All 17 named Threads have source/runtime coverage owners.
- Turns remain 132 required / 3 individually grounded / 129 gaps.
- Standing remains 147 required / 1 individually grounded / 146 gaps.
- Total deliberate mission identity gap remains **275**.
- Nine Who Hold remain exactly Herrera, Reyes, Cruz, de Silva, Vega, Blackwood, Sterling, Ashcroft, Thorne.
- Eight production world-region identities remain registered, with zero falsely claimed final region maps.
- Highmoore population remains a separate fifth register with four classes and no invented percentages/reputation meter.
- Fauna/flora source scale and named ecological anchors remain as recorded in PR #15.
- Animation remains 11 named requirements plus exact weapon timing/readability/wound/horse/carry/traversal laws; no absent binary animation asset is claimed.
- Presentation remains exactly 19 cutscene slots, 22 protected playable moments, 5 inserts and 6 slow pushes. Final-act #15–#19 remain unresolved rather than invented.
- Final voices remain intentionally late; prior AI voice previews are auditions only and not Shipping/commercial-clearance evidence.

## Next Rockstar-path work after PR #16 merge

1. Branch from the exact new `develop` merge head.
2. Continue real world/region/settlement/dungeon reachability and authored placement contracts from concrete source.
3. Continue source-backed main-story/mission integration, but do not touch the 275 missing identities without approved design.
4. Continue named encounter/boss/dungeon dependencies and physical evidence integration.
5. Continue complete fauna/flora identity transcription only from the authored corpus; no random/generated ecosystem filler.
6. When editor/runtime access exists, build and review actual `.umap`/`.uasset`, animation, AnimGraph/Montage, Sequencer, facial/performance and audio assets against the source contracts.
7. Then finish music/credits rights and final dialogue/subtitle/pronunciation/casting/voice production.
8. Bring Windows/Linux UE 5.5 runners online and execute compile + Unreal Automation; fix any failures in a follow-up PR.
9. Only after actual assets/runtime exist: save/load, performance, accessibility/controller, start-to-credits, same-commit Windows/Linux Shipping, checksums, private Pixel Streaming access test and operator Alpha approval.

## Never claim without evidence

- UE compilation passed,
- Unreal Automation passed,
- a binary map/animation/audio/cinematic asset exists because a source contract exists,
- 60 fps passed,
- start-to-credits passed,
- voice/music usage is commercially cleared,
- Windows/Linux packages are Alpha-ready,
- the Alpha is accepted.
