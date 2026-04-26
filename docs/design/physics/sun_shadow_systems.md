# SUN & SHADOW SYSTEMS — THE LIGHT THAT REVEALS

**Status:** Design Draft v1.0  
**Phase:** Phase 6 — Physics (Subsystem 3 of 15 — Foundation Tier)  
**Tier:** Foundation Tier  
**Related:** physics_overview.md, physics/water_physics.md, physics/wind_systems.md, mechanics/stealth.md, style_bible.md  
**Purpose:** Define the complete sun and shadow physics system — day-night cycle, dynamic sun position, realistic shadow casting, moon phases, eclipse events, and the cultural relationships with light.  
**Inspirations:** Red Dead Redemption 2 (cinematic lighting), The Legend of Zelda: Breath of the Wild (dynamic time), Death Stranding (atmospheric light), Ghost of Tsushima (golden hour beauty), real-world astronomy and ephemeris data

---

## 1. SUN & SHADOW PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Light Has Direction

The sun is not a vague "sky color." It has a specific position in the sky at every moment, casting specific shadows in specific directions. Players can read time of day by looking at any shadow.

**Design intent:** Atmospheric realism through physically accurate light.

### 1.2 Principle 2: Shadow Is Gameplay

Shadows are not decoration. They are stealth cover. They are time-tellers. They are atmospheric storytellers. They are tactical considerations. Players who learn to read shadows play better.

**Design intent:** Light and dark create gameplay opportunity beyond visual quality.

### 1.3 Principle 3: Time Has Mood

Dawn light differs from noon light differs from evening light differs from night. Each time of day has its own emotional character. The same place feels different at different hours.

**Design intent:** Time as emotional resource. Players choose when to do things partially based on mood.

### 1.4 Principle 4: The Moon Is the Sun's Echo

When the sun sets, the moon rises (sometimes). Lunar light has its own character — silver, soft, less directional. Some things only happen by moonlight. Phase matters.

**Design intent:** Night isn't just darkness. It's its own atmosphere with rules.

### 1.5 Principle 5: Cultures Read Light Differently

Imperial Christians follow sun for prayer times. Indigenous Rexan greet sunrise. Fjordlund celebrates solstices. Each culture's relationship with light reveals something about them.

**Design intent:** Light as cultural touchpoint, not just physics.

---

## 2. THE DAY-NIGHT CYCLE

### 2.1 Game Time vs Real Time

**Time Compression:**
- 1 in-game hour = approximately 2-3 real-time minutes
- Full 24-hour cycle = ~50-70 real-time minutes
- Standard for open-world games
- Allows full day experience

**Adjustable:**
- Player can choose pace
- Slower for atmosphere
- Faster for objectives
- Default balanced

### 2.2 The 24-Hour Structure

**Pre-Dawn (3-5 AM):**
- Deep darkness
- Stars maximum
- World quiet
- Specific NPCs awake (bakers, fishermen)
- Atmospheric mystery

**Dawn (5-6 AM):**
- First light
- Pink-purple sky
- Stars fading
- Birds beginning
- Magic hour

**Sunrise (6-7 AM):**
- Sun visible at horizon
- Golden light
- Long shadows
- World waking
- Beautiful

**Morning (7-11 AM):**
- Sun climbing
- Shadows shortening
- Active hours
- Productive feeling
- Bright

**Midday (11-1 PM):**
- Sun at peak
- Shortest shadows
- Hottest hour
- Some rest
- White light

**Afternoon (1-5 PM):**
- Sun descending
- Shadows lengthening
- Continued activity
- Warming light
- Variable

**Sunset (5-7 PM):**
- Sun at horizon
- Golden hour peaks
- Most beautiful
- Romantic atmosphere
- Cinematic

**Twilight (7-8 PM):**
- Sun below horizon
- Sky still glowing
- Brief beauty
- Transition

**Evening (8-10 PM):**
- Dark but visible
- Stars emerging
- Settling time
- Domestic warmth
- Lamps lit

