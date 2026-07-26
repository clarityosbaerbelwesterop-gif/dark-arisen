# COLONY SYSTEM CORE — THE MOMENT-TO-MOMENT WAR

**Status:** Design Draft v1.0  
**Phase:** Phase 7 — Colonial War, Castles & Dungeons (Foundation Tier Document 2 — Document 3 of 16)  
**Tier:** Foundation Tier  
**Related:** colonial_war_overview.md, colonial_war/war_state_system.md, colonial_war/retaliation_system.md, colonial_war/castle_ownership.md, npcs/building_evolution.md, npcs/regional_populations.md, npcs/children_and_families.md, mechanics/ship_management.md, style_bible.md  
**Purpose:** Define the moment-to-moment gameplay of the colonial war — the Support/Break/Liberate action systems, people-transport mechanics, supply line warfare, the colony development and decay loop, and the transition to Jake's own settlements.  
**Inspirations:** Assassin's Creed Odyssey (conquest activities), Mount & Blade (village/town management, caravan raiding), Death Stranding (delivery-as-connection), Anno series (settlement growth), Red Dead Redemption 2 (moral weight of actions), real historical convoy and colonial logistics

---

## 1. COLONY SYSTEM PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: People Are the Point

A colony is not buildings or flags — it is people. Growing a colony means bringing people. Breaking a colony means the people suffer, flee, or rise. Every action in this system ultimately touches human lives. The war is fought over, and with, and against, people.

**Design intent:** Human stakes underneath every strategic action.

### 1.2 Principle 2: The Verbs Are Physical

Support, Break, Liberate are not menu selections — they are things Jake DOES with his ship, his crew, his hands. Escorting a convoy is a naval mission. Delivering settlers is a voyage with human cargo. Sabotaging a fort is an infiltration. The strategy layer is expressed through action gameplay.

**Design intent:** Strategy felt through doing, never through clicking.

### 1.3 Principle 3: Growth and Decay Are Visible

Cross-reference npcs/building_evolution.md. A supported colony EXPANDS building by building across visits. A bled colony DECAYS window by boarded window. Jake witnesses the consequences of his choices rendered in architecture and population.

**Design intent:** The world visibly responds to the player's war.

### 1.4 Principle 4: Breaking Is Not Freeing

Cross-reference war_state_system.md Section 4.6 (The Fork). The system constantly teaches: tearing down an empire and building freedom are different acts requiring different work. Break without Liberate = the Vacuum. The gameplay makes the moral point mechanical.

**Design intent:** Construction and destruction as distinct disciplines.

### 1.5 Principle 5: Jake Can Build His Own

The endpoint of mastery: Jake stops influencing others' colonies and builds his own. His own settlement, his own plantation, his own trade post, his own people to protect — and his own thing for the empires to threaten (retaliation_system.md). Player-authored stakes.

**Design intent:** From influencing the world to authoring a piece of it.

---

## 2. THE THREE VERBS — ACTION VOCABULARIES

### 2.1 SUPPORT — Feeding a Colony

**The Support verb grows a faction's control by strengthening the colony's three pillars (M/E/P — war_state_system.md Section 2.4).**

**Support Action Types:**

**People-Transport (raises P, grows population):**
- Settler runs (Section 3)
- Skilled worker delivery (craftsmen, engineers)
- Family relocation (cross-reference children_and_families.md)

**Convoy Escort (raises E, protects supply):**
- Merchant convoy protection (Section 4)
- Treasure fleet escort (high-value, high-risk)
- Supply line defense (timed missions)

**Materiel Delivery (raises M/E):**
- War materiel (cannons, powder, muskets → M)
- Resources (timber, food, cloth → E)
- Livestock and seed (→ E, enables plantation growth)

**Infrastructure Funding (raises M/E, costs doubloons):**
- Garrison funding (→ M)
- Harbor improvements (→ E)
- Fortification contracts (→ M)

**Direct Defense (raises M, prevents Fall):**
- Repel a rival siege (the defensive set-piece)
- Hunt raiders threatening the colony
- Naval screen against blockade

### 2.2 BREAK — Bleeding a Colony

**The Break verb reduces a faction's control by attacking the three pillars.**

**Break Action Types:**

**Supply Warfare (lowers E):**
- Convoy raiding (Section 4.4)
- Blockade (timed port denial)
- Warehouse/plantation burning (cross-reference physics/fire_physics.md, destruction_physics.md)
- Treasury destruction (siege-tier)

