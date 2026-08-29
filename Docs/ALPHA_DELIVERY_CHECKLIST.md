# Dark Arisen Alpha delivery checklist

**Active source branch:** `feature/pre-runner-asset-production` / Draft PR #18  
**Integrated develop baseline:** `04a34e37f4f0c2f982cf28b942e11e120d103d29` after PR #17  
**Merged source history relevant to current production:** PR #7 (M3/M4), PR #8 (M5/M6), PR #9 (M7/M8 source contracts/native ops), PR #15 (authored content + Animation/Presentation source), PR #17 (world travel + treasures + encounter integration). None of these source merges proves the still-missing runtime or Alpha gates.  
**Engine:** Unreal Engine 5.5  
**Current runtime direction:** unavailable UE 5.5 runners do not block further source/content-authority work. They remain mandatory acceptance gates and are not treated as passed. Current Windows/Linux workflow jobs can be created but remain queued when no matching self-hosted runner is assigned.  
**Artifact definition:** the first Alpha is the complete private UE 5.5 game plus two separately verified, content-identical Shipping artifacts: `DarkArisen-Alpha-Windows-Arcware.zip` and `DarkArisen-Alpha-Linux-x86_64.zip`. The exact source archive accompanies them. A scaffold, greybox, source contract, external-media preview, streaming configuration or untested candidate is not an Alpha. Native PS5 work is deferred to Beta and is not an Alpha deliverable.

This file is the progress index. Detailed acceptance criteria remain authoritative in `M0_CHECKLIST.md`, `Docs/Handoff/ENGINEERING_HANDOFF.md` and the newest subsystem completion records. **A checkbox is marked only when its required source and runtime/production evidence both exist.** Source-only work is recorded as prose under the relevant milestone and must not be read as milestone acceptance.

## Non-negotiable delivery gates

- [ ] Windows and Linux Development/Shipping builds compile in UE 5.5 with zero game-module warnings.
- [ ] Native `DarkArisenOps` validation and Unreal Automation suites execute and pass on the exact delivery commit.
- [ ] Frame-time evidence meets the 60 fps floor in every milestone benchmark.
- [ ] The design-law audit passes and the combat HUD still contains posture only.
- [ ] Flo completes the private iPad/Pixel Streaming playtest.
- [ ] The source project and both packaged targets are archived privately and every checksum is recorded.
- [ ] The complete content manifest is reconciled against the design source register.
- [ ] Flo explicitly approves the Alpha candidate after the first full-game playthrough.
- [ ] Completing the main story enters a full credits sequence with an approved non-reactive music cue and complete third-party/tool/music attribution.

## M0 — Foundation

- [x] UE 5.5 C++ project, targets, configs, Git LFS and native greybox exist.
- [x] Design laws, deterministic validators, frame telemetry and private runner workflow exist.
- [x] Private one-player Pixel Streaming deployment path and idle deallocation are implemented in source.
- [ ] Windows and Linux UE 5.5 compile, Unreal tests and in-engine smoke evidence.
- [ ] Flo/operator-selected GPU host with any cost explicitly approved outside the repository.
- [ ] Flo/operator-created private iPad link plus NVENC, TURN, latency and shutdown evidence.

## M1 — Core loop

