# PHASE 4 — FAUNA & FLORA MASTER DOCUMENT

**Status:** Design Draft v1.0  
**Phase:** Phase 4 — Fauna & Flora (Master Overview)  
**Related:** mechanics_overview.md, mechanics/inventory_crafting.md, mechanics/interaction.md, style_bible.md  
**Purpose:** Define the complete living world of Dark Arisen — every animal, plant, and natural resource system. This document serves as the roadmap for all Phase 4 subsystems.  
**Inspirations:** Red Dead Redemption 2 (primary — fauna tier, hunting, skinning, cooking), The Witcher 3 (herbalism), Monster Hunter (material-based crafting), Far Cry 3/4 (hunting economy), Breath of the Wild (cooking systems), Green Hell (survival crafting)

---

## 1. PHASE 4 PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: The World Is Alive

Dark Arisen's world is not decorated with animals and plants. It is POPULATED by them. Every region has its own ecosystem. Every ecosystem has its own logic. Wolves hunt deer. Deer eat berries. Crows follow wolves. Everything is connected.

**Design intent:** When Jake walks through the Mire, he is moving through a real swamp. When he sails past Fjordlund's coast, he is passing a real arctic ecosystem. The world does not exist for Jake. Jake exists within the world.

### 1.2 Principle 3: Every Plant Has a Purpose

No decorative flora. Every plant Jake can interact with serves some function:
- Food (coconuts, fruits, vegetables)
- Medicine (herbs, bark extracts)
- Material (tobacco, cotton, hemp)
- Crafting (wood types, fibers)
- Narrative (specific quests)

**Design intent:** Players who pay attention to plants are rewarded. The world teaches them its systems through observation.

### 1.3 Principle 3: Hunting Has Weight

Killing animals is not trivial. Each hunt is a small story:
- Tracking the animal
- Stalking approach
- The kill itself
- Skinning and processing
- Use of every part

RDR2 proved this can be gameplay-rewarding. Dark Arisen takes the same approach but with a pirate-era twist: ship-based hunting, regional variety, trade economy integration.

**Design intent:** Players feel the life they take. They also feel the purpose of taking it.

### 1.4 Principle 4: Processing Is Craft

Raw materials become finished products through craft:
- Coconut → Coconut chips, meat, oil, shell containers
- Tobacco leaf → Cured tobacco → Rolled cigars
- Animal fat → Cooking oil → Preserved foods
- Herbs → Dried → Tinctures and medicines

Jake can do basic processing anywhere (campfire, ship galley). Advanced processing requires specific stations or crew expertise (Ines for alchemy, specialists in ports).

**Design intent:** The journey from raw material to finished product is part of the gameplay, not a menu transaction.

### 1.5 Principle 5: Regional Ecosystems Are Distinct

Each region has its own flora and fauna:
- **Rexa/Moran (Tropical):** Palms, jungle predators, reef life, monkeys, parrots
- **Fjordlund (Arctic):** Whales, seals, reindeer, boreal pines, arctic berries, whale bone
- **Ashenmoor (Volcanic desert):** Desert cacti, dragon-adjacent creatures, obsidian-colored wildlife
- **Pale Isle (Imperial):** Cultivated crops, domesticated animals, managed forests
- **Quiet Coast (Temperate):** Mixed forests, familiar wildlife, coastal creatures
- **Region 06 (Post-Game):** Unique creatures that shouldn't exist, philosophical fauna

**Design intent:** Players know where they are by what grows and lives there. Regional identity extends to the living world.

---

## 2. PHASE 4 STRUCTURE — SUBSYSTEM ROADMAP

Phase 4 will be organized into **14 subsystem documents** organized in three tiers:

### 2.1 Foundation Tier (Core Systems)

**Document 1: `fauna/hunting_system.md`**
- How hunting works mechanically
- Tracking, stalking, killing
- Weapon selection for hunts
- Difficulty scaling

**Document 2: `fauna/gathering_system.md`**
- Plant collection mechanics
- Herb identification
- Respawn systems
- Tool requirements

**Document 3: `fauna/processing_and_cooking.md`**
- Campfire mechanics
- Ship galley mechanics
- Material transformation
- Cooking recipes

### 2.2 Content Tier (The Actual Species)

