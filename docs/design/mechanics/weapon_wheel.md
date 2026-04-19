# WEAPON WHEEL SYSTEM

**Status:** Design Draft v1.0  
**Phase:** Phase 3 — Mechanics (Subsystem 5 of 10)  
**Tier:** Interface Tier — Depends on Combat Feel  
**Related:** mechanics_overview.md, mechanics/controls.md, mechanics/combat_feel.md, style_bible.md  
**Purpose:** Define the complete L1-hold weapon radial system — how Jake switches weapons, manages ammunition, and accesses his arsenal during combat and exploration.  
**Inspirations:** Red Dead Redemption 2 (primary reference — the L1 hold radial), Bloodborne (trick weapon transformation), Witcher 3 (sign wheel), Horizon Zero Dawn (ammo switching), Dead Space (context-sensitive tools)

---

## 1. WHEEL PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: The Wheel Is a Combat Hub, Not Just a Menu

RDR2 pioneered the idea that the weapon wheel is a combat decision space, not a pause menu. Dark Arisen embraces this: opening the wheel slows time (not pauses it) and presents Jake's entire arsenal in a single glance.

**Design intent:** Every time the wheel opens is a tactical moment. The player analyzes the fight, chooses the right tool, commits.

### 1.2 Principle 2: Time Dilation, Not Suspension

Like Bloodborne's menu behavior, Dark Arisen's wheel slows time to 30% but does NOT pause combat. Enemies continue moving (slowly). Projectiles continue traveling. The Rally window continues ticking. This is crucial for combat flow.

**Design intent:** The player cannot escape combat by opening the wheel. Decision-making happens under pressure, just informed pressure.

### 1.3 Principle 3: Eight Slots, Carefully Chosen

The wheel has exactly **8 primary slots** arranged in a circle:

