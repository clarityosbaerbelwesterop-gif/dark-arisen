# CLOTH & HAIR PHYSICS — THE LIVING SURFACE

**Status:** Design Draft v1.0  
**Phase:** Phase 6 — Physics (Subsystem 14 of 15 — Special Systems Tier)  
**Tier:** Special Systems Tier  
**Related:** physics_overview.md, physics/wind_systems.md, physics/water_physics.md, physics/ship_physics.md, characters/all_npcs, style_bible.md  
**Purpose:** Define the complete cloth and hair physics system — sail dynamics, clothing realism, hair movement, flag physics, cape and cloak behavior, and the atmospheric polish that makes characters and environments feel alive.  
**Inspirations:** Red Dead Redemption 2 (cloth realism), Final Fantasy XV (hair physics), Assassin's Creed series (cape dynamics), Sea of Thieves (sail physics), Ghost of Tsushima (wind through fabric), real-world textile science

---

## 1. CLOTH & HAIR PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Everything Moves

Nothing should look static when wind exists. Hair flows. Cloaks billow. Sails fill. Flags wave. Even subtle ambient air creates motion. The world breathes through its surfaces.

**Design intent:** Atmospheric authenticity through reactive surfaces.

### 1.2 Principle 2: Material Tells Truth

Linen moves differently than silk. Wool drapes differently than leather. Each material has specific physical properties that make characters and environments feel real.

**Design intent:** Material authenticity through realistic physics behavior.

### 1.3 Principle 3: Cultural Clothing Identity

Fjordlund furs feel cold-adapted. Rexan light fabrics feel tropical. Imperial formal feels structured. Ashenmoor nomadic feels protective. Cultures express through what they wear and how it moves.

**Design intent:** Cultural authenticity through clothing physics.

### 1.4 Principle 4: Hair Reveals Character

Mira's tight braid reveals discipline. Big Tom's wild beard reveals personality. Father Salvio's neat tonsure reveals devotion. Hair is character made visible.

**Design intent:** Character expression through hair design and physics.

### 1.5 Principle 5: Sails Are Lifeblood

In a pirate game, sails matter more than fashion. Sail physics is critical gameplay AND atmospheric beauty. They must work mechanically AND look gorgeous.

**Design intent:** Functional beauty through sail-specific attention.

---

## 2. CLOTH PHYSICS FUNDAMENTALS

### 2.1 Cloth Properties

**Universal Properties:**

**Mass:**
- Heavier cloth moves slower
- Lighter cloth more reactive
- Specific physics
- Realistic

**Stiffness:**
- Stiff fabric holds shape
- Soft fabric drapes
- Specific behavior
- Material-dependent

**Damping:**
- How quickly motion settles
- Specific physics
- Realistic
- Atmospheric

**Tear Resistance:**
- Damage potential
- Specific scenarios
- Realistic
- Tactical

### 2.2 Wind Reactivity

**Cross-Reference:** physics/wind_systems.md

**Cloth Response to Wind:**

**Calm Air:**
- Slight ambient movement
- Subtle
- Atmospheric
- Default

**Light Wind:**
- Visible flutter
- Direction visible
- Atmospheric
- Beautiful

**Moderate Wind:**
- Active movement
- Specific physics
- Atmospheric
- Realistic

**Strong Wind:**
- Major motion
- Specific physics
- Demanding
- Memorable

**Storm:**
- Whipping motion
- Tearing risk
- Crisis
- Memorable

### 2.3 Material Categories

**Light Fabrics:**

**Silk:**
- Smooth, flowing
- Imperial luxury
- Specific physics
- Beautiful

**Cotton:**
- Standard fabric
- Wide use
- Specific physics
- Practical

**Linen:**
- Slight stiffness
- Cool wear
- Specific physics
- Common

### 2.4 Medium Fabrics

**Wool:**

**Specific:**
- Heavy drape
- Warm wear
- Cultural Fjordlund
- Realistic

**Canvas:**

**Specific:**
- Stiff
- Sail material
- Specific physics
- Practical

**Sailcloth:**

**Specific:**
- Heavy duty
- Wind-catching
- Specific physics
- Critical

### 2.5 Heavy Fabrics

