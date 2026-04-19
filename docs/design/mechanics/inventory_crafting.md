# INVENTORY & CRAFTING SYSTEM

**Status:** Design Draft v1.0  
**Phase:** Phase 3 — Mechanics (Subsystem 7 of 10)  
**Tier:** Resource Tier — Depends on both Wheels (Weapon + Item)  
**Related:** mechanics_overview.md, mechanics/weapon_wheel.md, mechanics/item_wheel.md, mechanics/combat_feel.md, style_bible.md  
**Purpose:** Define the complete inventory management, material gathering, and crafting systems in Dark Arisen.  
**Inspirations:** Red Dead Redemption 2 (primary — satchel realism), The Witcher 3 (alchemy system), Monster Hunter (material-based crafting), Assassin's Creed Black Flag (ship inventory and trade), Fallout New Vegas (realistic weight system)

---

## 1. INVENTORY PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Realistic Carry, Not Infinite Bag

Jake is a man carrying items on his person. He cannot carry everything. His satchel has realistic weight limits. Heavy items slow him down. This creates meaningful decisions about what to bring.

**Design intent:** Every item in Jake's satchel was chosen deliberately. Nothing is "just in case."

### 1.2 Principle 2: Ship Is Primary Storage

La Liberación is Jake's mobile base. Her cargo hold can store hundreds of items. Jake transitions between satchel (carried) and cargo hold (stored) items. The ship is not just transport — it is home.

**Design intent:** Planning for missions means outfitting the ship before setting sail. The satchel is for this expedition; the cargo hold is for the campaign.

### 1.3 Principle 3: Crafting Requires Expertise

Jake is a captain, not a master craftsman. Certain items he can craft himself (basic bandages, simple tonics). Advanced crafting requires specialists — his crew members. Ines makes alchemy. Big Tom forges weapons. Father Salvio blesses items. The crew is essential, not decorative.

**Design intent:** Jake needs his people. They need him. The bond is mechanical, not just narrative.

### 1.4 Principle 4: Regional Materials Create Journey

Specific materials exist only in specific regions. Dragon-bone in Ashenmoor. Fjordlund iron in the North. Rexa tobacco in the Caribbean. This forces Jake to travel the world — which is exactly what a pirate does.

**Design intent:** Every region is economically meaningful. Every trade route has purpose. The world feels interconnected.

### 1.5 Principle 5: Legendary Items Have Stories

Legendary items aren't purchased. They are crafted from materials taken from defeated enemies, gifts from allies, or discovered in significant locations. Each legendary item carries the weight of its origin.

**Design intent:** The sword Jake wields at Chapter 9 tells the story of everything he endured to reach it.

---

## 2. THE SATCHEL SYSTEM

### 2.1 Satchel Specifications

| Parameter | Value | Notes |
|-----------|-------|-------|
| Base Weight Capacity | 80 kg | Chapter 4 baseline |
| Chapter 9 Capacity | 120 kg | Upgraded through progression |
| Maximum Possible | 150 kg | With best gear + stat investment |
| Weight Scaling | Affects stamina regen and dodge distance | |

### 2.2 Weight Categories

Every item has a weight in kilograms:

**Featherweight (0.1 - 0.3 kg):**
- Herbs, single cigars, small trinkets, letters
- Can carry hundreds

**Light (0.3 - 1.5 kg):**
- Bandages, vials, small tools, bows
- Flexible carry capacity

**Standard (1.5 - 5 kg):**
- Swords, most weapons, tonics
- Deliberate choice in loadout

**Heavy (5 - 15 kg):**
- Heavy weapons (greatswords, battle axes), large firearms, armor pieces
- Major loadout commitments

**Very Heavy (15 - 30 kg):**
- Complete armor sets, some siege weapons, large trade goods
- Usually stored on ship, not carried

**Impossible for Satchel (30+ kg):**
- Cannons, ballistae, ship parts
- Storage on ship only

### 2.3 Carry Weight Impact

Current weight affects Jake's performance:

| Weight % | Stamina Regen | Dodge Distance | Dodge i-frames | Movement Speed |
|----------|---------------|----------------|----------------|----------------|
| 0-30% | 100% | 100% | 12 frames | 100% |
| 30-60% | 85% | 85% | 10 frames | 95% |
| 60-100% | 70% | 70% | 8 frames | 85% |
| 100%+ | 40% | 50% | 6 frames | 70% |

**Overloaded State:**
- Jake visibly strains
- Cannot sprint
- Combat effectiveness reduced
- Penalty for carrying too much

### 2.4 Categories in the Satchel

Items are organized into categories:

**WEAPONS (8 slots max — active loadout):**
- Equipped weapons counted separately from inventory
- Stored weapons in cargo hold

**CONSUMABLES:**
- Healing items (bandages, tonics, potions)
- Food and drink
- Tobacco and alcohol
- Throwable items

**MATERIALS:**
- Raw materials for crafting
- Organized by type (metal, cloth, herbs, animal)
- Stackable in large quantities

