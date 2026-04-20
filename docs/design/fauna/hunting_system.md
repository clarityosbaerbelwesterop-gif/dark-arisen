# HUNTING SYSTEM

**Status:** Design Draft v1.0  
**Phase:** Phase 4 — Fauna & Flora (Subsystem 1 of 14)  
**Tier:** Foundation Tier  
**Related:** fauna_overview.md, mechanics/combat_feel.md, mechanics/stealth.md, mechanics/inventory_crafting.md, style_bible.md  
**Purpose:** Define the complete hunting experience — tracking, stalking, killing, processing. The foundation for all animal interactions in Dark Arisen.  
**Inspirations:** Red Dead Redemption 2 (primary — hunting feel, tracking, skinning), Monster Hunter (quality-based loot), Far Cry Primal (hunting as lifestyle), Assassin's Creed Origins (hunt variety), Ghost of Tsushima (respectful killing aesthetic), Horizon Zero Dawn (trap hunting)

---

## 1. HUNTING PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: The Hunt Is a Story

Every hunt in Dark Arisen is a small narrative with beginning, middle, and end:
- **Beginning:** Finding tracks, understanding the animal
- **Middle:** The stalk, the approach, the patience
- **End:** The kill itself and its aftermath

Players who rush through hunts miss the point. The system rewards patience and observation.

**Design intent:** A hunt is not a transaction. It is an experience.

### 1.2 Principle 2: Animals Are Not Enemies

Animals in Dark Arisen are not hostile mobs. They are creatures living their lives. Most flee from Jake. Some ignore him. A few attack, but only if threatened or defending territory/young. This is not combat — this is predation, and Jake is the predator.

**Design intent:** The relationship between hunter and hunted is fundamentally different from combat. The emotional weight is different. The game should reflect this.

### 1.3 Principle 3: Quality Reflects Skill

How Jake kills an animal determines what he gets:
- **Clean kill (headshot, first try):** Pristine pelt, maximum meat, best materials
- **Multiple shots:** Damaged pelt, reduced materials
- **Wounded and fled:** Potential loss of animal entirely
- **Overkill (excessive weapons):** Destroyed materials

**Design intent:** The system rewards skill and respect. A perfect hunt is beautiful. A botched hunt is wasteful.

### 1.4 Principle 4: Use the Whole Animal

Every animal provides multiple resources:
- Meat (food)
- Hide (leather, armor)
- Bones (tools, crafting)
- Fat (oil, preservation)
- Organs (alchemy)
- Teeth/Claws (decoration, crafting)
- Feathers (arrows, decoration)

Jake can take everything or just what he needs. Taking only meat and leaving the carcass has moral and ecological weight. Some NPCs notice.

**Design intent:** The pirate era respected whole-animal use out of economic necessity. Dark Arisen teaches this through mechanical reward.

### 1.5 Principle 5: The Ecosystem Notices

Hunting affects regional populations:
- Over-hunting reduces animal density
- Populations recover over time
- Some animals become scarce in over-hunted areas
- Legendary animals appear in pristine ecosystems

**Design intent:** Actions have consequences beyond the immediate kill. Players who hunt sustainably are rewarded with thriving ecosystems.

---

## 2. THE TRACKING SYSTEM

### 2.1 Finding Tracks

Animals leave evidence of their passage:

**Footprints:**
- Size indicates species and size
- Depth indicates weight
- Spacing indicates gait (walking, running)
- Freshness indicates how long ago
- Pattern indicates direction

**Scat:**
- Species-specific
- Freshness indicates recency
- Content indicates diet
- Can lead to feeding grounds

**Broken Vegetation:**
- Bent grass
- Broken branches
- Disturbed leaves
- Indicates passage direction

**Blood (from Wounded Prey):**
- Bright red = fresh injury
- Dark red = older
- Amount indicates wound severity
- Trail can be followed

**Feeding Signs:**
- Stripped bark (deer, rabbits)
- Dug earth (boars)
- Scattered feathers (predator kill)
- Cracked bones (hyenas, wolves)

**Resting Spots:**
- Matted grass
- Hair/fur left behind
- Ground impressions
- Temperature check (recent occupation)

### 2.2 Tracking Mechanics

