# COMBAT PHYSICS — IMPACT, WEIGHT, AND CONSEQUENCE

**Status:** Design Draft v1.0  
**Phase:** Phase 6 — Physics (Subsystem 5 of 15 — Combat & Movement Tier)  
**Tier:** Combat & Movement Tier  
**Related:** physics_overview.md, mechanics/combat_feel.md, physics/wind_systems.md, style_bible.md  
**Purpose:** Define the complete combat physics system — hit impact, weapon weight, ragdoll behavior, blood and gore (style-appropriate), projectile trajectories, and the physical consequences of violence.  
**Inspirations:** Bloodborne (combat weight and impact), Sekiro (precision combat physics), Red Dead Redemption 2 (realistic gunfights), Mordhau (weapon physics depth), Ghost of Tsushima (cinematic kills), real-world physics of combat

---

## 1. COMBAT PHYSICS PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Weight Tells Truth

A heavy sword swings differently than a light dagger. A war hammer crushes; a rapier pierces. Players feel weight through animation, sound, controller feedback, and visual response. Light weapons aren't slow weapons made faster — they're fundamentally different.

**Design intent:** Tactile authenticity. Weapon choice has physical consequence beyond stats.

### 1.2 Principle 2: Bodies Are Not Mannequins

When a person dies, they don't fall in a generic ragdoll loop. They fall based on hit location, hit force, current pose, current velocity, and gravity. Each death is unique because each combat moment is unique.

**Design intent:** Realistic death animation. Style B brutal honesty in physical reality.

### 1.3 Principle 3: Impact Has Layers

A strike doesn't just damage. It impacts visually (stagger, blood, knockback), aurally (specific sound by weapon and target), tactically (interrupts, opens openings), and emotionally (drama, weight). Every successful hit is a multi-sensory event.

**Design intent:** Bloodborne-tier combat feel through layered impact.

### 1.4 Principle 4: Projectiles Obey Physics

Arrows arc, drop, drift in wind. Bullets travel fast but penetrate variably. Cannonballs shatter what they hit. Each projectile follows realistic physics — players who learn ballistics dominate.

**Design intent:** Skill-based ranged combat through realistic physics.

### 1.5 Principle 5: Violence Has Consequence

Blood splatters realistically and persists. Wounds remain visible. Bodies stay where they fall. Violence isn't a combat animation — it's an event that marks the world.

**Design intent:** Combat feels real because consequences are real.

---

## 2. WEIGHT AND MOMENTUM

### 2.1 Weapon Weight Categories

**Featherweight (0.5-1.0 kg):**

- Throwing knives
- Daggers
- Light pistols

**Visual Behavior:**
- Quick whips
- Minimal follow-through
- Easily redirected
- Fast recovery

**Audio:**
- Light whistles
- Sharp clinks
- Quick sounds

**Player Feel:**
- Effortless
- Quick combos
- Low impact
- Precision focused

### 2.2 Light (1.0-2.5 kg)

**Examples:**
- Rapiers
- Cutlasses
- Single-handed swords
- Hand axes

**Visual Behavior:**
- Smooth swings
- Reasonable follow-through
- Reasonable recovery
- Tactical

**Audio:**
- Whooshing cuts
- Metallic clashes
- Sharp impacts

**Player Feel:**
- Balanced
- Combos flow
- Standard response
- Versatile

### 2.3 Standard (2.5-5.0 kg)

**Examples:**
- Longswords
- Pirate cutlasses (heavier)
- War axes
- Heavy pistols

**Visual Behavior:**
- Deliberate swings
- Clear follow-through
- Recovery time
- Powerful

**Audio:**
- Heavier whooshes
- Resonant clashes
- Solid impacts

**Player Feel:**
- Weighted
- Strategic combos
- Real impact
- Authority

### 2.4 Heavy (5.0-10 kg)

**Examples:**
- Two-handed swords
- War hammers
- Battle axes
- Heavy maces

**Visual Behavior:**
- Massive swings
- Long follow-through
- Significant recovery
- Devastating

**Audio:**
- Deep whooshes
- Bone-crushing impacts
- Resonant strikes
- Dramatic

**Player Feel:**
- Heavy in hands
- Each swing matters
- Massive impact
- Slow but deadly

### 2.5 Massive (10+ kg)

**Examples:**
- Greatswords
- Massive war hammers
- Specific legendary weapons

**Visual Behavior:**
- Slow but devastating
- Long animations
- Major recovery
- Cinematic

