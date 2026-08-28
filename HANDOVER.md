# Engineering handover

**Updated:** 2026-08-28  
**Source state:** M2 backlog plus M3/M4 source-completion tranche is integrated in PR #7  
**PR:** #7 — `feature/m2-runtime-completion` → `develop`  
**Runtime state:** UE 5.5 compile/runtime/playtest gates are deferred under the current operator direction, not waived and not marked passed.  
**Detailed M3/M4 record:** `Docs/M3_M4_SOURCE_COMPLETION.md`

## Current operator direction

Flo explicitly directed on 2026-08-28 that the currently unavailable UE 5.5 Runtime/self-hosted-runner path must not block further source work and that M3/M4 should be completed and PR #7 merged. Source integration may therefore proceed while Unreal/runtime evidence remains open. No document, checkbox, PR or report may turn that deferral into a claim that the runtime gate passed.

## Baseline carried into PR #7

- UE 5.5 C++ project boundary, native third-person pawn, input, camera, combat, wounds, stamina/posture, Rache, interaction and greybox telemetry exist.
- Twelve load-bearing design laws are encoded through constants, automation tests and deterministic source audits.
- M0 private Pixel Streaming/deployment source path exists but no host/runtime acceptance is claimed.
- M1 native loop and animation-state/notify boundaries exist; final authored assets and feel evidence remain open.
- M2 markerless quests/journal, Isabel Cruz state, heat, Cenote state, exact forty-person Las Raíces roster, child protections and sparse combat-proximity behavior exist in source.
- Las Raíces consumes the single canonical M4 world clock rather than maintaining a competing schedule clock.
- Alpha remains two content-identical UE 5.5 Shipping targets, Windows x64 and Linux x86_64; native PS5 stays Beta-only with authorised Sony tooling.

## M3 source completion

### La Liberación and sailing

- `ALaLiberacionShip` is the singular level-placeable ship source boundary.
- Weather / Upper / Mid / Hold are four stable deck roots on one continuous actor; no deck loading/teleport API is introduced.
- `UShipVoyageComponent` owns heading, commanded heading, wind, Point-of-Sail, speed and swept physical actor movement.
- Jake leaving the helm does not freeze the voyage.
- Handling depends on active hands and the first-mate role. If Mira is lost, a hand can step up badly so the mechanical function remains degraded; other named crew do not affect manoeuvre response. Father Salvio has no mechanical handling effect.
- Hands are bounded to the authored 40–90 mature-household range while catastrophic lower states remain representable.

### Crew household

- Exactly five canonical named crew are encoded: Mira, Big Tom, Esteban, Ines and Father Salvio.
- `UShipHouseholdComponent` consumes canonical world time and produces named deck/activity schedules, watches and meal periods.
- Up to eight specialist berths and twelve semi-named hands are represented.
- Morale has one private numeric authority and no numeric/UI getter. The public read is qualitative only: singing, shared mending, galley fire, unresolved argument, an empty forecastle at dusk, or an empty hammock after loss.
- Named loss is permanent and propagates into voyage state.

### Navigation, physical map and passage

- Physical chart acquisition/annotation state exists.
- `APhysicalMapActor` uses the held Anchored Examine path and records only sailed coastlines, visited settlements, routes actually given and persistent handwritten notes.
- No player dot, compass, minimap, map-click travel or water-fast-travel API exists in the M3 source boundary.
- `USeaPassageComponent` requires both authored canonical-world-time and physically observed distance before a passage can complete. It contains no level-open, server-travel, waypoint teleport or actor-location write.
- Great-Cabin rest routes into the canonical M4 rest/save owner and supports only the four authored dayparts.

## M4 source completion

### BODY / CRAFT / STANDING

- Jake owns `UProgressionEconomyComponent` as a native component.
- BODY laws are encoded: HP 200→380 through twelve +15 draughts, Stamina 120→200 through sixteen +5 pearls, Posture 100→175 through fifteen +5 named deflection sets, Carry 80→130 kg through authored acts.
- CRAFT structural invariants are locked: 68 nodes, branches 16/12/14/13/13, 94 available Marks, 141 full-tree cost, 23 canonical teachers, 23 teacher-gated nodes and 11 Standing-gated nodes.
- Skill nodes permit at most three authored prerequisites.
- Meeting a teacher does not satisfy a teacher gate. `CompleteTeachingScene(TeacherId, NodeId)` must be recorded for that exact person/technique before Marks may unlock it.
- No respec API and no money-to-BODY/CRAFT/STANDING path exists.
- The bible does not individually name all 68 techniques. The implementation does not fabricate filler abilities: `USkillTreeCatalogDataAsset` accepts the complete authored set only when every count, cost, teacher, Standing gate and prerequisite invariant is valid.

