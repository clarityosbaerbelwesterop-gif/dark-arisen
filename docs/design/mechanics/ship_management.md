# SHIP MANAGEMENT SYSTEM

**Status:** Design Draft v1.0  
**Phase:** Phase 3 — Mechanics (Subsystem 11 of 11 — FINAL)  
**Tier:** World Tier — Depends on all previous tiers  
**Related:** mechanics_overview.md, naval_combat_system.md, mechanics/ship_interior.md, mechanics/ship_navigation.md, mechanics/inventory_crafting.md, style_bible.md  
**Purpose:** Define the strategic captain-level decisions — ship upgrades, crew management, repairs, economics, and late-game fleet command.  
**Inspirations:** Assassin's Creed Black Flag (ship upgrade trees), Mass Effect 2 (crew recruitment and loyalty), FTL (crew management under pressure), Sid Meier's Pirates! (economic depth), Sea of Thieves (ship customization), Mount & Blade (fleet command)

---

## 1. MANAGEMENT PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Every Upgrade Is a Choice

La Liberación cannot be everything. More cannons mean less cargo space. Reinforced hull means reduced speed. Luxury quarters mean less storage. Every upgrade forces a trade-off. Players must decide what kind of captain they want to be.

**Design intent:** There is no optimal build. Different playstyles demand different configurations. The ship reflects the player's strategy.

### 1.2 Principle 2: Crew Are People, Not Resources

When a crew member dies, they stay dead. When one leaves, they rarely return. Each has family to support, personal goals, injuries that linger. Replacing crew is not the same as upgrading them. The player manages human lives, not interchangeable units.

**Design intent:** Every crew death should hurt. Every hiring decision should matter.

### 1.3 Principle 3: Repair Is Not Always Available

Ship damage is persistent. Without proper resources, supplies, or ports, La Liberación stays damaged. Combat-damaged ships perform worse. Storm damage compounds. Management means PLANNING — carrying spare wood, knowing the nearest friendly port, rationing supplies.

**Design intent:** Ship health is a real concern, not a regenerating meter.

### 1.4 Principle 4: Economics Have Weight

Money matters. Crew need wages. Repairs cost real gold. Upgrades require significant investment. Trade routes become essential. Piracy becomes tempting. Economic decisions shape the game as much as combat ones.

**Design intent:** Dark Arisen is not a resource-abundance game. Scarcity creates meaningful choice.

### 1.5 Principle 5: Fleet Command Is Earned

Late in the game, Jake can command multiple ships. This is not automatic — it requires specific story progression, reputation, and resource commitment. Commanding a fleet means commanding lives. It is a responsibility, not a reward.

**Design intent:** Scaling power comes with scaling consequence.

---

## 2. SHIP UPGRADE SYSTEM

### 2.1 Upgrade Categories

La Liberación can be upgraded across six major categories:

**1. Hull (Defense)**
- Reinforced planking
- Armor plating
- Ram (offensive addition)
- Keel upgrades (stability)

**2. Sails (Speed and Maneuverability)**
- Larger sail configurations
- Quality canvas
- Reinforced rigging
- Storm sails (weather-resistant)

**3. Cannons (Firepower)**
- Number of cannons
- Quality of cannons
- Specialized ammunition
- Auxiliary weapons (swivel guns, mortars)

**4. Capacity (Storage)**
- Cargo hold size
- Weapon stores
- Crew quarters expansion
- Provisions capacity

**5. Navigation (Efficiency)**
- Improved instruments
- Better charts
- Enhanced spyglass
- Weather prediction tools

**6. Luxury (Morale)**
- Captain's cabin improvements
- Crew quarters comfort
- Galley upgrades
- Chapel enhancements

### 2.2 Hull Upgrades

**Tier 1 — Reinforced Planking:**
- Cost: 1,500 Doubloons + 20 Ship Wood
- Effect: +500 ship HP
- Trade-off: -0.5 knots speed
- Required: Port access, 3 days construction

**Tier 2 — Iron Plating:**
- Cost: 4,000 Doubloons + 50 Steel + 30 Ship Wood
- Effect: +1,200 ship HP, +15% ram damage
- Trade-off: -1 knot speed, +30% weight
- Required: Major port, 7 days construction

**Tier 3 — Dragon-Bone Reinforcement (Chapter 6+):**
- Cost: 8,000 Doubloons + 10 Dragon-bone + 30 Steel
- Effect: +2,000 ship HP, +25% ram damage, immune to fire
- Trade-off: -1.5 knots speed, cannot dock at Imperial ports (visible threat)
- Required: Ashenmoor specialist, 10 days

