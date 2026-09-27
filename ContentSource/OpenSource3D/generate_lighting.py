#!/usr/bin/env python3
"""Four emissive fixture meshes with glTF punctual lights and O3DE setup data."""

import hashlib
import json

from generate_props import Model, ROOT
from gltf_format import embedded_gltf, read_embedded_gltf

OUT=ROOT/'Engine/O3DE/DarkArisen/Assets/Art/Lighting/OpenSource3D'

PROFILES={
    'wall_torch':{'position':[0,1.42,.22],'candela':65,'rgb':[1.0,.49,.16],
                  'o3de':{'type':'Point (simple punctual)','intensityMode':'Lumen','intensity':750,'color':[255,178,93]}},
    'harbor_lantern':{'position':[0,2.31,0],'candela':38,'rgb':[1.0,.67,.33],
                      'o3de':{'type':'Point (simple punctual)','intensityMode':'Lumen','intensity':430,'color':[255,207,149]}},
    'forge_brazier':{'position':[0,1.24,0],'candela':105,'rgb':[1.0,.31,.08],
                     'o3de':{'type':'Point (sphere)','intensityMode':'Lumen','intensity':1400,'color':[255,141,70],'shape':'Sphere Shape'}},
    'lighthouse_beacon':{'position':[0,4.09,0],'candela':250,'rgb':[1.0,.84,.48],
                         'o3de':{'type':'Spot (simple punctual)','intensityMode':'Candela','intensity':250,'color':[255,232,181]}},
}


def geometry(slug):
    m=Model('light.'+slug)
    if slug=='wall_torch':
        m.box((0,.95,-.22),(.37,.28,.11),'iron')
        m.beam((0,1,-.17),(0,1.18,.22),.09,.09,'iron')
        m.cylinder((0,.22,.22),(0,1.32,.22),.06,'oak',sides=9)
        m.ring((0,1.25,.22),.12,.02,(0,1,0),'black_iron')
        m.sphere((0,1.42,.22),(.1,.21,.09),'ember',n=9,bands=7)
    elif slug=='harbor_lantern':
        m.cylinder((0,0,0),(0,2.05,0),.09,'iron',sides=10)
        m.box((0,2.07,0),(.46,.07,.46),'brass')
        m.box((0,2.52,0),(.48,.1,.48),'brass')
        for x in (-.2,.2):
            for z in (-.2,.2):m.beam((x,2.12,z),(x,2.49,z),.024,.024,'brass')
        for z in (-.201,.201):m.quad((-.18,2.13,z),(.18,2.13,z),(.18,2.47,z),(-.18,2.47,z),'glass',double=True)
        m.sphere((0,2.3,0),(.085,.15,.08),'ember')
        m.ring((0,2.55,0),.15,.018,(0,0,1),'iron')
    elif slug=='forge_brazier':
        for x in (-.38,.38):
            for z in (-.38,.38):m.beam((x,.08,z),(x,.95,z),.035,.04,'iron')
        m.cylinder((0,.83,0),(0,.99,0),.6,'black_iron',sides=12,end_radius=.48)
        m.ring((0,.97,0),.51,.025,(0,1,0),'steel')
        for x,z in ((0,0),(.2,.15),(-.18,-.12)):
            m.sphere((x,1.19,z),(.13,.3,.11),'lava',n=10,bands=7)
    elif slug=='lighthouse_beacon':
        m.cylinder((0,0,0),(0,3.55,0),.7,'plaster',sides=12,end_radius=.61)
        for h in (.25,1.3,2.5,3.48):m.ring((0,h,0),.7,.07,(0,1,0),'stone',pieces=16)
        m.cylinder((0,3.56,0),(0,3.71,0),.93,'stone',sides=12)
        m.cylinder((0,4.4,0),(0,4.54,0),.85,'roof',sides=12)
        for i in range(8):
            import math
            t=i*math.pi/4;m.cylinder((.7*math.cos(t),3.7,.7*math.sin(t)),(.7*math.cos(t),4.41,.7*math.sin(t)),.035,'iron',sides=7)
        m.group('rotating_lens',(0,4.05,0),(0,1,0),360,8)
        m.sphere((0,0,0),(.29,.3,.28),'ember','rotating_lens')
        m.box((0,0,0),(.68,.2,.28),'brass','rotating_lens')
    return m


def main():
    OUT.mkdir(parents=True,exist_ok=True)
    manifest=[]
    for slug,profile in PROFILES.items():
        path=OUT/f'L_{slug}.gltf'
        rec=geometry(slug).write(path)
        doc,binary=read_embedded_gltf(path)
        doc.setdefault('extensionsUsed',[]).append('KHR_lights_punctual')
        doc.setdefault('extensions',{})['KHR_lights_punctual']={'lights':[{
            'name':slug,'type':'point','color':profile['rgb'],'intensity':profile['candela'],'range':12.0 if slug=='lighthouse_beacon' else 6.0}]}
        doc['nodes'].append({'name':slug+'_light','translation':profile['position'],
                             'extensions':{'KHR_lights_punctual':{'light':0}}})
        doc['nodes'][0]['children'].append(len(doc['nodes'])-1)
        contents=embedded_gltf(doc,binary);path.write_bytes(contents)
        rec.update({'bytes':len(contents),'sha256':hashlib.sha256(contents).hexdigest(),
                    'gltfExtension':'KHR_lights_punctual','o3deManualSetup':profile['o3de']})
        manifest.append(rec)
    (ROOT/'ContentSource/OpenSource3D/LightingManifest.json').write_text(json.dumps({
        'schemaVersion':1,'fixtures':manifest,'limitations':["O3DE Assimp's import of KHR_lights_punctual and emitter intensity requires Editor verification",
        'If lights are not imported, add the O3DE Atom Light component from o3deManualSetup to the fixture entity.']},indent=2)+'\n')
    print(len(manifest))


if __name__=='__main__':main()
