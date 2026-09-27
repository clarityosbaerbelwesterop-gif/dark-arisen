#!/usr/bin/env python3
"""Structural checks for world, creature, humanoid, weapon, light and water alpha assets."""

import hashlib
import json
from pathlib import Path

from generate_props import ROOT
from gltf_format import read_embedded_gltf
from validate_pack import inspect

HERE=Path(__file__).resolve().parent


def check_humanoid(record):
    path=ROOT/record['file'];doc,binary=read_embedded_gltf(path)
    assert hashlib.sha256(path.read_bytes()).hexdigest()==record['sha256']
    assert len(doc['skins'])==1 and len(doc['skins'][0]['joints'])==163
    assert doc['nodes'][0]['skin']==0
    attributes=doc['meshes'][0]['primitives'][0]['attributes']
    assert {'POSITION','NORMAL','TEXCOORD_0','JOINTS_0','WEIGHTS_0'}<=set(attributes)
    assert len(doc['animations'])==3
    assert [x['name'] for x in doc['animations']]==record['clips']
    names={node.get('name'):i for i,node in enumerate(doc['nodes'])}
    for name in ('head','spine02','wrist.R'):
        assert any(child>=164 for child in doc['nodes'][names[name]].get('children',[])),name
    for image in doc['images']:
        if 'uri' in image:
            assert (path.parent/image['uri']).exists(),(path,image['uri'])
        else:
            view=doc['bufferViews'][image['bufferView']]
            assert binary[view['byteOffset']:view['byteOffset']+8]==b'\x89PNG\r\n\x1a\n'
    for a in doc['accessors']:
        v=doc['bufferViews'][a['bufferView']]
        item={5121:1,5123:2,5125:4,5126:4}[a['componentType']]
        dim={'SCALAR':1,'VEC2':2,'VEC3':3,'VEC4':4,'MAT4':16}[a['type']]
        assert v.get('byteOffset',0)+v['byteLength']<=len(binary)
        assert a.get('byteOffset',0)+a['count']*item*dim<=v['byteLength']
    for anim in doc['animations']:
        for ch in anim['channels']:
            assert ch['target']['node'] in doc['skins'][0]['joints']
            assert ch['sampler']<len(anim['samplers'])
    assert doc['buffers'][0]['byteLength']==len(binary)


def check_hero(record):
    path=ROOT/record['file'];doc,binary=read_embedded_gltf(path)
    assert hashlib.sha256(path.read_bytes()).hexdigest()==record['sha256']
    assert len(doc['skins'])==1 and len(doc['skins'][0]['joints'])==163
    assert doc['nodes'][0]['skin']==0 and len(doc['animations'])==3
    assert len({n.get('name') for n in doc['nodes'][1:164]})==163
    assert len(record['clips'])==3 and [x['name'] for x in doc['animations']]==record['clips']
    for p in doc['meshes'][0]['primitives']:
        a=p['attributes']
        assert {'POSITION','NORMAL','TEXCOORD_0','JOINTS_0','WEIGHTS_0'}<=set(a)
        assert len({doc['accessors'][a[k]]['count'] for k in ('POSITION','JOINTS_0','WEIGHTS_0')})==1
        assert doc['accessors'][a['JOINTS_0']]['componentType']==5123
        assert doc['accessors'][a['WEIGHTS_0']]['componentType']==5126
    for image in doc['images']:
        assert (path.parent/image['uri']).exists(),(path,image['uri'])
    assert len({v for v in doc['scenes'][0]['nodes']})==2
    assert set(doc['skins'][0]['joints'])==set(range(1,164))
    for anim in doc['animations']:
        for ch in anim['channels']:
            assert ch['target']['node'] in doc['skins'][0]['joints']
            assert ch['target']['path'] in ('rotation','translation','scale')
    for a in doc['accessors']:
        v=doc['bufferViews'][a['bufferView']]
        item={5121:1,5123:2,5125:4,5126:4}[a['componentType']]
        dim={'SCALAR':1,'VEC2':2,'VEC3':3,'VEC4':4,'MAT4':16}[a['type']]
        assert v.get('byteOffset',0)+v['byteLength']<=len(binary)
        assert a.get('byteOffset',0)+a['count']*item*dim<=v['byteLength']
    assert doc['buffers'][0]['byteLength']==len(binary)
    assert record['transferP95Meters']<.3