**Night (10 PM-3 AM):**
- Full dark (or moonlit)
- Different world
- Specific NPCs (guards, sailors)
- Stealth gameplay prime
- Mysterious

### 2.3 Daylight Variation by Season

**Summer:**
- Long days (5 AM - 9 PM)
- Brief nights
- Warmer feel
- Active times extended

**Winter:**
- Short days (8 AM - 4 PM)
- Long nights
- Colder feel
- Indoor times longer

**Equinox (Spring/Autumn):**
- Equal day and night
- Balance feeling
- Cultural significance
- Ceremonial moments

### 2.4 Regional Daylight Variation

**Tropical (Rexa/Moran):**
- Consistent year-round
- About 12 hours daylight
- Sudden sunset (no twilight)
- Tropical specific

**Temperate (Quiet Coast, Pale Isle):**
- Standard variation
- 4-season changes
- Familiar pattern
- Comfortable

**Arctic (Fjordlund):**
- Extreme variation
- Summer: Sun barely sets
- Winter: Brief daylight only
- White nights phenomenon
- Cultural significance

**Desert (Ashenmoor):**
- Brutal noon sun
- Dramatic shadows
- Significant temp variation
- Specific gameplay

**Region 06:**
- Time questionable
- Sun behaves strangely
- Reality bends
- Symbolic light

---

## 3. SUN POSITION

### 3.1 Realistic Solar Path

**The Sun:**
- Rises in east
- Peaks at south (northern hemisphere)
- Sets in west
- Path varies by latitude

**Time-Based Position:**
- Calculated dynamically
- Realistic angles
- Specific to date and location
- Players can read time

### 3.2 Sun Visibility

**Direct Sun:**
- Visible disc in sky
- Don't look directly (eye damage atmospheric)
- Specific glare effects
- Realistic photography

**Glare Effects:**
- Lens flare-style (cinematic)
- Specific angles intense
- Visual storytelling
- Atmospheric beauty

### 3.3 Cloud Cover Affects Sun

**Visibility Variations:**

**Clear Sky:**
- Maximum sun
- Strongest shadows
- Brightest light
- Dramatic

**Partly Cloudy:**
- Sun in/out of clouds
- Variable lighting
- Beautiful chiaroscuro
- Dynamic

**Overcast:**
- Sun hidden
- Diffused light
- Soft shadows
- Different mood

**Storm Cover:**
- Dark, dramatic
- Sun nearly invisible
- Crisis lighting
- Memorable

### 3.4 Atmospheric Phenomena

**Sunbeams:**
- Through clouds
- Atmospheric rays
- Beautiful effect
- "God rays"

**Halos:**
- Around sun in specific conditions
- Ice crystals
- Rare beauty
- Cultural omens

**Sunsets:**
- Color spectacular
- Specific physics
- Realistic colors
- Cinematic possibility

**Sundogs:**
- Bright spots at sides of sun
- Atmospheric phenomenon
- Rare
- Cultural significance

### 3.5 Sun in Different Climates

**Tropical:**
- High overhead
- Strong direct light
- Brief twilight
- Vibrant colors

**Temperate:**
- Variable angle
- Seasonal changes
- Moderate intensity
- Familiar

**Arctic:**
- Low angle even noon
- Long shadows always
- Polar phenomena
- Unique character

**Desert:**
- Brutal directness
- Heat shimmer visible
- Long shadows dawn/dusk
- Dramatic

---

## 4. SHADOW PHYSICS

### 4.1 Realistic Shadow Casting

**Every Object Casts Shadow:**
- Direction from sun position
- Length based on sun height
- Sharpness based on conditions
- Realistic always

### 4.2 Shadow Length

**Time-Based:**

**Dawn:**
- Very long shadows
- Pointing west
- Atmospheric beauty
- Cinematic

**Morning:**
- Moderate length
- Pointing west-northwest
- Active feel

**Midday:**
- Shortest shadows
- Almost directly below
- Bright
- Sometimes harsh

