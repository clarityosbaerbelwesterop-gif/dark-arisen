# FIRE PHYSICS — THE BURNING ELEMENT

**Status:** Design Draft v1.0  
**Phase:** Phase 6 — Physics (Subsystem 12 of 15 — Special Systems Tier)  
**Tier:** Special Systems Tier  
**Related:** physics_overview.md, physics/wind_systems.md, physics/weather_physics.md, physics/lava_heat_physics.md, physics/vegetation_physics.md, fauna/processing_and_cooking.md, style_bible.md  
**Purpose:** Define the complete fire physics system — ignition mechanics, realistic spread, smoke dynamics, heat zones, environmental impact, and the cultural significance of fire across regions.  
**Inspirations:** Far Cry 2 (revolutionary fire spread), Red Dead Redemption 2 (atmospheric fires), The Long Dark (fire as survival), Total War series (fire on battlefield), real-world fire physics, ancient cultural relationships with fire

---

## 1. FIRE PHYSICS PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Fire Has Personality

A campfire is family. A torch is hope. A bonfire is celebration. A wildfire is terror. A ritual flame is sacred. Fire is not one thing — it is many things, and players feel each kind differently.

**Design intent:** Atmospheric depth through fire's varied character.

### 1.2 Principle 3: Fire Spreads Realistically

Wind direction. Vegetation density. Surface moisture. Distance to fuel. All combine to create realistic fire behavior. Players who understand fire can use it strategically. Players who don't get burned.

**Design intent:** Skill-based environmental interaction.

### 1.3 Principle 3: Smoke Is Tactical

Smoke obscures vision. Smoke chokes lungs. Smoke rises with heat. Smoke drifts with wind. Smoke as weapon, smoke as cover, smoke as warning — all dynamics matter.

**Design intent:** Layered tactical depth through atmospheric reality.

### 1.4 Principle 4: Fire Is Cultural

Father Salvio lights candles in prayer. Mama Jacinta burns sacred herbs. Mormor Astrid keeps the hearth. Mbah Seruni reads volcanic flames. Each culture relates to fire differently, and these relationships have depth.

**Design intent:** Cultural authenticity through universal element.

### 1.5 Principle 5: Damage Persists

Burned forest stays burned for chapters. Charred buildings remain. Scorched earth recovers slowly. Fire's mark lasts longer than the flame itself. Actions have lasting consequences.

**Design intent:** Persistent world reaction creates real consequence.

---

## 2. FIRE IGNITION

### 2.1 Ignition Sources

**Player-Controlled:**

**Torches:**
- Manual lighting
- Cultural item
- Specific gameplay
- Standard

**Flint and Steel:**
- Standard fire-starting
- Specific time
- Skill-based
- Realistic

**Matches (Late-Era):**
- Quick lighting
- Cultural progression
- Specific item
- Practical

**Cigars/Pipes:**
- Smoking ignition
- Cross-reference fauna/tobacco_system.md
- Cultural
- Atmospheric

**Fire Arrows:**
- Combat ignition
- Cross-reference physics/combat_physics.md
- Specific tactic
- Memorable

**Cooking Fires:**
- Crew tasks
- Cross-reference fauna/processing_and_cooking.md
- Cultural
- Practical

### 2.2 Natural Ignition

**Lightning:**
- Cross-reference physics/weather_physics.md
- Strikes start fires
- Wildfire risk
- Memorable

**Lava:**
- Cross-reference physics/lava_heat_physics.md
- Sets vegetation aflame
- Catastrophic
- Memorable

**Friction:**
- Specific scenarios
- Rare
- Realistic
- Atmospheric

**Sun (Magnifying):**
- Specific scenarios
- Rare
- Realistic
- Atmospheric

### 2.3 Ignition Requirements

**For Fire to Start:**

**Heat Source:**
- Spark, ember, flame
- Specific physics
- Realistic

**Fuel:**
- Combustible material
- Specific types
- Realistic

**Oxygen:**
- Always present
- Air movement aids
- Realistic

