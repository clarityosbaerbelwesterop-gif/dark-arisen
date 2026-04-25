# NPC SYSTEM CORE — MECHANICAL FOUNDATION

**Status:** Design Draft v1.0  
**Phase:** Phase 5 — NPCs (Subsystem 1 of 22 — Foundation Tier)  
**Tier:** Foundation Tier  
**Related:** npcs_overview.md, mechanics/interaction.md, style_bible.md  
**Purpose:** Define the complete mechanical foundation for all NPCs in Dark Arisen — memory tracking, schedules, reputation per NPC, behavior matrices, and cross-NPC communication. The technical backbone of the living population.  
**Inspirations:** Red Dead Redemption 2 (NPC schedules, reputation systems), Skyrim Radiant AI (behavior matrices), The Witcher 3 (NPC memory), Disco Elysium (deep tracking), real-world social network theory

---

## 1. CORE PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Specificity Over Statistics

NPCs aren't reputation numbers. They're individuals with memories. The shopkeeper doesn't have "+5 reputation with Jake" — she has "remembers Jake helped find her cat in Chapter 3, sometimes mentions it." Specificity creates emotional resonance.

**Design intent:** Every interaction creates specific memory traces, not just numerical adjustment.

### 1.2 Principle 2: Schedules Create Authenticity

NPCs aren't quest stations. They have lives — places to be, things to do, people to see. The blacksmith works mornings, eats lunch with his wife, drinks at the tavern evenings. Finding him requires understanding his schedule.

**Design intent:** Authentic life patterns make NPCs feel real.

### 1.3 Principle 3: Behavior Emerges From Context

NPC reactions depend on multiple factors: relationship with Jake, current situation, who's watching, time of day, recent events, NPC's own emotional state. Same NPC reacts differently in different contexts.

**Design intent:** Realistic complexity over scripted responses.

### 1.4 Principle 4: Information Flows

When Jake does something memorable, NPCs talk about it. Information spreads through social networks — friends tell friends, family tells family, gossip moves faster than truth. NPCs Jake hasn't met may already have opinions.

**Design intent:** Reputation is networked, not isolated.

### 1.5 Principle 5: NPCs Are Not Players

NPCs don't optimize. They don't always do what's logical. They have biases, prejudices, emotions, mistakes. They forget things. They misremember. They're influenced by mood. Realistic = imperfect.

**Design intent:** Avoid the "NPCs as game systems" trap. They're people.

---

## 2. THE MEMORY SYSTEM ARCHITECTURE

### 2.1 Memory Categories

Each named NPC has memory storage organized in categories:

**Direct Encounters:**
- Every time NPC and Jake interacted
- Date, location, context
- Specific dialogue choices
- Actions taken (give, take, ignore)
- Outcomes

**Witnessed Events:**
- Things NPC saw Jake do
- Even without direct interaction
- Major events spread widely
- Minor events local only

**Reported Events:**
- Things others told NPC about Jake
- Less reliable than witnessed
- Can be exaggerated or distorted
- Source matters (trusted vs untrusted)

**Cultural/Reputation:**
- General sense of Jake
- Built from rumor + direct experience
- Forms baseline opinion
- Modifies all interactions

### 2.2 Memory Weight System

**Memory Importance Scoring:**

**Trivial (Weight 1):**
- Brief greeting
- Standard transaction
- Background presence
- Forgotten in days

**Notable (Weight 5):**
- Specific dialogue
- Small kindness/insult
- Memorable interaction
- Forgotten in months

**Significant (Weight 20):**
- Major help/harm
- Important conversation
- Specific quest completion
- Remembered for years

**Defining (Weight 50):**
- Life-altering event
- Crisis moment
- Death of loved ones (caused by Jake)
- Salvation moment
- Permanent

**Mythic (Weight 100):**
- Story-shaping events
- Public dramatic moments
- Legend-creating actions
- Becomes part of NPC identity

### 2.3 Memory Decay

**Decay Formula:**
- Memory weight decreases over in-game time
- Trivial fades fastest
- Mythic never fades
- Specific to event type

