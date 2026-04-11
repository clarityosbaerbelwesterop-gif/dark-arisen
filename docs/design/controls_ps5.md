# Controls Specification — PlayStation 5

**Version:** 1.0  
**Status:** Design Draft  
**Related:** GDD v1.2 §5

---

## 1. Default Button Layout

| Button | Action | Context |
|--------|--------|---------|
| **R1** | Light Attack | Combat |
| **R2** | Heavy Attack (tap) / Charged Heavy (hold) | Combat |
| **R1 + Square** | Fire Equipped Firearm | Combat |
| **L1 (hold)** | Open Weapon Wheel | Anytime |
| **L2 (hold)** | Aim / Lock-On (toggle) | Combat |
| **L3 + R3** | Activate Rache (Dead Eye) | Combat, after Ch.9 |
| **Circle** | Dodge Roll (directional) | Combat / Movement |
| **Square** | Parry (timed tap) | Combat |
| **Triangle** | Interact / Loot / Critical Hit | Context |
| **Cross** | Jump / Climb / Vault | Movement |
| **D-Pad Up** | Quick Heal | Anytime |
| **D-Pad Down** | Whistle for Horse | Open World only |
| **D-Pad Left/Right** | Cycle Quick Items | Anytime |
| **Touchpad** | Open Full Map | Anytime |
| **Options** | Pause Menu | Anytime |
| **Left Stick** | Move | Anytime |
| **Right Stick** | Camera | Anytime |
| **L3 (click)** | Sprint (toggle) | Movement |
| **R3 (click)** | Reset Camera / Lock-On Target | Combat |

---

## 2. Edge Cases & Conflict Resolution

### 2.1 Simultaneous Inputs
| Conflict | Resolution |
|----------|------------|
| R1 + R2 same frame | Heavy Attack wins (R2 priority) |
| R1 during Heavy Attack windup | Buffered, plays after recovery |
| Square (parry) during own attack | Cannot parry — animation locks until recovery |
| Circle during attack recovery | Dodge cancels recovery (uses 20 SP extra) |
| L1 (Wheel) during combat | Wheel opens, time slows to 30%, but enemies can still hit Jake |
| L3+R3 (Rache) without unlock | Nothing happens, no visual feedback |
| L3+R3 (Rache) with empty meter | Brief "no fuel" sound, no activation |

### 2.2 Animation Cancels
- **Light Attack → Dodge:** Cancellable after frame 12 (impact frame)
- **Heavy Attack → Dodge:** Cancellable only after frame 25 (commitment penalty)
- **Healing animation → Dodge:** Cancels heal, item is consumed but no HP restored (punishing)
- **Reload → Sword Attack:** Cancels reload, ammo not loaded

### 2.3 Buffer Window
- Inputs buffered for 8 frames (133ms) before next available action
- Prevents missed inputs during animation locks
- Does not buffer Rache activation (must be timed precisely)

---

## 3. Weapon Wheel Behavior

- **Open:** Hold L1
- **Time Scale:** 30% (slow-mo, not pause)
- **Selection:** Right Stick to highlight slot
- **Confirm:** Release L1 (equips highlighted weapon)
- **Cancel:** Press Circle while wheel open
- **Combat Risk:** Enemies can still hit Jake while wheel is open — using mid-fight is risky
- **Quick Swap:** Double-tap L1 to swap to last-used weapon (no wheel needed)

---

## 4. Lock-On System

- **Activation:** L2 tap (toggle on/off)
- **Target Switch:** Right Stick flick left/right
- **Range:** 15 meters
- **Lost Lock:** If target moves >20m, breaks behind cover, or dies
- **Camera:** Centers between Jake and target
- **Movement:** Strafe-based (Left Stick is relative to lock)

---

## 5. Horse Controls (Mounted)

| Button | Action |
|--------|--------|
| **X (hold)** | Spur / Sprint |
| **Cross (tap)** | Trot / Canter cycle |
| **Square** | Dismount |
| **R1** | Sword Swing (mounted) |
| **R1 + Square** | Pistol Shot (mounted) |
| **L2** | Aim (mounted) |
| **D-Pad Down** | Hitch / Calm Horse |

---

## 6. Accessibility Options

- **Toggle vs. Hold:** Aim, Block, Sprint can be set to toggle or hold
- **Button Remapping:** Full custom remapping available
- **Vibration Intensity:** 0-100% slider
- **Lock-On Sensitivity:** Adjustable
- **Auto-Sprint:** Optional (sprint engages after 0.5s of full stick)
