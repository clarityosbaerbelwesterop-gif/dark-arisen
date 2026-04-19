# TRAVERSAL SYSTEM

**Status:** Design Draft v1.0  
**Phase:** Phase 3 — Mechanics (Subsystem 3 of 10)  
**Tier:** Core Tier — Peer to Combat Feel, depends on Controls Foundation  
**Related:** mechanics_overview.md, mechanics/controls.md, mechanics/combat_feel.md, style_bible.md  
**Purpose:** Define all non-combat movement systems in Dark Arisen. Walking, running, climbing, swimming, jumping, falling, and environmental interaction.  
**Inspirations:** Bloodborne (movement weight), Red Dead Redemption 2 (naturalistic animation), Sekiro (grapple hook verticality), Uncharted (cinematic climbing), Breath of the Wild (stamina-based exploration), Assassin's Creed Black Flag (ship rigging)

---

## 1. TRAVERSAL PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Movement Has Weight

Jake is a man, not a ninja. He has mass. He accelerates into motion. He decelerates when stopping. His momentum affects every action. A Jake running at full sprint cannot instantly stop — he skids, he repositions, he commits.

**This is the Bloodborne/RDR2 heritage.** Movement feels grounded, realistic, and physical.

### 1.2 Principle 2: The World Is Traversable, Not Parkour-able

Dark Arisen is NOT Assassin's Creed. Jake cannot climb every surface. Climbable surfaces are specifically marked (visually subtle but consistent). This creates intentional routing — the player must READ the environment, not assume they can go anywhere.

**Design intent:** The world should feel like an actual world, not a climbing puzzle. When Jake DOES climb, it matters. When he can't climb, he finds another way.

### 1.3 Principle 3: Water Is a Dimension, Not a Barrier

In most pirate games, water is aesthetic or lethal. In Dark Arisen, water is a real dimension of gameplay. Jake can swim, dive, hold breath, fight underwater in limited contexts, and navigate three-dimensional underwater spaces. This matters for:

- Pirate raids requiring waterline infiltration
- Shipwreck exploration
- Havfrue Modor's encounter
- Region 06's Heart-Chamber sequences
- Multiple quest-critical moments

### 1.4 Principle 4: Stamina Is the Universal Resource

Combat stamina, traversal stamina, and swimming stamina all come from the SAME pool. This creates meaningful choices — if Jake sprints to the fight, he has less stamina to fight with. If he climbs a long cliff, he arrives tired. The body is one body.

### 1.5 Principle 5: Traversal Tells Story

How Jake moves tells the player about his state. Injured Jake limps. Exhausted Jake breathes heavily. Cold Jake shivers. Wet Jake's clothing sticks, slowing him. Carrying heavy crew member from combat slows his run. **Traversal is narrative.**

---

## 2. WALKING — THE BASE STATE

### 2.1 Walk Specifications

| Parameter | Value | Notes |
|-----------|-------|-------|
| Base Speed | 3.5 m/s | Standard comfortable pace |
| Acceleration | 0.3 seconds to full speed | Feels grounded |
| Deceleration | 0.2 seconds to stop | Less committal than sprint |
| Stamina Cost | 0 | Free action |
| Turning Radius | Instant (snap to direction) | Full responsiveness |

### 2.2 Walk Variations

**Slow Walk (L Stick partial tilt):**
- 1.8 m/s
- Used in cautious exploration, investigation sequences
- Triggers investigation dialogue from Jake automatically

**Standard Walk (L Stick full tilt, no sprint):**
- 3.5 m/s (baseline)
- Default exploration pace

**Cautious Walk (ceremonial, scripted sequences):**
- 2.5 m/s
- Used in cinematic moments, sacred spaces (Havfrue Modor arena, funeral scenes)
- Jake's posture more formal, weapon holstered

### 2.3 Walking on Different Surfaces

Each surface affects walk feel and creates haptic/audio feedback:

| Surface | Speed Modifier | Haptic Feel | Audio |
|---------|----------------|-------------|-------|
| Stone | 100% | Firm, crisp pulses | Sharp heel-toe clicks |
| Wood (deck) | 100% | Slightly hollow | Deep wooden thuds |
| Sand | 85% | Soft, muffled | Soft crunching |
| Mud | 70% | Heavy drag | Wet squelching |
| Shallow Water | 75% | Splashing | Water displacement |
| Snow | 75% | Crunch with weight | Compacting snow |
| Ice | 60% (slippery) | Minimal control | Slight sliding |
| Grass | 100% | Standard | Muffled footsteps |
| Metal (ship grating) | 100% | Metallic | Clanging |
| Carpet/Cloth | 100% | Soft, damped | Quiet |

