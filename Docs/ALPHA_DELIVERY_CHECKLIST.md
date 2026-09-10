# Dark Arisen Alpha delivery checklist

**Active source branch:** `feature/m5-m6-source-completion` / PR #8
**Merged source history:** PR #4 → `develop`; PR #5 → `main`; PR #7 → `develop` on 2026-08-28. None of these source merges proves the still-missing runtime or Alpha gates.
**Engine:** Unreal Engine 5.8
**Current runtime direction:** Flo directed on 2026-08-28 that unavailable UE 5.8 runtime/runner gates should not block further source work. They remain mandatory later acceptance gates and are not treated as passed.
**Artifact definition:** the first Alpha is the complete private UE 5.8 game plus two separately verified, content-identical Shipping artifacts: `DarkArisen-Alpha-Windows-Arcware.zip` and `DarkArisen-Alpha-Linux-x86_64.zip`. The exact source archive accompanies them. A scaffold, greybox, vertical slice, streaming configuration or untested candidate is not an Alpha. Native PS5 work is deferred to Beta and is not an Alpha deliverable.

This file is the progress index. Detailed acceptance criteria remain authoritative in `M0_CHECKLIST.md` and `Docs/Handoff/ENGINEERING_HANDOFF.md`. **A checkbox is marked only when its required source and runtime evidence both exist.** Source-only work is recorded as prose under the relevant milestone and must not be read as milestone acceptance. M3/M4 source closure is recorded in `Docs/M3_M4_SOURCE_COMPLETION.md`; M5/M6 source closure is recorded in `Docs/M5_M6_SOURCE_COMPLETION.md`.

## Non-negotiable delivery gates

- [ ] Windows and Linux Development/Shipping builds compile in UE 5.8 with zero game-module warnings.
- [ ] Python and Unreal automation suites pass on the exact delivery commit.
- [ ] Frame-time evidence meets the 60 fps floor in every milestone benchmark.
- [ ] The design-law audit passes and the combat HUD still contains posture only.
- [ ] Flo completes the private iPad/Pixel Streaming playtest.
- [ ] The source project and both packaged targets are archived privately and every checksum is recorded.
- [ ] The complete content manifest is reconciled against the design source register.
- [ ] Flo explicitly approves the Alpha candidate after the first full-game playthrough.
- [ ] Completing the main story enters a full credits sequence with an approved non-reactive music cue and complete third-party/tool/music attribution.

## M0 — Foundation

- [x] UE 5.8 C++ project, targets, configs, Git LFS and native greybox exist.
- [x] Design laws, deterministic validators, frame telemetry and private runner workflow exist.
- [x] Private one-player Pixel Streaming deployment path and idle deallocation are implemented in source.
- [ ] Windows and Linux UE 5.8 compile, Unreal tests and in-engine smoke evidence.
- [ ] Flo/operator-selected GPU host with any cost explicitly approved outside the repository.
- [ ] Flo/operator-created private iPad link plus NVENC, TURN, latency and shutdown evidence.

## M1 — Core loop

- [x] Keyboard, gamepad and two-zone touch movement/combat inputs, including unlock-gated Rache activation.
- [x] Committed light/heavy/parry/dodge actions with no cancelling and locked six-frame deflection.
- [x] Five posture states, four wound layers and Crystal Katana animation exception.
- [x] Three-second Rally model and posture-only native HUD.
- [x] Startup-timed melee trace, health/posture resolution, deflection and Rally-on-contact.
- [x] One greybox duelling enemy with pursuit, committed attacks, posture and death.
- [ ] Authored montages and animation-notify timing.
- [x] Lock-on with living-combatant, 20 m acquisition/25 m retention leash, facing and line-of-sight validation; no marker/HUD.
- [x] Animation-ready wound presentation profile and wound-only low-frequency camera drift.
- [x] Native montage contact notify with exactly-once hit consumption and greybox fallback.
- [x] Native animation-state bridge plus light/heavy/parry/dodge/backstep montage slots; a running attack montage owns contact timing and a missing notify fails closed.
- [ ] Authored wound-aware locomotion animation assets and final feel tuning.
- [x] Physical greybox door/pickup, held Examine presentation and four-second corner prompt.
- [x] Stable-ID interaction SaveGame snapshot capture/restore without an unauthorised disk write.
- [ ] Authored interaction animation assets and in-engine persistence/presentation evidence.
- [ ] Five-minute encounter at measured 60 fps with controller/touch evidence.

## M2 — Vertical slice

