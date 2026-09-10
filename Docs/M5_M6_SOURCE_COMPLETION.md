# M5 / M6 source completion record

**PR:** #8 — `feature/m5-m6-source-completion` → `develop`
**Base:** PR #7 merge `adfb49d8c6ef43d45701af496664d7155985d727`
**Status:** source tranche implemented; UE 5.8 runtime/compile/playtest acceptance remains deferred and unclaimed.

## M5 — Colonial War

Implemented source contracts:

- Hidden regional Imperial / Albion / Liberation / Crimson state with no player-facing numeric war meter.
- Alpha/Beta/Gamma momentum by chapter: 4–6 / 7–8 / 9+.
- Physical mission outcomes report Support / Break / Liberate consequences into the strategic state; no strategy menu performs them.
- Crisis below 30 colonial control; Fall requires a separately reported authored assault; Fall resolves to Liberation at Liberation Strength ≥ 50, Crimson occupation at Crimson pressure ≥ 50, otherwise Vacuum.
- Once-per-region-per-chapter autonomous war ticks let the war move without Jake while keeping actual rates authored data.
- Separate Imperial and Albion retaliation Heat with exact five-stage thresholds, chapter caps and passive chapter decay.
- Hostage selection is deterministic from hidden attachment/ripple data and reachability. Children and explicitly protected people are excluded absolutely.
- Canon conflict ruling: the newer Phase-12 crew law supersedes the older Phase-7 core-crew exclusion. Adult named crew are eligible hostage candidates unless individually narrative-protected.
- `UColonyHoldingComponent` implements the four holding identities and five development stages. Razed castles cannot be claimed; Alliance Bastions are not Jake-owned; non-Alliance holdings require concrete delivered-person + voyage records before Establishment and retain unrepaired damage.
- Settlement specialization is explicit and cannot exist before the holding reaches the authored stage/type boundary.
- Four-phase castle siege state: Approach → Investment → Breach → Assault, with Wall / Back / Inside doors, legal blind assault, defensive variants and persistent breach damage.
- Army state uses named/cultural forces rather than generic unit cards: hired, militia, Alliance, faction levy and crew. Crew are not counted as an army.
- Upkeep anchors: hired 14 doubloons/man/chapter, militia 4, cavalry ×3, specialists ×2. Company marines stop after one unpaid chapter; hired troops leave after two; Alliance is not commandable by Jake.
- Jake command capacity starts at 200 and named officers add 150 each. The only command verbs are HOLD / PRESS / BREAK OFF.
- Large battles are 3–6 hidden-Cohesion front segments. Jake occupies only one segment, other segments can break independently, Rising battles reject direct Jake orders, and a final outcome cannot resolve before every segment resolves.
- Jake falling removes him from the line but does not end the battle; authored force/world simulation resolves the outcome later.

## M6 — Highmoore

Implemented source contracts:

- Crystal Caves are one permanent physical route. First passage requires at least 90 real minutes plus observed physical movement; repeat passages require an authored 4–6 real minutes plus observed movement. No instant travel API is present.
- Crystal Guardian begins dormant, does not attack first, consumes explicit light stimuli rather than body/player targets, has the 100–75 / 75–45 / 45–20 / 20–0 phase bands, can be bypassed and settles for four seconds on defeat.
- Bypassing the Guardian yields no Crystal Katana.
- Crystal Katana acquisition requires Guardian defeat. It has locked 74 cm blade / 1.1 kg mass, condition-independent attack stamina cost and a resting/moving light signature. No durability, repair, sharpening or upgrade path exists.
- Highmoore horse has bond 0–3 and Walk/Trot/Canter/Gallop without a player-facing stamina meter. Permanent death persists.
- Authored rides are real-time constrained: Lake 9 minutes with Trot ceiling; Arion 4 minutes unbroken Gallop with stamina suspension; Belos 6 minutes continuing that suspension; Return ~40 minutes capped to Walk with lasting exhaustion.
- Princess quest state starts canonical M4 lake→dock autosave suppression at false-letter handoff and ends it only at the dock.
- Arrow law is explicit: no slow motion, no music cue change, no camera move, no telegraph, no revive prompt; player control remains.
- Arion children and noncombatants are non-damageable through the quest-state safety boundary.
- Arion allows the unmarked west turn. Belos has distinct Katana-assault-interrupted and ordinary-weapon-overwhelmed source outcomes.
- No systemic romance layer, systemic magic layer or rendered Belos undercroft system was added.

## Verification added

- `Tools/ci/validate_m5_m6.py` fail-closed source-law validator.
- Negative regressions inject prohibited fast travel, systemic magic, illegal crew-hostage exclusion and Arrow slow motion.
- `Source/DarkArisen/Tests/M5M6SystemsSpec.cpp` covers regional war transitions, autonomous chapter ticks, retaliation caps, hostage laws, holding/population growth, siege persistence, army upkeep/desertion/command, independent battle segments, Crystal Caves timing, Guardian bypass, Katana properties, authored horse rides and Princess arrow/safety laws.
- Windows and Linux CI now run M5/M6 validation before existing Alpha/design-law/UE build steps. Existing M0–M4 gates were not removed or weakened.

## Explicitly not claimed complete by source alone

- UE 5.8 Windows/Linux compile and Unreal Automation execution on matching self-hosted runners.
- M5 authored colony/holding/castle levels, physical siege doors, naval bombardment, crowds, army AI, camps, battle scenes, retaliation missions and full campaign runtime simulation.
- M6 authored Crystal Caves geometry/minecart/puzzles, Guardian model/AI/animation/audio, Katana weapon assets, horse locomotion/physics, Highmoore world/settlements, Princess staging/dialogue/cinematics/audio and measured ride/return playtests.
- Save/load persistence evidence, performance, packaging, Pixel Streaming and Alpha acceptance.

This document records source closure only. Runtime checkboxes remain open until the corresponding UE 5.8 evidence exists.
