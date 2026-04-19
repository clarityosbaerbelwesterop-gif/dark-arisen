# CONTROLS FOUNDATION

**Status:** Design Draft v1.0  
**Phase:** Phase 3 — Mechanics (Subsystem 1 of 10)  
**Tier:** Foundation Tier — All other mechanics depend on this  
**Related:** mechanics_overview.md, combat_system.md, controls_ps5.md (this document supersedes controls_ps5.md)  
**Platforms:** PlayStation 5 (primary), PC (Steam), Xbox Series X/S  
**Inspirations:** Bloodborne (core combat layout), Red Dead Redemption 2 (interaction density), Sekiro (parry timing sensitivity), Ghost of Tsushima (stance switching feel)

---

## 1. CONTROL PHILOSOPHY — THE FIVE PRINCIPLES

Dark Arisen's controls are designed around five non-negotiable principles. Every button mapping decision must respect these.

### 1.1 Principle 1: Combat Buttons Never Move

In Bloodborne, Dark Souls, and Sekiro, the combat buttons stay constant regardless of context. R1 is always light attack. R2 is always heavy attack. Circle/B is always dodge. Square/X is always... something combat-related. The player's combat muscle memory becomes sacred.

**Dark Arisen follows this.** Once Jake has drawn his weapon, the combat buttons are fixed:
- **R1** — Light attack
- **R2** — Heavy attack
- **L1** — Weapon wheel (hold)
- **L2** — Parry / aim pistol
- **Square** — Parry (alternate), firearm shot in combat
- **Circle** — Dodge / roll
- **Cross** — Jump (Bloodborne uses this for step-dodge; we preserve jump but adjust dodge to Circle)

**The player never loses combat reflexes by entering a new context.** A dodge is always Circle. An attack is always R1. This is sacred.

### 1.2 Principle 2: Context-Sensitive Interaction

Like RDR2, Dark Arisen uses context-sensitive buttons for non-combat interaction. The Triangle button and D-Pad directions change meaning based on what Jake is doing:

**On Land (walking):**
- **Triangle** — Primary interaction (pick up, talk, investigate)
- **D-Pad Up** — Whistle (call horse / rally crew)
- **D-Pad Down** — Toggle weapon holster

**On Ship (at helm):**
- **Triangle** — Switch to deck view
- **D-Pad Up** — Issue "reload cannons" order
- **D-Pad Down** — Issue "prepare to board" order

**In Cabin:**
- **Triangle** — Interact with object (map table, journal, etc.)
- **D-Pad** — Navigate ship menu

**In Stealth:**
- **Triangle** — Stealth kill / takedown (when available)
- **D-Pad Up** — Crouch toggle
- **D-Pad Down** — Prone toggle

The player learns: **combat buttons = muscle memory**, **context buttons = situational awareness**.

### 1.3 Principle 3: Haptic Feedback Is Content

On PS5 DualSense, haptic feedback is not decoration — it is actual gameplay information:

- **Walking on different surfaces** feels different (sand soft, stone hard, mud sticky, wet wood slippery)
- **Parry timing** gives a specific haptic "click" when hit at the perfect frame
- **Low HP** causes slow heartbeat haptic in the left grip
- **Low stamina** causes quickening pulse in the right grip
- **Enemy detection** (during stealth) causes a subtle buzz growing stronger as they notice Jake
- **Bow tension** in L2 adaptive trigger increases with draw strength

This is information the player FEELS before they SEE. It is part of gameplay.

### 1.4 Principle 4: The Rally Commitment

The Bloodborne Rally system is adapted to Dark Arisen. When Jake takes damage, he has 3 seconds to regain a portion of it by dealing damage back. This creates a specific control philosophy:

**Aggression is rewarded, but only if committed.** The player cannot retreat after taking damage and expect to heal — they must engage, strike, and trust their offense. The Rally window (3 seconds) shapes the rhythm of every fight.

### 1.5 Principle 5: No Menu Pause in Combat

