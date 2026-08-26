# QUEST DESIGN PHILOSOPHY — NO MARKERS, NO FAILURE SCREENS

**Status:** Design Draft v1.0
**Phase:** Phase 11 — Quests & Missions (Document 1 of 6)
**Related:** quest_activation.md, main_story_structure.md, side_quest_catalog.md, mission_types.md, dungeon_system_global.md, colonial_war/dungeon_design_philosophy.md, npcs/recurring_quest_givers.md, npcs/regional_populations.md, colonial_war/war_state_system.md, style_bible.md
**Purpose:** The rules every quest in Dark Arisen obeys — the seven laws, the journal, failure as an outcome rather than a screen, timed content, mutual exclusion, the quest-giver doctrine, and the named anti-patterns.
**Inspirations:** The Witcher 3 (side quests with the weight of main quests), Kingdom Come: Deliverance (quests that fail forward), Morrowind (directions instead of markers), Outer Wilds (knowledge as the only progression), Disco Elysium (a journal that is a character), and the total rejection of the task-list open world

---

## 1. THE SEVEN LAWS

### 1.1 LAW ONE — NO MARKERS

**Cross-reference:** colonial_war/dungeon_design_philosophy.md Section 3.2, highmoore_region.md Section 6.2, progression_overview.md Section 1.1.

**No quest markers. No objective arrows. No minimap pips. No compass.** Cross-reference technical_targets.md Section 8 — none of these systems exist in the build.

**What replaces them:** directions, in words, from people. Cross-reference Section 4 and quest_activation.md Section 6.

### 1.2 LAW TWO — NO FAILURE SCREEN

**Cross-reference:** Section 5.

**No quest in Dark Arisen produces a "Quest Failed" message or a reload prompt.** Quests resolve badly. Cross-reference Section 5.2 — **and badly is an outcome the game keeps.**

### 1.3 LAW THREE — THE WORLD DOES NOT WAIT

**Cross-reference:** colonial_war/war_state_system.md Section 5, colonial_war/colony_system_core.md.

Cross-reference colonial_war/war_state_system.md Section 6 — **the colonial war resolves without Jake.** Quests inherit this: cross-reference Section 6 — **fourteen quests in the game expire.**

### 1.4 LAW FOUR — SIDE QUESTS CARRY MAIN-QUEST WEIGHT

**Cross-reference:** highmoore_overview.md Pillar 3.

**The most emotionally significant content in Dark Arisen is optional.** Cross-reference princess_quest_overview.md — the entire Princess questline is skippable, unmarked, and never mentioned by the main story.

**Design law:** cross-reference side_quest_catalog.md — **there is no writing-quality distinction between main and side content.** The tiering is structural, not qualitative.

### 1.5 LAW FIVE — CONSEQUENCES ARE NOT REPORTED

**Cross-reference:** colonial_war/war_state_system.md Section 8.1, npcs/regional_populations.md Section 10.

**No outcome summary. No "your choice mattered" card.**

Cross-reference npcs/regional_populations.md — **the ambient dialogue changes, the prices change, a gate opens or does not**, and the player finds out by living there.

### 1.6 LAW SIX — NOBODY EXPLAINS THE STAKES

**Cross-reference:** style_bible.md Pillar 2.

**No quest-giver summarises what is at risk.** Cross-reference princess_quest_the_ball.md — Elowen says nothing about her situation across ninety minutes of conversation, and cross-reference the_princess.md Section 8: **the five clues are objects, a pause, and a subject changed.**

### 1.7 LAW SEVEN — THE JOURNAL IS A CHARACTER

**Cross-reference:** Section 3.

---

## 2. THE FOUR TIERS

| Tier | Count | Length | Marker | Expires |
|---|---|---|---|---|
| **Spine** — main story | 10 chapters | 38h critical path | never | no |
| **Thread** — major optional questlines | **17** | 3-12h each | never | 4 do |
| **Turn** — self-contained | **~130** | 20-90 min | never | 10 do |
| **Standing** — repeatable structures | 9 types | 10-40 min | never | n/a |

**Cross-reference mission_types.md** for Standing. **Cross-reference side_quest_catalog.md** for Thread and Turn.

---

## 3. THE JOURNAL

### 3.1 It Is Jake's Notebook

**Cross-reference:** Law Seven, style_bible.md Pillar 1.

**Handwritten. In his words. First person.** Not a task database.

### 3.2 What It Contains

- What Jake has been told, **as he understood it**
- Directions, as given: *"east of the ford, past a stone with three cuts on it, and she said you can't miss the smell"*
- Names, sometimes misspelled and later corrected
- **His own annotations**, which are opinions

### 3.3 What It Does Not Contain

