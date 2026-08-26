# SOCIAL SYSTEM — TALKING TO PEOPLE WHO WANT NOTHING

**Status:** Draft v1.0
**Phase:** Phase 12 — Interaction, Crew & Travel (Document 2 of 6)
**Related:** interaction_system.md, crew_system.md, quest_activation.md, npcs/dialogue_system.md, npcs/npc_recurrence_system.md, npcs/regional_populations.md, npcs/tavern_keepers.md, colonial_war/indigenous_liberation_alliance.md, house_system.md, style_bible.md
**Purpose:** Interaction with people outside the quest layer — greeting and its four states, listening as a mechanic, the gossip web as playable content, drinking and eating, gifts, the register response, and the things Jake cannot do to people.
**Inspirations:** Red Dead Redemption 2's greet/antagonise, Kingdom Come: Deliverance's reputation-by-place, Morrowind's topic system, Disco Elysium's listening, and the rejection of the affinity meter

---

## 1. THE LAWS

### 1.1 MOST PEOPLE WANT NOTHING

**Cross-reference:** npcs/regional_populations.md, quest_activation.md Section 7.4.

**Cross-reference quest_activation.md Section 7.4** — *"people say hello and go back to what they were doing."*

### 1.2 NO AFFINITY METER

**Cross-reference:** progression_overview.md Section 2.3.

**No hearts, no bars, no numbers, no relationship screen.** Cross-reference Section 5 — **standing with a person is read in how they greet him.**

### 1.3 REGISTER RESPONDS TO REGISTER

**Cross-reference:** style_bible.md Pillar 1, skill_tree.md Section 4.5.

Cross-reference skill_tree.md Section 4.5 — **The Register** node lets Jake speak High, and **Whose Man** lets him answer Highmoore's question in a way that works.

**Design law:** without those nodes, **certain conversations are simply worse**, and nobody explains why.

---

## 2. GREETING

**Cross-reference:** npcs/npc_recurrence_system.md.

### 2.1 The Four States

| State | What it looks like |
|---|---|
| **Stranger** | acknowledged, not engaged. *"Aye."* |
| **Known** | named or nodded to. Cross-reference highmoore_populations.md §5.2 — **the yeomanry stop calling him *you*.** |
| **Owed** | cross-reference colonial_war/indigenous_liberation_alliance.md §5.5 — the gratitude economy. Prices change before greetings do. |
| **Wary** | cross-reference colonial_war/retaliation_system.md — a settlement that has heard. Doors close a beat early. |

### 2.2 It Is Not a Wheel

**One greet input.** Context decides what Jake says, and cross-reference Section 1.3: **register and standing decide how it lands.**

### 2.3 The Antagonise Question

**Cross-reference:** style_bible.md.

**There is no antagonise input.** Cross-reference Section 6 — **Jake can draw a weapon, which is a different and much worse thing**, and cross-reference npcs/regional_populations.md: a settlement remembers it.

---

## 3. LISTENING

**Cross-reference:** npcs/regional_populations.md Section 10, quest_activation.md Section 2.2.

### 3.1 It Is a Mechanic

**Fifteen percent of quests activate by overhearing.** Cross-reference quest_activation.md Section 2.2.

### 3.2 How It Works

**Standing near a conversation and not interrupting.** Cross-reference physics/sound_physics.md Section 2 — **audibility is distance and noise**, and cross-reference npcs/tavern_keepers.md Section 4.2: **Master Halvdan's storytelling nights contain three dungeon locations.**

### 3.3 Interrupting Ends It

Approaching too closely, drawing, or greeting **stops the conversation**, and cross-reference npcs/npc_recurrence_system.md: **they do not resume it.**

### 3.4 The Gossip Web Is Content

**Cross-reference:** npcs/regional_populations.md Section 10.3.

What people say about Jake changes. **Cross-reference colonial_war/assassin_network.md Section 4.2** — and it is how hunters are heard about before they arrive, and cross-reference quest_design_philosophy.md Section 3.4: **and it distorts, and the journal records the distortion.**

---

## 4. SITTING, DRINKING, EATING

**Cross-reference:** npcs/tavern_keepers.md, npcs/general_merchants.md.

### 4.1 Jake Can Sit Down

**Cross-reference:** interaction_system.md Section 1.2.

At a table, at a fire, on a wall. **Time passes at normal rate.** Cross-reference time_and_rest.md — **it is not a fast-forward.**

### 4.2 What Sitting Does

- **Ambient becomes audible.** Cross-reference Section 3.2 — **the best listening posture in the game.**
- **NPCs approach.** Cross-reference npcs/tavern_keepers.md Section 1.3 — a landlord shapes the room, and a seated stranger gets talked to.
- **Nothing else.** No buff, no regen bonus, no timer skip.

### 4.3 Drinking

Cross-reference npcs/tavern_keepers.md Section 8.1 — 5-1,000 doubloons.

**Cross-reference mechanics/combat_feel.md** — drink impairs. Cross-reference the Rache fuel requirement: **one cigar and one rum**, and cross-reference mechanics/combat_feel.md: **the rum is drunk and it does what rum does.**