Like Bloodborne and Sekiro, combat cannot be paused through menus. The Weapon Wheel (L1 hold) **slows time to 30%** but does not stop it. The Item Wheel (L1 + R1 hold) similarly slows time. This keeps combat tension continuous. Retreat is possible, but the world doesn't wait.

---

## 2. PLAYSTATION 5 — PRIMARY CONTROL LAYOUT

This is the canonical control scheme. All other platforms adapt from here.

### 2.1 Default Combat Controls

| Button | Action |
|--------|--------|
| **L Stick** | Move Jake |
| **R Stick** | Camera control |
| **L Stick click (L3)** | Sprint toggle (hold to sprint, click to toggle) |
| **R Stick click (R3)** | Lock-on target (+ R3 during Rache for fuel) |
| **L3 + R3 simultaneously** | Activate Rache (after Chapter 9) |
| **L1** | Weapon Wheel (hold) |
| **L2** | Parry / Aim Pistol |
| **L1 + R1 (hold)** | Item Wheel |
| **R1** | Light Attack |
| **R2** | Heavy Attack |
| **Square** | Firearm Shot (pistol, if equipped) |
| **Cross** | Jump / Vault |
| **Circle** | Dodge / Roll |
| **Triangle** | Interact / Context Action |
| **D-Pad Up** | Whistle Horse / Rally Crew |
| **D-Pad Down** | Holster Weapon |
| **D-Pad Left** | Consume Quick Item (bandage / rum) |
| **D-Pad Right** | Reload Firearm |
| **Touchpad** | Map / Journal |
| **Options** | Pause / System Menu |
| **Create (Share)** | Screenshot / Video |
| **PS Button** | System |

### 2.2 Stealth Mode Overrides

When Jake is crouched or prone, specific buttons change behavior:

| Button | Stealth Behavior |
|--------|------------------|
| **L Stick** | Slower movement (silent crouching) |
| **R1** | Silent attack (if weapon equipped) |
| **Triangle** | Stealth takedown (when behind enemy) |
| **D-Pad Up** | Stand up from crouch/prone |
| **D-Pad Down** | Deeper stance (crouch → prone) |
| **Square** | Throw distraction (if item equipped — bottle, rock, etc.) |

### 2.3 Ship Helm Controls

When Jake is at the helm of La Liberación:

| Button | Action |
|--------|--------|
| **L Stick** | Steer ship (yaw left/right) |
| **R Stick** | Camera |
| **L2** | Spyglass zoom |
| **R2** | Fire signal flare / fleet command |
| **L1** | Cycle sail configuration (Full/Half/Reefed) |
| **R1** | Tack (quick turn against wind) |
| **Square** | Fire port broadside |
| **Circle** | Fire starboard broadside |
| **Triangle** | Switch to Deck View |
| **Cross** | Climb to Crow's Nest |
| **D-Pad Up** | Order: Reload Cannons |
| **D-Pad Down** | Order: Prepare to Board |
| **D-Pad Left** | Order: Evasive Maneuvers |
| **D-Pad Right** | Order: Ramming Speed |
| **Touchpad** | Ship Status / Crew Overview |

### 2.4 Ship Deck Controls

When Jake walks La Liberación's deck:

| Button | Action |
|--------|--------|
| **L Stick** | Move Jake |
| **R Stick** | Camera |
| **R1** | Interact with station (cannon, helm, crew) |
| **Triangle** | Return to Helm View |
| **Square** | Melee combat (if enemies on deck during boarding) |
| **Cross** | Jump / Climb rigging |
| **Circle** | Dodge |
| **D-Pad Up** | Rally crew to position |
| **D-Pad Down** | Command crew retreat |

### 2.5 Cabin / Interior Controls

When Jake is in La Liberación's cabin or any interior space:

| Button | Action |
|--------|--------|
| **L Stick** | Move Jake (walking speed only) |
| **R Stick** | Camera |
| **Triangle** | Interact (map table, journal, wardrobe, bed) |
| **Square** | Sit / Stand |
| **L1** | Access quick inventory |
| **Circle** | Exit interior / return to deck |
| **D-Pad Up** | Open character menu |
| **D-Pad Down** | Open quest journal |
| **Touchpad** | Full menu |

