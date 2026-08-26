# MOUNTED TRAVEL — THE HORSE

**Status:** Design Draft v1.0
**Phase:** Phase 8 — Highmoore (Movement Two — Document 6 of ~30)
**Related:** highmoore_overview.md, highmoore_region.md, princess_quest_the_lake.md, princess_quest_arion_falls.md, physics/movement_physics.md, physics/weather_physics.md, physics/wind_systems.md, physics/sound_physics.md, physics/combat_physics.md, physics/cloth_hair_physics.md, fauna/land_animals.md, mechanics/combat_feel.md, npcs/named_crew_deep_dives.md, style_bible.md
**Purpose:** The mounted travel system — the horse as a relationship rather than a vehicle, acquisition and bonding, gaits and handling, terrain and weather interaction, mounted combat, the two questline rides that the entire system exists to serve, and death.
**Inspirations:** Red Dead Redemption 2 (the horse bond, and it is the reference point throughout), Shadow of the Colossus (Agro), The Last of Us Part II (the stable sequence), Ghost of Tsushima (mounted traversal feel), Kingdom Come: Deliverance (horse as animal, not vehicle), real equestrian handling

---

## 1. PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: The Horse Is Not a Vehicle

**Cross-reference:** highmoore_overview.md Pillar 4.

Dark Arisen's traversal has been a ship — a thing with systems, a crew, and physics, that Jake commands. Cross-reference physics/ship_physics.md.

**The horse is the opposite.** It is one animal, it has moods, it gets tired, it gets frightened, and it can decide things. Cross-reference fauna/land_animals.md — Highmoore's horse is built on the Phase 4 animal-behaviour foundation, not on a movement-system foundation.

### 1.2 Principle 2: It Is Introduced Once and It Matters

The horse arrives at the bottom of the switchback (cross-reference highmoore_region.md Section 1.5) and it is the only way to cross Highmoore. **There is no fast travel in Highmoore.** Cross-reference Section 6.4.

### 1.3 Principle 3: Bonding Is Earned, Not Purchased

Cross-reference Section 3. The bond is built through use, care, and time. It cannot be bought and it does not scale with doubloons — cross-reference colonial_war/indigenous_liberation_alliance.md Principle 1.2 for the same design law applied elsewhere.

### 1.4 Principle 4: The System Exists for Two Rides

**Cross-reference:** princess_quest_the_lake.md, princess_quest_arion_falls.md.

Everything in this document is infrastructure for two sequences: **the ride to the lake with her**, and **the ride to Arion afterward.** The first is slow and the second is the fastest Jake ever moves.

**Design intent:** the mechanics are built so that those two rides feel like different acts performed by the same animal.

### 1.5 Principle 5: It Can Die

Cross-reference Section 9. Permanently, and cross-reference npcs/the_named_dead.md — the game treats it with the same restraint it treats people.

---

## 2. ACQUISITION

### 2.1 The First Horse

At the bottom of the Crystal Caves switchback there is a **derelict staging post** — the Highmoore end of the mining charter's operation, abandoned with the mine. Cross-reference crystal_caves.md Section 3.1.

There is one horse in the paddock. It is not wild and it is not tame. Somebody has been feeding it.

**Design note:** who has been feeding a horse at an abandoned staging post for sixty years is a question the game does not answer. Cross-reference highmoore_overview.md Section 4.3.

### 2.2 The First Approach

Cross-reference fauna/land_animals.md — the Phase 4 animal-approach mechanics apply. Slow, no sudden movement, and cross-reference physics/sound_physics.md: **the horse hears Jake before it sees him.**

The first mount is not automatic. It takes three attempts minimum, and cross-reference Section 3.1 — **the bond starts at zero and the first ride is genuinely awkward.**

### 2.3 Later Horses

Cross-reference the_lesser_houses.md, highmoore_settlements.md. Horses can be bought, won, gifted, and taken. Highmoore's Houses breed distinct lines (Section 4.4).

**Design law:** the first horse is not special mechanically. **It is special because it was first**, and that is the player's business, not the game's.

---

## 3. THE BOND

**Cross-reference:** the RDR2 reference throughout.

### 3.1 The Bond Levels

Four levels, and each unlocks handling rather than stats:

**LEVEL 0 — UNKNOWN**
Skittish. Slow to respond. Refuses gaps, water, and fire. Bolts under fire. Cross-reference Section 5.3.

**LEVEL 1 — FAMILIAR**
Responds reliably. Will cross water. Still bolts from combat.

**LEVEL 2 — TRUSTED**
Holds under fire (cross-reference Section 7). Will jump gaps. Comes when whistled from distance.

