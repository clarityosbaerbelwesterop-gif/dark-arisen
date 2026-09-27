#!/usr/bin/env python3
"""Ten configurable sea-going silhouettes with separate sail and rudder nodes.

These are visual alpha hulls. They intentionally do not replace authored
physics hulls, ocean displacement sampling, collision, crew sockets or LODs.
"""

import json
import math

from generate_props import Model, PI, ROOT


OUT=ROOT/'Engine/O3DE/DarkArisen/Assets/Art/Ships/OpenSource3D'
HERE=ROOT/'ContentSource/OpenSource3D'
# Length, beam, mast count, gun ports per side, category, sail fabric.
FLEET={
    'draven_flagship':(27,7.4,3,9,'warship','red_canvas'),
    'armada_sloop':(12.5,3.8,1,3,'warship','red_canvas'),
    'armada_brig':(19,5.5,2,6,'warship','red_canvas'),
    'armada_galleon':(30,8.3,3,10,'galleon','red_canvas'),
    'prison_transport':(20,5.6,2,4,'prison','canvas'),
    'merchant_sloop':(12,3.7,1,0,'merchant','canvas'),
    'moran_fishing_boat':(8.4,2.6,1,0,'fishing','canvas'),
    'longboat':(7,2.1,0,0,'longboat','canvas'),
    'harlow_merchant':(21.5,6,2,2,'merchant','canvas'),
    'la_liberacion':(18,5.1,2,5,'liberation','canvas'),
}


def hull_width(t):
    # t=-1 sharp prow; t=+1 broad transom.
    knots=[(-1,.07),(-.81,.62),(-.52,.9),(0,1),(.53,.94),(.84,.77),(1,.62)]
    for (a,w),(b,v) in zip(knots,knots[1:]):
        if a<=t<=b:return w+(v-w)*(t-a)/(b-a)
    return knots[0][1] if t<0 else knots[-1][1]


def hull(m,length,beam,category):
    m.static_group('hull',(0,0,0))
    group='hull';n=20
    for i in range(n):
        t0=-1+2*i/n;t1=-1+2*(i+1)/n
        z0=t0*length*.5;z1=t1*length*.5
        for side in (-1,1):
            for y0,y1,w0,w1,mat in [(-1.12,-.57,.12,.76,'oak'),(-.57,.41,.76,1,'oak_light'),(.41,1.16,1,.96,'oak')]:
                a=(side*beam*.5*w0*hull_width(t0),y0,z0)
                b=(side*beam*.5*w0*hull_width(t1),y0,z1)
                c=(side*beam*.5*w1*hull_width(t1),y1,z1)
                d=(side*beam*.5*w1*hull_width(t0),y1,z0)
                m.quad(a,b,c,d,mat,group,double=True)
        width=beam*.94*(hull_width(t0)+hull_width(t1))*.25
        m.box((0,1.105,(z0+z1)*.5),(max(.1,width*2),.07,(z1-z0)*.93),
              'oak_light' if i%3 else 'oak',group)
    for side in (-1,1):
        m.beam((side*beam*.26,-.59,-length*.47),(side*beam*.3,-.59,length*.49),.1,.09,'black_iron',group)
        m.beam((side*beam*.46,.30,-length*.3),(side*beam*.43,.30,length*.43),.07,.08,'oak',group)
        m.beam((side*beam*.47,1.16,-length*.3),(side*beam*.44,1.16,length*.43),.09,.07,'brass' if category=='galleon' else 'iron',group)
        for z in (-.40,-.23,0,.23,.4):
            if category not in ('fishing','longboat'):
                m.cylinder((side*beam*.44,1.13,z*length),(side*beam*.46,1.66,z*length),.055,'oak',group,sides=8)
        if category not in ('fishing','longboat'):
            m.beam((side*beam*.46,1.66,-length*.4),(side*beam*.46,1.66,length*.4),.06,.06,'oak_light',group)
    m.box((0,.58,length*.495),(beam*.62,1.05,.11),'oak',group)
    # Angled bowsprit and prow trim distinguish the fore end of the vessel.
    m.beam((0,1.16,-length*.45),(0,1.66,-length*.68),.13,.13,'oak_light',group)
    m.sphere((0,1.22,-length*.51),(.15,.29,.18),'brass' if category=='galleon' else 'oak',group,n=9,bands=6)
    return group


