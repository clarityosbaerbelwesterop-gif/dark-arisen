# INTERACTION SYSTEM — REACHING FOR THINGS

**Status:** Draft v1.0
**Phase:** Phase 12 — Interaction, Crew & Travel (Document 1 of 6)
**Related:** social_system.md, crew_system.md, ship_as_home.md, animation_system.md, camera_system.md, technical_targets.md, mechanics/inventory_crafting.md, treasure_system.md, quest_activation.md, physics/destruction_physics.md, style_bible.md
**Purpose:** How Jake touches the world — the no-highlight doctrine, the four interaction classes, carrying, doors and containers, the absence of fade-to-black, inventory as a physical thing, and what cannot be interacted with.
**Inspirations:** Red Dead Redemption 2 (reaching, weight, and the courage to make it slow), Kingdom Come: Deliverance, Amnesia's physical doors, Death Stranding (cargo as a relationship), and the rejection of the contextual-highlight open world

---

## 1. THE LAWS

### 1.1 NO HIGHLIGHT

**Cross-reference:** quest_activation.md Section 3.1, technical_targets.md Section 8.

**No outline. No glow. No sparkle. No "press E" floating in world space at range.**

Cross-reference treasure_system.md Section 1.3 — **treasure is found by looking**, and so is everything else.

**The concession:** a small prompt appears at genuine interaction range (**1.4m**), in the corner, not on the object.

### 1.2 NO FADE-TO-BLACK

**Cross-reference:** animation_system.md Section 8.

**Everything is reached for.** Picking up, opening, carrying, drinking, reading, lighting. Cross-reference animation_system.md Section 1.2 — **contextual transitions everywhere**, and none of them are a cut.

### 1.3 INTERACTION HAS DURATION

Cross-reference princess_quest_the_lake.md Section 5.3 — **cleaning weapons on the stones is a real animation and takes real time**, and the player can stop.

### 1.4 THE WORLD IS NOT A CONTAINER FIELD

**Design law:** **no lootable barrels.** No crate that contains three coins. Cross-reference economy_core.md Section 6 — **no vendor trash exists**, so nothing needs to be a receptacle for it.

**A container in Dark Arisen contains something because somebody put it there.**

---

## 2. THE FOUR CLASSES

### 2.1 TAKE — instant-ish

Small objects. A reach animation, ~0.6s, interruptible.

Cross-reference mechanics/inventory_crafting.md — **and the object goes into a physical inventory** (Section 6), not a grid of icons.

### 2.2 USE — contextual

Doors, levers, winches, ropes, braziers, the minecart brake. Cross-reference crystal_caves.md Section 3.3 — **the Crystal Caves' entire upper works are a Use puzzle.**

Cross-reference technical_targets.md Section 2.1 — **adaptive triggers on PS5 carry resistance here**, and it is not decoration.

### 2.3 CARRY — a state, not an action

Cross-reference Section 4.

### 2.4 EXAMINE — the reading class

**Cross-reference:** treasure_system.md Section 2.3, style_bible.md Pillar 2.

**Twenty percent of all treasure in this game is paperwork**, and Examine is how it is read: Jake holds the document, cross-reference camera_system.md Section 2.2 — **Anchored, not Authored**, and the world is visible around it.

**Cross-reference princess_quest_arion_falls.md Section 4.4** — the real letter is Examine, player-paced, no skip, with rain audible on the window.

---

## 3. DOORS

**Cross-reference:** animation_system.md Section 1.2.

### 3.1 They Are Physical

Push, pull, shoulder. **No door in Dark Arisen opens by teleport or by fade.**

### 3.2 Speed Is Chosen

A held input opens slowly and quietly; a pushed input opens fast. Cross-reference mechanics/stealth.md, physics/sound_physics.md — **and the difference is audible to anyone in the room.**

Cross-reference colonial_war/castle_sieges.md Section 4.2 — **the Coventry House night infiltration is built around this.**

### 3.3 They Can Be Barred, Broken, and Left

Cross-reference physics/destruction_physics.md. **A door Jake breaks stays broken** for the rest of the game.

---

## 4. CARRYING

**Cross-reference:** animation_system.md Section 8, progression_overview.md Section 2.1.

### 4.1 The Load

**80kg → 130kg.** Cross-reference skill_tree.md Section 2.5 — grown by acts, not objects.

### 4.2 Carrying Affects Everything

Cross-reference physics/movement_physics.md — gait, turn rate, climb, swim, and **combat readiness.** Over 85% load, Jake cannot sprint or deflect cleanly.

### 4.3 CARRYING PEOPLE

