# MECHANICS & FUNCTIONS — MASTER OVERVIEW

**Status:** Design Draft v1.0  
**Phase:** Phase 3 — Mechanics & Functions  
**Related:** controls_ps5.md, combat_system.md, naval_combat_system.md, style_bible.md  
**Purpose:** Master index and design philosophy for all core gameplay systems in Dark Arisen. Every subsystem document must reference this file for shared assumptions, values, and dependencies.  
**Target Platforms:** PlayStation 5 (primary), PC (Steam), Xbox Series X/S

---

## 1. DESIGN PHILOSOPHY — THE FOUNDATION

Dark Arisen is a Souls-like Pirate Open-World Action-RPG. Its mechanical identity is built by combining the best systems from three specific reference games:

### 1.1 The Three Pillars

**BLOODBORNE — Combat & Traversal Feel**
- Fast, aggressive melee combat with emphasis on offensive dodging
- Rally system (regain HP through aggression)
- Trick weapons / transformation mechanics (adapted as "dual-mode weapons")
- Parry through firearms (adapted as pistol parry)
- Stamina management under constant pressure
- Movement feels weighty but responsive
- PS5 DualSense haptic feedback is central

**RED DEAD REDEMPTION 2 — Open World Interaction**
- Dead Eye system (adapted as "Rache" / Revenge Focus)
- Weapon Wheel (L1 hold radial)
- Item Wheel (L1+R1 hold radial)
- Satchel inventory with realistic weight
- Crafting from gathered materials
- Interaction density (every object has purpose or flavor)
- Horse bonding (adapted as ship bonding)
- Hunting and skinning systems
- Food and tobacco consumption with actual effects
- Clothing affects temperature/weather response
- NPC awareness and memory (they recognize Jake)

**SEKIRO — Posture & Mastery**
- Posture system for blade-on-blade combat
- Deflect timing (Souls parry evolved)
- Mastery gates (skill must grow to progress)
- Enemy positioning puzzles
- Stealth assassinations as combat opener

### 1.2 What Dark Arisen Adds That None of These Have

- **Ship-based life:** La Liberación is not a mount, it's a MOBILE BASE (more than Black Flag's ship)
- **Crew relationships:** Named crew with actual arcs, not just gameplay function
- **Moral weight tracking:** Every significant choice affects epilogue content
- **Naval Combat depth:** Separate, complete system documented in naval_combat_system.md
- **The Six-Region Breadth:** Each region has its own culture, climate, combat approach
- **Post-Game Region 06:** A philosophical ending that answers the game's core question

---

## 2. THE 10 SUBSYSTEMS — INDEX

Phase 3 Mechanics work will produce the following subsystem documents. Each will be created as a separate file in `docs/design/mechanics/`.

### 2.1 Foundation Layer (Must Build First)
1. **Controls Foundation** — `mechanics/controls.md` — PS5/PC/Xbox layouts, full button mapping, control feel philosophy
2. **Combat Feel Deep Dive** — `mechanics/combat_feel.md` — Stamina, posture, weapon archetypes, frame data, Rally system

### 2.2 Traversal Layer
3. **Traversal System** — `mechanics/traversal.md` — Running, climbing, swimming, jumping, dodging, falling damage
4. **Stealth System** — `mechanics/stealth.md` — Sneaking, assassination, detection, enemy AI awareness

### 2.3 Interaction Layer
5. **Weapon Wheel System** — `mechanics/weapon_wheel.md` — RDR2-style L1 radial, weapon switching, weapon bonds
6. **Item Wheel System** — `mechanics/item_wheel.md` — Consumables, quick-use, emergency items
7. **Inventory & Crafting** — `mechanics/inventory_crafting.md` — Satchel, weight, resource gathering, crafting recipes
8. **Interaction System** — `mechanics/interaction.md` — Dialogue, looting, investigation, environmental objects

### 2.4 Ship Life Layer
9. **Ship Interior System** — `mechanics/ship_interior.md` — La Liberación as mobile base, cabin, sleeping, crew interaction
10. **Ship Management** — `mechanics/ship_management.md` — Upgrades, repair, resupply, crew morale

---

## 3. SHARED VALUES & DEPENDENCIES