**Military Sabotage (lowers M):**
- Fort sabotage (powder magazines, water supplies, gate mechanisms)
- Patrol elimination (repeatable attrition)
- Castle Commander assassination (boss — colonial_war_bosses.md)
- **General elimination (boss — the biggest single blow)**

**Popular Incitement (lowers P, raises LS):**
- Free forced laborers (Section 5.2)
- Return stolen sacred items (cross-reference luxury_merchants.md Doña Carmen)
- Spread unrest (dialogue/propaganda quests)
- Expose atrocities (evidence-gathering quests)
- Public defiance (defeat officials openly)

### 2.3 LIBERATE — Building What Comes After

**The Liberate verb is distinct: it builds Liberation Strength so that when a colony Falls, the Alliance can hold it (war_state_system.md Section 4.6 Path A).**

**Liberate Action Types:**

**Arm the Alliance (raises LS):**
- Weapon deliveries to Alliance cells
- Captured empire materiel redistributed
- Ship access for Alliance movement

**Train the Alliance (raises LS):**
- Combat training quest chains
- Tactical coordination (teaching siege craft)
- Naval training (the Alliance has no navy — Jake changes that)

**Connect the Networks (raises LS, unlocks multiplier):**
- The great connection quests: link Mama Jacinta's web + Tito Diego's underground + Mbah Seruni's mountain networks + Fjordlund's resistance
- Each connection is a multi-stage questline
- Cross-reference npcs/cannabis_keeper_mama_jacinta.md, desert_sage_mbah_seruni.md, black_market_fences.md

**Win the Liberation Event (the payoff):**
- The great battle fought WITH Alliance armies (war_state_system.md Path A)
- Defend the newly-liberated region from reconquest

---

## 3. PEOPLE-TRANSPORT — THE HEART MECHANIC

### 3.1 Why People-Transport Is Central

People are the resource the whole system orbits. Colonies grow with people, suffer without them, and rise when their people revolt. The act of physically transporting human beings — settlers, workers, families, freed laborers, refugees — is the emotional and mechanical core of Support and Liberate alike.

### 3.2 The People-Cargo System

**Ships carry people as a distinct cargo type (cross-reference mechanics/ship_management.md):**

**People-Cargo Properties:**
- Occupies passenger capacity (separate from goods hold)
- Requires provisions (people eat — voyage length matters, cross-reference ship provisioning)
- Has a MORALE state (treatment during voyage affects outcome)
- Has a DESTINATION and often a STORY (they are not crates)

**Capacity by Ship State:**
- Cross-reference ship upgrade systems
- Base La Liberación: modest passenger capacity
- Upgraded/converted holds: larger transport capacity
- The choice: cargo space vs passenger space (strategic loadout)

### 3.3 Types of People-Transport

**Settlers (Support — Empire path):**
- Volunteers seeking colonial opportunity
- Delivered to a colony → raises P, grows population
- Building evolution triggers (new homes constructed)
- **Ambient payoff:** on return visits, the settlers Jake delivered are living there — recognizable, grateful

