# PROCESSING & COOKING SYSTEM

**Status:** Design Draft v1.0  
**Phase:** Phase 4 — Fauna & Flora (Subsystem 3 of 14 — FINAL Foundation Tier)  
**Tier:** Foundation Tier  
**Related:** fauna_overview.md, fauna/hunting_system.md, fauna/gathering_system.md, mechanics/inventory_crafting.md, style_bible.md  
**Purpose:** Define how raw materials become finished products — processing raw materials, cooking meals, creating medicines, preserving goods, and the cultural/mechanical weight of transformation.  
**Inspirations:** Red Dead Redemption 2 (cooking variety), Breath of the Wild (creative cooking), Monster Hunter World (meal buffs), The Witcher 3 (alchemy systems), Green Hell (survival crafting), Cooking Mama (animation detail), Sea of Thieves (shared meals)

---

## 1. PROCESSING PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Transformation Is Meaningful

Raw deer meat is not food. It is potential. The transformation from raw to cooked, from herb to tonic, from leaf to cigar, is where value is created. This transformation is not a menu operation — it is an experience with time, skill, and craft.

**Design intent:** The journey from material to product is gameplay, not abstraction.

### 1.2 Principle 2: Location Shapes Quality

The same raw venison becomes different meals at different locations:
- **Beach Campfire:** Simple roasted meat, basic buff
- **Ship Galley:** Seasoned venison with ship herbs, better buff
- **Port Kitchen:** Regional specialty, unique buff
- **Master Chef:** Legendary meal, powerful buff

Where Jake cooks matters as much as what he cooks with.

**Design intent:** Ports and ship become destinations not just for commerce but for food quality.

### 1.3 Principle 3: Cooking Is Culture

Fjordlund's hearty stews reflect arctic survival. Rexa's spiced meats reflect tropical abundance. Ashenmoor's preserved foods reflect desert practicality. Food tells the story of place and people. Jake can learn regional cooking and carry it with him.

**Design intent:** Every meal Jake cooks connects him to a culture.

### 1.4 Principle 4: Time Is an Ingredient

Quick preparation yields quick results. Slow, careful cooking yields masterwork. A campfire meal takes 5 minutes. A legendary feast takes hours. Players who invest time get rewards beyond mere buffs — they get experiences.

**Design intent:** Fast food and slow food coexist, rewarded differently.

### 1.5 Principle 5: Sharing Food Matters

The best meals are shared. Cooking for crew builds bonds. Meals at port connect Jake to communities. Specific quests involve preparing food for others. Food is social, not solitary.

**Design intent:** Cooking connects Jake to the world, not just to his stats.

---

## 2. COOKING STATIONS — A HIERARCHY

### 2.1 Emergency/Minimal (No Station)

**Raw Eating:**
- Berries, fruits, some vegetables
- Immediate, minimal preparation
- Risk of illness from some (meat, unknown plants)
- Survival option only

**Trail Preparation:**
- Jerky (pre-dried before travel)
- Biscuit and dried meat
- Cold rations
- Portable but limited buffs

### 2.2 Campfire (Basic — Available Always)

**Setup:**
- Jake gathers wood, kindles fire
- 2-3 minutes setup
- Needs dry wood (weather-dependent)
- Provides warmth, light, cooking

**Capabilities:**
- Roasting meat (simple cuts)
- Boiling water (for broth, tea)
- Basic herbal infusions
- Smoking small amounts

**Limitations:**
- Limited to simple recipes
- Weather-dependent (rain extinguishes)
- Basic buffs only
- One-person cooking

**Upgrades:**
- **Permanent Camp:** Set up semi-permanent fire with stones
- **Cooking Tripod:** Carried item for pot-hanging
- **Portable Grill:** Crafted item, better meat cooking

### 2.3 Ship Galley (Standard — Jake's Main Location)

**Setup:**
- Always ready on La Liberación
- Ramón the cook (NPC) operates
- Jake can use when Ramón busy/off-duty
- Well-stocked with basics

**Capabilities:**
- Full range of standard recipes
- Multiple simultaneous dishes
- Stewing, roasting, baking, frying
- Preservation (pickling, salting)
- Serving multiple crew

**Limitations:**
- Cannot craft legendary meals (needs specialists)
- Regional specialties need regional access
- Weather affects at-sea cooking (storm = no cooking)
- Rationing in long voyages

**Upgrades:**
- **Expanded Galley:** Ship upgrade, +cooking capacity
- **Quality Cookware:** Better meals, +10% buff quality
- **Spice Storage:** Preserves spices from rot
- **Hot Oven:** Enables bread and pastries

