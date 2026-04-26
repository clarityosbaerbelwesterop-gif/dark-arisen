# WIND SYSTEMS — THE INVISIBLE FORCE

**Status:** Design Draft v1.0  
**Phase:** Phase 6 — Physics (Subsystem 2 of 15 — Foundation Tier)  
**Tier:** Foundation Tier  
**Related:** physics_overview.md, physics/water_physics.md, naval_combat_system.md, mechanics/combat_feel.md, style_bible.md  
**Purpose:** Define the complete wind physics system — direction and strength, effects on sails, fire, sound, dust, hair, cloth, projectiles. The invisible force that touches everything in Dark Arisen.  
**Inspirations:** Sea of Thieves (wind affects sailing), Assassin's Creed Black Flag (sail dynamics), Red Dead Redemption 2 (wind atmospheric), Ghost of Tsushima (wind as visual storyteller), real-world meteorology and naval physics

---

## 1. WIND PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Wind Is Always Present

There is no truly windless moment in Dark Arisen. The air always moves. Sometimes a whisper, sometimes a scream. Wind is the constant character — invisible but always felt.

**Design intent:** Atmospheric reality through constant subtle wind. Players never feel a "frozen" world.

### 1.2 Principle 2: Wind Touches Everything

Wind affects sails, fire, sound, smoke, dust, leaves, hair, clothing, snow, rain, bird flight, arrow trajectories, voice carry, and the player's perception of distance. No system is unaffected.

**Design intent:** Universal force creates emergent gameplay through systemic interaction.

### 1.3 Principle 3: Wind Has Direction and Layer

Surface wind isn't always upper-air wind. Storm winds rotate. Local effects differ from regional patterns. Wind is three-dimensional and complex.

**Design intent:** Realistic complexity rewards skilled players who learn to read wind.

### 1.4 Principle 4: Reading Wind Is Skill

Experienced sailors read wind through visible cues. Flags. Smoke. Bird flight. Wave patterns. Hair on the neck. Players develop this skill, becoming better captains as they learn.

**Design intent:** Mastery emerges from observation. Not all wisdom comes from menus.

### 1.5 Principle 5: Wind Carries Story

A still day says calm. A building wind says action. A scream of storm says crisis. A mournful wind through ruins says past. Wind communicates emotion before any words are spoken.

**Design intent:** Wind as silent narrator, always speaking.

---

## 2. WIND DIRECTION

### 2.1 The Compass of Wind

**Standard Directions:**
- N (North)
- NE (Northeast)
- E (East)
- SE (Southeast)
- S (South)
- SW (Southwest)
- W (West)
- NW (Northwest)

**Plus Intermediate:**
- NNE, ENE, ESE, etc.
- 16-point compass total
- Realistic precision

### 2.2 Wind Direction Visual Indicators

**How Players Read Direction:**

**Flags:**
- Most obvious indicator
- Always present where appropriate
- Direction clear
- Strength visible too

**Smoke:**
- Chimney smoke
- Cooking fires
- Battle smoke
- Direction crystal clear
- Distance affects clarity

**Sails:**
- Filling indicates wind direction
- Slack sails = no wind
- Snapping sails = changing wind
- Strained sails = strong wind

**Water Surface:**
- Cat's paws (wind ripples on water)
- Direction visible
- Strength estimable
- Sailor's standard sign

**Birds:**
- Birds face into wind when landing
- Flight patterns affected
- Migration aided by wind
- Specific cues

**Hair and Cloth:**
- Player's own hair flows
- NPC clothing waves
- Direction obvious
- Constant feedback

**Dust and Leaves:**
- Direction of drift
- Subtle but visible
- Atmospheric storytelling
- Realistic

**Trees:**
- Branches lean
- Leaves rustle
- Sound from upwind
- Forest clues

### 2.3 Wind Shifts

**Realistic Changes:**
- Gradual shifts (most common)
- Sudden shifts (storm conditions)
- Predictable patterns
- Surprise moments

**Visible Warning:**
- Approaching squall line
- Cloud movement
- Bird behavior changes
- Experienced players read

### 2.4 Local Wind Effects

**Specific Areas Have Specific Patterns:**