**Afternoon:**
- Lengthening
- Pointing east-northeast
- Warming feeling

**Sunset:**
- Very long
- Pointing east
- Dramatic
- Beautiful

**Player Shadow:**
- Jake casts realistic shadow
- Can tell time by it
- Visible to NPCs (stealth)
- Atmospheric self-awareness

### 4.3 Shadow Sharpness

**Direct Sunlight:**
- Sharp, hard shadows
- Clear edges
- Strong contrast
- Definite

**Cloud Cover:**
- Soft shadows
- Diffused edges
- Gentle contrast
- Mood

**No Direct Sun:**
- Ambient shadows only
- Very soft
- Minimal contrast
- Low light

### 4.4 Multiple Light Sources

**Sun + Other:**

**Sun + Fire:**
- Two shadow sources
- Specific angles
- Atmospheric

**Sun + Reflection:**
- Water bouncing light
- Subtle shadows
- Realistic

**Indoor Lighting:**
- Lamps create shadows
- Multiple sources
- Complex casting
- Beautiful

### 4.5 Shadow Behavior

**Static Objects:**
- Shadows track with sun
- Watch them move
- Time visible

**Moving Objects:**
- Shadows update
- Walking creates moving shadow
- Realistic

**Particles:**
- Dust shadows
- Snow shadows
- Smoke shadows
- Atmospheric

---

## 5. SHADOW AS GAMEPLAY

### 5.1 Stealth Mechanics

**Shadow Provides Cover:**
- Detection reduced in shadow
- Movement easier
- Strategic depth
- Realistic

**Shadow Patches:**
- Dynamic stealth opportunities
- Shadow moves through day
- Tactical knowledge
- Skill rewarded

### 5.2 Time Tracking

**Players Read Time:**
- Shadow length and direction
- No need for UI clock
- Realistic competence
- Skill development

### 5.3 Sundials

**In-Game Sundials:**
- Functional time-tellers
- Specific locations
- Cultural artifact
- Practical use

### 5.4 Shadow Puzzles

**Specific Quests:**
- Time-based puzzles
- Shadow alignment
- Cultural significance
- Memorable content

**Examples:**
- Specific location at specific time
- Door opens only at sunset
- Quest item visible at noon only
- Atmospheric depth

### 5.5 Stealth Through Shadow

**Specific Tactics:**

**Ambush from Shadow:**
- Lie in dark spot
- Wait for target
- Strike from concealment
- Realistic

**Shadow Parkour:**
- Move through shadows
- Avoid sunlit areas
- Skill-based
- Late-game

**Roof Shadows:**
- Building shadows extend
- Specific paths possible
- Architectural awareness
- Strategic

### 5.6 Combat Considerations

**Sun in Eyes:**
- Facing sun = harder to aim
- Sun behind = enemies can't see well
- Tactical positioning
- Realistic

**Shadow Combat:**
- Hidden in shadow advantage
- Shadow attacks
- Specific tactics

---

## 6. NIGHT AND MOONLIGHT

### 6.1 Night Lighting

**Without Moon:**
- Very dark
- Stars provide minimal
- Stealth easier
- Vision limited

**With Full Moon:**
- Surprisingly bright
- Silver light
- Long shadows
- Beautiful

**Moonlight Reading:**
- Difficult but possible
- Specific scenes
- Atmospheric

### 6.2 Moon Phases

**The Lunar Cycle:**

**New Moon:**
- No moon visible
- Darkest nights
- Best stealth
- Specific atmosphere

**Waxing Crescent:**
- Slim curve
- Slight light
- Atmospheric

**First Quarter:**
- Half moon
- Moderate light
- Good night activity

**Waxing Gibbous:**
- Almost full
- Bright
- Active night

**Full Moon:**
- Maximum light
- Silver brilliance
- Cultural significance
- Ghost Reindeer (Phase 4 callback)
- Specific gameplay

**Waning Gibbous:**
- Diminishing
- Still bright
- Late-rising

