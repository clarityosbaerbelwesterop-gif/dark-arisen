# ARMY SYSTEM — WHAT IT COSTS TO HAVE MEN

**Status:** Design Draft v1.0
**Phase:** Phase 9 — Progression, Economy & Armies (Document 4 of 6)
**Related:** progression_overview.md, economy_core.md, battle_system.md, battle_catalog.md, colonial_war/war_state_system.md, colonial_war/castle_ownership.md, colonial_war/castle_sieges.md, colonial_war/indigenous_liberation_alliance.md, house_system.md, highmoore_reconstruction.md, mechanics/ship_management.md, npcs/named_crew_deep_dives.md, npcs/the_named_dead.md, style_bible.md
**Purpose:** The army system — the five force types, recruitment, composition, upkeep, loyalty, command capacity, what Jake can and cannot order, desertion, and the design law that an army is a liability with a use rather than a power fantasy.
**Inspirations:** Mount & Blade (upkeep as the real constraint), Bannerlord's party economy, Age of Sail privateering, the Border Reiver levy, real early-modern military logistics, and the explicit rejection of RTS-style unit spam

---

## 1. THE DESIGN LAWS

### 1.1 An Army Is a Liability

**Cross-reference:** economy_core.md Section 5.1.

**The largest recurring drain in Dark Arisen.** Men eat every chapter, forever, whether they fight or not, and cross-reference Section 5.3: **unpaid men leave, and some of them take things.**

### 1.2 Jake Is Not a General

**Cross-reference:** characters/jake_harlow.md, mounted_travel.md Section 7.1.

**He is a ship's captain.** He commands a crew of dozens by voice and by presence. Cross-reference Section 6 — **command above roughly two hundred requires officers, and officers require standing, money, or both.**

### 1.3 There Is No Unit Roster

**Design law:** cross-reference progression_overview.md Section 1.1 (readability).

**No RTS panel. No unit cards. No selection boxes.**

An army is read the way everything in this bible is read: **cross-reference Section 8 — you look at the camp.**

### 1.4 Force Composition Is Cultural

**Cross-reference:** house_system.md Section 5, colonial_war/indigenous_liberation_alliance.md, house_belos.md Section 4.

There is no generic "infantry." Cross-reference Section 3 — **Belos archers, Vane cavalry, Alliance fighters, hired Ridd levy, and Jake's own settlement militia are five different things that fight differently, cost differently, and want different things.**

---

## 2. WHEN ARMIES BECOME AVAILABLE

### 2.1 The Gate

**Chapter 6+**, and cross-reference colonial_war/castle_ownership.md Section 2.4: **Jake needs somewhere to put them.**

**Prerequisites:** one holding at Development Stage 2+, and standing with at least one source (Section 3).

### 2.2 The Scale Bands

| Band | Men | What it is for |
|---|---|---|
| **Crew** | 40-90 | boarding, raids. Cross-reference mechanics/ship_management.md — **this is not an army and always exists.** |
| **Company** | 90-250 | garrison, escort, a village's defence |
| **Force** | 250-800 | a siege's assault element; a Liberation Event's spine |
| **Host** | 800-2,500 | a battle. Cross-reference battle_system.md. |

**Design note:** cross-reference battle_catalog.md — **nothing in Dark Arisen requires a Host except three battles**, and all three are optional.

---

## 3. THE FIVE FORCE TYPES

### 3.1 HIRED TROOPS

**Source:** money. Cross-reference the_lesser_houses.md Section 4.7 — **House Ridd's levy fights for whoever pays and everyone employs them and nobody trusts them.**

**Cost:** 14 doubloons/man/chapter (archipelago) · 2 silver marks/man/chapter (Highmoore)
**Loyalty:** low. Cross-reference Section 5.3 — **first to leave, always.**
**Quality:** middling and reliable.
**Cap:** money.

### 3.2 SETTLEMENT MILITIA

**Cross-reference:** colonial_war/castle_ownership.md Section 6.2.

**Source:** Jake's own holdings. Cross-reference colonial_war/colony_system_core.md Section 3 — **the people he carried there.**

**Cost:** 4 doubloons/man/chapter
**Loyalty:** high, and cross-reference Section 5.4: **they are defending their own homes and they do not desert.**
**Quality:** poor in the field, excellent on a wall.
**Cap:** ~8% of a settlement's population.

### 3.3 ALLIANCE FIGHTERS

**Cross-reference:** colonial_war/indigenous_liberation_alliance.md Sections 3 and 6.2.

**Source:** Liberation Strength and network trust. **Not money.**

