# PHASE 6 — PHYSICS MASTER DOCUMENT

**Status:** Design Draft v1.0  
**Phase:** Phase 6 — Physics (Master Overview)  
**Related:** All Phase 1-5 documents, mechanics_overview.md, naval_combat_system.md, mechanics/combat_feel.md, mechanics/traversal.md, style_bible.md  
**Purpose:** Define the complete physics system of Dark Arisen — water, wind, sun/shadow, combat impact, ship dynamics, weather, regional environmental physics. The "feel" foundation that makes the world believable.  
**Inspirations:** Red Dead Redemption 2 (environmental physics, water, weather), Assassin's Creed Black Flag (naval physics, ocean systems), Sea of Thieves (multiplayer ship physics, water), The Legend of Zelda: Breath of the Wild (cohesive physics interactions), Death Stranding (movement physics in difficult terrain), real-world physics simulation in modern game engines

---

## 1. PHYSICS PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: The World Has Weight

Every object in Dark Arisen has mass. A barrel falls heavy. A leaf drifts light. A sword strikes with force. A ship rocks under wave. Nothing floats abstractly through space — everything is subject to gravity, momentum, and consequence.

**Design intent:** Physics creates believability. Players feel the world's reality through how it responds to forces.

### 1.2 Principle 2: Systems Interact

Wind affects sails. Wind affects fire. Wind affects sound travel. Wind affects bird flight. Wind affects dust patterns. Wind affects everything. Physics systems aren't isolated — they're a web of cause and effect.

**Design intent:** Emergent gameplay through systemic interaction. Players discover effects we didn't design.

### 1.3 Principle 3: The Ocean Is a Character

Water in Dark Arisen isn't a flat texture. It's a living system with currents, waves, depths, temperatures, and moods. The ocean responds to weather, to ships, to itself. It can be calm friend or murderous enemy.

**Design intent:** Pirate-era games depend on water. Dark Arisen's ocean must be the best in any pirate game ever made.

### 1.4 Principle 4: Combat Has Impact

When weapons strike, the world responds. Bodies stagger from blows. Steel rings against steel. Heavy weapons crush. Light weapons cut. Bullets penetrate. Fire spreads. Combat physics make every fight feel real, not animated.

**Design intent:** Bloodborne-tier combat feel through physics, not just animation.

### 1.5 Principle 5: Each Region Has Its Physics

Snow physics in Fjordlund. Sand physics in Ashenmoor. Jungle vegetation in Rexa. Stone in Pale Isle. Each region's physics reflects its character. Walking, fighting, surviving feel different in different places.

**Design intent:** Regional identity through physical experience, not just visuals.

---

## 2. PHASE 6 STRUCTURE — SUBSYSTEM ROADMAP

Phase 6 will be organized into **15 subsystem documents** across four tiers:

### 2.1 Foundation Tier (Universal Physics)

**Document 1: `physics/water_physics.md`**
- Ocean wave systems
- Currents and flow
- Foam and spray
- Underwater physics
- Water-object interaction

**Document 2: `physics/wind_systems.md`**
- Wind direction and strength
- Wind affects on sails, fire, sound, dust
- Storm wind dynamics
- Region-specific wind patterns

**Document 3: `physics/sun_shadow_systems.md`**
- Day-night cycle
- Sun position dynamic
- Shadow casting realistic
- Moonlight at night
- Eclipse rare events

**Document 4: `physics/weather_physics.md`**
- Rain mechanics
- Storm systems
- Lightning and thunder
- Snow accumulation
- Heat shimmer

### 2.2 Combat & Movement Tier

**Document 5: `physics/combat_physics.md`**
- Hit impact and weight
- Weapon physics
- Ragdoll systems
- Blood and gore (style-appropriate)
- Bullet/arrow trajectories

**Document 6: `physics/ship_physics.md`**
- Ship buoyancy
- Ship-wave interaction
- Naval combat physics
- Damage physics
- Cannon ball trajectories

**Document 7: `physics/movement_physics.md`**
- Walking, running, jumping
- Climbing physics
- Swimming and diving
- Falling and tumbling
- Slipping and sliding

### 2.3 Environmental Tier

**Document 8: `physics/snow_ice_physics.md`**
- Fjordlund-specific
- Snow accumulation
- Ice slipperiness
- Frozen water mechanics
- Snowstorm dynamics

