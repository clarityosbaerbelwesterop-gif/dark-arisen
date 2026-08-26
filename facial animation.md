# FACIAL ANIMATION — THE REGISTER IN THE FACE

**Status:** Design Draft v1.0
**Phase:** Phase 10 — Presentation & Technical (Document 4 of 6)
**Related:** animation_system.md, camera_system.md, cutscene_catalog.md, technical_targets.md, style_bible.md, npcs/dialogue_system.md, npcs/regional_populations.md, and the character documents
**Purpose:** The facial system — the three fidelity tiers, register expressed in the face, the restraint doctrine, the nine P1 performances specified line by line, ambient faces at population scale, and the explicit prohibitions.
**Inspirations:** Red Dead Redemption 2 (faces that do very little and mean a great deal), The Last of Us Part II, L.A. Noire's lesson learned in reverse, Ingmar Bergman (a face held too long), Robert Bresson (faces that refuse to perform), and the rejection of expressive-avatar animation

---

## 1. THE DOCTRINE

### 1.1 Faces Do Less Here

**Cross-reference:** style_bible.md Pillar 3, camera_system.md Section 4.2.

**Design law: the face never does more than the line.**

No widened eyes on a revelation. No trembling lip. No single tear. Cross-reference Section 4.4 — **the game's heaviest facial moment is a man saying *ah*.**

### 1.2 The Register Extends to the Face

**Cross-reference:** style_bible.md Pillar 1, npcs/regional_populations.md.

Five cultural registers in this game and **five facial vocabularies.**

- **Imperial High** — controlled, minimal, and cross-reference colonial_war/colonial_war_bosses.md Section 4: **Herrera's face barely moves for three phases and then stops entirely.**
- **Albion clerical** — pleasant, professional, and cross-reference colonial_war/assassin_network.md Section 3.3: **a Regulator delivering a death notice has the face of someone handing over a delivery.**
- **Rexan / Alliance** — open, and cross-reference colonial_war/indigenous_liberation_alliance.md: **the most expressive population in the game, and it is a cultural fact rather than a rig capability.**
- **Fjordlund** — weathered, slow, and cross-reference npcs/healer_mormor_astrid.md: **Astrid listens with her whole face and says nothing.**
- **Highmoore yeoman** — cross-reference highmoore_populations.md Section 1.3: **unimpressed. It is a specific set and it is used constantly.**

### 1.3 The Prohibitions

- No emotion-preset library (`Sad_01`, `Angry_02`)
- No lip-sync-only NPCs in speaking range
- **No facial animation on a dead body, ever.** Cross-reference camera_system.md Section 4.1.
- No "concerned" idle
- No blink-rate-as-tension
- **No crying, anywhere in Dark Arisen, by anyone.** Cross-reference Section 6.3.

---

## 2. THE THREE TIERS

### 2.1 P1 — Full Performance Capture

**Nine sequences.** Cross-reference cutscene_catalog.md Section 8.1.

4D facial capture, per-actor rig, full solve. **Roughly 22 minutes of final content.**

### 2.2 P2 — Body Capture, Curated Face

Named NPCs in extended dialogue. **~60 characters.** Audio-driven solve with hand-authored key moments — cross-reference Section 5: **each P2 character has three to six hand-keyed beats and nothing else.**

### 2.3 P3 — Procedural

**Cross-reference:** npcs/regional_populations.md — 80-150 visible NPCs per settlement.

Audio-driven lip sync, cultural micro-expression set (Section 1.2), gaze, and cross-reference technical_targets.md Section 5: **a hard budget of 0.4ms per frame for all P3 faces combined.**

---

## 3. GAZE

### 3.1 It Carries More Than Expression

**Design law:** in a game whose faces do very little, **where someone looks is the primary channel.**

### 3.2 The Named Gaze Beats

| Beat | Source |
|---|---|
| **Mira looks at the horse, then the sword, then Jake, in that order** | the_return §5.2 |
| **Elowen watching Jake before she speaks** | the_ball §4.4 — she has been watching a while |
| **Maerwyn on the gallery stair, not moving, watching all of it** | arion_falls §5.2 |
| **Cassian looking at the harbour** *(Herrera variant)* | colonial_war_bosses §4.4 |
| **Aldric looking at Jake's horse before his face** | belos §7.2 — *"you've his horse and you're not his"* |
| **Vidal, who does not look away during the confession** | assassin_network §5.4 |
| **Nobody looking at the man in the corner** | wizard_questline §2.2 — **an entire pub's gaze, systematically avoiding one table** |

