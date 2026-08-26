# CAMERA SYSTEM — THE HOLD AND THE REFUSAL

**Status:** Design Draft v1.0
**Phase:** Phase 10 — Presentation & Technical (Document 1 of 6)
**Related:** cutscene_catalog.md, animation_system.md, facial_animation.md, technical_targets.md, style_bible.md, mechanics/combat_feel.md, physics/sun_shadow_systems.md, physics/sound_physics.md, colonial_war/colonial_war_bosses.md, princess_quest_the_lake.md, princess_quest_belos.md, npcs/the_named_dead.md
**Purpose:** Consolidating the roughly forty camera instructions scattered across the bible into one system — the four laws, the shot vocabulary, the Hold, the Refusal, when control is taken and when it is not, and the explicit prohibitions.
**Inspirations:** Red Dead Redemption 2 (a camera that stays), The Assassination of Jesse James (duration as meaning), Kurosawa (stillness), Michael Haneke (the refusal to cut away and the refusal to look), Roger Deakins, and the total rejection of the modern action-camera vocabulary

---

## 1. THE FOUR LAWS

### 1.1 LAW ONE — THE HOLD

**Cross-reference:** colonial_war/colonial_war_bosses.md Principle 1.5, crystal_guardian.md Section 7.2, princess_quest_arion_falls.md Section 7.3.

**After a significant death, the camera holds four seconds longer than is comfortable.**

Not a zoom. Not a push. **It stays where it was and does not cut.**

**Implementation:** a `HoldShot` state with a fixed duration of **4.0s**, entered automatically on flagged deaths, during which input is accepted but the camera does not respond to look-input. Cross-reference Section 5.2.

**Where it fires:** all nine colonial commanders · the Crystal Guardian · Cassian Arion · Aldric Belos · the archer at the lake · any named crew or teacher death · the Light Elf encounter's ending on an empty hilltop.

### 1.2 LAW TWO — THE REFUSAL

**Cross-reference:** princess_quest_the_lake.md Section 8.2, princess_quest_belos.md Section 9.5, colonial_war/indigenous_liberation_alliance.md Section 6.4.

**The camera does not look at the thing the player expects it to look at.**

Three canonical cases:

- **The arrow.** Cross-reference princess_quest_the_lake.md Section 8.2 — **no drift toward the treeline.** The camera stays on two people dancing and one of them stops.
- **The stair.** Cross-reference princess_quest_belos.md Section 9.5 — the screen holds on cleared rubble in the rain **and then cuts.** It does not go down.
- **The aftermath.** Cross-reference colonial_war/indigenous_liberation_alliance.md Section 6.4 — *"the camera does not find Jake."* At every Liberation Event's closing address, the framing is on the network's leadership and their people, and **Jake is somewhere in the crowd and is not sought.**

### 1.3 LAW THREE — CONTROL IS NOT TAKEN

**Cross-reference:** cutscene_catalog.md Section 2, princess_quest_the_lake.md Section 8.3.

**Default: the player keeps the camera.**

Cross-reference princess_quest_the_lake.md Section 8.3 — thirty seconds in which Jake does not understand what has happened, **with full player control and nothing to do.** Cross-reference princess_quest_belos.md Section 10.2 — the walk out of a burning courtyard is player-controlled.

**Control is taken in exactly nineteen places across the entire game.** Cross-reference cutscene_catalog.md Section 3.

### 1.4 LAW FOUR — NO ACTION VOCABULARY

**Prohibited, absolutely, everywhere, including cutscenes:**

- **Slow motion.** Cross-reference mechanics/combat_feel.md — **the sole exception in Dark Arisen is Rache**, which is a player-triggered mechanic at 30% time scale for 5 seconds, and it is never used cinematically.
- **Finisher cameras.** No orbit, no hero angle, no snap-to on a killing blow.
- **Kill-cam.** Anywhere.
- **Shaky-cam** as an intensity signal. Cross-reference Section 4.3 — handheld exists and is used for one thing only.
- **Dutch angles.**
- **Lens flare as drama.**
- **Speed ramps.**
- **A camera that anticipates.** Cross-reference Section 1.2 — **the camera never knows what is about to happen.**

---

## 2. THE THREE CAMERA MODES

### 2.1 FREE — the default, ~94% of runtime

Standard third-person. Player-controlled. Cross-reference mechanics/combat_feel.md for combat framing and lock-on.

### 2.2 ANCHORED — ~5%

**The camera stops responding to look-input but the player keeps movement.**

Used for: the Hold (Section 1.1) · the two-second inserts (Section 3.2) · the emergence shelf (Section 6.1) · **and every moment where the design wants the player to be looking at something and to still be free to walk away from it.**

**Design note:** Anchored is the bible's most-used special state and it barely exists in other games. Cross-reference princess_quest_the_lake.md Section 8.3.

