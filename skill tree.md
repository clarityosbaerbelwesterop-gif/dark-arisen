# THE SKILL TREE — MARKS, TEACHERS, AND WHAT CANNOT BE BOUGHT

**Status:** Design Draft v1.0
**Phase:** Phase 9 — Progression, Economy & Armies (Document 2 of 6)
**Related:** progression_overview.md, economy_core.md, army_system.md, mechanics/combat_feel.md, mechanics/inventory_crafting.md, mechanics/stealth.md, physics/combat_physics.md, physics/ship_physics.md, physics/movement_physics.md, crystal_katana.md, mounted_travel.md, npcs/named_crew_deep_dives.md, npcs/blacksmiths_and_armorers.md, colonial_war/indigenous_liberation_alliance.md, style_bible.md
**Purpose:** The complete progression system — Body growth by object, the Marks economy, the 68 nodes across five branches, the 23 named teachers, the eleven Standing-gated nodes, the mutually exclusive paths, and the menu that shows no numbers.
**Inspirations:** Sekiro (prayer beads and gourd seeds; a tree that teaches techniques), Elden Ring (world-gated power), Kingdom Come: Deliverance (learning from named masters), Ghost of Tsushima (stances taught by people), and the explicit rejection of percentage-node design

---

## 1. THE ARCHITECTURE

### 1.1 Three Systems, One Screen

**Cross-reference:** progression_overview.md Section 2.

- **BODY** — objects and acts. No spend.
- **CRAFT** — 68 nodes, Marks, teachers.
- **STANDING** — no interface at all; it gates eleven Craft nodes and opens doors.

### 1.2 The Node Law

**Cross-reference:** progression_overview.md Section 1.3.

**Every node changes what Jake can do. No node changes how much of it.**

There is no *+damage*, no *+resistance*, no *+carry weight*, and no *+critical chance* anywhere in the tree. **Sixty-eight nodes, sixty-eight verbs.**

---

## 2. AXIS ONE — BODY

### 2.1 The Model

**Cross-reference:** mechanics/combat_feel.md — baseline Chapter 4: **HP 200 · Stamina 120 · Posture 100 · Carry 80kg.**

Growth comes from **four object families and a small number of acts.** There is no menu.

### 2.2 THE VITAL — HP

**Object:** cross-reference npcs/apothecaries_and_alchemists.md — a **Physician's Draught**, prepared once, permanently, by a Tier 4+ practitioner from materials the player brings.

**Count:** 12 across the game.
**Value:** +15 HP each.
**200 → 380.**

**Where:** Doctor Brandt (2), Doña Mercedes (2), Helga Thorsdotter (1), **Bu Wira (2 — legendary, and she takes months)**, Madame Celestine (1), Doc Wallace (1), and **3 in dungeons.** Cross-reference colonial_war/regional_dungeon_catalog.md.

**Design note:** cross-reference npcs/apothecaries_and_alchemists.md — **Ines can prepare none of them**, and she says why, and it is a good scene.

### 2.3 THE WIND — Stamina

**Object:** **Deep-Water Pearls.** Cross-reference physics/water_physics.md, colonial_war/indigenous_liberation_alliance.md Section 5.3 — **found only underwater**, and cross-reference the feared waters: **six of the sixteen require Alliance routes.**

**Count:** 16. **Value:** +5 each. **120 → 200.**

### 2.4 THE SET — Posture

**Cross-reference:** mechanics/combat_feel.md (the Sekiro pillar).

**Not an object. An act.**

**Posture grows by perfect deflection against named opponents.** Cross-reference colonial_war/colonial_war_bosses.md, crystal_guardian.md — **each Tier 1 boss and each named assassin permanently grants +5 Posture the first time Jake deflects three of their attacks perfectly in one encounter**, whether or not he wins.

**Count:** 15 available. **100 → 175.**

**Design note:** cross-reference crystal_guardian.md Section 4.3 — **the Guardian is the best teacher of this in the game and does not know it**, and a player who fights it twice will feel the difference.

### 2.5 THE BACK — Carry

**Cross-reference:** mechanics/inventory_crafting.md.

