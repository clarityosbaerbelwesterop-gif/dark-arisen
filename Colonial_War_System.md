# WAR STATE SYSTEM — THE ENGINE OF THE ARCHIPELAGO

**Status:** Design Draft v1.0  
**Phase:** Phase 7 — Colonial War, Castles & Dungeons (Foundation Tier Document 1 — Document 2 of 16)  
**Tier:** Foundation Tier  
**Related:** colonial_war_overview.md, colonial_war/colony_system_core.md, colonial_war/retaliation_system.md, npcs/regional_populations.md, npcs/building_evolution.md, npcs/npc_recurrence_system.md, bosses/crimson_armada.md, style_bible.md  
**Purpose:** Define the complete mechanical engine of the colonial war — regional control meters, the four-power balance, what moves the needle, threshold events, momentum phases across chapters, passive drift, and the Chapter 10 final world state.  
**Inspirations:** Assassin's Creed Odyssey (nation power meters, conquest battles), Mount & Blade (kingdom warfare simulation), Total War (campaign momentum), Shadow of Mordor (nemesis-adjacent persistence), real strategic simulation design

---

## 1. SYSTEM PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: The Meter Is the World

Every abstract number in this system has a visible, audible, physical expression. Control level 20 vs 80 is not a hidden stat — it is flags in the harbor, patrol density, prices at the merchant, the mood of ambient dialogue, the state of buildings. The player never reads the number. The player reads the WORLD.

**Design intent:** Simulation felt, never displayed as spreadsheet.

### 1.2 Principle 2: Four Powers, One Balance

Each region is a four-way tension: Imperial, Albion, Liberation, Crimson. They are not independent — pushing one down lifts others. This is a balance, not four sliders. Zero-sum pressure creates real strategic weight.

**Design intent:** Every action has a beneficiary Jake may not have chosen.

### 1.3 Principle 3: The Vacuum Punishes Carelessness

Cross-reference colonial_war_overview.md Pillar 2. Breaking a power without a successor creates The Vacuum — and the archipelago abhors a vacuum. The rival empire or the Crimson Armada fills it, often worse than before. Jake learns: destruction without construction is just handing the keys to the next monster.

**Design intent:** Consequence teaches strategy. RDR2/Odyssey-style earned wisdom.

### 1.4 Principle 4: The World Moves Without Jake

Cross-reference npcs/npc_recurrence_system.md. Regions drift on their own — supply lines flow, empires push, the Crimson raids. Jake returns from a long voyage to a changed map. Passive drift makes the war feel alive, not paused-when-unwatched.

**Design intent:** Living simulation, not scripted set-piece.

### 1.5 Principle 5: The Map Is the Memory

The Chapter 10 archipelago is the sum of every choice Jake made. Regions he liberated bloom. Regions he fed grow imperial. Regions he broke and abandoned rot under Crimson banners. The final map is the game's memory of the player, rendered in territory.

**Design intent:** Player agency made geographic and permanent.

---

## 2. THE CONTROL METER ARCHITECTURE

### 2.1 The Four Meters Per Region

Every contested region tracks four values, each 0-100:

**IMPERIAL CONTROL (IC)** — Spanish-coded empire's grip  
**ALBION CONTROL (AC)** — Crown/HAC's grip  
**LIBERATION STRENGTH (LS)** — indigenous/free-peoples' organized power  
**CRIMSON THREAT (CT)** — the Armada's predatory pressure

### 2.2 The Balance Rule

**IC + AC + LS do NOT need to sum to 100.** They represent organized presence, not percentages of a pie. A region can be:
- Contested: IC 45, AC 40 (two empires clawing)
- Dominated: IC 85, AC 5, LS 10 (imperial stronghold)
- Collapsing: IC 25, AC 15, LS 30, CT 40 (nobody holds it, Crimson circling)

**CRIMSON THREAT is different** — it rises as the other three FALL. CT feeds on weakness:

```
Regional Instability = 100 - MAX(IC, AC, LS)
CT drifts toward Regional Instability over time (passive)
```

A region firmly held by anyone (MAX >= 70) keeps Crimson low. A region where no power exceeds 40 becomes Crimson feeding ground.