**Ignition Temperature:**
- Material-specific
- Specific physics
- Realistic

### 2.4 Fuel Types

**Easy Burning:**

**Dry Grass:**
- Quick ignition
- Fast spread
- Specific scenarios
- Realistic

**Dry Leaves:**
- Quick ignition
- Specific patterns
- Realistic
- Atmospheric

**Paper:**
- Instant ignition
- Burns quick
- Specific scenarios
- Standard

**Cloth:**
- Easy ignition
- Specific gameplay
- Realistic
- Tactical

### 2.5 Moderate Burning

**Wood:**
- Standard fuel
- Sustained burn
- Cooking fires
- Practical

**Cured Tobacco:**
- Specific use
- Cigars/pipes
- Cultural
- Atmospheric

**Tar/Pitch:**
- Specific maritime use
- Sustained fire
- Tactical
- Memorable

### 2.6 Difficult Burning

**Wet Wood:**
- Hard to ignite
- Smoky burn
- Specific scenarios
- Realistic

**Green Vegetation:**
- Resistant
- Heavy smoke
- Specific tactics
- Atmospheric

**Stone/Metal:**
- Cannot burn
- Standard
- Realistic

### 2.7 Specific Cultural Fires

**Sacred Fire (Indigenous Rexan):**
- Specific protocols
- Mama Jacinta tends
- Cultural depth
- Memorable

**Hearth Fire (Fjordlund):**
- Always burning
- Mormor Astrid keeps
- Cultural significance
- Atmospheric

**Candle (Imperial Christian):**
- Father Salvio lights
- Religious significance
- Specific atmosphere
- Memorable

**Pyre (Funeral):**
- Cultural rituals
- Specific scenes
- Memorable
- Reverent

---

## 3. FIRE BEHAVIOR

### 3.1 Flame Physics

**Realistic Flame:**

**Color:**
- Yellow-orange standard
- Blue at hottest core
- Specific to fuel
- Realistic

**Movement:**
- Wind-affected
- Heat rises
- Flickering realistic
- Atmospheric

**Size:**
- Fuel-dependent
- Wind-affected
- Realistic
- Variable

### 3.2 Heat Output

**Heat Zones:**

**Direct (0-1m):**
- Lethal
- Cannot stand
- Specific damage
- Realistic

**Close (1-3m):**
- Painful
- Damage over time
- Strategic awareness
- Demanding

**Warm (3-5m):**
- Comfortable warmth
- No damage
- Cultural use (campfires)
- Standard

**Distant (5-10m):**
- Slight warmth
- No damage
- Atmospheric
- Standard

### 3.3 Heat Effects

**On Player:**

**Mild Heat:**
- Comfortable
- No effects
- Standard

**Strong Heat:**
- Slight stamina drain
- Sweating animation
- Specific gameplay
- Realistic

**Burning Heat:**
- Direct damage
- Cannot remain
- Crisis
- Memorable

### 3.4 Smoke Generation

**Realistic Production:**

**Clean Burn (Hot Fire):**
- Minimal smoke
- Mostly invisible
- Standard
- Atmospheric

**Standard Burn:**
- Visible smoke
- Atmospheric
- Standard
- Common

**Smoldering:**
- Heavy smoke
- Specific scenarios
- Tactical
- Atmospheric

**Wet Material:**
- Maximum smoke
- Specific tactical use
- Realistic
- Memorable

### 3.5 Light Output

**Cross-Reference:** physics/sun_shadow_systems.md

**Fire as Light Source:**

**Candle:**
- 1-2m radius
- Soft warm
- Cultural
- Atmospheric

**Torch:**
- 5-8m radius
- Active orange
- Practical
- Standard

**Campfire:**
- 8-12m radius
- Warm illumination
- Cultural gathering
- Atmospheric

**Bonfire:**
- 15-20m radius
- Major illumination
- Celebration
- Memorable

**Wildfire:**
- Massive radius
- Dangerous illumination
- Crisis
- Memorable

---

