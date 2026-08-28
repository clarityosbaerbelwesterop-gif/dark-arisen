# Engineering handover

**Updated:** 2026-08-28  
**Current work:** M2 backlog + M3/M4 source foundations; UE 5.5 runtime gates are deliberately deferred, not waived  
**Active PR:** #7 — `feature/m2-runtime-completion` → `develop`  
**Merge state:** PR #4 was merged to `develop` and PR #5 to `main` by the owner on 2026-08-28 while required runtime gates were still unexecuted. Those merges record source integration only. PR #7 remains Draft and is not M2, M3, M4, Alpha, or platform acceptance.

## Current direction

Flo explicitly directed on 2026-08-28 that the currently unavailable UE 5.5 Runtime/self-hosted-runner path should not block further source work. Source production therefore continues through M3 and M4 while runtime/compile/playtest evidence remains an open gate. No document, checkbox, PR or report may turn that deferral into a claim that the runtime gate passed.

## Completed in the active source line

- Converted the documentation-only repository into an Unreal Engine 5.5 C++ project boundary.
- Replaced the four isolated headers with matching implementations and corrected baseline HP/stamina.
- Added a native third-person pawn, camera, keyboard/gamepad input, two-zone touch input, sprint, damage/death, stamina, posture, attacks, dodge, six-frame parry, and unlock-gated Rache.
- Added a procedural greybox and JSON frame-time telemetry against the 16.6 ms budget.
- Encoded the twelve load-bearing design laws in constants, static assertions, Unreal automation tests, and deterministic source audits.
- Added private self-hosted CI so the repo does not rely on exhausted GitHub-hosted Actions minutes.
- Added the complete M0 Pixel Streaming automation path: pinned Epic frontend/infrastructure, secure signalling/private TURN, iPad UI, NVENC proof, versioned deployment/rollback, services, firewall rules, evidence collection, and provider-level idle stop.
- Read and indexed all 185 repository documents, including the extensionless DOCX GDD, and recorded canon hierarchy, legacy conflicts, malformed paths and unresolved cross-references.
- Selected AWS G6.2xlarge in Frankfurt as the provisional M0 host architecture with EBS-backed saves; added a strict-private Windows Server 2022 CloudFormation/Tailscale implementation that cannot launch without cost approval. This is not provisioning authorization.
- Hardened the M1 greybox path: broken posture cancels a queued strike, invalid Examine targets cannot strand the camera, death enters the authored four-second Hold, lock-on uses a 20 m acquisition/25 m retention leash, and Rache has its locked keyboard/stick-combination input gate.
- Added the native M1 animation-state bridge and montage-notify contact path, while keeping authored animation assets as a separate acceptance gate.
- Added the markerless six-trigger quest/journal foundation, including silent activation, fallible chronological notes, internal expiry outcomes and mutual exclusion.
- Added native M2 state for Isabel Cruz's locked Duty/List/1846 thresholds, killed/spared/avoided outcomes, non-hostile mercy state and four-second death hold.
- Added HUD-free heat pressure plus native Cenote breath/drowning and independent water-routing, green-gold image, Keeper outcome and mandatory Return state.
- Added the first four finite authored Rexa mission definitions (three Turns and one spoken-agreement Standing salvage variant), multi-voice local directions and a physical chronological notebook actor.
- Added the exact authored forty-person Las Raíces roster at the locked 16/12/8/4 cultural split, stable identities, professions, local mission knowledge and four purpose anchors per resident. All five children are engine-level protected; exact-50 m combat flight and fail-closed shelter/restore paths exist.
- Replaced the Rexa schedule's future-clock placeholder with the single canonical M4 world-time owner. Las Raíces now consumes `UDarkArisenWorldRulesSubsystem::GetTotalWorldMinutes()` rather than inventing a settlement clock.
- Locked Alpha delivery to content-identical UE 5.5 Windows x64 and Linux x86_64 Shipping targets, with native PS5 deferred to the authorised Sony Beta path.

### M3 source foundation now present

- Added `UShipVoyageComponent` with heading, wind, Point-of-Sail, forward-speed state and crew-dependent handling.
- Jake can leave the helm without freezing voyage state; the crew holds the last commanded course. This is source behavior, not sea-passage acceptance.
- Added exactly five canonical named crew records: Mira, Big Tom, Esteban, Ines and Father Salvio, plus authored 40–90-hands state.
- Added physical-chart acquisition/annotation state. There is no map-click movement, Compass, minimap, player-dot, or water-fast-travel API in the M3 source boundary.
- Added `ALaLiberacionShip` as a level-placeable actor with four stable deck roots: Weather, Upper, Mid and Hold. These are attachment boundaries, not a claim that final four-deck geometry exists.
- Connected Great-Cabin rest to the single M4 legal rest/autosave request path; the ship actor performs no disk write.
- Final hull geometry, continuous stairs/ladders, collision, buoyancy, roll/pitch/heel, ocean coupling, physical-map presentation, crew animation/audio and the real sea-passage gate remain open.

### M4 source foundation now present

