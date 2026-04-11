# Combat System — Technical Specification

**Version:** 1.0  
**Status:** Design Draft  
**Related:** GDD v1.2 §6, §5.1

---

## 1. Core Stats

### 1.1 Base Stats (Level 1)
- **Health (HP):** 100
- **Stamina (SP):** 100
- **Posture:** 80
- **Poise:** 1.0 (multiplier, modified by armor)

### 1.2 Level Scaling
Stats grow per level allocation. Each stat point invested adds:

| Stat | Per Point | Soft Cap | Hard Cap |
|------|-----------|----------|----------|
| Vitality | +8 HP | 40 (340 HP) | 60 (420 HP) |
| Endurance | +5 SP | 40 (300 SP) | 60 (380 SP) |
| Grit | +4 Posture | 40 (240) | 60 (320) |
| Strength | +2% Melee DMG | 40 | 60 |
| Precision | +2% Ranged DMG | 40 | 60 |
| Wrath | +1 sec Rache duration | 30 | 50 |

After soft cap, gains are halved. After hard cap, +1 per point.

---

## 2. Stamina System

### 2.1 Costs
| Action | Stamina Cost |
|--------|--------------|
| Light Attack (R1) | 15 |
| Heavy Attack (R2) | 28 |
| Charged Heavy (R2 hold) | 40 |
| Dodge Roll | 20 |
| Sprint | 8 per second |
| Block (hold) | 5 per second |
| Successful Parry | 0 (free) |
| Failed Parry (blocked hit) | 25 |
| Jump | 10 |

### 2.2 Regeneration
- **Out of combat:** 100 SP in ~2 seconds
- **In combat (no actions):** 100 SP in ~3 seconds
- **Regen Delay:** 1.0 sec after last action
- **Regen Pause:** During block, no regen

### 2.3 Out of Stamina
- Cannot attack, dodge, or sprint
- Can still walk and parry (parry uses no stamina)
- Forced "exhausted" state for 0.5 sec when hitting zero

---

## 3. Posture System (Sekiro-Inspired)

### 3.1 Mechanics
- Posture builds from blocking, getting hit, and successful parries (on enemies)
- When full, target is staggered → critical hit window opens
- Posture regenerates if not refilled within 4 seconds

### 3.2 Posture Damage Values
| Action | Posture DMG to Enemy |
|--------|----------------------|
| Light Attack | 8 |
| Heavy Attack | 18 |
| Charged Heavy | 30 |
| Successful Parry | 35 |
| Perfect Parry (within 5 frames) | 50 |
| Pistol Shot (close range) | 15 |

### 3.3 Critical Hit Window
After posture break: 1.5 second window to perform critical hit (Triangle button). Critical hits deal 3x normal damage and play a unique animation per weapon class.

---

## 4. Parry System

### 4.1 Parry Window
- **Total window:** 15 frames (250ms at 60fps) — Elden Ring style
- **Perfect parry:** First 5 frames (83ms) — bonus posture damage
- **Late parry:** Frames 6-15 — normal block, no parry effect

### 4.2 Parry Rules
- Triggered by Square button tap (not hold)
- Cooldown: 0.4 sec between parry attempts
- Cannot parry: Grab attacks, projectiles (without shield), unblockable attacks (red glow telegraph)
- Successful parry: Restores 10 SP, fills enemy posture by 35

---

## 5. Dodge System

### 5.1 i-Frames
- **i-Frame window:** Frames 4-14 (10 frames of invincibility)
- **Total dodge animation:** 30 frames (500ms)
- **Recovery (vulnerable):** Frames 15-30
- **Distance:** 4 meters in dodge direction
- **Stamina cost:** 20

### 5.2 Roll vs. Step
- Default: Dodge Roll (more distance, more i-Frames)
- Equipping the "Light Footwork" charm: Dodge Step (less distance, faster recovery)

---

## 6. Damage Formulas

### 6.1 Base Damage Calculation