**Document 9: `physics/lava_heat_physics.md`**
- Ashenmoor-specific
- Lava flow
- Heat damage
- Volcanic eruptions
- Ash effects

**Document 10: `physics/sand_desert_physics.md`**
- Ashenmoor desert
- Sand mechanics
- Dust storms
- Footstep physics
- Sandstorm visibility

**Document 11: `physics/vegetation_physics.md`**
- Jungle Rexa
- Trees and bushes
- Grass dynamics
- Hanging vines
- Player interaction

### 2.4 Special Systems Tier

**Document 12: `physics/fire_physics.md`**
- Fire ignition
- Fire spread
- Smoke dynamics
- Heat zones
- Environmental impact

**Document 13: `physics/destruction_physics.md`**
- Building damage
- Ship damage
- Object breakage
- World objects
- Persistent damage

**Document 14: `physics/cloth_hair_physics.md`**
- Atmospheric detail
- Sail dynamics
- Clothing realism
- Hair movement
- Flag physics

**Document 15: `physics/sound_physics.md`**
- Sound propagation
- Distance attenuation
- Echo and reflection
- Wind affects sound
- Underwater audio

---

## 3. WATER — THE OCEAN AS CHARACTER

### 3.1 Wave Systems

**Multiple Wave Layers:**

**Primary Waves:**
- Large rolling swells
- Direction follows wind (with delay)
- Height: 0.5m calm → 8m+ storm
- Period: 4-12 seconds

**Secondary Waves:**
- Smaller chop on top
- Faster movement
- Surface texture
- Wind-driven

**Tertiary (Ripples):**
- Surface detail
- Visual richness
- Light interaction

**Rogue Waves:**
- Rare, dramatic
- Surprise element
- Real danger
- Story potential

### 3.2 Wave-Ship Interaction

**Ship Response:**

**Pitching (Bow-Stern):**
- Cresting waves lift bow
- Through troughs, bow drops
- Realistic motion
- Affects gun aim

**Rolling (Side-Side):**
- Beam waves cause rolling
- Crew compensates
- Affects movement on deck
- Cargo can shift

**Heaving (Up-Down):**
- Vertical motion in swells
- Wave amplitude felt
- Crew member balance
- Combat impact

**Yawing (Twisting):**
- Rotation around vertical
- Less common
- Storm conditions
- Difficult to control

### 3.3 Currents and Flow

**Ocean Currents:**
- Major regional currents
- Affect ship travel speed
- Strategic navigation
- Cultural knowledge (Esteban masters this)

**Tidal Effects:**
- Coastal waters specifically
- Affects port access
- Reveals/hides things
- Time-based gameplay

**River Flow:**
- Specific river physics
- Direction affects boat travel
- Whitewater possible
- Different from ocean

### 3.4 Underwater Physics

**Beneath Surface:**
- Visibility based on water clarity
- Pressure simulation (deeper = harder)
- Currents pull divers
- Limited light at depth

**Diving Mechanics:**
- Breath holding
- Pressure squeeze
- Equipment helps
- Specific quest content

### 3.5 Water Interaction

**Objects in Water:**
- Buoyancy realistic
- Heavy objects sink
- Light objects float
- Corpses float (briefly), then sink

**Splashing:**
- Different objects different splashes
- Audio matches
- Visual realistic
- Affects nearby

---

## 4. WIND — THE INVISIBLE FORCE

### 4.1 Wind Direction and Strength

**Wind System:**

**Direction:**
- Compass direction tracked
- Visible via flags, smoke, hair
- Player can read
- Strategic information

**Strength:**
- Calm (0-2 mph)
- Light breeze (3-7 mph)
- Moderate (8-18 mph)
- Strong (19-31 mph)
- Gale (32-46 mph)
- Storm (47-63 mph)
- Hurricane (64+ mph)

**Variability:**
- Real-time changes
- Patterns by region
- Storm building
- Calm before storms

### 4.2 Wind Affects Sails

**Sail Dynamics:**
- Direction relative to wind matters
- Sailing into wind impossible (must tack)
- Crosswind moderate speed
- Following wind maximum speed

**Visible Effects:**
- Sails fill realistically
- Snap when shifting
- Strain visible
- Damage from too-strong wind