def mast(m,index,count,length,beam,sail):
    z=((index-(count-1)/2)*.29)*length
    h=(3.4+length*.31)*(1 if index==count//2 else .81)
    m.cylinder((0,1.15,z),(0,1.15+h,z),.12 if length<15 else .18,'oak', 'hull',sides=10,end_radius=.06)
    m.beam((-beam*.37,1.15+h*.67,z),(beam*.37,1.15+h*.67,z),.1,.12,'oak_light','hull')
    m.beam((-beam*.25,1.15+h*.31,z),(beam*.25,1.15+h*.31,z),.085,.085,'oak_light','hull')
    for side in (-1,1):
        for zoff in (-.11,.1):
            m.path([(0,1.15+h*.84,z),(side*beam*.48,1.19,z+zoff*length)],.018,'rope','hull')
    # Rotating square sail is a separate node to preview wind direction.
    name=f'sail_{index+1:02d}'
    m.group(name,(0,1.15+h*.48,z),(0,1,0),12,2.8)
    span=beam*.39;low=-h*.17;high=h*.18
    for segment in range(6):
        a=-span+2*span*segment/6;b=-span+2*span*(segment+1)/6
        belly=.15 if segment in (2,3) else .04
        m.quad((a,low,belly),(b,low,belly),(b,high,0),(a,high,0),sail,name,double=True)
    for y in (low,0,high):m.beam((-span,y,.02),(span,y,.02),.02,.02,'rope',name)
    return name


def decorate(m,length,beam,masts,guns,category,sail):
    if guns:
        for side in (-1,1):
            for i in range(guns):
                z=(-.33+(i+.5)*.68/guns)*length
                m.box((side*beam*.487,.74,z),(.02,.22,.31),'black_iron','hull')
                m.cylinder((side*beam*.49,.75,z),(side*beam*.65,.73,z),.09,'iron','hull',sides=8,end_radius=.074)
    if category in ('warship','galleon','prison','merchant','liberation'):
        m.box((0,1.47,length*.34),(beam*.59,.6,length*.19),'oak','hull')
        m.box((0,1.80,length*.34),(beam*.65,.10,length*.20),'roof','hull')
        for side in (-1,1):
            for z in (.29,.38):m.box((side*beam*.303,1.49,z*length),(.03,.2,.21),'black_iron','hull')
    if category=='prison':
        for z in (-.17,-.05,.07):
            m.box((0,1.155,z*length),(beam*.37,.06,.36),'black_iron','hull')
            for x in (-.21,-.07,.07,.21):m.beam((x*beam,1.19,z*length-.18),(x*beam,1.19,z*length+.18),.018,.015,'iron','hull')
    if category in ('merchant','fishing'):
        for i in range(3 if category=='fishing' else 5):
            z=(-.25+i*.12)*length
            m.cylinder((beam*.15,1.14,z),(beam*.15,1.62,z),.17,'oak','hull',sides=10)
            m.ring((beam*.15,1.44,z),.18,.025,(0,1,0),'iron','hull',pieces=12)
        if category=='fishing':
            for side in (-1,1):
                for i in range(3):
                    z=(-.3+i*.29)*length
                    m.path([(side*beam*.5,1.2,z),(side*beam*.74,.41,z)],.014,'rope','hull')
    if category=='longboat':
        for z in (-.32,-.17,0,.17,.32):
            m.box((0,1.24,z*length),(beam*.87,.12,.16),'oak_light','hull')
            for side in (-1,1):m.beam((side*beam*.3,1.34,z*length),(side*beam*.85,.89,z*length-.29),.07,.07,'oak','hull')
    if category in ('warship','galleon','liberation'):
        for i in range(3):
            z=(-.26+i*.13)*length
            m.beam((0,1.14,z),(beam*.55,1.14,z),.03,.02,'rope','hull')
        pole_z=.38*length
        m.cylinder((0,1.81,pole_z),(0,2.9,pole_z),.04,'oak','hull',sides=8)
        m.quad((0,2.9,pole_z),(.62,2.8,pole_z),(.62,2.52,pole_z),(0,2.62,pole_z),
               'red_canvas' if sail=='red_canvas' else 'canvas','hull',double=True)
    for i in range(masts):mast(m,i,masts,length,beam,sail)
    m.group('rudder',(0,.64,length*.51),(0,1,0),18,2.4)
    m.box((0,-.24,.1),(.15,.67,.36),'oak','rudder')
    m.cylinder((0,.05,0),(0,.75,0),.065,'iron','rudder',sides=8)


def main():
    catalog=json.loads((ROOT/'ContentSource/Higgsfield/HiggsfieldO3DEAssetCatalog.json').read_text())
    assert {'ship.'+slug for slug in FLEET}=={x['id'] for x in catalog['assets'] if x['id'].startswith('ship.')}
    records=[]
    for slug,(length,beam,masts,guns,category,sail) in FLEET.items():
        m=Model('ship.'+slug)
        hull(m,length,beam,category)
        decorate(m,length,beam,masts,guns,category,sail)
        record=m.write(OUT/f'KIT_ship_{slug}.gltf')
        records.append({'catalogId':'ship.'+slug,'lengthMeters':length,'beamMeters':beam,
                        'mastCount':masts,'gunPortsPerSide':guns,'role':category,**record})
    (HERE/'ShipManifest.json').write_text(json.dumps({'generator':'generate_ships.py','ships':records},indent=2)+'\n')
    print(json.dumps({'ships':len(records),'triangles':sum(r['triangles'] for r in records),
                      'previewAnimations':sum(len(r['animations']) for r in records)}))


if __name__=='__main__':main()
