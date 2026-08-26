# DUNGEON DESIGN PHILOSOPHY — THE PLACES UNDERNEATH

**Status:** Design Draft v1.0
**Phase:** Phase 7 — Colonial War, Castles & Dungeons (Dungeon Tier Document 1 — Document 14 of 16)
**Tier:** Dungeon Tier
**Related:** colonial_war_overview.md, colonial_war/regional_dungeon_catalog.md, colonial_war/secret_bosses.md, colonial_war/indigenous_liberation_alliance.md, physics/vegetation_physics.md, physics/water_physics.md, physics/lava_heat_physics.md, physics/snow_ice_physics.md, physics/sand_desert_physics.md, physics/sound_physics.md, physics/sun_shadow_systems.md, mechanics/combat_feel.md, style_bible.md
**Purpose:** Define the design standard for all Phase 7 dungeons — discovery philosophy, structure and pacing, the cultural puzzle languages, the boss standard, reward architecture, and the rules that keep twenty dungeons from becoming twenty of the same dungeon.
**Inspirations:** Elden Ring (catacombs and legacy dungeons, discovery-first design), Crimson Desert (environmental spectacle), Dark Souls (compact interlocking space), Tomb Raider (traversal puzzles), Outer Wilds (knowledge as the actual key), Studio Ghibli (places that feel older than the story)

---

## 1. DUNGEON PHILOSOPHY — THE FIVE PRINCIPLES

### 1.1 Principle 1: Found, Not Given

**Cross-reference:** colonial_war_overview.md Section 11.1

The majority of Phase 7's dungeons are discovered by exploring — a cave mouth behind a waterfall, a sunken arch visible only at low tide, a door in a cliff face that reads as geology until you are close. **No map marker. No quest that points at it. No NPC who says "there is a temple to the east."**

A minority are revealed: by ancient maps (Alliance-gated), by quest, or by a document found in another dungeon.

**Design intent:** the Elden Ring contract — the world rewards looking.

### 1.2 Principle 2: Compact and Dense

**Fifteen to forty-five minutes.** No dungeon in Phase 7 is a two-hour crawl. Every room does work: a puzzle, an encounter, a piece of environmental storytelling, or a view that stops the player.

**Design intent:** no filler corridors. If a space exists, it earns its existence.

### 1.3 Principle 3: One Unforgettable Image Each

**Design law:** every dungeon in Phase 7 contains at least one moment the player will describe to someone else. The flooded hall that drains twice a day. The tree growing through the altar. The chamber where the ash preserved everyone exactly where they were standing.

**Cross-reference:** colonial_war_overview.md Section 11.1 — *"every dungeon stunning."* This is not aspiration; it is the acceptance criterion.

### 1.4 Principle 4: The Puzzle Speaks a Culture

**Cross-reference:** Section 5, indigenous_liberation_alliance.md Section 5.2

Phase 7 dungeons do not use generic block-pushing. Each region's puzzles are written in that culture's logic — plant-and-water reasoning in Rexa, star-and-ash in Ashenmoor, saga-and-tide in Fjordlund, clockwork and acoustics in colonial sites. **Learning the culture is learning the puzzles.**

**Design intent:** cultural authenticity as mechanical depth, not decoration.

### 1.5 Principle 5: Something Waits at the Bottom

Every dungeon has a boss. Cross-reference colonial_war_overview.md Section 9.1 Tier 4: compact, mechanically sharp, atmospheric. Not a scaled-down general — **a different category of thing entirely.**

---

## 2. THE DUNGEON TIERS

### 2.1 Tier A — Minor Sites (8-15 minutes)

Single-chamber or two-room sites. One encounter, one small puzzle or none, one piece of loot or lore.

**Purpose:** texture. The archipelago is full of small old places, and most of them are not stories.

**Count target:** 12-20

### 2.2 Tier B — Standard Dungeons (20-45 minutes)

The Phase 7 workhorse. Three to six chambers, a coherent puzzle language, an environmental hazard, and a Tier 4 boss.

