# BOSS SPEC — The Jungle Warden

**Status:** Design Draft v1.0  
**Region:** Moran — The Overgrown Temple Inner Sanctum  
**Boss Category:** Story Main Boss (Chapter 2 Climax — Demo Climax)  
**Difficulty:** Hard but Fair (15-20 attempts average for first-time Souls players)  
**Related:** region_01_moran.md, combat_system.md, style_bible.md  
**Inspirations:** Asylum Demon (Dark Souls — first true test), Father Gascoigne (Bloodborne — emotional weight), Forest Spirit from Princess Mononoke (nature corrupted)

---

## 1. Concept — The First Test

The Jungle Warden is the most important boss in Dark Arisen, and not because he is the strongest. He is the FIRST. Every player who experiences Dark Arisen meets him in the demo — long before Ethan, before Draven, before the Ashen Wyrm. The Warden's job is to teach the player what kind of game they are playing. If he succeeds, the player commits to the next 50 hours. If he fails, the player refunds the demo.

He is also the boss that defines Moran's identity. After defeating him, Jake leaves the island for Rexa — and Moran becomes a memory. The Warden must make Moran feel like a place the player will remember.

**Design principle:** This is not a mythic boss like the Ashen Wyrm. The Warden is older than humanity but he is not a god. He is a guardian — built for one purpose, abandoned, and now defending something that no longer exists. He is the first character in Dark Arisen who teaches the player that Dark Arisen is a sad world, not a triumphant one.

---

## 2. Visual Description

