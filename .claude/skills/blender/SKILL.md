---
name: blender
description: "Render and inspect Dark Arisen in Blender: levels, the open world, models, characters with a pose, motion clips. Visual evidence for every milestone, checked against the design documents."
argument-hint: "[what to render or inspect, e.g. 'Rexa cathedral district from the harbour' | 'asset sheet tree_a tree_b']"
user-invocable: true
allowed-tools: Read, Glob, Grep, Write, Bash
---

# Blender for the studio

Blender is the studio's eye. Any CCGS skill or agent that changes how the game looks or moves renders the result here
and compares it with the governing design document before it reports. The game's assets themselves are **not** made
in Blender: the C++ ArtKit writes them byte-identically (`materialize --check`), and outside material enters only
through the CC0 import pipeline (`import-glb`). Nothing edited by hand in Blender is committed as game content.

The scripts below are development tooling for the assistant. They live outside `Source/` and `Tools/`
(`Docs/NATIVE_CPP_POLICY.md`), the build never uses them, and a render is evidence of the look, never runtime proof
(the GameLauncher on a GPU host is).

## 1. Setup (idempotent)

1. Blender: `blender --version` (the container's apt package, 4.0+). If missing: `apt-get install -y blender`, or a
   Python venv with the `bpy` wheel (`python3 -m venv <scratch>/bpyenv && <scratch>/bpyenv/bin/pip install bpy==4.2.0`)
   and run the scripts with `<scratch>/bpyenv/bin/python <script> -- <args>`.
2. The ops tool: `cmake -S Tools/o3de -B build/ops -DCMAKE_BUILD_TYPE=Release && cmake --build build/ops`.
3. Content the renders read (git-ignored or fetched, never hand-made):
   - `build/ops/DarkArisenO3DE fetch-art`: the CC0 scans and the HDRI sky pinned in `ContentSource/ThirdParty/PolyHaven/Scanned.lock`;
   - `build/ops/DarkArisenO3DE decimate-art`: game-ready scans;
   - `build/ops/DarkArisenO3DE materialize`: levels, models, materials;
   - `build/ops/DarkArisenO3DE world-build`: the open world's tiles, plantings and settlements (`Assets/World`, ~360 MB, about a minute).

## 2. Commands

All positions are in the O3DE frame: metres, X east, Y north, Z up.

| What | Command |
|---|---|
| A shot (level, world or items) | `blender -b --factory-startup -P .claude/skills/blender/scripts/preview.py -- <shot.json> <out.png>` |
| A row of scanned assets | `blender -b --factory-startup -P .claude/skills/blender/scripts/assets_sheet.py -- <out.png> <id> [<id> ...]` |
| A motion clip as a contact sheet | `blender -b --factory-startup -P .claude/skills/blender/scripts/bvh_sheet.py -- <clip.bvh> <out.png> <t0> <t1> <count> [cols]` |

Write shot files and renders to the scratchpad; copy only the images worth keeping to `Docs/Images/<Area>/`.

### Shot file

```json
{
  "level": "L_RexaHarbor",
  "world": {"tiles": 5000, "plants": 900, "small": 200, "grass": 40, "coarse_biome": "tropical"},
  "items": [{"gltf": "Assets/Art/Characters/SK_Art_jake.gltf", "at": [x, y, z], "yaw": 90, "motion": "Assets/...gltf", "frame": 12, "scale": 1}],
  "hide": ["Jake"],
  "eye": [x, y, z], "target": [x, y, z], "hfov": 70,
  "jake": [x, y, z], "look": [x, y, z], "pitch": -10, "arm": 3.5,
  "sun": [azimuth, elevation], "sun_energy": 4.0,
  "hdri": "kloofendal_48d_partly_cloudy_puresky_4k.hdr", "hdri_strength": 0.8, "hdri_rot": 30,
  "sea": true, "wave": 0.4, "fog": 3e-05, "fog_extent": 40000, "fog_color": [0.8, 0.86, 0.92, 1],
  "engine": "CYCLES", "samples": 48, "res": [2560, 1440], "look_cm": "AgX - Punchy"
}
```

- `level` loads a materialised prefab. `world` streams the open world around the camera instead; its numbers are
  radii in metres per layer.
- Use either `eye`/`target` or the third-person rig (`jake`, `look`, `pitch`, `arm`).
- `items` places extra models, posed with a motion clip if one is given.
- For the open world, take each element's position and height from `Assets/World/WorldTiles.json`: tile structures
  are `[structure index, x cm, y cm, z cm, yaw]`, and `structures[i].model.hint` names the model.

## 3. Verdict

Every Blender check ends in exactly one verdict, with the image path and the design source:

| Verdict | Meaning | Rank |
|---|---|---|
| `DEPARTS` | The render ran and differs from the design document. List each difference. | highest: known problems first |
| `NOT ASSESSED — <reason>` | The render did not run or shows nothing usable. Give the reason: no Blender, scans not fetched, not materialised, world not built, crash, black or empty frame. | outranks a match |
| `MATCHES` | The render ran, the subject is in frame and lit, and it follows the named document. | lowest |

A check that could not render never reports `MATCHES`. If the log has no `WROTE <path>` line, or the image is
missing, the verdict is `NOT ASSESSED`.

## 4. Before a render is shown to anyone

- **Camera:** the eye is above the ground and outside buildings. Take ground heights from the tile data, or start
  30 m up and come down.
- **Light:** daylight checks use a sun elevation of 25° or more and a sun that lights the subject's front. A black
  frame or a black subject means the camera or the light is wrong; it is not the art.
- **Design document:** name the document and section the subject is built from, and list what in the frame departs
  from it.
- **Bar:** the owner's bar is AAA (RDR2, Elden Ring). Report what falls short plainly; never call a render finished
  because it rendered.
- **Long jobs:** run renders over about a minute in the background. If a world shot crashes Blender, shrink the
  `world` radii.

## 5. Use per milestone

| Milestone | Studio skill / agents | Blender check |
|---|---|---|
| M2 regions | `/team-level`; level-designer, art-director, world-builder | Aerial and street-level shots of every settlement and site, against its `docs/design/region_*.md` |
| M3 characters and creatures | technical-artist, art-director | Turntables of each actor and boss with skin, hair and cloth, against `docs/characters/` and `docs/design/bosses/` |
| M4 gameplay | `/team-combat`; gameplay-programmer, technical-artist | Posed stills and motion sheets of traversal and combat clips (climb, vault, roll, parry) |
| M5 main quests | `/team-narrative`; level-designer | Set-piece layouts of mission places in the world |
| M6 side quests, living world | `/team-narrative`, `/team-level` | NPCs posed at their schedule anchors in the settlements |
| M7 cutscenes and map | technical-artist, art-director | Stills of each cutscene's camera (static hold, slow push, anchored wide), against `cutscene catalog.md` |
| M8 audio | `/team-audio` | Acoustic spaces (cathedral, caves, docks) framed for reference |
| M9 voice and lip sync | technical-artist | Face shapes and visemes as a sheet |
| M10 polish | `/team-polish`; performance-analyst, technical-artist | A before-and-after pair for every polish change |