**LEVEL 3 — BONDED**
Anticipates. Cross-reference Section 5.2 — at Level 3 the horse takes lines through terrain the player did not input, and they are correct.

### 3.2 How the Bond Grows

- **Riding it** — distance, over time
- **Care** — brushing, feeding, and cross-reference highmoore_settlements.md: stabling it rather than leaving it standing
- **Calming it** — under fire, in storms, near predators. Cross-reference fauna/land_animals.md
- **Not running it into the ground** — cross-reference Section 5.4

### 3.3 How the Bond Breaks

- Sustained hard riding to exhaustion
- Abandoning it
- **Cross-reference Section 9.2** — riding it into a fight it cannot survive

---

## 4. THE ANIMAL

### 4.1 Stats

Cross-reference mechanics/combat_feel.md for Jake's baseline.

| Property | Range | Note |
|---|---|---|
| Stamina | 100-180 | depletes with gait and terrain |
| Health | 150-300 | |
| Speed | varies by line | Section 4.4 |
| Handling | varies by line + bond | |
| Courage | 1-5 | how much it tolerates before bolting |

### 4.2 Stamina

**Cross-reference:** physics/movement_physics.md.

The horse's stamina is the system's central resource. Gallop drains it fast, canter slowly, trot barely, walk not at all.

**It recovers with rest, water, and feed** — and cross-reference Section 5.4: **exhausting a horse has consequences beyond the ride.**

### 4.3 Mood

Cross-reference fauna/land_animals.md. The horse has a visible state — ears, head carriage, gait quality — and cross-reference physics/cloth_hair_physics.md: **mane and tail behaviour communicate it.**

**Skittish, calm, tired, frightened, and content.** No UI element. Cross-reference highmoore_region.md Section 6.2 — the player reads the animal.

### 4.4 The Highmoore Lines

Cross-reference house_system.md — the Houses breed distinctly, and a rider who knows horses can identify a House by its mounts at distance.

- **Arion greys** — tall, fast, beautiful, and delicate. Cross-reference house_arion.md.
- **Belos blacks** — heavy, brave, and slow to frighten. Cross-reference house_belos.md.
- **Moor ponies** — small, ugly, indestructible, and they know the bogs. Cross-reference Section 5.5.
- **The staging-post horse** — cross-reference Section 2.1. Nobody can identify its line, including the Houses.

---

## 5. HANDLING

### 5.1 The Gaits

**Walk / Trot / Canter / Gallop**, and cross-reference physics/movement_physics.md — each has distinct camera behaviour, sound, and stamina cost.

**Design note:** cross-reference physics/sound_physics.md — hoofbeat on the four Highmoore surfaces (road, moor, bog, stone) is distinct enough that a player can hear what they are riding on with their eyes closed.

### 5.2 The Line

At Bond Level 3, the horse takes lines the player did not steer — around a rock, over a beck, through a gap in a wall.

**Design law:** cross-reference the RDR2 reference. **The player should stop steering in familiar terrain** and the horse should be right. That feeling is the entire point of the bond system.

### 5.3 Bolting

Cross-reference fauna/land_animals.md, physics/sound_physics.md.

At low bond and low courage, the horse bolts from: gunfire, fire, large predators, and cross-reference Section 7 — **arrows specifically**, which matters enormously in Highmoore.

Bolting is not a fail state. **It is the horse deciding**, and the player rides it out or comes off.

### 5.4 Exhaustion

Riding to zero stamina causes:
- Refusal to move above a walk for an extended period
- Bond loss
- **Cross-reference Section 9.3** — sustained repeated exhaustion damages the animal permanently

### 5.5 Terrain

**Cross-reference:** physics/movement_physics.md, highmoore_region.md Section 2.3.

| Terrain | Effect |
|---|---|
| Road | full speed, low stamina cost |
| Open moor | full speed, moderate cost, **hidden hazards** (rabbit holes, cross-reference fauna) |
| Heather | slowed, high cost |
| Bog | **dangerous.** Cross-reference physics/water_physics.md — a horse can go down in the Waterlands and not come up |
| Stone | fast, and cross-reference physics/movement_physics.md: slick when wet |
| Snow | cross-reference physics/snow_ice_physics.md — Highmoore gets winter |

**The moor ponies do not sink.** Cross-reference Section 4.4 — they know where the ground is, and a Level 3 bond with a moor pony makes the Waterlands navigable in a way nothing else does.

---

## 6. TRAVERSAL AND THE ABSENCE OF FAST TRAVEL

### 6.1 The Rule

**There is no fast travel in Highmoore.** Cross-reference highmoore_overview.md Pillar 4, highmoore_region.md Section 2.1 — twenty-five minutes end to end at a canter.

### 6.2 Why