### 2.4 The Walking Narrative

When walking, Jake experiences ambient awareness:
- Notices environmental details (will automatically look toward interesting objects)
- NPCs acknowledge him (nods, greetings, side-eye depending on reputation)
- Ambient conversations audible at walking speed (sprint muffles them)
- Jake's internal monologue may trigger in specific locations (via voiceover, Chapter 5+)

---

## 3. RUNNING — THE COMMON PACE

### 3.1 Run Specifications

| Parameter | Value | Notes |
|-----------|-------|-------|
| Base Speed | 6.5 m/s | Steady jog, sustainable |
| Acceleration | 0.5 seconds to full speed | From walk |
| Deceleration | 0.4 seconds to stop | Moderate commitment |
| Stamina Cost | 0 (free) | Running is sustainable |
| Turning Radius | 0.3-second transition | Feels weighted |

### 3.2 Run Activation

- **Input:** L Stick full tilt (without sprint engaged)
- **Automatic:** When combat distances matter but not urgent
- **Cancel:** Stop (go to walk), sprint (engage faster speed), or combat

### 3.3 Run in Combat Context

When weapon is drawn:
- Run speed reduced to 5.5 m/s (weapon ready stance is slightly slower)
- Combat transitions instant (attacks interrupt run smoothly)
- Lock-on doesn't cancel run, but circles target instead of moving away

### 3.4 Running Obstacles (Auto-Vault)

While running, Jake automatically vaults over low obstacles:
- **Knee-high walls** (under 0.8m): Auto-vault, 0.3 second animation
- **Waist-high walls** (0.8m - 1.2m): Auto-vault if Cross (jump) is pressed
- **Chest-high walls** (1.2m - 1.6m): Requires jump + grab (auto-triggers climb)
- **Over chest height:** Requires deliberate climb activation

Design principle: The player should NEVER be stopped by minor geometry while running. Flow matters.

---

## 4. SPRINTING — THE COMMITTED BURST

### 4.1 Sprint Specifications

| Parameter | Value | Notes |
|-----------|-------|-------|
| Base Speed | 10 m/s | Maximum ground speed |
| Acceleration | 0.8 seconds to full speed | Builds momentum |
| Deceleration | 0.6 seconds to stop | Heavy commitment |
| Stamina Cost | 10 / second | Finite resource |
| Turning Radius | 0.6-second transition | Limited agility |
| Minimum Duration | 0.5 seconds | Cannot toggle rapidly |

### 4.2 Sprint Input

**Hold Mode (default):**
- L3 hold while moving = sprint
- Release L3 = return to run
- Physical commitment through button hold

**Toggle Mode (accessibility):**
- Click L3 = sprint toggle
- Click L3 again = stop sprinting
- Auto-stops when stamina depletes
- Less physically demanding

### 4.3 Sprint Exhaustion

When stamina reaches 0 while sprinting:
- Jake is FORCED to walking speed (3.5 m/s)
- Duration: 3 full seconds minimum
- Cannot immediately sprint again
- Visual: Jake bent forward, breathing heavily (animation state)
- Audio: Heavy panting
- Combat ineffective — attacks have -25% damage during exhaustion

**Design intent:** This matters tactically. A Jake who sprinted to combat has weaker opening attacks. Planning matters.

### 4.4 Sprint Special Abilities