**Leather:**

**Specific:**
- Stiff
- Limited motion
- Cultural use
- Realistic

**Fur:**

**Specific:**
- Heavy
- Cold-adapted
- Cultural Fjordlund
- Memorable

**Velvet:**

**Specific:**
- Luxury
- Specific drape
- Imperial
- Atmospheric

### 2.6 Cloth Damage

**Tearing:**

**Specific:**
- Wind in storms
- Combat damage
- Specific physics
- Realistic

**Burning:**

**Cross-Reference:** physics/fire_physics.md

**Specific:**
- Quick ignition
- Specific physics
- Tactical
- Memorable

**Wear Over Time:**

**Specific:**
- Slow degradation
- Cultural items
- Realistic
- Atmospheric

### 2.7 Wet Cloth

**Cross-Reference:** physics/water_physics.md, physics/weather_physics.md

**Specific Effects:**

**Heavier:**
- Slower motion
- Specific physics
- Realistic
- Atmospheric

**Darker:**
- Visual change
- Atmospheric
- Realistic
- Memorable

**Cold:**
- Cross-reference physics/snow_ice_physics.md
- Specific danger
- Realistic
- Demanding

---

## 3. SAIL DYNAMICS

### 3.1 Sail Physics Detail

**Cross-Reference:** physics/wind_systems.md Section 4, physics/ship_physics.md Section 4

**Realistic Behavior:**

**Slack Sail:**
- Hangs loose
- No power
- Specific visual
- Standard

**Filling:**
- Wind catching
- Specific motion
- Beautiful
- Tactical

**Full Power:**
- Maximum curve
- Strain visible
- Specific physics
- Memorable

**Over-Stressed:**
- Tearing risk
- Specific physics
- Crisis
- Tactical

### 3.2 Sail Types

**Square Sails:**

**Specific:**
- Standard ship sails
- Wind from behind ideal
- Specific physics
- Cultural

**Triangle Sails (Lateen):**

**Specific:**
- Versatile
- Different angles
- Cultural variations
- Memorable

**Specific Configurations:**

**Cross-Reference:** mechanics/ship_navigation.md

**Specific:**
- Various combinations
- Strategic
- Cultural
- Memorable

### 3.3 Sail Damage

**Cannon Hits:**

**Specific:**
- Holes in sails
- Performance reduced
- Specific physics
- Memorable

**Storm Damage:**

**Specific:**
- Tearing
- Mast strain
- Cross-reference physics/ship_physics.md
- Crisis

**Chain Shot:**

**Cross-Reference:** physics/ship_physics.md Section 5.4

**Specific:**
- Specifically anti-sail
- Tactical
- Memorable
- Realistic

### 3.4 Sail Repair

**Cross-Reference:** physics/ship_physics.md, mechanics/ship_management.md

**Specific:**
- Crew effort
- Time-based
- Cultural
- Practical

### 3.5 Specific Sail Moments

**The Full Sail:**

**Cinematic:**
- Beautiful
- Atmospheric
- Memorable
- Iconic

**The Storm Sail:**

**Crisis:**
- Reduced canvas
- Specific gameplay
- Memorable
- Tactical

**The Damaged Sail:**

**Atmospheric:**
- Tells story
- Persistent
- Memorable
- Realistic

---

## 4. FLAG AND BANNER PHYSICS

### 4.1 Flag Reactivity

**Wind Tells Through Flags:**

**Calm:**
- Drooping
- Information visible
- Standard
- Atmospheric

**Light Wind:**
- Slight ripple
- Direction visible
- Standard
- Atmospheric

**Moderate:**
- Extended
- Strength visible
- Tactical
- Memorable

**Strong:**
- Whipping
- Loud
- Atmospheric
- Memorable

**Storm:**
- Tearing risk
- Crisis
- Memorable
- Cinematic

### 4.2 Player Flag Identity

**Jake's Flag:**

**Specific Design:**
- Personal symbol
- Cultural
- Cross-reference mechanics/ship_management.md
- Memorable

**Flying Proudly:**
- Atmospheric
- Cultural significance
- Memorable
- Iconic

### 4.3 Cultural Flags

**Specific Per Region:**

