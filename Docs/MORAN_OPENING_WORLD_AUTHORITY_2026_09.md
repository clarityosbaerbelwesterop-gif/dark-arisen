# Dark Arisen — Moran Opening World Authority

**Authority date:** 2026-09-10
**Status:** current implementation authority for the prologue, strand point, Chapter 1 landing, Chapter 2 onboarding route and first-world transition.
**Supersedes:** older Moran/Rexa chapter numbering and the old `La Esperanza -> Rexa -> buy La Liberación` progression where it conflicts with the current 34-mission story.

## 1. Exact strand point

Jake washes ashore at **Driftwood Beach**, on **Moran — Zone A: The Shattered Coast**.

The arrival composition is locked as:

- dawn-to-morning light after the raid;
- white sand broken by black wet wreck timber;
- turquoise shallows over coral, but the sea remains physically dangerous;
- Harlow cargo and splintered family-ship debris distributed along the tide line;
- one recognizable Harlow object close enough to establish that Jake did not dream the attack;
- no friendly NPC standing over Jake when control returns;
- no objective marker, minimap, compass or glowing path;
- the player sees **Driftwood Camp smoke**, reef birds and a broken mast line as physical navigation cues.

Jake reaches the beach through playable water survival. The final few meters are a crawl/wade, not a teleport or black-screen load.

## 2. Tutorial doctrine — no tutorial island

Moran is a real region, not a disposable training island. The opening teaches through story pressure and physical necessity.

### Family ship / raid

- movement: walking a ship Jake already knows;
- interaction: helping family/crew with a physical task;
- camera: naturally used while following people and looking through rigging;
- basic combat: boarding attack;
- posture/deflection: the first real hostile exchange Jake cannot brute-force;
- loss: the raid ends in the authored family catastrophe, not a `MISSION FAILED` screen.

### Driftwood Beach / Shattered Coast

- swimming and breath: survive the reef/current after the fall;
- stamina: wading, climbing wreckage and beach recovery;
- pickup/examine: Harlow debris and a useful survival item;
- healing: Mira later treats Jake, while the player learns the recovery system through the state of his body;
- journal: Jake writes after reaching shelter; the player reads afterward;
- no pop-up tutorial panels.

A control prompt may appear once, small in the corner for four seconds on first availability, then disappear. Repeated tutorial text is forbidden in-world.

## 3. Opening physical route

The first authored traversal chain is:

`Family Merchant Ship -> Draven Raid -> Open Water -> Outer Reef -> Driftwood Beach -> Driftwood Camp -> Mira's Cove -> Mangrove Shallows -> Koa's Trading Post -> Galleon Cove -> La Liberación -> Open Sea -> Rexa Harbor`

No node in this chain uses fast travel.

### 3.1 Driftwood Beach

The player regains reliable movement here after the water sequence. The beach contains:

- **Jake's Wreckage** — Harlow debris, physical story proof;
- **Driftwood Camp** — first safe rest/save location after the attack;
- tide pools, coral shelves and reef channels;
- distant sightline toward **Mira's Cove**;
- no mandatory combat animal immediately on wake-up. The old forced Coconut Crab tutorial is retired as a critical-path requirement because the ship raid already owns first-combat onboarding.

### 3.2 Mira's Cove

Mira is the first stable ally Jake meets after the raid. She is not a tutorial narrator. She treats his wounds, responds to the Harlow/Draven insignia evidence and gives locally credible directions rather than UI coordinates.

Mira may say what locals call landmarks; the journal may record those words. She does not tell Jake to follow a waypoint.

### 3.3 Mangrove Shallows

The Mangrove Shallows are the first area where navigation becomes intentionally less visually obvious. Knee-deep water, roots, current direction, smoke, bird movement and local directions teach the player to read the world.

The Sunken Chapel remains optional. It is never required to understand a core control.

### 3.4 Koa's Trading Post

Koa's Trading Post is the first compact service/economy node. It teaches repair, provisioning and trade through actual need after the wreck.

