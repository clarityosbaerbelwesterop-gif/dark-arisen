# PROGRESSION OVERVIEW — THE THREE AXES

**Status:** Design Draft v1.0
**Phase:** Phase 9 — Progression, Economy & Armies (Master — Document 1 of 6)
**Related:** skill_tree.md, economy_core.md, army_system.md, battle_system.md, battle_catalog.md, mechanics/combat_feel.md, mechanics/inventory_crafting.md, colonial_war/war_state_system.md, colonial_war/castle_ownership.md, highmoore_overview.md, style_bible.md
**Purpose:** The master architecture of player progression — the three non-convertible axes, the anti-patterns this project rejects by name, the readability discipline, the chapter time budget, and how skill, money, and force interlock without collapsing into one another.
**Inspirations:** Sekiro (posture, prayer beads, and a skill tree that teaches techniques rather than raising numbers), Elden Ring (world-gated power), Red Dead Redemption 2 (money that means something because there is nothing to spend it on until there is), Kingdom Come: Deliverance (skill by use), Disco Elysium (progression as characterisation), and the deliberate refusal of Ubisoft-open-world and live-service structures

---

## 1. THE DESIGN LAWS

### 1.1 The Readability Law

**Cross-reference:** colonial_war/war_state_system.md Section 8.1, highmoore_region.md Section 6.2, house_system.md Section 6.4.

One hundred and thirty documents of this bible have enforced a single discipline: **the player reads the world, not the interface.** War control, reputation, standing, colony health, and building states are all read through flags, prices, ambient dialogue, and architecture.

**Progression obeys the same law.**

- **No visible XP bar**
- **No level number**
- **No floating damage numbers**
- **No stat sheet as the primary read**

Cross-reference Section 5 — the player knows they are stronger because a fight they lost in Chapter 5 is winnable in Chapter 8, and because a smith says something different, and because a door opens.

### 1.2 The Non-Convertibility Law

**Three axes. None of them buy the others.**

- **BODY** cannot be purchased with money.
- **CRAFT** cannot be purchased with money.
- **STANDING** cannot be purchased with money, body, or craft.

**Cross-reference colonial_war/indigenous_liberation_alliance.md Principle 1.2** — Liberation Strength established this precedent: *a currency of trust, immune to the wealth that solves everything else.* **Progression generalises it.**

**Design consequence:** a rich Jake is not a strong Jake. A strong Jake is not a trusted Jake. Cross-reference Section 4 — this is what prevents the late game from flattening.

### 1.3 The Anti-Patterns, Named

This project explicitly rejects:

- **Level-gated content.** No enemy or region is closed because a number is too low. Cross-reference crystal_guardian.md Section 1.3 — the Guardian is a **skill** wall, not a level wall, and it can be walked past entirely.
- **Scaling enemies.** Cross-reference Section 5.3. Nothing in the world levels with Jake. A bandit in Chapter 9 is the same bandit.
- **Percentage-increase nodes.** No *+3% damage* anywhere in the skill tree. Cross-reference skill_tree.md Section 3.2 — **every node changes what Jake can do, not how much of it.**
- **Respec.** Cross-reference skill_tree.md Section 7.
- **Fetch-quest currency loops.** Cross-reference economy_core.md Section 6.
- **A crafting system that obsoletes found items.** Cross-reference mechanics/inventory_crafting.md.
- **Daily/weekly anything.**

### 1.4 The Slop Test

**Design law, applied to every node, price, and unit in this batch:**

*Could this line have been written without knowing anything about Dark Arisen?*

If yes, it is cut. Cross-reference skill_tree.md Section 4 — **the skill tree's nodes are taught by named people in named places**, and roughly a third of them cannot exist in any other game.

---

## 2. THE THREE AXES

### 2.1 AXIS ONE — BODY

**What it is:** Jake's physical envelope. HP, Stamina, Posture, Carry.

