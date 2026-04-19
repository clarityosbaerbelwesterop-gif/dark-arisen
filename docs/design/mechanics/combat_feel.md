# COMBAT FEEL DEEP DIVE

**Status:** Design Draft v1.0  
**Phase:** Phase 3 — Mechanics (Subsystem 2 of 10)  
**Tier:** Core Tier — Depends on Controls Foundation  
**Related:** mechanics_overview.md, mechanics/controls.md, combat_system.md, style_bible.md  
**Purpose:** Define the complete feel of melee, ranged, and hybrid combat in Dark Arisen. Every frame, every stamina point, every impact must be specified here.  
**Inspirations:** Bloodborne (aggression, Rally, trick weapons), Sekiro (posture, perfect deflect), Dark Souls III (weapon variety, infusions), Ghost of Tsushima (stance fluidity), Elden Ring (ash of war adaptability)

---

## 1. COMBAT PHILOSOPHY — THE THREE LAYERS

Dark Arisen's combat operates on three simultaneous layers. All three must be mastered for the player to feel competent.

### 1.1 Layer 1: The Stamina Layer (Foundation)

Every action costs stamina. Stamina regenerates, but slowly after aggressive play. The player must constantly manage stamina as a finite resource. This creates the "breathing rhythm" of combat — bursts of aggression followed by brief repositioning.

**This is Bloodborne's base inheritance.**

### 1.2 Layer 2: The Posture Layer (Middle)

Every combatant (Jake AND enemies) has a posture meter. Taking attacks without proper defense depletes posture. When posture breaks, the combatant is staggered — vulnerable to a critical attack. Parrying drains enemy posture faster than attacking HP.

**This is Sekiro's contribution, adapted.**

### 1.3 Layer 3: The Rally Layer (Top)

When Jake takes damage, he has a 3-second window to recover part of that HP by dealing damage. If he retreats, the HP is lost. If he commits to aggression, he regains it. This forces a specific psychology: **defense is not free. Aggression is not reckless. They are balanced choices with mathematical consequences.**

**This is Bloodborne's defining mechanic, preserved.**

### 1.4 How the Three Layers Interact