- [ ] Rexa jungle region slice, settlement, Cenote dungeon and required traversal.
- [x] Exact native forty-person Las Raíces roster, locked 16/12/8/4 census, authored purpose anchors, fail-closed spawn boundary and protected-child damage/physics exclusions.
- [x] Native settlement-scoped anchor registry and AI path-request boundary with missing/duplicate/unsheltered-midday failures and no competing world clock.
- [x] Sparse combat-proximity delivery plus exact-50 m child flight, fail-safe offscreen sheltering, protected restoration and zero damage/launch/physics/lock-on source enforcement.
- [ ] Authored settlement level/anchor placement, observed schedule/child-safety navigation, four ambient-dialogue layers, adult/population reactions and forty-resident runtime proof.
- [x] Native 30/60/90-second breath tiers, equipment exception and HUD-free drowning damage path.
- [x] Native Cenote state for lip entry, Rexan water routing, exact-hour image gating, Keeper outcome and mandatory Return dependency.
- [ ] Authored Cenote level, water devices/currents/swimming, green-gold lighting, Keeper creature/AI/animation, offerings decision and no-marker/no-music runtime proof.
- [ ] Isabel Cruz boss with killed/spared/avoided outcomes and four-second death hold.
- [x] Native Isabel Cruz state with locked Duty/List/1846 thresholds, fail-closed mercy/avoidance conditions, distinct non-hostile resolution and the existing four-second hold path.
- [x] HUD-free native heat pressure with authored cistern/shade/exposed states; provisional numeric rates remain marked `DESIGN-GAP`.
- [ ] Authored Fuerte San Rafael arena, heat volumes, routes, combat/dialogue animation, cabinet/list/report rewards and three-resolution persistence evidence.
- [x] Native markerless quest state with all six activation modes, silent activation, fallible chronological journal data, internal expiry outcomes and mutual exclusion.
- [x] Four finite authored Rexa mission definitions, multi-voice local directions and physical chronological-notebook source boundary.
- [ ] Physical held journal presentation, local-direction dialogue, three authored Turns and one authored Standing mission.
- [ ] Two-hour evidence run satisfying the M2 merge gate.

**Current source-only continuation:** Las Raíces consumes the canonical M4 world-time subsystem for schedule updates. This is integration source, not observed navigation/runtime evidence.

## M3 — Ship

- [ ] Walkable four-deck *La Liberación*, sailing, wind, physical map and navigation.
- [ ] Five named scheduled crew members, morale readability, great cabin, rest and autosave.
- [ ] Real sea-passage gate with no fast travel or loading transition.

**M3 source closure in PR #7:**

- `ALaLiberacionShip` is the singular level-placeable ship boundary with Weather / Upper / Mid / Hold roots on one actor.
- `UShipVoyageComponent` owns wind, Point-of-Sail, heading, speed and swept physical actor movement; leaving the helm does not freeze the vessel.
- Handling depends on First Mate + active hands only. Other named crew do not leak into manoeuvre quality; Father Salvio has no mechanical handling effect.
- Exactly five canonical named crew exist plus the 40–90 hands boundary. `UShipHouseholdComponent` adds canonical-clock schedules, meals, up to eight specialists, twelve semi-named hands and permanent named-loss propagation.
- Morale has one private authority and is exposed only as qualitative world cues: singing, mending, galley fire, argument, empty forecastle at dusk or empty hammock after loss.
- Physical chart acquisition/annotation exists and `APhysicalMapActor` is a held Examine object containing only sailed coastlines, visited settlements, routes actually given and persistent handwritten notes.
- There is no Compass, minimap, player dot, map-click movement or water-fast-travel API.
- `USeaPassageComponent` requires both canonical elapsed time and physically observed ship distance before completion; it contains no level-open, server-travel, teleport or actor-location write.
- Great-Cabin rest routes through the M4 rest authority and supports Dawn/Midday/Dusk/Night only.
- **Why the M3 checkboxes remain open:** final authored hull/interior, continuous player traversal keel→maintop, buoyancy/roll/pitch/heel, ocean/weather coupling, final animation/audio and measured UE sea-passage evidence are runtime/content acceptance, not source assertions.

## M4 — Systems

- [ ] Three-axis 68-node progression, 23 teachers and no respec/conversion.
- [ ] Three-currency economy, sinks, time/save rules and social greeting/listening systems.
- [ ] Named-person technique-learning scene proving the M4 gate.

**M4 source closure in PR #7:**