**Purpose:** the core dungeon experience.

**Count target:** 15-20 (catalogued in regional_dungeon_catalog.md)

### 2.3 Tier C — Major Complexes (60-90 minutes)

Cross-reference the Elden Ring "legacy dungeon" model at reduced scale. Multi-level, interlocking shortcuts, two bosses or one boss plus a major set-piece, and a genuine sense of place.

**Purpose:** landmark content. There should be four or five of these and they should be talked about.

**Count target:** 4-5

### 2.4 Tier D — The Old Fortresses

**Cross-reference:** castle_catalog.md Section 12

Not dungeons exactly — pre-colonial fortresses that must be reclaimed. They contain dungeon content (sealed lower levels, flooded chambers, ash-buried wings) but resolve into Alliance Bastions rather than being cleared and left.

**Count:** 4

### 2.5 Tier E — Secret Sites

**Cross-reference:** secret_bosses.md

Trust Tier 4, ancient maps, opened waters, and often a recovered artifact as a key. **An Empire-path player never sees one.**

**Count:** 4-6

---

## 3. DISCOVERY DESIGN

### 3.1 The Four Discovery Modes

**ENVIRONMENTAL** — the entrance is visible if the player is paying attention. A crack behind vegetation (cross-reference physics/vegetation_physics.md), an arch exposed at low tide (physics/water_physics.md), a shadow that is a doorway at a particular hour (physics/sun_shadow_systems.md).

**CARTOGRAPHIC** — marked on an ancient map. Cross-reference indigenous_liberation_alliance.md Section 5.2: the maps are written in cultural map-languages Jake must learn to read, and **a map Jake cannot read is a map that shows him nothing.**

**DOCUMENTARY** — referenced in something found elsewhere. A colonial survey noting "an anomalous void beneath the western works." A saga that is also a set of directions. Cross-reference npcs/tavern_keepers.md Section 4.2 — Master Halvdan's storytelling nights contain three dungeon locations, and the design never points this out.

**RELATIONAL** — someone tells Jake, because they trust him. Cross-reference indigenous_liberation_alliance.md Section 3.5 (trust tiers). At Kin tier, the networks show him doors.

### 3.2 The No-Marker Rule

**Design law:** discovered dungeons do not appear on the map until entered. There is no "undiscovered location" icon. Cross-reference war_state_system.md Section 8.1 — the same principle as the war meters: **the player reads the world, not the interface.**

**Exception:** cartographic discovery. An ancient map read successfully does mark its sites, because that is what a map is for.

### 3.3 The Sailing Layer

Cross-reference physics/water_physics.md, physics/wind_systems.md. Many dungeon entrances are only reachable by ship — sea caves at particular tides, islands that require crossing feared waters, reefs navigable only with Alliance routes.

**Design intent:** the exploration loop is a sailing loop. Dungeons are a reason to go somewhere, and going somewhere is the game.

---

## 4. STRUCTURE AND PACING

### 4.1 The Standard Shape (Tier B)

```
THRESHOLD  — the entrance, and the first image
DESCENT    — one to two chambers establishing the puzzle language
TURN       — the dungeon's distinctive mechanic or hazard arrives
DEPTH      — the hardest room; the unforgettable image usually lives here
THE FLOOR  — the boss
RETURN     — a shortcut back, opened from the inside
```

**The Return is mandatory.** Cross-reference Dark Souls's shortcut philosophy: the player should be able to leave quickly, and the shortcut opening is the dungeon's small reward for having gone all the way down.

### 4.2 Density Rules

- **No corridor longer than fifteen seconds of travel** without something in it
- **No more than three combat encounters** in a Tier B dungeon before the boss
- **Every chamber has a reason to look up.** Cross-reference physics/vegetation_physics.md Section 6 (canopy layers), and the archipelago's dungeons are frequently vertical

### 4.3 Hazard Layers

Each dungeon runs one primary environmental hazard drawn from the physics documents:

| Hazard | Source | Regions |
|---|---|---|
| Flooding / tides | physics/water_physics.md | Fjordlund, sea, coastal |
| Heat / gas | physics/lava_heat_physics.md | Ashenmoor |
| Cold / ice | physics/snow_ice_physics.md | Fjordlund |
| Sand / collapse | physics/sand_desert_physics.md | Ashenmoor desert |
| Vegetation / rot | physics/vegetation_physics.md | Rexa, Moran |
| Darkness | physics/sun_shadow_systems.md | all |
| Structural | physics/destruction_physics.md | colonial sites |

**Design law:** one primary hazard per dungeon. Two is muddy. Three is a mess.

### 4.4 Light

Cross-reference physics/sun_shadow_systems.md, physics/fire_physics.md Section 3.5.

**Torch discipline is a dungeon mechanic.** Jake's light radius (torch 5-8m, lantern larger) defines the experience. Some dungeons have their own light — bioluminescence in the deep jungle sites, volcanic glow in Ashenmoor, ice refracting daylight from above in Fjordlund — and those are the ones that get the unforgettable images.

---

## 5. THE CULTURAL PUZZLE LANGUAGES

### 5.1 The Principle

**Cross-reference:** indigenous_liberation_alliance.md Section 5.2

Five puzzle languages, each internally consistent, each learnable. A player who has solved three Rexan dungeons approaches the fourth with genuine competence, and that competence does not transfer to Ashenmoor.

### 5.2 REXAN — Plant and Water Logic

**The grammar:** growth, flow, season, and what a living thing does.

**Puzzle types:**
- Water routed through channels to feed or starve specific growth
- Root systems as circuits — cross-reference physics/vegetation_physics.md Section 5.4
- Sequence puzzles based on flowering order (the answer is in the garden outside, if the player looked)
- Vine traversal as spatial puzzle — cross-reference physics/vegetation_physics.md Section 4

**The teaching:** Mama Jacinta's network. Cross-reference npcs/cannabis_keeper_mama_jacinta.md — she does not teach puzzles. She teaches plants, and the puzzles become legible.

### 5.3 ASHENMOOR — Star and Ash Logic

**The grammar:** heat, time, sightline, and the sky.

**Puzzle types:**
- Star alignments — cross-reference physics/sun_shadow_systems.md Section 6.2: the night sky is modelled, and constellations are the key
- Heat channelling: opening and sealing vents to make a route survivable — cross-reference physics/lava_heat_physics.md
- Ash-fall reading: what was buried tells you where things were
- Shadow-hour puzzles: a chamber that only makes sense at one time of day

**The teaching:** Mbah Seruni, and her night-singing of names is also, if the player is listening, a star chart. Cross-reference npcs/desert_sage_mbah_seruni.md.

### 5.4 FJORDLUND — Saga and Tide Logic

**The grammar:** narrative sequence, tide tables, and cold.

**Puzzle types:**
- **The sagas are the instructions.** A hall's mechanism is operated in the order the story tells it. Cross-reference npcs/tavern_keepers.md Section 4.2 — a player who sat through storytelling nights at Bjornshallen has the answers.
- Tide windows: chambers accessible for forty minutes twice a day — cross-reference physics/water_physics.md
- Ice as a tool: freezing a channel to walk it, melting one to drain it — cross-reference physics/snow_ice_physics.md
- Rune-carving sequences that are genealogies

**The teaching:** Master Halvdan's hall, Mormor Astrid's stories, and Jarl Olaf's patience. Cross-reference npcs/healer_mormor_astrid.md Section 9.

### 5.5 COLONIAL — Clockwork and Acoustics

**The grammar:** mechanism, geometry, and the Empire's own engineering.

**Puzzle types:**
- Clockwork mechanisms in mines, siege tunnels, and plague vaults
- **Acoustic puzzles in cathedral crypts** — cross-reference physics/sound_physics.md Section 4: reverb times identify chamber sizes, and a sealed vault can be located by singing at a wall
- Survey-map reasoning: the colonial engineers left plans, and the plans are wrong in informative ways
- Lock and key logic, because the Empire locks things

