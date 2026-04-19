# SHIP NAVIGATION & SAILING SYSTEM

**Status:** Design Draft v1.0  
**Phase:** Phase 3 — Mechanics (Subsystem 10 of 11)  
**Tier:** World Tier — Depends on Ship Interior  
**Related:** mechanics_overview.md, naval_combat_system.md, mechanics/ship_interior.md, mechanics/traversal.md, style_bible.md  
**Purpose:** Define the peaceful sailing experience — how Jake commands La Liberación between missions, reads the sea, navigates by wind and stars, and experiences discovery.  
**Inspirations:** Assassin's Creed Black Flag (primary — sailing feel, shanties, atmosphere), Sea of Thieves (sailing as team experience), Sid Meier's Pirates! (discovery), Wind Waker (sailing as journey), Ghost of Tsushima (ambient wind), Red Dead Redemption 2 (travel as meditation)

---

## 1. NAVIGATION PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Sailing Is an Experience, Not a Task

In most games, travel is something to skip. In Dark Arisen, travel is something to experience. The journey from Rexa to Fjordlund takes real time. That time is not wasted — it is filled with crew, weather, stars, shanties, conversations, moments. The journey itself is content.

**Design intent:** Fast travel exists but is always inferior to the actual sailing experience. Players who sail manually get more of the game.

### 1.2 Principle 2: The Sea Is Alive

The ocean is not a texture. It is a dynamic system. Wind changes. Currents shift. Weather systems move. Fish run. Whales migrate. Sharks hunt. Birds follow ships. The sea has its own life that Jake moves through, not against.

**Design intent:** Players should feel they are traveling through a real ocean, not a flat surface with occasional obstacles.

### 1.3 Principle 3: The Crew Sails With Jake

Jake is not alone at the helm. His crew is working the ship constantly. They climb rigging. They adjust sails. They call out observations. They sing while they work. The ship is a living vessel with many hands making it move.

**Design intent:** The player commands, the crew performs. Sailing feels like captaining, not piloting a vehicle alone.

### 1.4 Principle 4: Discovery Matters

The world map reveals itself through sailing. Uncharted islands exist. Hidden coves. Sunken ruins visible beneath clear water. Sea monsters in deep water. Players who wander off established routes find things that reward their curiosity.

**Design intent:** The ocean is exploration territory, not just transit space.

### 1.5 Principle 5: Time at Sea Changes People

Long voyages let Jake think. Let crew relationships develop. Let plans form. The ship at sea is where the story processes itself. Major story decisions often happen during or after long voyages, because that is when characters have time to feel.

**Design intent:** Sailing is emotional pacing. It gives the narrative room to breathe.

---

## 2. THE HELM — COMMANDING THE SHIP

### 2.1 Taking the Helm

**Jake at the Wheel:**
- Approach ship's wheel at stern
- Triangle button to take helm
- Camera shifts to Jake-at-helm perspective (3rd person, positioned behind)
- Crew acknowledges (Mira steps back if she was there)

**The View:**
- Wide horizon visible
- Ship's deck in foreground
- Figurehead visible at bow
- Sails rising above
- Ocean stretching to infinity

### 2.2 Helm Controls

**Left Stick:**
- Turn the wheel (ship turns accordingly)
- Small movements = minor adjustments
- Large movements = hard turns (costs speed)

**R2 (Forward):**
- Increase sail (more speed)
- Progression: Furled → Partial → Full → Maximum
- Each level takes crew time to execute

**L2 (Backward):**
- Reduce sail (less speed)
- Opposite progression
- Emergency: can drop anchor (L2 hold)

**Square (Tacking):**
- Sharp maneuver against wind
- Costs significant speed
- Useful in tight navigation

**Circle (Furl All):**
- Emergency stop
- Crew rushes to furl
- Ship coasts to halt

**Triangle (Release Helm):**
- Jake steps away
- Mira takes over
- Autopilot active

### 2.3 Sail Configurations

