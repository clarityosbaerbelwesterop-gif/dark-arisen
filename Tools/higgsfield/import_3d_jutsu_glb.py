#!/usr/bin/env python3
"""Convert an exact committed Higgsfield 3D Jutsu GLB into an embedded glTF source file.

No network access, generation, or billing occurs here. The caller must supply the exact GLB
artifact obtained from the recorded project/revision manifest.
"""
from __future__ import annotations
import argparse, base64, json, struct
from pathlib import Path

JSON_CHUNK=0x4E4F534A
BIN_CHUNK=0x004E4942

def parse_glb(path: Path):
    raw=path.read_bytes()
    if len(raw)<20: raise ValueError("GLB too small")
    magic,version,total=struct.unpack_from("<4sII",raw,0)
    if magic!=b"glTF" or version!=2 or total!=len(raw):
        raise ValueError("Not a complete GLB 2.0 artifact")
    offset=12; doc=None; binary=None
    while offset<len(raw):
        if offset+8>len(raw): raise ValueError("Truncated GLB chunk header")
        length,kind=struct.unpack_from("<II",raw,offset); offset+=8
        payload=raw[offset:offset+length]; offset+=length
        if len(payload)!=length: raise ValueError("Truncated GLB chunk")
        if kind==JSON_CHUNK: doc=json.loads(payload.decode("utf-8").rstrip("\x00 "))
        elif kind==BIN_CHUNK: binary=payload
    if not isinstance(doc,dict) or binary is None: raise ValueError("GLB JSON/BIN chunks missing")
    buffers=doc.get("buffers",[])
    if len(buffers)!=1: raise ValueError("Importer currently requires one GLB binary buffer")
    if int(buffers[0].get("byteLength",-1))>len(binary): raise ValueError("Binary buffer shorter than declared")
    return raw,doc,binary

def stats(doc):
    access=doc.get("accessors",[])
    positions=0; normals=False; uv0=False
    for mesh in doc.get("meshes",[]):
        for prim in mesh.get("primitives",[]):
            attrs=prim.get("attributes",{})
            idx=attrs.get("POSITION")
            if isinstance(idx,int) and 0<=idx<len(access): positions+=int(access[idx].get("count",0))
            normals|="NORMAL" in attrs; uv0|="TEXCOORD_0" in attrs
    return {"positionAccessorVertices":positions,"meshes":len(doc.get("meshes",[])),
            "materials":len(doc.get("materials",[])),"images":len(doc.get("images",[])),
            "textures":len(doc.get("textures",[])),"normals":normals,"uv0":uv0}

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("input",type=Path); ap.add_argument("output",type=Path)
    ap.add_argument("--expected-size",type=int)
    ap.add_argument("--min-vertices",type=int,default=0)
    args=ap.parse_args()
    raw,doc,binary=parse_glb(args.input)
    if args.expected_size is not None and len(raw)!=args.expected_size:
        raise SystemExit(f"size mismatch: {len(raw)} != {args.expected_size}")
    s=stats(doc)
    if s["positionAccessorVertices"]<args.min_vertices:
        raise SystemExit(f"vertex floor failed: {s['positionAccessorVertices']} < {args.min_vertices}")
    if not (s["materials"] and s["images"] and s["textures"] and s["normals"] and s["uv0"]):
        raise SystemExit(f"release source attributes incomplete: {s}")
    doc["buffers"][0]["uri"]="data:application/octet-stream;base64,"+base64.b64encode(binary).decode("ascii")
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(doc,separators=(",",":")),encoding="utf-8")
    print(json.dumps({"inputBytes":len(raw),"outputBytes":args.output.stat().st_size,**s},sort_keys=True))
if __name__=="__main__": main()