### 4.4 Eating

Cross-reference mechanics/inventory_crafting.md. **No hunger meter.** Cross-reference progression_overview.md Section 1.1 — food restores and does nothing else.

---

## 5. GIFTS AND RETURNS

**Cross-reference:** treasure_system.md Section 7, colonial_war/indigenous_liberation_alliance.md Section 2.3.

### 5.1 The Return Question

**Asked forty times.** Cross-reference treasure_system.md Section 7.1 — sell, keep, or return.

**Cross-reference treasure_system.md Section 7.3** — and the count is never displayed and gates one door.

### 5.2 Gifts to Named People

**Cross-reference:** npcs/npc_recurrence_system.md.

**No gift menu.** Cross-reference Section 1.2 — **specific objects to specific people**, authored, and cross-reference mission_types.md Section 8: **five of them are missions.**

Cross-reference colonial_war/regional_dungeon_catalog.md Section 3.2 — **a drinking horn to a landlord who has been telling a story wrong for thirty years.**

### 5.3 What Nobody Wants

**Cross-reference:** npcs/luxury_merchants.md Section 3.3.

**Doña Carmen Ruiz will refuse a sale and say why.** Cross-reference npcs/black_market_fences.md Section 6.2 — and Tito Diego asks *which captain are you?*

---

## 6. DRAWING A WEAPON

### 6.1 It Is the Only Hostile Social Act

**Cross-reference:** Section 2.3.

### 6.2 What It Does

Cross-reference npcs/regional_populations.md, interaction_system.md Section 5 — **and cross-reference crystal_katana.md Section 6.2: drawing the Crystal Katana in Highmoore is the single most conspicuous act available to the player.**

Cross-reference crystal_katana.md Section 6.2 — guards step back, children are pulled indoors, **and nobody says the word magic.**

### 6.3 It Is Remembered

Cross-reference npcs/npc_recurrence_system.md — **the settlement's greeting state moves to Wary and takes chapters to recover.**

---

## 7. WHAT JAKE CANNOT DO

**Cross-reference:** interaction_system.md Section 5.

- **Harm a child.** No input connects. Cross-reference npcs/children_and_families.md.
- **Harm a non-combatant in Arion during the fall.** Cross-reference princess_quest_arion_falls.md Section 5.2.
- **Antagonise.** Cross-reference Section 2.3.
- **Romance anyone as a system.** Cross-reference Section 7.1.

### 7.1 The Romance Question

**There is no romance system.**

Cross-reference npcs/imperial_contacts.md Section 2.14 — **Margarethe Fitzmueller's is authored, singular, and has mechanical consequences** (cross-reference skill_tree.md Section 6.2: it closes Don Alejandro's fencing path). Cross-reference princess_quest_the_ball.md — **the Princess is not a romance option. She is a person in a questline, and there is no dialogue tree that pursues her.**

**Design law:** cross-reference the_princess.md Rule 1 — **she made her decision eight weeks before Jake arrived**, and a system that let a player "win" her would be an obscenity.

---

## 8. IMPLEMENTATION

```cpp
struct SocialState {
    GreetState  greeting;     // Stranger, Known, Owed, Wary — never displayed
    bool        hasRegister;  // §1.3 skill node
    bool        hasWhoseMan;  // §1.3, Highmoore only
    uint8_t     drawnHere;    // §6.3, decays over chapters
};
```

**Prohibited:** affinity meters · gift menus · antagonise inputs · romance trees · **any relationship UI of any kind.**

---

## 9. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1:** register responds to register, and a player without the node has worse conversations and is never told why.
- ✅ **Pillar 2:** a drinking horn, a chart, a cabinet, a sealing order — gifts are objects and objects are the whole social economy.
- ✅ **Pillar 3:** drawing a weapon is the only hostile social act and settlements remember it for chapters.
- ✅ **Pillar 4:** sitting down in a pub and being talked at is the best comedy delivery system in the game.
- ✅ **Pillar 5:** there is no romance system, and the woman at the centre of the game's most emotional questline cannot be pursued by any dialogue tree, because she decided eight weeks before he arrived.

---

## 10. CONCLUSION

Talking to People Who Want Nothing. Most of the archipelago says hello and goes back to what it was doing. Four greeting states and no meter anywhere — you find out where you stand because the yeomanry stop calling you *you*, or because a door closes a beat early, or because the price of bread went down before anybody smiled at you.

Listening as a mechanic: fifteen percent of the game's quests arrive by standing near two people and not interrupting, and a storytelling night in a Fjordlund hall contains three dungeons and never says so. Sitting down, at normal speed, with no buff and no skip, purely because it is the best listening posture in the game and because a landlord will come over.

No gift menu — a horn to a man who has told a story wrong for thirty years. No antagonise input; the only hostile social act is drawing a weapon, and in Highmoore drawing one specific weapon is the most conspicuous thing a person can do, and nobody says the word magic.

And no romance system, because the woman at the centre of it decided eight weeks before he rode up the road, and a mechanic for winning her would be an obscenity.

**Phase 12 Document 2 of 6.**

**People say hello and go back to what they were doing.** 🍺
