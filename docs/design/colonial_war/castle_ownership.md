# CASTLE OWNERSHIP — WHAT JAKE BUILDS

**Status:** Design Draft v1.0  
**Phase:** Phase 7 — Colonial War, Castles & Dungeons (Castle Tier Document 3 — FINAL Castle Tier — Document 10 of 16)  
**Tier:** Castle Tier  
**Related:** colonial_war_overview.md, colonial_war/castle_catalog.md, colonial_war/castle_sieges.md, colonial_war/colony_system_core.md, colonial_war/retaliation_system.md, colonial_war/indigenous_liberation_alliance.md, npcs/building_evolution.md, npcs/children_and_families.md, npcs/named_crew_deep_dives.md, npcs/regional_populations.md, npcs/the_named_dead.md, physics/destruction_physics.md, style_bible.md  
**Purpose:** Define the complete castle and settlement ownership system — the four holding types, development from ruin upward, population and who lives there, production economies including the ethical plantation, fortification and garrison, the settlement network, Alliance Bastions, vulnerability to retaliation, and the Governor Problem.  
**Inspirations:** Mount & Blade (fief management), Assassin's Creed II/Brotherhood (Monteriggioni renovation), Suikoden (castle-building and recruitment), Red Dead Redemption 2 (camp as home), Anno series (production chains), Fallout 4 settlements (attachment through investment), real colonial-era plantation and free-settlement economics

---

## 1. OWNERSHIP PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Ownership Turns Strategy Into Home

**Cross-reference:** colony_system_core.md Principle 5

Everything before this document was Jake affecting other people's places. Ownership is the moment he makes one. A holding is not a resource node — it is somewhere Jake's crew sleeps, somewhere freed people settle, somewhere a child he transported in Chapter 5 is growing up.

**Design intent:** The strategic layer acquires an emotional address.

### 1.2 Principle 2: You Build It Person by Person

**Cross-reference:** colony_system_core.md Section 3 (people-transport)

A holding's growth is not purchased with doubloons alone. It is populated by people Jake physically carried there. Every settler, every freed laborer, every refugee, every specialist. **The population of Jake's settlements is a manifest of his voyages.**

**Design intent:** Investment measured in journeys, not just gold.

### 1.3 Principle 3: What You Own, You Must Defend

**Cross-reference:** retaliation_system.md Sections 3.4-3.6, castle_sieges.md Section 7

Every holding is a target. This is the design's deliberate bargain: ownership grants income, production, and belonging, and in exchange it hands the empires something to burn. **The retaliation system has no teeth until Jake has something he made.**

**Design intent:** Stakes through construction.

### 1.4 Principle 4: The Ethical Version Is Not the Free Version

Jake can build a plantation that pays wages instead of holding contracts. It produces less, costs more, and takes longer. It is also the only argument the game lets him make in practice rather than dialogue.

**Design intent:** Morality with a price tag, honestly stated.

### 1.5 Principle 5: A Man With Nine Fortresses Is Not a Pirate

**Cross-reference:** indigenous_liberation_alliance.md Principle 5, castle_catalog.md Section 3.8

The game does not stop Jake from accumulating holdings, garrisons, and subjects. It simply lets the resemblance accumulate too — in what NPCs call him, in how the sages look at him, and in one question a network leader eventually asks out loud.

**Design intent:** The Governor Problem (Section 10). Unblocked, unpunished, and unmistakable.

---

## 2. THE FOUR HOLDING TYPES

### 2.1 Acquisition

A holding becomes available when:
- A castle is **taken intact** (castle_sieges.md Section 8.3)
- An **Old Fortress is reclaimed** (castle_catalog.md Section 12) — Alliance Bastion only
- An **abandoned or ruined site** is claimed (some exist without conquest — Full Vision)

**Razed castles cannot be owned.** Cross-reference castle_sieges.md Section 8.2 — this is the raze decision's real cost.

### 2.2 TYPE ONE — TRADE POST