**Coastal Wind:**
- Sea breeze (day, off ocean)
- Land breeze (night, off land)
- Predictable patterns
- Strategic knowledge

**Mountain Wind:**
- Up-slope (warming day)
- Down-slope (cooling night)
- Funnel effects in valleys
- Specific dangers

**Storm Center:**
- Rotating winds
- Eye of storm (calm)
- Walls of wind (extreme)
- Realistic complexity

**Building Wakes:**
- Wind disturbed by buildings
- Specific calm pockets
- Tactical implications
- Urban realism

### 2.5 Direction Matters Strategically

**For Sailing:**
- See `mechanics/ship_navigation.md` Section 2.4
- Direction relative to ship critical
- Tacking when into wind
- Strategic depth

**For Combat:**
- Sound carries with wind
- Smoke screens directional
- Fire spreads downwind
- Tactical knowledge

**For Stealth:**
- Smell carries with wind
- Sound carries with wind
- Approaches from upwind detected
- Realistic mechanics

---

## 3. WIND STRENGTH

### 3.1 The Beaufort Scale (Adapted)

**Scale 0: Calm (0-1 mph)**
- No wind perceptible
- Smoke rises straight
- Sails hang slack
- Visual: still
- Naval: cannot sail
- Sound: unnaturally quiet

**Scale 1: Light Air (1-3 mph)**
- Smoke drifts
- Sails barely fill
- Hair just visible movement
- Naval: drifting only
- Mood: peaceful

**Scale 2: Light Breeze (4-7 mph)**
- Leaves rustle
- Flags ripple
- Wind on face felt
- Naval: slow but moving
- Mood: pleasant

**Scale 3: Gentle Breeze (8-12 mph)**
- Leaves and twigs in motion
- Flags extend
- Light flag fully out
- Naval: comfortable sailing
- Mood: active

**Scale 4: Moderate Breeze (13-18 mph)**
- Small branches move
- Dust raised
- Flag flutters
- Naval: good sailing
- Mood: vigorous

**Scale 5: Fresh Breeze (19-24 mph)**
- Small trees sway
- Whitecaps on water
- Flag fully extended hard
- Naval: exciting sailing
- Mood: tense alertness

**Scale 6: Strong Breeze (25-31 mph)**
- Large branches move
- Wind whistles
- Difficult to use umbrella
- Naval: challenging sailing
- Mood: concern

**Scale 7: High Wind (32-38 mph)**
- Whole trees in motion
- Walking against difficult
- Hat-loss range
- Naval: difficult sailing
- Mood: serious

**Scale 8: Gale (39-46 mph)**
- Twigs break off
- Walking very difficult
- Damage possible
- Naval: dangerous
- Mood: alarm

**Scale 9: Strong Gale (47-54 mph)**
- Slight structural damage
- Roof tiles loose
- Trees break
- Naval: dangerous, reef sails
- Mood: crisis

**Scale 10: Storm (55-63 mph)**
- Trees uproot
- Significant damage
- Walking impossible
- Naval: emergency
- Mood: survival

**Scale 11: Violent Storm (64-72 mph)**
- Widespread damage
- Trees down everywhere
- Naval: barely surviving
- Mood: terror

**Scale 12: Hurricane (73+ mph)**
- Devastation
- Major destruction
- Naval: dying
- Mood: end-of-world

### 3.2 Strength Visual Indicators

**Players Read Strength Through:**

**Flag Behavior:**
- Light: Drooping
- Moderate: Extended
- Strong: Whipping
- Storm: Tearing

**Tree Movement:**
- Light: Leaves only
- Moderate: Small branches
- Strong: Major branches
- Storm: Whole trees

**Water Surface:**
- Calm: Glassy
- Light: Ripples
- Moderate: Wavelets
- Strong: Whitecaps
- Storm: Spray

**Smoke Behavior:**
- Calm: Rises straight
- Light: Slight drift
- Moderate: Bent significantly
- Strong: Horizontal
- Storm: Torn apart

**Player Body:**
- Light: Hair moves
- Moderate: Hair flowing
- Strong: Lean into wind
- Storm: Difficult to walk

### 3.3 Strength Audio Indicators

**Sound at Different Strengths:**

**Calm:**
- Distant sounds clear
- Quiet
- Almost too quiet