**Sprint Attack (first hit from sprint):**
- +15% damage
- +25% posture damage
- Increased range (Jake's forward momentum)

**Sprint Dodge (dodge from full sprint):**
- +30% distance (from 4m base to 5.2m)
- +2 i-frames (from 12 to 14)
- Stamina cost unchanged (25)

**Sprint Jump (jump from full sprint):**
- +50% horizontal distance
- Auto-triggers parkour vault on impact
- Can cross gaps up to 3m (vs 1.5m baseline jump)

### 4.5 Sprint Through Different Terrain

| Terrain | Sprint Speed | Notes |
|---------|--------------|-------|
| Clear ground | 100% (10 m/s) | Baseline |
| Grass, light brush | 95% | Minor resistance |
| Deep grass, brush | 80% | Noticeable drag |
| Sand | 75% | Heavy drag |
| Mud | 50% | Severe slowdown |
| Shallow water | 60% | Significant resistance |
| Ship deck | 100% (unless wet, then -15%) | Naval context |
| Rigging (on ropes) | N/A — climb instead |

---

## 5. JUMPING — DELIBERATE VERTICAL

### 5.1 Jump Specifications

| Parameter | Value | Notes |
|-----------|-------|-------|
| Base Height | 1.2m | Clear waist-high obstacles |
| Sprint-Assisted Height | 2.0m | Clear chest-high obstacles |
| Standing Jump Distance | 2.5m | From stationary |
| Running Jump Distance | 4.0m | From run |
| Sprint Jump Distance | 5.5m | From sprint |
| Stamina Cost | 12 | Per jump |
| Windup | 6 frames | Input delay |
| Active (in air) | Variable (trajectory-based) | |
| Recovery (on landing) | 12 frames (no sprint) / 8 frames (sprint-maintained) |

### 5.2 Jump Input

- **Input:** Cross button (PS) / A (Xbox) / Space (PC)
- **Directional:** L Stick determines trajectory
- **Hold for higher:** Holding Cross extends upward velocity by 20%
- **Auto-cancel:** If Jake would fall into lethal height, jump doesn't execute (prevents accidental deaths from ledges)

### 5.3 Jump Context Variations

**Walking Jump:**
- Vertical 1.2m, forward 2.5m
- Quick recovery
- Used for minor gaps

**Running Jump:**
- Vertical 1.6m, forward 4.0m
- Standard gap-crossing
- Mid-combat escape option

**Sprint Jump:**
- Vertical 2.0m, forward 5.5m
- Dramatic, committed
- Used for major gaps, escape routes

**Combat Jump:**
- While weapon drawn, jump height -20%
- Reflects weight of weapons
- Exception: Dagger class unaffected

**Jumping from Ship Rigging:**
- Different physics — Jake is elevated
- Can fall controllably onto deck, ground, or water
- Damage calculated by height of fall, not force of jump

### 5.4 What Jumping Is NOT For

Dark Arisen is not an action-platformer. Important limits:

- **No mid-air attacks** (except specific boss encounters like Caldera Titan tremor avoidance)
- **No double-jumping** (ever — this is grounded combat)
- **No wall-running** (Jake is not a ninja)
- **No parkour chains** (climbing is deliberate, not fluid)

### 5.5 Gap-Crossing Heuristics

The game provides visual/audio cues for jump difficulty:

- **Safe gaps (under 2.5m):** Jake confidently leaps
- **Dangerous gaps (2.5m - 4m):** Jake hesitates briefly before commit (tell frame)
- **Impossible gaps (over 5.5m):** Jump doesn't execute — Jake stops at edge

**Design intent:** The game respects the player's intelligence. Not every edge is a suicide. But not every gap is jumpable either. Reading distances is a skill.

---

## 6. CLIMBING — DELIBERATE VERTICALITY

### 6.1 Climbing Philosophy

Climbing in Dark Arisen is NOT Assassin's Creed parkour. It is deliberate, specific, and contextual. Only marked surfaces can be climbed. The marking is subtle but consistent — if the player learns to read the environment, climbing feels natural.

### 6.2 Climbable Surface Indicators

**Visual cues (all used together):**
- Slightly brighter edge highlighting (barely visible on cliffs)
- Rope hanging from top (explicit)
- Ladder structure (obvious)
- Grappling point (marked with small metal hook)
- Natural handholds visible (rocky protrusions, root systems, crevices)
- Ship rigging (always climbable)

**What is NOT climbable:**
- Smooth walls (unless ship side with specific ropes)
- Decorative stone facades
- Most trees (unless specifically marked)
- Roofs (unless ladder-accessible)

### 6.3 Climbing Specifications

| Parameter | Value | Notes |
|-----------|-------|-------|
| Climbing Speed | 2 m/s | Deliberately slow |
| Stamina Cost | 5 / second | Sustainable for ~24 sec at baseline |
| Maximum Climb Height (continuous) | 50m | Before forced rest |
| Recovery at top | 2 seconds | Jake pulls himself up, dusts off |
| Drop Option | Circle from climb | Jake lets go and falls |

### 6.4 Climbing Contexts

**Natural Cliffs (cave walls, mountain paths):**
- Moderate speed (2 m/s)
- Handholds visible
- Some paths require specific routing (ledges, traverse sections)
- Stamina-critical for long climbs

**Rope Climbing (fixed ropes):**
- Slightly faster (2.5 m/s)
- Most common in Fort Carrion, ship boarding
- Jake visibly grips and pulls
- Minor stamina bonus (-1/second cost)

**Ladder Climbing (wooden/metal ladders):**
- Fastest climbing method (3 m/s)
- Standard in pirate settings
- Jake climbs with rhythm
- Minimal stamina (-2/second cost)

**Ship Rigging (ropes, masts):**
- 2.5 m/s
- Unique to naval context
- Different animation set (sailor-style climbing)
- Stamina cost standard

**Grappling Hook (Chapter 4+ ability):**
- Fired at marked points
- Pulls Jake at 5 m/s (fastest vertical)
- Stamina cost: 20 per use (burst)
- Limited range: 15m maximum
- Specific marked grapple points

### 6.5 Climbing Mechanics

**Hand-Over-Hand:**
- When climbing vertically, Jake moves hand by hand
- Each "hand move" is 0.5 seconds
- Can slow or stop on any handhold

**Traversing (sideways):**
- Most climbing surfaces allow horizontal traversal
- Speed: 1.5 m/s sideways
- Used for navigating cliff faces

**The Grip Strength System:**
- Stamina represents grip
- At 25% stamina, Jake's hands visibly strain
- At 10% stamina, audio/haptic warning — slipping imminent
- At 0% stamina, Jake FALLS

**The Fall Survival:**
- If Jake runs out of stamina mid-climb:
  - 0-5m height: No damage
  - 5-10m: 30% HP damage
  - 10-20m: 60% HP damage
  - 20m+: Likely fatal (unless water below)

### 6.6 Wet/Weather Climbing

Weather affects climbing significantly:

| Condition | Speed Modifier | Stamina Modifier | Notes |
|-----------|----------------|------------------|-------|
| Dry | 100% | Base | Standard |
| Wet (rain) | 80% | +50% cost | Dangerous, slower |
| Storm | 60% | +100% cost | Very difficult |
| Hurricane | Impossible | N/A | Blocks climbing |
| Cold/snow | 90% | +20% cost | Numb fingers |
| Ice-covered | Impossible | N/A | Cannot grip |

---

## 7. FALLING — GRAVITY AND CONSEQUENCE

### 7.1 Fall Damage Calculations

Falling damage scales with height:

| Height | Damage |
|--------|--------|
| 0-3m | No damage (Jake absorbs with bent knees) |
| 3-6m | 15% HP damage |
| 6-10m | 40% HP damage |
| 10-15m | 75% HP damage (may knock unconscious) |
| 15-25m | 95%+ HP damage (near-lethal) |
| 25m+ | Guaranteed fatal |

### 7.2 Fall Mitigation

Several techniques reduce fall damage:

**Roll on Landing (Circle timed correctly):**
- Input: Circle at 0.2 seconds before ground contact
- Effect: Damage reduced by 50%
- Stamina cost: 25
- Timing window: 12 frames

**Landing on Water:**
- From any height under 15m: No damage
- From 15-30m: 20% HP damage (impact at high speed)
- From 30m+: 60% HP damage
- Deep water preferred (shallow water deals bone-breaking damage)

**Landing on Soft Surfaces:**
- Hay bales, cargo nets, cloth sails: -75% damage
- Grass, leaves: -20% damage
- Mud, sand: -15% damage
- Snow: -30% damage

**Landing on Enemies (assassination fall):**
- Jake lands ON enemy: Instant kill on standard enemies
- Elite enemies: -30% damage plus critical window
- Bosses: Immune (they're too heavy/aware)
- Fall damage to Jake: Reduced by 60%

### 7.3 Controlled Falling

**Rappel (ropes):**
- If rope is climbable, Jake can "release" controlled
- Circle button releases grip, but Jake grabs rope again lower
- Descends at 4 m/s safely

**Jump from Rigging:**
- Jake can intentionally jump from masts
- Calculated trajectory shown briefly (in tutorial)
- Skilled players use this for dramatic entries

### 7.4 Death from Fall

If Jake takes fatal fall damage:
- Screen turns red
- Audio goes distant
- Death animation begins (2 seconds)
- Respawn at last checkpoint
- Lesson learned (hopefully)

---

## 8. SWIMMING — THE SECOND DIMENSION

### 8.1 Swimming Philosophy

Water in Dark Arisen is a real gameplay dimension. Jake must swim, navigate depth, manage breath, and interact with underwater environments.

### 8.2 Entering Water

**Wading (shallow water, 0-1m depth):**
- Treated as ground with -25% speed
- Jake walks normally
- Lower body wet

**Standing Swim (1-2m depth):**
- Jake automatically transitions to swim
- Floats at surface
- Standard animation

**Dive Entry (from height):**
- Falling into water from 3m+ automatically triggers dive
- Jake can dive deeper immediately
- Preferred entry style

### 8.3 Surface Swimming

| Parameter | Value | Notes |
|-----------|-------|-------|
| Base Surface Speed | 2.5 m/s | Moderate pace |
| Sprint Swim Speed | 4 m/s | Stamina-costly |
| Base Stamina Cost | 8 / second | Sustainable for 15 sec |
| Sprint Stamina Cost | 15 / second | Dangerous for long distances |
| Turning | Smooth 360° | Intuitive control |

### 8.4 Underwater Swimming

| Parameter | Value | Notes |
|-----------|-------|-------|
| Underwater Speed | 2 m/s | Slower than surface |
| Dive Depth | Up to 20m | Beyond this, pressure issues |
| Maximum Depth | 30m | At this depth, forced ascent |

### 8.5 Breath Meter

Critical new mechanic — Jake must breathe.

| Stat | Value | Notes |
|------|-------|-------|
| Base Breath | 45 seconds | Chapter 4 baseline |
| Breath with Chapter 9 Upgrade | 90 seconds | Significant increase |
| Havfrue's Blessing | 180 seconds | Peace Route reward |
| Breath Loss Rate (calm) | 1 / second | Standard |
| Breath Loss Rate (combat) | 2 / second | Higher exertion |
| Breath Loss Rate (deep) | 1.5 / second | Pressure |

**Breath Warning States:**
- **75% breath:** No visible warning
- **50% breath:** Slight screen edge pulse
- **25% breath:** Audible heartbeat, urgent haptic
- **10% breath:** Screen visibly darkens, urgent audio
- **0% breath:** Jake begins taking drowning damage (15% HP/second)

### 8.6 Underwater Combat

Limited but possible combat underwater:

**Allowed:**
- Dagger attacks (reduced damage, -30%)
- Short sword attacks (reduced damage, -30%)
- Bow use (reduced range, arrows curve through water)

**Not Allowed:**
- Firearms (don't function underwater)
- Heavy weapons (Jake cannot swing effectively)
- Parry (too slow in water)

**Underwater-Specific:**
- Swim-tackle (grapple enemy, drag them deeper, drown them)
- Blade draw from below (surprise attack on surface enemies)
- Breath-hold endurance combat (rare, only at Havfrue's Peace Route)

### 8.7 Swimming Environments

**Calm Open Water:**
- Standard swimming
- Can be sustained for long distances

**Rough Seas:**
- Waves push Jake around (-30% speed)
- Requires more stamina management
- Dangerous in combat

**Storm Waters:**
- Severe stamina drain
- Can be thrown by waves
- High drowning risk

**Underwater Caves:**
- Limited light
- Pressure concerns
- Specific Region 06 gameplay

**Fjordlund Cold Water:**
- Temperature damage (if Jake lacks cold-weather gear)
- -1 HP/second when cold-vulnerable
- Requires specific warming items afterward

**Tropical Waters:**
- Higher comfort
- Faster swimming (+5%)
- Sea life interactions (some aggressive)

### 8.8 Sea Life Awareness

Underwater, Jake is NOT alone:

- **Sharks:** Attracted by blood (Jake's HP loss)
- **Jellyfish:** Passive, but touching them causes damage + slowness
- **Giant Squid (rare):** Can attack Jake in certain regions
- **Rays:** Passive, but can sting if touched
- **Whales:** Peaceful (except in Region 03 mythical encounters)
- **Havfrue Modor's Children (Region 06):** Respectful response only

---

## 9. LADDERS AND STAIRS — STANDARD VERTICAL

Beyond climbing, standard architectural verticality:

### 9.1 Ladders

| Type | Speed | Stamina | Notes |
|------|-------|---------|-------|
| Standard Ladder | 3 m/s | -2/sec | Hand-over-hand animation |
| Ship Rigging Ladder | 2.5 m/s | -1/sec | Naval context |
| Rope Ladder | 2 m/s | -3/sec | More challenging |
| Broken Ladder | Slow, unstable | -5/sec | Quest-specific |

### 9.2 Stairs

- **Speed:** Same as walking/running (no modifier)
- **Sprint up:** Reduced speed (-15%)
- **Sprint down:** Full speed, but fall risk on narrow stairs
- **Automatic foot placement:** Jake's animations handle stairs naturally

### 9.3 Ramps/Inclines

- **Gentle slope (0-15°):** No modifier
- **Medium slope (15-30°):** -10% speed going up, +5% going down
- **Steep slope (30-45°):** -25% speed up, +10% down (may slide)
- **Very steep (45°+):** Forces climb mode

---

## 10. SPECIAL TRAVERSAL SYSTEMS

### 10.1 The Grappling Hook (Chapter 4 unlock)

Jake acquires a grappling hook during the Rexa assault.

**Specifications:**
- Range: 15m maximum
- Marked points required (not all surfaces)
- Pull speed: 5 m/s
- Stamina cost: 20 per use
- Cooldown: 3 seconds between uses

**Uses:**
- Rapid vertical ascent
- Gap crossing (up to 15m)
- Pulling objects from distance
- Boarding actions (ship-to-ship)
- Combat (can pull light enemies)

### 10.2 Rope Swinging (Chapter 5+)

In specific locations (pirate villages, ship rigging), ropes allow swinging:

- **Speed:** 8 m/s at peak swing
- **Range:** Arc limited by rope length
- **Control:** L Stick controls release timing
- **Style:** Pirate-romantic animation (Jake grins)

### 10.3 Slide Mechanics (Rare, Situational)

On specific slopes or during specific events:

- Auto-triggers on very steep surfaces (45°+)
- Jake slides at 8 m/s
- Can steer left/right
- Collision with obstacles causes damage

### 10.4 Ice Skating (Fjordlund, Chapter 7)

On frozen lakes, specific movement style:
- 12 m/s on ice
- Minimal stamina cost (-5/sec)
- Turning is wide (not agile)
- Fun, thematic moments

### 10.5 Horse Riding (Quiet Coast Only)

In the Fjordlund region, horses are available:
- Speed: 14 m/s sprint
- Stamina: Uses horse stamina pool (separate)
- Bond: Horse bonds like weapons over time
- Limited: Not all regions have horses
- Design intent: RDR2-inspired but context-limited

---

## 11. STAMINA INTERACTION WITH TRAVERSAL

### 11.1 Shared Stamina Pool

As established in mechanics_overview.md, all stamina actions come from one pool:

- Combat actions
- Traversal actions
- Swimming
- Climbing
- Interaction (lifting, carrying)

This forces meaningful choice: Jake cannot be infinitely mobile AND infinitely combat-ready.

### 11.2 Stamina Regeneration Rules

| State | Regen Rate |
|-------|------------|
| Standing still (weapon holstered) | 25/second |
| Walking | 20/second |
| Running | 15/second |
| Sprinting | 0 (drains) |
| Climbing | 0 (drains -5) |
| Swimming | 0 (drains -8) |
| In Combat | 15/second (even when not attacking) |
| In Combat + Actively Hitting | 10/second |
| Exhausted (post-depletion) | 5/second for 3 sec, then full |

### 11.3 Stamina Strategies

**The Traversal Pre-plan:**
Before engaging long traversal, Jake should have full stamina. Running out mid-climb is dangerous.

**The Conservation Run:**
When combat is expected, walk or run (not sprint) to maintain stamina for the fight.

**The Commit Sprint:**
When immediate speed matters, sprint — but know you'll have exhaustion consequences.

**The Resting Pause:**
Find safe spots to wait for full stamina recovery. 20-30 seconds is usually sufficient.

---

## 12. WEATHER AND TRAVERSAL

Weather significantly affects traversal across all subsystems:

### 12.1 Rain Effects
- Slippery surfaces (-15% speed on stone, wood, metal)
- Poor visibility (-20% sight distance)
- Rope climbing harder (+50% stamina)
- Combat less affected (Jake is used to it)
- Mood: atmospheric, melancholy

### 12.2 Storm Effects
- Severe speed reduction (-25% all movement)
- Wind knockback (lateral forces)
- Extremely slippery cliffs
- Ship rocking impact on deck movement
- Dangerous for swimming
- Mood: threatening, intense

### 12.3 Fog Effects
- Minimal speed impact
- Severe visibility reduction (-60% sight)
- Stealth bonus (+40%)
- Combat harder (miss attacks)
- Mood: mysterious, slow

### 12.4 Snow Effects (Quiet Coast only)
- Deep snow slows movement (-30%)
- Climbing possible but stamina-draining (+20% cost)
- Ice patches cause slipping
- Freezing temperatures drain HP over time (if unprotected)
- Mood: harsh, solitary

### 12.5 Heat Effects (Rexa, Fort Carrion only)
- Baseline movement normal
- Stamina regen -15% without water
- Jake dehydrates (separate mechanic)
- Combat unaffected
- Mood: oppressive, sweaty

---

## 13. COMBAT-TRAVERSAL INTERACTION

### 13.1 Movement in Combat

When weapon is drawn:
- **Walk:** 100% normal speed
- **Run:** 85% normal speed (weapon-ready stance slower)
- **Sprint:** Available but transitions slower
- **Dodge:** Instant from any state
- **Backstep:** Shortened (2m vs 3m free)

### 13.2 Escape Traversal

During combat, Jake can:
- Break off and sprint to escape
- Climb away (if climbable surface available)
- Dive into water (common pirate escape)
- Jump off ships (to parallel ship or water)

Each escape method has tradeoffs:
- **Sprint escape:** Quickest, but enemy may pursue
- **Climb escape:** Slow, vulnerable to archers
- **Water escape:** Usually safe, but cold damage risk
- **Jump escape:** Dramatic, fall damage risk

### 13.3 Environmental Combat Using Traversal

Jake can use traversal mid-combat:

- **Kick enemy off cliff:** Triangle + directional attack near edge
- **Throw enemy over railing:** Grapple + directional push (ship combat)
- **Use high ground:** Climb during combat for tactical advantage
- **Dive to dodge:** Jump into water mid-combat to escape
- **Swing-kick:** Rope swing into enemy (rare but available)

---

## 14. PLATFORM-SPECIFIC TRAVERSAL

### 14.1 PS5 DualSense Features

**Haptic feedback during traversal:**
- Footsteps on different surfaces (Section 2.3)
- Climbing: Handhold grip sensation
- Swimming: Flow of water against body
- Jumping: Weight shift at apex
- Falling: Wind rushing, impact spike
- Running on sand: Soft scrunching in left grip

**Adaptive triggers:**
- L2 during climb holds: Tension matches grip strength
- R2 during grapple hook aim: Resistance builds

### 14.2 Xbox Series X/S

**Controller rumble:**
- Similar to PS5 but less zoned
- Primary traversal feedback through main rumble
- Trigger resistance where supported

### 14.3 PC

**Keyboard movement:**
- WASD for directional control
- Shift for sprint (hold or toggle)
- Space for jump
- Ctrl for crouch/slow walk
- Full remappable

**Mouse look:**
- Precise aiming for grapple hook
- Camera control during traversal

---

## 15. ACCESSIBILITY CONSIDERATIONS

### 15.1 Motor Accessibility

**Auto-traversal options:**
- Auto-climb (when climbable surface approached)
- Auto-jump (when gap detected)
- Hold-to-sprint instead of toggle (for motor limitations)
- One-button climbing (hold, not hand-over-hand)

**Simplified controls:**
- Combined sprint+jump for dramatic moments
- Auto-swim when in water (no continuous input)

### 15.2 Visual Accessibility

**Traversal indicators:**
- Climbable surfaces can be highlighted brighter (option)
- Gap distances shown with visual markers (option)
- Water depth indicators (when entering)
- Grapple points explicitly marked

### 15.3 Cognitive Accessibility

**Clear feedback:**
- Stamina depletion warnings (audio/visual)
- Fall height indicators
- Breath meter prominent when diving

### 15.4 Difficulty Scaling

**Traversal difficulty options:**
- Standard (baseline)
- Forgiving (extended stamina for traversal, reduced fall damage)
- Challenging (harsher stamina, unforgiving falls)

---

## 16. TRAVERSAL TUTORIAL PROGRESSION

### 16.1 Chapter 1 (Moran)
- Basic walking, running, sprinting
- First jumps
- Simple climbing (ladders)
- Introduction to stamina

### 16.2 Chapter 2 (Jungle)
- Sprint in combat
- Jumping in combat context
- First natural cliff climbs
- Stamina management in exploration

### 16.3 Chapter 3 (Moran Coast)
- First swimming sequences
- Breath meter introduction
- Dangerous falls

### 16.4 Chapter 4 (Rexa)
- Grappling hook unlock
- Ship rigging climbing
- Urban verticality

### 16.5 Chapter 6 (Ashenmoor)
- Deep climbing (long vertical)
- Ice terrain (first snow)
- Swimming in difficult currents

### 16.6 Chapter 7 (Quiet Coast)
- Horse riding
- Ice skating
- Cold water survival
- Storm traversal

### 16.7 Chapter 8 (Fort Carrion)
- Fort assault verticality
- Rope swinging
- Complex multi-level combat traversal

### 16.8 Chapter 10+ (Post-game Region 06)
- Underwater depth exploration
- Long-breath swimming
- Unique traversal mechanics (spoiler content)

---

## 17. THE TRAVERSAL FEEL TEST

Before traversal is final, it must pass these tests:

### 17.1 The RDR2 Test
> *"Does moving through the world feel NATURAL? Does Jake have weight, momentum, personality in his movement?"*

### 17.2 The Bloodborne Test
> *"Is movement committed enough that it matters? Can the player feel when they've overcommitted versus played safe?"*

### 17.3 The Breath of the Wild Test
> *"Does exploration feel rewarded? Are climbable surfaces satisfying to reach? Is vertical gameplay meaningful?"*

### 17.4 The Assassin's Creed Test (INVERSE)
> *"Is movement DELIBERATELY limited? Can the player tell this is NOT parkour?"*

### 17.5 The Swimming Test
> *"Does water feel like a real dimension? Is Jake's breath meter tense but fair? Are underwater environments worth exploring?"*

All five must pass. Then traversal is ready.

---

## 18. IMPLEMENTATION NOTES

### 18.1 Animation Requirements
- Walking animation (variations for surface type)
- Running animation (combat vs non-combat)
- Sprinting animation (exhausted state separate)
- Climbing animation (cliff, rope, ladder separate)
- Swimming animation (surface, underwater, distressed)
- Jumping animation (standing, running, sprint)
- Landing animation (roll, heavy, splash)
- Falling animation (controlled, uncontrolled, dramatic)

### 18.2 Physics Requirements
- Momentum carrying through direction changes
- Realistic gravity (9.81 m/s²)
- Water buoyancy and resistance
- Wind effects (storm weather)
- Surface friction variables

### 18.3 AI Requirements
- Enemies must have different pathing than Jake
- NPCs must walk naturally (varied speeds)
- Horses must have independent AI
- Sea creatures must have three-dimensional AI

### 18.4 Level Design Requirements
- Climbable surfaces must be clearly marked
- Gap distances must be calculatable
- Water depths must be rendered accurately
- Escape routes must exist (no dead-ends)

---

## 19. CONSISTENCY WITH EXISTING DESIGN

All existing documentation remains compatible:

- **Boss Arenas:** Traversal systems work within specified boss arenas
- **Regions:** Each region's terrain uses this traversal spec
- **Naval Combat:** Ship-specific traversal (rigging, deck, boarding) spelled out
- **Story:** Jake's evolution (Chapter 4 grappling hook, Chapter 7 horse riding) matches progression

No existing documents need revision. Traversal supports all previously-designed content.

---

## 20. NEXT STEPS

Next document: **`mechanics/stealth.md`** — Stealth System

Will cover:
- Crouch and prone mechanics
- Silent movement patterns
- Enemy detection (visual, audio, scent)
- Stealth takedowns
- Distraction items
- AI awareness states (idle, suspicious, alerted, searching, combat)
- Stealth reset mechanics
- Stealth kill animations per weapon class
- Disguises and infiltration
- The tension between stealth and combat readiness

Estimated time: 3-4 hours.

After Stealth: Weapon Wheel System (RDR2-inspired radial).

**Traversal is how Jake interacts with the world at every moment between combat. This document defines every footstep, every climb, every dive, every jump. Phase 3 Subsystem 3 of 10 — complete.**