### 4.3 Wind Affects Combat

**Arrow Trajectories:**
- Wind pushes arrows
- Long-distance shots affected
- Skill includes wind reading
- Mira teaches Jake

**Bullet Paths:**
- Less affected than arrows
- Still measurable at long range
- Sniper-tier shots considered

**Throwing Weapons:**
- Significantly affected
- Distance reduces with headwind
- Boost with tailwind

### 4.4 Wind Affects Fire

**Fire Spread:**
- Wind direction determines spread
- Strong wind = faster spread
- Calm = slow spread
- Strategic warfare

**Smoke:**
- Smoke goes downwind
- Tactical implications
- Vision blocking
- Specific scenes use this

### 4.5 Wind Affects Sound

**Sound Travel:**
- Sound travels with wind faster
- Against wind, attenuated
- Long-distance hearing affected
- Whisper through wind possible

### 4.6 Wind Affects Atmosphere

**Visual Effects:**
- Hair flowing
- Cloth waving
- Dust drifting
- Leaves rustling
- Snow drifting

**Audio Effects:**
- Wind itself (whoosh, howl)
- Sail flapping
- Tree creaking
- Rope groaning

---

## 5. SUN AND SHADOW

### 5.1 Sun Position

**Dynamic Sun:**
- Tracks across sky
- Time of day matters
- Angle changes shadows
- Affects lighting mood

**Day Length:**
- Longer summers
- Shorter winters
- Regional variation (Fjordlund extreme)
- Realistic seasonality

### 5.2 Shadow Casting

**Realistic Shadows:**
- All objects cast appropriate shadows
- Length varies with sun angle
- Direction tracks sun
- Soft edges where appropriate

**Player Shadow:**
- Jake casts shadow
- Useful for stealth
- Visible to NPCs
- Cover possibility

### 5.3 Lighting Mood

**Time-Based:**

**Dawn (5-7 AM):**
- Gold-pink light
- Long shadows
- Atmospheric beauty
- Cool air

**Morning (7-11 AM):**
- Bright yellow
- Sharp shadows
- Active feeling
- Warming

**Midday (11-2 PM):**
- White light
- Short shadows
- Heat visible
- Sometimes harsh

**Afternoon (2-6 PM):**
- Warm gold
- Long shadows again
- Active but mellower
- Beautiful hour

**Sunset (6-7 PM):**
- Orange-red
- Dramatic
- Romantic light
- Special atmosphere

**Twilight (7-8 PM):**
- Purple-blue
- Brief beauty
- Transition to night

**Night (8 PM-5 AM):**
- Dark, but moonlight if visible
- Stars
- Specific atmospheres
- Different gameplay

### 5.4 Moon Phases

**Lunar Cycle:**
- Full moon = bright nights
- New moon = dark nights
- Tracks with calendar
- Affects gameplay

**Specific Effects:**
- Stealth easier in dark phases
- Vision better in full moon
- Some plants only in moonlight
- Werewolf-adjacent superstition (Jake hears stories)

### 5.5 Eclipse

**Rare Events:**
- Solar eclipse
- Lunar eclipse
- Specific dates calculated
- Story significance possible

---

## 6. COMBAT PHYSICS

### 6.1 Impact and Weight

**Weapon Weight:**
- Each weapon has mass
- Heavy weapons feel heavy
- Light weapons fast
- Realistic momentum

**Hit Impact:**
- Strike force calculated
- Body reacts proportionally
- Stagger believable
- Death realistic

### 6.2 Ragdoll Systems

**Body Physics:**
- Realistic limb articulation
- Death reactions natural
- Falling bodies tumble
- Environmental interaction

**Specific Triggers:**
- Death from height
- Combat death
- Environmental death (lava, etc.)
- Each gets appropriate response

### 6.3 Blood and Gore

**Style-Appropriate:**

**Style B (Brutal):**
- Visible blood
- Realistic spurting
- Wound visualization
- Cinematic horror

**Style D (Reverent):**
- Less blood emphasis
- Quick deaths
- Respectful framing
- Grief-appropriate

**Player Tolerance:**
- Adjustable settings
- Some can dial down
- Default appropriately mature

### 6.4 Projectile Physics

**Arrows:**
- Trajectory realistic
- Wind affected (Section 4.3)
- Penetration calculated
- Some armor stops them

