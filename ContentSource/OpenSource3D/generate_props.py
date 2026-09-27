#!/usr/bin/env python3
"""Deterministic, dependency-free glTF 2.0 prop authoring for the O3DE alpha kit.

Run from the repository root: python ContentSource/OpenSource3D/generate_props.py
The models are individually designed modular first passes, not scanned/PBR hero art.
"""

from __future__ import annotations

import hashlib
import json
import math
import struct
import zlib
from collections import defaultdict
from pathlib import Path

from gltf_format import embedded_gltf, unpack_glb


ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "Engine/O3DE/DarkArisen/Assets/Art/Props/OpenSource3D"
PI = math.pi


def add(a, b): return tuple(x + y for x, y in zip(a, b))
def sub(a, b): return tuple(x - y for x, y in zip(a, b))
def mul(a, k): return tuple(x * k for x in a)
def dot(a, b): return sum(x * y for x, y in zip(a, b))
def cross(a, b): return (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0])
def unit(a):
    l = math.sqrt(dot(a, a))
    return mul(a, 1/l) if l else (0, 1, 0)


MATS = {
    "oak": ((0.31, 0.16, 0.075), 0.0, .83),
    "oak_light": ((0.47, .26, .12), 0.0, .78),
    "iron": ((.14, .17, .18), .72, .62),
    "black_iron": ((.08, .085, .085), .65, .73),
    "steel": ((.43, .47, .48), .84, .36),
    "brass": ((.57, .37, .12), .78, .36),
    "rope": ((.48, .37, .22), 0.0, .95),
    "canvas": ((.58, .51, .37), 0.0, .96),
    "red_canvas": ((.38, .085, .065), 0.0, .92),
    "leather": ((.25, .13, .07), 0.0, .83),
    "paper": ((.77, .67, .44), 0.0, 1.0),
    "glass": ((.37, .49, .48), .08, .25),
    "ember": ((.95, .57, .12), .0, .5),
    "stone": ((.28, .28, .26), .0, 1.0),
    "basalt": ((.12, .13, .13), .0, .98),
    "moss": ((.14, .23, .12), .0, .95),
    "leaf": ((.19, .34, .14), .0, .92),
    "coral": ((.46, .27, .20), .0, .89),
    "plaster": ((.65, .57, .43), .0, .94),
    "roof": ((.39, .21, .14), .0, .91),
    "water": ((.05, .25, .29), .0, .29),
    "lava": ((.95, .19, .025), .0, .44),
}


def png(color, seed):
    """Small repeatable surface-noise color map, embedded in each standalone GLB."""
    n = 32
    base = tuple(round(v*255) for v in color)
    pixels = bytearray()
    for y in range(n):
        pixels.append(0)
        for x in range(n):
            h = hashlib.blake2s(f"{seed}-{x//2}-{y//2}".encode(), digest_size=2).digest()
            grain = (h[0] / 255 - .5) * .15
            if seed.startswith("oak"):
                grain += .07 * math.sin((x + (h[1] % 5))*1.3)
            pixels.extend(max(0, min(255, int(v * (1 + grain)))) for v in base)
            pixels.append(255)
    def chunk(t, d): return struct.pack(">I", len(d)) + t + d + struct.pack(">I", zlib.crc32(t+d)&0xffffffff)
    return b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", n, n, 8, 6, 0, 0, 0)) + chunk(b"IDAT", zlib.compress(bytes(pixels), 9)) + chunk(b"IEND", b"")


