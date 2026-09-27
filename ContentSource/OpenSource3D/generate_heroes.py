#!/usr/bin/env python3
"""Retarget distinct existing MakeHuman static bodies to the Jake alpha skeleton.

SciPy's cKDTree assigns the closest skinned rest-pose Jake vertex to each
source vertex. This is an approximation for preview animation only, not
artist-authored deformation weights or character-specific performances.
"""

import copy
import hashlib
import json

import numpy as np
from scipy.spatial import cKDTree

from generate_bosses import MOTIONS, append_binary
from generate_props import ROOT
from gltf_format import embedded_gltf, read_embedded_gltf


ART=ROOT/'Engine/O3DE/DarkArisen/Assets/Art'
HERE=ROOT/'ContentSource/OpenSource3D'
DEST=ART/'Characters/Heroes/OpenSource3D'
SOURCES={
    'jake_harlow':'jake', 'ethan_harlow':'ethan', 'draven_voss':'draven',
    'mira':'mira', 'big_tom':'bigtom', 'esteban':'esteban',
    'ines':'townswoman', 'father_salvio':'marc', 'koa':'koa',
    'old_bones':'fisherman', 'don_mateo_salazar':'herrera',
    'crimson_boarder_light':'albion_soldier',
    'crimson_boarder_heavy':'imperial_soldier',
    'crimson_sharpshooter':'sharpshooter',
    'rexa_civilian_male':'dockworker',
    'rexa_civilian_female':'market_woman',
    'moran_survivor':'sailor', 'dream_ethan':'dream_ethan',
}


def accessor_array(doc, raw, index):
    a=doc['accessors'][index]
    view=doc['bufferViews'][a['bufferView']]
    assert 'byteStride' not in view and 'sparse' not in a
    dtype={5121:np.dtype('u1'),5123:np.dtype('<u2'),5125:np.dtype('<u4'),5126:np.dtype('<f4')}[a['componentType']]
    size={'SCALAR':1,'VEC2':2,'VEC3':3,'VEC4':4,'MAT4':16}[a['type']]
    offset=view.get('byteOffset',0)+a.get('byteOffset',0)
    return np.frombuffer(raw,dtype=dtype,count=a['count']*size,offset=offset).reshape(-1,size)


def append_array(doc, binary, values, component, kind):
    while len(binary)%4:binary.append(0)
    view=len(doc['bufferViews'])
    payload=values.tobytes(order='C')
    doc['bufferViews'].append({'buffer':0,'byteOffset':len(binary),'byteLength':len(payload),'target':34962})
    binary.extend(payload)
    index=len(doc['accessors'])
    doc['accessors'].append({'bufferView':view,'componentType':component,'count':len(values),'type':kind})
    return index


