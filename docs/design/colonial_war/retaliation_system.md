# RETALIATION SYSTEM — WHEN THE EMPIRES STRIKE BACK

**Status:** Design Draft v1.0  
**Phase:** Phase 7 — Colonial War, Castles & Dungeons (Foundation Tier Document 3 — FINAL Foundation Tier — Document 4 of 16)  
**Tier:** Foundation Tier  
**Related:** colonial_war_overview.md, colonial_war/war_state_system.md, colonial_war/colony_system_core.md, colonial_war/assassin_network.md, npcs/npc_recurrence_system.md, npcs/the_named_dead.md, npcs/regional_populations.md, npcs/imperial_contacts.md, physics/destruction_physics.md, bosses/crimson_armada.md, style_bible.md  
**Purpose:** Define the complete retaliation system — the five escalation stages, faction-flavored responses (Imperial threat vs Albion invoice), the named-assassin bounty, hostage selection from the NPC relationship web, counter-attacks on Jake's holdings, scorched earth, the Crimson tolerance lever, and de-escalation paths.  
**Inspirations:** Shadow of Mordor (nemesis reactivity), Red Dead Redemption 2 (bounty and consequence), Assassin's Creed Odyssey (mercenary hunters), Mount & Blade (faction war reprisals), The Godfather (escalating vendetta), real historical colonial reprisal and privateer-hunting

---

## 1. RETALIATION PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Actions Have Hunters

Break a colony and the empire notices. Notice becomes a letter. A letter becomes a bounty. A bounty becomes hunters at Jake's door. The war is not a sandbox with consequence-free destruction — every blow Jake lands generates a response with Jake's name on it.

**Design intent:** Consequence with a face, escalating toward the player.

### 1.2 Principle 2: The Empires Have Character

Imperial retaliation and Albion retaliation feel completely different. The Empire threatens with wounded aristocratic pride and church authority. Albion's HAC responds with itemized invoices and contract enforcement. How an empire strikes back reveals who it is.

**Design intent:** Faction identity expressed through reprisal.

### 1.3 Principle 3: They Hit What Jake Loves

Cross-reference npcs/the_named_dead.md, npcs/npc_recurrence_system.md. The retaliation system's cruelest tool is the relationship web Phase 5 built. The empire doesn't just chase Jake — it takes the merchant who always greeted him, burns the settlement he raised, threatens the friends whose names he knows. Jake's connections become his vulnerabilities.

**Design intent:** The personal cost of the strategic war.

### 1.4 Principle 4: Escalation Is a Ladder, Not a Switch

Retaliation climbs through discrete, telegraphed stages. Jake always sees it coming — the letter warns, the bounty announces, the counter-attack builds. The dread is in the ladder. Players can act at each rung: pay, fight, de-escalate, or climb higher.

**Design intent:** Telegraphed escalation creates dread and agency.

### 1.5 Principle 5: The War Feeds the Climax

Cross-reference bosses/crimson_armada.md. Retaliation's darkest lever: a desperate empire grants the Crimson Armada tolerance to hurt Jake. The colonial war is not separate from the main plot — pushed far enough, it strengthens the game's true antagonist. Jake's choices set the difficulty of his own ending.

**Design intent:** The side-system feeds the main story's stakes.

---

## 2. THE RETALIATION ENGINE

### 2.1 The Heat Meter (Per Faction)

Each empire tracks **Retaliation Heat** against Jake (0-100), separate for Imperial and Albion:

**Heat rises from Break actions (colony_system_core.md):**
- Convoy raids, sabotage, officer kills accumulate Heat
- Larger blows spike Heat harder (General defeat = massive Heat)
- Heat against the faction Jake DAMAGES; supporting a rival empire raises the rival's Heat too

**Heat determines the current Retaliation Stage (Section 3):**
Heat 0-19:   Stage 0 (Unnoticed)
Heat 20-39:  Stage 1 (The Letter)
Heat 40-59:  Stage 2 (The Bounty)
Heat 60-74:  Stage 3 (The Counter-Attack)
Heat 75-89:  Stage 4 (The Hostages)
Heat 90-100: Stage 5 (Scorched Earth)

### 2.2 Momentum Phase Caps

**Cross-Reference:** war_state_system.md Section 5

