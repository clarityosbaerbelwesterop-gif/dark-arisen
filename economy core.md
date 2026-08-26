# ECONOMY CORE — THREE CURRENCIES, FOUR SINKS

**Status:** Design Draft v1.0
**Phase:** Phase 9 — Progression, Economy & Armies (Document 3 of 6)
**Related:** progression_overview.md, skill_tree.md, army_system.md, colonial_war/colony_system_core.md, colonial_war/castle_ownership.md, colonial_war/krone_colonies.md, highmoore_reconstruction.md, highmoore_smiths.md, npcs/general_merchants.md, npcs/luxury_merchants.md, npcs/blacksmiths_and_armorers.md, npcs/black_market_fences.md, mechanics/inventory_crafting.md, mechanics/ship_management.md, style_bible.md
**Purpose:** The money system — three currencies that do not convert cleanly, the income model, the four bottomless sinks, the chapter-by-chapter pacing target, price anchoring against the existing bible, and the design law that prevents late-game trivialisation.
**Inspirations:** Red Dead Redemption 2 (money that matters because the sinks are real), Mount & Blade (an economy of upkeep), Kingdom Come: Deliverance (prices with weight), Age of Sail prize economics, the East India Company's actual books, and the explicit rejection of open-world money bloat

---

## 1. THE THREE CURRENCIES

### 1.1 Why Three

**Cross-reference:** colonial_war/krone_colonies.md, house_system.md, highmoore_smiths.md Section 2.2.

The bible already established them without calling them a system:

- **DOUBLOONS** — the archipelago. Imperial, pirate, and general trade.
- **POUNDS** — Albion. Cross-reference colonial_war/retaliation_system.md Section 3.2 — **the Company invoices in pounds**, and cross-reference colonial_war/assassin_network_questline.md Section 5.4: Bram Kettle costed a blood feud at just under nine hundred of them.
- **SILVER MARKS** — Highmoore. Cross-reference the_lesser_houses.md Section 3.3 — *"whoever comes with silver."*

### 1.2 They Do Not Convert Cleanly

**Design law:** there is no exchange menu.

| From → To | Where | Loss |
|---|---|---|
| Doubloons ↔ Pounds | HAC offices, Bahía Dorada banking | **12-18%** |
| Doubloons ↔ Silver | **nowhere** | — |
| Pounds ↔ Silver | **nowhere** | — |

**Cross-reference highmoore_endgame_state.md Section 3.1** — the archipelago does not know Highmoore exists. **There is no exchange because there is no trade.**

### 1.3 What This Produces

**Cross-reference:** highmoore_reconstruction.md Section 4.3.

A player with two million doubloons **cannot buy a bridge at the ford.** Silver is earned in Highmoore, by working in Highmoore, and cross-reference Section 5.4: the reconstruction runs on a currency the rest of the game does not touch.

**Full Vision:** cross-reference highmoore_reconstruction.md Section 4.3 — **the mountain supply line is the only conversion that ever exists, and building it is late-game content.**

---

## 2. PRICE ANCHORING

**Cross-reference:** the bible's established figures, which this document ratifies rather than replaces.

### 2.1 The Doubloon

| Thing | Cost |
|---|---|
| A meal | 5-15 |
| A night's lodging | 50-500 |
| Village smith repair | 50-200 |
| Common medicine | 20-100 |
| Town-quality weapon | 200-1,000 |
| City master weapon | 1,000-5,000 |
| Regional master work | 5,000-25,000 |
| A fine villa | 50,000-250,000 |
| Castle repair to Functional | 3,000-8,000 |
| Castle to Developed | 30,000-70,000 |
| Fortification, Redoubtable | 100,000+ |

**Cross-reference npcs/blacksmiths_and_armorers.md Section 10.1, apothecaries_and_alchemists.md Section 10.1, general_merchants.md Section 8.1, luxury_merchants.md Section 8.1, colonial_war/castle_ownership.md Section 3.2.**

### 2.2 The Pound

**Roughly 40 doubloons.** Cross-reference colonial_war/retaliation_system.md Section 3.2 — the £6,600 invoice is a **264,000-doubloon** demand, which is why it lands.

### 2.3 The Silver Mark

**Roughly 8 doubloons in purchasing power, and there is no way to check.**

| Thing | Marks |
|---|---|
| A meal at an inn | 1-2 |
| Thorn arming sword | 60-90 |
| Thorn plate harness | 300-450 |
| Belos warbow (Gruffydd) | 40-70 |
| A season's levy pay, one village | 200 |
| Cadwall masons, one castle wing | 4,000-9,000 |
| **The bridge at the ford** | **28,000** |

**Cross-reference highmoore_reconstruction.md Section 3.5** — *"I'll want a great deal of money and I would like it noted that I said yes immediately."* **That is 28,000 silver marks and Lord Gwilym is not joking.**

---

## 3. INCOME

### 3.1 The Four Streams

