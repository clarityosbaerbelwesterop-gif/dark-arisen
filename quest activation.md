# QUEST ACTIVATION — HOW A QUEST STARTS WITHOUT A MARKER

**Status:** Design Draft v1.0
**Phase:** Phase 11 — Quests & Missions (Document 2 of 6)
**Related:** quest_design_philosophy.md, main_story_structure.md, side_quest_catalog.md, mission_types.md, npcs/dialogue_system.md, npcs/npc_recurrence_system.md, npcs/regional_populations.md, colonial_war/war_state_system.md, camera_system.md, style_bible.md
**Purpose:** The activation system — the six trigger types, what happens at the moment of activation, the silent-activation doctrine, proximity and overhearing, how the player knows anything started, re-approach and refusal, and the implementation shape.
**Inspirations:** Morrowind (a quest is a conversation), Kingdom Come: Deliverance (walking into things), Outer Wilds (activation is learning), Red Dead Redemption 2's ambient encounters, and the total absence of the exclamation mark

---

## 1. THE PROBLEM

**Cross-reference:** quest_design_philosophy.md Law One.

No markers, no icons, no journal auto-population as a task list. **So how does a player know a quest exists?**

**The answer is that frequently they do not**, and cross-reference Section 4: **that is a design position, not an oversight.**

---

## 2. THE SIX TRIGGERS

### 2.1 CONVERSATION — ~55%

The dominant type. Cross-reference npcs/dialogue_system.md.

**Jake talks to someone and they mention something.**

**Design law:** cross-reference Section 3 — **there is no "accept quest" prompt.** The conversation ends. The journal gains an entry. Nothing else happens on screen.

**Cross-reference npcs/recurring_quest_givers.md** — and roughly a third of conversation activations happen on a **second or later** meeting, because cross-reference npcs/npc_recurrence_system.md: **people do not tell strangers their problems.**

### 2.2 OVERHEARD — ~15%

**Cross-reference:** npcs/regional_populations.md Section 10.

**The gossip web is a quest-delivery system.**

Two people in a market talking about something. A road warden mentioning a name. Cross-reference npcs/tavern_keepers.md Section 4.2 — **Master Halvdan's storytelling nights at Bjornshallen contain three dungeon locations and the design never points this out.**

**Design law:** cross-reference quest_design_philosophy.md Section 3.4 — **an overheard quest enters the journal in the form it was overheard, which is often wrong.**

### 2.3 OBJECT — ~12%

**Cross-reference:** treasure_system.md Section 2.3, style_bible.md Pillar 2.

A letter, a map, a ledger, a body with something on it.

**Cross-reference treasure_system.md Section 5.2** — the map chain: *"finding one dungeon is finding three."*

**Cross-reference princess_quest_arion_falls.md Section 4.3** — **and a letter on a pillow in a room the player did not have to enter.**

### 2.4 PROXIMITY — ~10%

**Cross-reference:** Section 5.

**Jake rides past something.**

Cross-reference colonial_war/dungeon_design_philosophy.md Section 3.1 — a cave mouth behind a waterfall, an arch at low tide, a shadow that is a door at one hour. Cross-reference highmoore_region.md Section 6.3 — **a castle on a skyline eight kilometres away that is the most beautiful thing in frame.**

**Design law:** cross-reference Section 5.2 — **proximity activation is silent.** No sound cue, no journal entry, no notification. The thing is simply there and the player either goes to it or does not.

### 2.5 WORLD STATE — ~5%

**Cross-reference:** colonial_war/war_state_system.md, colonial_war/retaliation_system.md.

A meter crosses a threshold. Cross-reference colonial_war/retaliation_system.md Section 3 — **the retaliation system generates quests without any quest-giver at all**: a letter arrives, a hunter appears, a settlement is raided.

Cross-reference colonial_war/krone_colonies.md Section 3.2 — **a Profitability collapse opens the withdrawal path**, and nobody tells Jake.

### 2.6 ABSENCE — ~3%

**Cross-reference:** npcs/npc_recurrence_system.md.

**Somebody who was there is not there.**

Cross-reference princess_quest_arion_falls.md Section 4.2 — **Alys's room is empty.** Cross-reference npcs/npc_recurrence_system.md — a merchant whose stall is shuttered, a hostage-guest who has gone home early, a road warden's post that is unmanned.

**Design law:** the absence is never remarked on by any NPC unless asked.

---

## 3. THE MOMENT OF ACTIVATION

### 3.1 What Happens On Screen

**Nothing.**

