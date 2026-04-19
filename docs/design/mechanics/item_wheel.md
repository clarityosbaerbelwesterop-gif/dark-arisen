# ITEM WHEEL SYSTEM

**Status:** Design Draft v1.0  
**Phase:** Phase 3 — Mechanics (Subsystem 6 of 10)  
**Tier:** Interface Tier — Peer to Weapon Wheel, depends on Combat Feel  
**Related:** mechanics_overview.md, mechanics/weapon_wheel.md, mechanics/combat_feel.md, style_bible.md  
**Purpose:** Define the L1+R1 hold item radial — how Jake uses consumables, healing items, food, tobacco, alcohol, and emergency tools during combat and exploration.  
**Inspirations:** Red Dead Redemption 2 (primary reference — item wheel and food/tobacco effects), Resident Evil (emergency healing tension), Fallout New Vegas (drug/alcohol/food system), Witcher 3 (potion preparation), Metal Gear Solid V (item management)

---

## 1. ITEM WHEEL PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Consumables Are Decisions, Not Buttons

In many games, healing is a one-button reflex — press item, HP restored. In Dark Arisen, consumables require deliberate choice. Which healing type? How much HP needed? Are status effects also present? This creates meaningful resource management.

**Design intent:** Every healing decision is tactical. Running out of good healing items is a real consequence of poor planning.

### 1.2 Principle 2: Food Is Game Mechanic, Not Flavor

RDR2 showed that food consumption can be meaningful gameplay. Dark Arisen expands this: Jake eats fruits, fish, bread, meat — each with specific effects over time. Food is not just HP restoration; it provides temporary buffs, prevents scurvy, interacts with weather systems.

**Design intent:** A well-fed Jake fights better. A starving Jake performs poorly. Planning for long voyages requires thinking about food.

### 1.3 Principle 3: Tobacco, Rum, and Substances Matter

Pirate setting demands authentic pirate items. Tobacco fuels Rache (revenge focus). Rum provides pain-dampening but visual blur. Specific substances have effects. This isn't gratuitous — it's narratively authentic and mechanically meaningful.

**Design intent:** Jake is a pirate in 1876, not a sanitized action hero. He smokes, he drinks, he uses substances. The game treats this maturely.

### 1.4 Principle 4: Emergency Tools Save Lives

Emergency items (smoke bombs for escape, healing tonics, morale boosters) are not optional nice-to-haves. In certain encounters, they are critical. Mastering their use separates skilled from unskilled players.

**Design intent:** The game respects players who prepare. Rewards planning ahead.

### 1.5 Principle 5: Wheel Accessibility

Like the weapon wheel, the item wheel must be usable under combat pressure. Time dilation, intuitive layout, clear information display. The wheel is a tool, not an interruption.

---

## 2. ITEM WHEEL ACTIVATION

### 2.1 Input Specifications

**PS5/Xbox Controller:**
- **Hold L1 + R1 simultaneously** — Item wheel opens
- **R Stick** — Navigate to item category
- **L Stick** — Navigate within sub-wheel (specific item)
- **Release L1+R1** — Wheel closes, selected item used

**PC:**
- **Hold Q + F** — Item wheel opens
- **Mouse movement** — Navigate wheel
- **Release** — Item used

### 2.2 Timing and Feel

**Opening:**
- **Delay:** 150ms from input to wheel visible (slightly slower than weapon wheel)
- **Animation:** Wheel scales from center with item-specific icons
- **Audio:** Satchel opening sound (cloth + buckle)
- **Haptic (PS5):** Double soft pulse in both grips

**While Open:**
- **Time dilation:** 30% speed (same as weapon wheel)
- **Combat continues:** Enemies move slowly, projectiles travel slowly
- **Rally window continues:** Cannot escape into the wheel

**Closing:**
- **Close speed:** 200ms fade-out
- **Use animation:** Item-specific (0.8-2.0 seconds)
- **Audio:** Item-specific consumption sound
- **Haptic:** Varies by item type

### 2.3 Quick Access Alternatives

**D-Pad Left (default — quick healing):**
- Instantly uses best available healing item
- No wheel open
- 0.8 second use animation
- Vulnerable during use

**Custom Quick Slots:**
- Player can assign specific items to D-Pad
- One-tap access for critical items
- Most useful for power players

---

## 3. THE FOUR CATEGORIES — WHEEL QUADRANTS