def main():
    catalog=json.loads((ROOT/'ContentSource/Higgsfield/HiggsfieldO3DEAssetCatalog.json').read_text())
    world=json.loads((HERE/'WorldManifest.json').read_text())['kits']
    bosses=json.loads((HERE/'BossManifest.json').read_text())
    weapons=json.loads((HERE/'WeaponExtrasManifest.json').read_text())['weapons']
    lighting=json.loads((HERE/'LightingManifest.json').read_text())['fixtures']
    surface=json.loads((HERE/'SurfaceManifest.json').read_text())
    props=json.loads((HERE/'AssetManifest.json').read_text())['assets']
    heroes=json.loads((HERE/'HeroManifest.json').read_text())['heroes']
    ships=json.loads((HERE/'ShipManifest.json').read_text())['ships']
    all_catalog=props+world+bosses['humanoids']+bosses['creatures']+heroes+ships
    ids=[r['catalogId'] for r in all_catalog]
    model_ids={a['id'] for a in catalog['assets'] if not a['id'].startswith('audio.')}
    assert len(ids)==len(set(ids))==len(model_ids)==102
    assert set(ids)==model_ids
    assert len(world)==24 and len(bosses['humanoids'])==14 and len(bosses['creatures'])==6
    assert len(heroes)==18 and len(ships)==10
    assert {x['catalogId'] for x in world}=={a['id'] for a in catalog['assets'] if a['id'].startswith('environment.')}
    assert {x['catalogId'] for x in bosses['humanoids']+bosses['creatures']}=={a['id'] for a in catalog['assets'] if a['id'].startswith('boss.')}
    for record in world+bosses['creatures']+weapons+lighting:
        path=ROOT/record['file']
        value=inspect(path)
        assert value['sha256']==record['sha256'],path
        assert record['meshNodes']==value['meshNodes'] and record['triangles']==value['triangles']
        if record in world:
            doc,_=read_embedded_gltf(path)
            groups=[n for n in doc['nodes'] if 'mesh' in n]
            assert len(groups)==3 and len(record['pieces'])==3
        if record in lighting:
            doc,_=read_embedded_gltf(path)
            assert 'KHR_lights_punctual' in doc['extensionsUsed']
            assert len(doc['extensions']['KHR_lights_punctual']['lights'])==1
            assert any('KHR_lights_punctual' in n.get('extensions',{}) for n in doc['nodes'])
    for record in bosses['humanoids']:check_humanoid(record)
    assert len({r['sourcePerson'] for r in heroes})==18
    for record in heroes:check_hero(record)
    for record in ships:
        path=ROOT/record['file']
        value=inspect(path)
        assert value['sha256']==record['sha256']
        assert value['triangles']==record['triangles'] and value['meshNodes']==record['meshNodes']
        doc,_=read_embedded_gltf(path)
        mesh_names={n.get('name') for n in doc['nodes'] if 'mesh' in n}
        assert 'hull' in mesh_names and 'rudder' in mesh_names
        assert len([name for name in mesh_names if name.startswith('sail_')])==record['mastCount']
        assert len(doc['animations'])==1+record['mastCount']
        assert [a['name'] for a in doc['animations']]==record['animations']
    assert len(surface['textures'])==11 and len(surface['oceanMaterials'])==3
    for path in surface['textures']:
        raw=(ROOT/path).read_bytes()
        assert raw[:8]==b'\x89PNG\r\n\x1a\n'
        import struct
        assert struct.unpack_from('>II',raw,16)==(256,256)
    ocean=ROOT/'Engine/O3DE/DarkArisen/Assets/Materials/DarkArisenOcean.material'
    base=json.loads(ocean.read_text())
    assert len([k for k in base['propertyValues'] if k.startswith('waves.')])==16
    for path in surface['oceanMaterials']:
        doc=json.loads((ROOT/path).read_text())
        assert doc['materialType']==base['materialType']
        assert all(doc['propertyValues'][k]==v for k,v in base['propertyValues'].items())
        assert {'surface.deepColor','surface.shallowColor','surface.skyColor','surface.specularStrength'}<=set(doc['propertyValues'])
    print(json.dumps({'worldKits':len(world),'creatureBosses':len(bosses['creatures']),
        'riggedHumanoidBosses':len(bosses['humanoids']),'humanBossClips':sum(len(x['clips']) for x in bosses['humanoids']),
        'weapons':len(weapons),'lights':len(lighting),'surfaceTextures':len(surface['textures']),
        'oceanMaterials':len(surface['oceanMaterials']),'ships':len(ships),
        'skinnedCharacters':len(heroes),'characterClips':sum(len(x['clips']) for x in heroes),
        'catalogCovered':len(all_catalog)}))


if __name__=='__main__':main()
