# TECHNICAL TARGETS — RESOLUTION, FRAMERATE, BUDGETS

**Status:** Design Draft v1.0
**Phase:** Phase 10 — Presentation & Technical (Document 5 of 6)
**Related:** animation_system.md, facial_animation.md, camera_system.md, physics/ (all fifteen), npcs/regional_populations.md, battle_system.md, crystal_caves.md, princess_quest_belos.md, style_bible.md
**Purpose:** Platform targets, resolution and framerate commitments, the frame budget, the eight most expensive systems in Dark Arisen and what they cost, the scenes that define the worst case, and the technical decisions that are locked by design rather than by preference.
**Engine:** Unreal Engine 5 · **Language:** C++ · **Target:** 2027
**Inspirations:** the actual shipping constraints of Elden Ring, Red Dead Redemption 2, and Kingdom Come: Deliverance II, and the discipline of deciding what to cut before the schedule decides for you

---

## 1. THE HARD FLOOR

### 1.1 60 FPS Is Not a Target

**Cross-reference:** animation_system.md Section 4.

**The deflection window is 6 frames.** At 30fps that is 3 frames of a 30Hz sample, which is not a mechanic — it is a coin flip.

**Design law: Dark Arisen does not ship a 30fps mode on any platform.** Every performance decision in this document exists to protect that.

### 1.2 What This Costs

Cross-reference Section 5 — it costs resolution, crowd density, ray tracing, and two systems that will be argued about. **It is not negotiable and the argument does not get reopened.**

---

## 2. PLATFORM TARGETS

| Platform | Resolution | Framerate | Notes |
|---|---|---|---|
| **PS5** | 1440p → 4K (TSR) | **60** locked | primary development target |
| **PS5 Pro** | 4K native-ish (TSR) | **60** locked | PSSR where available |
| **Xbox Series X** | 1440p → 4K (TSR) | **60** locked | parity with PS5 |
| **Xbox Series S** | 1080p → 1440p | **60** locked | see Section 6 |
| **PC minimum** | 1080p | 60 | RTX 3060 / RX 6600 class |
| **PC recommended** | 1440p | 60-90 | RTX 4070 class |
| **PC high** | 4K | 60-120 | uncapped, cross-reference Section 4.3 |

### 2.1 The DualSense Commitment

**Cross-reference:** physics/movement_physics.md, crystal_caves.md Section 4.2, physics/ship_physics.md.

The bible has specified adaptive-trigger behaviour throughout — **the minecart brake is a variable squeeze**, cross-reference crystal_caves.md Section 4.2, and cross-reference physics/ship_physics.md for helm resistance and mechanics/combat_feel.md for weapon weight.

**This is a shipped feature on PS5, not a nice-to-have.**

---

## 3. THE FRAME BUDGET

**16.6ms at 60fps. PS5 target allocation:**

| System | Budget | Note |
|---|---|---|
| Rendering (base) | 6.8ms | |
| Animation | 1.9ms | cross-ref animation_system.md |
| Physics | 2.4ms | see Section 5 |
| AI & crowd | 1.6ms | cross-ref npcs/regional_populations.md |
| Audio | 0.9ms | cross-ref physics/sound_physics.md — **high for this genre, deliberately** |
| Faces (P3) | 0.4ms | cross-ref facial_animation.md §8 |
| Gameplay & systems | 1.1ms | |
| Streaming | 0.7ms | |
| **Headroom** | **0.8ms** | |

### 3.1 The Audio Allocation

**0.9ms is roughly double the genre norm**, and it is deliberate.

**Cross-reference battle_system.md Section 7.1** — *"battles are an audio problem before they are a visual one,"* and cross-reference physics/sound_physics.md: reverb by volume, occlusion, distance-delayed bells, and cross-reference light_elf_boss.md Section 3.4 — **an eleven-minute encounter whose defining feature is the absence of wind noise in a wind region.**

### 3.2 What Was Cut To Pay For It

Cross-reference Section 4.2 — **hardware ray-traced global illumination.** Lumen software only.

---