## 4. FIRE SPREAD MECHANICS

### 4.1 Wind-Driven Spread

**Cross-Reference:** physics/wind_systems.md

**Direction:**
- Fire spreads downwind
- Source visible upwind
- Predictable pattern
- Realistic

**Speed:**

**Calm:**
- Slow creep
- 0.5 m/min
- Manageable
- Standard

**Light Wind:**
- Steady spread
- 1 m/min
- Specific gameplay
- Realistic

**Moderate Wind:**
- Active spread
- 3 m/min
- Concerning
- Tactical

**Strong Wind:**
- Fast spread
- 10 m/min
- Crisis
- Memorable

**Storm Wind:**
- Explosive
- 30+ m/min
- Catastrophic
- Crisis

### 4.2 Vegetation Spread

**Through Plants:**

**Dry Grass:**
- Very fast
- Wave-like
- Specific scenarios
- Realistic

**Dry Brush:**
- Fast
- Specific patterns
- Realistic
- Tactical

**Trees:**
- Slower
- Crowns can ignite
- Specific scenarios
- Memorable

**Forest Canopy:**
- Crown fires
- Devastating
- Specific physics
- Crisis

### 4.3 Fire Jumping

**Across Gaps:**

**Embers Fly:**
- Wind carries
- Start new fires
- Specific scenarios
- Realistic

**Distance:**
- Few meters typical
- Strong wind farther
- Realistic
- Demanding

**Through Buildings:**
- Through walls
- Roofs
- Specific physics
- Memorable

### 4.4 Spread Limits

**Natural Barriers:**

**Water:**
- Stops fire
- Specific tactical
- Realistic
- Practical

**Stone:**
- Cannot burn
- Specific terrain
- Tactical
- Realistic

**Cleared Area:**
- No fuel
- Stops fire
- Cultural knowledge
- Strategic

**Wet Areas:**
- Slows fire
- Specific scenarios
- Realistic
- Tactical

### 4.5 Player Fire Control

**Building Fire Lines:**
- Cultural knowledge
- Specific gameplay
- Strategic
- Cultural

**Backfires:**
- Specific tactics
- Cultural depth
- Memorable
- Realistic

**Water Fighting:**
- Specific gameplay
- Crew effort
- Cultural
- Practical

---

## 5. SMOKE DYNAMICS

### 5.1 Smoke Behavior

**Realistic Movement:**

**Rising:**
- Hot smoke goes up
- Rate based on heat
- Specific physics
- Realistic

**Drifting:**
- Wind-driven horizontally
- Cross-reference physics/wind_systems.md
- Specific patterns
- Atmospheric

**Settling:**
- Cool smoke descends
- Specific scenarios
- Realistic
- Atmospheric

**Plume Shape:**
- Distinctive forms
- Wind-shaped
- Visible from distance
- Atmospheric

### 5.2 Smoke Density

**Light Smoke:**
- Wispy
- Atmospheric
- Standard
- Common

**Moderate Smoke:**
- Visible cloud
- Some vision impact
- Standard
- Common

**Heavy Smoke:**
- Major obscuring
- Specific gameplay
- Tactical
- Memorable

**Choking Smoke:**
- Cannot breathe
- Damage over time
- Crisis
- Memorable

### 5.3 Smoke Effects on Player

**Vision:**

**Light:**
- Slight obscuring
- Atmospheric
- Standard

**Moderate:**
- Reduced visibility
- Specific gameplay
- Tactical

**Heavy:**
- Major impairment
- Crisis vision
- Memorable

**Total:**
- Cannot see
- Crisis
- Specific scenarios

### 5.4 Smoke and Breathing

**Respiratory Effects:**

**Light Exposure:**
- Slight cough
- Standard
- Realistic

**Moderate:**
- Stamina drain
- Specific gameplay
- Demanding

**Heavy:**
- Major damage
- Cannot remain
- Crisis
- Memorable

**Suffocation:**
- Specific death
- Style D atmospheric
- Memorable
- Cautionary

