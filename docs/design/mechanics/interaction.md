# INTERACTION SYSTEM

**Status:** Design Draft v1.0  
**Phase:** Phase 3 — Mechanics (Subsystem 8 of 10)  
**Tier:** World Tier — Depends on all previous tiers  
**Related:** mechanics_overview.md, all other mechanics files, style_bible.md  
**Purpose:** Define how Jake interacts with the world — NPCs, objects, clues, dialogues, environmental systems, and social cues. The bridge between Jake and everything else.  
**Inspirations:** Red Dead Redemption 2 (primary — NPC reactions, dialogue, investigation), Disco Elysium (dialogue depth, perception), The Witcher 3 (investigation quests), Assassin's Creed Origins (eagle vision investigation), Elden Ring (environmental storytelling)

---

## 1. INTERACTION PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: The World Reacts to Jake

Jake is not a camera floating through static environments. He is a man whose presence affects the world. NPCs notice him. Doors react to his approach. Dogs bark when he's too close. Birds take off when he runs through them. The world acknowledges his existence.

**Design intent:** Every environment feels alive because it responds to Jake specifically.

### 1.2 Principle 2: NPCs Have Memory

NPCs remember Jake. What he said. What he did. How he behaved. A shopkeeper Jake helped in Chapter 3 recognizes him in Chapter 7. A priest Jake insulted tells others. This creates a persistent social network that Jake navigates throughout the game.

**Design intent:** The player sees consequences of their choices reflected in how the world treats them.

### 1.3 Principle 3: Investigation Is Legitimate Gameplay

Like RDR2's missions that require observation and deduction, Dark Arisen offers investigation as a legitimate approach. Not every problem needs a sword. Some need a careful eye, a questioned NPC, a studied clue.

**Design intent:** Brain matters as much as combat. Investigation-focused players have a complete experience.

### 1.4 Principle 4: Social Gestures Have Weight

Small actions matter. Lifting a hat before an elder. Looking down when a widow passes. Paying respect at a funeral. Sitting properly in a church. Dark Arisen rewards cultural respect and punishes boorishness.

**Design intent:** The game teaches players about 1876 pirate-era social norms through mechanical reinforcement.

### 1.5 Principle 5: Every Object Tells a Story

Lootable items have context. A letter found in a dead pirate's pocket tells his story. A tin of polish suggests someone cared about their boots. A child's drawing in an abandoned house breaks your heart. The world is full of implied narratives.

**Design intent:** Environmental storytelling is not optional — it's core to Dark Arisen's atmosphere.

---

## 2. CORE INTERACTION MECHANICS

### 2.1 The Triangle Button System

Triangle is the universal interaction button. It changes meaning based on context:

**Near NPC:** Talk
**Near Object:** Interact (pickup, examine, use)
**Near Door:** Open/close
**Near Container:** Loot
**Near Evidence:** Examine/investigate
**Near Ally in need:** Help/revive

### 2.2 Interaction Range

Jake must be within specific distance to interact:

| Interaction Type | Range | Notes |
|-----------------|-------|-------|
| Talk to NPC | 2.5m | Conversational distance |
| Pickup object | 1.2m | Arm's reach |
| Open door | 1.5m | Doorframe approach |
| Examine clue | 1m | Close inspection |
| Loot container | 1.5m | Within reach |
| Revive ally | 2m | Immediate aid |

### 2.3 Interaction Prompts

Clear visual prompts indicate available interactions:

**Standard Prompt:**
- Button icon (Triangle)
- Action verb (Talk, Open, Pick up)
- Target name (if known)

**Held Prompt (Triangle hold):**
- For actions requiring commitment
- Shows hold duration bar
- Cancel available (release early)

**Multiple Options:**
- When multiple actions possible
- Shows secondary option with different button
- Contextual menu for complex interactions

### 2.4 Interaction Priority

When multiple interactables are near:

