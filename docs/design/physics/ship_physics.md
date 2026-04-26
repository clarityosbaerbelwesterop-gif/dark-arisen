# SHIP PHYSICS — LA LIBERACIÓN AS LIVING VESSEL

**Status:** Design Draft v1.0  
**Phase:** Phase 6 — Physics (Subsystem 6 of 15 — Combat & Movement Tier)  
**Tier:** Combat & Movement Tier  
**Related:** physics_overview.md, physics/water_physics.md, physics/wind_systems.md, naval_combat_system.md, mechanics/ship_management.md, style_bible.md  
**Purpose:** Define the complete ship physics system — buoyancy, wave-ship interaction, naval combat physics, ship damage and sinking, cannonball trajectories, and boarding action physics. The ship as living vessel with real physical reality.  
**Inspirations:** Assassin's Creed Black Flag (naval combat physics), Sea of Thieves (ship behavior), Sid Meier's Pirates! (ship-to-ship dynamics), Naval Action (realistic naval physics), real-world Age of Sail naval engineering

---

## 1. SHIP PHYSICS PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Ships Are Vessels, Not Vehicles

A ship doesn't drive — it sails. It doesn't steer like a car — it responds to wind, current, weight, and crew. Players who treat La Liberación like a vehicle will fail. Players who treat her like a vessel succeed.

**Design intent:** Realistic ship behavior creates authentic captaincy.

### 1.2 Principle 2: Weight Matters Always

Cargo weight affects depth. Damage shifts balance. Crew positions matter. Every load consideration impacts performance. Captains must think in physical reality.

**Design intent:** Cargo decisions matter physically, not just inventory-wise.

### 1.3 Principle 3: Damage Is Persistent and Real

A cannonball hole is a real hole. Water enters. Ship lists. Pumps fight back. Damage doesn't reset between battles — it accumulates and demands attention.

**Design intent:** Ship maintenance is meaningful gameplay, not menu task.

### 1.4 Principle 4: Naval Combat Is Physical Chess

Wind direction. Wave state. Hull damage. Crew morale. Cargo weight. All combine in naval combat. Skilled captains read all factors. Unskilled captains lose.

**Design intent:** Naval combat depth through layered physical reality.

### 1.5 Principle 5: Sinking Is a Story

A ship doesn't disappear when destroyed. It sinks slowly. Crew abandons. Cargo is lost. The slow death of a vessel carries weight. Every sinking is a loss with weight.

**Design intent:** Ships have emotional value. Their loss is felt.

---

## 2. BUOYANCY SYSTEM

### 2.1 Realistic Floating

**Displacement Calculation:**
- Ship displaces water equal to its weight
- Heavier ship = deeper sit
- Lighter ship = higher float
- Realistic physics

**Visible Effects:**

**Empty Ship:**
- Sits high
- Hull above water more visible
- Rolls more easily
- Less stable

**Light Cargo:**
- Standard sit
- Standard balance
- Optimal performance
- Default state

**Heavy Cargo:**
- Sits deeper
- Less freeboard visible
- More stable in waves
- Reduced speed

**Overloaded:**
- Dangerously deep
- Water near deck
- Compromised speed
- Capsizing risk

### 2.2 Cargo Distribution

**Even Loading:**
- Ship balanced
- Sits level
- Optimal performance
- Standard

**Uneven Loading:**

**Bow Heavy:**
- Bow dips
- Slower
- Steering compromised
- Specific consequence

**Stern Heavy:**
- Stern dips
- Different handling
- Specific issues
- Realistic

**Listing (Side Heavy):**
- Visibly tilted
- Combat compromised
- Crew uncomfortable
- Realistic

### 2.3 Stability Center

**Center of Gravity:**
- Affects roll behavior
- Heavier low = stable
- Heavier high = top-heavy
- Strategic considerations

**Cargo Placement:**
- Heavy items in lower hold
- Balanced port/starboard
- Bow/stern even
- Practical knowledge

### 2.4 Damage and Buoyancy

**Hull Breach Effects:**

**Above Waterline:**
- No immediate flooding
- Damage cosmetic (mostly)
- Strategic concerns
- Repair eventual

**At Waterline:**
- Slight flooding
- Listing if not centered
- Pumps required
- Ongoing concern