### 3.3 The Refusal to Look

**Cross-reference:** Section 3.2, camera_system.md Section 1.2.

**Elowen changes the subject once all evening** — cross-reference the_princess.md Section 8.3 — and the gaze does the work: she looks away on the rabbits and nowhere else in ninety minutes.

---

## 4. THE NINE P1 PERFORMANCES

### 4.1 #13 — CASSIAN IN THE COURTYARD

**Cross-reference:** cutscene_catalog.md Section 8.1, princess_quest_arion_falls.md Section 6.

**The highest-value facial performance in Dark Arisen.**

**The beats, in order:**
1. **Fluent.** He has prepared this. The redirection toward Belos is delivered by a man who is grieving and lying and is good at both, and **the face is doing grief honestly.**
2. **He sees the paper.** No widened eyes. Cross-reference Section 1.1. **He stops talking. That is the whole beat.**
3. ***"...ah."*** — cross-reference princess_quest_arion_falls.md Section 6.4.
4. ***"She wrote two."*** — and a small movement that is almost pride and is not survivable to watch.
5. **The unfinished sentence.** *"Of course she did, she was—"* and he does not finish it, and **the face does not resolve either.**
6. **The question.** *"Was she happy?"* — cross-reference Section 6.2: **and this is delivered flat, because he cannot hold it any other way.**

### 4.2 #6 — DON ALEJANDRO AND THE STRAIN

**Cross-reference:** npcs/cigar_collector_don_alejandro.md, fauna/cannabis_system.md.

A man who buried a son, holding a thing another dead son grew. **Cross-reference camera_system.md Section 3.3 — the shot pushes onto his hands, not his face**, and the face is in the frame's edge for most of it.

### 4.3 #11 — THE ARROW

**Cross-reference:** princess_quest_the_lake.md Section 8.

**1.5 seconds.** Cross-reference cutscene_catalog.md Section 3.3.

**Design law:** cross-reference Section 1.1 — **Jake's face does nothing.** He has not understood yet. The performance is entirely in the body, cross-reference animation_system.md Section 7 (the bad catch), and the face is blank because a face is blank for about two seconds before it isn't.

**Elowen's face:** cross-reference princess_quest_the_lake.md Section 8.3 — she says nothing and cannot. **The eyes are open and looking at him. That is all.**

### 4.4 #14 — THE WIZARD'S QUESTION

**Cross-reference:** princess_quest_belos.md Section 9, the_wizard.md Section 7.3.

**Ellis does not moralise and the face must not either.** He asks a question and waits, and cross-reference the_wizard.md Section 7.3: **he is asking himself too, and no line says so, and the face is the only place it can live.**

**The most difficult direction note in the document:** *"he is not accusing him. He is asking, and he already knows what his own answer is, and it is worse than Jake's."*

### 4.5 The Remaining Five

**#1 The Brother** · **#5 The Dream Fight** · **#8 The Voice From Behind** · **#9 The Ejection** · plus two endgame. Cross-reference cutscene_catalog.md Section 3.

**Cross-reference #8:** Elowen's register switch. Cross-reference the_princess.md Section 5.1 — **ninety seconds of flawless High performance and then it drops**, and **the face changes before the voice does, by about a third of a second.**

---

## 5. THE P2 KEY BEATS

**Cross-reference:** Section 2.2. Three to six hand-keyed moments per character. A sample of what they are spent on:

| Character | Beat |
|---|---|
| **Big Tom** | looking for somewhere to hold the Katana |
| **Mira** | *"It's a good sword."* — and the pause before the second sentence |
| **Ines** | the third test that disagrees with the first two |
| **Father Salvio** | saying nothing at Cruz's death |
| **Aldric Belos** | *"I've had four years and I still don't know."* |
| **Isolde Thorn** | *"Take it off my bench."* |
| **Cruz** | **talking for the first time in thirty years** |
| **Thorne** | *"What have I been reporting?"* |
| **Blackwood** | still expecting to persuade, at the moment of death |
| **Tam** | explaining his transfer strategy |
| **Maerwyn** | the half-second pause at breakfast — **cross-reference camera_system.md Section 4.2: and there is no shot of it** |