**Furled:**
- All sails tied
- Ship drifts only
- Used in port, battle prep
- 0-1 knots

**Partial Sail:**
- Some sails deployed
- Controlled movement
- Good for tight quarters
- 3-5 knots

**Full Sail:**
- Standard traveling configuration
- Good speed, manageable
- Most common setting
- 8-12 knots

**Maximum Sail:**
- All canvas deployed
- Top speed
- Risky in bad weather
- 14-18 knots

### 2.4 Wind Awareness

**Wind Direction Indicator:**
- Top-center HUD element
- Shows wind direction relative to ship
- Green = favorable, Yellow = neutral, Red = against

**Sailing With Wind (Green Zone):**
- Maximum speed possible
- Smooth movement
- Efficient travel

**Sailing Across Wind (Yellow Zone):**
- Reduced speed (70%)
- Slight instability
- Still effective

**Sailing Into Wind (Red Zone):**
- Severe speed reduction (30%)
- Difficult to progress
- Must tack (zigzag) to advance

### 2.5 The Skilled Sailor

Players who understand wind can:
- Predict wind changes from cloud patterns
- Position ship optimally for shifts
- Execute tactical maneuvers
- Outrun pursuers, outmaneuver hunters

**Accessibility:**
- Toggle for simplified wind (always favorable)
- Auto-tacking assistance
- Visual wind indicators can be enhanced

---

## 3. THE CREW AT SEA

### 3.1 Constant Activity

The crew is always working when the ship moves:

**Riggers:**
- Climbing rigging
- Adjusting sails
- Calling down reports
- Visible high above

**Deck Crew:**
- Coiling ropes
- Maintaining equipment
- Scrubbing decks
- General upkeep

**Lookouts:**
- Crow's nest always manned
- Calls out sightings
- First to spot storms, ships, land

**Helmsman (Mira):**
- At Jake's side at the helm when he's there
- Takes over when he steps away
- Constant navigation discussion

### 3.2 Crew Calls and Commands

**"Captain, wind shifting!"**
- Announces weather changes
- Gives Jake time to adjust

**"Sail ho!"**
- Another ship spotted
- Direction and distance called

**"Land ho!"**
- Land visible
- Specific direction given
- Often emotional moment

**"Storm approaching, Captain!"**
- Weather warning
- Time to prepare

**"All clear, Captain!"**
- Horizon is safe
- Routine status

### 3.3 Crew Routines at Sea

**Watch System:**
- 4-hour shifts
- Different crew on duty
- 24/7 ship manning

**Meal Times:**
- Announced by bell
- Crew rotates through galley
- Some eat on deck

**Off-Duty:**
- Crew in quarters below
- Some on deck relaxing
- Singing, card games visible

### 3.4 Specific Crew Moments

**Mira at Sunrise:**
- Often found on deck at dawn
- Checking navigation
- Will share coffee with Jake

**Big Tom's Smoke Break:**
- Midday pipe on deck
- Available for conversation
- War stories when in mood

**Father Salvio's Evening Prayer:**
- Quiet spot on deck at sunset
- Can be joined or observed
- Spiritual moment

**Esteban's Night Watch:**
- Grandfather figure at helm
- Teaching Jake constellations
- Ancient maritime wisdom

---

## 4. SEA SHANTIES SYSTEM

### 4.1 The Shanty as Living Mechanic

Sea shanties are not just ambient music. They are a dynamic, evolving system that reflects crew morale, regional culture, and Jake's journey.

**How Shanties Work:**
- Crew starts singing during certain conditions
- Initially one voice, others join
- Builds to full crew chorus
- Fades naturally or breaks off

**Triggering Conditions:**
- Calm weather with fair wind
- After meal times
- During repetitive tasks (weighing anchor, hauling lines)
- Crew morale above certain threshold
- Specific story beats

### 4.2 Shanty Library

Jake's crew accumulates shanties throughout the journey:

