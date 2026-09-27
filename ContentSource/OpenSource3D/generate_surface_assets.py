#!/usr/bin/env python3
"""Tileable alpha material textures and existing Atom ocean material variants."""

import json
import math
import struct
import zlib

from generate_props import ROOT

SIZE=256
TEXTURES=ROOT/'Engine/O3DE/DarkArisen/Assets/Art/Textures/OpenSource3D'
MATERIALS=ROOT/'Engine/O3DE/DarkArisen/Assets/Materials'


def noise(x,y,seed):
    t=2*math.pi/SIZE
    return (.48+.22*math.sin(t*(4*x+2*y)+seed)+.14*math.cos(t*(9*x-7*y)+seed*2)
            +.1*math.sin(t*(21*x+13*y)+seed*3))


def values(kind):
    arr=[]
    for y in range(SIZE):
        row=[]
        for x in range(SIZE):
            h=noise(x,y,{'wood':1,'stone':2,'bark':3,'foam':4,'caustics':5,'ash':6,'lava':7,'flame':8}[kind])
            if kind=='wood':h+=.16*math.sin(2*math.pi*x*14/SIZE+math.sin(y*.06))
            if kind=='bark':h+=.13*math.sin(2*math.pi*x*23/SIZE+math.sin(y*.08))
            if kind=='stone':h+=.09*math.sin(2*math.pi*(x*3+y*5)/SIZE)
            if kind=='foam':h=max(0,min(1,(h-.38)*2.8))
            if kind=='caustics':h=max(0,math.sin(2*math.pi*x*7/SIZE+math.cos(y*.08))*math.sin(2*math.pi*y*5/SIZE+math.sin(x*.04)))*.65+.1
            if kind=='lava':h=max(0,min(1,(h-.45)*5))
            if kind=='flame':h=max(0,min(1,(h-.24)*2.8))
            row.append(h)
        arr.append(row)
    return arr


def png_rgb(rows):
    payload=bytearray()
    for row in rows:
        payload.append(0)
        for pixel in row:payload.extend(max(0,min(255,int(round(v)))) for v in pixel)
    def chunk(t,b):return struct.pack('>I',len(b))+t+b+struct.pack('>I',zlib.crc32(t+b)&0xffffffff)
    return b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('>IIBBBBB',SIZE,SIZE,8,2,0,0,0))+chunk(b'IDAT',zlib.compress(payload,9))+chunk(b'IEND',b'')


def output(name,rows):
    TEXTURES.mkdir(parents=True,exist_ok=True)
    path=TEXTURES/name
    path.write_bytes(png_rgb(rows))
    return path


def base_map(kind,base,variation,name):
    hs=values(kind)
    return output(name,[[tuple(v+variation*(h-.5) for v in base) for h in row] for row in hs])


def normal_map(kind,name):
    hs=values(kind)
    rows=[]
    for y in range(SIZE):
        row=[]
        for x in range(SIZE):
            dx=(hs[y][(x+1)%SIZE]-hs[y][(x-1)%SIZE])*.75
            dy=(hs[(y+1)%SIZE][x]-hs[(y-1)%SIZE][x])*.75
            z=math.sqrt(max(0,1-dx*dx-dy*dy))
            row.append(((1-dx)*127.5,(1-dy)*127.5,(1+z)*127.5))
        rows.append(row)
    return output(name,rows)


def main():
    paths=[
      base_map('wood',(107,68,36),90,'T_WeatheredOak_BaseColor.png'),
      normal_map('wood','T_WeatheredOak_Normal.png'),
      base_map('stone',(83,83,78),80,'T_SaltStone_BaseColor.png'),
      normal_map('stone','T_SaltStone_Normal.png'),
      base_map('bark',(69,76,47),90,'T_MangroveBark_BaseColor.png'),
      normal_map('bark','T_MangroveBark_Normal.png'),
      base_map('ash',(40,39,36),60,'T_VolcanicAsh_BaseColor.png'),
      base_map('foam',(225,233,223),120,'T_SeaFoam_Mask.png'),
      base_map('caustics',(40,125,143),100,'T_ShallowCaustics_BaseColor.png'),
      base_map('lava',(197,53,8),150,'T_LavaGlow_Emissive.png'),
      base_map('flame',(244,140,25),90,'T_LanternFlame_Emissive.png'),
    ]
    original=json.loads((MATERIALS/'DarkArisenOcean.material').read_text())
    variants={
      'CoastMorning':{'deepColor':[.02,.13,.16],'shallowColor':[.08,.33,.31],'skyColor':[.52,.68,.75],'specularStrength':3.6},
      'StormNight':{'deepColor':[.007,.018,.038],'shallowColor':[.042,.08,.12],'skyColor':[.12,.18,.24],'specularStrength':1.8},
      'VolcanicAsh':{'deepColor':[.014,.022,.025],'shallowColor':[.16,.13,.105],'skyColor':[.32,.27,.23],'specularStrength':2.4},
    }
    for name,surface in variants.items():
        material=json.loads(json.dumps(original))
        material['propertyValues'].update({'surface.'+k:v for k,v in surface.items()})
        (MATERIALS/f'DarkArisenOcean_{name}.material').write_text(json.dumps(material,indent=2)+'\n')
    manifest={'schemaVersion':1,'textureSize':[SIZE,SIZE],'textures':[p.relative_to(ROOT).as_posix() for p in paths],
              'oceanMaterials':[f'Engine/O3DE/DarkArisen/Assets/Materials/DarkArisenOcean_{n}.material' for n in variants],
              'limitations':['material presets reuse existing shaders and material type; weather/runtime update must choose a preset',
                             'foam and caustics are source textures, not wired into the ocean shader']}
    (ROOT/'ContentSource/OpenSource3D/SurfaceManifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
    print(len(paths),len(variants))


if __name__=='__main__':main()
