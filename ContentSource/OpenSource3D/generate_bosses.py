#!/usr/bin/env python3
"""Rigged MakeHuman boss alpha variants and six creature silhouette meshes.

Humanoid variants retain the original 163-joint MakeHuman skin and reuse three
compatible CMU motions. Attached head/hand/shoulder props are still alpha
authoring and require in-engine placement, phase design and collision review.
"""

import base64
import copy
import hashlib
import json
import math

from generate_props import Model, PI, ROOT
from gltf_format import embedded_gltf, read_embedded_gltf

ART=ROOT/'Engine/O3DE/DarkArisen/Assets/Art'
HUM_OUT=ART/'Characters/Bosses/OpenSource3D'
CREATURE_OUT=ART/'Creatures/Bosses/OpenSource3D'
BASE=ART/'Characters/SK_Art_boarder.gltf'
MOTIONS={'idle':'77_02','walk':'02_01','swordplay':'02_07'}
TEMP=ROOT/'ContentSource/OpenSource3D/_temporary_gear.gltf'

HUMANS={
 'admiral_fitzmueller':('naval_tricorne','saber',(.23,.31,.38)),
 'brother_cleaver':('bare_head','cleaver',(.37,.22,.16)),
 'captain_corazon':('feather_hat','saber',(.45,.15,.12)),
 'captain_rojas':('iron_helm','axe',(.22,.25,.29)),
 'el_medico':('doctor_mask','medical_blade',(.63,.61,.49)),
 'halvard_grimm':('northern_helm','axe',(.35,.32,.27)),
 'high_priest_silvano':('ritual_crown','staff',(.54,.42,.24)),
 'la_viuda_negra':('hood','dagger',(.11,.12,.13)),
 'red_lieutenant_kota_api':('forge_mask','hammer',(.38,.16,.11)),
 'scarred_twin_kira':('scarred_band','saber',(.37,.22,.24)),
 'scarred_twin_mira':('scarred_band','dagger',(.28,.2,.22)),
 'twin_hook_castor':('pirate_wrap','hook',(.29,.17,.12)),
 'twin_hook_pollux':('pirate_wrap','hook',(.28,.16,.13)),
 'ulfar_stormhand':('fur_cap','axe',(.31,.37,.41)),
}


def gear(slug,head,weapon):
    m=Model('boss.'+slug+'_equipment')
    m.static_group('headgear',(0,.13,0));g='headgear'
    if head in ('naval_tricorne','feather_hat'):
        m.ring((0,0,0),.13,.03,(0,1,0),'black_iron',g,pieces=16)
        for s in (-1,1):m.beam((-.08,.03,s*.08),(.13,.15,s*.12),.05,.05,'black_iron',g)
        if head=='feather_hat':m.beam((.06,.1,0),(.22,.38,.07),.025,.04,'red_canvas',g)
    elif head=='ritual_crown':
        m.ring((0,0,0),.12,.027,(0,1,0),'brass',g)
        for i in range(5):
            t=2*PI*i/5;m.beam((.12*math.cos(t),0,.12*math.sin(t)),(.14*math.cos(t),.15,.14*math.sin(t)),.025,.022,'brass',g)
    elif head in ('iron_helm','northern_helm','forge_mask'):
        m.sphere((0,.03,0),(.135,.11,.125),'iron',g,n=10,bands=6)
        m.box((0,-.07,-.11),(.22,.08,.03),'black_iron',g)
        if head=='forge_mask':m.box((0,-.14,-.12),(.19,.14,.02),'iron',g)
    elif head in ('doctor_mask','hood','pirate_wrap','scarred_band','fur_cap'):
        mat='canvas' if head=='doctor_mask' else 'leather'
        m.ring((0,0,0),.13,.023,(0,1,0),mat,g)
        if head in ('hood','fur_cap'):
            m.sphere((0,.05,.02),(.15,.14,.16),mat,g,n=10,bands=6)
        elif head=='doctor_mask':m.box((0,-.045,-.12),(.16,.1,.025),'paper',g)
    elif head=='bare_head':m.ring((0,-.08,0),.12,.018,(0,1,0),'leather',g)

    m.static_group('shoulder_details',(0,0,0));g='shoulder_details'
    for s in (-1,1):
        m.sphere((0,.07,s*.16),(.1,.07,.1),'brass' if 'captain' in slug or 'admiral' in slug else 'leather',g,n=8,bands=5)
        if 'priest' in slug:m.path([(0,.1,s*.17),(.06,-.18,s*.17)],.024,'red_canvas',g)
    m.static_group('right_weapon',(0,0,0));g='right_weapon'
    m.cylinder((0,-.12,0),(0,.11,0),.025,'leather',g,sides=8)
    if weapon in ('saber','dagger','medical_blade','cleaver'):
        length={'saber':.62,'dagger':.32,'medical_blade':.38,'cleaver':.4}[weapon]
        width={'saber':.037,'dagger':.03,'medical_blade':.02,'cleaver':.11}[weapon]
        m.beam((0,-.12,0),(width*.15,-.12-length,0),width,.017,'steel',g)
        m.beam((-.09,-.12,0),(.09,-.12,0),.018,.023,'brass',g)
        if weapon=='saber':m.ring((.04,-.03,0),.095,.01,(0,0,1),'brass',g)
    elif weapon=='hook':
        m.cylinder((0,-.12,0),(0,-.35,0),.024,'iron',g)
        m.path([(0,-.35,0),(.12,-.44,0),(.18,-.4,0),(.18,-.3,0)],.028,'iron',g)
    elif weapon in ('axe','hammer'):
        m.cylinder((0,-.12,0),(0,-.65,0),.027,'oak',g)
        m.beam((-.13,-.55,0),(.16,-.55,0),.095 if weapon=='hammer' else .12,.06,'iron',g)
    elif weapon=='staff':
        m.cylinder((0,.09,0),(0,-1.25,0),.032,'oak',g)
        m.ring((0,-1.21,0),.095,.02,(0,0,1),'brass',g)
    return m


