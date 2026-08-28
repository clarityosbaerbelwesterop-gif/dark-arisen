# M5 / M6 source integration plan

**Branch:** `feature/m5-m6-source-completion`  
**Base:** `develop` at `adfb49d8c6ef43d45701af496664d7155985d727`  
**Runtime direction:** UE 5.5 compile/runtime/playtest gates remain deferred under the current operator direction. They are not waived and must not be marked passed by source-only work.

## Canon authority

Implementation follows `Docs/DesignAuthority.md`: current operator direction, locked design laws, then the newest narrowly scoped subsystem document.

### M5 conflict ruling — hostage eligibility

The Phase 7 retaliation draft says core crew are excluded from hostage selection. The later Phase 12 `crew system.md` explicitly says the retaliation Attachment Score may select crew. The later, narrower Phase 12 rule wins. M5 therefore treats named crew as eligible adults while preserving the absolute child exclusion. A lost named crew member remains a lost teacher with no replacement.

## M5 — Colonial War source goals

- Hidden per-region Imperial / Albion / Liberation / Crimson state; no player-facing numeric war meter.
- Chapter momentum: Alpha Ch4-6, Beta Ch7-8, Gamma Ch9-10.
- Physical Support / Break / Liberate outcomes reported into the strategic state; no strategy menu that performs the action itself.
- Crisis below faction control 30, Fall only after an authored assault, Liberation when Liberation Strength is at least 50, otherwise Vacuum/reflag outcomes.
- Separate Imperial and Albion retaliation Heat, five staged responses, chapter phase caps and chapter decay.
- Hostage selection from hidden Attachment Score: children never eligible; adult crew eligible under the newer Phase 12 rule; narrative-protected exclusions remain explicit data.
- Persistent holdings/castles, siege state and damage; no generic instant ownership toggle.
- Army and battle source boundaries with per-chapter upkeep, desertion risk and the rule that Jake falling in a large battle does not end the battle.
- Ten-chapter deterministic simulation contract for state invariants; no claim of runtime visual evidence.

## M6 — Highmoore source goals

- Highmoore is optional and reached through one permanent physical Crystal-Caves route; no Highmoore fast travel.
- Crystal Caves first passage remains a long authored traversal; repeat passage is a four-to-six-minute route, never a menu teleport.
- Crystal Guardian is optional/bypassable, tracks light rather than bodies, does not attack first and never chases beyond its threshold.
- Guardian defeat settles rather than explodes, with a four-second hold and no slow-motion/music sting; the Crystal Katana is optional because the Guardian can be skipped.
- Crystal Katana: no durability/repair/upgrade path, flat condition-independent attack cost, 74 cm reach, visible light signature and permanent ownership once taken.
- Horse system: four bond levels (0-3), four gaits, animal stamina/mood without UI, permanent death and no fast travel.
- Two authored ride contracts: lake ride is nine minutes, gait capped and unskippable; Arion ride is an unbroken gallop with horse-stamina suspension and lasting consequence.
- Princess lake-to-dock sequence owns the already-existing autosave suppression window.
- Arrow moment: no slow motion, no cue change, no camera move, no telegraph, player control retained and no revive/action prompt.
- Belos branch must support both Katana and no-Katana outcomes without adding a systemic romance layer.

## Verification

PR #8 will add deterministic M5/M6 source validators, negative regression tests and native Unreal automation specs. CI gates are additive; existing M0-M4/design-law/Alpha gates must not be weakened or removed.

## Explicitly deferred

Final authored levels, navmeshes, animation, audio, crowds, castles, armies, battle-scale performance, Crystal Caves geometry, Guardian assets, horse animation/physics, Princess staging, persistence playtests, package builds and measured runtime acceptance remain separate UE 5.5 evidence gates.