**Last Quarter:**
- Half again
- Decreasing
- Late-night light

**Waning Crescent:**
- Pre-new moon
- Almost dark
- Hidden
- Specific atmospheres

### 6.3 Lunar Position

**Moon Path:**
- Rises in east
- Sets in west
- Different times each night
- Realistic

**Visibility Times:**

**Full Moon:**
- Rises at sunset
- Visible all night
- Sets at sunrise

**New Moon:**
- Rises at sunrise
- Sets at sunset
- Not visible at night

**Phases Between:**
- Various rising times
- Specific calculations
- Realistic astronomy

### 6.4 Lunar Effects on Game

**Phase 4 Callbacks:**

**Specific Plants:**
- Moon-blooming flowers
- Full moon harvest
- Specific recipes
- (See plants_and_herbs.md)

**Specific Animals:**
- Ghost Reindeer (Fjordlund Northern Lights)
- Specific creatures
- Lunar timing

**Cultural Practices:**
- Mama Jacinta's ceremonies
- Lunar timing
- Spiritual significance

**Stealth Gameplay:**
- New moon = easiest stealth
- Full moon = hardest
- Strategic timing

### 6.5 Stars

**Realistic Star Field:**
- Constellations visible
- Specific patterns
- Cultural meaning
- Navigation use

**Esteban's Teaching:**
- Star navigation
- Cultural names
- Cross-regional knowledge
- Master skill

**Specific Constellations:**
- Polaris (North Star)
- Southern Cross
- Cultural variations
- Real astronomy

### 6.6 Northern Lights (Fjordlund)

**Aurora Borealis:**
- Realistic occurrence
- Late autumn through spring
- Spectacular visual
- Cultural significance

**Triggers:**
- Specific dates
- Solar weather
- Random events

**Effects:**
- Atmospheric beauty
- Ghost Reindeer appears
- Spiritual significance
- Memorable scenes

---

## 7. CULTURAL TIME

### 7.1 Imperial Christian Time

**Daily Prayer Times:**
- Dawn (Lauds)
- Mid-morning (Terce)
- Midday (Sext)
- Mid-afternoon (None)
- Evening (Vespers)
- Compline (Bedtime)

**Cultural Effects:**
- Father Salvio observes
- Bells ring
- NPCs pause
- Atmospheric

### 7.2 Indigenous Rexan Time

**Sun Worship:**
- Sunrise greeting
- Midday meditation
- Sunset gratitude
- Cultural ceremony

**Mama Jacinta:**
- Specific rituals
- Sun-aware practice
- Cultural depth

### 7.3 Fjordlund Pagan Time

**Solstices:**
- Summer solstice celebration
- Winter solstice survival
- Cultural significance
- Memorable scenes

**Mormor Astrid:**
- Knows old ways
- Teaches Mira
- Cultural treasure

### 7.4 Sailor Time

**Watches:**
- 4-hour shifts
- Specific names
- Bells mark time
- Universal across cultures

**Sun Sights:**
- Noon position critical
- Navigation requires
- Esteban's specialty

### 7.5 Player Time

**Jake's Routine:**
- Develops over game
- Cultural learning
- Personal preference
- Becomes intuitive

---

## 8. ECLIPSE EVENTS

### 8.1 Solar Eclipse

**Rare Phenomenon:**
- 1-2 per playthrough maximum
- Specific dates calculated
- Major story potential
- Memorable

**Atmospheric Effects:**
- Day becomes twilight
- Animals confused
- Birds settle
- Eerie quiet

**Cultural Reactions:**
- Imperial Christians pray
- Indigenous Rexan ceremony
- Sailors superstitious
- Universal awe

**Story Potential:**
- Specific quest hooks
- Boss encounters
- Atmospheric peak moments
- Memorable

### 8.2 Lunar Eclipse

**More Common:**
- Several per playthrough
- Specific dates
- Visible at night
- Less dramatic

