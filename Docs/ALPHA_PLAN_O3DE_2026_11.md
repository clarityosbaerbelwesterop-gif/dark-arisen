# Dark Arisen: ten milestones to Alpha (O3DE, 2026-11)

**Direction (owner, 2026-11-08):**
- **Open world.** Scale and quality are measured against RDR2 and Elden Ring.
- **Rework what looks poor.** The bar is AAA.
- **Cutscenes.**
- **Then gameplay, main quests, side quests and combat.**
- **M9** is voice and lip sync. **M10** is polish and the rest of the way to Alpha.
- **Order.** M1–M9 are worked through in order, each one completely. Milestone contents not named by the owner are set here.

**Definition of Alpha:** `Docs/ALPHA_DELIVERY_CHECKLIST.md`. A milestone counts as done only with its evidence: CI, probe, render, or runtime capture. Source on its own is PARTIAL.

**Free and legal only.** Third-party material is CC0 or open licence only (MakeHuman, CMU, Poly Haven, ambientCG), recorded in `ContentSource/ThirdParty`. Nothing is bought without approval.

## Scale target

The world is one archipelago of about 12 × 10 km (about 120 km², of which about 60–75 km² is land). For comparison, RDR2's map is about 75 km² and Elden Ring's Lands Between about 79 km². Sea lanes join the regions; there is no loading screen inside the world.

| Region | Character | Main story (from the contracts) |
|---|---|---|
| Moran | tropical start islands, Ethan's Grove | Chapters 1–2, 9 |
| Rexa & The Mire | colonial port, Puerto de la Corona, swamp back routes | Chapters 3–5 |
| Highmoore (the contested holding) | highland moor, Crystal Passage, northern oath | Chapters 5–7 |
| Open Sea & Fort Carrion | naval waters, Armada occupation and final screen | Chapters 5, 7, 8, 10 |
| Ashenmoor | volcanic island chain, ash jungle | no main-story contract yet: open-world region (exploration, side quests) |
| The Quiet Coast (Fjordlund) | cold fjords, villages, lighthouse chain | no main-story contract yet: open-world region (exploration, side quests) |
| The Maw | post-story only, never blocks the credits | — |

