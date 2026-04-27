# SNOW & ICE PHYSICS — THE FROZEN NORTH

**Status:** Design Draft v1.0  
**Phase:** Phase 6 — Physics (Subsystem 8 of 15 — Environmental Tier)  
**Tier:** Environmental Tier  
**Related:** physics_overview.md, physics/water_physics.md, physics/weather_physics.md, physics/movement_physics.md, style_bible.md  
**Purpose:** Define the complete snow and ice physics system specific to Fjordlund and other cold regions — accumulation, slipperiness, frozen water, snowstorms, cold damage, and the unique gameplay of arctic environments.  
**Inspirations:** Red Dead Redemption 2 (snow physics), The Long Dark (cold survival), Skyrim (snow accumulation), Tomb Raider (ice mechanics), Death Stranding (terrain demands), real-world arctic survival

---

## 1. SNOW & ICE PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Cold Is a Character

Cold is not a stat. It is a presence. It bites at exposed skin. It makes breath visible. It demands respect. Cold is Fjordlund's first inhabitant, and Jake must learn to share space with it.

**Design intent:** Atmospheric realism through environmental presence.

### 1.2 Principle 2: Snow Records Everything

Footprints in snow tell stories. A boot print, a paw print, blood, drag marks. Snow is a memory surface — what walks on it leaves trace. Tracking becomes possible. Stealth becomes harder.

**Design intent:** Environmental storytelling through accumulated marks.

### 1.3 Principle 3: Ice Is Beautiful and Deadly

A frozen lake is a path or a trap. Ice gleaming in sunlight is gorgeous and treacherous. Falling through is real possibility. Players learn that beauty hides danger.

**Design intent:** Aesthetic and tactical fusion. Ice rewards careful observation.

### 1.4 Principle 4: Cold Has Cultural Depth

Fjordlunders adapted over generations. Mormor Astrid lights specific fires. Hunters wear specific gear. Ships sail specific routes. Cold isn't survived alone — it's survived through inherited wisdom.

**Design intent:** Cultural authenticity through environmental adaptation.

### 1.5 Principle 5: Fire Is Sacred Here

In Fjordlund, fire isn't just warm — it's life. Crew members gather around. Strangers are welcomed at hearth. Cold cultures revere fire because they truly need it.

**Design intent:** Realistic relationship with fundamental element.

---

## 2. SNOW ACCUMULATION

### 2.1 Snow Layers

**Multiple Snow States:**

**Fresh Powder:**
- Just fallen
- Soft, fluffy
- Easy to disturb
- Standard snowfall

**Settled Snow:**
- Few hours old
- Compacted slightly
- Clear footprints
- Standard

**Old Snow:**
- Days old
- Compacted significantly
- Subtle footprints
- Realistic

**Crusty Snow:**
- Surface frozen
- Breakable
- Specific gameplay
- Realistic

**Slush:**
- Melting snow
- Wet and slippery
- Specific physics
- Realistic

### 2.2 Accumulation Rates

**Light Snowfall:**
- 2-5 cm per hour
- Atmospheric
- Slow buildup

**Standard Snowfall:**
- 5-15 cm per hour
- Visible accumulation
- Active buildup

**Heavy Snowfall:**
- 15-30 cm per hour
- Rapid accumulation
- Specific challenges

**Blizzard:**
- 30+ cm per hour
- Crisis conditions
- Visibility loss
- See Section 7

### 2.3 Surface-Specific Accumulation

**Open Ground:**
- Maximum accumulation
- Even spread
- Standard

**Under Trees:**
- Reduced accumulation
- Canopy intercepts
- Specific patches
- Realistic

**Wind-Exposed:**
- Less accumulation
- Wind-blown clear
- Specific patterns
- Realistic

**Wind-Sheltered:**
- Heavy drifts
- Realistic accumulation
- Tactical implications

**On Stone:**
- Standard accumulation
- Cold preserves
- Stays longer

**On Roofs:**
- Heavy accumulation
- Eventual avalanche risk
- Atmospheric danger
- Specific events

**On Water:**
- Floats briefly
- Eventually melts/sinks
- Specific physics
- Realistic

### 2.4 Drift Formation

**Wind-Driven:**
- Cross-reference physics/wind_systems.md
- Snow piles in lee of obstacles
- Realistic patterns
- Beautiful