These values must be consistent across ALL subsystem documents. Any change here propagates to all other files.

### 3.1 Jake's Base Stats (Chapter 4 baseline)

| Stat | Value | Notes |
|------|-------|-------|
| HP | 200 | Scales with Vigor investment |
| Stamina | 120 | Regenerates 20/sec after 2-sec pause |
| Posture | 100 | Regenerates after 3-sec pause |
| Rally Window | 3 seconds | Time to regain HP through aggression |
| Base Movement Speed | 5 m/s (walk), 8 m/s (run), 12 m/s (sprint) |
| Base Jump Height | 1.2m | Extended by momentum |
| Base Carry Weight | 80 kg | Affects stamina regen and dodge i-frames |

### 3.2 Stamina Cost Reference Table

All subsystems must use these baseline stamina costs:

| Action | Cost | System |
|--------|------|--------|
| Light Attack | 15 | Combat |
| Heavy Attack | 30 | Combat |
| Dodge Roll | 25 | Combat/Traversal |
| Parry | 10 | Combat |
| Sprint | 10/sec | Traversal |
| Climbing | 5/sec | Traversal |
| Swimming | 8/sec | Traversal |
| Jump | 12 | Traversal |
| Stealth Crouch Movement | 0 | Stealth (free) |
| Assassination | 20 | Stealth |
| Gathering/Skinning | 5 | Interaction |
| Heavy Object Lift | 20 | Interaction |
| Ship Rigging Climb | 15 | Ship |
| Boarding Grapple | 30 | Ship combat |

### 3.3 The Rache System (Dead Eye Equivalent)

Unlocked in Chapter 9. Core mechanic shared across combat and stealth.

- **Activation:** L3 + R3 (click both sticks simultaneously)
- **Effect:** Time slows to 30% speed for 5 seconds
- **Fuel:** Hand-rolled cigars (crafting material from tobacco) + rum (drinking)
- **Visual:** Red-tinted desaturation, blood-colored vignette
- **Audio:** Deep muffled heartbeat overlay
- **Combat use:** Aim precise strikes, chain combo setups, dodge instantly
- **Stealth use:** Observe patrol patterns, plan infiltration routes
- **Cooldown:** 60 seconds after use

### 3.4 The Weather System (Universal)

Weather affects multiple subsystems. Baseline definitions:

| Weather | Effect on Jake | Effect on Combat | Effect on Ship | Effect on Stealth |
|---------|---------------|------------------|----------------|-------------------|
| Clear | Normal | Normal | +0 speed | Normal visibility |
| Overcast | -5% perception | Normal | Normal | +10% stealth |
| Fog | -30% perception | -15% ranged accuracy | -1 knot speed | +40% stealth |
| Rain | -10% perception, -5% stamina regen | Wet surfaces slippery | -1 knot speed | +20% stealth |
| Storm | -30% perception, -20% stamina regen | Environmental damage possible | -3 knots, rogue waves | -10% stealth (noise) |
| Hurricane | Forces indoor/below deck | Combat disabled | Forces port | N/A |

---

## 4. THE CORE GAMEPLAY LOOP

Every 10-15 minutes of play, Jake cycles through this loop:

1. **Observe** — Scout area (visual, audio, dialogue with NPCs)
2. **Approach** — Choose stealth, diplomacy, or direct engagement
3. **Engage** — Combat, conversation, or environmental interaction
4. **Resolve** — Loot, reward, story consequence
5. **Recover** — Consume items, craft, rest at camp/ship
6. **Progress** — New objective or exploration opens

This loop is the foundation for all mission design. Every quest, every region, every boss encounter must support this rhythm.

---

## 5. THE MASTERY LAYERS

Dark Arisen has four skill layers the player progresses through:

### 5.1 Mechanical Mastery (Hours 1-20)
Learning the controls, timing dodges, understanding stamina. Most players master this by end of Chapter 2 (Jungle Warden fight).

### 5.2 Tactical Mastery (Hours 20-50)
Understanding which weapons counter which enemies, exploiting environmental advantages, managing resources across long missions. Mastered by end of Chapter 5.

### 5.3 Strategic Mastery (Hours 50-80)
Planning entire regions, managing crew morale, making moral choices with long-term consequences. Mastered by end of Chapter 7.

