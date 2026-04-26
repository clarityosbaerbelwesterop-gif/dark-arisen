# MOVEMENT PHYSICS — JAKE'S BODY IN THE WORLD

**Status:** Design Draft v1.0  
**Phase:** Phase 6 — Physics (Subsystem 7 of 15 — Combat & Movement Tier FINAL)  
**Tier:** Combat & Movement Tier  
**Related:** physics_overview.md, physics/water_physics.md, physics/weather_physics.md, mechanics/traversal.md, mechanics/stealth.md, style_bible.md  
**Purpose:** Define the complete movement physics system — walking, running, jumping, climbing, falling, swimming, slipping, and surface-specific movement. The constant tactile experience of Jake's body in the world.  
**Inspirations:** Red Dead Redemption 2 (movement weight), Death Stranding (terrain interaction), Uncharted (climbing physics), Assassin's Creed (parkour systems), Sekiro (precise platforming), real-world biomechanics

---

## 1. MOVEMENT PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Jake Has Mass

Jake is not a flying camera. He has weight. Stopping takes effort. Starting takes effort. Direction changes have momentum. Players feel his mass through every movement.

**Design intent:** Tactile authenticity creates physical immersion.

### 1.2 Principle 2: Surfaces Matter

Walking on stone is different than walking on mud. Running on sand is different than running on snow. Climbing on wet rock is different than dry. Every surface has its own physics relationship with the body.

**Design intent:** Environmental awareness through tactile difference.

### 1.3 Principle 3: Falling Has Real Consequence

Heights matter. A fall from 5m is different from 15m is different from 50m. Players learn realistic spatial awareness — not "what does the game allow" but "what would actually happen."

**Design intent:** Realistic spatial reasoning rewards careful play.

### 1.4 Principle 4: Climbing Requires Reality

Hands need handholds. Feet need footholds. Jake doesn't stick magically to walls. Realistic climbing means realistic limits — and realistic mastery when achieved.

**Design intent:** Climbing as skill, not as auto-pilot.

### 1.5 Principle 5: Water Is a Different World

Swimming is not running through wet air. Movement is fundamentally different. Stamina depletes faster. Cold matters. Currents pull. Drowning is real.

**Design intent:** Water as transformation — Jake becomes a different kind of body.

---

## 2. WALKING AND RUNNING

### 2.1 Walking Physics

**Default Movement:**
- Speed: ~5 km/h (1.4 m/s)
- Realistic gait
- Sustainable indefinitely
- Stamina-free

**Visible Behavior:**
- Natural stride
- Body weight transfer
- Realistic arm swing
- Footstep patterns visible

**Audio:**
- Footsteps surface-specific
- Realistic rhythm
- Environmental cues
- Atmospheric

### 2.2 Running Physics

**Sustained Speed:**
- Speed: ~12 km/h (3.3 m/s)
- Stamina drain (cross-reference mechanics/combat_feel.md)
- Realistic limit
- Sustainable for time

**Visible Behavior:**
- Forward lean
- Faster footstep
- Arm pumping
- Visible effort

**Audio:**
- Faster footsteps
- Heavier breathing
- Surface-specific
- Atmospheric

### 2.3 Sprinting Physics

**Maximum Speed:**
- Speed: ~22 km/h (6 m/s)
- Major stamina drain
- Brief duration
- Burst movement

**Visible Behavior:**
- Maximum lean
- Visible exertion
- Limited duration
- Realistic

**Audio:**
- Heavy panting
- Pounding footsteps
- Body strain
- Memorable

### 2.4 Surface Effects on Movement

**Stone:**
- Standard movement
- Sharp footstep audio
- Reliable footing
- Default

**Wood:**
- Slight resonance
- Hollow footstep audio
- Reliable
- Atmospheric

**Mud:**
- Slowing effect (-30% speed)
- Stamina drain higher
- Slipping risk
- Realistic

**Sand:**
- Slowing effect (-20% speed)
- Visible footprints
- Different audio
- Beach/desert

