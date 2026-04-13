# BOSS SPEC — Don Mateo Salazar "El Carnicero" (The Butcher)

**Status:** Design Draft v1.0  
**Region:** Rexa — The Slaughterhouse (Dungeon #9)  
**Boss Category:** Story Main Boss, Chapter 4 Climax  
**Difficulty:** Hard (12-18 attempts average)  
**Related:** region_02_rexa_overview.md, combat_system.md  
**Inspirations:** Sekiro's Lady Butterfly (elegance + cruelty), Bloodborne's Father Gascoigne (emotional stakes), Count Dracula from Castlevania (aristocratic menace)

---

## 1. Concept — Elegance Over Rot

Don Mateo Salazar is the most disturbing boss in Dark Arisen. He is not a monster. He is a man who chose, over a lifetime, to become what he is. Where the Ashen Wyrm evokes awe and the Caldera Titan evokes sorrow, Salazar evokes **disgust** — and the player should feel that disgust translate into the fight's rhythm.

Salazar fights like a Spanish fencing master. Precise. Ornamental. Beautiful. He will step delicately between Jake's attacks and compliment his technique mid-parry. He will stop the fight to retrieve a silk handkerchief and wipe a spot of blood from his white linen suit. He is having a good time. The contrast between his elegance and what he has done is the entire point.

**Design principle:** The fight must feel like a gentleman's duel on the surface and a horror story underneath. Every beautiful motion he makes should remind the player of who and what he is.

---

## 2. Visual Description

- **Height:** 1.85m, thin, upright posture
- **Age:** Mid-50s but moves like a man of 30
- **Clothing:** Pristine white linen suit with old bloodstains he refuses to wash out. A red silk cravat. White leather gloves.
- **Face:** Pale, gaunt, high cheekbones, trimmed black beard streaked with silver. Eyes like cold dark water.
- **Accessories:** Gold rings on every finger (taken from victims), a pocket watch he consults between exchanges, a silver-handled curved Toledo blade
- **Voice:** Soft, polite, Spanish-accented English. Never raises his voice. The calmer he sounds, the more dangerous he is.
- **Movement:** Fencing stance. Back straight. Sword arm relaxed. He never runs. He never needs to.

---

## 3. Arena — The Private Theater

- **Shape:** A grand oval hall, 30m x 20m, inside Salazar's private slaughterhouse
- **Floor:** Polished dark wood with ornate rugs — except the rugs hide old bloodstains Salazar chose not to remove
- **Walls:** Lined with velvet curtains. Trophies on display — not animal heads, but preserved hands and severed braids of hair. The player should scan the room in horror before the fight begins.
- **Lighting:** Candlelight from massive iron chandeliers. Flickering, warm, intimate — a contrast to the horror on the walls.
- **Center:** A small round table with two wine glasses and a decanter of red wine. Salazar drinks between phases.
- **Exit:** Grand double doors behind Jake. Locked during the fight.
- **Rest Point:** Outside the slaughterhouse entrance, 2 minutes walk back.

---

## 4. Stats

| Stat | Value |
|------|-------|
| Total HP | 5,500 |
| Phase 1 HP | 5,500 → 3,850 (70%) |
| Phase 2 HP | 3,850 → 1,650 (30%) |
| Phase 3 HP | 1,650 → 0 |
| Posture | 320 |
| Posture Regen Delay | 3 seconds (short — he recovers fast) |
| Defense | 20 (low — he relies on evasion) |
| Stagger Threshold | 90 posture damage in one combo |
| Critical Window | 2 seconds after posture break |
| Critical Multiplier | 3.5x |
| Dodge Speed | Extremely high — he can sidestep most attacks |

---

## 5. Moveset — Phase 1 (100% → 70% HP): The Gentleman's Duel

Phase 1 is a formal duel. Salazar fences with textbook Toledo style. Elegant. Clean. Almost beautiful. He will sidestep, parry, riposte — he is TEACHING Jake how to fight him, because he enjoys watching students learn before he kills them.

### 5.1 Attack: Lunge Thrust
- **Trigger:** Jake within 5m
- **Windup:** 25 frames — slight drop of his blade tip, back foot pivots
- **Active:** Frames 26-32 (fast forward lunge, 4m distance)
- **Recovery:** 30 frames
- **Damage:** 55
- **Posture Damage:** 30
- **Counter:** Parry at frame 28 for perfect timing, or sidestep with dodge
- **Audio Tell:** A soft *"hup"* — a fencer's breath
- **Design Note:** This is the most common attack in Phase 1. Learning to parry it is essential.

### 5.2 Attack: Riposte Combo
- **Trigger:** When Jake attacks and is blocked by Salazar
- **Windup:** 15 frames (very fast — it's a counter-attack)
- **Active:** Frames 16-30 (three rapid strikes)
- **Recovery:** 25 frames
- **Damage:** 30 per strike (90 if all hit)
- **Posture Damage:** 20 per strike
- **Counter:** Parry each strike individually (brutal timing), or dodge back out of range
- **Audio Tell:** Salazar's blade ringing against Jake's block
- **Design Intent:** Punishes players who attack carelessly without reading his stance

### 5.3 Attack: Cruel Flourish
- **Trigger:** Every 20 seconds
- **Windup:** 40 frames — he performs a decorative spin
- **Active:** Frames 41-60 — the spin becomes a wide horizontal slash
- **Recovery:** 35 frames
- **Damage:** 65
- **Posture Damage:** 35
- **Counter:** Dodge back or duck under it (crouch)
- **Audio Tell:** The whistle of his blade through air + a soft laugh

### 5.4 Attack: Handkerchief Bait
- **Trigger:** At 85% HP, once
- **Windup:** 50 frames — Salazar stops, retrieves a silk handkerchief, dabs a spot of blood from his suit, smiles at Jake
- **Active:** The moment Jake attacks during this animation, Salazar parries perfectly and ripostes for 100 damage
- **Counter:** DO NOT ATTACK. Wait. He will resume the fight on his own.
- **Design Intent:** Teaches players that Salazar baits recklessness. The handkerchief animation looks like a free punish window, but it's a trap.
- **Audio Tell:** The soft rustle of silk

### 5.5 Dialogue in Phase 1
Salazar speaks throughout the fight, always between attacks, never during.

- **Fight start:** *"Ah. You must be the one who has been killing my lieutenants. I confess, I am flattered. Do you know how few men have the dedication to hunt five of my people?"*
- **90% HP:** *"Your form is... untrained. But there is passion there. Passion is the seed of technique. Allow me to educate you."*
- **80% HP:** *"Your brother. I remember him. Draven brought him to my table for dinner, once. He was polite. He asked me about the quality of sugar. We had a lovely conversation."*
- **70% (Phase 2 trigger):** *"I confess, I was not taking you seriously. That was rude of me. Let me correct my error."*

---

## 6. Moveset — Phase 2 (70% → 30% HP): The True Butcher

At 70% HP, Salazar stops performing. He walks calmly to his center table, takes a sip of wine, sets the glass down, and turns. His posture shifts. The elegant fencing master is still there, but something crueler is beneath it now. His attacks become less predictable, more vicious.

### 6.1 Attack: Low Sweep → High Thrust
- **Trigger:** Jake within 5m
- **Windup:** 30 frames — he drops his blade low
- **Active:** Two-hit combo: frames 31-40 (low sweep at legs), frames 45-50 (high thrust at torso)
- **Recovery:** 30 frames
- **Damage:** 40 (low) + 55 (high) = 95 if both hit
- **Posture Damage:** 25 + 30
- **Counter:** Jump the low sweep, then parry the high thrust — brutal input sequence
- **Design Intent:** Tests the player's ability to read two different attacks in sequence

### 6.2 Attack: Throwing Dagger
- **Trigger:** Every 15 seconds, or when Jake retreats to range
- **Windup:** 20 frames — he produces a small dagger from his sleeve
- **Active:** Frame 21 (throw)
- **Recovery:** 15 frames
- **Damage:** 50 on hit, applies bleed DoT (10 HP/sec for 6 seconds)
- **Counter:** Dodge or block with a shield (if equipped)
- **Audio Tell:** The soft *ting* of metal leaving a sleeve

### 6.3 Attack: The Waltz (Multi-Strike)
- **Trigger:** Every 30 seconds
- **Windup:** 35 frames — Salazar takes a dancer's step, blade extended
- **Active:** Frames 36-80 — a 7-strike chain combo moving AROUND Jake (he literally waltzes around the player)
- **Recovery:** 25 frames
- **Damage:** 20 per strike (140 if all hit — almost a kill)
- **Posture Damage:** 15 per strike
- **Counter:** Back-dodge out of range, or parry the entire combo (requires near-perfect timing and is the ultimate skill check)
- **Audio Tell:** He starts humming a waltz tune

### 6.4 Attack: Wine Glass Throw
- **Trigger:** Once in Phase 2, at ~50% HP
- **Windup:** 40 frames — he walks back to his table, picks up his wine glass, looks at Jake over the rim
- **Active:** Frame 41 — he hurls the glass at Jake's face. If it hits, Jake is blinded for 3 seconds.
- **Damage:** 20 + blindness
- **Counter:** Dodge or parry
- **Design Intent:** A moment of pure insult. He is not treating Jake as a serious threat.

### 6.5 Dialogue in Phase 2
- **Phase 2 start:** *"Your brother chose Draven over his family. Do you blame him? Your father was a merchant of sugar. Draven offers a world. Your brother chose ambition."*
- **60% HP:** *"I wonder, when I kill you, what your brother will think. Will he mourn? Or will he toast me over wine, the way we toast the fallen in my circle?"*
- **50% HP (after Wine Glass Throw):** *"Forgive me. I should not waste good Rioja on you."*
- **40% HP:** *"You are better than I expected. You are NOT better than me."*
- **30% (Phase 3 trigger):** *"Enough theater. I tire of this. Let me end you properly."*

---

## 7. Moveset — Phase 3 (30% → 0% HP): Without Honor

At 30% HP, Salazar's elegance shatters. He has not been hit this hard in twenty years. He is angry — genuinely angry for the first time in the fight. The white linen suit is now stained with his own blood. His posture breaks. He begins to fight dirty.

### 7.1 Attack: Dirty Strike
- **Trigger:** Mid-combat, unpredictable
- **Examples:** Thrown sand, kicking Jake in the knee, grabbing Jake's shirt and headbutting
- **Damage:** 40-60 varied
- **Counter:** These attacks have NO telegraphs. The only defense is distance — stay 3m+ away
- **Design Intent:** He is abandoning the duel entirely. This is a man who has lost his composure.

### 7.2 Attack: The Cheater's Blade
- **Trigger:** Every 25 seconds
- **Windup:** 10 frames (fastest attack in the fight — no fair warning)
- **Active:** Frame 11 — he produces a SECOND hidden blade and strikes with both weapons
- **Damage:** 45 + 45 = 90
- **Posture Damage:** 30 + 30
- **Counter:** Almost impossible to react to. Must predict based on Salazar's positioning.
- **Narrative Note:** He has always carried two blades. He considered using only one a courtesy.

### 7.3 Attack: Grapple and Stab
- **Trigger:** Jake within 2m during Phase 3
- **Windup:** 20 frames — he lunges in to grab
- **Active:** If grabbed, a scripted stab animation plays
- **Damage:** 150 (very likely kill if Jake is already wounded)
- **Counter:** Stay OUT of grapple range. If grabbed, mash Square rapidly to break free.

### 7.4 Dialogue in Phase 3
- **Phase 3 start:** *"You understand nothing. Your father fed the empire that built everything I am. You have no right to judge me."*
- **20% HP:** *"I have killed children. I have killed mothers. I have killed men who begged. Do you think YOU are the first to want me dead?"*
- **10% HP:** *"Draven will replace me within a week. You accomplish nothing tonight."*
- **Final Hit (pre-death):** *"Tell your brother... tell him Don Mateo says hello."*

### 7.5 The Death Scene
When Jake deals the killing blow, Salazar does not fall dramatically. He looks at his wound with mild surprise, as if it is an inconvenience. He sits down calmly on the edge of his wine table. He produces his pocket watch, glances at it, and says softly: *"Hm. Earlier than I expected."* Then he pitches forward onto the floor and does not move again.

Jake, if the player explores the slaughterhouse after the fight, finds evidence of Salazar's crimes in the adjoining rooms — jars of preserved trophies, ledgers of sold people, children's drawings on the walls (made by kids he held before selling). The game does NOT force the player to see these things. They are hidden for players who want to understand the full horror of who they just killed.

---

## 8. Combat Flow — How the Fight Should Feel

**Phase 1 (The Duel):** Formal. Educational. Almost pleasant, in a grotesque way. The player feels like they're in a period drama. They start to understand Salazar's timing.

**Phase 2 (The Butcher):** A shift. The gentleman is still there, but the monster is showing through. Attacks become unpredictable. The Waltz is the hardest moment — most first-timers die to it.

**Phase 3 (Without Honor):** Ugly. Fast. Desperate. The player realizes they are not fighting a man — they are fighting a cornered animal wearing a man's clothes. The final line about Ethan sets up Chapter 9.

**The Final Hit:** Not triumphant. Sad. Jake has killed someone who deserved it a thousand times over, and it doesn't feel good. This is important — Dark Arisen is not a power fantasy. It is a story about how revenge shapes you.

---

## 9. Rewards

| Reward | Description |
|--------|-------------|
| **The Butcher's Blade** | Legendary one-handed sword, highest bleed damage in Region 2 |
| **Salazar's Pocket Watch** | Quest item that unlocks hidden lore dialogue with certain NPCs across the game |
| **Recipe: Aristocrat's Tincture** | Premium tincture that grants temporary +20% to all stats for 60 seconds |
| **5 Gold Bars** | Found in Salazar's personal safe (Phase 2 of dungeon loot) |
| **Story Flag:** `salazar_defeated` — enables Region 2 completion and ship purchase |
| **Lore Unlock:** Salazar's ledger — reveals names and fates of people he killed, adds emotional weight to future Crimson Armada encounters |
| **Trophy:** "The White Suit" — a preserved piece of Salazar's bloodied coat for the ship |

---

## 10. Why This Fight Matters

Don Mateo Salazar is the first time Dark Arisen makes Jake kill someone who truly deserves it — and it doesn't feel good. Where the mythical bosses (Titan, Wyrm) evoke awe and sorrow, Salazar's fight is about moral weight. The player starts to understand that Draven's Crimson Armada is not built on noble anti-imperial principles anymore. It's built on compromises with monsters. And Ethan — Jake's brother — is somewhere inside that machine, shaped by men like Salazar.

After this fight, Jake sails out of Rexa with his first ship, his first crew, and the first real understanding that the world he grew up in was always more complicated than his merchant father let him believe. Salazar's final line — *"Tell your brother Don Mateo says hello"* — plants the seed that Ethan knew Salazar. Knew what he was. And did not stop him.

---

## 11. Implementation Notes

- Parry timing is critical — Salazar's duel style demands tight combat feel
- Dialogue system must support interruption (if player attacks during a monologue, Salazar ends the line mid-sentence with a witty response)
- The slaughterhouse environment must remain explorable after the fight — hidden horror elements are player-choice discovery
- Story flag `salazar_defeated` must gate Region 2 completion and ship purchase at the Shipwright