**Imperial:**
- Specific designs
- Cultural
- Atmospheric
- Memorable

**Pirate Havens:**
- Specific designs
- Cultural
- Atmospheric
- Memorable

**Indigenous:**
- Specific designs
- Cultural depth
- Memorable
- Atmospheric

### 4.4 Strategic Flag Use

**Showing Allegiance:**

**Specific:**
- Strategic
- Cultural
- Memorable
- Tactical

**False Flags:**

**Specific:**
- Pirate tactic
- Strategic
- Memorable
- Cultural

---

## 5. CLOTHING PHYSICS

### 5.1 Player Clothing

**Jake's Outfits:**

**Cultural:**
- Multiple options
- Cross-reference mechanics/inventory_crafting.md
- Cultural depth
- Memorable

**Physics Per Item:**

**Coats:**
- Specific drape
- Wind-reactive
- Specific physics
- Atmospheric

**Capes:**
- Maximum dramatic
- Cinematic
- Specific physics
- Iconic

**Hats:**
- Wind-affected
- Possibly lost
- Specific physics
- Realistic

**Scarves:**
- Highly reactive
- Atmospheric
- Specific physics
- Beautiful

### 5.2 Damage to Clothing

**Combat:**

**Specific:**
- Cuts visible
- Persistent
- Atmospheric
- Realistic

**Wear:**

**Specific:**
- Time-based
- Cultural
- Atmospheric
- Realistic

**Repair:**

**Cross-Reference:** mechanics/inventory_crafting.md

**Specific:**
- Cultural significance
- Practical
- Atmospheric
- Realistic

### 5.3 Cultural Clothing

**Imperial Formal:**

**Specific:**
- Structured
- Status-based
- Specific physics
- Atmospheric

**Pirate Practical:**

**Specific:**
- Functional
- Cultural
- Specific physics
- Memorable

**Indigenous Rexan:**

**Specific:**
- Tropical adapted
- Cultural depth
- Specific physics
- Memorable

**Fjordlund Cold:**

**Specific:**
- Heavy fur
- Cold-adapted
- Specific physics
- Cultural

**Ashenmoor Nomadic:**

**Specific:**
- Sun-protective
- Cultural
- Specific physics
- Memorable

**Cross-Reference:** physics/lava_heat_physics.md, physics/snow_ice_physics.md, physics/sand_desert_physics.md

### 5.4 Crew Clothing

**Specific Per Crew Member:**

**Mira:**
- Practical sailor
- Specific to character
- Cross-reference npcs/named_crew_deep_dives.md
- Memorable

**Big Tom:**
- Heavy work clothes
- Specific physics
- Cultural
- Memorable

**Ines:**
- Layered for travel
- Specific physics
- Cultural
- Atmospheric

**Father Salvio:**
- Religious robes
- Specific physics
- Cultural depth
- Memorable

**Esteban:**
- Aged sailor
- Specific physics
- Cultural
- Memorable

### 5.5 NPC Clothing

**Cross-Reference:** npcs/regional_populations.md

**Specific:**
- Cultural variety
- Specific physics
- Atmospheric
- Realistic

---

## 6. HAIR PHYSICS

### 6.1 Hair Properties

**Length Categories:**

**Short:**
- Minimal motion
- Specific physics
- Standard
- Common

**Medium:**
- Moderate motion
- Specific physics
- Common
- Realistic

**Long:**
- Major motion
- Wind-reactive
- Specific physics
- Beautiful

**Very Long:**
- Maximum drama
- Specific physics
- Cinematic
- Memorable

### 6.2 Hair Styles

**Loose:**

**Specific:**
- Maximum motion
- Wind-reactive
- Beautiful
- Atmospheric

**Tied:**

**Specific:**
- Restrained motion
- Specific physics
- Cultural
- Practical

**Braided:**

**Specific:**
- Specific motion
- Cultural significance
- Atmospheric
- Memorable

**Hidden (Hat):**

**Specific:**
- Limited motion
- Cultural
- Practical
- Standard

### 6.3 Material Properties

**Hair Like Cloth:**

**Wind Reactivity:**
- Cross-reference Section 2.2
- Specific physics
- Atmospheric
- Beautiful