**Tracking Mode:**
- Activated by holding L3 while not in stealth
- Screen shifts to enhanced perception
- Tracks become visible (outlined)
- Audio cues highlighted
- Smell indicators appear (wind direction shown)

**Tracking Skill:**
- Grows with use (meta-skill)
- Chapter 1: Basic tracks visible only
- Chapter 4: Complex tracks readable
- Chapter 7: Expert-level tracking
- Chapter 9+: Master tracker (sees everything)

**Environmental Factors:**
- Rain washes tracks (tracking harder)
- Snow reveals everything (easy in Fjordlund)
- Sand holds tracks well (Ashenmoor)
- Jungle floor hides them (Rexa — tracking harder)
- Time of day (morning tracks clearer)

### 2.3 Reading the Land

Beyond tracks, the landscape tells stories:

**Watering Holes:**
- Animals visit regularly
- Best ambush locations
- Dawn and dusk peak activity
- Different species at different times

**Feeding Grounds:**
- Berries in clearings
- Grass meadows
- Fruit trees
- Specific to herbivores

**Hunting Trails:**
- Predator paths
- Well-worn routes
- Territorial boundaries
- Dangerous to walk

**Nesting/Den Areas:**
- Rocky outcrops (big cats)
- Hollow trees (small animals)
- Caves (bears, wolves)
- Ground burrows (rabbits, foxes)

### 2.4 Scent and Wind

**Wind Direction:**
- Always shown in tracking mode
- Animals smell Jake from downwind
- Must approach from upwind or crosswind
- Wind shifts affect strategy

**Jake's Scent:**
- Default strong
- Can be reduced with:
  - Mud bath (tropical regions)
  - Specific herbs rubbed on
  - Animal fat smearing
  - Specific potions from Ines
- Reduces detection range significantly

**Animal Scent Awareness:**
- Herbivores more alert
- Predators confident (less fleeing)
- Nocturnal animals very sensitive
- Diurnal animals less sensitive

### 2.5 Time of Day

Different animals active at different times:

**Dawn (5-7 AM):**
- Deer feeding
- Birds most active
- Predators returning to dens
- Best tracking light

**Morning (7-11 AM):**
- Mixed activity
- Most hunting possible
- Good visibility

**Midday (11 AM-3 PM):**
- Animals rest
- Hot regions: heat-seeking shade
- Hunting reduced
- Tracking good in sand/dust

**Afternoon (3-6 PM):**
- Activity increasing
- Second feeding time
- Good hunt window

**Evening (6-9 PM):**
- Peak activity
- Many species converge at water
- Best hunt window overall
- Beautiful lighting

**Night (9 PM-5 AM):**
- Nocturnal species
- Predators active
- Dangerous for Jake
- Torch/fire needed
- Some specific legendary hunts only possible

---

## 3. THE STALK

### 3.1 Approach Distance

Every species has detection ranges:

**Deer:** 40m sight, 60m smell, 25m hearing
**Boar:** 30m sight, 80m smell, 40m hearing
**Wolf:** 50m sight, 100m smell, 70m hearing
**Bear:** 40m sight, 120m smell, 50m hearing
**Rabbit:** 60m sight, 30m smell, 40m hearing
**Large Cat:** 80m sight, 70m smell, 60m hearing

Ranges modified by:
- Jake's movement speed
- Wind direction
- Cover used
- Noise created
- Jake's scent level

### 3.2 Stealth for Hunting

Uses same stealth system from `mechanics/stealth.md`:
- Crouch movement
- Cover usage
- Noise discipline
- Eyeline awareness

**Hunting-Specific Stealth:**
- Crawling (extremely slow but silent)
- Freeze state (Jake doesn't move at all, animals don't detect motion)
- Using distractions (throwing small objects)
- Tree/rock cover as blinds

### 3.3 The Critical Distance

**Weapon Effective Ranges:**

**Bow (Primary Hunting):**
- Best: 15-40m
- Max: 80m (accuracy drops)
- Silent, clean
- Multiple arrow types

**Crossbow:**
- Best: 10-30m
- Max: 60m
- Higher damage, slower reload
- Good for tough animals