**The teaching:** none required. The colonial language is the one the player already speaks, and that is the point — **the Empire's dungeons are legible because the Empire is the culture the game's audience shares.**

### 5.6 THE FIFTH — Region 06

**The grammar:** unreliable.

Cross-reference npcs/regional_populations.md Section 9. Region 06's dungeons do not have a consistent puzzle language, and the design's intent is that **the player's acquired competence stops working.** A tide table that is wrong. A star alignment for a sky that is not this one. Sagas that describe things that did not happen.

**Late-game only.** Cross-reference secret_bosses.md.

---

## 6. THE TIER 4 BOSS STANDARD

### 6.1 What They Are Not

Not scaled-down generals. Not reskins of Phase 1-2 bosses. Not health-sponges.

### 6.2 What They Are

**Compact, mechanically sharp, atmospheric.** One arena, one clear gimmick tied to the dungeon's hazard, two phases at most, and a fight that lasts three to six minutes.

**Categories:**
- **Guardians** — placed things, still doing their job centuries later
- **Fauna** — cross-reference the Phase 4 fauna documents: apex predators that have made a dungeon their den. Cross-reference fauna/land_animals.md — the Patriarch's lesser cousins.
- **Remnants** — what is left of the people who were here
- **The Colonial Dead** — cross-reference physics/destruction_physics.md and the plague vaults; the Empire buried some things badly

### 6.3 Boss Design Rules

- **The gimmick is the dungeon's hazard, weaponised.** A tide-chamber boss uses the flooding. An ash-chamber boss uses the heat.
- **No dialogue.** Cross-reference colonial_war_bosses.md Principle 1.1 by inversion: Phase 7's human bosses talk constantly. **The dungeon bosses do not talk at all**, and the contrast is the atmosphere.
- **Death is quiet.** Style D by default. Cross-reference style_bible.md Pillar 3 — several of these were people.

---

## 7. REWARD ARCHITECTURE

### 7.1 What Dungeons Give

- **Materials** — cross-reference mechanics/inventory_crafting.md, and specifically the legendary tiers: Pak Setyo and Bu Wira need things that only exist down there
- **Lore** — cross-reference style_bible.md Pillar 2: the object is the story
- **Maps** — dungeons contain routes to other dungeons
- **Keys** — cross-reference secret_bosses.md: several Tier E sites are opened by artifacts found in Tier B dungeons
- **Occasionally, a person** — cross-reference the survivor content in Section 8.2

### 7.2 What Dungeons Do Not Give

**Not doubloons in significant quantity.** Cross-reference colony_system_core.md Section 8 — the war economy is where money lives. Dungeons pay in things money cannot buy, which keeps them relevant to a wealthy late-game Jake.

### 7.3 The Cultural Return Question

Cross-reference indigenous_liberation_alliance.md Section 2.3, npcs/black_market_fences.md Section 6.2 (Tito Diego), npcs/luxury_merchants.md Section 3.3 (Doña Carmen Ruiz).

**Many dungeon artifacts belong to someone.** The recurring choice: sell it, keep it, or return it. Cross-reference Tito Diego's question — *"Which captain are you?"* — and the dungeon layer asks it roughly forty times.

---

## 8. ATMOSPHERE AND RESTRAINT

### 8.1 The Silence Rule

Cross-reference physics/sound_physics.md Section 14.

**Most dungeons have no music.** Drips, wind, settling stone, and Jake's own breathing. Score enters at the boss and at the unforgettable image, and nowhere else.

### 8.2 Environmental Storytelling Only

**Design law:** dungeons do not explain themselves. No audio logs, no convenient journals, no NPC who survived to narrate.