**Tier 4 — Master's Hull (Post-Game):**
- Cost: 15,000 Doubloons + unique materials
- Effect: +3,500 ship HP, +40% ram damage
- Trade-off: -2 knots speed, ship becomes landmark
- Required: Specific post-game questline

### 2.3 Sail Upgrades

**Tier 1 — Reinforced Rigging:**
- Cost: 1,200 Doubloons + 15 Rope
- Effect: +10% storm resistance, minor speed boost
- Trade-off: Minimal
- Required: Rigging specialist, 2 days

**Tier 2 — Larger Sail Configuration:**
- Cost: 3,500 Doubloons + 20 Silk/Canvas
- Effect: +1.5 knots speed, +20% tacking
- Trade-off: -20% storm resistance
- Required: Major port, 5 days

**Tier 3 — Silk Sails:**
- Cost: 6,000 Doubloons + 30 Fine Silk
- Effect: +2.5 knots speed, +35% tacking
- Trade-off: Fragile (damage in rough weather)
- Required: Luxury port access, 7 days

**Tier 4 — Storm Sails (Fjordlund Special):**
- Cost: 5,000 Doubloons + Fjordlund materials
- Effect: +50% storm resistance, full speed in bad weather
- Trade-off: -10% speed in calm weather
- Required: Fjordlund master sailmaker, 10 days

### 2.4 Cannon Upgrades

**Base La Liberación:** 12 cannons (6 per side)

**Tier 1 — Quality Cannons:**
- Cost: 500 Doubloons per cannon + 3 Iron per cannon
- Effect: +20% damage
- Trade-off: None
- Required: Blacksmith (Big Tom or port), 1 day per 2 cannons

**Tier 2 — Extended Battery:**
- Cost: 4,000 Doubloons + major construction
- Effect: +6 cannons total (9 per side, 18 total)
- Trade-off: -200 cargo capacity, +crew requirement
- Required: Major port, 10 days

**Tier 3 — Specialized Ammunition:**
- Cost: 3,000 Doubloons per type + materials
- Types:
  - Chain Shot (vs. rigging, +40% sail damage)
  - Grape Shot (vs. crew, +60% anti-personnel)
  - Hot Shot (vs. hull, fire damage)
  - Explosive Shot (rare, +100% hull damage)
- Trade-off: Specialized, not universal

**Tier 4 — Swivel Guns and Mortars:**
- Cost: 5,000 Doubloons + special materials
- Effect: +2 swivel guns (rapid fire, short range), +1 mortar (long range, high damage)
- Trade-off: -50 cargo capacity, complex maintenance
- Required: Master gunner, 7 days

### 2.5 Capacity Upgrades

**Cargo Hold:**
- Tier 1: +100 kg (Small expansion) — 1,000 Doubloons
- Tier 2: +300 kg (Medium expansion) — 3,000 Doubloons
- Tier 3: +700 kg (Large expansion) — 6,000 Doubloons
- Trade-offs: Ship sits deeper, -0.2 to -0.8 knots

**Weapon Stores:**
- Tier 1: +500 weapons/ammo capacity — 800 Doubloons
- Tier 2: +1,500 capacity — 2,500 Doubloons
- Trade-offs: Minor weight

**Crew Quarters:**
- Tier 1: +3 crew capacity (max 18 total) — 2,000 Doubloons
- Tier 2: +5 crew capacity (max 23 total) — 5,000 Doubloons
- Required: For larger crew
- Trade-offs: -100 cargo per tier

**Provisions Storage:**
- Tier 1: +20% food/water capacity — 1,500 Doubloons
- Tier 2: +50% capacity — 4,000 Doubloons
- Essential for long voyages

### 2.6 Navigation Upgrades

**Improved Sextant:**
- Cost: 2,000 Doubloons + precision components
- Effect: +25% navigation accuracy
- Required: Instrument maker (Esteban or port)

**Detailed Charts:**
- Cost: 1,500 Doubloons per region
- Effect: Full map visibility, all landmarks, hidden areas
- Required: Cartographer or specific discoveries

**Long-Range Spyglass:**
- Cost: 3,000 Doubloons + crystal lenses
- Effect: 3x zoom, detail visibility at distance
- Required: Optical specialist

**Weather Prediction Tools:**
- Cost: 4,000 Doubloons + scholarly materials
- Effect: Predict weather 6-12 hours ahead
- Required: Esteban Tier 3+

### 2.7 Luxury Upgrades (Morale)

**Captain's Cabin Improvements:**
- Personal touches: +5% crew morale
- Displays for trophies
- Larger map table
- Better bed/desk