**Rifle (when available, Chapter 7+):**
- Best: 50-150m
- Max: 300m
- Loud (spooks other animals)
- Highest damage

**Throwing (Short Range):**
- Best: 5-15m
- Knives, hatchets, spears
- Silent
- Limited

**Close-Combat (Dangerous):**
- Finishing kills on wounded animals
- Mercy kills
- Some specific hunts require
- Bloodborne-tier immersion

### 3.4 Patience Over Speed

The game rewards waiting:

**Observation Time:**
- Watch animal behavior for 30+ seconds
- Learn patterns
- Identify vulnerabilities
- Best shot moment appears

**Animal Routines:**
- Drinking at water (head down, vulnerable)
- Feeding (focused, less aware)
- Resting (deep sleep states)
- Mating (distracted)

**Rushed Hunts Fail:**
- Spooked animals flee
- Panic shots miss
- Wounded animals run far
- Track becomes hard to follow

---

## 4. THE KILL

### 4.1 Shot Placement

**Headshot Zones:**
- Brain: Instant kill, perfect materials
- Eyes: Difficult but devastating
- Temple: Clean kill

**Vital Zones:**
- Heart: Clean, quick kill
- Lungs: Death in seconds
- Major arteries: Bleeding death

**Wounding Shots:**
- Leg shots: Animal can't flee but suffers
- Belly: Long painful death
- Shoulders: Reduced mobility

**Design Intent:** Good hunters aim for clean kills. The game UI highlights vital zones when aiming carefully (hold aim 2+ seconds).

### 4.2 Weapon-Animal Matching

Not every weapon suits every animal:

**Small Game (Rabbits, Birds):**
- Small arrows, slings
- Shotguns if available
- Over-weaponing = destroyed meat

**Medium Game (Deer, Boar):**
- Standard bow, crossbow
- Rifle works
- Knife throwing for experts

**Large Game (Bear, Bull):**
- Heavy bow, strong arrows
- Rifle ideal
- Multiple hits often needed

**Predators (Wolves, Cats):**
- Heavy weapons
- Rifle preferred
- Dangerous at close range

**Sea Game (Fish, Whales):**
- Specialized (harpoons)
- Covered in Sea Animals document
- Different system entirely

### 4.3 Multiple Target Situations

**Herds/Groups:**
- First shot alerts others
- Quiet weapons preserve opportunities
- Panic response varies by species
- Strategic choice: multiple kills or focus

**Family Groups:**
- Moral weight (mothers with young)
- Some specific quests prohibit young
- Orphan consequences (ecosystem effects)

**Predator-Prey Encounters:**
- Jake can let them fight
- Weakened predator easier target
- Natural drama
- Sometimes triggers special events

### 4.4 The Mercy Kill

Wounded animals deserve quick deaths:

**Triggering:**
- Animal down but alive
- Jake approaches
- Triangle prompt: "Mercy Kill"
- Specific animation (knife to throat, usually)

**Not Doing It:**
- Animal suffers
- Reduced material quality
- Crew disapproves if they see
- Jake's morality affected

**Always Do It:**
- Respectful hunting
- Best materials
- Satisfying conclusion
- NPCs approve

### 4.5 Animal Fights Back

Dangerous animals defend themselves:

**Bears:**
- Attack if approached within 15m
- Maul Jake dealing heavy damage
- Must be downed quickly
- Very dangerous

**Big Cats (Panther, Jaguar):**
- Ambush predators
- Stalk Jake if he's careless
- Very fast
- Deadly at close range

**Boars:**
- Charge when cornered
- Tusks damaging
- Less dangerous but persistent
- Common threats

**Wolves (Pack):**
- Group attacks
- Flanking behavior
- Dangerous in numbers
- Wolfpack dynamics

**Snake Encounters:**
- Venomous varieties
- Ambush style
- Tropical/swamp regions
- Antidotes required

---

## 5. SKINNING AND PROCESSING

### 5.1 The Skinning Ritual

After the kill, approach the body:

**Triangle Prompt:** "Skin and Process"

**Animation:**
- Jake kneels beside animal
- Takes out hunting knife
- Begins careful work
- Duration: 30-90 seconds depending on size
- Jake hums sometimes (becomes more common with mastery)

