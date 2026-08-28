# Dark Arisen Alpha delivery checklist

**Delivery branch:** `feature/m2-rexa-vertical-slice`  
**Merged source history:** PR #4 → `develop`; PR #5 → `main` on 2026-08-28. Neither merge proves the still-missing runtime or Alpha gates.  
**Engine:** Unreal Engine 5.5  
**Artifact definition:** the first Alpha is the complete private UE 5.5 game plus two separately verified, content-identical Shipping artifacts: `DarkArisen-Alpha-Windows-Arcware.zip` and `DarkArisen-Alpha-Linux-x86_64.zip`. The exact source archive accompanies them. A scaffold, greybox, vertical slice, streaming configuration or untested candidate is not an Alpha. Native PS5 work is deferred to Beta and is not an Alpha deliverable.

This file is the progress index. Detailed acceptance criteria remain authoritative in `M0_CHECKLIST.md` and `Docs/Handoff/ENGINEERING_HANDOFF.md`. A checkbox is marked only when its required source and runtime evidence both exist.

## Non-negotiable delivery gates

- [ ] Windows and Linux Development/Shipping builds compile in UE 5.5 with zero game-module warnings.
- [ ] Python and Unreal automation suites pass on the exact delivery commit.
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
- [ ] AI Voice Generator confirms zero cost and acceptable commercial usage rights before generation; any charge or unclear term requires Flo's explicit approval.
- [ ] Distinct human-sounding voice assets generated, reviewed and integrated.
- [ ] End-credits names, ordering, music composition/source, licence and cost are approved; no cue is invented or purchased without Flo.

## M8 — Platform, packaging and Alpha sign-off

- [ ] Windows platform integrations, achievements, cloud saves and controller configuration.
- [ ] Linux x86_64 platform integration, controller configuration, saves and parity with Windows.
- [ ] Accessibility, remapping, subtitle sizing and permitted difficulty settings.
- [ ] Full regression, performance, save migration, privacy and content-completeness passes.
- [ ] Private UE 5.5 source-project ZIP and checksum delivered to Flo.
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

- No matching private Windows UE 5.5 GitHub runner is accepting main workflow run #22 / job `98789832571`; GitHub reports an empty runner name for the required labels. No verified Linux UE 5.5 runner has been supplied either.
- The active Codex workspace is Linux but has no licensed UE 5.5 installation, and it has no Windows environment. Pixel Streaming starts only after a packaged Unreal application exists and cannot manufacture either Alpha build.
- Flo will handle the GPU and private browser link; no host/runtime evidence has been supplied yet.
- The AI Voice Generator is expected to be free, but no generation may begin before dialogue lock plus zero-cost and commercial-rights verification.
- The repository contains no canonical credits list or approved end-credits music cue yet. Flo's current direction requires both, but the cue, rights and any cost remain an explicit approval gate.

Source work continues on `feature/m2-rexa-vertical-slice` under Flo's current direction, but blocked runtime evidence must never be marked complete by inference.