### 2.6 Menu Navigation

Universal across menus:
- **L Stick / D-Pad** — Navigate options
- **Cross** — Confirm / Select
- **Circle** — Back / Cancel
- **Triangle** — Quick option / Sort
- **Square** — Inspect / Details
- **L1 / R1** — Switch tabs
- **L2 / R2** — Page up / Page down

### 2.7 The Rache Activation Detail

Rache (Dead Eye equivalent) is activated by pressing **both L3 and R3 simultaneously**. This is intentionally difficult — it is not a casual toggle. The player must commit to the activation.

- **Pre-Rache state:** Jake takes a deep breath (visible animation, 0.3 seconds)
- **Activation:** Screen desaturates to red-orange, audio muffles, time slows to 30%
- **Duration:** 5 seconds base (expandable with upgrades)
- **Fuel consumption:** 1 hand-rolled cigar + 1 sip of rum per use
- **Cooldown:** 60 seconds after effect ends

If the player does not have fuel, the L3 + R3 combo gives a haptic feedback "refusal" — the controller does a short negative buzz indicating the ability is unavailable.

---

## 3. PC — KEYBOARD & MOUSE LAYOUT

### 3.1 Default Combat Controls

Every PS5 button maps to a keyboard key. Player can fully remap.

| PS5 Button | Default Key |
|-----------|-------------|
| L Stick | WASD |
| R Stick | Mouse |
| L3 (Sprint toggle) | Shift (hold) / Caps Lock (toggle) |
| R3 (Lock-on) | Middle Mouse Button |
| L3 + R3 (Rache) | R key |
| L1 (Weapon Wheel) | Q (hold) |
| L2 (Parry / Aim) | Right Mouse Button |
| L1 + R1 (Item Wheel) | Q + F (hold) |
| R1 (Light Attack) | Left Mouse Button |
| R2 (Heavy Attack) | Left Mouse Button (hold) |
| Square (Firearm) | E |
| Cross (Jump) | Space |
| Circle (Dodge) | C |
| Triangle (Interact) | F |
| D-Pad Up (Whistle) | V |
| D-Pad Down (Holster) | X |
| D-Pad Left (Quick Item) | 1 |
| D-Pad Right (Reload) | R |
| Touchpad (Map) | M |
| Options (Pause) | Esc |
| Create (Screenshot) | F12 |

### 3.2 Mouse Sensitivity

Separate sensitivities for:
- Aiming (firearms, bow)
- Camera (general look)
- Menu navigation
- Map navigation

Each is independently adjustable from 0.1x to 3.0x.

### 3.3 Keyboard Remapping

All actions are remappable through Options menu. Key conflicts are detected and warnings shown. Some players may prefer:

- Quake-style FPS layout (WASD + mouse camera, Space jump, Shift crouch)
- MMO-style layout (number keys for abilities)
- Custom ergonomic layouts

Pre-configured presets:
- **Classic FPS** — WASD, Space jump, Shift sprint, E interact
- **Souls-like** — WASD, Space dodge, Ctrl sprint, F interact (mimics Dark Souls on PC)
- **RDR2-style** — WASD, Space jump, Shift sprint, E interact, holster on Alt
- **Custom** — Fully customizable

### 3.4 Controller Support on PC

Any XInput or DualSense controller automatically works with the PS5 layout. DualSense on PC supports haptic feedback and adaptive triggers via recent Steam updates.

---

## 4. XBOX SERIES X/S — CONTROLLER LAYOUT

### 4.1 Default Layout

Xbox button labels replace PlayStation labels. Functionally identical.

| PS5 | Xbox |
|-----|------|
| Cross | A |
| Circle | B |
| Square | X |
| Triangle | Y |
| L1 | LB |
| L2 | LT |
| R1 | RB |
| R2 | RT |
| L3 | L3 (click left stick) |
| R3 | R3 (click right stick) |
| D-Pad | D-Pad |
| Touchpad | View button |
| Options | Menu button |
| Create | Share button |
| PS Button | Xbox button |