**Below Waterline:**
- Major flooding
- Significant listing
- Pumps fight constantly
- Crisis

**Multiple Breaches:**
- Cumulative effects
- Sinking imminent
- Crew at pumps
- Crisis mode

### 2.5 Capsizing

**Possible But Rare:**

**Triggers:**
- Major damage on one side
- Massive wave hit at wrong angle
- Severely overloaded
- Combination factors

**Process:**
- Critical lean
- Cargo shifts
- Worse lean
- Eventually capsizes
- Catastrophic

**Recovery:**
- Difficult/impossible underway
- May right itself in calm
- Often terminal
- Memorable failure

---

## 3. SHIP MOTION IN WAVES

### 3.1 The Six Motions

**Cross-Reference:** physics/water_physics.md Section 5.2

**Quick Reference:**

**Heave:** Vertical up-down with wave amplitude
**Sway:** Horizontal side-side (subtle usually)
**Surge:** Forward-backward (rare but felt)
**Pitch:** Bow up, bow down (most dramatic)
**Roll:** Side-to-side rotation (beam seas worst)
**Yaw:** Left-right rotation (twisting, storm conditions)

### 3.2 Wave Amplitude Effects

**Calm (0.5m waves):**
- Minimal motion
- Comfortable
- Standard

**Light (1.5m):**
- Pleasant rocking
- Standard motion
- Comfortable

**Moderate (3.5m):**
- Noticeable motion
- Some adjustment
- Active sailing

**Strong (7m):**
- Significant motion
- Crew adjusts
- Combat affected

**Storm (14m+):**
- Extreme motion
- Crisis sailing
- Standing difficult

**Hurricane (20m+):**
- Survival mode
- Crawling required
- Memorable

### 3.3 Crew Performance in Motion

**Standing on Deck:**

**Calm:**
- Standard movement
- No issues
- Comfortable

**Moderate:**
- Slight lean
- Minor adjustments
- Realistic

**Strong:**
- Hold rails
- Steady stance
- Combat harder

**Storm:**
- Crawling
- Falling possible
- Crisis

### 3.4 Combat in Wave Motion

**Aim Affected:**
- Pitch affects bow guns
- Roll affects beam guns
- Skill required
- Tactical depth

**Specific Implementation:**
- Aim adjusts with motion
- Skilled gunners compensate
- Realistic challenge
- Memorable

### 3.5 Cargo Shift

**Storm Cargo Shift:**
- Heavy roll = cargo slides
- Listing increases
- Crisis worsens
- Realistic

**Securing Cargo:**
- Pre-storm preparation
- Crew work
- Specific gameplay
- Practical

---

## 4. PROPULSION PHYSICS

### 4.1 Sail Propulsion

**Cross-Reference:** physics/wind_systems.md Section 4

**Physical Reality:**

**Sail Filling:**
- Wind pressure
- Realistic curves
- Force generation
- Direction-dependent

**Force Vectors:**
- Wind force on sails
- Translated to ship motion
- Realistic physics
- Skill in management

**Speed Limits:**
- Hull design constraint
- Maximum hull speed
- Realistic limitation
- Strategic awareness

### 4.2 Sailing Angles

**Direct Downwind:**
- Maximum thrust
- Less efficient than expected
- Realistic
- Pure sailing

**Broad Reach:**
- Wind from quarter
- Best speed often
- Realistic
- Strategic

**Beam Reach:**
- Wind perpendicular
- Good speed
- Stable
- Common cruising

**Close Reach:**
- Wind forward
- Reduced speed
- Realistic
- Demanding

**Close-Hauled:**
- Wind nearly forward
- Minimum speed
- Tacking required
- Specific skill

**Into Wind:**
- Cannot sail directly
- Must tack
- Realistic limitation
- Strategic

### 4.3 Tacking and Jibing

**Tacking:**
- Turn through wind
- Bow crosses wind
- Specific maneuver
- Skill-based

**Jibing:**
- Turn away from wind
- Stern crosses wind
- Different physics
- Sometimes dangerous

### 4.4 Rowing (Limited)

**For Specific Situations:**
- Calm conditions
- Harbor maneuvering
- Emergency
- Crew effort required

### 4.5 Anchoring Physics

**Cross-Reference:** physics/water_physics.md Section 5.5