Cross-reference the RDR2 reference and princess_quest_the_lake.md. **The questline's emotional structure depends on distance being real.** If Arion is a menu selection then the ride back after the letter is nothing, and the ride to the lake is nothing, and the ride to Arion afterward is nothing.

### 6.3 What Mitigates It

- **The road network** — cross-reference highmoore_region.md Section 2.4: roads are genuinely faster
- **Stabling** — cross-reference highmoore_settlements.md: a horse left at a stable can be whistled up at any stable
- **The gaits** — long-distance riding is done at a canter and it is comfortable

### 6.4 The Long Ride as Content

Cross-reference npcs/named_crew_deep_dives.md — **the crew are not in Highmoore**, and the long rides are the loneliest sustained stretches in Dark Arisen.

**Design note:** Highmoore's soundscape (cross-reference physics/wind_systems.md, sound_physics.md) has to carry twenty-five-minute rides. Wind, hooves, birds, weather, and distant bells. Cross-reference highmoore_region.md Section 1.3.

---

## 7. MOUNTED COMBAT

**Cross-reference:** physics/combat_physics.md, mechanics/combat_feel.md.

### 7.1 The Position

Jake can fight mounted, and he is **worse at it than he is on foot** — cross-reference his background: he is a sailor. Cross-reference characters/jake_harlow.md.

- Reduced attack precision
- No posture system while mounted. Cross-reference the Sekiro pillar — **mounted combat is not the game's good combat**, deliberately
- Speed and charge damage are the only advantages

### 7.2 Against Archers

**Cross-reference:** highmoore_region.md Section 7.1, house_belos.md.

Highmoore's dominant military technology is the bow, and cross-reference physics/wind_systems.md and physics/combat_physics.md — **arrows in wind at range against a moving horse.**

A mounted Jake against a Belos archer line is a bad idea, and the game does not warn him.

### 7.3 The Crystal Katana Mounted

Cross-reference crystal_katana.md Section 4.2 — 74cm, medium reach, from horseback, against spears.

**It works, barely, and only at speed.** Cross-reference princess_quest_arion_falls.md — the ten bodyguards in the Arion courtyard are fought **dismounted**, and the game makes Jake get off the horse.

### 7.4 The Horse in a Fight

At Bond 2+, it holds. At Bond 3, it **fights** — cross-reference fauna/land_animals.md: it kicks, it barges, and it puts itself between Jake and things.

**Cross-reference Section 9.2.** That is how it dies.

---

## 8. THE TWO RIDES

**Cross-reference:** Principle 1.4.

### 8.1 THE RIDE TO THE LAKE

**Cross-reference:** princess_quest_the_lake.md.

Slow. Two horses. Cross-reference house_arion.md — she rides an Arion grey and she rides it well, and Jake does not.

**Mechanically:** the game caps the gait. There is no galloping to the lake. **It takes as long as it takes**, and the conversation happens over it, and cross-reference physics/weather_physics.md — the rain comes in from the west across the moor while they ride and they both watch it.

**Design law:** the player cannot skip this ride, cannot speed it up, and cross-reference Section 6.1 — cannot fast travel to the lake. **It is nine minutes.**

### 8.2 THE RIDE TO ARION

**Cross-reference:** princess_quest_arion_falls.md.

Cross-reference the user's own framing: *"er reitet schneller als jeder."*

**Mechanically:** the stamina system is suspended. The horse does not tire, the gallop does not end, and cross-reference crystal_katana.md Section 1.2 — **it is the same design idea as the sword.** The animal will run until it kills itself and Jake does not slow down.

**Design note:** the ride is roughly four minutes and it is one unbroken gallop across the Folds, and cross-reference physics/weather_physics.md — it is still raining.

**And at the end of it the horse is finished.** Cross-reference Section 9.3 — it survives, and it is never the same animal again, and the game does not comment.

---

## 9. DEATH

### 9.1 The Rule

**Cross-reference:** npcs/the_named_dead.md, style_bible.md Pillar 3.

Horses die permanently. There is no revive, no respawn, and no replacement of *that* horse.

### 9.2 How

- Combat — cross-reference Section 7.4: at Bond 3 it puts itself between Jake and things, and it can be killed doing that
- Falls, bogs, and cross-reference Section 5.5
- Arrows. Cross-reference house_belos.md.

### 9.3 The Belos Sequence

**Cross-reference:** princess_quest_belos.md.

Jake rides to House Belos. **Cross-reference Section 8.2 — the horse arrives finished.**

The design does not kill it in the courtyard for pathos. It survives. It stands where he left it, blown, shaking, for nine minutes, and it is there when he comes back out, and cross-reference princess_quest_the_return.md — **it carries him back through the Crystal Caves at a walk and that takes a very long time.**