- [x] Keyboard, gamepad and two-zone touch movement/combat inputs, including unlock-gated Rache activation.
- [x] Committed light/heavy/parry/dodge actions with no cancelling and locked six-frame deflection.
- [x] Five posture states, four wound layers and Crystal Katana animation exception.
- [x] Three-second Rally model and posture-only native HUD.
- [x] Startup-timed melee trace, health/posture resolution, deflection and Rally-on-contact.
- [x] One greybox duelling enemy with pursuit, committed attacks, posture and death.
- [ ] Authored montages and animation-notify timing accepted in UE.
- [x] Lock-on with living-combatant, 20 m acquisition/25 m retention leash, facing and line-of-sight validation; no marker/HUD.
- [x] Animation-ready wound presentation profile and wound-only low-frequency camera drift.
- [x] Native montage contact notify with exactly-once hit consumption and greybox fallback.
- [x] Native animation-state bridge plus light/heavy/parry/dodge/backstep montage slots; a running attack montage owns contact timing and a missing notify fails closed.
- [ ] Authored wound-aware locomotion animation assets and final feel tuning accepted in UE.
- [x] Physical greybox door/pickup, held Examine presentation and four-second corner prompt.
- [x] Stable-ID interaction SaveGame snapshot capture/restore without an unauthorised disk write.
- [ ] Authored interaction animation assets and in-engine persistence/presentation evidence.
- [ ] Five-minute encounter at measured 60 fps with controller/touch evidence.

**Current source-only continuation:** PR #15 established the finite Animation Production authority: eleven named requirements, exact five weapon timing classes, eight-frame readability floor, exact six-frame deflection, committed recovery, wound layers, horse/carry/traversal requirements and Crystal Katana clean-set exception. External video/previs output in PR #18 may inform production but cannot satisfy the UE animation checkboxes above without real import, retarget/Montage/Notify ownership and review.

## M2 — Vertical slice

- [ ] Rexa jungle region slice, settlement, Cenote dungeon and required traversal.
- [x] Exact native forty-person Las Raíces roster, locked 16/12/8/4 census, authored purpose anchors, fail-closed spawn boundary and protected-child damage/physics exclusions.
- [x] Native settlement-scoped anchor registry and AI path-request boundary with missing/duplicate/unsheltered-midday failures and no competing world clock.
- [x] Sparse combat-proximity delivery plus exact-50 m child flight, fail-safe offscreen sheltering, protected restoration and zero damage/launch/physics/lock-on source enforcement.
- [ ] Authored settlement level/anchor placement, observed schedule/child-safety navigation, four ambient-dialogue layers, adult/population reactions and forty-resident runtime proof.
- [x] Native 30/60/90-second breath tiers, equipment exception and HUD-free drowning damage path.
- [x] Native Cenote state for lip entry, Rexan water routing, exact-hour image gating, Keeper outcome and mandatory Return dependency.
- [ ] Authored Cenote level, water devices/currents/swimming, green-gold lighting, Keeper creature/AI/animation, offerings decision and no-marker/no-music runtime proof.
- [ ] Isabel Cruz boss with killed/spared/avoided outcomes and four-second death hold accepted in runtime.
- [x] Native Isabel Cruz state with locked Duty/List/1846 thresholds, fail-closed mercy/avoidance conditions, distinct non-hostile resolution and the existing four-second hold path.
- [x] HUD-free native heat pressure with authored cistern/shade/exposed states; provisional numeric rates remain marked `DESIGN-GAP`.
- [ ] Authored Fuerte San Rafael arena, heat volumes, routes, combat/dialogue animation, cabinet/list/report rewards and three-resolution persistence evidence.
- [x] Native markerless quest state with all six activation modes, silent activation, fallible chronological journal data, internal expiry outcomes and mutual exclusion.
- [x] Four finite authored Rexa mission definitions, multi-voice local directions and physical chronological-notebook source boundary.
- [ ] Physical held journal presentation, local-direction dialogue, three authored Turns and one authored Standing mission accepted in runtime.
- [ ] Two-hour evidence run satisfying the M2 gate.

**Current source-only continuation:** Las Raíces consumes the canonical M4 world-time subsystem for schedule updates. PR #15/17 added broader authored world, dungeon, population/fauna, reward and travel authorities, but none is observed level/navigation/runtime evidence.

## M3 — Ship

- [ ] Walkable four-deck *La Liberación*, sailing, wind, physical map and navigation accepted in UE.
- [ ] Five named scheduled crew members, morale readability, great cabin, rest and autosave accepted in UE.
- [ ] Real sea-passage gate with no fast travel or loading transition accepted in runtime.

**M3 source closure in PR #7:**