### 5.5 Smoke Tactical Use

**Stealth:**
- Cover from enemies
- Specific gameplay
- Tactical
- Memorable

**Signaling:**
- Specific colored smoke
- Cultural use
- Long-distance communication
- Memorable

**Combat:**
- Throw smoke
- Specific tactics
- Memorable
- Strategic

**Extraction:**
- Cover escape
- Specific scenarios
- Tactical
- Memorable

---

## 6. SPECIFIC FIRE TYPES

### 6.1 Campfire

**Cultural and Practical:**

**Setup:**
- Cross-reference fauna/processing_and_cooking.md
- Specific gameplay
- Cultural
- Standard

**Use:**
- Cooking
- Warmth
- Light
- Cultural gathering

**Extinguishing:**
- Specific care
- Cultural respect
- Practical
- Realistic

**Atmosphere:**
- Crew gathering
- Memorable
- Cultural
- Beautiful

### 6.2 Torch

**Standard Light:**

**Carrying:**
- Single hand
- Light radius
- Practical
- Standard

**Combat:**
- Can be used as weapon
- Specific tactics
- Memorable
- Cultural

**Lighting Things:**
- Spread fire
- Specific gameplay
- Tactical
- Realistic

### 6.3 Bonfire

**Major Fire:**

**Setup:**
- Larger fuel
- Cultural events
- Specific gameplay
- Memorable

**Use:**
- Celebrations
- Signal fires
- Cultural significance
- Atmospheric

**Visual:**
- Spectacular
- Visible from far
- Memorable
- Cinematic

### 6.4 Building Fire

**Catastrophic:**

**Spread:**
- Through wood
- Specific physics
- Realistic
- Memorable

**Damage:**
- Cross-reference npcs/building_evolution.md
- Persistent
- Story-significant
- Memorable

**Crew Response:**
- Crisis mode
- Specific gameplay
- Cultural
- Memorable

### 6.5 Ship Fire

**Cross-Reference:** physics/ship_physics.md

**Specific:**
- Catastrophic possible
- Crew fights
- Memorable
- Crisis

### 6.6 Wildfire

**Devastating Spread:**

**Triggers:**
- Lightning
- Carelessness
- Combat
- Specific scenarios

**Spread:**
- Massive scale
- Days/weeks
- Devastating
- Memorable

**Recovery:**
- Years/chapters
- Persistent damage
- Specific gameplay
- Cultural impact

### 6.7 Volcanic Fire

**Cross-Reference:** physics/lava_heat_physics.md

**Specific:**
- Lava-driven
- Catastrophic
- Memorable
- Atmospheric

### 6.8 Sacred Fire

**Cultural:**

**Indigenous Rexan:**
- Mama Jacinta tends
- Specific protocols
- Cultural depth
- Memorable

**Imperial Christian:**
- Father Salvio's candles
- Religious significance
- Specific atmosphere
- Memorable

**Fjordlund Hearth:**
- Mormor Astrid keeps
- Always burning
- Cultural significance
- Atmospheric

**Volcanic (Ashenmoor):**
- Mbah Seruni reads
- Specific knowledge
- Cultural depth
- Memorable

---

## 7. FIRE COMBAT

### 7.1 Fire Arrows

**Cross-Reference:** physics/combat_physics.md

**Mechanics:**
- Arrow + flame
- Specific weapons
- Tactical
- Memorable

**Effects:**
- Sets target on fire
- Persistent damage
- Strategic
- Realistic

### 7.2 Hot Shot (Cannons)

**Cross-Reference:** physics/ship_physics.md

**Specific:**
- Heated cannonball
- Sets ships ablaze
- Tactical
- Memorable

### 7.3 Burning Oil

**Tactical Weapon:**

**Use:**
- Pour on enemies
- Light alight
- Specific scenarios
- Memorable

**Strategic:**
- Defensive walls
- Specific tactics
- Cultural
- Tactical

### 7.4 Molotov-Style

**Throwing Fire:**
- Specific weapons
- Cultural
- Memorable
- Tactical