### 2.4 Port Kitchens (Advanced)

**Availability:**
- Major ports only
- Hire specialist cooks
- Access to local ingredients
- Time-investment required

**Capabilities:**
- Regional specialty recipes
- Legendary ingredient access
- Master-level preparations
- Unique cultural meals

**Categories:**

**Rexan Kitchen:**
- Spiced meats, seafood
- Tropical fruit preparations
- Slow-cooked stews
- Specific legendary meals

**Fjordlund Kitchen:**
- Hearty stews
- Cold-preservation techniques
- Mead pairings
- Winter sustenance meals

**Ashenmoor Kitchen:**
- Preserved foods (travel-ready)
- Cactus preparations
- Desert-hardy recipes
- Volcanic-influenced cuisines

**Imperial Kitchen (Pale Isle):**
- Classical European-style
- Formal presentation
- Expensive ingredients
- Refined buffs

### 2.5 Master Chefs (Legendary)

**Specific NPCs:**

**Chef Antonio Vela (Rexa, Noble District):**
- Former imperial chef
- Fell from grace
- Now cooks in hidden location
- Teaches Jake legendary Rexan cuisine

**Mormor Astrid (Fjordlund):**
- Grandmother figure
- Traditional healer-cook
- Teaches ancient recipes
- Peace Route connection

**Old Man Hemingway (Quiet Coast):**
- Master fisherman-cook
- Knows all seafood preparation
- Legendary fish recipes
- Philosophical conversations

**Mbah Seruni (Ashenmoor):**
- Ashen Wyrm survivor
- Desert survival cuisine
- Unique plant preparations
- Wisdom and food

**Capabilities:**
- Legendary meals with unique buffs
- Multi-hour preparations
- Personal story integration
- Permanent recipe additions

---

## 3. COOKING MECHANICS

### 3.1 The Cooking Interface

**Pre-Cooking:**
- Select cooking station
- Browse available recipes (based on ingredients)
- See required materials
- Choose quality level

**Cooking Animation:**
- Time-appropriate (5 min to 3 hours game time)
- Interactive elements (stirring, flipping, seasoning)
- Can speed up or watch fully
- Skill checks possible

**Post-Cooking:**
- Quality revealed
- Buff description
- Serving options (self, crew, save)
- Storage considerations

### 3.2 Quality Factors

**Ingredient Quality:**
- Perfect pelt = Perfect cuts = Better meal
- Peak herb = Stronger flavor
- Fresh vs. preserved matters
- Regional variety counts

**Station Quality:**
- Campfire (basic) vs. Master Chef
- Tools available
- Seasoning access
- Expertise applied

**Cook's Skill:**
- Jake's cooking experience
- NPC's specialty
- Time invested
- Attention given

**Combination Effects:**
- Perfect × Perfect × Perfect = Legendary
- Mixing tiers averages down
- Exceptional overall requires all excellent

### 3.3 Cooking Skill Progression

**Jake's Cooking Tiers:**

**Novice (Chapter 1):**
- Simple recipes only
- Basic buffs
- Standard quality at best
- Learning

**Apprentice (Chapter 2-3):**
- Standard recipes mastered
- Can try intermediate
- Better than Novice quality
- Experimenting

**Cook (Chapter 4-6):**
- Most recipes available
- Can teach basic recipes
- Good quality regular
- Competent

**Chef (Chapter 7-9):**
- Advanced recipes accessible
- High quality consistent
- Can craft for others
- Skilled

**Master Chef (Post-Game):**
- All recipes
- Legendary quality possible
- Can teach masters
- Rare tier

### 3.4 Recipe Discovery

**Sources:**
- **Field notes:** Find in containers
- **NPCs teach:** Through relationships
- **Cookbooks:** Purchase from ports
- **Experimentation:** Combine ingredients (risk)
- **Questlines:** Specific recipe rewards
- **Crew contributions:** Each crew member shares favorites

**Total Recipes in Game:**
- 150+ standard recipes
- 30+ legendary recipes
- 20+ post-game only
- 50+ regional specialties

---

## 4. MEAL CATEGORIES AND BUFFS

### 4.1 Buff Categories

**Physical Buffs:**
- Stamina capacity increase
- HP boost (temporary max)
- Damage output increase
- Speed enhancement
- Carry weight increase

**Defensive Buffs:**
- Damage resistance
- Specific damage type resistance (fire, cold, poison)
- Posture bonus
- Critical hit chance reduction (incoming)

