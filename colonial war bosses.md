# COLONIAL WAR BOSSES — THE NINE WHO HOLD

**Status:** Design Draft v1.0
**Phase:** Phase 7 — Colonial War, Castles & Dungeons (Boss Tier Document 1 — Document 11 of 16)
**Tier:** Boss Tier
**Related:** colonial_war_overview.md, colonial_war/imperial_colonies.md, colonial_war/krone_colonies.md, colonial_war/castle_catalog.md, colonial_war/castle_sieges.md, colonial_war/indigenous_liberation_alliance.md, bosses/ (Phase 1-2 boss documents), mechanics/combat_feel.md, physics/combat_physics.md, physics/ship_physics.md, style_bible.md
**Purpose:** Complete Phase 1-2-standard boss designs for the nine colonial commanders, plus the Castle Commander tier — arenas, phase structures, mechanics, dialogue, defeat variants, and Style Bible deaths.
**Inspirations:** Sekiro (posture and duel structure), Bloodborne (aggression and phase escalation), Nioh (human bosses with weight), Assassin's Creed Odyssey (conquest battle commanders), Black Flag (naval boss engagements), Red Dead Redemption 2 (antagonists who talk while they lose)

---

## 1. BOSS PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: These Are People, Not Monsters

**Cross-reference:** bosses/ (Phase 1-2), style_bible.md Pillar 3

Phase 1-2's bosses included things that were not human. Every boss in Phase 7 is a person with a name, a career, a reason, and a bad night. They bleed conventionally. They tire. Several of them talk the entire fight.

**Design intent:** Human-scale combat weight. The Ashen Wyrm was awe. Herrera is dread.

### 1.2 Principle 2: The Castle Is the Arena

**Cross-reference:** castle_catalog.md Principle 1.1

Each commander is fought inside their own fortress, and the architecture is the encounter. Herrera's interlocking fields of fire. Cruz's shade galleries. Blackwood's vertical counting halls. **The level design document and the boss design document describe the same thing from two angles.**

### 1.3 Principle 3: Every Boss Has a Non-Combat Path

**Cross-reference:** castle_sieges.md Section 5 (the Inside)

Not one of the nine must be killed. Every commander can be avoided, out-manoeuvred, bought, exposed, defected, outlasted, or educated. Some paths are much harder than the fight. All exist.

**Design intent:** The war's central argument — that force is one tool among several — expressed at the boss layer.

### 1.4 Principle 4: The Fight Reflects the Flaw