### 2.3 AUTHORED — ~1%

Full cinematic. Cross-reference cutscene_catalog.md — **nineteen instances.**

---

## 3. THE SHOT VOCABULARY

### 3.1 THE STATIC HOLD

Locked-off. No movement of any kind. **The bible's signature shot.**

Cross-reference crystal_guardian.md Section 7.1 — the Guardian settling into a kneel; cross-reference light_elf_boss.md Section 7.2 — an empty hilltop; cross-reference princess_quest_arion_falls.md Section 7.3 — a courtyard with a body in it and a woman on a stair who has not moved.

### 3.2 THE INSERT

**Two seconds, control taken, one subject, then control returned.**

**Cross-reference princess_quest_belos.md Section 5.3 and crystal_katana.md Section 8.3** — the canonical instance: **Jake's hands, shaking, and a sword that is not.**

**There are five inserts in the game.** Cross-reference cutscene_catalog.md Section 3.2.

### 3.3 THE SLOW PUSH

The only permitted camera movement in an emotional beat, and it is used **six times**, and never faster than **0.15 m/s.**

Cross-reference cutscene_catalog.md Section 4 — including Don Alejandro examining Ethan's Strain, and Elowen's letter on a pillow.

### 3.4 THE WIDE

**Cross-reference:** highmoore_region.md Section 1.2, crystal_caves.md Section 5.4.

Landscape and interior scale. The emergence shelf, the Cathedral, La Ciudadela from the sea, the Highmoore basin.

**Design law:** cross-reference Section 6.1 — **the wide is never a flythrough.** It is a position the player walks into.

### 3.5 THE FOLLOW

Standard traversal framing. Cross-reference mounted_travel.md — mounted framing sits lower and further back and cross-reference physics/cloth_hair_physics.md: **it exists to show cloth and mane in wind.**

### 3.6 THE OVER-SHOULDER

Dialogue. Cross-reference facial_animation.md Section 3 — and it is **not** a shot-reverse-shot system by default. Cross-reference Section 7.2.

---

## 4. SPECIFIC RULINGS FROM THE BIBLE

### 4.1 The Dead Are Positioned, Never Posed

**Cross-reference:** colonial_war/dungeon_design_philosophy.md Section 8.3, style_bible.md Pillar 3.

**No composed corpse. No dramatic silhouette. No lighting on a body that the room does not justify.**

Cross-reference colonial_war/regional_dungeon_catalog.md Section 4.2 — the ash-preserved market, mid-transaction, and **the camera treats it as terrain.**

### 4.2 No Reaction Shots

**Cross-reference:** princess_quest_the_ball.md Section 8.3.

Maerwyn's line at breakfast — *"everyone is very kind to her"* — with a pause half a second too long, **and the scene moves on immediately.** No cut to Jake. No cut to her.

**Design law:** **the camera never tells the player that a line mattered.**

### 4.3 Handheld

**Used for exactly one thing: Jake's physical deterioration.**

Cross-reference animation_system.md Section 5, princess_quest_belos.md Section 5.2 — as Jake's stamina and posture collapse, **the camera acquires a low-frequency instability that is not a shake and is not signposted.**

**Amplitude scales with the wounded state and nothing else.**

### 4.4 Light Is Never Emphasised

**Cross-reference:** princess_quest_the_return.md Section 2.4.

The cloud breaks in the Folds on the ride home and the low light comes across the moor — **and there is no camera emphasis at all, and the player may not look at it.**

### 4.5 The Camera Does Not Enter

**Cross-reference:** princess_quest_belos.md Section 9.5, house_belos.md Section 9.4.

The undercroft. The stair is cleared and the screen holds on it and cuts. **There is no shot of what is down there and none will ever be authored.**

---

## 5. COMBAT

### 5.1 Framing

**Cross-reference:** mechanics/combat_feel.md, physics/combat_physics.md.

Lock-on with a soft leash. Standard third-person. **No automatic reframing on a kill.**

### 5.2 What Combat Never Does

- No hitstop beyond the physical (cross-reference physics/combat_physics.md — impact frames are animation, not camera)
- No zoom on a critical
- No desaturation on low health
- **No vignette.** Cross-reference princess_quest_the_lake.md Section 8.3 — **there is no red edge when the arrow lands, because it did not hit Jake**, and the absence of the vignette the player expects is the information.

### 5.3 The Battle Camera

**Cross-reference:** battle_system.md Section 3.4.

Unchanged from normal combat. **No tactical pull-back, no overview, no drone shot of the line.**

Cross-reference battle_system.md Section 3.4 — the player reads a segment by looking down the line from inside it, and cross-reference battle_system.md Section 7.1: **mostly by listening.**

---

## 6. THE AUTHORED WIDES

### 6.1 THE EMERGENCE