**Effects:**
- Moon turns red ("Blood Moon")
- Cultural significance
- Specific quest content
- Atmospheric

**Cultural Reactions:**
- Ill omen many cultures
- Specific NPC dialogue
- Rare reactions

### 8.3 Specific Eclipse Quests

**Solar Eclipse Boss:**
- Specific boss only fights during
- Player must time encounter
- Atmospheric challenge
- Memorable

**Lunar Eclipse Ritual:**
- Mama Jacinta's specific ceremony
- Cannabis quest variant
- Cultural depth
- Rare content

**Eclipse Discovery:**
- Specific items only visible during
- Hidden content
- Reward observation
- Easter egg quality

---

## 9. WEATHER AND LIGHT

### 9.1 Weather Affects Light

**Cross-Reference:** `physics/weather_physics.md` (forthcoming)

**Quick Reference:**
- Storms darken dramatically
- Rain dulls colors
- Snow reflects (brighter)
- Fog diffuses

### 9.2 Color Temperature

**Light Color Varies:**

**Dawn:**
- Warm pink/gold
- Long shadows
- Romantic

**Morning:**
- Cooler yellow
- Active
- Productive

**Noon:**
- White-blue
- Harsh
- Direct

**Afternoon:**
- Warming yellow
- Mellow
- Peaceful

**Sunset:**
- Orange-red
- Most dramatic
- Cinematic

**Twilight:**
- Purple-blue
- Brief beauty
- Magical

**Night:**
- Cool blue
- Silver moonlight
- Mysterious

### 9.3 Indoor vs Outdoor

**Outdoor:**
- Direct sun
- Sharp shadows
- Maximum brightness

**Indoor:**
- Diffused light
- Window beams
- Lamp lighting
- Different mood

**Cave/Underground:**
- No natural light (deep)
- Torch required
- Dramatic
- Realistic

---

## 10. SPECIFIC ENVIRONMENTAL LIGHTING

### 10.1 Forest Light

**Dappled Sunlight:**
- Through leaves
- Beautiful patterns
- Realistic
- Atmospheric

### 10.2 Underwater Light

**Refracted:**
- Wavy patterns
- Caustics on bottom
- Surface visible above
- Specific feel

### 10.3 Cave Light

**Limited:**
- Entrance light only
- Torches required deep
- Dramatic shadows
- Specific atmosphere

### 10.4 Storm Light

**Dramatic:**
- Lightning flashes
- Brief illumination
- Memorable
- Cinematic

### 10.5 Region 06 Light

**Impossible:**
- Doesn't follow rules
- Symbolic
- Atmospheric
- Otherworldly

---

## 11. GOLDEN HOURS AND BLUE HOURS

### 11.1 Golden Hour

**Sunrise/Sunset:**
- Magic light
- Photographer's dream
- Cinematic
- Romantic

**Specific Moments:**
- After sunrise
- Before sunset
- Brief
- Memorable

**Game Implementation:**
- Specific atmospheric
- NPCs respond ("Beautiful evening")
- Cultural awareness
- Beauty rewarded

### 11.2 Blue Hour

**Twilight:**
- Just after sunset
- Just before sunrise
- Deep blue sky
- Magical brief

**Cultural Significance:**
- Specific moments
- Romantic content
- Reflective time
- Quiet beauty

### 11.3 Player Awareness

**Develops Over Time:**
- Beginning: don't notice
- Middle: appreciate
- Late: actively choose times
- Skilled players use

---

## 12. SPECIFIC SCENES — LIGHT AS CHARACTER

### 12.1 Story Moments

**Specific Scenes Use Light:**

**Ethan's Grove Discovery:**
- Specific time of day
- Atmospheric
- Memorable

**Final Boss Approach:**
- Storm light typically
- Dramatic
- Cinematic

**Crew Meal:**
- Evening warm
- Atmospheric
- Domestic

**Quiet Moments:**
- Variable times
- Player choice
- Reflective

### 12.2 Region-Specific Light

