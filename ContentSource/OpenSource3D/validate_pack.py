#!/usr/bin/env python3
"""Check the 30 source-catalog props and write an honest production-stage manifest."""

import hashlib
import json
import math
import struct
from pathlib import Path

from generate_props import ASSETS, OUT, ROOT
from gltf_format import read_embedded_gltf

CATALOG = ROOT / 'ContentSource/Higgsfield/HiggsfieldO3DEAssetCatalog.json'
MANIFEST = Path(__file__).resolve().parent / 'AssetManifest.json'


def inspect(path):
    raw=path.read_bytes()
    d,binary=read_embedded_gltf(path)
    assert d['asset']['version']=='2.0'
    assert d['buffers'][0]['byteLength']<=len(binary)
    for view in d['bufferViews']:
        assert view.get('byteOffset',0)+view['byteLength']<=d['buffers'][view['buffer']]['byteLength']
    for a in d['accessors']:
        components={'SCALAR':1,'VEC2':2,'VEC3':3,'VEC4':4}[a['type']]
        itembytes={5121:1,5123:2,5125:4,5126:4}[a['componentType']]
        view=d['bufferViews'][a['bufferView']]
        assert a.get('byteOffset',0)+a['count']*components*itembytes<=view['byteLength']
    for img in d['images']:
        view=d['bufferViews'][img['bufferView']]
        assert binary[view.get('byteOffset',0):view.get('byteOffset',0)+8]==b'\x89PNG\r\n\x1a\n'
    triangles=0
    for mesh in d['meshes']:
        for p in mesh['primitives']:
            assert {'POSITION','NORMAL','TEXCOORD_0'}<=set(p['attributes'])
            assert p['material']<len(d['materials']) and p['mode']==4
            indices=d['accessors'][p['indices']]['count']
            assert indices%3==0
            triangles+=indices//3
            for kind in ('POSITION','NORMAL','TEXCOORD_0'):
                a=d['accessors'][p['attributes'][kind]]
                assert a['count']>0 and a['componentType']==5126
                if kind=='NORMAL':
                    v=d['bufferViews'][a['bufferView']]
                    off0=v.get('byteOffset',0)+a.get('byteOffset',0)
                    vals=struct.unpack_from('<'+str(a['count']*3)+'f',binary,off0)
                    assert all(math.isfinite(x) for x in vals)
            assert d['accessors'][p['attributes']['POSITION']]['count']>0
    assert triangles>0 and d['materials'] and d['images']
    for mat in d['materials']:
        idx=mat['pbrMetallicRoughness']['baseColorTexture']['index']
        assert d['textures'][idx]['source']<len(d['images'])
    clips=[]
    for anim in d.get('animations',[]):
        assert anim['channels'] and anim['samplers']
        for ch in anim['channels']:
            assert ch['sampler']<len(anim['samplers'])
            assert ch['target']['node']<len(d['nodes'])
        clips.append(anim['name'])
    return {'file':path.relative_to(ROOT).as_posix(),'bytes':len(raw),
            'sha256':hashlib.sha256(raw).hexdigest(),'triangles':triangles,
            'meshNodes':len(d['meshes']),'materials':len(d['materials']),
            'images':len(d['images']),'animations':clips}


def main():
    catalog=json.loads(CATALOG.read_text())
    props={a['id']:a for a in catalog['assets'] if a['profile']=='prop' and a['type']=='3d'}
    assert len(props)==30
    assert set(props)=={'prop.'+x for x in ASSETS}|{'prop.naval_cannon'}
    assets=[]
    for aid,item in props.items():
        slug=aid.removeprefix('prop.')
        data=inspect(OUT/f'SM_{slug}.gltf')
        data.update({'catalogId':aid,'name':item['name'],
                     'stage':'image_to_mesh_alpha' if slug=='naval_cannon' else 'procedural_alpha',
                     'source':'TencentARC/InstantMesh' if slug=='naval_cannon' else 'DarkArisen procedural generator',
                     'rigged':False,'pbrTextureDetail':'subtle grain, not authored UV material maps'})
        if slug=='naval_cannon':
            data['originalGlbSha256']='378c8387b2afd5144aa3fe4450fa9f3bd86c47c034378f7f2759234b900b8a36'
        assets.append(data)
    report={'schemaVersion':1,'catalog':'ContentSource/Higgsfield/HiggsfieldO3DEAssetCatalog.json',
            'sourceLicense':{'instantMesh':'Apache-2.0, https://github.com/TencentARC/InstantMesh/blob/main/LICENSE',
                             'code':'project authored'},
            'qualityGate':'Embedded glTF buffer, triangles, NORMAL, TEXCOORD_0, embedded PNG and material references verified; no O3DE Editor/Asset Processor available in this workspace',
            'animationPolicy':'Eight mechanical preview rotations/swing motions; no character rig, skin, or gameplay-ready animation.',
            'assets':assets,
            'counts':{'props':len(assets),'previewClips':sum(len(x['animations']) for x in assets),
                      'triangles':sum(x['triangles'] for x in assets),'bytes':sum(x['bytes'] for x in assets)}}
    MANIFEST.write_text(json.dumps(report,indent=2,ensure_ascii=False)+'\n')
    print(json.dumps(report['counts']))


if __name__=='__main__':main()