**Light to Moderate:**
- Pleasant rustle
- Sail tension subtle
- Ambient atmospheric

**Strong:**
- Whistling through rigging
- Sail strain audible
- Distant howls
- Tension building

**Storm:**
- Roaring overwhelming
- Other sounds drowned
- Voices lost in wind
- Crisis audio

### 3.4 Strength Affects Gameplay

**Combat:**
- Long-range shots harder
- Sound easier/harder to hear
- Movement affected
- Realistic complications

**Movement:**
- Walking slowed in strong wind
- Climbing dangerous in gusts
- Falling more dangerous
- Real consequences

**Resource Management:**
- Strong wind = harder fishing
- Strong wind = no fire-starting
- Strong wind = no flying birds (some hunts blocked)
- Adaptive gameplay

---

## 4. WIND AFFECTS SAILS — THE NAVAL CONNECTION

### 4.1 Cross-Reference

**Detailed in:** `mechanics/ship_navigation.md` Section 2.4

**Key Points:**
- Direction relative to ship determines effectiveness
- Sailing into wind = impossible, must tack
- Crosswind = moderate speed
- Following wind = maximum speed

### 4.2 Sail Filling Physics

**Realistic Behavior:**

**Empty Sail:**
- Hangs slack
- Visible cloth
- Useless

**Filling Sail:**
- Snaps and pops
- Tension building
- Visible inflation

**Full Sail:**
- Curved shape
- Maximum power
- Beautiful

**Over-Stressed Sail:**
- Strain visible
- Damage possible
- Tearing risk

### 4.3 Sail Configuration Affected by Wind

**Light Wind:**
- Maximum sail deployed
- All available canvas
- Capture every breath

**Moderate Wind:**
- Standard sail configuration
- Comfortable cruise
- Maximum efficiency

**Strong Wind:**
- Reefed sails (less canvas)
- Reduced strain
- Still effective

**Storm:**
- Storm sails only
- Minimum canvas
- Survival mode
- Specific specialized sails

### 4.4 Damage From Wind

**Sail Damage:**
- Over-stressed in storms
- Tearing possible
- Repair needed
- Cost real

**Mast Damage:**
- Extreme winds
- Catastrophic possible
- Major repair
- Long-term setback

**Rigging Damage:**
- Lines snap
- Harder to repair underway
- Crew danger
- Realistic concern

---

## 5. WIND AFFECTS FIRE

### 5.1 Fire Spread

**Direction:**
- Fire spreads downwind
- Source visible upwind
- Predictable pattern
- Strategic implications

**Speed:**
- Calm: Slow spread (0.5m/min)
- Light: Slow (1m/min)
- Moderate: Moderate (3m/min)
- Strong: Fast (10m/min)
- Storm: Explosive (30m+/min)

### 5.2 Fire Intensity

**Wind Increases Fire:**
- More oxygen
- Hotter burn
- Bigger flames
- Faster consumption

**Sometimes Wind Kills Fire:**
- Sudden gust extinguishes small fires
- Wind direction changes
- Realistic complexity

### 5.3 Strategic Fire Use

**Player Tactics:**

**Defending:**
- Set fires upwind of enemies
- Smoke screens directional
- Wind direction chosen battles
- Tactical depth

**Attacking:**
- Fire arrows downwind
- Burning ships from upwind
- Coordinated wind attacks
- Strategic combat

**Survival:**
- Build fires considering wind
- Smoke direction matters (concealment)
- Camp in lee of wind
- Practical knowledge

### 5.4 Wildfire Potential

**Major Fire Events:**
- Forest fires in dry conditions
- Massive scale
- Cannot be stopped easily
- Long-term consequences

**Player Triggers:**
- Carelessness
- Intentional sabotage
- Lightning (random)
- Story events

---

## 6. WIND AFFECTS SOUND

### 6.1 Sound Travel

**With Wind:**
- Sound carries farther downwind
- Voices audible at greater distance
- Hearing extended
- Realistic

**Against Wind:**
- Sound attenuated
- Voices lost
- Hearing reduced
- Tactical implications

**Crosswind:**
- Sound deflected
- Direction less clear
- Confusing audio
- Realistic

### 6.2 Wind Itself Audible