**Cost:** **zero.** Cross-reference colonial_war/indigenous_liberation_alliance.md Section 5.7 — Alliance Bastions garrison themselves.
**Loyalty:** absolute.
**Quality:** excellent in terrain they know, poor in the open, and **no siege capability.** Cross-reference colonial_war/indigenous_liberation_alliance.md Section 8.1.
**Cap:** network strength.

**Design law:** cross-reference colonial_war/indigenous_liberation_alliance.md Principle 1.5 — **they are not Jake's.** He receives objectives, not command. Cross-reference Section 6.5.

### 3.4 FACTION LEVY

**Cross-reference:** colonial_war/colony_system_core.md Section 2.1, house_system.md Section 2.1.

**Source:** an empire, a House, or a lord, lending men.

**Cost:** none directly — **paid in obligation.** Cross-reference Section 7.
**Loyalty:** to their lord, not to Jake.
**Quality:** varies enormously and cross-reference Section 3.6.
**Cap:** the lender's willingness.

### 3.5 THE CREW

**Cross-reference:** mechanics/ship_management.md, npcs/named_crew_deep_dives.md.

**Always present. Never counted as an army.** Cross-reference Section 6.4 — **detaching crew to hold ground costs ship effectiveness, and it is a real trade, and Mira will say something.**

### 3.6 The Cultural Specialists

**Cross-reference:** house_system.md Section 5, house_belos.md Section 4.

| Source | What they are |
|---|---|
| **Belos archers** | cross-reference house_belos.md Section 4.3 — range doctrine, and after the questline **they no longer exist** |
| **Vane cavalry** | cross-reference the_lesser_houses.md Section 3.2 — small, expensive, the only shock force in Highmoore |
| **Mere watermen** | cross-reference the_lesser_houses.md Section 2.2 — **the only troops who can move through the Waterlands** |
| **Cadwall masons** | cross-reference highmoore_reconstruction.md Section 3 — not combat; **siege engineering**, and cross-reference battle_system.md Section 6 |
| **Imperial regulars** | cross-reference colonial_war/imperial_colonies.md — fortress doctrine, heavy, slow |
| **Company marines** | cross-reference colonial_war/krone_colonies.md Section 2.3 — professional, contract-bound, **and they stop when the wages stop** |

---

## 4. RECRUITMENT

### 4.1 It Is Not a Menu

**Cross-reference:** Section 1.3.

**Every force is recruited from a named source through a scene.** Cross-reference the_lesser_houses.md Section 7 — Ridd's contracts, Ashby's willingness and total lack of spare men, Vane's fraying oath.

### 4.2 The Four Approaches

- **PAY** — Ridd, mercenary companies, Company marines if Albion-aligned
- **ASK** — a House, an empire, a lord. Cross-reference Section 7 — **it costs an obligation.**
- **RAISE** — Jake's own settlements. Cross-reference colonial_war/castle_ownership.md Section 4.
- **EARN** — the Alliance. Cross-reference colonial_war/indigenous_liberation_alliance.md Section 3.3.

### 4.3 The Highmoore Case

**Cross-reference:** house_system.md Section 2.1, highmoore_reconstruction.md Section 7.2.

Highmoore levies are **seasonal.** Cross-reference highmoore_weather.md Section 4.1 — a lord raises them after harvest and disbands them before the wet, and **a levy held past the season costs double and the yeomanry resent it audibly.**

*"I'm sworn to Pennard for the levy and that's the whole of it. He doesn't own the land and he doesn't own me and if he forgets I'll remind him."*

---

## 5. UPKEEP

### 5.1 The Numbers

| Force | Doubloons/man/chapter |
|---|---|
| Hired | 14 |
| Militia | 4 |
| Alliance | 0 |
| Faction levy | 0 (obligation) |
| Crew | cross-reference mechanics/ship_management.md |
| **Cavalry (any source)** | **×3** |
| **Specialists (Cadwall, Mere)** | **×2** |

**Cross-reference economy_core.md Section 3.1** — a Force of 500 hired men costs **7,000 doubloons per chapter**, which is a Trade Post's entire income.

### 5.2 The Other Costs

- **Provisions** — cross-reference colonial_war/colony_system_core.md Section 3.2: **men eat, and a Host on the move consumes a settlement's surplus.**
- **Replacement** — cross-reference Section 9. Losses are permanent and refilling costs recruitment again.
- **Quarters** — cross-reference colonial_war/castle_ownership.md Section 6.1. Unquartered troops lose Loyalty faster.

### 5.3 DESERTION

**Cross-reference:** Section 1.1.