**Identity:** an economic node. Warehouses, docks, a market, a counting room.

**What it does:**
- Generates steady passive income
- Provides a **market anchor** — buy and sell at favorable rates, store cargo beyond hold capacity
- Serves as a **convoy origin** — Jake can run his own supply lines (cross-reference colony_system_core.md Section 4)
- Fast-travel and repair anchor

**Best sites:** Castillo Dorado (banking infrastructure, Silvera access), Coventry House (the counting halls continue, counting different things), coastal sites with existing harbors

**Population need:** low (clerks, dockworkers, guards — 40-120)

**Upkeep:** low  
**Income:** high and reliable  
**Defense burden:** moderate (worth raiding, not worth besieging)

### 2.3 TYPE TWO — SETTLEMENT

**Identity:** a place people live. Homes, workshops, fields, a market, eventually a school and a chapel or shrine.

**What it does:**
- **Population growth** (cross-reference npcs/regional_populations.md, children_and_families.md)
- **Production** — plantations, workshops, fisheries, timber (Section 5)
- **Recruitment** — crew, garrison, and specialists emerge from a thriving population
- **The emotional core** — this is where the people Jake carried actually live

**Sub-specializations (chosen at development stage 3):**
- **Plantation Settlement** — agricultural production at scale (Section 5.3)
- **Craft Settlement** — workshops, smiths, alchemists (cross-reference the Geschäft Tier)
- **Free Settlement** — a haven for freed peoples and refugees; lower production, maximum LS and trust generation
- **Port Settlement** — shipbuilding and maritime trades

**Population need:** high and growing (200-2,000+)

**Upkeep:** moderate to high  
**Income:** variable by specialization  
**Defense burden:** **the highest** — settlements have civilians, and cross-reference retaliation_system.md Stage 5

### 2.4 TYPE THREE — MILITARY STRONGHOLD

**Identity:** a fortress kept as a fortress. Garrison, batteries, magazine, dry dock.

**What it does:**
- **Projects Control** for Jake's chosen alignment (cross-reference war_state_system.md — a held stronghold contributes Control to whichever faction Jake designates, or holds a region neutral)
- **Retaliation shield** — a strong garrison in a region reduces counter-attack success against Jake's other holdings there
- **Naval base** — repair, resupply, and refit at cost (cross-reference physics/ship_physics.md)
- **Staging** — Alliance forces or hired troops assemble here before operations

**Best sites:** the Sterling Bastion (if taken), Puesto del Norte or Fort Resolute (if the Northern Front is played that way), La Ciudadela (the apex, with all the discomfort that implies)

**Population need:** low civilian, high garrison (200-600 soldiers)

**Upkeep:** **very high** (soldiers eat, and soldiers are paid)  
**Income:** none directly  
**Defense burden:** high, but it is *built* for it

### 2.5 TYPE FOUR — ALLIANCE BASTION

**Cross-reference:** indigenous_liberation_alliance.md Section 5.7, castle_catalog.md Section 12.5

**Identity:** not Jake's. A joint holding of the Liberation Alliance, in which Jake has a room and a standing invitation.

**What it does:**
- **Self-garrisoning** — free, loyal, culturally rooted defenders (cross-reference castle_sieges.md Section 7.7)
- **Regional LS defense** — resists reconquest without Jake's presence
- **Cross-network crafting** — the only place Pak Setyo's volcanic work and Halvor Eriksson's northern steel actually meet (cross-reference indigenous_liberation_alliance.md Section 4.4)
- **Network gatherings** — the political heart of the Alliance
- **Sanctuary** — freed people, refugees, and those the empires hunt

**Restrictions:**
- Cannot be sold, converted, taxed, or garrisoned with hired troops
- **Jake does not command it.** He is consulted, welcomed, and occasionally overruled.

**Population need:** none — the Alliance populates it

**Upkeep:** zero  
**Income:** zero (but gratitude economy access — cross-reference indigenous_liberation_alliance.md Section 5.5)  
**Defense burden:** **zero**

