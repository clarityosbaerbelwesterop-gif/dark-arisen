# TIME AND REST — THE CLOCK AND THE SAVE

**Status:** Draft v1.0
**Phase:** Phase 12 — Interaction, Crew & Travel (FINAL — Document 6 of 6)
**Related:** travel_system.md, ship_as_home.md, crew_system.md, quest_design_philosophy.md, highmoore_weather.md, house_system.md, physics/sun_shadow_systems.md, physics/weather_physics.md, colonial_war/war_state_system.md, style_bible.md
**Purpose:** The time system — the day cycle, what runs on it, seasons, the chapter clock, sleep and rest, the save architecture and its two autosave points, the one place with none, and the design law that time is not a resource the player spends.
**Inspirations:** Red Dead Redemption 2's unhurried day, Majora's Mask inverted (a clock without a deadline), Kingdom Come: Deliverance's save discipline, Outer Wilds, and the rejection of both the hunger meter and the save-anywhere-anytime shrug

---

## 1. THE LAWS

### 1.1 TIME IS NOT A RESOURCE

**Cross-reference:** quest_design_philosophy.md Section 6.

**No hunger. No thirst. No sleep meter. No fatigue debuff.** Cross-reference progression_overview.md Section 1.1.

Time passes, things happen on it, and **the player is never punished for existing in it.**

### 1.2 THERE ARE DEADLINES AND THEY ARE NOT SHOWN

**Cross-reference:** quest_design_philosophy.md Section 6.2.

**Fourteen quests expire.** No timer, no countdown, no warning. Cross-reference Section 5.

### 1.3 THE WORLD RUNS WHETHER JAKE DOES OR NOT

**Cross-reference:** colonial_war/war_state_system.md Section 6, main_story_structure.md Section 1.3.

---

## 2. THE DAY

**Cross-reference:** physics/sun_shadow_systems.md.

### 2.1 The Ratio

**One in-game hour = 2.5 real minutes.** A full day is **one hour of play.**

**Cross-reference travel_system.md Section 1.3** — a long sea passage crosses a night, and cross-reference ship_as_home.md Section 4.1: **the watches rotate and the mid deck fills.**

### 2.2 What Runs On It

- **Light.** Cross-reference physics/sun_shadow_systems.md — and cross-reference highmoore_region.md Section 3.2: **Highmoore's low raking light is a time-of-day phenomenon.**
- **NPC schedules.** Cross-reference npcs/npc_recurrence_system.md — people are where they should be.
- **Tides.** Cross-reference physics/water_physics.md — and cross-reference colonial_war/regional_dungeon_catalog.md Section 3.2: **the Drowned Mead Hall drains twice a day for forty minutes.**
- **The butts.** Cross-reference house_belos.md Section 4.2 — **Belos villages shoot in the evening.**
- **The shaft.** Cross-reference light_elf_boss.md Section 2.2 — **broken cloud with a west wind**, which is weather, not clock, and cross-reference Section 3.2.

### 2.3 Dungeon Time

**Cross-reference:** colonial_war/dungeon_design_philosophy.md Section 5.3.

**Shadow-hour puzzles exist** — Ashenmoor chambers that only make sense at one time of day — and cross-reference Section 4: **there is no wait-until-hour command**, so the player either comes back or sits down.

---

## 3. SEASONS AND THE LONGER CLOCK

### 3.1 Highmoore Has Seasons

**Cross-reference:** highmoore_weather.md Section 3, house_system.md Section 4.5.

**The burning season, the wet, winter, and the working summer**, and cross-reference house_system.md Section 4.5: **the entire institution of Highmoore's war is a six-week weather window.**

### 3.2 The Archipelago Does Not

Tropical, and cross-reference physics/weather_physics.md: **weather states rather than seasons**, except cross-reference physics/snow_ice_physics.md — **Fjordlund, which has a winter and closes routes.**

### 3.3 The Chapter Clock

**Cross-reference:** colonial_war/war_state_system.md Section 5, colonial_war/castle_ownership.md Section 5.1.

**Holdings, armies, and the war tick per chapter, not per day.** Cross-reference economy_core.md Section 8 — income and upkeep resolve at chapter boundaries.

**Design note:** cross-reference Section 1.1 — **this keeps the economy from becoming a clock the player has to feed.**

---

## 4. REST

### 4.1 The Two Places

**THE GREAT CABIN.** Cross-reference ship_as_home.md Section 3.2.
**A SAFE HOUSE.** Cross-reference colonial_war/castle_ownership.md — an owned holding, or an inn (cross-reference highmoore_settlements.md Section 8.2).

### 4.2 What Rest Does

- Passes time to a chosen part of the day: **dawn, midday, dusk, night.** Not to an arbitrary hour.
- Heals. Cross-reference animation_system.md Section 5.1 — **and wound persistence is shortened by a surgeon, not by sleep.**
- **Autosaves.** Cross-reference Section 5.2.

### 4.3 What Rest Does Not Do

**No skip-to-hour. No wait-here. No campfire-anywhere.**

Cross-reference Section 2.3 — **a player who needs a shadow-hour comes back, or sits down and waits at normal speed.** Cross-reference social_system.md Section 4.1: **and sitting is already the best listening posture in the game.**

### 4.4 Sleeping Rough

**Cross-reference:** mounted_travel.md, highmoore_weather.md Section 6.

**Not a mechanic.** Jake does not carry a bedroll. Cross-reference Section 4.1 — **there are two rest points and the world is built so you are never more than a chapter's-worth of travel from one**, and in Highmoore that means inns and stables.

---

## 5. THE SAVE ARCHITECTURE