**Bullets:**
- Faster but similar
- Penetration higher
- Range farther
- Sound delay realistic

**Cannonballs:**
- Massive impact
- Buildings damaged
- Ship hulls breached
- Ground deformed

---

## 7. SHIP PHYSICS

### 7.1 Buoyancy and Stability

**Realistic Floating:**
- Ship displacement calculated
- Cargo affects sitting depth
- Listing under uneven load
- Capsizing possible (rare)

### 7.2 Wave Response

**Ship Movement:**
- Reacts to wave systems (Section 3.2)
- Realistic motion
- Affects crew movement
- Affects gun aiming

### 7.3 Naval Combat Physics

**Damage:**
- Cannon ball impacts realistic
- Hull breaches visible
- Mast destruction
- Ship sinks realistically

**Speed:**
- Wind direction affects
- Cargo weight matters
- Hull condition
- Sail condition

### 7.4 Crash Physics

**Ship-Object Collision:**
- Ramming damages both
- Coastal grounding
- Iceberg impact (Fjordlund)
- Reef damage (tropical)

**Rebound:**
- Realistic physics
- Damage proportional
- Recovery possible
- Sometimes terminal

---

## 8. MOVEMENT PHYSICS

### 8.1 Walking and Running

**Surface Response:**
- Different surfaces feel different
- Mud slows
- Sand drags
- Stone solid
- Wood resonant

### 8.2 Climbing

**Realistic Physics:**
- Hand and foothold logic
- Falls calculated
- Fatigue real
- Failure dangerous

### 8.3 Falling

**Velocity:**
- Distance and gravity
- Damage scales realistically
- Death possible
- Water saves (somewhat)

### 8.4 Slipping

**Specific Surfaces:**
- Ice extremely slippery
- Mud somewhat
- Wet stone slick
- Snow variable
- Lava deck (volcanic)

### 8.5 Swimming

**Water Movement:**
- Realistic stroke physics
- Currents affect
- Fatigue real
- Drowning possible

---

## 9. REGIONAL PHYSICS — KEY DIFFERENCES

### 9.1 Fjordlund (Snow and Ice)

**Snow Physics:**
- Footprints visible (tracking!)
- Snow accumulates
- Different depths
- Slowing effect

**Ice Physics:**
- Slippery surface
- Combat affected
- Falls dangerous
- Specific gear helps

**Cold Effects:**
- Visible breath
- Slow mechanics in cold
- Frostbite possible
- Specific gameplay

### 9.2 Ashenmoor (Volcanic Desert)

**Lava Physics:**
- Realistic flow
- Heat damage
- Cooling lava (eventual)
- Eruption events

**Sand Physics:**
- Footprints
- Sand storms
- Sliding
- Buried objects

**Heat Effects:**
- Heat shimmer visible
- Stamina drains
- Specific gear helps
- Water critical

### 9.3 Rexa/Moran (Tropical)

**Vegetation Physics:**
- Realistic plant interaction
- Trees and bushes
- Vines hangable
- Density varies

**Mud Physics:**
- Wet ground sticky
- Slowing
- Footprints visible
- Disease vectors

**Humidity Effects:**
- Visual atmosphere
- Affects equipment
- Disease likely
- Specific gameplay

### 9.4 Pale Isle (Temperate Imperial)

**Stone Physics:**
- Solid surface
- Echoing
- Slipping when wet
- Standard physics

**Civilized Setting:**
- Building interaction
- Doors and windows
- Furniture physics
- Urban environment

### 9.5 Quiet Coast (Temperate)

**Mixed Physics:**
- All standard
- Forest physics
- Wetland physics
- Beach physics

**Familiar Feel:**
- Standard settings
- RDR2-comparable
- Comfort zone

### 9.6 Region 06

**Impossible Physics:**
- Reality bends
- Gravity variable
- Time strange
- Symbolic physical experience

---

## 10. WEATHER PHYSICS

### 10.1 Rain

**Realistic Mechanics:**
- Drops fall and hit
- Accumulates on surfaces
- Forms puddles
- Affects visibility

**Effects on Gameplay:**
- Slipping increased
- Fire harder to start
- Stealth easier
- Mood different

### 10.2 Storm

**Major Weather Event:**
- Building intensity
- Clear warning signs
- Multiple physics interactions
- Real danger