**Snow (Light):**
- Slight slowing (-10%)
- Visible footprints
- Crunching audio
- Tracking enabled

**Snow (Deep):**
- Major slowing (-50%)
- Knee-deep slow
- Significant stamina drain
- Realistic crisis

**Ice:**
- Slipping risk high
- Sliding possible
- Specific challenges
- Specific gear helps

**Wet Stone:**
- Slipping possible
- Reduced traction
- Specific concerns
- Realistic

**Vegetation:**
- Slight slowing
- Specific audio
- Atmospheric
- Realistic

**Water (Shallow):**
- -40% speed
- Splashing
- Specific audio
- Realistic

### 2.5 Slope Effects

**Uphill:**

**Gentle Slope:**
- Minimal slowing
- Slight stamina increase
- Standard

**Moderate Slope:**
- Visible slowing
- Stamina drain
- Realistic

**Steep Slope:**
- Major slowing
- Heavy stamina drain
- Specific gameplay
- Demanding

**Very Steep:**
- Cannot run
- Walking only
- Specific paths
- Realistic limit

**Downhill:**

**Gentle:**
- Slight speed up
- Easier
- Standard

**Moderate:**
- Visible speed up
- Some control needed
- Realistic

**Steep:**
- Risk of falling
- Sliding possible
- Specific control
- Demanding

**Very Steep:**
- Cannot maintain
- Slide or fall
- Realistic limit
- Memorable

### 2.6 Direction Changes

**Realistic Momentum:**

**At Walk:**
- Quick direction changes
- Minimal lag
- Standard

**At Run:**
- Some momentum
- Slight delay
- Realistic

**At Sprint:**
- Significant momentum
- Visible "swing wide"
- Strategic awareness
- Realistic

### 2.7 Stop Physics

**Realistic Stopping:**

**From Walk:**
- Quick stop
- Standard

**From Run:**
- Brief slowing
- Realistic

**From Sprint:**
- Visible deceleration
- Sometimes stumble
- Realistic
- Memorable

---

## 3. JUMPING

### 3.1 Standing Jump

**Vertical Jump:**
- Height: ~0.5m
- Distance: minimal
- Standard movement

**Visible Behavior:**
- Crouch before jump
- Push off
- Apex
- Landing

**Audio:**
- Push-off grunt
- Landing thud
- Surface-specific
- Realistic

### 3.2 Running Jump

**Forward Jump:**
- Height: ~0.7m
- Distance: ~3m
- Common gameplay

**Use Cases:**
- Cross gaps
- Reach ledges
- Standard parkour
- Strategic

### 3.3 Sprint Jump

**Maximum Distance:**
- Height: ~0.8m
- Distance: ~4-5m
- Maximum athletic

**Use Cases:**
- Long gaps
- Spectacular leaps
- Strategic decisions
- Memorable

### 3.4 Specific Jump Rules

**Cannot Jump:**
- While exhausted (no stamina)
- Carrying too much weight
- On certain surfaces (deep mud)
- Specific limits

**Jump Cancellation:**
- During action
- Mid-attack
- Specific scenarios

### 3.5 Landing Physics

**Soft Landing:**
- Crouches slightly
- Absorbs impact
- Standard

**Hard Landing:**
- Stagger animation
- Brief delay
- Realistic
- Memorable

**Failed Landing:**
- Falls forward
- Damage possible
- Stamina drain
- Realistic

### 3.6 Jumping in Combat

**Specific Combat Jumps:**
- Aerial attacks (some)
- Dodge jumps
- Specific weapons enable
- Tactical depth

---

## 4. CLIMBING PHYSICS

### 4.1 Climbing Surfaces

**Climbable:**
- Specific markers visible (subtle)
- Realistic handholds
- Vines and ropes
- Specific architecture

**Not Climbable:**
- Smooth surfaces
- Wet (specific)
- Magical inability
- Realistic limits

### 4.2 Climbing Mechanics

**Hand and Foot Holds:**
- Realistic positions
- Body weight balanced
- Specific physics
- Real climbing