**Cross-reference:** highmoore_region.md Section 1.2, highmoore_overview.md Section 1.2.

**The single most important frame in Phase 8.**

**Implementation:** the counterweight lift's arrival places the player on the shelf in **Anchored** mode facing the basin for **6 seconds**, then releases to Free. Cross-reference Section 2.2 — **the player can immediately turn around and walk back into the mine, and the game does not stop them.**

Cross-reference highmoore_music.md — the second and last cue of the passage sequence.

### 6.2 THE CATHEDRAL

Cross-reference crystal_caves.md Section 5.4. **No authored camera at all.** The room does the work, and cross-reference physics/sound_physics.md Section 4.3: **a player who stands still hears the reverb and looks up on their own.**

### 6.3 THE ARCHIPELAGO WIDES

Cross-reference colonial_war/castle_catalog.md — La Ciudadela from the sea, the Sterling Bastion's approaches, Coventry House. **All Free camera. None authored.**

---

## 7. DIALOGUE

### 7.1 The Default

**Free camera.** Cross-reference npcs/dialogue_system.md — the player can look away, walk in a circle, or stare at the floor while someone talks, **and the conversation continues.**

### 7.2 When It Becomes Authored

**Nineteen conversations across the game.** Cross-reference cutscene_catalog.md Section 3.1.

**Design law:** cross-reference facial_animation.md Section 2 — **authored dialogue framing exists to show a face**, and it is spent only where a face is the content.

### 7.3 The Walk-and-Talk

**Cross-reference:** princess_quest_the_ball.md Section 7, mounted_travel.md Section 8.1.

The terrace. The nine-minute ride to the lake. **Free camera, player movement, and the dialogue plays across it.**

**Design note:** cross-reference princess_quest_the_lake.md Section 4 — the ride's gait is capped and the conversation is not, and **a player who stops to look at the rain hears the same lines from a standstill.**

---

## 8. IMPLEMENTATION

```cpp
enum class CameraMode : uint8_t { Free, Anchored, Authored };

struct CameraState {
    CameraMode  mode;
    float       holdTimer;        // 4.0s on flagged deaths
    bool        acceptsLookInput; // false in Anchored/Authored
    bool        acceptsMoveInput; // TRUE in Anchored — the key distinction
    float       woundInstability; // 0..1, drives handheld; §4.3
};
```

**Prohibited functions, enforced at code review:** `SlowMotion()` (except Rache) · `KillCam()` · `SnapToTarget()` · `ShakeOnDamage()` · `AutoFrameOnDeath()` · `LensFlareIntensityByDrama()`

**Tuning levers:** hold duration (4.0s) · slow-push speed (0.15 m/s) · wound instability amplitude · Anchored release timings.

---

## 9. THE COUNT

| Mode | Instances | % runtime |
|---|---|---|
| Free | continuous | ~94% |
| Anchored | ~140 | ~5% |
| Authored | **19** | ~1% |
| Static Hold | ~45 | — |
| Insert | **5** | — |
| Slow Push | **6** | — |

---

## 10. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** cross-reference Section 7.2 — authored framing is spent on faces, and cross-reference facial_animation.md: register is carried in the face as much as the voice.
- ✅ **Pillar 2 (Lore Through Objects):** cross-reference Section 3.3 — five of the six slow pushes are onto objects, not people: a letter, a cigar, a ledger, a boundary stone, an empty hutch.
- ✅ **Pillar 3 (Layered Death):** the Hold, the Refusal, positioned-never-posed, no finishers, no kill-cams, no slow motion, and **a stair that is cleared and never descended.**
- ✅ **Pillar 4 (Layered Humor):** cross-reference Section 4.2 — **no reaction shots means no camera mugging**, and Highmoore's dry comedy plays entirely in Free camera with nobody underlining it.
- ✅ **Pillar 5 (Theme — Revenge):** the camera never celebrates. Nineteen authored moments in a hundred-and-fifty-hour game, five two-second inserts, and one of them is a pair of shaking hands holding a sword that is not.

---

## 11. CONCLUSION

The Hold and the Refusal. A camera that stays four seconds past comfortable after every death that matters, that does not turn toward the treeline when the arrow lands, that does not find Jake at the ceremony afterward, and that holds on a cleared stair in the rain and then cuts rather than going down.

Ninety-four percent of a hundred and fifty hours in the player's hands. Five percent anchored — looking at something and still free to walk away from it. **Nineteen authored moments in the entire game.**

No slow motion outside a mechanic the player triggers. No finishers, no kill-cams, no zoom on a critical, no vignette when the arrow lands because it did not hit him. No reaction shot when a mother says the worst sentence in the game at breakfast, because the camera never tells the player that a line mattered.

Handheld exists and does exactly one thing: it gets less stable as Jake does.

**Phase 10 Document 1 of 6.**

**The camera never knows what is about to happen.** 🎥
