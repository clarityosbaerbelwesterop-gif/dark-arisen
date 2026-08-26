# BATTLE SYSTEM — ONE MAN IN A LINE

**Status:** Design Draft v1.0
**Phase:** Phase 9 — Progression, Economy & Armies (Document 5 of 6)
**Related:** army_system.md, battle_catalog.md, progression_overview.md, economy_core.md, mechanics/combat_feel.md, physics/combat_physics.md, physics/sound_physics.md, physics/wind_systems.md, physics/destruction_physics.md, colonial_war/castle_sieges.md, colonial_war/indigenous_liberation_alliance.md, princess_quest_belos.md, npcs/the_named_dead.md, style_bible.md
**Purpose:** How large engagements work — the design refusal that shapes everything, the four battle types, the Line as the core mechanic, the three orders, momentum, the player's actual role, sound, losses, and what a battle is allowed to feel like.
**Inspirations:** Mount & Blade (a first-person soldier in a real formation), Kingdom Come: Deliverance's Skalitz and Pribyslavitz, Ghost of Tsushima's charges, Ran and Kagemusha (Kurosawa's scale and stillness), Waterloo (1970), Sekiro's density over spectacle, and the explicit refusal of Dynasty Warriors

---

## 1. THE REFUSAL

### 1.1 What This Is Not

**Design law, and everything else follows from it:**

- **Not an RTS.** No overhead camera, no unit selection, no formations panel. Cross-reference army_system.md Section 1.3.
- **Not Dynasty Warriors.** Cross-reference Section 3.2 — **Jake cannot kill an army.** Cross-reference princess_quest_belos.md for the one sequence where he nearly does, and cross-reference crystal_katana.md Section 1.1: **that sequence is designed to feel wrong and is explicitly not the model for this system.**
- **Not a cutscene with inputs.** Cross-reference Section 5 — battles are lost, regularly, by competent players.

### 1.2 What It Is

**Jake is one man in a line, and the line is the mechanic.**

Cross-reference Section 3 — he can affect roughly forty metres of front by being in it, and cross-reference Section 4: **three shouted orders, and whether they are heard depends on distance, noise, and Loyalty.**

### 1.3 The Scale Discipline

**Cross-reference:** army_system.md Section 2.2.

**Nothing in Dark Arisen requires a Host.** Cross-reference battle_catalog.md — three battles reach 800+, all three are optional, and the vast majority of engagements are 90-400 a side.

**Design note:** cross-reference physics/combat_physics.md — **four hundred men in a real physics simulation on a Highmoore road is more frightening than four thousand rendered as a crowd**, and this project chooses the four hundred.

---

## 2. THE FOUR BATTLE TYPES

### 2.1 THE FIELD ACTION

Two forces on open ground. Cross-reference highmoore_region.md Section 2.3 — **the High Moor is the game's battlefield**, and cross-reference physics/wind_systems.md, highmoore_weather.md: **wind and rain are combatants.**

**Scale:** 150-800 a side.
**Duration:** 12-25 minutes.
**Cross-reference:** house_system.md Section 4.5 — Highmoore's burning season produces these.

### 2.2 THE ASSAULT

**Cross-reference:** colonial_war/castle_sieges.md Section 4.

The storming phase of a siege. Cross-reference colonial_war/castle_sieges.md Sections 3-5 — the four phases lead here, and **the battle system runs the last one.**

**Scale:** 200-600 attacking.
**Duration:** 15-30 minutes.

### 2.3 THE DEFENCE

**Cross-reference:** colonial_war/castle_sieges.md Section 7.

The system reversed. Jake on a wall, or — cross-reference colonial_war/castle_sieges.md Section 7.5 — **at sea, breaking the besiegers, while the wall holds without him.**

**Scale:** 80-400 defending.
**Duration:** 20-40 minutes.

### 2.4 THE RISING

**Cross-reference:** colonial_war/indigenous_liberation_alliance.md Section 6, imperial_colonies.md Section 4.11.

The Liberation Event. **Structurally different from the other three:** cross-reference colonial_war/indigenous_liberation_alliance.md Section 6.2 — Jake receives objectives rather than command, Alliance fighters win or lose it themselves, and **his contribution is decisive but not sufficient.**

**Scale:** variable, and cross-reference imperial_colonies.md Section 4.11: **at Nueva Esperanza the mechanic is a timing puzzle disguised as a battle.**

---

## 3. THE LINE

### 3.1 The Core Idea

**Cross-reference:** physics/combat_physics.md, mechanics/combat_feel.md.

A battle is a **front**, and the front has segments. Each segment is holding, pressing, or breaking.

**Jake occupies one segment.** Cross-reference Section 3.2 — **his presence in a segment is worth roughly forty men**, and it is worth that by two mechanisms:

- **Attrition** — he kills people. Cross-reference physics/combat_physics.md.
- **Cohesion** — cross-reference Section 3.3. **Men next to Jake do not break.**

### 3.2 What Jake Cannot Do

**He cannot win a battle alone.**

**Design law:** cross-reference Section 1.1. If the other three segments break, the battle is lost while Jake is winning his. Cross-reference Section 5 — **and the game lets this happen, visibly, repeatedly.**

**Cross-reference mechanics/combat_feel.md** — Jake's baseline is HP 200-380, and cross-reference physics/combat_physics.md: **a spear line does not care.**

### 3.3 COHESION

**The system's central number, and it is never displayed.**

Each segment has Cohesion 0-100. It falls with casualties, flanking, missile fire, and officer loss. It rises with Jake's presence, an officer's presence, a won exchange, and — cross-reference physics/sound_physics.md Section 8 — **a war-cry that is heard.**

**At Cohesion 0 the segment breaks and runs, and cross-reference army_system.md Section 9.1: men who run can still die.**

### 3.4 Reading It

**Cross-reference:** progression_overview.md Section 1.1.

**No bars.** The player reads a segment by looking at it:

- Men stepping back a half-pace at a time — **Cohesion falling**
- Men glancing sideways rather than forward — **about to break**
- A shield wall with gaps in it — **casualties past sustainable**
- **Someone at the back already walking** — **it is over there**

---

## 4. THE THREE ORDERS

**Cross-reference:** army_system.md Section 6.3.

### 4.1 HOLD · PRESS · BREAK OFF

**That is the entire vocabulary.** No formations, no facing, no targeting.

### 4.2 How They Are Issued

**Shouted.** Cross-reference physics/sound_physics.md Sections 2 and 13 — **a voice carries, and it carries less in wind, in rain, and in noise.**

**Range:** ~60m clear · ~30m in engagement noise · **~15m in a rainstorm.** Cross-reference highmoore_weather.md.

### 4.3 Whether They Are Obeyed

```
ObeyChance = base(Loyalty, Training)
           × heardModifier(distance, noise, weather)
           × officerPresent ? 1.4 : 1.0
           × commandOverCapacity ? 0.6 : 1.0
```

**Cross-reference army_system.md Section 6.1** — **over capacity, orders arrive late and formations break**, and the player sees it as men doing the previous thing for eight seconds too long.

### 4.4 Officers Relay

Cross-reference army_system.md Section 6.2. A named officer in a segment **repeats Jake's order to that segment**, which is what command capacity physically is.

**Design note:** cross-reference npcs/shippable_specialists.md Section 9 — **Lt. Sandford's voice is the single most valuable thing about him.**

---

## 5. LOSING

### 5.1 It Happens

**Design law:** cross-reference Section 1.1. **Battles are lost by competent players, regularly, and the game does not reload.**

### 5.2 What Losing Is

Cross-reference Section 3.3 — the line breaks. Cross-reference army_system.md Section 9.1: **losses are permanent**, cross-reference army_system.md Section 9.2: **named people die**, and cross-reference colonial_war/war_state_system.md: **the region's control shifts.**

**Jake survives.** Cross-reference Section 5.3.

### 5.3 Jake's Death

**Cross-reference:** mechanics/combat_feel.md.

If Jake falls, he is **pulled out** — by crew, by an officer, by Alliance fighters. Cross-reference npcs/named_crew_deep_dives.md.

**Design law:** this is not a fail state and not a reload. **The battle continues without him and is usually lost**, and cross-reference Section 5.2: everything that follows from losing, follows.

### 5.4 Withdrawing

**BREAK OFF is a real order and a real option.** Cross-reference army_system.md Section 6.3 — a withdrawal costs less than a rout, and cross-reference Section 3.3: **a segment ordered off while it still has Cohesion leaves in order.**

---

## 6. SIEGE ELEMENTS

**Cross-reference:** colonial_war/castle_sieges.md.

### 6.1 The Handoff

Cross-reference colonial_war/castle_sieges.md Sections 2 and 4 — **Approach and Investment happen outside this system.** The battle system runs Breach and Assault.

### 6.2 Engineering

**Cross-reference:** army_system.md Section 3.6, highmoore_reconstruction.md Section 3.

**Cadwall masons are siege engineers.** Cross-reference colonial_war/castle_sieges.md Section 3.3 — with them, a wall can be breached without hot shot, and cross-reference colonial_war/castle_sieges.md Section 9.4: **which means the state treasure survives.**

**Design note:** cross-reference colonial_war/castle_sieges.md Section 8.5 — *"the decision is made with cannons."* **Masons are how a player buys a different decision.**

### 6.3 Naval Support

Cross-reference colonial_war/castle_sieges.md Section 3.2, physics/ship_physics.md — bombardment as a battle element, and cross-reference physics/fire_physics.md: **hot shot burns the objective.**

---

## 7. SOUND

**Cross-reference:** physics/sound_physics.md.

### 7.1 The Design Priority

**Battles are an audio problem before they are a visual one.**

- **Orders must be legible** at the ranges in Section 4.2 and illegible past them
- **Cohesion is audible** — cross-reference Section 3.4: **a segment about to break gets quieter**, and this is the most reliable tell in the system
- Cross-reference physics/sound_physics.md Section 4 — **a walled courtyard builds reverb**, and cross-reference princess_quest_belos.md Section 5.7: it can become continuous

### 7.2 The Silence

**Design law:** cross-reference physics/sound_physics.md Section 14, style_bible.md Pillar 3.

**Every battle ends in near-silence**, and it lasts. Cross-reference physics/sound_physics.md Section 14.1 — wounded, birds returning, and the reverb dying, and cross-reference princess_quest_belos.md Section 10.2: **it takes almost a minute.**

---

## 8. WHAT A BATTLE IS ALLOWED TO FEEL LIKE

**Cross-reference:** style_bible.md Pillar 3, princess_quest_belos.md Section 1.1.

### 8.1 The Prohibitions

- **No score during the fighting.** Cross-reference highmoore_music.md — the same discipline. A cue may exist at the opening and at the silence, and nowhere between.
- **No slow motion.** Cross-reference colonial_war/colonial_war_bosses.md Principle 1.5.
- **No kill counter.**
- **No triumph.** Cross-reference style_bible.md Pillar 3 — **no battle in Dark Arisen ends in celebration**, including the ones that are victories.

### 8.2 The Aftermath

**Cross-reference:** physics/sound_physics.md Section 14.1, npcs/the_named_dead.md.

**Player-controlled, no prompt, no timer.** Cross-reference princess_quest_the_lake.md Section 10, princess_quest_belos.md Section 10.2 — **the established discipline: the player decides when to leave.**

Cross-reference army_system.md Section 9.2 — **and at least one of the dead is somebody they had spoken to.**

---

## 9. IMPLEMENTATION

```cpp
struct Segment {
    ForceRef    units[4];
    uint8_t     cohesion;      // 0–100, never displayed
    Order       currentOrder;  // Hold, Press, BreakOff
    float       frontMeters;   // ~40 per segment
    bool        jakePresent;
    OfficerID   officer;       // relays orders; +40% obey
};

struct Battle {
    Segment     line[3..6];
    Momentum    momentum;      // derived from segment cohesion deltas
    // no win/lose flag until every segment resolves
};
```

**Cohesion tick:** 1 Hz. **Order propagation:** modelled as travel time, not instant.

**Tuning levers, in order:** Jake's cohesion contribution (Section 3.1) · order-heard ranges (Section 4.2) · cohesion decay per casualty · officer multiplier.

---

## 10. INTEGRATION MAP

- **army_system.md** ← force types, upkeep, the three orders, command capacity, permanent losses
- **battle_catalog.md** → the specific engagements
- **colonial_war/castle_sieges.md** ← the four siege phases hand off to Breach and Assault
- **colonial_war/indigenous_liberation_alliance.md** ← the Rising; Jake receives objectives, not command
- **mechanics/combat_feel.md, physics/combat_physics.md** ← Jake in a segment
- **physics/sound_physics.md** ← orders, cohesion, reverb, and the silence afterward
- **physics/wind_systems.md, highmoore_weather.md** ← weather as a combatant
- **physics/destruction_physics.md, fire_physics.md** ← breaching, and what burning costs
- **princess_quest_belos.md** ← the one sequence that breaks these rules, deliberately, and is not the model
- **npcs/the_named_dead.md** ← the aftermath

---

## 11. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** three orders shouted in one voice, relayed by officers in theirs, into segments of hired men, militia, and levies who answer differently.
- ✅ **Pillar 2 (Lore Through Objects):** a shield wall with gaps in it. Men glancing sideways. Someone at the back already walking.
- ✅ **Pillar 3 (Layered Death):** no score, no slow motion, no kill counter, no triumph, permanent losses, at least one named dead per battle, and a minute of near-silence afterward with no prompt telling the player when to leave.
- ✅ **Pillar 4 (Layered Humor):** none during. Cross-reference army_system.md Section 8.2 — **the camp before is where it lives**, and the contrast is the point.
- ✅ **Pillar 5 (Theme — Revenge):** Jake is worth forty men and cannot win a battle alone, and the game lets him win his segment while the line loses, and there is no reload, and the dead stay dead.

---

## 12. CONCLUSION

One Man in a Line. Not an RTS, not a crowd of four thousand, not a cutscene with inputs — four hundred men in a real simulation on a Highmoore road, in wind, with a front divided into segments that hold or press or break, and one captain standing in one of them worth about forty men because the men beside him do not run.

Three orders. Hold, press, break off. Shouted, carrying sixty metres in the clear and fifteen in a rainstorm, relayed by borrowed officers, obeyed on a formula that includes whether anyone could hear.

No bars. You read a segment by watching men step back a half-pace at a time, then glance sideways instead of forward, then by noticing somebody at the back has already started walking. And by listening — because a line about to break gets **quieter**, and that is the most reliable tell in the system.

Battles are lost. Regularly. By good players. There is no reload, the losses are permanent, at least one of the dead is somebody who had a name and a line, and the region's flag changes.

And no battle in Dark Arisen ends in celebration, including the ones that are victories.

**Phase 9 Document 5 of 6.**

**A line about to break gets quieter.** 🛡️