### 7.5 Fire Defense

**Player Defense:**

**Avoiding:**
- Stay clear
- Specific tactics
- Realistic
- Demanding

**Extinguishing:**
- Roll on ground
- Cultural
- Specific gameplay
- Realistic

**Water:**
- Best defense
- Specific tactics
- Practical
- Realistic

---

## 8. CULTURAL FIRE

### 8.1 Indigenous Rexan

**Sacred Fire:**

**Mama Jacinta:**
- Specific protocols
- Cultural treasure
- Memorable
- Atmospheric

**Ceremonies:**
- Specific scenes
- Cultural depth
- Memorable
- Atmospheric

### 8.2 Imperial Christian

**Religious Fire:**

**Father Salvio:**
- Candle lighting
- Religious depth
- Specific atmosphere
- Memorable

**Specific Occasions:**
- Funerals
- Mass
- Specific scenes
- Memorable

### 8.3 Fjordlund Pagan

**Hearth Fire:**

**Mormor Astrid:**
- Always burning
- Cultural significance
- Specific atmosphere
- Memorable

**Solstice Fires:**
- Cultural celebrations
- Specific scenes
- Memorable
- Atmospheric

### 8.4 Ashenmoor Volcanic

**Earth Fire:**

**Mbah Seruni:**
- Reads volcanic flames
- Cultural knowledge
- Specific gameplay
- Memorable

**Cultural Connection:**
- Wyrm-related
- Cross-reference characters/ashen_wyrm.md
- Atmospheric
- Memorable

### 8.5 Sailor Fire

**Practical:**

**Galley Fire:**
- Cooking essential
- Crew need
- Specific gameplay
- Practical

**Beacon Fires:**
- Navigation aid
- Cultural
- Atmospheric
- Memorable

**Burning at Sea:**
- Specific dangers
- Crew crisis
- Memorable
- Tactical

---

## 9. FIRE AND WEATHER

### 9.1 Rain Effects

**Cross-Reference:** physics/weather_physics.md

**Light Rain:**
- Slows fire
- Specific scenarios
- Realistic
- Tactical

**Heavy Rain:**
- Extinguishes
- Specific gameplay
- Realistic
- Memorable

### 9.2 Wind Effects

**Cross-Reference:** physics/wind_systems.md, Section 4

### 9.3 Snow Effects

**Cross-Reference:** physics/snow_ice_physics.md

**Specific:**
- Wet snow extinguishes
- Powder less effective
- Realistic
- Memorable

### 9.4 Drought Risk

**Dry Conditions:**
- Higher fire risk
- Specific scenarios
- Realistic
- Memorable

---

## 10. PERSISTENT FIRE DAMAGE

### 10.1 Burned Vegetation

**Long-Term:**

**Cross-Reference:** fauna/regional_ecosystems.md

**Recovery:**
- Years
- Specific stages
- Cultural impact
- Memorable

### 10.2 Burned Buildings

**Cross-Reference:** npcs/building_evolution.md

**Persistent:**
- Story-significant
- Memorable
- Cultural impact
- Realistic

### 10.3 Burned Land

**Scorched Earth:**

**Specific Atmosphere:**
- Dead landscape
- Slow recovery
- Atmospheric
- Memorable

**Cultural Impact:**
- Specific reactions
- Memorable
- Story-rich

### 10.4 Permanent Marks

**Some Fires Don't Heal:**

**Specific Scenarios:**
- Story-significant
- Atmospheric
- Memorable
- Cautionary

---

## 11. FIRE STORYTELLING

### 11.1 What Fires Tell

**Reading Flames:**

**Sacred Fire:**
- Cultural reverence
- Atmospheric
- Memorable
- Beautiful

**Cooking Fire:**
- Domestic warmth
- Cultural
- Atmospheric
- Comforting

**Memorial Fire:**
- Loss and remembrance
- Cultural depth
- Style D
- Memorable

**Wildfire:**
- Crisis
- Devastation
- Memorable
- Atmospheric

