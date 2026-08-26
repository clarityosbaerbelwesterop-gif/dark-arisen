# CUTSCENE CATALOG — THE NINETEEN, AND THE ONES THAT ARE NOT

**Status:** Design Draft v1.0
**Phase:** Phase 10 — Presentation & Technical (Document 2 of 6)
**Related:** camera_system.md, animation_system.md, facial_animation.md, highmoore_music.md, style_bible.md, and every phase document
**Purpose:** The complete cutscene register — the nineteen authored sequences with camera, music, and animation specified; the five inserts; the six slow pushes; and — critically — the twenty-two major moments that are deliberately **not** cutscenes and must never be built as ones.
**Handoff note:** this document defines *where, what camera, what music, what animation.* Claude Code implements. **The "not a cutscene" list in Section 6 is the more important half of this document.**

---

## 1. THE GOVERNING LAW

**Cross-reference:** camera_system.md Section 1.3.

**Default: the player keeps control.**

A hundred and fifty hours of Dark Arisen contain **nineteen authored cinematics.** Cross-reference camera_system.md Section 9. For comparison, a typical AAA title of this scope ships 120-300.

**Design law:** cross-reference Section 6 — **the game's most important moments are in the Section 6 list, not this one.** If a moment appears in Section 6 and is built as a cutscene, the scene is destroyed. That is not an exaggeration and it applies to at least eight of them.

---

## 2. THE NOTATION

Each entry gives:

**CAMERA** — cross-reference camera_system.md Section 3 for the vocabulary
**MUSIC** — cross-reference highmoore_music.md for Phase 8; *silence* is a specification, not an omission
**ANIMATION** — cross-reference animation_system.md, facial_animation.md; **P1/P2/P3** = performance-capture priority tier
**LENGTH** — target runtime

---

## 3. THE NINETEEN

### 3.1 CHAPTER 1-3 — THE OPENING

**#1 — THE BROTHER**
Cross-reference characters/ethan_harlow.md. The game's opening; Ethan's death.
**CAMERA:** static hold, single position, no coverage · **MUSIC:** none until the last eight seconds · **ANIMATION:** P1, full facial · **LENGTH:** 2:10

**#2 — THE LIBERACIÓN**
Cross-reference mechanics/ship_management.md. Jake takes the ship; the crew assembles.
**CAMERA:** slow push (0.15 m/s) onto the ship's name, then release · **MUSIC:** the main theme, first statement · **ANIMATION:** P2 · **LENGTH:** 1:20

---

### 3.2 CHAPTER 4-6 — THE ARCHIPELAGO

**#3 — THE FIRST LETTER**
Cross-reference colonial_war/retaliation_system.md Section 3.2. Stage 1 retaliation; the Imperial letter arrives in the great cabin.
**CAMERA:** slow push onto the letter on the table · **MUSIC:** none · **ANIMATION:** P3, hands only · **LENGTH:** 0:35

**#4 — THE GROVE**
Cross-reference fauna/cannabis_system.md, characters/ethan_harlow.md. Ethan's Grove found.
**CAMERA:** anchored wide on entry (6s), then free · **MUSIC:** Ethan's motif, unresolved · **ANIMATION:** P2 · **LENGTH:** 0:50

**#5 — THE DREAM FIGHT ENTRY**
Cross-reference fauna/cannabis_system.md Section 6. The ceremony; the transition.
**CAMERA:** authored, and the only sequence in the game permitted a non-realist visual grammar · **MUSIC:** cross-reference the sage documents — Mama Jacinta's, sung, diegetic, becoming non-diegetic · **ANIMATION:** P1 · **LENGTH:** 1:40

**#6 — DON ALEJANDRO AND THE STRAIN**
Cross-reference npcs/cigar_collector_don_alejandro.md, fauna/cannabis_system.md. He is given Ethan's Strain and understands what it is.
**CAMERA:** slow push, then static hold on his hands · **MUSIC:** none · **ANIMATION:** **P1, the highest facial priority in the archipelago** · **LENGTH:** 1:05

---

### 3.3 CHAPTER 6-8 — HIGHMOORE

**#7 — THE EMERGENCE**
Cross-reference highmoore_region.md Section 1.2, camera_system.md Section 6.1.
**CAMERA:** anchored wide, 6s, facing the basin, then release. **Not a flythrough.** · **MUSIC:** cross-reference highmoore_music.md Section 3 — warm, minor, does not resolve · **ANIMATION:** none required · **LENGTH:** 0:06 anchored

**#8 — THE VOICE FROM BEHIND**
Cross-reference princess_quest_the_ball.md Section 4.4. *"I've never seen you before."*
**CAMERA:** authored over-shoulder for the turn only, then free · **MUSIC:** diegetic — the Arion musicians, cross-reference highmoore_music.md Section 3 · **ANIMATION:** **P1** · **LENGTH:** 0:18

**#9 — THE EJECTION**
Cross-reference princess_quest_the_ball.md Section 8.4. Cassian at the gate; the grey.
**CAMERA:** authored two-shot, static, no coverage cuts · **MUSIC:** none · **ANIMATION:** P1 · **LENGTH:** 1:15

