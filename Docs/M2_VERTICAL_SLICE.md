# M2 vertical-slice implementation record

**Status:** native quest, heat and Isabel Cruz proof-state source; not a vertical slice, Alpha, or Beta  
**Engine:** Unreal Engine 5.5  
**Authority:** current engineering handoff, `quest activation.md`, `quest design philosophy.md`, `mission types.md`, `dungeon design philosophy.md`, `regional dungeon catalog.md`, `colonial war bosses.md`, and `Docs/DesignAuthority.md`

## Delivered in this source tranche

- A native component owns exactly six activation modes: Conversation, Overheard, Object, Proximity, World State and Absence.
- Activation changes state only. It emits no HUD notification, sound, camera change, marker, acceptance prompt, reminder or completion card.
- Ordinary activation can add one dated entry in Jake's chronological notebook. Silent activation adds nothing until later knowledge gives Jake a reason to write.
- Rumours can enter as distorted information. A correction appends a later note and preserves the earlier wrong entry instead of rewriting history.
- The physical notebook's future presentation can query entries and perform plain text search. There are no objective checkboxes, distances, priorities, counts or completion percentages.
- Spoken contract agreement is represented as authored dialogue state, not an interface button.
- Authored expiry durations resolve internally as outcomes without timers, warnings or a failure state.
- Mutual exclusions make dormant alternatives unavailable without a point-of-no-return warning.
- Runtime quest state and journal entries are `SaveGame` data only. This component performs no disk write and cannot bypass the chapter/rest autosave law.
- Jake owns the component, but no physical journal asset or presentation has been claimed.
- Jake now owns a HUD-free heat-exposure component that distinguishes inactive space, cool cisterns, shade galleries and exposed heat.
- Cisterns recover heat pressure, shade accumulates it more slowly than exposed space, severe pressure impairs stamina regeneration and can apply environmental damage.
- The design sources do not lock numerical heat rates. Every provisional rate is an editable `DESIGN-GAP` and is not acceptance evidence.
- Isabel Cruz has native Duty, List and 1846 phase state at the locked 65% and 30% health thresholds.
- The encounter records exactly three authored resolutions: killed, spared during a posture break, or avoided only when external fort state proves surrender before combat.
- Spared and avoided are a distinct non-hostile combat state, so mercy is not represented as a fake death and lock-on/hit resolution reject the resolved actor.
- Killing Cruz invokes the existing four-second anchored hold. The actor creates no slow motion, music cue, kill camera, lifespan cleanup or forced view target.
- Encounter phase and resolution state are `SaveGame` data. This actor still performs no unauthorised disk write.

## Canon conflict rulings

The current handoff and later Phase 11 laws supersede conflicting older Rexa drafts:

- the old procedural Bounty Hunters Guild contracts are rejected because all 147 Standing variants must be authored and finite;
- the old Wanted-star interface is rejected because the build has no such HUD system;
- the old two-currency Rexa economy does not replace the current three-currency M4 requirement;
- the old 12 km² Mire plan is source material for future full content, not the required approximately 1 km² M2 Rexa slice;
- the M2 Tier 1 proof boss is Comandante Isabel Cruz with killed, spared and avoided outcomes, not an older Salazar commander substituted by assumption.

## Still required before M2 acceptance

- M1 runtime acceptance remains a dependency.
- A physical, held, handwritten journal presentation and local-direction dialogue integration.
- Three authored Turn quests and one authored Standing variant using this runtime.
- Approximately 1 km² of Rexa jungle, a forty-person settlement and the required traversal/vegetation physics.
- The Cenote of the First Mother with no marker, no music, no explanatory logs, one primary water/drowning hazard, its green-gold timed image and mandatory return shortcut.
- Authored heat/cistern trigger volumes, Fuerte San Rafael geometry, route logic, combat animations, three-phase dialogue, cabinet/list/report rewards and controller-tested balance.
- In-engine proof that killed, spared and fort-surrender avoidance routes each persist and drive their authored quest/world consequences; source state alone is insufficient.
- Windows UE 5.5 compilation, Unreal automation, complete two-hour play evidence and measured 60 fps.

No runtime, authored content or presentation gate may be inferred from this source foundation.