**Cross-reference:** colonial_war/colony_system_core.md Section 3, animation_system.md Section 8.

A full-body animation set with real weight. **The game uses it a great deal** — cross-reference colonial_war/castle_ownership.md Principle 1.2: *"the population of Jake's settlements is a manifest of his voyages."*

**Design law:** cross-reference npcs/children_and_families.md — **children are carried differently**, and the animation is gentle, and it is used in the transport missions and nowhere near combat.

### 4.4 Setting Down

Cross-reference Section 1.2 — **also an animation.** Cross-reference princess_quest_the_lake.md Section 8.3: **and one of them is a bad catch that puts them both on the stones.**

---

## 5. WHAT CANNOT BE INTERACTED WITH

**Cross-reference:** style_bible.md Pillar 3, npcs/children_and_families.md.

**Hard prohibitions, enforced as absence rather than refusal:**

- **Any child, hostilely.** Cross-reference npcs/children_and_families.md — **there is no attack input that connects.** Cross-reference princess_quest_arion_falls.md Section 5.2.
- **Any non-combatant in Arion during the fall.** Same mechanism.
- **Corpse faces.** Cross-reference facial_animation.md Section 6.4 — the rig is off.
- **Ellis.** Cross-reference the_wizard.md Section 9.2 — **the attack input connects and nothing happens and it is never explained.**
- **The undercroft stair, before the Wizard's line.** Cross-reference princess_quest_belos.md Section 6.3.

---

## 6. INVENTORY

**Cross-reference:** mechanics/inventory_crafting.md.

### 6.1 It Is a Pack, Not a Grid

Cross-reference Section 4.1 — **weight-limited, physically arranged**, and cross-reference technical_targets.md Section 8: **no rarity colours, no sort-by-power.**

### 6.2 Everything Can Be Dropped

**Cross-reference:** quest_design_philosophy.md Section 9.

**Including quest items.** Cross-reference Section 6.3 — **including the Crystal Katana.**

### 6.3 The Two Objects That Behave Oddly

- **The Crystal Katana.** Cross-reference crystal_katana.md Section 6 — **it glows and cannot be hidden**, which makes it a stealth penalty and a light source, and cross-reference crystal_katana.md Section 5.1: **it has no durability entry, so its inventory card is missing a line every other weapon has.**
- **The two letters.** Cross-reference princess_quest_the_truth.md Section 6.3 — **both in the inventory, both in her hand, one signed in full and one signed with a letter**, and the game never points at the difference.

### 6.4 The Map

**Cross-reference:** quest_activation.md Section 6.4.

**A physical object Jake unfolds**, held in Examine, with the world visible around it. Geography and settlements only. **No quests, no objectives, no player dot.**

---

## 7. IMPLEMENTATION

```cpp
enum class Interact : uint8_t { Take, Use, Carry, Examine };

struct Interactable {
    Interact    verb;
    float       range;        // 1.4m default
    float       duration;     // §1.3; interruptible
    bool        highlightable;// FALSE. Always. No exceptions.
    bool        persistState; // broken doors stay broken §3.3
};
```

**Prohibited:** any outline/glow shader on interactables · fade-to-black on any interaction · lootable generic containers · **any function named `HighlightNearbyLoot`.**

---

## 8. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 2:** twenty percent of treasure is paperwork and Examine is how the game delivers its lore, held in the hands, with the room still around it.
- ✅ **Pillar 3:** the rig is off on corpses; there is no attack input that reaches a child; a stair that cannot be approached until a man in a burning yard mentions it.
- ✅ **Pillar 4:** doors that can be shouldered, and a first mate's opinion about it.
- ✅ **Pillar 5:** the Crystal Katana can be dropped at any time, anywhere, permanently — and no scene in Dark Arisen exists in which Jake does.

---

## 9. CONCLUSION

Reaching For Things. No outline, no glow, no sparkle, no prompt floating on an object across a room — a small line in the corner at one metre forty, and everything else found by looking. Nothing fades to black: he reaches, he opens, he lifts, he sets down, and every one of those is an animation with a duration and can be stopped halfway.

Doors that are pushed or shouldered and stay broken. Containers that hold something because a person put it there. A pack rather than a grid, with weight that reaches into gait and climb and whether he can deflect. People carried with real mass, children carried differently, and one catch that is deliberately bad.

Two letters in the same inventory in the same handwriting, one signed in full and one signed with a letter, and the game never points at it.

And a sword that can be dropped anywhere, at any time, forever — which no scene in the game ever has him do.

**Phase 12 Document 1 of 6.**

**No highlight. Look at the world.** 🤲
