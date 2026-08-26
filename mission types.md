# MISSION TYPES — THE NINE STANDING STRUCTURES

**Status:** Design Draft v1.0
**Phase:** Phase 11 — Quests & Missions (FINAL — Document 6 of 6)
**Related:** quest_design_philosophy.md, quest_activation.md, side_quest_catalog.md, economy_core.md, army_system.md, battle_system.md, colonial_war/colony_system_core.md, colonial_war/retaliation_system.md, house_system.md, mounted_travel.md, style_bible.md
**Purpose:** The repeatable mission structures — the nine types, why they are authored rather than generated, variant counts, the moral pricing, degradation over time, and the design law that repeatable does not mean infinite.
**Inspirations:** The Witcher 3's contracts (authored variants, not templates), Red Dead Redemption 2's chance encounters, real privateering commission structures, and the total rejection of radiant quest generation

---

## 1. THE LAWS

### 1.1 AUTHORED, NOT GENERATED

**Cross-reference:** quest_design_philosophy.md Section 9.

**There is no radiant quest system in Dark Arisen.**

Every Standing mission is a **template with a hand-written variant set.** Cross-reference Section 2 — nine types, **147 authored variants**, and when they run out **they run out.**

### 1.2 REPEATABLE IS NOT INFINITE

**Cross-reference:** economy_core.md Section 6.

Each type has a **finite variant pool per region.** Cross-reference Section 10 — **when a region's pool is exhausted, that region stops offering that work**, and the NPC says so.

*"Nothing this month. Try Coldbeck."*

### 1.3 THE FIVE NON-TYPES

**Cross-reference:** quest_design_philosophy.md Section 9. These do not exist:

fetch-and-return · kill-ten · clear-the-camp · collect-N-of-X · **the escort where the NPC walks slowly.**

---

## 2. THE NINE TYPES

| # | Type | Variants | Pay | Cross-reference |
|---|---|---|---|---|
| 1 | **Escort** | 21 | 600-20,000 | colony_system_core.md §2.1 |
| 2 | **Convoy raid** | 18 | 800-6,000 | colony_system_core.md §4.4 |
| 3 | **Recovery** | 24 | material/standing | black_market_fences.md §6.2 |
| 4 | **Champion** | 12 | 150-400 marks | house_system.md §4.4 |
| 5 | **Transport** | 16 | 1,200 + progress | colony_system_core.md §3 |
| 6 | **Privateer commission** | 14 | 5,000-25,000 | colony_system_core.md §2.1 |
| 7 | **Road work** | 15 | 200-900 | highmoore_settlements.md §8.3 |
| 8 | **Hunt** | 13 | material | highmoore_fauna.md, fauna/ |
| 9 | **Salvage** | 14 | 400-9,000 | regional_dungeon_catalog.md §5 |

**Total: 147.**

---

## 3. ESCORT — 21

**Cross-reference:** Section 1.3 — **the escorted party moves at their own competent pace.**

They are carters, merchants, pilgrims, and officers who have done this before. **They ride properly, they do not stand in fires, and they can be lost.**

### 3.1 The Two That Pay Best

**Cross-reference:** economy_core.md Section 7.1.

**The Nueva Esperanza Labor Line** and **the Blackwood Landing Contract Line.** Cross-reference imperial_colonies.md Section 4.9, krone_colonies.md Section 6.9.

**Both pay roughly 40% above any comparable escort**, and cross-reference economy_core.md Section 7.2: **the game does not prevent it, does not warn, and does not editorialise.**

Cross-reference the sage reactions — **Mama Jacinta stops receiving him. Ines does not look at him for a while. Father Salvio asks once.**

---

## 4. CONVOY RAID — 18

**Cross-reference:** colonial_war/colony_system_core.md Section 4.

Supply · materiel · treasure · **people.**

### 4.1 The People Convoys

**Cross-reference:** colonial_war/colony_system_core.md Section 4.5.

**The raid-to-liberation pipeline**, and cross-reference battle_catalog.md Section 2.10: **the most emotionally charged repeatable content in the game.**

Cross-reference colonial_war/castle_ownership.md Section 4.2 — **and the freed become the population of Jake's holdings**, which means the repeatable content builds the settlement content.

### 4.2 Heat

Cross-reference colonial_war/retaliation_system.md Section 2.1 — **every raid raises Heat**, which is what stops this being a grind loop.

---

## 5. RECOVERY — 24

**Cross-reference:** npcs/black_market_fences.md Section 6.2, treasure_system.md Section 7.

**Tito Diego's network. *"We do not steal. We recover."***

**Pay:** cross-reference treasure_system.md Section 7.2 — **network strength and things money cannot buy.**

**Cross-reference treasure_system.md Section 7.3** — and every recovery returned rather than fenced feeds **the counted lock** on a door at the bottom of the ocean, and **the number is never displayed.**

---

## 6. CHAMPION — 12

**Cross-reference:** house_system.md Section 4.4, highmoore_populations.md.

**Highmoore only.** A feud settled by single combat, with Jake as a House's champion.

**Cross-reference house_system.md Section 6.3** — the fastest way to become known in the basin, and cross-reference highmoore_populations.md Section 5.2: **standing is read in whether the yeomanry stop calling him *you*.**

**Design note:** **the opponent is a named person with a household**, and cross-reference quest_design_philosophy.md Section 5 — **they can be spared, and four of the twelve have consequences for doing so.**

---

## 7. TRANSPORT — 16

**Cross-reference:** colonial_war/colony_system_core.md Section 3.

**Settlers, freed labourers, refugees, and specialists.**

Cross-reference colonial_war/castle_ownership.md Principle 1.2 — *"the population of Jake's settlements is a manifest of his voyages."*