**Speed:**
- Slower than walking
- Realistic exertion
- Stamina drain
- Demanding

**Stamina Drain:**
- Continuous
- Cannot climb forever
- Strategic resting
- Realistic

### 4.3 Climbing Types

**Vertical Wall:**
- Hand-over-hand
- Specific holds
- Realistic
- Common

**Overhang:**
- More demanding
- Stamina drain higher
- Specific gear helps
- Memorable

**Ladder:**
- Specific climbing
- Hand-foot pattern
- Realistic
- Standard

**Rope:**
- Specific physics
- Hand-over-hand
- Different mechanics
- Realistic

**Vines:**
- Tropical specific
- Specific physics
- Atmospheric
- Realistic

**Rigging (Ship):**
- Sailor-specific
- Specific mechanics
- Cultural
- Atmospheric

### 4.4 Climbing Failures

**Slip:**
- Hand slips
- Hold tightens
- Realistic
- Brief tension

**Fall:**
- Lost grip entirely
- Falling damage applies
- Catastrophic possible
- Real consequence

**Exhaustion:**
- Stamina depleted
- Cannot continue
- Falls
- Realistic

### 4.5 Climbing Equipment

**Climbing Hooks:**
- Specific items
- Reach for grips
- Specific gameplay
- Tactical

**Rope:**
- Used for descents
- Specific gameplay
- Practical
- Realistic

**Specific Gear:**
- Different regional
- Mountain gear (Fjordlund)
- Cliff gear (coastal)
- Cultural

### 4.6 Combat While Climbing

**Limited:**
- Cannot fight effectively
- Must reach safe spot
- Specific gameplay
- Realistic

**Reaching Top:**
- Vulnerable moment
- Enemies above advantage
- Specific tactics
- Realistic

---

## 5. FALLING PHYSICS

### 5.1 Realistic Fall Speeds

**Acceleration:**
- ~9.8 m/s² (real gravity)
- Realistic feel
- Standard physics

**Terminal Velocity:**
- Reached eventually
- Specific game limit
- Realistic

### 5.2 Fall Damage Calculation

**Height Brackets:**

**0-3m:**
- No damage
- Soft landing
- Standard movement

**3-5m:**
- Stagger
- Minor stamina
- Recovery quick

**5-10m:**
- Light damage
- Painful animation
- Recovery slower

**10-15m:**
- Moderate damage
- Heavy stagger
- Recovery time

**15-20m:**
- Heavy damage
- Stunned
- Long recovery

**20-30m:**
- Severe damage
- Possible knockout
- Critical state

**30-40m:**
- Near-fatal
- Critical wounds
- Death possible

**40m+:**
- Death likely
- Realistic
- Specific scenarios

### 5.3 Fall Damage Modifiers

**Surface Affects Damage:**

**Soft Ground (Mud, Sand):**
- 25% damage reduction
- Specific scenarios
- Realistic

**Water:**
- Significant reduction (proper height)
- Splash damage if too high
- Realistic

**Hay/Foliage:**
- Major reduction
- Specific environments
- Realistic

**Ice:**
- Sharp damage
- Slipping after
- Specific scenarios
- Realistic

**Stone:**
- Standard damage
- Default

### 5.4 Fall Animations

**Realistic Tumbling:**
- Body rotates
- Limbs flail
- Realistic
- Memorable

**Specific Death Falls:**
- Cinematic
- Style B brutal
- Memorable
- Atmospheric

### 5.5 Saving Falls

**Specific Mechanics:**

**Catch Edge:**
- Last-second grab
- Specific timing
- Vulnerable moment
- Memorable

**Roll on Landing:**
- Reduces damage
- Specific timing
- Skill-based
- Realistic

**Water Below:**
- Splash, no damage (height-dependent)
- Specific calculations
- Realistic

### 5.6 Falling Into Water

**Cross-Reference:** physics/water_physics.md Section 12.3

**Quick Reference:**
- Height matters
- Speed at impact
- Damage scaled
- Death from height possible

---

## 6. SWIMMING PHYSICS

