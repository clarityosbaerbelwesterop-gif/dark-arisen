#!/usr/bin/env python3
"""Generate 24 independently named modular environment alpha kits.

The three scene nodes in each kit are separated for source-art selection. They
are not gameplay levels, collision meshes, navigation or authored story layout.
"""

import json
import math

from generate_props import Model, OUT, PI, ROOT

WORLD_OUT = ROOT / 'Engine/O3DE/DarkArisen/Assets/Art/World/OpenSource3D'


def stone_arch(m,g):
    for x in (-.75,.75):
        m.box((x,1.15,0),(.55,2.3,.75),'stone',g)
        for y in (.2,1.25,2.25):m.box((x,y,0),(.66,.12,.84),'plaster',g)
    m.box((0,2.44,0),(2.3,.46,.82),'stone',g)
    m.box((0,2.83,0),(2.55,.35,.88),'stone',g)


def tower(m,g):
    m.cylinder((0,0,0),(0,3.2,0),.92,'stone',g,sides=12)
    m.cylinder((0,3.2,0),(0,3.38,0),1.05,'plaster',g,sides=12)
    for i in range(12):
        t=2*PI*i/12
        if i%2==0:m.box((.89*math.cos(t),3.61,.89*math.sin(t)),(.32,.45,.33),'stone',g)
    m.box((0,1.07,-.93),(.43,1.68,.045),'black_iron',g)


def palisade(m,g):
    for i in range(10):
        x=(i-4.5)*.31
        m.cylinder((x,0,0),(x,2.27+(i%3)*.1,0),.17,'oak',g,sides=6,end_radius=.025)
    for y in (.42,1.35):m.beam((-1.55,y,.2),(1.55,y,.2),.13,.16,'oak_light',g)


def pier(m,g):
    for z in range(6):m.box((0,.82,(z-2.5)*.44),(2,.13,.38),'oak',g)
    for x in (-.84,.84):
        for z in (-.96,.96):m.cylinder((x,-.35,z),(x,.89,z),.08,'oak',g,sides=8)
    for x in (-.82,.82):m.beam((x,.45,-1.3),(x,.45,1.3),.065,.08,'iron',g)


def hut(m,g):
    m.box((0,.96,0),(2.65,1.8,1.85),'plaster',g)
    m.box((0,.98,-.94),(.62,1.32,.06),'oak',g)
    for x in (-1.23,1.23):
        for z in (-.84,.84):m.box((x,1,z),(.13,2,.14),'oak',g)
    for s in (-1,1):
        m.quad((-1.46,2.6,0),(1.46,2.6,0),(1.46,1.88,s*1.12),(-1.46,1.88,s*1.12),'roof',g,double=True)
    for x in (-.83,0,.83):m.beam((x,1.9,-1.14),(x,2.62,0),.07,.08,'oak',g)


def warehouse(m,g):
    m.box((0,1.34,0),(3.45,2.6,2.3),'oak',g)
    for x in (-1.25,0,1.25):m.box((x,1.35,-1.17),(.1,2.6,.08),'iron',g)
    m.box((0,1.15,-1.19),(1.2,1.9,.09),'black_iron',g)
    m.box((0,2.75,0),(3.7,.25,2.53),'roof',g)
    for x in (-1.4,1.4):m.box((x,.12,0),(.18,.24,2.5),'stone',g)


def house(m,g):
    m.box((0,.85,0),(2.1,1.65,1.75),'plaster',g)
    m.box((0,.8,-.89),(.55,1.35,.05),'oak',g)
    m.box((-.66,1.16,-.91),(.38,.4,.04),'black_iron',g)
    for z in (-1,1):
        m.quad((-1.2,2.23,0),(1.2,2.23,0),(1.2,1.68,z),(-1.2,1.68,z),'roof',g,double=True)
    m.cylinder((.73,2.03,.12),(.73,2.7,.12),.14,'stone',g,sides=8)


def boulders(m,g):
    for x,y,z,scale in [(-.76,.38,0,.68),(.22,.58,.15,.86),(.9,.3,-.17,.55)]:
        m.sphere((x,y,z),(scale,scale*.75,scale*.7),'stone',g,n=9,bands=5)
        m.sphere((x,y+.17,z+.05),(scale*.6,scale*.26,scale*.5),'moss',g,n=9,bands=4)