**Drift Heights:**
- Light wind: 0.3m drifts
- Moderate: 1m drifts
- Strong: 2m+ drifts
- Storm: 3m+ extreme drifts

**Implications:**
- Path blocked by drifts
- Hidden objects revealed/buried
- Specific gameplay
- Realistic challenges

### 2.5 Snow Compression

**Player Compression:**
- Footprints remain
- Path of compressed snow visible
- Realistic
- Tracking aid

**Vehicle/Ship Compression:**
- Visible trails
- Tactical info
- Realistic
- Memorable

### 2.6 Snow Removal

**Natural:**
- Wind blows
- Sun melts (slow)
- Time passes
- Realistic

**Artificial:**
- Path clearing (NPCs)
- Specific gameplay
- Cultural
- Practical

---

## 3. FOOTPRINTS AND TRACKING

### 3.1 Footprint Visibility

**Fresh Footprints:**
- Sharply visible
- Direction clear
- Recent (less than 1 hour)
- Tracking enabled

**Aging Footprints:**
- Softening edges
- Wind-blurred
- Realistic decay
- Time-based

**Old Footprints:**
- Faded
- Hard to read
- Eventually invisible
- Realistic

### 3.2 Footprint Types

**Player (Jake):**
- Specific boot pattern
- Visible to NPCs
- Realistic
- Tracking enables

**NPC:**
- Various boot patterns
- Cultural variations
- Realistic
- Storytelling

**Animal:**
- Specific tracks per species
- Cross-reference fauna/hunting_system.md
- Realistic
- Hunting aid

**Specific Stories:**
- Drag marks (something pulled)
- Blood drops (wounded)
- Disturbed snow (struggle)
- Atmospheric

### 3.3 Stealth Implications

**Cannot Hide:**
- Tracks always visible (until aged)
- Strategic awareness
- Demanding

**Counter-Tactics:**
- Walk in others' tracks
- Use wind-cleared areas
- Specific routes
- Skill-based

**Winter Stealth:**
- Harder than other seasons
- Specific gear (white camo)
- Cultural adaptation
- Strategic

### 3.4 Hunting in Snow

**Cross-Reference:** fauna/hunting_system.md Section 4

**Snow Advantages:**
- Tracks visible
- Tracking easier
- Animal locations clearer
- Tactical

**Snow Disadvantages:**
- Player tracked too
- Cold concerns
- Specific gear needed
- Realistic

### 3.5 Tracking Skill Development

**Player Learns:**

**Beginner:**
- Generic prints
- Just "tracks"
- Limited info

**Intermediate:**
- Species recognition
- Direction clear
- Some age estimation

**Advanced:**
- Specific identification
- Age accurate
- Health estimation
- Skilled

**Master:**
- Full reading
- Story from tracks
- Cultural depth
- Fjordlund Hunter level

---

## 4. ICE PHYSICS

### 4.1 Ice Surface Types

**Clear Ice:**
- Transparent
- See-through
- Smooth
- Beautiful

**Cloudy Ice:**
- Opaque white
- Standard
- Stable
- Common

**Old Ice:**
- Cracked surface
- Less reliable
- Strategic awareness
- Realistic

**Wet Ice:**
- Surface melted
- Maximum slipperiness
- Most dangerous
- Memorable

**Black Ice:**
- Hidden danger
- Looks like wet ground
- Catches unaware
- Realistic surprise

### 4.2 Ice Slipperiness

**Slipping Risk:**

**Standard Ice:**
- High slip chance
- Cleated boots help
- Specific physics
- Demanding

**Wet Ice:**
- Maximum slip
- Sliding likely
- Combat compromised
- Memorable

**Cleated Movement:**
- Specific gear
- Reduced slip
- Cultural item
- Practical

### 4.3 Frozen Water Mechanics

**Ice Thickness:**

**Thin Ice (0-5 cm):**
- Cannot support weight
- Breaks immediately
- Falling through likely
- Realistic danger

**Moderate Ice (5-15 cm):**
- Supports player
- May crack under stress
- Specific risk
- Tactical

**Thick Ice (15+ cm):**
- Reliable support
- Standard movement
- Most fjordland frozen lakes
- Common