**Crew Quarters Comfort:**
- Softer hammocks: +10% crew morale
- Better lighting
- Personal space dividers
- Entertainment materials

**Galley Upgrades:**
- Better equipment: +15% food quality
- Larger cooking area
- Spice storage
- Preservation systems

**Chapel Enhancements:**
- Religious decorations
- Better candles
- Expanded space
- Effect: +10% spiritual crew morale

### 2.8 The Upgrade Tree

Upgrades require specific order:

**Basic Path (Chapter 1-4):**
- Tier 1 upgrades available
- Limited ports
- Simple improvements

**Intermediate (Chapter 4-7):**
- Tier 2 upgrades unlock
- Regional specialists accessible
- Meaningful trade-offs

**Advanced (Chapter 7-9):**
- Tier 3 upgrades available
- Legendary specialists
- Major transformations

**Master (Post-Game):**
- Tier 4 upgrades accessible
- Unique combinations
- La Liberación becomes legend

### 2.9 Upgrade Installation

**Process:**
1. Visit appropriate specialist (port or crew)
2. Review available upgrades
3. Confirm materials and cost
4. Agree to construction time
5. Wait or pay for faster work

**Time Considerations:**
- Ship must stay in port during most upgrades
- Some can be done at sea (minor)
- Rush jobs cost more
- Interrupted upgrades lose partial cost

**Impact:**
- Ship unavailable during work
- Crew can disembark or help
- Story progression may be affected
- Side quests available during wait

---

## 3. CREW MANAGEMENT

### 3.1 Crew Composition