**Realistic Behavior:**
- Anchor drops, takes hold
- Chain length matters (5x depth rule)
- Tension visible
- Failure possible

---

## 5. NAVAL COMBAT PHYSICS

### 5.1 Cannon Mechanics

**Cannon Types:**

**Light Cannon:**
- 6-12 pounder
- Quick reload
- Less damage
- Standard pirate

**Standard Cannon:**
- 18-24 pounder
- Moderate reload
- Standard damage
- Common

**Heavy Cannon:**
- 32-42 pounder
- Slow reload
- Massive damage
- Powerful ships

**Carronade:**
- Short range, heavy hit
- Quick reload
- Devastating close
- Specific tactics

### 5.2 Cannon Ball Trajectory

**Realistic Ballistics:**

**Initial Velocity:**
- 400-600 m/s typical
- Specific to cannon
- Realistic physics

**Arc:**
- Gravity affects
- Drop with distance
- Realistic
- Skill-based

**Range:**
- Effective: 200-500m
- Maximum: up to 1km
- Distance affects accuracy
- Realistic

**Wind Effect:**
- Less than arrows
- Still measurable
- Long-range matters
- Tactical

### 5.3 Cannon Ball Impact

**On Hull (Wood):**
- Splintering
- Wood debris flying
- Specific hole
- Crew injury possible from splinters

**On Mast:**
- Significant damage
- Possible break
- Catastrophic
- Memorable

**On Rigging:**
- Lines snap
- Sails affected
- Maneuverability lost
- Strategic

**On Deck:**
- Crater
- Crew nearby injured
- Visible damage
- Atmospheric

**On Crew:**
- Devastating
- Body destruction
- Style B brutal
- Realistic

### 5.4 Specialized Ammunition

**Round Shot:**
- Standard
- Hull damage
- General purpose

**Chain Shot:**
- Two balls connected
- Specifically damages rigging
- Tactical
- Specialized

**Grape Shot:**
- Multiple smaller projectiles
- Anti-personnel
- Devastating crew
- Specific tactics

**Hot Shot:**
- Heated cannonball
- Fire-starting
- Specific tactics
- Memorable

**Bar Shot:**
- Specifically anti-mast
- Connecting bar
- Tactical
- Specialized

### 5.5 Naval Damage Cascade

**Hull Damage Progression:**

**Light (0-25%):**
- Cosmetic mainly
- Minor flooding possible
- Pumps handle
- Continuing function

**Moderate (25-50%):**
- Visible holes
- Some flooding
- Pumps work
- Speed reduced

**Heavy (50-75%):**
- Major holes
- Significant flooding
- Pumps struggling
- Major issues

**Critical (75-100%):**
- Catastrophic
- Pumps overwhelmed
- Sinking imminent
- Crisis mode

### 5.6 Mast Damage

**Mast Hit Consequences:**

**Light Damage:**
- Cosmetic
- Some splinters
- Continuing function

**Moderate Damage:**
- Reduced sail capacity
- Slower
- Repair needed

**Heavy Damage:**
- Possible mast break
- Catastrophic event
- Crew danger
- Memorable

**Mast Break:**
- Falls onto deck
- Crew may die
- Massive damage
- Ship crippled
- Iconic moment

### 5.7 Multiple Damage Effects

**Cumulative:**
- Damage doesn't reset
- Each hit adds up
- Repair between fights
- Strategic depth

**Visible:**
- Damage shows
- Atmospheric
- Realistic
- Memorable

---

## 6. SHIP DAMAGE AND REPAIR

### 6.1 Damage Types

**Hull:**
- Cannon balls
- Ramming
- Reef contact
- Storm damage

**Mast:**
- Combat damage
- Storm damage
- Lightning strike (rare)
- Mast break catastrophic

**Sail:**
- Cannon damage
- Wind damage (storms)
- Wear over time
- Specific repairs

**Rigging:**
- Snapped lines
- Combat damage
- Storm damage
- Quick repairs

**Deck:**
- Combat impacts
- Storm wave damage
- General wear
- Cosmetic mostly

### 6.2 Active Damage

**Flooding:**
- Water enters through breaches
- Level rises gradually
- Pumps fight back
- Crisis if exceeds capacity

**Fire:**
- Cross-reference physics/fire_physics.md
- Spreads through ship
- Crew fights
- Catastrophic if uncontrolled

