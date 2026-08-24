# CASTLE SIEGES — TAKING AND HOLDING

**Status:** Design Draft v1.0  
**Phase:** Phase 7 — Colonial War, Castles & Dungeons (Castle Tier Document 2 — Document 9 of 16)  
**Tier:** Castle Tier  
**Related:** colonial_war_overview.md, colonial_war/castle_catalog.md, colonial_war/castle_ownership.md, colonial_war/war_state_system.md, colonial_war/colony_system_core.md, colonial_war/retaliation_system.md, colonial_war/indigenous_liberation_alliance.md, colonial_war/colonial_war_bosses.md, physics/ship_physics.md, physics/destruction_physics.md, physics/fire_physics.md, npcs/building_evolution.md, style_bible.md  
**Purpose:** Define the complete siege gameplay loop — the four phases, the three-door approaches in mechanical detail, naval bombardment, defensive sieges when Jake's own holdings are attacked, the raze-or-take-intact decision, state treasure recovery under fire, and Liberation Event battle integration.  
**Inspirations:** Mount & Blade (siege loop), Assassin's Creed Odyssey (conquest battles), Total War (investment and assault phases), Dishonored (multiple approaches), Elden Ring (fortress infiltration), real Vauban siegecraft and Age of Sail shore bombardment

---

## 1. SIEGE PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: The Siege Is Won Before the Assault

**Cross-reference:** war_state_system.md Section 4.4 (Crisis), colony_system_core.md

A castle at full Control with intact supply lines and a fed garrison cannot be stormed — not by one captain and a crew. The siege begins chapters earlier: convoys raided, magazines sabotaged, officers eliminated, unrest spread.

**The assault is the last five percent of the work.** A player who sails up to La Ciudadela and attacks the walls deserves what happens next.

**Design intent:** Strategic preparation rewarded; brute force punished.

### 1.2 Principle 2: Three Doors, Three Different Games

**Cross-reference:** castle_catalog.md Section 1.2

The Wall is a battle. The Back is an infiltration. The Inside is a conspiracy. Each is a distinct gameplay experience with its own pacing, skills, and rewards — and each castle supports at least three.

**Design intent:** The siege system serves combat players, stealth players, and strategic players equally.

### 1.3 Principle 3: The Ship Is a Siege Engine

**Cross-reference:** physics/ship_physics.md, physics/destruction_physics.md

La Liberación is Jake's artillery train. Coastal fortresses can be bombarded, breached, and suppressed from the water. Naval integration is not a bonus layer — for most castles it is the primary means of breaching walls no boarding party could climb.

**Design intent:** The pirate captain fights sieges as a captain, not as an infantry commander.

### 1.4 Principle 4: Holding Is Harder Than Taking

**Cross-reference:** retaliation_system.md Section 3.4

The siege system runs in both directions. Every castle Jake takes becomes a castle Jake must hold, and the empires' counter-attacks are the same system with the roles reversed. **Ownership is a promise to defend.**

**Design intent:** Conquest with consequence; the defensive siege as its own content pillar.

### 1.5 Principle 5: What You Break, You Own Broken

**Cross-reference:** physics/destruction_physics.md, npcs/building_evolution.md

Every breach persists. A castle stormed with fire and shot becomes Jake's ruin if he keeps it. The raze-or-take-intact decision (Section 8) is the siege system's central strategic tension, and it is answered with cannons before it is answered with intent.

**Design intent:** Destruction has an afterlife.

---

## 2. THE FOUR SIEGE PHASES

### 2.1 The Loop

Every siege moves through four phases. Phases 1-2 may span chapters; phases 3-4 are a single session's set-piece.
1.	APPROACH    — intelligence, scouting, door selection
	2.	INVESTMENT  — isolation, weakening, preparation
	3.	BREACH      — the way in: wall, back, or inside
	4.	ASSAULT     — the interior, the treasure, the commander

### 2.2 PHASE 1 — APPROACH

**Objective:** learn the castle.