**Cross-reference:** mechanics/combat_feel.md — Chapter 4 baseline **HP 200 · Stamina 120 · Posture 100 · Carry 80kg.**

**How it grows:** **found objects and specific acts only.** Cross-reference skill_tree.md Section 2 — the Sekiro model. There is no point spend and no menu.

**Ceiling:** cross-reference skill_tree.md Section 2.5 — **HP 380 · Stamina 200 · Posture 175 · Carry 130kg**, reachable only by a player who has explored thoroughly across both continents.

**Growth over the game:** roughly **+90% HP, +67% Stamina, +75% Posture.** Cross-reference Section 5.2 — deliberately modest, so that a Chapter 4 threat is still a threat.

### 2.2 AXIS TWO — CRAFT

**What it is:** techniques, methods, and knowledge. The skill tree proper.

**Cross-reference:** skill_tree.md Sections 3-4.

**How it grows:** **Marks**, earned by use and by specific acts, spent on nodes — **and roughly a third of all nodes additionally require a named teacher.**

**Ceiling:** cross-reference skill_tree.md Section 6 — **a single playthrough cannot reach every node**, because several teachers are mutually exclusive.

### 2.3 AXIS THREE — STANDING

**What it is:** what the world opens.

**Cross-reference:** colonial_war/indigenous_liberation_alliance.md Section 3.5 (trust tiers), house_system.md Section 6.4, colonial_war/war_state_system.md.

**How it grows:** deeds, witnessed. **No currency, no points, no meter.**

**What it unlocks:** cross-reference colonial_war/indigenous_liberation_alliance.md Section 5 — ancient maps, feared waters, secret sites, the sages' depths, Highmoore's gates, faction contracts, and **eleven skill-tree nodes that no amount of Marks will open.**

### 2.4 The Interlock

```
BODY   ← objects, exploration, specific acts
CRAFT  ← use + Marks, gated by teachers, gated by STANDING for 11 nodes
STANDING ← deeds, witnessed
MONEY  ← everything, and it buys none of the three
```

**Cross-reference economy_core.md Section 3** — money buys equipment, ships, settlements, armies, and information. **It does not buy capability.**

---

## 3. WHAT MONEY IS FOR

**Cross-reference:** economy_core.md.

### 3.1 The Problem Money Usually Has

In most open-world games money becomes meaningless by the midpoint because the sinks run out.

### 3.2 The Four Sinks That Do Not Run Out

- **ARMIES.** Cross-reference army_system.md Section 5 — **upkeep is per-chapter and permanent**, and a standing force is the largest recurring drain in the game.
- **HOLDINGS.** Cross-reference colonial_war/castle_ownership.md Section 5.1 — Military Strongholds run at **−5,000 to −20,000 doubloons per chapter, forever.**
- **RECONSTRUCTION.** Cross-reference highmoore_reconstruction.md — a basin-scale building project with no completion bonus.
- **THE LEGENDARY TIER.** Cross-reference npcs/blacksmiths_and_armorers.md Section 9.5 — Pak Setyo and Bu Wira work in materials, reputation, and time, and money is the *least* of what they need.

### 3.3 The Design Statement

**Cross-reference:** colonial_war/colony_system_core.md Section 8.

The Support player gets rich slowly and safely. The Break player gets rich fast and dangerously. **The Liberate player gets poor and powerful.**

**And all three of them can afford the same sword, and none of them can afford an army for more than a few chapters without a working economy behind it.**

---

## 4. WHY THE LATE GAME DOES NOT FLATTEN

### 4.1 The Standard Failure

By the final act of most open-world games the player has everything, and encounters become formalities.

### 4.2 The Three Defences

**BODY IS CAPPED LOW.** Section 2.1 — +90% HP across an entire game. Cross-reference Section 5.2.

**CRAFT IS INCOMPLETE.** Cross-reference skill_tree.md Section 6 — mutually exclusive teachers mean **no playthrough sees the whole tree.**