The item wheel is divided into four quadrants, each representing a category:

### 3.1 Quadrant 1: HEALING (Top — 12 o'clock)

**Purpose:** Direct HP restoration.

**Items Include:**

**Bandages (Basic):**
- Restores 40 HP over 3 seconds
- Common item, easily crafted/bought
- Stops light bleeding
- Use time: 1.0 second
- Vulnerable during use

**Cauterizing Bandage:**
- Restores 60 HP over 4 seconds
- Stops heavy bleeding instantly
- Painful (brief stagger)
- Use time: 1.5 seconds

**Healing Tonic (Minor):**
- Restores 80 HP instantly
- Single-use potion
- Rarer, requires herbs to craft
- Use time: 0.8 seconds

**Healing Tonic (Major):**
- Restores 160 HP instantly
- Boss-fight item, rare
- Expensive to craft
- Use time: 1.0 second

**Blood Tonic (Havfrue Blessing — Peace Route):**
- Restores 100% HP + heals status effects
- Single-use per chapter (not per fight)
- Reward for Havfrue Peace Route
- Use time: 0.5 seconds

### 3.2 Quadrant 2: BUFFS/TONICS (Right — 3 o'clock)

**Purpose:** Temporary enhancements.

**Items Include:**

**Strength Tonic:**
- +20% melee damage for 60 seconds
- Use time: 1.0 second
- Crafted from specific herbs

**Dexterity Tonic:**
- +20% attack speed for 60 seconds
- Use time: 1.0 second

**Resilience Tonic:**
- +30% damage reduction for 45 seconds
- Use time: 1.0 second

**Stamina Tonic:**
- Stamina regen +50% for 90 seconds
- Use time: 1.0 second

**Focus Tonic:**
- Rache cooldown -50% for 120 seconds
- Use time: 1.0 second

**Blood Focus Tonic:**
- Combines strength + resilience (stacked)
- +15% damage, +20% reduction, 45 seconds
- Rare crafting

**Combat Meal (Pre-battle):**
- Consumed before combat
- +10% all stats for 180 seconds
- Requires eating time (out of combat)

### 3.3 Quadrant 3: FOOD / DRINK (Bottom — 6 o'clock)

**Purpose:** Sustained HP restoration and hunger management.

**Items Include:**

**FRUIT:**
- Orange: +15 HP, prevents scurvy (long voyages)
- Apple: +10 HP
- Coconut: +20 HP, extra water
- Banana: +12 HP, quick energy

**MEAT:**
- Fish Fillet: +30 HP, common on ship
- Salted Pork: +40 HP, preserved
- Smoked Beef: +50 HP, rare
- Ship's Biscuit + Meat: Full meal, +80 HP

**BREAD/VEGETABLES:**
- Bread: +15 HP, common
- Hardtack: +10 HP, keeps forever
- Fresh Vegetables: +20 HP
- Stew (cooked): +60 HP, hot food bonus

**DRINK:**
- Water: Restores stamina regen in hot climates
- Tea (British): +10 HP, warms in cold
- Ale: +5 HP, minor buzz
- Wine: +15 HP, social effect

**The Hunger Mechanic (Optional):**
- Jake can become hungry on long voyages
- If not eaten for 12+ hours of gameplay: -10% stamina regen
- If not eaten for 24+ hours: -20% stamina regen, possible HP drain
- Warns player before penalties

### 3.4 Quadrant 4: EMERGENCY / SPECIAL (Left — 9 o'clock)

**Purpose:** Critical situation items.

**Items Include:**

**Smoke Bomb:**
- Deploys smoke (4m radius, 8 seconds)
- Use for combat escape
- Reduces enemy detection
- Use time: 0.5 seconds

**Fire Bomb:**
- Thrown fire projectile
- Area damage, sets enemies on fire
- Use time: 0.8 seconds

**Antitoxin:**
- Cures poison status
- Also cures buildup before trigger
- Use time: 1.0 second

**Revival Draught:**
- If crew member is downed, can revive them
- Major consequence if not used in time
- Use time: 2.0 seconds

**Signal Flare:**
- Fired into air, signals specific crew actions
- Can call allies from afar
- Fort Carrion siege: specific signals
- Use time: 0.8 seconds

**Emergency Rum Flask (Pain-Killer):**
- Dampens damage intake by 30% for 30 seconds
- BUT +20% posture damage from hits (less stability)
- Jake slightly staggers — visual/audio effect
- Use time: 1.0 second

