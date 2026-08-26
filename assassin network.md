# THE ASSASSIN NETWORK — THE HUNTERS

**Status:** Design Draft v1.0
**Phase:** Phase 7 — Colonial War, Castles & Dungeons (Boss Tier Document 2 — Document 12 of 16)
**Tier:** Boss Tier
**Related:** colonial_war_overview.md, colonial_war/retaliation_system.md, colonial_war/assassin_network_questline.md, colonial_war/colonial_war_bosses.md, npcs/regional_populations.md, npcs/imperial_contacts.md, npcs/black_market_fences.md, npcs/shippable_specialists.md, mechanics/stealth.md, mechanics/combat_feel.md, style_bible.md
**Purpose:** Complete design for the two assassin organisations — La Sombra Cofradía and the Company Regulators — the named hunter roster, the gossip-web tracking mechanic, encounter design and escalation, and the ways hunters can be evaded, defeated, or turned.
**Inspirations:** Assassin's Creed Odyssey (the mercenary system), Shadow of Mordor (nemesis persistence), Middle-earth's captains, Red Dead Redemption 2 (bounty hunters with names), No Country for Old Men (the inevitable pursuer), historical Inquisition familiars and East India Company enforcement

---

## 1. HUNTER PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: They Come to Jake

Cross-reference retaliation_system.md Section 3.3. Every other boss in Phase 7 waits behind walls. The hunters do not. They arrive in ports Jake is docked at, in taverns he drinks in, on decks he is sleeping below. **The player never plans an assassin encounter. The player survives one.**

**Design intent:** Reactive dread. The world hunts back.

### 1.2 Principle 2: Fame Is the Map

Cross-reference npcs/regional_populations.md Section 10.3 (the gossip web). The hunters find Jake the way everyone else learns about him: through talk. Every raided convoy, every fallen general, every liberated village becomes an entry on a route someone is following.

**Design intent:** The player's notoriety is the tracking mechanic. Reputation has a cost with a face.

### 1.3 Principle 3: Two Organisations, Two Nightmares

The Cofradía believes. The Regulators invoice. One will pray over Jake's body; the other will bill his estate for the ammunition. **How an empire hunts is how an empire is.**

**Design intent:** Faction identity extended to its sharpest instrument.

### 1.4 Principle 4: Each Hunter Is a Person

No generic assassins. Every hunter has a name, a face, a method, a reason, and a line the player remembers. Cross-reference colonial_war_bosses.md Principle 1.1 — the Phase 7 standard applies here too.

**Design intent:** Nemesis-tier memorability at boss-tier quality.

### 1.5 Principle 5: The Hunted Can Hunt Back

Cross-reference assassin_network_questline.md. The system's escalation has an answer: trace the contracts, find the cells, and end it — or take it over. Jake is not required to endure this indefinitely.

**Design intent:** Agency against a reactive system.

---

## 2. LA SOMBRA COFRADÍA — THE SHADOW BROTHERHOOD

### 2.1 What They Are

A lay religious order under Imperial patronage, formally dedicated to "the protection of the faithful in dangerous places." In practice: the Empire's sanctioned killers, blessed before departure and absolved on return.

**Cross-reference:** npcs/imperial_contacts.md Section 5 — Inquisitor Theodora Blanc does not command the Cofradía, but she signs the writs that release them, and she signs them readily.

### 2.2 Character

- **Fanatical but not mad.** They believe Jake is a genuine spiritual danger, and several of them are pleasant company right up until the attempt.
- **Theatrical.** They announce themselves. They leave marks. They pray over the dead, including their own targets.
- **Ritualised.** The Cofradía does not simply attack; it performs a sequence — the warning, the sign, the attempt.

### 2.3 The Warning

Every Cofradía hunter announces themselves before the attempt, in the same way: **a black candle, lit, left where Jake will find it.** On his table. In his cabin. At the foot of his hammock.

Cross-reference style_bible.md Pillar 2 — the candle is a collectible. Players accumulate them. Cross-reference retaliation_system.md Section 3.2: the letters and the candles together become a shelf of accumulated threat.

