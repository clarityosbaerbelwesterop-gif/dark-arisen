# Region 03 — Ashenmoor Volcanoes & Dungeons

**Status:** Design Draft v1.0  
**Related:** region_03_ashenmoor_overview.md, region_03_ashenmoor_settlements.md  
**Inspirations:** Mount Bromo (Indonesia), Sekiro's Hirata Estate, Dark Souls' Izalith, Princess Mononoke's Forbidden Mountain

---

## 1. Overview

The three volcanoes of Ashenmoor are not just background scenery — they are active gameplay spaces. Each volcano has its own mythology, its own dungeon, its own boss, and its own environmental mechanics. Together they form a vertical dungeon trilogy that culminates in the most ambitious encounter in Region 3: the Ashen Wyrm, Dark Arisen's dragon fight.

The Anak Gunung call the three volcanoes **"The Father, The Mother, and The Child"** — a family of fire that has watched over the islands since before human memory. Each has a personality in local lore, and that personality shapes how the player experiences it.

---

## 2. Gunung Bapa — The Father (Dormant Giant, Dragon Lair)

**Elevation:** 2,400m (tallest peak in Ashenmoor)  
**Status:** Dormant for centuries, but not dead  
**Local Belief:** The Father sleeps. What sleeps inside him does not.

### 2.1 Environmental Description
Gunung Bapa dominates the skyline. From a distance it looks peaceful — snow on its summit, jungle on its lower slopes. But the peak is false. The real interior of the mountain is hollow, carved out by ancient lava flows that left behind a massive network of tunnels, caverns, and pre-human ruins. This is where the Ashen Wyrm sleeps.