- `ALaLiberacionShip` is the singular level-placeable ship boundary with Weather / Upper / Mid / Hold roots on one actor.
- `UShipVoyageComponent` owns wind, Point-of-Sail, heading, speed and swept physical actor movement; leaving the helm does not freeze the vessel.
- Handling depends on First Mate + active hands only. Other named crew do not leak into manoeuvre quality; Father Salvio has no mechanical handling effect.
- Exactly five canonical named crew exist plus the 40–90 hands boundary. `UShipHouseholdComponent` adds canonical-clock schedules, meals, up to eight specialists, twelve semi-named hands and permanent named-loss propagation.
- Morale has one private authority and is exposed only as qualitative world cues.
- Physical chart acquisition/annotation exists and `APhysicalMapActor` is a held Examine object containing only sailed coastlines, visited settlements, routes actually given and persistent handwritten notes.
- No compass, minimap, player dot, map-click movement or water instant-travel capability is permitted.
- `USeaPassageComponent` requires both canonical elapsed time and physically observed ship distance before completion.
- Great-Cabin rest routes through the M4 rest authority and supports Dawn/Midday/Dusk/Night only.
- **Why the M3 checkboxes remain open:** final authored hull/interior, continuous player traversal keel→maintop, buoyancy/roll/pitch/heel, ocean/weather coupling, final animation/audio and measured UE sea-passage evidence remain production/runtime acceptance.

**PR #17 continuation:** `FAuthoredTravelProductionCatalog` now records physical sea passage and guide/trust-opened feared-water passage without introducing menu movement. Missing port-to-port floors/sea lanes remain explicit gaps.

## M4 — Systems

- [ ] Three-axis 68-node progression, 23 teachers and no respec/conversion accepted with production data/runtime evidence.
- [ ] Three-currency economy, sinks, time/save rules and social greeting/listening systems accepted in runtime.
- [ ] Named-person technique-learning scene proving the M4 gate.

**M4 source closure in PR #7:**

- Jake owns the native BODY / CRAFT / STANDING progression/economy component. BODY ceilings and unique authored object/act consumption are enforced.
- CRAFT locks the exact structural contract: 68 nodes, 16/12/14/13/13 branches, 94 available Marks, 141 full-tree cost, 23 canonical teachers, 23 teacher-gated nodes and 11 Standing-gated nodes.
- Nodes support up to three prerequisites. Meeting a teacher is insufficient; the exact named-person/node teaching scene must be completed before Marks can learn a teacher-gated technique.
- The bible does not individually name every ability. The catalog validates authored data without inventing filler nodes.
- No Respec API exists and no money path buys BODY, CRAFT or STANDING.
- Doubloons, Pounds and Silver Marks are separate balances with no generic conversion API.
- `FChapterLedger` resolves holding income, army upkeep, garrison upkeep and construction draw once per chapter; same-chapter compounding is rejected.
- `UDarkArisenWorldRulesSubsystem` owns the only world clock at 150 real seconds per in-game hour. Rest is limited to Great Cabin / Safe House and Dawn / Midday / Dusk / Night.
- Autosave requests remain exactly Rest or Chapter Boundary; Lake→Dock suppresses autosaves and manual save remains allowed.
- Stranger/Known/Owed/Wary, listening/interruption/overhear, sitting and multi-chapter Wary memory exist without affinity or relationship UI.
- Native Automation specs cover teaching/prerequisite gates, chapter non-compounding, qualitative social memory, daypart rest suppression and structural catalog laws.
- **Why the M4 checkboxes remain open:** production data and authored teaching/social/economy presentation must exist in UE, and runtime persistence/playtest evidence remains unexecuted.

## M5 — Colonial war

- [ ] Hidden regional war simulation, colonies, retaliation and hostage selection accepted in runtime.
- [ ] Castles, sieges, armies, battles and ten-chapter simulation evidence.

**M5 source closure in PR #8:**