Ship decks (La Liberación, Draven's flagship, the prison navigator) and the Harlow dream are places the story moves into, not regions on the map.

Mission levels become **places in the world**. The 34 missions keep their contracts; their geometry is placed at world coordinates.

## Milestones

| # | Milestone | Already in `main` | Still to do |
|---|---|---|---|
| **M1** | Open-world foundation and AAA look | Terrain kit, biomes, vegetation with wind, ponds, dressed architecture, foliage and lava shaders, ocean (Gerstner), byte-identical pipeline | World geography and heightfields at full scale on O3DE's Terrain and Vegetation gems (clipmaps, instanced spawning instead of meshes), region streaming, scanned CC0 materials (Poly Haven, ambientCG) in place of procedural textures, HDRI sky and light, shoreline foam, 1440p Cycles reference renders |
| **M2** | Regions complete | 17 land levels on terrain, Rexa quay, Moran coast, forts | All seven regions at full size: settlements (Rexa city, villages), landmarks, dungeons and caves, POIs every few hundred metres, Ashenmoor, The Quiet Coast and The Maw new |
| **M3** | Characters and creatures at AAA level | 27 skinned actors, 34 face shapes, visemes, clothes, hair | Engine Skin material (subsurface scattering), hair cards with anisotropy, LODs, detail normal maps, anim graphs, creatures and bosses (Keeper, sea creatures, Maw boss) |
| **M4** | Gameplay core | Combat model, enemy AI, swimming, ship voyage, naval combat, camera, lock-on, `CharacterMotion` | Animation-driven combat contact, traversal (climbing, ledges), open-sea sailing between regions, more enemy types, horses (Quiet Coast only, `traversal.md` 10.5) |
| **M5** | Main quests in depth | 34/34 missions physically materialised, story probe 27/27 | Dialogue with choices and consequences, mission set pieces (chases, boarding, sieges), duels |
| **M6** | Side quests, activities, living world | LivingWorld, QuestJournal, economy in Core | Side quests from `ContentSource/SideQuests`, activities, world events, NPC daily routines in the settlements |
| **M7** | Cutscenes and presentation | Opening director, credits, front end | Cutscene system (shots, camera, motion, faces, subtitles), all story cutscenes, map and minimap (ruled 2026-11-09, below) |
| **M8** | Music, ambience, sound | — | Ambience per biome, sea, footsteps per surface, combat, music cues, the credits cue |
| **M9** | Voice and lip sync | 15 visemes, 9 emotions | Voices for every line, lip sync from the visemes, subtitle timing; the production choice is in the section below |
| **M10** | Polish and Alpha | AzCore probe, CI | GPU runtime proof (GameLauncher), 60 fps, packaging for Windows and Linux, credits and attribution, the owner's playthrough and approval |

## Progress (2026-11-09)

- **M1, open-world foundation (built, not yet run in a GameLauncher).**
  - `world-build` turns `Archipelago.json` into 397 streamed 500 m terrain tiles plus a horizon mesh and plants the land with about 930,000 plantings: woods, clearings, a tree line, undergrowth.
  - The Gem's `WorldStreamerComponent`, `WorldRendererComponent` and `WorldColliderComponent` stream, draw and collide the world around Jake in `L_World`. The streamer is probed on O3DE 2605.0's AzCore runtime. The renderer and collider compile against the pinned Atom and physics headers in CI.
  - Scanned CC0 ground layers and fixes to the terrain shader's re-tint and reflectance give the islands their colour.
- **M2, regions (first pass).**
  - `Locations.json` places the documents' 95 named locations and all 61 dungeons, checked by `ConversionTests`.
  - Settlements stand on levelled ground. Rexa is a colonial street grid with a landmark on each district's square (cathedral, market hall, warehouses, mansion, watchtower); Ashenmoor has stilt villages, the Quiet Coast long houses under turf, and there are forts, camps and lighthouses.
  - Every dungeon has a visible way in.
  - Still to do: harbour quays and boats, paved streets, props in the streets, interiors, the story levels' own ground stamped into the world, and NPC routines (M6).

## Decisions taken here, revisable by the owner

- **World geography** is the region table above. Moran is in the south-west; the sea lanes follow the chapter order, and Ashenmoor and the Quiet Coast lie off the main lanes as places to discover.
- **Generated world data** (heightfields, splat maps) is built on the build host, not committed. The repository holds the generators and checksums. This settles the open "keep committing materialised art" question for world-scale data only.
- **Lighting reference** is a physically based sun plus a CC0 HDRI sky. Renders use AgX tone mapping.

## Decided 2026-11-09 (the owner delegated these: "den Rest musst du selbst festlegen")

- **Voice (M9).**
  - A free local TTS: Kokoro-82M (Apache-2.0), run offline when the lines are authored.
  - The voices are credited as synthetic. They are never presented as actors, and a human performance may replace any line later.
  - ElevenLabs is not used: it is paid.
- **Map and minimap.** The Alpha has a full map and a minimap, as the owner required on 2026-09-19 (`Docs/DesignAuthority.md`, Alpha navigation override). This replaces the earlier default of "no minimap".
  - Neither shows objective markers, a route line, enemies, collectibles or an objective arrow.
  - Water fast travel stays forbidden.
- **GPU host for runtime proof (M10).** No money is spent (owner, 2026-11-09), so the runtime proof stays open.
  - The free route to evaluate is O3DE on Mesa's software Vulkan (lavapipe) in CI.
  - That shows the GameLauncher starts, streams the world and plays the opening. It is not a 60 fps proof.
- **Horses** are on the Quiet Coast only (`traversal.md` 10.5), not in Highmoore.
- **Studio tooling.**
  - The Claude Code Game Studios skills and agents are installed by the environment's setup script.
  - The `blender` skill (`.claude/skills/blender`) gives every milestone its visual checks.
  - Level work goes through `/team-level`, for example `production/levels/rexa-city/layout.md`.

## Decided 2026-11-11 (the owner): the bosses are made in Blender

The owner rejected the ArtKit's boss models: "kein Triple-A-Standard, schau dir Elden Ring an ... macht es in Blender,
damit es gut aussieht".

- **Bosses are sculpted and baked in Blender** by the forge scripts (`.claude/skills/blender/SKILL.md`, section 6):
  block-out from the boss document, voxel fusion, numpy sculpting, a high-to-low bake (normal, occlusion, colour,
  roughness) and a GLB under `Assets/Art/Creatures/`. Each boss is judged in an arena shot, not on an empty sky.
- **The ArtKit's `SM_Art_Creature_*` models are superseded** boss by boss as each forge version lands.
- **Scope of the exception.** World, settlements, cast and props stay in the C++ ArtKit. The human bosses follow after
  the creatures, with the same pipeline over the MakeHuman base.
- **Evidence.** The forge output is not byte-identical across Blender versions. The GLBs are committed and are outside
  `materialize --check`.