**Acts, not objects:** long voyages completed, cargo hauled, and cross-reference colonial_war/colony_system_core.md Section 3 — **people carried.**

**80kg → 130kg.** Cross-reference physics/movement_physics.md — and it is the least interesting axis and the design does not pretend otherwise.

### 2.6 The Ceiling

**HP 380 · Stamina 200 · Posture 175 · Carry 130kg.**

**Cross-reference progression_overview.md Section 4.2** — reaching it requires both continents, six feared-water regions, and fifteen boss encounters. **Roughly 4% of players will.**

---

## 3. AXIS TWO — MARKS

### 3.1 What a Mark Is

Cross-reference progression_overview.md Section 1.1 — **not experience points.** There is no bar, no gain notification, and no total displayed anywhere.

**A Mark is a thing understood.** The player is told they have one when they have one, in plain text, once.

### 3.2 How Marks Are Earned

| Source | Marks | Cap |
|---|---|---|
| First perfect-deflect chain vs. a named boss | 1 | 15 |
| Defeating a Tier 1 boss (any resolution incl. avoided) | 1 | 9 |
| Completing a dungeon (Tier B+) | 1 | 25 |
| Naval engagement won against a superior force | 1 | 8 |
| A crew member's personal questline | 2 | 10 |
| A sage's deep tier reached | 2 | 8 |
| First time each weapon class lands 100 hits | 1 | 7 |
| Specific acts (see Section 3.3) | 1 | 12 |

**Theoretical maximum: 94. Total node cost of the full tree: 141.**

**Cross-reference Section 6 — the tree cannot be completed. Not by anyone. Ever.**

### 3.3 The Specific Acts

Cross-reference progression_overview.md Section 1.4 (the slop test). Twelve one-off Marks for things only this game contains:

- Crossing the Ashenmoor desert alone on foot. Cross-reference physics/sand_desert_physics.md.
- Surviving a Fjordlund winter storm at sea without losing a mast. Cross-reference physics/ship_physics.md.
- Reading an ancient map in its own map-language. Cross-reference colonial_war/indigenous_liberation_alliance.md Section 5.2.
- Completing the Crystal Caves without lighting a torch past the Cathedral. Cross-reference crystal_caves.md Section 6.1.
- Reaching Bond 3 with a horse. Cross-reference mounted_travel.md Section 3.1.
- Returning twenty recovered artifacts rather than fencing them. Cross-reference npcs/black_market_fences.md Section 6.2.
- (six further, cross-reference the phase documents)

### 3.4 The Notification

**Design law:** cross-reference progression_overview.md Section 1.1.

**No pop-up. No sound. No screen flash.**

The next time Jake rests — cross-reference mechanics/ship_management.md, the great cabin — a single line appears in the corner and stays for four seconds:

*You have learned something.*

That is the entire feedback system.

---

## 4. THE FIVE BRANCHES

**68 nodes. 141 Marks to open all of them. 94 available. 23 named teachers.**

### 4.1 BRANCH ONE — THE BLADE (16 nodes)

Cross-reference mechanics/combat_feel.md, physics/combat_physics.md.

**Sample nodes:**

**THE SEAMAN'S GUARD** *(1 Mark, no teacher)* — deflect while moving backward without losing footing. Cross-reference physics/movement_physics.md.

**THE MERCY** *(2 Marks, teacher: Father Salvio)* — a non-lethal takedown on a broken-posture human opponent. **Cross-reference princess_quest_belos.md Section 5.4 — it does not function in the Belos Outer Court, and the design does not explain why, and it is the only place in the game where a learned technique is silently disabled.**

**THE OFFICER'S LINE** *(3 Marks, teacher: Don Alejandro)* — Imperial naval fencing. Cross-reference npcs/cigar_collector_don_alejandro.md Section 3.2: **he was a captain for thirty-three years and taught it to a son who died with it.** Teaching Jake takes three sessions across three chapters and the third one is not about fencing.

**BIG TOM'S ANSWER** *(1 Mark, teacher: Big Tom)* — headbutt, elbow, and the boarding-axe grip. Cross-reference npcs/named_crew_deep_dives.md Section 3.