**Rexa Tropical:**
- Vibrant
- Intense
- Beautiful sunsets
- Cultural

**Fjordlund Arctic:**
- Different at all times
- White nights
- Aurora
- Unique

**Ashenmoor Desert:**
- Brutal noon
- Beautiful dawn/dusk
- Stars magnificent
- Specific

**Pale Isle Imperial:**
- Civilized light
- Familiar
- Architectural
- Comfortable

### 12.3 Cultural Light Use

**Indigenous Rituals:**
- Sunrise ceremonies
- Specific lighting
- Cultural depth

**Religious Light:**
- Father Salvio's services
- Specific moods
- Cultural meaning

**Pagan Light:**
- Solstice celebrations
- Bonfires
- Cultural

---

## 13. PERFORMANCE CONSIDERATIONS

### 13.1 Shadow Rendering

**Optimization:**
- LOD for shadows
- Distant shadows simpler
- Active areas detailed
- Performance friendly

### 13.2 Light Calculation

**Smart Updates:**
- Sun position calculated periodically
- Shadows update accordingly
- Smooth transitions
- Realistic feel

### 13.3 Time of Day Transitions

**Smooth:**
- Gradual lighting changes
- No jarring shifts
- Realistic feel
- Atmospheric

---

## 14. PS5 DUALSENSE INTEGRATION

### 14.1 Light-Based Feedback

**Subtle:**
- Eye-strain feedback in bright sun
- Cool feel in shade
- Specific moments
- Atmospheric

### 14.2 Time Awareness

**Visual:**
- Controller LED matches time of day
- Subtle but present
- Atmospheric
- Optional

---

## 15. ACCESSIBILITY

### 15.1 Vision

**Adjustable:**
- Brightness controls
- Contrast options
- Sun glare reducible
- Player friendly

### 15.2 Day-Night Speed

**Player Choice:**
- Standard
- Slower (atmospheric)
- Faster (objective focus)
- Pause at preferred time

### 15.3 Color Blindness

**Specific Settings:**
- Color blind modes
- Lighting adjusted
- Visual cues alternative
- Inclusive

---

## 16. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** Imperial speak of sun in High register ("the celestial body"). Sailors in Low ("good light today"). Indigenous in spiritual Middle.
- ✅ **Pillar 2 (Lore Through Objects):** Sundials, hourglasses, calendars — all light-related items reveal cultural relationships with time.
- ✅ **Pillar 3 (Layered Death):** Death at dawn (Style D rebirth), midday death (Style B brutal), sunset death (Style D peaceful). Light affects style.
- ✅ **Pillar 4 (Layered Humor):** Style C quirks in light reactions (Big Tom hating bright morning, sailor jokes about specific times).
- ✅ **Pillar 5 (Theme — Revenge):** Light reflects path. Bright midday for clear violence. Sunset for peace approaching. Storm light for vengeance. Atmospheric mirror.

---

## 17. NEXT STEPS

Next document: **`physics/weather_physics.md`** — Foundation Tier Document 4 (FINAL Foundation)

Will cover:
- Rain mechanics in detail
- Storm systems
- Lightning and thunder
- Snow accumulation
- Heat shimmer
- Weather as gameplay

After Weather: Foundation Tier complete. Then Combat & Movement Tier.

---

## 18. CONCLUSION

Light in Dark Arisen is not a uniform brightness across the world. It is a directional force from a sun at specific position, casting specific shadows, creating specific moods. Players who learn to read light and shadow play better. They know when to attack. When to hide. When to reflect. When to act.

When players watch the sun set over the harbor and the world turns gold, when they hide in shadow as a guard walks past in sunlight, when they harvest moon-blooming flowers under full moon, when an eclipse creates a moment that will be remembered — that's when light becomes more than rendering. That's when it becomes part of the experience.

Light reveals. Shadow conceals. Time passes. The world breathes through both.

**Phase 6 Subsystem 3 of 15 — complete.**

**The sun rises. The shadow moves. The moon waits. The day passes.** ☀️🌙
