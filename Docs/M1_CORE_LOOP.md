# M1 core-loop implementation record

**Status:** first source tranche; not a vertical slice, Alpha, or Beta  
**Engine:** Unreal Engine 5.5  
**Authority:** `Docs/DesignAuthority.md`, current engineering handoff, `animation system.md`, `camera system.md`, `interaction system.md`, `docs/design/mechanics/combat_feel.md`, and `docs/design/physics/movement_physics.md`

## Delivered in this tranche

- Jake has callable keyboard/gamepad inputs for light attack, heavy attack, parry, directional dodge/backstep, jump, sprint, and interaction.
- Combat actions are mutually exclusive and retain a weight-class commitment timer. The six-frame deflection window remains locked at 60 fps.
- Weapon timing exposes the authored Light, Medium, Heavy, Great, and Polearm startup/recovery values. The Cutlass begins as Medium; the Crystal Katana is Light.
- Posture drives the five authored visual states: Set, Pressed, Failing, Broken-imminent, and Broken. A break owns a 1.4-second vulnerability and resets to half posture afterward.
- Health and stamina drive Winded, Hurt, Bad, and Failing layers. Failing blocks sprint. The Crystal Katana suppresses only the wounded animation-set swap; it does not erase Jake's physical condition.
- Camera control has Free, Anchored, and Authored modes. Anchored refuses look input while preserving movement; Authored refuses both. The four-second death hold and six-second emergence anchor are explicit entry points.
- Interaction traces only to 1.4 metres, emits one corner-prompt event for four seconds, and runs Begin/Complete/Cancel over a real duration. The default Take duration is 0.6 seconds.
- Movement or damage cancels an in-progress interaction. No target-rendering mutation, world-space prompt, fade, or generic loot-container path exists.

## Conflict rulings applied

The later locked rules supersede contradictory Phase 3/6 draft clauses:

- attack-cancel tables are rejected; recovery is real;
- impact camera shake, kill cameras, critical zoom, and cinematic slow motion are rejected;
- interaction outlines, glow, focus-mode object highlighting, generic loot barrels/crates, and Take All are rejected;
- only 60 fps combat calculations and the PC/PS5 platform baseline survive.

## Still required before M1 acceptance

- authored animation montages, hit traces, damage resolution, rally, one complete enemy, lock-on, wound-aware locomotion poses, and the posture-only HUD;
- physical door and pickup actors, an Examine presentation, an interaction corner widget, and persistence tests;
- Windows UE 5.5 Development/Shipping compile, Unreal automation execution, controller/touch smoke testing, and measured 60 fps evidence.

No runtime gate may be inferred from static checks. This tranche is intentionally kept inside the existing private draft PR and must not be merged on its own.