**One chapter unpaid:** hired troops lose 30% Loyalty. Company marines simply stop — cross-reference colonial_war/krone_colonies.md Section 2.3.
**Two chapters unpaid:** **hired troops leave**, and cross-reference npcs/regional_populations.md: **some of them rob a settlement on the way out and it is one of Jake's.**
**Militia:** do not desert. Cross-reference Section 5.4.
**Alliance:** do not desert, and cross-reference colonial_war/indigenous_liberation_alliance.md Section 3.4: **but a betrayal resets network trust to zero.**

### 5.4 LOYALTY

**No meter.** Cross-reference progression_overview.md Section 1.1.

**Read in the camp.** Cross-reference Section 8.

---

## 6. COMMAND

### 6.1 The Capacity Rule

**Cross-reference:** Section 1.2.

| Command source | Men |
|---|---|
| Jake alone | **200** |
| + a named officer | +150 each |
| + Word-branch nodes | cross-reference skill_tree.md Section 4.5 |
| + faction commander attached | their whole levy, **on their terms** |

**Exceeding capacity:** cross-reference Section 6.3 — **orders arrive late, formations break, and the battle system punishes it visibly.**

### 6.2 The Officers

**Cross-reference:** npcs/shippable_specialists.md, npcs/named_crew_deep_dives.md.

Named, recruited, and few:

- **Lt. Marcus Sandford** — cross-reference npcs/shippable_specialists.md Section 9. **The only trained land officer available to Jake**, and he is an Imperial defector, and the Alliance is not comfortable with him.
- **Big Tom** — cross-reference npcs/named_crew_deep_dives.md Section 3. Imperial military past. **He can do it and does not want to.**
- **Captain Aldana** — cross-reference colonial_war/colonial_war_bosses.md Section 13.2. If spared at Castillo Dorado.
- **Ser Rowland Vance** — **only if Jake turned west.** Cross-reference princess_quest_arion_falls.md Section 8.4.
- **Yara Sun-Walker** — cross-reference npcs/shippable_specialists.md Section 10. Alliance forces only.

### 6.3 What Jake Can Order

**Three orders. That is the entire command vocabulary.**

- **HOLD** — a line, a wall, a gate
- **PRESS** — advance on a point
- **BREAK OFF** — disengage

**Design law:** cross-reference Section 1.3, battle_system.md Section 4 — **no formations, no facing, no unit micromanagement.** Jake is a captain shouting three things, and cross-reference battle_system.md Section 4.3: **whether they are heard depends on distance, noise, and Loyalty.**

### 6.4 The Crew Detachment

Cross-reference Section 3.5, mechanics/ship_management.md.

Leaving crew ashore reduces the ship's effectiveness proportionally, and cross-reference npcs/named_crew_deep_dives.md Section 2: **Mira raises it, once, and does not raise it again.**

### 6.5 The Alliance Exception

**Cross-reference:** colonial_war/indigenous_liberation_alliance.md Principle 1.5, Section 6.2.

**Jake does not command Alliance fighters.** He receives objectives from network leadership.

**Design law:** the three orders **do not function** on Alliance units. Cross-reference colonial_war/indigenous_liberation_alliance.md Section 6.4 — *"the camera does not find Jake."*

---

## 7. OBLIGATION

**Cross-reference:** Section 4.2, house_system.md Section 4.

### 7.1 Borrowed Men Are Owed For

A House or empire that lends troops calls it in. Cross-reference colonial_war/colony_system_core.md — **an escort, a raid, a champion's duel, or a vote.**

### 7.2 The Ledger

**No interface.** Cross-reference progression_overview.md Section 1.1 — **an NPC says so, in a scene, at the worst possible time.**

### 7.3 Refusing

Cross-reference house_system.md Section 6.4 — standing collapses with that source, and cross-reference the_lesser_houses.md Section 6: **the feud map notices.**

---

## 8. THE CAMP

**Cross-reference:** Sections 1.3 and 5.4, npcs/regional_populations.md.

### 8.1 How an Army Is Read

**The player walks through it.**

| What is visible | What it means |
|---|---|
| Fires lit and evenly spaced | quartered, paid, disciplined |
| Men mending kit | high Loyalty |
| Cooking pots half-full | provisions thin |
| **A dice game with real money on it** | paid recently |
| **Men sitting apart by origin** | mixed force, low cohesion |
| Weapons stacked properly | trained |
| **Nobody on watch** | Loyalty critical |
| An empty stretch where tents were | **desertion has started** |

### 8.2 The Ambient

Cross-reference npcs/regional_populations.md — **camps have a full ambient dialogue layer**, and cross-reference style_bible.md Pillar 1: hired men, militia, and Alliance fighters do not sound alike and complain about different things.

*"...three chapters I've been paid on time. Three. I've told Ridd's lot and they don't believe me."*