**Sounds Wind Makes:**

**Calm Whisper:**
- Faint movement of air
- Almost imperceptible
- Atmospheric

**Light Rustle:**
- Leaves moving
- Cloth moving
- Pleasant ambient

**Moderate Whoosh:**
- Wind through trees
- Atmospheric
- Constant background

**Strong Roar:**
- Wind through rigging
- Whistling
- Demanding attention

**Storm Howl:**
- Overwhelming
- Drowns other sounds
- Crisis audio
- Memorable

**Wind in Specific Spaces:**

**Through Rigging:**
- Whistling, humming
- Sailor's familiar sound
- Atmospheric ship

**Through Trees:**
- Rustle to roar
- Forest atmosphere
- Specific feel

**Around Buildings:**
- Whistling around corners
- Specific architecture sound
- Urban realism

**Through Caves/Ruins:**
- Echo and moan
- Atmospheric
- Sometimes spooky

**Across Open Water:**
- Constant susurrus
- Maritime feel
- Always present

### 6.3 Stealth Implications

**Sound Travel Matters:**
- Approach from downwind = enemies hear
- Approach from upwind = enemies don't hear
- Wind direction critical for stealth
- Skilled players learn

**Footsteps:**
- Wind covers footstep sound
- Strong wind = stealth easier
- Calm = stealth harder
- Realistic balance

### 6.4 Voice Communication

**Crew Calls Affected:**
- Strong wind = shouting required
- Storm = communication difficult
- Realistic challenge
- Memorable scenes

**Long-Distance Voice:**
- Calm: 50m+
- Light: 30m
- Moderate: 20m
- Strong: 10m
- Storm: shouting only, very limited

---

## 7. WIND AFFECTS PROJECTILES

### 7.1 Arrow Physics

**Wind Effect on Arrows:**

**Direction:**
- Crosswind pushes arrow
- Headwind reduces range
- Tailwind extends range
- Realistic ballistics

**Strength Affects Trajectory:**
- Light wind: minimal effect
- Moderate: noticeable at range
- Strong: significant
- Storm: archery near impossible

### 7.2 Wind Reading for Archery

**Skill Development:**

**Beginner:**
- No compensation
- Misses common
- Learning visible

**Intermediate:**
- Subtle adjustments
- Better hits
- Wind awareness

**Master:**
- Automatic compensation
- Reads instantly
- Mira can teach
- Long shots reliable

**Expert:**
- Uses wind tactically
- Compensates perfectly
- Mastery visible
- Few can match

### 7.3 Bullet Physics

**Less Affected Than Arrows:**
- Higher velocity
- Less surface area
- Still measurable
- Long range matters

### 7.4 Cannon Ball Physics

**Heavy Projectiles:**
- Wind effect minimal
- But not zero
- Long-range cannons affected
- Strategic depth

### 7.5 Throwing Weapons

**Most Affected:**
- Knives drift significantly
- Hatchets unpredictable
- Specific weapons better
- Wind compensation skill

---

## 8. WIND AFFECTS DUST AND DEBRIS

### 8.1 Dust Patterns

**Visible Dust:**
- Following wind direction
- Strength visible by amount
- Atmospheric realism
- Beautiful effects

### 8.2 Specific Environments

**Ashenmoor:**
- Sand always moving
- Dust devils form
- Sandstorms possible
- Visibility affected

**Dry Roads:**
- Dust raised by walkers
- Followed by wind
- Path visible
- Tracking aid

**Construction:**
- Dust from building work
- Stone dust drifts
- Realistic settings

### 8.3 Snow Drift

**Fjordlund:**
- Wind moves snow
- Drifts form
- Patterns realistic
- Atmospheric beauty

**Visibility:**
- Blowing snow blinding
- Whiteout conditions
- Specific gameplay
- Real danger

### 8.4 Leaves and Litter

**Atmospheric:**
- Leaves drift downwind
- Specific seasons
- Beautiful detail
- Subtle realism

### 8.5 Storm Debris

**Major Wind:**
- Branches blown
- Leaves torn
- Debris everywhere
- Damage visible

---

## 9. WIND AFFECTS CLOTH AND HAIR

### 9.1 Player Hair

