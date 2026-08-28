# Engineering handover

**Updated:** 2026-08-28  
**Integrated baseline:** PR #7 merged to `develop` at `adfb49d8c6ef43d45701af496664d7155985d727`  
**Active source work:** M5 Colonial War + M6 Highmoore source completion  
**Active PR:** #8 — `feature/m5-m6-source-completion` → `develop`  
**Runtime state:** UE 5.5 compile/runtime/playtest gates remain deferred under the current operator direction, not waived and not marked passed.  
**Boundary records:** `Docs/M3_M4_SOURCE_COMPLETION.md`, `Docs/M5_M6_SOURCE_COMPLETION.md`

## Current operator direction

Flo directed on 2026-08-28 that unavailable UE 5.5 runtime/self-hosted-runner gates must not block source production. Source work may therefore advance while runtime evidence remains open. A source merge is integration only: no document, checkbox, PR or report may translate that deferral into a claim that UE compile, runtime, performance, packaging or Alpha acceptance passed.

## Integrated baseline before PR #8

- M0/M1 project, interaction/combat, design-law audit, Pixel Streaming source path and platform contracts exist.
- M2 Rexa source foundations exist, including markerless quest/journal state, Isabel Cruz, heat/Cenote state, exact forty-person Las Raíces roster, protected children and canonical world-clock integration. Authored M2 levels/navigation/animation/two-hour runtime evidence remain open.
- M3 ship source closure and M4 systems source closure are integrated through PR #7. Their exact boundary and deferred runtime work are in `Docs/M3_M4_SOURCE_COMPLETION.md`.
- Alpha remains Windows x64 + Linux x86_64 UE 5.5 Shipping from one accepted source commit; native PS5 remains Beta-only with authorised Sony tooling.

## M5 source completion in PR #8

### Hidden regional war

- `UColonialWarStateSubsystem` owns hidden Imperial Control, Albion Control, Liberation Strength and Crimson Threat per region. No numeric player-facing war meter is exposed.
- Chapter momentum is Alpha for Chapters 4–6, Beta for 7–8 and Gamma for 9+.
- Support / Break / Liberate are report-only consequences from already completed physical content; there is no strategy-menu action that performs them.
- Crisis requires colonial control below 30. Fall is separate and requires an authored assault completion. After Fall, Liberation Strength ≥ 50 yields Liberation; otherwise Crimson pressure ≥ 50 can occupy the vacuum, else the result is Vacuum.
- `FRegionalAutonomousWarTick` permits one authored non-player tick per region/chapter so the war moves without Jake. Duplicate ticks fail closed and actual rates remain data rather than invented engine constants.

### Retaliation and hostages

- `URetaliationSubsystem` keeps Imperial and Albion Heat separate and hidden.
- Exact stages: 0–19 Unnoticed, 20–39 Letter, 40–59 Bounty, 60–74 CounterAttack, 75–89 Hostages, 90–100 ScorchedEarth.
- Chapter caps are Bounty in 4–6, Hostages in 7–8 and ScorchedEarth in 9+; passive chapter decay applies only when the faction was not attacked.
- Hostage selection uses hidden Attachment Score + ripple weight + region reachability and deterministic tie-breaking.
- Children and explicitly narrative-protected people are absolute exclusions.
- Canon conflict ruling: the later Phase-12 `crew system.md` supersedes the older Phase-7 blanket core-crew exclusion. Adult named crew are eligible unless individually protected. A lost crew teacher is not replaced.

### Holdings and colonies

- `UColonyHoldingComponent` implements Trade Post / Settlement / Military Stronghold / Alliance Bastion plus Claimed → Functional → Established → Developed → Thriving.
- Claim requires an intact taken castle, Alliance old fortress or authored abandoned site. A razed castle cannot be owned.
- Population is person-by-person: each delivered resident has a stable person ID, population kind and concrete voyage ID. A normal holding cannot reach Established with an empty delivered manifest.
- Settlements declare their type at Established and may choose Plantation / Craft / Free Settlement / Port specialization.
- Alliance Bastions are not Jake-owned and reject Jake-population delivery through this ownership path.
- Persistent siege/retaliation damage is tracked until explicitly repaired.

### Sieges

- `UCastleSiegeComponent` implements Approach → Investment → Breach → Assault → Resolved.
- Wall / Back / Inside are distinct doors; skipping prep into a blind assault is legal.
- Structural breach damage persists. A castle with breach damage cannot resolve as HeldIntact.
- Defensive sieges use the same state authority and resolve only Defended/Lost.

### Armies and battles

- `UArmyCampaignComponent` has only named/cultural forces: Hired, SettlementMilitia, Alliance, FactionLevy and Crew. Crew are never counted as an army.
- Upkeep anchors: hired 14 doubloons/man/chapter, militia 4, cavalry ×3, specialist ×2. Alliance/levy/crew have no generic cash upkeep here.
- Company marines depart after one unpaid chapter; normal hired forces after two. Militia do not use the hired-desertion path.
- Jake command capacity is 200 + 150 per named officer. Alliance forces reject Jake's orders.
- The entire order vocabulary is HOLD / PRESS / BREAK OFF; no unit-card, formation, facing or selection-box system exists.
- `ULargeBattleComponent` models 3–6 front segments with hidden Cohesion. Jake occupies at most one. Other segments can break independently and Rising battles reject direct Jake orders.
- Final battle outcome cannot resolve until every segment resolves. Jake falling removes him from his segment but deliberately leaves the battle active.

