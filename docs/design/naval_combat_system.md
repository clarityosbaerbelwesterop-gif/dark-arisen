# NAVAL COMBAT SYSTEM

**Status:** Design Draft v1.0  
**Related:** region_05_open_sea_overview.md, region_05_fort_carrion.md, combat_system.md, controls_ps5.md, style_bible.md  
**Purpose:** Complete mechanical specification for ship-to-ship combat in Dark Arisen, centered on Chapter 8 (Region 05) but usable across the entire game from Chapter 4 onward.  
**Inspirations:** Assassin's Creed IV: Black Flag (the gold standard), Sid Meier's Pirates!, Sea of Thieves (boarding), Master and Commander (realism layer), Sekiro (deck-based melee during boarding)

---

## 1. DESIGN PHILOSOPHY

Naval combat in Dark Arisen is not a mini-game. It is a core pillar of gameplay, equal in importance to sword combat, stealth, and exploration. From Chapter 4 onward — when Jake acquires La Liberación in Rexa — the ship is a character in the story, and naval engagements are not interludes between "real" content but central to the player's experience.

**The core design pillars:**

1. **Tactical, not arcade.** Positioning matters more than reflexes. Wind direction, sail configuration, relative angle between ships, and crew quality determine outcomes more than raw player dexterity.

2. **Slow but lethal.** Ships turn slowly. Cannons reload slowly. A single bad positioning decision can doom an entire engagement. The player must think in 30-second windows, not 3-second reactions.

3. **Crew-dependent.** La Liberación is nothing without her crew. The player commands them. They execute. A captain who treats his crew poorly (through story choices or neglect) will find his ship suffers in combat — slower reloads, worse accuracy, fewer boarding volunteers.

4. **Consequential damage.** Damage is visible and persistent. A shredded sail must be repaired. A breached hull takes on water. A wounded crew cannot fight at full strength in the next engagement. Dark Arisen does not hide the cost of war at sea.

5. **Boarding is the heart.** Ship-to-ship cannon exchanges are impressive and visceral, but the true pirate experience is in the boarding action — crossing planks, cutlasses drawn, fighting foot-by-foot across an enemy deck. Naval combat always culminates in either a boarding action or an enemy's destruction, and boarding is almost always the more rewarding path.

---

## 2. THE SHIP — LA LIBERACIÓN

The player's ship, La Liberación, is acquired in Chapter 4 (Rexa). She is a modified brig — originally a Crimson Armada patrol vessel captured and refitted by Don Mateo Salazar's network, then claimed by Jake after Salazar's death. Throughout Chapters 4-9, La Liberación evolves based on player choices.

### 2.1 Baseline Stats (Chapter 4 Acquisition)

| Stat | Value |
|------|-------|
| Hull Integrity | 600 HP |
| Speed (Max) | 12 knots |
| Turning Rate | 5° per second |
| Cannon Count | 14 (7 per side) |
| Crew Capacity | 40 |
| Starting Crew | 20 |
| Sail Configuration Options | 3 (Full, Half, Reefed) |
| Ammunition Capacity | 80 rounds |
| Powder Capacity | 8 barrels |

### 2.2 Upgrade Paths

La Liberación has four major upgrade categories. Each can be purchased at specific shipyards throughout the game (Rexa docks, Graavig, Ironmouth). Full upgrades are required for the Fort Carrion assault.

**Hull Upgrades:**
- **Iron Plating (Tier 1):** +150 HP, -1 knot speed. Cost: 8 Gold Bars.
- **Composite Hull (Tier 2):** +300 HP total, -1.5 knots speed. Cost: 15 Gold Bars.
- **Reinforced Keel (Tier 3):** +500 HP total, -2 knots speed, +50% resistance to ramming damage. Cost: 25 Gold Bars. (Required for Fort Carrion chain-ramming in Chapter 8.)

**Cannon Upgrades:**
- **Extended Cannon Arrays (Tier 1):** +6 cannons (3 per side), total 20. Cost: 12 Gold Bars.
- **Heavy Cannons (Tier 2):** Existing cannons replaced with heavier versions, +40% damage per shot, -20% reload speed. Cost: 18 Gold Bars.
- **Swivel Guns (Tier 3):** +4 anti-personnel swivel guns on the deck for boarding defense. Cost: 10 Gold Bars.