**QUEST ITEMS:**
- Mission-specific objects
- Cannot be dropped
- Not weight-limited (narrative priority)

**KEY ITEMS:**
- Lore objects (#001-#150)
- Letters, notes, maps
- Always accessible

**TRADE GOODS:**
- Items valued in trade but not for use
- Heavy, stored on ship typically
- Economic value in specific ports

---

## 3. SHIP CARGO HOLD

### 3.1 Cargo Hold Specifications

La Liberación has a cargo system separate from Jake's satchel:

| Parameter | Value | Notes |
|-----------|-------|-------|
| Base Cargo Capacity | 500 kg | Chapter 4 baseline |
| Max Capacity | 1200 kg | Upgraded through ship upgrades |
| Storage Type | Organized by category |
| Access Point | Cargo hold below deck |
| Transfer Time | 2-5 minutes game time to load/unload |

### 3.2 Cargo Categories

**Ship-Essential Storage:**
- Repair materials (wood, nails, ropes)
- Food supplies (barrels of salted meat, water, biscuit)
- Gunpowder and ammunition
- Medical supplies (for crew)

**Personal Storage:**
- Weapons not currently wielded
- Armor sets not worn
- Accessory items (jewelry, trophies)
- Personal wealth (gold, silver)

**Trade Storage:**
- Goods acquired for economic purposes
- Valuable but not immediately useful
- Sold at appropriate ports

**Crafting Materials:**
- Bulk materials for crafting
- Organized by material type
- Easily accessible when crafting

### 3.3 Transfer Between Satchel and Cargo

**At Ship Cabin:**
- Access inventory menu
- Drag items between satchel and cargo
- Weight automatically tracked
- Some items permanent (quest items, legendary)

**In Port (Simplified):**
- Auto-suggest items to store/retrieve
- Quick transfer option
- Smart inventory management

**At Sea (Limited):**
- Jake can access cargo but must make the trip below deck
- Takes in-game time (1-5 minutes)
- Some items require crew help (heavy items)

### 3.4 The Lock System (Optional)

Jake can designate certain items as "locked" — they won't be used accidentally:

- **Legendary weapons:** Can't be dropped/sold
- **Ceremonial items:** Specific quest objects
- **Personal items:** Emotional significance (father's pipe, mother's locket)

### 3.5 Cargo Hold Organization

Players can organize cargo:

**Sorting Options:**
- By category (default)
- By weight (heavy to light)
- By value (most valuable first)
- By recency (newest first)
- Alphabetical

**Favorites System:**
- Mark up to 20 items as favorites
- Top of cargo display
- Quick access for common items

---

## 4. MATERIAL SYSTEM

### 4.1 Material Categories

**METALS:**
- **Iron (Common):** Basic weapons, repairs, tools
- **Steel (Uncommon):** Quality weapons, armor
- **Silver (Rare):** Special items, decorative, anti-undead
- **Gold (Rare):** Trading, status items
- **Fjordlund Iron (Regional):** Cold-resistant, North exclusive
- **Rexa Copper (Regional):** Heat-resistant, Caribbean
- **Dragon-bone (Legendary):** Armor-piercing, Ashenmoor dragon remains
- **Imperial Brass (Regional):** Imperial military equipment

**WOOD & FIBERS:**
- **Common Wood:** Basic crafting, repairs
- **Hardwood (Oak, Ebony):** Quality weapons handles
- **Ship Wood:** Specialized vessel construction
- **Rope (Fiber):** Rigging, climbing, equipment
- **Silk (Luxury):** Fine clothing, banners

**LEATHER & ANIMAL:**
- **Basic Leather:** Common armor, bags
- **Cured Leather:** Quality armor pieces
- **Dragon Hide:** Post-dragon fight material
- **Whale Bone:** Fjordlund material
- **Shark Skin:** Ocean predator hide

**HERBS & ALCHEMICAL:**
- **Common Herbs:** Basic tonics
- **Rare Herbs:** Advanced tonics
- **Fjordlund Flowers:** Healing brews (northern)
- **Jungle Plants:** Poison preparations (Moran)
- **Desert Cacti:** Stimulants (Ashenmoor)
- **Havfrue Kelp:** Blessed after Peace Route

**STONE & MINERALS:**
- **Basic Stone:** Building, tools
- **Marble:** Decorative
- **Volcanic Glass:** Sharp edges, Ashenmoor
- **Crystal:** Rare, magical properties hinted
- **Pearl:** Jewelry, valuable trade

**CONSUMABLES (Raw):**
- **Meat (Various):** From hunts
- **Fish:** Caught from sea
- **Fruits:** Tropical, temperate
- **Vegetables:** Cultivated
- **Salt:** Preservation
- **Spices:** Flavor, value

### 4.2 Material Rarity Tiers

Material rarity affects both availability and crafting possibilities:

**Tier 1 (Common):**
- Found easily everywhere
- Basic crafts
- Examples: Iron, Common Wood, Basic Herbs, Stone

**Tier 2 (Uncommon):**
- Found in specific areas
- Good quality crafts
- Examples: Steel, Hardwood, Cured Leather, Rare Herbs

**Tier 3 (Rare):**
- Regional specialties
- Excellent crafts
- Examples: Fjordlund Iron, Silver, Jungle Poison Plants

**Tier 4 (Legendary):**
- Boss rewards, special locations
- Legendary crafts possible
- Examples: Dragon-bone, Havfrue Scale, Whale Bone from Region 06

**Tier 5 (Unique):**
- Single instance in entire game
- Unique legendary crafts
- Examples: Draven's Signature Blade Fragment, Ethan's Ring

### 4.3 Gathering Mechanics

Different materials require different gathering methods:

**Herbs:** Interact button (Triangle) near plant
**Minerals:** Mining pickaxe (tool required)
**Animals:** Successful hunt + skinning
**Wood:** Axe + time + specific trees
**Fish:** Fishing rod + patience + location
**Metals (from enemies):** Loot from defeated enemies

### 4.4 Gathering Animations

**Herb Picking:** 2-3 seconds, Jake bends down
**Mining:** 8-15 seconds with pickaxe, chunk hits
**Skinning:** 8-12 seconds, knife work
**Tree Felling:** 30-60 seconds, rhythmic axe strikes
**Fishing:** Variable (30 seconds - 5 minutes)
**Looting:** Instant from dead enemies

### 4.5 Material Respawn

Materials respawn based on type:

**Herbs:** 30 minutes real-time
**Trees:** 2 hours real-time (regrowth)
**Mineral Deposits:** 4 hours real-time
**Animal Populations:** 1 hour real-time
**Fish:** Instant respawn in most locations

**Design intent:** Gathering is meaningful but not infinitely scarce.

---

## 5. CRAFTING STATIONS AND CREW

### 5.1 Crafting Stations Overview

Different stations enable different crafts:

**Basic Fire/Campfire:**
- Simple food cooking
- Basic alchemy (infusions, teas)
- Bandage preparation
- Minimal tool requirement

**Ship Cabin Workshop:**
- Intermediate crafts
- Tonics and remedies
- Tobacco preparation
- Personal crafting

**Inn/Tavern Kitchen:**
- Advanced food preparation
- Meals with buffs
- Community crafting

**Specialist Shops (Villages):**
- Alchemy Shop (Ines-equivalent)
- Blacksmith (Big Tom-equivalent)
- Religious Shrine (Father Salvio-equivalent)
- Fishing Equipment
- Tailor/Armorer

**Ship Crew Crafting (Primary):**
- Ines (Alchemist): Tonics, poisons, cures
- Big Tom (Blacksmith): Weapons, repairs
- Father Salvio (Priestly Crafter): Blessed items
- Esteban (Master Navigator): Maps, charts
- Mira (Captain's Helm): Specific crew crafts

### 5.2 Ines's Alchemy (Chapter 4+)

**Ines's Unique Capability:**
- Deep alchemical knowledge from pre-Armada days
- Crafts Tonics and Potions
- Knows Caribbean herbs and remedies

**Her Available Crafts:**

**Tonics (Buffs):**
- Strength Tonic (1 hr buff)
- Dexterity Tonic (1 hr buff)
- Resilience Tonic (1 hr buff)
- Stamina Tonic (1 hr buff)
- Focus Tonic (Rache cooldown reduction)

**Healing:**
- Minor Healing Tonic (80 HP)
- Major Healing Tonic (160 HP)
- Blood Tonic (100% + status cure, Havfrue reward)

**Status Cures:**
- Antitoxin (poison)
- Burn Salve (fire damage)
- Frost Relief (cold)

**Advanced Items:**
- Sleep Potion (non-lethal combat)
- Paralysis Coating (weapons)
- Poison Flask (throwable)

**Ingredient Requirements:**
- Specific herbs per recipe
- Water (clean)
- Fire/heat for brewing
- Container/vial

**Time:** 15 minutes - 2 hours per craft

**Location:** Ines's workshop in ship cabin or village shops

### 5.3 Big Tom's Smithing

**Big Tom's Unique Capability:**
- Master blacksmith (30 years experience)
- Can work with any metal including dragon-bone
- Forges weapons and armor

**His Available Crafts:**

**Weapons:**
- Standard swords, axes, daggers
- Bows and firearms
- Specialty weapons (dual-mode, unique)

**Armor:**
- Light, medium, heavy sets
- Specialty armor pieces
- Themed armor (region-inspired)

**Upgrades:**
- Weapon reinforcement
- Damage augmentation
- Special properties

**Legendary Work:**
- Crafts from boss materials (see Section 7)
- Masterwork weapons
- Family heirloom restoration

**Ingredient Requirements:**
- Appropriate metal (iron, steel, dragon-bone)
- Leather/cloth for grips and wrapping
- Specific components (gems, special materials)
- Fuel (coal, wood)

**Time:** 2 hours - 24 hours (for master work)

**Location:** Ship forge area, or specialist smiths in ports

### 5.4 Father Salvio's Blessed Items

**Father Salvio's Unique Capability:**
- Rexan priest
- Can bless specific items
- Knows religious/spiritual practices

**His Available Crafts:**

**Blessed Weapons:**
- +10% damage to specific enemy types
- Holy water coatings
- Purifying blessings

**Protection Items:**
- Rosaries (passive buffs)
- Holy symbols (enemy resistance)
- Blessed water (damage to undead)

**Ritual Items:**
- Funeral items (quest-specific)
- Ceremonial objects
- Sacred cloth

**Ingredient Requirements:**
- Specific holy materials
- Silver (often)
- Candles and incense
- Appropriate prayer/ritual (time)

**Time:** 30 minutes - 2 hours per blessing

**Location:** Ship chapel area, or priest's chapels in ports

### 5.5 Esteban's Navigation Crafts

**Esteban's Unique Capability:**
- Master navigator (40 years experience)
- Creates maps and charts
- Knows celestial navigation

**His Available Crafts:**

**Maps:**
- Detailed charts of regions
- Trade route plans
- Hidden location markers
- Sea route optimizations

**Navigation Tools:**
- Sextants (custom made)
- Compasses
- Astrolabe copies

**Time:** 1-4 hours per map

**Location:** Ship navigation room

### 5.6 Jake's Personal Crafting

Jake can craft certain items without crew:

**Basic Crafts:**
- Simple Bandages
- Basic Tonics (from herbs)
- Cigar preparation (from tobacco leaves)
- Food preparation (basic cooking)
- Weapon maintenance (cleaning)
- Quick repairs

**Time:** 5-30 minutes

**Location:** Campfire, cabin, or standing still

### 5.7 Crafting UI

**Station Interaction:**
1. Jake approaches crafting station (Triangle to interact)
2. Select craft category (Weapons, Armor, Tonics, Food, etc.)
3. View available recipes (filtered by current materials)
4. Select recipe
5. Confirm craft
6. Wait for completion

**Information Display:**
- Recipe name and description
- Required materials (highlighted if possessed)
- Time to craft
- Skill/Specialist required
- Difficulty (if variable)

**Queue System:**
- Can queue multiple crafts
- Completes while Jake does other things
- Notifications when done

---

## 6. CRAFTING RECIPES

### 6.1 Example Basic Recipes

**Basic Bandage:**
- 1x Linen Cloth
- 1x Clean Water
- Skill: None (Jake can craft)
- Time: 5 minutes

**Minor Healing Tonic:**
- 1x Rare Herb
- 1x Common Herb
- 1x Clean Water
- Container: Glass Vial
- Skill: Ines (Alchemy Tier 1)
- Time: 15 minutes

**Iron Short Sword:**
- 3x Iron Ingot
- 1x Leather (grip)
- 1x Ropefiber (wrapping)
- Skill: Big Tom (Smithing Tier 1)
- Time: 2 hours

**Basic Meal (Cooked Fish):**
- 1x Fresh Fish
- 1x Salt
- 1x Fire access
- Skill: Jake (basic cooking)
- Time: 5 minutes

### 6.2 Advanced Recipes

**Resilience Tonic (Major):**
- 2x Fjordlund Healing Flower
- 1x Dragon's Blood extract (from Ashenmoor encounter)
- 1x Pure Water (from Havfrue's spring if Peace Route)
- 3x Silver dust
- Skill: Ines (Alchemy Tier 3)
- Time: 4 hours

**Steel Longsword with Flame Enchantment:**
- 5x Steel Ingot
- 1x Volcanic Glass (from Caldera Titan aftermath)
- 2x Rare Leather
- 1x Blessed oil (from Father Salvio)
- Skill: Big Tom (Smithing Tier 2)
- Time: 8 hours

**Sacred Battle Robe:**
- 2x Fine Silk
- 3x Holy Water
- 1x Silver thread
- 1x Prayer scroll
- Skill: Father Salvio (Blessed Crafting Tier 2)
- Time: 3 hours

### 6.3 Recipe Discovery

**Finding Recipes:**

**Books/Manuscripts:** Found in specific locations
- Libraries
- Religious buildings
- Ancient ruins
- Bookstores
- Quest rewards

**NPC Teaching:**
- Specific NPCs teach recipes
- Dialogue options unlock teaching
- Friendship/reputation required

**Experimentation:**
- Jake can try combinations
- May discover variants
- Failure common (wasted materials)

**Quest Rewards:**
- Story-critical recipes from quest lines
- Sometimes master-level crafting unlocked

### 6.4 Recipe Tiers (Per Crafter)

Each crafter has tiers that unlock:

**Ines (Alchemy):**
- Tier 1: Basic tonics (unlocked Chapter 4)
- Tier 2: Advanced tonics (unlocked Chapter 6)
- Tier 3: Master alchemy (unlocked Chapter 8)
- Tier 4: Legendary alchemy (post-game only)

**Big Tom (Smithing):**
- Tier 1: Basic weapons and armor
- Tier 2: Quality crafts
- Tier 3: Specialist crafts (dual-mode, unique)
- Tier 4: Legendary crafts (post-game)

**Father Salvio (Blessed):**
- Tier 1: Basic blessings
- Tier 2: Protection items
- Tier 3: Holy weapons
- Tier 4: Sacred artifacts (rare)

---

## 7. LEGENDARY CRAFTING

### 7.1 Legendary Crafting Philosophy

Legendary items aren't shopping. They are the culmination of specific story events, requiring materials gained through defeating specific bosses or achieving specific outcomes.

Each legendary has:
- **Origin story:** What material came from where
- **Crafting journey:** The specific crafter who makes it
- **Unique properties:** Mechanics reflecting origin
- **Narrative weight:** Jake's choices shape the result

### 7.2 Legendary Weapons Catalog

**Leviathan's Sorrow:**
- **Requirement:** Combat victory over Havfrue Modor (NOT Peace Route)
- **Materials:** Havfrue's Heart + 3x Dragon Hide + 5x Silver
- **Crafter:** Big Tom (with Father Salvio blessing)
- **Stats:** +280 damage, +50 vs. imperial enemies
- **Unique:** Ocean-themed, remembers its origin
- **Narrative Weight:** Jake carries the weight of killing Havfrue
- **Cannot be crafted:** If Havfrue is peacefully encountered (Jake cannot obtain materials)

**Draven's Vengeance:**
- **Requirement:** Complete Chapter 9 boss fight with Draven
- **Materials:** Draven's Personal Blade (rewarded) + Father Salvio's blessing
- **Crafter:** Big Tom (or Ines if specific dialogue)
- **Stats:** +320 damage, Rally recovery bonus
- **Unique:** Dual-mode transformation (Draven-style)
- **Narrative Weight:** The blade of the man who killed Jake's family

**Ethan's Legacy:**
- **Requirement:** Peaceful resolution with Ethan (Chapter 9 option)
- **Materials:** Ethan's Heirloom Ring + 5x Legendary Steel
- **Crafter:** Father Salvio
- **Stats:** +250 damage, Crew support bonus
- **Unique:** Grows stronger as crew bonds deepen
- **Narrative Weight:** Brotherly reconciliation

**El Médico's Trauma:**
- **Requirement:** Defeat El Médico (any outcome)
- **Materials:** His surgical tools + Rexa copper + Alchemy components
- **Crafter:** Ines (reluctantly — she knows what these were)
- **Stats:** High damage, status effect buildup
- **Unique:** Causes fear in enemies
- **Narrative Weight:** A tool of torture reforged

**The Sea-Mother's Scale Armor:**
- **Requirement:** Peace Route with Havfrue Modor
- **Materials:** Havfrue's given scale + 5x pearl + blessed silk
- **Crafter:** Big Tom (with Father Salvio blessing)
- **Stats:** Heavy armor, significant water resistance
- **Unique:** Havfrue's blessing active (good winds)
- **Narrative Weight:** A gift from the sea

**Fjordlund Crown:**
- **Requirement:** Complete Fjordlund peaceful route (Halvard, Havfrue, various)
- **Materials:** Fjordlund Iron + Whale Bone + Fjordlund flowers
- **Crafter:** Mormor Astrid (Region 4 Fjordlund grandmother)
- **Stats:** Cold immunity, Fjordlund inhabitants friendly
- **Unique:** Jake can speak to Fjordlund sailors as one of their own
- **Narrative Weight:** Acceptance into the Fjordlund people

### 7.3 Legendary Process

Creating a legendary takes significant time:

**Phase 1: Material Gathering (Game Time)**
- Defeat required boss or complete event
- Gather supporting materials (may take hours)
- Find specific ingredients

**Phase 2: Crafting Preparation (1-3 hours game time)**
- Deliver materials to appropriate crafter
- Crafter inspects materials
- Discusses with Jake the choices made

**Phase 3: Actual Crafting (24-72 hours game time)**
- Extended crafting process
- Special animation for legendary crafts
- Story moments during creation
- Jake can continue playing while it processes

**Phase 4: Reception Ceremony**
- Formal presentation of legendary weapon
- Story-appropriate scene
- Weapon named (may prompt Jake to name it)
- Unique properties revealed

### 7.4 Consequences of Legendary Crafting

Creating legendaries has story consequences:

**Leviathan's Sorrow (Combat Path):**
- The ocean seems colder when Jake sails
- Mormor Astrid won't speak to Jake anymore
- Weather worsens in Fjordlund

**Draven's Vengeance:**
- Former Armada members recognize the blade
- Triggers specific dialogue with ex-pirates
- Moral weight in Chapter 10 epilogue

**Ethan's Legacy:**
- Special connection with crew
- Bond level caps raised
- Emotional resonance

### 7.5 Multiple Path Legendaries

Some legendaries have different outcomes based on choices:

**The Scarred Twins Memorial Blade:**
- **If Jake left their retirement gold:** A memorial sword
- **If Jake took their gold:** A sword that carries their curse
- **If Jake properly buried them:** A blessed sword
- Same materials, different weapons

---

## 8. TRADING ECONOMY

### 8.1 Currency System

**Doubloons (Primary):**
- Standard currency
- Found as loot, earned from quests, trade
- Used in all regions (with some exchange penalties)

**Gold Bars (High Value):**
- Worth 100 Doubloons each
- Used for large transactions
- Requires counting and weighing

**Regional Currencies:**
- **Imperial Sovereign:** Favored in imperial cities
- **Rexan Peso:** Common in Caribbean
- **Fjordlund Silver:** Northern Atlantic
- **Crimson Token:** Among Armada

### 8.2 Merchants and Trade

**Categories of Merchants:**

**General Goods:**
- Basic consumables
- Common materials
- Foods

**Specialty Merchants:**
- Alchemist shops (tonics, potions)
- Weapon dealers
- Cloth merchants
- Jewelry dealers
- Book sellers (recipes!)

**Port Authorities:**
- Trade goods exchange
- Large quantity transactions
- Route information

### 8.3 Pricing System

**Dynamic Pricing:**
- Demand affects prices
- Regional availability
- Jake's reputation influences sales
- Some items cheaper in certain regions

**Example Price Variations:**

| Item | Base Price | Rexa Price | Fjordlund Price | Imperial Price |
|------|-----------|-----------|-----------------|----------------|
| Iron Ingot | 25 | 25 | 35 | 20 |
| Fjordlund Iron | 80 | 120 | 60 | 100 |
| Rare Herbs | 40 | 30 | 50 | 45 |
| Tobacco | 15 | 10 | 25 | 20 |

### 8.4 Reputation Effects on Trade

**Bad Reputation Effects:**
- Merchants overcharge by 20-40%
- Some refuse to trade entirely
- Quality selections limited
- Special items unavailable

**Good Reputation:**
- Standard pricing
- Full inventory access
- Occasional discounts
- Special items available

**Excellent Reputation:**
- Discounted prices (up to -20%)
- Exclusive items
- Trading partnerships
- Quest access

### 8.5 Trade Routes

**Profitable Trade Routes Examples:**

**Rexa to Moran:**
- Take: Tobacco, tropical fruits
- Sell: Silver, rare herbs
- Profit: Mid

**Fjordlund to Ashenmoor:**
- Take: Fjordlund iron, whale bone
- Sell: Dragon-bone (after battles)
- Profit: High

**Imperial Cities to Pale Isle:**
- Take: Imperial brass, gold
- Sell: Religious items, blessed materials
- Profit: High but risky

### 8.6 Piracy Revenue

Jake can make money through piracy:

**Attacking Merchant Ships:**
- Loot cargo
- Sell goods at different ports
- Risk: Imperial response

**Successful Raids:**
- Ship capture (cargo + passengers)
- Board and plunder
- Quick money, moral weight

**Consistent Piracy:**
- Reputation lowers with imperial authority
- Some shops refuse Jake
- Different story outcomes in Chapter 10

---

## 9. UPGRADE SYSTEM

### 9.1 Weapon Upgrades

**Basic Upgrades (Big Tom):**
- Sharpening: +10% damage
- Reinforcement: +15% durability
- Quality materials: +20% damage, +10% critical

**Material Upgrades:**
- Swap iron for steel: significant improvements
- Add dragon-bone pommel: armor piercing
- Silver infusion: effective vs. specific enemies

**Special Enchantments:**
- Fire enchantment (from dragon materials)
- Ice enchantment (from Fjordlund)
- Blessed enchantment (from Father Salvio)
- Cursed enchantment (from El Médico's tools)

### 9.2 Armor Upgrades

**Reinforcement:**
- Metal plates added
- Leather hardening
- Stitching improvement

**Resistance Additions:**
- Fire resistance (dragon-scale)
- Cold resistance (fur lining)
- Water resistance (oiled materials)

**Aesthetic:**
- Embossing, engraving
- Decorative elements
- Pirate style customization

### 9.3 Ship Upgrades

**La Liberación specific:**

**Hull Upgrades:**
- Reinforced planking: +500 HP
- Armored sides: +15% damage reduction
- Ram: Enables ramming attack

**Sail Upgrades:**
- Larger sails: +1 knot speed
- Reinforced rigging: -20% storm damage
- Silk sails: +10% speed, luxurious

**Weapon Upgrades:**
- Better cannons: +20% damage
- More cannons: +10 to total
- Chain shot specialty: armor-piercing

**Capacity Upgrades:**
- Larger cargo hold: +200 kg max
- Crew quarters expansion: +5 crew capacity
- Workshop improvements: Better crafting

**Navigation:**
- Better spyglass: Extended sight
- Improved compass: Weather prediction
- Chart room: Better maps

### 9.4 Upgrade Costs

Upgrades require:
- **Materials:** Appropriate resources
- **Currency:** Doubloons for labor
- **Time:** Hours to weeks game time
- **Location:** Appropriate crafter/facility

---

## 10. INVENTORY UI

### 10.1 Main Inventory Screen

**Layout:**
- **Left:** Category tabs (Weapons, Consumables, Materials, Key Items, Trade)
- **Center:** Item grid (current category)
- **Right:** Item details panel

**Item Display:**
- Icon + quantity
- Weight indicator
- Value (if sellable)
- Quality rating

### 10.2 Weight Display

**Prominent weight indicator:**
- Current weight / Max weight
- Color-coded (green, yellow, red)
- Warning when approaching limits

**Per-item weight:**
- Shown in item details
- Weight-class badge (Feather, Light, Heavy, etc.)

### 10.3 Filtering and Sorting

**Filters:**
- By category
- By rarity tier
- By crafting value
- By usage (quick-access items)

**Sorts:**
- Alphabetical
- Weight (light to heavy)
- Value
- Recent acquisition
- Condition

### 10.4 Comparison Tool

**Weapon Comparison:**
- Side-by-side stats display
- Damage, weight, range, speed
- Bond levels
- Upgrade potential

**Armor Comparison:**
- Defense values
- Resistance types
- Mobility impact
- Set bonuses

### 10.5 Information Tooltips

**Detailed Tooltips:**
- Extended description
- Crafting source (if known)
- Story significance (if any)
- Effects and bonuses
- Related items

---

## 11. CRAFTING UI

### 11.1 Station Interface

**Layout:**
- **Top:** Station name, specialist (Ines, Big Tom, etc.)
- **Left:** Recipe categories
- **Center:** Recipe list (with filters)
- **Right:** Selected recipe details
- **Bottom:** Current materials inventory

### 11.2 Recipe Display

**Per-Recipe Information:**
- Item name and icon
- Required materials (highlighted: red if missing, green if sufficient)
- Quantity needed
- Crafting time
- Success rate (if variable)
- Resulting item stats

### 11.3 Crafting Queue

**Multi-Craft Management:**
- Queue up to 10 items
- Progress tracking
- Resource allocation
- Time estimation

### 11.4 Discovery Interface

**Recipe Book:**
- Categorized by crafter and type
- Locked/unlocked indicators
- Progress tracking
- Source notes (where recipe was learned)

### 11.5 Material Browser

**Resource Management:**
- All materials in cargo + satchel
- Filter by type
- Quick transfer to crafting
- Inventory optimization

---

## 12. MATERIAL ACQUISITION PATTERNS

### 12.1 Early Game (Chapters 1-3)

**Common Materials:**
- Basic iron and wood
- Herbs (common)
- Standard leather
- Simple fish

**Scarcity:**
- Specialized materials rare
- Regional materials not yet accessible

### 12.2 Mid Game (Chapters 4-7)

**Regional Variety:**
- Rexa tobacco, fruits, jungle plants
- Imperial materials
- Fjordlund begins (iron, herbs)

**Quest-Specific:**
- Boss-related materials unlock
- Trade routes profitable
- Economic advantages from travel

### 12.3 Late Game (Chapters 8-9)

**Legendary Materials:**
- Dragon-bone from Ashenmoor
- Havfrue-related materials
- Imperial military gear from Fort Carrion

**Advanced Crafting:**
- Tier 4 recipes accessible
- Legendary weapons possible
- Master-level equipment

### 12.4 Post-Game (Region 06)

**Unique Materials:**
- Only obtainable in Region 06
- Post-game legendaries
- Special story-critical items

---

## 13. INVENTORY INTEGRATION WITH OTHER SYSTEMS

### 13.1 Weapon Wheel Integration

**Inventory ↔ Wheel:**
- Weapons in wheel deducted from inventory
- Wheel slots customizable from inventory menu
- Bond levels tracked through inventory

### 13.2 Item Wheel Integration

**Inventory ↔ Item Wheel:**
- All consumables visible in inventory
- Item wheel shows stored quantities
- Restocking from inventory to wheel seamless

### 13.3 Combat System

**Durability:**
- Some weapons degrade with use
- Requires repair at Big Tom
- Condition visible in inventory

**Consumable Tracking:**
- Combat uses reduce inventory
- Low-stock warnings
- Strategic resource management

### 13.4 Story System

**Quest Items:**
- Non-disposable
- Inventory-isolated
- Cannot be lost

**Gift Giving:**
- Some items can be gifted to NPCs
- Changes relationships
- Strategic social mechanic

---

## 14. PLATFORM-SPECIFIC FEATURES

### 14.1 PS5 DualSense

**Haptic Feedback:**
- Heavy items noted with haptic
- Crafting completion has specific pulse
- Weight warning intensifies as load increases

**Audio:**
- Item transfer sounds
- Crafting process audio
- Weight-appropriate sounds

### 14.2 Xbox

**Similar functionality:**
- Controller rumble for important events
- Trigger feedback for some interactions

### 14.3 PC

**Keyboard/Mouse:**
- Drag-and-drop inventory management
- Right-click context menus
- Keyboard shortcuts for common operations

---

## 15. ACCESSIBILITY

### 15.1 Motor Accessibility

**Simplified Management:**
- Auto-sort by default
- One-click transfer
- Reduced menu depth

**Extended Time:**
- Crafting timers can be disabled (instant)
- No rushed decisions

### 15.2 Visual Accessibility

**Large Icons:**
- Scalable UI (75%-200%)
- High contrast options
- Alternative color schemes

**Information Layout:**
- Simplified views available
- Clear hierarchies
- Important info always visible

### 15.3 Cognitive Accessibility

**Simplified Categories:**
- Fewer tabs
- Clearer labels
- Tutorial hints always available

### 15.4 Economic Accessibility

**Auto-pricing:**
- Shows best sell prices
- Regional recommendation
- Budget tracking

---

## 16. TUTORIAL PROGRESSION

### 16.1 Chapter 1 Introduction

**Basic Inventory:**
- Satchel concept
- First weight awareness
- Simple item collection

### 16.2 Chapter 2 Expansion

**Crafting Basics:**
- Jake's personal crafts
- Basic recipes
- First station use

### 16.3 Chapter 4 Ship Integration

**Cargo Hold:**
- Ship storage system
- Transfer mechanics
- Crew crafting introduced

**Ines and Big Tom:**
- Meet crew crafters
- Unlock their specialties
- Trade commissions

### 16.4 Chapter 6 Regional Materials

**Ashenmoor:**
- Dragon-bone introduction
- Regional specialty materials
- Advanced crafting possible

### 16.5 Chapter 8 Legendary Preparation

**Boss Materials:**
- Dragon-bone, Havfrue Scale, etc.
- Legendary recipes taught
- Pre-Ethan/Draven prep

### 16.6 Chapter 9+ Full System

**Mastery:**
- All tiers accessible
- Economic depth
- Full crafting potential

---

## 17. INVENTORY FEEL TESTS

Before the inventory system is final, it must pass these tests:

### 17.1 The RDR2 Test
> *"Does the satchel system feel realistic without being oppressive? Does weight matter but not frustrate?"*

### 17.2 The Witcher 3 Test
> *"Does crafting feel meaningful and rewarding? Does the alchemy system have depth?"*

### 17.3 The Realism Test
> *"Does Jake's inventory feel like a real man carrying his possessions?"*

### 17.4 The Progression Test
> *"Do items accumulate meaningfully? Does legendary crafting feel earned?"*

### 17.5 The Economic Test
> *"Does trading feel viable? Are materials scarce enough to be meaningful but available enough to progress?"*

All five must pass. Then the inventory system is ready.

---

## 18. IMPLEMENTATION NOTES

### 18.1 Technical Requirements
- Efficient inventory data structure
- Real-time weight calculations
- Material tracking across satchel/cargo
- Recipe database with conditions
- Crafter tier systems
- Economic simulation (prices, supply, demand)

### 18.2 UI Requirements
- Clean, readable interfaces
- Quick access shortcuts
- Detailed information on demand
- Search and filter functionality

### 18.3 Balance Requirements
- Material rarity balanced across regions
- Crafting costs reasonable but meaningful
- Legendary requirements appropriately challenging
- Economic system stable long-term

### 18.4 Content Requirements
- 100+ recipes across all crafters
- 50+ legendary items
- 200+ unique materials
- Extensive trade networks

---

## 19. CONSISTENCY WITH EXISTING DESIGN

All existing documentation remains compatible:

- **Combat System:** Supports all weapon types
- **Boss Encounters:** Drops materials referenced here
- **Naval Combat:** Ship inventory integrates
- **Crew:** Crafters specified match existing crew
- **Regions:** Regional materials from region overviews

No existing documents need revision. Inventory supports all previously-designed content.

---

## 20. NEXT STEPS

Next document: **`mechanics/interaction.md`** — Interaction System

Will cover:
- Dialogue system (branching, choice consequences)
- Investigation mechanics (observation, clues)
- Environmental interaction (objects, triggers, secrets)
- NPC reactions (recognition, memory)
- Looting mechanics (containers, bodies, stealthy vs. loud)
- Lock-picking and security
- Reading system (books, letters, notes)
- Social gestures (gifts, greetings, insults)
- Physical interaction (pickup, throw, examine)

Estimated time: 3-4 hours.

After Interaction: Ship Interior System (La Liberación as mobile base).

**The inventory and crafting system is the backbone of resource management in Dark Arisen. This document defines every material, every recipe, every crafter. Phase 3 Subsystem 7 of 10 — complete.**