**STANDING IS ZERO-SUM ACROSS FACTIONS.** Cross-reference colonial_war/war_state_system.md Section 2.2 — feeding the Empire costs Liberation Strength. **A player cannot max everything, and the game never pretends they can.**

### 4.3 The Fourth Defence

**Cross-reference:** colonial_war/retaliation_system.md Section 2.1.

**Retaliation scales with success.** A powerful Jake is a hunted Jake with burning settlements. Cross-reference colonial_war/retaliation_system.md Section 8.3 — **the system prices power rather than capping it.**

---

## 5. THE POWER CURVE

### 5.1 The Shape

| Chapter | Body | Craft | Standing | Money |
|---|---|---|---|---|
| 4 | baseline | 0 Marks | none | ~2,000 |
| 5-6 | +15% | 8-14 nodes | first tiers | 15-60k |
| 7-8 | +45% | 20-32 nodes | deep with 1-2 factions | 80-400k |
| 9-10 | +70-90% | 34-48 nodes | terminal in chosen paths | 200k-2M+ |

**Cross-reference economy_core.md Section 4** — the money range at Chapter 10 is enormous by design and cross-reference Section 3.2: **most of it is committed.**

### 5.2 The Threat Curve Does Not Follow

**Cross-reference:** Section 1.3 (no scaling enemies).

Chapter 4 bandits stay Chapter 4 bandits. **What changes is what the world sends.** Cross-reference colonial_war/retaliation_system.md Section 3 — the empires escalate from letters to named assassins to fleets, and cross-reference colonial_war/assassin_network.md: **the hunters are the difficulty curve, and they are a consequence, not a level band.**

### 5.3 What Never Becomes Trivial

- **Admiral Sterling.** Cross-reference colonial_war/colonial_war_bosses.md Section 10.
- **The Crystal Guardian.** Cross-reference crystal_guardian.md Section 1.3.
- **The Ashen Wyrm.** Cross-reference bosses/ashen_wyrm.md.
- **A hundred archers in an enclosed courtyard.** Cross-reference princess_quest_belos.md — **Jake is badly hurt by minute six even with the Crystal Katana**, and cross-reference crystal_katana.md Section 4.2: the sword's reach is 74cm and its answer to range is to get closer.

---

## 6. THE CHAPTER TIME BUDGET

**Cross-reference:** colonial_war/war_state_system.md Section 5 (momentum phases), highmoore_overview.md Section 3.2.

### 6.1 The Target

| Chapter | Main story | Optional available | Realistic play |
|---|---|---|---|
| 4 | 3-4h | 12h | 8-14h |
| 5 | 3-4h | 20h | 10-18h |
| 6 | 4-5h | 30h | 12-25h |
| 7 | 5-6h | 45h | 15-35h |
| 8 | 5-6h | 50h | 15-40h |
| 9 | 6-7h | 40h | 15-30h |
| 10 | 8-10h | 25h | 12-25h |

**Critical path: ~38 hours. Thorough single playthrough: 110-160 hours. Completionist: 220h+.**

### 6.2 Phase Placement

- **Phase 7 (Colonial War)** — Chapters 4-10, escalating. Cross-reference colonial_war/war_state_system.md Section 5.
- **Phase 8 (Highmoore)** — Chapters 6-8 recommended. Cross-reference highmoore_overview.md Section 3.2 — **before Rache unlocks at Chapter 9, deliberately.**
- **Armies** — Chapter 6+. Cross-reference army_system.md Section 2.1.
- **Large battles** — Chapter 7+. Cross-reference battle_system.md Section 2.

### 6.3 The Rache Gate

**Cross-reference:** mechanics/combat_feel.md, highmoore_overview.md Section 3.2.

Rache unlocks Chapter 9. **This is a progression decision, not a story one:** cross-reference princess_quest_belos.md — the House Belos massacre must be Jake **without** his vengeance mechanic, because a mercy-free power tool would make nine minutes read as designed catharsis rather than collapse.

---