- Added Jake-owned `UProgressionEconomyComponent` for BODY / CRAFT / STANDING boundaries, three currencies and social state.
- BODY source laws are encoded: Physician's Draught +15 HP to 380, sixteen Deep-Water Pearls +5 stamina to 200, fifteen named deflection sets +5 posture to 175, and authored carry milestones capped at 130 kg.
- CRAFT locks 68 nodes, five branches and the canonical 16/12/14/13/13 branch totals, 94 available Marks, 141 total full-tree cost, 23 canonical teachers, 23 teacher-gated nodes and 11 Standing-gated nodes.
- The bible names only a subset of the 68 nodes. Only those named nodes are pre-registered; the remaining authored definitions are deliberately not invented. `IsSkillCatalogComplete()` therefore fails closed until all 68 definitions satisfy every invariant.
- Skill definitions now support up to three authored prerequisites. Missing prerequisites block learning.
- Meeting a teacher no longer satisfies a teacher gate. A node requires `CompleteTeachingScene(TeacherId, NodeId)` for a teacher explicitly authored for that node; only then may Marks unlock it.
- No Respec API exists. Money has no path into BODY, CRAFT or STANDING.
- Added Doubloons, Pounds and Silver Marks as independent balances with credit/spend only; there is no generic conversion API.
- Added Stranger/Known/Owed/Wary greeting state plus listening/interruption/overhear state without affinity or relationship meters.
- Added `UDarkArisenWorldRulesSubsystem` as the canonical clock: one in-game hour equals 150 real seconds.
- Autosave requests have exactly two legal sources: rest and chapter boundary. The lake-to-dock authored window suppresses autosave while manual save remains allowed. The subsystem performs no disk IO itself.

### Verification source added

- Added `Tools/ci/validate_m3_m4.py` with fail-closed checks against Compass/minimap/fast-travel, Respec, generic currency conversion, relationship meters and illegal autosave APIs.
- Added negative Python regression cases that deliberately inject forbidden conversion, a sixth named crew entry and `AutosaveBeforeDecision` and require validation failure.
- Added native Unreal automation specs for the M3 ship contract, M4 progression/economy/social contract, teacher-scene gate, prerequisites and save suppression.
- Updated both Windows and Linux CI paths to run the M3/M4 validator and the expanded validator suite before Unreal compile/tests.
- These new M3/M4 gates are **not claimed as executed green yet** because the configured self-hosted UE runners remain unavailable.

## Deliberately not done

- No GPU host, VM, domain, certificate, TURN endpoint, paid voice job, paid asset service, or deployment was provisioned or started.
- No Unreal compile, package, browser stream, iPad input test, sea-passage playtest, named-teacher scene playtest or provider deallocation test is claimed as passed for this tranche.
- No authored M3 ship hull/interior asset or M4 final 68-node catalog is claimed complete.
- No Alpha/Beta or full-content build is claimed.
- No PS5 package is part of Alpha. Native PS5 work starts in Beta only with authorised Sony tools and development/test hardware.
- UE 5.8 was not adopted; the approved source/platform path remains UE 5.5 until a separate compatibility decision.

## Next source actions

1. Continue M3 from the stable ship boundary: authored deck traversal contract, physical map presentation, crew schedule/morale world-read state, helm/crew command boundaries and vessel-physics interface without inventing final tuning.
2. Continue M4 economy with the four canonical bottomless sinks and chapter ledger while preserving the no-compounding and no-capability-purchase laws.
3. Continue M4 time/rest integration for safe houses and rest dayparts without adding wait-until-hour, hunger, fatigue or additional autosave triggers.
4. Continue M4 social presentation boundaries: one greet input, no antagonise input, listening interruption and world-readable greeting consequences without relationship UI.
5. Build out the remaining 68-node catalog only from explicit repository sources. Do not create filler node names or percentage upgrades to hit the count.
6. Keep `Tools/ci/validate_m3_m4.py`, native automation specs, `HANDOVER.md`, PR #7 and `Docs/ALPHA_DELIVERY_CHECKLIST.md` synchronized with every completed source tranche.

## Deferred runtime / acceptance gates

- Windows and Linux UE 5.5 compilation and Unreal automation on matching self-hosted runners.
- M0 private iPad/Pixel Streaming runtime evidence.
- M2 authored Rexa jungle, Las Raíces anchors/NavMesh, Cenote, Fuerte San Rafael and two-hour vertical-slice play evidence.
- M3 final walkable ship, vessel physics and real sea-passage evidence.
- M4 named-person technique scene, complete authored catalog, economy/save/social presentation and runtime persistence evidence.
- Performance/frame-time, packaged candidates and full acceptance remain mandatory before any milestone can be called complete.

## Known external blockers

- No matching private Windows UE 5.5 or Linux UE 5.5 self-hosted runner is currently accepting the PR jobs; this is intentionally treated as a deferred external runtime gate under Flo's current direction, not as a source-work stop.
- The active workspace has no licensed UE 5.5 installation and no Windows environment. Pixel Streaming transports an already packaged application; it cannot compile the Windows or Linux build.
- Pixel Streaming Infrastructure itself does not provide free GPU capacity. Any billable host still requires Flo's explicit prior approval.
