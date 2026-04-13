# BOSS SPEC — Sturmkralle, the Sky Terror

**Status:** Design Draft v1.0  
**Region:** Ashenmoor — Gunung Anak (The Child), Eagle's Perch Summit  
**Boss Category:** Mythical Optional Boss (GDD §7.3)  
**Difficulty:** Hard but Fair (10-15 attempts average)  
**Related:** region_03_ashenmoor_volcanoes.md, combat_system.md  
**Inspirations:** Dragons of Dark Souls 2 (aerial mechanics), Sekiro's Demon of Hatred (phase structure), Princess Mononoke's wind spirits

---

## 1. Concept — Fury of the Sky

Where the Caldera Titan is grief, Sturmkralle is rage. She is an ancient apex predator — the last of her kind, older than any Anak Gunung legend. She has nested on the peak of Gunung Anak for centuries, feeding on whatever the young volcano stirs up in its eruptions. She does not grieve, she does not reason, she does not pause. She hunts. The storms that constantly circle her nest are not weather — they are her. Where she flies, lightning follows.

**Design principle:** The fight is a skill check on the player's ability to switch between melee and ranged combat fluidly. You cannot beat Sturmkralle with just sword or just gun. The fight demands the full Weapon Wheel.

---

## 2. Visual Description

- **Size:** 8 meters tall standing, 20 meter wingspan
- **Form:** Massive raptor — part eagle, part condor, part something older
- **Feathers:** Blue-black, streaked with silver. Every feather looks sharpened.
- **Wings:** When spread, storm clouds literally form at the tips; lightning arcs between her primary feathers
- **Talons:** Each claw is the length of a man's forearm, curved and golden
- **Eyes:** Amber, twin suns; she tracks Jake with terrifying intelligence
- **Sound:** Her cry is a multi-layered shriek that carries for kilometers; players should hear it from Desa Padi before they even climb

---

## 3. Arena — The Peak Nest

- **Shape:** Irregular rocky platform on the summit of Gunung Anak, ~35m x 40m
- **Nest:** A massive tangle of bones and charred branches in the center; Jake can use it as cover
- **Edges:** No walls — the platform drops off on all sides into clouds. Falling off = instant kill.
- **Weather:** Storm clouds rotate around the arena; periodic lightning strikes at marked locations in Phase 3
- **Wind:** Constant wind pushes Jake toward the edges; must compensate during movement
- **Rest Point:** Just below the peak, 45 seconds climb away

---

## 4. Stats

| Stat | Value |
|------|-------|
| Total HP | 5,200 |
| Phase 1 HP | 5,200 → 3,120 (60%) |
| Phase 2 HP | 3,120 → 1,300 (25%) |
| Phase 3 HP | 1,300 → 0 |
| Posture | 280 |
| Posture Regen Delay | 4 seconds |
| Defense | 25 (medium — her speed is her defense) |
| Weak Point | Head during aerial dives (bonus damage with ranged weapons) |
| Lightning Resistance | Immune |
| Stagger Threshold | 100 posture damage in one combo |
| Critical Window | 2 seconds after posture break |
| Critical Multiplier | 3x |

---

## 5. Moveset — Phase 1 (100% → 60% HP): Ground Fury

In Phase 1, Sturmkralle fights on the ground. She is massive and fast, using talons and beak. She wants Jake close so she can tear him apart.

### 5.1 Attack: Double Talon Slash
- **Trigger:** Jake within 6m
- **Windup:** 22 frames
- **Active:** Frames 23-32 (two quick strikes)
- **Recovery:** 28 frames
- **Damage:** 32 per slash (64 total if both hit)
- **Posture Damage:** 20 per slash
- **Counter:** Dodge sideways, parry window is tight
- **Audio Tell:** Sharp hiss before strike

### 5.2 Attack: Beak Stab
- **Trigger:** Jake directly in front, within 4m
- **Windup:** 18 frames (fastest attack — punishes careless positioning)
- **Active:** Frame 19
- **Recovery:** 22 frames
- **Damage:** 45
- **Posture Damage:** 30
- **Counter:** Parry (frames 14-19 for perfect) or dodge back
- **Audio Tell:** Short high-pitched shriek

### 5.3 Attack: Wing Buffet
- **Trigger:** Jake at her side
- **Windup:** 30 frames — wing extends
- **Active:** Frames 31-42
- **Recovery:** 25 frames
- **Damage:** 40, knocks Jake back 5m
- **Posture Damage:** 25
- **Counter:** Dodge through with i-frames or get behind her

### 5.4 Attack: Ground Slam (Feather Storm)
- **Trigger:** Every 25 seconds
- **Windup:** 40 frames — she rears up, wings spread wide
- **Active:** Frames 41-45 — slams wings forward, launching 12 razor feathers in a cone
- **Recovery:** 35 frames (big punish window)
- **Damage:** 20 per feather hit (max 60)
- **Posture Damage:** 15 per hit
- **Counter:** Get behind her during windup, or dodge through the cone

### 5.5 Tell: The Takeoff
At 60% HP, Sturmkralle lets out a long cry and leaps into the air. This is the Phase 1 → Phase 2 transition. She cannot be damaged during this 3-second animation.

---

## 6. Moveset — Phase 2 (60% → 25% HP): Aerial Dominance

Phase 2 is the defining challenge. Sturmkralle spends most of her time in the air. The player MUST have ranged weapons ready. Pure melee builds suffer here — which is intentional.