**#10 — THE LETTER (FALSE)**
Cross-reference princess_quest_the_lake.md Section 2.1. Ellis hands it over on the fell.
**CAMERA:** static two-shot, wind, wide · **MUSIC:** none · **ANIMATION:** P2 · **LENGTH:** 0:40

**#11 — THE ARROW**
Cross-reference princess_quest_the_lake.md Section 8.
**CAMERA:** **the camera does not move.** Cross-reference camera_system.md Section 1.2. Control is taken for **1.5 seconds only** — the catch — and then returned. · **MUSIC:** cross-reference highmoore_music.md Sections 4-5 — **the rain-dance cue continues unchanged for 90 more seconds and finishes** · **ANIMATION:** **P1**, and cross-reference animation_system.md Section 7: **the catch is deliberately bad** · **LENGTH:** 0:01.5

**#12 — THE LETTER (REAL)**
Cross-reference princess_quest_arion_falls.md Section 4.4.
**CAMERA:** slow push onto the pillow on entry. **The reading itself is player-paced with full control.** · **MUSIC:** none. Rain on glass. · **ANIMATION:** P3 · **LENGTH:** 0:08 push

**#13 — SHE WROTE TWO**
Cross-reference princess_quest_arion_falls.md Sections 6.3-6.5. Cassian in the courtyard.
**CAMERA:** authored two-shot, static, rain, **and Maerwyn on the stair in frame the entire time and never cut to** · **MUSIC:** none · **ANIMATION:** **P1, the highest facial priority in the game** · **LENGTH:** 2:30

**#14 — THE WIZARD'S QUESTION**
Cross-reference princess_quest_belos.md Section 9.2.
**CAMERA:** authored. **Ellis is simply in frame in a shot that was already running — no entrance, no reveal.** Cross-reference the_wizard.md Section 1.4. · **MUSIC:** none · **ANIMATION:** P1 · **LENGTH:** 1:50

---

### 3.4 CHAPTER 9-10 — THE ENDGAME

**#15-#19** — cross-reference bosses/crimson_armada.md and the main-story documents. Five authored sequences in the final act, specified in those documents.

**Design law:** cross-reference highmoore_endgame_state.md Section 1.1 — **none of them reference Highmoore.**

---

## 4. THE FIVE INSERTS

**Cross-reference:** camera_system.md Section 3.2. Two seconds, control taken, one subject, control returned.

| # | Subject | Where |
|---|---|---|
| **I-1** | **Jake's hands, shaking, holding a sword that is not** | princess_quest_belos.md §5.3 |
| **I-2** | The nine state treasures laid out together | colonial_war/castle_catalog.md §13.2 |
| **I-3** | The black candle, lit, on the cabin table | colonial_war/assassin_network.md §2.3 |
| **I-4** | The Notice of Engagement, its settlement figure | colonial_war/assassin_network.md §3.3 |
| **I-5** | Elowen's two signatures, side by side in inventory | princess_quest_the_truth.md §6.3 |

**Design note:** four of five are documents. Cross-reference style_bible.md Pillar 2.

---

## 5. THE SIX SLOW PUSHES

**Cross-reference:** camera_system.md Section 3.3. Maximum 0.15 m/s.

