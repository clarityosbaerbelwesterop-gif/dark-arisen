#!/usr/bin/env python3
"""Fail-closed visual source audit for the playable Alpha release candidate."""
import argparse,json,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
REQUIRED={
 "ContentSource/Characters/Jake/SK_Jake_Alpha.gltf":(20000,1,1),
 "ContentSource/Characters/Boarders/SK_Boarder_Alpha.gltf":(8000,1,1),
 "ContentSource/Ships/Harlow/SM_HarlowMerchantShip_Alpha.gltf":(12000,1,1),
 "ContentSource/Ships/LaLiberacion/SM_LaLiberacion_Alpha.gltf":(20000,1,1),
 "ContentSource/Ships/Draven/SM_DravenRaidSilhouette_Alpha.gltf":(12000,1,1),
}
def mesh_stats(path):
 d=json.loads(path.read_text(encoding="utf-8"))
 access=d.get("accessors",[])
 pos=0
 for mesh in d.get("meshes",[]):
  for prim in mesh.get("primitives",[]):
   idx=prim.get("attributes",{}).get("POSITION")
   if isinstance(idx,int) and 0<=idx<len(access): pos+=int(access[idx].get("count",0))
 textured=bool(d.get("images") or d.get("textures"))
 pbr=any("pbrMetallicRoughness" in m for m in d.get("materials",[]))
 normals=any("NORMAL" in p.get("attributes",{}) for m in d.get("meshes",[]) for p in m.get("primitives",[]))
 uvs=any("TEXCOORD_0" in p.get("attributes",{}) for m in d.get("meshes",[]) for p in m.get("primitives",[]))
 return pos,len(d.get("materials",[])),textured,pbr,normals,uvs
ap=argparse.ArgumentParser();ap.add_argument("--release",action="store_true");args=ap.parse_args()
issues=[]
for rel,(minv,minm,needtex) in REQUIRED.items():
 p=ROOT/rel
 if not p.is_file(): issues.append(f"MISSING {rel}");continue
 try:v,m,t,pbr,n,u=mesh_stats(p)
 except Exception as e:issues.append(f"INVALID {rel}: {e}");continue
 print(f"{rel}: vertices={v} materials={m} textured={t} pbr={pbr} normals={n} uv0={u}")
 if v<minv:issues.append(f"{rel}: {v} vertices < release floor {minv}")
 if m<minm:issues.append(f"{rel}: no authored material")
 if needtex and not t:issues.append(f"{rel}: no texture/image payload")
 if not (pbr and n and u):issues.append(f"{rel}: missing PBR/normals/UV0 release data")
if issues:
 print("\nVISUAL RELEASE BLOCKERS:")
 for x in issues:print(" - "+x)
 if args.release:sys.exit(1)
 print("\nReport-only mode: source/gameplay work may continue, but do not call this a no-slop release candidate.")
else:print("Visual source release gate passed.")