**Utility Buffs:**
- Stealth bonuses
- Detection range
- Swimming ability
- Climbing stamina
- Sight distance

**Mental Buffs:**
- Rache cooldown reduction
- Focus regeneration
- Quick-reload bonuses
- Perception increase

**Unique Buffs:**
- Specific to legendary meals
- Sometimes story-relevant
- Rare effects
- Memorable experiences

### 4.2 Buff Duration Scaling

**Campfire Meals:** 5-10 minute buffs
**Ship Galley Meals:** 15-30 minute buffs
**Port Kitchen Meals:** 30-60 minute buffs
**Master Chef Meals:** 1-3 hour buffs
**Legendary Meals:** Varied (some entire session)

### 4.3 Buff Stacking

**Stacking Rules:**
- Same-type buffs don't stack (stronger wins)
- Different categories can stack (Physical + Mental)
- Maximum 3 simultaneous buffs
- Some legendary meals occupy 2 slots

**Strategic Considerations:**
- Before boss fights: maximize relevant buffs
- Long voyages: endurance buffs
- Stealth missions: appropriate buffs
- Resource management for consumption

### 4.4 Consuming Meals

**Full Meals:**
- Sit-down eating
- 30-second animation
- Full buff application
- Social context (crew present)

**Quick Eating:**
- Standing consumption
- 10-second animation
- 75% buff value
- In-field convenience

**Emergency Eating:**
- Mid-combat consumption
- Limited to certain foods
- 50% buff value
- Critical situations

---

## 5. PROCESSING ANIMAL PRODUCTS

### 5.1 Meat Processing

**Immediate Use (Fresh Meat):**
- Peak quality window: 12 hours after kill
- Best flavor and nutrition
- Recipes: Roasts, grilled, raw preparations
- Must cook same day typically

**Preservation Methods:**

**Salting:**
- Requires salt (sea or mined)
- 1-2 days preparation
- Preserves 2-4 weeks
- Texture change (tougher)
- Flavor: salty, requires rehydration

**Smoking:**
- Requires smoker setup
- 4-12 hours process
- Preserves 2-6 weeks
- Distinctive flavor
- Great with many recipes

**Drying (Jerky):**
- Air or heat drying
- 1-3 days process
- Preserves 1-3 months
- Portable, trail-food
- Tough texture

**Pickling:**
- Acid/salt preservation
- 1 day preparation
- 1-3 months storage
- Unique flavor
- Limited meat types

**Curing:**
- Long-term preservation
- 2-4 weeks preparation
- 3-6 months storage
- Premium product
- High value trade

### 5.2 Bone Processing

**Soup/Broth Base:**
- Slow simmering 4-12 hours
- Extracts nutrition
- Base for many recipes
- Economic (no waste)

**Bone Meal:**
- Ground bone
- Fertilizer use
- Craft material
- Alchemy component

**Tool Making:**
- Specific bones
- Big Tom's work
- Needles, awls, handles
- Cultural items

**Trophy/Decoration:**
- Antlers, skulls, teeth
- Display on ship
- Trade items
- Status symbols

### 5.3 Fat and Organ Processing

**Rendering Fat:**
- Melt animal fat over heat
- Strain into jars
- Creates cooking oil
- Candle-making material
- Waterproofing

**Tallow:**
- Beef/deer fat specific
- Candle base
- Leather working
- Long storage

**Lard:**
- Pig fat specific
- Cooking preference
- Pastry making
- Preservation

**Organ Specialties:**
- Heart (rare meal ingredient, powerful buffs)
- Liver (nutritional, alchemy)
- Kidney (specific preparations)
- Brain (controversial, strong buffs)
- Gall bladder (alchemy only)

### 5.4 Hide Processing

**Immediate Processing:**
- Skin carefully within 12 hours
- Clean flesh side
- Prepare for tanning

**Tanning Methods:**

**Brain Tanning (Traditional):**
- Uses animal's own brain
- 3-5 days process
- Soft, supple leather
- Labor intensive

**Bark Tanning:**
- Uses tree bark tannins
- 2-4 weeks process
- Tougher leather
- Industrial capable

**Salt Tanning:**
- Quick preservation
- 1-2 weeks
- Ready for later processing
- Portable technique

**Oil Tanning:**
- Uses animal oils
- 1-2 weeks
- Water-resistant leather
- Marine applications

### 5.5 Processing Special Parts

**Feathers:**
- Plucking and sorting
- Arrow fletching
- Decoration
- Pillow stuffing
- Quill pens