### 9.4 The Handling

Cross-reference style_bible.md Pillar 3. If it dies, it dies the way people die in Dark Arisen: quietly, without a music sting, and the camera holds too long.

**No burial mechanic. No memorial option.** Cross-reference npcs/the_named_dead.md Section 2.3 — the game offers no closure.

---

## 10. INTEGRATION MAP

- **highmoore_region.md** ← the horse at the switchback; the twenty-five-minute crossing; the terrain
- **princess_quest_the_lake.md** → the capped nine-minute ride
- **princess_quest_arion_falls.md** → the suspended-stamina gallop
- **princess_quest_belos.md** → the horse standing blown outside for nine minutes
- **princess_quest_the_return.md** → the walk back through the Caves
- **fauna/land_animals.md** ← the animal-behaviour foundation, approach mechanics, mood
- **physics/movement_physics.md** ← gaits, terrain, footing
- **physics/wind_systems.md, weather_physics.md** ← the rain coming in from the west
- **physics/sound_physics.md** ← four surfaces, four hoofbeats; the twenty-five-minute soundscape
- **physics/cloth_hair_physics.md** ← mane and tail as mood communication
- **physics/combat_physics.md, mechanics/combat_feel.md** ← mounted combat's deliberate inferiority
- **crystal_katana.md** ← medium reach from horseback; the same design idea as the suspended stamina
- **house_system.md, house_arion.md, house_belos.md** ← the breeding lines

---

## 11. MVP CORE vs FULL VISION

### 11.1 MVP CORE

- The staging-post horse and the three-attempt first mount
- Four bond levels with handling unlocks
- Four gaits with stamina, and the four-surface hoofbeat audio
- Terrain effects including bog danger
- Bolting, and calming
- Mounted combat's reduced capability
- **Both questline rides** — the capped one and the suspended one
- Permanent death

### 11.2 FULL VISION

- The Highmoore breeding lines and House identification at distance
- Bond 3 line-taking (the player stops steering)
- Moor ponies and the Waterlands
- Full mood communication through mane, ears, and carriage
- The stable-whistle network
- Bond 3 combat behaviour, and what it costs
- The Belos aftermath: the horse that survives and is never the same

---

## 12. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** Highmoore's yeomanry talk about horses constantly and in a register the archipelago has no equivalent for. A stablehand's assessment of Jake's riding is one of the best available sources of Highmoore ambient comedy.
- ✅ **Pillar 2 (Lore Through Objects):** a horse in a paddock at an abandoned staging post that somebody has been feeding for sixty years, and the game never says who. The Houses' breeding lines readable at distance.
- ✅ **Pillar 3 (Layered Death):** permanent, quiet, no sting, camera holds too long, no burial mechanic, no closure. And the one that does not die — standing blown outside House Belos for nine minutes and never the same afterward.
- ✅ **Pillar 4 (Layered Humor):** the first mount taking three attempts. Jake, a sailor, riding badly next to a woman who rides beautifully, in the middle of the most romantic sequence in the game. Cross-reference princess_quest_the_lake.md — **she notices, and she is kind about it, and that is the joke.**
- ✅ **Pillar 5 (Theme — Revenge):** the ride to Arion is the theme in one mechanic. The stamina system suspends. The animal will run until it kills itself. Jake does not slow down. **It is the same design as the sword that does not tire, and it is on purpose, and the player rides both of them into the same courtyard.**

---

## 13. NEXT STEPS

Next batch: **`house_system.md`**, **`house_arion.md`**, **`house_belos.md`**, **`the_lesser_houses.md`**, **`highmoore_settlements.md`**, **`the_princess.md`**.

---

## 14. CONCLUSION

The Horse. One animal in a paddock at an abandoned staging post that somebody has been feeding for sixty years, which takes three attempts to mount and does not like Jake at all. Twenty-five minutes end to end across Highmoore with no fast travel, because if Arion is a menu selection then none of what happens there means anything.

It has moods you read in its ears and its mane. It bolts from arrows because it is an animal. It sinks in the Waterlands unless it is a moor pony, which knows where the ground is. At the highest bond it takes lines you did not steer and they are correct, and it puts itself between you and things, and that is how it dies.

And it exists for two rides: nine minutes to a lake at a capped gait with the rain coming in from the west while a woman who rides beautifully is kind about the fact that you don't — and four minutes afterward, at a flat gallop, with the stamina system switched off, because the animal will run until it kills itself and Jake does not slow down.

It is standing outside House Belos for nine minutes, blown and shaking, and it is still there when he comes out, and it carries him back through the mountain at a walk.

**Phase 8 Document 6 of ~30.**

**No fast travel. That's the whole point.** 🐎🌾