### 6.1 Attack: Dive Bomb
- **Trigger:** Every 6-8 seconds while airborne
- **Windup:** 50 frames — she circles high, then locks onto Jake's position
- **Active:** Frames 51-70 (dive from above at high speed)
- **Recovery:** 40 frames (lands on the ground, briefly grounded)
- **Damage:** 65 on direct hit, 30 splash within 4m
- **Posture Damage:** 45
- **Counter:** Dodge at the last moment OR shoot her with musket/rifle during the dive. A well-timed shot staggers her mid-dive, knocking her to the ground for 5 seconds of free damage.
- **Audio Tell:** A rising whistle as she descends

### 6.2 Attack: Lightning Shriek
- **Trigger:** Every 15 seconds while airborne
- **Windup:** 35 frames — she hovers and spreads her wings wide
- **Active:** Frame 36 — lightning arcs from her wings to 3 random ground points
- **Damage:** 70 per lightning strike
- **Posture Damage:** 40
- **Counter:** Marked red circles appear 1 second before strikes. MOVE.
- **Audio Tell:** Building electric hum

### 6.3 Attack: Wing Gust
- **Trigger:** Every 20 seconds
- **Windup:** 25 frames
- **Active:** Creates a horizontal wind blast across the arena
- **Damage:** 15 damage but pushes Jake 10m toward the arena edge
- **Counter:** Lock onto the ground (crouch or dig in with sword stance) during the blast
- **Design Intent:** Threatens instant-kill falls off the platform

### 6.4 Passive: Storm Circle
While airborne, 3 small tornadoes spawn at the arena edges. Touching one deals 20 damage and disorients Jake for 2 seconds. Creates area denial.

### 6.5 Forced Grounding
Every 30 seconds, Sturmkralle must land to catch her breath (4 seconds on the ground). This is the primary melee window. During grounding, she uses Phase 1 attacks but is vulnerable to heavy punishes.

---

## 7. Moveset — Phase 3 (25% → 0% HP): Storm Incarnate

At 25% HP, Sturmkralle becomes one with the storm. Lightning constantly crackles along her body. She is faster, more erratic, and her aerial phases shorten — she now dives every 3-4 seconds. The arena is under continuous weather hazards.

### 7.1 Attack: Thunder Dive
- **Trigger:** Replaces Dive Bomb
- **Same mechanics** but on impact, lightning explodes in a 6m radius dealing additional 40 damage
- **Counter:** Same timing as Dive Bomb, but don't stand in the impact zone after dodging

### 7.2 Attack: Sky Storm Barrage
- **Trigger:** Once, at 15% HP
- **Windup:** 60 frames — she flies to the highest point of the arena
- **Active:** Rains 15 lightning strikes across the arena in sequence over 8 seconds
- **Damage:** 50 per hit
- **Counter:** Strikes are telegraphed 0.5 seconds before impact. Constant movement required. There is always a safe path if read correctly.
- **Narrative Note:** This is her desperate all-in move. If Jake survives it, she lands exhausted for 6 full seconds — the biggest punish window in the fight.

### 7.3 Passive: Lightning Shroud
Sturmkralle has a 2-second window between ALL her attacks in Phase 3 where her body crackles with lightning. Melee attacks against her during this window deal 10 damage to Jake per hit. Ranged attacks are safer.

---

## 8. Combat Flow — How the Fight Should Feel

**Phase 1 (Ground Fury):** Feels like fighting a tyrannosaurus. Big, fast, dangerous. Tests melee fundamentals.

**Phase 2 (Aerial Dominance):** The shift. The player realizes this isn't a pure melee fight. Panic if they didn't bring a ranged weapon. Scramble to use the Weapon Wheel mid-combat. This is where Dark Arisen's hybrid combat system shines.

**Phase 3 (Storm Incarnate):** Beautiful chaos. The entire arena becomes the boss. Movement is survival. Damage comes from reading the storm, not hitting the bird.

**The Final Hit:** She falls from the sky in a slow, almost graceful descent. Lightning discharges harmlessly into the clouds. She lands on her nest, curls inward, and goes still. The storm clouds around the peak dissipate for the first time in centuries. Jake can see for hundreds of kilometers from the summit — a visual reward for the difficulty.

---

## 9. Rewards

| Reward | Description |
|--------|-------------|
| **Talon Amulet** | Fall damage immunity, +40% dodge speed, unlocks aerial downward-strike attack |
| **Storm Feather** | Crafts legendary bow "Sky's Verdict" at Master Smith Jaya |
| **Recipe: Lightning Arrows** | Arrows that chain lightning between up to 3 enemies |
| **3 Gold Bars** | Hidden in her nest |
| **Trophy: "Storm Heart"** | A crystallized piece of lightning that Jake can display on his ship |

---

## 10. Strategy Notes (Player-Facing Hints)

These hints come from Old Miner Darma in Kota Api if the player struggles:

- "She is the sky's fury. You cannot fight her with a blade alone. Climb with a rifle, and may the wind be kind."
- "Her dives are her hunger. When she falls, she is blind to everything but her target. Use that."
- "The storm is her. When you see the lightning in her feathers, do not touch her — shoot her."

**Gameplay Translation:**
- Bring a rifle or musket to Phase 2
- Shoot her mid-dive to ground her for free damage
- In Phase 3, use ranged weapons during her Lightning Shroud windows

---

## 11. Implementation Notes

- Aerial mechanics require a flight state machine with pathfinding for circling patterns
- Lightning strike markers: decal system with 1-second telegraph
- Wind gust system: directional force applied to player character during windup
- Arena fall kill zone: Z-coordinate trigger