**PLUNDER** — cross-reference colonial_war/colony_system_core.md Section 8.2. Burst, large, and it raises Retaliation Heat.

| Target | Doubloons |
|---|---|
| Supply convoy | 800-3,000 |
| Materiel convoy | 2,000-6,000 |
| Treasure fleet | 25,000-90,000 |
| Castle treasury (razed) | 40,000-400,000 |
| **Castillo Dorado's vault** | **~600,000** |

**CONTRACTS** — cross-reference colonial_war/colony_system_core.md Section 2.1. Steady, safe, reputation-building.

| Work | Doubloons |
|---|---|
| Escort, short | 600-1,500 |
| Escort, treasure fleet | 8,000-20,000 |
| Settler delivery | 1,200 + villa progress |
| Privateer contract | 5,000-25,000 |
| Champion in a settlement (Highmoore) | 150-400 **marks** |

**HOLDINGS** — cross-reference colonial_war/castle_ownership.md Section 5.1.

| Type | Per chapter |
|---|---|
| Trade Post | **+3,000 to +12,000** |
| Settlement (production) | **+2,000 to +15,000** |
| Settlement (free/refugee) | **−1,000 to +2,000** |
| Military Stronghold | **−5,000 to −20,000** |
| Alliance Bastion | **0** |

**SALVAGE AND RECOVERY** — cross-reference colonial_war/regional_dungeon_catalog.md Section 7.2. **Dungeons do not pay in doubloons**, deliberately, and cross-reference colonial_war/dungeon_design_philosophy.md Section 7.2: they pay in materials, maps, keys, and lore, **which keeps them relevant to a rich Jake.**

### 3.2 The Gratitude Economy

**Cross-reference:** colonial_war/indigenous_liberation_alliance.md Section 5.5.

Liberated regions trade at **30-50% off**, offer free healing and repair, and stock goods no money buys elsewhere.

**Design consequence:** the Liberate player has the lowest income in the game and **the lowest costs**, and cross-reference progression_overview.md Section 3.3: *poor and powerful.*

---

## 4. THE PACING TARGET

| Chapter | Liquid doubloons (typical) | Committed |
|---|---|---|
| 4 | 500-3,000 | — |
| 5 | 4,000-18,000 | — |
| 6 | 15,000-60,000 | first holding |
| 7 | 40,000-180,000 | 1-2 holdings, first troops |
| 8 | 60,000-350,000 | holdings + standing force |
| 9 | 80,000-800,000 | full network |
| 10 | 100,000-2,000,000+ | **most of it obligated** |

**Design law:** cross-reference progression_overview.md Section 3.2. **A player at two million doubloons in Chapter 10 with a four-holding network and a standing army is running a deficit**, and that is the intended end state of the acquisitive path.

---

## 5. THE FOUR BOTTOMLESS SINKS

### 5.1 ARMIES

**Cross-reference:** army_system.md Section 5.

**The largest recurring drain in Dark Arisen.** Per chapter, per soldier, forever, and cross-reference army_system.md Section 5.3: **unpaid troops leave, and some of them take things.**

### 5.2 HOLDINGS

Cross-reference Section 3.1, colonial_war/castle_ownership.md Section 7.3 — **overextension is a real ceiling**, and cross-reference colonial_war/retaliation_system.md Section 3.4: **more holdings means more places to be raided at once.**

### 5.3 THE LEGENDARY TIER

**Cross-reference:** npcs/blacksmiths_and_armorers.md Section 9.5, apothecaries_and_alchemists.md Section 9.5.

Pak Setyo and Bu Wira work in **materials, reputation, and time**, and cross-reference crystal_katana.md Section 5.1: **the best weapon in the game cannot be bought, upgraded, or serviced by anyone.**

**Design note:** money is the *least* important input at this tier, which is the point.

### 5.4 RECONSTRUCTION

**Cross-reference:** highmoore_reconstruction.md, colonial_war/castle_ownership.md Section 3.

A basin-scale project in a currency that does not convert, with **no completion bonus, no achievement, and nobody thanking him.** Cross-reference highmoore_reconstruction.md Section 1.1.

---

## 6. WHAT THE ECONOMY REFUSES

**Cross-reference:** progression_overview.md Section 1.3.

- **No fetch-loops.** No merchant who buys unlimited pelts. Cross-reference npcs/general_merchants.md — **stock and demand are finite per settlement per chapter.**
- **No vendor trash.** Cross-reference mechanics/inventory_crafting.md — if it can be picked up it has a use.
- **No gold-per-hour grind.** Every income stream has a cooldown, a reputation cost, or a Heat cost.
- **No paid respec.** Cross-reference skill_tree.md Section 7.
- **No buying capability.** Cross-reference progression_overview.md Section 1.2.
- **No fast-travel fee.** Cross-reference mounted_travel.md Section 6.1 — there is no fast travel in Highmoore at any price.

---

## 7. THE MORAL PRICING