**THE LONG NIGHT** *(4 Marks, teacher: none, requires Posture 150+)* — posture regenerates while blocking instead of draining. **The single strongest node in the tree** and cross-reference Section 2.4: **it requires fifteen boss deflection chains, which means fighting almost everything in the game.**

### 4.2 BRANCH TWO — THE SHADOW (12 nodes)

Cross-reference mechanics/stealth.md, physics/sound_physics.md Section 12.

**Sample nodes:**

**SOUND DISCIPLINE** *(1 Mark)* — footfall volume drops by surface. Cross-reference physics/sound_physics.md Section 12.2.

**THE COUNTING HALLS** *(2 Marks, teacher: Mateusz Voltari)* — move through occupied interiors at walking pace without detection. Cross-reference colonial_war/castle_sieges.md Section 4.2 — **named for the Coventry House sequence and unlocked before it if Mateusz was recruited.**

**THE FOURTH RULE** *(2 Marks, STANDING-GATED: Highmoore)* — cross-reference light_elves.md Section 4.1. **It is not a stealth technique. It is knowing not to answer**, and cross-reference light_elf_boss.md Section 5.5: **a player who has this node survives the shaft's asking automatically.**

### 4.3 BRANCH THREE — THE SEA (14 nodes)

Cross-reference physics/ship_physics.md, physics/wind_systems.md, mechanics/ship_navigation.md.

**Sample nodes:**

**ESTEBAN'S SKY** *(2 Marks, teacher: Esteban)* — night navigation without instruments. Cross-reference npcs/named_crew_deep_dives.md Section 6, physics/sun_shadow_systems.md Section 6.2.

**THE WEATHER GAUGE** *(3 Marks, teacher: Mira OR Admiral Sterling)* — hold the windward position through a manoeuvring engagement. **Cross-reference Section 6.2 — two teachers, one node, and Sterling's version is better and requires the alliance path.**

**HOT SHOT** *(2 Marks, teacher: Maeve "Hammer" Donovan)* — cross-reference physics/fire_physics.md Section 7.2, colonial_war/castle_sieges.md Section 3.2.

### 4.4 BRANCH FOUR — THE LAND (13 nodes)

Cross-reference mounted_travel.md, physics/movement_physics.md, and the regional physics documents.

**Sample nodes:**

**THE HOT STONE WALK** *(2 Marks, teacher: Mbah Seruni, STANDING-GATED)* — cross-reference npcs/desert_sage_mbah_seruni.md Section 4.7, physics/lava_heat_physics.md. Heat damage floor reduced to zero on volcanic stone.

**COLD IS A TEACHER** *(2 Marks, teacher: Mormor Astrid)* — cross-reference npcs/healer_mormor_astrid.md, physics/snow_ice_physics.md. **Hypothermia accrual halved, and she does not teach it as a technique. She teaches it as an attitude, over tea, in about eleven minutes.**

**THE LINE** *(3 Marks, requires Horse Bond 3)* — cross-reference mounted_travel.md Section 5.2.

**THE CANOPY** *(2 Marks, STANDING-GATED: Cultural Web Tier 3)* — vine traversal. Cross-reference physics/vegetation_physics.md Section 4.

### 4.5 BRANCH FIVE — THE WORD (13 nodes)

Cross-reference npcs/dialogue_system.md, colonial_war/, house_system.md.

**Sample nodes:**

**THE REGISTER** *(2 Marks, teacher: Margarethe Fitzmueller OR Don Alejandro)* — cross-reference style_bible.md Pillar 1. **Jake can speak High.** Doors in Pale Isle, Silvera, and Highmoore's halls.

**THE INVOICE** *(2 Marks, teacher: Bram Kettle, requires the Regulator turn)* — cross-reference colonial_war/assassin_network_questline.md Section 5.5. Read Company paperwork correctly, and cross-reference krone_colonies.md Section 3: **spot a Profitability collapse two chapters early.**

**WHOSE MAN** *(1 Mark, STANDING-GATED: Highmoore)* — cross-reference house_system.md Section 6.1. **Answer the question in a way that works**, and it does not involve telling the truth.