### 6.1 Swimming Speed

**Surface Swimming:**
- Slower than walking
- Realistic stroke
- Stamina-based
- Sustainable but demanding

**Specific Speeds:**
- Casual: 1 m/s
- Active: 1.5 m/s
- Sprint: 2 m/s (limited)

### 6.2 Stamina in Water

**Drain Rate:**
- Higher than land
- Continuous
- Realistic exhaustion
- Strategic depth

**Recovery:**
- Slow in water
- Faster on land
- Realistic
- Demanding

### 6.3 Currents Affect Swimming

**Cross-Reference:** physics/water_physics.md Section 4

**With Current:**
- Helped swimming
- Faster
- Less stamina

**Against Current:**
- Slower
- Higher stamina drain
- Sometimes impossible
- Realistic

**Cross Current:**
- Pushed sideways
- Constant correction
- Realistic challenge

### 6.4 Cold Water

**Fjordlund Waters:**
- Hypothermia possible
- Specific damage over time
- Limited duration
- Crisis gameplay

**Tropical:**
- Comfortable
- No special concerns
- Pleasant

### 6.5 Diving Physics

**Cross-Reference:** physics/water_physics.md Section 6

**Quick Reference:**
- Pressure increases
- Breath holding
- Equipment helps
- Specific gameplay

### 6.6 Drowning

**Risk Factors:**
- No stamina + in water
- Can't reach surface
- Trapped underwater
- Realistic danger

**Drowning Process:**
- Panic begins
- Stamina critical
- Damage starts
- Death possible

**Rescue:**
- Crew can save
- Specific scenarios
- Tactical importance
- Realistic

---

## 7. SLIPPING PHYSICS

### 7.1 Slip Triggers

**Specific Conditions:**

**Wet Stone:**
- After rain
- Near waterfalls
- Coastal rocks
- Specific danger

**Ice:**
- Fjordlund standard
- Anywhere cold
- Specific surfaces
- High danger

**Mud:**
- After rain
- Specific terrain
- Slowing first
- Slipping possible

**Wet Wood:**
- Ship decks in rain
- Specific scenarios
- Realistic
- Atmospheric

**Smooth Surfaces:**
- Polished stone
- Specific environments
- Realistic
- Demanding

### 7.2 Slip Animation

**Realistic Reaction:**
- Arms flail
- Brief recovery attempt
- Sometimes succeeds
- Sometimes fails

**Falls on Slip:**
- Realistic tumble
- Damage possible
- Stamina drain
- Memorable

### 7.3 Slip Tactical Use

**Combat Scenarios:**
- Enemies slip on ice
- Player slips too
- Tactical awareness
- Realistic depth

**Stealth Use:**
- Wet floor traps
- Specific scenarios
- Strategic
- Memorable

### 7.4 Slip Prevention

**Specific Gear:**
- Cleated boots (Fjordlund)
- Sand grippers (Ashenmoor)
- Specific items
- Cultural

**Skill Development:**
- Player learns
- Awareness grows
- Specific scenarios mastered
- Realistic

### 7.5 Slipping in Combat

**Mid-Battle:**
- Vulnerable moment
- Tactical
- Realistic
- Sometimes catastrophic

**Recovery:**
- Quick if skilled
- Slower if exhausted
- Specific scenarios
- Memorable

---

## 8. CARRYING WEIGHT

### 8.1 Weight Categories

**Light Load:**
- Standard movement
- No effects
- Default

**Moderate Load:**
- Slight slowing (-10%)
- Slight stamina drain
- Realistic

**Heavy Load:**
- Significant slowing (-25%)
- Higher stamina drain
- Specific gameplay
- Realistic

**Maximum Load:**
- Major slowing (-40%)
- Cannot run
- Cannot jump
- Realistic limit

### 8.2 Specific Carrying

**Bodies:**
- Slow movement
- Specific use
- Story moments
- Memorable

**Cargo:**
- Specific items
- Specific physics
- Realistic
- Atmospheric