**Tobacco (Hand-Rolled Cigar):**
- Fuels Rache activation
- Single cigar = 1 Rache use
- Multiple cigars stored
- Use time: 3.0 seconds (Jake lights up)

---

## 4. TOBACCO, RUM, AND THE PIRATE LIFE

### 4.1 Tobacco System

Tobacco is more than flavor — it's mechanic.

**Sources of Tobacco:**
- Tobacco plants in Rexa (Caribbean region)
- Tobacco shops in villages
- Smuggler trades
- Gift from specific NPCs

**Tobacco Preparation:**
- Raw tobacco leaves (gathered)
- Requires crafting: Dry, cure, roll
- Craft station needed: Cabin, inn, workshop
- Each bundle creates 5 hand-rolled cigars

**Cigars as Rache Fuel:**
- Each Rache use consumes 1 cigar
- Jake carries max 20 cigars
- Limited resource adds strategic depth

**Secondary Effects:**
- Lighting a cigar before combat provides brief "calm" bonus
- +10% precision for first 10 seconds after smoking
- Social: certain NPCs appreciate offered cigars

### 4.2 Rum and Alcohol System

Drinking is part of pirate life — with consequences.

**Types of Alcohol:**

**Rum (Light):**
- Restores 10 HP
- No immediate effects
- Social acceptability

**Rum (Heavy):**
- Restores 20 HP
- -10% accuracy for 120 seconds
- +20% pain tolerance
- Social bonding with crew

**Grog (Crew Drink):**
- Basic ship drink
- Restores 8 HP, minor effects
- Standard supply

**Wine (Quality):**
- 25 HP, prestigious social item
- Calms crew morale
- Used in cabin conversations

**Whiskey (Imperial):**
- 30 HP, rare
- Status symbol
- Special effect in specific quests

### 4.3 The Pain-Killer Mechanic

Rum can be used strategically as combat painkiller:

**Pre-Combat Use:**
- Jake drinks rum flask
- -30% damage taken for next 30 seconds
- Visual: Slight screen blur
- Audio: Muffled sounds
- Balance: +20% posture damage (less stable)

**Risks:**
- Accuracy reduced (-10%)
- Reactions slightly slower (-2 frames)
- Addictive over long term (Chapter 10 epilogue consideration)

### 4.4 Tobacco and Rum Storage

Jake's carrying capacity for these items:
- Tobacco: Max 20 cigars + 5 tobacco bundles (for crafting more)
- Rum: Max 5 flasks (different types combined)
- Managed through inventory menu (not wheel-accessible for storage)

---

## 5. STATUS EFFECT CURES

Combat Feel (Section 8.5) defined four status effects. Each has specific cures:

### 5.1 Bleed Cure

**Bandage:**
- Stops bleeding buildup
- Prevents trigger
- Use time: 1.0 second

**Cauterizing Bandage:**
- Stops heavy bleeding already triggered
- Pain but immediate
- Use time: 1.5 seconds

### 5.2 Poison Cure

**Antitoxin:**
- Stops poison buildup
- Cures active poison effect
- Use time: 1.0 second

**Clean Water:**
- Partial poison reduction (25%)
- Basic item
- Use time: 0.8 seconds

### 5.3 Burn Cure

**Cold Compress:**
- Stops burn damage
- Soothes burned areas
- Use time: 1.2 seconds

**Water Immersion:**
- Free (if water available)
- Instant extinguish
- Use time: 2.0 seconds

### 5.4 Frozen Cure

**Hand Warmer:**
- Restores heat quickly
- +10 HP bonus
- Use time: 1.0 second

**Hot Tea:**
- Specific Fjordlund item
- Warms + calms
- +15 HP bonus
- Use time: 1.5 seconds

**Fire Activity:**
- Standing by fire for 30 seconds
- Free but takes time
- Full cure

---

## 6. ITEM USE MECHANICS

### 6.1 Use Animations

Each item has a specific use animation:

**Quick Items (0.5 - 1.0 second):**
- Drink from flask
- Apply basic bandage
- Throw smoke bomb

**Standard Items (1.0 - 1.5 seconds):**
- Apply tonic
- Eat fruit
- Take tonic

**Long Items (1.5 - 2.0 seconds):**
- Eat meal (stop and chew)
- Light cigar (fire matches, puff)
- Prepare emergency tonic