- `UColonialWarStateSubsystem` owns hidden Imperial / Albion / Liberation / Crimson regional state and Alpha/Beta/Gamma chapter momentum.
- Support / Break / Liberate are reported only after physical authored actions; autonomous ticks make the war move without Jake.
- `URetaliationSubsystem` keeps Imperial and Albion Heat separate and preserves authored escalation/decay laws.
- Hostage selection excludes children and explicit narrative-protected people; newer adult-crew precedence remains authoritative.
- `UCastleSiegeComponent` implements Approach / Investment / Breach / Assault and Wall / Back / Inside, supports defensive sieges and persists structural breach damage.
- `UArmyCampaignComponent` and `ULargeBattleComponent` retain source-authored campaign/battle laws including battle continuation after Jake falls.
- Native specs exist for the major state transitions, upkeep/command capacity, desertion and battle continuation.
- **Why the M5 checkboxes remain open:** final colony/holding/castle levels, actual siege gameplay, army AI/camps, retaliation missions, battle-scale performance and full UE campaign simulation evidence remain unexecuted.

**PR #17 continuation:** all nine State Treasures are now exact documents bound to their authored castles, and siege recovery fails closed for fake/wrong-castle IDs. Physical documents and castle placement remain production work.

## M6 — Highmoore

- [ ] Crystal Caves, Guardian, Katana, mounted travel and authored rides accepted in runtime.
- [ ] Princess questline and suppressed-autosave window accepted in runtime.
- [ ] Locked Arrow moment verified in engine with no slow motion, cue reaction or unauthorised camera behavior.

**M6 source closure in PR #8:**

- `UCrystalCavesPassageComponent` requires observed physical movement plus 90–120 real minutes for first passage; repeat passage remains 4–6 real minutes.
- `UCrystalGuardianComponent` begins dormant, does not attack first, uses light stimuli, can be bypassed and settles for exactly four seconds on defeat; bypass yields no Katana.
- `UCrystalKatanaComponent` preserves 74 cm / 1.1 kg and its locked combat/condition behavior.
- `UHighmooreHorseComponent` implements bond/gait/mood/death and authored ride contracts.
- `UPrincessQuestStateComponent` owns the Lake→Dock autosave suppression and locked Arrow/Arion protection laws.
- No systemic romance or magic system was added, and the Belos undercroft remains unrendered/unimplemented.
- **Why the M6 checkboxes remain open:** authored caves/minecart/puzzles, Guardian/Katana assets, horse locomotion/physics, Highmoore locations, Princess dialogue/staging/audio, measured rides and persistence evidence remain unexecuted.

**PR #17 continuation:** twelve Highmoore anchors and world-scale/readability requirements are source-grounded with zero fake coordinates/runtime placements.

## M7 — Content scale-out

- [ ] Remaining regions and world traversal implemented as real production levels/assets.
- [ ] 61 dungeons, 17 Threads, 132 Turns and 147 mission variants reconciled and complete.
- [ ] Nine Tier-1 bosses plus authoritative boss register implemented with production arenas/AI/assets.
- [ ] Assassin network, reconstruction, NPC populations, fauna and remaining authored content implemented in production world.
- [ ] All 19 authored cutscenes and all 22 protected playable moments implemented and reviewed in UE.
- [ ] Dialogue lock, casting matrix, pronunciation guide and subtitle pass complete.
- [ ] Voice provider cost/rights are explicitly accepted before Shipping generation; any charge or unclear term requires operator approval.
- [ ] Distinct human-sounding final voice assets generated, reviewed, imported and runtime-accepted.
- [ ] End-credits names, ordering, music composition/source, licence and cost are approved; no cue is invented or purchased without operator approval.

**Current source-only content reconciliation:**