**Sail Upgrades:**
- **Improved Rigging (Tier 1):** +2 knots max speed, +2° turning rate. Cost: 6 Gold Bars.
- **Storm Sails (Tier 2):** Enables safe navigation in heavy weather, +1 knot average speed in storms. Cost: 10 Gold Bars.
- **Topgallants (Tier 3):** +3 knots max speed, total max of 15+ knots with full upgrades. Cost: 14 Gold Bars.

**Crew Quarters Upgrades:**
- **Expanded Crew Quarters (Tier 1):** Crew capacity +20 (to 60). Cost: 8 Gold Bars.
- **Surgeon's Bay (Tier 2):** Father Salvio gets a dedicated medical facility, crew recovery after engagements is faster. Cost: 10 Gold Bars.
- **Reinforced Quarters (Tier 3):** Crew morale bonus during extended operations, fewer desertions, better combat performance. Cost: 12 Gold Bars.

**Total cost for full upgrades: ~158 Gold Bars.** This is deliberately demanding — the player must engage with Region 5's economic systems (naval combat loot, salvage, side missions) to afford it. A speedrunning player cannot skip the grind; Fort Carrion cannot be assaulted on a stock vessel.

### 2.3 Final Upgraded Stats (Required for Fort Carrion)

| Stat | Value |
|------|-------|
| Hull Integrity | 1,100 HP |
| Speed (Max) | 15 knots |
| Turning Rate | 7° per second |
| Cannon Count | 24 (11 per side + 2 chase guns) |
| Crew Capacity | 60 |
| Ammunition Capacity | 160 rounds |
| Powder Capacity | 16 barrels |
| Ramming Damage Dealt | 400 |
| Ramming Resistance | +50% |

---

## 3. CONTROL SCHEME (PS5)

### 3.1 Helm View

The default naval combat view. Jake stands at the helm of La Liberación.

| Button | Action |
|--------|--------|
| L Stick | Steer ship (yaw left/right) |
| R Stick | Camera control |
| L2 | Spyglass (zoom to survey enemy) |
| R2 | Fire signal flares / give fleet commands |
| L1 | Cycle sail configuration (Full/Half/Reefed) |
| R1 | Tack (quick turn against wind) |
| Square | Fire port-side broadside |
| Circle | Fire starboard-side broadside |
| Triangle | Switch to Deck View |
| Cross | Call rigging command (climb, rescue, etc.) |
| D-Pad Up | Issue Order: Reload All Cannons |
| D-Pad Down | Issue Order: Prepare to Board |
| D-Pad Left | Issue Order: Evasive Maneuvers |
| D-Pad Right | Issue Order: Ramming Speed |
| Options | Pause / Menu |
| Touchpad | Ship Status Overview |

### 3.2 Deck View

Jake walks the deck of La Liberación, can operate specific cannons personally, direct crew, or prepare for boarding.

| Button | Action |
|--------|--------|
| L Stick | Move Jake |
| R Stick | Camera |
| L2 | Aim |
| R2 | Fire (if at cannon or with firearm) |
| L1 | Weapon Wheel |
| R1 | Interact / Enter cannon station |
| Square | Light attack (if enemies on deck) |
| Cross | Jump / Climb rigging |
| Circle | Dodge |
| Triangle | Return to Helm View |
| D-Pad Up | Issue Crew Order |
| D-Pad Down | Rally crew to Jake's position |
| D-Pad Left | Retreat order |
| D-Pad Right | Charge order (for boarding) |

### 3.3 Crow's Nest View

Elevated observation from the top of the mainmast. Used for long-range spotting and tactical assessment.

| Button | Action |
|--------|--------|
| R Stick | Look around |
| L2 | Spyglass (even higher zoom than helm) |
| R2 | Mark targets for crew awareness |
| Triangle | Return to Deck |

---

## 4. COMBAT MECHANICS

### 4.1 Positioning and Wind

Wind direction is displayed as a blue arrow on the HUD. La Liberación's speed varies dramatically based on her angle to the wind:

- **Wind at back (running):** Maximum speed, 100%.
- **Wind at 45° (broad reach):** 90% speed.
- **Wind at 90° (beam reach):** 70% speed.
- **Wind at 135° (close reach):** 50% speed.
- **Wind at 180° (in irons):** Near-stationary, ship drifts backward.

Smart captains use wind strategically — positioning their ship to take the wind while denying it to enemies. This is the highest-skill layer of naval combat.

### 4.2 Broadside Cannons

Each side of La Liberación has its own set of cannons (upgradeable to 11 per side at max). Broadsides fire all cannons on one side simultaneously. The effect:

- **Damage:** Base 200 HP per broadside (upgrades multiply this).
- **Spread:** Cannons on the front of the ship hit the enemy's bow; cannons on the back hit the stern. A broadside from mid-range creates a wide damage pattern.
- **Reload Time:** 12 seconds (Chapter 4 baseline), improvable to 8 seconds with crew upgrades.
- **Range:** Maximum effective range is ~200 meters. Beyond this, damage and accuracy drop sharply.

### 4.3 Ammunition Types

The player selects ammunition type before firing (via quick-select menu, D-Pad Up):

**Round Shot** — Standard iron cannonball. Hull damage. Default choice.

**Chain Shot** — Two iron balls connected by chain. Damages rigging, reduces enemy ship's speed by 30% for 60 seconds. Minimal hull damage. Situational.

**Grape Shot** — Multiple smaller balls. Devastating anti-personnel effect. Clears enemy decks, kills 5-15 enemy crew per broadside. Almost no hull damage. Used immediately before boarding.

**Hot Shot** — Heated iron balls. Hull damage + 30% chance to set fire. Fires spread over 90 seconds, causing ongoing damage. Expensive (requires furnace, limited ammo).

**Explosive Shells** — Late-game ammunition (only available from the Pale Isle onward, either looted or purchased). Hull damage + area-of-effect. Most powerful single shot but extremely limited (8 shots max per engagement).

### 4.4 Ramming

La Liberación can ram enemy vessels by aiming directly at them and activating D-Pad Right (Ramming Speed).

- **Damage to Target:** 400-600 HP depending on upgrades.
- **Damage to La Liberación:** 200-400 HP (without Reinforced Keel; 100-200 with it).
- **Effect:** Ramming stuns the target ship for 10 seconds, during which time it cannot maneuver or fire. La Liberación is also slowed but still responsive.
- **Special Use:** Ramming is the ONLY way to break the iron chain blocking Fort Carrion's harbor in Chapter 8. The game forces a ramming sequence at that point.

### 4.5 Ship Damage and Repair

La Liberación has four separate damage systems:

**Hull HP** — Overall integrity. At 0 HP, the ship sinks.

**Sail Condition** — Damaged sails reduce speed. Repaired via crew climbing the rigging during combat (takes 60 seconds, leaves them vulnerable).

**Crew Losses** — Enemy broadsides and boarding can kill Jake's crew. Each loss weakens future performance. Dead crew do not respawn. Named crew members can die permanently in certain engagements (story-gated to prevent losing Mira or Big Tom in random combat).

**Fire Damage** — If hit with hot shot or explosive shells, fires spread across La Liberación. Crew must be ordered to fight the fire (diverting them from other tasks). Unattended fires damage hull over time.

### 4.6 Boarding Actions

Once enemy ship HP is below 30% or enemy crew is mostly dead (<30% remaining), Jake can initiate a boarding action:

1. **Approach:** Position La Liberación within 20 meters of the target. The game prompts "Initiate Boarding?" (D-Pad Down).
2. **Grappling:** Crew members throw grappling hooks. Enemy sailors may cut some free. Within 10 seconds, the two ships are locked together.
3. **Deck Combat:** Jake leads the boarding party across planks/rigging onto the enemy deck. The game switches to Deck View. Standard melee combat begins.
4. **Environmental Factors:** Fires on either ship spread across the linked decks. Grape-shot damage persists — enemy corpses and wounded are everywhere. Weather affects visibility.
5. **Victory Condition:** Either kill all enemies on the enemy deck (Jake's crew helps, acts autonomously) OR defeat the enemy captain in personal combat. Enemy captains are always named characters and function as mini-bosses.
6. **Resolution:** When the enemy deck is cleared, Jake can:
   - **Loot:** Claim cargo, gold, supplies. (30-120 seconds, Jake is vulnerable if other enemies appear.)
   - **Recruit:** Certain surviving enemy crew will join La Liberación if offered. Dialog check.
   - **Take the Ship:** Assign skeleton crew (minimum 10) to sail the captured vessel. La Liberación's crew is reduced accordingly. Useful for fleet-building scenarios.
   - **Scuttle:** Set fires, destroy the ship. No loot, but fast and clean. Prevents Crimson recovery of the vessel.

### 4.7 Crew Orders During Combat

Jake issues orders to his crew via the D-Pad during both Helm and Deck views. Available orders:

- **Reload All Cannons:** Crew focuses on reload, all hands on guns. Reload speed +30%, but no other tasks performed.
- **Prepare to Board:** Crew arms themselves and moves to the bulwarks. Grappling hooks ready.
- **Evasive Maneuvers:** Crew performs rigging adjustments to maximize turning rate. +30% turn speed, -20% forward speed.
- **Ramming Speed:** Crew sets maximum canvas, prepares for impact. +2 knots speed but vulnerable to enemy fire.
- **Repel Boarders:** Defensive stance. If an enemy ship tries to board La Liberación, this order maximizes defensive efficiency.
- **Fight the Fire:** Ordered when La Liberación takes fire damage. Diverts 10 crew from other tasks.
- **Rally to Me:** All crew converge on Jake's current position. Used during deck combat for concentrated strength.

---

## 5. THE KRAKEN ENCOUNTER

The Kraken is a unique set-piece encounter in Region 05, not a repeatable boss fight. It uses the naval combat system but introduces special mechanics.

### 5.1 Setup

Approximately 40% through Chapter 8, La Liberación enters the Obsidian Waters. Weather deteriorates. The sky turns black. The crew grows quiet. Mira reports unusual disturbances in the water — massive shapes moving beneath the surface.

Then the first tentacle rises.

### 5.2 Kraken Stats

The Kraken is not a ship; it is a creature. Its "health" is represented by 6 destroyable tentacles + 1 head/body target that emerges at the end of the encounter.

| Target | HP | Notes |
|--------|-----|-------|
| Tentacle 1-6 | 300 each | Individually destroyable |
| Kraken Head | 800 | Emerges only after all 6 tentacles are destroyed |

### 5.3 Phases

**Phase 1 — The First Grip (Tentacles 1-2):**  
Two tentacles rise from the water and wrap around La Liberación's hull. They attempt to crush the ship — dealing 50 HP per second to the hull while attached. The player must direct crew to cut them loose (melee attacks by Jake or crew) OR blast them with cannons. Each tentacle has 300 HP.

**Phase 2 — The Swarm (Tentacles 3-4):**  
Two more tentacles join. Now four are active. La Liberación takes 100 HP per second while all are attached. Crew must fight the fire of desperation. Cannons and melee must be coordinated. Jake can switch to Deck View to personally attack tentacles with his sword (dealing 40 HP per hit, faster than cannons but more dangerous).

**Phase 3 — The Full Assault (Tentacles 5-6):**  
All six tentacles are active. La Liberación is being crushed. The deck is flooded with seawater. Crew are screaming. The ship's HP is dropping fast. The player must commit fully — Jake personally fighting tentacles with his sword, crew firing cannons and cutting with axes, the entire ship fighting for its life.

**Phase 4 — The Emergence:**  
When all six tentacles are destroyed, the Kraken's massive head rises from the water directly in front of La Liberación. It opens its beak — enormous, black, ancient. The player has a brief moment to see it clearly. The Kraken emits a deafening roar (this is a distinct audio event that players will remember). Then it attacks directly.

The Kraken's head has 800 HP. It attacks by lunging at La Liberación with its beak (200 damage per attack) and by spraying ink that blinds the crew (reduces cannon accuracy by 80% for 30 seconds).

### 5.4 Resolution

The Kraken cannot be killed. After it takes 800 HP of damage (which may take 60-90 seconds of intense combat), it submerges. The water roils. The creature is gone.

La Liberación is battered but afloat. Hull HP is severely reduced. Several crew members are dead or wounded. The sea calms unnaturally fast, as though the creature took the storm with it.

Big Tom says his famous line — *"Captain. I want you to know. That experience. That experience right there. That was not in the brochure when I signed up."*

The chapter continues. But something has shifted in the crew. They are quieter now. Jake is quieter. The remaining approach to Fort Carrion feels smaller than it did before.

### 5.5 Optional: Finding the Kraken Again

If the player completes Chapter 8 and later revisits the Obsidian Waters (possible in Region 6, the post-game), they can encounter the Kraken a second time. This encounter is harder (scaled to end-game Jake) and has no story progression — it is a pure combat challenge for players seeking optional difficulty. Defeating the Kraken in this second encounter yields a unique reward: **The Kraken's Tooth**, a legendary weapon with unique stats and the only weapon in the game that can instantly kill any non-boss enemy on a critical hit.

---

## 6. TYPES OF ENEMIES

### 6.1 Patrol Vessels

- **Crew:** 20-40
- **Cannons:** 6-10 per side
- **Hull HP:** 400-600
- **Speed:** 11-13 knots
- **Strategy:** Fast, numerous, sent in pairs or trios. Best approach: destroy sails to slow them, then board systematically.

### 6.2 Merchant Raiders

- **Crew:** 60-80
- **Cannons:** 12-14 per side
- **Hull HP:** 800-1000
- **Speed:** 10-12 knots
- **Strategy:** Tougher than patrol vessels, typically carrying valuable cargo. Often escorted by patrol ships. Prioritize the escort first.

### 6.3 Ships of the Line

- **Crew:** 200-300
- **Cannons:** 40-60 total
- **Hull HP:** 2500-3500
- **Speed:** 8-10 knots
- **Strategy:** These are mini-boss encounters. Only 3-4 exist in Region 5. Each one is named and has a named captain. Fighting a Ship of the Line without full La Liberación upgrades is suicide.

### 6.4 Named Crimson Flagships

Specific, unique vessels belonging to notable Crimson Armada captains. Each has custom stats, unique combat patterns, and its own narrative weight.

**HMS Blacktide** (Captain Fitzmueller) — 3,200 HP, 50 cannons, 280 crew. Encountered in the Obsidian Waters approach to Fort Carrion. See boss spec.

**HMS Crimson Verdict** (Captain Draven Voss, nominally) — Draven's personal flagship. Not a conventional boss fight; the ship is the setting of the Ethan + Draven boss fights in Chapter 9. Stats irrelevant — the ship is narrative, not mechanical.

Other flagships exist but are optional encounters in the Crimson Sea (*HMS Diamond Dragon*, *HMS Midnight Wake*, *HMS Ironclad Serpent*). Each is an optional naval boss with its own captain and story.

### 6.5 The Kraken

As described in Section 5. Unique, unrepeatable set-piece.

---

## 7. WEATHER AND ENVIRONMENTAL SYSTEMS

### 7.1 Weather Types

**Clear:** Ideal conditions. 100% visibility, normal speed.  
**Overcast:** 80% visibility, -1 knot speed.  
**Fog:** 40% visibility, -2 knots speed. Enemy ships may surprise Jake.  
**Rain:** 70% visibility, -1 knot speed, cannon accuracy -20%.  
**Storm:** 30% visibility, -3 knots speed (with Storm Sails: -1 knot), -40% cannon accuracy. Rogue waves may deal 50 HP damage per hit to hulls. Dangerous but can be exploited for surprise attacks.  
**Hurricane:** Exists only in specific scripted moments. Forces ships to port or sink. Jake cannot normally engage in combat during hurricanes.

### 7.2 Day/Night

Combat is possible at any time, but night operations have significant advantages and drawbacks:

- **Advantage:** Visibility is low for both sides. Enemy patrol awareness reduced by 50%.
- **Disadvantage:** Jake's cannon accuracy reduced by 25%.
- **Special:** Night approaches to Fort Carrion (in Chapter 8) are significantly easier in terms of stealth but mechanically harder in terms of combat.

### 7.3 Time of Day Tactical Use

The player can time approaches strategically. Jake can dock at Ironmouth, sleep until nightfall, then sail out to engage patrols in the dark — reducing enemy awareness while accepting accuracy penalties. This is a valid tactical choice, not a gimmick.

---

## 8. THE CREW AS COMBAT MULTIPLIER

La Liberación's named crew members are not decorative. Each one contributes specific stats and abilities during naval combat.

### 8.1 Mira (Navigator)

**Role:** Steers La Liberación when Jake is in Deck View. Acts as Jake's voice at the helm.  
**Passive:** +1° to base turning rate.  
**Active Ability:** "Navigator's Instinct" — once per engagement, Mira can identify the fastest route to optimal wind positioning. Activation reduces time-to-favorable-wind by 50%.  
**Story Interaction:** If Mira is present (always, unless player kills her in a story choice), she calls out wind changes and enemy movements in real-time dialogue during combat.

### 8.2 Big Tom (Master Gunner)

**Role:** Operates the heaviest cannon personally. Coordinates cannon crews.  
**Passive:** +15% damage on all broadsides.  
**Active Ability:** "Tom's Signature Shot" — once per engagement, Big Tom fires his personal cannon with precision, dealing 600 damage to a single chosen target (player selects via quick-menu).  
**Story Interaction:** Names his cannon after a different woman every engagement. Jake hears this naming ritual before each battle.

### 8.3 Esteban "El Viejo" (Quartermaster)

**Role:** Coordinates crew assignments. Manages the ship's internal logistics.  
**Passive:** +20% reload speed for all cannons.  
**Active Ability:** "Everyone to Stations" — one-time per engagement, resets all cannon reloads to instant. Powerful game-changer in critical moments.  
**Story Interaction:** Barks orders in Rexan Spanish-accented common tongue during combat. Adds authenticity.

### 8.4 Father Salvio (Ship's Surgeon)

**Role:** Medical care for wounded crew during and after combat.  
**Passive:** Reduces crew deaths during combat by 25%. Wounded crew can return to action faster.  
**Active Ability:** "The Blessing of the Condemned" — once per engagement, Salvio performs rites over La Liberación's dying crew. Doubles the surviving crew's effective performance for 30 seconds (they fight with desperate courage).  
**Story Interaction:** Prays over enemy dead after combat. Asks Jake to consider what the Crimson Armada's sailors believed before they died.

### 8.5 Ines (Bosun)

**Role:** Manages rigging and sail configuration. Communicates with topmen.  
**Passive:** +2 knots to max speed. Sail repair time reduced by 40%.  
**Active Ability:** "Running Rigging" — once per engagement, Ines coordinates a lightning-fast sail adjustment. La Liberación gains +5 knots for 20 seconds.  
**Story Interaction:** Screams at the topmen in colorful colonial-era sailor's language. Her curses are one of Dark Arisen's small comedic delights.

---

## 9. ECONOMIC LOOP — LOOT AND UPGRADES

Naval combat generates significant loot, which is the primary path to funding La Liberación's upgrades.

### 9.1 Typical Loot Per Engagement

| Enemy Type | Gold Bar Value | Other Loot |
|------------|----------------|------------|
| Patrol Vessel | 0.5-1.5 Gold Bars | 20-50 Doubloons, ammunition, minor crafting materials |
| Merchant Raider | 2-5 Gold Bars | Cargo specific to route (wine, silk, spices), 100-300 Doubloons |
| Ship of the Line | 5-12 Gold Bars | Large amounts of imperial cargo, advanced weapons, 500-1500 Doubloons |
| Named Flagship | 8-20 Gold Bars | Unique item rewards, significant Doubloons, optional story progress |

### 9.2 Optional: Prize Crews

If Jake captures an enemy ship (rather than scuttling it), he can assign a prize crew to sail it to the nearest friendly port. The ship is sold at port for additional gold (roughly equal to the ship's construction cost, typically 4-15 Gold Bars depending on class). This is the most lucrative but most complex option — it requires Jake to permanently reduce his own crew size until he resupplies.

### 9.3 Full Upgrade Economics

To fully upgrade La Liberación (~158 Gold Bars required), the player must engage in sustained naval combat across Region 5. A rough guideline:

- **Minimum engagements:** ~40 successful actions
- **Typical engagements:** ~60-80 actions including side content
- **Time investment:** 8-12 hours of dedicated naval play

This ensures that by the time Jake approaches Fort Carrion, he has mastered the naval combat system through practice — not through tutorials.

---

## 10. STORY-CRITICAL NAVAL MOMENTS

Certain naval encounters are mandatory story beats:

- **Chapter 4 — The First Battle:** Jake's first naval engagement with La Liberación. Tutorial-level combat. Forced. Teaches basic mechanics.
- **Chapter 5 — The Ashenmoor Approach:** Naval traversal through volcanic waters. Limited combat but introduces hazardous navigation.
- **Chapter 6 — The Crimson Intercept:** First major Crimson Armada engagement. Forced boarding action. Teaches advanced tactics.
- **Chapter 8 — The Kraken:** Unique set-piece described in Section 5.
- **Chapter 8 — Fort Carrion Approach:** Naval combat against the HMS Blacktide + patrol screen. One of the hardest engagements in the game.
- **Chapter 8 — Harbor Breach:** The forced ramming sequence through Fort Carrion's chain barrier.
- **Chapter 9 — The Ramming:** Final naval action. La Liberación rams the Crimson Verdict. Non-combat cinematic sequence; culminates in the transition to the Ethan fight.

---

## 11. STYLE BIBLE COMPLIANCE CHECK

- ✅ **Pillar 1 (Staggered Register):** Crew dialogue during combat follows each character's established register — Big Tom's Low Register with quirky humor, Father Salvio's Middle Register with quiet gravity, Mira's Low Register practical communications, Ines's Low Register sailor curses. No one becomes poetic during naval combat except in designed narrative moments (the Kraken's silence, the Fort Carrion approach cinematics).

- ✅ **Pillar 2 (Lore Through Objects):** The ship itself is a lore object. Jake's cabin preserves items across Chapters 4-9 that reflect his journey. Captured ship items (imperial logs, crew personal effects, letters from Crimson sailors to their families) provide ongoing lore without explicit dumps. The Kraken's Tooth (if recovered) is a physical trophy of the unkillable encounter.

- ✅ **Pillar 3 (Layered Death):** Enemy crew die in Style C (ritualistic combat flow) — hundreds die in large engagements and this is accepted as the nature of naval warfare. Named enemy captains die in Style B (brutal realism) with appropriate weight. La Liberación's own crew, if they die, are mourned — Father Salvio's post-combat prayers emphasize the cost. The Kraken cannot die, which fits Style D (the tragic, the mythic).

- ✅ **Pillar 4 (Layered Humor):** Big Tom's Pillar 4 Style C humor is the primary humor source in naval combat — his cannon-naming ritual, his dry observations during and after engagements. Ines's sailor curses add Pillar 4 Style C flavor. The serious characters (Jake, Mira, Salvio) maintain their restraint.

- ✅ **Pillar 5 (Theme — Revenge):** Naval combat is where Jake's revenge scales up from personal to institutional. Each engagement forces the player to sink ships, kill crews, loot cargo — actions that define what kind of pirate captain Jake is becoming. The game does not moralize, but the cost accumulates. Father Salvio's prayers are Dark Arisen's quietest voice against becoming what Draven became. Jake can listen or not. The player decides.

---

## 12. FINAL NOTE

Naval combat is the system through which Dark Arisen's second half earns its scale. Without robust ship-to-ship combat, Region 5 would be a walking simulator between scripted events. With this system, it becomes a genuine pirate experience — tactical, demanding, rewarding, and morally weighted.

The mechanics described here are intentionally complex. Players who engage deeply will find satisfaction in mastery. Players who prefer melee can largely bypass naval combat by sticking to minimum-required engagements and relying on story-gated set-pieces. Both playstyles are valid.

What is non-negotiable: naval combat must SERVE the story, not distract from it. Every engagement should feel like it matters to Jake's arc, not like filler content between land-based adventures. The designers of Dark Arisen must resist the temptation to pad Chapter 8 with meaningless naval encounters simply to justify the system. Every battle should be a choice or a consequence. Every kill should cost something, even if that cost is only Father Salvio's silent prayer afterward.

Next: Region 06 — The Maw (post-game content).