**Cross-reference:** colonial_war/colony_system_core.md Section 5.3, krone_colonies.md Section 6.9.

### 7.1 The Two Best-Paying Contracts in the Game

- **Escorting the Nueva Esperanza Labor Line.** Cross-reference imperial_colonies.md Section 4.9.
- **Escorting the Blackwood Landing Contract Line.** Cross-reference krone_colonies.md Section 6.9.

**Both pay roughly 40% above any comparable escort.**

### 7.2 The Design Law

**Cross-reference:** colonial_war/colony_system_core.md Section 5.3.

**The game does not prevent it, does not warn, and does not editorialise.** It pays, and cross-reference the sage reactions: Mama Jacinta stops receiving him, Ines does not look at him for a while, and Father Salvio asks once.

### 7.3 The Ethical Plantation

**Cross-reference:** colonial_war/castle_ownership.md Section 5.3.

**Output 60-70% of a forced-labour equivalent. Costs significantly higher. Net profit substantially lower.**

**Design law:** cross-reference colonial_war/castle_ownership.md Section 5.3 — *"making it secretly optimal would be a lie."* **It is not secretly optimal. It stays worse, forever, and the player chooses it knowing.**

---

## 8. IMPLEMENTATION

```cpp
struct Wallet {
    int64_t doubloons;
    int64_t pounds;      // HAC territories only
    int64_t silverMarks; // Highmoore only
};

struct ChapterLedger {
    int64_t holdingIncome;     // sum of owned assets
    int64_t armyUpkeep;        // negative, see army_system.md §5
    int64_t garrisonUpkeep;    // negative
    int64_t constructionDraw;  // negative, scheduled
    // no interest, no investment vehicles, no compounding
};
```

**No compounding.** Cross-reference Section 6 — **there is no way to make money make money except by owning things that produce**, and those things cost upkeep and can be burned.

**Tuning levers, in order of sensitivity:** holding income bands (Section 3.1) · army upkeep per head (army_system.md Section 5.1) · plunder magnitudes (Section 3.1) · the pound rate (Section 2.2).

---

## 9. INTEGRATION MAP

- **progression_overview.md** ← money buys none of the three axes
- **skill_tree.md** ← no node has a price
- **army_system.md** → the largest sink, in detail
- **colonial_war/colony_system_core.md** ← the three verbs' economic identities
- **colonial_war/castle_ownership.md** ← holding income and upkeep bands, and the ethical plantation
- **colonial_war/krone_colonies.md, retaliation_system.md** ← pounds, invoices, the settlement clause
- **highmoore_reconstruction.md, highmoore_smiths.md** ← silver marks and the bridge
- **npcs/general_merchants.md, luxury_merchants.md, blacksmiths_and_armorers.md, apothecaries_and_alchemists.md, black_market_fences.md** ← every price band in Section 2.1
- **colonial_war/dungeon_design_philosophy.md** ← dungeons do not pay in coin
- **colonial_war/indigenous_liberation_alliance.md** ← the gratitude economy

---

## 10. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** three currencies, three commercial registers — Imperial haggling, Company invoicing, and a Highmoore lord saying he wants a great deal of money and would like it noted that he said yes immediately.
- ✅ **Pillar 2 (Lore Through Objects):** an invoice in pounds nailed to a mast. A vault under a palace with gardens on its firing platform. Twenty-eight thousand silver for a bridge at a ford where four generations drowned.
- ✅ **Pillar 3 (Layered Death):** the two best-paying escort contracts in the game are people-convoys, and the game pays without comment.
- ✅ **Pillar 4 (Layered Humor):** Gwilym Cadwall's negotiation. Kettle costing a vendetta at just under nine hundred pounds including the letters.
- ✅ **Pillar 5 (Theme — Revenge):** two million doubloons and he still cannot buy a bridge, a technique, a door, or one point of anything — and the ethical plantation stays worse forever and the game never once pretends otherwise.

---

## 11. CONCLUSION

Three Currencies, Four Sinks. Doubloons that buy the archipelago, pounds that arrive as an itemised demand nailed to a mast, and silver marks that exist in a basin the rest of the world does not know is there — with no exchange between the third and the other two, because there is no trade, because there is no road.

Prices anchored to a hundred and thirty documents: fifty for a repair, five thousand for a master's work, a quarter of a million for a villa, six hundred thousand in a vault under a palace with gardens on its firing platform, and twenty-eight thousand silver for a bridge at a ford, from a man who wants it noted that he said yes immediately.

Four sinks that never close: soldiers who eat every chapter forever, holdings that run at a loss and can be burned, a legendary tier where money is the least of what is needed, and a basin-scale reconstruction with no completion bonus and nobody thanking him.

No compounding. No investment. No grind. No vendor trash. No paid respec. No buying a single technique, a single door, or a single point of anything.

**Phase 9 Document 3 of 6.**

**Two million doubloons and he still cannot cross a river.** 💰