### 4.2 Xbox-Specific Features

- **Rumble feedback:** Matches DualSense haptics in simplified form (Xbox controller has fewer haptic zones)
- **Trigger resistance:** Series X/S controllers support adaptive tension similar to DualSense
- **Quick Resume:** Full support — game state preserved exactly
- **Elite controllers:** Support additional paddle bindings for advanced players

---

## 5. CONTEXT-SENSITIVE CONTROL MATRIX

This is the master reference for how buttons change meaning across contexts.

### 5.1 Triangle Button Across Contexts

| Context | Triangle Action |
|---------|-----------------|
| Walking (land) | Interact / Talk / Loot |
| Combat (land) | Pistol finisher (if enemy staggered) |
| Stealth | Stealth takedown (if behind enemy) |
| Ship Helm | Switch to Deck View |
| Ship Deck | Return to Helm View |
| Cabin | Interact with object |
| Swimming | Dive underwater |
| Mounted (horse) | Dismount |
| Boarding ship | Grapple across / climb onto enemy deck |

### 5.2 Square Button Across Contexts

| Context | Square Action |
|---------|---------------|
| Walking | Melee attack with fists (if unarmed) |
| Combat | Firearm shot / Parry (depends on weapon equipped) |
| Stealth | Throw distraction item |
| Ship Helm | Fire port broadside |
| Ship Deck | Melee combat |
| Cabin | Sit down / Stand up |
| Swimming | Dash swim (short stamina burst) |

### 5.3 D-Pad Directions Across Contexts

| Context | D-Pad Up | D-Pad Down | D-Pad Left | D-Pad Right |
|---------|----------|------------|------------|-------------|
| Walking | Whistle horse | Holster weapon | Quick bandage | Reload |
| Combat | Whistle crew rally | Taunt | Bandage (combat) | Reload (combat) |
| Stealth | Stand up | Go prone | Silent reload | Silent weapon swap |
| Ship Helm | Reload cannons | Prepare to board | Evasive | Ramming speed |
| Ship Deck | Rally crew here | Crew retreat | Crew left flank | Crew right flank |
| Cabin | Character menu | Quest journal | Item menu | Ship menu |

### 5.4 Mode Switching Feedback

When Jake transitions between contexts, the game provides clear feedback:

- **Audio cue:** Each context has a subtle musical/sound shift
- **UI cue:** Button prompts change in bottom-right corner
- **Haptic cue (PS5):** Brief controller "pulse" when entering new context
- **Visual cue:** Jake's posture animation shifts subtly

The player should NEVER be confused about what context they are in.

---

## 6. CONTROL FEEL — THE DETAILS

### 6.1 Dodge (Circle) — The Sacred Button

The dodge is the single most used action in Dark Arisen. Its feel must be perfect.

- **Input window:** 8 frames after command (responsive)
- **i-frames:** 12 frames (frames 2-14 of 30-frame total dodge)
- **Stamina cost:** 25
- **Direction:** Determined by L Stick at moment of input
- **No L Stick input:** Backstep (shorter, less stamina, no i-frames but quick recovery)
- **Movement distance:** 4m in 0.5 seconds
- **Recovery:** 18 frames after i-frame window ends
- **Cannot be canceled** — commitment is part of the risk

### 6.2 Parry (L2 or Square) — The Mastery Gate

Parrying is where Dark Arisen demands mastery.

- **Parry window:** 15 frames (generous compared to Souls — more like Sekiro)
- **Perfect parry:** Frames 5-10 (deals double posture damage)
- **Parry stamina cost:** 10 (15 for failed parry that becomes block)
- **Failed parry:** Becomes a block, reducing damage 50% but taking full stamina and posture
- **Parry against pistols:** Requires L2 specifically (timing firearm discharge)
- **Parry against grabs:** Different window (10 frames), different button (Triangle)
- **Haptic on perfect parry (PS5):** Sharp single click
- **Haptic on failed parry:** Dull thump