It does **not** become a menu-heavy tutorial hub. Physical counters, objects, workshop stations and dialogue carry the experience.

### 3.5 Galleon Cove

The old document's `La Esperanza` departure is superseded. **Galleon Cove now contains the impound/prize berth where La Liberación can be acquired during Chapter 2.**

Esteban is tied to this maritime space and becomes the natural navigation teacher. Big Tom enters the Chapter-2 crew path before the escape. Mira, Big Tom and Esteban are the first three canonical crew members.

`Main.C02.02.AShipToTake` physically runs through Galleon Cove and ends only once La Liberación clears Moran's harbor under her own movement.

### 3.6 First Wake to Rexa

`Main.C02.03.FirstWake` begins outside Galleon Cove. Jake crews before commanding. Esteban teaches wind, heading, chart reading and landmark navigation in dialogue and through required physical actions.

The first major destination is **Rexa Harbor**. Arrival at Rexa closes Chapter 2 and begins the broader archipelago phase.

## 4. Moran environment retained from the older region draft

The following environmental identity remains valuable and is promoted into current canon unless later technical world-production evidence requires a geometry change:

- Shattered Coast — white sand, coral reef, shipwreck debris;
- Mangrove Shallows — dense roots and shallow-water traversal;
- Deep Jungle — high-canopy interior and river systems;
- Paradise Lake — optional hidden inland location;
- Pirate Encampment / Smuggler's Caves — optional authored content;
- Galleon Cove — eastern maritime exit and current La Liberación acquisition site.

The names `Driftwood Beach`, `Driftwood Camp`, `Mira's Cove`, `Koa's Trading Post` and `Galleon Cove` are current opening-world names.

## 5. Retired Moran critical-path material

These old draft elements are not current main-story requirements:

- Moran as a separate skippable/demo-only tutorial island;
- mandatory Coconut Crab as the player's first combat lesson;
- `La Esperanza` as the required ship to Rexa;
- acquiring La Liberación only after completing old Rexa Chapter 4;
- a minimap reveal from the Great Tree;
- any marker-driven treasure/tutorial flow;
- old chapter numbering that places Moran's jungle crawl as Chapter 2 while La Liberación is obtained much later.

Optional Moran dungeons, bosses and side stories remain candidates, but they must be reconciled against current content registers before being called shipping canon.

## 6. World progression map

The numbered historical world design is retained as geographic source material, while current story chapters are remapped independently:

1. **Moran** — tropical castaway/start region; Driftwood Beach and the Chapter-2 ship acquisition route.
2. **Rexa & The Mire** — dense colonial/pirate city and swamp network; first major archipelago destination.
3. **Ashenmoor** — volcanic island chain, vertical culture and jungle/ash environments.
4. **The Quiet Coast** — cold northern fjord coast, villages and lighthouse chain.
5. **The Open Sea & Fort Carrion** — naval pressure region and Armada-controlled waters/fortress geography.
6. **The Maw** — post-main-story region only; it never blocks the Chapter-10 credits path.

**Highmoore** remains a separate optional world layer reached from the mid-game and is not required to finish the main story.

The player's world should feel physically contiguous through sea lanes and authored land approaches. Numbered region labels are production taxonomy, not HUD chapter cards.

## 7. First-preview target

The first meaningful playable preview should prove this exact slice rather than a generic greybox room:

1. short family-ship playable beat;
2. Draven vessel approach / boarding onset;
3. cut to the aftermath boundary only where current assets are missing;
4. playable water survival;
5. **Driftwood Beach** landfall;
6. walk from wreckage to Driftwood Camp;
7. first physical direction toward Mira's Cove;
8. no minimap/compass/objective marker;
9. 60-fps gameplay renderer acceptance separate from path-traced cinematic capture;
10. Higgsfield previs used as animation/camera reference for the raid and landfall, then rebuilt/retargeted in Unreal.

That slice is the first preview milestone. It proves story, world, water, traversal, onboarding, camera discipline and visual direction in one coherent section.