**Claws/Teeth:**
- Cleaning
- Polishing
- Jewelry making
- Weapon attachments
- Trophy display

**Shells/Scales:**
- Armor components
- Decorative items
- Craft materials
- Cultural significance

---

## 6. PROCESSING PLANT PRODUCTS

### 6.1 Fresh Use

Many plants used immediately:
- Salad greens
- Fresh fruits
- Garnish herbs
- Quick medicine

### 6.2 Drying

Most common preservation:

**Air Drying:**
- Hung in ventilated space
- 3-14 days depending on plant
- Preserves most qualities
- Cost-effective

**Sun Drying:**
- Direct sunlight
- Faster process
- Quality loss for some
- Weather-dependent

**Shade Drying:**
- Slower, gentler
- Preserves color, aroma
- Better for delicate herbs
- Traditional method

**Oven Drying:**
- Controlled temperature
- Faster than air
- Specific quality
- Ship/port capability

### 6.3 Grinding

**Hand Grinding:**
- Mortar and pestle
- Fine control
- Small batches
- Quality control

**Larger Grinders:**
- Port mills
- Commercial quantities
- Bulk processing
- Economic scale

**Resulting Products:**
- Herbal powders
- Spice blends
- Medicinal preparations
- Cooking ingredients

### 6.4 Extraction

**Oil Extraction:**

**Cold Pressing:**
- Mechanical pressing
- Preserves all qualities
- Premium oil
- Lower yield

**Hot Extraction:**
- Heat-based
- Higher yield
- Some quality loss
- Industrial scale

**Solvent Extraction:**
- Alcohol-based
- Specific compounds
- Alchemy preparation
- Advanced technique

**Tincture Making:**
- Alcohol + herbs
- 2-6 weeks steeping
- Medicinal preparations
- Ines's specialty

**Infusion Making:**
- Water + herbs
- Quick process
- Tea-like preparations
- Immediate use

### 6.5 Fermentation

**Alcoholic Fermentation:**
- Rum from sugar cane
- Mead from honey
- Wine from fruits
- Beer from grains
- Weeks to months

**Non-Alcoholic Fermentation:**
- Pickles and kimchi-like
- Sourdough
- Cultured foods
- Gut health benefits

### 6.6 Specific Plant Processing

**Coconut Processing:**
- Crack open shell
- Drain coconut water (drink)
- Extract white meat
- Press for coconut oil
- Dry meat for chips
- Shell for containers
- ALL PARTS USED

**Cocoa Processing:**
- Harvest pods
- Ferment 5-7 days
- Dry 1-2 weeks
- Roast carefully
- Grind to paste
- Make chocolate (luxury)

**Coffee Processing (Regional):**
- Cherry harvest
- Wet or dry processing
- Drying 1-3 weeks
- Roasting
- Grinding
- Brewing

---

## 7. COOKING RECIPE TIERS

### 7.1 Basic Recipes (Campfire Level)

**Simple Roasted Meat:**
- Ingredients: Any meat + fire
- Time: 5 minutes
- Buff: +50 HP, +10 stamina (10 minutes)
- Difficulty: Trivial

**Berry Medley:**
- Ingredients: 3 different berries
- Time: 1 minute
- Buff: +25 HP, +5% stamina regen (15 minutes)
- Difficulty: Trivial

**Basic Fish Stew:**
- Ingredients: Fish + water + salt
- Time: 20 minutes
- Buff: +75 HP, +15 stamina (20 minutes)
- Difficulty: Easy

**Campfire Coffee:**
- Ingredients: Coffee beans + water
- Time: 5 minutes
- Buff: +10% stamina regen (15 minutes)
- Difficulty: Trivial

### 7.2 Standard Recipes (Ship Galley)

**Venison Stew:**
- Ingredients: Deer meat + vegetables + herbs + broth
- Time: 1.5 hours
- Buff: +100 HP max, +20% stamina (45 minutes)
- Difficulty: Standard

**Spiced Pork Roast:**
- Ingredients: Pork + spice blend + fat
- Time: 2 hours
- Buff: +150 HP, +15% damage (1 hour)
- Difficulty: Standard

**Fish and Chips:**
- Ingredients: Fish + batter + oil
- Time: 30 minutes
- Buff: +80 HP, +10% stamina regen (30 minutes)
- Difficulty: Easy

**Coconut Curry:**
- Ingredients: Coconut milk + meat + spices + vegetables
- Time: 1 hour
- Buff: +120 HP, +15% damage reduction (45 minutes)
- Difficulty: Standard (requires Rexan recipe)