def attach_material_tint(doc,palette):
    for mat in doc['materials']:
        name=mat.get('name','').lower()
        if any(x in name for x in ('outfit','sash','headcloth','sleeve')):
            factor=mat.setdefault('pbrMetallicRoughness',{}).get('baseColorFactor',[1,1,1,1])
            mat['pbrMetallicRoughness']['baseColorFactor']=[max(.03,min(1,factor[i]*palette[i]*1.6)) for i in range(3)]+[1]


def append_binary(doc,data,view_list,accessor_list):
    while len(data)%4:data.append(0)
    offset=len(data)
    newviews=len(doc['bufferViews'])
    for v in view_list:
        copyview=dict(v);copyview['byteOffset']=copyview.get('byteOffset',0)+offset
        doc['bufferViews'].append(copyview)
    start=len(doc['accessors'])
    for a in accessor_list:
        copyacc=dict(a);copyacc['bufferView']+=newviews
        doc['accessors'].append(copyacc)
    return newviews,start,offset


def create_human(slug,configuration,original):
    head,weapon,palette=configuration
    doc=copy.deepcopy(original)
    binary=bytearray(base64.b64decode(doc['buffers'][0].pop('uri').partition(',')[2]))
    doc['nodes'][0]['name']='SK_Boss_'+slug
    for img in doc.get('images',[]):
        if img.get('uri','').startswith('../Textures/'):
            img['uri']=img['uri'].replace('../Textures/','../../../Textures/',1)
    attach_material_tint(doc,palette)
    details=gear(slug,head,weapon)
    details.write(TEMP)
    part,payload=read_embedded_gltf(TEMP)
    TEMP.unlink()
    viewbase,accessbase,_=append_binary(doc,binary,part['bufferViews'],part['accessors'])
    binary.extend(payload)
    imagebase=len(doc['images']);texturebase=len(doc['textures']);samplerbase=len(doc.get('samplers',[]));materialbase=len(doc['materials']);meshbase=len(doc['meshes'])
    for image in part['images']:
        item=dict(image);item['bufferView']+=viewbase;doc['images'].append(item)
    for sampler in part.get('samplers',[]):doc.setdefault('samplers',[]).append(sampler)
    for tex in part['textures']:
        item=dict(tex);item['source']+=imagebase
        if 'sampler' in item:item['sampler']+=samplerbase
        doc['textures'].append(item)
    for mat in part['materials']:
        item=copy.deepcopy(mat);pbr=item.get('pbrMetallicRoughness',{})
        if 'baseColorTexture' in pbr:pbr['baseColorTexture']['index']+=texturebase
        if 'emissiveTexture' in item:item['emissiveTexture']['index']+=texturebase
        doc['materials'].append(item)
    for mesh in part['meshes']:
        item=copy.deepcopy(mesh)
        for prim in item['primitives']:
            prim['attributes']={key:acc+accessbase for key,acc in prim['attributes'].items()}
            prim['indices']+=accessbase;prim['material']+=materialbase
        doc['meshes'].append(item)
    bones={n['name']:i for i,n in enumerate(doc['nodes']) if 'name' in n}
    for node in part['nodes']:
        if 'mesh' not in node:continue
        item=copy.deepcopy(node);item['mesh']+=meshbase
        new_id=len(doc['nodes']);doc['nodes'].append(item)
        bind={'headgear':'head','shoulder_details':'spine02','right_weapon':'wrist.R'}[node['name']]
        doc['nodes'][bones[bind]].setdefault('children',[]).append(new_id)
    clip_names=[]
    for label,motion_id in MOTIONS.items():
        clip=json.loads((ART/f'Characters/Motions/AN_Art_Human_{motion_id}.gltf').read_text())
        names=[n['name'] for n in clip['nodes']]
        assert names==[n['name'] for n in doc['nodes'][1:164]]
        moved=base64.b64decode(clip['buffers'][0]['uri'].partition(',')[2])
        _,accbase,_=append_binary(doc,binary,clip['bufferViews'],clip['accessors'])
        binary.extend(moved)
        animation=copy.deepcopy(clip['animations'][0]);animation['name']=f'{label}_{motion_id}_CMU_preview'
        for sampler in animation['samplers']:
            sampler['input']+=accbase;sampler['output']+=accbase
        for channel in animation['channels']:
            channel['target']['node']+=1
        doc.setdefault('animations',[]).append(animation)
        clip_names.append(animation['name'])
    doc['asset']['generator']='DarkArisen MakeHuman/CMU boss variant (OpenSource3D alpha)'
    doc['buffers'][0]['byteLength']=len(binary)
    out=HUM_OUT/f'SK_Boss_{slug}.gltf'
    out.parent.mkdir(parents=True,exist_ok=True)
    data=embedded_gltf(doc,bytes(binary));out.write_bytes(data)
    return {'file':out.relative_to(ROOT).as_posix(),'catalogId':'boss.'+slug,
            'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest(),
            'jointCount':len(doc['skins'][0]['joints']),'clips':clip_names,
            'equipment':[head,weapon],'stage':'skinned_humanoid_alpha'}