**Listing:**
- Ship leans
- Affects all systems
- Repair urgent
- Crisis

### 6.3 Repair Mechanics

**At Sea Repairs:**
- Wood scarce
- Limited tools
- Slower
- Temporary fixes

**At Port Repairs:**
- Full capability
- Time and money
- Better quality
- Long-term

**Cross-Reference:** mechanics/ship_management.md

### 6.4 Crew at Damage Control

**Specific Crew Roles:**

**Big Tom:**
- Master at repair
- Speeds repairs
- Specific physics
- Critical

**Pump Crew:**
- Generic crew at pumps
- Fight flooding
- Specific gameplay
- Crisis management

**Fire Crew:**
- Combat ship fires
- Specific gameplay
- Critical role

**Sail Repair:**
- Mira leads
- Specific skills
- Quick repairs
- Critical

---

## 7. SHIP SINKING

### 7.1 The Sinking Process

**Stages of Death:**

**Stage 1: Critical Damage**
- Multiple hull breaches
- Pumps overwhelmed
- Listing increasing
- Crisis evident

**Stage 2: Final Maneuvers**
- Crew evacuating
- Captain's choices
- Specific scenes
- Memorable

**Stage 3: Loss of Buoyancy**
- Ship slowly sinks
- Slow process (minutes)
- Atmospheric
- Memorable

**Stage 4: Below the Surface**
- Continues descent
- Eventually rests on bottom
- Deep water = lost
- Shallow = recoverable

### 7.2 Crew Survival

**Abandon Ship:**
- Crew jumps
- Lifeboats deployed
- Specific physics
- Memorable

**Drowning Risk:**
- Crew may drown
- Specific NPCs at risk
- Player choices matter
- Realistic

**Rescue Possible:**
- Other ships pickup
- Floating debris
- Specific scenes
- Story hooks

### 7.3 Cargo Loss

**Lost in Sinking:**
- Specific items recoverable
- Some lost forever
- Economic impact
- Realistic

**Salvage Possible:**
- Specific quests
- Diving for cargo
- See physics/water_physics.md Section 6
- Adventure content

### 7.4 La Liberación Sinking

**If This Happens:**
- Major story event
- Possibly game-altering
- Specific scenes
- Memorable

**Recovery:**
- New ship needed
- Story-driven
- Specific quests
- Heavy emotional weight

### 7.5 Enemy Ship Sinking

**Standard Naval Defeat:**
- Enemy goes down
- Specific scenes
- Crew choices
- Atmospheric

**Watching Sink:**
- Captain ritual
- Crew respect (sometimes)
- Atmospheric
- Memorable

---

## 8. RAMMING AND COLLISION

### 8.1 Ramming Physics

**Player Ramming:**

**Effects:**
- Both ships damaged
- Specific physics
- Strategic move
- Tactical

**Ship Build for Ramming:**
- Reinforced bow
- Specific upgrade
- Strategic choice
- Specific advantage

### 8.2 Collision Damage

**Calculations:**

**Speed Matters:**
- Faster impact = more damage
- Realistic physics
- Strategic timing

**Angle Matters:**
- Direct = maximum damage
- Glancing = less
- Specific physics
- Skill-based

**Mass Matters:**
- Heavier ship dominant
- Lighter ship suffers more
- Realistic
- Strategic

### 8.3 Reef and Rock Collision

**Coastal Hazards:**
- Reefs damage hulls
- Rocks tear bottoms
- Captain must know
- Specific gameplay

**Damage:**
- Significant
- Often catastrophic
- Memorable
- Real consequence

**Recovery:**
- May tear loose
- May be stuck
- Specific scenarios
- Tactical

### 8.4 Iceberg Collision (Fjordlund)

**Specific Hazard:**
- Cold waters
- Ice damages hulls
- Specific physics
- Northern danger

---

## 9. BOARDING ACTION PHYSICS

### 9.1 Ship-to-Ship Contact

**Approach:**
- Ships close
- Speed matched
- Specific physics
- Tactical

**Side-by-Side:**
- Ships rub
- Both rock
- Specific physics
- Atmospheric

**Grappling Hooks:**
- Specific items
- Hook physics
- Pull together
- Combat begins

### 9.2 Boarding Combat