**Components:**
- Heavy rain
- Strong wind
- Lightning
- Thunder
- Wave systems amplified

### 10.3 Lightning

**Visual:**
- Realistic flash
- Branching patterns
- Light shifts
- Atmospheric

**Audio:**
- Thunder delay
- Distance audible
- Echo dynamics
- Specific scenes use

### 10.4 Snow

**Fjordlund Standard:**
- Realistic falling
- Accumulation
- Wind drives
- Visual obscuring

**Effects:**
- Tracks visible
- Cold gameplay
- Slow movement
- Beautiful atmosphere

### 10.5 Sand Storm

**Ashenmoor:**
- Building visibility loss
- Skin damage (specific)
- Wind extreme
- Specific gameplay

---

## 11. FIRE PHYSICS

### 11.1 Ignition

**Fire Sources:**
- Torches
- Cannons
- Lightning strikes
- Volcanic activity
- Cooking fires
- Player intentional

### 11.2 Spread

**Realistic Propagation:**
- Wind direction matters (Section 4.4)
- Material flammability
- Wet vs dry
- Distance to flame

**Strategic Use:**
- Burn enemies' positions
- Defend with fire
- Smoke screen
- Environmental damage

### 11.3 Smoke

**Visual Effects:**
- Realistic plume
- Wind-driven direction
- Density visible
- Distance effects

**Gameplay:**
- Vision blocking
- Stealth (in smoke)
- Breathing impact
- Long-distance signals

---

## 12. DESTRUCTION PHYSICS

### 12.1 Buildings

**Damage:**
- Walls crack
- Roofs collapse
- Windows shatter
- Doors break

**Cascading:**
- Damage causes more damage
- Realistic collapse
- Burying possible
- Long-term effects

### 12.2 Ships

**Already Detailed:**
- Section 7.3
- Specific naval damage
- Realistic sinking
- Impact systems

### 12.3 World Objects

**Crates, Barrels, Furniture:**
- Realistic breakage
- Splinter physics
- Audio matches
- Persistent damage

### 12.4 Persistent Damage

**Memory:**
- Damaged areas stay damaged
- Can be repaired (slowly)
- Some permanent
- Building Evolution integration (Phase 5)

---

## 13. SOUND PHYSICS

### 13.1 Sound Propagation

**Realistic Travel:**
- Sound takes time
- Distance attenuation
- Reflective surfaces
- Echo systems

### 13.2 Wind Effects

**Sound Distortion:**
- Wind carries sound
- Against wind muffled
- Gusts shift sources
- Atmospheric realism

### 13.3 Underwater Audio

**Different World:**
- Water muffles
- Bubbles audible
- Distance shorter
- Distinct soundscape

### 13.4 Ear-Specific

**Realistic Ear Function:**
- Loud noises briefly deafen
- Ringing after explosions
- Recovery time
- Realistic limitation

---

## 14. PHYSICS INTERACTION WEB

### 14.1 The Systems Talk

**Examples of Interaction:**

**Wind + Fire = Spread**
**Rain + Fire = Suppression**
**Wind + Rain = Drifting Drops**
**Wind + Sound = Distortion**
**Wind + Snow = Drifting Snow**
**Sun + Snow = Melting**
**Heat + Body = Sweat**
**Cold + Body = Shiver**
**Wave + Ship = Rolling**
**Wave + Shore = Foam**
**Currents + Ship = Speed Variation**

### 14.2 Emergent Gameplay

**Players Discover:**
- Wind direction affects strategy
- Smoke screens through fire+wind
- Slipping on wet stones
- Tracking through snow/mud
- Sound carrying advantageously

### 14.3 Realistic Consequences

**Actions Have Results:**
- Burn forest, smoke for chapters
- Capsize ship, lose cargo
- Slip on ice, fall off cliff
- Sand storm, lose orientation
- Each action consequences

---

## 15. TECHNICAL CONSIDERATIONS

### 15.1 Performance Budget

**Physics Complexity:**
- Active area: Full simulation
- Distant: Simplified
- Off-screen: Minimal
- Performance friendly

### 15.2 Engine Integration

**Modern Engine Features:**
- Cloth physics
- Hair physics
- Soft body
- Fluid simulation
- Realistic lighting

### 15.3 Optimization

