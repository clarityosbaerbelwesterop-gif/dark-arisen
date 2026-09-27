#!/usr/bin/env python3
"""Six additional period weapon pickup meshes for the boss/world alpha pass."""

import json

from generate_props import Model, ROOT

OUT=ROOT/'Engine/O3DE/DarkArisen/Assets/Art/Weapons/OpenSource3D'
SLUGS=('boarding_pike','harpoon','iron_cleaver','surgeon_knife','grappling_hook','forge_hammer')


def create(slug):
    m=Model('weapon.'+slug)
    if slug in ('boarding_pike','harpoon'):
        m.cylinder((0,0,0),(0,2.15,0),.045,'oak',sides=10,end_radius=.032)
        m.cylinder((0,2.06,0),(0,2.42,0),.043,'iron',sides=10,end_radius=.005)
        m.ring((0,2.07,0),.06,.011,(0,1,0),'brass')
        if slug=='harpoon':
            for s in (-1,1):m.beam((0,2.28,0),(s*.19,2.09,0),.025,.026,'steel')
            m.path([(0,.1,0),(.1,.03,.1),(.35,.02,.09),(.7,.02,.12)],.017,'rope')
        else:
            m.beam((-.12,1.83,0),(.12,1.83,0),.03,.03,'iron')
    elif slug in ('iron_cleaver','surgeon_knife'):
        m.cylinder((0,0,0),(0,.25,0),.047 if slug=='iron_cleaver' else .028,'leather',sides=10)
        m.ring((0,.26,0),.054,.012,(0,1,0),'brass')
        length=.48 if slug=='iron_cleaver' else .28
        width=.16 if slug=='iron_cleaver' else .045
        m.beam((0,.26,0),(width*.15,.26+length,0),width,.019,'steel')
        m.beam((-.08,.26+length,0),(.09,.26+length,0),.025,.018,'steel')
    elif slug=='grappling_hook':
        m.cylinder((0,0,0),(0,.49,0),.04,'iron',sides=9)
        m.ring((0,.06,0),.09,.018,(0,0,1),'iron')
        for s in (-1,1):m.path([(0,.43,0),(s*.18,.58,0),(s*.29,.54,0),(s*.31,.38,0)],.034,'steel')
        m.path([(0,.02,0),(.1,-.13,.05),(.12,-.27,.07),(.15,-.44,.04)],.02,'rope')
    else:
        m.cylinder((0,0,0),(0,1.16,0),.052,'oak',sides=10)
        m.box((0,1.03,0),(.43,.21,.22),'iron')
        m.box((-.22,1.03,0),(.05,.25,.25),'steel')
        m.box((.22,1.03,0),(.05,.25,.25),'steel')
        for h in (.22,.32,.42,.52):m.ring((0,h,0),.052,.004,(0,1,0),'rope',pieces=10)
    return m


def main():
    manifest=[]
    for slug in SLUGS:
        details=create(slug).write(OUT/f'SM_{slug}.gltf')
        manifest.append({'id':'weapon.'+slug,**details,'stage':'pickup_alpha'})
    (ROOT/'ContentSource/OpenSource3D/WeaponExtrasManifest.json').write_text(json.dumps({'schemaVersion':1,'weapons':manifest},indent=2)+'\n')
    print(len(manifest))


if __name__=='__main__':main()