**THE LEDGER** *(3 Marks, teacher: Ines)* — cross-reference krone_colonies.md Section 6.12. **Read a labour contract and understand the arithmetic in under a minute**, which is what she does, and cross-reference npcs/named_crew_deep_dives.md Section 4: she teaches it angrily.

---

## 5. THE TEACHERS

### 5.1 The Twenty-Three

**Crew (5):** Mira · Big Tom · Ines · Father Salvio · Esteban
**Sages (4):** Mama Jacinta · Mbah Seruni · Mormor Astrid · Don Alejandro
**Specialists (5):** Mateusz Voltari · Cassandra Wells · Dr. Eleanor Whitcomb · Lt. Marcus Sandford · Yara Sun-Walker
**Masters (5):** Pak Setyo · Bu Wira · Isolde Thorn · Gruffydd · Maeve Donovan
**Contingent (4):** Margarethe Fitzmueller · Admiral Sterling · Bram Kettle · Captain "Wooden Tooth" Holloway

### 5.2 The Teaching Is Content

**Design law:** cross-reference progression_overview.md Section 1.4.

**No teacher opens a menu.** Every taught node is a scene, and cross-reference Section 4.1 — **Don Alejandro's third fencing session is not about fencing.**

**Cross-reference style_bible.md Pillar 1** — twenty-three teachers, twenty-three registers, and cross-reference npcs/blacksmiths_and_armorers.md Section 5: **Gruffydd will correct Jake's draw or sell him a bow that suits the wrong one, and both are node paths.**

### 5.3 Teachers Can Die

**Cross-reference:** colonial_war/retaliation_system.md Section 3.5, npcs/the_named_dead.md.

**Design law:** **a teacher lost is a node lost, permanently.**

Cross-reference colonial_war/retaliation_system.md Section 4 — the hostage algorithm selects from NPCs with high Attachment Score, and cross-reference Section 5.1: **eleven of the twenty-three teachers are eligible.**

**No replacement teacher is ever provided.**

---

## 6. WHAT CANNOT BE COMPLETED

### 6.1 The Arithmetic

**94 Marks available. 141 needed. A perfect playthrough opens 67% of the tree.**

### 6.2 The Mutual Exclusions

Cross-reference progression_overview.md Section 4.2.

- **Mira OR Sterling** (Section 4.3) — cross-reference krone_colonies.md Section 5.4: Sterling requires the alliance path and it is not available to a player who sank her squadron.
- **Margarethe OR Don Alejandro** for The Register — cross-reference npcs/imperial_contacts.md Section 2.14: **the romance path closes Don Alejandro's version**, and the game never says so.
- **Bram Kettle** requires turning the Regulators rather than destroying them. Cross-reference colonial_war/assassin_network_questline.md Section 5.5.
- **The eleven Standing-gated nodes** require trust tiers that are zero-sum across factions. Cross-reference colonial_war/war_state_system.md Section 2.2.

### 6.3 The Design Statement

**Cross-reference:** progression_overview.md Section 4.2.

**A second playthrough is a different character**, not the same character with more. Cross-reference Section 7.

---

## 7. NO RESPEC

**Cross-reference:** progression_overview.md Section 1.3.

### 7.1 The Rule

**Marks spent are spent. There is no reallocation, no consumable, and no NPC service.**

### 7.2 Why

Cross-reference Section 5.2 — **nodes are taught by people.** A respec would mean un-learning what Don Alejandro spent three chapters showing Jake, and cross-reference style_bible.md Pillar 2: **the bible has never once let anything be undone.**

### 7.3 The Concession

**None.** Cross-reference Section 6.3.

---

## 8. THE MENU

**Cross-reference:** progression_overview.md Sections 1.1 and 7.

### 8.1 What It Shows

**No numbers. Anywhere. Including in the stat view.**

**BODY** — a figure, and four descriptive lines that change as values cross thresholds:
*"Wind: he can hold a boarding action to its end."* → *"Wind: two boarding actions, and the second is honest."*

**CRAFT** — the tree. Nodes are named, described in verbs, and locked nodes show **either** their Mark cost **or** a teacher's name **or** a single line about a place.