**Memory Triggers:**
- Encountering location reminds NPC
- Mention of related event refreshes
- Anniversary effects (some events)
- Other NPCs mention Jake

**Replacement Memory:**
- Specific memories blur into general impression
- "Helped my brother once" becomes "trustworthy person"
- Detail lost, feeling preserved

### 2.4 Memory Database Structure

**Per-NPC Storage:**
NPC_ID: [unique identifier]
Memories: [
{
timestamp: when occurred
type: encounter/witnessed/reported
content: specific event
weight: importance
emotional_color: positive/negative/neutral
decay_rate: how fast it fades
trigger_keywords: what brings it up
}
]
Reputation_Score: calculated from memories
Specific_Trust_Level: -100 to +100
Behavioral_Modifiers: [list of memory effects]

### 2.5 Memory Examples

**Don Alejandro Memory Slot:**
First Meeting (Chapter 4): Polite, formal introduction. Weight 5.
	•	Cigar Quest Acceptance (Chapter 4): Significant moment. Weight 20.
	•	First Cigar Returned (Chapter 5): Quality work. Weight 15.
	•	Conversation about Lost Son (Chapter 6): Defining intimacy. Weight 50.
	•	Final Cigar Brought (Chapter 9): MYTHIC moment. Weight 100.
	•	The Smoke Together (Chapter 9): Permanent bond. Weight 100.

  
**Bitter Widow Memory Slot:**
Husband’s Death (Chapter 1): Defining tragedy (not Jake’s fault). Weight 50.
	•	First Encounter with Jake (Chapter 2): Hostile. Weight 5.
	•	Jake Helps with Roof (Chapter 3): Surprised gratitude. Weight 15.
	•	Jake Defends Village (Chapter 5): Significant respect. Weight 30.
	•	Jake’s Reputation Reaches Her (Chapter 7): General respect. Cultural memory.
	•	Encounter Years Later (Chapter 9): Tells children about Jake. Mythic now. Weight 100.
  
---

## 3. THE SCHEDULE SYSTEM

### 3.1 Daily Schedule Templates

**Standard NPC Day:**

**Morning (5-12):**
- Wake (specific time per NPC)
- Personal routine (eating, washing)
- Travel to work
- Working hours
- Mid-morning break sometimes

**Midday (12-15):**
- Lunch (often with family)
- Rest period
- Continued work or social

**Afternoon (15-18):**
- Continued work
- Errands
- Social visits

**Evening (18-22):**
- Return home
- Dinner
- Social time (tavern, family, friends)
- Personal pursuits

**Night (22-5):**
- Sleep
- Some specific NPCs awake (guards, sailors)
- Quiet city

### 3.2 Job-Specific Schedules

**Blacksmith Example:**
- 5:30: Wake, light forge
- 6:00: Begin work (forge needs heat)
- 8:00: Customer transactions begin
- 12:00: Lunch with family
- 13:00: Resume work
- 17:00: Major work ends
- 18:00: Tavern visit usually
- 21:00: Home, family time
- 23:00: Sleep

**Tavern Keeper Example:**
- 8:00: Wake (later than most)
- 9:00: Open for breakfast
- 10:00-15:00: Slow service, prep work
- 15:00-18:00: Afternoon crowd
- 18:00-23:00: Peak business
- 24:00: Last call
- 1:00: Cleanup
- 2:00: Sleep

**Fisherman Example:**
- 4:00: Wake (very early)
- 4:30: Boat ready
- 5:00-12:00: Fishing
- 13:00: Return, sell catch
- 14:00-16:00: Maintenance
- 17:00: Family time
- 19:00: Tavern (sometimes)
- 21:00: Sleep (early)

### 3.3 Weekly Patterns

**Market Days:**
- Specific days of week
- Sellers appear
- More NPCs in town
- Different schedule

**Religious Days:**
- Father Salvio holds services
- Devout NPCs attend
- Some shops closed
- Cultural variation

**Rest Days:**
- Specific cultural pattern
- Some regions different
- Limited services
- Family time