**Audio:**
- Heaviest sounds
- Earth-shaking impacts
- Memorable

**Player Feel:**
- Weight you wear
- Strategic critical
- Each hit decisive
- Mastery required

### 2.6 Momentum Conservation

**Realistic Physics:**
- Heavy weapons keep momentum
- Hard to stop mid-swing
- Direction changes costly
- Realistic limitation

**Strategic Implications:**
- Plan attacks
- Commit to swings
- Read opponents
- Skill-based

### 2.7 Weight Affects Stamina

**Heavier = More Stamina:**
- Featherweight: 5 stamina/swing
- Light: 8 stamina/swing
- Standard: 12 stamina/swing
- Heavy: 18 stamina/swing
- Massive: 25 stamina/swing

**Cross-Reference:** mechanics/combat_feel.md (stamina system)

---

## 3. HIT IMPACT MECHANICS

### 3.1 The Strike Moment

**Multi-Phase Process:**

**Phase 1: Approach (0-50ms before impact)**
- Weapon traveling
- Target may dodge/parry
- Animation building
- Audio building

**Phase 2: Contact (Impact frame)**
- Hit registers
- Damage calculated
- Visual reaction triggered
- Audio peaks
- DualSense feedback

**Phase 3: Reaction (50-300ms after)**
- Stagger animation
- Blood effects
- Sound aftermath
- Tactical opening

**Phase 4: Recovery (300-1000ms)**
- Both fighters reset
- Position changes
- Strategic decisions
- Continue or break

### 3.2 Hit Location Matters

**Body Zones:**

**Head:**
- Critical damage
- Disorienting
- Possible instant kill (specific weapons)
- Visible reaction

**Torso (Center Mass):**
- Standard hit zone
- Moderate damage
- Knockback
- Stagger common

**Arms:**
- Reduced damage
- Weapon may drop
- Movement affected
- Tactical option

**Legs:**
- Reduced damage
- Movement compromised
- Strategic disable
- Cinematic possibilities

**Specific Critical Zones:**
- Throat (instant kill if cut)
- Heart (instant kill if pierced)
- Spine (paralysis/death)
- Eyes (blinding)

### 3.3 Damage Types

**Slashing:**
- Cuts skin/flesh
- Bleeding wounds
- Blood splatter visible
- Sword/cutlass standard

**Piercing:**
- Penetration damage
- Specific wound visible
- Less blood (initially)
- Rapier/spear/arrow

**Crushing:**
- Bone damage
- Bruising visible
- Deformation
- Hammer/mace

**Cleaving:**
- Major damage
- Severe wounds possible
- Heavy weapons
- Dramatic

**Burning:**
- Fire damage
- Specific visuals
- Ongoing damage
- Cross-reference physics/fire_physics.md

**Poisoning:**
- Slow damage
- Ines's specialty
- Status effect
- Cross-reference fauna/plants_and_herbs.md

### 3.4 Knockback Physics

**Force Calculation:**

**Light Hit:**
- Slight stagger
- Minimal movement
- Quick recovery

**Standard Hit:**
- Visible knockback
- Stagger animation
- Moderate recovery

**Heavy Hit:**
- Significant knockback
- Possibly fall
- Major recovery

**Critical Hit:**
- Massive knockback
- Possible knockdown
- Stunned period
- Vulnerable

### 3.5 Environmental Knockback

**Hits Push Targets:**
- Into walls (stunned)
- Off ledges (falling damage)
- Into water (specific consequences)
- Into hazards (fire, spikes)

**Strategic Use:**
- Position for environmental kill
- Knockback into traps
- Shoves into water
- Tactical depth

---

## 4. RAGDOLL SYSTEMS

### 4.1 Death Trigger

**Body Becomes Ragdoll When:**
- HP reaches 0
- Specific finishing moves
- Environmental death (long fall, etc.)
- Specific weapon kills

### 4.2 Ragdoll Components

**Joint System:**
- Realistic skeleton
- Each joint articulates
- Physics-driven
- Real-time

**Mass Distribution:**
- Realistic weight
- Heavier in torso
- Lighter in extremities
- Realistic falls

**Limb Independence:**
- Limbs can move independently
- Realistic arrangement
- Specific damage affects specific limbs
- Detailed

### 4.3 Death Pose Variations

**Based On:**

**Hit Direction:**
- Forward strike: falls back
- Backstrike: falls forward
- Side strike: falls sideways
- Realistic physics