**Smart Simulation:**
- LOD for physics
- Time-step variable
- Multi-threading
- GPU acceleration where possible

### 15.4 Platform Considerations

**Performance Targets:**
- 60fps target
- Variable detail
- Quality settings
- Graceful degradation

---

## 16. ACCESSIBILITY

### 16.1 Visual

**Adjustable:**
- Camera shake reducible
- Motion sickness consideration
- Visual effects toggleable
- Photo-sensitive support

### 16.2 Difficulty

**Physics Difficulty Settings:**
- Realistic (default)
- Forgiving (helps players)
- Casual (simplified)
- Player choice

### 16.3 Specific Accommodations

**Drowning:**
- Auto-save before swims
- Easier escape
- Clear visual warnings

**Slipping:**
- Reduced for accessibility
- Auto-stabilization optional
- Player control

**Heights:**
- Falls less punishing
- Vertigo warnings
- Auto-grab options

---

## 17. CONTENT SCALE

### 17.1 Total Documents

**15 documents covering:**
- 4 Foundation (Universal)
- 3 Combat & Movement
- 4 Environmental
- 4 Special Systems

### 17.2 Estimated Time

**Per Document:**
- Foundation: 2-3 hours each (8-12 total)
- Combat & Movement: 2-3 hours each (6-9 total)
- Environmental: 2-3 hours each (8-12 total)
- Special Systems: 2-3 hours each (8-12 total)

**Total Phase 6: 30-45 hours focused work**

### 17.3 Realistic Pacing

**Suggested Order:**
1. Water Physics (most critical, naval game)
2. Wind Systems (interacts with everything)
3. Sun/Shadow (atmospheric foundation)
4. Weather (general)
5. Combat Physics
6. Ship Physics
7. Movement Physics
8. Then Environmental tier
9. Then Special Systems

---

## 18. WHY PHASE 6 PHYSICS MATTERS

Phase 1-2 gave Dark Arisen its story. Phase 3 gave it its rules. Phase 4 gave it its life. Phase 5 gave it its people. **Phase 6 gives it its FEEL.**

When players draw their bow and feel the wind pull the arrow off course, when they crouch in shadow and watch the sun creep across the wall toward them, when their ship rises on a cresting wave and falls into a trough, when fire spreads downwind exactly as it should — that's when Dark Arisen becomes more than systems. That's when it becomes alive.

Physics is what separates a game world from a game environment. It's what makes players believe.

This is the layer that turns Dark Arisen into Dark Arisen.

---

## 19. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** NPCs of different classes describe physics differently. Sailors speak of wind in maritime terms (Low). Aristocrats describe weather poetically (High).
- ✅ **Pillar 2 (Lore Through Objects):** Physical objects tell story through how they react. Old worn things behave differently than new.
- ✅ **Pillar 3 (Layered Death):** Physics affects death style. Drowning Style D atmospheric. Combat death Style B. Fall death Style A philosophical.
- ✅ **Pillar 4 (Layered Humor):** Style C quirks in physics interactions. Crew slipping, comedic moments, real reactions.
- ✅ **Pillar 5 (Theme — Revenge):** Physics-based combat reflects path. Brutal physics for rage path. Clean physics for measured path. The way Jake fights physically reflects who he is.

---

## 20. NEXT STEPS

**Immediate next document:** `physics/water_physics.md` — Foundation Tier Document 1

This is the most important physics system in a pirate game. Water makes or breaks Dark Arisen. We start with the ocean.

**After Water:** Wind Systems, Sun/Shadow, Weather (completing Foundation Tier).

**Then Combat & Movement Tier.**

**Then Environmental.**

**Then Special Systems.**

---

## 21. CONCLUSION

15 documents. The complete physics foundation of Dark Arisen. The wave that lifts the ship. The wind that fills the sail. The sun that creates the shadow. The fire that spreads with the wind. The blood that splashes when steel strikes flesh.

Physics is invisible when done well — and exquisitely felt. Players don't think about water physics during a storm; they feel terror. They don't think about wind during an arrow shot; they feel mastery. They don't think about shadow during stealth; they feel intelligence.

Phase 6 is where Dark Arisen earns its "feel." Where it stops being designed and starts being inhabited.

**Phase 6 Master Overview — complete.**

**Phase 6 begins. The world's physics awakens.** 🌊💨☀️