### 3.4 Seasonal Schedules

**Winter Adjustments:**
- Earlier nights
- Late starts
- Indoor activities
- Fjordlund especially

**Summer Adjustments:**
- Long days exploited
- Outdoor work focus
- Fishing extended
- Different patterns

**Harvest Time:**
- Massive labor needs
- Schedules disrupted
- Community focus
- All hands needed

### 3.5 Special Event Disruptions

**Festivals:**
- Schedules suspend
- Community gathering
- Different opportunities
- Some quests only available

**Storms:**
- Outdoor work stops
- Indoor crowding
- Specific NPC conversations
- Weather-locked content

**Personal Events:**
- Wedding (community involved)
- Funeral (mourning period)
- Birth (celebration)
- Disease (quarantine possible)

### 3.6 Schedule Tracking

**System Implementation:**
- Each NPC has 24-hour schedule
- Adjusts for day of week, season
- Special events override
- Player can learn patterns

**Visible to Player:**
- NPCs visible at expected locations
- Patterns observable
- Predictable but not robotic
- Some randomization

---

## 4. REPUTATION SYSTEM PER NPC

### 4.1 The Personal Reputation Score

**Range: -100 to +100**

**Hostile (-100 to -60):**
- Won't speak normally
- May attack on sight
- Spreads negative rumors
- Cannot be redeemed easily

**Distrustful (-60 to -20):**
- Cold interactions only
- Won't help
- Suspicious of motives
- Difficult to repair

**Neutral (-20 to +20):**
- Standard interaction
- Business-only
- Default state
- Most strangers

**Friendly (+20 to +60):**
- Warm greetings
- Help offered freely
- Information shared
- Builds toward loyalty

**Loyal (+60 to +100):**
- Sacrificial support possible
- Personal investment in Jake
- Family-tier connection
- Hard to lose

### 4.2 Reputation Modifiers

**Positive Actions:**
- Help with task: +5 to +20
- Save from danger: +30 to +50
- Solve major problem: +50
- Personal kindness: +5 to +15
- Gift accepted: +5 to +10
- Defend honor: +20 to +40
- Save loved one: +50 to +100

**Negative Actions:**
- Insult: -5 to -15
- Refuse help: -10 to -20
- Cheat in transaction: -20 to -40
- Threaten: -30 to -50
- Harm loved one: -50 to -100
- Kill loved one: -100 (often permanent)

**Neutral Actions:**
- Business transaction: 0
- Standard greeting: 0
- Brief encounter: 0
- Pass without acknowledging: -1 to -2 (over time)

### 4.3 Reputation Decay

**Time Effects:**
- Recent events weighted more
- Old grudges fade slowly
- Old friendships fade slowly
- Without contact, drift toward neutral

**Decay Rates:**
- Hostile: Slowest decay (deep wounds)
- Distrustful: Slow decay
- Neutral: No decay (already there)
- Friendly: Slow decay
- Loyal: Slowest decay (deepest bonds)

### 4.4 Reputation Visualization

**Player Information:**
- Subtle visual cues (NPC body language)
- Greeting variations
- Specific dialogue indicators
- Journal entries note relationships

**No Hard Numbers:**
- Never display "+5 reputation"
- Players read context
- Realistic ambiguity
- Uncertainty creates engagement

---

## 5. BEHAVIOR MATRICES

### 5.1 The Decision Tree

**When NPC encounters Jake:**

**Layer 1: Immediate Recognition**
- Has seen Jake before? (memory)
- Reputation level?
- Current emotional state?
- Time/context?

**Layer 2: Contextual Factors**
- Who else is present?
- Public or private space?
- NPC's current activity?
- Recent events affecting NPC?

**Layer 3: Behavioral Selection**
- Choose appropriate greeting
- Select dialogue branch
- Determine helpfulness
- Set body language

**Layer 4: Response Generation**
- Generate specific response
- Update memory
- Adjust reputation slightly
- Set follow-up potential

### 5.2 Behavioral States

**NPCs Have Emotional States:**