**Hit Force:**
- Light: slumps
- Standard: falls
- Heavy: thrown
- Massive: launched

**Current Pose:**
- Standing: standard fall
- Crouched: slumps
- Mid-attack: caught mid-motion
- Running: continues briefly then falls
- Realistic

**Current Velocity:**
- Stationary: simple fall
- Walking: stumbles forward
- Running: tumbles
- Realistic continuation

### 4.4 Specific Death Animations

**Style B (Brutal):**
- Realistic violent
- Blood splatter
- Visceral
- Honest

**Style C (Quirky):**
- Some absurd moments
- Big Tom's exaggerated reactions
- Comic relief sometimes
- Real but lighter

**Style D (Reverent):**
- Dignified falls
- Slower motion sometimes
- Atmospheric
- Respectful

**Style A (Philosophical):**
- Thoughtful framing
- Sometimes slow
- Meaningful pauses
- Cinematic

### 4.5 Body Persistence

**Bodies Stay:**
- Remain in world
- Don't despawn quickly
- Atmospheric
- Realistic consequence

**Decomposition Over Time:**
- Eventually scavengers come
- Vultures (fauna integration)
- Realistic time
- Atmospheric

**Recovery:**
- Loot bodies
- Bury bodies (cultural)
- Burial at sea (Father Salvio)
- Memorial possible

### 4.6 Ship Combat Ragdoll

**Bodies on Deck:**
- Slide with ship motion
- Roll into corners
- Realistic physics
- Atmospheric

**Falling Overboard:**
- Hit water
- Briefly float
- Eventually sink
- Realistic

**Rigging Death:**
- Falls from rigging
- Impacts deck
- Specific visuals
- Memorable

---

## 5. BLOOD AND GORE

### 5.1 Blood Physics

**Realistic Behavior:**
- Splatter on hit
- Pools on ground
- Drips from wounds
- Soaks into surfaces

**Splatter Patterns:**

**Slashing:**
- Linear splatter
- Direction follows blade
- Specific patterns
- Realistic

**Piercing:**
- Spurt from wound
- Less external splatter
- Specific visual
- Realistic

**Crushing:**
- Less blood
- More internal damage
- Visible bruising
- Realistic

**Major Wounds:**
- Heavy bleeding
- Pooling
- Dramatic
- Style B

### 5.2 Wound Persistence

**Visible Damage:**
- Cuts remain
- Wounds bleed visibly
- Clothing stained
- Realistic

**On Survivors:**
- Healed wounds = scars
- Permanent marks
- Character development
- Realistic

### 5.3 Style-Appropriate Blood

**Style B (Brutal):**
- Maximum visible blood
- Realistic gore
- Honest violence
- Standard

**Style C (Lighter):**
- Less emphasis
- Quicker fade
- Specific scenes
- Comic moments

**Style D (Reverent):**
- Specific framing
- Less explicit
- Atmospheric
- Respectful

**Style A (Philosophical):**
- Symbolic framing
- Specific composition
- Meaningful
- Cinematic

### 5.4 Player Settings

**Adjustable:**
- Blood intensity (0-100%)
- Gore level (None/Reduced/Standard/Maximum)
- Family-friendly mode possible
- Player choice

**Default:**
- Standard mature
- Style B baseline
- Adjustable up/down
- Inclusive design

### 5.5 Blood on Player

**Jake's Appearance:**
- Splatters from kills
- Persists for time
- Cultural reactions
- NPC notice

**Effect on NPCs:**
- Bloody Jake = scary
- Reputation impact
- Specific dialogue
- Realistic

**Cleaning:**
- Wash off (water, baths)
- Specific actions
- Cultural significance
- Restoration

---

## 6. PROJECTILE PHYSICS

### 6.1 Arrow Physics

**Realistic Behavior:**

**Trajectory:**
- Arc due to gravity
- Drops with distance
- Wind affects (physics/wind_systems.md)
- Realistic ballistics

**Drop Calculations:**
- 50m: minimal drop
- 100m: slight drop
- 150m: significant drop
- 200m+: major drop
- Players learn to compensate

**Speed:**
- Initial: ~60-80 m/s
- Decelerates with distance
- Realistic ranges
- Skill-based

### 6.2 Arrow Penetration

**Hit Behavior:**

**Soft Targets (Cloth, Flesh):**
- Penetrates fully
- Sticks in target
- Realistic
- Visual

**Medium Targets (Leather, Wood):**
- Penetrates partly
- Sticks
- Damage moderate
- Realistic