- **Top (12 o'clock):** Primary Melee Weapon
- **Top-right (1:30):** Secondary Melee Weapon
- **Right (3 o'clock):** Primary Firearm (Pistol)
- **Bottom-right (4:30):** Secondary Firearm / Special Weapon
- **Bottom (6 o'clock):** Bow / Ranged
- **Bottom-left (7:30):** Thrown Weapons / Distraction
- **Left (9 o'clock):** Dual-Mode Toggle (trick weapon transform)
- **Top-left (10:30):** Context-Specific Slot (stealth tool, ship weapon, etc.)

**Design intent:** Enough variety for strategic choice, constrained enough to require decisions.

### 1.4 Principle 4: Wheel Memory

The wheel remembers Jake's last used weapons. When the player opens the wheel, the pointer starts on the last-used weapon. This enables quick-switching between favorites without visual confusion.

**Design intent:** Muscle memory develops. Players learn to "pre-select" by knowing where the cursor starts.

### 1.5 Principle 5: Wheel Respects Context

The wheel's contents change based on Jake's context:

- **Land combat:** Standard 8 weapon slots
- **Ship helm:** Ship weapons (broadsides, chase guns, boarding orders)
- **Ship deck combat:** Mixed (personal weapons + boarding tools)
- **Stealth mode:** Silent options prioritized
- **Cabin/menu:** Wheel disabled, use inventory instead

**Design intent:** The wheel is always useful, never useless, always relevant to what Jake is doing.

---

## 2. WHEEL ACTIVATION — THE L1 HOLD

### 2.1 Input Specifications

**PS5/Xbox Controller:**
- **Hold L1/LB** — Wheel opens
- **R Stick** — Navigate to weapon slot (cursor follows stick direction)
- **Release L1/LB** — Wheel closes, selected weapon equipped

**PC:**
- **Hold Q** — Wheel opens
- **Mouse** — Navigate to weapon slot
- **Release Q** — Wheel closes, selected weapon equipped
- **Alternative:** Number keys 1-8 for instant slot selection

### 2.2 Timing and Feel

**Opening:**
- **Delay:** 120ms from L1 press to wheel visible
- **Animation:** Wheel scales from center, 180ms total
- **Audio:** Subtle "open" sound (mechanical click)
- **Haptic (PS5):** Single soft pulse in left grip

**While Open:**
- **Time dilation:** 30% speed (everything slows)
- **Cursor responsiveness:** Immediate R Stick response
- **Visual intensity:** Surrounding world slightly desaturated
- **Combat continues:** Enemies move (slowly), projectiles travel (slowly)

**Closing:**
- **Close speed:** 150ms fade-out
- **Commit animation:** 300ms weapon swap (Jake reaches for new weapon)
- **Audio:** Weapon-specific draw sound
- **Haptic:** Weapon-weight-appropriate pulse

### 2.3 Quick Access Alternatives

For experienced players, faster options exist:

**Tap L1 (without hold):**
- Quick-switches to previous weapon
- No wheel open
- Instant swap (only 150ms total)
- Useful for dual-weapon combat styles

**D-Pad direct access:**
- D-Pad Up: Primary weapon
- D-Pad Right: Reload (for current firearm)
- Specific combos: Custom quick-access

**Favorite Pairs:**
- Players can designate 2 weapons as "pair"
- Tap L1 swaps between them instantly
- Advanced users combine this with combat flow

### 2.4 Wheel Interaction Flow

Typical combat usage:
1. Player in combat, health dropping, decides to switch weapons
2. **Holds L1** — Wheel opens (120ms)
3. **Time slows** — Jake and player both have moment to think
4. **R Stick to top** — Cursor moves to Primary Melee
5. **R Stick to right** — Changes to Pistol
6. **Releases L1** — Wheel closes, Pistol equipped
7. **Total elapsed:** Approximately 1.2 seconds real-time

---

## 3. THE 8 WHEEL SLOTS — DETAILED

### 3.1 Slot 1: Primary Melee Weapon (Top, 12 o'clock)

**Purpose:** Jake's main sword/blade for combat.

**Default contents:** Long Sword (Chapter 4 baseline)  
**Player-configurable:** Yes, any melee weapon can be assigned

**Display Elements:**
- Weapon icon (specific to type)
- Bond Level badge (0-10)
- Durability/Condition indicator (if damaged)
- Weapon name below icon

### 3.2 Slot 2: Secondary Melee (1:30)

**Purpose:** Alternative melee for specific situations.

**Default contents:** Dagger (for stealth) or Axe (for heavy combat)  
**Player-configurable:** Yes

**Usage pattern:**
- Quick switch during combat for type advantage
- Against armored enemies: switch to Axe
- For stealth approach: switch to Dagger

### 3.3 Slot 3: Primary Firearm (3 o'clock)

**Purpose:** Main pistol for ranged combat and parry.

**Default contents:** Flintlock Pistol (single shot, slow reload)  
**Player-configurable:** Yes, but only firearms

**Display Elements:**
- Firearm icon
- Current ammunition count
- Total reserves
- Loaded status (unloaded warning)

### 3.4 Slot 4: Secondary Firearm (4:30)

**Purpose:** Backup firearm or special ranged weapon.

**Default contents:** Second pistol (doubles effective rate of fire)  
**Chapter 6+:** Revolver unlocked (6 shots, slightly faster reload)

**Advanced Option:** Ship Hand Cannon (massive damage, very slow)

### 3.5 Slot 5: Bow (6 o'clock)

**Purpose:** Silent ranged weapon, stealth-compatible.

**Default contents:** Composite Bow  
**Arrow Types Accessible via Sub-Wheel:**
- Standard Arrows (baseline)
- Poison Arrows
- Fire Arrows
- Sleep Arrows
- Paralysis Arrows
- Flash Arrows
- Smoke Arrows
- Explosive Arrows

**Sub-Wheel Activation:** Hold R3 while bow is selected

### 3.6 Slot 6: Thrown Weapons / Distractions (7:30)

**Purpose:** Distraction items and thrown weapons.

**Contents:**
- Throwing Knives
- Rocks (free, pickable from ground)
- Bottles (distraction)
- Smoke Bombs
- Fire Bombs
- Poison Flasks

### 3.7 Slot 7: Dual-Mode Toggle (9 o'clock) — SPECIAL

**Purpose:** Transform current melee weapon between two forms (Bloodborne-style).

**Mechanics:**
- Not all weapons have dual modes
- Those that do can transform mid-combat
- Each form has different stats, combo patterns
- Transformation takes 0.4 seconds

**Example: Trick Sword**
- Form 1: Standard saber (medium range, fast)
- Form 2: Whip-blade (long range, slower, crowd control)

**Example: Pistol-Blade**
- Form 1: Pistol (ranged)
- Form 2: Attached blade as shortsword (melee)
- Mid-combat transformation creates combo opportunities

### 3.8 Slot 8: Context Slot (10:30) — ADAPTIVE

**Purpose:** Changes based on Jake's current context.

**Context Variations:**

**Standard Combat:**
- Healing items quick-access
- Stamina tonic
- Food (snacks for minor HP recovery)

**Stealth Mode:**
- Silent takedown tools
- Bone saw (for dismemberment without trace)
- Garrote

**Ship Combat:**
- Signal flare
- Grappling hook
- Crew rally command

**Boarding Action:**
- Grappling hook
- Boarding axe (specialized)
- Ship-to-ship plank

**Fjordlund Cold:**
- Warming tonic
- Hand warmer
- Insulating cloth

**Tropical Heat:**
- Water flask
- Cooling salve
- Salt tablets

---

## 4. AMMUNITION AND SUB-WHEELS

### 4.1 Sub-Wheel Activation

When a ranged weapon is selected, holding **R3** while the wheel is open opens an ammunition sub-wheel for that weapon.

**Process:**
1. Hold L1 (wheel opens)
2. Move cursor to Bow (Slot 5)
3. Hold R3 (sub-wheel appears around bow slot)
4. Navigate to desired arrow type
5. Release R3 (arrow selected)
6. Release L1 (wheel closes, bow equipped with chosen ammo)

### 4.2 Bow Arrow Sub-Wheel

**Eight Arrow Types Arranged Around Bow Slot:**

| Position | Arrow Type | Effect |
|----------|-----------|--------|
| Top | Standard | Normal damage, infinite reserves |
| Top-right | Poison | Poison buildup 30/hit |
| Right | Fire | Fire damage + ignite |
| Bottom-right | Sleep | Unconsciousness on hit |
| Bottom | Paralysis | Immobilize 3 seconds |
| Bottom-left | Flash | Blind 2 seconds |
| Left | Smoke | Concealment area |
| Top-left | Explosive | AOE damage |

**Ammunition Management:**
- Each arrow type has separate reserve count
- Standard arrows infinite
- Others crafted or purchased
- Visual indicator shows reserves (green = plenty, yellow = low, red = <5 remaining)

### 4.3 Firearm Ammunition Sub-Wheel

**Three Ammunition Types:**

| Position | Ammo Type | Effect |
|----------|-----------|--------|
| Top | Ball (Standard) | Baseline damage |
| Bottom-left | Poison Shot | Reduced damage + poison |
| Bottom-right | Dragon-Bone | High damage, ignores armor |

**Simpler Management:**
- Firearms have fewer ammo types than bows
- Ball shots most common
- Dragon-Bone extremely rare (crafted from specific materials)

### 4.4 Ammunition as Resources

Ammunition is a meaningful resource:
- Standard ammunition easily available
- Special types require crafting or purchase
- Running out of special ammo is common mid-fight
- Resource management adds depth

---

## 5. WEAPON DISPLAY — THE VISUAL LANGUAGE

### 5.1 Icon Design

Each weapon slot shows:

**Primary Visual:**
- Weapon silhouette (distinctive, readable at glance)
- Color-coded by type:
  - Swords: Silver
  - Daggers: Dark gray
  - Axes: Brown
  - Firearms: Dark bronze
  - Bows: Wooden brown
  - Thrown: Varies by item

**Secondary Information:**
- Bond Level badge (small number 0-10)
- Ammunition count (for ranged)
- Condition bar (if damaged)
- Active effect indicator (buffs, enchantments)

**Tertiary (on hover):**
- Full weapon name
- Brief stats summary (damage, range, stamina cost)
- Bond info (kills, special abilities unlocked)

### 5.2 Cursor Visualization

**Cursor Behavior:**
- Green halo on selected slot
- Pulsing animation (subtle, not distracting)
- Additional glow if weapon has special properties

**Unavailable Slots:**
- Grayed out (no weapon assigned)
- Red overlay (weapon broken, needs repair)
- Lock icon (context-inappropriate, e.g. firearms underwater)

### 5.3 Wheel Center Display

The center of the wheel shows:

**Primary Display:**
- Jake's health bar (miniature)
- Stamina bar
- Posture bar
- Active status effects

**Contextual Display:**
- Rache charges (when unlocked)
- Current buff timers
- Active item effects

**Environmental Info:**
- Current weather icon
- Time of day indicator
- Location name (brief)

### 5.4 Weapon Assignment Interface

Outside combat, players can customize wheel slots:

**Menu Access:** Options > Weapon Wheel Configuration
**Layout:** Grid view of all owned weapons
**Actions:**
- Drag weapon to wheel slot
- Set favorites
- Configure quick-switch pair
- Save loadouts (3 preset slots)

---

## 6. WEAPON SWAP MECHANICS

### 6.1 The Swap Animation

When Jake switches weapons:

**Standard Swap (wheel-initiated):**
- Sheath current weapon (0.3 seconds)
- Draw new weapon (0.3 seconds)
- Total: 0.6 seconds
- Jake is vulnerable during this (cannot attack, can dodge)

**Quick Swap (L1 tap):**
- Faster animation (0.15 seconds total)
- Less vulnerability window
- Only works for designated pair

**Emergency Swap:**
- Circle (dodge) during swap animation
- Cancels swap, keeps current weapon
- Useful escape from commitment

### 6.2 Combat-Integrated Swaps

Advanced players can weave weapon swaps into combat:

**Combo Extension:**
- Light attack combo (sword) → Quick swap (dagger) → Continuation
- Total combo length increases
- Higher damage output
- Requires timing mastery

**Parry-Swap:**
- Perfect parry → L1 quick swap → Counter-attack with different weapon
- Uses the post-parry window
- Advanced technique
- High damage potential

**Defense Swap:**
- Mid-dodge → L1 to lighter weapon → Faster recovery after dodge
- Tactical retreat with weapon adjustment

### 6.3 Swap Cooldowns

Weapon swapping has brief cooldown to prevent spam:
- **Same-slot refresh:** 200ms minimum
- **Different weapon swap:** 400ms minimum
- **Sub-wheel access:** 200ms

### 6.4 Dual-Wielding Mechanics

Some weapon combinations allow dual-wielding:

**Dual Pistols:**
- Equip pistol in both primary and secondary firearm slots
- Can fire both simultaneously (alternating triggers)
- +50% fire rate but +75% ammunition consumption

**Dual Blades (Kira's style):**
- Equip two matched short blades
- Unique combo set with complex rhythms
- Higher skill ceiling, higher damage potential

**Sword + Dagger:**
- Parry with dagger, attack with sword
- Enables "Parry & Strike" combo
- Tactical combat option

### 5.5 Weapon Bond Integration

Wheel visually represents weapon bonds:

**Bond Level Indicators:**
- Level 0-2: Simple icon
- Level 3-5: Subtle glow
- Level 6-8: Pronounced effects on icon
- Level 9-10: Fully stylized, almost heraldic presentation

**Named Weapons:**
- Weapons at Bond 7+ can be renamed
- Custom name displays in wheel
- Personal investment visible at a glance

---

## 7. CONTEXT-SPECIFIC WHEEL VARIATIONS

### 7.1 Ship Helm Wheel

When Jake is at the helm, the wheel transforms:

**Slot Changes:**
- Slot 1 (Top): Port Broadside
- Slot 2 (1:30): Starboard Broadside
- Slot 3 (Right): Chase Guns (bow)
- Slot 4 (4:30): Stern Guns
- Slot 5 (6 o'clock): Signal Flare
- Slot 6 (7:30): Fleet Command (formation)
- Slot 7 (Left): Ramming Speed toggle
- Slot 8 (10:30): Crew Orders (boarding prep, etc.)

**Sub-wheels for cannons:**
- Ammunition type (roundshot, chainshot, grapeshot)
- Timing (volley vs staggered)
- Targeting priority

### 7.2 Ship Deck Combat Wheel

When Jake is on his ship's deck (during boarding):

**Mixed Arsenal:**
- Slot 1: Boarding Cutlass (specialized close-quarters)
- Slot 2: Boarding Axe (for ropes and enemies)
- Slot 3: Flintlock Pistol (close-range pistol)
- Slot 4: Blunderbuss (crowd control)
- Slot 5: Throwing Knives (fast, silent)
- Slot 6: Grenades (pirate-era, explosive)
- Slot 7: Dual-Mode Toggle (if applicable)
- Slot 8: Grappling Hook (for ship-to-ship crossing)

### 7.3 Stealth Mode Wheel

When Jake is crouched or prone, wheel prioritizes silent options:

**Silent-First Layout:**
- Slot 1: Dagger (always top priority)
- Slot 2: Bow (silent ranged)
- Slot 3: Garrote (silent takedown)
- Slot 4: Poison Tools
- Slot 5: Distraction Items
- Slot 6: Smoke Bomb
- Slot 7: Climbing Claws (for silent climbing)
- Slot 8: Context (current environment stealth tool)

**Firearms are hidden/grayed out** (they break stealth).

### 7.4 Cabin/Safe Area Wheel

When Jake is in safe areas (cabin, inn rooms, villages):
- **Wheel disabled entirely**
- Use full inventory menu instead
- Allows for unhurried decisions

---

## 8. INTEGRATION WITH COMBAT SYSTEMS

### 8.1 Wheel and Stamina

Opening the wheel does NOT consume stamina. Weapon swaps themselves do not cost stamina. However:
- The first attack after a swap takes slightly longer (stamina calculation same)
- No penalty for strategic wheel use

### 8.2 Wheel and Posture

Wheel use does not affect posture regen. Actually, time spent with wheel open helps:
- Posture regenerates normally (not accelerated, but not interrupted)
- Allows brief "breather" moments
- Strategic pauses in combat intensity

### 8.3 Wheel and Rally

The Rally window continues during wheel use:
- Time dilation slows it visually
- But Rally timer still counts in real time
- Player must decide: swap weapons or attack to recover HP?

### 8.4 Wheel and Rache

When Rache is active:
- Wheel opens faster (0ms delay)
- Time dilation compounds with Rache (approaching pure pause)
- Swap animations are faster (50% speed increase during Rache)
- Enables rapid weapon combo chains

### 8.5 Wheel and Lock-On

While locked onto target:
- Camera adjusts during wheel use
- Target tracking continues
- Swap to appropriate weapon for target type
- Lock-on maintained through swap

---

## 9. WHEEL CUSTOMIZATION

### 9.1 Player Preferences

Options menu allows customization:

**Visual Settings:**
- Wheel size (small/medium/large)
- Opacity (25% - 100%)
- Color scheme (default, high contrast, colorblind options)
- Information density (minimal/standard/detailed)

**Timing Settings:**
- Time dilation intensity (20% - 40%)
- Wheel open delay (instant - 250ms)
- Animation speed (50% - 200%)

**Input Settings:**
- Hold L1 vs Tap L1 for wheel
- Alternative keybind for PC users
- Stick sensitivity for cursor

### 9.2 Loadout System

Players can save multiple wheel configurations:

**Preset Slots:** 3 (Expand at higher Bond)
**Per-Preset Customization:**
- Weapon assignments to each slot
- Ammunition preferences
- Quick-switch pair designation

**Preset Switching:**
- Menu access to change presets
- Cannot change mid-combat
- Location-aware (certain zones auto-suggest presets)

### 9.3 Context Auto-Save

The wheel remembers:
- Last weapon used in each context (land, ship, stealth)
- Ammunition last selected
- Preferred equipment

**On context change:**
- Wheel adapts to appropriate context
- Remembers previous context's state
- Returning to previous context restores that state

---

## 10. PLATFORM-SPECIFIC WHEEL FEATURES

### 10.1 PS5 DualSense

**Haptic Feedback:**
- Subtle pulse when wheel opens
- Directional feedback during cursor navigation (indicates which slot is nearest)
- Weapon-weight pulse when committing selection
- Dual-mode transform has distinctive mechanical feel

**Adaptive Triggers:**
- L1 has subtle resistance click when wheel opens
- Cursor stops have slight feedback at each slot position

**Visual Enhancement:**
- DualSense LED bar changes color to match selected weapon type

### 10.2 Xbox Series X/S

**Similar Feedback:**
- Controller rumble matches PS5 haptics in simplified form
- Trigger resistance where supported
- Slightly reduced tactile nuance but functionally identical

### 10.3 PC

**Keyboard/Mouse:**
- Q hold for wheel, mouse for navigation
- Number keys (1-8) for direct slot selection
- Middle-click for sub-wheel access
- Customizable keybindings

**Controller support:**
- Full DualSense features via Steam
- Xbox controller support
- Generic controller fallback

---

## 11. ACCESSIBILITY

### 11.1 Motor Accessibility

**Simplified Wheel Interaction:**
- Toggle wheel (click L1 to open, click again to close)
- Hold-to-open alternative with longer activation time (300ms)
- Fewer slots option (4-slot simplified wheel)
- Auto-selection after cursor dwell (cursor stays on slot 1 second = selected)

**Combat Easing:**
- Pause during wheel (full stop instead of slow) — for accessibility mode only
- Quick swap extended windows

### 11.2 Visual Accessibility

**High Contrast:**
- Wheel slot boundaries emphasized
- Weapon icons can be replaced with text labels
- Colorblind-friendly palette options

**Size Adjustments:**
- Wheel size 75% - 150%
- Icon size 75% - 150%
- Text size independent scaling

### 11.3 Cognitive Accessibility

**Simplified Information:**
- Option to show only weapon names (no stats)
- Reduced information density
- Clear current selection indicator

**Tutorial Access:**
- Wheel tutorial repeatable from menu
- Contextual help (explains each slot when highlighted)
- Practice mode (no combat pressure)

### 11.4 Audio Accessibility

**Audio Cues:**
- Each weapon slot has distinct tone when highlighted
- Confirmation sound varies by weapon type
- Verbal descriptions available (narrator reads slot names)

---

## 12. WHEEL TUTORIALS

### 12.1 Chapter 1 Introduction

**First Wheel Use:**
- Player has 2 weapons (sword, pistol)
- Simple 2-slot wheel demonstration
- Time dilation feel introduced
- L1 hold gesture taught

### 12.2 Chapter 2 Expansion

**Multi-Weapon Wheel:**
- 4 slots active
- Bow and thrown weapons added
- Sub-wheel tutorial (bow arrows)
- Quick-swap (L1 tap) introduced

### 12.3 Chapter 4 Ship Integration

**Ship Helm Wheel:**
- Naval wheel variation
- Cannon types and ammunition
- Fleet command options

### 12.4 Chapter 6 Advanced

**Dual-Mode Weapons:**
- Trick weapon transformations
- Combo extensions
- Advanced combat flow

### 12.5 Chapter 9 Rache Integration

**Rache-Enhanced Wheel:**
- Faster wheel during Rache
- Strategic weapon cycling
- Ultimate combat fluidity

---

## 13. WHEEL UI PHILOSOPHY

### 13.1 The "Glance Principle"

All critical information must be comprehensible in 200ms glance:
- Weapon type (icon)
- Ammo status (number)
- Bond level (badge)
- Condition (bar)

**No information buried.** If it matters, it's visible.

### 13.2 The "Commit Principle"

Releasing L1 is final. No "are you sure?" dialogs. No accidental selections.
- Cursor clearly shows target
- Release = commitment
- Mistakes are part of play

### 13.3 The "Flow Principle"

Wheel use should feel fluid, not interrupt rhythm:
- Fast animations
- Minimal interruption
- Seamless integration with combat

### 13.4 The "Memory Principle"

Wheel remembers player patterns:
- Last weapon used
- Preferred pairings
- Context-specific choices

Players develop muscle memory through consistent behavior.

---

## 14. COMMON PLAYER PATTERNS

### 14.1 The "Sword Main"

**Typical setup:**
- Slot 1: Primary Long Sword
- Slot 3: Pistol
- Slot 5: Bow

Mostly uses Slot 1, occasionally Slot 3 for parry/ranged.

### 14.2 The "Assassin"

**Typical setup:**
- Slot 1: Daggers
- Slot 2: Short Sword (backup)
- Slot 5: Bow with Poison Arrows
- Slot 6: Smoke Bombs

Stealth-focused, uses wheel constantly.

### 14.3 The "Dual-Wielder"

**Typical setup:**
- Slot 1: Primary Matched Blade
- Slot 2: Second Matched Blade
- Slots 1 and 2 as dual pair

Uses L1 tap constantly for combo extensions.

### 14.4 The "Ranged Specialist"

**Typical setup:**
- Slot 1: Melee (basic)
- Slot 3: Primary Pistol
- Slot 4: Secondary Pistol
- Slot 5: Bow with multiple ammo types

Uses sub-wheels extensively for ammo switching.

### 14.5 The "Hybrid"

**Typical setup:**
- All 8 slots filled with variety
- Context-switching based on situation
- Uses wheel most of any archetype

High skill ceiling, maximum flexibility.

---

## 15. WHEEL FEEL TEST

Before the weapon wheel is final, it must pass these tests:

### 15.1 The RDR2 Test
> *"Does the L1 hold feel natural? Is the time dilation calibrated correctly? Does the wheel feel like it belongs in a serious action game?"*

### 15.2 The Combat Fluidity Test
> *"Can a skilled player use the wheel mid-combat without disrupting their flow? Does it become a tool, not an obstacle?"*

### 15.3 The Strategic Depth Test
> *"Do different weapon combinations create meaningfully different playstyles? Is there real variety to be found?"*

### 15.4 The Context Awareness Test
> *"Does the wheel adapt appropriately to ship combat, stealth, and cabin contexts? Does it always feel contextually relevant?"*

### 15.5 The Accessibility Test
> *"Can players with motor limitations use the wheel effectively through accessibility options?"*

All five must pass. Then the wheel is ready.

---

## 16. IMPLEMENTATION NOTES

### 16.1 Visual Requirements
- Wheel graphic (high-quality, animates smoothly)
- 8 slot positions with clear visual separation
- Dynamic weapon icon system (scales with weapon type)
- Bond level badge rendering
- Ammunition count overlay
- Context indicator (environment/mode)

### 16.2 Animation Requirements
- Wheel open/close animations (scale + fade)
- Cursor movement (smooth interpolation)
- Selection feedback (glow + pulse)
- Weapon swap animations (sheath + draw)
- Sub-wheel emergence animations

### 16.3 Audio Requirements
- Wheel open sound (mechanical, satisfying)
- Cursor movement tones (subtle, slot-specific)
- Selection confirmation (weapon-appropriate)
- Weapon draw sounds (varied by weapon)
- Dual-mode transformation sound (unique per weapon)

### 16.4 System Integration
- Time dilation must integrate cleanly with combat timers
- Rally timer continues accurately
- Enemy AI must handle time dilation
- Projectile physics must respect time scale
- Haptic feedback synchronized with animations

---

## 17. CONSISTENCY WITH EXISTING DESIGN

All existing documentation remains compatible:

- **Combat System:** Wheel supports all weapon archetypes defined in combat_feel.md
- **Boss Encounters:** Wheel use valid during all boss fights
- **Ship Combat:** Ship helm wheel integrates with naval_combat_system.md
- **Story Progression:** Weapon unlocks through chapters match wheel expansion

No existing documents need revision. Wheel system supports all previously-designed content.

---

## 18. NEXT STEPS

Next document: **`mechanics/item_wheel.md`** — Item Wheel System

Will cover:
- L1+R1 hold radial (parallel to weapon wheel)
- Consumable items (healing, buffs, tonics)
- Quick-use item management
- Food and tobacco system
- Item wheel customization
- Status effect cures
- Ship-specific items
- Emergency items

Estimated time: 2-3 hours.

After Item Wheel: Inventory & Crafting (the deep inventory system).

**The weapon wheel is the player's primary interface for combat decision-making. This document defines every slot, every animation, every interaction. Phase 3 Subsystem 5 of 10 — complete.**