**Starting Shanties (Chapter 1):**
- "The Wind's Own Daughter" — classic Caribbean traveling song
- "Doubloons for Breakfast" — cheerful work song
- "Mother Sea, Forgive Me" — melancholic ballad

**Rexa Shanties (Chapter 4):**
- "La Luna de Rexa" — Spanish-influenced, slow
- "Bread and Iron" — working song, fast tempo
- "Don Mateo's Lament" — learned AFTER defeating him

**Fjordlund Shanties (Chapter 7):**
- "The Whale's Road" — Norse-style, deep voices
- "Halvard's Song" — tragic, learned after Halvard's death
- "Mormor Astrid's Tea" — gentle, warm, domestic

**Ashenmoor Shanties (Chapter 6):**
- "The Dragon's Daughter" — mythic, powerful
- "Smoke and Silver" — metallic rhythms

**Special Shanties:**
- "Margarethe's Waltz" — romantic, played only when Margarethe Fitzmueller is aboard
- "Ethan's Song" — Jake's brother's favorite from childhood, triggers specific emotions
- "Draven's Dirge" — sung only after final boss defeat

### 4.3 Unlocking Shanties

**Discovery Methods:**
- Visit taverns, learn local songs
- Specific NPCs teach songs
- Found in old ship's logs
- Taught by crew members
- Story events unlock key songs

**Total Catalog:**
- 47 unique shanties in game
- Players unlock average 30-35 per playthrough
- Some only unlock on specific paths

### 4.4 Jake Singing

**Join the Chorus:**
- Jake can sing along when shanty starts
- Button prompt appears
- His voice joins the crew
- Morale increases

**Jake Leading:**
- At later stages, Jake can initiate shanties
- Select from learned library
- Crew joins enthusiastically
- Major morale boost

**Silence Choice:**
- Jake can choose not to sing
- Listens to crew
- Contemplative moment
- Still enjoyable

### 4.5 Shanty Meaning

Different shanties carry different weights:

**Work Shanties:**
- Light, energetic
- Make tasks feel shorter
- Crew bonding

**Traveling Shanties:**
- Slower, meditative
- Long voyage music
- Time passing markers

**Lament Shanties:**
- Processing loss
- Often after deaths
- Emotional catharsis

**Celebration Shanties:**
- After victories
- Port arrivals
- Joy expression

---

## 5. WEATHER AND THE SEA

### 5.1 Weather Systems

**Clear Weather:**
- Bright sun, fair winds
- Excellent visibility
- Best traveling conditions
- Good for discovery

**Overcast:**
- Gray skies
- Moderate winds
- Good visibility
- Standard travel

**Fog:**
- Reduced visibility (50-200m)
- Dangerous near coast
- Muffled sounds
- Eerie atmosphere

**Rain:**
- Reduces visibility
- Slippery decks
- Crew less enthusiastic
- Shanties rare

**Storm:**
- Dangerous conditions
- High waves
- Lightning possible
- Crew tense

**Hurricane (rare):**
- Extreme danger
- Ship damage certain
- Specific story events
- Test of crew

### 5.2 Reading Weather

Experienced sailors (and players) learn to read:

**Cloud Patterns:**
- Cumulus = fair weather
- Dark building clouds = storm coming
- Mackerel sky = wind shift
- Mare's tails = major change

**Wind Behavior:**
- Steady = stable weather
- Gusting = transition
- Calm = potential storm
- Shifting = weather system

**Sea State:**
- Glassy = no wind
- Ripples = light breeze
- Waves = good wind
- Whitecaps = strong wind
- Mountains = storm

**Atmospheric Pressure:**
- Shown via barometer in cabin
- Falling = storm approaching
- Rising = clearing
- Players can plan ahead

### 5.3 Weather Response

**Approaching Storm:**
- Crew begins preparations
- Secure loose items
- Close hatches
- Shorten sail

**During Storm:**
- Reduced visibility
- Ship damage accumulates
- Crew morale drops
- Navigation harder

**Storm Survival:**
- Proper sail management
- Ride waves correctly
- Protect vulnerable areas
- Crew coordination

