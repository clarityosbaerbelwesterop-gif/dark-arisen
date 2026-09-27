# Dark Arisen: open-source 3D prop batch (alpha)

This batch covers **all 30 `prop.*` entries** in
`ContentSource/Higgsfield/HiggsfieldO3DEAssetCatalog.json`. Each file is a
standalone glTF 2.0 source under
`Engine/O3DE/DarkArisen/Assets/Art/Props/OpenSource3D/SM_*.gltf`. Binary
buffers and PNG materials are embedded; no sidecar files or Git LFS objects
are needed. This matches the project's existing plain-Git `.gltf` convention.

- 29 props were authored as distinct, deterministic, modular meshes using the
  checked-in Python generator. They have embedded procedural grain/color maps,
  PBR metallic/roughness settings, normals and UVs. They are **stylized alpha
  models**, not realistic, final art.
- `SM_naval_cannon.gltf` comes from the [InstantMesh](https://github.com/TencentARC/InstantMesh)
  official Hugging Face demo, using the original transparent reference in
  `References/naval_cannon_6_pounder.png`. `Raw/naval_cannon_instantmesh.gltf`
  preserves the exact downloaded **geometry and vertex colors**, converted
  from its GLB container to embedded glTF. Original download SHA-256:
  `378c8387b2afd5144aa3fe4450fa9f3bd86c47c034378f7f2759234b900b8a36`.
  Its source appearance is vertex color;
  the preparation script adds computed normals, projected UVs and restrained
  surface grain. It is a **single fused mesh**, with no barrel recoil or wheel
  articulation. InstantMesh's published license is
  [Apache-2.0](https://github.com/TencentARC/InstantMesh/blob/main/LICENSE).
- Eight objects include simple node-transform preview animation clips: swivel
  gun crank, wheel, capstan, swinging lantern, crane hook, prison door, watch
  hands and compass needle. These are **mechanical previews**, not gameplay
  animation or humanoid skeletal rigs. Static objects have no animation.

From the repository root, reproducibly build and validate the batch:

```bash
python ContentSource/OpenSource3D/generate_props.py
python ContentSource/OpenSource3D/prepare_instantmesh.py  # requires NumPy
python ContentSource/OpenSource3D/validate_pack.py
```

`AssetManifest.json` records each catalog ID, file size, SHA-256, triangle
count, material/image count, and embedded clip names. The validation script
checks the embedded buffer bounds, mesh/material/texture references, triangles,
normals, UVs and animation channels. The visual contact sheet is
`Docs/Art/OpenSource3D_30Props_Preview.png`.

The next batch of **24 region kits, 20 boss first passes, six extra weapons,
four lights, eleven texture maps and three ocean material variants** is
described in `EXTENDED_ASSETS.md` and its separate manifests.

**Integration gate:** These files are new source art and do not overwrite
existing `Greybox`, collisions, skeletons or gameplay references. Import and
inspect them in the O3DE 2605.0 project, then author collision/LOD, set final
scale and origin, check PBR and vertex-color interpretation, choose animation
playback rates, and replace references only after that review. O3DE Editor and
Asset Processor were unavailable in this build environment; successful native
asset compilation and in-game appearance are not claimed. The remaining
catalog includes 32 hero/boss/NPC figures, 24 environment kits, 10 ships and
6 creatures; the extended batch provides source art for those 24 environments
and 20 bosses. The ten ships and 18 non-boss characters remain catalog work.
The repo's existing MakeHuman and motion assets are reused for human boss
alpha variants, rather than treated as finished bespoke encounters.