Retaliation cannot exceed the current momentum phase's cap:
- **Phase Alpha (Ch 4-6):** capped at Stage 2 — the war is cold, reprisals are limited
- **Phase Beta (Ch 7-8):** capped at Stage 4 — open war enables serious retaliation
- **Phase Gamma (Ch 9-10):** Stage 5 unlocked — the empires fight for survival

This gates the darkest content to the story's later acts and prevents early-game overwhelm.

### 2.3 Heat Decay

Heat decays slowly over time (passive, per chapter transition) IF Jake stops attacking that faction. Walking away cools the vendetta gradually. But active de-escalation (Section 9) is faster and more reliable.

### 2.4 The Two-Empire Complication

Jake can be at Stage 4 with the Empire and Stage 1 with Albion simultaneously. Supporting one empire against the other means the SUPPORTED empire's Heat cools (gratitude) while the ATTACKED empire's Heat climbs. Playing both sides means managing two vendettas.

---

## 3. THE FIVE ESCALATION STAGES

### 3.1 STAGE 0 — UNNOTICED (Heat 0-19)

Jake's activities are beneath notice or attributed to general piracy. No named response. The calm before.

### 3.2 STAGE 1 — THE LETTER (Heat 20-39)

**The empire formally notices Jake. A letter arrives.**

**Delivery methods:**
- Handed over by a harbor master
- Left with a tavern keeper (cross-reference tavern_keepers.md)
- Nailed to La Liberación's mast (found on return to ship)
- Passed by a nervous messenger

**IMPERIAL LETTER — High register, wounded authority:**

*"Captain Harlow,*

*The Crown has taken note of your recent activities against Imperial interests in these waters. Such actions do not go unobserved, nor unremembered. You are advised, in the strongest terms, to cease. The Empire's patience is considerable. It is not infinite.*

*Consider this your single courtesy.*

*— By order of the Colonial Authority, Puerto de la Corona"*

**ALBION LETTER — the invoice, ice-cold and itemized:**

*"Captain Harlow,*

*Per the attached schedule of damages incurred by the Honourable Archipelago Company through your documented actions:*

*— One (1) supply convoy, cargo and vessel: £4,200*
*— Disruption of trade at Port Sterling, est.: £1,800*
*— Sundry losses and administrative costs: £600*

*Total outstanding: £6,600*

*Settlement is expected within one quarter. Non-payment will result in escalation of collection efforts. The Company does not forgive debts. It collects them.*

*— Office of the Lord-Director, New Coventry"*

**Design Note:** The letters are COLLECTIBLES. Players keep them (cross-reference the_named_dead.md collectible-dread precedent — like the retaliation letters mentioned in colonial_war_overview.md). A wall of accumulating threats becomes a trophy of Jake's war. The Albion invoice specifically — a bounty delivered as accounting — is Style C gold (Pillar 4).

### 3.3 STAGE 2 — THE BOUNTY (Heat 40-59)

**Named assassins are activated.**

**Cross-Reference:** colonial_war/assassin_network.md (full assassin designs)

- The empire places a bounty; named hunters take the contract
- Odyssey-mercenary-style: each assassin has a name, personality, weapon style, approach
- **Imperial assassins:** La Sombra Cofradía (the Shadow Brotherhood — church-blessed killers, fanatical, theatrical)
- **Albion assassins:** Company Regulators (contract-precise professionals who NOTIFY Jake of his own bounty in writing before hunting — bureaucratic menace)

**The Tracking Mechanic (Gossip Web):**
- Cross-reference regional_populations.md Section 10.3
- Assassins find Jake through his fame — the gossip web that spreads his deeds is their intelligence network
- The more notorious Jake is, the easier he is to hunt
- Jake can go quiet (low-profile play) to evade, or lean in (they come to him — an opportunity to eliminate them)

**Encounter Texture:**
- Assassins appear in ports, ambush on roads, board during voyages, wait in taverns
- Telegraphed (a warning, a watching figure, a hush in the crowd) — Style Bible restraint
- Each is a boss-tier fight (colonial_war_bosses.md Tier 3)

### 3.4 STAGE 3 — THE COUNTER-ATTACK (Heat 60-74)

**The empire strikes Jake's ASSETS militarily.**

