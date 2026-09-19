#!/usr/bin/env python3
import base64,json,struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'ContentSource/World/Rexa/Story';OUT.mkdir(parents=True,exist_ok=True)
def make(name,boxes,color):
    verts=[];idx=[]
    for cx,cy,cz,sx,sy,sz in boxes:
        b=len(verts);x0,x1=cx-sx/2,cx+sx/2;y0,y1=cy-sy/2,cy+sy/2;z0,z1=cz-sz/2,cz+sz/2
        verts += [(x0,y0,z0),(x1,y0,z0),(x1,y1,z0),(x0,y1,z0),(x0,y0,z1),(x1,y0,z1),(x1,y1,z1),(x0,y1,z1)]
        faces=[0,2,1,0,3,2,4,5,6,4,6,7,0,1,5,0,5,4,1,2,6,1,6,5,2,3,7,2,7,6,3,0,4,3,4,7]
        idx += [b+i for i in faces]
    pos=b''.join(struct.pack('<3f',*v) for v in verts); inds=b''.join(struct.pack('<H',i) for i in idx); raw=pos+inds
    xs=[v[0] for v in verts];ys=[v[1] for v in verts];zs=[v[2] for v in verts]
    doc={'asset':{'version':'2.0','generator':'Dark Arisen authored Alpha Chapter 03 source'},'scene':0,'scenes':[{'nodes':[0]}],'nodes':[{'name':name,'mesh':0}],
         'meshes':[{'name':name,'primitives':[{'attributes':{'POSITION':0},'indices':1,'material':0}]}],
         'materials':[{'pbrMetallicRoughness':{'baseColorFactor':color,'roughnessFactor':0.86}}],
         'buffers':[{'byteLength':len(raw),'uri':'data:application/octet-stream;base64,'+base64.b64encode(raw).decode()}],
         'bufferViews':[{'buffer':0,'byteOffset':0,'byteLength':len(pos),'target':34962},{'buffer':0,'byteOffset':len(pos),'byteLength':len(inds),'target':34963}],
         'accessors':[{'bufferView':0,'componentType':5126,'count':len(verts),'type':'VEC3','min':[min(xs),min(ys),min(zs)],'max':[max(xs),max(ys),max(zs)]},{'bufferView':1,'componentType':5123,'count':len(idx),'type':'SCALAR'}]}
    (OUT/f'{name}.gltf').write_text(json.dumps(doc,separators=(',',':'))+'\n')
make('SM_RexaWarehouseQuarter_Alpha',[(0,0,1.8,18,8,3.6),(0,7,2.6,12,4,5.2),(10,-1,1.4,6,5,2.8),(-10,-2,1.2,4,12,2.4),(4,-7,.5,16,2,1)], [.34,.29,.22,1])
make('SM_ArmadaTenderWreckCove_Alpha',[(0,0,1.1,16,4,2.2),(0,0,2.4,10,3,1.1),(-7,0,3.7,.6,.6,7),(5,1,3,.5,8,.5),(10,7,.7,5,5,1.4),(-11,-8,.8,6,4,1.6)], [.22,.20,.18,1])
make('SM_CrownCitadelApproach_Alpha',[(0,0,3,24,3,6),(-11,8,2.5,3,16,5),(11,8,2.5,3,16,5),(0,16,2,25,4,4),(-9,-6,.7,5,9,1.4),(9,-6,.7,5,9,1.4),(-15,3,.8,5,4,1.6)], [.42,.38,.31,1])
print('generated',*(p.name for p in sorted(OUT.glob('*.gltf'))))