**Intelligence sources:**
- **Reconnaissance in person** — sail past, walk the town, observe patrol patterns (cross-reference npcs/regional_populations.md ambient dialogue as intel)
- **Information brokers** — the Whisper Lady, Cass Wells, Helga Brennan, Capitán Salazar (cross-reference npcs/black_market_fences.md, imperial_contacts.md, recurring_quest_givers.md)
- **Alliance knowledge** — Trust Tier 3+ networks give what no broker sells: the undercity, the mountain paths, the tidal windows
- **Insider contacts** — a bribed officer, a sympathetic clerk, a disaffected overseer

**What Approach reveals:**
- Garrison strength and rotation schedule
- Supply line status and the castle's stores
- **Which doors are currently viable** (doors open and close based on war state)
- The commander's current disposition
- State treasure location within the structure

**Design note:** a player who skips Approach can still siege — blind. The game does not block it. It simply becomes much harder, and the state treasure may be missed entirely.

### 2.3 PHASE 2 — INVESTMENT

**Objective:** make the castle takeable.

This is where the war-state work pays off. Investment actions (cross-reference colony_system_core.md Section 2.2, war_state_system.md Section 3.1):

**Isolation:**
- Cut or blockade supply lines (the castle eats its stores)
- Destroy or divert reinforcement routes
- Eliminate the relief force (cross-reference imperial_colonies.md Section 8.2 — mutual reinforcement)