### 7.3 Advanced Recipes (Port Kitchens)

**Paella Rexana:**
- Ingredients: Rice + seafood + saffron + chicken + vegetables
- Time: 2 hours
- Buff: +200 HP, +25% stamina regen, +15% damage (1 hour)
- Difficulty: Advanced
- Region: Rexa specialty

**Fjordlund Reindeer Stew:**
- Ingredients: Reindeer + root vegetables + mushrooms + specific herbs
- Time: 3 hours
- Buff: +180 HP, +30% cold resistance, +20% stamina (1.5 hours)
- Difficulty: Advanced
- Region: Fjordlund specialty

**Ashenmoor Spiced Goat:**
- Ingredients: Goat + desert spices + cactus paddles + specific herbs
- Time: 2.5 hours
- Buff: +160 HP, +25% heat resistance, +15% damage (1 hour)
- Difficulty: Advanced
- Region: Ashenmoor specialty

**Imperial Royal Roast:**
- Ingredients: Noble stag + imperial spices + expensive wine reduction
- Time: 4 hours
- Buff: +250 HP, +20% all damages, +10% critical (1.5 hours)
- Difficulty: Expert
- Region: Pale Isle specialty
- Note: Expensive, class-significant

### 7.4 Legendary Recipes (Master Chef Only)

**The Sea-Witch's Feast:**
- Ingredients: Havfrue kelp (Peace Route) + legendary fish + pearls (dissolved) + specific herbs
- Time: 6 hours
- Buff: +400 HP, +30% damage, +25% stamina regen, Rache cooldown -50% (3 hours)
- Chef: Mormor Astrid
- Requires: Peace Route with Havfrue
- Unique: Only cookable once per playthrough

**Ethan's Childhood Dinner:**
- Ingredients: Specific meats Jake remembers + mother's recipe herbs + father's preferred sauce
- Time: 3 hours
- Buff: +300 HP, unique "Brotherhood" buff (all crew morale max, 4 hours)
- Chef: Jake alone (emotional cooking)
- Requires: Chapter 8+ revelation
- Story: Massive emotional beat when cooked

**The Dragon's Breath Roast:**
- Ingredients: Ashen Wyrm meat (Peace Route) + volcanic spices + dragon's blood (if obtained)
- Time: 8 hours
- Buff: +500 HP, fire immunity, +40% damage (4 hours)
- Chef: Mbah Seruni (only with high relationship)
- Requires: Peace Route with Ashen Wyrm
- Unique: Post-game only

**Captain's Farewell:**
- Ingredients: Best ingredients from all 6 regions
- Time: 12 hours
- Buff: All stats maximum for 8 hours, Rache infinite for 10 minutes
- Chef: Jake with full crew helping
- Requires: Chapter 10 preparation
- Story: The meal before the final confrontation

### 7.5 Post-Game Recipes

**Region 06 Specialties:**
- Unique plant combinations
- Philosophy as ingredient
- Reality-bending buffs
- Once-per-discovery

**Master's Secret Dishes:**
- Unlocked after all chefs taught
- Personalized recipes
- Jake's own signature dishes
- Named after his journey

---

## 8. THE SHIP'S DAILY MENU

### 8.1 Ramón's Role

**The Ship Cook:**
- Manages daily meal cycles
- Feeds entire crew
- Expertise builds throughout game
- Character depth through food

**Daily Routine:**
- Dawn: Breakfast preparation
- Midday: Light meal or bread
- Evening: Main meal
- Night: Leftovers and snacks

### 8.2 Crew Meals

**Breakfast:**
- Quick energy
- Minor stamina boost
- Communal atmosphere
- Daily bonding

**Midday Meal:**
- Sustenance focus
- No major buffs
- Work-fuel
- Quick consumption

**Evening Meal:**
- The important meal
- Full buffs
- Crew gathering
- Social depth

### 8.3 Jake's Eating Choices

**Eat with Crew:**
- Morale boost for crew
- Shared buff (smaller but for all)
- Relationship building
- Story moments

**Eat Alone:**
- Full buff for Jake
- Reflective time
- Journal writing opportunity
- Personal choice

**Special Meals:**
- Pre-battle dinner (morale-boosting)
- Victory feasts (major morale)
- Mourning meals (after losses)
- Celebration meals (achievements)

### 8.4 Crew Special Requests

Each crew member has food preferences:

**Mira:**
- Favorite: Fish dishes from Fjordlund
- Disliked: Overly spicy Rexan food
- Special: Mom's reindeer stew (quest unlocks)