### 2.3 The Dominant Power

At any moment, a region has a **Dominant Power** = whichever of IC/AC/LS is highest, IF it exceeds 40. Below 40 for all three = **Contested/Failing** state.

**Dominant Power determines:**
- Flags flown in ports
- Which faction's patrols spawn
- Legal/tariff structure at merchants
- Ambient dialogue pool (cross-reference regional_populations.md Layer 2)
- Which retaliation faction targets Jake if he acts against them

### 2.4 Control Sub-Components (Full Vision)

**MVP CORE:** Control is a single meter per faction.

**FULL VISION:** Each Control meter decomposes into three pillars that can be attacked separately:

- **MILITARY (M)** — garrison strength, fort integrity, fleet presence
- **ECONOMIC (E)** — supply lines, treasury, trade flow
- **POPULAR (P)** — civilian compliance, labor stability, local legitimacy

```
Control = (M x 0.4) + (E x 0.35) + (P x 0.25)
```

This lets Jake choose HOW to break a colony: crush the garrison (M), starve the economy (E), or incite the people (P). Each has different retaliation flavor and different Liberation synergy (P-attacks build LS fastest).

---

## 3. WHAT MOVES THE NEEDLE

### 3.1 Weakening Actions (Reduce a Faction's Control)

Each action reduces target Control by a value; larger actions have cooldowns and escalate retaliation (cross-reference retaliation_system.md).

**Naval / Economic (hits E primarily):**

| Action | Control Impact | Retaliation Weight |
|---|---|---|
| Raid supply convoy | -3 to -6 E | Low |
| Sink war materiel transport | -5 to -8 E | Medium |
| Blockade a port (timed) | -2 E per day held | Medium |
| Burn a plantation/warehouse | -8 to -12 E | High |
| Destroy the treasury (siege) | -20 E | Very High |

**Military (hits M primarily):**

| Action | Control Impact | Retaliation Weight |
|---|---|---|
| Eliminate patrol (repeatable) | -1 to -2 M | Low |
| Sabotage fort (powder/water) | -6 to -10 M | High |
| Defeat Castle Commander (boss) | -15 M | Very High |
| **Defeat colony General (boss)** | **-25 M, -10 P** | **Maximum** |
| Win the siege (The Fall) | Colony breaks | Maximum |

**Popular (hits P primarily, builds LS):**

| Action | Control Impact | LS Gain | Retaliation |
|---|---|---|---|
| Free forced laborers | -4 P | +3 LS | Medium |
| Return stolen sacred item | -2 P | +5 LS | Low |
| Spread unrest (quest) | -3 to -6 P | +2 LS | Medium |
| Public defiance (defeat officer openly) | -5 P | +4 LS | High |
| Expose colony atrocity (evidence quest) | -10 P | +6 LS | High |

### 3.2 Strengthening Actions (Raise a Faction's Control)

**For the Empire path (either empire):**

| Action | Control Impact | Reward Tier |
|---|---|---|
| Escort supply convoy | +3 to +5 E | Standard pay |
| Deliver settlers (people) | +4 to +8 P | Population + villa progress |
| Fund garrison | +5 M (costs doubloons) | Base rights progress |
| Privateer contract (hunt rival) | +5 vs rival's Control | Letters of Marque progress |
| Defend colony from siege | +10 M, prevents Fall | Major faction rep |
| Deliver war materiel | +6 M | Warship upgrade progress |

### 3.3 Building Liberation Strength

LS is special — it cannot be bought, only earned:

| Action | LS Gain |
|---|---|
| Popular-attack actions (Section 3.1) | +2 to +6 |
| Arm Alliance fighters (deliver weapons) | +5 |
| Train Alliance (quest chains) | +8 per chain |
| Connect the networks (Jacinta+Diego+Seruni+Fjordlund quests) | +10 per connection |
| Win a Liberation Event battle | +25, region liberated |
| Defend a liberated region | +5, prevents reconquest |

**The Network Multiplier (Full Vision):** Once 3+ regional networks are connected, all LS gains receive a +25% multiplier. The Alliance becomes a real power with momentum of its own.