- Dungeons: 41 named required / 40 grounded; 20 Minor/Tier-A required / 0 individual identities grounded; Crystal Caves separate and grounded.
- Threads: 17/17 identities source-grounded with runtime/source owners, while documented internal sub-gaps remain where source is withheld.
- Turns: 132 required / 3 individual identities grounded / **129 authoring gaps**.
- Standing: 147 required / 1 individual identity grounded / **146 authoring gaps**.
- Total deliberate mission identity gap: **275**. These slots must not receive AI-generated titles/NPCs/locations/premises/outcomes.
- Tier-1 commanders: 9/9 source-grounded; final arena/AI/animation/audio production remains absent.
- World regions: 8/8 production identities source-grounded; final region map packages are not evidenced.
- State Treasures: 9/9 source-grounded; production document assets/placement are not evidenced.
- Highmoore anchors: 12 source-grounded; production placement is not evidenced.
- Animation: 11 named requirements source-grounded; final imported animation set not evidenced.
- Presentation: 19 slots required / 14 cutscene identities currently grounded; final-act #15–#19 unresolved; all 22 protected playable moments are source-grounded.
- Current external Higgsfield attempt is provider-plan blocked before job creation and counts as **zero production assets**.

## M8 — Platform, packaging and Alpha sign-off

- [ ] Windows platform integrations, achievements, cloud saves and controller configuration.
- [ ] Linux x86_64 platform integration, controller configuration, saves and parity with Windows.
- [ ] Accessibility, remapping, subtitle sizing and permitted difficulty settings.
- [ ] Full regression, performance, save migration, privacy and content-completeness passes.
- [ ] Private UE 5.5 source-project ZIP and checksum delivered to Flo.
- [ ] Private Windows Shipping candidate and checksum produced from the exact accepted commit; it contains no secrets, server credentials or public-link configuration.
- [ ] Private Linux x86_64 Shipping candidate and checksum produced from the same commit and complete content manifest.
- [ ] Complete native Windows and Linux start-to-credits playthroughs pass against the recorded candidate hashes.
- [ ] Flo imports/deploys the Windows ZIP on his chosen approved GPU service and privately completes the browser-link access-denial test; the link is never published in the repository.
- [ ] Acceptance evidence records every gate and Flo's approval; immutable candidates are promoted to `DarkArisen-Alpha-Windows-Arcware.zip` and `DarkArisen-Alpha-Linux-x86_64.zip` with unchanged hashes.

## Post-Alpha / Beta — native PS5

- [ ] PlayStation Partner registration/GDPA, authorised Sony tools and PS5 dev/test-kit access exist outside the repository.
- [ ] Native PS5 work, DualSense features, TRC and certification are implemented against the accepted Alpha content baseline.
- [ ] A native PS5 Beta package is built and tested in the authorised PlayStation environment, then delivered privately in the platform-permitted form with a checksum. It is never represented by an Arcware stream or converted PC ZIP.

## Current blockers outside source authoring

- Matching private Windows/Linux UE 5.5 self-hosted runners remain the compile/runtime path. Workflow jobs may exist in `queued` state with no runner assigned; this blocks execution evidence but not source integration.
- The connected work environment currently supplies no accepted UE editor/runtime evidence for final maps, imported animation, Sequencer, navmesh, AI, audio integration, save/load or performance.
- Higgsfield account preflight on 2026-08-30 reported free plan / 10 credits, but attempted Seedance Mini jobs were rejected before job creation with `Requires basic plan or higher`. No upgrade or purchase is authorised; no generation credit was consumed by those failed submissions.
- External media, when later generated, remains Previs/Candidate evidence until actual Unreal import/review/runtime acceptance exists.
- Final AI Voice Generator work remains late-stage: dialogue/casting/pronunciation/subtitle lock and cost/commercial-rights acceptance precede Shipping voice production.
- The repository still lacks approved final end-credits music/licensing evidence.
- The largest pure-authorship blocker is the **275 missing finite mission identities**; do not generate them procedurally or with AI filler.

Draft PR #18 is the active **pre-runner source/asset-production-authority** boundary. Its eventual merge must not turn any unchecked runtime/content checkbox above into a pass.