**Post-Storm:**
- Damage assessment
- Crew relief
- Sometimes discoveries washed ashore
- Renewed calm

### 5.4 The Ocean Itself

**Day-Night Cycle:**
- Sunrise: golden hour on water
- Noon: bright, hot, peaceful
- Sunset: dramatic skies
- Night: stars, moon reflection

**Moon Phases:**
- Affect tides
- New moon = darkest nights
- Full moon = navigation by moonlight
- Have subtle gameplay effects

**Tides:**
- Matter near coasts
- Low tide reveals things
- High tide floods areas
- Tidal patterns learned

---

## 6. NAVIGATION BY STARS AND INSTRUMENTS

### 6.1 Celestial Navigation

**The Stars:**
- Visible on clear nights
- Different constellations in different hemispheres
- Polaris (North Star) for northern regions
- Southern Cross for south

**Learning Navigation:**
- Esteban teaches Jake over time
- Chapter 1: Basic instruments only
- Chapter 4: Star-reading introduced
- Chapter 7: Full celestial navigation
- Chapter 9: Master navigator

### 6.2 Navigation Tools

**The Sextant:**
- Measures angle between stars and horizon
- Determines latitude
- Requires practice
- Found in Esteban's navigation room

**The Compass:**
- Magnetic north indicator
- Always functional
- Basic navigation

**The Chronometer:**
- Precise timekeeping
- Determines longitude
- Rare and valuable
- Don Mateo's chronometer a trophy

**Maps and Charts:**
- Region-specific detail
- Updated with discoveries
- Hand-drawn by Esteban for Jake
- Accumulate over time

### 6.3 Dead Reckoning

When instruments fail or skies are cloudy:
- Estimate position from speed and direction
- Time elapsed from last known position
- Account for currents
- Experience-based navigation

**Gameplay:**
- UI dims when overcast
- Player must estimate
- Errors possible
- Rewards attention to sailing

### 6.4 Finding Known Locations

**Established Routes:**
- Well-traveled sea lanes
- Navigation buoys
- Lighthouse positions
- Port approaches marked

**Finding Islands:**
- Exact coordinates help
- Visual confirmation required
- Current can push off course
- Patience rewarded

---

## 7. FAST TRAVEL VS. MANUAL SAILING

### 7.1 Fast Travel System

**Availability:**
- Between discovered ports
- Unlocks as regions visited
- Simple menu selection

**Costs:**
- Requires supplies (food, water)
- Time passes (can affect quests)
- Small fee to crew

**What You Miss:**
- Random encounters
- Weather experiences
- Discoveries
- Crew conversations
- Shanties

### 7.2 Manual Sailing Benefits

**Exploration Rewards:**
- Hidden islands
- Sea monster encounters
- Treasure flotsam
- Random NPC help

**Story Moments:**
- Key conversations happen at sea
- Crew relationships develop
- Personal questlines triggered
- Emotional beats

**Economic Benefits:**
- Trade opportunities
- Pirate victims
- Lost cargo recovery
- Fishing profits

### 7.3 The Middle Ground

**Guided Travel:**
- Set destination
- Autopilot active
- Jake free to walk around
- Crew manages basics

**Benefits:**
- Travel feels active
- Can still have events
- Less demanding than manual
- Good compromise

---

## 8. DISCOVERY MECHANICS

### 8.1 "Land Ho!" Moments

**Crew Announcement:**
- Lookout calls out
- Direction indicated
- Jake can choose to investigate

**Island Categories:**

**Known Islands:**
- On Jake's map
- Regular destinations
- Safe to approach

**Rumored Islands:**
- Hints heard in taverns
- Approximate location
- Reward for exploration

**Uncharted Islands:**
- Completely unknown
- Found through wandering
- Major discovery rewards
- Story potential

### 8.2 Sea Wonders

**Floating Objects:**
- Wreckage from battles
- Lost cargo
- Personal effects from tragedies
- Message in bottles