**Ship Ice:**
- Different concerns
- Damage to hulls
- Specific physics
- Memorable

### 4.4 Ice Breaking

**Triggers:**

**Weight:**
- Single player on thin ice = breaks
- Group on moderate ice = breaks
- Specific calculations
- Realistic

**Impact:**
- Heavy fall = breaks
- Combat impacts
- Cannon balls
- Specific scenarios

**Stress:**
- Cracking sound first
- Visible cracks spreading
- Brief warning
- Realistic

### 4.5 Falling Through Ice

**The Catastrophe:**

**Immediate:**
- Below freezing water
- Specific cold damage
- Breath knocked out
- Crisis instantly

**Survival:**
- Hypothermia begins (Section 6)
- Limited time to escape
- Specific gameplay
- Memorable

**Rescue:**
- Crew throw rope
- Lay flat on ice
- Specific actions
- Tense scene

**Death Possible:**
- Realistic
- Specific scenarios
- Memorable
- Cautionary

### 4.6 Ice Combat

**Slipping Mid-Battle:**
- Vulnerable moment
- Tactical
- Specific physics
- Memorable

**Knocking Enemies onto Ice:**
- Strategic positioning
- Tactical depth
- Realistic
- Memorable

**Falling Through During Combat:**
- Catastrophic
- Combat ends abruptly
- Crisis
- Memorable

### 4.7 Ice as Weapon

**Ice Spikes:**
- Specific weapons
- Throwable
- Cultural
- Specific gameplay

**Frozen Surfaces:**
- Smashing ice = impromptu weapon
- Specific scenarios
- Tactical
- Memorable

---

## 5. SNOWSTORM DYNAMICS

### 5.1 Building Storm

**Cross-Reference:** physics/weather_physics.md Section 4

**Snowstorm-Specific:**

**Stage 1: Distant Indicators**
- Specific cloud patterns
- Pressure dropping
- Animals seeking shelter
- Mira reads it

**Stage 2: Approaching**
- Heavier snowfall
- Wind picking up
- Visibility reducing
- Time to seek shelter

**Stage 3: Storm Hits**
- Wind extreme
- Snow horizontal
- Visibility minimal
- Crisis

**Stage 4: Storm Continues**
- Sustained intensity
- Cold deepens
- Survival mode
- Memorable

**Stage 5: Storm Passes**
- Slowly diminishing
- Eventual calm
- Recovery
- Beautiful

### 5.2 Whiteout Conditions

**Visibility:**

**Light Snow:** 100m+
**Heavy Snow:** 30-50m
**Blizzard:** 10-20m
**Whiteout:** 5m or less
**Crisis Whiteout:** Less than 2m

**Effects:**
- Disorientation
- Lost easily
- Specific gameplay
- Crisis

### 5.3 Wind in Snowstorm

**Cross-Reference:** physics/wind_systems.md

**Snow-Specific:**
- Drives snow horizontal
- Skin damage from impact
- Sound dominated
- Crisis intensity

### 5.4 Snowstorm Dangers

**Hypothermia:**
- See Section 6
- Real risk
- Specific gameplay
- Crisis

**Lost Direction:**
- Cannot navigate
- Specific gear helps
- Cultural knowledge
- Demanding

**Avalanche Risk:**
- Mountain slopes
- Wind-loaded snow
- Specific events
- Memorable

**Crew Loss:**
- Specific scenarios
- Realistic
- Tragic
- Memorable

### 5.5 Surviving Snowstorm

**Find Shelter:**
- Specific places
- Cultural knowledge
- Strategic
- Critical

**Build Fire:**
- If possible
- Limited fuel
- Specific gameplay
- Survival

**Stay Together:**
- Crew unity
- Specific dialogue
- Cultural
- Practical

**Wait It Out:**
- Patience
- Realistic
- Demanding
- Atmospheric

### 5.6 Specific Snowstorm Events

**Story Moments:**

**Lost in Storm:**
- Specific quest possibility
- Atmospheric
- Memorable
- Survival mode

**Saving Crew:**
- Specific scenes
- Heroic moments
- Cultural depth
- Memorable

**Ghost Ship Encounter:**
- Cross-reference fauna/sea_animals.md
- Storm conditions
- Atmospheric
- Memorable

---

