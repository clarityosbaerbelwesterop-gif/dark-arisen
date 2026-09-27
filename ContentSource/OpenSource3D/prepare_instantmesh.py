#!/usr/bin/env python3
"""Add normals, UVs and a subtle PBR grain map to InstantMesh vertex-color GLB.

The color remains the source model's per-vertex color. This does not invent a
rig, split the carriage/barrel, or claim a photogrammetric texture bake.
"""

import hashlib
import json
from pathlib import Path

import numpy as np

from generate_props import OUT, ROOT, png
from gltf_format import embedded_gltf, read_embedded_gltf

SOURCE = ROOT / 'ContentSource/OpenSource3D/Raw/naval_cannon_instantmesh.gltf'
DEST = OUT / 'SM_naval_cannon.gltf'


def main():
    raw = SOURCE.read_bytes()
    doc,packed=read_embedded_gltf(SOURCE)
    binary=bytearray(packed)
    attrs = doc['meshes'][0]['primitives'][0]['attributes']
    def array(accessor_id, dtype, shape):
        a = doc['accessors'][accessor_id]
        v = doc['bufferViews'][a['bufferView']]
        off = v.get('byteOffset', 0) + a.get('byteOffset', 0)
        return np.frombuffer(binary, dtype=dtype, count=a['count']*shape, offset=off).reshape(-1, shape).copy()
    xyz = array(attrs['POSITION'], '<f4', 3)
    ix = array(doc['meshes'][0]['primitives'][0]['indices'], '<u4', 1).reshape(-1, 3)
    assert ix.max() < len(xyz) and len(ix)>1000
    a,b,c=xyz[ix[:,0]],xyz[ix[:,1]],xyz[ix[:,2]]
    fn=np.cross(b-a,c-a)
    normals=np.zeros_like(xyz)
    for k in range(3):np.add.at(normals,ix[:,k],fn)
    length=np.linalg.norm(normals,axis=1)
    normals /= np.maximum(length[:,None],1e-12)
    normals[length<1e-12] = (0,1,0)
    lo=xyz.min(axis=0);hi=xyz.max(axis=0)
    # Projection is only used for the restrained repeating grain. COLOR_0 carries
    # the authored appearance; the original color buffer remains untouched.
    uv=np.column_stack(((xyz[:,0]-lo[0])/(hi[0]-lo[0])*5,
                        (xyz[:,2]-lo[2])/(hi[2]-lo[2])*5)).astype('<f4')
    def add_bytes(data,target=None):
        while len(binary)%4:binary.append(0)
        off=len(binary);binary.extend(data)
        view={'buffer':0,'byteOffset':off,'byteLength':len(data)}
        if target:view['target']=target
        doc['bufferViews'].append(view)
        return len(doc['bufferViews'])-1
    nv=add_bytes(normals.astype('<f4').tobytes(),34962)
    tv=add_bytes(uv.tobytes(),34962)
    doc['accessors'].append({'bufferView':nv,'componentType':5126,'count':len(xyz),'type':'VEC3'})
    attrs['NORMAL']=len(doc['accessors'])-1
    doc['accessors'].append({'bufferView':tv,'componentType':5126,'count':len(xyz),'type':'VEC2'})
    attrs['TEXCOORD_0']=len(doc['accessors'])-1
    tex=png((.96,.96,.94),'cannon_grain')
    img=add_bytes(tex)
    doc['images']=[{'name':'neutral_surface_grain','bufferView':img,'mimeType':'image/png'}]
    doc['textures']=[{'source':0,'sampler':0}]
    doc['samplers']=[{'wrapS':10497,'wrapT':10497}]
    doc['materials']=[{'name':'iron_and_oak_vertex_colored','pbrMetallicRoughness':{
        'baseColorTexture':{'index':0},'metallicFactor':.25,'roughnessFactor':.72}}]
    doc['meshes'][0]['primitives'][0]['material']=0
    doc['meshes'][0]['name']='naval_cannon_instantmesh_alpha'
    doc['asset']['generator']='InstantMesh + DarkArisen normal/UV preparation'
    doc['buffers'][0]['byteLength']=len(binary)
    output=embedded_gltf(doc,bytes(binary))
    DEST.parent.mkdir(parents=True,exist_ok=True)
    DEST.write_bytes(output)
    record={'file':DEST.relative_to(ROOT).as_posix(),'bytes':len(output),'triangles':len(ix),
            'vertices':len(xyz),'meshNodes':1,'materials':1,'animations':[],
            'sha256':hashlib.sha256(output).hexdigest(),'sourceSha256':hashlib.sha256(raw).hexdigest(),
            'stage':'instantmesh_alpha','bounds':[lo.tolist(),hi.tolist()],
            'limitations':['single fused mesh','vertex-color primary appearance','no articulation or collision authoring','scale/axis requires O3DE visual review']}
    print(json.dumps(record,indent=2))


if __name__=='__main__':main()
