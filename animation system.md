# ANIMATION SYSTEM — WEIGHT, POSTURE, DETERIORATION

**Status:** Design Draft v1.0
**Phase:** Phase 10 — Presentation & Technical (Document 3 of 6)
**Related:** camera_system.md, facial_animation.md, cutscene_catalog.md, technical_targets.md, mechanics/combat_feel.md, physics/combat_physics.md, physics/movement_physics.md, physics/cloth_hair_physics.md, mounted_travel.md, crystal_katana.md, princess_quest_belos.md, style_bible.md
**Purpose:** The animation architecture — the three pillars made technical, weight classes, the posture system's visual language, the wounded set, the deterioration curve, mounted animation, and the specific animations the bible has already specified by name.
**Inspirations:** Sekiro (deflection readability), Bloodborne (aggression and recovery frames), Red Dead Redemption 2 (weight, contextual transitions, and the courage to be slow), Kingdom Come: Deliverance (armour as mass), The Last of Us Part II (injury persistence), and the refusal of animation-cancelling as a systems crutch

---

## 1. THE THREE PILLARS, TECHNICALLY

**Cross-reference:** mechanics/combat_feel.md — the bible's founding statement: **Bloodborne (combat) · RDR2 (world) · Sekiro (posture).**

### 1.1 BLOODBORNE — Commitment

**Every attack has recovery and the recovery is real.** No cancel-into-dodge, no cancel-into-block.

**Cross-reference physics/combat_physics.md** — recovery frames scale with weapon weight class (Section 3), and **the player is inside their own animation.**

### 1.2 RDR2 — Weight and Transition

**Cross-reference:** physics/movement_physics.md.

**No instant direction change.** Jake has mass. Turning, starting, and stopping cost frames, and cross-reference Section 6: **surface, slope, and load all modify them.**

**Contextual transitions everywhere:** mounting, doors, ladders, climbing over rather than through, and cross-reference Section 8 — **picking things up is an animation, not a fade.**

### 1.3 SEKIRO — Readability

**Cross-reference:** mechanics/combat_feel.md, physics/combat_physics.md.

**Every attack is legible at least 8 frames before contact**, and cross-reference Section 4: **the deflection window is 6 frames at 60fps** and the animation must make it findable.

---

## 2. THE POSTURE SYSTEM, VISUALLY

**Cross-reference:** mechanics/combat_feel.md — Posture 100 baseline, 175 ceiling.

### 2.1 Posture Is Not a Bar

**Cross-reference:** progression_overview.md Section 1.1.

There **is** a posture indicator in combat — it is the one HUD concession in Dark Arisen, cross-reference mechanics/combat_feel.md — **and the animation must make it redundant.**

### 2.2 The Five Postural States

| State | Posture | Visual |
|---|---|---|
| **Set** | 100-80% | full guard, weight centred, breathing even |
| **Pressed** | 79-55% | guard higher and tighter, weight back, steps shorter |
| **Failing** | 54-30% | guard drops between exchanges, visible recovery breaths |
| **Broken-imminent** | 29-10% | **off-hand comes up to brace the weapon**, feet cross |
| **Broken** | <10% | one knee, weapon down, ~1.4s vulnerability |

**Design law:** cross-reference Section 2.1 — **a player who never looks at the indicator loses nothing.** That is the acceptance test.

### 2.3 Enemy Posture

Identical five states, and cross-reference colonial_war/colonial_war_bosses.md Section 2.2 — **a broken commander is a person on one knee, and the game gives the player a beat to decide what that means.**

---

## 3. WEIGHT CLASSES

**Cross-reference:** physics/combat_physics.md, mechanics/inventory_crafting.md.

| Class | Mass | Startup | Recovery | Examples |
|---|---|---|---|---|
| **Light** | <1.3kg | 9f | 14f | dagger, **Crystal Katana (1.1kg)** |
| **Medium** | 1.3-2.2kg | 13f | 20f | cutlass, arming sword, sabre |
| **Heavy** | 2.2-4.0kg | 19f | 31f | boarding axe, broadsword |
| **Great** | >4.0kg | 26f | 44f | Vega's two-hander, maul |
| **Polearm** | varies | 15f | 24f | pike, boarding pike, spear |

### 3.1 The Katana Exception

**Cross-reference:** crystal_katana.md Sections 4 and 5.2.

**1.1kg, Light class, fastest chain in the game — and its stamina cost does not scale with Jake's condition.**