def quadruped(m,slug):
    wet=slug in ('jaw_of_the_mire','havfrue_modor')
    body='black_iron' if wet else 'basalt'
    m.sphere((0,1.2,0),(.55,.58,1.26),body,n=14,bands=9)
    for s in (-1,1):
        for z in (-.74,.74):
            m.beam((s*.42,1.08,z),(s*.77,.08,z+.08),.22,.2,body)
            m.sphere((s*.76,.15,z+.14),(.21,.16,.3),body,n=9,bands=6)
    m.sphere((0,1.24,-1.17),(.47,.34,.55),body,n=12,bands=8)
    for x in (-.22,.22):m.sphere((x,1.37,-1.54),(.055,.065,.06),'ember' if slug!='jaw_of_the_mire' else 'coral',n=8,bands=5)
    m.group('jaw',(0,1.02,-1.3),(1,0,0),17,2)
    m.box((0,-.07,-.42),(.57,.1,.8),'iron','jaw')
    m.group('tail',(0,1.13,1.0),(0,1,0),24,2.7)
    m.cylinder((0,0,0),(0,.07,1.55),.22,body,'tail',sides=12,end_radius=.025)
    for i in range(5):m.sphere((0,1.68,.72-i*.33),(.31,.1,.23),'moss' if slug=='jungle_warden' else 'stone',n=8,bands=4)
    if slug=='jungle_warden':
        for s in (-1,1):
            for z in (-.6,.2,.8):m.cylinder((s*.27,1.34,z),(s*.55,1.92,z),.09,'oak',sides=8,end_radius=.015)