**Weakening:**
- Sabotage magazines (fewer guns firing during the assault)
- Sabotage water (San Rafael's cisterns — decisive)
- Eliminate senior officers (garrison coordination degrades)
- Spread unrest (the garrison watches the town instead of the walls)

**Preparation:**
- Position Alliance forces (if LS ≥ 50 — cross-reference indigenous_liberation_alliance.md Section 6)
- Stage supplies and powder
- Secure a landing point or infiltration entry

**The Siege Readiness Read:**

Rather than a percentage bar, Investment progress reads through the world: fewer patrols, thinner smoke from the kitchens, sentries who look tired, a commander's proclamation nailed to a door. Cross-reference war_state_system.md Section 8.1 — **the player reads the castle, not a meter.**

### 2.4 PHASE 3 — BREACH

**Objective:** get inside.

This is the door selection made real. Full mechanics in Sections 3-6.

### 2.5 PHASE 4 — ASSAULT

**Objective:** the interior, the treasure, the commander.

**Structure:**
- Fighting through the castle's districts (cross-reference castle_catalog.md — each fortress's districts are the assault's level design)
- **Treasure recovery** under time pressure (Section 9)
- The commander encounter (cross-reference colonial_war_bosses.md Tier 2 for castle commanders, Tier 1 where the colony General is present)
- Resolution: the flag, the garrison's surrender or rout, and the raze-or-hold decision

---

## 3. DOOR ONE — THE WALL

### 3.1 The Combat Approach

Frontal assault: bombardment, breach, and storm. The loudest, fastest, and most destructive route.

### 3.2 Naval Bombardment

**Cross-reference:** physics/ship_physics.md, physics/destruction_physics.md

**Applicable to:** coastal castles — La Ciudadela, Castillo Dorado, the Sterling Bastion, Coventry House, Fort Resolute, Puesto del Norte, and San Rafael's seaward face.

**The bombardment loop:**
1. **Position** — enter the engagement envelope; shore batteries return fire (this is a naval combat encounter, not a cutscene)
2. **Suppress** — target the batteries first, or take sustained damage
3. **Breach** — concentrate fire on a wall section until it fails (cross-reference destruction_physics.md — persistent, visible, and it stays broken)
4. **Land** — boats away, the breach is the entry

**Ammunition matters:**
- **Round shot** — wall damage, slow, effective
- **Chain shot** — cross-reference physics/ship_physics.md Section 5.4: anti-rigging at sea, and against a fort it strips flagstaffs and signal masts (morale, coordination)
- **Hot shot** — cross-reference physics/fire_physics.md Section 7.2: sets interiors ablaze. **Devastating, and it burns state treasures** (Section 9.4). The fastest breach and the costliest.

**Design note:** hot shot into a magazine is the single most spectacular breach in Phase 7 and destroys everything in that wing. The choice is real.

### 3.3 The Escalade (Non-Coastal / No Breach)

Where bombardment is impossible — Ashcroft Hall inland, or a player without naval superiority — the wall is climbed:
- Ladders and grapples under fire
- Cross-reference physics/movement_physics.md Section 4 (climbing) and physics/combat_physics.md
- Casualties are real: crew and Alliance fighters can die here (cross-reference npcs/the_named_dead.md)

### 3.4 The Wall's Cost

- **Maximum destruction** — the breach persists, ownership means owning a ruin
- **Maximum casualties** — Jake's crew, Alliance fighters, garrison, and civilians in the blast radius
- **Maximum retaliation** — cross-reference retaliation_system.md: a stormed castle spikes Heat harder than any other approach
- **Treasure risk** — fire and collapse destroy documents (Section 9.4)

### 3.5 The Wall's Reward

- **Speed.** Investment requirements are lower; a well-armed captain can force a weakly-held fortress
- **Plunder.** Razing yields maximum immediate loot (Section 8.2)
- **Reputation.** Cross-reference npcs/regional_populations.md — the Feared Path is built on stormed walls

---

## 4. DOOR TWO — THE BACK

### 4.1 The Infiltration Approach

Enter unseen, take what matters, and either open the gate from inside or leave with the objective and no one the wiser.

### 4.2 Entry Routes by Castle

**Cross-reference:** castle_catalog.md for each fortress's specific back door

- **La Ciudadela** — the undercity (Alliance Trust Tier 3, Cultural Web)
- **Coventry House** — the night counting halls (the signature stealth sequence)
- **Ashcroft Hall** — the strongroom heist
- **Fuerte San Rafael** — the mountain paths (Alliance Trust Tier 3, Mountain Paths)
- **The Drowned Hold** and other tidal sites — the tide window (cross-reference physics/water_physics.md)
- **Castillo Dorado** — barely needed; the front door is purchasable

### 4.3 Infiltration Mechanics

**Cross-reference:** mechanics/stealth.md, physics/sound_physics.md Section 12

- Patrol patterns learned in Approach
- Sound propagation matters (the counting halls' pen-scratch silence makes every footfall a risk)
- Light and shadow (cross-reference physics/sun_shadow_systems.md — night infiltration, lantern discipline)
- Non-lethal and lethal options both viable
- **Discovery does not fail the mission.** It converts it — the back door becomes a running fight toward the wall or the gate.

### 4.4 The Infiltration Objectives

Infiltration can achieve any of:
- **The Treasure** — take the document and leave. Cross-reference Section 9.3: a pure heist, no siege required
- **The Gate** — open it from inside; the assault begins with the walls already breached
- **The Sabotage** — magazine, cistern, or records; weaken and withdraw for a later assault
- **The Commander** — assassination, avoiding the general battle entirely (cross-reference colonial_war_bosses.md: several commanders have infiltration-specific encounter variants)

### 4.5 The Back's Cost

- **Slow.** Requires Approach intelligence, often Alliance trust, and patience
- **Fragile.** One bad alarm and the advantage is gone
- **Smaller haul.** No general plunder — you take what you can carry

### 4.6 The Back's Reward

- **The castle survives intact** (ownership option preserved — Section 8.3)
- **Minimal casualties** on all sides
- **Lower retaliation Heat** than storming
- **The treasure guaranteed** — documents taken by hand are not burned by hot shot
- Cross-reference npcs/named_crew_deep_dives.md — Father Salvio notices when a fortress changes hands without a massacre

---

## 5. DOOR THREE — THE INSIDE

### 5.1 The Conspiracy Approach

The castle opens because someone inside opens it. The most varied and most strategically satisfying route.

### 5.2 The Five Interior Doors

**PURCHASE (Castillo Dorado, and partially New Coventry)**  
Cross-reference imperial_colonies.md Section 6.11. Bribe the gate rotation, buy the vault combination, purchase the governor's cooperation. **The Empire's most corrupt holding taken by the Empire's own methods.**

**UPRISING (Fuerte Esperanza, Ashcroft Hall)**  
Cross-reference indigenous_liberation_alliance.md Section 6.3. The people inside outnumber the garrison. Free enough convoys, expose enough ledgers, arm enough hands — and the gate opens from the inside because the inside decided.

**Timing is the mechanic.** Cross-reference imperial_colonies.md Section 4.11: at Fuerte Esperanza the guns face the fields, and the instant the villages rise, Jake's job is to silence those guns before they fire. **Arrive late and the liberation is a massacre Jake caused.**

**DEFECTION (Fuerte San Rafael, potentially Port Sterling)**  
Cross-reference imperial_colonies.md Section 5.3 (Cruz's list), krone_colonies.md Section 5.4 (Sterling's honour). Officers with consciences can be reached — with evidence, not gold.

**PAPERWORK (Fort Resolute, New Coventry)**  
Cross-reference krone_colonies.md Sections 3 and 7.9. Drive Profitability below the threshold, or expose Thorne's dispatches, and the Company withdraws. **The garrison boards transports and Jake walks in.** The only door that requires no violence at all.

**BETRAYAL (situational)**  
An overseer who wants out. A quartermaster in debt to Mateusz. A clerk with a grievance. Cross-reference npcs/black_market_fences.md — the Whisper Lady sells names of the reachable.

### 5.3 The Inside's Cost

- **The longest lead time in Phase 7.** Uprisings need chapters of Liberation work; profitability collapse needs sustained economic warfare; defection needs evidence found elsewhere
- **Dependency.** Jake is not in control of the timing; the inside decides when
- **Moral exposure** — an uprising Jake sparked and mistimed is his to answer for

### 5.4 The Inside's Reward

- **The castle intact, the garrison often surrendering, the treasure preserved**
- **The strongest liberation outcomes** — a fortress taken from within belongs to the people who opened it
- **Lowest retaliation Heat** (the empire's response to withdrawal or defection is bureaucratic, not vengeful)
- Cross-reference indigenous_liberation_alliance.md Section 6.4 — the aftermath scene is strongest here, because Jake genuinely wasn't the one who took it

---

## 6. THE LIBERATION EVENT SIEGE

### 6.1 The Special Case

**Cross-reference:** war_state_system.md Section 4.6 Path A, indigenous_liberation_alliance.md Section 6

When a castle falls with Regional LS ≥ 50, the Liberation Event fires: a second battle, immediately, in which the Alliance takes and holds what fell.

### 6.2 What Changes Mechanically

- **Jake receives objectives, not authority** (cross-reference indigenous_liberation_alliance.md Principle 5)
- **Alliance fighters are a real force** — they hold flanks, take objectives, and die
- **Multiple simultaneous objectives** the player cannot personally cover (the network handles what Jake doesn't)
- **Named losses** — cross-reference the_named_dead.md: semi-named NPCs Jake knows can fall here, permanently
- **The aftermath scene** — the network's leadership addresses their own people, and the camera does not find Jake

### 6.3 The Five Battle Characters

Cross-reference indigenous_liberation_alliance.md Section 6.3 for full descriptions:

- **Corona — "The Quarter Rises"** — urban, three-dimensional, the undercity and the bell tower
- **Nueva Esperanza — "The Arithmetic Resolves"** — a timing puzzle disguised as a battle
- **San Rafael — "Thirty Years of Stillness"** — minimal combat, maximum weight
- **Blackwood Landing — "Void for Impossibility"** — a heist that becomes a bonfire
- **The Northern Front — "Both Flags Down"** — two campaigns, naval-heavy, patient

---

## 7. DEFENSIVE SIEGES — HOLDING WHAT JAKE TOOK

### 7.1 The System Reversed

**Cross-reference:** retaliation_system.md Section 3.4 (Stage 3 Counter-Attack), colony_system_core.md Section 6.3

At Retaliation Heat 60+, the empires come for Jake's holdings. The siege system runs backward: Jake is inside, the fleet is outside, and the four phases belong to someone else.

### 7.2 The Warning

**Cross-reference:** retaliation_system.md Section 6.1

Defensive sieges are telegraphed. Intelligence — bought from brokers or earned from Alliance networks — gives Jake advance notice:

*"An Imperial squadron was seen provisioning at Corona. Heavy transports. They're not hunting ships, Captain. They're carrying siege guns."*

**The warning window is the preparation window.**

### 7.3 Defensive Preparation

**Cross-reference:** castle_ownership.md (full fortification systems)

- **Garrison** — hire, arm, and train defenders; Alliance bastions garrison themselves
- **Repair** — a castle taken by bombardment has breaches; unrepaired breaches are where they come in
- **Stores** — food, powder, water. A besieged castle eats.
- **Guns** — restore or improve the batteries
- **The Fleet** — La Liberación outside the walls is worth more than inside them (Section 7.5)

### 7.4 The Defensive Loop

1.	WARNING     — intelligence, preparation window
	2.	ARRIVAL     — the enemy invests; supply cut; the world outside the walls goes quiet
	3.	BOMBARDMENT — their guns work on the walls; Jake’s guns answer; breaches form
	4.	ASSAULT     — they come through; the interior fight; hold or lose
 
**Holding requires surviving the assault.** Losing means the castle is retaken — and cross-reference physics/destruction_physics.md: whatever Jake built there is damaged or gone.

### 7.5 The Naval Relief Option

**The most interesting defensive choice:** Jake does not have to be inside.

**Cross-reference:** physics/ship_physics.md

La Liberación can attack the besieging fleet from outside — cutting their supply, sinking their siege transports, breaking the blockade. **The castle holds if the siege fleet breaks.**

This makes defensive sieges a genuine naval-vs-land choice:
- **Inside:** Jake's presence stiffens the defense; the interior fight is winnable with him
- **Outside:** the garrison holds alone (risky) while Jake dismantles the besiegers at sea

**Design note:** the Outside option is thematically correct — Jake is a captain, not a castellan — and mechanically riskier. Cross-reference castle_ownership.md: a well-garrisoned castle can survive without him. A neglected one cannot.

### 7.6 The Impossible Choice

**Cross-reference:** retaliation_system.md Section 3.4

At Stage 3-5 with multiple holdings, the empires strike more than one place at once. Jake cannot defend everything.

**What is lost is lost** — cross-reference physics/destruction_physics.md, npcs/npc_recurrence_system.md: the burned settlement, the retaken castle, the people who were there. Cross-reference npcs/the_named_dead.md.

### 7.7 Alliance Bastions Defend Themselves

**Cross-reference:** indigenous_liberation_alliance.md Section 5.7

Reclaimed Old Fortresses garrison themselves — free, loyal, culturally rooted. They resist reconquest without Jake's presence.

**This is a major Liberation-path advantage:** the Empire path builds holdings Jake must personally defend. The Liberation path builds holdings that hold themselves.

---

## 8. RAZE OR TAKE INTACT — THE CENTRAL DECISION

### 8.1 The Fork

**Cross-reference:** colonial_war_overview.md Section 8.3

At the moment of victory, every siege presents the same question, and the answer was partly determined by *how Jake got in*.

### 8.2 RAZE

**What it means:** strip everything portable, burn what isn't, pull down what will fall.

**Rewards:**
- **Maximum immediate plunder** — the treasury, the stores, the fittings, everything
- **Permanent Control denial** — the faction cannot re-garrison a ruin; cross-reference war_state_system.md, this is the surest way to keep a region from flipping back
- **No defensive obligation** — nothing to hold, nothing to lose to retaliation
- **Speed** — done in one session, no ongoing investment

**Costs:**
- **The state treasure may burn** (Section 9.4) — documents do not survive razing well
- **No ownership** — no trade post, no settlement, no bastion
- **Reputation** — cross-reference npcs/regional_populations.md: razing builds the Feared Path
- **Cultural cost** — razing a cathedral-fortress, or an Old Fortress, or Fuerte San Rafael, has consequences with the sages and the networks

### 8.3 TAKE INTACT

**What it means:** the garrison surrenders or routs, the structure stands, the flag changes.

**Rewards:**
- **Ownership** — cross-reference castle_ownership.md: trade post, settlement, stronghold, or Alliance bastion
- **The treasure preserved** — documents intact and usable
- **Control projection** — a held castle projects Control for its holder (war_state_system.md)
- **Infrastructure** — everything the empire built, now Jake's

**Costs:**
- **Less immediate plunder** — you don't strip a building you intend to use
- **The defensive obligation** (Section 7) — it becomes a target
- **Investment required** — repairs, garrison, stores, upkeep
- **Damage inherited** — cross-reference Principle 5: a castle stormed with hot shot is intact only in the technical sense

### 8.4 THE THIRD OPTION — GIVE IT AWAY

**Cross-reference:** indigenous_liberation_alliance.md Section 5.7, imperial_colonies.md Section 5.12

Jake can hand a taken castle to the Alliance, to a liberated community, or — in specific cases — leave it deliberately empty.

**Examples established elsewhere:**
- **Fuerte San Rafael** — left standing and empty as a marker; the Alliance's choice, and insisting on holding it costs Mountain Paths trust
- **Fuerte Esperanza** — the villages may ask Jake to pull it down, because the guns face their homes
- **Puesto del Norte and Fort Resolute** — both left empty, watching each other across the strait

**Design note:** giving a castle away yields no mechanical reward beyond LS and trust. **That is the point.** Cross-reference indigenous_liberation_alliance.md Principle 5.

### 8.5 The Decision Is Made With Cannons

The elegant tension: **by the time Jake reaches the fork, his approach has already half-answered it.**

- Hot shot and a breached wall → razing is barely a choice
- A night infiltration and a bloodless gate → intact is the obvious outcome
- An uprising → the people inside may have opinions about what happens to the building

**Design intent:** strategic coherence. The player who wants a trade post learns to take castles gently.

---

## 9. STATE TREASURE RECOVERY

### 9.1 The Objective Within the Objective

**Cross-reference:** castle_catalog.md Section 13.2 (the document web)

Every castle's state treasure is a distinct objective with its own location, protection, and failure state. Nine documents and instruments across the archipelago, and **most of them burn.**

### 9.2 Recovery Under Fire

During Assault, treasures are on a **timer of circumstance**:
- Fires spread (cross-reference physics/fire_physics.md Section 4 — wind-driven, realistic, and the archive vaults are full of paper)
- Defenders destroy documents rather than surrender them (Herrera absolutely will; de Silva absolutely will not — he'll sell them)
- Structural collapse (destruction_physics.md) buries vaults
- Commanders flee with them — cross-reference krone_colonies.md Section 4.3: **Blackwood's entire boss encounter is him leaving with the Master Ledger**

### 9.3 The Heist Alternative

**Cross-reference:** Section 4.4

Several treasures can be taken by infiltration alone, without any siege:
- Ashcroft Hall's contracts (the strongroom heist)
- New Coventry's Master Ledger (the counting halls at night)
- San Rafael's 1846 reports (the mountain paths and the records room)
- Castillo Dorado's ledgers (or simply buy them)

**Design note:** the heist route is the *safest* way to get the documents and the *only* way to get some of them without burning the rest.

### 9.4 What Fire Destroys

**Cross-reference:** physics/fire_physics.md

Hot shot, spreading fires, and razing destroy paper. Permanently.

**The consequences are real and unmitigated:**
- The Conquest Archives burn → the barrio's two-century oral record is never confirmed in the Empire's own hand; the plaza reading (imperial_colonies.md Section 3.11) does not happen
- The Labor Ledgers burn → **freed families cannot find each other.** Cross-reference imperial_colonies.md Section 4.6.
- The 1846 reports burn → Mbah Seruni never learns the officer's name, and Ibarra grows orchids undisturbed
- The Contracts burn → this one is *fine*, and the game should let players notice the asymmetry

**Design intent:** Style Bible Pillar 2 with teeth. The lore is loot, and loot is destructible, and some of it mattered to people.

### 9.5 The Nine-Treasure Collection

**Cross-reference:** castle_catalog.md Section 13.2

A player who recovers all nine has the complete written record of two empires' conduct. **Full Vision late-game content:** what that collection is worth, and to whom — Kessler, Sterling, the Recovery Underground, or the archipelago itself.

**And it requires taking most castles gently**, which is itself a statement about how the war was fought.

---

## 10. SIEGE ECONOMY AND SCALE

### 10.1 Costs

Sieging is expensive:
- Powder and shot (bombardment consumes ammunition)
- Ship repair (shore batteries hit hard — physics/ship_physics.md)
- Crew casualties and recruitment
- Alliance supplies, if Alliance forces participate
- Time (investment spans chapters)

### 10.2 Returns

- **Raze:** immediate massive plunder; treasury, stores, fittings
- **Intact:** lower immediate return, ongoing income via ownership (castle_ownership.md)
- **Control swing:** cross-reference war_state_system.md Section 3.1 — a castle taken is the single largest Control shift available

### 10.3 Scale by Castle

Cross-reference castle_catalog.md Section 13.1:

- **Approachable** (San Rafael, Fort Resolute, Puesto del Norte, Castillo Dorado): a session's work with modest preparation
- **Moderate** (Fuerte Esperanza, Ashcroft Hall, Coventry House): real investment, real preparation
- **Hard** (the Sterling Bastion): a naval campaign in itself
- **Apex** (La Ciudadela): chapters of investment, Alliance support, and every door working at once

---

## 11. MVP CORE vs FULL VISION

### 11.1 MVP CORE

- The four-phase loop, fully functional
- All three doors on **three castles**: Fuerte Esperanza (uprising + wall), Ashcroft Hall (heist + uprising), Fuerte San Rafael (cisterns + mountain paths)
- Naval bombardment with round shot and hot shot
- The raze-or-intact fork with real consequences
- One defensive siege (retaliation Stage 3 against a Jake holding)
- State treasure recovery with fire destruction

### 11.2 FULL VISION

- All nine colonial castles plus the Old Fortresses
- All five interior-door types including the paperwork/withdrawal route
- The Liberation Event sieges with full Alliance battle AI
- The naval relief option for defensive sieges
- The impossible-choice simultaneous defensive sieges
- The nine-treasure collection and its endgame consequences
- Commander infiltration-variant encounters

---

## 12. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** Surrender negotiations, garrison shouts, and Alliance battle-calls in their own registers. Herrera's terms are High and absolute; Reyes's are Middle and reasonable; de Silva's are an auction.
- ✅ **Pillar 2 (Lore Through Objects):** State treasure recovery IS the pillar mechanized. The documents burn, and what burns is history that mattered to specific people.
- ✅ **Pillar 3 (Layered Death):** Named losses in Liberation Events. Cruz's defeat. Vega on his unfinished wall. Style D throughout — no castle falls as a trophy, and the aftermath scenes are quiet.
- ✅ **Pillar 4 (Layered Humor):** Castillo Dorado's garrison discovering they are not soldiers. A withdrawal siege where the enemy simply files out. Crew commentary on gardens sited atop firing platforms.
- ✅ **Pillar 5 (Theme — Revenge):** The raze-or-intact fork is the theme in miniature, asked at every fortress: do you burn it down, or do you build something in it? And the third option — give it away — is the answer the game respects most and rewards least.

---

## 13. NEXT STEPS

Next document: **`colonial_war/castle_ownership.md`** — Castle Tier Document 3 (FINAL Castle Tier)

Will cover:
- The four ownership types (Trade Post, Settlement, Stronghold, Alliance Bastion)
- Building evolution applied to Jake's own holdings
- Population, plantations, workshops, and production
- Fortification and garrison systems
- Income, upkeep, and the settlement network economy
- The crew and family integration — who lives in Jake's castles

After Ownership: the Boss Tier (War Bosses, Assassin Network, Assassin Questline).

---

## 14. CONCLUSION

Taking and Holding. Four phases that begin chapters before the walls are touched — the approach that reads the fortress, the investment that starves it, the breach that opens it, and the assault that finishes it. Three doors into every castle: the wall with its hot shot and its persistent breaches, the back with its tide windows and counting-hall silences, and the inside, where a purchase, an uprising, a defection, or a well-placed dispatch does what cannons could not.

And then the harder half. The empires come back. The siege system reverses and Jake stands on a wall he breached, watching transports carry siege guns toward a castle he now has to keep — or takes his ship out to break the besiegers at sea, because he was always a captain and never a castellan.

At the end of every siege, the same question, half-answered already by how he got in: burn it, keep it, or hand it to the people who opened the gate. The first is loudest. The second is heaviest. The third pays nothing at all.

**Phase 7 Subsystem 9 of 16 — complete.**

**The guns speak. The walls fall. The flags change. And then the holding begins.** ⚔️🏰