**The exception, used sparingly (two or three times in the whole phase):** someone *did* survive, and has been down there a long time, and is not entirely all right. Cross-reference npcs/regional_populations.md Section 9 (Region 06's remaining) for the register.

### 8.3 The Bodies

Cross-reference style_bible.md Pillar 3, npcs/the_named_dead.md.

There are dead people in these places. The design treats them with the same restraint it treats every other death: **positioned, never posed.** A person who died reaching for something. A person who died sitting down. Cross-reference npcs/children_and_families.md — **and the protection rules extend here: no child remains, ever, in any dungeon in the game.**

---

## 9. INTEGRATION MAP

- **regional_dungeon_catalog.md** → the specific dungeons built to this standard
- **secret_bosses.md** → Tier E and the Alliance gating
- **indigenous_liberation_alliance.md** ← ancient maps, opened waters, trust tiers, the artifact keys
- **castle_catalog.md** ← the Old Fortresses (Tier D), and La Ciudadela's undercity is functionally a dungeon
- **physics/** ← every hazard layer, every light rule, the sound design, the tides, the stars
- **mechanics/combat_feel.md, inventory_crafting.md** ← boss combat and material rewards
- **fauna/** ← the apex-predator boss category
- **npcs/tavern_keepers.md, healer_mormor_astrid.md, cannabis_keeper_mama_jacinta.md, desert_sage_mbah_seruni.md** ← the puzzle languages are taught by the sages and the storytellers
- **npcs/black_market_fences.md, luxury_merchants.md** ← the artifact return question

---

## 10. MVP CORE vs FULL VISION

### 10.1 MVP CORE

- **Three Tier B dungeons**, one each in Rexa, Fjordlund, and Ashenmoor — establishing three puzzle languages
- Environmental and cartographic discovery modes
- The standard six-beat structure with mandatory Return shortcut
- One hazard layer per dungeon
- Three Tier 4 bosses, one per hazard category
- No-marker discovery, no music, no explanatory logs

### 10.2 FULL VISION

- All five tiers, the full count targets
- All five puzzle languages including Region 06's unreliable grammar
- All four discovery modes including the saga-as-directions and the survey-map documentary route
- The Old Fortress reclamation layer
- The survivor encounters
- The full artifact-return economy across forty-plus objects

---

## 11. STYLE BIBLE COMPLIANCE

- ✅ **Pillar 1 (Staggered Register):** the dungeons are silent, so register lives in the sages who teach the puzzle languages and in the rare survivor's voice. Cross-reference the deliberate contrast with Phase 7's constantly-talking human bosses.
- ✅ **Pillar 2 (Lore Through Objects):** the entire dungeon reward architecture is this pillar. No logs, no narrators — the object, the position of the body, the thing someone died reaching for.
- ✅ **Pillar 3 (Layered Death):** Tier 4 bosses die quietly. The dead are positioned, never posed. No child remains anywhere, ever.
- ✅ **Pillar 4 (Layered Humor):** almost entirely absent by design, and that absence is what makes the dungeons feel different from the rest of Phase 7. Humour lives in the crew's reactions afterward, on the ship, in the light.
- ✅ **Pillar 5 (Theme — Revenge):** forty-odd artifacts that belong to someone, and a question asked forty times. The dungeon layer is where Jake's war stops being about empires and becomes about what he personally chooses to carry out of a dark room.

---

## 12. NEXT STEPS

Next document: **`colonial_war/regional_dungeon_catalog.md`** — the specific dungeons.

---

## 13. CONCLUSION

The Places Underneath. Found rather than given — a crack behind vegetation, an arch that appears at low tide, a shadow that is a door for one hour a day. Fifteen to forty-five minutes each, and every room does work. One image per dungeon that the player will describe to somebody else.

Five puzzle languages, and learning them is learning the cultures: plant-and-water in Rexa, star-and-ash in Ashenmoor, saga-and-tide in Fjordlund, clockwork and acoustics in the colonial dark, and in Region 06 a grammar that lies. A boss at the bottom that does not speak, in a place with no music, where the dead are positioned and never posed.

And forty-odd objects that belong to somebody, each one asking the same question Tito Diego asked once: which captain are you?

**Phase 7 Subsystem 14 of 16 — complete.**

**No marker. No music. No explanation. Go and look.** 🕯️🗿