### Economy

- Doubloons, Pounds and Silver Marks are independent balances with no generic conversion API.
- `FChapterLedger` resolves holding income, army upkeep, garrison upkeep and construction draw exactly once per chapter.
- No interest, passive investment or compounding path exists.
- Legendary work, Highmoore reconstruction in Silver and crew-share payment are explicit sink paths.
- Money cannot unlock capability.

### Time, rest and saves

- `UDarkArisenWorldRulesSubsystem` is the single world clock: one in-game hour = 150 real seconds.
- Rest is limited to Great Cabin or Safe House and can target only Dawn, Midday, Dusk or Night.
- There is no arbitrary wait-until-hour, hunger/thirst/fatigue or bedroll-anywhere mechanic.
- Manual save remains unrestricted.
- Autosave requests have exactly two legal sources: completed rest and chapter boundary.
- Lake→Dock suppresses autosave requests for the whole authored window while manual save stays available.
- World rules queue save requests but perform no disk IO themselves.

### Social

- Stranger / Known / Owed / Wary greeting states exist without affinity numbers or relationship UI.
- Listening supports begin/interruption/completion and only uninterrupted completion records an overheard conversation.
- Sitting is explicit social state and does not accelerate time or grant a buff.
- Drawing a weapon is the represented hostile social act: it interrupts passive listening/sitting, moves the context to Wary and persists for an authored number of chapter boundaries before restoring the previous greeting.
- No antagonise input, gift-menu system or systemic romance layer is introduced.

## Verification encoded in PR #7

- `.github/workflows/ci.yml` runs M0, M1, M2, M3/M4, Alpha-platform and design-law validators before Unreal compile/tests on both configured self-hosted platforms.
- `Tools/ci/validate_m3_m4.py` fails closed on prohibited compass/minimap/fast-travel, respec, generic currency conversion, relationship-meter and illegal-autosave APIs and checks the new M3/M4 source files.
- Python regression tests deliberately inject representative prohibited APIs and an invalid sixth named crew entry.
- Native Unreal specs cover M3 voyage/household contracts, immediate sea-passage rejection, M4 teaching/prerequisite rules, chapter non-compounding, qualitative social memory, daypart rest/save suppression and the exact synthetic 68-node structural contract.
- Final static review also corrected the Phase-12 Salvio rule: named-crew count no longer feeds handling; only First Mate + Hands do.

## Verification that is still deferred

- At the current PR head, GitHub reports both configured UE 5.5 self-hosted check jobs as queued rather than executed.
- The active workspace cannot clone GitHub directly because outbound DNS/network access is unavailable, so a second independent local validator run could not be performed here.
- No UE 5.5 Windows/Linux compile, Unreal automation run, package, browser stream, iPad test, sea-passage playtest or persistence/performance run is therefore claimed green by this tranche.

## Deferred authored/runtime acceptance

- M2 final Rexa jungle/Las Raíces/Cenote/Fuerte levels, navigation, animation and two-hour evidence.
- M3 final authored La Liberación hull/interior, continuous player traversal keel→maintop, buoyancy/roll/pitch/heel, ocean/weather coupling, final crew animation/audio and measured real passage evidence.
- M4 production Data Asset containing all 68 canon-approved node definitions, all authored teaching scenes, final vendor/holding/army/safe-house/social presentation and save/load persistence evidence.
- Performance/frame-time, packaged candidates, private Pixel Streaming/iPad evidence and Alpha approval.

## Cost / service state

- No GPU host, VM, paid runner, domain, certificate, TURN endpoint, paid voice job, paid asset service or deployment was provisioned or started by this tranche.
- Any future billable infrastructure remains an explicit operator approval gate.

## Next engineering line after PR #7

With M3/M4 source integration merged, the next source milestone is M5 Colonial War unless the operator explicitly redirects to deferred UE/runtime authoring first. Do not reopen M3/M4 by inventing missing abilities or replacing runtime evidence with source assertions; use `Docs/M3_M4_SOURCE_COMPLETION.md` as the boundary record.