**Cross-reference:** imperial_colonies.md, krone_colonies.md (each commander's Blind Spot)

Every commander's blind spot is a mechanic. Herrera does not defend the undercity because he does not know it exists — so a player who came through it starts the fight behind his geometry. Reyes has spent twenty years preparing for an attack from the sea. De Silva cannot conceive of someone who will not take an offer.

**Design intent:** Character written in mechanics.

### 1.5 Principle 5: Nobody Dies as a Trophy

**Cross-reference:** style_bible.md Pillar 3 (Layered Death)

All nine deaths are Style D or Style C — weighted, quiet, and often unpleasant to have caused. No slow-motion finisher. No triumphal music sting. The camera holds a beat too long every time.

**Design intent:** Victory without celebration.

---

## 2. THE BOSS TIER STRUCTURE

### 2.1 Five Tiers (Recap)

**Cross-reference:** colonial_war_overview.md Section 9.1

- **Tier 1 — The Nine Commanders** (this document, Sections 4-12)
- **Tier 2 — Castle Commanders** (this document, Section 13)
- **Tier 3 — The Named Assassins** (assassin_network.md)
- **Tier 4 — Dungeon Bosses** (regional_dungeon_catalog.md)
- **Tier 5 — Secret Bosses** (secret_bosses.md)

### 2.2 Shared Systems

**Posture (cross-reference mechanics/combat_feel.md, Sekiro pillar):**
All nine commanders use the posture system. Human bosses break; they do not simply deplete. A broken commander is a person on one knee, and the game gives the player a beat to decide what that means.

**Rache (cross-reference mechanics/combat_feel.md):**
L3+R3, 30% time slow for 5 seconds, fuel = 1 cigar + 1 rum, 60s cooldown, Chapter 9+. Available in all nine fights. **Design note:** using Rache — Jake's vengeance mechanic — against a man like Cruz or Thorne should feel slightly wrong, and the design does nothing to soften that.

**Crew Presence:**
Cross-reference npcs/named_crew_deep_dives.md. Crew can be brought to castle assaults. Their combat contributions are modest; their **commentary during and after** is the point. Father Salvio at Cruz's death. Ines in Ashcroft's contract office. Mira at Puesto del Norte.

**Alliance Presence:**
At LS ≥ 50, Liberation Event boss fights include Alliance fighters as a real force. Cross-reference indigenous_liberation_alliance.md Section 6.2 — they hold flanks, take objectives, and die.

---

## 3. THE SPARE / KILL / AVOID FRAMEWORK

### 3.1 The Three Resolutions

Every Tier 1 boss resolves one of three ways, and the game tracks which:

**KILLED** — the fight ends in death. Simplest, loudest, and it enters the_named_dead.md from the other side.

**SPARED** — posture broken, and Jake chooses not to finish. What happens next varies per commander (some surrender, some flee, one refuses to be spared).

**AVOIDED** — the castle falls without the encounter. Withdrawal, purchase, uprising timing, defection, or simple absence.

### 3.2 Consequences

**Cross-reference:** retaliation_system.md, npcs/regional_populations.md, the four sages

- Killed commanders spike Retaliation Heat hardest
- Spared commanders can **return** — some as later enemies, one or two as something else
- Avoided commanders leave the archipelago alive, and the world notices when a fortress changes hands without a body

### 3.3 The Sage Ledger

**Cross-reference:** colonial_war_overview.md Section 10.3

The four wisdom keepers track how Jake resolves these encounters — not as a morality score, but as a growing impression. Mbah Seruni counts. Mama Jacinta counts. Father Salvio, who is crew and not a sage, counts most closely of all.

---

## 4. GENERAL-GOBERNADOR ALONSO DE HERRERA

**Colony:** Puerto de la Corona | **Castle:** La Ciudadela de la Corona
**Cross-reference:** imperial_colonies.md Section 3.3, castle_catalog.md Section 3

### 4.1 Boss Identity

**The apex human boss of Phase 7.** Fifty-four years old, twenty-six years of colonial service, three suppressed uprisings, and no discernible doubt. He fights the way he governs: correctly, economically, and without waste.

**Weapon:** Imperial officer's rapier and main-gauche, later a cavalry pistol
**Armour:** light cuirass under uniform coat — he expects to fight, not to be shot at
**Style:** precise, patient, punishing. He does not lunge. He waits for the player to be stupid.

### 4.2 The Arena — The Bastion Geometry

**Cross-reference:** castle_catalog.md Section 3.3

The fight takes place across the interlocking bastion faces and the connecting curtain walls — a space engineered so that every position is covered by another position.

**The arena's active mechanic:** garrison marksmen on the adjacent bastion. As long as they live, standing still in the open is punished. The player must fight Herrera **while moving through cover his engineers designed**, which is the entire experience of fighting Herrera compressed into level geometry.

**The undercity variant:** if Jake entered via the undercity (Alliance Trust Tier 3), the fight begins in the archive vaults instead — a cramped, paper-filled interior where Herrera's geometry is worthless and he is visibly, briefly, wrong-footed. **This is his blind spot made mechanical, and it makes the hardest boss in Phase 7 meaningfully easier for a player who did the Alliance work.**

### 4.3 Phases

**PHASE 1 — THE MEASURE (100-70%)**
Herrera fences conservatively, testing. Marksmen active on the flanking bastion. He speaks between exchanges, and he is assessing out loud.

*"Good. You've been taught. Somewhere expensive, or by someone desperate — the schools produce the same posture."*

**PHASE 2 — THE PRESSURE (70-35%)**
He commits. Faster combinations, deliberate posture attacks, and he begins using the arena — retreating into covered angles, forcing the player into the marksmen's field.

*"You are being herded, Captain. I mention it because you have not noticed."*

**PHASE 3 — THE ARITHMETIC (35-0%)**
The pistol comes out — one shot, telegraphed, devastating, and then discarded. Herrera fights the rest at maximum aggression, and for the first time he stops narrating.

**Design note:** his silence in Phase 3 is the tell. Herrera has stopped assessing because there is nothing left to assess.

### 4.4 Defeat

**KILLED:** he does not speak at the end. He looks at the harbour. Style D — no music, and the bell in the cathedral tower is still ringing the hour, badly timed and indifferent.

**SPARED:** Herrera does not thank Jake. He asks, once, a genuine question — *"Why?"* — and does not appear satisfied by any answer. He is taken into Alliance custody or leaves for Pale Isle in disgrace. **He returns in the endgame in a non-combat capacity if the Empire still stands: an advisor, an accuser, or a witness.**

**AVOIDED:** if Corona falls via the barrio's rising while Herrera is elsewhere, he arrives after and finds the city already lost — cross-reference imperial_colonies.md Section 3.11. He does not attempt to retake it. He reads the archives that were published, and it is unclear what he does with what he reads.

### 4.5 Rewards

- **Massive Control swing** (cross-reference war_state_system.md Section 3.1 — −25 M, −10 P)
- **The Empire's retaliation coordination degrades archipelago-wide** (cross-reference imperial_colonies.md Section 8.3 — Herrera is the nervous system)
- Herrera's rapier — cross-reference mechanics/inventory_crafting.md: a legitimately excellent weapon with a maker's mark that identifies its owner to every Imperial officer who sees it

---

## 5. GENERAL BALTASAR REYES

**Colony:** Nueva Esperanza | **Castle:** Fuerte Esperanza
**Cross-reference:** imperial_colonies.md Section 4.3, castle_catalog.md Section 4

### 5.1 Boss Identity

Forty-seven, risen from nothing, and very good at managing people who hate him. Reyes fights competently and without flourish — a man who learned violence as an administrative tool.

**Weapon:** cavalry sabre and a brace of pistols
**Style:** practical, brutal, and increasingly desperate as the fight reveals what he has actually been holding back for twenty years.

### 5.2 The Arena — The Inner Yard and the Firing Steps

**Cross-reference:** castle_catalog.md Section 4.3

The fight happens in the fort's inward-facing structure: the yard, the steps, and the platforms where the guns point at the cane.

**The arena's active mechanic:** **the guns.** During the Liberation Event variant, the fort's artillery crews are attempting to bring the inward guns to bear on the rising villages. The fight has a second objective running in parallel: reach and disable the gun positions.

**This is the timing puzzle made combat.** Cross-reference imperial_colonies.md Section 4.11 — every turn Jake spends duelling Reyes is a turn the guns get closer to loaded. **The boss fight is a race against the boss's own fortress.**

### 5.3 Phases

**PHASE 1 — THE PROFESSIONAL (100-60%)**
Reyes fights defensively, buying time for his gun crews. He is not trying to win. He is trying to delay.

*"You're in a hurry. That tells me everything about where the rest of your people are."*

**PHASE 2 — THE ARITHMETIC ARRIVES (60-25%)**
The first gun position falls (or the villages breach the gate). Reyes's composure cracks — not into rage, into something closer to vindication.

*"Twenty years. Twenty years I held this with names and small mercies. And it was always going to be this. I want that noted."*

**PHASE 3 — THE OVERSEER (25-0%)**
He fights like a man who has stopped calculating. Fast, ugly, without technique.

### 5.4 Defeat

**KILLED:** Reyes dies in the yard with the villages coming through the gate behind him. Style D. **Cross-reference imperial_colonies.md Section 4.3 — he asked Jake to notice that he was aware of what he was. The game gives him no absolution for that awareness.**

**SPARED:** he does not resist. He asks to be handed to the workers rather than the Alliance, and it is not clear whether that is courage, penance, or a final piece of management. **What the freed villages do with him is their decision and happens off-screen.** Cross-reference style_bible.md Pillar 3.

**AVOIDED:** if the uprising takes the fort before Jake reaches Reyes, he is simply gone — and the ledgers say he was there that morning. Cross-reference imperial_colonies.md Section 4.6.

### 5.5 Rewards

- Control collapse at Nueva Esperanza
- **The labor ledgers become accessible** (cross-reference castle_catalog.md Section 4.6 — the document that lets freed families find each other)
- Reyes's sabre — plain, well-maintained, unremarkable, and the description notes he clearly never enjoyed using it

---

## 6. COMANDANTE ISABEL CRUZ

**Colony:** Fuerte San Rafael | **Castle:** Fuerte San Rafael
**Cross-reference:** imperial_colonies.md Section 5.3, castle_catalog.md Section 5

### 6.1 Boss Identity

**The fight the design most wants the player to avoid.**

Forty-three, twice refused orders she considered atrocities, twice paid for it, and has commanded a mining post at the edge of the world for thirty years while keeping a list of the shrine-clearances she could not stop.

**Weapon:** infantry sword, worn plain; a shield in Phase 2 (she is the only commander who fights defensively by doctrine)
**Style:** disciplined, economical, and she does not press advantages she could take. Cross-reference: she fights like someone defending a home she does not believe she deserves.

### 6.2 The Arena — The Shade Galleries and the Cistern Complex

**Cross-reference:** castle_catalog.md Section 5.3

Close, hot, low-ceilinged. The galleries that let the garrison move without heat exposure become a duelling corridor. Cross-reference physics/lava_heat_physics.md — **the arena applies heat pressure throughout**, and the cistern complex at the centre is the only cool space.

**The arena's active mechanic:** heat accumulation. The player takes gradual heat damage in the galleries and recovers in the cistern chamber — which is exactly where Cruz retreats to, and exactly what she is defending. **The fight's rhythm is dictated by the resource the fortress was built around.**

### 6.3 Phases

**PHASE 1 — THE DUTY (100-65%)**
Cruz fights well and says almost nothing. She has no speeches prepared.

*"I'm not going to argue with you, Captain. You've made your decision. Come on."*

**PHASE 2 — THE LIST (65-30%)**
Shield up, ground given deliberately, retreating toward the cisterns. And she starts talking — not to persuade, but because she has been silent for thirty years and this is the last opportunity.

*"There's a cabinet in my office. Locked. Names and dates. Every clearance I couldn't stop."*

*"I kept it because someone should have. I never decided who."*

**PHASE 3 — EIGHTEEN FORTY-SIX (30-0%)**
In the cistern chamber, in the cool, with the water behind her.

*"I was nineteen. I was in this fort. I heard the report come in and I filed it and I have been filing things ever since."*

*"Ask me if I'd stop it now. Go on. Ask me and watch me not know."*

### 6.4 Defeat

**KILLED:** Style D, and among the heaviest in the game. She does not fall dramatically. Cross-reference imperial_colonies.md Section 5.12: *"she fights for it, well, and loses, and the game does not pretend that is triumphant."*

**Crew reaction:** Father Salvio, if present, says nothing at all, and that is the loudest thing in the scene.

**SPARED:** Cruz surrenders the fort. She gives Jake the cabinet key and the 1846 reports without being asked. **She asks to be allowed to walk into the desert alone** — cross-reference physics/sand_desert_physics.md, and whether she survives is never stated. Mbah Seruni's mountain people do not stop her.

**AVOIDED:** if San Rafael falls to the cisterns and the mountain paths without a fight, Cruz surrenders formally to the Alliance. **This is the outcome the design considers correct and rewards least.** Cross-reference indigenous_liberation_alliance.md Section 6.3.

### 6.5 Rewards

- Control collapse at San Rafael
- **The 1846 patrol reports** — cross-reference castle_catalog.md Section 5.6, and the Ibarra thread opens
- **Cruz's list** — thirty years of shrine clearances, names and dates, worth more to Mbah Seruni's networks than any weapon in the game

### 6.6 The Ibarra Encounter

**Cross-reference:** imperial_colonies.md Sections 5.9 and 6.9

**Teniente-Coronel Marcos Ibarra — retired, comfortable, growing orchids in Bahía Dorada — is not a boss fight.**

He is seventy-one. He is undefended. He does not recognise the name Jaya when Jake says it, and then he does, and his face does something complicated.

**The design gives the player no combat mechanics here. No arena, no phases, no health bar.** Whatever Jake does happens in a garden, and cross-reference the_named_dead.md Section 2.3: the game offers no closure mechanic and no correct answer. Mbah Seruni's wishes — if Jake asked her, and if she gave any — are the only guidance available.

---

## 7. GOBERNADOR FRANCISCO DE SILVA

**Colony:** Bahía Dorada | **Castle:** Castillo Dorado
**Cross-reference:** imperial_colonies.md Section 6.3, castle_catalog.md Section 6

### 7.1 Boss Identity

**The boss who tries hardest not to be one.**

Sixty-one, corrupt in the constitutive sense, warm, funny, cultured, and a coward. He does not fight. His guard captain fights. De Silva negotiates, flees, and offers, and his health bar is functionally a patience bar.

### 7.2 The Arena — The Reception Wing and the Gardens

**Cross-reference:** castle_catalog.md Section 6.3

A running encounter through beautiful rooms: the reception wing, the ballroom, the terrace, and finally the gardens on the seaward bastion.

**The arena's active mechanic:** **Captain Miguel Aldana**, de Silva's guard captain — a genuinely capable Tier 2 fighter who is doing his job with visible resentment. He is the actual combat. De Silva runs between rooms, and each room he reaches produces a new offer.

### 7.3 Phases — The Escalating Offers

**PHASE 1 — THE OPENING BID**
*"Captain! Before anyone does anything permanent — the vault. Half. I'll open it myself, no conditions, and we call this a misunderstanding between gentlemen."*

**PHASE 2 — THE SWEETENER**
*"All of it. All of it, the vault entire, and the Silvera introductions, and I will personally write to Pale Isle explaining that you were never here."*

**PHASE 3 — THE LEDGERS**
*"The ledgers. My private books. Every bribe I have ever taken and every officer I have ever bought, and Captain — this is the part you should attend to — **that includes the men currently hunting you.**"*

**PHASE 4 — THE TRUTH**
Cornered in the gardens, out of offers.

*"...I have never held a weapon in earnest. Not once. Sixty-one years. I find I am not curious about the experience."*

### 7.4 Defeat

**KILLED:** among the orchids and the roses, and it is faintly pathetic. Style C bleeding into D — the death of a man who was never a soldier and never pretended to be. **Cross-reference style_bible.md Pillar 4: the humour in de Silva's fight curdles the instant it ends.**

**SPARED:** de Silva is delighted, immediately loyal in his transactional way, and **becomes a genuinely useful long-term asset** — an informant inside Imperial society who will sell Jake anything, including things Jake did not know to ask for. He is also, obviously, selling Jake to someone else. The game does not resolve which.

**AVOIDED / PURCHASED:** cross-reference imperial_colonies.md Section 6.11 — Bahía Dorada can be bought. De Silva will assist in his own colony's fall for the right consideration and will not consider this a betrayal of anything.

### 7.5 Rewards

- The largest doubloon haul in Phase 7 (the vault)
- **De Silva's bribe ledgers** — cross-reference castle_catalog.md Section 6.6: Inspector Kessler's entire arc terminates here
- **Captain Aldana** — if spared, he is recruitable. Cross-reference npcs/shippable_specialists.md format: a competent officer who spent six years guarding a man he despised.

---

## 8. GENERAL RODRIGO VEGA

**Colony:** Puesto del Norte | **Castle:** Puesto del Norte
**Cross-reference:** imperial_colonies.md Section 7.3, castle_catalog.md Section 7

### 8.1 Boss Identity

Fifty-eight, eleven years abandoned, four letters of encouragement, and one obsession: he will not be the general who lost the northern line.

**Weapon:** heavy Imperial broadsword, two-handed — a cold-climate weapon, and he has been practising with it in a courtyard for over a decade
**Style:** relentless, heavy, and entirely without self-preservation.

### 8.2 The Arena — The Wall Walk and the Unfinished Bastion

**Cross-reference:** castle_catalog.md Section 7.3, physics/snow_ice_physics.md

The wall walk in winter, and the skeletal third bastion with its eight-year-old frozen scaffolding.

**The arena's active mechanic:** **ice.** Cross-reference physics/snow_ice_physics.md — the wall walk is slick, the scaffolding is treacherous, and both fighters are affected. Vega has spent eleven winters on this wall. **He does not slip. Jake does.**

### 8.3 Phases

**PHASE 1 — THE LINE (100-60%)**
Vega does not retreat a single step. He fights the way a wall fights.

*"Eleven years. Do you understand what that is? No. You're what — thirty? You have never held anything for eleven years."*

**PHASE 2 — THE SCAFFOLDING (60-25%)**
The fight moves into the unfinished bastion. Vertical, unstable, and Vega uses it fearlessly because he has walked it a thousand times.

*"I requested the funds for this eleven times. Eleven. They sent commendations. You are standing in the reply."*

**PHASE 3 — THE NAME (25-0%)**
Maximum aggression, no defence at all.

*"Someone in Pale Isle will have to write my name. That's all. That's the whole of it now."*

### 8.4 Defeat

**KILLED:** on the wall, in the snow, facing the strait where Fort Resolute's flag is visible on a clear day. **Style D at maximum weight.** Cross-reference imperial_colonies.md Section 7.11: *"the final fight at Puesto del Norte is Style D from the first shot."*

**Crew reaction:** Mira, if present. Cross-reference npcs/named_crew_deep_dives.md — this is her coast, and she watched a man die for a line an empire drew on it.

**SPARED — the refusal:** **Vega is the one commander who cannot be spared.** Posture broken, on one knee, he stands back up. If Jake refuses to finish, Vega attacks again. And again. The game will not let the player resolve this cleanly, because Vega will not permit it.

**Design note:** the only exit is to leave. Jake can walk away, and Vega will hold a wall alone until the supply line kills him. Cross-reference: this is arguably worse, and the game says nothing about it.

**AVOIDED:** cut the Northern Run before the freeze. Puesto del Norte starves. Cross-reference imperial_colonies.md Section 7.11 — *"it is a cold thing to do to men the Empire already abandoned."*

### 8.5 Rewards

- Control collapse on the Northern Front (cross-reference the three-way puzzle — and this may hand the strait to Thorne)
- **Vega's eleven years of correspondence** — worthless as loot, devastating as a document. Cross-reference castle_catalog.md Section 7.6 and the paired indictment with Thorne's dispatches.
- His broadsword — enormous, cold-forged, and cross-reference npcs/blacksmiths_and_armorers.md: Halvor Eriksson can identify the smith, and has opinions.

---

## 9. LORD-DIRECTOR EDMUND BLACKWOOD

**Colony:** New Coventry | **Castle:** Coventry House
**Cross-reference:** krone_colonies.md Section 4.3, castle_catalog.md Section 8

### 9.1 Boss Identity

**The pursuit boss.** Fifty-one, a true believer, and he does not fight — not from cowardice like de Silva, but because fighting is not among the tools he considers relevant.

**The encounter is a chase, vertically, through Coventry House, while Blackwood carries the Master Ledger and argues.**

### 9.2 The Arena — Coventry House, Vertically

**Cross-reference:** castle_catalog.md Section 8.3

Ground floor warehousing → first floor counting halls → second floor offices → third floor archives → the roof, or the dock, depending on how it goes.

**The arena's active mechanics:**
- **Company Regulators** — cross-reference assassin_network.md: elite, contract-precise, and the hardest garrison troops in Phase 7. They fight in relays, buying Blackwood floors.
- **The clerks.** Hundreds of them, at their desks, in the middle of a running battle. **They do not flee.** They keep working, or they hide under the desks, and some of them keep working under the desks. Cross-reference style_bible.md Pillar 4 — this is horrifying and funny and the design does not choose between them.
- **The paper.** Fire spreads catastrophically in the archives (cross-reference physics/fire_physics.md). A player who uses fire here may destroy the Master Ledger and never know it burned.

### 9.3 Phases — The Continuous Argument

Blackwood talks for the entire encounter, floor by floor, and he is not stalling. He genuinely wants Jake to understand.

**GROUND FLOOR:**
*"Captain. You are in my warehouse. There is nothing in my warehouse but goods that people wanted and paid for."*

**COUNTING HALLS:**
*"Look at them. Look. Forty-one clerks, literate, salaried, none of them starving. Show me the Imperial colony that produced forty-one literate salaried men in eighteen years and I will concede the entire argument."*

**OFFICES:**
*"The hospital. The school. The aqueduct. You will not burn those, I think, because you are not a stupid man. So what precisely is it you object to? Say the word. Say the actual word."*

**ARCHIVES:**
*"Debt. You object to debt. Captain, every civilisation that has ever existed runs on obligation — I have simply written mine down. **Everything is written down. That is not my crime. That is my defence.**"*

**THE ROOF / THE DOCK:**
*"...you are going to take that book and burn it, and four thousand people will be free of an obligation they entered into voluntarily, and the hospital will close within two years because nothing funds it, and you will have done something you can feel excellent about. I want that in the record too."*

### 9.4 Defeat

**KILLED:** Blackwood dies mid-sentence. **The design should let the sentence be unfinished and never reveal how it ended.** Style D, with the unsettling note that he was still, at the moment of death, expecting to persuade.

**SPARED / ESCAPED:** if Blackwood reaches the dock, he leaves the archipelago with the Master Ledger. **The debts remain enforceable.** He surfaces later — Full Vision — testifying, litigating, or simply writing, and he is never quite dealt with.

**AVOIDED — WITHDRAWAL:** cross-reference krone_colonies.md Section 3.2. If New Coventry is driven below the profitability threshold, Blackwood is recalled by the shareholders. **He does not lose to Jake. He loses to a vote.** He would find that infuriating and would not say so.

### 9.5 Rewards

- **THE MASTER LEDGER** — cross-reference krone_colonies.md Section 4.8: take it, burn it, or publish it, and the three outcomes are genuinely different
- **The Regulator coordination office** — cross-reference castle_catalog.md Section 8.4: destroying it stalls Albion's retaliation network archipelago-wide
- The Company treasury

---

## 10. ADMIRAL KATHERINE STERLING

**Colony:** Port Sterling | **Castle:** The Sterling Bastion
**Cross-reference:** krone_colonies.md Section 5.3, castle_catalog.md Section 9

### 10.1 Boss Identity

**The hardest naval engagement in Phase 7, and the one the design most hopes never happens.**

Forty-six, the finest officer in the archipelago, honest, brilliant, and holding a commission she has repeatedly told her superiors she despises the terms of.

**She is fought at sea, at the head of her squadron. There is no duel.**

### 10.2 The Arena — The Approaches to Port Sterling

**Cross-reference:** physics/ship_physics.md, physics/water_physics.md, physics/wind_systems.md

A naval boss fight in three dimensions: Sterling's flagship **HMS Intractable**, two escorting frigates, and the harbour batteries.

**The arena's active mechanics:**
- **Squadron coordination** — Sterling's ships work together properly. Cross-reference physics/ship_physics.md: they will cross Jake's T, they will use the wind, and they will concentrate fire.
- **The batteries** — cross-reference castle_sieges.md Section 3.2: the Bastion's guns cover the anchorage
- **Sterling's own seamanship** — the Intractable is handled better than any AI ship in the game

### 10.3 Phases

**PHASE 1 — THE ENGAGEMENT**
Formal, disciplined, and she offers terms before firing.

*"Captain Harlow. This is Admiral Sterling. You are in my waters with hostile intent. Strike your colours and I will guarantee your crew's lives and a fair hearing. You have one minute."*

**PHASE 2 — THE SQUADRON**
Both frigates commit. Coordinated manoeuvres, and the fight becomes a problem of angles.

*"Understood. Then we do it properly."*

**PHASE 3 — THE INTRACTABLE**
Escorts disabled or sunk. Ship to ship, and Sterling fights it out with total professionalism and zero theatrics.

**BOARDING VARIANT:** if Jake boards the Intractable, Sterling meets him on her own quarterdeck with a naval cutlass — the only melee variant, and she is very good.

### 10.4 Defeat

**KILLED:** *"I will hang you with respect"* was not rhetoric, and neither is this. She dies on her own deck. **Style D at the game's maximum weight.** No music. The Intractable burning behind her.

**Cross-reference krone_colonies.md Section 5.10:** *"the design should make the fight feel like a failure of imagination even when it is a victory."*

**SPARED:** Sterling accepts quarter — for her crew, not herself. She surrenders her sword and asks about her wounded before anything else. **Held or paroled, she does not become an ally through defeat.** Respect and captivity are different things and she says so.

**AVOIDED — THE ALLIANCE:** cross-reference krone_colonies.md Section 5.4. The path exists: mutual respect, the Crimson common cause, the evidence, the choice. **Sterling as an endgame ally with her squadron is one of the strongest strategic outcomes in Phase 7**, and it requires never fighting her.

**AVOIDED — THE STANDOFF:** the neutral outcome. She will not hunt Jake; Jake will not sink her ships. Both keep it. Neither is comfortable.

### 10.5 Rewards

- **Sterling's protest correspondence** — cross-reference castle_catalog.md Section 9.5: **and on the alliance path she hands it over herself, because she kept the copies deliberately and has been waiting for someone to ask.**
- The Bastion's naval infrastructure
- **On the alliance path:** her squadron in the endgame, and cross-reference bosses/crimson_armada.md — this materially changes the final act

---

## 11. GENERAL PERCIVAL ASHCROFT

**Colony:** Blackwood Landing | **Castle:** Ashcroft Hall
**Cross-reference:** krone_colonies.md Section 6.3, castle_catalog.md Section 10

### 11.1 Boss Identity

Fifty-five, the foreclosure enforcer, and the least self-aware antagonist in Phase 7. He does not have Reyes's moral tension because he has never located the question.

**Weapon:** duelling sabre, and a bailiff's cudgel he switches to when the sabre proves insufficient — a detail the design should not comment on
**Style:** competent gentleman's fencing that degrades into something uglier under pressure.

### 11.2 The Arena — The Hall

**Cross-reference:** castle_catalog.md Section 10.3

Corridors, the dining room, the study, and the terrace overlooking the fields. Domestic architecture as a battlefield.

**The arena's active mechanic:** **the bailiffs.** Armed men with legal warrants who fight for wages. Cross-reference krone_colonies.md Section 6.10 — as the fight progresses and the villages rise, **bailiffs begin to leave.** Not rout: they put down their weapons and walk out, individually, at different points, and the arena empties around Ashcroft in real time.

### 11.3 Phases

**PHASE 1 — THE INSTRUMENT (100-65%)**
Ashcroft is not alarmed. He is annoyed.

*"You are trespassing on Company property in breach of every applicable statute, and I want you to know that this will be litigated."*

**PHASE 2 — THE BAILIFFS THIN (65-30%)**
Men leaving. Ashcroft notices and cannot process it.

*"Where are you going? Kessler — where are you GOING? You are under contract!"*

**PHASE 3 — THE ASTONISHMENT (30-0%)**
Alone, with the cudgel, on the terrace, with the fields below and the fires starting.

*"They signed. Every one of them signed. I have the documents. **I HAVE THE DOCUMENTS.**"*

### 11.4 Defeat

**KILLED:** on the terrace, in view of everything he owned. **Style C bleeding into D** — his last words are procedural, and the horror is that he means them.

**SPARED:** Ashcroft immediately begins negotiating terms of surrender as though this were a commercial matter. He appeals. He files. **He genuinely expects the contracts to be honoured by whoever holds the land next**, and cross-reference castle_ownership.md Section 5.4 — that option technically exists.

**AVOIDED — THE HEIST:** cross-reference castle_sieges.md Section 4.4. Take the four thousand contracts from the strongroom and leave. Ashcroft wakes to a legal position that no longer exists and never sees Jake at all. **Cross-reference krone_colonies.md Section 6.11: the colony does not fall — it ends.**

### 11.5 Rewards

- **THE FOUR THOUSAND CONTRACTS** — cross-reference castle_catalog.md Section 10.5: the originals, and Company law requires originals to enforce
- The season's revenue
- Cross-reference indigenous_liberation_alliance.md Section 3.3 — the single largest LS gain available from any castle

---

## 12. COLONEL MARCUS THORNE

**Colony:** Fort Resolute | **Castle:** Fort Resolute
**Cross-reference:** krone_colonies.md Section 7.3, castle_catalog.md Section 11

### 12.1 Boss Identity

Twenty-nine, genuinely capable, and he has never lost anything.

**Weapon:** modern Albion officer's sabre, excellent technique, expensively taught
**Style:** textbook, fast, and completely unprepared for someone who does not fight to a textbook.

### 12.2 The Arena — The Batteries and the Wall Walk

**Cross-reference:** castle_catalog.md Section 11.3, physics/snow_ice_physics.md

Modern gun positions, good sightlines, and a garrison that rotates seasonally and has never adapted to the cold.

**The arena's active mechanic:** **the cold, and the rotation.** Thorne's men fight well for approximately four minutes and then start making mistakes — cross-reference krone_colonies.md Section 7.4: *"nobody stays long enough to learn."* The arena gets easier as the fight goes on, and the reason is written into the colony's design document.

### 12.3 Phases

**PHASE 1 — THE COMMAND (100-60%)**
Thorne fights confidently and directs his men well.

*"Steady! Volley on my word — Captain Harlow, I would genuinely rather not do this, my budget for the quarter is already—"*

**PHASE 2 — THE GARRISON FALTERS (60-25%)**
His men are cold, slow, and losing. Thorne begins to understand that something is wrong with his entire model of the situation.

*"Why are they— sergeant, get UP— why is nobody— "*

**PHASE 3 — THE BEWILDERMENT (25-0%)**
He fights well and alone.

*"Six years. I have reported six years of progress. What have I been reporting?"*

### 12.4 Defeat

**KILLED:** Style C into D. He dies competent, young, and confused, and cross-reference krone_colonies.md Section 7.11: *"he never understood what game he was in."*

**SPARED:** Thorne surrenders the fort. **And then asks Jake to explain.** He genuinely wants to know, and cross-reference krone_colonies.md Section 7.3 — this is the entry point to the education path.

**AVOIDED — THE DISPATCHES:** cross-reference castle_catalog.md Section 11.6, krone_colonies.md Section 7.9. Take Thorne's six years of confident reports to New Coventry, expose the gap, and the shareholder review triggers. **Fort Resolute is withdrawn from beneath him.**

**AVOIDED — THE EDUCATION (Full Vision):** show Thorne what eleven years does. Take him across the strait. Let him meet Vega, or see a Fjordlund winter properly, or read what he has been writing next to what has actually been happening. **He is not a liar. He writes the honest dispatch himself, and takes the withdrawal recommendation with him.**

**The bloodless victory over a young man who was only ever ambitious.**

### 12.5 Rewards

- **Thorne's dispatch copies** — and paired with Vega's correspondence, cross-reference castle_catalog.md Section 13.2: the definitive indictment of colonial vanity in the archipelago
- Northern Front Control swing
- **On the education path:** Thorne survives, leaves the archipelago, and — Full Vision — writes something years later that matters.

---

## 13. TIER 2 — CASTLE COMMANDERS

### 13.1 The Role

Where a colony's General is absent, elsewhere, or already dealt with, each castle has a **Castle Commander** — a mini-boss to boss-tier officer who holds the walls. Roughly ten across Phase 7.

**Design standard:** less elaborate than Tier 1, more elaborate than a captain. One arena, two phases, one memorable trait, one line worth remembering.

### 13.2 Selected Commanders

**Captain Miguel Aldana** (Castillo Dorado) — de Silva's guard captain. Genuinely capable, visibly resentful, six years guarding a man he despises. **Recruitable if spared.**

*"I'm not paid enough to die for him. I am, unfortunately, paid exactly enough to fight you for a while first."*

**Teniente Rafael Ocampo** (La Ciudadela, outer bastions) — Herrera's protégé, twenty-six, and he has modelled himself on his general so precisely that fighting him is a rehearsal for the real thing.

**Sergeant-Major Bram Kettle** (Coventry House Regulators) — the Company's senior Regulator in the archipelago. Cross-reference assassin_network.md: he is the network's coordinator and the reason the retaliation machine functions.

*"Nothing personal in it, Captain. There's an instruction and a fee. You'd be the same in my chair."*

**Overseer-Captain Domingo Salas** (Fuerte Esperanza) — runs the inward-facing guns. **The Liberation Event's actual objective.** Killing Reyes is optional; stopping Salas is not.

**Lieutenant Anders Holt** (Fort Resolute) — Thorne's second, thirty-four, on his fourth northern rotation, and the only man in the fort who has understood the situation for two years. He has said nothing because saying it would end his career.

*"The Colonel writes what New Coventry wants to read. I countersign it. That's the job. That's been the job for two years."*

**Prior-Captain Eugenio Vasco** (La Ciudadela cathedral wing) — Inquisition-attached, and cross-reference npcs/imperial_contacts.md: Blanc's man. The only Tier 2 commander the design permits to be genuinely hateful.

### 13.3 Tier 2 Design Rules

- Each fights in a specific district of their castle (cross-reference castle_catalog.md district lists)
- Each has one **mechanical signature** tied to that district
- Each can be avoided by a different door
- Several are recruitable, and cross-reference npcs/shippable_specialists.md for the format

---

## 14. INTEGRATION MAP

- **castle_catalog.md** ← every arena is a castle district
- **castle_sieges.md** ← the four phases lead into these encounters; the three doors determine which variant fires
- **imperial_colonies.md / krone_colonies.md** ← every commander's character, voice, and blind spot originates there
- **war_state_system.md** ← boss defeats are the largest single Control swings in Phase 7
- **retaliation_system.md** ← killed commanders spike Heat hardest; Kettle's death stalls the Regulator network
- **indigenous_liberation_alliance.md** ← Liberation Event variants, Alliance combat presence, Mbah Seruni and the 1846 reports
- **assassin_network.md** → the Regulators fought at Coventry House are the same organisation
- **npcs/named_crew_deep_dives.md** ← crew reactions at Cruz, Vega, Ashcroft, Sterling
- **npcs/the_named_dead.md** ← spared commanders who return; killed commanders who are named
- **mechanics/combat_feel.md, physics/combat_physics.md, physics/ship_physics.md** ← the mechanical foundation
- **the four sages** ← the ledger of how Jake resolved these

---

## 15. MVP CORE vs FULL VISION

### 15.1 MVP CORE

- **Three Tier 1 bosses fully built:** Reyes (the timing puzzle), Cruz (the heat arena and the heaviest death), Ashcroft (the thinning bailiffs)
- All three resolutions (killed / spared / avoided) functional for each
- Two Tier 2 commanders (Salas, Aldana)
- Posture, Rache, and crew presence integrated
- The Ibarra non-encounter

### 15.2 FULL VISION

- All nine Tier 1 commanders with every arena variant (including Herrera's undercity opening)
- Sterling's full naval engagement plus the boarding variant plus the alliance avoidance path
- Blackwood's complete vertical pursuit with the clerks and the burnable archives
- Vega's refusal-to-be-spared logic
- Thorne's education path
- All ten Tier 2 commanders with recruitment options
- Full sage-ledger reactivity to resolutions

---

## 16. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** Nine commanders, nine registers, and each fights in the voice their colony document established. Herrera's absolute High. Cruz's soldier's Middle. Blackwood's warm educated High delivered while retreating up a staircase.
- ✅ **Pillar 2 (Lore Through Objects):** Every commander's reward is their document or their weapon, and both identify them. Herrera's marked rapier. Reyes's unloved sabre. Cruz's locked cabinet. Vega's cold-forged broadsword that Halvor can name the maker of.
- ✅ **Pillar 3 (Layered Death):** All nine are Style D or C-into-D. Vega in the snow facing a flag. Sterling on her own burning deck. Blackwood mid-sentence. **The camera holds a beat too long every time, and there is never a sting.**
- ✅ **Pillar 4 (Layered Humor):** De Silva's escalating offers as a phase structure. The clerks who keep working under their desks. Thorne's budget for the quarter. Aldana's honesty about his wages. Every joke curdles at the moment of death, deliberately.
- ✅ **Pillar 5 (Theme — Revenge):** Nine men and women who each believe something nearly true, all of whom can be spared, avoided, outwitted, or simply left alive — and the game counts which Jake chose. Ibarra in his garden is the theme with no mechanics attached at all: no arena, no phases, no correct answer, and an old man with orchids who has just remembered a name.

---

## 17. NEXT STEPS

Next document: **`colonial_war/assassin_network.md`** — Boss Tier Document 2

Will cover: La Sombra Cofradía and the Company Regulators, the named assassin roster, the gossip-web tracking mechanic, encounter design, and escalation across the retaliation stages.

---

## 18. CONCLUSION

The Nine Who Hold. Herrera in his geometry, herding a pirate through fields of fire his engineers designed, and going silent in the last phase because there is nothing left to assess. Reyes buying time for gun crews that will shell the villages he learned every name in. Cruz in the cool of the cistern chamber, talking for the first time in thirty years because there will not be another chance. De Silva running out of offers among his orchids. Vega on a wall he requested funding for eleven times, refusing to be spared. Blackwood arguing up a staircase, still expecting to persuade. Sterling offering terms before she fires. Ashcroft shouting about documents on a terrace above burning fields. Thorne, twenty-nine, asking what he has been reporting for six years.

None of them have to die. All of them can. And an old man in a garden in Bahía Dorada, seventy-one and growing orchids, is not a boss fight at all.

**Phase 7 Subsystem 11 of 16 — complete.**

**Nine commanders. Nine arenas. Twenty-seven ways it ends.** ⚔️