**Sea Creatures:**
- Whale pods (majestic)
- Dolphin schools (playful)
- Sharks (dangerous)
- Massive squid (rare, threatening)

**Weather Phenomena:**
- Waterspouts
- Rainbow over ocean
- Green flash at sunset
- Northern lights (Fjordlund)

**Mysterious Sights:**
- Ghost ships (rare encounters)
- Mermaid glimpses (unconfirmed)
- Underwater glows
- Sea-mother's wake

### 8.3 Hidden Areas

**Beneath Clear Water:**
- Sunken ships visible
- Ancient ruins
- Treasure glints
- Dive to investigate (Chapter 5+)

**Coastal Secrets:**
- Hidden coves
- Smuggler caves
- Secret ports
- Abandoned settlements

**Deep Ocean:**
- Things that should not be
- Deep trenches (danger)
- Unknown islands
- Region 06 access

### 8.4 Discovery Rewards

**Experience:**
- Each discovery adds to logs
- Esteban updates charts
- Jake's journal entries
- Satisfaction of exploration

**Material Rewards:**
- Treasure caches
- Unique items
- Rare materials
- Information

**Story Content:**
- New questlines
- Character backstories
- Lore revelations
- World-building depth

---

## 9. TRAVEL EVENTS

### 9.1 Random Encounters

**Distress Calls:**
- Ship in trouble
- Jake can help or ignore
- Reputation consequences
- Potential rewards

**Pirate Attacks:**
- Being pursued
- Naval combat triggered
- Escape or fight decision

**Imperial Patrols:**
- Checkpoint situations
- Documentation requests
- Bribes possible
- Combat if bad reputation

**Lost Sailors:**
- Floating survivors
- Must be rescued
- Story potential
- Crew additions possible

### 9.2 Fishing

**Fishing Mechanics:**
- Available at slow speed
- Various fish types
- Regional specialties
- Calm activity

**Fishing Rewards:**
- Fresh food for crew
- Trade goods
- Rare fish for recipes
- Occasional treasures

**Fishing Partners:**
- Crew can join
- Relaxed conversations
- Bonding moments

### 9.3 Whale Watching

**Encounter Whales:**
- Peaceful migrations
- Don't attack unless provoked
- Spectacular to observe

**Whaling (Moral Choice):**
- Possible but consequences
- Fjordlund has traditions
- Peace route with Havfrue = never whale
- Crew reactions vary

### 9.4 Pirate Activities

**Raiding Merchants:**
- Select targets carefully
- Imperial response possible
- Good profits
- Moral weight

**Escorting Others:**
- Protect merchant ships
- Legitimate earnings
- Reputation building

**Treasure Hunting:**
- Follow treasure maps
- X marks the spot
- Pirate traditions

---

## 10. REGIONAL SAILING CHARACTERISTICS

### 10.1 Caribbean (Rexa, Moran)

**Weather:**
- Warm, tropical
- Hurricane season (dangerous)
- Frequent calm periods
- Afternoon storms

**Crew Mood:**
- Cheerful in warmth
- Active shanty singing
- Good working conditions

**Visual Style:**
- Brilliant blues
- White sand beaches
- Lush islands
- Clear water visibility

### 10.2 Fjordlund (Northern)

**Weather:**
- Cold, windy
- Fog common
- Ice hazards in winter
- Short days in season

**Crew Mood:**
- More subdued
- Warmer clothing
- Hot drinks important
- Morale maintenance needed

**Visual Style:**
- Deep greens and grays
- Dramatic cliffs
- Northern lights possible
- Misty atmosphere

### 10.3 Ashenmoor

**Weather:**
- Hot, dry winds
- Volcanic ash possible
- Dust storms
- Clear desert nights

**Crew Mood:**
- Cautious near volcanic activity
- Water conservation important
- Unique landscape wonder

**Visual Style:**
- Red and orange tones
- Dramatic lighting
- Ash clouds
- Ancient geological features