**Big Tom:**
- Favorite: Heavy meat dishes
- Disliked: Light vegetarian meals
- Special: Coal-roasted everything

**Ines:**
- Favorite: Herbal and healing meals
- Disliked: Excessive meat
- Special: Her mother's remedy soup

**Father Salvio:**
- Favorite: Simple, blessed meals
- Disliked: Extravagance
- Special: Ritual meals on holy days

**Esteban:**
- Favorite: Sea-based dishes
- Disliked: Nothing (grateful)
- Special: Any meal with the crew

**Cooking Their Favorites:**
- Relationship boost
- Unique dialogue
- Personal stories shared
- Mechanical rewards

---

## 9. MEDICINAL PROCESSING (INES'S DOMAIN)

### 9.1 Tonic Preparation

**Basic Healing Tonic:**
- Ingredients: Red Valerian + Mountain Sage + Clean Water
- Time: 15 minutes (Ines)
- Effect: Restore 150 HP
- Shelf Life: 2 weeks

**Stamina Tonic:**
- Ingredients: Specific energizing herbs + alcohol base
- Time: 30 minutes
- Effect: +30% stamina regen for 45 minutes
- Shelf Life: 1 month

**Antidotes:**
- Specific counter-agents
- Quick crafting when needed
- Multiple types (poison, venom, toxin)
- Essential to carry

### 9.2 Advanced Alchemy

**Blood Tonic (Post-Havfrue):**
- Ingredients: Havfrue scale + specific herbs + blessed water
- Time: 4 hours
- Effect: Full heal + poison cure + status cure
- Shelf Life: 6 months

**Dragon's Fire Elixir (Post-Ashen Wyrm):**
- Ingredients: Dragon-adjacent materials + volcanic herbs
- Time: 6 hours
- Effect: Fire immunity + +40% damage for 30 minutes
- Shelf Life: 3 months

**Rache Booster:**
- Ingredients: Special herbs + Jake's own blood (drop)
- Time: 2 hours
- Effect: -75% Rache cooldown for 1 hour
- Special: Morally questionable?

### 9.3 Poisons

**Ines's Controlled Use:**
- Teaches Jake carefully
- Ethical considerations
- Specific applications
- Non-lethal options

**Types:**

**Sleep Poison:**
- Non-lethal incapacitation
- Useful for stealth
- Specific application
- Minimal moral weight

**Paralytic:**
- Immobilizes target
- Combat application
- Specific missions
- Moral weight

**Lethal Poisons:**
- Ines reluctant to make
- Specific story justification
- Major moral weight
- Ethical consequences

### 9.4 Ritual Preparations (Father Salvio)

**Blessed Water:**
- Standard water + priestly blessing
- Holy uses
- Specific combat applications (undead)
- Ceremonial value

**Holy Oils:**
- Extracted from specific plants
- Blessed in chapel
- Anointing uses
- Religious significance

**Incense Preparations:**
- Mixed aromatic plants
- Burned in chapel
- Atmospheric/ceremonial
- Some have combat utility (confusion, calm)

---

## 10. SPECIALTY PROCESSING

### 10.1 Tobacco Processing

**Brief Overview:**
Full details in `fauna/tobacco_system.md`

**Process Summary:**
- Harvest leaves at peak
- Sort by quality
- Cure (air, fire, or sun)
- Age 2-6 months
- Roll into cigars
- Quality grading

### 10.2 Cannabis Processing

**Brief Overview:**
Full details in `fauna/cannabis_system.md`

**Process Summary:**
- Harvest flowers
- Dry slowly (1-2 weeks)
- Cure for 4-8 weeks
- Prepare for consumption (roll, pipe, edibles)

### 10.3 Tea Processing

**Various Herbs:**
- Fresh or dried
- Specific water temperatures
- Steeping times
- Cultural significance

**Specific Teas:**
- **Fjordlund Healing Tea:** Mormor Astrid's specialty
- **Rexan Energy Tea:** Work-day beverage
- **Ashenmoor Cooling Tea:** Desert necessity
- **Imperial Afternoon Tea:** Formal protocol

### 10.4 Alcohol Production

**Rum:**
- Rexa specialty
- Sugar cane base
- Fermentation + distillation
- Years of aging for premium
- Crew morale essential

**Mead:**
- Fjordlund tradition
- Honey base
- Fermentation only
- Seasonal availability
- Ceremonial drink

**Wine:**
- Pale Isle culture
- Grape-based
- Wineries available
- Aging matters
- Status symbol

**Spirits/Liquors:**
- Various types
- High proof
- Medical uses (Ines)
- Trade value