**Document 4: `fauna/land_animals.md`**
- All mammals across regions
- Tracking data, behavior, drops
- Regional distributions

**Document 5: `fauna/sea_animals.md`**
- Fish species
- Whales, dolphins, sharks
- Reef life
- Monster-tier sea creatures

**Document 6: `fauna/birds.md`**
- Songbirds (flavor)
- Birds of prey (useful)
- Water birds
- Scavengers (crows, vultures)

**Document 7: `fauna/insects_and_small_creatures.md`**
- Useful insects (bees for honey, silkworms)
- Dangerous insects (tropical threats)
- Reptiles and amphibians

**Document 8: `fauna/plants_and_herbs.md`**
- Medicinal herbs
- Alchemy ingredients
- Flavor herbs
- Regional varieties

**Document 9: `fauna/trees_and_fruits.md`**
- Tree species (for wood, fruits)
- Fruit harvesting
- Palm trees (coconut focus)
- Regional variety

### 2.3 Special Systems Tier (Unique Content)

**Document 10: `fauna/tobacco_system.md`**
- Multiple tobacco varieties
- Drying process
- Cigar rolling mechanic
- **Collector Quest: 10 Cigars from Different Varieties**
- Cultural/economic significance

**Document 11: `fauna/cannabis_system.md`**
- Cannabis as game mechanic
- Growing, drying, preparation
- **Side Quest: Jake Gets High and Fights Dream Enemies**
- Consumption effects in-game
- Cultural integration

**Document 12: `fauna/regional_ecosystems.md`**
- How species interact per region
- Food webs
- Seasonal patterns
- Day/night cycles

**Document 13: `fauna/legendary_creatures.md`**
- Named unique creatures (like RDR2's legendary animals)
- Special hunts
- Unique rewards
- Story integration

**Document 14: `fauna/economy_and_trade.md`**
- Fauna products in economy
- Trade routes for animal goods
- Luxury markets
- Sustainable vs. over-hunting

---

## 3. CONNECTION TO EXISTING SYSTEMS

### 3.1 Integration with Inventory & Crafting

Phase 4 provides the CONTENT for the mechanical framework defined in `mechanics/inventory_crafting.md`:
- All materials listed in crafting recipes get detailed sources here
- Regional materials get ecosystems
- Rarity tiers get justification (why is Dragon-bone rare? Because Ashen Wyrm is a unique creature)

### 3.2 Integration with Combat

Phase 4 defines all non-human combat encounters:
- Wild animal attacks
- Defensive creatures
- Hunting-as-combat
- Ecosystem threats (venomous snakes, aggressive boars)

### 3.3 Integration with Story

Specific creatures have story weight:
- Havfrue (already a boss)
- Ashen Wyrm (already a boss)
- Legendary creatures tied to chapters
- Regional wildlife reflects regional character

### 3.4 Integration with Interaction System

- NPCs discuss local wildlife
- Hunters as quest-givers
- Fauna-based dialogue
- Environmental storytelling through animals

---

## 4. CONTENT SCALE

### 4.1 Target Numbers

**Animals:**
- 40+ mammal species
- 60+ fish species
- 30+ bird species
- 25+ insect/reptile species
- 10-15 legendary creatures

**Plants:**
- 50+ herb varieties
- 20+ tree species
- 30+ fruit varieties
- 10+ tobacco varieties
- 8+ cannabis varieties

**Total Species: ~280+ across all categories**

### 4.2 Per-Region Scale

Each region has:
- 15-25 mammal species
- 10-20 fish species
- 8-15 bird species
- 15-30 plant varieties
- 1-3 legendary creatures

**Regional distinctiveness is paramount.** A player should never confuse which region they're in based on wildlife alone.

---

## 5. TUTORIAL AND PROGRESSION

### 5.1 Chapter 1-2: Foundation

- Basic hunting (no stalking needed)
- Simple gathering (obvious herbs)
- Campfire cooking
- First recipes learned

### 5.2 Chapter 3-4: Expansion

- Tracking introduced
- Skinning quality matters
- Tobacco introduced (Rexa)
- Cannabis accessible (Rexa jungles)
- Regional variety begins

### 5.3 Chapter 5-7: Mastery

- Legendary hunts possible
- Advanced cooking
- Rare herbs accessible
- Trade economy engaged
- Cross-regional hunting expeditions

### 5.4 Chapter 8-10: Legend

- All systems mastered
- Post-game preparation
- Unique hunts
- Master chef/alchemist level

---

## 6. SPECIAL QUEST LINES PREVIEW

### 6.1 The Cigar Collector (Tobacco System)

Jake meets **Don Alejandro** in Rexa's Noble District — an aging cigar connoisseur who collects rare cigars from across the world.

**Quest:** Roll 10 perfect cigars, each from a different tobacco variety:
1. Rexa Dark Leaf (common)
2. Moran Highland (uncommon)
3. Imperial Virginian (trade-route)
4. Fjordlund Peat-Cured (unusual cold-cure)
5. Ashenmoor Volcanic Ash (rare)
6. Silvera Golden (luxury)
7. Pale Isle Sacramental (religious cultivation)
8. Quiet Coast Homegrown (regional)
9. Region 06 Spore-Touched (post-game legendary)
10. **Ethan's Recipe** (Chapter 9 revelation — Jake's brother cultivated this one)