*"...it's my village behind that wall. I'm not going anywhere. Ask me again in the spring."*

---

## 9. LOSSES

### 9.1 Permanent

**Cross-reference:** npcs/the_named_dead.md, colonial_war/indigenous_liberation_alliance.md Section 6.2.

**Men who die stay dead.** No replenishment pool, no auto-refill.

### 9.2 Named Losses

**Cross-reference:** colonial_war/indigenous_liberation_alliance.md Section 6.2 — *"people Jake knows die in these battles, and they go into the_named_dead.md."*

**Design law:** every battle with Alliance or militia participation kills **at least one semi-named NPC the player has met.** Cross-reference npcs/regional_populations.md — and the settlement notices, and the ambient changes.

### 9.3 Officers

Cross-reference Section 6.2, skill_tree.md Section 5.3 — **a lost officer is a lost command capacity and, for two of them, a lost teacher.**

---

## 10. IMPLEMENTATION

```cpp
struct Force {
    ForceType   type;          // Hired, Militia, Alliance, Levy, Crew
    CultureTag  culture;       // Belos, Vane, Mere, Imperial, Company, …
    uint16_t    strength;      // headcount; losses are permanent
    uint8_t     loyalty;       // 0–100, never displayed
    uint8_t     training;      // 0–100, set at recruitment, does not grow
    OfficerID   commander;     // NONE = counts against Jake's 200
    bool        acceptsOrders; // false for Alliance
};
```

**No experience. No unit levelling.** Cross-reference progression_overview.md Section 1.3 — **training is set at recruitment and never rises**, because veterans who become superhuman is exactly the pattern this project rejects.

**Tuning levers:** upkeep per head (Section 5.1) · command capacity (Section 6.1) · desertion thresholds (Section 5.3) · militia cap percentage (Section 3.2).

---

## 11. INTEGRATION MAP

- **economy_core.md** ← the largest sink; upkeep bands
- **battle_system.md, battle_catalog.md** → what armies are actually for
- **colonial_war/castle_ownership.md** ← garrison, quarters, the holding network
- **colonial_war/castle_sieges.md** ← assault elements and defensive garrisons
- **colonial_war/indigenous_liberation_alliance.md** ← Alliance fighters, and the command exception
- **house_system.md, the_lesser_houses.md, house_belos.md** ← levies, seasons, cultural specialists
- **highmoore_weather.md, highmoore_reconstruction.md** ← seasonal levies; Cadwall's masons
- **npcs/shippable_specialists.md, named_crew_deep_dives.md** ← the officers
- **skill_tree.md** ← three Word nodes affect command, none affect combat
- **npcs/the_named_dead.md, regional_populations.md** ← permanent losses; the camp's ambient

---

## 12. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** hired men, militia, Alliance fighters, and a Highmoore levy complain about four different things in four registers, and *"he doesn't own the land and he doesn't own me."*
- ✅ **Pillar 2 (Lore Through Objects):** an army read entirely by walking through a camp — fires, pots, stacked weapons, a dice game with real money on it, and an empty stretch where tents were.
- ✅ **Pillar 3 (Layered Death):** losses are permanent, every battle takes at least one semi-named person the player has met, and the settlement notices.
- ✅ **Pillar 4 (Layered Humor):** *"three chapters I've been paid on time and I've told Ridd's lot and they don't believe me."*
- ✅ **Pillar 5 (Theme — Revenge):** an army is a liability with a use. It eats forever, it leaves when unpaid and robs a settlement of Jake's on the way out, it cannot be commanded past two hundred without borrowing someone, and the best troops in the game are free, absolutely loyal, and **not his.**

---

## 13. CONCLUSION

What It Costs to Have Men. Fourteen doubloons a head a chapter, forever, whether they fight or not — a Force of five hundred consuming a Trade Post's entire income to stand in a field. Four a head for militia who are defending their own village and will not leave. Nothing at all for Alliance fighters who are excellent in ground they know, useless in the open, cannot besiege anything, and **do not take orders from Jake.**

A command vocabulary of three words — hold, press, break off — shouted by a ship's captain who can manage two hundred men by voice and needs a borrowed officer for every hundred and fifty after that, and whose only trained land officer is an Imperial defector the Alliance is not comfortable with.

No unit roster. No panel. You walk through the camp and look at the fires, the pots, the stacked weapons, whether there is a dice game with real money on it, and whether there is an empty stretch of ground where tents used to be.

And every battle takes at least one person the player has met, permanently, and the village finds out.

**Phase 9 Document 4 of 6.**

**Hold. Press. Break off. That is the entire vocabulary.** ⚔️