**Hard Targets (Metal, Stone):**
- Bounces off
- Specific clinking
- Minimal damage
- Realistic

**Heavy Armor:**
- Stops most arrows
- Realistic
- Specific arrow types help

### 6.3 Arrow Recovery

**Picking Up:**
- Arrows in environment
- Bodies have arrows
- Recovery realistic
- Resource management

### 6.4 Bullet Physics

**Different from Arrows:**
- Much faster (speed of sound)
- Less arc (still some)
- More penetration
- Wind less affected

**Trajectory:**
- 100m: minimal drop
- 300m: slight drop
- 500m+: significant
- Skilled shooting

**Penetration:**
- Through wood
- Through some armor
- Realistic stopping
- Specific calculations

**Specific Weapons:**

**Pistol:**
- Short range
- Quick
- Light
- Standard

**Musket:**
- Long range
- Slow reload
- Heavy
- Devastating

**Rifle (Imperial):**
- Longest range
- Most accurate
- Specific scenes
- Memorable

### 6.5 Cannonball Physics

**Massive Projectile:**
- Slower flight
- Massive damage
- Trajectory visible
- Realistic ballistics

**Range:**
- Naval combat
- Realistic accuracy
- Specific tactics
- Strategic

**Impact:**
- Devastating
- Building damage
- Ship damage
- Memorable

### 6.6 Throwing Weapons

**Knives:**
- Spinning flight
- Sticking on hit
- Specific physics
- Skill-based

**Hatchets:**
- Heavier flight
- Longer range
- Similar physics
- Effective

**Spears:**
- Long projectile
- Specific flight
- Memorable hits
- Cinematic

---

## 7. IMPACT ON ENVIRONMENT

### 7.1 Strikes on Surfaces

**Stone:**
- Sparks fly
- Specific sound
- Minor chip damage
- Atmospheric

**Wood:**
- Splinters
- Stuck weapons
- Specific damage
- Realistic

**Metal:**
- Sparks
- Ringing sound
- Bouncing weapons
- Realistic

**Cloth/Fabric:**
- Tears
- Soft impact
- Specific sound
- Realistic

### 7.2 Destructible Environment

**Cross-Reference:** physics/destruction_physics.md (forthcoming)

**Combat Causes Damage:**
- Walls scarred
- Furniture broken
- Pots smashed
- Atmospheric

### 7.3 Stuck Weapons

**Realistic Behavior:**
- Heavy strikes embed weapons
- Recovery required
- Vulnerable moment
- Realistic

**Examples:**
- Axe in wall
- Sword in tree
- Spear in ground
- Strategic awareness

---

## 8. PARRIES AND DEFLECTIONS

### 8.1 Parry Mechanics

**Cross-Reference:** mechanics/combat_feel.md

**Physical Implementation:**
- Weapons clash visibly
- Sparks fly
- Specific sound
- Stagger possibility

### 8.2 Parry Sound

**Distinct Audio:**
- Metal on metal
- Specific tones
- Echoing
- Satisfying

### 8.3 Visual Effects

**Sparks:**
- Realistic generation
- Direction follows physics
- Beautiful
- Atmospheric

**Slow-Motion (Specific):**
- Critical parries
- Bloodborne moments
- Memorable
- Cinematic

### 8.4 Failure States

**Parry Fails:**
- Weapon glances
- Specific sound
- Vulnerable
- Realistic

**Weapon Breaks:**
- Possible (rare)
- Specific weapons
- Vulnerable moment
- Realistic

---

## 9. STAGGER SYSTEMS

### 9.1 Stagger Triggers

**When Targets Stagger:**

**Heavy Hit:**
- Significant damage
- Knockback
- Brief stun
- Standard

**Critical Hit:**
- Major damage
- Major stagger
- Tactical opening
- Important

**Posture Break:**
- Cross-reference mechanics/combat_feel.md
- Sekiro-tier mechanic
- Critical opening
- Memorable

**Specific Weak Points:**
- Hit specific zones
- Major stagger
- Skill rewarded
- Strategic

### 9.2 Stagger Animation

**Realistic Reactions:**
- Body responds to hit direction
- Specific recoil
- Recovery animation
- Realistic

**Continued Pressure:**
- Hit while staggered = bigger stagger
- Combo opportunity
- Strategic
- Memorable

### 9.3 Stagger Recovery

**Time-Based:**
- Light: 200-400ms
- Standard: 500-800ms
- Major: 1000-1500ms
- Critical: 2000-3000ms

