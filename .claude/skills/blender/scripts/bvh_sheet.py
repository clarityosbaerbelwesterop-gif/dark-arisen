# Contact sheet of a BVH clip: stick figures at evenly spaced times, labelled with the time.
# blender -b --factory-startup -P bvh_sheet.py -- clip.bvh out.png t0 t1 count [cols]
import bpy, sys, math, bmesh
from mathutils import Vector
a = sys.argv[sys.argv.index('--') + 1:]
path, out, t0, t1, count = a[0], a[1], float(a[2]), float(a[3]), int(a[4])
cols = int(a[5]) if len(a) > 5 else 6
bpy.ops.wm.read_factory_settings(use_empty=True)
sc = bpy.context.scene
bpy.ops.import_anim.bvh(filepath=path, global_scale=0.056, frame_start=0, use_fps_scale=False, update_scene_fps=True, rotate_mode='NATIVE', axis_forward='-Z', axis_up='Y')
arm = bpy.context.object
fps = 30.0
rows = math.ceil(count / cols)
mat = bpy.data.materials.new('M'); mat.diffuse_color = (0.85, 0.75, 0.6, 1)
gmat = bpy.data.materials.new('G'); gmat.diffuse_color = (0.25, 0.3, 0.35, 1)
for k in range(count):
    t = t0 + (t1 - t0) * k / max(1, count - 1)
    sc.frame_set(int(round(t * fps)))
    dg = bpy.context.evaluated_depsgraph_get()
    mw = arm.matrix_world
    hips = arm.pose.bones[0]
    root = mw @ hips.head
    cu = bpy.data.curves.new('f%d' % k, 'CURVE'); cu.dimensions = '3D'; cu.bevel_depth = 0.025
    cx, cy = (k % cols) * 2.2, -(k // cols) * 2.6
    o = Vector((cx - root.x, 0 - root.y, cy))
    for pb in arm.pose.bones:
        sp = cu.splines.new('POLY'); sp.points.add(1)
        h = mw @ pb.head + o; tl = mw @ pb.tail + o
        sp.points[0].co = (h.x, h.y, h.z, 1); sp.points[1].co = (tl.x, tl.y, tl.z, 1)
    ob = bpy.data.objects.new('f%d' % k, cu); sc.collection.objects.link(ob)
    ob.data.materials.append(mat)
    txt = bpy.data.curves.new('t%d' % k, 'FONT'); txt.body = '%.2f' % t; txt.size = 0.25
    to = bpy.data.objects.new('t%d' % k, txt); sc.collection.objects.link(to)
    to.location = (cx - 0.9, 0, cy - 0.35); to.rotation_euler = (math.radians(90), 0, 0)
    bpy.ops.mesh.primitive_plane_add(size=1.8, location=(cx, 0, cy + 0.0))
    g = bpy.context.object; g.rotation_euler = (0, 0, 0); g.data.materials.append(gmat)
    g.location.z = cy + min((mw @ pb.head).z for pb in arm.pose.bones) - root.z + 0.0 if False else cy - 0.02 + (min((mw @ pb.head).z for pb in arm.pose.bones) - 0)
cam = bpy.data.cameras.new('c'); cam.type = 'ORTHO'; cam.ortho_scale = max(cols * 2.2, rows * 2.6 * 16 / 9) + 0.5
co = bpy.data.objects.new('c', cam); sc.collection.objects.link(co); sc.camera = co
co.location = ((cols - 1) * 2.2 / 2, -20, -(rows - 1) * 2.6 / 2 + 0.9); co.rotation_euler = (math.radians(90), 0, 0)
l = bpy.data.lights.new('l', 'SUN'); l.energy = 3; lo = bpy.data.objects.new('l', l); sc.collection.objects.link(lo); lo.rotation_euler = (math.radians(60), 0, math.radians(30))
w = bpy.data.worlds.new('w'); sc.world = w; w.color = (0.35, 0.37, 0.4)
sc.render.engine = 'BLENDER_EEVEE'; sc.render.resolution_x = 1600; sc.render.resolution_y = 900
sc.eevee.taa_render_samples = 8
sc.render.filepath = out
bpy.ops.render.render(write_still=True)