- Objectives with checkboxes
- Distances
- **Anything Jake has not been told.** Cross-reference Section 3.4.
- A completion percentage
- A quest count

### 3.4 IT CAN BE WRONG

**Design law, and it is the point of the whole system.**

Cross-reference npcs/regional_populations.md Section 10.3 — **the gossip web distorts.** A quest heard secondhand enters the journal in its distorted form and **stays wrong until Jake learns otherwise.**

**Cross-reference princess_quest_the_lake.md Section 2.2** — the false letter's contents go into the journal as fact, and cross-reference princess_quest_arion_falls.md Section 4.3: **the entry is corrected only when the real letter is read**, and cross-reference princess_quest_the_truth.md Section 8: **on the missed-letter path it is never corrected at all and the player finishes the game with a lie in their notebook.**

### 3.5 The Sort

**Chronological. Not by region, not by type, not by priority.**

**Cross-reference Section 3.1** — it is a notebook. Cross-reference Section 3.6 — Jake can be searched, because Jake is not an idiot and he indexes his own book.

### 3.6 The One Concession

A search field. Cross-reference technical_targets.md Section 8 — **it is the only quest-tracking affordance in the game.**

---

## 4. DIRECTIONS

**Cross-reference:** Law One, quest_activation.md Section 6.

### 4.1 They Are Spoken

**Every quest location in Dark Arisen is described in dialogue in terms the player can act on.**

Cross-reference highmoore_settlements.md Section 8.3 — *"road's sworn to Pennard from the beck to the crossroads. After the crossroads it's Ridd's."*

### 4.2 The Three Direction Registers

- **Precise** — Imperial and Company NPCs give bearings, distances, and landmarks. Cross-reference npcs/imperial_contacts.md.
- **Relational** — Alliance and village NPCs give directions relative to things: *"past the grove where the old road bends, and if you reach the water you have gone too far."* Cross-reference colonial_war/indigenous_liberation_alliance.md.
- **Unhelpful** — cross-reference highmoore_populations.md Section 2.2. **Some people are bad at this** and the game does not correct them.

### 4.3 Asking Again

**Every quest-giver will repeat directions, indefinitely, and some of them will be visibly annoyed about it.**

### 4.4 Asking Someone Else

**Cross-reference:** npcs/regional_populations.md.

**Locals know where things are.** A player lost in Highmoore can ask a road warden, and cross-reference Section 3.4: **the answer goes into the journal and may be worse than the original.**

---

## 5. FAILURE

### 5.1 There Is No Failure State

**Cross-reference:** Law Two.

### 5.2 There Are Bad Outcomes

**Cross-reference:** colonial_war/retaliation_system.md, imperial_colonies.md Section 4.11.

| Quest | The bad outcome |
|---|---|
| **Fuerte Esperanza's rising** | cross-ref imperial_colonies.md §4.11 — *"mistime it and the reprisal is a massacre Jake caused"* |
| **A hostage crisis** | cross-ref colonial_war/retaliation_system.md §3.5 — **the hostage dies and enters npcs/the_named_dead.md** |
| **A settlement raid** | cross-ref colonial_war/castle_ownership.md §9 — it burns, and the survivors are refugees Jake now transports |
| **The Princess questline** | **there is no version where she lives** |

**Design law:** cross-reference Law Two — **the game continues. There is no reload prompt and no offer.**

### 5.3 Manual Reload Is Allowed and Not Encouraged

The player can load a save. Cross-reference Section 5.4 — **the game does not help them decide to.**

### 5.4 The Autosave Discipline

**Cross-reference:** Section 5.2.

**Autosave fires on chapter boundaries and safe-house rests only.** It does **not** fire before a decision, before a boss, or after a bad outcome.

**Design note:** cross-reference princess_quest_the_lake.md Section 8 — **no autosave exists in the two hours around the lake**, which is deliberate, and which a player will notice afterward.

---

## 6. TIMED CONTENT

**Cross-reference:** Law Three.

### 6.1 Fourteen Expiring Quests

Four Thread and ten Turn.

### 6.2 They Are Never Announced

**No timer. No countdown. No warning.**

**Cross-reference Section 6.3** — the world tells the player, in the way the world tells the player anything.

### 6.3 The Signals

- Ambient dialogue mentioning it is getting late
- A colony's Control meter visibly shifting. Cross-reference colonial_war/war_state_system.md Section 8.1 — **read in flags, prices, and patrol density.**
- **Somebody who was there not being there.** Cross-reference quest_activation.md Section 5.4.
- Cross-reference highmoore_weather.md Section 4 — **the burning season ending.**

### 6.4 What Expiry Looks Like

**Cross-reference:** npcs/recurring_quest_givers.md.

