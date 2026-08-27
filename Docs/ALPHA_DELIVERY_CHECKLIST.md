# Dark Arisen Alpha delivery checklist

**Delivery branch:** `feature/pr4-full-game-delivery`  
**Pull request:** #4  
**Engine:** Unreal Engine 5.5  
**Artifact definition:** the first Alpha is the complete, private UE 5.5 game project and a reproducibly packaged Windows build containing every approved M0–M8 system and all authorised game content. A source-only scaffold, greybox, vertical slice, or streaming configuration is not an Alpha.

This file is the progress index. Detailed acceptance criteria remain authoritative in `M0_CHECKLIST.md` and `Docs/Handoff/ENGINEERING_HANDOFF.md`. A checkbox is marked only when its required source and runtime evidence both exist.

## Non-negotiable delivery gates

- [ ] Windows Development and Shipping builds compile in UE 5.5 with zero game-module warnings.
- [ ] Python and Unreal automation suites pass on the exact delivery commit.
- [ ] Frame-time evidence meets the 60 fps floor in every milestone benchmark.
- [ ] The design-law audit passes and the combat HUD still contains posture only.
- [ ] Flo completes the private iPad/Pixel Streaming playtest.
- [ ] The packaged project is archived as a private UE 5.5 ZIP and its checksum is recorded.
- [ ] The complete content manifest is reconciled against the design source register.
- [ ] Flo explicitly approves the Alpha candidate after the first full-game playthrough.

## M0 — Foundation

- [x] UE 5.5 C++ project, targets, configs, Git LFS and native greybox exist.
- [x] Design laws, deterministic validators, frame telemetry and private runner workflow exist.
- [x] Private one-player Pixel Streaming deployment path and idle deallocation are implemented in source.
- [ ] Windows UE 5.5 compile, Unreal tests and in-engine smoke evidence.
- [ ] Explicitly approved GPU host and cost ceiling.
- [ ] Private iPad stream, NVENC, TURN, latency and shutdown evidence.

## M1 — Core loop

- [x] Keyboard, gamepad and two-zone touch movement/combat inputs.
- [x] Committed light/heavy/parry/dodge actions with no cancelling and locked six-frame deflection.
- [x] Five posture states, four wound layers and Crystal Katana animation exception.
- [x] Three-second Rally model and posture-only native HUD.
- [x] Startup-timed melee trace, health/posture resolution, deflection and Rally-on-contact.
- [x] One greybox duelling enemy with pursuit, committed attacks, posture and death.
- [ ] Authored montages and animation-notify timing.
- [x] Lock-on with living-combatant, range, facing and line-of-sight validation; no marker/HUD.
- [x] Animation-ready wound presentation profile and wound-only low-frequency camera drift.
- [ ] Authored wound-aware locomotion animation assets and final feel tuning.
- [ ] Physical doors/pickups, Examine presentation, corner widget and persistence tests.
- [ ] Five-minute encounter at measured 60 fps with controller/touch evidence.

## M2 — Vertical slice

- [ ] Rexa jungle region slice, settlement, Cenote dungeon and required traversal.
- [ ] Isabel Cruz boss with killed/spared/avoided outcomes and four-second death hold.
- [ ] Physical journal, six invisible quest activation paths, three Turns and one Standing mission.
- [ ] Two-hour evidence run satisfying the M2 merge gate.

## M3 — Ship

- [ ] Walkable four-deck *La Liberación*, sailing, wind, physical map and navigation.
- [ ] Five named scheduled crew members, morale readability, great cabin, rest and autosave.
- [ ] Real sea-passage gate with no fast travel or loading transition.

## M4 — Systems

- [ ] Three-axis 68-node progression, 23 teachers and no respec/conversion.
- [ ] Three-currency economy, sinks, time/save rules and social greeting/listening systems.
- [ ] Named-person technique-learning scene proving the M4 gate.

## M5 — Colonial war

- [ ] Hidden regional war simulation, colonies, retaliation and hostage selection.
- [ ] Castles, sieges, armies, battles and ten-chapter simulation evidence.

## M6 — Highmoore

- [ ] Crystal Caves, Guardian, Katana, mounted travel and two authored rides.
- [ ] Princess questline and suppressed-autosave window.
- [ ] Locked arrow moment verified with no slow motion, cue change or camera move.

## M7 — Content scale-out

- [ ] Remaining regions and world traversal.
- [ ] 61 dungeons, 17 Threads, 132 Turns and 147 mission variants reconciled and complete.
- [ ] Nine Tier 1 bosses plus the authoritative reconciled boss register.
- [ ] Assassin network, reconstruction, NPC populations, fauna and remaining authored content.
- [ ] All 19 authored cutscenes and all 22 playable moments implemented.
- [ ] Dialogue lock, casting matrix, pronunciation guide and subtitle pass complete.
- [ ] Voice usage rights and exact cost approved by Flo before generation.
- [ ] Distinct human-sounding voice assets generated, reviewed and integrated.

## M8 — Platform, packaging and Alpha sign-off

- [ ] Windows platform integrations, achievements, cloud saves and controller configuration.
- [ ] PS5 dev-kit work, DualSense features, TRC and certification when external access exists.
- [ ] Accessibility, remapping, subtitle sizing and permitted difficulty settings.
- [ ] Full regression, performance, save migration, privacy and content-completeness passes.
- [ ] Private UE 5.5 Alpha ZIP and checksum delivered to Flo.
- [ ] Private browser play link delivered only after successful deployment and access denial test.

## Current blockers outside source authoring

- No matching private Windows UE 5.5 GitHub runner is accepting the queued workflow.
- No paid/free GPU entitlement has been proven and approved for deployment.
- No voice generation may begin before dialogue lock, rights review and explicit cost approval.
- PS5 packaging/certification requires Sony programme access and hardware not present in this workspace.

Source work may continue in PR #4 under Flo's current direction, but blocked runtime evidence must never be marked complete by inference.
