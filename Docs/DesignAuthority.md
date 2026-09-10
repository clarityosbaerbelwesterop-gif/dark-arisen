# Design authority and conflict register

**Reviewed:** 2026-09-10  
**Current implementation branch:** `feature/story-ue58-rendering-preview`  
**Primary story authority:** `Docs/MAIN_STORY_AUTHORITY_2026_09.md`  
**Rendering authority:** `Docs/UE58_RENDERING_AND_UE6_BRIDGE.md`

## Canon order

When two sources disagree, implementation uses this order:

1. Flori's current written direction and current engineering handoff.
2. This register and documents explicitly marked as current implementation authority.
3. The twelve locked design laws and documents that explicitly call a decision a law, lock, prohibition or absolute rule.
4. The newest narrowly scoped subsystem specification.
5. The phase master document for shared values and dependency order.
6. Older character, boss, region and mechanics drafts where they do not conflict with a higher source.
7. `Dark Arisen GDD v1.2` as historical concept context only.

No historical source is deleted merely because it is superseded. It can remain useful for discarded alternatives, names, tone and imagery, but it cannot silently reintroduce old runtime behavior.

## Current locked baseline

| Area | Canonical decision | Primary authority |
|---|---|---|
| Engine | Unreal Engine **5.8** production bridge; UE6 only after an actual public production release passes a separate compatibility milestone | Current operator direction; `UE58_RENDERING_AND_UE6_BRIDGE.md` |
| Shipping platforms | Alpha packages target Windows and Linux PC; native PlayStation 5 begins in Beta with authorised Sony tools | Current handoff |
| Performance | 60 fps is the hard gameplay floor; generated frames do not replace base-frame acceptance; six-frame deflection window | `technical targets.md`, `animation system.md`, current rendering authority |
| Rendering | Nanite + VSM + Lumen; hardware RT support when available; Path Tracer is an explicit cinematic/preview path, not normal gameplay | `UE58_RENDERING_AND_UE6_BRIDGE.md` |
| DLSS | Official integration only. Public UE5.8 baseline is DLSS 4.5; DLSS 5 is gated until an official compatible package/SDK is available to the build environment | `UE58_RENDERING_AND_UE6_BRIDGE.md` |
| Combat HUD | Posture is the only permitted combat HUD element | Current handoff, `technical targets.md` |
| Interaction | No highlights and no fade-to-black; one four-second corner prompt at 1.4 m | `interaction system.md`, current handoff |
| Time dilation | Only player-triggered Rache, 0.30 time scale, five real seconds, Chapter 9+ | `camera system.md`, current story authority |
| Music | No reactive music, stingers or adaptive layers | `cutscene catalog.md` |
| End credits | Main-story completion is followed by full credits with music; cue remains non-reactive and must be approved/licensed | Current written direction; `cutscene catalog.md` |
| Cinematics | Authored cinematics remove control only at explicit boundaries; protected playable moments remain playable | `cutscene catalog.md`, current story authority |
| Main story | Ten chapters, **34 finite critical-path missions**. Marc and Denise die in Chapter 1; Ethan is abducted alive, recovered physically in Chapter 8, and is not a Chapter-9 boss. Draven Voss is the final human antagonist | `MAIN_STORY_AUTHORITY_2026_09.md`, native `MainStoryMissionCatalog` |
| Quests and journal | Six invisible activation modes; no markers, acceptance UI, failure screens, reminders or consequence cards; Jake's chronological notebook may remain wrong until later correction | `quest activation.md`, `quest design philosophy.md` |
| M2 Isabel Cruz | Duty/List/1846 phases cross at 65% and 30%; killed, spared and fort-surrender avoidance remain distinct outcomes; death path holds four seconds without spectacle additions | `colonial war bosses.md`, current handoff |
| M2 fort heat | Fuerte San Rafael alternates exposed pressure, shade galleries and cool cistern recovery; exact rates remain an explicit tuning gap until runtime acceptance | `lava_heat_physics.md`, `castle_catalog.md`, current handoff |
| M2 Cenote | Environmental lip discovery; Rexan water routing; drowning; one-hour green-gold sunlight image; Keeper Below; mandatory inside-opened Return; no marker, ambient music or explanatory logs | `regional dungeon catalog.md`, `dungeon design philosophy.md`, `water_physics.md` |
| Travel | No fast travel over water; only authored physical land entries | `travel system.md`, current handoff |
| Saves | Autosave only on chapter boundary or rest; suppressed from false letter through dock where authored | `time and rest.md`, current handoff |
| Progression | Body, Craft and Standing do not convert; no respec | `progression overview.md`, `skill tree.md` |
| Children | Untargetable, undamageable, force-immune and ragdoll-excluded at engine level | `docs/design/npcs/children_and_families.md` |
| Romance | No systemic romance; only specifically authored narrative material may exist | `social system.md`, current handoff |
| Belos | The undercroft is never rendered or shown | `princess quest belos.md`, `technical targets.md` |