**Base Crew (Chapter 4):**
- Jake (Captain)
- Mira (First Mate / Navigator)
- Big Tom (Ship's Blacksmith)
- Ines (Ship's Alchemist)
- Father Salvio (Ship's Chaplain)
- Esteban (Master Navigator)
- 6-8 general sailors

**Total: 12-15 crew initially**

### 3.2 Named Crew (Officers)

**Mira — First Mate:**
- Role: Second-in-command, navigation expert
- Skills: Helm mastery, maritime combat, crew discipline
- Personal Quest: Finding her missing brother
- Loyalty: High from start, grows to devotion
- Cannot be fired (core crew)

**Big Tom — Blacksmith:**
- Role: Weapons, repairs, armor
- Skills: Smithing, forge work, combat
- Personal Quest: Confronting his past
- Loyalty: Earned through fair treatment
- Cannot be fired without major consequence

**Ines — Alchemist:**
- Role: Potions, medicine, poisons
- Skills: Alchemy, healing, chemistry
- Personal Quest: Finding her missing daughter
- Loyalty: Deep but conditional on moral choices
- Can leave if Jake's actions horrify her

**Father Salvio — Chaplain:**
- Role: Spiritual guidance, blessed items
- Skills: Religious crafting, counseling, morale
- Personal Quest: Testing his faith
- Loyalty: Unconditional but can break
- Will leave if Jake crosses specific moral lines

**Esteban — Master Navigator:**
- Role: Navigation, celestial expertise, teaching
- Skills: All navigation arts, wisdom
- Personal Quest: Final voyage acceptance
- Loyalty: Immediate and absolute
- May die of old age (Chapter 9+ event)

### 3.3 General Sailors

**Backgrounds:**
- Various origins (Caribbean, Fjordlund, Imperial, Ashenmoor)
- Personal stories (briefer than officers)
- Skills vary (combat, sailing, crafting, service)

**Management:**
- Can be hired at ports
- Can be dismissed (with severance)
- Can leave voluntarily (poor treatment)
- Can die (combat, disease, accident)

### 3.4 Hiring New Crew

**Available at Ports:**
- Posted notices at harbors
- Bars and taverns
- Recommendations from current crew
- Rescued sailors from events

**Hiring Process:**
1. Meet potential hire
2. Interview (dialogue tree)
3. Negotiate wages
4. Confirm hire
5. Integration period

**Wage Structure:**
- Basic sailor: 50 Doubloons/week
- Skilled sailor: 80 Doubloons/week
- Veteran: 120 Doubloons/week
- Specialist: 200+ Doubloons/week

### 3.5 Crew Development

**Skill Growth:**
- Crew improve with experience
- Survive battles = combat skills
- Long voyages = sailing skills
- Time with Big Tom = smithing aptitude
- Variable paths per crew member

**Loyalty Growth:**
- Fair treatment builds loyalty
- Fair pay matters
- Respect shown matters
- Difficult choices tested

**Relationship Dynamics:**
- Crew develop relationships among themselves
- Rivalries possible
- Friendships form
- Couples possible (some crew romance)

### 3.6 Crew Loyalty System

**Loyalty Scale: 0-100**

**0-20 (Mutinous):**
- Crew actively considers leaving
- Passive resistance
- Potential desertion
- Possible mutiny (rare, severe)

**21-40 (Unhappy):**
- Work slower
- Complain openly
- Reduced combat effectiveness
- Morale contagious

**41-60 (Content):**
- Normal function
- Neutral attitudes
- Professional relationships

**61-80 (Loyal):**
- Eager to please
- Voluntary extra work
- Defend captain's decisions
- Strong bonds

**81-100 (Devoted):**
- Would die for Jake
- Inspired performance
- Unshakeable morale
- Family feeling

### 3.7 Loyalty Modifiers

**Positive:**
- Victory in combat: +5 to +15
- Good treatment: +2 per instance
- Fair pay: +5 per month
- Personal attention: +3 per conversation
- Completing crew quests: +20

**Negative:**
- Combat losses: -5 to -20
- Poor treatment: -3 per instance
- Missed pay: -10 per incident
- Ignoring crew: -2 per week
- Breaking promises: -15

### 3.8 Crew Deaths

**Permanent Consequences:**
- Dead crew stay dead
- Memorial service on ship
- Belongings distributed
- Empty hammock visible
- Effects on surviving crew morale

**Named Crew Deaths:**
- Story-specific events
- Cannot be prevented in some cases
- Major emotional beats
- Long-lasting consequences

**General Crew Deaths:**
- Combat casualties
- Disease outbreaks
- Accidents
- Storm deaths

**Mourning Period:**
- 1-3 days of reduced function
- Reduced crew morale
- Ship runs minimal operations
- Specific dialogue available

### 3.9 Firing Crew

**Reasons to Fire:**
- Insubordination
- Poor performance
- Financial necessity
- Reputation management
- Specific moral reasons

**Firing Process:**
- Formal dismissal
- Severance required (1 month wages minimum)
- Reputation impact
- Disembark at next port

**Consequences:**
- Other crew notice
- Potential morale drop
- Fired crew may spread negative reputation
- Some fired crew return as enemies (rare)

---

## 4. REPAIR SYSTEM

### 4.1 Ship Damage Types

**Hull Damage:**
- Combat impacts
- Storm damage
- Ramming
- Ground contact (running aground)

**Sail Damage:**
- Combat (chain shot)
- Storm tears
- Wear and tear
- Fire damage

**Rigging Damage:**
- Combat
- Storms
- Rot over time
- Crew accidents

**Cannon Damage:**
- Combat counter-fire
- Misfire accidents
- Corrosion from salt
- Heavy use

### 4.2 Repair Methods

**At Sea (Limited):**
- Minor repairs only
- Uses ship supplies (wood, rope, canvas)
- Slower than port repair
- Temporary patches

**At Friendly Port:**
- Full repairs available
- Faster with payment
- Specialist work possible
- All damage addressable

**At Hostile Port:**
- Expensive
- Poor quality
- Risk of betrayal
- Emergency only

**At Secret Ports (Pirate Havens):**
- No questions asked
- Premium prices
- Full services
- Underground network

### 4.3 Repair Costs

**Light Damage (0-25% HP):**
- Cost: 200-500 Doubloons
- Time: 1 day
- Materials: Basic

**Moderate Damage (25-50% HP):**
- Cost: 1,000-2,000 Doubloons
- Time: 2-3 days
- Materials: Wood, rope, canvas

**Heavy Damage (50-75% HP):**
- Cost: 3,000-5,000 Doubloons
- Time: 5-7 days
- Materials: Extensive

**Critical Damage (75-100% HP):**
- Cost: 8,000+ Doubloons
- Time: 10+ days
- Materials: Major components
- May require transport to specialized port

### 4.4 Preventive Maintenance

**Regular Maintenance:**
- Crew performs at sea
- Requires supplies
- Prevents decay
- Morale boost

**Careening:**
- Beaching ship for hull cleaning
- Specific locations needed
- 2-3 days process
- Improves ship performance

**Ongoing Upgrades:**
- Small improvements constantly
- Keep ship in peak condition
- Crew takes pride in

### 4.5 Emergency Repairs

**In Combat:**
- Crew can patch damage
- Father Salvio blesses
- Big Tom emergency welds
- Temporary measures

**After Storm:**
- Immediate damage assessment
- Critical repairs first
- Crew exhaustion
- Supply usage

**Battle Aftermath:**
- Wounded ship but alive
- Careful return to port
- Emergency rations
- Crew morale crucial

---

## 5. ECONOMIC SYSTEM

### 5.1 Revenue Sources

**Quest Rewards:**
- Main story progression
- Side quest completions
- Reputation-based rewards

**Trade:**
- Regional price differences
- Cargo transportation
- Specialty goods
- Trade route optimization

**Piracy:**
- Boarding merchant ships
- Raiding coastal settlements
- Ransom demands
- Moral cost

**Crew Income:**
- Salvage from wrecks
- Discovered treasures
- Fishing operations
- Pearl diving

**Investments:**
- Port property
- Business partnerships
- Letters of marque
- Banking

### 5.2 Expenses

**Fixed Costs:**
- Crew wages (weekly)
- Ship maintenance
- Food supplies
- Standard operations

**Variable Costs:**
- Repairs (damage-dependent)
- Upgrades (when pursued)
- Special purchases
- Emergency expenses

**Luxury Costs:**
- Quality food
- Better accommodations
- Entertainment
- Reputation building

### 5.3 Financial Management

**Income Tracking:**
- Journal records all income
- Crew informed of finances
- Transparency builds trust
- Strategic planning

**Expense Management:**
- Crew wages prioritized
- Essential supplies next
- Repairs third
- Luxuries last

**Emergency Funds:**
- Reserve for emergencies
- Rule of thumb: 1 month wages minimum
- Prevents crisis management
- Enables opportunity

### 5.4 Bankruptcy Consequences

**Missing Wages:**
- Crew morale plummets
- Potential desertion
- Reputation damage
- Financial spiral

**Unable to Repair:**
- Ship performance degrades
- Combat effectiveness drops
- Potential sinking
- Stranded in hostile waters

**Running Debt:**
- Borrowing from pirates/merchants
- Interest rates high
- Enforcement possible
- Long-term burden

### 5.5 Regional Economics

**Rexa (Colonial/Pirate):**
- Mid-range prices
- Black market available
- Smuggling opportunities
- Variable reliability

**Fjordlund (Trade Hub):**
- Higher prices generally
- Quality goods
- Reliable merchants
- Long-term relationships

**Imperial Ports (Chapter 8):**
- Highest prices
- Best quality
- Dangerous for pirates
- Formal regulations

**Pirate Havens:**
- Black market prices
- No questions
- Moderate quality
- Underground network

---

## 6. FLEET COMMAND (LATE GAME)

### 6.1 Earning Fleet Command

**Requirements:**
- Chapter 9+ story progression
- Specific reputation threshold
- Financial resources
- Trusted sub-commanders

**Story Triggers:**
- Specific events unlock
- Alliance formations
- Resource accumulation
- Proven leadership

### 6.2 Fleet Composition

**Jake's Personal Ship:**
- La Liberación (primary)
- Always Jake's flagship
- Most customized
- Personal emphasis

**Sub-Commander Ships:**
- 2-5 additional vessels
- Commanded by trusted officers
- Different specializations
- Coordinated operations

**Temporary Allies:**
- Story-based joint operations
- Specific missions
- Temporary arrangements
- Reliability varies

### 6.3 Ship Specializations

**Flagship (La Liberación):**
- Jake's personal command
- Balanced capabilities
- Most customized
- Center of fleet

**Combat Ships:**
- Heavy armor and guns
- Slower but powerful
- Boarding specialists
- Defensive role

**Scout Ships:**
- Fast, light, maneuverable
- Reconnaissance
- Intelligence gathering
- Flanking operations

**Cargo Ships:**
- Large holds
- Minimal armament
- Economic focus
- Protected by fleet

**Specialized (Fire Ships, etc.):**
- Unique roles
- Limited uses
- Tactical options
- Situational deployment

### 6.4 Sub-Commander Selection

**From Current Crew:**
- Mira as sub-commander (most common)
- Other trusted officers
- Promoted from general crew
- Proven loyalty required

**External Allies:**
- Former enemies redeemed
- Regional leaders joining
- Special recruitment events
- Unique capabilities

**Sub-Commander Stats:**
- Leadership (affects their crew's performance)
- Combat skill
- Navigation
- Specialization areas

### 6.5 Fleet Operations

**Coordinated Missions:**
- Multi-ship operations
- Tactical positioning
- Communication via signal flags
- Strategic depth

**Split Operations:**
- Fleet divides for multiple objectives
- Jake commands one group
- Sub-commanders lead others
- Report back afterward

**Large Battles:**
- Fleet vs. fleet combat
- Tactical overview
- Individual ship control possible
- Massive scale

### 6.6 Fleet Management

**Communication:**
- Signal flags
- Specific instructions
- Ship-to-ship calls (when close)
- Pre-planned protocols

**Supply Chains:**
- Fleet needs enormous resources
- Supply ships essential
- Port networks vital
- Economic complexity

**Personnel:**
- Each ship needs full crew
- Management of hundreds of sailors
- Training and discipline
- Morale across fleet

### 6.7 Fleet Consequences

**Larger Footprint:**
- Imperial threat level increases
- Cannot use small ports
- More visible to enemies
- Reputation effects amplified

**Greater Risk:**
- Can lose entire ships
- Casualties larger
- Economic devastation possible
- Responsibility heavy

**Greater Reward:**
- Major objectives achievable
- Regional dominance possible
- Legend-tier accomplishments
- Historic significance

---

## 7. PORT INTERACTIONS

### 7.1 Port Categories

**Major Ports:**
- Full services
- All specialists
- Dense commerce
- Information hub
- Political center

**Minor Ports:**
- Basic services
- Limited specialists
- Local trade
- Character backstories
- Community focus

**Pirate Havens:**
- Underground services
- Black market
- No authorities
- Dangerous but useful
- Network connections

**Hidden Ports:**
- Secret locations
- Specific access needed
- Unique services
- Hidden benefits

### 7.2 Port Services

**Standard Services:**
- Repair facilities
- Supply purchase
- Crew recruitment
- General goods

**Specialist Services:**
- Alchemy shops
- Blacksmiths (weapons)
- Religious institutions
- Cartographers
- Luxury goods

**Entertainment:**
- Taverns (information)
- Gambling houses
- Brothels (reputation risk)
- Theaters (culture)

**Political:**
- Authority offices
- Legal services
- Letters of marque
- Diplomacy

### 7.3 Port Relationships

**Friendly Ports:**
- Fair prices
- Quick service
- Special offers
- Priority access

**Neutral Ports:**
- Standard prices
- Normal service
- No special treatment
- Transactional

**Hostile Ports:**
- Overcharge
- Slow service
- Potential confrontation
- Dangerous to visit

**Banned Ports:**
- Cannot enter
- Potential attack on sight
- Reputation permanently damaged
- Must find alternatives

### 7.4 Port Events

**Random Port Events:**
- Recruitable veterans
- Interesting encounters
- Side quest hooks
- Rumors and information

**Seasonal Events:**
- Festivals
- Special markets
- Trading windows
- Cultural observations

**Political Events:**
- Authority changes
- New laws
- Wars and alliances
- Opportunity and danger

### 7.5 Jake's Reputation

**Overall Reputation Tracking:**
- Regional reputations vary
- Specific actions remembered
- Word travels (sometimes)
- Long-term consequences

**Impact on Ports:**
- Welcome vs. wanted
- Prices and availability
- Service quality
- Information access

**Building Reputation:**
- Consistent behavior
- Completing quests
- Word of mouth
- Deliberate cultivation

---

## 8. SUPPLY MANAGEMENT

### 8.1 Essential Supplies

**Food:**
- Daily consumption
- Based on crew size
- Quality affects morale
- Spoilage over time

**Water:**
- Daily consumption
- Critical for survival
- Spoilage rapid
- Replenishment vital

**Rum/Alcohol:**
- Morale item
- Also medicinal
- Weekly consumption
- Cultural importance

**Gunpowder:**
- Combat necessity
- Storage danger (explosion risk)
- Quality matters
- Specialist purchase

### 8.2 Supply Calculations

**Crew of 15 for 30 days:**
- Food: 450 daily rations
- Water: 450 liters minimum
- Rum: 105 bottles (standard ration)
- Gunpowder: Combat-dependent

**Scaling with Crew:**
- Per crew member per day
- Weight considerations
- Storage space
- Spoilage timing

### 8.3 Supply Shortage

**Food Shortage:**
- Reduced rations
- Morale drops
- Combat effectiveness reduced
- Hunger events possible

**Water Shortage:**
- Rapid crisis
- Desperate measures
- Potential deaths
- Emergency port needed

**Morale Shortage (Rum):**
- Subtle effects
- Crew dissatisfaction
- Minor issues compound

**Gunpowder Shortage:**
- Combat avoidance necessary
- Vulnerable to attack
- Emergency resupply critical

### 8.4 Supply Strategies

**Regular Resupply:**
- Port stops planned
- Known suppliers
- Predictable costs
- Reliable system

**Foraging:**
- Island stops for food
- Hunting and fishing
- Less reliable
- Adventure-based

**Trade Opportunities:**
- Buy low in one region, sell high
- Cargo makes profit
- Economic complexity
- Risk vs. reward

**Piracy Supplementation:**
- Seize from captured ships
- Moral cost
- Supply security
- Reputation impact

---

## 9. SHIP STATUS UI

### 9.1 Captain's Overview

**Main Status Screen:**
- Ship health
- Crew status
- Cargo summary
- Finance overview
- Current objective

**Accessible From:**
- Captain's cabin (primary)
- Map table (quick version)
- Pause menu

### 9.2 Detailed Management

**Ship Details:**
- All upgrades installed
- Damage status (by location)
- Upgrade availability
- Performance statistics

**Crew Roster:**
- All crew listed
- Individual statuses
- Loyalty levels
- Personal quests
- Skills and specializations

**Economic Dashboard:**
- Current funds
- Weekly budget
- Upcoming expenses
- Income tracking
- Trade opportunities

### 9.3 Management Menus

**Upgrade Menu:**
- Available upgrades
- Costs and benefits
- Trade-offs shown
- Planning tools

**Crew Menu:**
- Hiring options
- Management decisions
- Personal quest progress
- Relationship dynamics

**Repair Menu:**
- Current damage
- Repair priorities
- Available specialists
- Time and cost

### 9.4 Decision Support

**Recommendations:**
- System-suggested actions
- Context-appropriate
- Based on situation
- Optional guidance

**Consequences Preview:**
- Show impact of decisions
- Short and long-term
- Trade-off clarity
- Informed choice

---

## 10. STRATEGIC DEPTH

### 10.1 Build Archetypes

**The Combat Captain:**
- Heavy armor and cannons
- Combat-specialized crew
- Aggressive reputation
- Piracy focus

**The Merchant Captain:**
- Large cargo capacity
- Skilled negotiators
- Clean reputation
- Trade network

**The Explorer Captain:**
- Speed and navigation
- Discovery-focused
- Curious crew
- Wandering path

**The Diplomat Captain:**
- Balanced capabilities
- Skilled speakers
- Reputation building
- Political navigation

**The Legend Captain (Late Game):**
- Master of all
- Elite crew
- Legendary ship
- Everyone knows

### 10.2 Playstyle Support

Each archetype has dedicated:
- Upgrade paths
- Crew specializations
- Quest lines
- Endings

**Design Intent:** Players who lean into an archetype feel rewarded, but hybrids are viable.

### 10.3 Endgame Goals

**Complete La Liberación:**
- All Tier 4 upgrades
- Legendary crew
- Perfect reputation
- Master Captain

**Fleet Dominance:**
- Maximum ships
- Regional control
- Strategic influence
- Political power

**Personal Legacy:**
- Jake's story complete
- Specific ending achieved
- Personal satisfaction
- Meaningful conclusion

---

## 11. SHIP MANAGEMENT UI

### 11.1 Captain's Desk

**Central Management Hub:**
- Located in captain's cabin
- Primary management interface
- All systems accessible
- Strategic planning space

**Contents:**
- Ship management books
- Crew ledgers
- Financial records
- Map collection
- Personal journal

### 11.2 Map Table Integration

**Strategic Overview:**
- Fleet positions (late game)
- Trade routes
- Enemy territories
- Friendly ports
- Resource locations

**Decision Making:**
- Plan operations
- Allocate resources
- Coordinate fleet
- Strategic depth

### 11.3 Crew Ledger

**Complete Crew Records:**
- All crew documented
- Service history
- Pay records
- Personal information
- Achievement tracking

**Management Tools:**
- Promotion decisions
- Assignment changes
- Dismissal processing
- Memorial entries

### 11.4 Upgrade Planning

**Upgrade Workspace:**
- Current ship configuration
- Upgrade options displayed
- Cost calculations
- Timeline planning

**Trade-off Visualizer:**
- Before/after comparisons
- Performance impact
- Strategic implications
- Informed decisions

---

## 12. PLATFORM-SPECIFIC FEATURES

### 12.1 PS5 DualSense

**Haptic Feedback:**
- Coin counting (financial)
- Hammer strikes (repairs)
- Crew interactions
- Important decisions

**Audio:**
- 3D crew conversations
- Port ambience
- Financial transactions
- Specialist workshops

### 12.2 Xbox and PC

Similar implementations adapted to platform.

---

## 13. ACCESSIBILITY

### 13.1 Management Simplification

**Simplified Mode:**
- Automatic optimizations
- Reduced micromanagement
- Core decisions only
- Streamlined interface

**Guided Management:**
- Tutorial hints persistent
- Recommendations active
- Simplified menus
- Step-by-step processes

### 13.2 Economic Accessibility

**Financial Assistance:**
- Optional budget recommendations
- Automatic expense handling
- Simplified trading
- Clear profit/loss

### 13.3 Visual Accessibility

**Interface Options:**
- Scalable UI
- Color-blind modes
- High contrast
- Large text

---

## 14. TUTORIAL PROGRESSION

### 14.1 Chapter 1-3

**Basic Management:**
- Simple crew
- Minimal upgrades
- Basic economics
- Learning fundamentals

### 14.2 Chapter 4-6

**Intermediate Systems:**
- Full officer crew
- Tier 1-2 upgrades
- Regional economics
- Strategic thinking

### 14.3 Chapter 7-8

**Advanced Management:**
- Complex upgrades
- Large crew
- Multi-regional operations
- Reputation management

### 14.4 Chapter 9+

**Master Captain:**
- Fleet command possible
- All systems active
- Strategic mastery
- Legacy building

---

## 15. MANAGEMENT FEEL TESTS

Before management is final, it must pass these tests:

### 15.1 The Black Flag Test
> *"Does the upgrade system feel as satisfying as Black Flag's?"*

### 15.2 The FTL Test
> *"Does crew management feel meaningful? Does every death hurt?"*

### 15.3 The Pirates! Test
> *"Does the economic system have Sid Meier's depth? Are trade routes interesting?"*

### 15.4 The Captain Test
> *"Does Jake feel like a real captain making real decisions?"*

### 15.5 The Legacy Test
> *"Does the late-game fleet command feel like legendary captaincy? Does it satisfy?"*

All five must pass.

---

## 16. IMPLEMENTATION NOTES

### 16.1 Technical Requirements
- Complex economic simulation
- Crew AI with individual personalities
- Ship customization engine
- Fleet coordination systems
- Save state complexity

### 16.2 Content Requirements
- Detailed upgrade tree
- Character-rich crew options
- Regional economic variation
- Multiple ship types (late game)

### 16.3 Balance Requirements
- Upgrade costs vs. benefits
- Economic scarcity management
- Crew loyalty tuning
- Fleet command complexity

---

## 17. INTEGRATION WITH EXISTING SYSTEMS

### 17.1 Naval Combat

- Upgrades directly affect combat
- Crew roles in battle
- Damage persists
- Strategic depth enhanced

### 17.2 Ship Interior

- Upgrade visibility
- Crew presence
- Physical manifestation of decisions

### 17.3 Ship Navigation

- Performance affects sailing
- Crew morale in voyages
- Supply management critical

### 17.4 Story System

- Management decisions affect narrative
- Crew deaths meaningful
- Reputation shapes world
- Endings vary by choices

---

## 18. CONSISTENCY WITH EXISTING DESIGN

All existing documentation remains compatible. Previously specified crew members (Mira, Big Tom, Ines, Father Salvio, Esteban) have their roles formalized here without contradiction.

---

## 19. THE CAPTAIN'S JOURNEY

From Chapter 4 forward, Jake grows from having a ship to BEING a captain in the fullest sense. The management system is how that transformation happens mechanically. Every decision — from which upgrade to install to which crew member to trust with command — shapes who Jake becomes.

By Chapter 9, the player should feel the weight of what Jake has built. His ship. His crew. His reputation. His legacy. These are not just game systems — they are the concrete manifestation of everything Jake has earned and endured.

This system exists to make that journey tangible.

---

## 20. PHASE 3 MECHANICS — COMPLETE

With this document, **Phase 3 Mechanics is officially complete.**

**The 12 subsystems of Dark Arisen's mechanics are:**
1. ✅ Master Overview
2. ✅ Controls Foundation
3. ✅ Combat Feel
4. ✅ Traversal
5. ✅ Stealth
6. ✅ Weapon Wheel
7. ✅ Item Wheel
8. ✅ Inventory & Crafting
9. ✅ Interaction
10. ✅ Ship Interior
11. ✅ Ship Navigation
12. ✅ Ship Management

**The Mechanics layer of Dark Arisen is now fully designed.**

Combined with the previously completed Bosses (21) and Regions (6), Dark Arisen has:
- A complete cast of story antagonists
- A fully-designed open world
- A complete mechanical foundation

**Next Phases (in any order Flo chooses):**
- Phase 4: Physics
- Phase 5: Cutscenes
- Phase 6: Small Items/Lore Catalog
- Phase 7: Ship Functions (additional detail)
- Phase 8: Animals
- Phase 9: NPCs (3000+ named)
- Phase 10: Voiceover
- Phase 11: Final Cutscenes

**Phase 3 Mechanics — complete.**

**La Liberación now has her full soul.**

🏴‍☠️⚓
