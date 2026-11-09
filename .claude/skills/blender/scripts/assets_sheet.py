# Showcase of decimated scans: blender -b --factory-startup -P assets_sheet.py -- out.png id1 id2 ...
import bpy, sys, math, os
from mathutils import Vector
a = sys.argv[sys.argv.index('--') + 1:]; out = a[0]; ids = a[1:]
ROOT = os.environ.get('DARK_ARISEN_ROOT') or os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '..'))
G = os.path.join(ROOT, 'Engine', 'O3DE', 'DarkArisen', 'Assets', 'Art', 'Scanned', 'game')
bpy.ops.wm.read_factory_settings(use_empty=True); sc = bpy.context.scene
x = 0.0
for i in ids:
    before = set(bpy.data.objects)
    bpy.ops.import_scene.gltf(filepath=os.path.join(G, i + '.gltf'))
    new = [o for o in bpy.data.objects if o not in before and o.type == 'MESH']
    xs = [ (o.matrix_world @ Vector(c)).x for o in new for c in o.bound_box]
    w = max(xs) - min(xs); cx = (max(xs) + min(xs)) / 2
    for o in new: o.location.x += x + w / 2 - cx
    x += w + 1.5
bpy.ops.mesh.primitive_plane_add(size=400); g = bpy.context.object
m = bpy.data.materials.new('g'); m.use_nodes = True; m.node_tree.nodes['Principled BSDF'].inputs['Base Color'].default_value = (0.25, 0.22, 0.18, 1); g.data.materials.append(m)
w = bpy.data.worlds.new('w'); sc.world = w; w.use_nodes = True; nt = w.node_tree
env = nt.nodes.new('ShaderNodeTexEnvironment'); env.image = bpy.data.images.load(os.path.join(ROOT, 'Engine', 'O3DE', 'DarkArisen', 'Assets', 'Art', 'Scanned', 'hdri', 'kloofendal_48d_partly_cloudy_puresky_4k.hdr'))
nt.links.new(env.outputs['Color'], nt.nodes['Background'].inputs['Color']); nt.nodes['Background'].inputs['Strength'].default_value = 0.8
sun = bpy.data.lights.new('s', 'SUN'); sun.energy = 3.5; so = bpy.data.objects.new('s', sun); sc.collection.objects.link(so); so.rotation_euler = (math.radians(45), 0, math.radians(30))
cam = bpy.data.cameras.new('c'); cam.lens = 35; co = bpy.data.objects.new('c', cam); sc.collection.objects.link(co); sc.camera = co
co.location = (x / 2, -x * 0.62, x * 0.16); co.rotation_euler = (math.radians(80), 0, 0)
sc.render.engine = 'CYCLES'; sc.cycles.samples = 48; sc.cycles.use_denoising = True; sc.cycles.denoiser = 'OPENIMAGEDENOISE'
sc.render.resolution_x = 1920; sc.render.resolution_y = 1080; sc.view_settings.view_transform = 'AgX'
sc.render.filepath = out; bpy.ops.render.render(write_still=True); print('WROTE', out)