**STANDING** — **not in the menu.** Cross-reference progression_overview.md Section 2.3 — it has no interface, and the player reads it through gates, greetings, and prices.

### 8.2 The Marks Display

A single line: *"Three things understood, not yet used."*

### 8.3 The Locked Node Text

Cross-reference Section 4.4 — a locked Standing node reads:

**THE HOT STONE WALK**
*"An old woman on a mountain knows how. She has not offered."*

---

## 9. IMPLEMENTATION NOTES

```cpp
struct SkillNode {
    NodeID      id;
    Branch      branch;          // Blade, Shadow, Sea, Land, Word
    uint8_t     markCost;        // 1–4
    TeacherID   requiredTeacher; // NONE for 45 of 68
    StandingReq standingGate;    // set on 11 of 68
    NodeID      prerequisites[3];
    // no float multipliers exist on this struct, by design
};
```

**Design law:** cross-reference Section 1.2 — **if a future node needs a float multiplier, the node is wrong.**

**Tuning levers, in order of sensitivity:** Mark earn caps (Section 3.2) · Body object counts (Sections 2.2-2.4) · the 141 total cost · the eleven Standing gates.

---

## 10. INTEGRATION MAP

- **progression_overview.md** ← the three axes, the anti-patterns, the readability law
- **economy_core.md** → money buys none of this
- **army_system.md** → three Word nodes affect command; none affect combat
- **mechanics/combat_feel.md, physics/combat_physics.md** ← posture, deflection, the Blade branch
- **mechanics/stealth.md, physics/sound_physics.md** ← the Shadow branch
- **physics/ship_physics.md, wind_systems.md** ← the Sea branch
- **mounted_travel.md, the regional physics documents** ← the Land branch
- **npcs/named_crew_deep_dives.md, the four sage documents, npcs/shippable_specialists.md, npcs/blacksmiths_and_armorers.md** ← the twenty-three teachers
- **colonial_war/retaliation_system.md, npcs/the_named_dead.md** ← teachers can be lost permanently
- **light_elves.md, light_elf_boss.md** ← the Fourth Rule as a node
- **crystal_guardian.md** ← the best posture teacher in the game

---

## 11. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** twenty-three teachers in twenty-three registers, and Ines teaching contract arithmetic angrily.
- ✅ **Pillar 2 (Lore Through Objects):** twelve draughts, sixteen pearls, and no menu in which Jake becomes stronger. Locked nodes described as *an old woman on a mountain knows how, and she has not offered.*
- ✅ **Pillar 3 (Layered Death):** a teacher taken by the retaliation system is a node gone forever, with no replacement, and **The Mercy silently does not function in one courtyard.**
- ✅ **Pillar 4 (Layered Humor):** Big Tom's Answer. Gruffydd selling a bow that suits a wrong draw. *"Wind: two boarding actions, and the second is honest."*
- ✅ **Pillar 5 (Theme — Revenge):** sixty-eight verbs and not one number, none of it purchasable, two-thirds of it reachable at best — and a non-lethal technique taught by a priest that quietly stops working for nine minutes in a courtyard in the Stonecountry.

---

## 12. CONCLUSION

Marks, Teachers, and What Cannot Be Bought. Twelve draughts a physician makes once and Ines cannot make at all. Sixteen pearls under water, six of them behind routes only trust opens. Posture that grows by deflecting three attacks perfectly from something that is trying to kill you, fifteen times, whether or not you win.

Sixty-eight nodes and not one percentage among them. Ninety-four Marks available against a hundred and forty-one needed, so that the tree is two-thirds reachable at absolute best and a second playthrough is a different man rather than the same one with more. No respec, because a retired Imperial captain spent three chapters teaching a pirate his son's fencing and the third session was not about fencing.

No bar, no level, no pop-up. Just a line in the corner of the great cabin, once, for four seconds: *you have learned something.*

And a locked node that says an old woman on a mountain knows how, and she has not offered.

**Phase 9 Document 2 of 6.**

**Sixty-eight verbs. Zero numbers.** 🗡️