### 10.4 Open Ocean

**Weather:**
- Most variable
- Weather systems visible from distance
- Long stretches of similar conditions
- Remoteness

**Crew Mood:**
- Can become restless
- Shanties important for morale
- Stories and games
- Crew bonding time

**Visual Style:**
- Endless horizons
- Sky dominates
- Isolation beauty
- Stars at night

---

## 11. TIME AT SEA

### 11.1 Voyage Duration

**Short Voyages (Under 1 hour real time):**
- Port to port within region
- Quick missions
- Minimal events

**Medium Voyages (1-3 hours):**
- Cross-regional travel
- Multiple crew conversations
- Potential encounters
- Standard sailing

**Long Voyages (3+ hours):**
- Major transitions (Rexa to Fjordlund)
- Deep relationship development
- Major story beats
- Extensive events

### 11.2 Voyage Pacing

**Beginning:**
- Departure preparations
- Initial setting of course
- Crew settles into routine
- Fair-winds optimism

**Middle:**
- Deep sailing rhythm
- Conversations emerge
- Weather events possible
- Discovery opportunities

**End:**
- Land sighting excitement
- Arrival preparations
- Crew enthusiasm
- Port approach

### 11.3 Speeding Up Time

**Sleep in Cabin:**
- Time advances
- Crew continues sailing
- Wake at chosen time
- Arrive at destination

**Guided Travel:**
- Accelerated but experienced
- Events still happen
- Can witness moments

**Full Fast Travel:**
- Instant arrival
- Miss everything between
- Available but penalized

---

## 12. NAVIGATION UI

### 12.1 Helm UI

**On-Screen Elements:**
- Wind direction indicator
- Speed gauge
- Sail configuration
- Compass heading
- Health status (ship)

**Minimal During Sailing:**
- Reduces when steady
- Expands when needed
- Non-intrusive

### 12.2 Map Interface

**World Map:**
- All known regions
- Travel routes visible
- Current position marked
- Weather patterns shown

**Regional Maps:**
- Detailed coastlines
- Known ports
- Discoveries noted
- Hazards marked

**Nautical Charts:**
- Depth indicators
- Current directions
- Wind patterns (historical)
- Navigation hazards

### 12.3 Status Displays

**Ship Status:**
- Hull integrity
- Crew morale (aggregate)
- Supplies level
- Cargo weight

**Position Indicators:**
- Latitude/longitude
- Nearest landmark
- Estimated arrival
- Current speed

### 12.4 Discovery UI

**New Discovery:**
- Subtle notification
- Added to journal
- Map updates
- Jake's reaction

**Exploration Progress:**
- Percentage of region explored
- Hidden areas remaining
- Achievements tracking

---

## 13. PLATFORM-SPECIFIC FEATURES

### 13.1 PS5 DualSense

**Haptic at Helm:**
- Wheel resistance varies with conditions
- Wave impacts felt
- Wind changes sensed
- Storm turbulence

**Audio:**
- 3D positional wind
- Sail snaps and creaks
- Crew positions audible
- Shanty harmonies

**Adaptive Triggers:**
- Sail adjustment resistance
- Heavy in storms
- Light in calm
- Physical feedback

### 13.2 Xbox

**Rumble:**
- Wave impact feedback
- Storm effects
- Combat transitions

### 13.3 PC

**Keyboard/Mouse:**
- Alternate control scheme
- Mouse for wheel control
- Hotkeys for sail configurations

---

## 14. ACCESSIBILITY

### 14.1 Sailing Assistance

**Autopilot Options:**
- Full autopilot available
- Semi-assist modes
- Manual for enthusiasts
- Seamless switching

**Wind Simplification:**
- Always favorable wind option
- Simplified tacking
- Reduced complexity

### 14.2 Visual Accessibility

**Wind Indicators:**
- Enhanced visuals
- Color-blind options
- Audio cues for direction
- Large UI mode

**Horizon Line:**
- Motion sickness considerations
- Stabilization options
- Reduced camera movement