**Design note:** the Alliance Bastion is mechanically the best holding type in the game and gives Jake the least ownership. That inversion is the point.

---

## 3. THE DEVELOPMENT SYSTEM

### 3.1 Building Evolution, Applied to Jake

**Cross-reference:** npcs/building_evolution.md (the 7-state system)

The same engine that grows and decays colonies runs Jake's holdings — but here **the player is the cause.**

### 3.2 The Five Development Stages

**STAGE 0 — CLAIMED**  
The flag is Jake's. Nothing else has changed. Battle damage present (cross-reference physics/destruction_physics.md — whatever the siege did is still there). Empty rooms, a skeleton caretaker crew.

**STAGE 1 — FUNCTIONAL**  
Repairs to structure, walls, and roofs. Basic quarters, a kitchen, a store. It can house people and survive weather.  
*Requirements:* materials, labor, ~3,000-8,000 doubloons

**STAGE 2 — ESTABLISHED**  
Type declared (Section 2). Core infrastructure built — warehouse, market, barracks, or workshops depending on type. Population arrives.  
*Requirements:* ~10,000-25,000 doubloons, people delivered

**STAGE 3 — DEVELOPED**  
Specialization chosen. Production running. Distinct architecture emerging. A community exists.  
*Requirements:* ~30,000-70,000 doubloons, sustained population, specialists delivered

**STAGE 4 — THRIVING**  
Self-sustaining. Growing without Jake's direct investment. Children (cross-reference children_and_families.md — the vitality signal). A place with its own life.  
*Requirements:* time, stability, and the absence of catastrophe

### 3.3 What Jake Sees Between Visits

**Cross-reference:** npcs/npc_recurrence_system.md

Development happens while Jake is away, and he returns to changes:
- Scaffolding gone, a new building finished
- A wall he ordered repaired, repaired
- New faces, and old faces older
- Children playing where there was rubble
- A name for the place, chosen by the people living in it (**Jake does not name his settlements. They name themselves, and he finds out.**)

### 3.4 Damage and Repair

**Cross-reference:** physics/destruction_physics.md

- Siege breaches persist until repaired (and repair costs scale with how the castle was taken — cross-reference castle_sieges.md Principle 5)
- Retaliation damage sets development back (Section 9)
- **Some damage is left deliberately.** Full Vision: players can choose to leave a breach unrepaired as a marker. The community may prefer it.

---

## 4. POPULATION — WHO LIVES HERE

### 4.1 The Manifest Becomes a Census

**Cross-reference:** colony_system_core.md Section 3

Every person in Jake's holdings arrived on his ship. The population screen — if it exists at all — should read less like a resource count and more like a passenger list with dates.

### 4.2 The Five Populations

**SETTLERS** — volunteers seeking opportunity. Delivered from Imperial or Albion recruitment, or from overcrowded ports. Ordinary, practical, and they build things.

**FREED LABORERS** — from Nueva Esperanza's fields, from Blackwood Landing's contract villages, from intercepted people-convoys (cross-reference colony_system_core.md Section 4.5). **The heaviest passengers Jake carries, and the ones with the strongest reason to make the place work.**

**REFUGEES** — displaced by the war, sometimes by Jake's own campaigns (cross-reference retaliation_system.md Stage 5). A settlement that takes refugees is a settlement that has decided something.

**SPECIALISTS** — craftsmen, healers, teachers, engineers. Cross-reference the Geschäft Tier: a delivered master smith or apothecary transforms a settlement's capability. Some are recruitable from existing colonies; some are Shippable Specialists' relatives or students.

**THE CREW'S PEOPLE** — Section 11.

### 4.3 Capacity and Growth