1. The ship's name (#2)
2. The first Imperial letter (#3)
3. Ethan's Strain in Don Alejandro's hands (#6)
4. The real letter on the pillow (#12)
5. **The boundary stone at the lake** — cross-reference princess_quest_the_lake.md Section 4.4: **only if the player has already read the real letter on a subsequent playthrough**, and it is the only new-game-plus camera event in Dark Arisen
6. **An empty rabbit hutch** — cross-reference highmoore_reconstruction.md Section 5.5, **only if the player fills them**

---

## 6. THE TWENTY-TWO THAT ARE NOT CUTSCENES

**This section is the more important half of the document.**

**Design law:** every moment below is a moment a competent developer will assume should be a cinematic. **All of them are player-controlled. Building any of them as a cutscene destroys it.**

| # | Moment | Why it must stay playable |
|---|---|---|
| 1 | **The thirty seconds after the arrow** | Cross-ref princess_quest_the_lake.md §8.3 — full control, nothing to do. **The helplessness is the mechanic.** |
| 2 | **The standing at the lake** | §10 — no prompt, no timer. **The player decides when to leave her.** |
| 3 | **Killing the archer** | §9.4 — gameplay. Two seconds. A boy who followed the rules. |
| 4 | **Reading the real letter** | arion_falls §4.4 — player-paced, no skip, rain on glass. |
| 5 | **Killing Cassian** | §7.1 — **one input.** Not a cinematic. |
| 6 | **The nine minutes at Belos** | Entirely playable. Cross-ref crystal_katana.md §8.2. |
| 7 | **The walk out of Belos** | §10.2 — player-controlled through what he made. |
| 8 | **Going to the stair** | §9.5 — the input exists, the rubble clears, **and then it cuts.** |
| 9 | **The return through the mountain** | the_return §4 — forty minutes at a walk. |
| 10 | **The churchyard** | §3.4 — enterable, no prompt, **nothing happens.** |
| 11 | **Every boss death** | colonial_war_bosses §1.5 — the Hold, not a cinematic. |
| 12 | **Ibarra in the garden** | imperial_colonies §5.9 — **no arena, no phases, no camera.** |
| 13 | **The Guardian settling** | crystal_guardian §7.1 — static hold. |
| 14 | **The Light Elf ending** | light_elf_boss §7 — an empty hilltop and wind returning. |
| 15 | **Vidal's confession** | assassin_network §5.4 — dialogue, player-paced, real options. |
| 16 | **Kettle's office** | assassin_network_questline §5.4 — a man behind a desk talking. |
| 17 | **Aldric's ninety seconds** | belos §7.2 — playable, standing over him. |
| 18 | **Rowland's offer** | arion_falls §3.4 — and no option to accept. |
| 19 | **Every Liberation Event's aftermath** | indigenous_liberation_alliance §6.4 — **free camera, and it does not find Jake.** |
| 20 | **The crew scene in the great cabin** | the_return §5.3 — dialogue, three options, Salvio's second question. |
| 21 | **Maerwyn's line at breakfast** | the_ball §8.3 — **no reaction shot, scene moves on immediately.** |
| 22 | **The first sight of a stopped shaft** | light_elf_boss §2.4 — the player is standing there. No cue. |

---

## 7. MUSIC SUMMARY

**Cross-reference:** highmoore_music.md.

### 7.1 Where Score Exists

| Context | Cue |
|---|---|
| The nineteen | 11 of 19 have **no music at all** |
| Boss encounters | one sustained element, entering at the fight, absent at the death |
| The Crystal Caves | **two cues in two hours** — the Guardian, the emergence |
| The lake dance | the ball's dance tune, chamber, slow, **plus one low held note that was not there before** |
| Battles | opening and the silence afterward. **Nothing between.** Cross-ref battle_system.md §8.1 |
| Dungeons | **none.** Cross-ref dungeon_design_philosophy.md §8.1 |

### 7.2 The Rule the Score Obeys

**Cross-reference:** highmoore_music.md Section 4, princess_quest_the_lake.md Section 8.2.

**Music never reacts to events.** It starts, it runs its length, it stops.

**The canonical demonstration:** the arrow lands and the cue does not notice.

---

## 8. ANIMATION PRIORITY

**Cross-reference:** animation_system.md, facial_animation.md.

### 8.1 P1 — Full Performance Capture, Face and Body

**Nine sequences.** #1, #5, #6, #8, #11, #13, #14, plus two endgame.

**Design note:** cross-reference facial_animation.md Section 4 — **#13 is the highest-value facial performance in Dark Arisen**: a man who has heard eleven of his people die comes out unarmed, delivers a fluent prepared redirection, sees a piece of paper, stops mid-sentence, says *ah*, and then asks whether his daughter was happy.

### 8.2 P2 — Body Capture, Keyframed Face

Six sequences.

### 8.3 P3 — Keyframed or Procedural

Four sequences and all five inserts.

---

## 9. HANDOFF TO CLAUDE CODE

**What this document provides:** location, trigger, camera mode, shot type, music state, animation tier, length.

**What Claude Code implements:** the sequences themselves, the state machine, the capture pipeline, the transitions.

**The three non-negotiables:**

1. **Section 6 is a prohibition list.** No moment in it acquires a cutscene, ever, in any revision.
2. **`SlowMotion()` outside Rache does not compile.** Cross-reference camera_system.md Section 8.
3. **Music does not react.** Cross-reference Section 7.2 — no dynamic stingers, no event-driven layers, no adaptive intensity system anywhere in Dark Arisen.

---

## 10. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1:** nine P1 performances across nine registers.
- ✅ **Pillar 2:** four of five inserts are documents; four of six slow pushes are objects.
- ✅ **Pillar 3:** eleven of nineteen cutscenes are silent, and every significant death in the game is in Section 6 rather than here.
- ✅ **Pillar 4:** no cutscene in Dark Arisen contains a joke. **The comedy is entirely in free-camera dialogue**, unframed and unemphasised.
- ✅ **Pillar 5:** nineteen authored moments in a hundred and fifty hours, and the worst thing Jake ever does is nine minutes of uninterrupted gameplay with one two-second insert of his own hands.

---

## 11. CONCLUSION

The Nineteen, and the Ones That Are Not. Nineteen authored cinematics in a hundred and fifty hours, eleven of them with no music at all, none of them containing a joke, and not one of them containing a death that matters.

Five inserts of two seconds each, four of which are documents. Six slow pushes at fifteen centimetres a second, four of which are objects, one of which only exists on a second playthrough and one of which only exists if a player refills six rabbit hutches nobody asked him to.

And a list of twenty-two moments — the arrow, the standing, the reading, the courtyard, the nine minutes, the walk out, the stair, the forty-minute ride home, the churchyard where nothing happens — that any competent studio would make cinematic, and that must never be.

**Phase 10 Document 2 of 6.**

**Music does not react. The arrow lands and the cue does not notice.** 🎬
