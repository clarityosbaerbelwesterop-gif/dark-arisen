# KRONE COLONIES — THE COMPANY'S HOLDINGS

**Status:** Design Draft v1.0  
**Phase:** Phase 7 — Colonial War, Castles & Dungeons (Colony Tier Document 2 — Document 6 of 16)  
**Tier:** Colony Tier  
**Related:** colonial_war_overview.md, colonial_war/imperial_colonies.md, colonial_war/war_state_system.md, colonial_war/colony_system_core.md, colonial_war/retaliation_system.md, colonial_war/colonial_war_bosses.md, npcs/regional_populations.md, npcs/recurring_quest_givers.md, npcs/desert_sage_mbah_seruni.md, bosses/crimson_armada.md, style_bible.md  
**Purpose:** Complete individual character portraits of the four Crown of Albion colonies — the Honourable Archipelago Company's holdings, their Directors and officers, the Profitability Threshold withdrawal system, debt-labor architecture, the Sterling alliance question, and the Support/Break/Liberation scenarios unique to each.  
**Inspirations:** Assassin's Creed Odyssey (rival faction identity), the historical East India Company (fictionalized), Black Sails (colonial commerce), Master and Commander (naval professionalism), The Wire (institutional logic as antagonist), real debt-peonage history

---

## 1. DESIGN PHILOSOPHY — THE PAPER POWER

### 1.1 The Core Contrast

**Cross-reference:** colonial_war/imperial_colonies.md Section 2

The Empire took with the sword, then the cross, then the tax. **Albion takes with the treaty, then the debt, then the foreclosure.** Every design decision in this document serves that distinction.

Where the Empire's colonies are old, proud, and rooted, Albion's are new, hungry, and *contingent*. The Empire holds ground because letting go is dishonor. **Albion holds ground because it pays.**

### 1.2 The Foundational Insight — Albion Can Be Made to Leave

This is the single most important mechanical difference in Phase 7.

An Imperial colony must be broken: starved, stormed, or risen against. It will never withdraw, because Herrera and Vega would rather die at their posts than write the letter admitting failure.

**An Albion colony can be made unprofitable.** The Honourable Archipelago Company answers to shareholders in a distant capital who have never seen the archipelago and care only about the return. Drive a holding's profitability below the threshold for long enough, and the Company votes to withdraw. The flag comes down. The marines board the transports. Nobody dies in the final act.

**This creates an entirely distinct victory condition: economic warfare as conquest.** (Full system: Section 3.)

### 1.3 The Four Silhouettes

- **New Coventry** — the ledger-fortress where the Company's true weapon is filing
- **Port Sterling** — the naval bastion commanded by the one enemy Jake might rather have as a friend
- **Blackwood Landing** — the plantation where nobody is a slave and everybody is owned
- **Fort Resolute** — the young colonel's career ladder, planted in Fjordlund's ice

### 1.4 The Design Warning — Albion Is Not the Good Empire

**Cross-reference:** npcs/desert_sage_mbah_seruni.md, npcs/regional_populations.md Section 4.2

Mbah Seruni's line, established in Phase 5 and load-bearing here:

*"...they take our land with paper now instead of swords. Paper cuts slower but deeper..."*

Albion is more polite, more modern, more legally scrupulous, and takes MORE. Its colonies have less visible brutality and more total extraction. A player who breaks the Empire and then feels comfortable with Albion has been fooled exactly as the design intends — until Blackwood Landing shows them what a contract can do that a chain cannot.

**Neither empire is the good one. They are two grammars of taking.**

---

## 2. THE HONOURABLE ARCHIPELAGO COMPANY

### 2.1 Structure

**The HAC is a chartered corporation with sovereign powers:**
- Its own army (Company marines — professional, well-paid, contract-bound)
- Its own navy (supplemented by Royal Navy squadrons)
- Its own courts (Company law adjudicated by Company magistrates)
- Its own currency instruments (Company notes circulate as money)

**Above it:** a distant Crown that chartered it and a shareholder assembly that owns it. **The Crown reigns. The Company rules.**

### 2.2 The Doctrine — Treaty Before Sword

The HAC's standard acquisition sequence:

1. **The Treaty** — arrive with gifts, sign an agreement of "mutual protection and preferred trade." The terms are generous. The clauses are not read closely.
2. **The Debt** — extend credit for infrastructure, goods, protection. Interest accrues. The obligation compounds.
3. **The Foreclosure** — collect. Land, labor, sovereignty, whatever the contract specifies. Legally. Politely. Completely.

**Design note:** The HAC has never needed to invade anywhere. Every Albion holding was *acquired*, and every acquisition is documented, signed, and enforceable. This is what makes it worse.

### 2.3 Military Character