**Cross-reference:** quest_design_philosophy.md Section 5.4.

### 5.1 Manual Save Is Unrestricted

**Anywhere, any time, any number of slots.** Cross-reference quest_design_philosophy.md Section 5.3 — **the player can reload, and the game does not help them decide to.**

### 5.2 Autosave Has Exactly Two Triggers

1. **Chapter boundaries.**
2. **Rest.** Cross-reference Section 4.1.

**That is the entire list.**

### 5.3 What Autosave Never Does

**Cross-reference:** quest_design_philosophy.md Section 5.4.

- **Not before a decision.**
- **Not before a boss.**
- **Not after a bad outcome.**
- **Not on entering a dungeon.**
- **Not on picking up a document.**

**Design law:** cross-reference quest_design_philosophy.md Law Two — **an autosave before a choice is the game telling the player the choice is reversible.** It is not.

### 5.4 THE PLACE WITH NONE

**Cross-reference:** quest_design_philosophy.md Section 5.4.

**There is no autosave in the two hours around the lake.**

From the moment the false letter is handed over on the fell — cross-reference princess_quest_the_lake.md Section 2.1 — until Jake reaches the dock at the end of the return, **cross-reference princess_quest_the_return.md Section 5.1: no autosave fires.**

Not at the argument. Not at the kiss. Not at the lake. Not at the arrow. **Not at the Arion gate, not in her chambers, not in the courtyard, not at Belos, and not at the stair.**

**Design note:** a player can still manually save. Most will not think to. **Those who do will remember doing it.**

### 5.5 No Death Save

**Cross-reference:** battle_system.md Section 5.3, mechanics/combat_feel.md.

**Jake's death loads the last save**, manual or auto. Cross-reference battle_system.md Section 5.3 — **and in a battle, Jake falling is not a death: he is pulled out, and the battle continues without him, and is usually lost.**

---

## 6. WHAT TIME DOES TO PEOPLE

**Cross-reference:** npcs/npc_recurrence_system.md, npcs/children_and_families.md.

### 6.1 Children Grow

**Cross-reference:** npcs/children_and_families.md Section 4, highmoore_populations.md Section 6.4.

Across chapters. Cross-reference the_lesser_houses.md Section 4.4 — **Tam gets his transfer eventually and is insufferable about it.**

### 6.2 Buildings Change

Cross-reference npcs/building_evolution.md — the seven states, and cross-reference colonial_war/castle_ownership.md Section 3.3: **Jake returns to changes he did not watch happen.**

### 6.3 People Age and Are Not Replaced

Cross-reference npcs/the_named_dead.md, crew_system.md Section 6 — **and Esteban is sixty-something at the start.**

---

## 7. IMPLEMENTATION

```cpp
struct WorldClock {
    uint32_t minutes;        // 1 in-game hour = 150 real seconds
    uint8_t  day, season;    // seasons: Highmoore + Fjordlund only
    uint8_t  chapter;        // drives economy, war, upkeep ticks
};

struct SaveRules {
    bool manualAnywhere = true;
    // autosave: chapter boundary, rest. Nothing else.
    bool suppressAutosave;   // TRUE for the entire lake→dock window §5.4
};
```

**Prohibited:** hunger/thirst/fatigue meters · wait-until-hour · bedroll-anywhere · autosave before choices or bosses · **any function named `AutosaveBeforeDecision`.**

---

## 8. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1:** watches rotate and a ship's day has four different social registers on it depending on the hour.
- ✅ **Pillar 2:** a mead hall that drains twice a day for forty minutes; a chamber that only makes sense at one hour; a village shooting at butts every evening.
- ✅ **Pillar 3:** cross-reference Section 5.4 — **no autosave anywhere in the two hours around the lake**, which a player will notice afterward and understand.
- ✅ **Pillar 4:** Tam getting his transfer three chapters later and being insufferable.
- ✅ **Pillar 5:** an autosave before a choice tells the player the choice is reversible, so there are none — and the longest stretch without one in the game covers a letter, a lake, a courtyard, nine minutes, and a stair.

---

## 9. PHASE 12 COMPLETE

**Six documents:** interaction_system · social_system · crew_system · ship_as_home · travel_system · time_and_rest

**What Phase 12 built:**
- A world with no interaction highlights and no fade-to-black
- A social layer with no affinity meter, no gift menu, no antagonise input, and no romance system
- A crew that is a household on a schedule, whose only member with no mechanical function asks the second question
- A four-deck ship that has to be good enough to compete with a settlement
- Travel with no compass, no minimap, and no fast travel over water anywhere
- A save architecture with two autosave triggers and a two-hour window that has neither

---

## 10. CONCLUSION

The Clock and the Save. One in-game hour every two and a half real minutes, with no hunger, no thirst, no fatigue, and no bedroll — time that passes and carries tides and watches and evening archery and a low raking light, and never once asks to be fed.

Fourteen deadlines with no timers. A war that ticks per chapter so the economy never becomes a clock. Seasons in two places out of eight, because Highmoore's entire institution of warfare is a six-week window when the ground is hard.

Rest in two places only — a great cabin and a safe house — and autosave on two triggers only, never before a decision and never before a boss, because an autosave before a choice is the game promising the choice is reversible.

And one window, from a letter handed over on a fell to a dock on the other side of a mountain, covering an argument, a kiss, nine minutes at a trot, a lake, an arrow, thirty seconds of nothing to do, a courtyard, nine more minutes, and a cleared stair in the rain — **in which no autosave fires at all.**

**Phase 12 Document 6 of 6. PHASE 12 COMPLETE.**

**An autosave before a choice says the choice is reversible.** ⏳