**Content:**
- Default for most
- Standard interactions
- Open to engagement

**Stressed:**
- Reduced patience
- Less helpful
- Quick interactions

**Happy:**
- Generous mood
- Good prices
- More information shared

**Sad:**
- Subdued
- Want to be left alone
- Sometimes need comfort

**Angry:**
- Hostile responses
- May refuse service
- Specific triggers

**Excited:**
- Talkative
- Information flows
- Easy to influence

### 5.3 Mood Triggers

**What Affects Mood:**

**Personal Events:**
- Family member sick: Sad/Stressed
- Recent victory: Happy/Excited
- Lost something: Sad/Angry
- Achieved goal: Happy

**Environmental:**
- Bad weather: Stressed
- Festival day: Happy
- After conflict: Stressed/Sad
- Beautiful day: Content

**Interaction-Based:**
- Just had argument: Angry
- Just had compliment: Happy
- Boring day: Bored (state)
- Busy day: Stressed

### 5.4 Behavior Examples

**Same NPC, Different Days:**

**Day 1: Blacksmith on Good Day**
- Cheerful greeting
- Discount offered
- Tells story about son's success
- Open to long conversation

**Day 5: Same Blacksmith, Bad Day**
- Curt greeting
- Standard prices only
- No personal sharing
- Wants to work

**Day 8: Same Blacksmith, After Loss**
- Quiet
- Sad
- May not work today
- Needs sympathy not chatter

**Same NPC, fully different responses based on internal state.**

### 5.5 Reputation × Mood Interaction

**High Reputation + Good Mood = Maximum Help**
**High Reputation + Bad Mood = Still helps, but minimally**
**Low Reputation + Good Mood = Reluctant help**
**Low Reputation + Bad Mood = Hostile rejection**

---

## 6. CROSS-NPC COMMUNICATION

### 6.1 Information Networks

**NPCs Have Connections:**

**Family:**
- Strongest connection
- Information shared freely
- Reputation transfers significantly

**Friends:**
- Strong connection
- Regular gossip
- Reputation influences

**Professional:**
- Work-related sharing
- Business reputation matters
- Network effects

**Community:**
- General awareness
- Major events shared
- Cultural reputation

**Strangers:**
- No direct connection
- Public reputation only
- General sense

### 6.2 Information Spread Speed

**Major Events (Spread Fast):**
- Within hours: Direct witnesses
- Within day: Local community
- Within week: Town/region
- Within month: Multi-region

**Minor Events (Spread Slow):**
- Witnesses only
- Specific friends
- May never spread widely
- Limited impact

**Personal Stories (Slow):**
- Family first
- Close friends
- Selective sharing
- Privacy respected

### 6.3 Gossip Mechanics

**The Telephone Effect:**
- Stories change as they spread
- Details exaggerated
- Context lost
- Reputation can grow beyond reality

**Examples:**
- Jake helps farmer → "Jake the helpful captain"
- Jake helps drunk → "Jake friend of common man"
- Jake fights guard → "Jake against tyranny"
- Combined: Folk hero status

**Or Reverse:**
- Jake refuses help → "Jake the cold captain"
- Jake takes goods → "Jake the thief"
- Jake kills bandits → "Jake the killer"
- Combined: Feared figure

### 6.4 Network Visualization

**Internal Tracking:**
- Each NPC has connection list
- Information propagates through network
- Decay over distance/time
- Importance weighted

**Player Doesn't See This:**
- Just experiences consequences
- NPCs Jake never met have opinions
- Realistic social dynamics
- Discovery and surprise

---

## 7. NPC AWARENESS LEVELS

### 7.1 Detection Tiers

**NPCs Notice Jake at Different Distances:**

**Awareness Range:**
- 30m: Visible presence
- 15m: Recognition possible
- 5m: Direct interaction range
- 2m: Conversation range
- 1m: Intimate range

**Modifiers:**
- Time of day (lighting)
- Weather
- Crowd density
- NPC's task focus
- Jake's clothing/disguise

### 7.2 Recognition Speed