## 4. RENDERING DECISIONS

### 4.1 Nanite — Yes, Broadly

**Cross-reference:** crystal_caves.md Section 5.4, colonial_war/castle_catalog.md.

The Cathedral, La Ciudadela, ten Highmoore castles, and the Hollow City's eleven square kilometres. **Nanite is load-bearing for this project.**

### 4.2 Lumen — Software Only

**Cross-reference:** Section 3.2, physics/sun_shadow_systems.md.

Hardware RT is cut on console. **What this loses:** cross-reference crystal_caves.md Section 6.1 — the Crystal Cathedral's reflections, and cross-reference colonial_war/regional_dungeon_catalog.md Section 4.1 (the Obsidian Vault's hundred reflections) and crystal_guardian.md Section 3.2.

**Mitigation:** cross-reference Section 4.4.

### 4.3 PC Exception

Hardware RT is available on PC. **The Obsidian Vault and the Threshold Chamber are the two scenes where the difference is a gameplay difference**, and cross-reference crystal_guardian.md Section 3.2: **the false-image count is capped identically on all platforms so the fight is the same fight.**

### 4.4 The Reflection Solution

Screen-space plus parallax-corrected cubemaps, hand-placed in the four crystal environments. **Cross-reference crystal_guardian.md Section 3.2** — the eleven columns' false images are **authored as a fixed set**, not derived from real reflections, which makes them consistent and cheap and cross-reference Section 4.3: identical everywhere.

### 4.5 TSR, Not FSR/DLSS-Dependent

Base upscaling is engine-native so the console experience is not vendor-coupled. DLSS/FSR/XeSS offered on PC as alternatives.

---

## 5. THE EIGHT EXPENSIVE SYSTEMS

**Cross-reference:** the fifteen physics documents.

| # | System | Cost | Where it peaks |
|---|---|---|---|
| 1 | **Water** | 0.9ms | ship_physics + water_physics; the Waterlands; the Deep Harbour |
| 2 | **Cloth & hair** | 0.6ms | cloth_hair_physics; **the lake dance in rain and wind** |
| 3 | **Vegetation** | 0.5ms | vegetation_physics; Rexa canopy; **Highmoore heather in permanent wind** |
| 4 | **Crowd** | 0.7ms | 80-150 NPCs per settlement; **the Arion hall at 200** |
| 5 | **Destruction** | variable | destruction_physics; **the Belos collapse** |
| 6 | **Fire** | 0.4ms | fire_physics; **the Belos courtyard, wind-driven** |
| 7 | **Audio** | 0.9ms | §3.1 |
| 8 | **Combat sim** | 0.8ms | **300 combatants with real physics** |

### 5.1 THE WORST CASE

**Cross-reference:** princess_quest_belos.md.

**The Belos Outer Court, minutes six to nine.**

Simultaneously: 300 combatants · full wounded-set animation on the player · wind-driven fire · a structural collapse of the western range · rain · cloth and hair on all of it · a walled-court reverb build across 300 impacts · and cross-reference camera_system.md Section 4.3, the handheld instability layer.

**This scene sets the engineering ceiling for the entire project.** Cross-reference Section 5.2.

### 5.2 The Mitigations, Decided in Advance

- **Combatant count is 300 and does not rise.** Cross-reference battle_system.md Section 1.3 — the scale discipline exists partly for this reason and is honest about it.
- **The collapse is a pre-authored destruction event**, not a live simulation. Cross-reference physics/destruction_physics.md — **the debris field is authored; the falling is simulated.**
- **Fire is a wind-driven propagation grid**, not per-particle. Cross-reference physics/fire_physics.md.
- **The undercroft is never rendered.** Cross-reference princess_quest_belos.md Section 6 — **the design decision and the performance decision are the same decision**, which is a rare and pleasant thing.

### 5.3 The Second Worst Case

**The Arion ball.** Cross-reference princess_quest_the_ball.md Section 4.1 — 200 NPCs, hundreds of candles, marble with real specular response, cloth on all of them, live musicians, and cross-reference facial_animation.md Section 2.3: **P3 faces across the whole room.**