def sea_monster(m):
    for i in range(8):
        z=(i-3.5)*.55
        y=1.16+.25*math.sin(i*.62)
        r=.33+(.17 if 2<=i<=5 else 0)
        m.sphere((0,y,z),(r,.35,r*.9),'water',n=12,bands=7)
        if 1<=i<=6:
            m.quad((0,y+.32,z-.23),(.03,y+.7,z),(.03,y+.54,z+.34),(0,y+.3,z+.25),'coral',double=True)
    m.sphere((0,1.55,-2.35),(.41,.43,.6),'black_iron',n=12,bands=8)
    for side in (-1,1):
        m.sphere((side*.28,1.64,-2.55),(.07,.06,.07),'ember',n=8,bands=5)
        m.path([(side*.25,1.29,-2.69),(side*.48,1.2,-2.88),(side*.66,1.09,-2.95)],.012,'rope')
        m.quad((side*.22,1.28,-1.9),(side*.93,1.51,-1.48),(side*.96,1.19,-1.1),(side*.35,1.02,-1.39),'water',double=True)
    m.group('fin_tail',(0,1.03,1.83),(0,1,0),26,2.8)
    m.cylinder((0,0,0),(0,.05,1.22),.23,'water','fin_tail',sides=12,end_radius=.04)
    m.quad((0,.04,1.14),(.92,.12,1.82),(0,.06,1.58),(-.92,.12,1.82),'water','fin_tail',double=True)


def crocodile(m):
    m.sphere((0,.76,0),(.66,.36,1.2),'black_iron',n=14,bands=8)
    m.sphere((0,.73,-1.18),(.5,.23,.83),'black_iron',n=12,bands=7)
    for side in (-1,1):
        m.sphere((side*.31,.9,-1.57),(.065,.055,.085),'coral',n=8,bands=5)
        for z in (-.7,.75):
            m.beam((side*.43,.68,z),(side*.83,.16,z+.06),.22,.18,'black_iron')
            m.sphere((side*.82,.1,z+.08),(.24,.1,.29),'black_iron',n=8,bands=5)
    for row in range(3):
        for z in range(6):
            m.sphere(((row-1)*.29,1.03,-.9+z*.36),(.13,.075,.14),'stone',n=7,bands=4)
    m.group('lower_jaw',(0,.63,-1.34),(1,0,0),-21,2.2)
    m.box((0,-.12,-.45),(.83,.1,.85),'black_iron','lower_jaw')
    for side in (-1,1):
        for z in (-.25,-.55,-.75):m.cylinder((side*.36,-.05,z),(side*.36,.08,z),.035,'paper','lower_jaw',sides=6,end_radius=.003)
    m.group('armored_tail',(0,.75,1.0),(0,1,0),23,3.2)
    for i in range(5):m.cylinder((0,.02,i*.43),(0,.02,(i+1)*.43),.3-i*.054,'black_iron','armored_tail',sides=8,end_radius=.23-i*.049)


def wyrm(m):
    for i in range(7):
        z=(i-3)*.57;y=1.35+.22*math.sin(i*.5)
        m.sphere((0,y,z),(.53-i*.035,.49-i*.026,.47),'basalt',n=12,bands=8)
        m.sphere((0,y+.39,z),(.27,.08,.3),'lava',n=8,bands=5)
    m.sphere((0,1.8,-2.13),(.63,.42,.64),'basalt',n=12,bands=8)
    for s in (-1,1):
        m.sphere((s*.38,1.93,-2.42),(.08,.08,.08),'ember',n=8,bands=6)
        m.cylinder((s*.35,2.05,-2.35),(s*.52,2.48,-2.1),.13,'stone',sides=8,end_radius=.02)
        for z in (-1.4,1.4):m.beam((s*.4,1.24,z),(s*.69,.23,z+.13),.24,.24,'basalt')
    m.group('wyrm_tail',(0,1.27,2),(0,1,0),32,3)
    for i in range(4):m.cylinder((0,.03,i*.5),(0,.08,(i+1)*.5),.25-i*.052,'basalt','wyrm_tail',sides=9,end_radius=.18-i*.05)