### 6.2 Vulnerability During Use

While using items, Jake is vulnerable:

**Actions blocked during use:**
- Attacks (R1, R2)
- Sprinting
- Heavy movement

**Actions allowed during use:**
- Walking (slow)
- Looking around
- Dodging (cancels use, no effect)
- Communication with crew

### 6.3 Cancellation

Player can cancel mid-use:

**Circle (Dodge) cancel:**
- Instantly cancels use
- Does NOT consume item
- Stamina cost for dodge

**L1 + R1 cancel:**
- Opens item wheel again
- Can select different item
- Previous item still consumed

### 6.4 Multi-Use Items

Some items can be used in sequence:

**Drinking (Rum/Water):**
- Jake can drink multiple flasks
- Same animation, combined effects
- Stamina cost doubled

**Smoking (Cigars):**
- Jake lights single cigar
- Uses until finished or canceled
- Cannot interrupt mid-smoke

### 6.5 Environmental Effects

Environment affects item use:

**Rain:**
- Cigars harder to light (may fail, takes extra time)
- Fire items may extinguish (failed use)
- Wet bandages less effective

**Cold:**
- Some items freeze (rum, water)
- Hand warmers more effective
- Tea especially welcome

**Heat:**
- Water extra valuable
- Food spoils faster
- Rum dehydrates (negative effect)

---

## 7. ITEM WHEEL UI AND DISPLAY

### 7.1 Wheel Layout

**Central Hub:**
- Jake's current HP (miniature)
- Stamina bar
- Status effects visible
- Current buffs active

**Four Quadrants (Categories):**
- Healing (Top)
- Buffs/Tonics (Right)
- Food/Drink (Bottom)
- Emergency (Left)

**Per-Category Sub-Items:**
- 4-6 items per category
- Arranged radially within quadrant
- Icons and quantity indicators

### 7.2 Icon Design

**Healing Items:**
- Red cross motif
- Bandage, vial, tonic icons
- Color-coded by strength

**Buff Items:**
- Blue crystal or star motif
- Each buff type has distinct color
- Duration indicator visible

**Food Items:**
- Brown/gold color
- Recognizable silhouettes (apple, fish, bread)
- Freshness indicator

**Emergency Items:**
- Yellow/black warning coloring
- Distinct from other quadrants
- Critical visibility

### 7.3 Information Display

**On cursor hover:**
- Item name
- Quantity remaining
- Effect summary (brief)
- Duration (for buffs)
- Quality rating (for food)

**Extended info (hold Triangle):**
- Full effect description
- Cooldowns (if any)
- Crafting info
- Story/flavor text

### 7.4 Visual Feedback

**Selection:**
- Green glow on hovered item
- Slight pulsing animation
- Clear cursor visibility

**Use Confirmation:**
- Item icon dims after use
- Number counter decrements
- Effect appears on character UI

**Warnings:**
- Low quantity: Yellow highlight
- Critical quantity: Red highlight
- Unavailable: Grayed out

---

## 8. INTEGRATION WITH OTHER SYSTEMS

### 8.1 Item Wheel and Combat Feel

**During combat:**
- Wheel use matters tactically (time-dilated but not paused)
- Rally window continues
- Posture regenerates normally
- Enemies adjust their behavior (if observed)

**Mid-fight item use:**
- Dangerous — commit carefully
- Some bosses will specifically target Jake during use
- Reading enemy patterns important

### 8.2 Item Wheel and Stealth

**Silent consumption:**
- Most items make minimal noise
- Can be used while hidden
- No auditory detection for most uses
- Exception: Cigar lighting creates brief light + sound

### 8.3 Item Wheel and Exploration

**Exploration use:**
- Healing items for between combats
- Food for sustained energy
- Weather-appropriate items
- Stamina tonics for long climbs

**Out-of-combat pace:**
- No time dilation
- More relaxed selection
- Can double-check items before use

### 8.4 Item Wheel and Crafting

**Direct link:**
- Items crafted in workshop go directly to wheel
- Wheel reflects current inventory
- Changes visible immediately

### 8.5 Item Wheel and Ship Life

**Ship-specific items unlocked:**

**Repair Kit:**
- Restores La Liberación hull HP
- Used while at sea or in port
- Crew member uses it
- Use time: 5.0 seconds