- No sound
- No screen flash
- No "Quest Added"
- No journal pop-out
- **No camera change.** Cross-reference camera_system.md Section 1.3.

### 3.2 The Only Feedback

**Cross-reference:** quest_design_philosophy.md Section 3.

**The journal has a new entry, in Jake's hand, dated.** A player who opens the notebook finds it. A player who does not, does not.

### 3.3 No Acceptance

**Cross-reference:** Section 2.1.

**There is no accept/decline.** Cross-reference Section 7 — **declining is done by not going**, which is the same thing the world does with everything else in this game.

### 3.4 The One Exception

**Cross-reference:** mission_types.md Section 2.

**Contract work has a handshake.** Cross-reference colonial_war/colony_system_core.md Section 2.1 — escort, privateer, and champion contracts are *agreements*, spoken aloud, with terms, and **the player says yes in dialogue.**

**Design note:** that is a fictional handshake, not a UI prompt. **There is still no accept button.**

---

## 4. SILENT ACTIVATION

**Cross-reference:** Section 1, princess_quest_overview.md Section 5.3.

### 4.1 The Doctrine

**Some quests are running before the player knows they exist.**

**The canonical case:** cross-reference princess_quest_overview.md Section 5.3 — *"the player should not be certain they are in a questline until the letter arrives four hours later."*

### 4.2 The Count

**Nineteen quests activate silently**, with no journal entry at activation.

### 4.3 When the Journal Catches Up

**When Jake would have written something.**

Cross-reference princess_quest_the_lake.md Section 2.1 — the Princess questline's first journal entry is **the false letter**, four hours after the ball, and the ball is not in the notebook at all, and cross-reference quest_design_philosophy.md Section 3.1: **because Jake does not write down a nice evening.**

### 4.4 The Design Statement

**Cross-reference:** style_bible.md Pillar 5.

A player who plays Dark Arisen attentively and a player who plays it efficiently **do not experience the same game**, and cross-reference Section 8.2: **the difference is roughly forty hours of content that the second player never learns existed.**

---

## 5. PROXIMITY IN DETAIL

### 5.1 The Radius Problem

**Cross-reference:** Section 2.4.

**There is no trigger volume that announces itself.** Cross-reference technical_targets.md Section 8 — no minimap means no edge-of-radius signal.

### 5.2 What Actually Fires

**Two things, both invisible:**

1. **The thing becomes interactable** at normal interaction range.
2. **Ambient audio may change.** Cross-reference physics/sound_physics.md — a waterfall's reverb has a cave behind it, and cross-reference colonial_war/dungeon_design_philosophy.md Section 3.1: **the sound is the discovery.**

### 5.3 The Skyline Method

**Cross-reference:** highmoore_region.md Section 4.3, colonial_war/castle_catalog.md.

**Highmoore's entire navigation is proximity activation at three kilometres.** Cross-reference highmoore_region.md Section 4.3 — castles are readable at range and the player rides toward them because they are visible, and cross-reference highmoore_region.md Section 6.3: **that is how the Princess questline begins and nothing prompted it.**

### 5.4 The Absence Variant

**Cross-reference:** Section 2.6.

Proximity to where something **was.** Cross-reference npcs/npc_recurrence_system.md — the recurrence system already tracks where NPCs should be, and **the quest is the gap.**

---

## 6. DIRECTIONS AS THE MARKER SUBSTITUTE

**Cross-reference:** quest_design_philosophy.md Section 4.

### 6.1 The Requirement

**Every activated quest must be findable from what Jake was told.**

**Acceptance test:** a player with the journal open and no map markers must be able to reach every quest location in the game. Cross-reference Section 6.3 — **and if they cannot, they can ask.**

### 6.2 The Three Registers

Cross-reference quest_design_philosophy.md Section 4.2 — **precise · relational · unhelpful.**

### 6.3 Asking For Directions Is a Mechanic

**Cross-reference:** quest_design_philosophy.md Section 4.4.

**Any NPC with local knowledge can be asked about any journal entry.**

Cross-reference npcs/regional_populations.md — the answer depends on who is asked: a road warden knows roads, a fisherman knows water, a child knows where the good climbing is, and cross-reference quest_design_philosophy.md Section 3.4: **the new directions go into the journal and may be worse.**

### 6.4 The Map

**Cross-reference:** treasure_system.md Section 5.

**Jake has a map. It is a physical object he unfolds.** Cross-reference camera_system.md Section 2.1 — it is not a UI overlay; he holds it and the world is still visible around it.