**Animation consequence, and it is the whole point:** cross-reference Section 5.3. **The Katana's animation set does not degrade.** Every other weapon in Dark Arisen swaps into the wounded set as Jake deteriorates. **The Katana plays its clean animations at three percent health.**

Cross-reference crystal_katana.md Section 5.3 — *"the sword comes up cleanly, every time, at the same speed."*

---

## 4. THE DEFLECTION WINDOW

**Cross-reference:** mechanics/combat_feel.md, physics/combat_physics.md.

**6 frames at 60fps.** Cross-reference technical_targets.md Section 3 — **this is why 60fps is a hard floor and not a target.**

### 4.1 The Tell

Every attack: **8+ frames of wind-up with a silhouette change readable at 15 metres.**

**Cross-reference crystal_guardian.md Section 4.2** — the Guardian's Step is four frames and is *not* an attack, and cross-reference crystal_guardian.md Section 4.4: **its attacks obey the 8-frame rule and its movements do not**, which is the whole encounter.

### 4.2 The Spark

**Cross-reference:** crystal_guardian.md Section 4.3, physics/combat_physics.md.

Perfect deflection produces light and sound — and cross-reference crystal_guardian.md Section 3.2: **in one fight, that is how the boss finds you.**

---

## 5. THE WOUNDED SET

**Cross-reference:** physics/combat_physics.md, princess_quest_belos.md Section 5.2.

### 5.1 Persistence

**Cross-reference:** mechanics/combat_feel.md.

Injuries persist across encounters until treated. Cross-reference npcs/apothecaries_and_alchemists.md — **and the animation shows which limb.**

### 5.2 The Four Wounded Layers

| Layer | Trigger | Effect |
|---|---|---|
| **Winded** | Stamina <30% | breathing audible, guard slower to return |
| **Hurt** | HP <60% | favoured side, off-hand guarding a wound |
| **Bad** | HP <30% | stance collapsed, walk becomes a limp, **run becomes a stagger-run** |
| **Failing** | HP <12% | cannot sprint, weapon drags between swings |

**Cross-reference camera_system.md Section 4.3** — **the handheld camera instability keys off this and nothing else.**

### 5.3 THE DETERIORATION CURVE

**Cross-reference:** princess_quest_belos.md Section 5.2, crystal_katana.md Section 8.2.

**The single most important animation work in Dark Arisen.**

Across the nine minutes at Belos, Jake passes through all four layers in sequence, and cross-reference Section 3.1: **the Katana does not.**

**Requirement:** the full wounded set must read at a distance, in rain, in firelight, from behind, with 300 combatants on screen. Cross-reference technical_targets.md Section 5.

**Cross-reference cutscene_catalog.md Section 4** — and then the game takes two seconds and shows his hands.

---

## 6. LOCOMOTION

**Cross-reference:** physics/movement_physics.md.

### 6.1 The Modifiers

Surface (six types) · slope · load (cross-reference mechanics/inventory_crafting.md — 80-130kg carry) · wounded layer · wetness · **and cross-reference physics/snow_ice_physics.md, sand_desert_physics.md: snow depth and sand.**

### 6.2 The Named Surfaces

Cross-reference physics/sound_physics.md — **four Highmoore surfaces with distinct footfall** (road, moor, bog, stone), and cross-reference crystal_caves.md Section 6.1: **polished crystal, which is slick.**

### 6.3 Climbing and Traversal

**Cross-reference:** physics/vegetation_physics.md Section 4 (vine traversal), crystal_caves.md Section 5.3.

**No ledge-magnetism.** Cross-reference Section 1.2 — Jake gets over things with visible effort and occasionally does not.

---

## 7. THE NAMED ANIMATIONS

**Cross-reference:** the bible's existing specifications. These are already written and are collected here as a build list.

| Animation | Source | Note |
|---|---|---|
| **The bad catch** | princess_quest_the_lake.md §8.3 | **deliberately clumsy.** Both go down onto the stones. |
| **Jake's hands, shaking** | princess_quest_belos.md §5.3 | the game's most-scrutinised 2 seconds |
| **The Guardian settling** | crystal_guardian.md §7.1 | **settles into a kneel; does not fall** |
| **Picking up the Katana** | crystal_guardian.md §7.3 | requires walking around the thing he killed |
| **Vega standing back up** | colonial_war_bosses.md §8.4 | posture broken, refuses to stay down |
| **The bailiffs leaving** | krone_colonies.md §6.10 | individually, at different points, walking |
| **Cassian's stop** | arion_falls.md §6.4 | mid-sentence, on seeing paper |
| **The dance (×2)** | the_ball.md §6, the_lake.md §7.2 | **neither of them is good at it** |
| **Big Tom holding the Katana** | crystal_katana.md §5.1 | he cannot find a purchase and it shows |
| **Three attempts to mount** | mounted_travel.md §2.2 | the first horse |
| **Cleaning weapons on the stones** | the_lake.md §5.3 | real duration, real animation |