### 3.4 Crimson Threat Mechanics

CT is mostly passive (feeds on instability, Section 2.2) but Jake can affect it:

| Action | CT Impact |
|---|---|
| Defeat Crimson raiders (repeatable) | -3 CT locally |
| Complete Crimson-hunt quests | -8 CT |
| Leave a broken colony ungarrisoned/unliberated | +5 CT/chapter (The Vacuum) |
| Trigger Retaliation Stage 5 (empire tolerates Crimson) | +15 CT region-wide |

---

## 4. THRESHOLD EVENTS — THE STATE MACHINE

### 4.1 The Event Ladder

Regions transition through discrete states triggered by meter thresholds. These are the dramatic beats — the moments the war becomes visible story.

### 4.2 STABLE (Dominant Power >= 60)

- Normal patrols, normal prices, confident ambient dialogue
- Buildings in maintained/growing states (building_evolution.md)
- Retaliation against Jake at baseline
- **Ambient mood:** *"The Crown holds. Order holds. For now."*

### 4.3 PRESSURED (Dominant Power 45-59)

- Increased patrols (nervous), tariffs rise (defensive economy)
- Some building stagnation
- Rival faction actively pushing (their Control climbing)
- **Ambient mood:** *"Three shipments late this month. The garrison looks thin."*

### 4.4 CRISIS (Dominant Power drops below 30)

**THE COLONY CRISIS EVENT — a major beat.**

**Triggers when the Dominant Power's Control falls under 30 AND no other faction exceeds 40 (i.e. real instability, not a clean handoff).**

**Visible manifestations:**
- Riots in the streets (crowd behavior, regional_populations.md Section 12.2)
- Desertion (patrols thin dramatically, deserters become ambient NPCs)
- Boarded buildings, emptying markets (building_evolution decay states)
- The General/Governor issues desperate proclamations (ambient + possible quest)
- **Opportunity window opens:** the colony is ripe for the final assault

**Ambient mood:** *"The Gobernador hasn't left the fort in a week. They say he's writing to the Crown for ships that won't come."*

**Design Note:** Crisis is the invitation. The colony won't fall on its own from Crisis — Jake (or a rival power) must strike. Crisis can also be REVERSED: if the controlling empire reinforces (or Jake defends for pay), Control climbs back and Crisis lifts. This is the Athens-Sparta flip in action.

### 4.5 THE FALL (Crisis + Decisive Assault)

**The colony breaks.** Triggered by: Crisis state + successful siege (castle_sieges.md) OR General defeated during Crisis OR Control driven to near-zero.

**What happens at The Fall:**
- The siege battle plays out (the set-piece)
- The General/Governor is defeated, flees, or dies (Style Bible death weight)
- The flag comes down (a scripted, weighted moment)
- **The critical fork is evaluated (Section 4.6)**

### 4.6 THE FORK — What Fills the Space

**At the moment of The Fall, the system checks Liberation Strength and rival Control:**

**PATH A — LIBERATION EVENT (LS >= 50 at Fall):**
- The Alliance is strong enough to hold
- A second battle: the Liberation assault, fought WITH Alliance armies
- Region becomes **Liberated** — indigenous architecture returns, sacred sites restored, mixed governance
- Deepest Protector Path legacy (regional_populations.md Section 11.4)
- **Ambient mood:** *"The old paths are walked openly now. The children learn the songs without hiding."*

**PATH B — THE VACUUM (LS < 50, rival Control < 50):**
- Nobody can hold it
- **Crimson Threat surges** — the Armada moves in within 1-2 chapters
- The region becomes a lawless Crimson-preyed zone (worse than under empire)
- **Ambient mood:** *"We traded the Crown's tax for the Crimson's knife. Some trade."*
- **This is the punishment for breaking without building.**

**PATH C — RIVAL CONQUEST (rival empire Control >= 50 at Fall):**
- The OTHER empire was waiting
- Region flips Imperial->Albion or Albion->Imperial
- The war continues, new master, same people suffering
- **Ambient mood:** *"New flag. New language on the tax forms. Same forms."*

### 4.7 State Persistence