---

## 11. PRESERVATION TECHNIQUES

### 11.1 Why Preservation Matters

**Voyage Necessity:**
- Long trips need stored food
- No refrigeration (historical)
- Crew must eat for months
- Planning essential

**Economic Value:**
- Preserved foods trade well
- Regional specialties
- Travel goods
- Long-term storage

### 11.2 Salt

**Traditional Essential:**
- Sea salt harvested
- Mineral salt mined
- Essential preservative
- Flavor enhancer

**Applications:**
- Meat curing
- Fish preservation
- Vegetable pickling
- Universal use

### 11.3 Smoke

**Fire Smoke Preservation:**
- Specific woods impart flavor
- Multi-day processes
- Creates distinct foods
- Protein focus

**Wood Choices:**
- Hickory: Standard
- Maple: Sweet
- Mesquite: Strong
- Apple: Mild
- Regional availability

### 11.4 Cold (Fjordlund Specialty)

**Ice Houses:**
- Natural cold storage
- Extends shelf life dramatically
- Northern culture advantage
- Trade benefit

**Freezing:**
- Fish particularly
- Long-term storage
- Quality preservation
- Seasonal planning

### 11.5 Dehydration

**Sun/Wind/Fire:**
- Remove moisture
- Prevents decay
- Extends life significantly
- Portable food

**Rehydration:**
- Soak in water before use
- Recovers most qualities
- Cooking adjusted
- Traveler's resource

### 11.6 Fermentation

**Lactic Acid Preservation:**
- Sauerkraut-like foods
- Kimchi-adjacent
- Yogurt-like products
- Traditional worldwide

**Vinegar:**
- Produced from wine/beer
- Pickling base
- Preservation medium
- Cooking acid

---

## 12. COMMUNITY COOKING

### 12.1 Sharing Food

**Crew Meals:**
- Build bonds
- Share stories
- Morale boost
- Regular occurrence

**Community Feasts:**
- Port celebrations
- Crew achievements
- Regional holidays
- Major story beats

**Gift Giving Food:**
- NPCs appreciate
- Specific preferences
- Relationship building
- Cultural exchange

### 12.2 Cooking for Others

**Preparing for Friends:**
- Quest rewards
- Specific characters
- Favorite foods matter
- Shows care

**Community Events:**
- Festivals
- Weddings
- Funerals
- Celebrations

**Cultural Learning:**
- Teaching others Jake's style
- Learning regional ways
- Cultural exchange
- World-building through food

---

## 13. THE RITUAL OF COOKING

### 13.1 Slow Cooking as Meditation

**Extended Preparations:**
- 2+ hour dishes
- Jake's time to think
- Journal writing during
- Crew conversations while cooking

**Craft Appreciation:**
- Fine details matter
- Skill development visible
- Pride in work
- Personal improvement

### 13.2 The Story of Food

**Each Meal Has:**
- Source of ingredients (regional, seasonal)
- Teacher of recipe (NPC, book, experience)
- Occasion (context when cooked)
- Reception (who enjoyed it)

**Jake's Cookbook:**
- Grows throughout game
- Personal annotations
- Favorite recipes marked
- Memories attached

### 13.3 Emotional Cooking

**Stress Cooking:**
- After major story beats
- Jake processing
- Specific recipes emerge
- Character moment

**Celebration Cooking:**
- Joy in preparation
- Bigger meals
- Special ingredients used
- Team involvement

**Memorial Cooking:**
- Remembering lost ones
- Specific recipes
- Mother's dish
- Father's favorite
- Deeply emotional

---

## 14. ACCESSIBILITY

### 14.1 Cooking Simplification

**Auto-Cook:**
- Skip animations
- Instant preparation
- Quality preserved
- Time-saver option

**Recipe Suggestions:**
- Based on inventory
- Priority by usefulness
- Quick access
- Tutorial support

### 14.2 Visual Accessibility

- Large recipe displays
- Clear ingredient lists
- High-contrast UI
- Animation skipping

### 14.3 Dietary Accommodations

**Vegetarian Path:**
- All meals accessible via plant alternatives
- Alternative recipe versions
- Equal buff potency
- Respects preferences

**Pacifist-Compatible:**
- No hunting-based meals required
- Gathering emphasis
- Trade alternatives
- Complete playable experience

---

## 15. INTEGRATION WITH OTHER SYSTEMS

### 15.1 Hunting System

- Meat quality from hunt → meal quality
- Specific animal parts → specific recipes
- Legendary hunts → legendary meals
- Ecosystem health → ingredient availability