**How Quickly NPC Recognizes Jake:**

**Stranger:**
- Generic person
- No special attention
- Standard interaction available

**Acquaintance:**
- Recognizes face
- Brief greeting
- Standard relationship plays out

**Friend:**
- Immediate warm recognition
- Specific greeting
- Eager engagement

**Enemy:**
- Immediate negative recognition
- Wariness or hostility
- Specific avoidance/confrontation

### 7.3 Awareness Failures

**NPCs Sometimes Don't Notice:**
- Distracted by task
- Crowded environment
- Poor lighting
- Bad weather
- Their own preoccupation

**Realistic Imperfection:**
- Not omniscient
- Allows stealth gameplay
- Creates surprises
- Believable

---

## 8. EMOTIONAL CONTAGION

### 8.1 Mood Spreading

**Among NPCs:**
- Happy NPC influences others nearby
- Sad NPC dampens mood
- Angry NPC creates tension
- Spreads through social networks

**To Jake:**
- Tavern atmosphere affects mood
- Crowd energy felt
- Specific spaces have feeling
- Subtle player effect

### 8.2 Group Dynamics

**Crowd Behaviors:**
- Festival crowds: Excited
- Funeral crowds: Sad
- Market crowds: Busy
- Tavern crowds: Variable

**Player Influence:**
- Major actions affect crowd
- Speeches sway opinion
- Combat scares
- Generosity warms

---

## 9. NPC PERSONALITY TYPES

### 9.1 The Big Five for NPCs

Each NPC has personality scores in:

**Openness:**
- High: Adventurous, curious, accepts new ideas
- Low: Traditional, cautious, prefers familiar

**Conscientiousness:**
- High: Organized, reliable, planning
- Low: Spontaneous, flexible, sometimes flaky

**Extraversion:**
- High: Outgoing, talkative, social
- Low: Reserved, quiet, private

**Agreeableness:**
- High: Friendly, cooperative, trusting
- Low: Suspicious, competitive, hostile

**Neuroticism:**
- High: Anxious, emotional, reactive
- Low: Calm, stable, even-keeled

### 9.2 Personality-Driven Behavior

**Same Action, Different Reactions:**

**Jake gives unsolicited gift:**
- High Agreeableness: Touched, grateful
- Low Agreeableness: Suspicious of motives
- High Openness: Tries it immediately
- Low Openness: Sets aside cautiously

**Jake asks personal question:**
- High Extraversion: Shares freely
- Low Extraversion: Deflects
- High Neuroticism: Defensive
- Low Neuroticism: Calm response

### 9.3 Relationship Tendencies

**High Agreeableness + High Extraversion:**
- Easy to befriend
- Quick rapport
- Many NPC connections
- Spreads positive info about Jake

**Low Agreeableness + High Conscientiousness:**
- Hard to befriend but loyal once friendly
- Professional relationships
- Reliable contacts
- Slow trust building

**High Neuroticism + Low Openness:**
- Easily offended
- Holds grudges
- Difficult to repair relationship
- Can become hostile easily

---

## 10. NPC QUESTGIVING SYSTEM

### 10.1 Quest Generation

**NPCs Generate Quests Based On:**

**Their Needs:**
- Lost item
- Threatened by something
- Family in trouble
- Business problem

**Their Goals:**
- Personal ambitions
- Status seeking
- Wealth building
- Justice seeking

**Their Personality:**
- Risk-taking → adventurous quests
- Conservative → safe deliveries
- Ambitious → manipulation possible
- Kind → genuine help requests

### 10.2 Quest Types Available

**Direct Help:**
- "Find my lost X"
- "Protect me from Y"
- "Deliver this Z"

**Information Seeking:**
- "Discover what happened to..."
- "Bring me news from..."
- "Spy on..."

**Combat Tasks:**
- "Hunt the beast threatening..."
- "Defeat the bandits at..."
- "Avenge the death of..."

**Social Tasks:**
- "Convince X to..."
- "Mediate between..."
- "Get an audience with..."

**Crafting/Trade:**
- "Bring me materials for..."
- "Craft me a..."
- "Trade with..."