### 2.2 Access Path
Jake cannot enter Gunung Bapa directly. The mountain is sealed. Access comes through:
1. **The Deep Mine Shaft in Kota Api** (story-locked until Chapter 7)
2. **The Secret Entry from Kuil Atas** (unlocked by passing the Abbot's Test)
3. **The Hidden Tunnel in Desa Padi** (Mbah Seruni's path, unlocked via the Dragon's Lullaby questline)

All three paths converge in the same central cavern: **The Cathedral of Bones**, where the dungeon truly begins.

### 2.3 The Dungeon: The Ashen Depths
A multi-level descending dungeon. Each level gets hotter, older, and stranger.

| Level | Name | Hazard | Key Feature |
|-------|------|--------|-------------|
| 1 | The Cathedral of Bones | Dust, darkness | Ancient skeletons of unknown creatures; puzzle gate |
| 2 | The Obsidian Hallways | Echoing sound (alerts enemies) | Mirror-black walls that reflect enemies; stealth focus |
| 3 | The Ember Gardens | Heat damage over time | Impossible trees made of living fire; beautiful and deadly |
| 4 | The Primordial Library | Knowledge hazard (lore triggers) | Pre-human language carvings; translating them progresses the story |
| 5 | The Wyrm's Heart | Lava floor, falling rocks | Final arena before the dragon |

**Dungeon Length:** ~2-3 hours first playthrough  
**Rest Points:** 3 (before Level 2, before Level 4, before boss arena)

### 2.4 The Boss: The Ashen Wyrm (Dragon)
See `docs/design/bosses/ashen_wyrm.md` (to be written) for full spec. Summary:

- **Size:** Massive — 30 meters long, wingspan of 40 meters
- **Appearance:** Black scales streaked with glowing cracks where molten lava seeps out; eyes like twin suns; ancient, intelligent, bored
- **Personality:** Not a mindless beast. The Wyrm is old enough to speak the pre-human language Jake learned in the Primordial Library. It monologues between attacks. It considers Jake beneath its notice, and that arrogance is the only thing that gives Jake a chance.
- **Three Phases:** Ground combat → Aerial phase with flight → Ground desperation phase
- **Reward:** Dragon Amulet (fire resistance + fire damage bonus), Dragon Heartscale (crafts legendary armor), 10 Gold Bars (massive economy spike), lore unlock that reshapes the story

### 2.5 Why the Dragon Matters Narratively
The Ashen Wyrm is not just a big monster fight. Through its dialogue, Jake learns something devastating: the Crimson Armada, and specifically Draven Voss, came to Ashenmoor years ago looking for something in the Wyrm's lair. They found it. And whatever they took is connected to Ethan's transformation. The dragon doesn't care about Jake's revenge. But its words plant a seed that pays off in Chapters 8-9.

---

## 3. Gunung Ibu — The Mother (Active, Caldera Titan)

**Elevation:** 2,100m  
**Status:** Actively smoking, low-level eruptions every few years  
**Local Belief:** The Mother is angry. She grieves for something she lost.

### 3.1 Environmental Description
Gunung Ibu is the volcano Kota Api clings to. Her slopes are terraced with mines, forges, and lava-channels. Her caldera at the top is a bowl of molten rock kept barely stable by ancient Anak Gunung rituals that the Crimson Armada no longer performs. This means Gunung Ibu is becoming more dangerous during the story — eruptions intensify as Chapter 6 progresses.

### 3.2 Access Path
The caldera is reached by climbing from Kota Api. The path up is dangerous: falling ash, lava bomb impacts, and Crimson Armada patrols who have fortified the higher slopes.

### 3.3 The Dungeon: The Smoking Path
A linear ascent rather than a traditional dungeon. Think Dark Souls' climb to Bell Gargoyles, but with active environmental hazards.

| Section | Hazard | Notes |
|---------|--------|-------|
| The Lower Slopes | Crimson Armada patrols | Standard combat |
| The Ash Bridges | Collapsing terrain | Timing-based traversal |
| The Lava Channels | Rising and falling lava flows | Puzzle-like movement |
| The Upper Terraces | Lava bombs from the caldera | Dodge-timing challenge |
| The Caldera Rim | Extreme heat, poor footing | Final approach |

**Length:** ~1-1.5 hours  
**Rest Points:** 2 (Lower Slopes, Upper Terraces)

### 3.4 The Boss: The Caldera Titan
- **Size:** 15 meters tall, humanoid but built of living rock and molten stone
- **Appearance:** A massive figure that rises FROM the caldera itself as Jake steps onto the rim. Its body is cracked basalt with glowing magma veins. Its face is a mask of ancient stone carvings worn smooth by centuries.
- **Arena:** The caldera rim — a circular battlefield around a pit of bubbling lava. Jake must never fall in.
- **Phase 1:** Rock-throwing and slow, devastating stone-fist attacks. Stay mobile.
- **Phase 2 (60% HP):** The Titan submerges halfway into the lava and fights with arms only — the arena shrinks as lava rises.
- **Phase 3 (25% HP):** The Titan fully emerges, cracks open, and fights as a pure molten creature. Every attack leaves fire on the ground. Arena becomes a memory maze of safe spots.
- **Reward:** Magma Amulet (immunity to lava, fire damage bonus), Titan's Core (crafts legendary two-handed weapon), Recipe: Molten Rounds (pistol ammo that sets enemies on fire)

---

## 4. Gunung Anak — The Child (Erupting, Sturmkralle's Nest)

**Elevation:** 1,600m  
**Status:** Erupting periodically throughout the chapter — creates dynamic environmental hazards  
**Local Belief:** The Child is wild, unpredictable, alive with storm and fire

### 4.1 Environmental Description
Gunung Anak is the smallest and youngest of the three. It erupts in short, violent bursts every in-game hour, sending lava flows down its slopes and creating massive ash clouds that drift across the region. During an eruption, certain paths become blocked and new ones open. The player must learn to read the volcano's rhythm.

### 4.2 Dynamic World Mechanic
Gunung Anak's eruptions are the first "living world" mechanic in Dark Arisen:
- Every ~60 in-game minutes, the volcano erupts for 3-5 minutes
- Lava flows block certain roads and open others
- Ash clouds reduce visibility across nearby areas
- Some treasure chests and materials are only accessible DURING eruptions (hot lava reveals hidden paths by burning away vines)
- Some are only accessible BETWEEN eruptions (cooled lava creates bridges)
- This creates a puzzle of timing: when to explore, when to wait, when to run

### 4.3 The Dungeon: The Stormspire
Unlike the other two volcanoes, Gunung Anak's "dungeon" is the climb itself. There is no interior — the mountain is too young, too active. The climb takes Jake up a series of stone spires, rope bridges, and wind-battered ledges to the peak, where Sturmkralle nests.

| Section | Hazard | Notes |
|---------|--------|-------|
| The Wind Terraces | Gusting winds that knock Jake off ledges | Timing + positioning |
| The Rope Bridges | Ashfall rain and rot | Run, don't walk |
| The Spire Climb | Free-climbing sections | New mechanic for this region |
| The Nest Approach | Sturmkralle dive-bombs from the sky | Pre-boss harassment |
| The Peak Nest | Combat arena | Boss fight |

**Length:** ~1.5 hours  
**Rest Points:** 1 (before the nest approach)

### 4.4 The Boss: Sturmkralle, Sky Terror
- **Size:** Giant raptor, 8 meters tall, 20 meter wingspan
- **Appearance:** Blue-black feathers, storm clouds form around its wings, lightning arcs from its talons, eyes like amber coals
- **Arena:** The peak nest — a rocky platform with half of it open to sky. Sturmkralle alternates between ground combat and aerial attacks.
- **Phase 1:** Ground-based melee with talons and beak. Devastating but slow.
- **Phase 2 (60% HP):** Takes flight, dive-bombs Jake from above. Jake must use flintlock/musket to ground it, then punish during recovery.
- **Phase 3 (25% HP):** Lightning storm phase. The arena is struck by lightning at marked points. Sturmkralle uses the weather itself as a weapon.
- **Reward:** Talon Amulet (fall damage immunity, +40% dodge speed, aerial attacks unlocked), Storm Feather (crafts legendary bow), Recipe: Lightning Arrows

---

## 5. The Three Dungeons Compared

| Dungeon | Style | Difficulty | Length | Reward Tier |
|---------|-------|------------|--------|-------------|
| The Ashen Depths (Bapa) | Classic descending dungeon | Hardest in region | 2-3 hours | Top tier (Dragon) |
| The Smoking Path (Ibu) | Linear ascent with hazards | Mid-hard | 1-1.5 hours | Mid-high |
| The Stormspire (Anak) | Vertical climb with free-climbing | Mid | 1.5 hours | Mid-high |

---

## 6. Additional Dungeons in Ashenmoor (Non-Volcanic)

Beyond the three volcanic dungeons, Region 3 contains several smaller dungeons scattered across the settlements and wilderness:

| Dungeon | Location | Type | Reward |
|---------|----------|------|--------|
| The Red Lieutenant's Quarters | Kota Api | Story combat | Unlocks final Kota Api arc |
| The Submerged Shrine | Tanjung Hitam coast | Underwater mini-dungeon | Pearl Amulet (optional) |
| The Kuil Atas Archives | Kuil Atas temple | Lore + puzzle | Story revelations, stat boosts |
| The Forgotten Terrace | Desa Padi jungle | Environmental puzzle | Hidden treasure cache |
| The First Burning Shrine | Kuil Atas (hidden) | Lore + combat | Connects to dragon storyline |

---

## 7. Environmental Mechanics Unique to Ashenmoor

- **Ashfall:** Periodic ash drops reduce visibility, slow movement 10%
- **Volcanic Tremors:** Shake the screen, can trigger rockfalls
- **Lava Flows:** Permanent instant-kill hazards; force pathing choices
- **Thermal Vents:** Heal over time near them (reward for exploration)
- **Wind Gusts:** High altitudes can push Jake off ledges
- **Acid Steam:** Sulfur vents deal poison damage; requires Antidote Tincture