## 6. COLD DAMAGE SYSTEM

### 6.1 Temperature Awareness

**Visible Indicators:**
- Breath visible (cold threshold)
- Shivering animation
- Frost on equipment
- Cultural cues

**Player Information:**
- Subtle UI elements
- Atmospheric mostly
- Realistic
- Specific gameplay

### 6.2 Cold Levels

**Mild Cold (0-10°C / 32-50°F):**
- Cool feeling
- Minor effects
- Most temperate winter
- Standard

**Cold (-10 to 0°C / 14-32°F):**
- Visible breath
- Slight stamina drain
- Specific gear helps
- Realistic

**Very Cold (-20 to -10°C / -4-14°F):**
- Significant effects
- Stamina drain real
- Specific gear required
- Demanding

**Extreme Cold (-30°C / -22°F or below):**
- Survival mode
- Hypothermia risk
- Specific gameplay
- Crisis

**Polar Cold (-40°C / -40°F or below):**
- Crisis immediately
- Death possible
- Specific scenarios
- Memorable

### 6.3 Hypothermia Stages

**Stage 1: Cold:**
- Shivering
- Slight stamina effects
- Manageable
- Standard cold

**Stage 2: Mild Hypothermia:**
- Severe shivering
- Coordination affected
- Specific debuffs
- Crisis approaching

**Stage 3: Moderate Hypothermia:**
- Shivering stops (bad sign)
- Confusion
- Specific gameplay
- Real danger

**Stage 4: Severe Hypothermia:**
- Vital signs dropping
- Death imminent
- Crisis mode
- Memorable

**Stage 5: Death:**
- Specific death animation
- Style D atmospheric
- Realistic
- Memorable cautionary

### 6.4 Cold Equipment

**Cultural Gear:**

**Light Layer:**
- Standard temperate
- No protection from severe
- Common

**Medium Cold Gear:**
- Warm coat
- Gloves
- Hat
- Standard winter

**Heavy Cold Gear:**
- Furs
- Insulation
- Specific Fjordlund
- Survival level

**Master Cold Gear:**
- Best Fjordlund
- Maximum protection
- Mormor Astrid recommendations
- Premium

### 6.5 Frostbite

**Specific Damage:**

**Fingers:**
- Reduced dexterity
- Combat affected
- Specific gameplay
- Realistic

**Toes:**
- Movement affected
- Specific debuffs
- Realistic
- Demanding

**Face:**
- Visible damage
- Permanent if severe
- Cultural
- Memorable

**Recovery:**
- Warm gradually
- Specific care (Ines helps)
- Time-based
- Realistic

### 6.6 Crew Cold Performance

**Cold Affects All Crew:**

**Mira:**
- Adapted (Fjordlund native)
- Comfortable
- Cultural advantage
- Atmospheric

**Big Tom:**
- Hates cold
- Complains often
- Specific dialogue
- Comic relief

**Ines:**
- Worries about everyone
- Specific tonics
- Critical role
- Practical

**Father Salvio:**
- Specific prayers
- Warmth from faith?
- Cultural
- Atmospheric

**Esteban:**
- Old bones feel cold
- Specific dialogue
- Vulnerable
- Memorable

---

## 7. SPECIFIC FJORDLUND TERRAIN

### 7.1 Frozen Lakes

**Common Feature:**
- Walkable winter
- Tactical shortcuts
- Specific dangers
- Atmospheric

**Player Considerations:**
- Test ice thickness
- Specific gear
- Cultural knowledge
- Realistic

### 7.2 Frozen Rivers

**Strategic Travel:**
- Highways in winter
- Specific dangers
- Cultural knowledge
- Atmospheric

### 7.3 Glaciers

**Massive Ice:**
- Specific terrain
- Crevasses dangerous
- Visual spectacular
- Memorable

**Crevasses:**
- Hidden by snow
- Falling possible
- Specific gameplay
- Memorable danger

### 7.4 Snow Mountains

**Steep Terrain:**
- Avalanche risk
- Specific gameplay
- Memorable
- Cultural

### 7.5 Ice Caves

**Specific Locations:**
- Beautiful interior
- Atmospheric
- Specific gameplay
- Memorable

**Inside:**
- Slightly warmer
- Visual spectacle
- Specific dangers
- Story potential