**Crew Members:**
- Wounded carry
- Specific scenarios
- Heroic moments
- Memorable

### 8.3 Cross-Reference

**Inventory Weight:**
- mechanics/inventory_crafting.md Section 2.3
- Detailed weight system
- Strategic depth

---

## 9. STEALTH MOVEMENT

### 9.1 Crouching

**Cross-Reference:** mechanics/stealth.md

**Physical:**
- Lower profile
- Slower movement
- Specific physics
- Tactical

### 9.2 Sneak Walking

**Quiet Movement:**
- Surface-specific noise
- Slower pace
- Strategic
- Skill-based

### 9.3 Specific Surfaces

**Loud:**
- Gravel
- Dry leaves
- Wood (some)
- Stone (some)

**Quiet:**
- Grass
- Mud (slow)
- Sand
- Carpet

**Variable:**
- Wet stone (loud and slippery)
- Wet wood (quieter but slippery)
- Specific
- Tactical

### 9.4 Stealth Climbing

**Specific:**
- Quieter climbing
- Slower
- Realistic
- Tactical

---

## 10. ENVIRONMENTAL MOVEMENT

### 10.1 In Water

**Cross-Reference:** Section 6

### 10.2 In Snow

**Specific Physics:**
- Tracks visible
- Slow movement
- Cold concerns
- Cross-reference physics/snow_ice_physics.md (forthcoming)

### 10.3 In Sand

**Specific Physics:**
- Slow but stable
- Visible tracks
- Heat concerns
- Cross-reference physics/sand_desert_physics.md (forthcoming)

### 10.4 In Vegetation

**Cross-Reference:** physics/vegetation_physics.md (forthcoming)

**Quick Reference:**
- Slowing in dense
- Pushing through
- Specific physics
- Atmospheric

### 10.5 On Ship

**Wave-Adjusted:**
- Specific physics
- Cross-reference physics/ship_physics.md
- Body adjusts
- Realistic

### 10.6 Underground

**Caves:**
- Limited light
- Specific surfaces
- Tactical considerations
- Atmospheric

### 10.7 In Buildings

**Standard:**
- Stone/wood floors
- Stairs
- Standard physics
- Civilized

---

## 11. REGIONAL MOVEMENT CHARACTER

### 11.1 Caribbean (Rexa/Moran)

**Movement Feel:**
- Tropical heat affects
- Vegetation thick
- Mud after rain
- Sand on beaches
- Variable

**Specific Adaptations:**
- Light clothing
- Sandals practical
- Cultural

### 11.2 Northern Atlantic (Fjordlund)

**Movement Feel:**
- Cold affects
- Snow common
- Ice frequent
- Demanding

**Specific Gear:**
- Heavy boots
- Cleats
- Layered clothing
- Survival

### 11.3 Volcanic (Ashenmoor)

**Movement Feel:**
- Heat exhausting
- Sand slowing
- Ash dust
- Extreme

**Specific Gear:**
- Sun protection
- Sand-friendly
- Cultural

### 11.4 Imperial (Pale Isle)

**Movement Feel:**
- Civilized
- Standard
- Comfortable
- Default

### 11.5 Quiet Coast

**Movement Feel:**
- American-style
- Comfortable
- Standard temperate
- Familiar

### 11.6 Region 06

**Movement Feel:**
- Reality bends
- Specific physics
- Symbolic
- Memorable

---

## 12. MOVEMENT IN COMBAT

### 12.1 Combat Stance

**Positioning:**
- Slower than free movement
- Defensive options
- Specific physics
- Tactical

### 12.2 Dodge

**Cross-Reference:** mechanics/combat_feel.md

**Physical:**
- Quick burst
- Direction-based
- Stamina cost
- Specific physics

### 12.3 Backpedaling

**Slower:**
- Cannot match forward speed
- Realistic
- Tactical limit

### 12.4 Strafe

**Sideways:**
- Slower than forward
- Realistic
- Tactical

### 12.5 Combat Acrobatics

**Specific Moves:**
- Combat rolls
- Vault attacks
- Specific weapons
- Skill-based