def titan(m):
    m.sphere((0,2.7,0),(.88,1.05,.52),'basalt',n=12,bands=8)
    m.sphere((0,4.03,-.04),(.55,.61,.46),'basalt',n=10,bands=8)
    for x in (-.24,.24):m.sphere((x,4.13,-.44),(.095,.095,.04),'lava',n=8,bands=6)
    for s in (-1,1):
        m.beam((s*.74,3.3,0),(s*1.3,1.72,0),.42,.4,'basalt')
        m.sphere((s*1.34,1.53,0),(.32,.35,.33),'basalt',n=9,bands=6)
        m.beam((s*.48,2.12,0),(s*.58,.03,0),.53,.49,'basalt')
        for y in (2.45,2.9,3.28):m.beam((s*.78,y,-.41),(s*.86,y+.17,-.45),.025,.025,'lava')
    m.group('titan_head',(0,3.67,0),(0,1,0),12,4)
    m.cylinder((0,0,0),(0,.37,0),.23,'basalt','titan_head')


def bird(m):
    m.sphere((0,1.75,0),(.41,.48,.75),'black_iron',n=12,bands=8)
    m.sphere((0,2.01,-.68),(.34,.34,.38),'stone',n=10,bands=7)
    m.beam((0,1.91,-.94),(0,1.74,-1.36),.17,.13,'brass')
    for x in (-.16,.16):m.sphere((x,2.1,-.96),(.045,.05,.045),'ember',n=8,bands=5)
    for side,name in ((-1,'left_wing'),(1,'right_wing')):
        m.group(name,(side*.35,1.9,0),(0,0,1),side*27,3.2)
        for i in range(8):
            z=(i-3.5)*.17
            m.quad((0,0,z),(side*1.15,.17,z-.18),(side*2.28,-.14,z-.38),(side*.78,-.28,z+.12),'black_iron',name,double=True)
        m.beam((0,0,0),(side*2.08,-.08,-.16),.12,.11,'stone',name)
        m.beam((side*.21,1.54,0),(side*.29,.1,.14),.16,.16,'black_iron')
        for claw in (-.09,.09):m.cylinder((side*.29,.1,claw),(side*.29,-.13,claw-.16),.035,'steel',sides=7,end_radius=.01)


CREATURES=('ashen_wyrm','caldera_titan','havfrue_modor','jaw_of_the_mire','jungle_warden','sturmkralle')


def create_creature(slug):
    m=Model('boss.'+slug)
    if slug=='ashen_wyrm':wyrm(m)
    elif slug=='caldera_titan':titan(m)
    elif slug=='sturmkralle':bird(m)
    elif slug=='havfrue_modor':sea_monster(m)
    elif slug=='jaw_of_the_mire':crocodile(m)
    else:quadruped(m,slug)
    path=CREATURE_OUT/f'SK_Boss_{slug}_silhouette.gltf'
    data=m.write(path)
    return {'catalogId':'boss.'+slug,**data,'stage':'unrigged_creature_silhouette_alpha',
            'rigged':False,'motion':'node motion preview only'}


def main():
    catalog=json.loads((ROOT/'ContentSource/Higgsfield/HiggsfieldO3DEAssetCatalog.json').read_text())
    bosses={a['id'][5:] for a in catalog['assets'] if a['id'].startswith('boss.')}
    assert set(HUMANS)|set(CREATURES)==bosses
    original=json.loads(BASE.read_text())
    humans=[create_human(slug,configuration,original) for slug,configuration in HUMANS.items()]
    creatures=[create_creature(slug) for slug in CREATURES]
    report={'schemaVersion':1,'humanoidSource':BASE.relative_to(ROOT).as_posix(),
            'mocapSource':'ContentSource/ThirdParty/README.md (CMU/MakeHuman attribution)',
            'humanoids':humans,'creatures':creatures,
            'qualityLimits':['humanoids reuse boarder body topology and generic motions; face, outfit and boss-specific moves need authoring',
                             'creatures are unrigged silhouette models; preview node motions do not animate flesh or locomotion',
                             'weapon attachment orientation and collision require O3DE Editor visual checks']}
    (ROOT/'ContentSource/OpenSource3D/BossManifest.json').write_text(json.dumps(report,indent=2,ensure_ascii=False)+'\n')
    print(len(humans),len(creatures))


if __name__=='__main__':main()