## 7. THE DATA MODEL

**Design note:** cross-reference Section 1.4 — these documents are written to be implementable, and the following is the shape an engineer needs.

```cpp
struct PlayerProgression {
    // AXIS ONE — BODY (no point pool; grown by world objects/acts)
    int maxHP;              // 200 → 380
    int maxStamina;         // 120 → 200
    int maxPosture;         // 100 → 175
    float carryKg;          // 80  → 130
    BitSet bodyUnlocks;     // which objects/acts have been consumed

    // AXIS TWO — CRAFT
    int marksEarned;        // lifetime
    int marksSpent;
    BitSet nodesUnlocked;   // 68 nodes total
    BitSet teachersMet;     // 23 named teachers

    // AXIS THREE — STANDING (no numeric surface to the player)
    FactionStanding imperial, albion, liberation, pirate, highmoore;
    TrustTier networkTrust[5];   // Alliance networks
    BitSet worldFlags;           // gates, doors, invitations
};
```

**Design law:** cross-reference Section 1.1 — **none of these values are ever displayed as numbers to the player**, including in a stat menu. Cross-reference skill_tree.md Section 8 for what the menu shows instead.

---

## 8. INTEGRATION MAP

- **skill_tree.md** → the three axes in mechanical detail; the 68 nodes; the 23 teachers
- **economy_core.md** → three currencies, income, sinks, pacing
- **army_system.md, battle_system.md, battle_catalog.md** → the largest money sink and what it is for
- **mechanics/combat_feel.md** ← the baseline, posture, Rache
- **mechanics/inventory_crafting.md** ← equipment, which money buys, and capability, which it does not
- **colonial_war/war_state_system.md** ← the meters the player never sees; momentum phases
- **colonial_war/indigenous_liberation_alliance.md** ← trust as a currency immune to wealth; the precedent for Axis Three
- **colonial_war/retaliation_system.md** ← difficulty as consequence rather than level band
- **colonial_war/castle_ownership.md, highmoore_reconstruction.md** ← the bottomless sinks
- **crystal_guardian.md, crystal_katana.md** ← skill walls, not level walls; a weapon that is capability, not a stat

---

## 9. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** cross-reference skill_tree.md Section 4 — a third of all progression comes from being taught by a named person, and each of them teaches in their own register.
- ✅ **Pillar 2 (Lore Through Objects):** Axis One is entirely objects. Cross-reference skill_tree.md Section 2 — **there is no menu in which Jake becomes stronger**, only things found and things done.
- ✅ **Pillar 3 (Layered Death):** cross-reference Section 5.3 — nothing becomes trivial, and the things that stay dangerous are the ones the bible has already made heavy.
- ✅ **Pillar 4 (Layered Humor):** cross-reference skill_tree.md Section 4 — Big Tom teaching Jake to fight dirty, and Gruffydd offering to sell him a bow that suits a wrong draw, are progression content.
- ✅ **Pillar 5 (Theme — Revenge):** money buys equipment, ships, settlements, and armies, and **it does not buy a single point of capability, a single technique, or a single door.** Cross-reference Section 1.2 — the three things Jake actually needs are earned by doing, by being taught, and by being trusted.

---

## 10. CONCLUSION

The Three Axes. Body that grows from objects found and things done and never from a menu, capped at plus ninety percent across an entire game so that a Chapter 4 threat is still a threat in Chapter 10. Craft earned in Marks and spent on techniques, a third of which require a named person in a named place, eleven of which no amount of Marks will open, and none of which is a percentage.

Standing that is not a currency at all, that no wealth touches, and that decides which doors exist. And money — which buys ships, castles, cannon, and three thousand men under arms, and does not buy one point of any of it.

No XP bar. No level. No floating numbers. No scaling enemies. No respec. No node in the tree that could have been written without knowing what this game is.

**Phase 9 Document 1 of 6.**

**Rich is not strong. Strong is not trusted. Nothing converts.** ⚖️
