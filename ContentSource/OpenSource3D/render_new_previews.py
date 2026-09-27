#!/usr/bin/env python3
"""Create lightweight orthographic source-art contact sheets (Pillow/NumPy)."""

import io
import json
import math

import numpy as np
from PIL import Image, ImageDraw, ImageFont

from generate_heroes import ART, accessor_array
from generate_props import ROOT
from gltf_format import read_embedded_gltf


HERE=ROOT/'ContentSource/OpenSource3D'
PREVIEWS=ROOT/'Docs/Art'


def image_map(doc,blob,path,index):
    tex=doc['textures'][index]
    entry=doc['images'][tex['source']]
    if 'uri' in entry:
        img=Image.open(path.parent/entry['uri'])
    else:
        v=doc['bufferViews'][entry['bufferView']]
        off=v.get('byteOffset',0)
        img=Image.open(io.BytesIO(blob[off:off+v['byteLength']]))
    return np.asarray(img.convert('RGB').resize((128,128),Image.Resampling.BILINEAR))


def geometry(path):
    doc,blob=read_embedded_gltf(path)
    mesh_nodes={n['mesh']:np.array(n.get('translation',[0,0,0])) for n in doc['nodes'] if 'mesh' in n}
    images={}
    faces=[];colors=[]
    for mi,mesh in enumerate(doc['meshes']):
        for primitive in mesh['primitives']:
            a=primitive['attributes']
            pos=accessor_array(doc,blob,a['POSITION'])+mesh_nodes.get(mi,np.zeros(3))
            uv=accessor_array(doc,blob,a['TEXCOORD_0'])
            triangles=accessor_array(doc,blob,primitive['indices']).reshape(-1,3).astype(np.int32)
            verts=pos[triangles]
            material=doc['materials'][primitive['material']]['pbrMetallicRoughness']
            factor=np.array(material.get('baseColorFactor',[1,1,1,1])[:3],dtype=np.float32)
            if 'baseColorTexture' in material:
                ix=material['baseColorTexture']['index']
                if ix not in images:images[ix]=image_map(doc,blob,path,ix)
                coords=np.mean(uv[triangles],axis=1)
                px=(np.mod(coords[:,0],1)*127).astype(np.int32)
                py=((1-np.mod(coords[:,1],1))*127).astype(np.int32)
                c=images[ix][py,px].astype(np.float32)
            else:c=np.tile(np.array([200,200,200],dtype=np.float32),(len(triangles),1))
            faces.append(verts)
            colors.append(c*factor)
    return np.concatenate(faces),np.concatenate(colors)


def tile(path,title,kind):
    tris,albedo=geometry(path)
    w,h=(330,246) if kind=='ships' else (238,292)
    canvas=Image.new('RGB',(w,h),(19,26,35));draw=ImageDraw.Draw(canvas)
    camera=np.array((.92,.48,-.69) if kind=='ships' else (.54,.27,-1.5),dtype=np.float64)
    camera/=np.linalg.norm(camera)
    right=np.cross((0,1,0),camera);right/=np.linalg.norm(right)
    up=np.cross(camera,right);up/=np.linalg.norm(up)
    projected=np.stack((tris@right,tris@up),axis=-1)
    low=projected.min(axis=(0,1));high=projected.max(axis=(0,1))
    margins=(17,17,22,41)
    scale=min((w-margins[0]-margins[1])/max(.1,high[0]-low[0]),
              (h-margins[2]-margins[3])/max(.1,high[1]-low[1]))
    center=(low+high)*.5
    xy=(projected-center)*scale+np.array([w/2,(h-margins[3]+margins[2])/2])
    xy[:,:,1]=h-xy[:,:,1]-margins[3]*.3
    normal=np.cross(tris[:,1]-tris[:,0],tris[:,2]-tris[:,0])
    norm=np.linalg.norm(normal,axis=1)
    norm[norm==0]=1
    normal/=norm[:,None]
    light=np.array([-.3,.85,-.5]);light/=np.linalg.norm(light)
    shade=.49+.51*np.abs(normal@light)
    rgba=np.uint8(np.clip(albedo*shade[:,None],0,255))
    depth=np.mean(tris@camera,axis=1)
    for i in np.argsort(depth):
        if norm[i]<1e-8:continue
        draw.polygon([tuple(x) for x in xy[i]],fill=tuple(int(v) for v in rgba[i]))
    draw.rectangle((0,h-30,w,h),fill=(34,42,53))
    font=ImageFont.load_default()
    draw.text((8,h-23),title,fill=(237,237,228),font=font)
    return canvas


def main():
    ships=json.loads((HERE/'ShipManifest.json').read_text())['ships']
    heroes=json.loads((HERE/'HeroManifest.json').read_text())['heroes']
    for kind,records,cols in [('ships',ships,5),('characters',heroes,6)]:
        size=(330,246) if kind=='ships' else (238,292)
        rows=math.ceil(len(records)/cols)
        sheet=Image.new('RGB',(size[0]*cols,size[1]*rows),(12,16,24))
        for ix,record in enumerate(records):
            path=ROOT/record['file'] if kind=='ships' else ART/'Models'/record['sourcePerson']
            item=tile(path,record['catalogId'].split('.',1)[-1],kind)
            sheet.paste(item,((ix%cols)*size[0],(ix//cols)*size[1]))
        name='OpenSource3D_Ships_Preview.png' if kind=='ships' else 'OpenSource3D_Characters_Preview.png'
        sheet.save(PREVIEWS/name,optimize=True)
        print(name,sheet.size)


if __name__=='__main__':main()