**It shows geography and settlements. It does not show quests, objectives, or anything Jake has not personally been to or been given a chart of.**

---

## 7. DECLINING, DEFERRING, AND RE-APPROACH

### 7.1 Declining Is Not Going

**Cross-reference:** Section 3.3.

### 7.2 Deferring Is Free — Mostly

**Cross-reference:** quest_design_philosophy.md Section 6.

Most quests wait indefinitely. **Fourteen do not**, and cross-reference quest_design_philosophy.md Section 6.2: **there is no timer.**

### 7.3 Re-Approach

**Cross-reference:** npcs/npc_recurrence_system.md.

**Every quest-giver will discuss an open quest again**, repeat directions, and — cross-reference quest_design_philosophy.md Section 4.3 — some of them will be visibly annoyed.

### 7.4 The Refusal to Nag

**Design law:** **no NPC ever initiates a conversation to remind Jake about a quest.**

No shouting across a market. No *"still haven't dealt with that?"* on approach. Cross-reference npcs/regional_populations.md — **people say hello and go back to what they were doing.**

---

## 8. WHAT A PLAYER CAN MISS

### 8.1 The Honest Number

**Cross-reference:** quest_design_philosophy.md Section 2.

Of ~157 Thread and Turn quests, a first playthrough that is not actively hunting content will find **60-75.**

### 8.2 Whether That Is a Problem

**No.** Cross-reference Section 4.4, highmoore_overview.md Pillar 3.

**Design law:** cross-reference quest_design_philosophy.md Section 9 — **there is no completion percentage anywhere in Dark Arisen**, no collectible counter, and no end-screen tally, and cross-reference colonial_war/secret_bosses.md Section 9.3: **an Empire-path player finishes the game without learning six locations exist and the game never mentions it.**

### 8.3 The Second Playthrough

**Cross-reference:** progression_overview.md Section 4.2, skill_tree.md Section 6.3.

Different character, different faction, different doors. **And the same twenty-two moments hit differently** — cross-reference princess_quest_the_truth.md Section 9.

---

## 9. IMPLEMENTATION

```cpp
enum class Trigger : uint8_t {
    Conversation, Overheard, Object, Proximity, WorldState, Absence
};

struct QuestActivation {
    Trigger     trigger;
    bool        silent;          // 19 quests: no journal entry at activation
    JournalID   entryOnActivate; // INVALID if silent
    bool        distorted;       // §2.2 — enters the journal wrong
    NPCRef      askableFor[8];   // who can give directions §6.3
    GameTime    expiresAt;       // INVALID for all but 14
};
```

**Prohibited:** any on-screen notification at activation · any trigger volume with a visual or audio boundary · any NPC-initiated reminder · any completion counter · **any function named `AddQuestMarker`.**

---

## 10. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1:** three direction registers, and a fisherman, a road warden, and a child each know a different part of the same county.
- ✅ **Pillar 2:** a letter on a pillow, a map behind a waterfall, an empty stall, and a storytelling night that contains three dungeon locations and never says so.
- ✅ **Pillar 3:** absence as a trigger — a room that has been cleared out, a post that is unmanned, and nobody mentioning it unless asked.
- ✅ **Pillar 4:** quest-givers who repeat directions and are visibly annoyed about it, and never once shout across a market.
- ✅ **Pillar 5:** nineteen quests running before the player knows they exist, a hundred-odd quests a first playthrough will never find, and no counter anywhere that would tell them.

---

## 11. CONCLUSION

How a Quest Starts Without a Marker. Fifty-five percent of them start because somebody said something, a third of those only on a second meeting because people do not tell strangers their problems. Fifteen percent because Jake overheard two people in a market, and the journal writes down what he heard rather than what is true. Twelve percent because he picked something up. Ten percent because he rode past.

And nothing happens on screen. No sound, no flash, no *Quest Added*, no camera move, no accept button — just a new entry in a notebook in his own handwriting, dated, that a player finds if they open it.

Nineteen of them do not even do that. They are running for hours before the notebook admits they exist, because Jake does not write down a nice evening at a party.

No markers, so directions in words, in three registers, from people who will repeat them indefinitely and occasionally sigh. A map he unfolds and holds, showing geography and nothing else. And any local can be asked about any entry, and the answer might be worse than what he had.

A first playthrough will find sixty of a hundred and fifty-seven. There is no counter that would tell them.

**Phase 11 Document 2 of 6.**

**Nothing happens on screen. That is the whole system.** 🕯️