**Movement Damping:**
- Specific physics
- Realistic
- Atmospheric

### 6.4 Player Hair

**Jake's Hair:**

**Specific:**
- Customizable
- Cross-reference character creation
- Cultural
- Memorable

**Reactive:**
- Wind affects
- Cross-reference physics/wind_systems.md
- Atmospheric
- Beautiful

### 6.5 Crew Hair

**Specific Per Crew Member:**

**Mira:**
- Tight braid (discipline)
- Cross-reference npcs/named_crew_deep_dives.md Section 2.1
- Cultural
- Memorable

**Big Tom:**
- Bald head, full beard
- Cross-reference npcs/named_crew_deep_dives.md Section 3.1
- Memorable
- Cultural

**Ines:**
- Pinned up practical
- Cross-reference npcs/named_crew_deep_dives.md Section 4.1
- Cultural
- Memorable

**Father Salvio:**
- Tonsure (religious)
- Cross-reference npcs/named_crew_deep_dives.md Section 5.1
- Cultural depth
- Memorable

**Esteban:**
- Long white, often bound
- Cross-reference npcs/named_crew_deep_dives.md Section 6.1
- Cultural
- Memorable

### 6.6 Cultural Hair Significance

**Imperial:**

**Specific:**
- Status indicators
- Specific styles
- Cultural depth
- Memorable

**Indigenous:**

**Specific:**
- Spiritual significance
- Specific styles
- Cultural depth
- Memorable

**Fjordlund:**

**Specific:**
- Cold-adapted
- Specific styles
- Cultural
- Memorable

**Pirate:**

**Specific:**
- Practical
- Sometimes wild
- Cultural
- Memorable

### 6.7 Hair Damage

**Combat:**

**Cuts:**
- Specific scenarios
- Cultural significance
- Memorable
- Atmospheric

**Burning:**

**Cross-Reference:** physics/fire_physics.md

**Specific:**
- Catastrophic
- Cultural significance
- Memorable
- Atmospheric

**Cultural Cutting:**

**Specific:**
- Mourning rituals
- Cultural depth
- Memorable
- Story-rich

---

## 7. BEARD PHYSICS

### 7.1 Beard Categories

**Full:**

**Big Tom Style:**
- Cross-reference npcs/named_crew_deep_dives.md
- Wind-reactive
- Specific physics
- Memorable

**Trim:**

**Specific:**
- Less motion
- Cultural
- Standard
- Realistic

**Goatee/Mustache:**

**Specific:**
- Specific physics
- Cultural
- Atmospheric
- Realistic

### 7.2 Beard Care

**Cultural:**

**Specific:**
- Big Tom maintains
- Cultural significance
- Atmospheric
- Memorable

### 7.3 Beard Story

**Specific:**

**Big Tom's Beard:**
- Cultural significance
- Specific care
- Memorable
- Atmospheric

**Religious Beards:**

**Father Salvio:**
- Trimmed and neat
- Cultural
- Specific physics
- Memorable

---

## 8. SHIP CLOTH ELEMENTS

### 8.1 Awnings

**Specific:**

**Visual:**
- Provide shade
- Wind-reactive
- Cultural
- Atmospheric

**Cultural Use:**
- Captain's deck
- Specific scenes
- Memorable
- Atmospheric

### 8.2 Crew Hammocks

**Specific:**

**Wind-Reactive:**
- Specific physics
- Cultural
- Atmospheric
- Realistic

### 8.3 Curtains and Drapes

**Captain's Cabin:**

**Cross-Reference:** mechanics/ship_interior.md

**Specific:**
- Specific atmosphere
- Cultural
- Memorable
- Atmospheric

### 8.4 Wet Sails Drying

**Specific:**

**Atmospheric:**
- After rain
- Crew tasks
- Specific scenes
- Realistic

---

## 9. CULTURAL CLOTHING DEEP DIVES

### 9.1 Fjordlund Furs

**Specific:**

**Layered Heavy:**
- Cold protection
- Specific physics
- Cultural
- Memorable

**Mira's Coat:**
- Cross-reference npcs/named_crew_deep_dives.md Section 2.1
- Brother's coat
- Cultural significance
- Memorable