**During Animation:**
- Can be cancelled if threatened
- Players can watch full or speed up
- Sounds realistic
- Jake's expressions show focus, respect

### 5.2 Processing Quality

**Three Quality Tiers:**

**Perfect (Pristine):**
- Requires: Clean kill + correct weapon + appropriate skill level
- Provides: Maximum materials, legendary crafting possible
- Visual: Unblemished pelt, perfect meat cuts

**Good (Standard):**
- Most hunts result in this
- Provides: Standard materials
- Visual: Minor imperfections

**Poor (Damaged):**
- Multiple shots, bad aim, wrong weapon
- Provides: Reduced materials
- Visual: Tears, blood stains, damage

### 5.3 What Jake Gets

Per animal, depending on size:

**Small Animal (Rabbit):**
- 1-2 Small Pelt
- 1-2 Rabbit Meat
- 1 Rabbit Foot (luck charm)
- Bones (small)

**Medium Animal (Deer):**
- 1 Standard Pelt (quality varies)
- 3-5 Venison Cuts
- Antlers (if male)
- Bones (medium)
- Hooves
- Fat (small amount)

**Large Animal (Bear):**
- 1 Large Pelt (premium quality)
- 5-8 Heavy Meat
- Claws (4 + teeth)
- Bones (large)
- Major Fat
- Gall Bladder (alchemy)
- Heart (special meal)

**Predator (Wolf):**
- 1 Wolf Pelt
- 2-3 Lean Meat
- Teeth (canines for jewelry)
- Bones (medium)
- Claws
- Small Fat

### 5.4 Partial Processing

Jake can choose what to take:

**Full Process:** Everything useful
**Quick Process:** Meat and pelt only (faster)
**Minimum:** Just meat
**Symbolic:** Trophy only (antler, claw)

**Design Intent:** Players with full inventory need to choose wisely. Leaving material has consequences (scavengers come, meat rots, trail of evidence).

### 5.5 Tools Required

**Hunting Knife:**
- Basic skinning
- Quality affects result
- Can be upgraded

**Butchering Kit (Advanced):**
- Better cuts
- More materials extracted
- Improved quality
- Big Tom crafts

**Field Dressing Tools:**
- For large animals
- Efficient processing
- Crew help possible

---

## 6. HUNTING FROM THE SHIP

### 6.1 The Unique Dimension

This is where Dark Arisen innovates beyond RDR2:

**Coastal Hunting:**
- Approach beaches/coasts
- Spot animals from ship
- Send landing party
- Tactical approach

**Sea-Level Hunts:**
- Seals from ship deck
- Sea birds (for feathers/meat)
- Flying fish (jumping at ship)
- Dolphin pods (usually protected)

**Whaling (Specific):**
- Covered in Sea Animals document
- Harpoon-based
- Crew coordination
- Moral weight

### 6.2 Ship Hunting Mechanics

**Spotter in Crow's Nest:**
- Dedicated crew position
- Calls out sightings
- "Seals ashore, port side, 2 miles!"
- Jake decides to investigate

**Landing Party:**
- Jake + 2-4 crew
- Take rowboat to shore
- Hunt on foot
- Return with game

**Ship Weapons for Hunting:**
- Swivel guns (close range)
- Rifle from deck (distance)
- Harpoons (sea game)
- NOT cannons (destroys everything)

### 6.3 Strategic Hunting Missions

**Meat Runs:**
- Extended hunting trips
- Restock ship supplies
- Crew morale important
- Multi-day expeditions

**Specialty Hunts:**
- Seeking specific materials
- For crafting needs
- Long-term planning
- Expedition structure

### 6.4 Dangers of Ship Hunting

**Sharks at Sea:**
- Attracted by blood in water
- Dangerous when whaling
- Require coordinated defense

**Territorial Animals:**
- Bull elephant seals (Fjordlund)
- Protective mothers
- Ambush risks

**Weather:**
- Storms make hunting impossible
- Must return to ship
- Risk assessment

---

## 7. REGIONAL HUNTING

### 7.1 Rexa / Moran (Tropical)

**Main Quarry:**
- Wild boars
- Jungle deer (small)
- Monkeys (controversial)
- Parrots (feathers)
- Large reptiles (crocodilians)
- Jaguars (rare, legendary)