### 15.2 Gathering System

- Plant quality → recipe quality
- Regional herbs → regional cuisine
- Seasonal gathering → seasonal eating
- Sustainable harvest → ongoing supply

### 15.3 Inventory & Crafting

- Processed goods storage
- Recipe ingredients
- Preservation mechanics
- Weight considerations

### 15.4 Ship Interior/Management

- Galley as central location
- Ramón as key crew
- Upgrades affect cooking
- Economic considerations

### 15.5 Interaction System

- NPCs discuss food
- Food as gift
- Cultural exchange
- Social bonding

---

## 16. TUTORIAL PROGRESSION

### 16.1 Chapter 1-2

- Basic campfire cooking
- Simple meals taught
- Ramón introduction
- First recipes

### 16.2 Chapter 3-4

- Ship galley usage
- Intermediate recipes
- Regional cooking begins
- Crew preferences shown

### 16.3 Chapter 5-7

- Port kitchens accessible
- Master chefs met
- Advanced preservation
- Legendary recipes begin

### 16.4 Chapter 8-10

- Full cooking mastery
- Emotional cooking moments
- Final feast preparation
- Character-defining meals

### 16.5 Post-Game

- Region 06 cuisine
- Master chef status
- Recipe collection complete
- Jake as teacher

---

## 17. PROCESSING FEEL TESTS

### 17.1 The RDR2 Test
> *"Does cooking feel satisfying and meaningful? Are meals worth preparing?"*

### 17.2 The BotW Test
> *"Is cooking experimentation encouraging? Do discoveries feel rewarding?"*

### 17.3 The Monster Hunter Test
> *"Do meal buffs genuinely affect gameplay? Are they worth hunting for ingredients?"*

### 17.4 The Cultural Test
> *"Do regional cuisines feel distinct? Does cooking teach about culture?"*

### 17.5 The Character Test
> *"Does cooking reveal character? Do meals have emotional weight?"*

All five must pass.

---

## 18. IMPLEMENTATION NOTES

### 18.1 Technical Requirements
- Recipe database system
- Ingredient tracking and quality
- Buff management system
- Preservation timer tracking
- Cooking animation library

### 18.2 Content Requirements
- 150+ standard recipes fully designed
- 30+ legendary recipes
- Regional variation
- Cultural authenticity

### 18.3 Balance Requirements
- Buff tuning (not game-breaking)
- Ingredient availability
- Cooking time pacing
- Economic balance

### 18.4 Audio Requirements
- Sizzling, bubbling, chopping sounds
- Specific per-ingredient audio
- Environmental cooking ambience
- Satisfying completion audio

---

## 19. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** Chefs speak in Middle Register with elevation to High for ceremonial dishes; Ramón uses Low with crew warmth; master chefs elevate based on occasion
- ✅ **Pillar 2 (Lore Through Objects):** Every legendary meal tells a story (Ethan's Childhood Dinner, The Sea-Witch's Feast, Captain's Farewell — these dishes ARE lore)
- ✅ **Pillar 3 (Layered Death):** Memorial meals for fallen crew represent Style D grief; specific dishes for specific deaths; food as mourning
- ✅ **Pillar 4 (Layered Humor):** Style C quirks in food (Big Tom's charred preferences, Ramón's dramatic complaints, crew food disputes) — cooking as humor source
- ✅ **Pillar 5 (Theme — Revenge):** Emotional cooking reflects Jake's state; Ethan's Dinner is confrontation with what he's lost; Captain's Farewell is preparation for what he might lose

---

## 20. NEXT STEPS

**FOUNDATION TIER COMPLETE.**

Next document: **`fauna/land_animals.md`** — Content Tier Document 1 (largest content block)

Will cover:
- All 40+ mammal species
- Behavior patterns per species
- Regional distributions
- Tracking data
- Materials provided
- Legendary variants
- Cultural significance

After Land Animals: Sea Animals, then continuing through Content Tier.

---

## 21. CONCLUSION

Processing and cooking is where the world's raw abundance becomes Jake's personal experience. Every meal is memory. Every preservation is planning. Every tonic is care. The pirate era respected whole-ingredient use because of necessity; Dark Arisen honors this respect because it creates better games.

From a bear's liver stew cooked at a lonely beach to Captain's Farewell prepared with crew for the final battle — cooking in Dark Arisen is never merely stats. It is story.

**Phase 4 Subsystem 3 of 14 — complete.**

**FOUNDATION TIER COMPLETE.** 🔥🍳⚓

**Content Tier begins next.**
