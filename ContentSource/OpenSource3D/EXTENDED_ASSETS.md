# Additional O3DE alpha art

This expansion stays on the same feature branch and leaves existing greybox,
collision meshes, ship simulation and character bases intact.

| Category | Added source files | Character of the work |
|---|---:|---|
| Region kits | 24 `.gltf` | Three individually named, separated modules per kit: buildings, fortifications, wreckage, piers, jungle or volcanic nature. Each ID matches the 24 `environment.*` catalog entries. These are props, **not** complete levels. |
| Bosses | 14 `.gltf` skinned humans + 6 creature `.gltf` | Humans reuse the repository's MakeHuman boarder topology and its 163-joint skeleton, with palette and attached head/shoulder/weapon elements. Each embeds the existing CMU idle, walk and swordplay clips. Creatures are distinct **unrigged alpha silhouettes** with small node-motion previews. All 20 `boss.*` catalog IDs are represented. |
| Additional weapons | 6 `.gltf` | Pike, harpoon, cleaver, doctor knife, grappling hook and forge hammer pickup silhouettes, alongside the original 30-prop batch's weapons. |
| Lighting | 4 `.gltf` | Emissive torch, harbor lantern, brazier and lighthouse source models, with `KHR_lights_punctual`. `LightingManifest.json` specifies manual Atom Light component values if O3DE's scene import does not create the lights. |
| Texture maps | 11 `.png`, 256² | Tileable base color / normal map pairs for oak, salt stone and mangrove bark; ash, foam, shallow caustic and emissive lava/flame source maps. The latter are **not** connected to the ocean shader. |
| Water | 3 `.material` | Coast morning, storm night and volcanic ash variants of the existing `DarkArisenOcean.materialtype`. The 16 authored wave constants remain the same, while color and specular values differ. Runtime weather and wave synchronization still belong to `OceanComponent`. |
| Characters | 18 `.gltf` skinned alpha variants | 18 different existing MakeHuman person meshes retain their source textures and are assigned approximate weights from the 163-joint Jake rest pose. Each includes existing CMU idle, walk and swordplay previews (54 clips in total). A proximity transfer can deform shoulders, clothes or hair poorly; per-asset transfer-distance statistics are in `HeroManifest.json`. |
| Ships | 10 `.gltf` | Distinct lengths, masts, gun-port counts and roles for all 10 `ship.*` catalog IDs. Named hull, sails and rudder nodes have 27 simple rotation previews in total. These are visual hulls, not collision, buoyancy or integrated ship simulation. |

From the repository root:

```bash
python ContentSource/OpenSource3D/generate_world.py
python ContentSource/OpenSource3D/generate_bosses.py
python ContentSource/OpenSource3D/generate_weapons.py
python ContentSource/OpenSource3D/generate_lighting.py
python ContentSource/OpenSource3D/generate_surface_assets.py
python ContentSource/OpenSource3D/generate_heroes.py  # NumPy + SciPy required
python ContentSource/OpenSource3D/generate_ships.py
python ContentSource/OpenSource3D/render_new_previews.py  # Pillow + NumPy required
python ContentSource/OpenSource3D/validate_pack.py
python ContentSource/OpenSource3D/validate_extended.py
```

`WorldManifest.json`, `BossManifest.json`, `WeaponExtrasManifest.json`,
`LightingManifest.json`, `SurfaceManifest.json`, `HeroManifest.json` and
`ShipManifest.json` identify every file and
record its stage. The visual sheets are in `Docs/Art/OpenSource3D_*_Preview.png`.
Every generated mesh is an embedded-buffer glTF 2.0 file (plain Git, matching
the existing O3DE art kit). Boss human base textures point to the repository's
existing `Assets/Art/Textures` folder; the validator checks these paths.

**Before gameplay use:** Open these files in the O3DE 2605.0 Editor, run the
Asset Processor, inspect scale/orientation and humanoid attachments in motion,
author collision/LOD and boss-specific phases/attacks, and test lighting and
water on the actual GPU. In particular, a 163-bone human boss with generic CMU
clips is **not** a finished unique boss encounter. Light and water assets have
source data but their final prefab/entity placement and effects hookup have
not been performed in this environment. Character skin-weight transfer needs
animation-pose review and likely repainting before gameplay. Ships need rigged
cloth, authored buoyancy/collision, deck walkability and a ship component hookup.
All **102 non-audio catalog IDs** now have source-art alpha entries. The ten
`audio.*` catalog IDs are outside this 3D pack and remain separate work.

Original human geometry is the existing MakeHuman CC0 art kit. The embedded
idle/walk/swordplay motions derive from the project's CMU source files; keep
the acknowledgement in `ContentSource/ThirdParty/README.md` with releases.
InstantMesh provenance and license for the cannon are in `README.md`.