**Player Awareness:**
- Predictable
- Strategic timing
- Combo windows
- Skill development

---

## 10. SPECIFIC WEAPON BEHAVIORS

### 10.1 Sword Combat

**Cutlasses:**
- Pirate standard
- Light, fast
- Slashing focus
- Specific feel

**Rapiers:**
- Imperial style
- Piercing focus
- Quick thrusts
- Specific tactics

**Longswords:**
- Versatile
- Both hands possible
- Standard knight feel
- Solid

**Greatswords:**
- Two-handed always
- Massive damage
- Slow
- Memorable

### 10.2 Axe Combat

**Hand Axes:**
- Versatile
- Throwable
- Big Tom uses
- Practical

**Battle Axes:**
- Two-handed
- Devastating
- Slow
- Specific feel

**Boarding Axes:**
- Pirate specific
- Multi-purpose
- Sailor's tool
- Versatile

### 10.3 Hammer/Mace Combat

**War Hammers:**
- Crushing damage
- Armor-defeating
- Heavy
- Big Tom's preference

**Maces:**
- Spike or smooth
- Crushing
- Specific feel
- Realistic

### 10.4 Polearms

**Spears:**
- Long range
- Piercing
- Specific tactics
- Throwable

**Halberds:**
- Multi-purpose
- Imperial military
- Specific feel
- Versatile

### 10.5 Firearms

**Pistols:**
- Single shot
- Quick draw
- Limited range
- Pirate standard

**Muskets:**
- Two-handed
- Long range
- Slow reload
- Devastating

**Blunderbusses:**
- Spread shot
- Short range
- Pirate special
- Memorable

**Cannons:**
- Ship-mounted
- Massive
- See physics/ship_physics.md (forthcoming)

### 10.6 Bows and Crossbows

**Bows:**
- Quick reload
- Quiet
- Multiple arrow types
- Mira's specialty

**Crossbows:**
- Slower reload
- More powerful
- Specific feel
- Tactical

---

## 11. UNARMED COMBAT

### 11.1 Punching

**Realistic Physics:**
- Fist mass
- Speed
- Body weight transfer
- Real damage

### 11.2 Grappling

**Wrestling Mechanics:**
- Realistic holds
- Counters possible
- Specific gameplay
- Tactical

### 11.3 Kicks

**Powerful Strikes:**
- Body weight engaged
- Specific damage
- Tactical
- Realistic

### 11.4 Improvised Weapons

**Environment as Weapon:**
- Bottles
- Chairs
- Rocks
- Specific objects
- Realistic

---

## 12. DUAL WIELDING

### 12.1 Two-Weapon Combat

**Specific Implementation:**
- Off-hand attacks
- Different rhythms
- Specific combos
- Skill-based

### 12.2 Off-Hand Items

**Beyond Weapons:**
- Lanterns
- Pistols (one shot)
- Daggers (parrying)
- Strategic options

### 12.3 Two-Hand to Dual

**Switching:**
- Some weapons two-hand or dual
- Specific moments
- Tactical depth
- Player choice

---

## 13. COMBAT FEEDBACK

### 13.1 Visual Feedback

**On Hit:**
- Hit sparks
- Blood (style appropriate)
- Specific particle effects
- Atmospheric

**On Block:**
- Specific sparks
- Different from hit
- Player understands
- Clear

**On Miss:**
- No specific feedback
- Just animation
- Clear failure
- Realistic

### 13.2 Audio Feedback

**Layered Audio:**

**Weapon:**
- Whoosh through air
- Strike sounds
- Specific to weapon

**Target:**
- Body reactions
- Voice (grunts, screams)
- Environmental
- Layered

**Distance:**
- Clear if close
- Muffled if far
- Realistic

### 13.3 DualSense Feedback

**Specific Feedback:**

**On Strike:**
- Sharp impact
- Direction-specific
- Weapon-specific
- Memorable

**On Block:**
- Different feel
- Vibration pattern
- Specific
- Clear

**On Critical:**
- Maximum feedback
- Memorable moment
- Earned satisfaction
- Bloodborne-tier

### 13.4 Time Effects

**Hit Stop:**
- Brief freeze on impact
- Cinematic emphasis
- Bloodborne signature
- Specific frames

**Slow Motion (Specific):**
- Critical kills
- Specific scenes
- Memorable
- Tasteful use

---

## 14. SPECIFIC ENVIRONMENTAL COMBAT

### 14.1 Combat in Water