### 10.3 Personal Investment

**Quest Reward Structure:**

**Mechanical:**
- Standard payment
- Items
- Information
- Reputation

**Personal:**
- Relationship deepening
- Personal story revealed
- Friendship cemented
- Long-term contact

**Cultural:**
- Cultural learning
- Tradition introduction
- Regional understanding
- Social access

### 10.4 Quest Refusal

**Players Can Decline:**
- NPC reaction varies
- Some respect refusal
- Others offended
- Quest may not return

**Realistic NPC Response:**
- Not begging persistently
- Offer once, accept refusal
- May ask others
- Personal hurt sometimes

---

## 11. INTERACTION MECHANICS

### 11.1 Initial Approach

**Approaching NPC:**
- Recognized at distance
- Body language indicates receptivity
- Standard interaction prompt
- Context-specific options

**NPC States Affect Availability:**
- Working: Brief interaction only
- Eating: Casual interaction
- Sleeping: Cannot interact (unless emergency)
- Mourning: Limited interaction
- Celebrating: Open and engaged

### 11.2 Conversation Flow

**Standard Structure:**
- Greeting (relationship-based)
- Topic selection (player choice)
- Response (NPC personality + memory)
- Branch options (continue or switch topic)
- Goodbye (relationship-appropriate)

### 11.3 Specific Interaction Types

**Information Gathering:**
- Ask about local events
- Inquire about other NPCs
- Seek directions/advice
- Cultural exchange

**Trade:**
- Show inventory
- Negotiate prices
- Standard transactions
- Bartering possible

**Social Bonding:**
- Personal questions
- Share own story
- Activities together
- Time investment

**Quest Acceptance:**
- Listen to need
- Ask clarifying questions
- Negotiate terms
- Accept or decline

### 11.4 Special Interactions

**Gift Giving:**
- D-pad gesture
- NPC accepts/refuses
- Affects relationship
- Specific gift preferences

**Insults/Threats:**
- Specific dialogue options
- Major reputation impact
- Possible combat
- Lasting consequences

**Apologies:**
- Possible after wrongs
- Reputation recovery
- Some accept, some don't
- Time may help

**Confessions:**
- Player chooses to share
- Builds intimacy
- May change NPC view
- Creates vulnerability

---

## 12. SPECIAL NPC CATEGORIES

### 12.1 Crew (Maximum Tracking)

**For Crew Members:**
- Every interaction tracked
- Long-term relationship arc
- Personal questline progression
- Hundreds of dialogue lines

### 12.2 Recurring Significant (Heavy Tracking)

**For Story-Important NPCs:**
- Major encounters tracked
- Cross-chapter continuity
- Personal evolution
- Significant memory storage

### 12.3 Local Important (Medium Tracking)

**For Town NPCs:**
- Standard tracking
- Reputation maintained
- Schedule patterns
- Quest-relevant

### 12.4 Background (Light Tracking)

**For Crowd NPCs:**
- Generic responses
- Limited memory
- Population feel
- Performance-friendly

### 12.5 Special Cases

**Dead NPCs:**
- Memory persists in others
- Dialog references
- Memorial attended
- Long shadow

**Missing NPCs:**
- Other NPCs comment
- Search quests possible
- Eventual resolution
- Realistic uncertainty

**Hidden NPCs:**
- Discovered through exploration
- Specific quest gates
- Reward for thoroughness
- Easter egg moments

---

## 13. CULTURAL VARIATION

### 13.1 Regional Behavior Patterns

**Rexan/Moran:**
- Warmer initial greeting
- Physical touch common
- Cultural festivals frequent
- Indigenous etiquette

**Fjordlund:**
- Reserved initial meetings
- Direct communication
- Loyalty deep once earned
- Cultural traditions strong

**Ashenmoor:**
- Hospitality emphasized
- Trust earned slowly
- Communal sharing
- Survival focus

**Pale Isle:**
- Class-conscious
- Formal greetings
- Status matters
- Imperial protocols