**Reward:**
- Don Alejandro's custom humidor (permanent inventory slot for cigars)
- Legendary cigar recipe (passive +25% Rache cooldown reduction)
- Backstory dialogue revealing regional history through tobacco

**Style Bible Compliance:** Pillar 2 (Lore Through Objects) — the cigars themselves tell stories.

### 6.2 The Dream Fight (Cannabis System)

In a Rexan jungle, Jake meets **Mama Jacinta** — an old herbalist who has spent her life understanding the sacred cannabis plant.

**Quest:** She offers Jake a specific strain grown only at a sacred grove. It reveals truths the conscious mind cannot see. Jake consumes it.

**Gameplay:**
- Screen distorts, colors heighten
- Time slows, then accelerates
- Jake enters a **dream arena** — a psychedelic version of his childhood home
- **Dream enemies appear:** manifestations of his guilt, fear, rage
- Each defeated dream enemy reveals something about Jake's psyche
- Final dream opponent: **Ethan** (as he was before leaving)
- The fight is as much about conversation as combat — dream-Ethan speaks truths
- Jake awakens with **permanent** mental clarity buffs

**Reward:**
- "Dream-Clarity" passive (+10% critical hit rate, permanent)
- Specific dialogue unlocked with crew afterward
- Mama Jacinta becomes a recurring NPC with deep wisdom

