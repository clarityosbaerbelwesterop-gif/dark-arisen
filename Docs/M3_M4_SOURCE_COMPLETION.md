# M3 / M4 source completion record

**Date:** 2026-08-28  
**PR:** #7 — `feature/m2-runtime-completion` → `develop`  
**Scope:** source completion only; UE 5.5 compile/runtime/playtest acceptance remains deferred under the current operator direction.

This record exists to prevent two opposite errors: treating source work as if nothing was completed, or treating unexecuted Unreal/runtime gates as if they passed. The M3/M4 engineering contracts below are complete enough to integrate into `develop`; authored final assets and runtime evidence remain later acceptance work.

## M3 — ship source closure

### Singular vessel and physical travel

- `ALaLiberacionShip` is the single level-placeable vessel source boundary.
- Weather / Upper / Mid / Hold are stable deck attachment roots on the same actor; no deck-loading or teleport API exists.
- `UShipVoyageComponent` owns heading, commanded heading, wind, Point-of-Sail, crew-dependent handling and forward speed.
- Voyage tick physically rotates and moves the owning actor through world space with swept movement. Route completion never moves the actor.
- Leaving the helm does not freeze the voyage. First-mate availability plus active hands determine handling; other named crew do not leak into manoeuvre quality. Father Salvio therefore has no mechanical handling effect.

### Household

- Exactly five canonical named crew records exist: Mira, Big Tom, Esteban, Ines, Father Salvio.
- Hands are bounded at the authored 40–90 mature-household ceiling; the system can represent catastrophic lower states without fabricating replacement people.
- Up to eight specialist berths and twelve semi-named hands are represented.
- `UShipHouseholdComponent` consumes the canonical world clock and produces scheduled deck/activity reads for the five named crew, including meals, Mira at dawn, Esteban at night, Big Tom's forge, Ines below and Salvio at dusk.
- Morale has one private numeric authority and no UI getter. Presentation is qualitative: singing, shared mending, galley fire, unresolved argument, empty forecastle at dusk, or an empty hammock after a loss.
- Named losses propagate into voyage state and are permanent. The role may degrade/recover through hands; the person is not replaced.

### Navigation and map

- Physical charts support acquisition and persistent annotation.
- `APhysicalMapActor` extends the held/Anchored Examine path and records only sailed coastlines, visited settlements, routes actually given and Jake's handwritten notes.
- There is no player dot, compass, minimap, quest-pin movement or map-click travel API.

### Real sea-passage gate

- `USeaPassageComponent` can finish a route only after both an authored minimum of canonical world time and an authored minimum of physically observed ship distance have been satisfied.
- The passage component contains no level-open, server-travel, waypoint teleport or actor-position write.
- This makes instant route completion fail closed while leaving final route lengths to authored content.

### Great Cabin

- Great-Cabin presence is explicit state.
- Rest routes through the canonical M4 rest/save authority.
- Rest can target only Dawn, Midday, Dusk or Night.

## M4 — systems source closure

### BODY / CRAFT / STANDING

- BODY baseline and ceilings are enforced: HP 200→380, Stamina 120→200, Posture 100→175, Carry 80→130 kg.
- BODY gains are unique authored objects/acts, not money or points.
- CRAFT locks the structural contract at 68 nodes, branches 16/12/14/13/13, 94 available Marks vs. 141 full-tree cost, 23 canonical teachers, 23 teacher-gated nodes and 11 Standing-gated nodes.
- Nodes support up to three authored prerequisites.
- Meeting a teacher is not teaching. `CompleteTeachingScene(TeacherId, NodeId)` is required for a teacher-gated technique.
- No respec path exists and money cannot buy BODY, CRAFT or STANDING.
- The bible names only a subset of the 68 capabilities. `USkillTreeCatalogDataAsset` therefore provides the complete authored-data boundary without inventing filler techniques. It accepts a catalog only when every locked structural invariant and prerequisite reference is valid.

### Economy

- Doubloons, Pounds and Silver Marks are separate balances with no generic conversion API.
- `FChapterLedger` resolves holding income, army upkeep, garrison upkeep and construction draw once per chapter.
- The same chapter cannot resolve twice; there is no interest, passive investment or compounding path.
- Legendary work, Highmoore reconstruction in Silver Marks and crew-share payment are explicit sinks.
- Money has no capability-purchase path.

### Time, rest and saves

- `UDarkArisenWorldRulesSubsystem` is the single world-time owner at one in-game hour per 150 real seconds.
- Rest locations are limited to Great Cabin and Safe House.
- Rest targets are limited to Dawn / Midday / Dusk / Night; there is no arbitrary wait-until-hour or bedroll path.
- Manual save remains unrestricted.
- Autosave has exactly two request sources: completed rest and chapter boundary.
- Lake→Dock suppression prevents autosave requests throughout the authored window while leaving manual save available.
- The subsystem queues save requests but performs no disk IO itself.

### Social

- Greeting states are Stranger / Known / Owed / Wary with no affinity value or relationship UI.
- Listening can be started, interrupted and completed; interrupted dialogue is not recorded as overheard.
- Sitting is explicit state and never advances time or grants a buff.
- Drawing a weapon is the only hostile social-state entry represented here: it interrupts passive social presence, moves the authored context to Wary and persists for an authored number of chapter boundaries before restoring the prior greeting state.
- No antagonise input, gift menu or systemic romance layer is introduced.

## Verification encoded in the repository

- `Tools/ci/validate_m3_m4.py` is wired into both Windows and Linux CI before Unreal compile/tests.
- The validator rejects prohibited fast-travel/compass/minimap, respec, generic currency conversion, relationship-meter and illegal autosave APIs.
- Python regression tests inject representative violations and require fail-closed behavior.
- Native Unreal automation specs cover the base M3/M4 contracts plus household, immediate sea-passage rejection, chapter non-compounding, Wary chapter memory, daypart rest/save suppression and a synthetic exact 68-node structural catalog.

## Deferred acceptance — not claimed by this record

The following are deliberately **not** marked passed:

- UE 5.5 Windows/Linux compilation and Unreal automation execution on the configured self-hosted runners.
- Final authored La Liberación hull/interior geometry, continuous player traversal from keel to maintop, buoyancy/roll/pitch/heel, water/weather coupling, final animation/audio and measured sea-passage playtest.
- Final authored production Data Asset containing all 68 canon-approved node definitions and all teaching-scene content.
- Final economy vendors/holdings/army content, safe-house presentation, social animation/dialogue presentation and save/load persistence proof.
- Performance, packaging, Pixel Streaming, iPad and Alpha acceptance.

At the time of this record, GitHub reports both UE 5.5 self-hosted PR jobs queued rather than executed. A separate local clone attempt from the active workspace also could not reach GitHub because outbound DNS/network access was unavailable. Neither condition is represented as a successful runtime test.

## Merge ruling

Under the operator's explicit 2026-08-28 direction, unavailable UE 5.5 runtime/runner gates do not block M3/M4 **source integration**. PR #7 may therefore be merged to `develop` after final diff/metadata review, while every deferred runtime gate above remains open and visible.