def reef(m,g):
    for x,z,h in [(-.7,0,.8),(.0,.35,1.05),(.65,-.2,.92)]:
        m.cylinder((x,.08,z),(x,h,z),.09,'coral',g,sides=6,end_radius=.045)
        for sign in (-1,1):m.cylinder((x,h*.6,z),(x+sign*.28,h*.88,z+.13),.045,'coral',g,sides=6,end_radius=.02)
    for x in (-.65,.65):m.sphere((x,.1,-.3),(.3,.18,.26),'stone',g,n=9,bands=5)


def palm(m,g):
    m.cylinder((0,0,0),(.13,2.75,0),.17,'oak',g,sides=9,end_radius=.1)
    for i in range(8):
        t=2*PI*i/8;dx=math.cos(t);dz=math.sin(t)
        m.beam((.13,2.7,0),(.13+dx*1.25,2.42,dz*1.25),.06,.06,'oak',g)
        m.quad((.13,2.72,0),(.13+dx*.9,2.55,dz*.9),(.13+dx*1.37,2.28,dz*1.37),(.13+dx*.7,2.42,dz*.7),'leaf',g,double=True)


def mangrove(m,g):
    m.cylinder((0,.3,0),(.09,2.5,.03),.27,'oak',g,sides=9,end_radius=.13)
    for i in range(6):
        t=2*PI*i/6;u=(math.cos(t),math.sin(t))
        m.cylinder((.02,.75,.02),(.9*u[0],.02,.9*u[1]),.085,'oak',g,sides=7,end_radius=.027)
        m.cylinder((.09,2.2,0),(.96*u[0],2.83,.96*u[1]),.095,'oak',g,sides=7,end_radius=.025)
        for r in (.56,.92):m.sphere((r*u[0],2.72,r*u[1]),(.5,.28,.44),'leaf',g,n=9,bands=5)


def wreck(m,g):
    for z in range(7):
        zz=(z-3)*.39
        m.beam((0,.16,zz),(-.76,.58,zz),.085,.1,'oak',g)
        m.beam((0,.16,zz),(.76,.58,zz),.085,.1,'oak',g)
    for side in (-1,1):
        for y in (.46,.72):m.beam((side*.73,y,-1.3),(side*.73,y,1.28),.1,.14,'oak_light',g)
    m.cylinder((0,.25,.16),(.35,2.28,.16),.11,'oak',g,sides=9,end_radius=.06)
    m.quad((.36,1.04,.2),(1.5,1.07,.2),(1.34,2.06,.2),(.5,2.19,.2),'canvas',g,double=True)


def bridge(m,g):
    for i in range(10):m.box((0,.63,(i-4.5)*.29),(1.64,.09,.26),'oak',g)
    for side in (-1,1):
        for z in (-1.3,1.3):m.cylinder((side*.78,.54,z),(side*.78,1.62,z),.07,'oak',g)
        m.path([(side*.78,1.51,-1.3),(side*.78,1.17,0),(side*.78,1.51,1.3)],.028,'rope',g)


def stairs(m,g):
    for i in range(7):
        m.box((0,.08+i*.24,-1.05+i*.32),(1.55,.16+i*.2,.31),'stone',g)
    for side in (-1,1):m.beam((side*.77,.26,-1.3),(side*.77,2,1.2),.17,.19,'stone',g)


def cliff(m,g):
    for i in range(5):
        x=(i-2)*.62;h=.6+(i%3)*.48
        m.cylinder((x,0,0),(x,h,0),.37,'basalt',g,sides=6)
    for x in (-.65,.5):m.sphere((x,.13,.47),(.4,.25,.38),'moss',g,n=9,bands=4)


def brazier(m,g):
    for i in range(3):
        t=2*PI*i/3;m.beam((0,.74,0),(.54*math.cos(t),.05,.54*math.sin(t)),.052,.05,'iron',g)
    m.cylinder((0,.72,0),(0,.88,0),.43,'black_iron',g,sides=12)
    for x,z in ((0,0),(.14,.14),(-.12,.08)):
        m.sphere((x,1.01,z),(.12,.23,.1),'lava',g,n=8,bands=5)


def column(m,g):
    for i in range(6):
        t=2*PI*i/6;x=.63*math.cos(t);z=.63*math.sin(t)
        m.cylinder((x,0,z),(x,1.1+(i%3)*.41,z),.3,'basalt',g,sides=6)
    m.quad((-.85,.03,-.75),(.85,.03,-.75),(.85,.03,.75),(-.85,.03,.75),'lava',g,double=True)