**Mormor Astrid:**
- Cultural treasure
- Specific styles
- Cultural depth
- Memorable

### 9.2 Imperial Formal

**Specific:**

**Margarethe Fitzmueller:**
- Imperial elegance
- Specific physics
- Cultural depth
- Memorable

**Aristocratic Layers:**
- Specific drape
- Status display
- Cultural
- Memorable

### 9.3 Indigenous Rexan

**Specific:**

**Mama Jacinta:**
- Cultural depth
- Specific styles
- Memorable
- Atmospheric

**Tribal Variations:**
- Specific cultures
- Atmospheric
- Memorable
- Realistic

### 9.4 Ashenmoor Nomadic

**Specific:**

**Mbah Seruni:**
- Cultural depth
- Specific styles
- Memorable
- Atmospheric

**Sun Protection:**
- Practical
- Specific physics
- Cultural
- Realistic

### 9.5 Pirate Eclectic

**Specific:**

**Various Styles:**
- Stolen/traded
- Cultural mixing
- Specific atmosphere
- Memorable

**Big Tom's Style:**
- Practical heavy
- Cultural
- Memorable
- Specific

---

## 10. CAPE AND CLOAK PHYSICS

### 10.1 The Iconic Cape

**Visual Drama:**

**In Wind:**
- Major motion
- Cinematic
- Beautiful
- Memorable

**At Standstill:**
- Slight motion
- Atmospheric
- Specific
- Realistic

### 10.2 Hooded Cloaks

**Specific:**

**Hood Up:**
- Wind-affected
- Cultural
- Atmospheric
- Memorable

**Hood Down:**
- Standard motion
- Cultural
- Standard
- Atmospheric

### 10.3 Combat with Cape

**Tactical:**

**Whipping Motion:**
- Specific gameplay
- Tactical
- Memorable
- Cinematic

**Visual Distraction:**
- Specific tactics
- Tactical
- Memorable
- Cultural

### 10.4 Cultural Cloaks

**Specific:**

**Imperial Officer:**
- Status indicator
- Specific physics
- Cultural
- Memorable

**Religious:**
- Father Salvio's cassock
- Specific physics
- Cultural depth
- Memorable

**Pirate:**
- Practical
- Specific physics
- Cultural
- Memorable

---

## 11. FABRIC IN ENVIRONMENT

### 11.1 Tents

**Cross-Reference:** physics/sand_desert_physics.md

**Specific:**

**Wind-Reactive:**
- Specific physics
- Cultural
- Atmospheric
- Realistic

**Storm Damage:**
- Specific scenarios
- Crisis
- Realistic
- Memorable

### 11.2 Fabric Awnings

**Specific:**

**Markets:**
- Specific atmosphere
- Cultural
- Atmospheric
- Memorable

**Wind Effects:**
- Specific physics
- Atmospheric
- Realistic

### 11.3 Hanging Cloth

**Specific:**

**Drying Laundry:**
- Atmospheric
- Cultural
- Realistic
- Memorable

**Curtains:**
- Cultural
- Specific atmosphere
- Memorable
- Realistic

### 11.4 Banners and Tapestries

**Specific:**

**Imperial:**
- Cultural
- Status display
- Specific physics
- Memorable

**Religious:**
- Father Salvio's spaces
- Cultural depth
- Specific atmosphere
- Memorable

---

## 12. HAIR IN ENVIRONMENT

### 12.1 Crowd Hair

**Specific:**

**NPCs:**
- Cultural variation
- Specific physics
- Atmospheric
- Realistic

**Quality vs Performance:**
- Distance LOD
- Specific optimization
- Realistic
- Practical

### 12.2 Animal Fur/Hair

**Cross-Reference:** fauna documents

**Specific:**
- Wind-reactive
- Specific physics
- Atmospheric
- Realistic

---

## 13. SPECIFIC ATMOSPHERIC MOMENTS

### 13.1 The Cape in Wind

**Cinematic:**

**Specific Scenes:**
- Standing on bow
- Specific atmosphere
- Iconic
- Memorable

### 13.2 The Sail Filling

**Beautiful:**

**Specific Scenes:**
- Departure
- Specific atmosphere
- Iconic
- Memorable