**Design note:** Maerwyn's is authored and **never framed.** A player will only ever catch it in peripheral vision, and cross-reference princess_quest_the_truth.md Section 9.2: **on a second playthrough they will be looking for it.**

---

## 6. WHAT FACES DO NOT DO

### 6.1 They Do Not Announce

**Cross-reference:** Section 1.1, camera_system.md Section 4.2.

No character's face tells the player that something important is happening.

### 6.2 They Do Not Hold Emotion Open

**Cross-reference:** Section 4.1 beat 6.

Cassian asks whether his daughter was happy **flat**, because that is how people ask that. Cross-reference style_bible.md Pillar 3.

### 6.3 Nobody Cries

**Design law, absolute:** there is no crying in Dark Arisen.

Not Jake, not Cassian, not Maerwyn, not Ines, not any villager at any graveside. Cross-reference style_bible.md Pillar 3 and the game's entire treatment of death — **grief in this bible is people continuing to do things.**

### 6.4 Dead Faces Are Static

**Cross-reference:** camera_system.md Section 4.1, colonial_war/dungeon_design_philosophy.md Section 8.3.

**Positioned, never posed** applies to faces. No agonised expressions, no peaceful ones. **The rig is off.**

---

## 7. AMBIENT FACES

**Cross-reference:** npcs/regional_populations.md, technical_targets.md Section 5.

### 7.1 What P3 Must Convey

Cross-reference npcs/regional_populations.md Section 10.3 — **the gossip web has a facial layer.** A settlement that has heard about Jake looks at him, and one that has not does not.

Three states, procedural: **unaware · aware · avoiding.**

### 7.2 The Highmoore Set

**Cross-reference:** highmoore_populations.md Section 1.3.

**Unimpressed** is a distinct P3 micro-expression set, used across an entire region, and cross-reference highmoore_populations.md Section 5.5: **it survives the reconstruction and it is what refusing to call him a lord looks like.**

### 7.3 The Children

**Cross-reference:** npcs/children_and_families.md.

Warmth standards apply. Cross-reference princess_quest_arion_falls.md Section 5.2 — **a woman with her hand over a girl's eyes**, and the girl's face is not shown, and the boy's is.

---

## 8. IMPLEMENTATION

```cpp
struct FaceState {
    FaceTier    tier;          // P1, P2, P3
    RegisterSet cultural;      // Imperial, Albion, Alliance, Fjordlund, Highmoore
    GazeTarget  gaze;          // §3 — primary channel
    bool        rigActive;     // FALSE on all corpses, always
};
```

**Budgets:** cross-reference technical_targets.md Section 5. **P3 total: 0.4ms/frame.** P2 in dialogue: 0.6ms. P1: offline solve, no runtime cost.

**Prohibited assets:** any emotion-preset library · any crying blendshape · **any facial animation authored for a corpse.**

---

## 9. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1:** five cultural facial vocabularies, and *unimpressed* is a shipped micro-expression set covering an entire region.
- ✅ **Pillar 2:** the primary channel is gaze — Mira looking at three things in order and saying none of them; an entire pub systematically not looking at one table.
- ✅ **Pillar 3:** nobody cries. The rig is off on every corpse. The heaviest facial moment in the game is a man saying *ah*.
- ✅ **Pillar 4:** Tam explaining his transfer strategy. Isolde saying take it off my bench.
- ✅ **Pillar 5:** a father asks flatly whether his daughter was happy, and the game does not let his face do the work for him, and the answer is one of the two most important choices in Phase 8.

---

## 10. CONCLUSION

The Register in the Face. Nine full performances in a hundred and fifty hours, twenty-two minutes of final content, and the highest-value one is a man in a courtyard in the rain who stops talking when he sees a piece of paper, says *ah*, says *she wrote two* with something that is almost pride, starts a sentence he cannot finish, and then asks flatly whether his daughter was happy at the lake because he was not there.

Five cultural vocabularies, one of which is *unimpressed* and covers an entire region and survives everything Jake does to it. Gaze as the primary channel — a first mate looking at a horse, then a sword, then a man, in that order; a pub whose every occupant has arranged not to look at one corner table.

No emotion presets. No widened eyes. No trembling lip. No blink-rate tension. **Nobody in Dark Arisen cries, ever, at any graveside**, because grief in this bible is people continuing to do things.

And the rig is off on every corpse in the game.

**Phase 10 Document 4 of 6.**

**The face never does more than the line.** 😐
