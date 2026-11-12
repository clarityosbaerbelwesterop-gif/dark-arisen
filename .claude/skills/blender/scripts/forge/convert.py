"""A boss .glb to the committed form (<Name>/<Name>.gltf, .bin, JPEG maps):  convert.py -- <in.glb> <out_dir> <Name>"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import bpy  # first: the bpy wheel provides mathutils

import forge as F

src, out_dir, name = sys.argv[sys.argv.index('--') + 1:][:3]
F.reset()
bpy.ops.import_scene.gltf(filepath=src)
folder = os.path.join(out_dir, name)
os.makedirs(folder, exist_ok=True)
for o in bpy.context.scene.objects:
    o.select_set(o.type == 'MESH')
bpy.ops.export_scene.gltf(filepath=os.path.join(folder, name + '.gltf'), export_format='GLTF_SEPARATE', use_selection=True, export_image_format='AUTO',
                          export_yup=True, export_tangents=True, export_materials='EXPORT')
print('FORGE converted', name, sum(os.path.getsize(os.path.join(folder, f)) for f in os.listdir(folder)), 'bytes')
F.done()