**Hunting Style:**
- Jungle stalking
- Dense vegetation
- Limited visibility
- Humidity affects tracks
- Venomous dangers (snakes, spiders)

**Cultural Notes:**
- Rexan tradition respects the hunt
- Festivals celebrate hunters
- Boar hunts are communal
- Some animals are taboo (specific cultural)

### 7.2 Fjordlund (Arctic/Cold)

**Main Quarry:**
- Reindeer/caribou
- Arctic foxes (premium pelts)
- Seals (multiple species)
- Whales (specialized)
- Brown bears
- Moose (inland)
- Lynx (rare)

**Hunting Style:**
- Open terrain
- Long sight lines
- Snow reveals tracks
- Cold endurance needed
- Daylight scarce in winter

**Cultural Notes:**
- Spiritual dimension to hunting
- Respect for prey spirits
- Whaling is sacred (Peace Route)
- Wasting is deeply taboo

### 7.3 Ashenmoor (Volcanic Desert)

**Main Quarry:**
- Desert antelope
- Ash-colored wolves (unique)
- Giant lizards
- Hardy goats
- Desert birds
- Volcanic cave creatures (legendary)

**Hunting Style:**
- Heat management
- Water scarcity
- Long-distance shots
- Sand obscures tracks (unless fresh)
- Unique shadow-use

**Cultural Notes:**
- Nomadic hunting traditions
- Desert survival skills integrated
- Some animals considered spirits

### 7.4 Pale Isle (Imperial)

**Main Quarry:**
- Domestic/semi-wild (managed)
- Imperial stags (royal hunting)
- Red foxes
- Hunting dogs (escaped, feral)
- Pheasants
- Rabbits

**Hunting Style:**
- Managed forests
- Potentially illegal (poaching)
- Imperial patrols
- Class implications (noble hunts)
- Specific protocols

**Cultural Notes:**
- Hunting as aristocratic activity
- Poaching severely punished
- Jake must be careful here

### 7.5 Quiet Coast (Temperate)

**Main Quarry:**
- Whitetail deer
- Turkeys
- Ducks (seasonal)
- Bears
- Rabbits
- Small game

**Hunting Style:**
- Classic forest hunting
- Mixed terrain
- Temperate weather
- RDR2-comparable experience
- Most familiar to players

**Cultural Notes:**
- Settler hunting traditions
- Community importance
- Economic necessity

### 7.6 Region 06 (Post-Game)

**Main Quarry:**
- Unique creatures that shouldn't exist
- Things evolution forgot
- Philosophical fauna
- Legendary tier

**Hunting Style:**
- Reality-bending
- Rules change
- Unique mechanics per creature
- Special rewards

---

## 8. LEGENDARY HUNTS

### 8.1 The Legendary System

Like RDR2, Dark Arisen features legendary animals:

**Characteristics:**
- Unique, named individuals
- One per region (main ones)
- Vastly larger/older than regular specimens
- Unique visual (scars, coloration, size)
- Persistent after normal animals respawn
- Unique reward materials

### 8.2 Examples of Legendary Animals

**The Patriarch (Rexa Jungles):**
- Massive jaguar, 40% larger than normal
- Estimated 20+ years old
- Extensive scarring
- Hunts specific areas at night
- Reward: Legendary Jaguar Pelt (unique armor)

**Old Iron-Back (Quiet Coast):**
- Massive grizzly bear
- Territorial, aggressive
- Rumored to have killed hunters
- Reward: Iron-Back Pelt Cloak

**Ghost Reindeer (Fjordlund):**
- Pure white caribou
- Mystical presence
- Only appears during northern lights
- Reward: Ghost Antlers (alchemical potency)

**The Ash-Shadow (Ashenmoor):**
- Black wolf, completely silent
- Volcanic region predator
- Stalks Jake more than he stalks it
- Reward: Shadow-Wolf Teeth (weapon enhancement)

**The Emperor's Stag (Pale Isle):**
- 14-point imperial stag
- Illegal to hunt
- Heavily protected
- Reward: Imperial Antlers + massive reputation consequences