### 2.4 Register

High-Middle, formal, devout. They call Jake *"hermano"* — brother — without irony, and it is worse than an insult.

---

## 3. THE COMPANY REGULATORS

### 3.1 What They Are

A department. Specifically: the Honourable Archipelago Company's Office of Contract Enforcement, Irregular Division. They have desks. They have a coordination office at Coventry House (cross-reference castle_catalog.md Section 8.4). They file reports.

**Cross-reference:** krone_colonies.md Section 4.4 — the Regulators are the hardest garrison troops in Phase 7, and this is the same organisation off-duty.

### 3.2 Character

- **Contract-precise.** They fulfil the terms of the engagement and nothing beyond it. A Regulator will not kill a bystander because the contract does not cover bystanders.
- **Bureaucratic.** Cross-reference retaliation_system.md Section 3.2 — **the Regulator notifies Jake of his own bounty in writing before hunting him.** It is considered proper practice.
- **Professional.** No fanaticism, no personal animus, and no hesitation whatsoever.

### 3.3 The Notification

Every Regulator delivers, in advance, a **Notice of Engagement** — a single sheet, correctly formatted:

> *NOTICE OF ENGAGEMENT*
>
> *Re: Outstanding obligation, Harlow, J. (Captain, vessel "La Liberación")*
>
> *This office has been engaged in the matter above. The engaging party's terms permit resolution by settlement at any point prior to conclusion. Settlement figure and remittance instructions are appended.*
>
> *Should you wish to settle, present this notice at any Company office.*
>
> *Yours faithfully,*
> *A. Finch, Regulator (First Class)*
> *Office of Contract Enforcement, Irregular Division*

**Cross-reference style_bible.md Pillar 4:** this is the funniest thing in Phase 7 and it stops being funny the first time the appended figure is one Jake could actually pay and he doesn't.

### 3.4 Register

Middle, clerical, exact. They say *"the matter"* and *"the engagement"* and never *"kill."*

---

## 4. THE TRACKING SYSTEM

### 4.1 Notoriety and the Web

Cross-reference npcs/regional_populations.md Section 10.3, retaliation_system.md Section 2.1.

Jake's **Notoriety** rises with visible, witnessed action — sinking ships in crowded waters, public defiance, defeating officers in front of crowds, liberating colonies. It falls with time and low-profile play.

**Notoriety determines:**
- How quickly hunters locate Jake after a Heat threshold is crossed
- How accurate their information is
- Whether they arrive prepared for the right Jake (cross-reference the rumour-distortion mechanic — a hunter working from six-week-old gossip may expect the wrong ship, the wrong crew size, the wrong reputation entirely)

### 4.2 The Trail

Hunters do not teleport. They **travel**, and a perceptive player can see them coming:

- Ambient dialogue changes in ports (*"...man came through asking after the Liberación. Not Imperial. Not Company either, by the coat..."*)
- Merchants and tavern keepers Jake has relationships with warn him (cross-reference npcs/tavern_keepers.md — Big Bertha, Black Cat, Doña Patricia all pass word)
- Information brokers sell precise warnings (cross-reference Section 4.4)
- The candle, or the notice

### 4.3 Going Quiet

**Cross-reference:** mechanics/stealth.md, retaliation_system.md Section 7.5

