# World + Rewards + Encounter source completion boundary

**Date:** 2026-08-29  
**Base:** `develop` at `d9f268ddf4dee30160d4f50e520fbba241d50adf`  
**Branch / PR:** `feature/world-rewards-encounters-production` / replacement PR #17 (former Draft PR #16) → `develop`  
**Evidence class:** native source contracts, static repository gates and Unreal Automation source specs only. UE 5.5 compile/runtime/asset acceptance remains unexecuted while required self-hosted runners/editor evidence is unavailable.

## Purpose

This tranche continues the Rockstar-path production line after the Animation + Presentation source merge. It closes source-authority gaps that were already answered by design documents, strengthens cross-system integration, and leaves every genuinely missing production fact explicit instead of filling it with generic content.

## Treasure and physical evidence

`treasure system.md` already supplies concrete data that the previous placeholder implementation had not transcribed. The native reward authority now records:

- all five treasure classes: Coin, Material, Document, Map, Object;
- the approximate corpus targets 15 / 30 / 20 / 15 / 20 percent without pretending those percentages assign an unauthored class to each missing treasure;
- approximately forty living-community artifacts that carry the sell / keep / return question as a locked corpus total;
- sixteen buried-hoard chains split exactly twelve archipelago + four Highmoore, while leaving unnamed chains unwritten;
- all nine state treasures as authored documents with their castle of origin:
  1. The Conquest Archives, 1651 — La Ciudadela
  2. The Labor Ledgers — Fuerte Esperanza
  3. The 1846 Patrol Reports — Fuerte San Rafael
  4. The Bribe Ledgers — Castillo Dorado
  5. Vega's Eleven Years — Puesto del Norte
  6. The Master Ledger — Coventry House
  7. Sterling's Correspondence — The Sterling Bastion
  8. The Four Thousand Contracts — Ashcroft Hall
  9. Thorne's Dispatches — Fort Resolute
- Sterling's source-specific taken-versus-voluntary-handover distinction remains documented rather than flattened into generic loot;
- the Crystal Katana remains the currently grounded named unique weapon reward.

No random loot table, rarity-colour tier, treasure-found counter or returned-count UI is introduced.

`UTreasureStateComponent` adds hidden persistence for sell / keep / return decisions without taking over inventory, economy, Alliance/network or secret-site authority. Returning a counted artifact yields the source-authored +5 network-strength delta for the real network owner to apply. The exact cumulative count has no public/UI getter. The secret-site-facing output is only the boolean return-bias result used by the existing Harbour That Was First access evidence.

## Siege / reward integration

`UCastleSiegeComponent::RecordStateTreasureRecovered` no longer accepts an arbitrary non-empty ID. During an assault it resolves against the nine authored state treasures and also requires that the treasure's authored castle matches the active castle. A fake treasure or a real treasure at the wrong castle fails closed.

This is integration, not a claim that the physical documents, castle placements or acquisition scenes already exist as Unreal assets.

## Travel and world reachability laws

`FAuthoredTravelProductionCatalog` now gives one native production authority to the source-backed travel modes without moving actors itself:

- physical archipelago sea passage through the existing ship/SeaPassage authority;
- guide/trust-opened feared waters while still requiring physical sailing;
- first Crystal Caves continental passage: 90–120 real minutes and physical traversal;
- repeat Crystal Caves route: 4–6 real minutes and physical traversal;
- Highmoore stable relocation moves the horse only, never Jake;
- within-settlement travel remains walking.

Water instant travel, map-click movement, compass, mini-map and player-dot capability remain absent. Pairwise port-route distance/time data and actual streaming/world links remain explicit production gaps rather than receiving invented numbers.

## Highmoore world anchors

`FHighmooreWorldProductionCatalog` transcribes twelve currently concrete named anchors from `highmoore region.md`, `highmoore settlements.md` and narrow related sources:

- Arion and Belos castle skyline identities;
- Ashby Under Moor;
- Lynnwater;
- Halford Cross;
- Stonecroft;
- The Mere Villages;
- Coldbeck;
- Dunford;
- Wynne's Cross;
- Halford Cross Abbey;
- The Drover's Rest.

It also locks the world-scale reads the design actually specifies: at least six castles visible from the emergence shelf, an 8–10 minute on-foot switchback descent, and roughly 25 minutes end-to-end at a canter.

No coordinates are invented. No generic castle stands in for an unnamed lesser House. No question-mark discovery marker is introduced. Every runtime placement flag/path remains empty until reviewed Unreal placement exists.

## Alpha reconciliation

The Alpha source-readiness snapshot now tracks two additional families separately:

- nine state treasures: source 9/9, production assets 0/9;
- twelve Highmoore named world anchors: source 12/12, production placements 0/12.

The snapshot therefore expands from fourteen to sixteen tracked families while still failing closed on production completeness. This is deliberate: stronger source reconciliation must not make the Alpha look closer to runtime acceptance than the evidence supports.

## Native verification source added/strengthened

- `DarkArisenOps validate-rewards` now requires the treasure laws, exact state-treasure identities, hidden return-state contract, castle-origin integration and tests; it scans the reward source boundary for generic/random loot-system symbols.
- `DarkArisenOps validate-world` now requires travel and Highmoore world production catalogs/tests in addition to existing region/population/fauna checks.
- `DarkArisenOps validate-alpha-content` now requires the sixteen-family readiness snapshot including state treasures and Highmoore anchors.
- Unreal Automation source specs cover treasure return bias, one-time decisions, state-treasure castle matching, Crystal Caves travel times, horse-only stable relocation, absence of instant travel/navigation HUD features, and Highmoore anchor/scale laws.

These validators/tests are source evidence. They are **not reported as executed or passed** until the approved UE 5.5 self-hosted runners actually run them.

## Deliberately open after this merge

- real `.umap` region/settlement/castle/dungeon production and coordinates;
- real state-treasure document meshes/materials/readable-document presentation and castle placement;
- all unnamed return-question artifacts and buried-hoard identities not supplied by governing source;
- actual sea lanes, port route floors, stable actors and streaming links;
- the remaining 275 unauthored Turn/Standing identities;
- remaining source-backed main-story/mission integration where concrete design still exists;
- final encounter AI, animation, audio and presentation assets;
- UE 5.5 Windows/Linux compilation and Unreal Automation;
- runtime persistence, 60 fps, packaging, start-to-credits and private Alpha acceptance.

The merge boundary means: **the source now knows more precisely what the world must contain and how authored evidence connects; it still does not pretend the Unreal world has been built.**
