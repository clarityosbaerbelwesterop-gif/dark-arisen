# Open-source 3D alpha batch: what was taken from the feature branch

Source: branch `feature/meshy-o3de-assets-2026-09-27` at `e5d73ca` (three commits on top of `4803258`,
authored by the repository owner account). The branch stays as it is; this directory records what was
integrated from it and why the rest was not.

## Integrated (byte-identical copies)

| Folder | Files | Size |
|---|---:|---:|
| `Engine/O3DE/DarkArisen/Assets/Art/Props/OpenSource3D` | 29 props (`SM_*.gltf`) | 6.7 MB |
| `Engine/O3DE/DarkArisen/Assets/Art/Lighting/OpenSource3D` | 4 light fixtures (`L_*.gltf`) | 0.7 MB |

The props are original work of the branch: deterministic, procedurally modelled, embedded-buffer glTF 2.0
with embedded grain/colour textures, PBR metallic/roughness, normals and UVs. They are stylised alpha
models, not final art. No third-party licence applies. The branch's Python generators produced them. They
are not copied, because project logic is native C++ (NATIVE_CPP_POLICY). The glTF files are committed as
source art, the same way as the art kit's output.

### Where they are used

The materialiser (`Tools/o3de/Ops/Materializer.cpp`, set dressing) places 14 of them next to the authored
routes, never on a route. The ground height comes from the level's own greybox triangles:

- **Rexa Harbor, quay and garrison levels** (`AddLandGround`): clusters along the quay edge every 9 m
  (crane, crates, barrels and kegs, rope with anchor, nets, cannonballs), harbour lanterns every 18 m and
  market stalls along the town rows. Garrison pairs (weapon rack, powder, cannonballs) stand 4 m off every
  other anchor on flat ground.
- **Moran route**: the Harlow wreckage and family debris (Driftwood Beach), the shelter and fire (Driftwood
  Camp), the boat work (Mira's Cove), Big Tom's field forge with anvil and brazier (Mangrove Shallows),
  Koa's chart table, supply stall and repair bench (Koa's Trading Post), and the approach (Galleon Cove).

The crane, market stall, anvil and chart table have box colliders. Everything else is dressing without
collision. The other 17 props (sabres, pistol, spyglass, compass, pocket watch, satchel, shackles, powder
bomb, boarding axe, hammock, capstan, ship wheel, ship lantern, swivel gun, cutlass, prison cage) are hand
props, ship fittings and set pieces. They wait for attachment points on the people and the ships, or for a
scene that calls for them.

The four light fixtures carry `KHR_lights_punctual`. O3DE's scene import does not create lights from
it, so for now they are meshes only. The Atom point-light values the branch specifies (harbour lantern
430 lm, 255/207/149; forge brazier 1400 lm, 255/141/70; wall torch 750 lm; lighthouse beacon) are to be
authored as light components once they can be checked in the O3DE Editor.

| Asset | Size | SHA-256 (first 16) |
|---|---:|---|
| `SM_anchor` | 98 KB | `6d7d04a4e9e3d3ff` |
| `SM_black_powder_keg` | 293 KB | `ef109db3a051d5ca` |
| `SM_boarding_axe` | 24 KB | `e1f786a1e745f95f` |
| `SM_cannonball_stack` | 337 KB | `000aeef28cd24002` |
| `SM_capstan` | 277 KB | `32cd3a7a33fd1b5a` |
| `SM_cargo_crate` | 38 KB | `7dc58f31a49e8d4a` |
| `SM_chart_table` | 64 KB | `ef68ce78bf1c0c61` |
| `SM_cutlass` | 411 KB | `f729cde007740151` |
| `SM_dock_crane` | 187 KB | `bdcc8a2904e842c2` |
| `SM_draven_saber` | 492 KB | `2c20e12e1cffbe86` |
| `SM_fishing_net` | 683 KB | `9eb7d77ab79c490e` |
| `SM_flintlock_pistol` | 121 KB | `733d462090d60f1d` |
| `SM_forge_anvil` | 20 KB | `5aeb97c8cedb8446` |
| `SM_hammock` | 53 KB | `e867a77dd80aed4c` |
| `SM_jake_saber` | 411 KB | `fa150bdb3d4d7980` |
| `SM_marine_compass` | 158 KB | `005d072607cbb0c3` |
| `SM_market_stall` | 33 KB | `7543a40f27f0b6ba` |
| `SM_medicine_satchel` | 36 KB | `4bb5264c60832c13` |
| `SM_oak_barrel` | 279 KB | `9ec9e99d5adde70c` |
| `SM_powder_bomb` | 163 KB | `4854f0ac831b8fa5` |
| `SM_prison_cage` | 113 KB | `b2d463ed28b7da4a` |
| `SM_rope_coil` | 660 KB | `d6d2c1ab54ac33ae` |
| `SM_salazar_pocket_watch` | 230 KB | `8a2d905adfe47d41` |
| `SM_shackles` | 180 KB | `cdc3c74de0d5562d` |
| `SM_ship_lantern` | 126 KB | `168f1cfe38c2a153` |
| `SM_ship_wheel` | 200 KB | `3c3fdc85358ed97c` |
| `SM_spyglass` | 630 KB | `e7f48164f24276ea` |
| `SM_swivel_gun` | 366 KB | `b5e1821aa4f684bb` |
| `SM_weapon_rack` | 29 KB | `3669542f05a372dd` |
| `L_forge_brazier` | 152 KB | `553e445386dd2fb4` |
| `L_harbor_lantern` | 128 KB | `5fd44ebd56e3d4b7` |
| `L_lighthouse_beacon` | 304 KB | `345012dcef152ab3` |
| `L_wall_torch` | 112 KB | `d1b03036795b856e` |

`Materialized.json` records every placed file as a materialiser input. A changed prop makes
`materialize --check` report its levels stale.

## Not integrated, and why

| Branch content | Reason |
|---|---|
| `SM_naval_cannon.gltf` and its raw download | Generated with the InstantMesh web demo from a reference image whose own source is not recorded. The Apache-2.0 licence covers the InstantMesh code, not the reference. It stays out until the reference's provenance is documented. |
| 24 region kits (`KIT_*.gltf`) | Each kit is three generic, reused modules (hut, table, palm, tower, gate and similar), not region art. The Quiet Coast kit even has a palm. The levels already use the art kit's region set pieces. |
| 10 ships | Cruder than the art kit's `SM_Art_LaLiberacion`, `SM_Art_HostileSloop` and `SM_Art_HarlowMerchantShip`, and without deck collision or buoyancy. |
| 18 characters and 14 human boss variants | Copies of the MakeHuman bodies with proximity-transferred weights. The branch itself notes that this deforms shoulders, clothes and hair. The human pipeline (`Tools/o3de/Ops/ArtKitHuman*.cpp`) already builds skinned actors with source weights for the cast. |
| 6 creature bosses | Unrigged silhouettes. |
| 11 texture maps, 3 ocean material variants | Nothing references them yet. The variants change only the ocean's colours, which the region's sea state should drive. |
| All Python generators and validators | NATIVE_CPP_POLICY. |

Nothing was bought. No external service was used for this integration.