**On Both Decks:**
- Wave motion continues
- Combat in motion
- Specific physics
- Cinematic

**Crew Spread:**
- Multiple combat zones
- AI behavior
- Specific gameplay
- Tactical depth

### 9.3 Capturing Ships

**Successful Boarding:**
- Enemy crew defeated
- Ship captured
- Specific outcomes
- Strategic

**What Player Can Do:**
- Take ship as prize
- Plunder cargo
- Recruit crew
- Sink the ship
- Specific choices

### 9.4 Boarding Dangers

**Falling:**
- Off deck
- Between ships
- Crushed possible
- Specific danger

**Being Boarded:**
- Enemy attacks La Liberación
- Defensive combat
- Specific dynamics
- Tactical

---

## 10. SPECIFIC SHIP TYPES

### 10.1 La Liberación (Player)

**Specifications:**
- Sloop or brig class (depending on upgrade)
- Standard handling
- Specific cannons
- Player home

**Physics:**
- Standard player vessel
- Upgradeable (mechanics/ship_management.md)
- Specific feel
- Cherished

### 10.2 Imperial Frigate

**Larger Enemy:**
- More cannons
- More crew
- Slower turn
- Heavier hits

### 10.3 Imperial Ship of the Line

**Massive:**
- Multiple decks
- Cannon broadsides
- Massive damage
- End-game content

### 10.4 Pirate Sloops

**Fast:**
- Quick handling
- Lighter
- Less cannons
- Tactical

### 10.5 Merchant Vessels

**Cargo Heavy:**
- Slow
- Lightly armed
- Loot rich
- Standard prey

### 10.6 Specific Story Ships

**Crimson Armada Vessels:**
- Specific design
- Pirate elite
- Memorable
- Specific encounters

**Ghost Ship:**
- Cross-reference fauna/sea_animals.md
- Supernatural
- Specific physics
- Memorable

---

## 11. WIND, WAVE, AND SHIP

### 11.1 Combined Effects

**All Three Interact:**

**Calm Sea, Light Wind:**
- Easy sailing
- Slow
- Beautiful
- Standard

**Wind With Waves:**
- Standard sailing
- Realistic
- Active

**Wind Against Waves:**
- Confused water
- Difficult
- Wave hits hard
- Tactical

**Storm:**
- All amplified
- Crisis
- Skill required
- Memorable

### 11.2 Reading Conditions

**Captain's Skill:**
- Watch sky (clouds)
- Watch sea (waves)
- Feel wind
- Synthesize

**Esteban Teaches:**
- Master reads everything
- Esteban shares wisdom
- Player learns
- Skill development

### 11.3 Storm Sailing

**Specific Tactics:**
- Reef sails
- Specific course
- Crew at posts
- Memorable scenes

---

## 12. CREW PERFORMANCE IMPACT

### 12.1 Crew Affects Ship

**Skilled Crew:**
- Faster maneuvers
- Better gunnery
- Damage control
- Maximum performance

**Tired Crew:**
- Slower
- Mistakes
- Reduced performance
- Realistic

**Crew Casualties:**
- Reduced effectiveness
- Specific gameplay
- Realistic
- Strategic

### 12.2 Specific Crew Roles

**Helm:**
- Mira primary
- Steers ship
- Critical role
- Specific physics

**Gunners:**
- Multiple crew
- Specific stations
- Combat critical
- Performance vary

**Sail Handlers:**
- Generic crew
- Adjust sails
- Constant work
- Skilled needed

**Damage Control:**
- Big Tom leads
- Specific physics
- Critical
- Memorable

### 12.3 Captain's Position

**Jake's Location:**
- Helm = direct steering
- Bow = combat oversight
- Cabin = strategic
- Specific options

**Position Affects:**
- What crew sees
- Morale
- Combat awareness
- Specific gameplay

---

## 13. PORT INTERACTION

### 13.1 Approaching Port

**Specific Maneuvers:**
- Slow approach
- Sail reduction
- Specific physics
- Skill-based

**Anchor or Dock:**
- Anchor in harbor
- Dock at pier
- Specific choices
- Strategic

### 13.2 Specific Docking

**Pier Mooring:**
- Lines tied
- Specific physics
- Crew work
- Standard

### 13.3 Departing