## Current Harlow / Draven story decision

The current opening is authoritative:

- Jake and Ethan travel on the family merchant ship with parents **Marc and Denise Harlow**.
- Draven Voss attacks.
- Marc and Denise are killed.
- Ethan is abducted alive.
- Jake goes into the sea, survives and washes ashore.
- The campaign follows Jake searching for Ethan and the reason the Harlows were targeted.
- Ethan is coerced for remembered Harlow route knowledge; he does not voluntarily become Draven's adopted heir.
- Ethan is recovered alive in Chapter 8.
- Chapter 9 uses Ethan's Grove and a memory/dream confrontation without turning the real Ethan into a boss.
- Chapter 10 converges on Draven and the Crimson Armada.

Any source claiming that Ethan died in Chapter 1, secretly joined Draven, or is the real Chapter-9 boss is now legacy-only.

## Superseded decisions

| Historical decision | Current canon |
|---|---|
| UE 5.4/5.5 baseline | UE 5.8 bridge; UE6 migration only after public production release and compatibility proof |
| Multiple 30 fps modes | 60 fps hard gameplay floor |
| Health/stamina/Rache/ammo/boss bars/minimap HUD | Posture-only combat HUD; no minimap or compass |
| Dynamic combat score | Music never reacts |
| Rache about eight seconds | Maximum five real seconds at 30% time scale |
| Ship as fast-travel hub | No water fast travel |
| Mission checkpoints and mission-failed flow | Chapter/rest autosave discipline; no failure screens |
| Organic proficiency with no skill tree | Three-axis progression and authored tree |
| Systemic Mira romance | No romance system |
| Phase-11 Ethan dies in Chapter 1 | Ethan is abducted alive and recovered in Chapter 8 |
| Legacy Ethan joins Draven / Ethan Chapter-9 boss | Explicitly rejected by current story authority |

## Repository-quality findings still relevant

1. `ANIMATIONS.md` is foreign mobile/React incremental-app material and is not a Dark Arisen animation authority.
2. Duplicated directory-prefix paths and orphaned legacy files remain cleanup work; detail does not grant them authority.
3. Older Ethan/Draven character and boss documents remain legacy references only where they do not conflict with the current 34-mission story.
4. Source existence is not runtime acceptance. A document, C++ class, generated previs clip or enabled project setting is not proof of a packaged playable asset.
5. DLSS 5 must never be represented as integrated until the official compatible plugin/SDK is actually present and compiled.

## Current implementation routing

- Story behavior is owned by `UMainStoryMissionCatalog` plus `UMainStoryDirectorComponent` and the existing markerless `UQuestJournalComponent`.
- The game mode registers the finite critical path into the journal at player start and fails closed if registration is invalid.
- Unreal Automation must prove catalog cardinality, ordering, no legacy Ethan-boss leak, and a complete start-to-credits state transition.
- Higgsfield owns motion/cinematic **previs/reference** only. Generated video is not automatically a shipping Unreal animation.
- Rendering gameplay acceptance and cinematic path-traced acceptance are separate evidence gates.

Any future change to these locked decisions requires a newer explicit operator direction or a newer authority record that names what it supersedes.