**Cross-reference animation_system.md Section 8** — people are carried, with a full-body animation set and real weight.

---

## 8. THE FIVE THAT LOOK LIKE FETCHES

**Cross-reference:** Section 1.3, quest_design_philosophy.md Section 9.

Five missions in Dark Arisen involve carrying an object to a person. **All five are about the person.**

| Object | Person |
|---|---|
| A drinking horn | cross-ref regional_dungeon_catalog.md §3.2 — **Master Halvdan puts it on the wall and tells the story properly, having told it wrong for thirty years** |
| A locked cabinet's contents | cross-ref imperial_colonies.md §5 — **Mbah Seruni, and thirty years of shrine clearances** |
| A chart | cross-ref side_quest_catalog.md §2.1 — Esteban, and the one place he never went |
| A sealing order with a name on it | cross-ref regional_dungeon_catalog.md §6.3 — **Inspector Kessler** |
| A bow | cross-ref highmoore_smiths.md §3.2 — **Gruffydd, before the questline. After it, there is nobody to bring it to.** |

---

## 9. THE OTHER THREE

**ROAD WORK — 15.** Cross-reference highmoore_settlements.md Section 8.3, house_system.md Section 4.3. Highmoore's road-oath made playable: **a road Jake makes safe changes ambient dialogue in three villages.**

**HUNT — 13.** Cross-reference highmoore_fauna.md, fauna/. **Not bounties.** Cross-reference highmoore_fauna.md Section 2.1 — **hunting rights are House rights**, and crossing a boundary after a deer is formally a raid.

**SALVAGE — 14.** Cross-reference colonial_war/regional_dungeon_catalog.md Section 5, physics/water_physics.md. Wrecks, and cross-reference regional_dungeon_catalog.md Section 5.1: **one of them has a log naming a survivor who should not have survived.**

---

## 10. EXHAUSTION AND DEGRADATION

### 10.1 Pools Run Out

**Cross-reference:** Section 1.2.

Per region, per type. **When exhausted, the NPC says so** and cross-reference quest_activation.md Section 7.4: **they do not apologise or offer an alternative.**

### 10.2 Pay Degrades

**Cross-reference:** economy_core.md Section 6.

**Repeated work of the same type in the same region pays progressively less** — down to 60% by the sixth. Cross-reference npcs/general_merchants.md: **supply and demand, and the NPC will explain it if asked, and be slightly embarrassed.**

### 10.3 Standing Does Not

Cross-reference progression_overview.md Section 2.3 — **standing gains do not degrade**, which means the grind-resistant currency is the one that matters. Cross-reference progression_overview.md Section 1.2.

---

## 11. IMPLEMENTATION

```cpp
struct StandingMission {
    MissionType type;
    VariantID   variant;        // authored; 147 total
    RegionID    region;
    uint8_t     timesRunHere;   // drives §10.2 pay decay
    bool        raisesHeat;     // convoy raids, some escorts
    bool        countsForReturn;// §5 — feeds the hidden count
};
```

**Prohibited:** any procedural mission generator · any infinite pool · any repeatable that does not decay in pay · **any function named `GenerateRadiantQuest`.**

---

## 12. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1:** *"Nothing this month. Try Coldbeck."* And a merchant slightly embarrassed to explain why he is paying less.
- ✅ **Pillar 2:** five fetch-shaped missions and all five are a horn, a cabinet, a chart, a sealing order, and a bow that arrives too late.
- ✅ **Pillar 3:** the bow has nobody to be brought to after Chapter 8, and the game does not remove the mission — it just cannot be completed.
- ✅ **Pillar 4:** a hunting trespass. A road warden's opinion of Ridd. Master Halvdan discovering he has been telling a story wrong for thirty years.
- ✅ **Pillar 5:** the two best-paying repeatable contracts in the game are people-convoys, paid without comment, and the only currency that does not degrade with repetition is the one money cannot touch.

---

## 13. PHASE 11 COMPLETE

**Six documents:** quest_design_philosophy · quest_activation · main_story_structure · dungeon_system_global · side_quest_catalog · mission_types

**What Phase 11 built:**
- Seven quest laws, no markers, no failure screens, and a journal that can be wrong and stays wrong
- Six activation triggers, nineteen silent quests, and nothing whatsoever on screen when one starts
- Ten chapters, thirty-eight hours of spine, one hard gate, and no tutorial
- Sixty-one dungeons, four with nothing at the bottom, one that gives nothing at all
- Seventeen Threads and a hundred and thirty-two Turns with no quality tier between them and the plot
- Nine repeatable structures, 147 authored variants, and no generator anywhere

---

## 14. CONCLUSION

The Nine Standing Structures. A hundred and forty-seven hand-written variants across nine shapes, and when a region's pool is empty an NPC says *nothing this month, try Coldbeck* and does not apologise. Pay that decays to sixty percent by the sixth repetition, explained by a merchant who is slightly embarrassed about it. And standing, which does not decay at all, because it is the one thing money never touched.

No generator. No infinite pool. No fetch, no kill-ten, no clear-the-camp, and no escort where somebody walks slowly — the escorted ride properly, do not stand in fires, and can be lost.

Five missions that look like fetches and are all about a person: a horn that lets a landlord finally tell a story right after thirty years of getting it wrong, a cabinet of shrine clearances, a chart of the one place a sixty-year sailor never went, a sealing order with a name on it, and a bow that after Chapter 8 has nobody left to receive it — and the mission stays in the journal, uncompletable, because nothing removes it.

And the two best-paying repeatable contracts in Dark Arisen are people-convoys, forty percent above market, and the game pays without a word.

**Phase 11 Document 6 of 6. PHASE 11 COMPLETE.**

**Nothing this month. Try Coldbeck.** 📜