### 7.6 Frozen Coast

**Where Sea Meets Cold:**
- Specific landscape
- Memorable beauty
- Specific gameplay
- Atmospheric

---

## 8. AVALANCHE PHYSICS

### 8.1 Avalanche Triggers

**Natural:**
- Heavy snow load
- Sudden warming
- Wind loading
- Specific timing

**Player-Triggered:**
- Loud noises (sometimes)
- Disturbing snow
- Specific movements
- Realistic

**Combat-Triggered:**
- Cannons in mountains
- Massive impacts
- Specific scenarios
- Tactical

### 8.2 Avalanche Behavior

**Realistic:**
- Massive snow movement
- Specific direction
- Devastating speed
- Crisis

**Speed:**
- 30-100 km/h typical
- Catches anything
- Realistic
- Demanding

### 8.3 Avalanche Survival

**If Caught:**
- Specific gameplay
- Swimming motion (cultural)
- Air pocket efforts
- Crisis mode

**Death Possible:**
- Realistic
- Specific scenarios
- Memorable cautionary

**Rescue:**
- Crew search
- Specific gameplay
- Tense scenes
- Heroic

### 8.4 Avoidance

**Reading Slopes:**
- Cultural knowledge
- Specific gameplay
- Skill-based
- Realistic

**Specific Paths:**
- Safer routes
- Cultural knowledge
- Strategic
- Practical

---

## 9. ICE FISHING

### 9.1 The Activity

**Cultural:**
- Fjordlund tradition
- Specific gameplay
- Cultural depth
- Atmospheric

### 9.2 Mechanics

**Drilling Hole:**
- Specific tool
- Through thick ice
- Specific gameplay
- Realistic

**Fishing:**
- Specific lines
- Cultural variations
- Specific gameplay
- Atmospheric

**Catches:**
- Cross-reference fauna/sea_animals.md
- Cold-water species
- Cultural eating
- Practical

### 9.3 Ice Fishing Quests

**Cultural:**
- Mira teaches
- Mormor Astrid involves
- Specific quests
- Memorable

---

## 10. SHIP IN ICE WATERS

### 10.1 Sailing in Cold Seas

**Cross-Reference:** physics/ship_physics.md

**Cold-Specific:**
- Ice on rigging
- Hull damage from ice
- Specific gameplay
- Demanding

### 10.2 Iceberg Encounters

**Specific Hazard:**
- Visible above water
- Most below
- Specific dangers
- Memorable

**Damage:**
- Significant
- Often catastrophic
- Real consequence
- Memorable

### 10.3 Frozen Harbors

**Ports Inaccessible:**
- Winter complications
- Specific gameplay
- Strategic
- Realistic

### 10.4 Polar Vortex Sailing

**Most Dangerous:**
- Specific scenarios
- Crew danger
- Memorable
- Realistic

---

## 11. CULTURAL COLD KNOWLEDGE

### 11.1 Mormor Astrid

**Master of Cold:**
- Generations of knowledge
- Specific teachings
- Cultural treasure
- Critical NPC

### 11.2 Mira's Heritage

**Born to Cold:**
- Cultural advantage
- Specific dialogue
- Cultural depth
- Atmospheric

### 11.3 Fjordlund Settlements

**Architectural Adaptation:**
- Specific buildings
- Cultural design
- Realistic
- Atmospheric

### 11.4 Fire Reverence

**Cultural:**
- Fire as life
- Specific rituals
- Cultural depth
- Memorable

### 11.5 Cold Cultural Foods

**Cross-Reference:** fauna/processing_and_cooking.md

**Cold-Specific:**
- Hot mead
- Specific stews
- Warming foods
- Cultural

---

## 12. NORTHERN LIGHTS

### 12.1 Aurora Borealis

**Cross-Reference:** physics/sun_shadow_systems.md Section 6.6

**Atmospheric:**
- Dancing lights
- Beautiful
- Cultural significance
- Memorable

### 12.2 Cultural Significance

**Spiritual:**
- Mormor Astrid teaches
- Cultural depth
- Specific dialogue
- Atmospheric

### 12.3 Game Effects

**Specific:**
- Ghost Reindeer trigger (fauna)
- Cultural ceremonies
- Memorable scenes
- Atmospheric

---