def build(slug, base, donor, donor_raw, joints, weights, tree, clips):
    doc,original=read_embedded_gltf(ART/f'Models/SM_Art_Person_{base}.gltf')
    binary=bytearray(original)
    doc['nodes'][0]['name']='SK_Character_'+slug
    doc['nodes'][0]['skin']=0
    for image in doc['images']:
        if image.get('uri','').startswith('../Textures/'):
            image['uri']=image['uri'].replace('../Textures/','../../../Textures/',1)
    distances=[]
    for prim in doc['meshes'][0]['primitives']:
        p=accessor_array(doc,original,prim['attributes']['POSITION']).astype(np.float32)
        distance,lookup=tree.query(p,workers=1)
        distances.append(distance)
        chosen_joints=joints[lookup].astype('<u2',copy=False)
        chosen_weights=weights[lookup].astype('<f4',copy=False)
        assert chosen_joints.max()<163 and np.allclose(chosen_weights.sum(axis=1),1,atol=2e-4)
        prim['attributes']['JOINTS_0']=append_array(doc,binary,chosen_joints,5123,'VEC4')
        prim['attributes']['WEIGHTS_0']=append_array(doc,binary,chosen_weights,5126,'VEC4')

    start=len(doc['nodes'])
    def remap(old):
        assert 1<=old<=163
        return start+old-1
    for node in donor['nodes'][1:]:
        item=copy.deepcopy(node)
        if 'children' in item:item['children']=[remap(i) for i in item['children']]
        doc['nodes'].append(item)
    doc['scenes'][0]['nodes'].append(remap(donor['skins'][0]['skeleton']))
    ibm=donor['skins'][0]['inverseBindMatrices']
    a=donor['accessors'][ibm]
    viewbase,accessbase,_=append_binary(doc,binary,[donor['bufferViews'][a['bufferView']]],[a])
    doc['accessors'][accessbase]['bufferView']=viewbase
    binary.extend(donor_raw)
    skin=copy.deepcopy(donor['skins'][0])
    skin['name']='SK_Character_'+slug+'_Skeleton'
    skin['joints']=[remap(x) for x in skin['joints']]
    skin['skeleton']=remap(skin['skeleton'])
    skin['inverseBindMatrices']=accessbase
    doc['skins']=[skin]
    animation_names=[]
    for label,motion_id in MOTIONS.items():
        clip,clip_raw=clips[motion_id]
        assert [node['name'] for node in clip['nodes']]==[node['name'] for node in doc['nodes'][start:start+163]]
        _,accbase,_=append_binary(doc,binary,clip['bufferViews'],clip['accessors'])
        binary.extend(clip_raw)
        animation=copy.deepcopy(clip['animations'][0])
        animation['name']=label+'_'+motion_id+'_CMU_preview'
        for sampler in animation['samplers']:
            sampler['input']+=accbase
            sampler['output']+=accbase
        for channel in animation['channels']:
            channel['target']['node']=remap(channel['target']['node']+1)
        doc.setdefault('animations',[]).append(animation)
        animation_names.append(animation['name'])
    doc['asset']['generator']='DarkArisen MakeHuman approximate weight transfer (OpenSource3D alpha)'
    doc['buffers'][0]['byteLength']=len(binary)
    out=DEST/f'SK_Character_{slug}.gltf'
    raw=embedded_gltf(doc,bytes(binary))
    out.write_bytes(raw)
    distance=np.concatenate(distances)
    return {'catalogId':'character.'+slug,'file':out.relative_to(ROOT).as_posix(),
            'sourcePerson':'SM_Art_Person_'+base+'.gltf','skinDonor':'SK_Art_jake.gltf',
            'sha256':hashlib.sha256(raw).hexdigest(),'bytes':len(raw),
            'jointCount':163,'clips':animation_names,
            'transferMedianMeters':round(float(np.median(distance)),4),
            'transferP95Meters':round(float(np.percentile(distance,95)),4),
            'stage':'approximate_skin_transfer_alpha'}


def main():
    DEST.mkdir(parents=True,exist_ok=True)
    donor,donor_raw=read_embedded_gltf(ART/'Characters/SK_Art_jake.gltf')
    primitives=donor['meshes'][0]['primitives']
    position=np.concatenate([accessor_array(donor,donor_raw,p['attributes']['POSITION']) for p in primitives])
    joints=np.concatenate([accessor_array(donor,donor_raw,p['attributes']['JOINTS_0']) for p in primitives])
    weights=np.concatenate([accessor_array(donor,donor_raw,p['attributes']['WEIGHTS_0']) for p in primitives])
    assert len(position)==len(joints)==len(weights)
    tree=cKDTree(position)
    clips={id:read_embedded_gltf(ART/f'Characters/Motions/AN_Art_Human_{id}.gltf') for id in MOTIONS.values()}
    records=[build(slug,base,donor,donor_raw,joints,weights,tree,clips)
             for slug,base in SOURCES.items()]
    catalog=json.loads((ROOT/'ContentSource/Higgsfield/HiggsfieldO3DEAssetCatalog.json').read_text())
    assert {x['catalogId'] for x in records}=={x['id'] for x in catalog['assets'] if x['id'].startswith('character.')}
    assert len({x['sourcePerson'] for x in records})==18
    (HERE/'HeroManifest.json').write_text(json.dumps({'generator':'generate_heroes.py','heroes':records},indent=2)+'\n')
    print(json.dumps({'characters':len(records),'embeddedClips':sum(len(r['clips']) for r in records),
                      'maxP95TransferMeters':max(x['transferP95Meters'] for x in records)}))


if __name__=='__main__':main()