---

## 13. CREW MOVEMENT

### 13.1 NPC Movement

**Specific Behaviors:**
- Realistic AI
- Path finding
- Combat positioning
- Atmospheric

### 13.2 Crew on Ship

**Wave-Adjusted:**
- Specific physics
- Realistic adjustments
- Atmospheric
- Memorable

### 13.3 Crew Combat

**Tactical:**
- Position aware
- Realistic AI
- Specific behaviors
- Tactical

---

## 14. PS5 DUALSENSE INTEGRATION

### 14.1 Footstep Feedback

**Surface-Specific:**
- Stone sharp
- Mud squelchy
- Snow crunchy
- Sand soft
- Atmospheric

### 14.2 Climbing Feedback

**Hand and Foot:**
- Specific tactile
- Each grip felt
- Strain felt
- Memorable

### 14.3 Falling Feedback

**Through Air:**
- Specific feel
- Ground impact
- Damage felt
- Memorable

### 14.4 Slipping Feedback

**Loss of Control:**
- Specific feedback
- Tactical
- Atmospheric

### 14.5 Swimming Feedback

**Water Resistance:**
- Specific feel
- Stroke rhythm
- Atmospheric
- Immersive

### 14.6 Adaptive Triggers

**Climbing:**
- Tension as climb
- Strain felt
- Realistic

**Stamina:**
- Heavier as tired
- Specific feedback
- Atmospheric

---

## 15. ACCESSIBILITY

### 15.1 Movement Difficulty

**Adjustable:**
- Realistic
- Forgiving
- Casual
- Player choice

### 15.2 Auto-Climbing

**Specific Settings:**
- Auto-grab
- Reduced precision
- Player friendly
- Inclusive

### 15.3 Fall Damage

**Adjustable:**
- Realistic
- Reduced
- Minimal
- Player choice

### 15.4 Swimming

**Settings:**
- Realistic stamina
- Extended
- Unlimited
- Player choice

### 15.5 Slipping

**Adjustable:**
- Realistic
- Reduced
- None
- Player choice

---

## 16. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** NPCs comment on movement in cultural register. Aristocrats notice grace. Sailors note skill.
- ✅ **Pillar 2 (Lore Through Objects):** Specific gear reveals culture. Climbing hooks tell stories. Worn boots show journeys.
- ✅ **Pillar 3 (Layered Death):** Falling deaths styled. Style B brutal heights. Style D atmospheric drowning.
- ✅ **Pillar 4 (Layered Humor):** Style C quirks (slipping comedy, Big Tom's clumsiness, real moments).
- ✅ **Pillar 5 (Theme — Revenge):** Movement reflects path. Aggressive sprint for vengeance. Measured walk for peace. Atmospheric mirror.

---

## 17. NEXT STEPS

**COMBAT & MOVEMENT TIER COMPLETE.**

Next document: **`physics/snow_ice_physics.md`** — Environmental Tier Document 1

Will cover Fjordlund-specific physics:
- Snow accumulation detailed
- Ice slipperiness
- Frozen water mechanics
- Snowstorm dynamics
- Cold damage system

After Snow/Ice: Lava/Heat (Ashenmoor), Sand/Desert (Ashenmoor), Vegetation (Rexa).

---

## 18. CONCLUSION

Jake's body in the world. Walking on stone, running on sand, climbing wet rock, falling from heights, swimming against currents, slipping on ice. The constant physical experience that grounds players in reality.

When players feel Jake's stamina drain on the climb, when they slip on wet deck and fall hard, when they run through deep snow with realistic exhaustion, when they swim against current toward a sinking crewmate — that's when movement becomes more than navigation. That's when it becomes the body itself.

This is the third pillar of Combat & Movement Tier. The constant tactile foundation.

**Phase 6 Subsystem 7 of 15 — complete.**

**COMBAT & MOVEMENT TIER COMPLETE.**

**Jake walks. Jake runs. Jake climbs. Jake falls. Jake swims. The world responds.** 🏃⛰️