**The Scarred Leviathan (Various Seas):**
- Ancient sperm whale
- Evidence of past whaling survived
- Deeply respectful hunt only
- Reward: Leviathan Bone (unique legendary weapon material)

### 8.3 Legendary Hunt Mechanics

**Discovery:**
- Hints in towns
- Tracks in specific areas
- NPCs discuss rumors
- Pattern recognition required

**The Hunt:**
- Extended tracking (often 10+ minutes game time)
- Multiple locations needed
- Specific times of day
- Perfect shot required (multiple encounters if botched)

**The Kill:**
- Must be perfect
- Wrong weapon = escape
- Wrong time = escape
- Multiple attempts possible but each harder

**The Reward:**
- Unique materials
- Crafting components for legendary items
- Specific NPC reactions
- Story consequences sometimes

### 8.4 Narrative Integration

Legendary animals sometimes tied to story:

**The Scarred Leviathan** — Connected to Havfrue Modor (the whale's family was affected by whaling Don Mateo's ancestors did)

**The Ghost Reindeer** — Connected to Fjordlund Peace Route (appears only if Havfrue respected)

**The Ash-Shadow** — Connected to Ashen Wyrm (the dragon's mythology)

---

## 9. SUSTAINABILITY AND ECOSYSTEM

### 9.1 Population Tracking

Each region tracks animal populations:

**Healthy Ecosystem:**
- 100% population
- Legendary animals present
- Diverse species visible
- Predator-prey balance

**Declining Ecosystem:**
- 60-80% population
- Legendary hidden
- Reduced sightings
- Noticeable to attentive players

**Damaged Ecosystem:**
- 30-60% population
- Specific species gone
- Environmental consequences
- NPCs comment

**Devastated Ecosystem:**
- 0-30% population
- Almost no wildlife
- Long recovery time
- Major NPC reactions

### 9.2 Recovery Mechanics

**Natural Regeneration:**
- Populations slowly recover
- Chapter progression = time passing
- Fastest with ecosystem health

**Accelerated Recovery:**
- Some quests help ecosystems
- Peace Route with Havfrue affects whale populations
- Protective actions noticed

**Permanent Consequences:**
- Legendary animals killed don't respawn
- Entire species can be locally extinct
- Jake's reputation as "the bloody captain" possible

### 9.3 Jake's Hunting Reputation

**Hunter Types (How World Sees Jake):**

**Respectful Hunter:**
- Clean kills
- Whole-animal use
- Sustainable practices
- NPCs approve, prices reduce, special quests unlock

**Professional Hunter:**
- Efficient, effective
- Standard practices
- Neutral reputation
- Business-as-usual

**Butcher:**
- Over-hunting
- Wasteful practices
- Destroyed pelts
- NPCs judge harshly, prices rise

**Ecosystem Destroyer:**
- Wiping out populations
- Killing for killing
- Major consequences
- Specific quest lines trigger (repentance or doom)

### 9.4 Ethical Hunting Quests

Some NPCs offer ethical hunting quests:

**The Old Hunter's Wisdom (Rexa):**
- Master hunter teaches Jake
- Respectful practices rewarded
- Secret techniques taught
- Long-term tutorial

**The Whale Song (Fjordlund):**
- Working WITH whales, not hunting
- Spiritual practices
- Peace Route content
- Unique rewards

---

## 10. COOKING FROM HUNTS

(Brief overview — full system in `fauna/processing_and_cooking.md`)

### 10.1 Cooking Tiers

**Campfire (Basic):**
- Simple meat cooking
- Minimal skill required
- Available anywhere
- Basic food buffs

**Ship Galley (Intermediate):**
- Better food
- Seasoning available
- Crew benefits
- Recipes learnable

**Inn/Tavern (Expert):**
- Master-level cooking
- Unique recipes
- Significant buffs
- Social element

### 10.2 Meat Quality Impact

Quality from hunt = quality meal:

- **Perfect Pelt → Perfect Cuts → Legendary Meal**
- **Good Pelt → Standard Cuts → Good Meal**
- **Poor Pelt → Damaged Cuts → Basic Meal**

### 10.3 Specific Hunt → Specific Recipe

Some recipes require specific hunts:

**Wild Boar Roast:** Requires Wild Boar
**Venison Stew:** Requires Deer
**Bear Heart Stew:** Requires Bear Heart (rare drop)
**Legendary Jaguar Curry:** Requires Legendary Jaguar meat
**Whale Blubber Pie:** Requires sustainable whale hunt

---

## 11. HUNTING TOOLS AND EQUIPMENT

### 11.1 Bows

**Basic Bow:**
- Available Chapter 1
- Standard hunting weapon
- Simple ammunition
- Silent

**Recurve Bow:**
- Available Chapter 4
- +25% damage, range
- Better accuracy
- Recommended upgrade

**Compound Bow:**
- Available Chapter 7
- Advanced mechanism
- Best ranged hunting bow
- High skill threshold

**Legendary Bows:**
- Unique items
- Specific stats
- Associated with master hunters
- Post-game tier

### 11.2 Arrow Types

**Standard Arrows:**
- General purpose
- Moderate damage
- Retrievable

**Small Game Arrows:**
- Less damage
- Preserves small animal bodies
- Required for quality small-game hunting

**Poison Arrows:**
- Slow kill but certain
- Tracking advantage (blood trail + slowing)
- Ines crafts
- Ethical questions

**Fire Arrows:**
- Panic effect on animals
- Trigger stampedes
- Sometimes useful
- Ecosystem damage

### 11.3 Firearms

**Flintlock Musket:**
- Available Chapter 4
- High damage
- Very loud
- Slow reload

**Rifled Musket:**
- Available Chapter 7
- Longer range, accuracy
- Still loud
- Better ammunition

**Precision Rifle (Rare):**
- Imperial military weapon
- Highest accuracy
- Legendary-tier weapon
- Chapter 8+ availability

### 11.4 Specialty Tools

**Traps:**
- Snares (small game)
- Pit traps (medium game)
- Bear traps (large game)
- Requires setup time, rewards patience

**Decoys:**
- Wooden duck decoys
- Deer calls
- Predator lures
- Sophisticated hunting

**Binoculars/Spyglass:**
- Long-range spotting
- Identify species at distance
- Plan approaches
- Ship integration

### 11.5 Clothing and Camouflage

**Hunting Attire:**
- Specific armor set
- Reduced detection
- Climate-appropriate
- Regional variants

**Camouflage:**
- Applied to Jake
- Mud, leaves, branches
- Very effective
- Temporary

**Ghillie Suit:**
- Maximum concealment
- Advanced hunting gear
- Slow movement
- Specific crafting

---

## 12. HUNTING COMPANIONS

### 12.1 Crew as Hunters

**Mira:**
- Natural huntress (Fjordlund heritage)
- Excellent with bow
- Teaches techniques
- Companion for important hunts

**Big Tom:**
- Former poacher (his backstory)
- Trap expert
- Teaches advanced techniques
- Cheerful companion

**Ines:**
- Doesn't hunt but processes
- Best skinner
- Extracts maximum materials
- Alchemy from hunt

### 12.2 Hunting Dogs

**Post-Chapter 5 Addition:**
Jake can acquire a hunting dog:

**Options:**
- **Boswell** — imperial hound (noble, reserved)
- **Luna** — wolf-hybrid (from Fjordlund friend)
- **Chilo** — Rexan mutt (loyal, playful)
- **Silver** — Ashenmoor sight hound (fast, quiet)

**Dog Capabilities:**
- Tracks game
- Flushes hidden animals
- Protects from threats
- Emotional companion

### 12.3 NPC Hunters

**Local Hunters:**
- Can be hired temporarily
- Teach regional techniques
- Companion quests
- Depth characters

---

## 13. ACCESSIBILITY

### 13.1 Motor Accessibility

- Auto-tracking option
- Simplified stalking
- Guided kill system
- Reduced precision requirements

### 13.2 Visual Accessibility

- Enhanced tracking visualizations
- Audio cue priority
- Colorblind-friendly indicators
- Size scaling

### 13.3 Ethical Accessibility

**Vegetarian Mode:**
- No hunting required
- All gameplay accessible
- Alternative protein sources
- Respects player preferences

**Pacifist Options:**
- Avoid all hunting
- Game fully playable
- Alternative content paths
- Peace Route enhanced

---

## 14. TUTORIAL PROGRESSION

### 14.1 Chapter 1: First Hunt

- Mira teaches basic hunting
- Simple rabbit hunt
- Learn tracking mode
- Basic skinning

### 14.2 Chapter 2: Expansion

- First larger animal (deer)
- Quality system introduced
- Campfire cooking
- Basic recipes

### 14.3 Chapter 4: Depth

- Big Tom teaches advanced stalking
- Predator encounters
- Regional variety (Rexa jungle)
- Legendary animal introduction

### 14.4 Chapter 5-6: Mastery

- First legendary hunt
- Trap usage
- Dog companion possible
- Economic hunting

### 14.5 Chapter 7-8: Expert

- All regions hunted
- Multiple legendary hunts
- Master crafting possible
- Ecosystem awareness

### 14.6 Chapter 9-10: Legend

- All systems mastered
- Post-game preparation
- Region 06 hunts
- Master hunter status

---

## 15. HUNTING FEEL TESTS

### 15.1 The RDR2 Test
> *"Does hunting feel as satisfying as Red Dead Redemption 2's? Do kills feel earned?"*

### 15.2 The Patience Test
> *"Are players rewarded for taking time? Does rushing feel wrong?"*

### 15.3 The Respect Test
> *"Do players feel the weight of the lives they take? Does the game honor the animals?"*

### 15.4 The Quality Test
> *"Does skill genuinely affect rewards? Is there mechanical incentive to hunt well?"*

### 15.5 The Regional Test
> *"Do hunts feel different in different regions? Is there regional identity?"*

All five must pass.

---

## 16. IMPLEMENTATION NOTES

### 16.1 Technical Requirements
- Animal AI (behavior, detection, flight responses)
- Tracking visualization system
- Detection radius calculations
- Material quality system
- Ecosystem population tracking

### 16.2 Content Requirements
- 40+ mammal species with full behavior
- Unique animations per species
- Regional audio atmospheres
- Weather/time integration

### 16.3 Balance Requirements
- Difficulty scaling per region
- Legendary hunt tuning
- Economic balance (hunt income)
- Ethical consequence calibration

---

## 17. CONNECTION TO OTHER SYSTEMS

### 17.1 Inventory & Crafting
- Materials from hunts feed crafting
- Quality affects crafting quality
- Storage considerations

### 17.2 Combat System
- Similar core (aiming, damage) different feel
- Weapons overlap but use differs
- Stealth principles shared

### 17.3 Stealth System
- Direct integration
- Hunting uses stealth core
- Detection mechanics similar

### 17.4 Interaction System
- NPCs discuss Jake's hunting
- Reputation tracked
- Cultural responses vary

---

## 18. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** Hunters speak in Middle Register typically, rising to High for ceremonial kills, Low for casual bag counts
- ✅ **Pillar 2 (Lore Through Objects):** Trophies tell stories (Legendary Jaguar Pelt, Ghost Antlers). Specific hunts remembered
- ✅ **Pillar 3 (Layered Death):** Animal deaths vary — noble stag (Style D), rabid boar (Style B), mercy kills emphasized
- ✅ **Pillar 4 (Layered Humor):** Crew comments on Jake's hunting (Style C quirks) — Big Tom's stories, Mira's skepticism
- ✅ **Pillar 5 (Theme — Revenge):** Hunting is Jake's chance to take life cleanly, respectfully — contrast to the violence of his revenge path

---

## 19. NEXT STEPS

Next document: **`fauna/gathering_system.md`** — Foundation Tier Document 2

Will cover:
- Plant identification
- Herb harvesting
- Seasonal patterns
- Tool requirements
- Cultural/regional variety

After Gathering: **`fauna/processing_and_cooking.md`** (completing Foundation Tier).

Then begins Content Tier with Land Animals.

---

## 20. CONCLUSION

Hunting is where Jake becomes part of the world's natural order. Every clean kill is skill demonstrated. Every respectful processing is character shown. Every legendary hunt is a small epic.

RDR2 set the bar. Dark Arisen aims to match it, with the added dimension of ship-based hunting and regional variety that no game has captured at this depth.

**Phase 4 Subsystem 1 of 14 — complete.**

**The hunt begins.** 🏹🦌🐺