- Jake owns the native BODY / CRAFT / STANDING progression/economy component. BODY ceilings and unique authored object/act consumption are enforced.
- CRAFT locks the exact structural contract: 68 nodes, 16/12/14/13/13 branches, 94 available Marks, 141 full-tree cost, 23 canonical teachers, 23 teacher-gated nodes and 11 Standing-gated nodes.
- Nodes support up to three prerequisites. Meeting a teacher is insufficient; the exact named-person/node teaching scene must be completed before Marks can learn a teacher-gated technique.
- The bible does not individually name all 68 abilities. `USkillTreeCatalogDataAsset` validates the complete authored data set without inventing filler nodes; every count, cost, teacher, Standing gate and prerequisite must resolve before the catalog is accepted.
- No Respec API exists and no money path buys BODY, CRAFT or STANDING.
- Doubloons, Pounds and Silver Marks are separate balances with no generic conversion API.
- `FChapterLedger` resolves holding income, army upkeep, garrison upkeep and construction draw once per chapter; same-chapter compounding is rejected. Legendary work, Silver reconstruction and crew share are explicit sinks.
- `UDarkArisenWorldRulesSubsystem` owns the only world clock at 150 real seconds per in-game hour. Rest is limited to Great Cabin / Safe House and Dawn / Midday / Dusk / Night.
- Autosave requests remain exactly Rest or Chapter Boundary; Lake→Dock suppresses autosaves and manual save remains allowed.
- Stranger/Known/Owed/Wary, listening/interruption/overhear, sitting and multi-chapter Wary memory exist without affinity or relationship UI. Drawing a weapon interrupts passive social state; no antagonise or systemic romance layer is added.
- Native automation specs cover teaching/prerequisite gates, chapter non-compounding, qualitative social memory, daypart rest suppression and a synthetic exact 68-node structural catalog.
- **Why the M4 checkboxes remain open:** the production 68-node Data Asset must still be authored only from canon-approved definitions, teaching scenes and social/economy presentation must exist in UE, and runtime persistence/playtest evidence remains unexecuted.

## M5 — Colonial war

- [ ] Hidden regional war simulation, colonies, retaliation and hostage selection.
- [ ] Castles, sieges, armies, battles and ten-chapter simulation evidence.

**M5 source closure in PR #8:**

- `UColonialWarStateSubsystem` owns hidden Imperial / Albion / Liberation / Crimson regional state and Alpha/Beta/Gamma chapter momentum. Player-facing reads are qualitative outcomes rather than numeric control meters.
- Support / Break / Liberate are reported only after physical authored actions. Crisis occurs below the 30-control threshold; Fall requires a separately reported assault; Liberation requires Liberation Strength ≥ 50 after Fall, otherwise Crimson occupation or Vacuum may result.
- Authored once-per-region-per-chapter autonomous ticks make the war move without Jake and reject duplicate chapter ticks.
- `URetaliationSubsystem` keeps Imperial and Albion Heat separate with exact Letter/Bounty/Counter-Attack/Hostage/Scorched-Earth thresholds, chapter caps and passive decay.
- Hostage selection excludes children and explicit narrative-protected people. By canon precedence the newer Phase-12 crew law wins: adult named crew are eligible unless individually protected.
- `UCastleSiegeComponent` implements Approach / Investment / Breach / Assault and Wall / Back / Inside, permits a blind assault, supports defensive sieges and preserves structural breach damage into ownership.
- `UArmyCampaignComponent` uses named/cultural force sources, exact 14/4 doubloon upkeep anchors, cavalry ×3, specialists ×2, hired/company desertion rules and the exact HOLD / PRESS / BREAK OFF vocabulary. Alliance forces reject Jake's orders; crew are not counted as an army.
- `ULargeBattleComponent` keeps the battle active after Jake falls; authored world/force simulation resolves the outcome later.
- Native specs exercise Chapter 4–10 war movement, duplicate-tick rejection, retaliation caps, child/crew hostage law, siege persistence, 500-man/7,000-doubloon upkeep, officer command capacity, desertion and battle continuation.
- **Why the M5 checkboxes remain open:** final colony/holding/castle levels, actual siege gameplay, army AI/camps, retaliation missions, battle-scale performance and full UE campaign simulation evidence remain unexecuted.

## M6 — Highmoore

- [ ] Crystal Caves, Guardian, Katana, mounted travel and two authored rides.
- [ ] Princess questline and suppressed-autosave window.
- [ ] Locked arrow moment verified with no slow motion, cue change or camera move.

**M6 source closure in PR #8:**