Population capacity scales with development stage and available housing. Growth comes from:
- **Delivery** (Jake's voyages — the primary source)
- **Natural growth** (cross-reference children_and_families.md — thriving settlements have children, and children grow across chapters)
- **Migration** (Full Vision: at Stage 4, people arrive on their own, drawn by reputation — the settlement recruits for itself)

### 4.4 Morale and Character

Settlements have a **character**, visible in ambient dialogue and architecture, shaped by population mix:

- **Freed-majority settlement:** cautious pride, communal decision-making, cultural revival. Cross-reference indigenous_liberation_alliance.md — these generate the most LS.
- **Settler-majority:** practical, commercial, frontier-pragmatic. Cross-reference npcs/regional_populations.md Section 7 — Quiet Coast texture.
- **Mixed:** the most interesting, and occasionally the most tense. Full Vision: freed peoples and settlers negotiating what the place is going to be.
- **Refugee-heavy:** grief in the architecture. Quieter. Cross-reference the_named_dead.md aesthetic.

### 4.5 Ambient Samples

**A thriving free settlement:**

*"...came off the captain's ship with nothing but the clothes. Now I've a roof, a plot, and a boy who can read. Say what you like about pirates..."*

**A settlement rebuilding after a raid:**

*"...second time they came. Second time we're still here. Third time they'll find the walls higher..."*

**A refugee settlement:**

*"...nobody asks where you're from here. You'll notice. It's deliberate..."*

---

## 5. PRODUCTION AND ECONOMY

### 5.1 The Income Model

Net Income = (Production value × Market access)
− (Upkeep + Garrison pay + Development amortization)

**Rough scales (per chapter):**
- **Trade Post:** +3,000 to +12,000 doubloons net
- **Settlement (production):** +2,000 to +15,000 net, scaling hard with specialization and stage
- **Settlement (free/refugee focus):** often −1,000 to +2,000 — **it costs money and pays in something else**
- **Military Stronghold:** −5,000 to −20,000 (pure expense)
- **Alliance Bastion:** 0

### 5.2 Production Chains

**Cross-reference:** mechanics/inventory_crafting.md, fauna documents

- **Agricultural** — sugar, tobacco, coffee, indigo, food (Plantation Settlement)
- **Extraction** — timber, ore, stone, volcanic glass
- **Craft** — worked goods, weapons, tools, medicines (Craft Settlement; cross-reference the Geschäft Tier NPCs delivered as specialists)
- **Maritime** — shipbuilding, repair, provisioning (Port Settlement)

**Cross-reference colony_system_core.md Section 4:** Jake's holdings can supply *each other* via his own convoys — a timber settlement feeding a shipyard settlement.

### 5.3 THE ETHICAL PLANTATION

**The pointed contrast.** Cross-reference imperial_colonies.md Section 4 (Nueva Esperanza — chains), krone_colonies.md Section 6 (Blackwood Landing — contracts).

Jake can build a plantation. He chooses its labor model:

**THE WAGE MODEL:**
- Workers are paid, free to leave, and share in the harvest
- **Output: roughly 60-70% of a forced-labor plantation of equal size**
- **Costs: significantly higher** (wages, better housing, shorter hours)
- **Net profit: substantially lower**

**What it produces instead:**
- **LS and trust generation** (the strongest per-chapter source outside Liberation Events)
- **Population growth** — word travels; freed people choose Jake's fields
- **Premium market access** — cross-reference the Phase 4 sustainability economics: goods produced without forced labor command a premium in specific markets (some Pale Isle reform-aligned buyers, the gratitude economy, and — pointedly — Admiral Sterling's provisioning contracts)
- **The argument** — Ashcroft said *"bring me numbers"* (krone_colonies.md Section 6.3). At Stage 4, Jake has numbers. They are worse numbers. **And the settlement is still standing and still full, fifteen years after Blackwood Landing's villages would have been emptied and refilled twice.**

**Design note:** the game must not fudge this. The ethical plantation is *less profitable.* Making it secretly optimal would be a lie, and the honest version is a better argument: Jake chooses it knowing the cost. **Full Vision: a late-game conversation where Ashcroft, or Blackwood, examines Jake's books and is genuinely, briefly, unsettled — not by the morality, but by the fact that it works at all.**

### 5.4 The Contract Question

If Jake takes Blackwood Landing's contracts (castle_catalog.md Section 10.5) and owns a plantation, an option exists that the design should present without comment: **he could enforce them.**

The instruments are legal. The people are on the land. Nothing prevents it.

Cross-reference indigenous_liberation_alliance.md Section 3.4 (betrayal), and every sage in the game. **The option exists. That is all the design does with it.**

---

## 6. FORTIFICATION AND GARRISON

### 6.1 Fortification Stages

**Cross-reference:** castle_sieges.md Section 7.3

| Stage | Effect | Cost |
|---|---|---|
| **Damaged** | breaches open; assaults enter freely | — |
| **Repaired** | walls intact; standard defense | 3,000-8,000 |
| **Reinforced** | improved walls, cleared fields of fire | 15,000-30,000 |
| **Fortified** | batteries restored/improved, magazine, cisterns | 40,000-80,000 |
| **Redoubtable** | the fortress at its best; sieges become campaigns | 100,000+ |

**Design note:** cross-reference castle_catalog.md — each castle's fortification ceiling differs. Coventry House can never be Redoubtable; it is a counting house. Puesto del Norte's unfinished bastion can finally be *finished*, eight years late, by a pirate.

### 6.2 Garrison

**Sources:**
- **Hired troops** — reliable, expensive, and they leave if unpaid
- **Settlement militia** — cheap, motivated, less trained; grows with population
- **Freed-people defenders** — highly motivated, cross-reference LS; they are defending their own homes
- **Alliance fighters** — only at Bastions, and they are not Jake's to command
- **Crew detachment** — Jake can leave crew behind, at the cost of ship effectiveness (a real trade, cross-reference mechanics/ship_management.md)

**Garrison quality matters more than size** in defensive sieges (castle_sieges.md Section 7.4).

### 6.3 The Warning Network

**Full Vision:** investment in signal stations, scout boats, and paid informants gives earlier warning of counter-attacks (cross-reference retaliation_system.md Section 6.1). The difference between "they arrived" and "they will arrive in four days" is the difference between losing a settlement and holding one.

---

## 7. THE SETTLEMENT NETWORK

### 7.1 Holdings Interact

Multiple holdings in a region form a network:

- **Supply chains** — production feeds production (timber → shipyard; food → garrison)
- **Mutual defense** — a stronghold shields nearby settlements from raids (Section 2.4)
- **Population flow** — Full Vision: people move between Jake's holdings as needs shift
- **Control projection** — cumulative, cross-reference war_state_system.md

### 7.2 The Network Economy

A well-built network becomes Jake's own economic engine — genuinely self-sustaining by late game, and capable of funding campaigns without plunder.

**And it becomes a target worth a fleet.** Cross-reference retaliation_system.md Stage 5.

### 7.3 Overextension

**The honest constraint:** more holdings means more upkeep, more garrison, more defensive obligation, and more places the empires can hurt.

**Cross-reference castle_sieges.md Section 7.6 (the impossible choice):** at Stage 3-5 retaliation with five holdings, Jake will lose something. The network's size is a strategic choice with a real ceiling.

---

## 8. ALLIANCE BASTIONS IN DETAIL

### 8.1 Reclamation

**Cross-reference:** castle_catalog.md Section 12.4, indigenous_liberation_alliance.md Section 5.7

Old Fortresses are reclaimed across multiple stages — Alliance knowledge and labor, Jake's ship and resources:

**Shelter → Garrison → Workshop → Bastion**

Each stage unlocks capability, and each is a joint project in which Jake supplies and the Alliance decides.

### 8.2 What Bastions Give

- **Cross-network crafting** — the volcanic-northern steel unlock, and other pairings (Full Vision: five networks, ten possible craft fusions)
- **Sanctuary** — freed people, hunted people, and anyone the empires want
- **The gatherings** — cross-reference indigenous_liberation_alliance.md Section 4.8
- **Regional LS defense** — the strongest anti-reconquest asset in Phase 7
- **A room for Jake** — and cross-reference the_named_dead.md aesthetic: the room is always ready, and it is always small.

### 8.3 What Bastions Refuse

Jake cannot: garrison them with hired troops, tax them, convert them, sell them, or overrule the network's decisions about them.

**Full Vision:** a scene where Jake proposes something for a Bastion and is told no, politely, by people who are grateful to him and are still saying no.

---

## 9. VULNERABILITY — WHEN THEY COME FOR IT

### 9.1 The Bargain

**Cross-reference:** retaliation_system.md Sections 3.4-3.6, castle_sieges.md Section 7

Ownership hands the empires targets. This is deliberate and it is the source of Phase 7's emotional stakes.

### 9.2 What Retaliation Does

**Stage 3 — Counter-Attack:** raids on trade posts and settlements. Goods stolen, buildings damaged, development set back. Defensible with warning.

**Stage 4 — Hostages:** the algorithm's candidate pool includes **settlement residents Jake delivered** (cross-reference retaliation_system.md Section 4.1 — Attachment Score). The family he relocated in Chapter 5 is exactly the kind of person the empire takes.

**Stage 5 — Scorched Earth:** settlements **burned**. Cross-reference physics/fire_physics.md, destruction_physics.md. The people Jake settled: killed, scattered, or made refugees — and the refugee-transport loop begins again, carrying people out of a place he built for them.

### 9.3 Rebuilding

A burned settlement can be rebuilt. It costs, it takes chapters, and **it is not the same place.** Cross-reference npcs/npc_recurrence_system.md: the population is different. Some names are gone (the_named_dead.md). The rebuilt settlement's ambient dialogue reflects it.

**Full Vision:** the community may choose to rebuild *elsewhere*, and ask Jake to carry them. He can refuse. He can also say yes.

### 9.4 The Design Statement

Cross-reference retaliation_system.md Section 8.3 (the restraint reward): **a player who never takes a holding never has one burned.** The Support-path merchant captain with a villa and no settlements sails through Phase 7 with far less to lose. That is a legitimate way to play, and the game should let it be quietly, unheroically viable.

---

## 10. THE GOVERNOR PROBLEM

### 10.1 The Accumulation

By late Phase 7 a maximally acquisitive Jake may hold: a trade post, two settlements, a military stronghold with four hundred paid soldiers, and several thousand people whose homes he owns.

**He is, functionally, a colonial power.**

### 10.2 How the Game Notices

**No mechanic punishes it. Everything else does:**

- **Ambient dialogue shifts** (cross-reference npcs/regional_populations.md Layer 3 — Jake-reactive). *"The Captain's holdings"* becomes a phrase people use. Then *"the Captain's territory."* Then, from someone who does not mean it kindly, *"his colonies."*
- **The sages.** Mama Jacinta will visit one of Jake's settlements once and ask one question about who decides things there. Mbah Seruni will say something about how the mountain does not need a governor. Mormor Astrid will simply note how many people depend on him now, and whether he has thought about what happens if he dies.
- **The Alliance.** Cross-reference indigenous_liberation_alliance.md Section 8.3 — at full connection, a network leader (Don Eduardo Mendoza or Tito Diego) asks Jake plainly what he wants out of this, and the holdings are the uncomfortable half of the answer.
- **Don Alejandro.** The Imperial officer who spent thirty-three years administering colonies, watching a pirate acquire them. He will not moralize. He will offer practical administrative advice, warmly, and that will be worse than a lecture.

### 10.3 The Available Answers

- **Give holdings away** — to communities, to the Alliance, to the people living in them. Cross-reference castle_sieges.md Section 8.4: it pays nothing.
- **Hold them and accept it** — a valid path, played honestly, with Jake as a new kind of power in the archipelago. Cross-reference war_state_system.md Section 7.3: no configuration is "the win."
- **Never accumulate** — the pirate who takes castles and razes them, owning nothing, owing nothing.

**Design law:** the game presents the resemblance and takes no position. Cross-reference style_bible.md Pillar 5.

---

## 11. THE CREW AND THE FOUND FAMILY

### 11.1 Where the Crew Lives

**Cross-reference:** npcs/named_crew_deep_dives.md, npcs/children_and_families.md Section 3.5

La Liberación is home. But a settlement gives the crew something they have not had: **somewhere to come back to that isn't a ship.**

**Full Vision — crew presence at holdings:**
- **Big Tom** takes over the forge, and it is the best-equipped workshop he has ever had. He complains about the space anyway.
- **Ines** builds a proper apothecary — the first fixed one she has had since the daughter she is looking for.
- **Father Salvio** does not build a church. He builds a room where people can sit, and does not put a symbol on the door.
- **Esteban** teaches navigation to whoever shows up, which turns out to be children.
- **Mira** does not settle. She visits. She is the crew member most uncomfortable with Jake owning things, and cross-reference her Fjordlund roots — her people are watching two empires do exactly this.

### 11.2 Esperanza

**Cross-reference:** npcs/named_crew_deep_dives.md Section 4 (Ines's daughter quest)

If Ines's daughter Esperanza is found, and if Jake has a settlement, **the settlement is where she can be safe.**

This is the single strongest emotional argument for ownership in the game, and it should be earned rather than announced. The design does not offer it as a reward. It becomes available, quietly, and Ines is the one who raises it.

### 11.3 The Found Family Made Physical

**Cross-reference:** npcs/children_and_families.md Section 3.5 (pirate haven found-family architecture)

Phase 5 established the crew as found family and the pirate havens as communal child-raising. A Jake settlement is that theme given walls: crew, freed people, refugees, and children who do not all share blood and share everything else.

**Design note:** the parallel should never be stated. Cross-reference children_and_families.md Section 6.2 — restraint absolute.

---

## 12. CONVERSION, ABANDONMENT, AND LOSS

### 12.1 Conversion

A holding can change type (Trade Post → Settlement, Settlement → Stronghold) at a cost in doubloons and development stages. **Alliance Bastions cannot be converted, by anyone, ever.**

### 12.2 Abandonment

Jake can walk away from a holding. What happens next:
- **Trade Post:** decays, then someone else takes it
- **Settlement:** the people remain and become an independent community — or they don't, and cross-reference war_state_system.md Section 4.6 (the Vacuum)
- **Stronghold:** the garrison disperses; the empire re-occupies

**Full Vision:** an abandoned settlement can be revisited chapters later. Sometimes it is thriving without him. Sometimes it is empty. **The game does not tell Jake which until he sails there.**

### 12.3 Loss

Cross-reference Section 9.2, retaliation_system.md Stage 5.

A holding destroyed is a holding gone. The site remains — ruined, and visitable. Cross-reference physics/destruction_physics.md, npcs/the_named_dead.md.

**Design note:** the ruins of Jake's own settlement should be among the heaviest locations in the game. No music. The buildings he paid for. The names on the wall of a chapel Salvio built.

---

## 13. INTEGRATION MAP

- **castle_sieges.md** ← taking intact enables ownership; defensive sieges protect it
- **castle_catalog.md** ← each castle's ownership potential and fortification ceiling
- **colony_system_core.md** ← people-transport populates holdings; Jake's convoys supply them
- **war_state_system.md** ← held strongholds project Control
- **retaliation_system.md** ← holdings as targets across Stages 3-5; hostage pool
- **indigenous_liberation_alliance.md** ← Alliance Bastions, LS generation, the Governor question
- **npcs/building_evolution.md** ← the development engine
- **npcs/children_and_families.md** ← who lives there, and the vitality signal
- **npcs/named_crew_deep_dives.md** ← crew presence, Esperanza
- **npcs/regional_populations.md** ← settlement character, ambient, demographic growth
- **npcs/the_named_dead.md** ← what is lost when a settlement burns
- **physics/destruction_physics.md, fire_physics.md** ← damage, ruin, persistence
- **the four sages** ← the Governor Problem's mirrors

---

## 14. MVP CORE vs FULL VISION

### 14.1 MVP CORE

- **One ownable castle** (from the MVP siege set — Ashcroft Hall or Fuerte San Rafael)
- Three holding types: Trade Post, Settlement, Alliance Bastion
- Development stages 0-3
- Population from settlers and freed laborers
- One production specialization (the ethical plantation, because it carries the argument)
- Basic fortification and garrison
- Retaliation Stage 3 counter-attacks against the holding
- Crew presence at the settlement

### 14.2 FULL VISION

- All four holding types, all specializations, Stage 4 self-sustaining growth
- The full settlement network with inter-holding supply chains
- Migration (settlements recruiting for themselves)
- The warning network
- Stage 5 destruction, rebuilding, and relocation
- The Governor Problem's full reactive layer (ambient, sages, Alliance, Don Alejandro)
- Esperanza's settlement thread
- The contract-enforcement option, presented and unremarked
- Abandonment and the uncertain return

---

## 15. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** Settlement ambient shifts with population mix — freed-majority pride, settler pragmatism, refugee quiet. The name the settlement chooses for itself is a register statement.
- ✅ **Pillar 2 (Lore Through Objects):** The scaffolding gone, the finished bastion eight years late, the breach deliberately left unrepaired, Salvio's room with no symbol on the door, the empty forge after a raid.
- ✅ **Pillar 3 (Layered Death):** The ruins of Jake's own settlement, without music. The names lost in a Stage 5 burning. The rebuilt place that is not the same place.
- ✅ **Pillar 4 (Layered Humor):** Big Tom complaining about the best forge he has ever had. Esteban's navigation students turning out to be seven years old. The settlement choosing a name Jake finds slightly embarrassing.
- ✅ **Pillar 5 (Theme — Revenge):** Ownership is where the theme turns its hardest question on the player. Jake came to burn an empire down and has acquired a trade post, two settlements, four hundred soldiers, and three thousand people who depend on him. **The game does not stop him. It just lets everyone notice.**

---

## 16. NEXT STEPS

**CASTLE TIER COMPLETE.**

Next document: **`colonial_war/colonial_war_bosses.md`** — Boss Tier Document 1

Will cover the full Phase 1-2-style boss designs:
- The nine Generals and Directors (Herrera, Reyes, Cruz, de Silva, Vega, Blackwood, Sterling, Ashcroft, Thorne)
- Castle Commanders (Tier 2)
- Arenas, phases, mechanics, and Style Bible deaths for each

After War Bosses: the Assassin Network and its questline. Then the Dungeon Tier.

---

## 17. CONCLUSION

What Jake Builds. A trade post in a counting house that now counts different things. A settlement whose name he did not choose and found out about on his third visit. A plantation that pays wages and produces less and is still standing and still full fifteen years later. An Old Fortress reclaimed with the Alliance, where he has a room that is always ready and always small.

The people in them arrived on his ship — settlers, freed laborers, refugees, a family he relocated in Chapter 5 whose daughter now runs the morning errands. Big Tom's forge. Ines's apothecary, and maybe, eventually, Esperanza. A room Father Salvio built with nothing on the door.

And the bargain underneath all of it: everything Jake makes is something the empires can burn, and everything he holds makes him look a little more like the thing he came here to fight. The game does not stop him from accumulating. It just lets the word change in people's mouths — holdings, then territory, then *his colonies* — and waits to see whether he notices.

**Phase 7 Subsystem 10 of 16 — complete.**

**CASTLE TIER COMPLETE.**

**The walls stand. The people arrive. The place gets a name. And now there is something to lose.** 🏡⚓