**No combat. No physics. And it is the second most expensive scene in the game.**

---

## 6. XBOX SERIES S

**Cross-reference:** Section 1.1.

**60fps holds. Everything else gives.**

- 1080p → 1440p TSR
- Crowd: **60 max** per settlement (cross-reference Section 5 — Arion's ball drops to 120)
- Belos: **300 combatants held**, cross-reference Section 5.2 — **this is the one thing that does not scale down**, because the sequence's meaning depends on it
- Vegetation density −35%
- Cloth simulation on named characters only

---

## 7. STREAMING AND WORLD STRUCTURE

### 7.1 Two Continents, One Mountain

**Cross-reference:** crystal_caves.md Section 9.1.

The Crystal Caves are a **permanent two-way route, four to six minutes on repeat runs**, and cross-reference highmoore_endgame_state.md Section 3.1: **the archipelago and Highmoore never need to be resident simultaneously.**

**The mountain is the streaming boundary**, and it was a narrative decision first. Cross-reference Section 5.2 — the second time the design and the engineering agree.

### 7.2 World Partition

Standard UE5 World Partition. Cross-reference colonial_war/war_state_system.md — **the war state is simulated globally at low frequency (1 tick per in-game hour) and does not require regions to be loaded.**

### 7.3 No Loading Screens

Between regions within a continent: **none.** Between continents: **the minecart is the load.** Cross-reference crystal_caves.md Section 4.4.

---

## 8. WHAT IS NOT IN THE BUILD

**Cross-reference:** progression_overview.md Section 1.3, camera_system.md Section 8.

- No dynamic music system. Cross-reference cutscene_catalog.md Section 7.2 — **music does not react, so there is no adaptive layer to build.**
- No slow-motion subsystem outside Rache
- No kill-cam
- No damage numbers
- No minimap
- No compass. Cross-reference highmoore_region.md Section 6.2.
- No quest markers. Cross-reference colonial_war/dungeon_design_philosophy.md Section 3.2.
- **No photo mode.** Cross-reference camera_system.md — the camera discipline is the game's authorial voice, and handing it over undoes forty documents of work.

---

## 9. THE LOCKED DECISIONS

**These are design-locked and not open to performance renegotiation:**

1. **60fps everywhere** (§1)
2. **6-frame deflection window** (animation_system.md §4)
3. **300 combatants at Belos on every platform** (§6)
4. **0.9ms audio** (§3.1)
5. **Full wounded-set animation, always** (animation_system.md §5.3)
6. **The undercroft is never rendered** (§5.2)
7. **No photo mode** (§8)

---

## 10. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 2:** cross-reference Section 5.2 — **the design decision and the performance decision are the same decision**, twice, and both times the design came first.
- ✅ **Pillar 3:** the undercroft is never rendered, is never seen, and costs nothing, and those three facts are one fact.
- ✅ **Pillar 5:** the most expensive scene in the game is nine minutes of a man doing the worst thing he ever does, and the engineering ceiling of a hundred-and-fifty-hour AAA production is set by it.

---

## 11. CONCLUSION

Resolution, Framerate, Budgets. Sixty frames a second on every platform including Series S, because the deflection window is six frames and at thirty that is a coin flip rather than a mechanic — and everything else in this document is what that costs.

Hardware ray tracing cut on console to pay for nine-tenths of a millisecond of audio, in a game where a line about to break gets quieter and an eleven-minute encounter is defined by the absence of wind noise. Reflections authored by hand in four crystal environments so that a fight against something that tracks light is the same fight on every machine.

A mountain that is the streaming boundary because a narrative designer decided two continents should not know about each other, and a chamber under a collapsed western range that is never rendered because it is never shown because the game refuses to look at it.

And an engineering ceiling set by three hundred men in a walled courtyard in the rain, with fire, with a collapse, with a man falling apart in full wounded animation, in minute six of nine.

**Phase 10 Document 5 of 6.**

**Sixty frames. That is not a target.** ⚙️
