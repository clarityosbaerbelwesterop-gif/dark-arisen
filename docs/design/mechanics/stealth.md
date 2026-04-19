# STEALTH SYSTEM

**Status:** Design Draft v1.0  
**Phase:** Phase 3 — Mechanics (Subsystem 4 of 10)  
**Tier:** Integrated Tier — Depends on Combat Feel AND Traversal  
**Related:** mechanics_overview.md, mechanics/controls.md, mechanics/combat_feel.md, mechanics/traversal.md, style_bible.md  
**Purpose:** Define all stealth, detection, and covert-action mechanics in Dark Arisen. How Jake sneaks, assassinates, hides, and manipulates enemy awareness.  
**Inspirations:** Sekiro (earned stealth kills), Ghost of Tsushima (natural stealth flow), Red Dead Redemption 2 (realistic detection + scent), Metal Gear Solid V (AI awareness states), Assassin's Creed Origins (investigation + blend), Dishonored (environmental stealth)

---

## 1. STEALTH PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Stealth Is Legitimate, Not Shortcut

In many games, stealth is a "for completionists" option — the game can be beaten without it. In Dark Arisen, stealth is a fully-supported legitimate combat style. Some encounters are specifically designed around stealth (Fort Carrion infiltration, El Médico's Underdistrict approach, certain boss pre-fights). Other encounters allow stealth as a genuine alternative to combat.

**Design intent:** A player who masters stealth should be able to complete the game differently than a player who masters combat. Both paths are valid.

### 1.2 Principle 2: Detection Is Realistic

Enemies see, hear, and smell Jake. Each sense has specific mechanics:

- **Vision cones** (not 360° awareness — realistic FOV)
- **Audio detection** (distance-based, muffled by environment)
- **Scent detection** (blood, sweat, recent combat — triggers specific creatures)
- **Environmental awareness** (footprints, disturbed objects, missing guards)

If Jake is seen by realistic human perception, he is seen. No magical "awareness auras."

### 1.3 Principle 3: Consequences Persist

Stealth in Dark Arisen has lasting consequences:

- **Kill a patrol guard:** Other guards will notice his absence on next patrol rotation (5-15 minutes real-time)
- **Kill a noble:** The household discovers within the hour, declares emergency
- **Steal from a household:** Items are missed, investigations begin
- **Leave bodies:** They are found. Witnesses react. Reputations change.

This creates depth. A single stealth kill is not just mechanical — it ripples through the world.

### 1.4 Principle 4: The Blood Trail

Jake gets bloody during combat. After significant kills or injury, he visibly carries blood on his clothing and weapons. This affects stealth in unique ways:

- **Scent attraction:** Predator animals detect blood up to 80m away
- **Visual alarm:** Guards notice bloody Jake at 2x normal detection range
- **Forensic evidence:** Dead bodies can be identified by blood type if guards investigate
- **Cleansing required:** Jake must wash/change clothes between stealth infiltrations

This is realistic, tense, and narratively consistent with a pirate setting.

### 1.5 Principle 5: Environmental Stealth

The world itself supports stealth. Tall grass hides Jake. Shadows obscure him. Rain masks sound. Storms hide visual presence. The environment is a tool, not just backdrop.

---

## 2. STEALTH MOVEMENT — THE CROUCH AND PRONE SYSTEM

### 2.1 Basic Stealth States

Jake has four movement stealth states:

**Standing (Normal):**
- Full speed and visibility
- Default state
- Most detectable

**Crouch:**
- 50% walking speed (1.75 m/s)
- 60% reduced visibility
- 70% reduced audio footprint
- Stamina cost: 0

**Prone:**
- 15% walking speed (0.5 m/s)
- 85% reduced visibility
- 90% reduced audio footprint
- Stamina cost: 0
- Cannot perform most actions

**Sprint Crouch (Rare, Advanced):**
- 75% sprint speed (7.5 m/s)
- -30% visibility reduction vs. crouch (more visible)
- Significant audio increase
- Stamina cost: 12/second
- Unlocked Chapter 5+

### 2.2 Transitions

**Entering Crouch (D-Pad Down from Standing):**
- Animation: 0.5 seconds
- Jake lowers smoothly
- No stamina cost

**Entering Prone (D-Pad Down from Crouch):**
- Animation: 0.8 seconds
- Jake lies flat
- Can roll to change direction (0.4 seconds)

**Exit (D-Pad Up):**
- From Prone: 1.0 second (slow to rise from ground)
- From Crouch: 0.5 second (standing up)
- Can be interrupted by emergency combat

### 2.3 Movement Sounds

Movement creates audio footprint detectable by enemies:

| Action | Audio Radius (audible to enemies) |
|--------|-----------------------------------|
| Walking | 8m |
| Running | 15m |
| Sprinting | 25m |
| Walking (crouch) | 3m |
| Running (crouch sprint) | 12m |
| Crouch stationary | 0m |
| Prone moving | 2m |
| Prone stationary | 0m |
| Jump | 6m |
| Climbing | 5m |
| Swimming (surface) | 8m |
| Swimming (underwater) | 2m |

Different surfaces modify audio:

| Surface | Audio Multiplier |
|---------|------------------|
| Stone | 1.2x |
| Wood (dry) | 1.0x |
| Wood (wet) | 0.8x |
| Sand | 0.6x |
| Grass | 0.7x |
| Carpet | 0.5x |
| Water (shallow) | 1.4x |
| Metal grating | 1.5x |
| Mud | 0.5x |
| Snow | 1.1x (crunch) |

---

## 3. ENEMY DETECTION — THE AWARENESS SYSTEM

### 3.1 Visual Detection

Enemies have realistic field of view (FOV):

**Standard Pirate Guard:**
- FOV: 120° horizontal
- Vision Range: 25m (full), 40m (silhouette only)
- Turn speed: 90°/second
- Attention span: Frequent head turns, realistic scanning

**Elite Guard / Officer:**
- FOV: 140° horizontal
- Vision Range: 35m (full), 55m (silhouette)
- Turn speed: 120°/second
- Attention span: Methodical patrol patterns

**Lookout / Sniper:**
- FOV: 90° (focused)
- Vision Range: 80m (with spyglass extension)
- Turn speed: 60°/second (methodical)
- Specific tower positions

**Animal (predator, wolf/bear):**
- FOV: 160° (hunting predator)
- Vision Range: 15m (visual), 60m (scent-enhanced)
- Turn speed: 180°/second
- Tracks via motion and scent

### 3.2 Vision Modifiers

Visibility changes with light, weather, and Jake's state:

| Condition | Detection Range Modifier |
|-----------|-------------------------|
| Full daylight | 100% |
| Overcast day | 85% |
| Dusk/dawn | 70% |
| Clear night (moonlight) | 50% |
| New moon (dark night) | 30% |
| Inside (torchlight) | 60-80% |
| Dense fog | 30% |
| Rain | 70% |
| Storm | 40% |
| Tall grass | -60% (for crouched Jake) |
| Behind cover | 0% (fully hidden) |

### 3.3 Audio Detection

Enemies hear Jake within specific ranges based on his actions. See Section 2.3 for audio radii.

**Audio modifiers:**
- Rain adds +40% background noise (harder to hear Jake)
- Storm adds +80% background noise
- Crowd/busy environment adds +60% noise
- Silent environment (temple) has -20% noise (Jake more audible)

### 3.4 Scent Detection (The Blood Trail System)

Unique mechanic — certain enemies track by smell.

**Scent Sources:**
- Blood on Jake's clothes (from combat or injury)
- Blood on weapons (recent kills)
- Unwashed sweat (after long sprinting or climbing)
- Specific substances (poison, alcohol, specific foods)

**Enemies Using Scent:**
- **Sharks (underwater):** Detect blood up to 80m, aggressive if Jake bleeding
- **Wolves (Ashenmoor/Quiet Coast):** Detect blood up to 60m, follow trails
- **Bears (Quiet Coast):** Detect up to 40m, highly territorial
- **Tracking Dogs (Imperial Guards, Fort Carrion):** Detect up to 30m, alert handlers
- **Specific Boss:** Jaw of the Mire detects scent through water

**Cleaning Options:**
- Swim in clean water: Removes blood (takes 20 seconds submersion)
- Wash basins in villages: Full cleanse in 30 seconds
- Change clothes: Instant cleanse (requires spare outfit)
- Rain: Slowly reduces blood scent (50% reduction after 3 minutes in rain)

### 3.5 Detection States

Enemies exist in five awareness states:

**State 1: Idle (Default)**
- Standard patrol or stationary
- Normal FOV and hearing
- Not actively hunting for Jake
- Icon: None (no indicator)

**State 2: Suspicious**
- Detected something — movement, sound, or missing object
- Increased scanning, focus on suspected area
- FOV focused in direction of stimulus
- Duration: 15 seconds without confirmation
- Icon: Yellow question mark above head

**State 3: Alerted**
- Confirmed something is wrong
- Calls out ("Who's there?", "Show yourself!")
- Moves to investigate specific location
- Nearby enemies become Suspicious
- Duration: 30 seconds of active searching
- Icon: Yellow exclamation point

**State 4: Hunting**
- Seen Jake clearly
- Pursuing actively
- Weapon drawn
- Alerts all nearby enemies
- Duration: 60 seconds of active pursuit
- Icon: Red exclamation point

**State 5: Combat**
- In active combat with Jake
- All combat systems engaged
- Full aggression
- Icon: Red skull

### 3.6 State Transitions

**Idle → Suspicious:**
- Triggered by: Sound in their range, seeing movement at edge of vision, finding footprints, seeing missing-but-expected guard
- They investigate but don't alert others yet

**Suspicious → Alerted:**
- Triggered by: Clear view of figure (not identified), sound confirmed as humanoid, finding blood or body
- They call out and move to investigate
- Nearby enemies become Suspicious

**Alerted → Hunting:**
- Triggered by: Seeing Jake clearly, finding a body, hearing combat sounds
- Weapon drawn, actively hunting
- All nearby enemies become Alerted

**Hunting → Combat:**
- Triggered by: Visual contact within combat range
- Attack begins

**Combat → Hunting → Alerted → Suspicious → Idle:**
- Returns through stages if Jake escapes
- Each de-escalation takes specific time (30-120 seconds per stage)
- Full reset requires extended time out of contact

---

## 4. STEALTH TAKEDOWNS

### 4.1 Takedown Types

Jake can perform stealth kills/neutralizations based on positioning:

**Silent Assassination (Dagger/Short Blade):**
- **Position:** Behind enemy, unnoticed
- **Input:** Triangle when positioned
- **Animation:** 1.5 seconds, unique per weapon
- **Damage:** Instant kill on standard enemies
- **Noise:** Minimal (the body falls audibly)
- **Stamina:** 20

**Loud Takedown (Heavy Weapon):**
- **Position:** Behind enemy, unnoticed
- **Input:** Triangle with heavy weapon equipped
- **Animation:** 2 seconds
- **Damage:** Heavy damage (doesn't always kill bosses or elites)
- **Noise:** Medium (attracts nearby attention)
- **Stamina:** 30

**Non-Lethal Takedown:**
- **Position:** Behind enemy, unnoticed
- **Input:** Triangle with fist + specific holdings
- **Animation:** 2 seconds
- **Result:** Enemy knocked unconscious for 5 minutes
- **Wakes up:** If disturbed or after duration
- **Use case:** When Jake wants minimal witnesses but not to kill

**Elevated Assassination (from above):**
- **Position:** On ledge/rafters above enemy
- **Input:** Triangle while looking down
- **Animation:** Jake drops, lands on enemy
- **Damage:** Instant kill + significant impact
- **Noise:** Medium
- **Effects:** Jake takes -50% fall damage if assassination successful

**Thrown Projectile Assassination:**
- **Position:** Any, with bow or throwing weapon
- **Requirement:** Headshot from distance
- **Damage:** Instant kill if headshot, injury otherwise
- **Noise:** Bow = minimal, firearm = LOUD (definitely detected)

**Hanging Takedown:**
- **Position:** Hanging from ledge below enemy's patrol route
- **Input:** Triangle when enemy approaches edge
- **Animation:** Jake pulls them over the edge
- **Result:** Fall damage to enemy (usually fatal if high enough)

**Stealth Grab:**
- **Position:** Directly behind enemy
- **Input:** Square (instead of Triangle)
- **Result:** Jake grabs and holds enemy, can interrogate (Triangle) or kill (R1) or release (Circle)

### 4.2 Takedown Interrogation

When Jake grabs an enemy alive, he can interrogate them:

**Interrogation Inputs:**
- **Triangle (quick):** Ask brief question, enemy gives limited info
- **Square (threaten):** Apply pressure, enemy reveals more (may scream — detection risk)
- **Circle (release):** Let them go (they stay in Alerted state, will call for help)
- **R1 (kill):** Quick kill

**Information Tiers:**
- Level 1 (Triangle): Current patrol routes of other guards
- Level 2 (Square): Specific named targets' locations, passwords
- Level 3 (Hold Square longer): Deep secrets, specific quest information

**Moral Weight:**
- Interrogation is recorded in Jake's moral tracking
- Prolonged/violent interrogations affect Chapter 10 epilogue
- Father Salvio's crew members may disapprove

### 4.3 Chain Takedowns

Advanced stealth — taking out multiple enemies in rapid sequence:

**Requirements:**
- First target unnoticed
- Second target also unnoticed (just of first kill)
- Specific distance between targets (usually 3-5m)

**Input:**
- First takedown normal (Triangle)
- Chain indicator appears briefly (green glow on nearby takedown target)
- Second takedown within 2 seconds

**Effects:**
- Visually impressive
- Stamina cost: First 20, second 25 (higher)
- Noise: Minimal if properly sequenced

### 4.4 Body Handling

After a takedown, Jake may need to hide the body:

**Drag Body (F/Interact button):**
- Movement speed: 40% normal
- Stamina cost: 8/second
- Can drag 3-5 meters before needing rest
- Appropriate cover spots: Bushes, water, dark corners

**Dump Body in Water:**
- If body dragged to water, sinks (takes evidence away)
- May attract sharks (blood in water = scent detection radius)

**Hide in Container:**
- Large barrels, chests, haystacks
- Specific interactive objects
- Body stays hidden until container opened

**Leave in Open:**
- Will be discovered by patrols
- Triggers progressive alert (nearby enemies become Suspicious → Alerted)

---

## 5. ENVIRONMENTAL STEALTH

### 5.1 Cover System

Dark Arisen has a natural cover system (no "sticky cover"):

**What Counts as Cover:**
- Solid walls (full cover)
- Tall grass (waist-high or above — cover if crouched)
- Shadows (dim light — 60% reduction in detection)
- Foliage (trees, bushes)
- Crates, barrels, stacks
- Corners (edge peek)

**Cover Benefits:**
- Vision blocked from enemy
- Audio reduced (environmental dampening)
- Jake can peek without exposing body
- Allows passive observation for 60+ seconds without enemy advancement

### 5.2 Tall Grass Hiding

Specific environmental mechanic:

**Requirements:**
- Jake must be crouched
- Grass must be waist-high (35cm+ above Jake's crouched height)

**Effects:**
- 85% visual concealment
- Audio standard (grass doesn't dampen)
- Enemies must be within 2m to confirm Jake's presence
- Combat from hiding possible (assassination ambush)

### 5.3 Shadow System

Darkness levels affect detection:

| Light Level | Detection Modifier |
|-------------|-------------------|
| Full daylight | 100% |
| Partial shadow | 85% |
| Dim area | 70% |
| Dark shadow | 50% |
| Pitch black | 30% |
| Behind cover in dark | 15% |

**Light Sources Jake Can Extinguish:**
- Torches (wall-mounted) — Interact to douse
- Campfires — Requires water/sand
- Lanterns — Interact to extinguish
- Chandeliers — Can be dropped with arrow shot

**Extinguishing light makes enemies Suspicious but not directly alert them.**

### 5.4 Water Stealth

Water provides unique stealth options:

**Swimming Silent:**
- Underwater: No audio footprint
- Surface: 8m audio radius (splashing)
- Dive preferred for infiltration

**Under-Dock Hiding:**
- Jake can hide beneath wooden docks
- Visual cover + water escape route
- Breath meter still applies

**Mud/Water Immersion:**
- Jake can immerse himself in mud/water for cover
- Visual 90% reduction
- Audio silent
- Must hold breath (if underwater)

### 5.5 Disguises (Chapter 5+ System)

In specific quests, Jake can wear enemy uniforms:

**Imperial Uniform (Fort Carrion approach):**
- Blends with imperial soldiers
- Guards don't detect on sight
- But passwords/codes required
- Blood on uniform = blown cover

**Pirate Uniform (Armada infiltration):**
- Blends with Crimson Armada
- Less strict identification
- Tattoo pattern may be asked for
- Inappropriate for non-Armada regions (suspicious)

**Priest Robes (Pale Isle Temple):**
- Blends with religious orders
- Silence expected (can't talk much)
- Ritual knowledge may be tested
- Short-term disguise usually

**Disguise Risks:**
- Specific NPCs recognize Jake despite disguise
- Disguise blown if Jake does suspicious things
- Fight ends disguise immediately
- Changing environment may require different disguise

---

## 6. STEALTH TOOLS AND ITEMS

### 6.1 Distraction Items

**Rocks/Small Stones:**
- Throw to make noise at distance
- Guards investigate noise source
- Low-cost distraction
- Infinite use (Jake can pick them up anywhere)

**Bottles:**
- Throw to break glass — loud noise
- Attracts multiple guards
- Broken bottle leaves evidence
- Limited availability

**Meat/Food (bait for animals):**
- Distracts dogs, wolves (temporarily)
- Can also attract predators into guard territories
- Creates chaos for infiltration

**Tobacco Smoke Bombs (Craftable):**
- Create smoke cloud (4m radius)
- Enemies lose sight for 8 seconds
- Perfect for escape
- Jake coughs if he walks through own smoke

### 6.2 Poison Tools

**Poison Tipped Arrows:**
- Silent kill if target dies to poison
- Longer kill time (enemy may alert before dying)
- Valuable for eliminating targets from cover

**Contact Poison (food/water):**
- Jake can poison enemy food supplies
- Affects multiple targets over time
- Investigation of deaths leads to discovery

**Sleep Poison:**
- Non-lethal, renders targets unconscious
- Good for non-combat missions
- Targets wake after 30 minutes

### 6.3 Stealth Gear

**Assassin's Hood (unlocked by completing certain stealth content):**
- +15% detection resistance
- +20% silent movement
- Iconic stealth look

**Soft-Soled Boots:**
- -30% audio footprint
- Slightly reduced combat effectiveness (poor traction)

**Dark-Colored Armor:**
- +10% visual concealment at night
- Standard stats otherwise

**The Silence Gloves (rare):**
- Prevent noise during climbing, lockpicking, interaction
- Found in specific post-game content

---

## 7. STEALTH AI DEEP DIVE

### 7.1 Patrol Patterns

Enemies have specific patrol behaviors:

**Standard Patrol:**
- Walk between 2-4 waypoints
- Pause at each waypoint (10-30 seconds)
- Sometimes check specific areas (look down cliff, examine cargo, etc.)
- Random variation prevents pure memorization

**Stationary Guard:**
- Stand at specific post
- Scan area with occasional turns
- Investigate if anything suspicious in their view
- Rotate with other guards (every 30-60 minutes game time)

**Mobile Officer:**
- Move through multiple areas
- Check on subordinates
- Less predictable but covering more ground
- May call for reinforcements if alerted

**Hunter (Active):**
- Specifically hunting Jake after alert
- Tracks blood, footprints, sounds
- Has dog companion in certain regions
- Very dangerous — do not engage

### 7.2 Investigation Behavior

When a guard becomes Suspicious or Alerted:

**Suspicious Investigation:**
- Guard moves toward stimulus source
- Looks around specifically
- Calls out vocally (some dialogue)
- Returns to patrol if nothing found (15 second timeout)

**Alerted Investigation:**
- Guard draws weapon
- Moves more actively to search
- Signals nearby guards (specific whistle/shout)
- More thorough search — checks behind objects, in shadows
- Duration: 30 seconds of active search

**Return to Normal:**
- If Jake stays hidden long enough, guards return to Idle
- De-escalation takes time (as noted in state transitions)
- Some guards remember "this area had something suspicious" for longer

### 7.3 Communication Between Enemies

**Verbal Communication:**
- Guards shout warnings ("Intruder!")
- Calls travel up to 30m
- Nearby guards respond within 2 seconds
- Officer commands relayed through voice

**Signal Flares:**
- Elite guards may fire signal flares
- Summons reinforcements from further areas
- Takes 30 seconds for reinforcements to arrive

**Horn/Bell:**
- Major forts have alarm bells/horns
- Wakens all guards in fortress
- Creates hunting pursuit state
- Can be silenced (takedown bell-ringer, destroy bell)

### 7.4 Guard Memory

Important — guards remember recent events:

**Short-term (2-5 minutes):**
- Missing guards are noted
- Sounds heard are remembered
- Investigations carry over between patrols

**Long-term (quest-length):**
- Bodies found change patrol routes
- Entire regions go on high alert after major incidents
- Reputation affects future encounters

### 7.5 Reinforcement System

If alarm is triggered at a major location:

**Minor Alarm (single guard's shout):**
- Nearby patrols (within 50m) become Alerted
- One or two additional guards arrive
- Resolvable if Jake hides quickly

**Major Alarm (multiple guards shouting):**
- All guards in facility become Alerted
- Patrols combine to search
- New guards rotate in from adjacent areas
- Takes 10-15 minutes real-time to fully de-escalate

**Fortress Alert (horn/bell triggered):**
- Entire fortress on combat footing
- Reinforcements arrive continuously
- Key exits locked down
- Must be escaped or endured

---

## 8. STEALTH IN COMBAT CONTEXT

### 8.1 Breaking Combat for Stealth

Jake can attempt to re-enter stealth during active combat:

**Requirements:**
- Break line of sight with all enemies
- Move at least 30m away
- Enter cover or environmental stealth
- Wait for all enemies to de-escalate (2-5 minutes)

**Difficulty:**
- Very hard once combat begins
- Becomes easier in large environments (fortresses, cities)
- Nearly impossible in open terrain or small rooms

### 8.2 Stealth Attacks Mid-Combat

Even in active combat, specific actions recreate stealth elements:

**Assassination Opener:**
- Sprint to unnoticed enemy
- Triangle on approach = sprint assassination (3.5x damage)
- Disengage quickly after

**Smoke Bomb Escape:**
- Deploy smoke in combat
- Disengage during 8-second blindness
- Reset to Alerted state (not Combat)

**Environmental Kills:**
- Push enemy off cliff
- Throw enemy overboard (ship combat)
- Lure enemies into traps/hazards

### 8.3 The Hybrid Approach

Many encounters reward mixing stealth with combat:

**Start Stealth, End Combat:**
- Remove 50% of enemies stealthily
- Engage remaining in combat
- More manageable combat situations

**Combat Interrupt:**
- Enter combat
- If outnumbered, retreat to stealth
- Stealthily eliminate enemies while they search
- Re-engage combat on Jake's terms

**Dragnet Avoidance:**
- After combat, if backup arrives, transition to stealth
- Hide while new enemies search
- Reset combat to Jake's advantage

---

## 9. SPECIFIC STEALTH ENCOUNTERS

### 9.1 Mandatory Stealth Sections

Several story missions require stealth:

**El Médico Approach (Chapter 4):**
- Navigate the Underdistrict quietly
- Avoid his "Sample Collectors"
- If caught: 8-minute timer starts, stakes high

**Fort Carrion Initial Infiltration (Chapter 8):**
- Penetrate outer defenses silently
- Alternative: brute force assault (much harder)
- Success opens interior paths

**Pale Isle Temple Access (Chapter 8):**
- Avoid lesser priests during Silvano approach
- Disguise option (Priest Robes)
- Stealth or bluff required

**Draven's Personal Quarters (Chapter 9, Option):**
- If attempted: alternative to final battle at helm
- Assassination would end story differently
- Very difficult, requires prior Rache

### 9.2 Stealth-Encouraged Sections

Many sections reward stealth but allow combat:

- Captain Rojas' camp (can eliminate Rojas stealthily)
- La Viuda Negra's estate (entry method matters)
- Admiral Fitzmueller's ship boarding (stealth approach possible)
- Scarred Twins' inner keep (stealth won't avoid fight, but changes setup)

### 9.3 Stealth Impossible Sections

Some sections have no stealth option:

- Jake vs. Draven (Chapter 9 — direct confrontation)
- Mythical boss fights (they sense Jake on arrival)
- Post-game Region 06 (Heart-Chamber — no stealth possible)
- Crew rescue missions (speed over stealth)

---

## 10. STEALTH PROGRESSION

### 10.1 Chapter-by-Chapter Introduction

**Chapter 1-2 (Moran):**
- Basic crouching tutorial
- Simple silent kills
- No disguises yet

**Chapter 3 (Moran deep):**
- First proper stealth missions
- Basic AI awareness introduced
- Chain takedowns taught

**Chapter 4 (Rexa):**
- Advanced stealth (disguises, distraction items)
- El Médico Underdistrict
- Scent mechanic introduced

**Chapter 6-7 (Ashenmoor/Quiet Coast):**
- Weather-based stealth
- Predator animal tracking
- Moral weight of stealth kills

**Chapter 8 (Fort Carrion):**
- Full stealth mastery expected
- Infiltration vs. assault choice
- Full reinforcement system active

**Chapter 9+ (Ethan/Draven):**
- Rache-enhanced stealth
- Final stealth tests
- Post-game stealth challenges

### 10.2 Skill Tree

Jake's stealth skills develop through:

**Tier 1 (Early Game):**
- Basic Crouch Mastery (reduces noise 10%)
- Silent Step (reduces noise 15% total)
- Assassination Technique (+25% assassination damage)

**Tier 2 (Mid Game):**
- Blood Resistance (reduces scent attraction 30%)
- Shadow Skill (+20% concealment in darkness)
- Chain Master (+20% chain takedown reliability)

**Tier 3 (Late Game):**
- Ghost Walking (Jake can walk normally at 90% crouch stealth)
- Terror (enemies lose composure when Jake kills others stealthily)
- Master of Shadows (Rache-enhanced stealth gives superior positioning)

---

## 11. STEALTH UI

### 11.1 HUD Elements for Stealth

**Detection Indicator:**
- Visible to enemy (red eye above head)
- Suspicious (yellow question mark)
- Alerted (yellow exclamation)
- Hunting (red exclamation)
- Combat (red skull)

**Stealth Status:**
- Bottom-left shows Jake's current state
- Icon: Standing / Crouched / Prone / In Cover
- Visibility meter (how visible Jake is right now)
- Audio meter (how loud Jake is right now)

**Enemy Status:**
- Small indicators above each visible enemy
- Detection state, facing direction
- Scent trail (if Jake is being tracked)

### 11.2 Minimap (Optional)

Some players enable mini-map for stealth:
- Shows enemy positions within 30m
- Shows detection cones
- Shows patrol paths
- Accessibility: Can be disabled for pure stealth experience

### 11.3 Color Coding

**Green:** Jake undetected, safe
**Yellow:** Suspicious — possible danger
**Orange:** Alerted — danger rising
**Red:** Hunting/Combat — engaged
**Blue:** Stealth takedown available
**Purple:** Interrogation available

---

## 12. PLATFORM-SPECIFIC STEALTH

### 12.1 PS5 DualSense Features

**Haptic feedback for stealth:**
- Subtle buzz when Jake is being observed
- Growing pulse as detection increases
- Sharp "alerted" pulse when spotted
- Heartbeat haptic during tense stealth moments

**Audio features:**
- 3D audio crucial for hearing guards at distance
- Controller speaker for subtle cues
- Built-in mic can detect player breathing in silent mode (optional)

### 12.2 Xbox and PC

**Similar but simplified:**
- Audio cues primary
- HUD indicators slightly more prominent
- Controller rumble for detection pulses

---

## 13. ACCESSIBILITY IN STEALTH

### 13.1 Motor Accessibility

**One-button takedowns:**
- Auto-trigger when in position
- Simplified input sequences
- No complex combinations required

**Auto-crouch:**
- Automatically enter crouch when approaching enemies
- Auto-exit when in combat

### 13.2 Visual Accessibility

**Clear indicators:**
- Detection meters prominent
- Enemy sight cones visible (optional toggle)
- High contrast for stealth states

**Color blind modes:**
- Detection indicators use shape variation
- Not solely color-dependent

### 13.3 Audio Accessibility

**Visual audio cues:**
- Sound direction indicators
- Footstep visualization
- Alarm indicators prominent

### 13.4 Difficulty Scaling

**Stealth Difficulty:**
- Forgiving: Guards slower to detect, longer investigation
- Standard: Balanced
- Hardcore: Faster detection, shorter patience, harder to escape

---

## 14. THE STEALTH FEEL TEST

Before stealth is final, it must pass these tests:

### 14.1 The Sekiro Test
> *"Is stealth earned? Does the player feel rewarded for patience and positioning, not just rewarded for cheesing AI?"*

### 14.2 The Realism Test
> *"Do enemies respond to realistic stimuli? Can the player predict their behavior through observation?"*

### 14.3 The Agency Test
> *"Does the player feel they can approach any encounter with stealth if they want to? Are enough options provided?"*

### 14.4 The Consequence Test
> *"Do stealth actions have lasting effects on the world? Do kills ripple through NPC behavior?"*

### 14.5 The Tension Test
> *"Does stealth feel tense? Does detection matter? Do escape scenarios feel genuinely dangerous?"*

All five must pass. Then stealth is ready.

---

## 15. IMPLEMENTATION NOTES

### 15.1 AI Requirements
- Realistic FOV cones (rendered internally)
- Audio propagation system (with environmental modifiers)
- Scent tracking system (new — blood, sweat)
- Patrol pathfinding with realistic scanning
- Inter-enemy communication system

### 15.2 Animation Requirements
- Crouch walk (multiple speed variants)
- Prone movement (slow, realistic crawl)
- Cover adhesion and peek
- Takedown animations per weapon class
- Chain takedown smooth transitions
- Detection animations (enemy reactions at each state)
- Investigation animations (looking around, checking corners)

### 15.3 Level Design Requirements
- Sufficient cover density in stealth sections
- Climbable alternative routes
- Multiple paths to objectives
- Hide spots for bodies (water, bushes, containers)
- Lighting contrast (light/dark areas)
- Vegetation for tall grass stealth

### 15.4 Audio Design Requirements
- Distinct footstep sounds per surface
- Directional audio for guard positions
- Alarm sounds (horns, bells, shouts)
- Environmental dampening (indoor vs. outdoor)
- 3D audio for multi-level environments

---

## 16. CONSISTENCY WITH EXISTING DESIGN

All existing documentation remains compatible:

- **Boss Encounters:** Stealth is valid approach where specified
- **Regional Design:** Each region's stealth mechanics work with terrain
- **Naval Combat:** Ship boarding has stealth elements
- **Story Progression:** Jake's skill growth matches stealth system maturation

No existing documents need revision. Stealth supports all previously-designed content.

---

## 17. NEXT STEPS

Next document: **`mechanics/weapon_wheel.md`** — Weapon Wheel System

Will cover:
- RDR2-inspired L1 hold radial
- Weapon slot system (primary, secondary, sidearm)
- Wheel UI layout
- Weapon switching during combat
- Time dilation during wheel
- Weapon bonds and favorites
- Ammunition management
- Context-specific weapons (ship cannons via wheel)
- Accessibility modifications

Estimated time: 2-3 hours.

After Weapon Wheel: Item Wheel (parallel system for consumables).

**Stealth connects combat and traversal into a coherent gameplay system. This document defines every detection mechanic, every takedown, every cover interaction. Phase 3 Subsystem 4 of 10 — complete.**
