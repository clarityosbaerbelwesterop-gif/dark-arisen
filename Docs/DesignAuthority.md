# Design authority and conflict register

**Reviewed:** 2026-08-26  
**Scope:** 185 repository documents (184 Markdown/text sources plus the OOXML file `Dark Arisen GDD v1.2`) and the two current engineering handoff documents supplied by Flo.

## Canon order

When two sources disagree, implementation uses this order:

1. Flo's current written direction and the current engineering handoff.
2. The twelve locked design laws and documents that explicitly call a decision a law, lock, prohibition, or absolute rule.
3. The newest narrowly scoped subsystem specification for the feature being implemented.
4. The phase master document for shared values and dependency order.
5. Older character, boss, region, and mechanics drafts where they do not conflict with a higher source.
6. `Dark Arisen GDD v1.2` as historical concept context only.

No source is deleted. A legacy source remains useful for names, tone, imagery, and discarded alternatives, but it cannot silently reintroduce a superseded mechanic.

## Locked implementation baseline

| Area | Canonical decision | Primary authority |
|---|---|---|
| Engine | Unreal Engine 5.5 for M0; evaluate upgrades only as a separate compatibility milestone | Current engineering handoff and Pixel Streaming setup |
| Shipping platforms | Windows PC and PlayStation 5; no Xbox, Switch, or mobile build target | Current engineering handoff |
| Performance | 60 fps is the hard floor; six-frame deflection window | `technical targets.md`, `animation system.md` |
| Combat HUD | Posture is the only permitted combat HUD element | Current engineering handoff, `technical targets.md` |
| Interaction | No highlights and no fade-to-black; one four-second corner prompt at 1.4 m | `interaction system.md`, current engineering handoff |
| Time dilation | Only player-triggered Rache, 0.30 time scale, five real seconds, Chapter 9+ | `camera system.md`, current engineering handoff |
| Music | No reactive music, stingers, or adaptive layers | `cutscene catalog.md` |
| End credits | Main-story completion is followed by full credits with music; the cue must remain non-reactive and be approved/licensed before integration | Flo's current written direction; `cutscene catalog.md` music law |
| Cinematics | Nineteen authored cutscenes; twenty-two listed moments must remain playable | `cutscene catalog.md` |
| Quests and journal | Six invisible activation modes; no markers, acceptance UI, failure screens, reminders or consequence cards; Jake's chronological notebook may remain wrong until a later correction | `quest activation.md`, `quest design philosophy.md` |
| Travel | No fast travel over water; only the three authored land entries | `travel system.md`, current engineering handoff |
| Saves | Autosave only on chapter boundary or rest; suppressed from the false letter through the dock | `time and rest.md`, current engineering handoff |
| Progression | Body, Craft, and Standing do not convert; no respec | `progression overview.md`, `skill tree.md` |
| Children | Untargetable, undamageable, force-immune, and ragdoll-excluded at engine level | `docs/design/npcs/children_and_families.md` |
| Romance | No systemic romance; only specifically authored narrative material may exist | `social system.md`, current engineering handoff |
| Belos | The undercroft is never rendered or shown | `princess quest belos.md`, `technical targets.md` |

## Superseded GDD v1.2 decisions

The April 2026 GDD is internally coherent as the original six-region pirate concept, but the later handoff and Phase 7-12 documents rewrite major parts of it.

| GDD v1.2 concept | Current canon |
|---|---|
| PS5, Xbox Series X/S, and PC | PC and PS5 only |
| UE 5.4+ and multiple 30 fps modes | UE 5.5 M0 pin and 60 fps on every supported platform |
| Health, stamina, Rache, ammo, boss bars, and minimap HUD | Posture-only combat HUD; no minimap or compass |
| Dynamic combat score | Music never reacts |
| Rache lasts about eight seconds | Rache lasts at most five real seconds at 30% time scale |
| Ship as a fast-travel hub | No water fast travel |
| Mission checkpoints and mission-failed flow | Chapter/rest autosave discipline; no failure screens |
| Organic proficiency with no skill tree | Three-axis progression and a 68-node authored tree |
| Mira as a potential systemic romance | No romance system |
| Ethan is captured, joins Draven, and becomes the Chapter 9 boss | Later Phase 10/11 structure opens with Ethan's death and treats Chapter 9 around Ethan's Grove; the old betrayal/boss branch is legacy |
| Twenty-three optional bosses plus the old story-boss list | Later boss and campaign catalogs govern; counts must be reconciled before content scale-out |

## Repository-quality findings

1. `ANIMATIONS.md` describes an unrelated mobile/React incremental application with money counters, prestige, and iPhone rendering. It is quarantined as foreign content.
2. `region_04_quiet_coast_overview_part2.md` begins mid-sentence and is an orphaned continuation, not a standalone authority.
3. Three paths contain duplicated directory prefixes:
   - `docs/characters/docs/characters/docs/characters/draven_voss.md`
   - `docs/characters/docs/characters/ethan_harlow.md`
   - `docs/design/colonial_war/docs/design/colonial_war/castle_catalog.md`
4. Five explicit Markdown cross-references have no matching repository file:
   - `bosses/corazon.md`
   - `bosses/the_twins.md`
   - `npcs/cross_chapter_npc_index.md`
   - `physics/cloth_hair_sound_physics.md`
   - `region_05_naval_combat.md`
5. Phase 3 combat sources still mention Xbox and a 30 fps mode. Those clauses are superseded; their non-conflicting stamina, posture, and frame-data specifications remain provisional M1 inputs.
6. The old German Ethan/Draven character bibles and their boss specifications conflict with the current Chapter 1/Chapter 9 structure. They remain legacy references until rewritten against Phase 11.

## M0 decisions derived from the review

- M0 stays on UE 5.5 because the approved handoff and Pixel Streaming Infrastructure revision are matched to that engine version.
- Baseline HP, stamina, and posture remain 200/120/100.
- The combat-feel deep dive supplies provisional M1 stamina tuning: 20/sec regeneration, one-second normal delay, three-second exhaustion delay, light 15, heavy 30, dodge 25, parry 10, and sprint 10/sec.
- The old GDD's HUD, music, checkpoint, romance, platform, and Chapter 9 behavior must not leak into code.
- Content production beyond the M0 greybox must cite its governing source in the PR and must pass the twelve-law audit.

## Open canon work before M2 scale-out

- Rewrite the Ethan and Draven character/boss material to the current Phase 11 story, or explicitly restore the older betrayal branch.
- Publish one authoritative boss register and reconcile the old “23 optional bosses” promise.
- Repair or replace the five unresolved cross-references and the three malformed paths.
- Split or rename the orphaned Quiet Coast continuation.
- Move `ANIMATIONS.md` out of the design corpus.
- Resolve the remaining shared-value discrepancy between `mechanics_overview.md` (two-second stamina delay) and `mechanics/combat_feel.md` (one-second normal, three-second exhaustion delay). M0 currently follows the narrower combat specification.

This register is an engineering routing decision, not a rewrite of the design bible. Any change to a locked law requires Flo's explicit approval.