### 6.3 Light Attack (R1) — The Bread and Butter

- **Input delay:** 4 frames (near-instant)
- **Recovery:** 20 frames (full combo end)
- **Stamina cost:** 15 per hit
- **Chain capacity:** Up to 4-hit combo without weapon change
- **Combo timing:** 20-frame window between hits to continue chain
- **Cancel options:** Can be canceled into dodge (after frame 15) or heavy attack (after frame 12)

### 6.4 Heavy Attack (R2) — The Commitment

- **Input delay:** 2 frames (slightly faster than light attack response)
- **Windup:** 25 frames (telegraphed)
- **Recovery:** 35 frames
- **Stamina cost:** 30
- **Posture damage:** 2.5x light attack
- **Charge option:** Hold R2 for charged heavy (50-frame windup, 3x damage, 4x posture)
- **Cannot be canceled** during active frames (frames 26-32)

### 6.5 Sprint (L3 hold or toggle) — Traversal Core

- **Activation delay:** 15 frames (Jake builds momentum)
- **Top speed:** 12 m/s (sprint) vs. 8 m/s (run)
- **Stamina cost:** 10/sec
- **Stamina-out effect:** Forced to walking speed for 3 seconds (no immediate sprint again)
- **Combat impact:** First attack from sprint deals +15% damage
- **Dodge impact:** Dodge from sprint has +30% distance, +2 i-frames
- **Obstacles:** Jake automatically vaults over low obstacles while sprinting (Principle: Don't break flow for minor geometry)

### 6.6 Jump (Cross) — The Overlooked Button

- **Input delay:** 6 frames
- **Base height:** 1.2m (can clear waist-high obstacles)
- **With momentum:** 2.0m (can clear chest-high obstacles)
- **Stamina cost:** 12
- **Combat use:** Jump attacks are NOT standard (Dark Arisen is not an action-platformer). Jump is primarily traversal.
- **Exception:** Specific enemy attacks (Jaw of the Mire tail sweep, Caldera Titan tremor) REQUIRE jumping to avoid

### 6.7 Lock-On (R3) — Target Tracking

- **Activation:** Click R3 to engage nearest enemy in cone of vision
- **Lock range:** 20m maximum
- **Deactivation:** Click R3 again, enemy dies, or Jake moves >25m away
- **Camera behavior:** Camera tracks the locked target automatically
- **Multiple enemies:** Flick R Stick left/right to switch targets
- **Weak point targeting:** During certain boss fights, lock-on allows cycling to weak points (Ashen Wyrm's exposed chest, Jaw's embedded harpoon)

---

## 7. PLATFORM-SPECIFIC FEATURES DETAIL

### 7.1 PS5 DualSense Deep Features

**Haptic Feedback Zones:**
- Left grip — HP, heartbeat (low HP), breath (Rache activation)
- Right grip — Stamina, fatigue, weapon weight
- Top — Environmental (rain drops, ship rocking, nearby explosions)
- Triggers — Weapon-specific (bow tension, firearm trigger pull)

**Adaptive Triggers:**
- L2 during pistol aim: Light resistance, holds steady when aligned
- L2 during bow draw: Progressive resistance as draw increases (1kg at 30%, 2kg at 60%, 3kg at 90%)
- R2 during firearm reload: Resistance spike during bullet insertion
- R2 during ship ramming: Heavy resistance during collision

**Built-in Mic:**
- Silent-mode: Enemies react to Jake's actual breathing in stealth (optional, off by default)
- Voice commands: "Whistle horse," "Reload," "Bandage" work as alternate inputs

**Speaker:**
- Distant audio cues come through controller speaker
- Crew member voices during ship combat play from controller
- Creates immersive audio separation

### 7.2 Xbox Series X/S Specific

**Haptic Trigger Feedback:**
- Less zoned than DualSense but supports resistance
- Trigger rumble on firearm discharge
- Trigger lock on weapon jam

**Smart Delivery:**
- Series S gets 1440p 60fps version
- Series X gets 4K 60fps or 4K 30fps raytracing version
- Seamless upgrade when transferring to Series X

**Quick Resume:**
- Game state preserved exactly on Xbox pause
- Returning players find Jake exactly where he was

### 7.3 PC Specific Features

**Mod Support:**
- Steam Workshop integration
- Approved modifications:
  - Weapon skins (cosmetic)
  - UI layout changes
  - Keybinding presets
  - Accessibility modifications (colorblind improvements, audio boosts)
- NOT approved: Gameplay changes (no cheats, no unlimited stamina mods)

**Graphics Scaling:**
- DLSS 3 support (NVIDIA RTX 40 series)
- FSR 3 support (AMD)
- XeSS support (Intel Arc)
- Ray tracing: Reflections, shadows, ambient occlusion (separately toggleable)
- Minimum: GTX 1060 / RX 580 equivalent (30 FPS 1080p Medium)
- Recommended: RTX 4060 / RX 7700 XT (60 FPS 1440p High)
- Enthusiast: RTX 4080+ / RX 7900 XT+ (4K 60 FPS Ultra with RT)

**Monitor Support:**
- Ultrawide (21:9, 32:9) natively supported
- HDR10 and Dolby Vision support
- Frame rate: Uncapped with vsync options

---

## 8. ACCESSIBILITY CONTROL OPTIONS

Accessibility must not feel like a "mode" — it must feel like preferences.

### 8.1 Motor Accessibility

**One-handed play options:**
- Hold-to-toggle conversions (sprint, aim, block) with customizable timing
- Combined inputs (L1+L2 combined to single button)
- Alternative mapping presets for limited-mobility players

**Reduced input complexity:**
- Auto-parry (when input registered during parry window, treats as perfect parry)
- Auto-dodge (when damage incoming within 15 frames, Circle dodges optimal direction)
- Auto-lock-on (engages nearest enemy automatically)
- Simplified Rache (single button press instead of L3+R3)

**Input timing extensions:**
- Parry window extended: +25%, +50%, or +100%
- Dodge i-frames extended: +25%, +50%, or +100%
- Combat input buffer extended: +50%

### 8.2 Visual Accessibility

**Colorblind modes:**
- Protanopia, Deuteranopia, Tritanopia each with adjustable intensity
- UI element distinction via shapes (not just colors)
- Enemy attack indicators use shape + color

**Visual aids:**
- High contrast UI option
- Motion reduction (reduced camera shake)
- Screen flash reduction (photosensitivity)
- Custom UI scale (75% to 200%)
- Text size scale (100% to 250%)
- Subtitle customization (background opacity, font selection)

**Screen reader support:**
- Menu navigation narration
- Item description narration
- Quest log narration
- In-game object announcements (approaching objects described)

### 8.3 Audio Accessibility

**Hearing aids:**
- Subtitle all combat sounds (including "enemy attacking from left")
- Directional audio indicators (small arrows showing sound source)
- Visual rhythm indicators (flashing UI element for audio cues)
- Separate volume sliders for all audio categories

**Audio descriptions:**
- Narrated cutscenes option
- Environmental description mode (Jake describes surroundings aloud)

### 8.4 Cognitive Accessibility

**Simplified UI:**
- Combat indicator simplification (what's happening, when to act)
- Clear objective markers
- Reduced HUD clutter option

**Tutorial accessibility:**
- Tutorials can be repeated at any time from main menu
- Optional training area with no consequences
- "Pause and explain" option (combat pauses, explains current situation)

**Reading time:**
- Dialogue can be slowed 50% or 75%
- Auto-advance dialogue can be disabled entirely
- Speaker portraits always visible

### 8.5 Difficulty Accessibility

Three difficulty modes, but accessible within each:

**Story Mode:**
- Enemy damage -40%
- Jake damage +40%
- All accessibility aids available
- No penalty on leaderboards (leaderboards track per difficulty)

**Standard Mode (Default):**
- Baseline experience

**Souls Mode:**
- Enemy damage +30%
- Jake damage -20%
- No auto-accessibility aids
- Hardcore bragging rights

**Custom:**
- All sliders independently adjustable
- Enemy damage: 25%, 50%, 75%, 100% (Standard), 125%, 150%
- Jake damage: Same scale
- Enemy aggression: Passive, Normal, Aggressive
- Enemy response time: Slow (12 frame reaction), Normal (8 frame), Fast (4 frame)

---

## 9. THE RACHE CONTROL DETAIL

Rache is the most unique control in Dark Arisen. It deserves its own section.

### 9.1 Activation

- **Default:** L3 + R3 (both stick clicks) pressed simultaneously within 3 frames
- **Accessibility option:** Single press of a remappable button
- **Pre-activation:** Jake visibly inhales deeply (0.3-second animation, uninterruptible)
- **Activation moment:** Screen effect begins, audio muffles, time slows

### 9.2 Active State

During Rache (5 seconds base, expandable):
- **Time slows to 30%** — enemy animations, projectile speeds, environmental events all slow
- **Jake maintains full speed** — this is the key advantage
- **Screen desaturates** to red-orange palette
- **Audio muffles** as if underwater, with deep heartbeat pulse
- **Haptic feedback:** Slow pulse in left grip (heartbeat)
- **All combat actions available** but with additional Rache-specific options:
  - **Circle (Dodge) during Rache:** Enhanced with +50% distance, +6 i-frames
  - **R2 (Heavy Attack) during Rache:** Chains into "perfect hit" animation if timed precisely
  - **L2 (Aim) during Rache:** Bullet/arrow/thrown weapon trajectory visible before release
  - **Triangle during Rache:** "Study enemy" — reveals enemy weak points and patterns for next 10 seconds after Rache ends

### 9.3 Consumption

- **1 hand-rolled cigar** consumed per activation (crafted from tobacco in regions)
- **1 "sip" of rum** consumed per activation (rum bottles have 5 sips each)
- Without both items, Rache cannot activate (haptic refusal feedback)

### 9.4 Cooldown

- **60 seconds** after Rache ends before it can be used again
- Cooldown visible on HUD as a small circular timer
- Cooldown can be reduced by specific Chapter 9+ upgrades

### 9.5 Why Rache Is Controlled This Way

The L3 + R3 simultaneous activation is deliberately awkward. Rache is not a casual ability. It is a specific, committed decision. The player must intentionally activate it — no accidental triggers, no spam.

The accessibility option (single button) exists because motor-limited players should still have access to this core mechanic. But for most players, the dual-stick commitment matches the thematic weight: Rache is Jake's revenge focus made manifest. It should feel like a decision, not a reflex.

---

## 10. CONTROL INSTRUCTION FLOW (TUTORIAL PROGRESSION)

Players learn controls in a specific order across Chapters 1-3:

### 10.1 Chapter 1 (Moran) — Foundation
- Basic movement (L Stick, R Stick)
- Jump (Cross)
- Sprint (L3)
- Interact (Triangle)
- First combat: Light attack (R1), Dodge (Circle)
- First parry opportunity (L2 or Square)

### 10.2 Chapter 2 (Moran deeper / Jungle Warden) — Combat Depth
- Heavy attack (R2)
- Lock-on (R3)
- Weapon wheel basics (L1 hold)
- Firearm mechanics (Square)
- Quick item (D-Pad Left)

### 10.3 Chapter 3 (Ashen Wyrm / mythical bosses) — Advanced
- Item wheel (L1 + R1)
- Stealth basics (crouch via D-Pad Up)
- Reload mechanics (D-Pad Right)
- Weapon holster / context switch (D-Pad Down)

### 10.4 Chapter 4 (Rexa) — Ship Integration
- Ship helm controls (complete new layout)
- Ship deck controls (transitions)
- Cabin controls (interior navigation)
- Whistle crew (D-Pad Up)

### 10.5 Chapter 6-7 (Ashenmoor / Quiet Coast) — Mastery
- Perfect parry timing drills
- Environmental interaction expansion
- Advanced stealth (prone, silent takedowns)

### 10.6 Chapter 9 — Rache Unlock
- Rache tutorial (first use during Ethan pre-fight)
- Rache in combat (Draven fight)
- Rache in exploration (post-game use)

---

## 11. CONTROL REFERENCE CARDS (IN-GAME HELP)

Players can access control reference at any time:
- **Options menu → Controls** — Full reference
- **Start Menu → Quick Reference** — Most common controls
- **Touchpad (or M on PC)** — Context-sensitive help (shows buttons relevant to current situation)

Reference cards are context-sensitive:
- Walking: Shows combat + interaction buttons
- Ship: Shows helm + combat buttons
- Stealth: Shows stealth-specific buttons
- Cabin: Shows interaction + menu buttons
- Combat: Minimal overlay (player is busy) but hold Options for full reference

---

## 12. IMPLEMENTATION NOTES

### 12.1 Priority for Development

- **Phase 1 (Foundation):** Land walking + combat controls must be rock-solid before anything else
- **Phase 2 (Extension):** Ship helm + deck controls integrate next
- **Phase 3 (Polish):** Accessibility, context-switching smoothness, haptic refinement

### 12.2 Testing Considerations

- **Controller dead zones:** Customizable from 5% to 25%
- **Input latency:** Target <80ms from input to on-screen response
- **Platform parity:** Xbox and PC must feel as close to PS5 as possible (except where platform features differ)
- **Network considerations:** For any multiplayer (if added), client-side prediction for movement, server-side validation for combat

### 12.3 Localization Considerations

- Button labels in all supported languages
- Voice command support in 5+ languages (English, German, French, Spanish, Japanese at launch)
- UI text all localized
- Cultural adaptation: D-Pad Up for "agree" in most cultures, adjusted if needed

### 12.4 Platform Certification Notes

- PS5 requires Trophy support (Platinum achievable without accessibility aids, but not blocked by them)
- Xbox requires Achievement support (Gamerscore same)
- Steam Deck Verified target (important for PC audience)
- Epic Games Store version (same as Steam)

---

## 13. CONSISTENCY WITH EXISTING DESIGN

This document supersedes `controls_ps5.md` but preserves all decisions made there. Specifically:

- All boss fight controls reference this document
- All combat system frame data remains valid
- All naval combat controls remain as designed
- All character ability unlocks (Rache, dual weapons) fit this framework

If any existing document conflicts with this one, this document wins (as Foundation Tier per mechanics_overview.md Section 8.7).

---

## 14. THE CONTROL FEEL TEST

Before this document is considered final, controls must pass this test:

> *"Can a player who has never played a Souls-like pick up Dark Arisen and, within 30 minutes, feel: 'I understand how to move, how to fight, how to explore'?"*

AND

> *"Can a veteran Bloodborne player pick up Dark Arisen and feel: 'The combat rhythm is familiar. The buttons respect my muscle memory. I can focus on learning THIS game's specific systems rather than relearning how to attack'?"*

Both must be yes. The first test ensures accessibility. The second ensures the reference game's DNA is honored.

---

## 15. NEXT STEPS

Next document: **`mechanics/combat_feel.md`** — Combat Feel Deep Dive

Will cover:
- Stamina management deep mechanics
- Posture system (Sekiro-style, adapted)
- Weapon archetypes (sword, axe, spear, hammer, dagger, bow, firearm, dual-wield)
- Frame data standards across weapon types
- Rally system specifics (HP recovery windows)
- Weapon bond system (ships and Jake's crew)
- Hit stop and impact feel
- Damage calculation formulas
- Critical hit system
- Status effects (bleed, poison, burn, frozen)

Estimated time: 4-5 hours of focused work.

**Controls are the foundation. With this document finalized, we can build combat, traversal, stealth, and everything else on top of a stable base. Phase 3 Subsystem 1 of 10 — complete.**