**Morale Booster (Crew):**
- Grog distribution to crew
- Temporary crew combat bonus
- Use time: 2.0 seconds

**Navigation Tools:**
- Sextant: Reveals precise location
- Compass: Ship direction assist
- Spyglass: Distant observation

---

## 9. CONTEXT-SPECIFIC ITEM VARIATIONS

### 9.1 Land Combat Items

**Standard wheel with:**
- All healing items
- Combat buffs
- Food (quick consumption)
- Emergency items

### 9.2 Ship Helm Items

**Naval-specific:**
- Ship repair kits
- Signal flares
- Sextant (navigation)
- Crew morale boosters
- Emergency rum (for crew)

### 9.3 Ship Deck Combat Items

**Mixed:**
- Standard combat items
- Ship-specific boarding tools
- Grappling hook fuel
- Crew rally items

### 9.4 Stealth Items

**Silent variants prioritized:**
- Silent bandages (no noise)
- Poison coatings (stealthy)
- Smoke bombs (for escape)
- Antitoxin (if poisoned by target)

### 9.5 Cabin/Safe Items

**Full inventory access:**
- Can combine items
- Can craft within wheel
- Can assign items to wheel slots

---

## 10. ITEM CRAFTING AND ACQUISITION

### 10.1 Gathering Resources

**Plant materials:**
- Herbs (various throughout regions)
- Tobacco leaves (Rexa)
- Fruits (Caribbean, tropical)
- Flowers (Fjordlund, healing)

**Animal materials:**
- Fish (for food)
- Game animals (meat, leather)
- Sea creatures (rare, ocean specific)

**Mineral materials:**
- Salt (for preservation)
- Specific minerals for alchemy
- Gunpowder components

### 10.2 Crafting Stations

**Ship Cabin:**
- Basic healing crafting
- Simple tonics
- Tobacco preparation

**Inn/Tavern:**
- More advanced tonics
- Preserved foods
- Social crafting (recipes from NPCs)

**Specialist Shops:**
- Alchemy shops (buffs, status cures)
- Weapon smiths (not items directly)
- Merchants (ingredients)

### 10.3 Recipe Learning

**How Jake learns:**
- Recipes found in quests
- NPCs teach through dialogue
- Books provide knowledge
- Experimentation (minor discovery)

### 10.4 Crafting Time

**Simple items (bandages, basic tonics):** 15-30 seconds  
**Standard items (healing tonics, food):** 30-60 seconds  
**Complex items (advanced tonics, special foods):** 1-3 minutes  
**Master items (rare crafts):** 5-15 minutes

---

## 11. COMMON ITEM USAGE PATTERNS

### 11.1 The Boss Fighter

**Preparation:**
- 5 Healing Tonics (Major)
- 3 Strength Tonics
- 2 Resilience Tonics
- 2 Focus Tonics (for Rache)
- Full cigar inventory
- 1 Blood Focus Tonic

**Usage pattern:** Aggressive buff stacking before engagement.

### 11.2 The Explorer

**Preparation:**
- 8 Bandages (common healing)
- 4 Stamina Tonics
- Fruits and preserved foods
- Weather-specific items
- No tobacco/rum (saves resources)

**Usage pattern:** Efficiency, economy, sustainability.

### 11.3 The Stealth Operative

**Preparation:**
- Silent bandages (4)
- Smoke bombs (5)
- Poison coatings (3)
- Antitoxin (2)
- Limited food (quiet types)

**Usage pattern:** Precise, minimalist, escape-focused.

### 11.4 The Pirate Classic

**Preparation:**
- Rum flask (2)
- Cigars (15)
- Standard healing
- Emergency items
- Crew morale booster

**Usage pattern:** Authentic pirate experience, social items prioritized.

### 11.5 The Minimalist

**Preparation:**
- 3 Healing items
- 2 Emergency items
- Basic food
- No buffs

**Usage pattern:** Relies on skill over items. Challenging self-imposed.

---

## 12. PLATFORM-SPECIFIC FEATURES

### 12.1 PS5 DualSense

**Haptic per item:**
- Bandage application: Soft wrapping sensation
- Tonic drinking: Gulping pulse
- Cigar lighting: Match strike + puff
- Rum: Heavy swallow + warm spread
- Food: Chewing rhythm

**Adaptive triggers:**
- Flask opening: Resistance of cork/cap
- Bow drawing for fire arrows: Progressive tension