- **Strength:** fast frigates, superior gunnery drill, coordinated escort doctrine, professional marines
- **Strength:** economic warfare (they will ruin a rival's trade before firing a shot)
- **Weakness:** thin garrisons (marines cost money; the Company economizes)
- **Weakness:** **profit-driven retreat logic** — the Company abandons what does not pay (Section 3)
- **Weakness:** contract-bound loyalty. Company marines fight for wages. Wages can be disrupted.

---

## 3. THE PROFITABILITY THRESHOLD — ECONOMIC CONQUEST

### 3.1 The System

**Cross-reference:** war_state_system.md Section 2.4 (M/E/P decomposition)

Every Albion colony carries an additional hidden value beyond the standard Control meters: **Profitability (0-100)**.
Profitability ≈ (Economic pillar) − (Garrison upkeep) − (Losses incurred)

**What lowers Profitability:**
- Convoy raiding (direct revenue loss)
- Blockades (trade denial)
- Destroying warehouses and cargo
- Forcing garrison reinforcement (upkeep rises)
- Sabotaging plantations and production
- Burning the ledgers (Section 4.8 — administrative collapse)

**What raises it:**
- Support actions (escorts, deliveries, funding)
- Successful Company operations Jake enables
- Suppressing local unrest cheaply

### 3.2 The Withdrawal Vote

**When Profitability falls below 25 and stays there for a sustained period (two chapter-transitions, or one with severe losses):**

**THE SHAREHOLDER REVIEW triggers.**

- A Company auditor arrives from the distant capital (a named minor NPC, blackly comic — cross-reference Style Bible Pillar 4)
- The colony's books are examined. Its officers plead. Its Director explains.
- **The vote happens off-screen, in a boardroom Jake will never see, by men who could not name the colony.**
- Result: **WITHDRAWAL.** The flag comes down. The marines board transports. The warehouses are emptied of anything portable.

### 3.3 Why Withdrawal Is Different from The Fall

**Cross-reference:** war_state_system.md Section 4.5-4.6

**The Fall** (Imperial-style) is a battle: the general defeated, the fort stormed, blood and fire.

**Withdrawal** is an evacuation: orderly, bureaucratic, almost anticlimactic — and then the Fork still applies (war_state_system.md Section 4.6). What fills the space matters exactly as much. A withdrawn Albion colony with LS below 50 becomes a Vacuum just as surely as a stormed Imperial one.

**The design payoff:** withdrawal is a *bloodless* conquest available only against Albion. A player who wants to win the archipelago without massacres has a path — and it runs through the account books. Cross-reference the crew: Father Salvio notices when Jake wins a region without killing anyone.

### 3.4 The Cruelty in the Comedy

Withdrawal has a dark edge the design should not soften: **the Company leaves its debtors behind.** The contracts remain enforceable. The people who owe money to a Company that no longer maintains a presence are still, legally, in debt — to nobody who will come collect and nobody who will forgive.

Cross-reference colony_system_core.md — this is what makes the Liberation step necessary even after a bloodless victory. Withdrawal frees the land. It does not cancel the paper. **Someone has to burn the paper.**

---

## 4. NEW COVENTRY — THE LEDGER FORTRESS

### 4.1 Quick Reference

**Region:** Coastal, adjacent to the Quiet Coast  
**Type:** Administrative headquarters (HAC archipelago seat)  
**Size:** Colony (growth stage 4, expanding fast)  
**Economy:** Administration, banking, contract enforcement, the debt portfolio for the entire archipelago  
**Castle:** **Coventry House** — a trade citadel: warehouse-fortress, counting halls, thin walls, fat vaults  
**Commander:** Lord-Director Edmund Blackwood  
**Profitability:** 84 (the Company's healthiest holding)  
**Starting Control:** AC 79, IC 4, LS 9, CT 8

### 4.2 History

Founded 1858 — **eighteen years old.** The newest major settlement in the archipelago and already the second-richest.

It was not conquered. The HAC signed a treaty with three Quiet Coast settlement councils for harbor rights and "mutual development." Eighteen years later the Company owns the harbor, holds paper on two of the three councils, and New Coventry has grown from a wharf into a city while the settlements that signed the treaty have grown into debtors.

**Nobody fought. Everybody lost.** Cross-reference npcs/recurring_quest_givers.md Section 6.1 — Sheriff Whitaker was at one of those signings. He has thoughts.

### 4.3 The Director — Edmund Blackwood

**Full boss design:** colonial_war_bosses.md Tier 1 (unusual: an economic boss)

**Name:** Lord-Director Edmund Blackwood  
**Age:** 51  
**Register:** High (educated, precise, genuinely warm)

**Character:**

Not a coward, not a hypocrite, and not corrupt — which makes him the most dangerous man in the archipelago.

Blackwood is a **true believer**. He holds, sincerely and with evidence he will happily present, that commerce is civilization's engine and that the Company brings order, medicine, literacy, and prosperity where empires brought only extraction. He is not lying. New Coventry has a hospital, a school, clean water, and a lower mortality rate than Puerto de la Corona.

It also has a debt portfolio that owns three settlement councils, four thousand indentured contracts, and the legal right to foreclose on half the coast.

He sees no contradiction. He would explain, patiently and at length, why there isn't one.

**Voice Samples:**

*"Captain Harlow. Sit. You've come to accuse me of something and I should like to hear it properly phrased — most people arrive with feeling and no argument. Tea?"*

*"The Empire hanged forty men in Puerto de la Corona last year. I have hanged none. I have never burned a village. I have never cleared a shrine. Every acre this Company holds was signed over by its owner in the presence of witnesses. And you look at me as though I were the worse thing. I am genuinely curious as to why."*

*"You say I own them. I say they entered into an agreement. You say they did not understand the agreement. I say — and Captain, do consider this — that if a man's ignorance voids his contracts, then he cannot own anything at all, and you have argued him into being property. I have not. The Company has never held a slave. Read the charter."*

*"Everything is written down. That is not my crime. That is my defence."*

**His Blind Spot:**

Blackwood believes consent obtained through asymmetry of understanding is still consent. He has built a civilizational argument on that single premise and never examined it, because examining it would require him to be a different man in a different life. He is the paper power given a face, and the face is kind.

**The Boss Fight (design note):**

Blackwood does not fight. When New Coventry falls, he attempts to **leave with the master ledger** — the consolidated debt portfolio for the entire archipelago. The encounter is a pursuit and a choice: stop him, take the ledger, or let him go and let the paper go with him. His Company Regulator bodyguards fight. He does not. He negotiates the entire way. He is still negotiating when it ends, however it ends.

### 4.4 Garrison Character

- **Strength:** Company Regulators (elite, contract-precise — cross-reference retaliation_system.md, assassin_network.md), well-drilled marines
- **Doctrine:** deterrence, legal enforcement, rapid response to *commercial* threats above military ones
- **Weakness:** **it is a counting house with walls.** Coventry House was built to store value and process paper, not to withstand a siege. The garrison is competent and thin.

### 4.5 Economy & Supply Lines

**Three supply lines:**
1. **The Home Run** — traffic to and from distant Albion (dividends out, orders and personnel in). Raiding this hits Profitability directly and *visibly to the shareholders*.
2. **The Collection Circuit** — the routes Company enforcers travel to collect debts across the region. **Raiding this is a Liberation action:** every intercepted collection is a debt uncollected.
3. **The Coastal Trade Line** — legitimate commerce, genuinely prosperous, and the source of the Company's civilizational argument.

### 4.6 Population & Demographics

- **The Company Quarter:** clerks, factors, Company officers. Modern, orderly, quietly ambitious. Register: precise Middle-High.
- **The New Town:** genuine prosperity — tradesmen, professionals, immigrants who came for the schools and hospital and got them
- **The Indentured Districts:** contract-labor housing. Legally free residents who cannot leave until their obligation clears. Clean, dry, adequate, and inescapable.
- **The Quiet Coast Fringe:** settlers from the old settlements, in New Coventry because their own towns owe the Company

**Ambient — Company Quarter:**

*"...quarterly returns are up eleven percent. Eleven! And the Director credits the new collection schedule..."*

**Ambient — New Town:**

*"...say what you like about the Company, my boy reads and my daughter lived through the fever. The Empire gave us neither..."*

**Ambient — Indentured District:**

*"...four years on the contract. Two to go. Then three more because of the tools they charged me for. Then — I stopped counting. It doesn't get shorter..."*

**Design note:** The New Town ambient is essential. **New Coventry must be genuinely attractive.** A player who feels no pull toward Albion has not been given the real choice.

### 4.7 The Castle — Coventry House

**Full design:** castle_catalog.md

- Trade citadel: warehouse levels, counting halls, the Director's residence, Regulator barracks
- Purpose-built for value storage and document security, not warfare
- **State Treasure:** the Company treasury (enormous) + **THE MASTER LEDGER** — the consolidated debt portfolio: every contract, every obligation, every foreclosure right in the archipelago, in one bound volume with copies at three removes

### 4.8 The Weakness — Burn the Right Ledger

**Cross-reference:** colonial_war_overview.md Section 7.3

New Coventry runs on paper. Its army is filing. Its conquests are clauses.

**Three attack vectors:**
- **Take the Master Ledger** — the debts become unenforceable in practice; three settlement councils walk free; P collapses across the region
- **Burn it** (cross-reference physics/fire_physics.md) — faster, cruder, and it destroys the record of what was owed *to* people as well as *by* them, which has costs
- **Publish it** — the Full Vision option: hand it to a Quiet Coast council, to Kessler's reform faction, to anyone. The Company's legal architecture becomes public, and Blackwood's civilizational argument dies in the open air.

**And the Profitability route:** New Coventry is the Company's most profitable holding, which means it is the *hardest* to drive to withdrawal — but if it goes, the entire HAC archipelago operation loses its administrative heart.

### 4.9 Support Scenario

Supporting New Coventry is the most *comfortable* Support content in the game. The work is clean: escort legitimate trade, deliver settlers into a growing city with schools, invest in genuine prosperity.

**And every voyage strengthens the collection circuit.** The design does not point this out. The Indentured District ambient does.

**Growth looks like:** the best-looking growth in the game. New buildings that are actually good — a wing on the hospital, a second school, a proper aqueduct. And the Indentured Districts expanding at the same rate.

**Reward tier:** HAC trade contracts, Company base rights, Albion warship refits (speed-and-gunnery), the New Coventry villa (modern, elegant, comfortable).

### 4.10 Break Scenario

- **Economic:** raid the Home Run and the Coastal Trade Line — drive Profitability toward the threshold
- **Legal:** take or destroy the Master Ledger — the single highest-leverage act available against Albion anywhere
- **Military:** Coventry House folds faster than any Imperial fort, but the Regulators are the hardest garrison troops in the game
- **Blackwood:** the pursuit, the ledger, the negotiation that never stops

### 4.11 Liberation Scenario — "Burn the Paper"

**What freedom looks like HERE:**

Not an uprising — there is no oppressed majority with a generational wound. New Coventry's liberation is **legal**: the ledger destroyed or published, the contracts voided, the settlement councils' debts erased, the indentured released by the simple fact that nobody can prove what they owe.

**The liberated New Coventry:** the city *stays*. The hospital stays, the school stays, the aqueduct stays. What leaves is the Company's ownership of the coast. It becomes a free port that keeps everything the Company built and none of the Company's claims — the strangest and gentlest liberation in the game.

**Cross-reference:** the Quiet Coast's culture (npcs/regional_populations.md Section 7.3 — *"deeds only"*). Sheriff Whitaker, who was at the signing, gets to watch the paper burn.

**The cost:** Blackwood is right about one thing, and the design must let him be right about it — some of what the Company built was good, and it was built with money that came from the contracts. Erasing the debt does not un-build the hospital, but it does end the funding. **There is no version of this where nothing is lost.**

### 4.12 Cross-References

Sheriff James Whitaker (was at the treaty signing) · Old Pete Henderson, Mama Henderson, Pete Junior (Quiet Coast — their settlement's council may be one of the indebted three) · Doc Wallace (the Company hospital undercuts him, and he has complicated feelings) · the settler family network · Cassandra "Cass" Wells (has Company intel) · Mateusz Voltari (the ledger has a fence value nobody should pay) · Inspector Kessler (an Imperial reformer who would find the ledger *fascinating*)

---

## 5. PORT STERLING — THE HONOURABLE ENEMY

### 5.1 Quick Reference

**Region:** Central archipelago, strategic waters  
**Type:** Royal Navy base / naval colony  
**Size:** Town (growth stage 3, militarily dense)  
**Economy:** Naval logistics, shipbuilding, provisioning — the Company subsidizes it  
**Castle:** **The Sterling Bastion** — a naval fortress, harbor-locked, fleet-supported  
**Commander:** Admiral Katherine Sterling  
**Profitability:** 41 (it does not pay; it *protects* what pays)  
**Starting Control:** AC 84, IC 6, LS 6, CT 4

### 5.2 History

Established 1864 as the Royal Navy's archipelago station — a *Crown* installation, not a Company one, and the distinction matters enormously.

Sterling's squadron exists to protect Albion shipping and project force. It is the only Albion holding not answerable to the shareholders, which means **Port Sterling cannot be driven to withdrawal.** The Profitability system does not apply. It must be fought or befriended.

### 5.3 The Admiral — Katherine Sterling

**Full boss design:** colonial_war_bosses.md Tier 1 (and the game's most significant potential ally-boss)

**Name:** Admiral Katherine Sterling  
**Age:** 46  
**Register:** Middle-High (naval professional, plain-spoken, precise)

**Character:**

The finest officer in the archipelago and the enemy Jake will most regret fighting.

Sterling is honest, brilliant, personally brave, and holds herself to a standard that has cost her repeatedly. She has twice reported Company abuses to the Crown over the Company's objections. She refuses to escort collection convoys. She has told Blackwood — in writing, on the record — that she considers indenture "slavery with better stationery," and she still protects Albion shipping because that is her commission and she does not get to choose which parts of her duty she likes.

**She is the one senior figure on either side who would treat Jake as an equal.**

**Voice Samples:**

*"Captain Harlow. I have read the reports and discarded most of them. Reports are written by men who need something from me. Tell me yourself, and I will decide what I think."*

*"You've been sinking my convoys. I would do the same in your position. That is not absolution, Captain — I will still hang you if I take you. But I will hang you with respect, and I will not pretend the Company's hands are clean while I do it."*

*"I do not defend indenture. I have said so to the Lord-Director in language he did not enjoy. I defend Albion shipping, because that is my commission, and if I begin selecting which lawful duties suit my conscience, I am no longer an admiral. I am simply a woman with cannons and opinions. The world has enough of those."*

**On the Crimson Armada:**

*"The Armada takes ships flying every flag including mine. If you tell me you are hunting them, Captain, then for that specific purpose and no other, we are on the same side. I want it minuted that this arrangement disgusts me. I also want it to work."*

**Her Weakness — Her Honour:**

Sterling can be *maneuvered by her own standards*. Present her with proof of a Company atrocity and she must act on it, even against her own side. Give her a genuine Crimson Armada threat and she will cooperate with a pirate. Make her choose between her commission and her conscience often enough and something breaks — but she cannot be bribed, frightened, or lied to twice.

### 5.4 The Sterling Alliance (Major System)

**Cross-reference:** npcs/imperial_contacts.md Section 4 (Captain Eleanora Voss — Sterling's Imperial mirror), bosses/crimson_armada.md

**Sterling is a boss who can become an ally.** The path:

1. **Mutual respect** — encounters where Jake behaves honorably (sparing crews, honoring truces, protecting civilians in contested waters)
2. **The Crimson common cause** — joint operations against the Armada, limited and explicitly uncomfortable
3. **The evidence** — bringing Sterling proof of Company abuses (the Master Ledger, Blackwood Landing's contracts, Ashcroft's foreclosure records)
4. **The choice** — late-game, Sterling must decide between her commission and what she now knows

**Outcomes:**
- **Ally:** Sterling's squadron in the endgame. Cross-reference the escort-fleet reward tier, and the Crimson Armada climax
- **Neutral:** an honorable standoff — she will not hunt Jake and Jake will not sink her ships. Both keep the arrangement. Neither is comfortable.
- **Enemy:** the hardest naval boss fight in Phase 7, and the game's most regrettable victory

**Design note:** Sterling and Voss (imperial_contacts.md) are deliberate mirrors — the honorable officer inside each empire. **If Jake befriends both, they can be brought into the same room,** and two professional officers from rival empires discover they agree with each other more than with their own governments. That scene is a Full Vision target.

### 5.5 Garrison & Fleet

- **Strength:** the archipelago's best-drilled naval force; Sterling's squadron is the single most dangerous fleet Jake can face
- **Doctrine:** blue-water dominance, disciplined gunnery, no unnecessary cruelty
- **Weakness:** Sterling's fleet is stronger than her *base*. The Bastion is harbor-locked — break or lure away the squadron and the fortress is a stone box.

### 5.6 Population & Demographics

- **The Naval Town:** sailors, shipwrights, chandlers, naval families. Professional, proud, decent.
- **The Yard:** shipbuilding labor — paid, skilled, unionized in practice if not in law
- **No indentured district.** Sterling would not permit one. Cross-reference: this absence is the loudest statement in the colony.

**Ambient:**

*"...the Admiral came down to the yard herself when Tomkins lost the hand. Stood there while the surgeon worked. You don't forget that..."*

*"...heard she told the Director to his face what she thought of the contracts. In front of clerks. In writing after..."*

### 5.7 The Castle — The Sterling Bastion

- Harbor-locked naval fortress: batteries, dry dock, magazine, the Admiral's quarters (spartan)
- **State Treasure:** naval stores and the fleet's payroll + **Sterling's correspondence** — her complete written record of protests to the Crown against Company practices. The most credible indictment of the HAC in existence, written by Albion's most respected officer. **She has kept copies deliberately.** She wanted a record to exist.

**Design note:** Sterling's correspondence can be *given* rather than stolen. If the alliance path is walked, she hands it over herself. **The Company's greatest vulnerability is in the desk drawer of its own best defender, and she has been waiting for someone to ask.**

### 5.8 The Weakness

**Her honour, and the harbor.** Militarily: separate fleet from fortress. Politically: give her the evidence and let her standards do the work.

### 5.9 Support Scenario

Working for Sterling is the cleanest employment in the game: convoy escorts, anti-piracy patrols, Crimson hunts. She pays fairly, briefs honestly, and does not ask Jake to do anything she would not do.

**And it is Albion's shield.** Every convoy Sterling's squadron protects because Jake helped is a Company cargo delivered.

**Growth looks like:** a better dockyard, a bigger squadron, and a Company that trades more safely because of it.

### 5.10 Break Scenario

- Lure the squadron out, then take the harbor
- Or defeat Sterling at sea — the hardest naval engagement in Phase 7 (colonial_war_bosses.md)
- **No withdrawal path.** Crown installation, not Company. Sterling does not have shareholders. She has orders.
- Cross-reference retaliation_system.md — attacking Port Sterling brings the Royal Navy's response, which is *slower and more thorough* than the Company's

### 5.11 Liberation Scenario — "Nothing to Free, Someone to Convince"

**What freedom looks like HERE:** Port Sterling has no oppressed population. LS starts at 6, the lowest in the game alongside Bahía Dorada.

**The third path is not liberation but conversion.** Port Sterling's contribution to a free archipelago is Sterling herself — her squadron, her testimony, her correspondence. The colony does not need to fall. **It needs to change sides, and the only lever is being worth her respect.**

### 5.12 Cross-References

Captain Eleanora Voss (Imperial mirror — the two-officers scene) · Lt. Marcus Sandford (Imperial defector — he understands Sterling's dilemma from the other side; cross-reference npcs/shippable_specialists.md) · Dr. Eleanor Whitcomb (naval surgeon connections) · the Crimson Armada (common enemy) · Frau Helga Brennan (Imperial spymaster with a thick Sterling file and grudging admiration)

---

## 6. BLACKWOOD LANDING — THE CONTRACT PLANTATION

### 6.1 Quick Reference

**Region:** Fertile coastal lowland  
**Type:** Plantation colony (debt-labor model)  
**Size:** Colony (growth stage 4)  
**Economy:** Sugar, indigo, coffee, hardwood — worked by indentured contract labor  
**Castle:** **Ashcroft Hall** — a fortified estate-compound, more manor than fort  
**Commander:** General Percival Ashcroft  
**Profitability:** 91 (the highest in the game — this is where the money comes from)  
**Starting Control:** AC 68, IC 7, LS 21, CT 4

### 6.2 History

Founded 1861 and named, without irony, for the Lord-Director. Blackwood Landing is the HAC's proof of concept and the archipelago's most profitable acre.

**Nobody here is a slave.** Every worker signed a contract — passage, tools, housing, and provisions advanced against future wages, with interest, plus charges. Every contract is legal, witnessed, and enforceable. Every worker is legally free to leave the moment their obligation clears.

**No obligation has ever cleared.**

**Design note — the deliberate contrast with Nueva Esperanza (imperial_colonies.md Section 4):** the Empire's plantation uses chains and calls it property. Albion's uses paperwork and calls it employment. The suffering is comparable. The legal architecture is completely different. **And Blackwood Landing is more profitable, better fed, healthier, and harder to liberate** — because you cannot break a chain that is a clause.

### 6.3 The General — Percival Ashcroft

**Full boss design:** colonial_war_bosses.md Tier 1

**Name:** General Percival Ashcroft  
**Age:** 55  
**Register:** High (landed manner, Company function)

**Character:**

The foreclosure enforcer, and he enjoys his work in a way Reyes never could — because Ashcroft does not have to manage a moral tension. He has a contract. The contract is clear. He enforces it. There is nothing to feel.

Where Reyes at Nueva Esperanza knows exactly what he is and asks Jake to notice that he knows (imperial_colonies.md Section 4.3), Ashcroft has never had that thought and would find it self-indulgent. He is a landowner-administrator with legal instruments and a Company mandate, and the workers are in breach or they are not.

**He is the least self-aware and most comfortable villain in Phase 7.**

**Voice Samples:**

*"Captain. You've walked the fields, I imagine, and worked yourself into a state. Everyone does. Then someone explains the contracts and they go quiet, and I watch them decide whether to be honest about it. Which will you be?"*

*"Breach is breach. A man signs, a man is advanced goods and passage, a man owes. If he wishes to leave he clears the obligation. The instrument is not cruel, Captain, it is simply *binding*, and you are confusing those things because binding is inconvenient to your feelings."*

*"The Empire's plantations use the whip. Mine use the courts. Mine produce forty percent more and bury nine-tenths fewer. If you have come to lecture me on humanity, I would ask you to bring numbers."*

**His Blind Spot:**

Ashcroft cannot conceive that a contract might be void because it was never actually possible to fulfill. To him the ledger is reality and the people are line items in it. He will still be quoting clauses when the paper is on fire.

### 6.4 Garrison Character

- **Strength:** Company marines plus a private enforcement force (bailiffs, contract officers — armed men with legal warrants)
- **Doctrine:** legal coercion first, force second. Debt is the primary weapon; the marines are the collections department.
- **Weakness:** **one spark.** Cross-reference colonial_war_overview.md Section 7.3 — *"the indebted would burn their contracts given one spark."* The workers do not need weapons. They need the paper to stop existing.

### 6.5 Economy & Supply Lines

**Three supply lines:**
1. **The Export Run** — sugar, indigo, coffee out. The single richest cargo route in the game.
2. **The Contract Line** — **people convoys.** New indentured workers arriving from wherever the Company recruits. Raiding this is the raid-to-liberation pipeline (colony_system_core.md Section 4.5) with a twist: these passengers have *signed papers*, and Jake has to decide whether freeing them means taking the papers too.
3. **The Provision Line** — food and goods advanced to workers *at Company prices*, which is how obligations grow. Cutting this line paradoxically helps the workers.

### 6.6 Population & Demographics

- **The Hall and Estate:** Ashcroft, contract officers, bailiffs, house staff. Register: High, insulated.
- **The Contract Villages:** the majority. Multi-origin, legally free, practically owned. Housing is adequate. Food is adequate. The ledger is eternal.
- **The Cleared:** a handful — perhaps two dozen in fifteen years — who somehow cleared their obligations. They stayed, because they have nowhere else, and they work for wages now, and the villages regard them the way one regards a rumor confirmed.

**Ambient — The Hall:**

*"...the eastern fields are three percent under projection. Ashcroft will have somebody's schedule for that..."*

**Ambient — Contract Village:**

*"...my father signed. I inherited it. The clause says obligations pass to next of kin. My son is four years old and he owes them money he has never seen..."*

*"...they showed me the numbers. Every figure was correct. Every single figure was correct and somehow the total is that I will die here..."*

**Ambient — The Cleared:**

*"...twenty-three of us in fifteen years. They point at us and say the system works. We are the proof it works. We are also the proof of the arithmetic..."*

### 6.7 The Castle — Ashcroft Hall

- Fortified estate: manor, contract offices, bailiff barracks, the strongroom
- **State Treasure:** the season's revenue (enormous) + **THE CONTRACTS** — four thousand original signed instruments, the legal foundation of the entire colony, stored in the strongroom because Ashcroft understands exactly what they are worth

**The originals matter.** Company law requires the original instrument to enforce. Copies exist at New Coventry (the Master Ledger's summary) but the *enforceable* documents are here.

### 6.8 The Weakness — One Spark

Blackwood Landing has the game's cleanest weakness-to-action mapping:

**Destroy the contracts and the colony ends.** Not falls — *ends*. Without enforceable instruments, four thousand legally-free people are simply free, standing on land they have worked for fifteen years, and Ashcroft has a manor, some bailiffs, and a legal position that no longer exists.

**The three approaches:**
- **The Raid:** infiltrate the strongroom, burn or take the originals (a heist — cross-reference stealth systems)
- **The Uprising:** arm the villages, take the Hall, burn the papers in the yard (the loud version)
- **The Legal Route (Full Vision):** get the contracts to Sterling, to Kessler, to a Crown court — argue them void for impossibility. Slower, bloodless, and it sets a precedent that threatens every HAC holding in the archipelago.

### 6.9 Support Scenario

**The most quietly damning Support content in the game**, and the design must not flinch.

Supporting Blackwood Landing means escorting the richest export runs in the archipelago for excellent pay, and — the line the game will not blur — **the Company will ask Jake to escort the Contract Line.** Not slaves. Passengers with signed papers. Legally free people traveling voluntarily to honor agreements they made.

The pay is superb. Ashcroft is a courteous employer. **And every voyage delivers people into a ledger nobody escapes.**

**Crew and world reaction:** Father Salvio will ask, once, what Jake believes he is doing. Ines will read a contract and go very quiet. Mama Jacinta will not receive him. Mbah Seruni will say something about paper.

**Growth looks like:** more fields, more villages, more contracts. The building evolution here is prosperous, tidy, well-drained, and the ugliest thing in Phase 7.

### 6.10 Break Scenario

- **Economic:** raid the Export Run — this is the Company's profit center; hitting it drives Profitability down *archipelago-wide*
- **The contracts:** the highest-leverage single act available against Albion (Section 6.8)
- **The Contract Line:** intercept new arrivals before the ledger opens on them
- **Ashcroft:** he will not flee like Blackwood and will not fight like Vega. He will attempt to *litigate*, then use his bailiffs, then be genuinely astonished

### 6.11 Liberation Scenario — "Void for Impossibility"

**What freedom looks like HERE:**

A bonfire in the yard of Ashcroft Hall, and four thousand people watching paper burn.

Not an army marching in. The contracts destroyed — by heist, by uprising, or by a Crown court ruling — and the legal architecture that owned a coastline simply ceasing to be true. The villages become villages. The fields stay, and the people who worked them own them. Cross-reference colony_system_core.md Section 6.2 — Jake's ethical-plantation model finds its most pointed argument here: the same land, the same crop, the same work, and a completely different ledger.

**The liberated Blackwood Landing:** renamed (the villages choose it — and the design should let the players' actions determine whether the new name is defiant, commemorative, or plain). The Hall becomes something communal. Ashcroft's strongroom stands open and empty as a monument.

**The cost:** Ashcroft appeals, and the Company does not forget a lost profit center. Cross-reference retaliation_system.md Stage 5 — **this is the holding most likely to trigger scorched earth.** The HAC will burn what it can no longer own, and Blackwood himself will authorize it while explaining that it is a regrettable necessity of contract enforcement.

### 6.12 Cross-References

Ines (a woman who reads a contract and understands the arithmetic instantly — cross-reference npcs/named_crew_deep_dives.md) · Father Salvio (the moral question, asked once, quietly) · Mateusz Voltari (fencing routes and the question of what stolen contracts are worth) · Tito Diego (his recovery network has never dealt with paper as loot before) · Admiral Sterling (*"slavery with better stationery"* — she wrote that about this place) · Inspector Kessler (an Imperial officer who would find the legal precedent explosive)

---

## 7. FORT RESOLUTE — THE CAREER LADDER

### 7.1 Quick Reference

**Region:** Fjordlund edge — **the Northern Front** (shares regional Control meters with Puesto del Norte)  
**Type:** Strategic military outpost  
**Size:** Settlement (growth stage 2)  
**Economy:** Nothing. Subsidized entirely. Timber and whale oil barely cover the fires.  
**Castle:** **Fort Resolute** — modern, well-supplied, badly sited  
**Commander:** Colonel Marcus Thorne  
**Profitability:** 12 (**below the withdrawal threshold already** — see Section 7.9)  
**Starting Control:** shared Northern Front meters — IC 48, **AC 22**, LS 19, CT 11

### 7.2 History

Established 1870 — **six years old** — specifically to contest the Empire's northern claim. It was not built for profit. It was built because a Company director wanted the line on the map and a young officer wanted a command.

Across the frozen strait, visible on clear days, flies the Empire's Puesto del Norte, where General Vega has been holding on for eleven years (imperial_colonies.md Section 7). **Two garrisons, one channel, and a Fjordlund population that has watched this coast for a thousand years and can wait another winter** (npcs/regional_populations.md Section 5.4).

### 7.3 The Colonel — Marcus Thorne

**Full boss design:** colonial_war_bosses.md Tier 1

**Name:** Colonel Marcus Thorne  
**Age:** 29  
**Register:** High (excellent education, no experience of consequence)

**Character:**

The youngest commander in Phase 7 and the only one who has never lost anything.

Thorne is genuinely capable — good tactical mind, well-trained, personally courageous — and has spent six years treating a frozen strait as a career ladder. He has four times Vega's supply and half his understanding. He writes dispatches that read well in New Coventry. He believes the Northern Front is a *contest*, with a winner, and that winning it will make him a general.

He has never buried a man he liked. He does not know what the cold does over eleven years. He does not know why the Fjordlunders keep smiling at him.

**Voice Samples:**

*"Captain Harlow. Sit — no, take the chair by the stove, I insist, the cold here is genuinely remarkable. Now. I am told you can be bought, hired, or fought, and I have budget for two of those."*

*"Across that water is a starving Imperial general who has been refused reinforcement for a decade. I have three fresh transports a season. Explain to me how this is not already decided."*

*"The locals are pleasant enough. Cordial. They bring us fish. My predecessor said something odd before he transferred out — he said they were *waiting*. I have never understood what he meant by that and I have stopped asking."*

**His Blind Spot:**

Thorne thinks the war on the Northern Front is Albion versus the Empire. **It is two exhausted garrisons and a people who are outlasting both.** Every Fjordlunder he has met knows this. None of them have told him, because telling him would spoil it.

**Design note:** Thorne is the only commander in Phase 7 who could plausibly be *educated* rather than defeated. A Full Vision thread: if Jake shows Thorne what eleven years does — takes him to see Vega, or lets him watch a Fjordlund winter properly — he might request transfer and take the Company's withdrawal recommendation with him. **The bloodless victory over a young man who was only ever ambitious.**

### 7.4 Garrison Character

- **Strength:** modern equipment, fresh troops, seasonal rotation (nobody stays long enough to break)
- **Weakness:** **nobody stays long enough to learn.** The garrison rotates before it adapts. Cross-reference physics/snow_ice_physics.md — Fjordlunders learn from the cold, Vega's men have hardened against it, and Thorne's men are simply visitors who leave before winter teaches them anything.
- **Weakness:** the supply line is long and seasonal

### 7.5 Economy & Supply Lines

**One supply line:**
1. **The Northern Company Run** — everything, from Albion via New Coventry, three transports a season. Longer than the Empire's northern run and better funded.

**Cut it in autumn and Fort Resolute has a choice: withdraw, or discover what Vega has known for eleven years.**

### 7.6 Population & Demographics

- **The Fort:** rotating garrison, a Company factor, no civilians to speak of
- **The Fjordlund Communities:** the actual population — resisting both empires with patience rather than violence, organized around Jarl Olaf and the old ways
- **The Traders:** seasonal, playing both forts against each other for better prices, and enjoying it enormously

**Ambient — Fort Resolute:**

*"...four months to rotation. Four months. The Colonel says spring will be decisive. He said that about autumn..."*

**Ambient — Fjordlund community:**

*"...the young one across the water sends us tea now. Tea. As though we were a difficulty to be managed. We drink the tea. It is quite good tea..."*

**Ambient — Trader:**

*"...sell fish to the Empire in the morning, timber to the Company in the afternoon, and neither one asks why the prices went up. Best six years of my life..."*

### 7.7 The Castle — Fort Resolute

- Modern Albion design: proper batteries, good magazines, excellent stoves, thin walls
- Well-built for a war it will not get; badly sited for the war it has
- **State Treasure:** modest stores and the garrison payroll + **Thorne's dispatch copies** — six years of confident reports to New Coventry describing a decisive front. Read alongside Vega's eleven years of unanswered letters (imperial_colonies.md Section 7.7), the two document sets together are **the definitive indictment of colonial vanity in the archipelago**, and a player who collects both has assembled something the reform factions of *both* empires would kill for.

### 7.8 The Weakness

**Autumn, and the rotation, and the fact that it does not pay.**

### 7.9 The Withdrawal Trap — Fort Resolute's Special Case

**Profitability is already 12. Below the threshold. So why hasn't the Company withdrawn?**

Because **Thorne's dispatches are good.** He has been reporting a decisive, winnable front for six years, and the New Coventry directors — who have never seen ice — believe him. Blackwood keeps subsidizing the post because his own officer's paperwork says it is nearly won.

**This creates the game's most elegant break scenario: Jake does not need to defeat Fort Resolute. He needs to make the truth reach New Coventry.**

**Three routes:**
- **Take Thorne's dispatch copies to New Coventry** — expose the gap between the reports and the reality; the shareholder review triggers immediately
- **Inflict visible, documentable losses** — a defeat too large for Thorne's prose to survive
- **Educate Thorne himself** (Section 7.3) — let him write the honest dispatch, and he will, because he is not a liar; he is a young man who has never been shown

**The paper power is defeated by better paperwork.** Cross-reference Section 1.2, and Mbah Seruni's line about paper cutting deeper — here it cuts *Albion*.

### 7.10 Support Scenario

Supporting Fort Resolute is well-paid, comfortable, and faintly absurd: run supplies to a post that produces nothing, on behalf of a company that is being lied to by its own colonel, in a region that regards both flags as weather.

**Growth looks like:** better stoves. That is essentially it.

### 7.11 Break Scenario

- **The dispatches:** the elegant route (Section 7.9)
- **The supply line:** cut it before autumn
- **Thorne:** he will fight well and lose gracefully and be genuinely bewildered, and the design should let his bewilderment land — he never understood what game he was in

### 7.12 Liberation Scenario — Both Flags Down

**Cross-reference:** imperial_colonies.md Section 7.12 (the Northern Front's shared liberation)

**Fort Resolute cannot be liberated alone.** The Northern Front is one region with two occupiers, and Fjordlund wants the coast — not a change of tenant. Breaking Albion here without breaking the Empire hands the strait to Vega. Breaking the Empire alone hands it to Thorne.

**Both, or neither.** This is the only liberation in Phase 7 requiring two full campaigns in one region, and it demands exactly the patience Fjordlund culture is built on (npcs/recurring_quest_givers.md Section 4.1 — Jarl Olaf: *"With time. With consistency. With showing up."*).

**The liberated north:** two empty forts across a strait, fishing fleets sailing between them, Bjornshallen's long tables full, and no patrol passing Mormor Astrid's window.

### 7.13 Cross-References

General Rodrigo Vega (across the strait — and the two commanders have never met, which is a Full Vision opportunity) · Mormor Astrid · Jarl Olaf Bjornson · Helga Sigurdsdotter · Mira (her homeland; and ?Bjorn's fate lies in these contested waters) · Master Halvdan Halvorsson (Bjornshallen) · the Stone Field · Halvor Eriksson and the Fjordlund trades network

---

## 8. CROSS-COLONY SYSTEMS — THE COMPANY NETWORK

### 8.1 The Money Flows One Way

- **Blackwood Landing** generates the profit (Profitability 91)
- **New Coventry** administers it and holds the debt portfolio
- **Port Sterling** protects the shipping (and costs money — Sterling's squadron is a expense line Blackwood tolerates because Sterling makes the trade routes safe)
- **Fort Resolute** consumes it for a line on a map

**Strategic implication:** **Blackwood Landing is the Company's heart.** Hit its export runs and every other Albion holding's Profitability suffers. The HAC is a machine with one engine.

### 8.2 The Shareholder Logic (Full Vision)

The Company reallocates rationally. As Profitability falls in one holding, the Company:
1. First, reinforces (protect the investment)
2. Then, economizes (cut the garrison, reduce the subsidy)
3. Then, withdraws (Section 3.2)

**And it withdraws from the *least* profitable first.** A player squeezing the whole HAC will see Fort Resolute go, then Port Sterling's subsidy get cut (weakening Sterling — a moral complication if she is an ally), then New Coventry's administrative reach contract, and Blackwood Landing held to the very last because it is the only one that pays.

### 8.3 The Regulator Network

**Cross-reference:** retaliation_system.md Section 3.3, assassin_network.md

Company Regulators are stationed across all four holdings and coordinate through New Coventry. They are the HAC's retaliation arm — precise, contract-bound, invoice-first. **Where Herrera is the Empire's retaliation nervous system (imperial_colonies.md Section 8.3), the Regulator network is Albion's — and it is institutional, not personal.** Kill a Regulator and another is assigned. Burn the New Coventry coordination office and the whole network stalls.

### 8.4 Empire vs Albion — Playing Them Against Each Other

**Cross-reference:** war_state_system.md Section 4.6 Path C

The two empires are genuine rivals. Jake can:
- **Privateer for one against the other** (Letters of Marque — legal piracy, excellent pay, and it raises the employer's Control)
- **Feed information to both** — Salazar, Cass Wells, the Whisper Lady, Helga Brennan all trade in this
- **Trigger direct confrontations** — most explosively on the Northern Front
- **Let them bleed each other** and walk into a weakened region with the Alliance

**The trap:** every empire Jake weakens by strengthening the other leaves a *stronger* rival. The two-empire system punishes lazy play. The only outcome that removes both is Liberation — which is why LS exists and why it cannot be bought.

---

## 9. THE MIRRORS — HOW THE WORLD READS ALBION

### 9.1 Mbah Seruni

Her line predates this document and defines it: *paper cuts slower but deeper*. She has no illusions about Albion and never had any. If Jake feeds the Company she will not be surprised, which is worse than anger.

### 9.2 Don Alejandro

An Imperial officer of the old school watching the new power take with contracts what his Empire took with cannons. His view is complicated and unflattering to everyone, himself included. **Full Vision:** a veranda conversation about which method he considers more honest, and the answer is not comfortable.

### 9.3 Ines

Cross-reference npcs/named_crew_deep_dives.md. Ines reads a Blackwood Landing contract and understands the arithmetic in under a minute. She is the crew member most likely to name what the Company is, and the most likely to be furious that it is legal.

### 9.4 Father Salvio

The Company's contracts are lawful and Salvio's objection is not legal. He will ask the question once, plainly, and then he will not raise it again — and the not-raising will be louder.

### 9.5 The Quiet Coast

Cross-reference npcs/regional_populations.md Section 7.3 — *"deeds only."* The Quiet Coast's culture is uniquely resistant to the Company's model: they judge actions, not instruments. Sheriff Whitaker was at a treaty signing and has spent years watching what he signed become something else. **The Quiet Coast is the Alliance's most natural non-indigenous recruiting ground in the game.**

---

## 10. INTEGRATION MAP

- **war_state_system.md** ← starting Control per region; **Profitability as an Albion-only overlay**; withdrawal as an alternative Fall trigger feeding the same Fork
- **colony_system_core.md** ← Albion supply lines, the Contract Line as a people-convoy variant, the withdrawal-doesn't-cancel-the-paper problem
- **retaliation_system.md** ← Albion's invoice flavor, the Regulator network, Blackwood Landing as the likeliest Stage 5 trigger
- **imperial_colonies.md** ← the Northern Front's shared meters; the two-plantation contrast; the Empire/Albion rivalry
- **colonial_war_bosses.md** → Blackwood (pursuit/economic), Sterling (naval, ally-capable), Ashcroft (litigating), Thorne (educable)
- **indigenous_liberation_alliance.md** → the Quiet Coast recruiting ground; the paper-burning liberation model
- **castle_catalog.md** → Coventry House, the Sterling Bastion, Ashcroft Hall, Fort Resolute
- **bosses/crimson_armada.md** → the Sterling common cause
- **npcs/*** ← Quiet Coast population, Fjordlund population, Ines, Salvio, Kessler, Voss, Sandford, the information brokers

---

## 11. MVP CORE vs FULL VISION

### 11.1 MVP CORE

- **Two colonies fully implemented: Blackwood Landing and Fort Resolute**
  - Blackwood Landing: the contract-burning liberation — the clearest, sharpest thematic statement Albion offers
  - Fort Resolute: the Profitability/withdrawal system demonstrated at its simplest, plus the Northern Front's two-empire puzzle
- Ashcroft and Thorne as full bosses
- The Profitability Threshold and one withdrawal event
- New Coventry and Port Sterling present as locations with basic simulation; Blackwood and Sterling as major NPCs (Sterling's neutral-standoff path at minimum)

### 11.2 FULL VISION

- All four colonies, all four commanders as bosses
- Full shareholder reallocation logic (Section 8.2)
- The complete Sterling alliance arc including the Sterling/Voss meeting
- The Master Ledger's three uses (take / burn / publish) with distinct consequences
- The Blackwood Landing legal route (void for impossibility, argued in a Crown court)
- The Thorne education thread
- Thorne's dispatches + Vega's letters as a paired collectible with reform-faction consequences in both empires

---

## 12. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** Four Albion voices distinct from the Empire's and from each other — Blackwood's warm educated High, Sterling's plain naval Middle-High, Ashcroft's insulated landed High, Thorne's confident inexperienced High. The indentured districts speak in a register the Empire's chained laborers do not: the language of arithmetic.
- ✅ **Pillar 2 (Lore Through Objects):** Albion's state treasures are all instruments — the Master Ledger, the four thousand contracts, Sterling's protest correspondence, Thorne's dispatches. The Empire's crimes are its archives; **Albion's crimes are its paperwork, and the paperwork is still in force.**
- ✅ **Pillar 3 (Layered Death):** Sterling's defeat if Jake fights her is among the game's heaviest Style D moments. Thorne's bewilderment. And the quiet Style D of the Indentured District ambient — a four-year-old who owes money he has never seen.
- ✅ **Pillar 4 (Layered Humor):** The Company auditor arriving to review a colony's books mid-war is Style C gold. The Fjordlund trader's best six years. Thorne's tea. The withdrawal vote conducted by men who cannot name the place. Never at the expense of the indentured.
- ✅ **Pillar 5 (Theme — Revenge):** Albion offers the theme's hardest version: **there is no one to take revenge on.** Ashcroft is following a contract. Blackwood believes he is building civilization. The shareholders have never seen the archipelago. Jake's rage has no throat to close around — only paper. **What do you do with what was done to you, when the thing that did it is a document?**

---

## 13. NEXT STEPS

Next document: **`colonial_war/indigenous_liberation_alliance.md`** — Colony Tier Document 3 (FINAL Colony Tier)

Will cover:
- The Alliance's structure, leaders, and regional networks
- The Network Connection questlines (Jacinta + Diego + Seruni + Fjordlund + the Quiet Coast)
- Liberation Strength growth in detail, the Network Multiplier
- Ancient treasure maps, the feared waters and jungles, secret site access
- Joint conquests and Alliance bastions
- The Liberation Event battles

After the Alliance: the Castle Tier (Catalog, Sieges, Ownership).

---

## 14. CONCLUSION

The Company's Holdings. New Coventry, eighteen years old and already owning a coastline, where Lord-Director Blackwood will explain over excellent tea exactly why consent obtained through asymmetry of understanding is still consent — and mean every word. Port Sterling, where the finest officer in the archipelago protects a Company she has told in writing that she despises, because a commission is not a menu. Blackwood Landing, the richest acre in the archipelago, where nobody is a slave and no obligation has ever cleared, and a four-year-old owes money he has never seen. And Fort Resolute, six years old and already lying to its own directors, where a twenty-nine-year-old colonel sends tea to a people who have watched this coast for a thousand years.

Four holdings that can be made to leave. One profit engine that holds them all together. And the theme's hardest question, which the Empire never asks and Albion asks constantly: **what does vengeance do when the thing that hurt you is a signature?**

**Phase 7 Subsystem 6 of 16 — complete.**

**The contracts hold. The ledgers balance. The Company collects — until the paper burns.** 📜⚓