**Style Bible Compliance:** Pillar 3 (Layered Death — dream-Ethan's "death" is narrative, not literal) + Pillar 5 (Core Theme Revenge — Jake must face what his revenge is costing him internally).

### 6.3 Other Planned Quest Lines

Each major category has special quests:
- **Hunting:** Legendary animal hunts (RDR2-style)
- **Gathering:** Rare herb collections for Ines
- **Fishing:** Master angler challenges
- **Cooking:** Recipe collection
- **Trade:** Supply specific markets

Full details in respective documents.

---

## 7. CONTENT CREATION METHODOLOGY

### 7.1 Per-Species Template

Every creature documented with:
- **Name & Classification**
- **Region(s)**
- **Behavior Patterns**
- **Tracking Difficulty**
- **Combat Capability (if any)**
- **Materials Provided**
- **Uses & Crafting**
- **Narrative Hooks**
- **Legendary Variant (if applicable)**

### 7.2 Per-Plant Template

Every plant documented with:
- **Name & Classification**
- **Region(s)**
- **Appearance**
- **Harvest Method**
- **Seasonal Patterns**
- **Respawn Timer**
- **Materials Provided**
- **Uses & Crafting**
- **Narrative Hooks**

### 7.3 Per-Recipe Template

Every craft/recipe documented with:
- **Name & Description**
- **Required Materials**
- **Required Station**
- **Skill/Specialist Required**
- **Time to Craft**
- **Result & Effects**
- **Discovery Method**

---

## 8. AUDIO AND VISUAL CONSIDERATIONS

### 8.1 Audio Design

**Animal Sounds:**
- Distinct for every species
- Regional ambient layers
- Behavioral audio (hunting calls, mating, danger)
- Realistic not cartoonish

**Processing Sounds:**
- Skinning audio (visceral but not gratuitous)
- Cooking sounds (satisfying)
- Herb crushing, tobacco rolling
- Tactile audio feedback

### 8.2 Visual Design

**Animal Animations:**
- Realistic movement
- Behavior-appropriate idle states
- Combat animations for dangerous ones
- Death animations respectful

**Plant Rendering:**
- Seasonal changes
- Wind interaction
- Harvest state changes
- Regeneration visible

**Processing Animations:**
- Full process visible
- Time-lapse options
- Skill-based variations
- Quality indicators

---

## 9. ACCESSIBILITY CONSIDERATIONS

### 9.1 Hunting Accessibility

- Optional auto-tracking
- Simplified stalking
- Guided kills
- Pacifist mode (no hunting needed)

### 9.2 Gathering Accessibility

- Herb identification hints
- Plant highlighting options
- Automatic sorting
- Recipe suggestions

### 9.3 Processing Accessibility

- Skip animations option
- Quick-craft mode
- Recipe book searchable
- Guided tutorials persistent

---

## 10. ESTIMATED COMPLETION

### 10.1 Document Timeline

Rough estimates per document:
- **Master Overview (this document):** Complete now
- **Foundation Tier (3 docs):** 4-6 hours total
- **Content Tier (6 docs):** 10-15 hours total (biggest workload)
- **Special Systems (5 docs):** 8-12 hours total

**Total Phase 4: ~25-35 hours of focused writing**

### 10.2 Realistic Pacing

Given that yesterday's Phase 3 took one ambitious day, Phase 4 is **larger in scope** but more varied in content type. Suggested approach:
- 2-3 documents per focused session
- Breaks between sessions
- Not trying to complete in one day

### 10.3 Prioritization

If time is limited, prioritize:
1. Foundation Tier (enables everything else)
2. Land Animals + Sea Animals (majority of content)
3. Plants and Herbs (connects to alchemy)
4. Special Systems (Tobacco, Cannabis — unique content)
5. Remaining content as time allows

---

## 11. STYLE BIBLE COMPLIANCE

All Phase 4 content must adhere to:

- ✅ **Pillar 1 (Staggered Register):** NPCs discussing fauna use regional/class-appropriate register
- ✅ **Pillar 2 (Lore Through Objects):** Fauna products tell stories (each cigar has lineage, each herb has cultural meaning)
- ✅ **Pillar 3 (Layered Death):** Hunt deaths vary by animal (noble stag = Style D, rabid boar = Style B)
- ✅ **Pillar 4 (Layered Humor):** Style C quirks in NPC reactions to Jake's hunting (crew opinions on meat quality)
- ✅ **Pillar 5 (Theme — Revenge):** Some creatures reflect Jake's state (predators he admires, prey he pities, the Dream Fight confrontation)

---

## 12. WHY PHASE 4 MATTERS

Phase 4 is where Dark Arisen stops being "a Souls-Like with a ship" and becomes a true **living world**. Combat is excellent (Phase 3). Story is deep (Phase 1-2). But the MOMENT-TO-MOMENT feeling of exploring Dark Arisen's regions comes from the world itself.

**When Jake walks through a Rexan jungle and hears howler monkeys, sees parrots take flight, smells the tobacco drying in a village, spots a boar track, notes which coconut palms look ready to harvest — THAT is the experience Phase 4 creates.**

Without Phase 4, the world is background. With Phase 4, the world is character.

RDR2 is the benchmark. Phase 4 aims to match it — or exceed it in the areas where pirate-era setting allows (sea life, tropical variety, tobacco/cannabis cultures).

---

## 13. NEXT STEPS

**Immediate next document:** `fauna/hunting_system.md` — Foundation Tier Document 1

Hunting is foundational because it defines HOW Jake interacts with animals. Until that's established, individual animals can't be fully specified.

**After Hunting System:** Gathering System, then Processing & Cooking (completing Foundation Tier).

**Then Content Tier begins** with Land Animals (the largest content block).

---

## 14. CONCLUSION

Phase 4 is the natural heart of Dark Arisen. It transforms the world from setting to living system. The work is substantial — 14 documents, thousands of species and plants, complete ecosystems — but the reward is a game world that feels truly alive.

**Phase 3 built the mechanics. Phase 4 fills them with life.**

Let's begin. 🏴‍☠️🌴🔥