**Cross-Reference:** colony_system_core.md Section 6 (Jake's settlements), physics/destruction_physics.md

**Targets:**
- Jake's trade posts (raided, goods stolen, buildings damaged)
- Jake's allied convoys (his supported routes attacked)
- Jake's castle holdings (besieged — a defensive set-piece, castle_sieges.md)
- Alliance networks Jake built (cells raided, LS reduced)

**Mechanics:**
- Attacks are TELEGRAPHED (intelligence warns Jake: *"An Imperial fleet was seen provisioning for a strike east..."*)
- Jake can DEFEND (rush to protect the target — a mission) or ACCEPT THE LOSS (can't be everywhere)
- Damage PERSISTS (destruction_physics.md) — a raided trade post stays damaged until rebuilt
- Successful defense: Heat doesn't drop but Jake's holdings survive + reputation

**Design Note — The Impossible Choice:** With multiple holdings, Jake can't defend them all. The empire may strike two places at once. Choosing what to save (and what to lose) is the counter-attack's teeth.

### 3.5 STAGE 4 — THE HOSTAGES (Heat 75-89)

**The empire takes people Jake knows.**

**This is the retaliation system's cruelest and most sophisticated stage. Cross-reference npcs/the_named_dead.md, npcs/npc_recurrence_system.md.**

**Hostage Selection Algorithm (Section 4 — full detail):**
- The system selects from NPCs with HIGH Jake-interaction history
- NOT core crew (protected by narrative — Mira, Big Tom, Ines, Salvio, Esteban are never hostages)
- CANDIDATES: recurring quest givers, favored merchants, befriended NPCs, settlement residents Jake delivered
- The more Jake invested in an NPC, the more likely they're targeted

**The Hostage Event:**
- Notification arrives (a letter, a witness, a ransom demand)
- **Imperial:** demands cessation + tribute, framed as justice
- **Albion:** demands debt payment + "collateral held pending settlement" — hostages as accounting
- A RESCUE MISSION becomes available (timed, high-stakes)

**Rescue Outcomes:**
- **Success:** hostage freed, relationship deepened, Heat unchanged but a victory
- **Failure/Ignored:** the hostage is lost — **permanently** (the_named_dead.md grows). The NPC dies or is broken. Their absence persists in the world.
- **The Cost:** a favored merchant Jake ignored to save another is simply... gone on the next visit. Their shop boarded. The world quieter.

**Design Note — Why Not Crew:** Core crew are narrative-protected because their arcs are authored (named_crew_deep_dives.md). But the SEMI-named web — the merchant Jake always visited, the family he relocated, the quest-giver he helped across chapters — those are fair game. The player's own attachments, formed organically, become the weapon. This is the most emotionally dangerous system in the game and must be handled with Style D weight (Pillar 3).

### 3.6 STAGE 5 — SCORCHED EARTH (Heat 90-100, Phase Gamma only)

**The desperate empire abandons restraint.**

**Cross-Reference:** colony_system_core.md (Jake's settlements), fauna sustainability systems, bosses/crimson_armada.md

**The Scorched Earth Tools:**

**Settlement Burning:**
- Settlements Jake built are BURNED (not just raided — destroyed)
- Cross-reference physics/fire_physics.md, destruction_physics.md
- The people Jake settled there: killed, scattered, or made refugees (the refugee-transport loop, colony_system_core.md Section 3.3)
- A settlement Jake spent chapters building can be a smoking ruin

**Regional Devastation:**
- Regions Jake protects are raided en masse
- Liberated zones assaulted to reconquer (war_state_system.md reconquest)
- Building evolution slammed into decay states

**Ecological Salting:**
- Cross-reference fauna sustainability systems
- Animal populations Jake preserved: slaughtered
- The empire salts what Jake grew — a pointed cruelty
- Fishing grounds, protected herds, cultivated groves (Ethan's Grove is narratively protected, but OTHER groves are not)

**THE CRIMSON TOLERANCE LEVER (the darkest):**
- The desperate empire grants the Crimson Armada TOLERANCE to operate against Jake
- Cross-reference bosses/crimson_armada.md
- Crimson Threat surges region-wide (war_state_system.md CT +15)
- **The main antagonist grows stronger BECAUSE of Jake's colonial war**
- The empire would rather feed the monster than let Jake win — and Jake's endgame gets harder
- **This is the war feeding the climax made literal**

**Design Note:** Stage 5 is Phase Gamma only — it's the endgame's desperation. It should feel like the empire choosing mutual destruction over defeat. The Crimson tolerance especially reframes the whole war: Jake realizes his campaign against the empires may have armed his true enemy. The Style Bible's revenge theme reaches its darkest question — what did my war cost, and who did it serve?

---

## 4. THE HOSTAGE SELECTION ALGORITHM (Deep Detail)

### 4.1 The Candidate Pool

**Cross-Reference:** npcs/recurring_quest_givers.md, npcs/general_merchants.md, npcs/children_and_families.md, all Geschäft/Population Tier NPCs

Every non-crew named NPC carries an **Attachment Score** — a hidden value tracking Jake's investment:
- Quests completed for/with them
- Times traded with / visited
- Gifts, favors, relationship milestones
- Whether Jake delivered/settled them
- Cross-document connections (an NPC linked to others weighs heavier — losing them ripples)

### 4.2 The Selection Logic

When Stage 4 triggers:
1. Filter to NPCs in regions where the retaliating faction has reach
2. Weight by Attachment Score (high investment = high risk)
3. Exclude narrative-protected NPCs (core crew, the four sages, key plot figures)
4. Prefer NPCs whose loss creates maximum RIPPLE (connected NPCs, quest-givers mid-arc)
5. Select 1 (or 2 for simultaneous impossible-choice events)

### 4.3 The Ethical Design Guardrails

**Cross-Reference:** children_and_families.md protection rules

- **Children are NEVER hostages** (engine-level protection extends here absolutely)
- Hostage THREAT to families is shown through the adults, never children in peril
- The loss is handled Style D — off-screen, reverent, never exploitative
- No torture depiction, no suffering spectacle — the WEIGHT is in the absence, the boarded shop, the empty chair (the_named_dead.md aesthetic)

### 4.4 Why This System Is Worth the Risk

This is deliberately the game's most dangerous emotional system. Its justification: it makes Jake's connections MATTER mechanically. In most games, befriending NPCs is consequence-free flavor. Here, every attachment is real — because it can be threatened. The player who invested in the world has something to lose. That investment, and its vulnerability, is the point of Phase 5's entire relationship web. The dead drive the living (the_named_dead.md Principle 1) — and now the living-who-might-die drive the player.

---

## 5. FACTION-FLAVORED RETALIATION SUMMARY

### 5.1 The Empire (Spanish-Coded)

- **Tone:** wounded authority, aristocratic pride, church-backed righteousness
- **Letters:** formal, High register, "the Crown's patience"
- **Assassins:** La Sombra Cofradía — fanatical, theatrical, church-blessed
- **Logic:** justice and honor; they retaliate to punish disrespect
- **De-escalation:** appeals to honor, aristocratic intermediaries (Margarethe, Don Alejandro), formal apology/tribute

### 5.2 The Crown of Albion (English-Coded)

- **Tone:** cold accounting, contract enforcement, mercantile menace
- **Letters:** itemized invoices, debts in pounds, "the Company collects"
- **Assassins:** Company Regulators — precise professionals, invoice-first, bureaucratic
- **Logic:** profit and loss; they retaliate to recover costs and deter future losses
- **De-escalation:** PAYMENT (Albion genuinely respects settling the debt), contract renegotiation, HAC intermediaries

### 5.3 The Design Contrast

The two empires' retaliation is a character study. The Empire is offended; Albion is charging interest. The Empire wants respect; Albion wants payment. Fighting the Empire feels like a blood feud; fighting Albion feels like being pursued by a collections agency with cannons. Both are terrifying differently.

---

## 6. THE COUNTER-ATTACK & DEFENSE LAYER

### 6.1 Intelligence Warnings

Cross-reference imperial_contacts.md (Whisper Lady, Helga Brennan), npcs/recurring_quest_givers.md (Salazar, information brokers). Jake can BUY or EARN advance warning of counter-attacks and hostage attempts. Intelligence turns retaliation from ambush into a defensible threat. Information brokers become strategically vital under high Heat.

### 6.2 Defensive Set-Pieces

- **Settlement defense:** repel a raid on Jake's holding (combat + protecting NPCs)
- **Castle defense:** withstand a siege on Jake's castle (castle_sieges.md, defensive variant)
- **Convoy defense:** protect an allied convoy from a reprisal strike
- **Rescue missions:** the hostage-recovery operations (Stage 4)

### 6.3 The Fortification Investment

Cross-reference castle_ownership.md. Jake can invest in defending his holdings — garrisons, walls, warning networks. Well-defended settlements survive counter-attacks; neglected ones burn. The retaliation system rewards the player who builds defensively, punishes the one who over-extends.

---

## 7. DE-ESCALATION PATHS

### 7.1 Pay the Debt (Albion-Specific)

Albion's HAC genuinely respects payment. Settling the invoice (Stage 1) or paying ransom/damages (later stages) cools Albion Heat reliably. The Company would rather be paid than pursue. **This is a real, if expensive, off-ramp.** The Empire is less amenable to mere money — its pride wants more.

### 7.2 Broker Peace (Diplomatic)

**Cross-Reference:** imperial_contacts.md (Margarethe, Diplomat Marcus Aurelius, Lord Hartwell)

- Margarethe can intercede with Imperial authority (romance/friendship leveraged)
- Diplomat Aurelius brokers inter-faction terms
- Reform-faction contacts (Kessler, Voss) can vouch for Jake
- Formal peace reduces Heat significantly — but costs favors, reputation, or concessions

### 7.3 Destroy the Local Cell (Aggressive)

Cross-reference assassin_network.md, assassin_network_questline.md. Jake can go on OFFENSE against retaliation — trace and destroy the assassin network's local cell, eliminate the officer coordinating reprisals, burn the bounty records. Aggressive de-escalation: violence to buy peace. Feeds directly into the assassin questline.

### 7.4 Win So Hard They Sue for Terms

At extreme Liberation success, a broken empire may seek terms with Jake to avoid total collapse in a region. The ultimate de-escalation: not appeasement but victory. The empire retaliates less because it's LOSING and needs to cut deals. Power as peace.

### 7.5 Go Quiet (Passive)

Simply stopping attacks on a faction lets Heat decay over chapters (Section 2.3). Low-profile play (avoiding the gossip web's amplification) speeds evasion of active hunters. The patient player can let a vendetta cool — though the empire remembers.

---

## 8. THE RETALIATION-AS-CONTENT PHILOSOPHY

### 8.1 Retaliation Generates Gameplay

Every stage produces missions: assassin fights, defensive set-pieces, rescue operations, de-escalation quests, network raids. Retaliation is not just punishment — it's a CONTENT ENGINE that responds to player aggression with tailored challenges. The more Jake breaks, the more the world fights back, the more there is to do.

### 8.2 The Dread Loop

The telegraphed escalation creates a signature emotional loop: land a blow → feel the Heat rise → receive the warning → brace for the response → survive it or pay the cost → decide whether to escalate or cool. This loop, repeated and deepening, is the heartbeat of the aggressive playstyle.

### 8.3 The Restraint Reward

Players who play carefully — supporting more than breaking, de-escalating wisely, defending their holdings — experience a different game: slower, safer, building rather than burning. Retaliation makes RESTRAINT a valid, rewarded playstyle. The system doesn't punish aggression per se; it prices it. Some players will pay gladly. Others will build empires of trade and never see Stage 5. Both are correct.

---

## 9. INTEGRATION MAP

### 9.1 Systems This Reads and Feeds

- **war_state_system.md** ← Break actions generate Heat; Stage 5 feeds Crimson Threat
- **colony_system_core.md** ← Jake's settlements and supported convoys are counter-attack targets
- **assassin_network.md / questline** ← Stage 2 bounties; aggressive de-escalation
- **castle_sieges.md** ← counter-attack and defensive sieges
- **the_named_dead.md** ← hostage losses grow the named dead
- **npc_recurrence_system.md** ← hostage selection, persistent consequences, the changed world
- **regional_populations.md** ← gossip web (assassin tracking), scorched-earth demographics
- **children_and_families.md** ← protection guardrails, refugee generation
- **imperial_contacts.md** ← de-escalation diplomacy, intelligence
- **destruction_physics.md** ← persistent counter-attack and scorched-earth damage
- **bosses/crimson_armada.md** ← the tolerance lever, the war feeding the climax
- **the four sages** ← witnesses to what Jake's war costs

### 9.2 The Central Feedback Loop

Retaliation closes the war's core loop: **Jake acts → the world responds → the response is personal → Jake must reckon with the cost.** Without retaliation, breaking colonies is consequence-free. With it, every act of war has a price measured in hunters, holdings, friends, and — at the extreme — the strength of Jake's true enemy.

---

## 10. MVP CORE vs FULL VISION

### 10.1 MVP CORE

- The Heat meter (single value per faction) and the five-stage ladder
- Stages 1-3 fully functional (Letter, Bounty, Counter-Attack)
- Faction-flavored letters (Imperial threat, Albion invoice)
- 2-3 named assassins (Stage 2)
- Counter-attacks on one Jake settlement (Stage 3)
- Basic de-escalation (pay Albion, broker via Margarethe)
- Momentum-phase caps

### 10.2 FULL VISION

- Stage 4 Hostages with the full selection algorithm and the relationship web
- Stage 5 Scorched Earth with settlement burning, ecological salting, and the Crimson tolerance lever
- The full named-assassin roster (assassin_network.md)
- Intelligence economy (buying warnings)
- All de-escalation paths including "sue for terms"
- The impossible-choice simultaneous events
- Deep integration with the four sages' reactions

---

## 11. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** The two empires' retaliation is a register study — Imperial High wounded authority vs Albion cold mercantile accounting. The letters are register made weaponized.
- ✅ **Pillar 2 (Lore Through Objects):** The retaliation letters as collectibles. The Albion invoice as artifact. The boarded shop of a lost hostage. The smoking ruin of a burned settlement. Consequence made physical.
- ✅ **Pillar 3 (Layered Death):** Hostage losses are Style D always — off-screen, reverent, the weight in absence. Scorched earth is Style D devastation. The named dead grow. Never exploitative spectacle.
- ✅ **Pillar 4 (Layered Humor):** The Albion invoice-assassins — a bounty delivered as accounting — are Style C gold. Sailor gallows-humor about the empires' pettiness. Never mocking the victims.
- ✅ **Pillar 5 (Theme — Revenge):** Retaliation IS the theme turned back on Jake. His war has a price. His connections are vulnerabilities. And at the extreme, his campaign may have fed his true enemy — the revenge theme's darkest question: what did it cost, and who did it serve?

---

## 12. NEXT STEPS

**FOUNDATION TIER COMPLETE.**

Next document: **`colonial_war/imperial_colonies.md`** — Colony Tier Document 1

Will cover the five Imperial colonies as individual character portraits:
- Puerto de la Corona, Nueva Esperanza, Fuerte San Rafael, Bahía Dorada, Puesto del Norte
- Each with its General/Governor, economy, military character, weakness, population mood, castle, and liberation scenario

After Imperial Colonies: Albion Colonies, then the Liberation Alliance. Then the Castle Tier.

---

## 13. CONCLUSION

When the Empires Strike Back. The letter that arrives nailed to the mast — the Empire's wounded High-register warning, or Albion's ice-cold itemized invoice charging Jake for the convoy he sank. The named assassins who track him through his own fame. The counter-attacks on the trade posts and settlements he built, forcing the impossible choice of what to save. The hostages — the merchant who always greeted him, the family he relocated, taken because he dared to invest in them — and the permanent absence when he can't save them all. And at the desperate end, scorched earth: the burned settlements, the salted groves, and the darkest lever of all — a dying empire feeding the Crimson Armada tolerance, so that Jake's war against the colonizers strengthens the monster he truly hunts.

When a player receives the Albion invoice and laughs, then stops laughing when the Regulators arrive; when they lose a merchant they'd visited for twenty chapters to a hostage-taking they couldn't reach in time; when they realize in the endgame that breaking every colony they could just handed the Crimson Armada the archipelago — that's when retaliation stops being a punishment system. That's when it becomes the war's conscience, asking Jake the question the whole game asks: what does your vengeance cost, and who pays it?

**Phase 7 Subsystem 4 of 16 — complete.**

**FOUNDATION TIER COMPLETE.**

**The letters arrive. The hunters come. The holdings burn. The war strikes back.** 🔥📜