### 13.3 The Hair in Storm

**Dramatic:**

**Specific Scenes:**
- Crisis moments
- Specific atmosphere
- Cinematic
- Memorable

### 13.4 The Flag Tearing

**Tragic:**

**Specific Scenes:**
- Storm damage
- Cultural significance
- Memorable
- Atmospheric

### 13.5 The Quiet Drape

**Peaceful:**

**Specific Scenes:**
- Cabin moments
- Specific atmosphere
- Memorable
- Beautiful

---

## 14. PS5 DUALSENSE INTEGRATION

### 14.1 Cape Feedback

**Specific:**

**In Wind:**
- Subtle feedback
- Atmospheric
- Realistic
- Immersive

### 14.2 Sail Handling

**Specific:**

**Adjusting Sails:**
- Specific tension
- Realistic
- Demanding
- Atmospheric

### 14.3 Hair Wind

**Subtle:**

**Specific:**
- Atmospheric only
- Realistic
- Standard

### 14.4 Adaptive Triggers

**Cloth Pulling:**
- Specific tension
- Realistic
- Demanding
- Atmospheric

---

## 15. PERFORMANCE OPTIMIZATION

### 15.1 LOD for Cloth

**Distance-Based:**

**Far:**
- Simplified physics
- Performance friendly
- Standard

**Medium:**
- Standard physics
- Realistic
- Common

**Near:**
- Maximum detail
- Beautiful
- Memorable

### 15.2 Hair LOD

**Specific:**

**Far NPCs:**
- Reduced physics
- Performance
- Standard

**Player/Major NPCs:**
- Full physics
- Beautiful
- Memorable

### 15.3 Sail Optimization

**Specific:**

**Active Ship:**
- Full physics
- Critical
- Beautiful

**Distant Ships:**
- Simplified
- Performance
- Standard

---

## 16. ACCESSIBILITY

### 16.1 Visual Effects

**Adjustable:**
- Cloth motion intensity
- Hair physics quality
- Player choice
- Inclusive

### 16.2 Performance

**Settings:**
- High quality
- Standard
- Performance mode
- Player choice

### 16.3 Motion Sensitivity

**Considerations:**
- Reduced motion option
- Player friendly
- Standard
- Inclusive

---

## 17. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** NPCs comment on clothing in cultural register. Aristocrats notice fashion in High. Sailors in Low. Cultural variation.
- ✅ **Pillar 2 (Lore Through Objects):** Specific clothing items reveal culture. Mira's brother's coat tells story. Cultural items carry weight.
- ✅ **Pillar 3 (Layered Death):** Funeral cloths Style D reverent. Burned clothing atmospheric. Cultural significance.
- ✅ **Pillar 4 (Layered Humor):** Style C quirks (Big Tom's beard care, Father Salvio's robes catching wind, real moments).
- ✅ **Pillar 5 (Theme — Revenge):** Clothing reflects path. Bloody for vengeance. Pristine for peace. Atmospheric mirror.

---

## 18. NEXT STEPS

Next document: **`physics/sound_physics.md`** — Special Systems Tier Document 4 (FINAL Phase 6 Document)

Will cover:
- Sound propagation
- Distance attenuation
- Echo and reflection
- Wind affects sound
- Underwater audio
- Cultural sound

After Sound Physics: **PHASE 6 COMPLETE.**

---

## 19. CONCLUSION

Cloth and hair. The atmospheric polish that makes characters feel alive. Mira's tight braid that never moves out of place. Big Tom's wild beard catching wind. Father Salvio's tonsure showing devotion. Esteban's long white hair binding his stories. Sails filling with wind and pulling La Liberación through the waves. Capes billowing dramatically as Jake stands on the bow. Flags tearing in storms. Banners proud at festivals.

When players see Mira's braid stay perfectly disciplined while her cloak whips in the wind, when they watch a sail fill from slack to full power, when their cape billows as they jump from rigging — that's when cloth and hair transcend visual polish. That's when they become character itself, made physical.

**Phase 6 Subsystem 14 of 15 — complete.**

**The cloth flows. The hair moves. The sails fill. The world breathes through its surfaces.** 👘