**Physics-Based:**
- Length matters
- Strength visible
- Direction obvious
- Aesthetic beauty

**Specific Implementation:**
- Hair flows with wind
- Tied hair more controlled
- Loose hair dramatic
- Visual richness

### 9.2 Player Clothing

**Cloth Physics:**

**Light Cloth:**
- Flows dramatically
- Beautiful in wind
- Realistic motion
- Atmospheric

**Heavy Cloth:**
- Less affected
- Solid feel
- Practical
- Realistic

**Capes/Cloaks:**
- Maximum dramatic
- Wind shows direction
- Beautiful gameplay
- Iconic moments

### 9.3 NPC Clothing

**Same Physics:**
- All NPCs affected
- Realistic ambient
- Atmospheric immersion
- World feels alive

### 9.4 Sail Cloth

**Detailed in Section 4**

### 9.5 Tents and Awnings

**Camping Mechanics:**
- Tents flap in wind
- Strong wind damages
- Realistic concerns
- Practical gameplay

---

## 10. WIND AFFECTS RAIN AND SNOW

### 10.1 Driven Rain

**Wind Effects:**
- Rain at angle
- Slanted patterns
- Coverage uneven
- Realistic complexity

### 10.2 Driven Snow

**Fjordlund Specific:**
- Snow at angle
- Drifts form
- Whiteout possible
- Atmospheric cold

### 10.3 Combined Storm

**Wind + Rain + Cold:**
- Brutal conditions
- Hypothermia possible
- Crisis gameplay
- Memorable scenes

---

## 11. SPECIFIC REGIONAL WIND PATTERNS

### 11.1 Caribbean (Rexa/Moran)

**Trade Winds:**
- Generally easterly
- Reliable patterns
- Sailor's friend
- Strategic knowledge

**Hurricane Season:**
- Specific months
- Storms build
- Major weather
- Cultural awareness

**Land/Sea Breeze:**
- Diurnal pattern
- Predictable
- Strategic for sailing
- Realistic detail

### 11.2 Northern Atlantic (Fjordlund)

**Westerlies:**
- Generally west
- Strong typically
- Cold biting
- Demanding

**North Sea Storms:**
- Frequent
- Severe
- Test of seamanship
- Brutal beauty

**Polar Vortex:**
- Specific extreme conditions
- Survival mode
- Memorable scenes

### 11.3 Imperial Waters

**Mediterranean-Style:**
- Variable
- Land effects
- Predictable patterns
- Familiar feel

### 11.4 Quiet Coast

**Standard Temperate:**
- Frontal systems
- Variable but predictable
- Familiar to American players
- Comfortable

### 11.5 Open Ocean

**Pure Patterns:**
- Stronger winds
- Long fetches
- Big waves (with water physics)
- Real test

### 11.6 Region 06

**Impossible Winds:**
- Patterns don't follow rules
- Sometimes from no direction
- Sometimes from all
- Symbolic atmosphere

---

## 12. WIND AND SEAMANSHIP

### 12.1 Esteban's Wisdom

**Master Navigator's Knowledge:**
- Knows regional patterns
- Reads wind 5 minutes before changes
- Teaches Mira (and Jake)
- Cultural treasure

**Specific Teaching:**
- Wind names per culture
- Seasonal patterns
- Reading clouds for wind
- Birds for wind

### 12.2 Mira's Skill

**First Mate:**
- Daily wind reading
- Steering optimized
- Constant adjustment
- Skilled

### 12.3 Jake's Learning

**Player Progression:**

**Chapter 1-3:**
- Limited awareness
- Just sailing
- Wind seems random

**Chapter 4-6:**
- Learning patterns
- Esteban teaches
- Better captains

**Chapter 7-9:**
- Skilled reading
- Strategic use
- Master sailor

**Chapter 10:**
- Full mastery
- Wind feels intuitive
- Captain's earned

---

## 13. WIND AND COMBAT TACTICS

### 13.1 Approach Strategies

**Upwind Approach:**
- Stealth easier (sound, smell)
- Standard tactic
- Strategic advantage

**Downwind Approach:**
- Scent betrays
- Sound carries
- Detected

### 13.2 Naval Combat

**Wind Gauge:**
- Position for wind advantage
- Critical in ship-to-ship
- Skilled captains seek
- Loss = vulnerability