def lava_vent(m,g):
    m.sphere((0,.28,0),(1.04,.44,.83),'basalt',g,n=10,bands=6)
    m.cylinder((0,.33,0),(0,.54,0),.29,'lava',g,sides=12)
    for i in range(6):
        t=2*PI*i/6;m.cylinder((.45*math.cos(t),.34,.45*math.sin(t)),(.63*math.cos(t),.8,.63*math.sin(t)),.12,'basalt',g,sides=6,end_radius=.035)


def deck(m,g):
    for i in range(9):m.box(((i-4)*.28,.13,0),(.26,.12,2.75),'oak',g)
    for x in (-1.37,1.37):
        m.beam((x,.3,-1.4),(x,.3,1.4),.09,.1,'iron',g)
        for z in (-1.23,0,1.23):m.cylinder((x,.24,z),(x,1.2,z),.07,'oak',g)
    m.box((0,.3,0),(.72,.05,.72),'black_iron',g)


def rail(m,g):
    for x in (-1,1):m.box((x,1.5,0),(.12,3,.18),'iron',g)
    m.beam((-1,2.9,0),(1,2.9,0),.1,.11,'iron',g)
    for x in (-.62,0,.62):
        m.path([(x,2.88,0),(x,1.75,0),(x+.13,1.68,0),(x+.18,1.86,0)],.035,'iron',g)


MODULES={
    'arch':stone_arch,'tower':tower,'palisade':palisade,'pier':pier,
    'hut':hut,'warehouse':warehouse,'house':house,'rock':boulders,
    'reef':reef,'palm':palm,'mangrove':mangrove,'wreck':wreck,
    'bridge':bridge,'stairs':stairs,'cliff':cliff,'brazier':brazier,
    'basalt':column,'vent':lava_vent,'deck':deck,'rail':rail,
}

KITS={
    'moran_outer_reef':('reef','rock','palm'),
    'driftwood_beach':('palm','wreck','rock'),
    'harlow_wreckage':('wreck','deck','pier'),
    'driftwood_camp':('hut','palisade','brazier'),
    'miras_cove':('hut','pier','palm'),
    'mangrove_route':('mangrove','bridge','reef'),
    'koa_trading_post':('hut','palisade','pier'),
    'galleon_cove':('wreck','rock','reef'),
    'rexa_warehouse_quarter':('warehouse','pier','rail'),
    'armada_tender_wreck_cove':('wreck','rock','pier'),
    'crown_citadel_approach':('arch','tower','stairs'),
    'holders_wake_back_routes':('cliff','bridge','tower'),
    'no_safe_harbor_route':('cliff','rock','bridge'),
    'war_current_harbor':('tower','palisade','pier'),
    'broken_compact_settlement':('house','brazier','palisade'),
    'herrera_archive_fort':('arch','tower','palisade'),
    'salt_and_iron_blockade':('palisade','tower','pier'),
    'ashenmoor_volcanic':('basalt','vent','cliff'),
    'stormspire':('cliff','stairs','tower'),
    'quiet_coast':('house','pier','palm'),
    'fort_carrion':('arch','tower','palisade'),
    'the_maw':('basalt','arch','brazier'),
    'draven_black_deck':('deck','rail','brazier'),
    'rexa_slaughterhouse':('warehouse','rail','pier'),
}


def main():
    catalog=json.loads((ROOT/'ContentSource/Higgsfield/HiggsfieldO3DEAssetCatalog.json').read_text())
    expected={a['id'][12:] for a in catalog['assets'] if a['id'].startswith('environment.')}
    assert set(KITS)==expected,(set(KITS)-expected,expected-set(KITS))
    manifest=[]
    for slug,pieces in KITS.items():
        model=Model('environment.'+slug)
        for i,kind in enumerate(pieces):
            name=f'{i+1:02d}_{kind}'
            model.static_group(name,((i-1)*5.5,0,0))
            MODULES[kind](model,name)
        details=model.write(WORLD_OUT/f'KIT_{slug}.gltf')
        manifest.append({'catalogId':'environment.'+slug,'pieces':list(pieces),**details})
    path=ROOT/'ContentSource/OpenSource3D/WorldManifest.json'
    path.write_text(json.dumps({'schemaVersion':1,'stage':'modular_alpha','kits':manifest},indent=2)+'\n')
    print(len(manifest),sum(x['triangles'] for x in manifest))


if __name__=='__main__':main()