**The quest-giver is still there and does not mention it again.** If asked, they answer, once, plainly, and it is worse than a failure message.

*"Ah. No. That's — no, we sorted that. Or it sorted itself. Don't worry about it."*

---

## 7. MUTUAL EXCLUSION

**Cross-reference:** progression_overview.md Section 4.2, skill_tree.md Section 6.2.

### 7.1 The Principle

**Cross-reference colonial_war/war_state_system.md Section 2.2** — faction standing is zero-sum, and quests inherit it.

### 7.2 The Count

**Thirty-one quests are mutually exclusive with at least one other quest.**

Cross-reference krone_colonies.md Section 5.4 — **allying with Admiral Sterling closes eleven quests and opens nine.**

### 7.3 It Is Never Signposted

**Cross-reference:** Law Five, skill_tree.md Section 6.2.

**No warning dialogue. No point-of-no-return prompt.** Cross-reference npcs/imperial_contacts.md Section 2.14 — the Margarethe romance closes Don Alejandro's fencing path and **the game never says so.**

---

## 8. THE QUEST-GIVER DOCTRINE

**Cross-reference:** npcs/recurring_quest_givers.md, npcs/npc_recurrence_system.md.

### 8.1 They Are People First

**Cross-reference:** npcs/recurring_quest_givers.md — established across Phase 5. **Nobody in Dark Arisen exists to dispense a task.**

### 8.2 They Remember

Cross-reference npcs/npc_recurrence_system.md — a quest-giver Jake has helped four times speaks differently the fifth time, and cross-reference Section 6.4: **one Jake has ignored speaks differently too.**

### 8.3 They Can Die

**Cross-reference:** colonial_war/retaliation_system.md Section 4, npcs/the_named_dead.md, skill_tree.md Section 5.3.

**A dead quest-giver's quests are gone**, and cross-reference skill_tree.md Section 5.3: **for eleven of them, so is a skill node.**

### 8.4 They Are Not Grateful in a Useful Way

**Cross-reference:** Law Five.

Cross-reference colonial_war/indigenous_liberation_alliance.md Section 5.5 — **the gratitude economy is real and it is not a reward screen.** Prices change. Doors open. Nobody hands Jake a chest.

---

## 9. THE NAMED ANTI-PATTERNS

**Rejected by name:**

- **The question-mark map.** No icons of any kind.
- **The radiant quest generator.** Cross-reference mission_types.md — **Standing missions are authored templates with authored variants**, not procedural fill.
- **The three-choice consequence card.** Cross-reference Law Five.
- **Fetch-and-return.** Cross-reference mission_types.md Section 8 — **five quests in the game involve carrying an object to a person and all five are about the person.**
- **Kill-ten.** Cross-reference mission_types.md.
- **The escort quest where the NPC walks slowly.** Cross-reference mission_types.md Section 4 — **escorts exist and the escorted party moves at their own competent pace and can be lost.**
- **Quest items that cannot be dropped.**
- **The reputation bar.**
- **A "current objective" line anywhere on screen.**

---

## 10. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1:** three direction registers, and some people are simply bad at giving directions and the game does not fix them.
- ✅ **Pillar 2:** a journal in Jake's handwriting that can be wrong and stays wrong, and on one path contains a lie about a dead woman for the rest of the game.
- ✅ **Pillar 3:** no failure screen, no reload prompt, no autosave in the two hours around the lake.
- ✅ **Pillar 4:** *"we sorted that. Or it sorted itself. Don't worry about it."*
- ✅ **Pillar 5:** thirty-one mutually exclusive quests, never signposted; fourteen that expire without a timer; and the most important questline in the game is optional, unmarked, and never mentioned by the main story.

---

## 11. CONCLUSION

No Markers, No Failure Screens. A hundred and fifty-odd hours of quests with no icons, no arrows, no compass, no minimap, and no line on the screen telling anyone what they are currently doing — replaced by people giving directions in words, in three registers, some of them badly, all of them willing to repeat it and one or two visibly annoyed about it.

A journal in Jake's own hand, chronological, with his opinions in it and his misspellings, that records what he was told rather than what is true, that goes wrong when the gossip web distorts something, and that on one path contains a lie about a dead woman's child for the remainder of the game because nobody ever corrected it.

Fourteen quests that expire with no timer and no warning, whose ending is a man saying *ah, no, we sorted that, or it sorted itself.* Thirty-one that lock each other out and never say so. No failure screen anywhere, because a rising that is mistimed produces a massacre and the game keeps it and moves on.

And no autosave at all in the two hours around a lake.

**Phase 11 Document 1 of 6.**

**The journal can be wrong. That is the point.** 📓
