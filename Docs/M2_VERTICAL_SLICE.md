# M2 vertical-slice implementation record

**Status:** native quest/environment/boss state plus authored Rexa mission, notebook and forty-person roster source; not a vertical slice, Alpha, or Beta  
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
- Native water-breath state locks the source-defined bare/trained/master limits to 30/60/90 seconds and treats authored equipment as indefinite.
- Head submersion depletes breath and then applies environmental drowning damage without a meter, warning delegate or failure card. The undocumented damage cadence remains `DESIGN-GAP`.
- The Cenote has independent, saveable records for lip entry, Rexan water routing, the green-gold image, the Keeper Below outcome and the mandatory Return shortcut.
- The green-gold image accumulates witness time only while Jake is inside its chamber and the external world-time owner confirms the catalog's exact one-hour sunlight window. Leaving either condition resets the attempt.
- The Return cannot open before the Keeper encounter has an explicit outcome; completion requires every independent Cenote beat.
- Added three finite, named Rexa Turn definitions: *The Empty Hammock*, *Three Cuts in Stone* and *The Salt Ledger*. Each has stable IDs, multiple local direction voices and authored outcome IDs rather than generic objectives.
- Added one finite Standing salvage variant, *The San Telmo Bell*. It begins only after a spoken fictional handshake and is one authored pool member, never a generated/repeating job.
- The absence Turn activates silently; the overheard Turn enters as a distorted rumour; asking a local appends what that person actually said to the chronological notebook without solving navigation.
- Added a physical notebook actor that is held through the existing anchored Examine path and renders dated first-person journal entries in order. Plain-text search remains the only tracking concession.
- Jake registers the four authored definitions fail-closed and the native test room exposes the physical notebook for later UE runtime proof.
- Added exactly forty named Las Raíces residents with stable IDs, occupations and authored home offsets. The roster locks the Rexan design-source census to sixteen Indigenous Rexans, twelve mixed Rexans, eight Imperial colonists and four sailors/traders.
- Every resident has dawn, shaded-midday, evening and night purpose anchors rather than a random wander point. Seven named knowledge roles connect specific locals to the four authored mission direction variants.
- A level-placeable settlement director validates the complete roster before spawning and cleans up its own partial population on failure. It never generates residents and is deliberately absent from the unrelated M0 test room.
- Settlement residents contain no health or combat component, so the existing hit and lock-on paths reject them; initialization fails if a derived resident adds either component. All five authored children additionally override damage and launch force to zero, disable actor damage and character physics interaction, carry an explicit protected-child tag and use a non-colliding, non-ragdoll greybox presentation.
- Added non-colliding, settlement-scoped schedule anchors and AI path requests. A future canonical time owner supplies the game minute; the settlement does not invent a competing clock or teleport residents. Duplicate/missing anchors and unsheltered tropical-midday destinations fail the whole schedule update.

## Canon conflict rulings

The current handoff and later Phase 11 laws supersede conflicting older Rexa drafts:

- the old procedural Bounty Hunters Guild contracts are rejected because all 147 Standing variants must be authored and finite;
- the old Wanted-star interface is rejected because the build has no such HUD system;
- the old two-currency Rexa economy does not replace the current three-currency M4 requirement;
- the old 12 km² Mire plan is source material for future full content, not the required approximately 1 km² M2 Rexa slice;
- the M2 Tier 1 proof boss is Comandante Isabel Cruz with killed, spared and avoided outcomes, not an older Salazar commander substituted by assumption.

## Still required before M2 acceptance

- M1 runtime acceptance remains a dependency.
- Final handwritten page art, page-turn/hand animation, subtitle/accessibility presentation and controller-tested physical-journal evidence. The native held presentation is source-complete but not visually accepted.
- World actors, conversations, route beats, consequences and persistence evidence for all three authored Turns and the Standing salvage variant. Catalog/state source alone is not completed quest content.
- Approximately 1 km² of Rexa jungle, authored Las Raíces level/anchor geometry, navigation and required traversal/vegetation physics. The exact forty-person roster and spawn boundary now exist in source; forty scheduled residents have not been compiled or observed running in UE.
- Authored placement of every schedule anchor on real navigation geometry plus controller-tested path completion/crowd avoidance. The native route request and fail-closed registry exist, but no UE navigation result has been observed.
- Four contextual ambient-dialogue layers, crowd reactions, adult gathering/fleeing and the absolute 50 m child-flee/removal behavior with runtime evidence.
- Authored Cenote geometry, environmental lip discovery, water-routing devices, swimming/current physics, image lighting, Keeper Below creature/AI/animation, offerings/ownership choice and physical Return shortcut.
- In-engine proof that the Cenote contains no marker, ambient music or explanatory log and that all required beats persist; source state alone is insufficient.
- Authored heat/cistern trigger volumes, Fuerte San Rafael geometry, route logic, combat animations, three-phase dialogue, cabinet/list/report rewards and controller-tested balance.
- In-engine proof that killed, spared and fort-surrender avoidance routes each persist and drive their authored quest/world consequences; source state alone is insufficient.
- Windows UE 5.5 compilation, Unreal automation, complete two-hour play evidence and measured 60 fps.

No runtime, authored content or presentation gate may be inferred from this source foundation.