### 11.2 Specific Scenes

**Fire Story Moments:**

**The Funeral Pyre:**
- Cultural
- Style D reverent
- Memorable
- Cinematic

**The Burning Ship:**
- Crisis
- Memorable
- Tactical
- Cinematic

**The Forest Fire:**
- Major event
- Devastating
- Memorable
- Story-rich

**The Hearth Welcome:**
- Cultural
- Mormor Astrid
- Atmospheric
- Memorable

### 11.3 Cultural Fire Scenes

**Specific Moments:**

**Sacred Ceremony:**
- Mama Jacinta
- Cultural depth
- Memorable
- Atmospheric

**Religious Service:**
- Father Salvio
- Specific atmosphere
- Memorable
- Cultural

**Solstice Celebration:**
- Mormor Astrid
- Cultural depth
- Memorable
- Beautiful

---

## 12. PS5 DUALSENSE INTEGRATION

### 12.1 Fire Heat Feedback

**Subtle:**
- Heat sensation
- Specific feedback
- Atmospheric
- Immersive

### 12.2 Burning

**Critical Feedback:**
- Specific damage
- Crisis felt
- Memorable
- Realistic

### 12.3 Torch Carrying

**Steady Flame:**
- Specific feedback
- Atmospheric
- Realistic
- Immersive

### 12.4 Fire Combat

**Fire Arrow:**
- Specific feedback
- Memorable
- Cinematic
- Atmospheric

### 12.5 Adaptive Triggers

**Cooking:**
- Specific tension
- Realistic
- Atmospheric

**Fire Starting:**
- Specific feedback
- Skill-based
- Realistic

---

## 13. ACCESSIBILITY

### 13.1 Fire Damage

**Adjustable:**
- Realistic
- Forgiving
- Casual
- Player choice

### 13.2 Smoke Effects

**Settings:**
- Realistic
- Reduced
- Visual only
- Player friendly

### 13.3 Visibility

**Smoke Adjustable:**
- Realistic
- Enhanced
- Standard
- Player friendly

### 13.4 Visual Effects

**Bright Flames:**
- Adjustable intensity
- Photo-sensitive friendly
- Standard
- Inclusive

---

## 14. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** Cultural fire keepers speak in cultural Middle-High. Sacred fire High register. Practical fire Low. Cultural variation.
- ✅ **Pillar 2 (Lore Through Objects):** Specific fire items reveal culture. Sacred fires tell stories. Cultural items carry weight.
- ✅ **Pillar 3 (Layered Death):** Pyre deaths Style D reverent. Burning deaths Style B brutal. Specific cultural deaths.
- ✅ **Pillar 4 (Layered Humor):** Style C quirks (Big Tom's bonfires, comedy in fire-starting failures, real reactions).
- ✅ **Pillar 5 (Theme — Revenge):** Fire reflects path. Burning rage for vengeance. Sacred flame for peace. Atmospheric mirror.

---

## 15. NEXT STEPS

Next document: **`physics/destruction_physics.md`** — Special Systems Tier Document 2

Will cover:
- Building damage detailed
- Ship damage cross-reference
- Object breakage
- World object physics
- Persistent damage tracking

After Destruction: Cloth/Hair Physics, Sound Physics (final two).

---

## 16. CONCLUSION

Fire. The element that warms and destroys. The element that gathers crew around it for stories, that lights Mama Jacinta's sacred ceremonies, that burns Father Salvio's candles in prayer, that keeps Mormor Astrid's hearth for centuries, that connects Mbah Seruni to volcanic spirits.

When players light their first campfire and feel its warmth gather the crew, when they watch a burning ship sink slowly into dark waters, when they witness a forest fire's devastation, when they observe a funeral pyre with appropriate reverence — that's when fire becomes more than mechanic. That's when it becomes the universal element it has always been to humanity.

**Phase 6 Subsystem 12 of 15 — complete.**

**The flame burns. The smoke rises. The world warms or burns. Fire lives.** 🔥