### 14.3 Audio Accessibility

**Shanty Volume:**
- Separate audio channel
- Lyrics subtitles
- Translation options
- Cultural notes

### 14.4 Motor Accessibility

**Simplified Controls:**
- Single-button sailing
- Auto-tacking
- Context-sensitive actions

---

## 15. TUTORIAL PROGRESSION

### 15.1 Chapter 1

**Basic Sailing:**
- Take the helm
- Simple wind
- Short voyages
- First shanty

### 15.2 Chapter 2

**Weather Introduction:**
- First storm
- Preparation needed
- Crew coordination

### 15.3 Chapter 4

**Full Navigation:**
- Celestial introduction
- Long voyages
- Regional transitions

### 15.4 Chapter 7

**Master Sailor:**
- Complex navigation
- Storm mastery
- Deep exploration

### 15.5 Chapter 9+

**Legend of the Sea:**
- All systems mastered
- Ultimate voyages
- Discovery expert

---

## 16. SAILING FEEL TESTS

Before sailing is final, it must pass these tests:

### 16.1 The Black Flag Test
> *"Does sailing feel as good as Assassin's Creed Black Flag? Do shanties move players emotionally?"*

### 16.2 The Meditation Test
> *"Can sailing be meditative? Do players enjoy simply being at sea?"*

### 16.3 The Discovery Test
> *"Does manual sailing reward exploration? Are discoveries meaningful?"*

### 16.4 The Crew Life Test
> *"Does the ship feel lived-in during voyages? Do crew feel alive?"*

### 16.5 The Weather Test
> *"Does weather matter? Do players fear storms and love fair winds?"*

All five must pass.

---

## 17. IMPLEMENTATION NOTES

### 17.1 Technical Requirements
- Dynamic weather system
- Wind simulation
- Ocean rendering (waves, currents)
- Crew AI for routines
- Shanty audio system

### 17.2 Content Requirements
- 47 unique shanties
- Multiple discovery locations
- Random encounter variety
- Regional sailing characteristics

### 17.3 Audio Requirements
- Full shanty implementations
- Environmental audio (wind, waves)
- Crew voice calls
- Spatial audio design

### 17.4 Balance Requirements
- Manual vs. fast travel incentives
- Storm difficulty tuning
- Discovery reward scaling
- Voyage pacing

---

## 18. INTEGRATION WITH EXISTING SYSTEMS

### 18.1 Naval Combat

- Sailing transitions smoothly to combat
- Wind continues to matter
- Crew roles shift
- Sail management critical

### 18.2 Ship Interior

- Helm is part of ship interior
- Crew moves between spaces
- Cabin accessible during sailing
- Continuous experience

### 18.3 Story System

- Voyages framework story beats
- Character development at sea
- Relationship deepening
- Narrative pacing

### 18.4 Weather (General)

- Sea weather affects land weather
- Regional climate consistency
- Weather-based events
- Atmospheric continuity

---

## 19. THE SAILING DREAM

At its best, Dark Arisen's sailing captures a specific feeling: Jake at the helm, Big Tom smoking his pipe on the foredeck, Mira at Jake's side reading the wind, the crew singing "The Wind's Own Daughter" across the deck, the sun setting in colors that don't exist on land, the ship cutting through calm blue water toward an island Jake has been seeking for three chapters.

That moment. That exact moment. That is what this system exists to create.

**Black Flag did this. Dark Arisen must do it better — with deeper character, richer music, more meaningful discovery.**

---

## 20. NEXT STEPS

Next document: **`mechanics/ship_management.md`** — FINAL SUBSYSTEM

Will cover:
- Ship upgrades and economics
- Crew management (hiring, firing, development)
- Repair systems
- Fleet command (late game)
- Port interactions
- Economic systems

After Ship Management: **Phase 3 Mechanics COMPLETE (12 of 12 subsystems).**

**Phase 3 Subsystem 10 of 11 — complete. One more and Mechanics is finished.**