- **Stamina constrains offensive commitment** (you can't attack forever)
- **Posture rewards defensive skill** (perfect parries break enemies faster than trading hits)
- **Rally incentivizes offensive commitment** (you CAN'T just play defensive and heal)

This creates a combat rhythm where pure defense is INCORRECT. Pure offense is INCORRECT. The correct play is informed aggression — attacking when Rally matters, parrying when posture can be broken, retreating only when stamina requires it.

---

## 2. STAMINA SYSTEM — COMPLETE SPECIFICATION

Stamina is the finite resource that governs all combat actions. Its behavior is precisely tuned.

### 2.1 Base Values

| Parameter | Value | Notes |
|-----------|-------|-------|
| Max Stamina (Chapter 4 baseline) | 120 | Scales with Endurance stat investment |
| Max Stamina (Chapter 9 target) | 180 | With typical Endurance investment |
| Max Stamina (hard cap) | 250 | Absolute maximum with full investment + gear |
| Base Regeneration Rate | 20 / second | When not performing stamina actions |
| Regen Delay After Action | 1.0 second | Stamina does not regen during this delay |
| Regen Delay After Exhaustion | 3.0 seconds | When stamina reaches 0, longer penalty delay |
| Combat Regen Penalty | -25% | While weapon is drawn, regen rate reduced |
| Blocking Regen Penalty | -50% | While actively blocking, regen slows further |

### 2.2 Stamina Costs by Action

All values standardized across weapon types unless specified:

| Action | Stamina Cost |
|--------|--------------|
| Light Attack (R1) | 15 |
| Heavy Attack (R2) | 30 |
| Charged Heavy (R2 held) | 50 |
| Dodge Roll (Circle) | 25 |
| Backstep (Circle, no direction) | 10 |
| Parry (L2 / Square) | 10 |
| Failed Parry → Block | 15 |
| Sprint (L3 hold) | 10 / second |
| Jump (Cross) | 12 |
| Climbing | 5 / second |
| Swimming | 8 / second |
| Weapon Drawing | 0 |
| Weapon Switching (via Wheel) | 0 |
| Firearm Shot (Square) | 10 |
| Bow Draw (L2 hold) | 8 / second of draw |
| Bow Release | 0 (cost was in draw) |
| Thrown Weapon | 15 |

### 2.3 Weapon Class Modifiers

Different weapon classes modify base stamina costs:

| Weapon Class | Light Modifier | Heavy Modifier | Dodge Modifier |
|--------------|----------------|----------------|----------------|
| Dagger | -40% (9) | -30% (21) | 0% |
| Short Sword | -20% (12) | -20% (24) | 0% |
| Long Sword | 0% (15) | 0% (30) | 0% |
| Saber (Officer's Blade) | 0% (15) | 0% (30) | 0% |
| Greatsword | +40% (21) | +60% (48) | +20% (30) |
| Axe (one-hand) | +20% (18) | +30% (39) | 0% |
| Great Axe | +40% (21) | +60% (48) | +20% (30) |
| Hammer | +20% (18) | +40% (42) | 0% |
| War Hammer | +60% (24) | +80% (54) | +30% (33) |
| Spear | -10% (14) | +10% (33) | 0% |
| Halberd | +20% (18) | +40% (42) | +10% (28) |
| Dual Short Blades | +10% (17) | +20% (36) | 0% |
| Staff (Silvano's type) | 0% (15) | +20% (36) | 0% |
| Unarmed | -50% (8) | -40% (18) | 0% |

### 2.4 Armor Weight Modifiers

Heavier armor increases stamina costs:

| Armor Class | Equip Load | Stamina Regen | Dodge Distance | Dodge i-frames |
|-------------|------------|---------------|----------------|----------------|
| Light (under 30% capacity) | Full mobility | 100% | 100% (4m) | 12 frames |
| Medium (30-60%) | Standard | 85% | 85% (3.4m) | 10 frames |
| Heavy (60-100%) | Slow | 70% | 70% (2.8m) | 8 frames |
| Overloaded (>100%) | Immobile | 40% | 50% (2m) | 6 frames |

### 2.5 Stamina Management Strategies

The player must learn these patterns:

**The 3-Hit Rule:** Most light attack combos are sustainable for 3 hits before stamina requires pause. This creates the baseline attack rhythm.

**The Heavy Commit:** Heavy attacks cost 2x stamina but deal 2.5x damage. They are worth committing to against staggered enemies or posture-broken foes.

**The Dodge Budget:** At max stamina (120), Jake can dodge roll approximately 4 times before forced pause. This shapes how many attacks he can safely evade in sequence.

**The Sprint Limit:** 12 seconds of continuous sprint at max stamina. Plan longer traversal segments with pauses.

---

## 3. POSTURE SYSTEM — THE SEKIRO LAYER

Posture is a secondary meter separate from HP. When posture breaks, the combatant is staggered and vulnerable to a critical attack.

### 3.1 Posture Values

| Combatant | Base Posture | Regen Delay | Regen Rate |
|-----------|--------------|-------------|------------|
| Jake (Chapter 4) | 100 | 3 seconds | 30 / second |
| Jake (Chapter 9) | 140 | 3 seconds | 35 / second |
| Common Pirate | 40 | 4 seconds | 8 / second |
| Elite Soldier | 80 | 4 seconds | 12 / second |
| Imperial Officer | 100 | 4 seconds | 15 / second |
| Boss (Mini) | 150 | 5 seconds | 20 / second |
| Boss (Main) | 250-300 | 5 seconds | 25 / second |
| Boss (Final - Draven/Ethan) | 400 | 6 seconds | 20 / second |

### 3.2 Posture Damage Sources

How different actions damage posture:

| Action | Posture Damage |
|--------|----------------|
| Light Attack (successful hit) | Weapon base × 0.3 |
| Heavy Attack (successful hit) | Weapon base × 0.7 |
| Charged Heavy | Weapon base × 1.2 |
| Perfect Parry | Weapon base × 1.5 |
| Block (weapon absorbs hit) | 0 (no posture damage to attacker) |
| Parry Strike (after successful parry) | Weapon base × 2.0 |
| Backstab | Full posture bar × 1.0 (usually instant break) |

### 3.3 The Break State

When posture reaches 0, the combatant enters "Break State":

- **Duration:** 3 seconds for enemies, 1.5 seconds for Jake
- **Vulnerability:** Opens critical attack window
- **Animation:** Combatant stumbles, weapon slightly lowered, brief moment of visible recovery
- **Reset:** Posture immediately refills to 50% after Break State ends

### 3.4 Critical Attacks (Triangle + R1 during Break)

When an enemy is in Break State, Jake can perform a critical attack:

- **Input:** Triangle within 1 second of enemy entering Break State
- **Damage:** 3.0x normal weapon damage
- **Animation:** Unique critical animation per weapon type
- **HP Drain:** Critical attacks also drain enemy maximum HP (5% permanent for that encounter)
- **Rally Recovery:** Jake regains 100% of Rally-available HP during critical

### 3.5 The Parry-Posture Interaction

This is the skilled player's path:

- Normal parry drains enemy posture significantly
- Perfect parry (frames 5-10 of 15-frame window) drains double
- Parry followed by heavy attack: "Parry Strike" — costs 30 stamina but deals posture damage × 2
- Three consecutive perfect parries: "Rhythm Break" — forces enemy into Break State immediately

**Design intent:** Skilled defensive players can win fights without taking damage, through perfect parrying. But the timing demands are real. Most players will mix parry with aggression.

### 3.6 Posture vs. HP Balance

For most enemies, HP ≈ 3-4x Posture. This means:
- **Pure HP damage:** Can defeat enemy by depleting HP directly
- **Pure Posture damage:** Break State + Critical is roughly equivalent damage efficiency
- **Mixed strategy:** Optimal — wear down HP while accumulating posture breaks

For major bosses, HP ≈ 5-8x Posture. Pure posture strategies become more efficient against bosses.

---

## 4. RALLY SYSTEM — THE BLOODBORNE LAYER

Rally is the defining mechanic that shapes combat psychology.

### 4.1 Rally Mechanics

When Jake takes damage:

1. **Damage is dealt normally** (HP reduced)
2. **A Rally portion appears** (orange section of HP bar)
3. **Rally window begins** (3 seconds default)
4. **During the window:** Dealing damage restores Rally HP
5. **Window ends:** Remaining Rally HP is lost

### 4.2 Rally Percentage

Not all damage is Rally-recoverable:

| Damage Source | Rally % |
|---------------|---------|
| Standard Enemy Attack | 70% Rally |
| Elite Enemy Attack | 60% Rally |
| Boss Attack (non-grab) | 50% Rally |
| Grab Attack | 30% Rally |
| Environmental Damage | 40% Rally |
| Fire Damage | 25% Rally |
| Poison Damage | 0% Rally (no recovery) |
| Fall Damage | 50% Rally |
| Bleed Damage | 30% Rally |

### 4.3 Rally Recovery Rates

How fast Rally converts back to HP when dealing damage:

- **Light Attack landed:** Recovers 15% of Rally HP per hit
- **Heavy Attack landed:** Recovers 25% of Rally HP per hit
- **Parry Strike:** Recovers 40% of Rally HP
- **Critical Attack (on Break enemy):** Recovers 100% of available Rally HP

### 4.4 The Rally Window

The 3-second Rally window is precisely tuned:

- **Base duration:** 3.0 seconds
- **Extended by Rally-related gear:** Up to +1.5 seconds
- **Reduced by poison:** -1.0 second
- **Reduced by fear/status:** -0.5 seconds per stack

### 4.5 Visual and Audio Feedback

**Visual cues for Rally state:**
- HP bar shows orange section (Rally portion)
- Screen edge has subtle red pulse during Rally window
- Jake's body has faint crimson glow during window
- Post-window, the Rally HP visibly drains (converts to black)

**Audio cues:**
- Heart-beat audio overlay during Rally window
- Sound intensifies as window closes
- Specific "impact" sound when Rally HP is recovered

**Haptic (PS5):**
- Pulsing heartbeat in left grip during Rally
- Sharp "confirmation" pulse when HP recovered
- Dull thud when Rally window expires with HP remaining

### 4.6 Rally Psychology

The Rally system creates a specific combat psychology:

**Without Rally:** Player takes damage → heals with bandages → fights cautiously. Defense is always safer.

**With Rally:** Player takes damage → MUST decide in 3 seconds — commit to aggression to recover, or retreat and lose HP. There is no free healing through defense.

This changes the entire feel of combat. Dark Arisen is not about cautious dueling. It is about informed aggression. The Rally system is why.

---

## 5. WEAPON ARCHETYPES — THE EIGHT CLASSES

Dark Arisen has eight primary weapon archetypes. Each has distinct frame data, range, and strategic purpose.

### 5.1 Archetype 1: Dagger (Speed Class)

**Examples:** Fishing Knife, Assassin's Dagger, Thieves' Blade
**Philosophy:** Fast, short range, high crit potential, low damage per hit, mobility-focused

**Frame Data:**
- Light Attack: 4 frame windup, 6 frame active, 12 frame recovery
- Heavy Attack: 12 frame windup, 8 frame active, 20 frame recovery
- Combo capacity: Up to 5 hits
- Range: 1.2m effective

**Stats (Chapter 4 baseline):**
- Base Damage: 45 (light) / 90 (heavy)
- Posture Damage: 14 (light) / 65 (heavy)
- Stamina Cost: 9 (light) / 21 (heavy) [modified by class]
- Critical Multiplier: 4.0x (highest in game)

**Unique Mechanic:** Dagger backstab multiplier is 5.0x. Daggers can perform silent kills from stealth.

### 5.2 Archetype 2: Short Sword (Balance Class)

**Examples:** Navy Cutlass, Pirate's Blade, Imperial Service Sword
**Philosophy:** Balanced speed and damage, versatile for most situations

**Frame Data:**
- Light Attack: 6 frame windup, 8 frame active, 14 frame recovery
- Heavy Attack: 15 frame windup, 10 frame active, 25 frame recovery
- Combo capacity: Up to 4 hits
- Range: 1.4m effective

**Stats:**
- Base Damage: 60 (light) / 125 (heavy)
- Posture Damage: 18 (light) / 87 (heavy)
- Stamina Cost: 12 (light) / 24 (heavy)
- Critical Multiplier: 3.5x

**Unique Mechanic:** Short swords have the best parry frame generosity (+2 frame window).

### 5.3 Archetype 3: Long Sword (Versatility Class)

**Examples:** Cavalry Saber, Duelist Blade, Draven's Signature (not available until Chapter 9+)
**Philosophy:** Standard weapon, excellent all-around performance

**Frame Data:**
- Light Attack: 8 frame windup, 10 frame active, 16 frame recovery
- Heavy Attack: 18 frame windup, 12 frame active, 28 frame recovery
- Combo capacity: Up to 4 hits
- Range: 1.7m effective

**Stats:**
- Base Damage: 75 (light) / 160 (heavy)
- Posture Damage: 23 (light) / 112 (heavy)
- Stamina Cost: 15 (light) / 30 (heavy)
- Critical Multiplier: 3.0x

**Unique Mechanic:** Long swords have the best posture damage-to-stamina-cost ratio. Balanced all-around.

### 5.4 Archetype 4: Greatsword (Power Class)

**Examples:** Two-Handed Greatsword, Executioner's Blade, Leviathan's Sorrow
**Philosophy:** Massive damage, slow, requires commitment, high posture damage

**Frame Data:**
- Light Attack: 14 frame windup, 15 frame active, 30 frame recovery
- Heavy Attack: 28 frame windup, 20 frame active, 45 frame recovery
- Combo capacity: Up to 3 hits
- Range: 2.4m effective

**Stats:**
- Base Damage: 120 (light) / 280 (heavy)
- Posture Damage: 36 (light) / 196 (heavy)
- Stamina Cost: 21 (light) / 48 (heavy)
- Critical Multiplier: 2.5x

**Unique Mechanic:** Greatsword heavy attacks have HYPER ARMOR — Jake cannot be interrupted during the active frames of a heavy attack. Trade damage is possible.

### 5.5 Archetype 5: Axe / Hammer (Impact Class)

**Examples:** Boarding Axe, War Hammer, Sailor's Cleaver
**Philosophy:** High posture damage, crush armored foes, heavy swings

**Frame Data (Axe):**
- Light Attack: 10 frame windup, 12 frame active, 20 frame recovery
- Heavy Attack: 22 frame windup, 14 frame active, 35 frame recovery
- Combo capacity: Up to 3 hits
- Range: 1.6m effective

**Stats (Axe):**
- Base Damage: 90 (light) / 200 (heavy)
- Posture Damage: 32 (light) / 140 (heavy)
- Stamina Cost: 18 (light) / 39 (heavy)
- Critical Multiplier: 2.8x

**Unique Mechanic:** Axes and hammers ignore 40% of enemy armor. Heavy attacks have a chance (15%) to trigger posture break on hit if enemy is already at low posture.

### 5.6 Archetype 6: Spear / Halberd (Reach Class)

**Examples:** Whaler's Spear, Imperial Halberd, Boarding Pike
**Philosophy:** Long range, thrust-focused, good for crowd control, mediocre damage

**Frame Data (Spear):**
- Light Attack (thrust): 8 frame windup, 10 frame active, 18 frame recovery
- Heavy Attack (sweep): 20 frame windup, 15 frame active, 30 frame recovery
- Combo capacity: Up to 4 thrusts
- Range: 2.8m effective (best in game for melee)

**Stats:**
- Base Damage: 70 (thrust) / 150 (sweep)
- Posture Damage: 21 (thrust) / 105 (sweep)
- Stamina Cost: 14 (thrust) / 33 (sweep)
- Critical Multiplier: 3.5x on thrusts, 2.5x on sweeps

**Unique Mechanic:** Spears have a "first strike" advantage — the first thrust from beyond 2m range deals +30% damage and ignores 20% armor.

### 5.7 Archetype 7: Dual Blades (Combo Class)

**Examples:** Matched Sabers (Corazón's), Twin Shortswords (Kira's), Paired Daggers
**Philosophy:** Rapid multi-hit, high total DPS, complex combo trees

**Frame Data:**
- Light Attack (one blade): 5 frame windup, 6 frame active, 10 frame recovery
- Heavy Attack (both blades): 14 frame windup, 12 frame active, 22 frame recovery
- Combo capacity: Up to 7 hits with specific rhythm
- Range: 1.3m effective

**Stats:**
- Base Damage: 35 per blade (70 total for dual hit) / 140 heavy
- Posture Damage: 10 per blade / 98 heavy
- Stamina Cost: 17 (light single) / 36 (heavy dual)
- Critical Multiplier: 3.2x

**Unique Mechanic:** Dual blades can parry (left hand) AND attack (right hand) simultaneously. "Parry & Strike" combo costs 25 stamina but deals damage while gaining parry benefit.

### 5.8 Archetype 8: Bow / Firearm (Ranged Class)

**Bow (Mira's style):**
- Draw time: 60 frames (full draw) / 30 frames (half draw)
- Recovery after shot: 24 frames
- Range: 80m effective
- Base Damage: 130 (full draw) / 70 (half draw)
- Critical (headshot): 6.0x
- Stamina during draw: 8/sec

**Firearm (Flintlock Pistol):**
- Fire delay: 15 frames (after Square press)
- Reload time: 90 frames (1.5 seconds — very slow, single shot)
- Range: 25m effective (accurate)
- Base Damage: 150
- Critical: Weak points trigger +100% damage
- Parry value: Firearm shot can be parried by enemies (with timing)

**Unique Mechanic (Firearms):** Pistol shots can be used as parry-equivalent against sword attacks. The visceral "Bloodborne-style" parry. Enemy staggers briefly, opening critical window.

### 5.9 Weapon Comparison Table

| Archetype | DPS Rating | Range | Stamina Efficiency | Skill Ceiling |
|-----------|------------|-------|-------------------|---------------|
| Dagger | Medium | Low | Excellent | High |
| Short Sword | Medium | Medium | Very Good | Medium |
| Long Sword | High | Medium | Good | Medium |
| Greatsword | Very High | Medium | Poor | High (commit) |
| Axe / Hammer | High | Medium | Medium | Medium |
| Spear | Medium | High | Good | Medium |
| Dual Blades | Very High | Low | Poor | Very High |
| Bow | Medium | Very High | Medium | High (aim) |

---

## 6. FRAME DATA STANDARDS

All weapons and actions follow standardized frame data notation.

### 6.1 Frame Data Vocabulary

- **Windup:** Frames from input to damage being dealt (startup)
- **Active:** Frames during which damage is dealt (hit-box active)
- **Recovery:** Frames after active, during which the action cannot be canceled
- **Total:** Windup + Active + Recovery

### 6.2 Cancellable Windows

Most attacks can be canceled into specific other actions:

| From | Can Cancel Into | During Frames |
|------|----------------|---------------|
| Light Attack | Dodge | After frame 15 |
| Light Attack | Heavy Attack | After frame 12 |
| Light Attack | Next Light Attack | After frame 10 (combo chain) |
| Heavy Attack | Dodge | After frame 25 |
| Heavy Attack | Cannot cancel | During active frames |
| Dodge | Any attack | After i-frames end |
| Parry | Parry Strike | Within 30 frames of successful parry |
| Sprint | Any attack | Anytime |

### 6.3 Standard Frame Rates

Dark Arisen targets 60 FPS for combat calculations. All frame data in this document assumes 60 FPS.

- **60 FPS = 1 frame per 16.67ms**
- **At 30 FPS mode:** All frame data is mathematically halved (8 frames windup becomes 4 frames)
- **Frame-perfect inputs** possible on PS5/PC/Xbox
- **Server-side validation** for any competitive context

---

## 7. HIT STOP AND IMPACT FEEL

Hit stop is the brief pause when weapons connect. It is critical for making combat feel WEIGHTY.

### 7.1 Hit Stop Duration

Different attacks create different hit stops:

| Attack Type | Hit Stop Duration |
|-------------|-------------------|
| Light Attack | 3 frames |
| Heavy Attack | 5 frames |
| Charged Heavy | 7 frames |
| Critical Hit | 9 frames |
| Parry Strike | 8 frames |
| Counter Critical | 12 frames |

### 7.2 Camera Shake

On impact, the camera shakes proportional to attack weight:

| Attack Type | Shake Intensity | Duration |
|-------------|-----------------|----------|
| Light Attack | 1 (subtle) | 0.1 sec |
| Heavy Attack | 3 (noticeable) | 0.2 sec |
| Charged Heavy | 5 (strong) | 0.3 sec |
| Critical Hit | 7 (dramatic) | 0.5 sec |
| Explosion | 8 (intense) | 0.8 sec |
| Boss Ground Slam | 10 (maximum) | 1.0 sec |

### 7.3 Particle Effects

Hit particles vary by weapon and target:

- **Metal on metal:** Bright sparks (3-5 particles, 0.3 sec)
- **Blade on flesh:** Blood spray (8-12 particles, 0.5 sec)
- **Heavy blunt on flesh:** Heavy impact spray (5-8 particles + dust, 0.7 sec)
- **Blade on stone:** Stone chips (4-6 particles, 0.4 sec)
- **Arrow impact:** Minimal spark (2-3 particles, 0.2 sec)
- **Firearm impact:** Muzzle flash + smoke (0.4 sec) + impact spark

### 7.4 Audio Feedback

Sound design for hits must convey weight:

- **Light weapon hits:** Sharp "clang" or "slice" (120ms audio)
- **Heavy weapon hits:** Deep "thud" or "crack" (250ms audio)
- **Critical hits:** Unique dramatic sound (400ms audio with sub-bass)
- **Parries:** Metallic "ring" on success, dull "thunk" on failure
- **Blocks:** Slightly muffled hit sound (attack absorbed)

### 7.5 Haptic Feedback (PS5)

DualSense haptics amplify impact feel:

- **Light hit:** Sharp short pulse (both grips, 100ms)
- **Heavy hit:** Deep extended pulse (focused in hit direction, 250ms)
- **Critical hit:** Multi-layered pulse (rhythmic, 400ms)
- **Parry success:** Crisp single click (right trigger area)
- **Parry failure:** Dull thud (left grip)
- **Taking damage:** Directional pulse from damage source direction

---

## 8. DAMAGE CALCULATION FORMULAS

### 8.1 Base Damage Formula

Final Damage = (Base Weapon Damage × Attack Multiplier)
× (1 - Target Armor Reduction)
× Status Multipliers
× Rally/Critical Multipliers


Where:
- **Base Weapon Damage:** Stat from weapon (e.g., Long Sword = 75)
- **Attack Multiplier:** Light = 1.0, Heavy = 2.1, Charged = 3.0, Critical = varies
- **Target Armor Reduction:** 0.1-0.6 based on armor class (10-60% reduction)
- **Status Multipliers:** Status effects (bleed, poison, burn) add multipliers
- **Rally/Critical:** Contextual multipliers for timing/position

### 8.2 Weapon Scaling

Weapons scale with specific Jake stats:

| Stat | Scales | Effect |
|------|--------|--------|
| Strength | Heavy weapons (greatsword, axe, hammer) | +1% damage per point |
| Dexterity | Fast weapons (dagger, short sword, dual blades) | +1% damage per point |
| Faith | Fjordlund/religious weapons (rare) | +1% damage per point |
| Intelligence | Alchemical/exotic weapons | +1% damage per point |

Maximum stat: 40. Maximum bonus: +40% weapon damage through stat scaling.

### 8.3 Damage Types

Five damage types affect armor differently:

| Damage Type | Weapons | Armor Counter |
|-------------|---------|---------------|
| Slash | Swords, daggers, light blades | Hard armor resists |
| Pierce | Spears, arrows, rapiers | Light armor vulnerable |
| Blunt | Hammers, staves, fists | Heavy armor vulnerable to this |
| Bleed | Causes bleed buildup | No direct counter |
| Fire | Alchemical, specific weapons | Leather armor vulnerable |

### 8.4 Critical Strike System

Criticals trigger under specific conditions:

**Types of Criticals:**
1. **Backstab:** 4.0x damage + posture break. Must attack from behind enemy, unnoticed.
2. **Parry Critical:** 3.5x damage. Follows successful parry + Parry Strike combo.
3. **Posture Break Critical:** 3.0x damage. Triangle during enemy Break State.
4. **Weak Point:** Varies by enemy (2.0-3.5x). Specific body parts.
5. **Bow Headshot:** 6.0x damage. Precise aim required.
6. **Firearm Headshot:** 3.5x damage. Precise aim required.
7. **Stealth Kill:** Instant kill on most enemies. Daggers especially.

### 8.5 Status Effects

Four primary status effects build up and trigger:

**BLEED:**
- Buildup required: 80-150 (varies by enemy)
- Trigger effect: 15% of target max HP damage over 10 seconds
- Application: Slash weapons primarily, specific weapons have higher bleed rate
- Counter: Cauterize bandage, passive regen

**POISON:**
- Buildup required: 60-120
- Trigger effect: 2% HP/second for 30 seconds (60% total max HP possible)
- Application: Poison coatings, specific weapons (El Médico's tools)
- Counter: Antitoxin, specific immunity gear

**BURN:**
- Buildup required: 100-200
- Trigger effect: 8% HP damage + 2% additional for 5 seconds + Rally % reduced
- Application: Fire weapons, alchemical arrows, environmental
- Counter: Water immersion, wet clothing, cooling tonic

**FROZEN:**
- Buildup required: 150-300
- Trigger effect: Movement speed -50% for 10 seconds, damage +25% taken
- Application: Cold weather in specific regions, specific weapons
- Counter: Heavy layered clothing, warmth tonics, active movement

---

## 9. WEAPON BOND SYSTEM

Weapons grow with Jake through use. This is the RDR2-inspired relationship system applied to arms.

### 9.1 Bond Levels

Each weapon has a Bond Level (0-10):

| Bond Level | XP Required | Benefits |
|------------|-------------|----------|
| 0 | 0 | Base stats |
| 1 | 500 kills/hits | +5% damage |
| 2 | 1,500 | +10% damage, reduced stamina cost (-5%) |
| 3 | 3,000 | +15% damage, unlocks unique combo variation |
| 4 | 5,500 | +20% damage, weapon visibly weathered |
| 5 | 9,000 | +25% damage, reduced stamina (-10%) |
| 6 | 14,000 | +30% damage, new critical animation |
| 7 | 20,000 | +35% damage, unique weapon name possible |
| 8 | 28,000 | +40% damage, signature moves |
| 9 | 38,000 | +45% damage, Jake visibly attached |
| 10 | 50,000 | +50% damage, "Soul-Bound" — cannot be dropped |

### 9.2 Bond Mechanics

- **Bond XP:** Earned per successful hit or kill with weapon
- **Visible changes:** Higher Bond weapons show wear, personalized markings, Jake handles them differently
- **Unique Names:** At Bond 7+, the player can rename the weapon permanently
- **NPC Recognition:** Certain NPCs (Esteban, Mormor Astrid) notice when Jake is using a highly-bonded weapon

### 9.3 Story Weapons

Certain weapons have predetermined Bond trajectories:

- **Jake's starter weapon (Father's Cutlass):** Always begins at Bond 3, grows to 10
- **Ethan's Gift (Chapter 9 pre-fight):** Begins at Bond 5, can reach 10
- **Corazón's Sabers (looted):** Inherit her Bond (7) if Jake chose to carry them
- **Leviathan's Sorrow (Havfrue crafted):** Special — Bond affects whether she "accepts" Jake as wielder

---

## 10. COMBAT READING — LEARNING TO WIN

Skilled combat in Dark Arisen requires the player to READ enemies. This section defines what "reading" means mechanically.

### 10.1 Enemy Tell Animations

Every enemy attack has visible tell frames (windup):

- **Human enemies:** 12-20 frame windups (readable)
- **Elite enemies:** 8-14 frame windups (faster, more subtle)
- **Boss enemies:** Varying, some very fast (6-8 frames) for late-game challenge
- **Mythical enemies:** Unique tell patterns requiring study

### 10.2 Audio Tells

Sound precedes many attacks:

- **Sword unsheathe:** Specific sound before draw attack
- **Weight shift:** Breathing pattern change before heavy swing
- **War cry:** Specific vocalizations before charge attacks
- **Clink of armor:** Before repositioning

### 10.3 Visual Cues Design

Colored highlights help during tutorial phases:

**Chapter 1-2 (Tutorial):**
- Red flash on enemy before attack (disable after Chapter 2)
- Blue flash before parryable attack
- Yellow flash before dodgeable-only attack

**Chapter 3+:**
- All visual aids remove (skilled play expected)
- Player must read animation timing alone
- Accessibility option to re-enable visual aids

### 10.4 Parryable vs Unparryable

Not all attacks can be parried. Key distinctions:

**Always Parryable:**
- Blade-on-blade attacks (sword, dagger, spear, halberd thrusts)
- Firearm shots (pistol parry — timing-critical)
- Most light attacks

**Never Parryable:**
- Grab attacks
- Explosive/projectile AOE
- Unblockable enemy specials (always telegraphed specifically)
- Certain boss signature attacks (Draven's Final Breath, Caldera Titan's Ground Shake)

**Conditionally Parryable:**
- Heavy swings (parryable with perfect timing, fail = full damage block)
- Boss critical strikes (parryable, huge reward but extreme timing)

---

## 11. COMBAT-ADJACENT MECHANICS

### 11.1 Healing Items in Combat

Using a bandage or healing tonic:
- **Input:** D-Pad Left
- **Animation time:** 30 frames (hold) + 15 frames recovery
- **Total commitment:** 45 frames (0.75 seconds)
- **Vulnerable:** Yes — Jake cannot dodge or parry during this
- **Rally interaction:** Healing items also restore Rally HP instantly

### 11.2 Consumable Combat Items

Throwing items during combat:
- **Input:** D-Pad Left (after quick menu selects throwables) or Item Wheel
- **Types:** Poison flask, smoke bomb, fire bottle, distraction rock
- **Animation time:** 25 frames to throw
- **Cooldown:** 3 seconds between throws

### 11.3 Ability Unlocks (Chapter Progression)

Combat expands with progression:

| Chapter | Combat Unlock |
|---------|---------------|
| 1 | Basic combat, parry, dodge |
| 2 | Heavy attack, lock-on, weapon switch |
| 3 | Charged heavy, second weapon slot |
| 4 | Ship boarding combat (naval) |
| 5 | Advanced stealth assassinations |
| 6 | Dual-mode weapons (like Bloodborne trick weapons) |
| 7 | Environmental combat interactions (throw enemies off cliffs etc.) |
| 8 | Crew ability calls during combat (support abilities) |
| 9 | RACHE UNLOCK — transforms all combat |
| 10 | Second Rache charge, combat upgrades |

### 11.4 The "Flow State" Mechanic

When Jake chains specific actions perfectly, he enters a Flow State:

**Flow State Triggers:**
- 3 perfect parries in succession
- 5 consecutive hits without taking damage
- 2 criticals within 10 seconds
- Kill an enemy with the killing-blow input being the start of a combo

**Flow State Effects:**
- Damage dealt: +20% for next 10 seconds
- Stamina regen: +50% for next 10 seconds
- Posture regen: +50% for next 10 seconds
- Visual: Subtle gold edge on Jake's weapon
- Audio: Brief musical sting when entering Flow

**Flow State Duration:** 10 seconds or until Jake takes damage (whichever first).

---

## 12. COMBAT UI

### 12.1 HUD Elements

During combat, the player sees:

**Top-Left (Jake's status):**
- HP bar (with Rally orange section)
- Stamina bar
- Posture bar (below stamina, smaller)
- Active status effect icons
- Rache cooldown (when unlocked)

**Top-Right (Enemy status):**
- Enemy HP bar (appears when targeted)
- Enemy Posture bar (smaller)
- Enemy status effects
- Lock-on indicator

**Bottom-Left (Quick slots):**
- Current healing item count
- Current consumable items
- Current weapon name + Bond indicator

**Bottom-Right (Context):**
- Context-sensitive button prompts
- Rally window indicator (orange highlight)
- Flow State indicator (when active)

### 12.2 HUD Minimization

Accessibility option to reduce HUD:

- **Minimal:** Only HP and critical prompts
- **Standard:** All elements
- **Verbose:** Detailed stat readouts (for frame-perfect practice)

### 12.3 Color Palette

Combat UI colors:
- **HP:** Red (#C91F1F)
- **Rally:** Orange (#FF7A00)
- **Stamina:** Green (#4CAF50)
- **Posture:** Yellow-Gold (#FFD700)
- **Status - Poison:** Purple-Green (#6B8E23)
- **Status - Bleed:** Dark Red (#8B0000)
- **Status - Burn:** Orange-Red (#FF4500)
- **Status - Frozen:** Pale Blue (#87CEEB)

---

## 13. PLATFORM-SPECIFIC COMBAT DETAILS

### 13.1 PS5 DualSense Features

**Haptics during combat:**
- Weapon strike reverberates through appropriate grip
- Successful parry: Sharp click in trigger
- Critical hit: Multi-stage pulse rhythm
- Low HP: Slow heartbeat in left grip
- Stamina exhaustion: Rapid shallow pulse in right grip

**Adaptive Triggers:**
- R2 tension during charge attack (builds resistance as hold increases)
- L2 tension during parry (subtle click at perfect moment)
- L2 tension during bow draw (progressive resistance)

### 13.2 Xbox Series X/S

**Trigger rumble:**
- Similar to DualSense but less zoned
- Primary impact feedback through main rumble motors
- Trigger resistance on specific actions (where supported)

**Performance:**
- Series X: 4K/60 FPS in Performance mode
- Series S: 1440p/60 FPS
- All frame data identical between platforms

### 13.3 PC

**Keyboard/Mouse combat:**
- All combat actions remappable
- Mouse sensitivity for aim actions independently adjustable
- Monitor refresh rate considerations (144 Hz support, frame data maintained)

**Advanced features:**
- Frame-perfect inputs possible with low-latency setups
- Replay/practice mode for combat training
- Mod support for HUD customization

---

## 14. ACCESSIBILITY IN COMBAT

Critical that all combat systems work for all players.

### 14.1 Timing Aids

- **Parry window extension:** +25%, +50%, +100%
- **Dodge i-frame extension:** +25%, +50%, +100%
- **Combat input buffer:** +50% extension
- **Auto-parry:** Automatically parries if input during window

### 14.2 Reading Aids

- **Attack telegraph:** Red flash before all enemy attacks (re-enable for skilled-players too)
- **Parry indicator:** Blue flash for parryable attacks
- **Attack direction arrows:** Show direction attack is coming from
- **Focus target:** Auto-highlight priority target

### 14.3 Input Simplification

- **One-button combos:** Press once, full combo executes
- **Auto-dodge:** Auto-activates on incoming damage
- **Auto-lock:** Engages nearest threat automatically
- **Combined inputs:** Reduce simultaneous button presses

### 14.4 Difficulty Customization

Custom difficulty sliders affect combat specifically:

- **Enemy damage:** 25%, 50%, 75%, 100%, 125%, 150%
- **Enemy aggression:** Passive, Normal, Aggressive, Very Aggressive
- **Enemy response time:** +4, 0, -4, -8 frames
- **Jake damage:** 50%, 75%, 100%, 125%, 150%
- **Stamina regen:** 50%, 75%, 100%, 125%, 150%

---

## 15. THE FEEL TEST

Before combat is considered final, it must pass these tests:

### 15.1 The Bloodborne Test

> *"After 30 minutes, does a Bloodborne veteran feel: 'This is the Bloodborne combat rhythm. I trust these weapons. I understand this Rally mechanic. I can focus on THIS game's innovations rather than relearning the basics.'"*

### 15.2 The Depth Test

> *"After 20 hours, does a dedicated player discover new combat strategies? New weapon combinations? New enemy reading techniques?"*

### 15.3 The Accessibility Test

> *"Can a player with motor limitations enjoy the combat through accessibility options without feeling the game is holding back?"*

### 15.4 The Weight Test

> *"Does every attack feel IMPACTFUL? Do hits satisfy? Do deaths feel earned?"*

### 15.5 The Boss Test

> *"Do boss fights teach the player something new about combat? Do they require execution of all three layers (stamina, posture, rally) together?"*

All five must pass. Then combat is ready.

---

## 16. IMPLEMENTATION PRIORITIES

### 16.1 Phase 1 of Development
- Base combat (light, heavy, dodge, parry)
- Stamina system
- Basic posture mechanics
- First weapon archetype (Long Sword as baseline)
- Hit stop and camera shake

### 16.2 Phase 2
- Additional weapon archetypes
- Posture break + critical system
- Rally system implementation
- Status effects (begin with Bleed)

### 16.3 Phase 3
- Weapon Bond system
- All status effects
- Boss combat specific mechanics
- Ship combat integration (see naval_combat_system.md)

### 16.4 Phase 4
- Rache system integration
- Flow State
- Accessibility passes
- Advanced combat (dual-mode weapons, environmental)

---

## 17. CONSISTENCY CHECK AGAINST EXISTING DESIGN

All 21 boss fights must be re-validated against this combat spec:

### 17.1 Boss Fight Frame Data Check
Each boss spec (jungle_warden.md, ethan_harlow.md, draven_voss.md, etc.) must have frame data that aligns with this document. Earlier boss specs may need v1.1 updates.

### 17.2 Stamina Cost Validation
All boss movesets should use the stamina cost structure defined here. Previous boss specs may have implied different costs.

### 17.3 Posture Value Integration
Boss posture values listed here (250-300 for mains, 400 for finals) match existing boss specs. ✅

### 17.4 Rally Specifics
Rally percentages per boss should match the 50% baseline for main bosses, 30% for grabs. Existing specs referenced this; this doc formalizes.

---

## 18. NEXT STEPS

Next document: **`mechanics/traversal.md`** — Traversal System

Will cover:
- Walking, running, sprinting feel
- Climbing mechanics (where climbable, how it works)
- Swimming (depth levels, holding breath, underwater combat)
- Jumping refinement (platforming expectations)
- Falling damage calculations
- Stamina implications outside combat
- Environmental obstacles (water, rope bridges, cliffs)
- Mount system (if horses included in Region-specific gameplay)

Estimated time: 3-4 hours.

After Traversal: Stealth System (building on Combat + Traversal foundations).

**Combat is the heart of Dark Arisen. This document defines every frame, every stamina point, every weapon, every impact. With this foundation solid, all other systems can build upon it. Phase 3 Subsystem 2 of 10 — complete.**