Cross-reference npcs/npc_recurrence_system.md, physics/destruction_physics.md. Every state change PERSISTS. A liberated region stays liberated unless retaliation reconquers it. A Vacuum region stays Crimson-preyed unless Jake cleans it up. The map remembers.

---

## 5. MOMENTUM PHASES — THE WAR'S CHAPTER RHYTHM

### 5.1 The Three Phases

The colonial war escalates across the game's chapters, gated to story progression.

### 5.2 PHASE ALPHA — THE COLD WAR (Chapters 4-6)

**Character:**
- Two empires posture; open warfare rare
- Skirmishes, proxy conflicts, economic sabotage
- The Alliance is underground, scattered
- Jake ESTABLISHES relationships, learns the board

**System state:**
- Control meters relatively stable, slow drift
- Retaliation caps at Stage 2 (letters, early bounties)
- Liberation networks not yet connectable (foundation quests only)
- Regions rarely Fall in this phase (tutorial-safe)

**Design intent:** Learn the systems at low stakes. Meet the generals. Read the letters.

### 5.3 PHASE BETA — OPEN WAR (Chapters 7-8)

**Character:**
- The cold war goes hot — a triggering event (main story) ignites open conflict
- Colonies actively contested, sieges become possible
- The Alliance can be forged into a real power
- Jake's choices start reshaping the map

**System state:**
- Control meters volatile, passive drift accelerates
- Retaliation escalates to Stages 3-4 (counter-attacks, hostages)
- Liberation networks connectable; LS can climb fast
- Regions Fall; the map starts changing color
- Assassin network fully active

**Design intent:** The war becomes the player's playground. Maximum agency, maximum consequence.

### 5.4 PHASE GAMMA — THE ENDGAME (Chapters 9-10)

**Character:**
- The archipelago's fate crystallizes
- Empires desperate (Stage 5 retaliation available — Crimson tolerance)
- The Alliance either a triumphant power or a scattered dream
- Jake's cumulative war legacy locks in

**System state:**
- Control meters approach their final positions
- Retaliation at maximum (scorched earth, Crimson tolerance)
- Liberation Events reach climax (the great liberation battles)
- The Chapter 10 world state is computed (Section 7)

**Design intent:** Consequences come due. The map becomes memory.

### 5.5 Phase Gating

Momentum phases are STORY-GATED, not meter-gated. A player can't rush to Open War by grinding convoys in Chapter 4 — the geopolitical ignition is a main-story beat. This keeps the war's escalation aligned with narrative pacing while leaving regional outcomes player-driven.

---

## 6. PASSIVE DRIFT — THE LIVING SIMULATION

### 6.1 The Drift Tick

Cross-reference npcs/npc_recurrence_system.md. When time passes (long voyages, chapter transitions, extended absence), regions drift without Jake:

**Drift factors per region (evaluated on tick):**
- **Momentum bias:** the phase's dominant trend (Beta favors instability, etc.)
- **Adjacency pressure:** strong neighbors bleed influence into weak regions
- **Supply health:** intact supply lines slowly raise E; cut lines slowly lower it
- **Crimson hunger:** CT drifts toward Regional Instability (Section 2.2)
- **Standing forces:** garrisons slowly recover M if not actively suppressed

### 6.2 Drift Rate

**MVP CORE:** Drift is slow and gentle (+/-2-5 per meter per chapter transition). The world changes noticeably but not chaotically. Jake's active choices dwarf passive drift.