**Skilled Workers (Support — high value):**
- Craftsmen, engineers, healers, teachers
- Delivered → unlocks specific building upgrades (a delivered master smith improves the colony's forge)
- Cross-reference the Geschäft Tier NPCs — Jake can transport specialists between colonies

**Families (Support — emotional weight):**
- Cross-reference children_and_families.md
- Family relocation carries the game's family theme
- Children among the passengers (protected, per children_and_families rules)
- **The care taken in transport matters** — a family delivered safely enriches the destination

**Freed Laborers (Liberate — the moral core):**
- Liberated from plantations/forced-labor colonies
- Transported to safety (liberated regions, Alliance settlements, or freedom elsewhere)
- Lowers the source colony's P, raises LS
- **The heaviest cargo Jake carries** — people whose lives he just changed

**Refugees (consequence — retaliation):**
- Created by war (Vacuum zones, scorched earth — retaliation_system.md)
- Fleeing violence Jake may have caused or failed to prevent
- Transporting them to safety: a Liberation/Protector act
- Cross-reference the_named_dead.md, children_and_families.md orphan handling

### 3.4 The Voyage Matters

**People-transport is not fire-and-forget delivery. The voyage is gameplay:**

**Morale Factors:**
- Provisions adequacy (starving passengers = bad outcome)
- Voyage safety (attacked convoys traumatize/lose passengers)
- Voyage length (long hauls need more provisions, more risk)
- Treatment choices (Jake can spend on comfort or cram them in)

**Voyage Events (Full Vision):**
- Storms threaten passengers (physics/weather_physics.md)
- Sickness can spread (a sick passenger, a choice)
- Pursuit (an empire trying to reclaim freed laborers — a chase)
- Personal moments (a passenger's story, a conversation, a child asking where they're going)

**Design Note — The Death Stranding Lesson:** Delivery becomes meaningful when the cargo is precious and the journey is real. People-transport should feel like carrying something that matters, because it is.

### 3.5 The Manifest (Full Vision)

Named passengers among the anonymous. Some transported people are semi-named (cross-reference children_and_families.md growing children — a family Jake relocates in Chapter 5 might be a recurring presence by Chapter 9). The manifest turns cargo into characters.

---

## 4. SUPPLY LINE WARFARE

### 4.1 Supply Lines as Living Arteries

Every colony is fed by supply lines — visible routes on the sea and coast along which convoys move goods, materiel, and people. These arteries are where much of the war is fought. Jake can protect them (Support) or sever them (Break).

### 4.2 The Supply Line System

**Each colony has 1-3 supply lines (scaled by size):**

**Supply Line Properties:**
- **Route:** a visible path (sea lane, coastal road) between source and colony
- **Traffic:** convoys spawn and travel along it periodically
- **Health:** intact lines feed the colony (passive E gain); disrupted lines starve it (passive E loss)
- **Guard level:** how defended the convoys are (scales with faction control and retaliation stage)

### 4.3 Protecting Supply Lines (Support)

**Escort Missions:**
- Accompany a convoy along its route
- Defend against raiders (Crimson, rival empire, or pirates)
- Successful escort → E gain, faction reputation, pay
- Cross-reference retaliation — escorting a faction's convoys makes their RIVAL target Jake

**Route Patrol:**
- Clear a supply route of threats (timed, area-based)
- Keeps the line healthy passively for a duration
- Repeatable maintenance work

### 4.4 Severing Supply Lines (Break)

**Convoy Raiding — the core Break loop:**
- Intercept convoys along the route
- Naval combat (cross-reference physics/ship_physics.md, combat)
- Plunder the cargo (immediate reward) AND lower colony E (strategic reward)
- **Double payoff:** Jake gets rich AND the colony starves

**Convoy Types (raid targets):**
- **Supply convoys** — food/goods (lower E, modest plunder)
- **Materiel convoys** — weapons/powder (lower M, dangerous — well-guarded, explosive cargo)
- **Treasure fleets** — payroll/tax (lower E significantly, massive plunder, maximum guard — a set-piece)
- **People convoys** — the empire moving forced laborers (raiding these = LIBERATION opportunity: free the cargo, raise LS)

**Blockade:**
- Hold a position off a colony's port (timed, contested)
- Denies all supply lines simultaneously (rapid E drain)
- Draws heavy response (the colony's fleet sorties — a naval siege)

### 4.5 The Raid-to-Liberation Pipeline

A key synergy: raiding **people convoys** (empire transporting forced laborers) lets Jake free the cargo. This single act: lowers source colony P, raises LS, provides freed-laborer passengers to transport, and generates the game's most emotionally charged content. The Break and Liberate verbs meet here.

---

## 5. THE COLONY DEVELOPMENT & DECAY LOOP

### 5.1 Cross-Reference

**See:** npcs/building_evolution.md — the 7-state building system IS the colony development engine. This section applies it to colonies specifically.

### 5.2 Development (Support Consequences)

**As a colony's Control/population grows, buildings advance through evolution states:**

**Colony Growth Stages (visible across visits):**
1. **Outpost** — minimal: dock, a few structures, small garrison
2. **Settlement** — homes, a market, a chapel/shrine, modest fort
3. **Town** — districts forming, multiple merchants, proper walls, a proper garrison
4. **Colony** — thriving: full market, specialists, plantations, a real castle/fort
5. **Jewel** — the crown holdings: dense, wealthy, culturally significant, heavily defended

**What Jake Sees Growing:**
- New buildings constructed between visits (scaffolding → completion)
- New NPCs (the settlers he delivered, now residents)
- New commerce (more merchants, better goods, cross-reference Geschäft Tier)
- Expanding population (children_and_families.md — more children, the world's vitality signal)
- **Ambient mood rising:** *"Good times. New families every month. The captain's shipments keep coming."*

### 5.3 Decay (Break Consequences)

**As Control/economy falls, buildings decay through evolution states in reverse:**

**Colony Decline Stages (visible across visits):**
1. **Strained** — some boarded windows, thinner crowds, defensive tariffs
2. **Declining** — abandoned buildings, emptying market, desperate garrison
3. **Crisis** — riots, desertion, ruin creeping in (war_state_system.md Crisis state)
4. **Broken** — the Fall aftermath: depends on the Fork (liberated bloom / Vacuum rot / rival reflag)

**What Jake Sees Declining:**
- Buildings boarding up, then crumbling (building_evolution decay)
- NPCs leaving (the demographic exodus, regional_populations.md Section 11.3)
- Commerce drying (merchants close, goods vanish)
- **Ambient mood falling:** *"Third family left this month. Market's half of half. Something's coming."*

### 5.4 The Liberation Bloom (Distinct Aesthetic)

**A liberated colony doesn't just "recover" — it transforms:**
- Indigenous/local architecture returns (cross-reference cultural building styles)
- Sacred sites restored (stolen items returned, ceremonies resume)
- Mixed governance visible (Alliance + local + sympathetic settlers)
- A DIFFERENT kind of thriving than imperial growth — the Protector Path made architectural
- **Ambient mood:** *"The old ways and the new, side by side. Nobody hiding. First time in generations."*

---

## 6. JAKE'S OWN SETTLEMENTS

### 6.1 The Transition to Author

**Cross-Reference:** colonial_war/castle_ownership.md (full ownership systems)

Beyond influencing empires' colonies, Jake can build his OWN. Taking a castle intact (castle_sieges.md) or claiming a liberated/abandoned site gives Jake a settlement to develop from scratch.

### 6.2 What Jake Can Build

**Settlement Types (full detail in castle_ownership.md):**
- **Trade Post** — income node, market, convoy anchor
- **Plantation** — resource production (the ethical version: fair-labor, vs the empires' forced-labor model — a pointed contrast)
- **Free Settlement** — a haven for freed peoples, refugees, the crew's chosen families
- **Military Stronghold** — a base, a shield against retaliation, an Alliance bastion

### 6.3 Jake's Settlements Use the Same Loop

**Development:** Jake's settlements grow through the same building-evolution stages — but JAKE chooses the investments, delivers the people, sets the character.

**Population:** Jake transports settlers to his OWN holdings (people-transport, Section 3). Freed laborers can choose to settle in Jake's free settlements — the Liberation payoff made permanent.

**The Vulnerability:** Jake's settlements are TARGETS. Retaliation Stage 3-5 (retaliation_system.md) strikes them. The empire burns what Jake built. This is the emotional teeth of the retaliation system — Jake now has something to lose that HE made.

### 6.4 The Crew Connection

**Cross-Reference:** npcs/named_crew_deep_dives.md, children_and_families.md

Jake's settlements can become home to the crew's stories: Ines's daughter Esperanza might settle safely there; a place for the found-family theme to root. The settlement is where the personal and strategic layers merge.

---

## 7. THE SUPPORT/BREAK/LIBERATE DECISION TEXTURE

### 7.1 No Menu, Only Missions

Jake never opens a "manage colony" screen. Every verb is expressed as available missions, quests, and opportunities in the world:
- A harbor master offering an escort contract (Support)
- A raided convoy's cargo floating in the water (Break opportunity)
- An Alliance contact requesting weapons (Liberate)
- A plantation's laborers whispering of revolt (Break/Liberate)

### 7.2 Directional Clarity

**Cross-Reference:** war_state_system.md Section 8.2

Each opportunity communicates its strategic direction without numbers: *"The Gobernador would reward this generously"* (Support) vs *"This would strike at the Crown's grip here"* (Break) vs *"Mama Jacinta's people could use these"* (Liberate). Players understand consequences directionally.

### 7.3 The Mixed-Play Reality

Most players will mix verbs across regions and time: support Albion against the Empire in one theater while liberating another. The systems track it all (war_state_system.md). The four sages witness it all (colonial_war_overview.md Section 10.3). No verb is locked out by earlier choices — but reputation and relationships remember.

---

## 8. ECONOMY & REWARD FLOW

### 8.1 Support Economics

- Steady, reliable income (escort pay, delivery fees, faction stipends)
- Long-term dividends (villas generate income, base rights save costs)
- Reputation currency (faction standing → access, titles, upgrades)
- **The Support player gets RICH SLOWLY and SAFELY**

### 8.2 Break Economics

- Burst income (plunder is immediate and large)
- Escalating risk (retaliation grows — the money comes with hunters)
- Treasure fleets and treasuries = massive scores
- **The Break player gets RICH FAST and DANGEROUSLY**

### 8.3 Liberate Economics

- Poor in doubloons, rich in access (war_state_system.md rewards)
- Liberation discounts (gratitude prices — long-term savings)
- Unique goods/content no money buys
- **The Liberate player gets POOR but POWERFUL** (in access, secrets, trust)

### 8.4 The Settlement Economy

- Jake's own holdings generate passive income and production
- Investment sink (settlements cost to build and defend)
- The long game: a settlement network as Jake's economic engine
- Vulnerable to retaliation (the risk of building)

---

## 9. INTEGRATION MAP

### 9.1 Systems This Feeds and Reads

- **war_state_system.md** ← every verb writes to the Control meters
- **retaliation_system.md** ← Break actions and Jake's settlements drive retaliation
- **castle_ownership.md** → Jake's settlement systems detailed there
- **building_evolution.md** ← the development/decay visual engine
- **regional_populations.md** ← demographic growth/exodus, ambient mood shifts
- **children_and_families.md** ← family transport, settlement homes, the human stakes
- **ship_management.md** ← people-cargo, convoy combat, loadout choices
- **Geschäft Tier NPCs** ← specialists transportable, colony merchants growing
- **The four sages** ← moral witnesses to the verbs Jake chooses

---

## 10. MVP CORE vs FULL VISION

### 10.1 MVP CORE

- The three verbs functional (Support/Break/Liberate)
- People-transport with settlers and freed laborers (the two key types)
- One supply line per colony, escort and raid missions
- Building-evolution development/decay on one colony pair
- One Jake-buildable settlement (from a taken castle)
- Directional mission clarity (no numbers exposed)

### 10.2 FULL VISION

- All people-transport types with the manifest system (named passengers)
- Voyage events (storms, sickness, pursuit, personal moments)
- Multiple supply lines with materiel/treasure/people convoy variety
- The raid-to-liberation pipeline
- Full colony growth stages (Outpost → Jewel) and the Liberation Bloom aesthetic
- Jake's settlement network with all four settlement types
- The crew-settlement personal integration

---

## 11. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** Colony mood dialogue shifts with development — confident growth-talk, desperate decline-talk, liberated pride-talk. Settlers, laborers, and officials each in their register.
- ✅ **Pillar 2 (Lore Through Objects):** The colony's state read through buildings, market goods, the manifest, supply convoys. Growth and decay are physical.
- ✅ **Pillar 3 (Layered Death):** Freed-laborer transport and refugee flight carry Style D weight. The people Jake couldn't save. The families the war scattered. Never mere logistics.
- ✅ **Pillar 4 (Layered Humor):** Settler and sailor commentary Style C — the absurdities of colonial bureaucracy, the grumbling of transported passengers. Warmth, never mockery of the vulnerable.
- ✅ **Pillar 5 (Theme — Revenge):** The verbs ARE the theme in action. What Jake feeds grows; what he bleeds dies; what he frees blooms. People-transport makes the human cost literal cargo in his hold.

---

## 12. NEXT STEPS

Next document: **`colonial_war/retaliation_system.md`** — Foundation Tier Document 3 (FINAL Foundation Tier)

Will cover:
- The five escalation stages in full mechanical detail
- The Letter/Bounty/Counter-Attack/Hostages/Scorched Earth systems
- Faction-flavored retaliation (Imperial threat vs Albion invoice)
- Hostage selection from the NPC relationship web
- The Crimson tolerance lever
- De-escalation paths

After Retaliation: the Colony Tier begins (Imperial Colonies, Albion Colonies, Liberation Alliance — the individual portraits with their generals).

---

## 13. CONCLUSION

The Moment-to-Moment War. Three verbs made physical: Support that carries settlers in the hold and escorts convoys through dangerous waters; Break that raids treasure fleets and starves colonies convoy by convoy; Liberate that arms the scattered networks and frees the forced laborers whose lives change the instant Jake's crew breaks their chains. People as the resource everything orbits — settlers delivered, families relocated, laborers freed, refugees carried from the fires. Supply lines as living arteries to protect or sever. Colonies that grow building by building or decay window by boarded window. And finally, Jake's own settlements — the moment he stops influencing the world and starts authoring a piece of it, with all the vulnerability that authorship invites.

When a player delivers a family in Chapter 5 and finds them thriving in Chapter 9, when they raid a people-convoy and choose to free the cargo and feel the weight of those lives in their hold, when they watch their own free settlement rise from a taken castle and then watch the empire's retaliation fleet appear on the horizon to burn it — that's when the colonial war stops being strategy. That's when it becomes the story of what Jake chose to carry, and what he chose to leave behind.

**Phase 7 Subsystem 3 of 16 — complete.**

**The people board. The convoys sail. The colonies rise and fall. The verbs endure.** 🚢