class Model:
    def __init__(self, label):
        self.label = label
        self.tris = defaultdict(list)  # (group, material) -> [(positions, uvs)]
        self.groups = {"root": (0, 0, 0)}
        self.motions = []

    def group(self, name, at, axis=(0, 0, 1), degrees=0, seconds=2):
        self.groups[name] = at
        self.motions.append((name, axis, degrees, seconds))

    def static_group(self, name, at):
        self.groups[name] = at

    def triangle(self, a, b, c, mat="oak", group="root", uv=((0, 0), (1, 0), (0, 1))):
        if dot(cross(sub(b, a), sub(c, a)), cross(sub(b, a), sub(c, a))) > 1e-13:
            self.tris[group, mat].append(((a, b, c), uv))

    def quad(self, a, b, c, d, mat="oak", group="root", double=False):
        self.triangle(a, b, c, mat, group, ((0, 0), (1, 0), (1, 1)))
        self.triangle(a, c, d, mat, group, ((0, 0), (1, 1), (0, 1)))
        if double:
            self.quad(d, c, b, a, mat, group)

    def box(self, p, size, mat="oak", group="root"):
        x,y,z = p; a,b,c = (v/2 for v in size)
        v = [(x+u*a, y+w*b, z+t*c) for u,w,t in [(-1,-1,-1),(1,-1,-1),(1,1,-1),(-1,1,-1),(-1,-1,1),(1,-1,1),(1,1,1),(-1,1,1)]]
        for f in [(0,3,2,1),(4,5,6,7),(0,4,7,3),(1,2,6,5),(3,7,6,2),(0,1,5,4)]:
            self.quad(*(v[i] for i in f),mat,group)

    def beam(self, start, end, width, depth, mat="oak", group="root"):
        w=unit(sub(end,start)); ref=(0,1,0) if abs(w[1])<.85 else (1,0,0)
        u=unit(cross(w,ref)); v=unit(cross(w,u))
        verts=[]
        for center in (start,end):
            verts.extend([add(add(center,mul(u,s*width/2)),mul(v,t*depth/2)) for s,t in [(-1,-1),(1,-1),(1,1),(-1,1)]])
        self.quad(verts[3],verts[2],verts[1],verts[0],mat,group)
        self.quad(verts[4],verts[5],verts[6],verts[7],mat,group)
        for i in range(4):
            j=(i+1)%4;self.quad(verts[i],verts[j],verts[4+j],verts[4+i],mat,group)

    def cylinder(self, start, end, radius, mat="oak", group="root", sides=16, end_radius=None):
        direction=unit(sub(end,start)); ref=(0,1,0) if abs(direction[1])<.9 else (1,0,0)
        u=unit(cross(direction,ref));v=unit(cross(direction,u));r2=radius if end_radius is None else end_radius
        for i in range(sides):
            t0=2*PI*i/sides;t1=2*PI*(i+1)/sides
            def point(center,r,t):return add(center,add(mul(u,r*math.cos(t)),mul(v,r*math.sin(t))))
            a=point(start,radius,t0);b=point(start,radius,t1);c=point(end,r2,t1);d=point(end,r2,t0)
            self.quad(a,b,c,d,mat,group)
            if radius: self.triangle(start,b,a,mat,group)
            if r2: self.triangle(end,d,c,mat,group)

    def sphere(self, p, radii, mat="oak", group="root", n=12, bands=8):
        def q(t,f):return (p[0]+radii[0]*math.sin(t)*math.cos(f),p[1]+radii[1]*math.cos(t),p[2]+radii[2]*math.sin(t)*math.sin(f))
        for j in range(bands):
            for i in range(n):
                t0=PI*j/bands;t1=PI*(j+1)/bands;f0=2*PI*i/n;f1=2*PI*(i+1)/n
                if j==0:self.triangle(q(t0,f0),q(t1,f1),q(t1,f0),mat,group)
                elif j==bands-1:self.triangle(q(t0,f0),q(t0,f1),q(t1,f0),mat,group)
                else:self.quad(q(t0,f0),q(t0,f1),q(t1,f1),q(t1,f0),mat,group)

    def path(self, points, radius, mat="rope", group="root", sides=6):
        for a,b in zip(points,points[1:]):self.cylinder(a,b,radius,mat,group,sides)

    def ring(self, center, major, minor, axis=(0,1,0), mat="iron", group="root", pieces=24):
        ax=unit(axis);ref=(1,0,0) if abs(ax[0])<.8 else (0,1,0)
        u=unit(cross(ax,ref));v=unit(cross(ax,u))
        for i in range(pieces):
            def q(t):return add(center,add(mul(u,major*math.cos(t)),mul(v,major*math.sin(t))))
            self.cylinder(q(i*2*PI/pieces),q((i+1)*2*PI/pieces),minor,mat,group,6)

    def write(self, path):
        blob=bytearray();views=[];accessors=[];meshes=[];nodes=[{"name":self.label,"children":[]}]
        material_names=list(dict.fromkeys(mat for (_,mat) in self.tris))
        images=[];textures=[];materials=[]
        def append_bytes(data, target=None):
            while len(blob)%4:blob.append(0)
            off=len(blob);blob.extend(data);entry={"buffer":0,"byteOffset":off,"byteLength":len(data)}
            if target:entry["target"]=target
            views.append(entry);return len(views)-1
        for name in material_names:
            color,metal,rough=MATS[name]
            idx=append_bytes(png(color,name))
            images.append({"name":f"{name}_grain","bufferView":idx,"mimeType":"image/png"})
            textures.append({"source":len(images)-1})
            material={"name":name,"pbrMetallicRoughness":{"baseColorTexture":{"index":len(textures)-1},"metallicFactor":metal,"roughnessFactor":rough},"doubleSided":name in {"canvas","red_canvas","paper","glass","leaf","water","lava"}}
            if name in {"ember","lava"}:
                material["emissiveFactor"]=[1.0,.56,.12] if name=="ember" else [1.0,.22,.03]
                material["emissiveTexture"]={"index":len(textures)-1}
            materials.append(material)
        for group in self.groups:
            prims=[]
            for mat in material_names:
                faces=self.tris.get((group,mat),[])
                if not faces:continue
                points=[];normals=[];uvs=[];indices=[]
                for xyz,tex in faces:
                    n=unit(cross(sub(xyz[1],xyz[0]),sub(xyz[2],xyz[0])))
                    for p,uv in zip(xyz,tex):points.extend(p);normals.extend(n);uvs.extend(uv);indices.append(len(indices))
                def acc(data,fmt,typ,comp,tag,limit=False):
                    raw=struct.pack("<"+fmt*len(data),*data)
                    vi=append_bytes(raw,34963 if tag=="INDEX" else 34962)
                    d={"bufferView":vi,"componentType":comp,"count":len(data)//({"VEC3":3,"VEC2":2,"SCALAR":1}[typ]),"type":typ}
                    if limit:
                        d["min"]=[min(data[i::3]) for i in range(3)]
                        d["max"]=[max(data[i::3]) for i in range(3)]
                    accessors.append(d);return len(accessors)-1
                ix=acc(indices,'I','SCALAR',5125,'INDEX')
                pos=acc(points,'f','VEC3',5126,'POSITION',True)
                norm=acc(normals,'f','VEC3',5126,'NORMAL')
                uv=acc(uvs,'f','VEC2',5126,'TEXCOORD_0')
                prims.append({"attributes":{"POSITION":pos,"NORMAL":norm,"TEXCOORD_0":uv},"indices":ix,"material":material_names.index(mat),"mode":4})
            if not prims:continue
            meshes.append({"name":group,"primitives":prims})
            node={"name":group,"mesh":len(meshes)-1}
            if group!="root":node["translation"]=list(self.groups[group])
            nodes.append(node);nodes[0]["children"].append(len(nodes)-1)
        animations=[]
        for name,axis,degrees,seconds in self.motions:
            node_id=next((i for i,n in enumerate(nodes) if n["name"]==name),None)
            if node_id is None:continue
            keys=[0.0,seconds*.25,seconds*.5,seconds*.75,seconds]
            theta=[0,degrees*.25,degrees*.5,degrees*.75,degrees]
            ax=unit(axis)
            values=[v for deg in theta for v in (*mul(ax,math.sin(deg*PI/360)),math.cos(deg*PI/360))]
            iv=append_bytes(struct.pack('<5f',*keys))
            ov=append_bytes(struct.pack('<20f',*values))
            accessors.append({"bufferView":iv,"componentType":5126,"count":5,"type":"SCALAR","min":[0.0],"max":[seconds]});ia=len(accessors)-1
            accessors.append({"bufferView":ov,"componentType":5126,"count":5,"type":"VEC4"});oa=len(accessors)-1
            animations.append({"name":f"{name}_preview","samplers":[{"input":ia,"output":oa,"interpolation":"LINEAR"}],"channels":[{"sampler":0,"target":{"node":node_id,"path":"rotation"}}]})
        gltf={"asset":{"generator":"DarkArisen OpenSource3D alpha prop generator","version":"2.0"},"scene":0,"scenes":[{"nodes":[0]}],"nodes":nodes,"meshes":meshes,"materials":materials,"textures":textures,"images":images,"samplers":[{"wrapS":10497,"wrapT":10497}],"buffers":[{"byteLength":len(blob)}],"bufferViews":views,"accessors":accessors}
        if animations:gltf["animations"]=animations
        j=json.dumps(gltf,separators=(',',':'),ensure_ascii=False).encode();j+=b' '*((-len(j))%4)
        blob.extend(b'\0'*((-len(blob))%4))
        data=b'glTF'+struct.pack('<II',2,12+8+len(j)+8+len(blob))+struct.pack('<I4s',len(j),b'JSON')+j+struct.pack('<I4s',len(blob),b'BIN\0')+blob
        if path.suffix == '.gltf':
            doc,packed=unpack_glb(data)
            data=embedded_gltf(doc,packed)
        path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(data)
        return {"file":path.relative_to(ROOT).as_posix(),"bytes":len(data),"triangles":sum(len(v) for v in self.tris.values()),"meshNodes":len(meshes),"materials":len(materials),"animations":[a["name"] for a in animations],"sha256":hashlib.sha256(data).hexdigest(),"stage":"modular_alpha"}


def blade(m, style):
    offset=.08 if style=='saber' else -.05
    m.cylinder((0,-.12,0),(0,.27,0),.055,'leather',sides=12)
    for i in range(8):m.ring((0,-.1+i*.045,0),.055,.005,(0,1,0),'brass',pieces=12)
    m.sphere((0,-.16,0),(.075,.055,.075),'brass')
    m.beam((-.16,.29,0),(.16,.29,0),.035,.045,'brass')
    m.ring((.1,.13,0),.17,.015,(0,0,1),'brass',pieces=14)
    p=[(-.045,.30,-.014),(.045,.30,-.014),(.045+offset,.92,-.012),(.02+offset,1.12,0),(-.02+offset,1.12,0),(-.045+offset,.92,-.012)]
    m.quad(p[0],p[1],p[2],p[5],'steel',double=True)
    m.triangle(p[5],p[2],p[3],'steel');m.triangle(p[5],p[3],p[4],'steel')
    m.beam((0,.37,-.023),(offset*.75,1.02,-.023),.008,.005,'steel')


def barrel(m, powder=False):
    for i in range(12):
        t=2*PI*i/12;r=.31+(.06 if 2<=i<=9 else 0)
        a=(r*math.cos(t),.13,r*math.sin(t));b=(r*math.cos(t),.88,r*math.sin(t))
        m.beam(a,b,.12,.075,'oak' if i%3 else 'oak_light')
    for y,r in ((.14,.32),(.32,.37),(.68,.37),(.87,.32)):
        m.ring((0,y,0),r,.025,(0,1,0),'iron',pieces=18)
    m.cylinder((0,.89,0),(0,.91,0),.295,'oak',sides=18)
    if powder:
        m.cylinder((0,.91,0),(0,.96,0),.07,'black_iron')
        m.box((0,.51,-.376),(.32,.18,.014),'red_canvas')


def cannonballs(m):
    for layer in range(3):
        n=3-layer
        for x in range(n):
            for z in range(n):m.sphere(((x-(n-1)/2)*.25,.12+layer*.21,(z-(n-1)/2)*.25),(.12,.12,.12),'black_iron',n=12)
    for z in (-.39,.39):m.beam((-.42,.04,z),(.42,.04,z),.05,.08)


def make(asset):
    m=Model(asset)
    if asset in ('jake_saber','draven_saber','cutlass'):
        blade(m,'cutlass' if asset=='cutlass' else 'saber')
        if asset=='draven_saber':m.ring((0,.25,0),.22,.016,(0,0,1),'brass')
    elif asset=='flintlock_pistol':
        m.beam((0,.12,0),(.12,.23,.57),.12,.13,'oak')
        m.beam((0,.11,-.03),(-.06,-.14,-.11),.11,.1,'oak')
        m.cylinder((.12,.27,.08),(.12,.27,.72),.05,'black_iron')
        m.cylinder((.12,.27,.70),(.12,.27,.73),.061,'steel')
        m.box((-.02,.21,.28),(.022,.1,.2),'brass')
        m.beam((-.02,.18,.11),(-.04,.26,.05),.02,.02,'iron')
        m.ring((-.04,.12,.03),.055,.011,(1,0,0),'iron')
        m.cylinder((.1,.14,.15),(.1,.14,.69),.012,'oak_light',sides=8)
    elif asset=='powder_bomb':
        m.sphere((0,.23,0),(.22,.23,.22),'black_iron',n=20,bands=12)
        m.cylinder((0,.43,0),(0,.49,0),.075,'iron')
        m.path([(0,.49,0),(.04,.56,0),(.09,.6,.02)],.015,'rope')
        m.ring((0,.2,0),.22,.015,(0,1,0),'iron')
    elif asset=='boarding_axe':
        m.cylinder((0,0,0),(0,1.15,0),.043,'oak',sides=12,end_radius=.036)
        m.cylinder((0,.93,0),(0,1.13,0),.049,'iron',sides=12)
        m.beam((.02,1.04,0),(.36,1.06,0),.18,.08,'steel')
        m.beam((-.1,1.05,0),(-.19,1.05,0),.08,.07,'steel')
        m.beam((.37,1.06,0),(.47,1.06,0),.025,.05,'steel')
    elif asset=='swivel_gun':
        m.cylinder((0,.45,-.35),(0,.46,.65),.14,'black_iron',sides=20,end_radius=.11)
        for z in (-.30,.12,.55):m.ring((0,.46,z),.145,.018,(0,0,1),'iron')
        m.cylinder((0,.46,.65),(0,.46,.69),.125,'iron')
        m.beam((-.21,.43,-.08),(.21,.43,-.08),.06,.07,'iron')
        m.cylinder((0,.05,0),(0,.47,0),.06,'iron')
        m.box((0,.05,0),(.3,.08,.32),'oak')
        m.beam((0,.46,-.37),(0,.36,-.55),.038,.035,'oak')
        m.group('elevation_crank',(0,.45,-.08),(1,0,0),20)
        m.ring((0,0,0),.12,.016,(1,0,0),'brass','elevation_crank')
    elif asset=='ship_wheel':
        m.box((0,.55,0),(.28,1.1,.32),'oak')
        m.group('wheel',(0,1.25,0),(0,0,1),360,5)
        m.ring((0,0,0),.63,.045,(0,0,1),'oak','wheel',pieces=32)
        for i in range(12):
            t=2*PI*i/12;u,v=math.cos(t),math.sin(t)
            m.beam((.11*u,.11*v,0),(.66*u,.66*v,0),.045,.05,'oak_light','wheel')
            m.cylinder((.63*u,.63*v,0),(.79*u,.79*v,0),.035,'oak','wheel',sides=8)
        m.cylinder((0,0,-.09),(0,0,.09),.13,'brass','wheel')
    elif asset=='capstan':
        m.cylinder((0,0,0),(0,.1,0),.64,'oak',sides=16)
        m.group('capstan',(0,.1,0),(0,1,0),360,6)
        m.cylinder((0,0,0),(0,1.22,0),.23,'oak','capstan',sides=12,end_radius=.16)
        for y in (.18,.7,1.16):m.ring((0,y,0),.23 if y<1 else .17,.025,(0,1,0),'iron','capstan')
        for i in range(8):
            t=2*PI*i/8;m.beam((0,.9,0),(.9*math.cos(t),.9,.9*math.sin(t)),.07,.065,'oak','capstan')
    elif asset=='anchor':
        m.beam((0,.12,0),(0,1.7,0),.12,.12,'iron')
        m.ring((0,1.82,0),.17,.042,(0,0,1),'iron')
        m.beam((-.6,1.47,0),(.6,1.47,0),.09,.12,'oak')
        for s in (-1,1):
            m.beam((0,.14,0),(.48*s,.2,0),.09,.1,'iron')
            m.beam((.48*s,.2,0),(.64*s,.54,0),.07,.1,'iron')
            m.beam((.64*s,.53,-.15),(.64*s,.53,.15),.19,.055,'iron')
    elif asset=='ship_lantern':
        m.box((0,0,0),(.32,.06,.32),'black_iron')
        m.group('swing',(0,.65,0),(0,0,1),12,2.5)
        m.box((0,-.36,0),(.33,.05,.33),'brass','swing')
        m.box((0,.33,0),(.33,.09,.33),'brass','swing')
        for x in (-.15,.15):
            for z in (-.15,.15):m.beam((x,-.33,z),(x,.3,z),.019,.019,'brass','swing')
        for z in (-.151,.151):m.quad((-.13,-.30,z),(.13,-.30,z),(.13,.28,z),(-.13,.28,z),'glass','swing',double=True)
        m.sphere((0,-.16,0),(.06,.1,.06),'ember','swing')
        m.ring((0,.42,0),.12,.018,(0,0,1),'iron','swing')
    elif asset=='chart_table':
        m.box((0,.8,0),(1.35,.095,.82),'oak')
        for x in (-.54,.54):
            for z in (-.27,.27):m.box((x,.39,z),(.085,.78,.08),'oak')
        m.box((0,.66,0),(1.06,.17,.58),'oak_light')
        m.box((-.2,.858,0),(.67,.005,.48),'paper')
        for i in range(5):m.path([(-.49+i*.13,.863,-.16),(-.38+i*.12,.864,0),(-.48+i*.11,.863,.18)],.004,'rope')
        m.cylinder((.44,.85,0),(.44,.875,0),.07,'brass')
    elif asset=='cargo_crate':
        for y in (.07,.22,.37,.52):
            for z in (-.36,.36):m.box((0,y,z),(.78,.13,.06),'oak' if y<.5 else 'oak_light')
            for x in (-.39,.39):m.box((x,y,0),(.06,.13,.67),'oak')
        m.box((0,.59,0),(.82,.06,.74),'oak_light')
        for s in (-1,1):m.beam((s*.38,.02,-.34),(s*.38,.6,.34),.035,.025,'iron')
    elif asset in ('oak_barrel','black_powder_keg'):barrel(m,asset=='black_powder_keg')
    elif asset=='rope_coil':
        for h in range(4):
            pts=[(.37*math.cos(i*2*PI/48),.065+h*.052,.37*math.sin(i*2*PI/48)) for i in range(49)]
            m.path(pts,.026,'rope')
        m.path([(.37,.23,0),(.28,.27,.1),(.2,.28,.33),(.27,.05,.5)],.025,'rope')
    elif asset=='market_stall':
        for x in (-1,1):
            for z in (-.55,.55):m.box((x,1.1,z),(.075,2.2,.075),'oak')
        m.box((0,.95,0),(2.15,.095,1.2),'oak')
        for x in (-.8,-.4,0,.4,.8):m.beam((x,2.2,-.72),(x,2.45,0),.035,.05,'oak')
        for x in (-.8,-.4,0,.4,.8):m.beam((x,2.45,0),(x,2.2,.72),.035,.05,'oak')
        for x0,x1 in ((-1.1,-.55),(-.55,0),(0,.55),(.55,1.1)):
            m.quad((x0,2.2,-.75),(x1,2.2,-.75),(x1,2.45,0),(x0,2.45,0),'red_canvas' if x0<-.5 or x0>=0 else 'canvas',double=True)
            m.quad((x0,2.45,0),(x1,2.45,0),(x1,2.2,.75),(x0,2.2,.75),'red_canvas' if x0<-.5 or x0>=0 else 'canvas',double=True)
    elif asset=='dock_crane':
        m.box((0,.1,0),(1,.2,1),'stone')
        m.beam((0,.15,0),(0,3,0),.21,.24,'oak')
        m.beam((0,2.9,0),(2.3,3.7,0),.19,.2,'oak')
        m.beam((0,1.15,0),(1.55,3.43,0),.12,.12,'oak')
        m.ring((2.18,3.62,0),.15,.025,(0,0,1),'iron')
        m.path([(2.18,3.62,0),(2.15,2.25,0)],.018,'rope')
        m.group('hook',(2.15,2.25,0),(0,0,1),9,2)
        m.ring((0,-.09,0),.13,.027,(0,0,1),'iron','hook')
        m.path([(0,-.18,0),(-.05,-.31,0),(.06,-.34,0)],.026,'iron','hook')
    elif asset=='fishing_net':
        for i in range(11):
            x=-1+i*.2;m.path([(x,.12,-.52+j*.13) for j in range(9)],.007,'rope')
        for j in range(9):
            z=-.52+j*.13;m.path([(-1+i*.2,.12,z) for i in range(11)],.007,'rope')
        for x in (-1,-.5,0,.5,1):m.sphere((x,.12,-.53),(.04,.04,.04),'oak_light',n=8)
    elif asset=='prison_cage':
        for y in (.07,1.9):m.box((0,y,0),(1.65,.12,1.45),'oak')
        for x in (-.77,.77):
            for z in (-.67,.67):m.box((x,.95,z),(.07,1.9,.07),'iron')
        for x in (-.55,-.275,0,.275,.55):
            for z in (-.69,.69):m.cylinder((x,.12,z),(x,1.85,z),.016,'iron',sides=8)
        for z in (-.4,-.2,0,.2,.4):m.cylinder((-.78,.12,z),(-.78,1.85,z),.016,'iron',sides=8)
        m.group('door',(.78,0,-.68),(0,1,0),75,2)
        for z in (.08,.28,.48,.68,1.08,1.28):m.cylinder((0,.12,z),(0,1.85,z),.016,'iron','door',sides=8)
        for y in (.2,1.75):m.beam((0,y,0),(0,y,1.35),.035,.035,'iron','door')
    elif asset=='shackles':
        for x in (-.43,.43):
            m.ring((x,.12,0),.17,.036,(0,1,0),'iron')
            m.beam((x-.14,.08,0),(x+.14,.08,0),.026,.026,'iron')
        m.path([(-.26,.12,0),(-.16,.07,.05),(0,.1,.03),(.16,.07,.05),(.26,.12,0)],.027,'iron')
    elif asset=='salazar_pocket_watch':
        m.cylinder((0,0,0),(0,0,.07),.22,'brass',sides=24)
        m.cylinder((0,0,.071),(0,0,.073),.18,'paper',sides=24)
        m.ring((0,0,.07),.19,.018,(0,0,1),'brass')
        for i in range(12):
            t=2*PI*i/12;m.beam((.155*math.cos(t),.155*math.sin(t),.076),(.17*math.cos(t),.17*math.sin(t),.077),.007,.006,'iron')
        m.group('watch_hands',(0,0,.084),(0,0,1),360,12)
        m.beam((0,0,0),(0,.125,0),.014,.005,'black_iron','watch_hands')
        m.beam((0,0,0),(-.09,0,0),.018,.005,'black_iron','watch_hands')
        m.ring((0,.25,0),.06,.018,(0,0,1),'brass')
        m.path([(.01,.31,0),(.1,.36,0),(.3,.29,0),(.42,.42,0)],.018,'brass')
    elif asset=='medicine_satchel':
        m.box((0,.33,0),(.72,.57,.31),'leather')
        m.box((0,.65,.17),(.73,.2,.06),'oak')
        m.box((0,.55,.209),(.14,.18,.024),'brass')
        m.path([(-.34,.43,0),(-.38,.92,0),(-.22,1.17,0),(.22,1.17,0),(.38,.92,0),(.34,.43,0)],.035,'leather')
        for x in (-.25,.25):m.cylinder((x,.32,.166),(x,.48,.167),.025,'brass',sides=8)
    elif asset=='marine_compass':
        m.cylinder((0,.04,0),(0,.15,0),.29,'brass',sides=24)
        m.cylinder((0,.151,0),(0,.155,0),.25,'paper',sides=24)
        m.ring((0,.16,0),.27,.025,(0,1,0),'brass')
        for i in range(8):
            t=2*PI*i/8;m.beam((.19*math.cos(t),.16,.19*math.sin(t)),(.24*math.cos(t),.16,.24*math.sin(t)),.009,.009,'iron')
        m.group('needle',(0,.18,0),(0,1,0),360,7)
        m.beam((0,0,-.21),(0,0,.21),.035,.008,'black_iron','needle')
        m.beam((0,0,0),(0,0,-.21),.032,.008,'red_canvas','needle')
        m.sphere((0,.01,0),(.027,.02,.027),'brass','needle')
    elif asset=='spyglass':
        for z0,z1,r in ((0,.42,.105),(.4,.88,.084),(.87,1.37,.067)):
            m.cylinder((0,0,z0),(0,0,z1),r,'brass',sides=20)
            for z in (z0,z1):m.ring((0,0,z),r+.006,.012,(0,0,1),'black_iron')
        for z,r in ((0,.105),(1.37,.067)):
            m.cylinder((0,0,z),(0,0,z+.009),r*.9,'glass',sides=20)
        m.ring((0,0,.25),.115,.012,(0,0,1),'leather')
    elif asset=='cannonball_stack':cannonballs(m)
    elif asset=='weapon_rack':
        for x in (-.62,.62):m.box((x,.9,0),(.11,1.8,.24),'oak')
        for y in (.13,.78,1.48):m.box((0,y,0),(1.45,.095,.22),'oak')
        for x in (-.45,-.15,.15,.45):
            for y in (.8,1.5):m.beam((x,y,-.11),(x,y,-.42),.035,.035,'iron')
        for x in (-.62,.62):m.beam((x,.2,0),(x*.9,.2,.52),.07,.07,'oak')
    elif asset=='hammock':
        for s in (-1,1):
            m.cylinder((s*1.18,.67,0),(s*1.18,1.53,0),.055,'oak')
        for i in range(16):
            x0=-.96+i*.12;x1=x0+.12
            y0=.62+.33*(abs(x0)/1.0)**2;y1=.62+.33*(abs(x1)/1.0)**2
            m.quad((x0,y0,-.45),(x1,y1,-.45),(x1,y1,.45),(x0,y0,.45),'canvas',double=True)
        for s in (-1,1):
            for z in (-.42,0,.42):m.path([(s*.95,.93,z),(s*1.18,1.38,0)],.012,'rope')
    elif asset=='forge_anvil':
        m.box((0,.15,0),(.69,.25,.41),'iron')
        m.box((0,.38,0),(.46,.3,.33),'black_iron')
        m.box((-.12,.57,0),(.72,.13,.36),'steel')
        m.cylinder((.16,.57,0),(.69,.56,0),.17,'steel',sides=12,end_radius=.015)
        for x in (-.22,.22):m.box((x,.03,0),(.16,.1,.48),'iron')
    else:raise ValueError(asset)
    return m


ASSETS = [
    'jake_saber','draven_saber','flintlock_pistol','powder_bomb','boarding_axe','cutlass',
    'swivel_gun','ship_wheel','capstan','anchor','ship_lantern','chart_table',
    'cargo_crate','oak_barrel','rope_coil','market_stall','dock_crane','fishing_net',
    'prison_cage','shackles','salazar_pocket_watch','medicine_satchel','marine_compass',
    'spyglass','black_powder_keg','cannonball_stack','weapon_rack','hammock','forge_anvil',
]


def main():
    report={}
    for slug in ASSETS:
        asset='prop.'+slug
        report[asset]=make(asset.removeprefix('prop.')).write(OUT/f'SM_{slug}.gltf')
    print(json.dumps(report,indent=2))


if __name__=='__main__':main()
