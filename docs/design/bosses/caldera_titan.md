# BOSS SPEC — The Caldera Titan (Ibu's Grief)

**Status:** Design Draft v1.0  
**Region:** Ashenmoor — Gunung Ibu (The Mother)  
**Boss Category:** Mythical Optional Boss (GDD §7.3)  
**Difficulty:** Hard but Fair (10-15 attempts average)  
**Related:** region_03_ashenmoor_volcanoes.md, combat_system.md  
**Inspirations:** Shadow of the Colossus (scale & emotion), Elden Ring's Magma Wyrm, Princess Mononoke's Forest Spirit

---

## 1. Concept — The Titan IS the Volcano

The Caldera Titan is not a creature that lives inside Gunung Ibu. The Titan IS Gunung Ibu — the volcano's grief given physical form. For millennia the Anak Gunung performed rituals at the caldera's edge that soothed the Mother's pain, kept her sleeping peacefully, made her feel seen. When the Crimson Armada took over Kota Api, those rituals stopped. No one sang the old songs. No one left the offerings. And so, slowly, the Mother's grief solidified into a body that walks.

When Jake climbs the Smoking Path and steps onto the caldera rim, the Titan rises from the molten rock itself. She is not angry at Jake specifically. She is angry at the world that forgot her. Killing her isn't victory — it's the most painful mercy in the game. When she dies, she doesn't roar. She exhales. And Gunung Ibu stops erupting.

**The Titan's design principle:** Every mechanic should reinforce that this is a fight against sorrow, not malice. She hesitates. She grieves between attacks. Jake's victory should feel like he's ending a long suffering, not earning a trophy.

---

## 2. Visual Description

- **Height:** 15 meters (approximately 5 stories tall)
- **Form:** Humanoid, feminine, asymmetrical — one arm is made of cooled black basalt, the other is a flowing river of active magma that slowly reshapes itself
- **Torso:** Cracked stone with glowing veins of molten rock beneath; the cracks widen and narrow as she "breathes"
- **Face:** A smooth stone mask carved in the style of ancient Anak Gunung funeral masks — no eyes, just two black hollows where tears of molten rock sometimes drip
- **Hair:** Strands of obsidian glass that catch light and shatter when she is hit hard
- **Aura:** Heat distortion visible from 20 meters away; Jake takes 1 HP/second passive damage within 5 meters unless wearing heat-resistant gear

---

## 3. Arena — The Caldera Rim

- **Shape:** Circular platform, 40 meters diameter, surrounding the lava pit
- **Center:** A bubbling pit of molten rock (instant kill if Jake falls in)
- **Edge:** A 3-meter safe ring around the outer rim (falls off the mountain = instant kill)
- **Surface:** Cracked volcanic rock, stable but hot
- **Heat Zones:** Marked by visible shimmer; standing in them deals 3 HP/second
- **Rest Point:** Located just before the arena entrance, 30 seconds walk from boss

### 3.1 Arena Mechanics by Phase
| Phase | Arena State |
|-------|-------------|
| 1 | Full arena, 2-3 small heat zones at fixed points |
| 2 | Lava pit rises, shrinking safe ground to 25m diameter, 5 heat zones (dynamic) |
| 3 | Arena is 70% lava with 6 solid stone "islands" the Titan creates as she moves |

---

## 4. Stats

| Stat | Value |
|------|-------|
| Total HP | 4,800 |
| Phase 1 HP | 4,800 → 2,880 (60%) |
| Phase 2 HP | 2,880 → 1,200 (25%) |
| Phase 3 HP | 1,200 → 0 |
| Posture | 350 |
| Posture Regen Delay | 6 seconds (long — parries are valuable) |
| Defense | 35 (heavy) |
| Fire Resistance | Immune |
| Stagger Threshold | 120 posture damage in one combo |
| Critical Window (after posture break) | 2.5 seconds |
| Critical Damage Multiplier | 3.5x |

---

## 5. Moveset — Phase 1 (100% → 60% HP)

The Mother is slow and sad in Phase 1. Her attacks are telegraphed heavily — almost as if she's warning Jake to run away. Every move has a long windup accompanied by a low, grieving moan.

### 5.1 Attack: Stone Fist Slam
- **Trigger:** Jake within 8m, facing her
- **Windup:** 45 frames (750ms) — arm rises high, glowing audible charge
- **Active:** Frames 46-52 (7 frames, ~117ms impact window)
- **Recovery:** 40 frames (667ms) — arm stuck in ground, massive punish window
- **Damage:** 35 base damage (direct hit), 15 splash damage (3m radius)
- **Posture Damage:** 25
- **Counter:** Dodge to either side during frame 40-50, or parry during frames 48-52 for massive posture gain
- **Audio Tell:** Low-frequency rumble builds during windup; impact is a stone-crack sound