**Mechanics:**
- Slower
- Heavier feel
- Specific tactics
- Limited

**Cross-Reference:** physics/water_physics.md Section 10

### 14.2 Combat in Snow

**Mechanics:**
- Slipping risk
- Tracks visible
- Cold concerns
- Specific feel

### 14.3 Combat on Ship

**Mechanics:**
- Wave motion
- Falling overboard
- Specific challenges
- Cinematic

### 14.4 Combat in Buildings

**Tight Spaces:**
- Long weapons impractical
- Knives/daggers preferred
- Specific tactics
- Realistic

### 14.5 Mounted Combat (Limited)

**On Horse:**
- Specific scenes only
- Pirate-era reality
- Limited gameplay
- Specific moments

---

## 15. NPC COMBAT BEHAVIOR

### 15.1 NPC Reactions

**Bystanders:**
- Flee from combat
- Realistic responses
- Cultural variation
- Atmospheric

**Combatants:**
- Engage realistically
- Tactical AI
- Specific behaviors
- Skill-based

### 15.2 Specific Crew Combat

**Mira:**
- Twin swords
- Efficient style
- Specific physics
- Smooth

**Big Tom:**
- War hammer
- Devastating swings
- Heavy physics
- Memorable

**Ines:**
- Stays back
- Throws poison vials
- Specific mechanics
- Strategic

**Father Salvio:**
- Walking staff
- Defensive style
- Specific feel
- Surprising

**Esteban:**
- Limited combat (age)
- Specific support
- Specific gameplay

---

## 16. TRAINING AND LEARNING

### 16.1 Player Skill Development

**Beginner:**
- Basic swings
- Misses common
- Learning feel

**Intermediate:**
- Combos working
- Tactical thinking
- Improvement visible

**Advanced:**
- Master combinations
- Reading opponents
- Tactical mastery

**Master:**
- Bloodborne-tier
- Smooth flow
- Crew respects
- Memorable

### 16.2 NPC Teachers

**Mira:**
- Teaches dual-wield
- Naval combat
- Practical

**Big Tom:**
- Heavy weapons
- Brutal honesty
- Practical

**Specific NPCs:**
- Master swordsmen exist
- Specific quests
- Skill development
- Memorable

---

## 17. ACCESSIBILITY

### 17.1 Difficulty Settings

**Combat Tier:**
- Casual: Forgiving
- Standard: Realistic
- Bloodborne: Hard
- Player choice

### 17.2 Visual

**Adjustable:**
- Blood intensity
- Gore level
- Specific options
- Player friendly

### 17.3 Motor

**Specific Settings:**
- Auto-target options
- Reduced precision
- Forgiving timing
- Inclusive

---

## 18. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** Combat dialogue varies by character. Aristocrats fight in measured High style. Pirates curse in Low. Dialogue during combat reflects character.
- ✅ **Pillar 2 (Lore Through Objects):** Weapons reveal histories. Specific blades have stories. Combat objects carry weight beyond mechanics.
- ✅ **Pillar 3 (Layered Death):** Death animations match style. Style B brutal honesty. Style D reverent. Each kill weighed.
- ✅ **Pillar 4 (Layered Humor):** Style C combat moments (Big Tom's exuberance, comic moments in chaos).
- ✅ **Pillar 5 (Theme — Revenge):** Combat physics reflect Jake's path. Brutal physics for vengeance. Clean physics for measured. Atmospheric mirror.

---

## 19. NEXT STEPS

Next document: **`physics/ship_physics.md`** — Combat & Movement Tier Document 2

Will cover:
- Ship buoyancy detailed
- Wave-ship interaction (cross-reference water_physics)
- Naval combat physics
- Ship damage and sinking
- Cannon ball physics
- Boarding action physics

After Ship Physics: Movement Physics (final Combat & Movement document).

---

## 20. CONCLUSION

Combat in Dark Arisen is not animation playing damage numbers. It is physical reality. Each strike has weight. Each death has uniqueness. Each parry has spark and sound. Each arrow has realistic flight.

When players feel a heavy two-handed sword arc through the air with proper momentum, when they see a body fall realistically based on hit angle, when they compensate for arrow drop at distance, when blood splatters tell the story of the wound — that's when combat transcends design and becomes experience.

This is Bloodborne-tier feel through systemic physics. The third pillar of the world's reality.

**Phase 6 Subsystem 5 of 15 — complete.**

**The blade arcs. The body falls. The blood marks the moment.** ⚔️🏴‍☠️