**Quiet Coast:**
- Friendly settler approach
- Practical interactions
- Community focus
- Family-centric

### 13.2 Code-Switching

**NPCs Adjust Behavior:**
- More formal with strangers
- Casual with friends
- Professional with clients
- Personal with family

**Same NPC, Different Modes:**
- Public persona
- Private personality
- Work mode
- Family mode

---

## 14. PERFORMANCE OPTIMIZATION

### 14.1 LOD for NPCs

**Distance-Based Detail:**
- Far away: Generic crowd
- Medium: Recognized as specific NPCs
- Near: Full behavior tracking
- Adjacent: Maximum detail

### 14.2 Memory Management

**Active vs Stored:**
- Currently relevant: Active memory
- Distant past: Compressed
- Very old: Summarized
- Optimization friendly

### 14.3 Behavior Calculation

**Lazy Evaluation:**
- Calculate on interaction
- Pre-cache likely responses
- Background simulation light
- Performance budget respected

---

## 15. TECHNICAL IMPLEMENTATION

### 15.1 Database Structure

**NPC Database:**
- Identifier
- Personality vector
- Memory storage
- Schedule template
- Current state
- Relationship list
- Reputation network position

### 15.2 Save System

**What's Saved:**
- All named NPC states
- All memory content
- All relationship trackers
- All schedule deviations
- All quest states

**Size Considerations:**
- Significant save file size
- Compression important
- Cloud sync friendly
- Backup recommended

### 15.3 Modular Architecture

**System Components:**
- Memory module
- Schedule module
- Reputation module
- Behavior module
- Communication module
- All interconnected

---

## 16. DESIGNER TOOLS

### 16.1 NPC Editor

**Tool For Designers:**
- Create new NPCs easily
- Set personality
- Define schedule
- Establish connections
- Test behaviors

### 16.2 Memory Inspector

**Debug Tool:**
- View any NPC's memories
- Adjust if needed
- Trace behavior reasons
- Quality assurance

### 16.3 Network Visualizer

**Relationship Mapping:**
- See NPC connections
- Information flow visible
- Identify hubs
- Designer awareness

---

## 17. ACCESSIBILITY

### 17.1 Memory Accessibility

**For Players:**
- Journal tracks all relationships
- Reminders available
- Quest status clear
- NPC location indicators

### 17.2 Interaction Accessibility

**Conversation:**
- Adjustable text speed
- Subtitle options
- Voice acting clarity
- Skip/repeat options

### 17.3 Reputation Visibility

**Optional Display:**
- For players who want
- Off by default
- Subtle indicators
- Tutorial available

---

## 18. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** NPC personalities determine register naturally. Cultural variation respected.
- ✅ **Pillar 2 (Lore Through Objects):** NPC possessions and stories reveal lore. Each character is living document.
- ✅ **Pillar 3 (Layered Death):** NPC reactions to death reflect Style A-D depending on character and context.
- ✅ **Pillar 4 (Layered Humor):** NPC personalities create natural Style C quirks. Real characters generate real humor.
- ✅ **Pillar 5 (Theme — Revenge):** NPC reactions to Jake's path reflect theme. The world judges through individual people.

---

## 19. NEXT STEPS

Next document: **`npcs/dialogue_system.md`** — Foundation Tier Document 2

Will cover:
- Dialogue tree mechanics
- Voice acting integration
- Branching consequences
- Cultural variation
- Specific dialogue patterns

---

## 20. CONCLUSION

This is the technical heart of every NPC interaction in Dark Arisen. Memory tracked. Schedules followed. Reputations earned. Behaviors emerged. Information flowed.

Every NPC the player meets — from Don Alejandro to the random fisherman — runs on this system. The blacksmith remembering Jake helped his apprentice. The widow who tells her grandchildren about Jake decades later. The barkeeper who recognizes Jake from a story heard in a distant port.

This is the architecture that makes the world feel populated by people, not by quest dispensers.

**Phase 5 Subsystem 1 of 22 — complete.**

**Foundation laid. The people can now live.** 🏴‍☠️👥