## 13. ENVIRONMENTAL STORYTELLING

### 13.1 What Snow Tells

**Reading the World:**

**Pristine Snow:**
- Untouched
- Peaceful
- Beautiful
- Standard

**Tracked Snow:**
- Recent activity
- Story to tell
- Atmospheric
- Tactical

**Disturbed Snow:**
- Specific events
- Combat
- Discovery
- Atmospheric

**Bloody Snow:**
- Death recently
- Specific stories
- Atmospheric
- Memorable

### 13.2 Specific Scenes

**Story-Marked Snow:**

**Crime Scenes:**
- Bodies in snow
- Specific atmosphere
- Investigation
- Memorable

**Hunt Scenes:**
- Animal kills
- Predator-prey stories
- Atmospheric
- Realistic

**Combat Aftermath:**
- Battle in snow
- Specific atmosphere
- Long shadow
- Memorable

### 13.3 Cultural Snow Scenes

**Specific Moments:**

**Solstice Celebration:**
- Specific scenes
- Cultural
- Atmospheric
- Memorable

**Funeral Pyre:**
- Cultural
- Style D
- Memorable
- Cinematic

---

## 14. PS5 DUALSENSE INTEGRATION

### 14.1 Cold Feedback

**Subtle:**
- Cold sensation
- Specific feedback
- Atmospheric
- Immersive

### 14.2 Snow Walking

**Crunch Feedback:**
- Each step felt
- Surface-specific
- Atmospheric
- Realistic

### 14.3 Ice Slipping

**Specific Feedback:**
- Loss of control felt
- Tactical
- Atmospheric
- Memorable

### 14.4 Ice Breaking

**Critical Moment:**
- Specific feedback
- Crisis felt
- Memorable
- Cinematic

### 14.5 Adaptive Triggers

**Cold:**
- Stiffer in cold
- Specific feedback
- Atmospheric
- Realistic

**Climbing on Ice:**
- Specific tension
- Realistic
- Demanding

---

## 15. ACCESSIBILITY

### 15.1 Cold Damage

**Adjustable:**
- Realistic
- Forgiving
- Casual
- Player choice

### 15.2 Slipping

**Settings:**
- Realistic
- Reduced
- None
- Inclusive

### 15.3 Visibility

**Snowstorm Adjustable:**
- Realistic
- Enhanced
- Standard
- Player friendly

### 15.4 Tracking

**Difficulty Settings:**
- Realistic
- Enhanced (highlighted)
- Auto-detected
- Player choice

---

## 16. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** Fjordlund speaks of cold in cultural Middle. Outsiders complain in High. Sailors in Low. Cultural variation.
- ✅ **Pillar 2 (Lore Through Objects):** Specific cold gear reveals culture. Worn boots show miles. Cultural items tell stories.
- ✅ **Pillar 3 (Layered Death):** Cold deaths Style D atmospheric. Frozen bodies. Memorial in snow. Reverent.
- ✅ **Pillar 4 (Layered Humor):** Style C quirks (Big Tom hating cold, comedy in shivering, real reactions).
- ✅ **Pillar 5 (Theme — Revenge):** Cold reflects path. Frozen heart for vengeance. Warmth shared for peace. Atmospheric mirror.

---

## 17. NEXT STEPS

Next document: **`physics/lava_heat_physics.md`** — Environmental Tier Document 2

Will cover Ashenmoor-specific physics:
- Lava flow mechanics
- Heat damage system
- Volcanic eruption events
- Ash effects
- Heat-specific gameplay

After Lava/Heat: Sand/Desert (Ashenmoor), Vegetation (Rexa).

---

## 18. CONCLUSION

Fjordlund. The frozen north. Where cold is a constant character. Where snow records every footstep. Where ice gleams beautifully and kills relentlessly. Where Mira walks confident in her birthplace and outsiders learn or die.

When players hear the crunch of snow underfoot, when they watch their breath cloud in cold air, when they test ice thickness before crossing, when they survive a blizzard with frostbitten fingers and earn Mormor Astrid's approval — that's when Fjordlund becomes more than setting. That's when it becomes experience.

**Phase 6 Subsystem 8 of 15 — complete.**

**The snow falls. The ice gleams. The cold demands respect. Fjordlund lives.** ❄️🏔️