## M6 source completion in PR #8

### Crystal Caves

- `UCrystalCavesPassageComponent` is the only Highmoore route boundary.
- First passage requires at least 90 real minutes plus observed physical movement.
- Once opened, the route remains permanent. Repeat traversal requires an authored 4–6 real minutes plus physical movement.
- No instant level/network/waypoint transition path exists in the component.

### Crystal Guardian

- `UCrystalGuardianComponent` starts Dormant and must be explicitly engaged; it does not attack first.
- It consumes explicit light stimuli rather than body/player target state.
- Locked phase bands are 100–75 / 75–45 / 45–20 / 20–0.
- It may be bypassed entirely; bypass yields no Crystal Katana.
- Defeat enters Settling and holds four seconds before Defeated. No slow-motion/music-sting authority exists in this component.

### Crystal Katana

- `UCrystalKatanaComponent` can be acquired only from a defeated Guardian.
- Locked physical anchors: 74 cm blade and 1.1 kg.
- Attack stamina cost is condition-independent: the authored base cost is returned unchanged regardless of Jake's condition.
- Conventional armour-ignore and resting/moving light read are encoded.
- No durability, repair, sharpening or upgrade system exists.

### Highmoore horse and authored rides

- `UHighmooreHorseComponent` has bond 0–3, Walk/Trot/Canter/Gallop and qualitative mood with no player-facing stamina number.
- Permanent horse death persists.
- Lake ride: exactly 9 real minutes, gait cannot exceed Trot.
- Arion ride: 4 real minutes, unbroken Gallop, horse stamina suspended.
- Belos continuation: 6 real minutes, Gallop and the same suspension.
- After Belos the horse has lasting exhaustion; Return is ~40 real minutes capped to Walk.

### Princess quest / Arrow / Belos / Return

- `UPrincessQuestStateComponent` begins canonical M4 Lake→Dock autosave suppression when the false letter is handed over and only ends it at the dock.
- The Arrow law is explicit: no slow motion, no cue change, no camera move, no audio telegraph, no revive prompt; player control remains.
- Arion children and all noncombatants are non-damageable through the quest-state safety boundary.
- The real letter remains optional to read; walking past it is representable.
- Turning west at Arion is a valid unmarked terminal branch before Belos.
- Belos has distinct Katana-assault-interrupted and ordinary-weapon-overwhelmed outcomes.
- No systemic romance or systemic magic layer is introduced. No Belos undercroft rendering/level actor is implemented; the locked undercroft rule remains intact.

## Verification encoded in PR #8

- `.github/workflows/ci.yml` is additive and now runs M0, M1, M2, M3/M4, M5/M6, Alpha-platform and design-law validators on both configured platforms before UE build/test steps.
- `Tools/ci/validate_m5_m6.py` checks the M5/M6 source set and fails closed on fast-travel/teleport symbols, compass/minimap/player-dot, systemic romance/relationship UI, systemic magic, illegal autosave helpers, Belos undercroft rendering, Katana maintenance, Princess revive/slow-motion helpers and RTS-style army UI/control.
- Negative Python tests deliberately inject prohibited fast travel, systemic magic, crew-hostage exclusion and Arrow slow motion.
- Native `M5M6SystemsSpec.cpp` covers regional Crisis/Fall/Liberation, Chapter 4–10 autonomous movement, duplicate-tick rejection, retaliation caps, child exclusion/adult-crew eligibility, holding person-manifest growth, persistent siege damage, army upkeep/capacity/desertion, independent battle segments/Jake fall, Crystal-Caves timing, Guardian bypass, Katana anchors, authored horse rides and Princess Arrow/Arion safety laws.
- Existing M0–M4 validators and gates were not removed or weakened.

## Deferred UE/runtime acceptance

- Windows/Linux UE 5.5 compilation and Unreal Automation execution on matching self-hosted runners.
- M2 authored Rexa levels/navigation/animation and two-hour evidence.
- M3 authored full ship geometry, ocean/physics/animation/audio and measured sea-passage playtest.
- M4 production 68-node canon Data Asset, all teaching scenes and final persistence/social/economy presentation.
- M5 authored colony/holding/castle environments, real siege traversal/bombardment, army camps/AI, retaliation missions, battle-scale simulation/performance and full campaign evidence.
- M6 authored Crystal Caves/minecart/puzzles, Guardian AI/model/animation/audio, Katana assets, horse locomotion/physics, Highmoore locations/populations, Princess staging/dialogue/audio and measured ride/return evidence.
- Save/load persistence, frame-time, packaging, private Pixel Streaming/iPad and Alpha acceptance.

## Cost / service state

- No GPU host, VM, paid runner, domain, certificate, TURN endpoint, voice generation, paid asset service or deployment was started in PR #8.
- Any future billable infrastructure remains an explicit operator approval gate.

## Next engineering line

PR #8 remains Draft until its source audit is complete and the operator decides whether to merge. After M5/M6 source integration, the next source milestone is M7 content scale-out unless the operator redirects to deferred runtime authoring. Do not fill missing content with invented canon and do not convert source closure into runtime acceptance.