- `UCrystalCavesPassageComponent` requires observed physical movement plus at least 90 real minutes for the first passage; the permanent repeat route requires an authored 4–6 real minutes. No instant-travel path exists in this boundary.
- `UCrystalGuardianComponent` begins dormant, does not attack first, consumes explicit light stimuli instead of body/player targets, uses the locked four health phases, can be bypassed and settles for exactly four seconds on defeat.
- Guardian bypass deliberately yields no Crystal Katana.
- `UCrystalKatanaComponent` locks 74 cm / 1.1 kg, armour-ignore and condition-independent stamina-cost behavior. It contains no durability, repair, sharpening or upgrade system.
- `UHighmooreHorseComponent` implements bond 0–3, four gaits, qualitative mood, permanent death and the authored ride contracts: Lake 9 min/Trot ceiling; Arion 4 min/Gallop with stamina suspension; Belos 6 min continuing it; Return about 40 min/Walk with lasting exhaustion.
- `UPrincessQuestStateComponent` begins the canonical M4 Lake→Dock autosave suppression at the false letter and ends it only at the dock. It preserves the unmarked west turn and distinct Katana/no-Katana Belos outcomes.
- Arrow-law capabilities are hard false for slow motion, cue change, camera move, telegraph and revive; player control remains true. Arion children and noncombatants are never damage-valid.
- No systemic romance or magic system was added, and the Belos undercroft remains unrendered/unimplemented as required.
- **Why the M6 checkboxes remain open:** authored caves/minecart/puzzles, Guardian/Katana assets, horse locomotion/physics, Highmoore locations, Princess dialogue/staging/audio, exact measured rides/return and full runtime persistence evidence remain unexecuted.

## M7 — Content scale-out

- [ ] Remaining regions and world traversal.
- [ ] 61 dungeons, 17 Threads, 132 Turns and 147 mission variants reconciled and complete.
- [ ] Nine Tier 1 bosses plus the authoritative reconciled boss register.
- [ ] Assassin network, reconstruction, NPC populations, fauna and remaining authored content.
- [ ] All 19 authored cutscenes and all 22 playable moments implemented.
- [ ] Dialogue lock, casting matrix, pronunciation guide and subtitle pass complete.
- [ ] AI Voice Generator confirms zero cost and acceptable commercial usage rights before generation; any charge or unclear term requires Flo's explicit approval.
- [ ] Distinct human-sounding voice assets generated, reviewed and integrated.
- [ ] End-credits names, ordering, music composition/source, licence and cost are approved; no cue is invented or purchased without Flo.

## M8 — Platform, packaging and Alpha sign-off

- [ ] Windows platform integrations, achievements, cloud saves and controller configuration.
- [ ] Linux x86_64 platform integration, controller configuration, saves and parity with Windows.
- [ ] Accessibility, remapping, subtitle sizing and permitted difficulty settings.
- [ ] Full regression, performance, save migration, privacy and content-completeness passes.
- [ ] Private UE 5.8 source-project ZIP and checksum delivered to Flo.
- [ ] Private Windows Shipping candidate and checksum produced from the exact accepted commit; it contains no secrets, server credentials or public-link configuration.
- [ ] Private Linux x86_64 Shipping candidate and checksum produced from the same commit and complete content manifest.
- [ ] Complete native Windows and Linux start-to-credits playthroughs pass against the recorded candidate hashes.
- [ ] Flo imports/deploys the Windows ZIP on his chosen GPU service and privately completes the browser-link access-denial test; the link is never published in the repository.
- [ ] Acceptance evidence records every gate and Flo's approval; the immutable candidates are promoted to `DarkArisen-Alpha-Windows-Arcware.zip` and `DarkArisen-Alpha-Linux-x86_64.zip` with unchanged hashes.

## Post-Alpha / Beta — native PS5

- [ ] PlayStation Partner registration/GDPA, authorised Sony tools and PS5 dev/test-kit access exist outside the repository.
- [ ] Native PS5 work, DualSense features, TRC and certification are implemented against the accepted Alpha content baseline.
- [ ] A native PS5 Beta package is built and tested in the authorised PlayStation environment, then delivered privately in the platform-permitted form with a checksum. It is never represented by an Arcware stream or converted PC ZIP.

## Current blockers outside source authoring

- Matching private Windows/Linux UE 5.8 self-hosted jobs remain the compile/runtime path. Per Flo's current direction their unavailability does not stop source integration, but it still blocks runtime acceptance.
- The active workspace has no licensed UE 5.8 installation or Windows environment; local UE compile/playtest evidence cannot be created here.
- Flo will handle any GPU/private browser link. No paid host/runtime evidence is implied by PR #8.
- The AI Voice Generator is expected to be free, but no generation may begin before dialogue lock plus zero-cost and commercial-rights verification.
- The repository contains no canonical credits list or approved end-credits music cue yet. The cue, rights and any cost remain an explicit approval gate.

PR #8 is the active M5/M6 **source-integration** boundary. Its eventual merge must not turn any unchecked runtime/content checkbox above into a pass.