### 5.2 Attack: Magma Sweep (Horizontal)
- **Trigger:** Jake within 12m, at her side
- **Windup:** 35 frames (583ms) — magma arm pulls back across her body
- **Active:** Frames 36-50 (15 frames of sweeping hitbox)
- **Recovery:** 30 frames
- **Damage:** 42 base damage, leaves a burning line on the ground for 4 seconds (10 HP/sec)
- **Posture Damage:** 30
- **Counter:** Dodge roll THROUGH the attack during i-frames (frames 4-14 of dodge), or duck by using Crouch (Circle tap)
- **Audio Tell:** A hiss building into a roar as magma gathers

### 5.3 Attack: Lava Tear Drop
- **Trigger:** Jake far from her (>15m)
- **Windup:** 50 frames (833ms) — the Titan tilts her head back, a single molten tear forms at her mask
- **Active:** Frames 51+ (projectile travel, ~60 frames to reach target)
- **Recovery:** 25 frames
- **Damage:** 50 base damage on direct hit, 3m explosion radius
- **Posture Damage:** 20
- **Counter:** Move sideways as the tear falls; tracking is slow. Can be shot out of the air with a firearm before impact.
- **Audio Tell:** A soft, almost musical note — the sound of sorrow
- **Narrative Note:** When this attack hits the ground without hitting Jake, the Titan briefly stops, looks at the cooling lava where her tear fell, and hesitates for 1 full second. This is a MASSIVE free punish window.

### 5.4 Attack: Mourning Step
- **Trigger:** Jake at mid-range (6-10m)
- **Windup:** 30 frames — she takes a heavy step toward Jake
- **Active:** The step itself is the attack — frame 30 creates a shockwave 5m around her foot
- **Recovery:** 20 frames
- **Damage:** 25 base damage, knocks Jake back 4m
- **Posture Damage:** 35 (high — hard to parry)
- **Counter:** Jump (Cross button) exactly on frame 30, or be outside 5m range
- **Audio Tell:** The stone groans as she lifts her foot

### 5.5 Passive: The Grieving Pause
Every 30 seconds in Phase 1, the Titan stops attacking for 4 full seconds. She drops to one knee. A single molten tear drips from her mask. No attack follows. This is a clear design signal to the player: **she is grieving, not hunting you.** The player can safely heal, reposition, or deal free damage during this window. The Grieving Pause disappears in Phase 2.

---

## 6. Moveset — Phase 2 (60% → 25% HP)

Phase 2 Trigger: At 60% HP, the Titan walks to the center of the caldera, kneels into the lava pit, and submerges to the waist. The arena changes — lava rises, safe ground shrinks. She is now half-submerged and uses her arms only. Faster. More desperate. The Grieving Pause is gone.

### 6.1 Attack: Double Fist Hammer
- **Trigger:** Jake within 10m
- **Windup:** 30 frames (faster than Phase 1)
- **Active:** Frames 31-38
- **Recovery:** 25 frames
- **Damage:** 50 per fist (can hit twice for 100), 5m radius per impact
- **Posture Damage:** 40 per fist
- **Counter:** Dodge away from both impact zones or parry one fist for partial posture gain
- **Audio Tell:** Both arms raise simultaneously, a deep "BOOM" chord sounds

### 6.2 Attack: Magma Lash (Whip)
- **Trigger:** Jake at any range
- **Windup:** 25 frames — magma arm extends and stretches to 15m length
- **Active:** Frames 26-45 (long whip swing across arena)
- **Recovery:** 30 frames
- **Damage:** 55 base damage
- **Posture Damage:** 35
- **Counter:** Dodge through (frames 4-14 i-frames) or duck under (timing-dependent)
- **Audio Tell:** The crack of molten rock stretching

### 6.3 Attack: Lava Eruption (AoE)
- **Trigger:** Every 20 seconds, unblockable
- **Windup:** 60 frames (1 second) — the Titan raises both arms, the entire lava pit glows brighter
- **Active:** Frame 61 — 4 columns of lava erupt from random points in the arena, marked by red circles on the ground during windup
- **Damage:** 80 damage, knocks Jake prone
- **Posture Damage:** 50
- **Counter:** Read the marked red circles during windup and move to uncharted spots. There is ALWAYS a safe zone.
- **Audio Tell:** A deep inhale followed by rising harmonic tones

### 6.4 Attack: The Reach
- **Trigger:** Jake stays on the opposite side of the arena too long (>15 seconds)
- **Windup:** 40 frames — she extends her magma arm fully across the arena
- **Active:** Frames 41-60
- **Recovery:** 50 frames (massive punish)
- **Damage:** 60 base damage, drag effect pulls Jake toward her
- **Posture Damage:** 25
- **Counter:** Parry at the exact moment of contact, or dodge sideways. Parrying this staggers the Titan for 3 seconds.
- **Design Intent:** Prevents "cheese" strategies where Jake hides across the arena

---

## 7. Moveset — Phase 3 (25% → 0% HP)

Phase 3 Trigger: At 25% HP, the Titan cracks open. Her stone mask shatters, revealing not a face but a pure molten core. She emerges fully from the lava pit, taller now (18 meters), and the arena becomes 70% lava with 6 solid stone islands. She creates more islands as needed.