**Setting Sail:**
- Crew works
- Specific physics
- Memorable
- Beautiful sometimes

---

## 14. ENVIRONMENTAL HAZARDS

### 14.1 Reefs

**Coral Reefs:**
- Caribbean specific
- Visible underwater
- Damage if struck
- Navigation critical

### 14.2 Sandbars

**Coastal Hazard:**
- Hidden danger
- Ship grounds
- Specific consequences
- Tide-dependent

### 14.3 Whirlpools

**Cross-Reference:** physics/water_physics.md Section 9.2

**Effects on Ships:**
- Pulls in
- Damage massive
- Death possible
- Memorable

### 14.4 Storm Damage

**Cross-Reference:** physics/weather_physics.md

**Specific to Ships:**
- Hull strain
- Mast danger
- Sail damage
- Crew danger

---

## 15. SHIP COMBAT TACTICS

### 15.1 Wind Gauge

**Position for Wind Advantage:**
- Upwind = advantage
- Speed maximum
- Maneuverability max
- Strategic

### 15.2 T-Crossing

**Naval Tactic:**
- Cross enemy bow/stern
- Full broadside vs limited fire
- Devastating
- Skilled

### 15.3 Line of Battle

**Ship-to-Ship:**
- Form firing line
- Specific tactics
- Memorable
- Strategic

### 15.4 Hit and Run

**Pirate Style:**
- Quick attacks
- Disable rigging
- Plunder
- Escape

### 15.5 Boarding Approach

**Specific Tactics:**
- Disable cannons first
- Approach undamaged
- Board and capture
- Strategic

---

## 16. PS5 DUALSENSE INTEGRATION

### 16.1 Helm Feedback

**At Wheel:**
- Resistance varies
- Wave impacts felt
- Wind through wheel
- Immersive

### 16.2 Combat Feedback

**Cannon Fire:**
- Major haptic
- Boom feel
- Memorable

**Hit on Ship:**
- Ship shudders
- Specific feedback
- Atmospheric

### 16.3 Speaker

**Ship Sounds:**
- Spatial audio
- Wood creaking
- Wave impacts
- Atmospheric

---

## 17. ACCESSIBILITY

### 17.1 Sailing Difficulty

**Adjustable:**
- Realistic sailing
- Forgiving sailing
- Casual sailing
- Player choice

### 17.2 Combat Naval

**Tier Settings:**
- Realistic
- Forgiving
- Casual
- Tutorial extensive

### 17.3 Damage Control

**Difficulty:**
- Realistic
- Forgiving
- Auto-managed (casual)
- Player friendly

---

## 18. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** Sailors describe ships in maritime Low. Captains in Middle. Aristocrats in High. Cultural variation.
- ✅ **Pillar 2 (Lore Through Objects):** Ships carry stories. Specific items aboard reveal history. Damage marks events.
- ✅ **Pillar 3 (Layered Death):** Ship deaths styled appropriately. Style D for La Liberación if she sinks. Style B for enemy ships in combat.
- ✅ **Pillar 4 (Layered Humor):** Style C quirks aboard (Big Tom in storm, crew comedy in chaos).
- ✅ **Pillar 5 (Theme — Revenge):** Ship choices reflect path. Aggressive ramming for vengeance. Careful navigation for measured. Atmospheric mirror.

---

## 19. NEXT STEPS

Next document: **`physics/movement_physics.md`** — Combat & Movement Tier Document 3 (FINAL Combat & Movement)

Will cover:
- Walking and running physics
- Climbing physics
- Falling and jumping
- Swimming detailed
- Slipping mechanics
- Movement in different environments

After Movement Physics: Combat & Movement Tier complete. Then Environmental Tier.

---

## 20. CONCLUSION

La Liberación is not a vehicle Jake drives. She is a vessel he commands. She floats based on her weight. She rocks based on the waves. She suffers when damaged. She dies if abandoned. Her physics are her life.

When players feel their ship rock in heavy seas, when they hear wood creak under wave pressure, when they see cannonballs splinter their hull, when they watch enemy ships slowly sink after a successful broadside — that's when ships become more than gameplay objects. That's when they become the vessels they should be.

**Phase 6 Subsystem 6 of 15 — complete.**

**The ship floats. The waves push. The wood remembers. La Liberación lives.** ⚓⛵