### 5.4 Thematic Mastery (Hours 80+)
Understanding what the game is actually asking. Making choices based on accumulated wisdom rather than optimization. Post-game Region 06 is where this fully lands.

**Design Principle:** The game must remain MECHANICALLY challenging at Hour 80 even though the player has mastered the controls — because the game introduces new mechanical layers (Rache, dual weapons, ship combat, boarding actions) progressively.

---

## 6. PLATFORM-SPECIFIC FEATURES

### 6.1 PlayStation 5 (Primary)
- **DualSense haptic feedback:** Different textures for walking on sand, stone, wood, mud, metal
- **Adaptive triggers:** L2 resistance scales with bow tension, R2 resistance scales with firearm reload
- **Audio:** 3D audio through headphones preferred, supports Tempest Audio Engine
- **Activities integration:** Chapter/region progress tracked in PS5 Activities
- **60 FPS target** in Performance mode, 30 FPS in Fidelity mode (4K)

### 6.2 Xbox Series X/S
- **Controller rumble:** Matches DualSense haptics in simplified form
- **Trigger resistance:** Similar to DualSense adaptive triggers
- **Quick Resume:** Full support
- **60 FPS target:** Series X (4K Performance), Series S (1440p Performance)

### 6.3 PC (Steam)
- **Keyboard + Mouse support:** Full control remapping
- **Controller support:** Any XInput or DualSense controller
- **Graphics:** DLSS/FSR/XeSS support, ray tracing optional
- **Modding:** Basic support via Steam Workshop (weapon skins, UI tweaks, NPC behavior tweaks)

---

## 7. ACCESSIBILITY

These features must be present across ALL subsystems:

- **Aim assist:** Adjustable from Off to Strong
- **Auto-dodge window:** Extend i-frames by +25% or +50%
- **Visual cues:** Color-blind modes, high-contrast UI option
- **Audio cues:** Subtitles with speaker identification, directional audio indicators
- **Difficulty scaling:** Story / Standard / Souls (Standard is baseline, Story reduces enemy damage by 40%, Souls increases by 30%)
- **Control remapping:** Full on all platforms
- **Text scaling:** 100% / 125% / 150% / 200%

---

## 8. HOW SUBSYSTEMS RELATE — DEPENDENCY STRUCTURE

The 10 subsystems form a dependency tree. Some systems must exist before others can be built.

### 8.1 Foundation Tier — Build First
**1. Controls Foundation** — The base everything else depends on. No other system can be specified until control inputs are defined.

### 8.2 Core Tier — Depends on Controls
**2. Combat Feel Deep Dive** — Depends on Controls. Defines the core action vocabulary.  
**3. Traversal System** — Depends on Controls. Peer to Combat (they share stamina pool).

### 8.3 Integrated Tier — Depends on Combat + Traversal
**4. Stealth System** — Depends on Combat (for assassinations) and Traversal (for sneaking movement).

### 8.4 Interface Tier — Depends on Combat
**5. Weapon Wheel** — Depends on Combat (defines what weapons do).  
**6. Item Wheel** — Depends on Combat (defines what items interrupt). Peer to Weapon Wheel.

### 8.5 Resource Tier — Depends on Wheels
**7. Inventory & Crafting** — Depends on both Wheels (defines what can be held/crafted).

### 8.6 World Tier — Depends on Interaction
**8. Interaction System** — Depends on all above tiers. Bridge between land and ship systems.  
**9. Ship Interior System** — Depends on Interaction. Peer system that handles La Liberación as mobile base.  
**10. Ship Management** — Depends on Ship Interior. Top of the dependency tree.

### 8.7 Dependency Rules