### 13.3 Ground Combat

**Wind Considerations:**
- Long range archery
- Smoke direction
- Fire spread
- Sound travel

**Tactical Depth:**
- Skilled players use wind
- Becomes second nature
- Real strategic layer

---

## 14. WIND AS NARRATIVE

### 14.1 Atmospheric Storytelling

**Wind Mood:**

**Gentle Wind:**
- Peace
- Reflection
- Calm moment
- Healing

**Building Wind:**
- Tension
- Approaching action
- Story heating

**Howling Wind:**
- Crisis
- Darkness
- Memorable

**Sudden Calm:**
- Eerie
- Foreboding
- Surprise coming

### 14.2 Specific Scenes

**Wind-Marked Moments:**

**Ethan's Grove Discovery:**
- Specific wind
- Atmospheric
- Memorable

**Final Boss Approach:**
- Wind builds
- Tension grows
- Cinematic

**Quiet Moments:**
- Wind subtle
- Reflection time
- Beautiful

### 14.3 Cultural Wind Beliefs

**Sailor Superstitions:**
- Whistling brings wind
- Specific names
- Cultural respect

**Mama Jacinta's Wind:**
- Spiritual significance
- Wind speaks
- Cultural depth

**Father Salvio:**
- "The Holy Spirit moves like wind"
- Religious wind
- Symbolic importance

---

## 15. PLATFORM IMPLEMENTATION

### 15.1 PS5 DualSense

**Haptic Wind:**
- Subtle continuous feedback
- Strength matches wind
- Direction sometimes felt
- Immersive

**Adaptive Triggers:**
- Sail handling tension
- Rope strain felt
- Realistic tension

**Speaker:**
- Wind sounds spatial
- Direction cues
- Atmospheric

### 15.2 Xbox

**Rumble:**
- Wind feedback
- Less precise but present
- Atmospheric

### 15.3 PC

**Standard:**
- Audio important
- Visual cues
- Standard implementation

---

## 16. ACCESSIBILITY

### 16.1 Visual Wind Indicators

**Enhanced Options:**
- Direction arrows
- Strength meters
- Clear UI option
- Player choice

### 16.2 Audio Description

**For Hearing Impaired:**
- Visual wind effects emphasized
- Direction indicators
- Strength visual

### 16.3 Motion Considerations

**Reduced Effects:**
- Camera shake reducible
- Character motion dampening
- Motion sickness friendly

---

## 17. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** Sailors describe wind in maritime Low ("brisk wind today"). Aristocrats poetically High ("the breeze caresses"). Indigenous spiritual Middle.
- ✅ **Pillar 2 (Lore Through Objects):** Flags, weather vanes, wind chimes — all reveal wind history. Specific items wind-related tell stories.
- ✅ **Pillar 3 (Layered Death):** Wind death subtle. Mournful wind through ruins (Style D). Storm death (Style B). Calm wind sigh (Style A).
- ✅ **Pillar 4 (Layered Humor):** Style C quirks — Big Tom losing hat, crew comedy in wind, real reactions.
- ✅ **Pillar 5 (Theme — Revenge):** Wind reflects mood. Calm wind for peaceful path. Storm wind for vengeance. Atmospheric mirror.

---

## 18. NEXT STEPS

Next document: **`physics/sun_shadow_systems.md`** — Foundation Tier Document 3

Will cover:
- Day-night cycle
- Sun position dynamic
- Shadow casting realistic
- Moonlight at night
- Eclipse rare events
- Time-based atmosphere

---

## 19. CONCLUSION

Wind in Dark Arisen is the invisible character that touches everything. The breath of the world. Sometimes whisper, sometimes scream. Always present, always speaking.

When players feel the wind shift before a storm, when they read a flag and know they should reef sails, when they shoot an arrow and compensate for crosswind without thinking, when they hear a voice carry across distance because the wind allows it — that's when wind becomes more than mechanic. That's when it becomes experience.

This is the second pillar of Dark Arisen's physics. The unseen force behind so much of what makes the world believable.

**Phase 6 Subsystem 2 of 15 — complete.**

**The wind blows. Sails fill. Hair flies. The world breathes.** 💨🏴‍☠️