- **Height:** 4.5 meters (3x Jake's height — large but not god-scale)
- **Form:** Vaguely humanoid, but built from twisted vines, ancient stone slabs, and weathered armor fragments. His "skeleton" is ironwood roots that have grown over and through stone artifacts. He moves like a thing that has forgotten how bodies work and is reconstructing the motion from memory.
- **Mask:** A carved wooden mask covers what should be his face. The mask has no eyes — only two black hollows. The carving is in an ancient style that matches the Anak Gunung temple aesthetic from Region 3 (this is intentional — the lore connection comes later).
- **Body:** Moss and small flowers grow on his back and shoulders. His left arm is significantly larger than his right — uneven growth over centuries. His chest has a hollow cavity where a glowing green light pulses slowly, like a heartbeat.
- **Movement:** He does not walk so much as ROOT. When he stands still, vines extend slightly from his feet into the temple floor. When he moves, those vines tear free with a low cracking sound.
- **Sound:** He does not roar or scream. He breathes — long, slow, wooden breaths that rattle through his chest cavity. When he is hurt, he sighs.

---

## 3. Arena — The Inner Sanctum

- **Shape:** A circular stone chamber, 25m diameter, deep within the Overgrown Temple
- **Architecture:** Ancient pillars covered in vines. A vaulted ceiling with shafts of golden sunlight breaking through where the roof has collapsed. The light catches dust and pollen in the air, creating beams that the player can see through.
- **Floor:** Polished stone, cracked. Roots push up through the cracks. The center of the floor has an ancient carved seal — barely visible, eroded by time. (This seal is a Pre-Human Language symbol that the player won't recognize until much later in the game.)
- **The Heart Tree:** At the back of the chamber stands a massive dead tree, blackened and lifeless. Its roots disappear into the floor. The Warden is connected to this tree by a single thick vine that runs from the tree's base to his back.
- **Atmospheric Detail:** Pollen drifts through the air constantly. Birds occasionally fly through the broken ceiling, oblivious to the fight. A small stream of water trickles from one wall and pools in a corner. The arena feels ALIVE despite the dead tree at its center.
- **Rest Point:** Just outside the sanctum entrance, 30 seconds back. The player will see this rest point many times.

---

## 4. Stats

| Stat | Value |
|------|-------|
| Total HP | 3,200 |
| Phase 1 HP | 3,200 → 1,920 (60%) |
| Phase 2 HP | 1,920 → 960 (30%) |
| Phase 3 HP | 960 → 0 |
| Posture | 220 |
| Posture Regen Delay | 5 seconds |
| Defense | 30 |
| Stagger Threshold | 100 posture damage in one combo |
| Critical Window | 2.5 seconds after posture break |
| Critical Multiplier | 3x |
| Weak Point | The glowing green chest cavity — visible only when his right arm is raised |

---

## 5. Moveset — Phase 1 (100% → 60% HP): The Guardian Awakens

When Jake enters the sanctum, the Warden is motionless against the dead tree. As Jake approaches the seal in the center of the floor, the Warden's chest light pulses brighter and he steps forward. Vines tear free from the floor. He raises his uneven arms and breathes the first deep breath in centuries.

He does not attack immediately. He looks at Jake (or where Jake is — the eyeless mask makes it ambiguous) for three full seconds. Then he attacks.

### 5.1 Attack: Vine Grasp
- **Trigger:** Jake at any range
- **Windup:** 35 frames (583ms) — the Warden raises his right arm high
- **Active:** Frames 36-50 — vines erupt from the floor in a 3m radius around Jake's CURRENT position
- **Recovery:** 30 frames
- **Damage:** 35 base damage, holds Jake in place for 2 seconds (vulnerable to follow-up)
- **Posture Damage:** 25
- **Counter:** Move during the windup (track where the vines target). If grabbed, mash Square to break free in 1.5 seconds instead of 2.
- **Audio Tell:** A low groaning sound — the floor stones grinding as vines push through

### 5.2 Attack: Heavy Slam (Left Arm)
- **Trigger:** Jake within 5m of the Warden
- **Windup:** 50 frames (833ms) — his oversized left arm rises slowly
- **Active:** Frames 51-58 (impact window)
- **Recovery:** 45 frames (massive punish window)
- **Damage:** 60 direct, 30 splash
- **Posture Damage:** 40
- **Counter:** Dodge sideways during windup, OR parry on frame 53 for perfect timing. Parrying this attack staggers the Warden for 4 seconds.
- **Audio Tell:** A long inhale building to a wooden crack

### 5.3 Attack: Sweeping Step
- **Trigger:** Jake at mid-range (5-10m)
- **Windup:** 25 frames — he takes a heavy step toward Jake
- **Active:** Frame 26-35 (the step itself sends a shockwave forward 6m)
- **Recovery:** 20 frames
- **Damage:** 25, knocks Jake back 4m
- **Posture Damage:** 20
- **Counter:** Jump (Cross) on frame 25, or be outside 6m
- **Audio Tell:** The temple floor cracks under his foot

### 5.4 Attack: Pollen Burst
- **Trigger:** Every 25 seconds
- **Windup:** 40 frames — flowers on his shoulders glow brighter
- **Active:** Frame 41 — releases a cloud of golden pollen in a 8m radius around himself
- **Damage:** 15, but applies Drowsy debuff for 5 seconds (movement -30%, attack speed -20%)
- **Counter:** Run out of the radius during windup. The pollen is visible early.
- **Design Intent:** Forces the player to disengage briefly, breaks aggressive playstyles
- **Audio Tell:** A soft hum, like wind through grass

### 5.5 Passive: The Long Breath
Every 35 seconds, the Warden stops moving for 3 full seconds. He breathes in deeply. His chest light dims briefly. This is NOT vulnerability — if Jake attacks during this, the Warden ignores it and continues breathing. The player should learn that this is the Warden remembering something. It is sad. It is also a free heal window for Jake, if he needs it.

---

## 6. Moveset — Phase 2 (60% → 30% HP): The Forgetting

At 60% HP, the Warden's mask cracks slightly. A fragment of wood falls to the floor. He stops mid-attack and looks at it. Then he looks at Jake. He does not roar or rage. He simply continues fighting — but faster now, more desperate. The Long Breaths become shorter and more frequent. He is starting to forget what he is doing here.

### 6.1 Attack: Double Slam Combo
- **Trigger:** Jake within 6m
- **Windup:** 30 frames (faster)
- **Active:** Two-hit combo. Frame 31-38 (left arm slam), then frame 50-57 (right arm slam from the opposite side)
- **Recovery:** 35 frames
- **Damage:** 45 + 35 = 80 if both hit
- **Posture Damage:** 30 + 25
- **Counter:** Dodge LEFT to avoid the first slam, then dodge RIGHT to avoid the second. Or parry both for massive posture damage.
- **Audio Tell:** Two long breaths in quick succession

### 6.2 Attack: Vine Tide
- **Trigger:** Every 20 seconds
- **Windup:** 45 frames — both arms raised, the dead tree in the back glows faintly green
- **Active:** Vines erupt in a wave across the entire arena floor, tracking Jake's movement
- **Damage:** 50, knocks prone if hit
- **Posture Damage:** 35
- **Counter:** Run perpendicular to the vine wave's direction. The wave moves in a straight line — sidestep is the only option.
- **Audio Tell:** The dead tree creaks loudly

### 6.3 Attack: Mask Strike
- **Trigger:** When Jake stands in front of the Warden's face for >2 seconds
- **Windup:** 20 frames (fast — punishes face-hugging)
- **Active:** Frame 21 — he headbutts forward with his masked face
- **Recovery:** 25 frames
- **Damage:** 70
- **Posture Damage:** 35
- **Counter:** Don't stand directly in front of his face. Move around to his side.
- **Design Intent:** Prevents players from camping in his blind spot

### 6.4 Attack: Root Pillar
- **Trigger:** Every 30 seconds
- **Windup:** 35 frames — his right arm rises
- **Active:** A pillar of roots erupts from the floor at Jake's CURRENT position. 4m wide, 3m tall.
- **Damage:** 65, launches Jake into the air
- **Counter:** Move 5m+ away during windup. The targeted spot is briefly highlighted.

### 6.5 Passive Change: The Long Breath becomes shorter
The free-breath window in Phase 2 is now only 1.5 seconds, and happens every 25 seconds. The Warden is forgetting the rhythm of his own peace.

---

## 7. Moveset — Phase 3 (30% → 0% HP): The Forgetting Becomes Total

At 30% HP, the Warden's mask shatters completely. The wooden pieces fall to the floor. Beneath the mask there is nothing — no face, no eyes, no mouth. Just hollow darkness with the green chest-light reflecting on it. He stops moving for 5 full seconds. Then he resumes the fight, but he is no longer fighting Jake. He is fighting something he sees that Jake cannot see. He attacks at random points in the arena. He sometimes attacks the dead tree behind him. He has lost track of who his enemy is.

This phase is mechanically EASIER than Phase 2 — but emotionally harder. The player should feel that they are not fighting a guardian anymore. They are putting down something that has been broken for centuries.

### 7.1 Attack: Confused Slam
- **Trigger:** Random, every 5-8 seconds
- **Windup:** 35 frames
- **Active:** He slams the ground with whichever arm is closer. The slam is at his FORMER position when Jake last looked at him 2 seconds ago — not his current position.
- **Damage:** 55, splash 25
- **Counter:** Easier to dodge than Phase 2 attacks because the targeting is delayed
- **Design Intent:** Demonstrates his mental decay through gameplay

### 7.2 Attack: Tree Slam
- **Trigger:** When the Warden faces away from Jake
- **Windup:** 40 frames
- **Active:** He slams the dead tree behind him with his left arm. The tree shatters partially and falls. Splinters fly across the arena.
- **Damage:** 30 to Jake from flying splinters (splash damage)
- **Design Intent:** This is the Warden literally destroying what he was built to protect. He doesn't recognize it anymore. Players should pause the first time they see this.

### 7.3 Attack: The Final Embrace
- **Trigger:** Once, at 10% HP
- **Windup:** 60 frames — he turns slowly toward Jake and opens both arms wide
- **Active:** If Jake is within 6m, the Warden grabs him in a slow embrace. His chest cavity opens fully, exposing the green light directly to Jake's face.
- **Damage:** 150 if grabbed (likely kill)
- **Counter:** Be outside 6m. If grabbed, mash Square — narrow escape window.
- **Narrative Note:** During the grab animation, the green light from his chest pulses softly, almost gently. It is not malicious. He thinks Jake is something else — someone he was supposed to protect, maybe, long ago. If Jake escapes the grab, the Warden looks at his empty arms for one full second before resuming combat.

### 7.4 Passive: The Fading Light
In Phase 3, the green light in his chest pulses irregularly — sometimes bright, sometimes nearly going out. When it dims, he is briefly slower. Attentive players can use these moments for free hits.

---

## 8. The Death Scene

When Jake deals the killing blow, the Warden does not fall. He lowers himself slowly to one knee, then both knees, then sits back on the temple floor like a tired old man. The green light in his chest dims to almost nothing. The vines connecting him to the dead tree shrivel and fall away. Without the mask, the player can see that there is something inside him — a small wooden carving, no bigger than a hand, in the shape of a child holding a flower. It rests in his chest cavity where the light was.

He looks at Jake (or where Jake is). He breathes one last long breath. Then, in the only words he speaks in the entire fight, he whispers in High Register:

> *"I am sorry. I do not remember who I was waiting for. I hope... I hope they were saved."*

The green light goes out. The Warden becomes still. The dead tree behind him crumbles into dust over the next ten seconds. The seal in the floor of the chamber begins to glow faintly — the same color as the Warden's light. A door at the back of the sanctum, previously hidden, slides open. Beyond it is the path back to the surface, and the way to the Galleon Cove.

Jake can interact with the wooden carving in the Warden's chest. Picking it up adds it to his inventory permanently. It is **Lore Object #006: The Child With The Flower** — and it will become the most important small object in the entire game.

---

## 9. Combat Flow — How the Fight Should Feel

**Phase 1 (The Guardian Awakens):** Awe and confusion. The player thinks "this is the boss?" because the Warden is slow. Then they take 60 damage from the Heavy Slam and they understand. The Long Breaths give them hope and breathing room.

**Phase 2 (The Forgetting):** Real challenge. The combos punish careless dodging. The Vine Tide forces movement. Players will die here several times. They learn the rhythm.

**Phase 3 (Forgetting Becomes Total):** Sadness. The player has mastered the fight by now. The Warden is broken, attacking the wrong things, no longer dangerous in the same way. The player KNOWS they will win. The question becomes: how do I feel about this victory?

**The Final Hit:** The Warden's last words are one of the few moments in Dark Arisen where the screen darkens slightly and the music stops. The player should remember them. Many will.

---

## 10. Rewards

| Reward | Description |
|--------|-------------|
| **Warden's Heartwood** | Crafting material — used to craft "The Patient Blade" at Koa's Trading Post (a starter-tier legendary cutlass with high posture damage) |
| **The Child With The Flower** | Lore Object #006 — small wooden carving, no immediate use, but becomes critical late-game |
| **Warden's Mask Fragment** | Trophy item, displays on Jake's ship later |
| **Cutscene Unlock:** The temple opens, revealing the path to the Galleon Cove |
| **Story Flag:** `jungle_warden_defeated` — required for Chapter 2 completion and demo end |
| **300 Doubloons** | Modest currency reward |
| **Ashenmoor Echo:** When Jake later reaches Kuil Atas in Region 3, an old monk will see the Warden's Mask Fragment in his inventory and react with shock — this triggers a unique dialogue revealing the Warden's true nature |

---

## 11. The Hidden Truth (Lore Connection)

The Warden was built by the Anak Gunung. Specifically, by the FIRST Anak Gunung — the ones who learned from the dragon herself. They built him to guard their children when the adults were away tending the rice terraces and the volcanic shrines. He was not a war machine. He was a **babysitter**. The wooden carving in his chest was made by one of those children — given to him as a thank-you gift for keeping her safe through a thunderstorm.

When the imperial Specimen Recovery Program came to Moran 280 years ago, the dragons in the river caves were killed. The Anak Gunung who lived on Moran fled to the larger islands of Ashenmoor. They left the Warden behind because they had to move fast. They forgot him.

For 280 years he has stood in the temple, waiting for children who are not coming back, defending nothing from no one. When Jake walks in, the Warden's chest light brightens because he thinks — for one moment — that one of the children has finally returned. Then he sees Jake's sword. Then he attacks.

**This is the saddest fight in Moran. The player will not understand this until they reach Kuil Atas in Region 3 and an old monk explains what they have done.**

---

## 12. Style Bible Compliance Check

- ✅ **Pillar 1 (Staggered Register):** The Warden speaks ONCE in the entire fight, in High Register. Everything else is silence and breath. This is the correct register for an ancient guardian.
- ✅ **Pillar 2 (Lore Through Objects):** The Child With The Flower is a physical object Jake picks up. The truth is hidden until he meets the Kuil Atas monk hundreds of hours later. Player connects dots themselves.
- ✅ **Pillar 3 (Layered Death):** Style D (tragic, meaningful). Last words. Long pause after final hit. No triumph music. The player feels the weight.
- ✅ **Pillar 4 (Layered Humor):** None. The Warden is not a humor character. Correct restraint.
- ✅ **Pillar 5 (Theme — Revenge):** Indirect but present. The Warden was once protected children. He was abandoned because of imperial expansion (the Specimen Recovery Program — the same program that drives Draven's pre-pirate guilt). Jake kills him as part of his hunt for Ethan. The pattern of revenge cascading through generations starts here, in the very first major boss fight.

---

## 13. Implementation Notes for Claude Code / UE5

- Pollen Burst requires a particle system with debuff trigger
- The Long Breath passive needs a state machine that ignores damage during the breathe animation
- Mask shattering at 60% HP is a scripted event with custom animation
- Phase 3 random targeting requires a separate AI behavior tree branch
- Death scene must be a scripted cinematic — including dust effects on the dead tree
- The carving (Lore Object #006) must be a unique inventory item with custom interaction
- Story flag `jungle_warden_defeated` triggers Galleon Cove path opening AND adds the Warden's Mask Fragment to the inventory