- A system cannot be fully specified until all its dependencies are specified.
- **Shared values** (stamina costs, Jake's base stats, weather effects) must be agreed in this Master Document first and referenced by all subsystems.
- When a conflict arises between two subsystems, the earlier-tier system wins (Controls overrides Combat overrides Traversal, etc.).
- Implementation order follows this tree: We build subsystem documents top-to-bottom, never skipping tiers.

---

## 9. WHAT COMES AFTER MECHANICS (PREVIEW OF PHASES 4-11)

### Phase 4: Physics
- Body physics (ragdoll, falling, collision)
- Environmental physics (breakable objects, rope systems, water, fire)
- Weather physics (rain accumulation, wind on sails, storm effects)
- Naval physics (ship buoyancy, wave response, cannon ballistics)

### Phase 5: Cutscenes & Cinematics
- Pre-rendered vs real-time decisions
- Key story moments identification
- Camera language across regions

### Phase 6: Items (Lore, Quest Items, Consumables)
- Complete item catalog
- Lore object #001-#150 expansion
- Quest item tracking
- Consumable effects (detailed)

### Phase 7: Ship Functions
- Crew management deep dive
- Ship upgrade economics
- Navigation system
- Port interaction

### Phase 8: Animals (Fauna)
- Land animals (hunting targets, dangers, companions)
- Sea animals (whales, sharks, fish, kraken, Havfrue Modor's children)
- Sky creatures (birds, bats, Sturmkralle's kind)
- Ecosystem behavior

### Phase 9: NPCs
- Core NPC roster (3000+ named characters across all regions)
- AI behavior trees
- Schedule systems
- Memory and recognition

### Phase 10: Voiceover
- Voice direction per character
- Accent and dialect guidelines
- Performance notes for key scenes

### Phase 11: Final Cutscenes
- Complete cinematic list
- Ending variations
- Transitions between acts

---

## 10. IMPLEMENTATION STRATEGY

### 10.1 Document Style
Each subsystem document follows this structure:
- **Concept** (the design philosophy)
- **Player Experience** (what the player feels/does)
- **Mechanical Specification** (numbers, values, frame data)
- **UI/UX Elements** (how the player interfaces with the system)
- **Platform Variations** (PS5/PC/Xbox differences)
- **Accessibility Considerations**
- **Implementation Notes** (for Claude Code / UE5)
- **Integration Points** (dependencies on other systems)

### 10.2 Iteration Expectation
Each subsystem document will be v1.0 at creation. We will likely iterate v1.1, v1.2 as we discover interactions between systems. This is normal. The master document will track version states.

### 10.3 Validation Against Existing Design
Every mechanics decision must be compatible with:
- The 21 boss fights already designed
- The 6 regions already designed
- The Style Bible's 5 Pillars
- The Naval Combat System spec
- The Character designs (Jake, Ethan, Draven, crew)

If a mechanics decision conflicts with existing design, the existing design wins (unless we explicitly decide to revise).

---

## 11. THE FINAL CHECK — DOES IT FEEL LIKE DARK ARISEN?

Before any subsystem document is finalized, it must pass this check:

> *"If a player experiences this system without any context, do they feel: 'This is a pirate game that respects me, trusts my attention, and doesn't lie about what revenge costs'?"*

If yes — the system belongs.  
If no — rework.

Dark Arisen's identity is not "Bloodborne + RDR2 + Sekiro with pirates." Its identity is: **a game that makes revenge feel like consequence**. Every mechanical system must serve that identity.

The Rally system reinforces aggression as risk-reward (every fight costs something).
The Stamina system reinforces finite resource pressure (you cannot fight forever).
The Rache system reinforces obsession as a tool (revenge focus makes you better AND burns you up).
The Ship system reinforces responsibility (your crew depends on you).
The Wheel systems reinforce preparation (what you brought defines what you can do).

Every system teaches the theme. That is the test.

---

## 12. NEXT STEPS

Immediate next document: **`mechanics/controls.md`** — Controls Foundation

This will contain:
- Complete PS5 button mapping (evolves from current controls_ps5.md)
- PC keyboard + mouse mapping
- Xbox Series X/S button mapping
- Controller feel philosophy (why these buttons do these things)
- Context-sensitive controls (land vs ship vs cabin vs stealth)
- Rache activation specifics
- Menu/inventory navigation
- Accessibility remapping

Estimated time: 3-4 hours of focused work.

After Controls: Combat Feel Deep Dive (building on existing combat_system.md).

**The journey of Phase 3 begins. One system at a time. Nine more documents in Mechanics alone. Then Physics. Then Items. Then NPCs. Then voiceover.**

**Dark Arisen will be built.**