**FULL VISION:** Drift responds to a lightweight regional AI — empires reinforce their most-threatened valuable colonies, abandon unprofitable ones (Albion's profit-retreat logic), and the Alliance spreads along connected networks. The map feels genuinely alive.

### 6.3 The Return Shock

A deliberate design goal: Jake returns from a multi-chapter absence or a long voyage arc to find the board CHANGED. A colony he weakened has fallen to the rival. A region he ignored is now Crimson-preyed. An Alliance network he seeded has grown. This is the world proving it lives — the strategic mirror of the demographic changes in regional_populations.md Section 11.

---

## 7. THE CHAPTER 10 WORLD STATE

### 7.1 The Final Map Computation

At the transition into the endgame's final act, the archipelago's state locks into one of several regional configurations based on cumulative player influence. Each region resolves to a final banner:

- **Imperial Ascendant** — the old empire held or grew
- **Albion Ascendant** — the company won the archipelago
- **The Free Archipelago** — Liberation succeeded broadly
- **The Contested Ruin** — Vacuum/Crimson zones dominate (the tragic outcome)
- **The Divided Sea** — a patchwork (the realistic outcome for most players)

### 7.2 What the Final Map Affects

- **Endgame ambient world:** which flags, which mood, which peoples thriving or suffering
- **The four sages' final states** (cross-reference colonial_war_overview.md Section 10.3): Mbah Seruni's grief or peace, Mama Jacinta's liberated or lost land, etc.
- **Available endgame content:** liberated regions unlock final Alliance content; imperial regions unlock final establishment content
- **The Crimson Armada's final strength:** more Vacuum zones = stronger final antagonist (the war feeds the climax)
- **Epilogue framing:** the world Jake leaves behind

### 7.3 No "Correct" Ending

Design law: no configuration is mechanically "the win." The Free Archipelago is the Protector Path's dream but costs the Empire-path rewards. The Divided Sea is neither triumph nor tragedy. Even The Contested Ruin is a valid (if dark) reflection of a player who broke everything and built nothing. The Style Bible's refusal of tidy closure (the_named_dead.md) applies at civilization scale.

---

## 8. THE PLAYER-FACING LAYER

### 8.1 How Jake Reads the War (No Spreadsheet)

The player never sees IC/AC/LS/CT numbers directly. The war is read through:

**The War Map (Full Vision):** A stylized, in-world map (in Jake's cabin, cross-reference luxury_merchants.md cartographer content) showing regional banners, contested markers, and rough momentum arrows — hand-drawn aesthetic, updated as Jake learns intel. Reads like a period naval chart, not a strategy-game overlay.

**The World Itself:** Flags, patrols, prices, building states, ambient dialogue, population mood. A perceptive player reads the war by sailing through it.

**Intel Reports:** Information brokers (Whisper Lady, Cass Wells, Salazar — cross-reference) sell "the state of things" — narrative summaries of regional conditions, rumors of coming Falls, warnings of retaliation.

### 8.2 Objective Clarity

While the SIMULATION is hidden, Jake's ACTIONS are clear. When Jake takes a "Raid the Convoy" quest, the game communicates the intent (*"This will hurt the Empire's grip on Puerto de la Corona"*) without exposing the -5 E value. Players understand consequences directionally, feel them experientially.

---

## 9. INTEGRATION MAP

### 9.1 Systems That Read the War State

- **colony_system_core.md** -> Support/Break/Liberate actions write to these meters
- **retaliation_system.md** -> retaliation intensity scales with damage dealt to each faction
- **castle_sieges.md** -> sieges are the decisive Fall triggers
- **castle_ownership.md** -> held castles project Control for their faction (or Jake)
- **colonial_war_bosses.md** -> General defeats are the largest single Control swings
- **regional_populations.md** -> ambient dialogue Layer 2 reads region state; demographics reflect it
- **building_evolution.md** -> building states reflect economic health of the controlling faction
- **npc_recurrence_system.md** -> passive drift, state persistence, the Return Shock
- **bosses/crimson_armada.md** -> CT mechanics feed the main antagonist's strength

### 9.2 The Feedback Loop with the Main Plot

The war is not a side-system. Vacuum zones strengthen the Crimson Armada. Stage 5 retaliation grants Crimson tolerance. The Chapter 10 world state sets the final antagonist's power. **Jake's colonial choices literally determine how hard the main story's climax is.** The war and the plot are one machine.

---

## 10. TECHNICAL NOTES

### 10.1 Data Model (Per Region)

```
Region {
  id, name, current_dominant_power, current_state
  imperial_control: { M, E, P } -> IC
  albion_control:   { M, E, P } -> AC
  liberation_strength: LS
  crimson_threat: CT
  building_states[] (ref building_evolution)
  active_general (ref boss)
  connected_networks[] (for LS multiplier)
  persistent_flags[] (Fell, Liberated, Vacuum, etc.)
}
```

### 10.2 Tick Timing

- **Active changes:** immediate (Jake completes an action -> meter updates -> world responds)
- **Passive drift:** evaluated on chapter transitions and long-voyage completions
- **Threshold checks:** evaluated after every meter change
- **Phase gates:** story-triggered

### 10.3 Save/Persistence

Full war state serialized in save. The Return Shock requires the world to have advanced correctly during absence — drift is computed on load if time-skips occurred.

### 10.4 Balance Tuning Levers

All values in Section 3 are first-draft. Key tuning levers: action impact magnitudes, drift rate, threshold values (30/40/50/60), retaliation escalation speed, LS earn rate. The Vacuum's severity and the LS-50 Liberation threshold are the two most narratively-load-bearing numbers — tune with care.

---

## 11. MVP CORE vs FULL VISION

### 11.1 MVP CORE (Shippable Vertical Slice)

- Single Control meter per faction (no M/E/P decomposition)
- One contested region pair (e.g. Puerto de la Corona vs a liberation scenario)
- The full threshold ladder (Stable->Pressured->Crisis->Fall->Fork)
- All three Fork outcomes (Liberation/Vacuum/Rival)
- Gentle passive drift
- World-read layer (flags, prices, ambient)
- One General boss, one castle, the Fall set-piece

### 11.2 FULL VISION (The Dream)

- M/E/P decomposition (attack colonies three ways)
- All 9 regions, full four-power simulation
- Regional AI (empires reinforce/abandon intelligently)
- Network multiplier for the Alliance
- The in-cabin War Map
- Full momentum-phase escalation
- The complete Chapter 10 world-state computation
- Intel economy (brokers selling war state)

---

## 12. STYLE BIBLE COMPLIANCE

- **Pillar 1 (Staggered Register):** Region-state ambient dialogue shifts register with control — confident High under stable empire, desperate Low under Crisis, liberated Middle in free regions.
- **Pillar 2 (Lore Through Objects):** The war read through flags, tax forms, boarded windows, the in-cabin chart. The state is physical.
- **Pillar 3 (Layered Death):** The Fall carries Style D weight — flags lowered, generals fallen. The Vacuum is Style D devastation. War outcomes are never celebrated as mere score.
- **Pillar 4 (Layered Humor):** Ambient Style C in the war's absurdities — "same forms, new flag," sailors on the pointlessness of it all. Never mocking the suffering.
- **Pillar 5 (Theme — Revenge):** The engine IS the theme quantified. What Jake feeds grows; what he breaks and abandons rots. The meters are the mathematics of consequence.

---

## 13. NEXT STEPS

Next document: **`colonial_war/colony_system_core.md`** — Foundation Tier Document 2

Will cover:
- The Support/Break/Liberate action systems in full mechanical detail
- Transporting people, stuetzing supply lines, cutting them
- The colony development/decay loop (building_evolution integration)
- The moment-to-moment gameplay of feeding or bleeding a colony

After Colony Core: Retaliation System. Then the Colony Tier (Imperial, Albion, Liberation individual portraits).

---

## 14. CONCLUSION

The Engine of the Archipelago. Four meters per region — Imperial, Albion, Liberation, Crimson — none of them a number Jake ever sees, all of them the world he sails through. The balance where pushing one power down lifts another. The Vacuum that punishes destruction without construction. The threshold ladder from Stable to Crisis to the Fall to the Fork. The momentum phases that escalate the war in rhythm with the story. The passive drift that changes the map while Jake's away. And the Chapter 10 world state — the final map that is nothing more, and nothing less, than the memory of everything the player chose.

When a player returns from a long voyage to find a colony they weakened has fallen to the rival empire, when they realize breaking Fuerte San Rafael without arming Mbah Seruni's people just fed the Crimson Armada, when they watch the flags change across the archipelago in the endgame and understand that map is their own reflection — that's when the war stops being a system. That's when it becomes the world's response to who Jake chose to be.

**Phase 7 Subsystem 2 of 16 — complete.**

**The meters move. The regions shift. The map remembers. The engine runs.**