---

## 8. WORLD INTERACTION

**Cross-reference:** Section 1.2, mechanics/inventory_crafting.md.

**No fade-to-black pickups.** Everything is reached for.

**Cross-reference colonial_war/colony_system_core.md Section 3** — **and people are carried**, which is a full-body animation set with weight, and cross-reference colonial_war/castle_ownership.md Section 4.2: the game uses it a great deal.

---

## 9. MOUNTED

**Cross-reference:** mounted_travel.md, fauna/land_animals.md.

### 9.1 The Horse Is Animated as an Animal

**Cross-reference:** mounted_travel.md Section 4.3 — **mood is communicated through ears, head carriage, and gait quality**, with no UI.

Cross-reference physics/cloth_hair_physics.md — **mane and tail are the primary mood readout** and cross-reference camera_system.md Section 3.5: **the mounted camera exists to show them.**

### 9.2 The Four Gaits

Walk · trot · canter · gallop, with **real transition animations** in both directions. Cross-reference mounted_travel.md Section 5.1.

### 9.3 Jake Rides Badly

**Cross-reference:** mounted_travel.md Section 7.1, princess_quest_the_lake.md Section 4.3.

**A distinct, permanently worse rider layer.** He is a sailor. Cross-reference the_lake.md Section 4.3 — *"you ride like a man who has spent his life on something that moves under him for other reasons"* — **and it must be visible next to Elowen, who rides beautifully, for nine minutes.**

### 9.4 The Two Rides

**Cross-reference:** mounted_travel.md Section 8. The capped nine minutes, and the suspended-stamina gallop.

**Animation note:** cross-reference mounted_travel.md Section 9.3 — **the arrival at Belos plays a "finished" state**: blown, head down, sides heaving, and it holds that for the entire nine minutes it stands in the gateway.

---

## 10. IMPLEMENTATION

```cpp
struct AnimState {
    WeightClass weapon;        // Light..Polearm
    PostureVis  posture;       // Set..Broken
    WoundLayer  wound;         // None, Winded, Hurt, Bad, Failing
    Surface     ground;
    float       loadRatio;     // carried / carryKg
    bool        katanaEquipped;// §3.1 — suppresses wound-set swap
};
```

**Prohibited:** attack-cancel-into-dodge · attack-cancel-into-block · ledge magnetism · instant turn · fade-to-black interaction · **any animation-cancel that exists to make combat feel faster.**

**Tuning levers:** deflection window (6f — **do not change**) · recovery frames per class · wound layer thresholds · locomotion transition costs.

---

## 11. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1:** cross-reference facial_animation.md — body language is register. A Highmoore yeoman stands differently to an Imperial officer.
- ✅ **Pillar 2:** Big Tom looking for somewhere to hold a sword. Weapons cleaned on stones for real duration.
- ✅ **Pillar 3:** the Guardian settles rather than falls. Vega stands back up. No finisher animation exists in the game.
- ✅ **Pillar 4:** two dances, and neither of them is good at it, and that is authored rather than emergent.
- ✅ **Pillar 5:** every weapon in Dark Arisen degrades into the wounded set as Jake falls apart. **One does not.**

---

## 12. CONCLUSION

Weight, Posture, Deterioration. A six-frame deflection window that makes sixty frames a second a floor rather than a target, and an eight-frame tell on every attack in the game so the window can be found. Five postural states that make the one HUD element in Dark Arisen redundant to a player who stops looking at it.

Four wounded layers that persist across encounters and drive the only handheld camera in the game. A sailor who rides badly and visibly, for nine minutes, next to somebody who does not. A horse whose mood lives in its ears and its mane and never in an interface. A catch that is deliberately clumsy and puts them both on the stones.

And a deterioration curve across nine minutes in a Stonecountry courtyard where a man passes through winded, hurt, bad, and failing — while the thing in his hands plays its clean animation set at three percent health, at the same speed, every time.

**Phase 10 Document 3 of 6.**

**Every weapon degrades with him. One does not.** 🎞️