**Priority rules:**
1. Currently being attacked (combat priority)
2. Immediate danger (ally down, fire, etc.)
3. Closest interactable
4. Most relevant (quest-tagged)
5. Most specific (named objects over generic)

---

## 3. THE DIALOGUE SYSTEM

### 3.1 Dialogue Initialization

**Approach Behavior:**
- NPC notices Jake at 5m range
- Reacts appropriately (greeting, wariness, fear depending on relationship)
- Eye contact + body language
- Initial dialogue triggered by Triangle

**Environmental Awareness:**
- NPCs continue their tasks during conversation
- Background activity realistic
- Interruptions possible (from combat, weather, quest events)

### 3.2 Dialogue Choices

**Choice Display:**
- 2-6 options typical
- Choice previews (first few words of what Jake will say)
- Tone indicators (calm, aggressive, diplomatic, etc.)
- Consequence hints (subtle, only for major choices)

**Types of Choices:**

**Directive Choices:**
- Ask specific question
- Direct request
- Clear intent

**Tonal Choices:**
- Same information, different approach
- Friendly, neutral, hostile
- Formal, casual, intimate

**Moral Choices:**
- Ethical decisions
- Significant consequences
- Long-term impact

**Information Choices:**
- Reveal specific info to NPC
- Withhold specific info
- Trade information

### 3.3 Dialogue Flow

**Linear Conversations:**
- Simple exchanges
- Pre-scripted
- Common NPCs

**Branching Conversations:**
- Major choices affect outcome
- Significant story elements
- Memorable moments

**Dynamic Conversations:**
- Responsive to Jake's reputation
- Based on current story state
- May include callbacks to previous encounters

### 3.4 Dialogue Registers

All dialogue follows the Style Bible's staggered register:

**High Register:**
- Ancient/mystical beings (Havfrue Modor, Ashen Wyrm)
- Philosophical moments (priests at rituals)
- Elevated formal speech
- Some specific characters (Silvano's doctrine)

**Middle Register:**
- Professional characters (Fitzmueller, Corazón)
- Military/naval dialogue
- Cultured but practical speech
- Most named NPCs

**Low Register:**
- Common pirates and sailors
- Street characters
- Casual conversation
- Background dialogue

**Mixed Register:**
- Emotional moments shift registers
- Formal characters break into Low when desperate
- Common folk rise to High during ceremony

### 3.5 Non-Verbal Communication

**Body Language:**
- NPCs express through posture
- Jake can observe intent
- Important for stealth and social situations

**Facial Expressions:**
- Detailed face rigs for named characters
- Emotional responses visible
- Lying/truth detection available to attentive players

**Voice Tone:**
- Beyond words, audio conveys emotion
- Voice acting crucial for immersion
- Subtitles indicate tone when audio unclear

### 3.6 Specific Dialogue Types

**Greetings:**
- Neutral: "Captain" / "Sir"
- Friendly: "Jake!" / "Harlow!"
- Respectful: "Captain Harlow"
- Hostile: Cold silence or threats
- Based on relationship status

**Farewells:**
- Match greeting register
- Convey relationship state
- Some are final (character dying, moving away)

**Small Talk:**
- Weather, current events
- Helps world feel alive
- Occasionally reveals clues

**Requests:**
- NPCs ask Jake for help
- Side quests generated
- Moral weight possible

**Warnings:**
- NPCs warn Jake of dangers
- Important intel
- Save lives sometimes

---

## 4. NPC MEMORY AND RECOGNITION

### 4.1 The NPC Memory System

Each named NPC has a memory database tracking:

**Encounters with Jake:**
- Date/location of first meeting
- All subsequent interactions
- Specific dialogue choices
- Actions Jake took

**Reputation Changes:**
- Helpful actions remembered
- Harmful actions remembered
- Neutral actions stored
- Overall impression formed

**Shared Experiences:**
- Quests completed together
- Combat shared
- Significant conversations
- Crisis moments

### 4.2 Recognition Types

**Visual Recognition:**
- NPC sees Jake from distance
- Immediate response based on relationship
- Different greeting depending on status
- Some NPCs actively seek Jake out

**Audio Recognition:**
- Jake's name mentioned in NPC conversations
- Rumors spread through population
- Reputation travels ahead of Jake

**Reputation Recognition:**
- Even NPCs Jake hasn't met may know him
- Stories precede him in regions
- Fame or infamy shape initial responses

### 4.3 Cross-Regional Memory

NPCs remember even across regions:

**Example:**
- Chapter 3: Jake helps a Moran orphan named Nathan
- Chapter 7: Nathan appears in Fjordlund as merchant, recognizes Jake
- Chapter 9: Nathan has become a captain himself, proudly greets Jake

**Example (Negative):**
- Chapter 4: Jake insults a Rexan widow publicly
- Chapter 6: Her cousin in Ashenmoor knows, treats Jake coldly
- Chapter 10: The reputation haunts Jake in epilogue

### 4.4 Memory Decay

NPCs don't remember everything forever:

**Strong Memory (Persists):**
- Major story events
- Direct harm or help
- Named quest involvement
- Public moments

**Fades Over Time:**
- Minor interactions
- Casual comments
- Small courtesies or slights

**Reputation Replaces Detail:**
- Eventually, Jake becomes a reputation
- Specific memories blur into general feeling
- "The one who saved the village" > specific dialogue

### 4.5 Group Memory

Certain groups share knowledge:

**Crew Members:**
- Know each other's interactions with Jake
- Discuss Jake when he's not present
- Collective feelings influence individual behaviors

**Village Communities:**
- Gossip spreads quickly
- Jake's actions affect entire village
- Collective response possible

**Factions:**
- Crimson Armada remembers Jake's actions
- Imperial forces track him
- Fjordlund assess Jake as potential ally

---

## 5. INVESTIGATION SYSTEM

### 5.1 Investigation Mode

**Activation:**
- Triangle button hold on crime scene or clue
- Jake enters "careful examination" mode
- Reduced ambient audio, focused visuals

**Capabilities:**
- Inspect objects in detail
- Read small text/clues
- Identify patterns
- Connect disparate evidence

### 5.2 Clue Types

**Physical Clues:**
- Bloodstains
- Footprints
- Discarded items
- Broken furniture
- Burn marks

**Documentary Clues:**
- Letters
- Ledgers
- Maps
- Receipts
- Personal notes

**Social Clues:**
- NPC dialogue (direct statements)
- Rumors (indirect information)
- Body language (non-verbal cues)
- Reactions (truth vs. lie indicators)

**Environmental Clues:**
- Missing items
- Time-of-day evidence
- Weather-related effects
- Disturbed layouts

### 5.3 Clue Combination

Jake's journal tracks clues:

**Collection:**
- Automatic entry when clue found
- Organized by case/quest
- Timeline tracking

**Analysis:**
- Manually combine clues
- Form hypotheses
- Test theories through action

**Resolution:**
- Complete investigation
- Accuse specific person
- Reveal truth
- Consequences based on accuracy

### 5.4 Investigation Quest Examples

**Example 1: The Missing Apprentice (Chapter 4 side quest)**
- Apprentice hasn't come home
- Jake investigates his workshop
- Finds letters revealing secret romance
- Interviews shopkeeper (evasive)
- Discovers apprentice ran away with lover
- Must decide: Tell family truth, tell noble lie, or help them escape

**Example 2: The Poisoned Well (Chapter 6)**
- Village children getting sick
- Jake inspects well (traces of substance)
- Interviews villagers (conflicting stories)
- Follows trail to abandoned shack
- Finds poisoner (local man with grudge)
- Decides: Justice, mercy, or understanding

### 5.5 Failed Investigations

Not every investigation succeeds:

**Incomplete Evidence:**
- Not enough clues gathered
- Missing key information
- NPC refuses to talk
- Physical evidence destroyed

**Wrong Conclusion:**
- Accusing wrong person
- Missing true perpetrator
- Consequences: injured innocent

**No Investigation:**
- Jake can skip investigations
- Consequences: problems unsolved
- Some quests have time limits

---

## 6. LOOTING SYSTEM

### 6.1 Loot Containers

**Corpses (enemies):**
- Loot what they carried
- Weapons, ammo, small items
- Letters or notes revealing story
- Moral weight of looting

**Barrels and Crates:**
- Standard containers
- Usually food, supplies
- Sometimes hidden treasures
- Common throughout world

**Chests and Strongboxes:**
- Higher-value contents
- Often locked
- Require picking or keys
- Major loot rewards

**Hidden Caches:**
- Specific locations (behind loose stones, under floorboards)
- Require investigation to find
- Premium rewards
- Story significance

### 6.2 Looting Mechanics

**Basic Loot:**
- Triangle button to interact
- Contents displayed in menu
- Take All / Select Items
- Weight considered

**Stealth Looting:**
- Silent if Jake crouched
- NPCs don't notice if unaware
- Important for stealth missions

**Loud Looting:**
- Rummaging creates noise
- Can alert nearby enemies
- Quick vs. thorough choice

### 6.3 Lock Picking

**Lock Types:**

**Simple Lock:**
- Requires basic lockpick
- 30 second process
- Minor skill check
- Common locks

**Complex Lock:**
- Requires skilled lockpick
- 60-90 second process
- Significant skill check
- Valuable containers

**Master Lock:**
- Requires master lockpick
- 2-4 minute process
- High skill challenge
- Vault-level security

**Lockpicking Mini-Game:**
- Precision-based (tension + rake)
- Audio feedback important
- Haptic feedback on PS5
- Silent when done well, noisy when fumbled

### 6.4 Loot Tables

**Predictable Loot:**
- Specific enemies carry expected items
- Tradesmen have related materials
- Imperial soldiers have imperial gear

**Randomized Loot:**
- Quality varies within ranges
- Chance of rare items
- Influenced by region and context

**Story Loot:**
- Specific items from specific enemies
- Quest rewards predetermined
- Boss drops scripted

### 6.5 Hidden Caches

**Discovery Methods:**
- Environmental investigation
- NPC dialogue hints
- Map fragments
- Treasure hunts

**Cache Types:**
- Pirate treasures (buried)
- Imperial stashes (hidden)
- Smuggler caches (obscured)
- Personal hideaways (family heirlooms)

---

## 7. ENVIRONMENTAL INTERACTION

### 7.1 Objects in the World

**Categories:**

**Decorative (Background):**
- Visual only, adds atmosphere
- Cannot be interacted with
- Sets scene dressing

**Interactable (Flavor):**
- Can be examined for flavor text
- No gameplay impact
- Environmental storytelling

**Usable (Functional):**
- Actually used (doors, chairs, etc.)
- Gameplay impact
- Part of world systems

**Destructible (Dynamic):**
- Can be broken
- May contain items
- Physical interaction

### 7.2 Doors and Passages

**Door Types:**

**Standard Doors:**
- Open/close with Triangle
- No lock
- Sound varies (wooden creak, metal clang)

**Locked Doors:**
- Require key or picking
- Block specific areas
- Part of quest structure

**Sealed Doors:**
- Cannot be opened directly
- Require specific condition
- Story-gated

**Secret Passages:**
- Hidden behind objects
- Require investigation
- Reveal hidden areas

### 7.3 Furniture and Fixtures

**Sitting:**
- Jake can sit in most chairs/benches
- Camera settles, ambient view
- Useful for resting, observing

**Beds:**
- Sleep to pass time
- Save game option
- Skip to next time of day

**Tables:**
- Can place items on
- Workspace for certain crafts
- Pickup objects

**Fireplaces:**
- Provide warmth (Fjordlund)
- Cooking station
- Illuminate area

### 7.4 Windows and Views

**Window Interactions:**
- Look out (observation)
- Open/close
- Climb through (context-dependent)
- Hide from enemies

**Spyglass Use:**
- At tall points (towers, crow's nest)
- Observe distant areas
- Identify landmarks
- Track quest targets

### 7.5 Environmental Puzzles

**Types:**

**Switch/Lever Puzzles:**
- Activate mechanisms
- Open paths
- Common in ancient ruins

**Pressure Plates:**
- Weight-activated
- Require specific placement
- Often involve objects

**Sequence Puzzles:**
- Activate in correct order
- Logic-based
- Story-integrated

**Environmental Solutions:**
- Use world elements creatively
- Non-obvious approaches
- Investigation-rewarded

---

## 8. SOCIAL INTERACTION

### 8.1 Greetings and Salutations

**Hat Tipping (Classical gesture):**
- Button: D-Pad Up
- Context: Approaching elder, woman, dignitary
- Effect: Small reputation boost
- Cultural significance high

**Nod:**
- Button: D-Pad Up (quick)
- Context: Passing acquaintance
- Effect: Neutral acknowledgment
- Less formal than hat tip

**Bow:**
- Button: Context-sensitive
- Context: Formal situations, royalty, extreme respect
- Effect: Major respect gesture
- Rare but impactful

**Salute:**
- Button: Military context
- Context: Crew member, naval officer
- Effect: Professional acknowledgment
- Navy/military cultures

### 8.2 Physical Courtesy

**Handshake:**
- Extended on meeting
- Agreement ratification
- Relationship building

**Embrace:**
- Reunion with allies
- Farewell with close friends
- Emotional moments

**Pat on Shoulder:**
- Comforting gesture
- Crew members
- Showing respect

**Kiss (Cheek):**
- Close family/lovers
- Cultural variations (Rexan common, Imperial rare)
- Highly relationship-specific

### 8.3 Funeral Etiquette

**At Funerals:**
- Hat removed automatically
- Subdued speech only
- No combat allowed
- Respect gestures track reputation

**During Burials:**
- Jake can help (offered)
- Memorial services respectful
- Refusal of refreshments

**Graveside Visits:**
- Can leave flowers
- Silent prayer option
- Emotional beat for Jake

### 8.4 Religious Spaces

**Churches/Temples:**
- Remove hat entering
- Speak quietly
- No weapon drawn
- Respect sacred objects

**Rituals:**
- Can observe or participate
- Specific protocols per religion
- Missteps noticed

**Clergy Interaction:**
- Formal address (Father, Mother, Reverend)
- Specific etiquette
- Confession opportunities (Jake can unburden)

### 8.5 Taverns and Gatherings

**Entering Taverns:**
- Greet bartender
- Acknowledge patrons
- Find seat naturally

**Drinking Etiquette:**
- Paying for own drinks
- Offering drinks to others
- Buying rounds (social bonding)

**Gambling/Games:**
- Card games available
- Dice games
- Wagering systems

**Fighting Prohibited:**
- Bar fights possible but consequence
- Thrown out
- Reputation damage

### 8.6 Formal Events

**Weddings:**
- Dress appropriately
- Gift bringing
- Dance participation
- Toast etiquette

**Balls/Parties:**
- Dress codes
- Dance conventions
- Conversation rules
- Political navigation

**Official Ceremonies:**
- Proper attire
- Standing protocols
- Speech etiquette

### 8.7 Market Etiquette

**Greeting Merchants:**
- Proper address
- Patience while they work
- Haggling respect

**Haggling:**
- Acceptable in most cultures
- Offensive in some (imperial high society)
- Relationship affects success

**Tipping:**
- Common practice
- Improves service quality
- Social grease

---

## 9. GIFTING SYSTEM

### 9.1 Gift Mechanics

**Offering Gifts:**
- Triangle to offer item to NPC
- Appropriate context required
- NPC can accept or refuse

**Gift Reception:**
- Appreciated: Relationship improves
- Neutral: No change
- Inappropriate: Relationship damaged
- Valuable: Significant improvement

### 9.2 NPC Preferences

**Each major NPC has preferences:**

**Ines:**
- Appreciates: Rare herbs, alchemy books
- Neutral: General items
- Dislikes: Overly expensive gifts (feels awkward)

**Big Tom:**
- Appreciates: Quality tobacco, rum, tool improvements
- Neutral: Food
- Dislikes: Frivolous items

**Father Salvio:**
- Appreciates: Religious texts, silver items, peaceful tokens
- Neutral: Most items
- Dislikes: Weapons

**Mormor Astrid:**
- Appreciates: Fjordlund items, tea, stories
- Neutral: Gold, jewelry
- Dislikes: Imperial items

### 9.3 Gift Occasions

**Birthdays:**
- Specific dates for named NPCs
- Remember and gift matters
- Significant relationship boost

**Holidays:**
- Regional celebrations
- Cultural gift-giving
- Community bonding

**Achievement Celebrations:**
- Crew member promotions
- Personal victories
- Shared accomplishments

### 9.4 Gift Consequences

**Thoughtful Gifts:**
- +20% relationship gain
- Unlock special dialogue
- May trigger personal quests

**Generic Gifts:**
- +5% relationship
- Standard appreciation
- No special effects

**Inappropriate Gifts:**
- -10% relationship
- Confusion or offense
- May require explanation

---

## 10. READING SYSTEM

### 10.1 Readable Materials

**Letters:**
- Personal correspondence
- Quest-relevant information
- Character backstory

**Books:**
- Full texts to read
- Historical, fiction, technical
- Some teach recipes or skills

**Newspapers:**
- Imperial news (Chapter 6+)
- Regional publications
- Event tracking

**Notes and Maps:**
- Quest markers
- Treasure guidance
- Character clues

**Signs and Notices:**
- Public announcements
- Business signs
- Warnings and proclamations

### 10.2 Reading Mechanics

**Pickup:**
- Triangle to collect
- Added to inventory
- Readable any time

**Active Reading:**
- Open from inventory
- Full text display
- Illustrations for some
- Can be re-read

**Lore Objects:**
- 150+ specific lore items (#001-#150)
- Each tells piece of world history
- Rewarded for collection

### 10.3 Content Variety

**Official Documents:**
- Formal language
- Informational
- Often long

**Personal Letters:**
- Informal tone
- Emotional content
- Character revelation

**Fiction/Poetry:**
- Entertainment value
- Cultural insight
- Sometimes coded messages

**Technical Texts:**
- Recipes
- Combat manuals
- Crafting instructions

### 10.4 The Journal System

**Jake's Journal:**
- Auto-tracked personal diary
- Entries on significant events
- Sketches of important locations/people
- Reflections on choices

**Journal Access:**
- Touchpad button (PS5) / M (PC)
- Always accessible
- Organized chronologically and thematically

**Content:**
- Chapter summaries
- Character introductions
- Personal reflections
- Recipe collection
- Map annotations

---

## 11. OBSERVATION AND PERCEPTION

### 11.1 Jake's Perception

**Environmental Awareness:**
- Jake notices unusual details
- Can point things out to player
- Story triggers from observation

**Social Perception:**
- Reading NPC emotions
- Detecting lies (with skill)
- Understanding context

**Tactical Perception:**
- Spotting ambushes
- Noting escape routes
- Identifying threats

### 11.2 Focus Mode

**Activation:** Hold R3 (stealth mode)
- Highlights important elements
- Enhances hearing
- Slows passage of time

**Effects:**
- Enemies outlined (Chapter 5+)
- Important objects glow
- NPC status visible
- Hidden details revealed

### 11.3 Passive Observation

**Background Details:**
- Environmental storytelling
- Ambient conversations
- Visual narratives

**Active Listening:**
- Standing still near NPCs
- Overheard conversations
- Market rumors
- Tavern talk

### 11.4 Clue-Highlighting System

In investigation mode:
- **Yellow highlight:** Standard clues
- **Orange highlight:** Important clues
- **Red highlight:** Critical evidence
- **Blue highlight:** Contextual information

Accessibility:
- Colors can be adjusted
- Text labels optional
- Audio cues for clues

---

## 12. CONTEXT-SPECIFIC INTERACTIONS

### 12.1 Combat Interactions

**During Combat:**
- Most interactions disabled
- Emergency actions available
- Environmental weapon use
- Ally revival

### 12.2 Ship Context

**On Ship Deck:**
- Crew interaction
- Station operation
- Maintenance tasks

**In Cabin:**
- Menu access
- Personal interactions
- Planning conversations

### 12.3 Stealth Context

**While Crouched:**
- Silent interactions
- Priority to silent options
- Interrogation possible

### 12.4 Dialogue Context

**During Conversation:**
- Other interactions paused
- Focus on dialogue partner
- Environmental awareness reduced

### 12.5 Cutscene Context

**During Cinematic:**
- Limited interactions
- Dialogue choices only
- Some scripted actions

---

## 13. NPC RELATIONSHIP TRACKING

### 13.1 Relationship Meters

Major NPCs have relationship values:

**Scale: -100 to +100**
- -100 to -60: Hostile
- -59 to -20: Unfriendly
- -19 to +19: Neutral
- +20 to +59: Friendly
- +60 to +100: Loyal/Devoted

### 13.2 Relationship Effects

**Hostile:**
- May attack on sight
- Refuse all dialogue
- Spread negative rumors

**Unfriendly:**
- Cold dialogue only
- Won't help
- May lie or obstruct

**Neutral:**
- Standard interactions
- Fair dealings
- No special privileges

**Friendly:**
- Warm greetings
- Small favors
- Share information

**Loyal:**
- Deep conversations
- Major help available
- Sacrificial support possible

### 13.3 Building Relationships

**Positive Actions:**
- Help completed
- Thoughtful gifts
- Respectful behavior
- Shared experiences

**Negative Actions:**
- Insults
- Refusal to help
- Harmful actions
- Broken promises

### 13.4 Named vs. Generic NPCs

**Named NPCs:**
- Full relationship tracking
- Individual memory
- Unique dialogue trees
- Personal quest chains

**Generic NPCs:**
- Group relationships
- Regional attitudes
- Simplified tracking
- Typical dialogue

---

## 14. INTERACTION UI

### 14.1 Contextual Prompts

**Display:**
- Bottom-center screen
- Button icon + verb
- Fade in/out based on proximity
- Non-intrusive

**Priority System:**
- Most important action shown
- Alternatives available via menu
- Clear and immediate

### 14.2 Dialogue UI

**Layout:**
- Speaker's face portrait
- Dialogue text
- Choice options
- Response previews

**Tone Indicators:**
- Icons showing dialogue tone
- Colors for different approaches
- Hints at consequences

### 14.3 Investigation UI

**Clue Highlighting:**
- Subtle glow on examinable objects
- Icon indicators
- Connection lines (when theory formed)

**Journal Integration:**
- Clues auto-added
- Case progress visible
- Theory construction tools

### 14.4 Relationship UI

**Status Indicators:**
- Small badge near NPC
- Relationship level visible
- Recent changes highlighted

**Dialogue Effects:**
- Button press ripple to character
- Immediate feedback
- Long-term visible on subsequent meetings

---

## 15. PLATFORM FEATURES

### 15.1 PS5 DualSense

**Haptic for Interactions:**
- Doorknob turn sensation
- Book page texture
- Letter paper feel
- Handshake firm pulse

**Adaptive Triggers:**
- Door resistance (heavy doors)
- Container opening (chest latch)
- Book binding snap

**Audio:**
- 3D audio for NPCs
- Spatial dialogue
- Controller speaker for whispers

### 15.2 Xbox

**Similar features:**
- Controller rumble for interactions
- Trigger resistance where supported

### 15.3 PC

**Keyboard/Mouse:**
- F or E for interact (remappable)
- Hold for extended interactions
- Menu-based alternatives

---

## 16. ACCESSIBILITY

### 16.1 Motor Accessibility

**Simplified Interactions:**
- Auto-interact on approach
- Reduced button combinations
- Hold alternatives available

### 16.2 Visual Accessibility

**Interaction Clarity:**
- Large interaction icons
- High contrast prompts
- Descriptive text available

### 16.3 Audio Accessibility

**Subtitle Customization:**
- Speaker identification
- Tone indicators
- Adjustable size/contrast

### 16.4 Cognitive Accessibility

**Dialogue Assistance:**
- Slowed pacing option
- Repeatable exchanges
- Clear choice previews

### 16.5 Reading Accessibility

**Text Options:**
- Font choice
- Size scaling
- Background contrast
- Audio narration option

---

## 17. TUTORIAL PROGRESSION

### 17.1 Chapter 1

**Basic Interactions:**
- Talk to NPCs
- Pick up items
- Open doors

### 17.2 Chapter 2

**Expanded:**
- Investigation basics
- Gift giving introduction
- Relationship awareness

### 17.3 Chapter 4

**Deep Systems:**
- Complex dialogue trees
- Investigation mode
- Social etiquette emphasis

### 17.4 Chapter 6

**Advanced:**
- Cross-regional memory
- Complex investigations
- Cultural nuance

### 17.5 Chapter 8+

**Mastery:**
- Full systems active
- Consequences visible
- World feels responsive

---

## 18. INTERACTION FEEL TESTS

Before the interaction system is final, it must pass these tests:

### 18.1 The RDR2 Test
> *"Does the world feel lived in? Do NPCs feel like real people rather than quest dispensers?"*

### 18.2 The Witcher 3 Test
> *"Do dialogue choices matter? Do investigations reward careful thinking?"*

### 18.3 The Cultural Authenticity Test
> *"Does the game respect the cultural conventions of its 1876 pirate setting? Do social gestures feel meaningful?"*

### 18.4 The Reactive World Test
> *"Does the world remember Jake? Do NPCs respond to his history?"*

### 18.5 The Environmental Storytelling Test
> *"Do objects tell stories? Does investigation reveal depth?"*

All five must pass. Then interaction is ready.

---

## 19. IMPLEMENTATION NOTES

### 19.1 Technical Requirements
- Efficient NPC memory database
- Reputation calculation engine
- Dialogue branching system
- Investigation clue tracking
- Animation variety for gestures

### 19.2 Content Requirements
- Thousands of lines of dialogue
- Dozens of investigation quests
- Hundreds of lootable objects
- Rich environmental storytelling

### 19.3 AI Requirements
- NPC behavior scripts
- Memory persistence
- Reputation-based responses
- Realistic schedules

### 19.4 Polish Requirements
- Smooth animation transitions
- Voice acting quality
- Facial capture detail
- Cultural authenticity

---

## 20. CONSISTENCY WITH EXISTING DESIGN

All existing documentation remains compatible:

- **All characters:** Have relationship systems
- **Boss encounters:** Include dialogue and interaction
- **Regions:** Each has cultural specifics
- **Quests:** Build on interaction systems

No existing documents need revision.

---

## 21. NEXT STEPS

Next document: **`mechanics/ship_interior.md`** — Ship Interior System

Will cover:
- La Liberación as mobile base
- Cabin layout and navigation
- Crew quarters and dynamics
- Below deck areas
- Navigation room
- Workshop spaces
- Personal items placement
- Ship-specific activities

Estimated time: 2-3 hours.

After Ship Interior: Ship Management (final subsystem).

**Interaction is where Jake meets the world, and the world meets Jake. This document defines every dialogue, every gesture, every investigation. Phase 3 Subsystem 8 of 10 — complete.**