### 12.2 Xbox Series X/S

**Controller rumble:**
- Simplified version of DualSense
- Primary impact feedback
- Trigger resistance where supported

### 12.3 PC

**Keyboard/Mouse:**
- Q+F for item wheel
- Mouse for selection
- Keybind customization

---

## 13. ACCESSIBILITY

### 13.1 Motor Accessibility

**Simplified wheel:**
- Toggle instead of hold
- Fewer quadrants option (2 categories)
- Extended selection time

**Auto-use options:**
- Auto-heal on low HP
- Auto-cure status effects
- Auto-use emergency items in critical situations

### 13.2 Visual Accessibility

**Icon clarity:**
- Large, distinctive icons
- Text labels option
- High contrast mode

**Information display:**
- Enlarged quantity numbers
- Clear effect descriptions
- Extended hover time

### 13.3 Audio Accessibility

**Audio cues:**
- Per-item sound (distinctive)
- Quantity low warnings (audible)
- Use confirmation tones

### 13.4 Cognitive Accessibility

**Simplified inventory:**
- Essentials-only option
- Clear categorization
- Recipe tutorials

---

## 14. TUTORIALS

### 14.1 Chapter 1 Introduction

**First Item Use:**
- Jake takes damage, needs healing
- Simple 2-item wheel (bandage + rum)
- Tutorial teaches L1+R1 hold
- Time dilation demonstrated

### 14.2 Chapter 2 Expansion

**Category Variety:**
- Buffs introduced
- Food system activates
- Crafting basics

### 14.3 Chapter 4 Ship Integration

**Naval Items:**
- Ship repair kits
- Morale items
- Navigation tools

### 14.4 Chapter 6 Advanced

**Full System:**
- All 4 categories active
- Complex tonics available
- Advanced crafting

### 14.5 Chapter 9 Rache Integration

**Rache Fuel:**
- Tobacco importance
- Rum strategic use
- Resource management under pressure

---

## 15. ITEM WHEEL FEEL TEST

Before the item wheel is final, it must pass these tests:

### 15.1 The RDR2 Test
> *"Does the item wheel feel as natural as RDR2's? Is the tobacco/rum system authentic and meaningful?"*

### 15.2 The Tactical Depth Test
> *"Do different item preparations create meaningfully different combat approaches?"*

### 15.3 The Combat Fluidity Test
> *"Can a skilled player use the wheel mid-combat without breaking flow?"*

### 15.4 The Resource Management Test
> *"Is running out of good items a real consequence? Does planning ahead matter?"*

### 15.5 The Accessibility Test
> *"Can players with motor limitations still use the wheel effectively?"*

All five must pass. Then the item wheel is ready.

---

## 16. IMPLEMENTATION NOTES

### 16.1 Visual Requirements
- 4 quadrant wheel design
- Item icons with quantity overlays
- Buff timer indicators
- Status effect visualization
- Weather-affected items (rain on matches, etc.)

### 16.2 Animation Requirements
- Item-specific use animations
- Consumption animations (eating, drinking)
- Lighting animations (cigars)
- Applying animations (bandages)
- Emergency animations (smoke bomb deployment)

### 16.3 Audio Requirements
- Per-item sound effects
- Chewing, drinking, striking match
- Buff activation sounds
- Consumption confirmations

### 16.4 System Integration
- Time dilation synchronized with weapon wheel
- Status effect interactions
- Weather system integration
- Environment affects item usability

---

## 17. CONSISTENCY WITH EXISTING DESIGN

All existing documentation remains compatible:

- **Combat System:** Items support all combat mechanics
- **Boss Encounters:** Items valid during all boss fights
- **Status Effects:** Cures match combat_feel.md specifications
- **Rache System:** Tobacco fueling matches mechanics_overview.md

No existing documents need revision.

---

## 18. NEXT STEPS

Next document: **`mechanics/inventory_crafting.md`** — Inventory & Crafting System

Will cover:
- Satchel inventory (RDR2-style)
- Weight management
- Resource gathering
- Crafting recipes and stations
- Material acquisition
- Storage (ship cabin, chests)
- Trading and economy
- Legendary crafting

Estimated time: 3-4 hours.

After Inventory/Crafting: Interaction System (dialogue, looting, world interaction).

**The item wheel is the tactical resource management system. This document defines every item, every effect, every interaction. Phase 3 Subsystem 6 of 10 — complete.**