Low-profile play genuinely works:
- Avoid witnessed engagements
- Change flags (cross-reference physics/cloth_hair_physics.md Section 4.4 — false flags as a pirate tactic)
- Use lesser-known ports
- Sail the feared waters where nobody follows (cross-reference indigenous_liberation_alliance.md Section 5.3 — **the Alliance's opened routes are also hiding places**)

### 4.4 Buying Warnings

Cross-reference npcs/black_market_fences.md, npcs/imperial_contacts.md, npcs/shippable_specialists.md:

- **The Whisper Lady** sells names, methods, and arrival windows. Expensive, accurate, and she sells to both directions.
- **Cassandra "Cass" Wells** — if recruited, she runs counter-intelligence continuously and this becomes passive
- **Frau Helga Brennan** — will trade Imperial hunter information for Albion information, and enjoys the arrangement
- **Mateusz Voltari** — the underground hears about contracts before they are executed

---

## 5. THE COFRADÍA ROSTER

### 5.1 HERMANO TOBÍAS SERRA — "The First Letter"

**Tier:** entry | **Stage:** 2 | **Age:** 31

The first hunter the Cofradía sends, and by design not their best. Tobías is sincere, undertrained, and visibly frightened.

**Method:** direct confrontation in a public place, dagger and zeal
**Arena:** wherever Jake is standing — a tavern, a dock, a street

**Line:**
*"Hermano. I am sorry. Truly. They told me you would be a monster and you are having a drink, and I still have to do this."*

**Design note:** Tobías is the tutorial hunter and the design's first ethical prod. **Sparing him has consequences** — cross-reference Section 8.2.

---

### 5.2 SOR ÁNGELA DEL VELO NEGRO — "The Widow"

**Tier:** mid | **Stage:** 2-3 | **Age:** 54

A professed sister of the Cofradía, and the order's poisoner. She does not fight if she can avoid it.

**Method:** poison — food, drink, rope, cloth. Cross-reference npcs/apothecaries_and_alchemists.md Section 11.3
**Arena:** she is not fought so much as **discovered**. Cross-reference mechanics/stealth.md — Sor Ángela's encounter is an investigation with a timer: something aboard La Liberación has been tampered with, and finding it before someone in the crew drinks it is the encounter.

**Line (if cornered):**
*"You were meant to die in your sleep, hermano. Comfortable. Unafraid. I consider that a kindness and I will not apologise for it."*

**Design note:** her encounter risks a named crew member. Cross-reference npcs/named_crew_deep_dives.md — failure is survivable but costly, and the design should make the crew's recovery visible for chapters.

---

### 5.3 BRUNO SALCEDO — "El Carnicero de Valdeña"

**Tier:** mid-high | **Stage:** 3 | **Age:** 44

A former Imperial soldier from Herrera's own province, given to the Cofradía when regular service could no longer contain him. Enormous, slow, and absolutely without technique.

**Method:** a two-handed cleaving sword and a complete disinterest in his own safety
**Arena:** he chooses **enclosed spaces** — alleys, holds, cellars — where Jake cannot use distance

**Line:**
*"They pray over me before I go. After, too. Between those, I don't think about it much."*

**Design note:** Salcedo is the pure combat check in the roster. Cross-reference mechanics/combat_feel.md — he is a posture puzzle: unbreakable by pressure, breakable by patience.

---

### 5.4 PADRE ANSELMO VIDAL — "The Confessor"

**Tier:** elite | **Stage:** 4 | **Age:** 58

The Cofradía's finest, and the design's most unsettling hunter. Vidal is a genuine priest, genuinely devout, and he **hears confession before he kills.**

**Method:** rapier, precision, and a conversation
**Arena:** somewhere quiet he has arranged — an empty chapel, a shuttered warehouse, Jake's own cabin. He waits. He has been waiting some time.

**The encounter's structure:** Vidal opens by offering confession. **The player may take it.** If Jake speaks — and the dialogue options are real, covering everything from Ethan to the colonies to what Jake has become — Vidal listens properly, absolves him, and then attacks.

**Lines:**
*"Sit down, hermano. There is no hurry. You will find I am not in a hurry either."*

*"I have killed nineteen men and heard nineteen confessions and I will tell you honestly that the confessions have been the harder work."*

*"You may refuse. Many do. It makes no difference to what happens next, only to what you carry into it."*

**Design note:** Vidal is where the Cofradía's theology stops being decoration. Cross-reference style_bible.md Pillar 3 — his own death, if it comes, is Style D and he faces it with complete equanimity, which is somehow worse.

---

### 5.5 THE PRIMERA — "La Sombra Primera"

**Tier:** apex | **Stage:** 5 | **Age:** unknown

The Cofradía's master, whose name appears on no writ. Sent only at Stage 5, and only if Jake has killed multiple brothers.

**Method:** unknown until the encounter. Cross-reference assassin_network_questline.md — **the Primera's identity is a questline reveal, not a roster entry**, and the design keeps it out of this document deliberately.

---

## 6. THE REGULATOR ROSTER

### 6.1 REGULATOR (SECOND CLASS) OWEN PRYCE — "The Notice"

**Tier:** entry | **Stage:** 2 | **Age:** 27

Delivers Jake's first Notice of Engagement personally, correctly, and then waits the statutory interval before beginning.

**Method:** pistols and a short sword, competent and unremarkable
**Arena:** a dockside, in daylight, at the appointed hour — because the notice specified one

**Line:**
*"Twenty-four hours from receipt, sir, that's the standard. I did check whether it was business days. It isn't."*

**Design note:** Pryce is the roster's joke and its warning simultaneously. He is entirely reasonable and he will absolutely kill Jake at 2pm.

---

### 6.2 REGULATOR (FIRST CLASS) ADELAIDE FINCH — "The Books"

**Tier:** mid-high | **Stage:** 3 | **Age:** 61

Thirty-one years in the Irregular Division. Immaculate records. Never once exceeded a contract's terms.

**Method:** a small-calibre pistol at close range, and preparation. Cross-reference mechanics/stealth.md — Mrs Finch does not ambush; she **arranges**. Doors that lock. Crowds that thin. A gangway that is not where it was.
**Arena:** an environment she has spent two days modifying

**Lines:**
*"Captain. Sit down, I've paid for the table. Now. Your settlement figure has risen since the notice — I do apologise, that's the compounding."*

*"I have never killed a man who wasn't on a contract. I mention it because people assume, and it matters to me."*

**Design note:** Finch's encounter is the roster's environmental puzzle. Cross-reference physics/sound_physics.md — she is the only hunter who can be located by sound before being seen.

---

### 6.3 REGULATOR SILAS VANE — "The Long Room"

**Tier:** mid-high | **Stage:** 3-4 | **Age:** 39

The Company's marksman. Vane does not approach. He does not speak until it is over.

**Method:** a rifled long gun from four hundred yards. Cross-reference physics/combat_physics.md, physics/wind_systems.md — **wind, range, and light all matter, to both of them**
**Arena:** the encounter is a hunt in reverse. Jake is in the open; Vane is somewhere with a sightline. The player must find the shot's origin while under fire.

**Line (only after, if he loses):**
*"Two hundred and eighty. Wind was two points off. I don't miss twice."*

**Design note:** Vane is the roster's stealth-inverted encounter and the one that most rewards the Approach discipline the siege system teaches.

---

### 6.4 SENIOR REGULATOR BRAM KETTLE — "The Desk"

**Tier:** elite | **Stage:** 4-5 | **Age:** 52

**Cross-reference:** colonial_war_bosses.md Section 13.2 — Kettle is also the Coventry House Regulator commander. He is the same man in two contexts, and the design uses that deliberately.

Kettle coordinates the entire Albion hunter network from an office. He goes into the field twice in the whole game: once if the network fails badly enough, and once if Jake comes for the office.

**Method:** a heavy pistol brace and a boarding axe, used with the economy of a man who has done this since he was nineteen
**Arena:** either Coventry House's Regulator station, or a place of his choosing where the exits have been costed

**Lines:**
*"Nothing personal in it, Captain. There's an instruction and a fee. You'd be the same in my chair."*

*"You've done for four of mine. I've written four letters. I'd rather write none, so I've come myself, and if that reads as sentiment, I'd remind you I invoiced the Company for the travel."*

**Design note:** Kettle is the questline's central target. Cross-reference assassin_network_questline.md — killing him stalls the Regulator network archipelago-wide; **turning him is harder and worth more.**

---

### 6.5 THE FREELANCE — "MAGDALENA REYES-COLE"

**Tier:** wildcard | **Stage:** 3-5 | **Age:** 41

Neither Cofradía nor Company. Magdalena takes contracts from either empire, from rival captains, and once — cross-reference assassin_network_questline.md — from someone the player will not expect.

**Method:** whatever the contract's budget permits. She is genuinely versatile and genuinely mercenary.
**Arena:** varies. She has appeared as a passenger, a merchant, a rescued survivor, and a fellow captain in distress.

**Line:**
*"I don't hate you. I want to be clear about that because people find it comforting and it shouldn't. Hating you would at least be a reason."*

**Design note:** **Magdalena can be bought.** Cross-reference Section 8.3 — she is the roster's negotiable hunter, and she honours a purchase absolutely, which raises the obvious question of what happens when someone outbids Jake.

---

## 7. ENCOUNTER DESIGN

### 7.1 The Approach Types

The roster deliberately spans encounter formats so that hunters never feel like a repeated combat interrupt:

| Hunter | Format |
|---|---|
| Tobías | public confrontation |
| Sor Ángela | investigation under timer |
| Salcedo | enclosed melee check |
| Vidal | conversation, then duel |
| Pryce | scheduled fight |
| Finch | modified environment |
| Vane | reverse-hunt under fire |
| Kettle | boss-tier duel |
| Magdalena | varies by contract |

### 7.2 Telegraphing

**Design law:** no hunter arrives without warning. Cross-reference Section 4.2. The player should always have had a chance to notice — a candle, a notice, an ambient line, a broker's tip, a tavern keeper's glance.

**Missing the warning is the player's failure, not the game's ambush.**

### 7.3 Crew Involvement

Hunters attacking in port or aboard ship pull crew into the encounter. Cross-reference npcs/named_crew_deep_dives.md:
- Crew fight, and can be wounded
- Sor Ángela's poison specifically targets crew
- Crew commentary after each hunter is distinctive (Big Tom is unbothered; Ines takes Sor Ángela very personally; Father Salvio has extensive thoughts about Vidal)

### 7.4 Environmental Consistency

Cross-reference physics documents: hunters use the same world systems the player does. Vane accounts for wind. Finch uses sound. Salcedo chooses spaces where physics/combat_physics.md weapon-length rules favour him.

---

## 8. RESOLUTIONS

### 8.1 Killed

The standard outcome. Cross-reference retaliation_system.md — killing hunters does **not** reduce Heat and in the Cofradía's case actively raises it (they escalate for their dead).

### 8.2 Spared

**Cofradía:** a spared brother must report the failure. Cross-reference — Tobías, if spared, **returns twice more across the game**, better trained each time, and the third meeting is genuinely difficult and genuinely sad.

**Regulators:** a spared Regulator files an accurate report. **This is mechanically useful:** the Company's assessment of Jake becomes more precise, which means later Regulators arrive better prepared. Mercy has a cost here and it is an information cost.

### 8.3 Bought

**Regulators:** the Notice of Engagement's settlement clause is real. **Paying the figure ends the engagement, permanently, and the Regulator leaves.** Cross-reference retaliation_system.md Section 7.1 — Albion respects payment, and this is that principle at the individual level.

**Magdalena:** purchasable at any point, and she stays bought until outbid.

**Cofradía:** cannot be bought. Not once, not by anyone, and attempting it is taken as further evidence.

### 8.4 Turned

Cross-reference assassin_network_questline.md. Two hunters can be brought over entirely — and one of them is not who the player expects.

---

## 9. ESCALATION ACROSS RETALIATION STAGES

**Cross-reference:** retaliation_system.md Sections 2.2 and 3

| Retaliation Stage | Hunter Activity |
|---|---|
| **0-1** | none; the letters only |
| **2** | first hunters (Tobías, Pryce). One at a time. |
| **3** | mid-tier (Sor Ángela, Salcedo, Finch, Vane). Occasionally two in sequence. |
| **4** | elite (Vidal, Kettle). Coordinated: a hunter may arrive while Jake is already dealing with a hostage crisis. |
| **5** | the Primera; multiple simultaneous engagements; and cross-reference retaliation_system.md Section 3.6 — the Crimson Armada may be tolerated into the hunt |

**Momentum phase caps apply.** Cross-reference war_state_system.md Section 5 — Phase Alpha never sees more than Stage 2 hunters.

---

## 10. INTEGRATION MAP

- **retaliation_system.md** ← Heat drives hunter dispatch; hunters are Stage 2's content
- **assassin_network_questline.md** → going on offence against both organisations
- **colonial_war_bosses.md** ← Kettle appears in both documents; the Regulators garrison Coventry House
- **npcs/regional_populations.md** ← the gossip web is the tracking system
- **npcs/imperial_contacts.md** ← Blanc signs the Cofradía's writs; Brennan trades hunter intelligence
- **npcs/black_market_fences.md** ← the Whisper Lady and Mateusz sell warnings
- **npcs/shippable_specialists.md** ← Cass Wells as passive counter-intelligence
- **npcs/tavern_keepers.md** ← keepers pass word
- **npcs/named_crew_deep_dives.md** ← crew involvement, Sor Ángela's poison, Salvio on Vidal
- **mechanics/stealth.md, combat_feel.md** ← encounter mechanics
- **physics/** ← wind, sound, weapon length, and light all matter

---

## 11. MVP CORE vs FULL VISION

### 11.1 MVP CORE

- Both organisations with their signature warnings (candle, notice)
- **Four hunters:** Tobías, Pryce, Salcedo, Finch — covering four encounter formats
- The gossip-web tracking with going-quiet functional
- Killed / spared / bought resolutions
- Stage-gated escalation

### 11.2 FULL VISION

- The complete roster including Vidal's confession encounter, Vane's reverse-hunt, and Kettle
- Tobías's three-meeting arc
- Sor Ángela's crew-poison investigation
- Magdalena's outbidding logic
- The Primera
- Simultaneous Stage 5 engagements
- Full broker counter-intelligence economy

---

## 12. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** The Cofradía's devout High-Middle against the Regulators' clerical Middle. *"Hermano"* against *"the matter."* Two organisations that never once sound alike.
- ✅ **Pillar 2 (Lore Through Objects):** The black candles. The Notices of Engagement with their appended settlement figures. Both accumulate on a shelf in Jake's cabin and both mean something different.
- ✅ **Pillar 3 (Layered Death):** Vidal's equanimity. Tobías at the third meeting. Style D for the ones who were people first and hunters second, which is most of them.
- ✅ **Pillar 4 (Layered Humor):** Pryce checking whether twenty-four hours means business days. Kettle invoicing the Company for travel to a personal vendetta. The settlement clause. Every joke here is load-bearing and every one of them stops being funny on schedule.
- ✅ **Pillar 5 (Theme — Revenge):** Jake's fame is what brings them, and Vidal will hear his confession about it before killing him. The hunters are the war's answer to the question of what notoriety costs — and Magdalena's line, *"hating you would at least be a reason,"* is the theme with the reason removed.

---

## 13. NEXT STEPS

Next document: **`colonial_war/assassin_network_questline.md`** — Boss Tier Document 3

Will cover: the offensive questline, tracing contracts to cells, the infiltration, the destroy-or-turn decision, and the reveals in both organisations.

---

## 14. CONCLUSION

The Hunters. A black candle on Jake's table, lit by a frightened man who was told he would find a monster and found someone having a drink. A single correctly-formatted sheet of paper with a settlement figure appended and remittance instructions, delivered in person, twenty-four hours before the appointment. A poisoner who considers dying in your sleep a kindness. A butcher who does not think about it much. A priest who has heard nineteen confessions and found them harder work than the killings. A marksman at two hundred and eighty yards with the wind two points off. And a woman who does not hate Jake at all, and wants that understood, because hating him would at least be a reason.

They come because he is famous. He became famous by winning.

**Phase 7 Subsystem 12 of 16 — complete.**

**The candle is lit. The notice is served. Twenty-four hours from receipt.** 🕯️📄
