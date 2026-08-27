# M2 vertical-slice implementation record

**Status:** markerless quest-foundation source only; not a vertical slice, Alpha, or Beta  
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
- Comandante Isabel Cruz's heat/cistern arena, three phases, cabinet/list rewards, killed/spared/avoided paths and four-second Style D death hold.
- Windows UE 5.5 compilation, Unreal automation, complete two-hour play evidence and measured 60 fps.

No runtime, authored content or presentation gate may be inferred from this source foundation.
