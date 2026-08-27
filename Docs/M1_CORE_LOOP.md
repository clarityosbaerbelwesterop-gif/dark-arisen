# M1 core-loop implementation record

**Status:** active source implementation; not yet a vertical slice, Alpha, or Beta  
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
- Jake owns the authored three-second Rally window. Damage-source recovery ranges from 70% for standard attacks to 0% for poison; landed light/heavy/parry-strike/critical hooks recover 15/25/40/100% of the currently available Rally pool.
- The native combat HUD draws one posture indicator only. It has no health, stamina, Rally, ammo, boss bar, minimap, damage-number, or status-icon path.
- Light and heavy attacks now queue one sphere trace at the authored startup frame. A valid combatant receives health and posture damage exactly once; a six-frame deflection redirects posture damage to the attacker instead.
- Successful light/heavy/parry-strike/critical contacts call Jake's Rally recovery path. Critical contacts bypass deflection and consume the full available Rally pool.
- The C++ greybox now spawns one visible duelling enemy with awareness, pursuit, committed attacks, stamina, health, five-state posture, deflection response, death handling, and no extra HUD.
- Lock-on chooses the best living combatant inside range, facing cone and line of sight, then smoothly owns facing until toggled or invalidated. It creates no marker, outline, widget or additional HUD element.
- Every wound layer exposes an animation-ready physical profile: breathing, favoured side, limp, stagger-run and weapon drag. Jake applies the deterioration to locomotion even when the Katana keeps its clean animation set.
- The camera boom receives only subtle low-frequency wound drift, scaled exclusively by the wound layer. There is no damage event, impact shake, critical zoom or kill framing path.
- The greybox contains an interruptible physical door, a 0.6-second physical pickup and a held Examine document. The document uses player-paced Anchored camera while the world remains visible.
- The HUD renders the sole four-second interaction prompt in the screen corner and a translucent Examine reading surface; neither path mutates, highlights or outlines a world target.
- Door open/broken and pickup taken states can be captured/restored by stable IDs into a SaveGame snapshot. The snapshot performs no disk write and therefore cannot bypass the locked chapter/rest autosave policy.

## Conflict rulings applied

The later locked rules supersede contradictory Phase 3/6 draft clauses:

- attack-cancel tables are rejected; recovery is real;
- impact camera shake, kill cameras, critical zoom, and cinematic slow motion are rejected;
- interaction outlines, glow, focus-mode object highlighting, generic loot barrels/crates, and Take All are rejected;
- only 60 fps combat calculations and the PC/PS5 platform baseline survive.

## Still required before M1 acceptance

- authored animation assets/poses and animation-notify refinement of the source-level hit timing;
- authored hand/door/pickup animation assets and in-engine persistence/presentation tests;
- Windows UE 5.5 Development/Shipping compile, Unreal automation execution, controller/touch smoke testing, and measured 60 fps evidence.

No runtime gate may be inferred from static checks. This tranche is intentionally kept inside the existing private draft PR and must not be merged on its own.

`DESIGN-GAP:` the source specifies wound-driven low-frequency camera instability and locomotion degradation but gives no numeric amplitude or speed multipliers. Conservative editable defaults are present for playtesting; Flo's feel review must lock them before M1 acceptance.