This is the emotional climax of the fight. She is no longer grieving — she is burning out. Every attack she makes costs her. Her moveset is faster but less predictable.

### 7.1 Attack: Cascade of Tears
- **Trigger:** Random, every 15 seconds
- **Windup:** 45 frames
- **Active:** 5 lava tears fall in sequence, 20-frame gaps between each
- **Damage:** 35 per tear (175 total if all hit)
- **Counter:** Each tear can be dodged independently if the player stays mobile

### 7.2 Attack: Island Shatter
- **Trigger:** Jake stands on the same island >5 seconds
- **Windup:** 20 frames (fast warning)
- **Active:** The Titan slams a fist down on Jake's current island
- **Damage:** 100 damage if hit, island is destroyed (forces repositioning)
- **Counter:** Keep moving between islands

### 7.3 Attack: The Final Embrace
- **Trigger:** Once, at 10% HP
- **Windup:** 60 frames — the Titan extends both arms wide, invites Jake in
- **Active:** Any Jake within 8m is grabbed and dragged toward her chest
- **Damage:** 200 damage (likely kill), unblockable
- **Counter:** Be outside 8m during windup. If grabbed, mash Square to escape (narrow window).
- **Narrative Note:** If Jake survives this, the Titan pauses for a full 2 seconds, as if surprised Jake is still alive, before resuming normal attacks.

### 7.4 Passive: The Cooling
Every 45 seconds in Phase 3, the Titan freezes mid-attack as cold cracks spread across her body. She gasps audibly. This is the player's window — 3 seconds of total vulnerability. Narrative reason: she is beginning to die, and in those moments she remembers what it was like to be the mountain, not the monster.

---

## 8. Combat Flow — How the Fight Should Feel

**Phase 1 (The Grieving):** Patient. Jake learns her telegraphs. The Grieving Pauses let him breathe and understand. First-time players often feel sympathy — "wait, is this thing... sad?"

**Phase 2 (The Desperate):** Reactive. The arena shrinks, the pace quickens, the grieving stops. The player thinks "okay now it's a real boss fight." Mistakes punish harder. This is where most deaths happen.

**Phase 3 (The Burning Out):** Elegant chaos. The player has learned her, and now the arena tests their movement. The Cooling passive is a reward for enduring — three seconds of peace in the heart of fire.

**The Final Hit:** When Jake deals the killing blow, the Titan does not roar or fall dramatically. She exhales. She lowers her head. She turns her mask toward Jake, and for one frame, the mask seems to smile. Then she crumbles into cooling rock. Gunung Ibu stops smoking. The ash clouds clear. Somewhere far below in Kota Api, the miners look up in confusion at the sudden silence.

---

## 9. Rewards

| Reward | Description |
|--------|-------------|
| **Magma Amulet** | Lava immunity, +30% Strength near heat sources, +15% fire damage on all weapons |
| **Titan's Core** | Crafts legendary two-handed weapon "The Mourning Hammer" at Master Smith Jaya |
| **Recipe: Molten Rounds** | Pistol ammo that sets enemies on fire for 8 seconds (10 HP/sec DoT) |
| **5 Gold Bars** | Hidden in the cooled remains after the fight |
| **Lore Unlock:** "The First Ritual" — ancient text that reveals what the Anak Gunung used to sing to calm the Mother |
| **Trophy:** "Ibu's Tear" — a cooled lava teardrop the player can display on their ship |
| **Ashenmoor Karma:** If on Path A (Liberator), the Anak Gunung hold a funeral ritual for the Titan the next in-game day, and Jake is invited. Emotional cutscene. |

---

## 10. Strategy Notes (For Player-Facing Hints)

These are the hints that Kuil Atas monks give Jake if he dies to the Titan multiple times. They are NEVER shown as a tutorial — they emerge through dialogue.

- "The Mother grieves. Watch her pauses. Do not strike when she weeps — or do, but know that it is mercy, not glory."
- "Fire fears cold, stone fears patience. You will not break her by rushing."
- "Her tears fall slowly. Her hand moves slowly. Only her heart is fast — and her heart is buried deep."
- "Bring water from the high shrine. It will not cool her, but it will cool you."

**Gameplay Translation:**
- Use the Grieving Pause in Phase 1 for free damage
- Don't panic-heal during her Cooling passive in Phase 3 — use it for damage
- Water of Kuil Atas = Cold Spring Tincture (+30% fire resistance for 90 sec, available from Abbot Purnama)

---

## 11. Implementation Notes for Claude Code / UE5

When this boss is implemented in UE5:
- Use `ABossCharacter` base class (inherits from `AEnemyBaseClass`, to be written)
- State machine: Idle → Phase1 → Phase2Transition → Phase2 → Phase3Transition → Phase3 → Death
- Each attack should be a separate animation montage with explicit frame triggers
- Arena state changes driven by HP thresholds — trigger lava rise via animated blueprints
- Audio cues are critical: the Titan should "feel" emotional through sound design even more than visuals
