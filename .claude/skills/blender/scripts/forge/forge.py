"""The boss forge: sculpt, detail, bake and present a creature in Blender (headless).

The way a character artist works, scripted so every boss can be rebuilt from its design document:
  1. block-out   closed forms (lofts, tubes, ellipsoids) placed from the document's anatomy;
  2. fuse        one voxel remesh into a single organic skin, smoothed;
  3. sculpt      subdivide, then displace in numpy: scales, plates, cracks, wrinkles (the high-poly);
  4. paint       colour and roughness as point attributes on the high-poly, from the same masks;
  5. retopo      the fused skin decimated to the game mesh, unwrapped;
  6. bake        normal, ambient occlusion, colour and roughness from the high-poly to the game mesh;
  7. export      one GLB (mesh, base colour, normal, occlusion-roughness-metal);
  8. present     a lit arena shot of the game mesh, which is what the player gets.

Blender 4.0+, run with:  blender -b --factory-startup -P <boss>.py -- [--quick] [--out DIR] [--shots DIR]
"""
import math
import os
import sys

import bpy  # first: the bpy wheel provides bmesh and mathutils
import bmesh
import numpy as np
from mathutils import Matrix, Vector

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '..', '..'))
ASSETS = os.path.join(ROOT, 'Engine', 'O3DE', 'DarkArisen', 'Assets')


# ------------------------------------------------------------------ scene helpers

def reset():
    bpy.ops.wm.read_factory_settings(use_empty=True)
    sc = bpy.context.scene
    sc.unit_settings.system = 'METRIC'
    return sc


def link(ob):
    bpy.context.scene.collection.objects.link(ob)
    return ob


def activate(ob, others=()):
    bpy.context.view_layer.update()  # an object removed since the last update leaves a hole in the layer's list
    for o in bpy.context.scene.objects:
        o.select_set(False)
    for o in others:
        o.select_set(True)
    ob.select_set(True)
    bpy.context.view_layer.objects.active = ob


def new_obj(name, verts, faces):
    me = bpy.data.meshes.new(name)
    me.from_pydata([tuple(v) for v in verts], [], faces)
    me.update()
    bm = bmesh.new()
    bm.from_mesh(me)
    bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
    bm.to_mesh(me)
    bm.free()
    return link(bpy.data.objects.new(name, me))


def apply_modifier(ob, mod):
    activate(ob)
    bpy.ops.object.modifier_apply(modifier=mod.name)


def join(name, objs):
    activate(objs[0], objs[1:])
    bpy.ops.object.join()
    ob = bpy.context.view_layer.objects.active
    ob.name = name
    return ob


def duplicate(ob, name):
    cp = ob.copy()
    cp.data = ob.data.copy()
    cp.name = name
    return link(cp)


# ------------------------------------------------------------------ block-out forms

def ring(c, right, up, w, hu, hd, n=28, pu=2.3, pd=2.6):
    """A closed cross-section: half width w, height hu above and hd below the centre, squarer for larger exponents."""
    c, right, up = Vector(c), Vector(right), Vector(up)
    pts = []
    for i in range(n):
        a = 2 * math.pi * i / n
        ca, sa = math.cos(a), math.sin(a)
        p, h = (pu, hu) if sa >= 0 else (pd, hd)
        pts.append(c + right * (w * math.copysign(abs(ca) ** (2 / p), ca)) + up * (h * math.copysign(abs(sa) ** (2 / p), sa)))
    return pts


def loft(name, rings):
    n = len(rings[0])
    verts = [p for r in rings for p in r]
    faces = []
    for k in range(len(rings) - 1):
        for i in range(n):
            a, b = k * n + i, k * n + (i + 1) % n
            faces.append((a, b, b + n, a + n))
    faces.append(tuple(range(n - 1, -1, -1)))
    faces.append(tuple(range((len(rings) - 1) * n, len(rings) * n)))
    return new_obj(name, verts, faces)


def body(name, sections, n=28):
    """Loft through sections given as dicts: c (centre), w, hu, hd, optional right/up axes and exponents."""
    rings = []
    for k, s in enumerate(sections):
        c = Vector(s['c'])
        if 'right' in s:
            right, up = Vector(s['right']).normalized(), Vector(s['up']).normalized()
        else:
            a = Vector(sections[max(k - 1, 0)]['c'])
            b = Vector(sections[min(k + 1, len(sections) - 1)]['c'])
            t = (b - a).normalized()
            right = t.cross(Vector((0, 0, 1)))
            right = right.normalized() if right.length > 1e-6 else Vector((0, 1, 0))
            up = right.cross(t).normalized()
            right = -right  # +Y is the creature's left when it faces +X
            up = up if up.z >= 0 else -up
        rings.append(ring(c, right, up, s['w'], s['hu'], s['hd'], n, s.get('pu', 2.3), s.get('pd', 2.6)))
    return loft(name, rings)


def tube(name, path, radii, n=14, flat=1.0):
    """A limb along a path with a radius at each point (flat < 1 squashes it across its second axis)."""
    path = [Vector(p) for p in path]
    rings = []
    prev_u = None
    for k, p in enumerate(path):
        t = (path[min(k + 1, len(path) - 1)] - path[max(k - 1, 0)]).normalized()
        ref = prev_u if prev_u is not None else (Vector((0, 0, 1)) if abs(t.z) < 0.9 else Vector((1, 0, 0)))
        u = (ref - t * ref.dot(t)).normalized()
        v = t.cross(u)
        prev_u = u
        r = radii[k]
        rings.append([p + u * (r * math.cos(2 * math.pi * i / n)) + v * (r * flat * math.sin(2 * math.pi * i / n)) for i in range(n)])
    return loft(name, rings)


def blob(name, c, r, rot=(0, 0, 0), seg=20):
    bm = bmesh.new()
    bmesh.ops.create_uvsphere(bm, u_segments=seg, v_segments=seg // 2, radius=1.0)
    me = bpy.data.meshes.new(name)
    bm.to_mesh(me)
    bm.free()
    ob = link(bpy.data.objects.new(name, me))
    r = (r, r, r) if isinstance(r, (int, float)) else r
    m = Matrix.Translation(Vector(c)) @ Matrix.Rotation(rot[2], 4, 'Z') @ Matrix.Rotation(rot[1], 4, 'Y') @ Matrix.Rotation(rot[0], 4, 'X') @ Matrix.Diagonal((r[0], r[1], r[2], 1.0))
    me.transform(m)
    return ob


def cone(name, base, tip, r, seg=10, bend=0.0, bend_dir=(0, 0, 0)):
    """A tooth, claw or horn: from base to tip, bowed by bend along bend_dir."""
    base, tip, bd = Vector(base), Vector(tip), Vector(bend_dir)
    steps = 6
    path = [base.lerp(tip, k / steps) + bd * (bend * math.sin(math.pi * k / steps)) for k in range(steps + 1)]
    radii = [max(r * (1 - (k / steps) ** 1.4), r * 0.03) for k in range(steps + 1)]
    return tube(name, path, radii, seg)


def spline(points, per=8):
    """Catmull-Rom through the control points, per samples a span."""
    pts = [Vector(p) for p in points]
    ext = [pts[0] * 2 - pts[1]] + pts + [pts[-1] * 2 - pts[-2]]
    out = []
    for k in range(1, len(ext) - 2):
        p0, p1, p2, p3 = ext[k - 1], ext[k], ext[k + 1], ext[k + 2]
        for j in range(per):
            t = j / per
            out.append(0.5 * ((2 * p1) + (-p0 + p2) * t + (2 * p0 - 5 * p1 + 4 * p2 - p3) * t * t + (-p0 + 3 * p1 - 3 * p2 + p3) * t ** 3))
    out.append(pts[-1])
    return out


def frames(path):
    """Per point: (forward, left, up) with left kept level, and the arc-length fraction."""
    path = [Vector(p) for p in path]
    lens = [0.0]
    for k in range(1, len(path)):
        lens.append(lens[-1] + (path[k] - path[k - 1]).length)
    out = []
    for k, p in enumerate(path):
        f = (path[min(k + 1, len(path) - 1)] - path[max(k - 1, 0)]).normalized()
        left = Vector((0, 0, 1)).cross(f)
        left = left.normalized() if left.length > 1e-5 else Vector((0, 1, 0))
        up = f.cross(left).normalized()
        out.append((f, left, up, lens[k] / lens[-1]))
    return out, lens[-1]


def along_axes(q, bases, tips):
    """For points q: the fraction (0 base .. 1 tip) along the nearest of the given axes."""
    best = np.full(len(q), 1e9)
    frac = np.zeros(len(q))
    for b, t in zip(bases, tips):
        b, t = np.array(b), np.array(t)
        ax = t - b
        ln = np.dot(ax, ax)
        f = np.clip(((q - b) @ ax) / ln, 0, 1)
        dist = np.linalg.norm(q - (b + f[:, None] * ax), axis=1)
        near = dist < best
        best = np.where(near, dist, best)
        frac = np.where(near, f, frac)
    return frac


def bundle(name, path, count, spread0, spread1, r0, r1, twist, seed=1, per=6):
    """Twisted strands round a path: roots grown into a limb. Returns the strands as tubes."""
    rnd = np.random.default_rng(seed)
    path = spline(path, per) if len(path) > 2 else [Vector(path[0]).lerp(Vector(path[1]), k / per) for k in range(per + 1)]
    fr, _ = frames(path)
    out = []
    for s in range(count):
        phase = 2 * math.pi * s / count + rnd.uniform(-0.3, 0.3)
        thick = rnd.uniform(0.8, 1.2)
        pts, radii = [], []
        for k, p in enumerate(path):
            t = k / (len(path) - 1)
            f, left, up, _ = fr[k]
            a = phase + twist * 2 * math.pi * t
            sp = spread0 + (spread1 - spread0) * t
            wob = 1 + 0.25 * math.sin(t * 9 + s * 2.1)
            pts.append(p + (left * math.cos(a) + up * math.sin(a)) * sp * wob)
            radii.append((r0 + (r1 - r0) * t) * thick * (1 + 0.15 * math.sin(t * 13 + s)))
        out.append(tube(name, pts, radii, 10))
    return out


def slab(name, c, half, rot=(0, 0, 0)):
    """A stone block or plate: a box of half sizes, turned by XYZ Euler angles. Returns (object, world-to-local matrix, half)."""
    bm = bmesh.new()
    bmesh.ops.create_cube(bm, size=2.0)
    bmesh.ops.subdivide_edges(bm, edges=bm.edges, cuts=3, use_grid_fill=True)
    me = bpy.data.meshes.new(name)
    bm.to_mesh(me)
    bm.free()
    ob = link(bpy.data.objects.new(name, me))
    m = Matrix.Translation(Vector(c)) @ Matrix.Rotation(rot[2], 4, 'Z') @ Matrix.Rotation(rot[1], 4, 'Y') @ Matrix.Rotation(rot[0], 4, 'X')
    me.transform(m @ Matrix.Diagonal((half[0], half[1], half[2], 1.0)))
    return ob, np.array(m.inverted()), np.array(half)


def box_mask(p, inv, half, feather=0.04):
    """1 inside the slab given by slab()'s matrix and half sizes, falling to 0 over feather metres outside it."""
    q = p @ inv[:3, :3].T + inv[:3, 3]
    d = np.max(np.abs(q) - half, axis=1)
    return smoothstep(feather, 0.0, d)


def fuse(name, objs, voxel, smooth=6):
    """One skin from overlapping closed forms: voxel remesh, then relax."""
    ob = join(name, list(objs))
    m = ob.modifiers.new('fuse', 'REMESH')
    m.mode = 'VOXEL'
    m.voxel_size = voxel
    m.adaptivity = 0.0
    apply_modifier(ob, m)
    if smooth:
        s = ob.modifiers.new('relax', 'SMOOTH')
        s.factor = 0.6
        s.iterations = smooth
        apply_modifier(ob, s)
    return ob


def subdivide(ob, levels):
    m = ob.modifiers.new('sub', 'SUBSURF')
    m.levels = levels
    m.render_levels = levels
    apply_modifier(ob, m)
    return ob


# ------------------------------------------------------------------ numpy access and noise

def co(ob):
    a = np.empty(len(ob.data.vertices) * 3, np.float32)
    ob.data.vertices.foreach_get('co', a)
    return a.reshape(-1, 3).astype(np.float64)


def set_co(ob, a):
    ob.data.vertices.foreach_set('co', a.astype(np.float32).ravel())
    ob.data.update()


def normals(ob):
    a = np.empty(len(ob.data.vertices) * 3, np.float32)
    ob.data.vertices.foreach_get('normal', a)
    return a.reshape(-1, 3).astype(np.float64)


def push(ob, h):
    """Move every vertex along its normal by h (metres)."""
    set_co(ob, co(ob) + normals(ob) * h[:, None])


def smoothstep(e0, e1, x):
    t = np.clip((x - e0) / (e1 - e0), 0.0, 1.0)
    return t * t * (3 - 2 * t)


def _hash(i):
    """Lattice points (..., 3) int -> (..., 3) floats in [0, 1)."""
    x = i[..., 0] * 127.1 + i[..., 1] * 311.7 + i[..., 2] * 74.7
    y = i[..., 0] * 269.5 + i[..., 1] * 183.3 + i[..., 2] * 246.1
    z = i[..., 0] * 113.5 + i[..., 1] * 271.9 + i[..., 2] * 124.6
    s = np.stack([np.sin(x), np.sin(y), np.sin(z)], -1) * 43758.5453
    return s - np.floor(s)


def vnoise(p):
    i = np.floor(p)
    f = p - i
    u = f * f * (3 - 2 * f)
    out = 0.0
    for dx in (0, 1):
        for dy in (0, 1):
            for dz in (0, 1):
                h = _hash(i + np.array([dx, dy, dz]))[..., 0]
                w = (u[..., 0] if dx else 1 - u[..., 0]) * (u[..., 1] if dy else 1 - u[..., 1]) * (u[..., 2] if dz else 1 - u[..., 2])
                out = out + h * w
    return out


def fbm(p, octaves=5, lac=2.0, gain=0.5):
    """Fractal noise in [0, 1]."""
    a, s, t = 1.0, 0.0, 0.0
    for _ in range(octaves):
        s = s + a * vnoise(p)
        t += a
        p = p * lac + 17.3
        a *= gain
    return s / t


def worley(p):
    """Cell noise: distance to the nearest and second nearest feature point, and a random value per cell."""
    i = np.floor(p)
    f1 = np.full(p.shape[:-1], 9.0)
    f2 = np.full(p.shape[:-1], 9.0)
    cid = np.zeros(p.shape[:-1])
    for dx in (-1, 0, 1):
        for dy in (-1, 0, 1):
            for dz in (-1, 0, 1):
                c = i + np.array([dx, dy, dz])
                h = _hash(c)
                d = np.linalg.norm(c + h - p, axis=-1)
                closer = d < f1
                f2 = np.where(closer, f1, np.minimum(f2, d))
                cid = np.where(closer, h[..., 1], cid)
                f1 = np.where(closer, d, f1)
    return f1, f2, cid


def cells(p, crack=0.08):
    """Scales or plates: a domed top per cell, a crack between cells. Returns (dome 0..1, crack 0..1, cell id)."""
    f1, f2, cid = worley(p)
    edge = f2 - f1
    return smoothstep(0.0, 0.55, edge), 1.0 - smoothstep(0.0, crack, edge), cid


def set_attr(ob, name, rgb):
    me = ob.data
    at = me.color_attributes.get(name) or me.color_attributes.new(name, 'FLOAT_COLOR', 'POINT')
    rgba = np.concatenate([np.clip(rgb, 0, 1), np.ones((len(rgb), 1))], 1).astype(np.float32)
    at.data.foreach_set('color', rgba.ravel())


def mix(a, b, t):
    return a * (1 - t[:, None]) + np.asarray(b)[None, :] * t[:, None] if np.ndim(b) == 1 else a * (1 - t[:, None]) + b * t[:, None]


# ------------------------------------------------------------------ materials

def flat_material(name, color, rough=0.5, metal=0.0, emit=None, strength=0.0):
    m = bpy.data.materials.new(name)
    m.use_nodes = True
    b = m.node_tree.nodes['Principled BSDF']
    b.inputs['Base Color'].default_value = (*color, 1)
    b.inputs['Roughness'].default_value = rough
    b.inputs['Metallic'].default_value = metal
    if emit:
        b.inputs['Emission Color'].default_value = (*emit, 1)
        b.inputs['Emission Strength'].default_value = strength
    return m


def assign(ob, mat):
    ob.data.materials.clear()
    ob.data.materials.append(mat)
    for p in ob.data.polygons:
        p.use_smooth = True
    return ob


def _emit_material(attr):
    m = bpy.data.materials.new('forge_emit')
    m.use_nodes = True
    nt = m.node_tree
    nt.nodes.clear()
    a = nt.nodes.new('ShaderNodeAttribute')
    a.attribute_name = attr
    a.name = 'attr'
    e = nt.nodes.new('ShaderNodeEmission')
    o = nt.nodes.new('ShaderNodeOutputMaterial')
    nt.links.new(a.outputs['Color'], e.inputs['Color'])
    nt.links.new(e.outputs[0], o.inputs['Surface'])
    return m


def _occlusion_group():
    g = bpy.data.node_groups.get('glTF Material Output')
    if not g:
        g = bpy.data.node_groups.new('glTF Material Output', 'ShaderNodeTree')
        g.interface.new_socket(name='Occlusion', in_out='INPUT', socket_type='NodeSocketFloat')
        g.nodes.new('NodeGroupInput')
    return g


def game_material(name, base, normal, orm, emit=None, emit_strength=1.0, two_sided=False):
    """Principled material on the baked maps (emission optional), wired the way the glTF exporter expects."""
    m = bpy.data.materials.new(name)
    m.use_nodes = True
    nt = m.node_tree
    b = nt.nodes['Principled BSDF']
    tb = nt.nodes.new('ShaderNodeTexImage'); tb.image = base
    tn = nt.nodes.new('ShaderNodeTexImage'); tn.image = normal
    to = nt.nodes.new('ShaderNodeTexImage'); to.image = orm
    nm = nt.nodes.new('ShaderNodeNormalMap')
    sep = nt.nodes.new('ShaderNodeSeparateColor')
    occ = nt.nodes.new('ShaderNodeGroup'); occ.node_tree = _occlusion_group()
    mul = nt.nodes.new('ShaderNodeMix'); mul.data_type = 'RGBA'; mul.blend_type = 'MULTIPLY'; mul.inputs['Factor'].default_value = 0.85
    nt.links.new(to.outputs['Color'], sep.inputs['Color'])
    nt.links.new(tb.outputs['Color'], mul.inputs['A'])
    nt.links.new(sep.outputs['Red'], mul.inputs['B'])
    nt.links.new(mul.outputs['Result'], b.inputs['Base Color'])  # the look in Cycles; the exporter still finds the base map
    nt.links.new(tn.outputs['Color'], nm.inputs['Color'])
    nt.links.new(nm.outputs['Normal'], b.inputs['Normal'])
    nt.links.new(sep.outputs['Green'], b.inputs['Roughness'])
    nt.links.new(sep.outputs['Blue'], b.inputs['Metallic'])
    nt.links.new(sep.outputs['Red'], occ.inputs['Occlusion'])
    if emit is not None:
        te = nt.nodes.new('ShaderNodeTexImage'); te.image = emit
        nt.links.new(te.outputs['Color'], b.inputs['Emission Color'])
        b.inputs['Emission Strength'].default_value = emit_strength
    m.use_backface_culling = not two_sided
    m['forge_base'] = base.name
    return m


# ------------------------------------------------------------------ retopology and baking

def retopo(high_base, name, tris, angle=66.0, margin=0.004):
    """The game mesh: the fused skin decimated to a triangle budget and unwrapped."""
    low = duplicate(high_base, name)
    count = sum(len(p.vertices) - 2 for p in low.data.polygons)
    m = low.modifiers.new('dec', 'DECIMATE')
    m.ratio = min(1.0, tris / max(count, 1))
    apply_modifier(low, m)
    t = low.modifiers.new('tri', 'TRIANGULATE')
    apply_modifier(low, t)
    activate(low)
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(angle_limit=math.radians(angle), island_margin=margin)
    bpy.ops.object.mode_set(mode='OBJECT')
    for p in low.data.polygons:
        p.use_smooth = True
    return low


def bake_maps(high, low, size, extrusion, ao_samples=24, tmp=None):
    """Normal, AO, colour ('Col') and roughness ('Rough', red channel) from high to low. Returns (base, normal, orm) images."""
    sc = bpy.context.scene
    sc.render.engine = 'CYCLES'
    sc.cycles.device = 'CPU'
    sc.render.bake.margin = 12
    em = _emit_material('Col')
    high.data.materials.clear()
    high.data.materials.append(em)
    bm = bpy.data.materials.new('forge_bake_target')
    bm.use_nodes = True
    node = bm.node_tree.nodes.new('ShaderNodeTexImage')
    bm.node_tree.nodes.active = node
    low.data.materials.clear()
    low.data.materials.append(bm)

    def run(kind, label, samples, colorspace, s):
        img = bpy.data.images.new(label, s, s, alpha=False, float_buffer=False)
        img.colorspace_settings.name = colorspace
        node.image = img
        sc.cycles.samples = samples
        activate(low, [high])
        bpy.ops.object.bake(type=kind, use_selected_to_active=True, cage_extrusion=extrusion, max_ray_distance=extrusion * 2.5, margin=12)
        return img

    normal = run('NORMAL', 'normal', 1, 'Non-Color', size)
    ao = run('AO', 'ao', ao_samples, 'Non-Color', size // 2)
    base = run('EMIT', 'base', 1, 'sRGB', size)
    em.node_tree.nodes['attr'].attribute_name = 'Rough'
    rough = run('EMIT', 'rough', 1, 'Non-Color', size // 2)

    emit = None
    if high.data.color_attributes.get('Emit'):
        em.node_tree.nodes['attr'].attribute_name = 'Emit'
        emit = run('EMIT', 'emit', 1, 'sRGB', size // 2)

    def px(img):
        a = np.empty(img.size[0] * img.size[1] * 4, np.float32)
        img.pixels.foreach_get(a)
        return a.reshape(img.size[1], img.size[0], 4)

    o = px(ao)
    r = px(rough)
    orm = bpy.data.images.new('orm', size // 2, size // 2, alpha=False)
    orm.colorspace_settings.name = 'Non-Color'
    packed = np.ones_like(o)
    packed[..., 0] = np.clip(o[..., 0] * 1.15, 0, 1)
    packed[..., 1] = r[..., 0]
    packed[..., 2] = 0.0
    orm.pixels.foreach_set(packed.ravel())
    for img in (base, normal, orm) + ((emit,) if emit else ()):
        if tmp:
            img.filepath_raw = os.path.join(tmp, low.name + '_' + img.name + '.png')
            img.file_format = 'PNG'
            img.save()
        else:
            img.pack()
    return (base, normal, orm, emit) if emit else (base, normal, orm)


def bake_part(low, material, paint, size=1024, levels=2, extrusion=0.012, tmp=None, emit_strength=1.0, two_sided=False):
    """A hard part (teeth, an eye, horns) with maps of its own: the part subdivided, sculpted and painted is its high-poly.
    paint(points, normals) returns (displacement or None, colour (n, 3), roughness (n,))."""
    activate(low)
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(angle_limit=math.radians(66), island_margin=0.01)
    bpy.ops.object.mode_set(mode='OBJECT')
    high = subdivide(duplicate(low, low.name + '_high'), levels)
    painted = paint(co(high), normals(high))
    h, col, rough = painted[:3]
    if h is not None:
        push(high, h)
    set_attr(high, 'Col', col)
    set_attr(high, 'Rough', np.repeat(np.clip(rough, 0.03, 1.0)[:, None], 3, 1))
    if len(painted) > 3:
        set_attr(high, 'Emit', painted[3])
    maps = bake_maps(high, low, size, extrusion, 8, tmp)
    bpy.data.objects.remove(high)
    return assign(low, game_material(material, *maps[:3], emit=maps[3] if len(maps) > 3 else None, emit_strength=emit_strength, two_sided=two_sided))


def export_gltf(out_dir, name, objs):
    """<out_dir>/<name>/<name>.gltf with its .bin and JPEG maps beside it. Not a .glb: the repository sends *.glb to Git
    LFS, and plain files keep the boss in ordinary history like the rest of the art."""
    folder = os.path.join(out_dir, name)
    os.makedirs(folder, exist_ok=True)
    for o in objs:
        t = o.modifiers.new('tri', 'TRIANGULATE')
        apply_modifier(o, t)
    activate(objs[0], objs[1:])
    path = os.path.join(folder, name + '.gltf')
    bpy.ops.export_scene.gltf(filepath=path, export_format='GLTF_SEPARATE', use_selection=True, export_image_format='JPEG', export_jpeg_quality=90,
                              export_yup=True, export_apply=True, export_tangents=True, export_materials='EXPORT')
    return sum(os.path.getsize(os.path.join(folder, f)) for f in os.listdir(folder))


# ------------------------------------------------------------------ presentation

def stage(ground='mud', fog=0.035, hdri_strength=0.12, tint=(0.55, 0.62, 0.6), relief=0.5, columns=None):
    """An arena to judge the creature in: wet ground, mist, a dim sky."""
    sc = bpy.context.scene
    w = bpy.data.worlds.new('stage')
    sc.world = w
    w.use_nodes = True
    nt = w.node_tree
    bg = nt.nodes['Background']
    hdr = os.path.join(ASSETS, 'Art', 'Scanned', 'hdri', 'kloofendal_48d_partly_cloudy_puresky_4k.hdr')
    if os.path.exists(hdr):
        env = nt.nodes.new('ShaderNodeTexEnvironment')
        env.image = bpy.data.images.load(hdr)
        mixn = nt.nodes.new('ShaderNodeMix'); mixn.data_type = 'RGBA'; mixn.blend_type = 'MULTIPLY'; mixn.inputs['Factor'].default_value = 1.0
        mixn.inputs['B'].default_value = (*tint, 1)
        nt.links.new(env.outputs['Color'], mixn.inputs['A'])
        nt.links.new(mixn.outputs['Result'], bg.inputs['Color'])
    bg.inputs['Strength'].default_value = hdri_strength
    # the ground
    bpy.ops.mesh.primitive_grid_add(x_subdivisions=220, y_subdivisions=220, size=120)
    g = bpy.context.object
    g.name = 'stage_ground'
    p = co(g)
    p[:, 2] = ((fbm(p * 0.35, 5) - 0.5) * 0.5 + (fbm(p * 2.2, 3) - 0.5) * 0.05) * relief / 0.5
    set_co(g, p)
    gm = bpy.data.materials.new('stage_ground')
    gm.use_nodes = True
    b = gm.node_tree.nodes['Principled BSDF']
    scan = {'mud': 'brown_mud_02', 'rock': 'rock_face_03', 'ash': 'rock_face_03', 'obsidian': 'rock_face_03', 'forest': 'forrest_ground_01'}[ground]
    base = os.path.join(ASSETS, 'Art', 'Scanned', scan, scan + '_diff_2k.jpg')
    nor = os.path.join(ASSETS, 'Art', 'Scanned', scan, scan + '_nor_gl_2k.jpg')
    if os.path.exists(base):
        nt2 = gm.node_tree
        tc = nt2.nodes.new('ShaderNodeTexCoord')
        mp = nt2.nodes.new('ShaderNodeMapping'); mp.inputs['Scale'].default_value = (0.25, 0.25, 0.25)
        tb = nt2.nodes.new('ShaderNodeTexImage'); tb.image = bpy.data.images.load(base)
        nt2.links.new(tc.outputs['Object'], mp.inputs['Vector'])
        nt2.links.new(mp.outputs['Vector'], tb.inputs['Vector'])
        dark = nt2.nodes.new('ShaderNodeMix'); dark.data_type = 'RGBA'; dark.blend_type = 'MULTIPLY'; dark.inputs['Factor'].default_value = 1.0
        dark.inputs['B'].default_value = {'ash': (0.12, 0.11, 0.11, 1), 'obsidian': (0.03, 0.03, 0.035, 1)}.get(ground, (0.45, 0.45, 0.45, 1))
        nt2.links.new(tb.outputs['Color'], dark.inputs['A'])
        nt2.links.new(dark.outputs['Result'], b.inputs['Base Color'])
        if os.path.exists(nor):
            tn = nt2.nodes.new('ShaderNodeTexImage'); tn.image = bpy.data.images.load(nor); tn.image.colorspace_settings.name = 'Non-Color'
            nm = nt2.nodes.new('ShaderNodeNormalMap'); nm.inputs['Strength'].default_value = 1.5
            nt2.links.new(mp.outputs['Vector'], tn.inputs['Vector'])
            nt2.links.new(tn.outputs['Color'], nm.inputs['Color'])
            nt2.links.new(nm.outputs['Normal'], b.inputs['Normal'])
    b.inputs['Roughness'].default_value = {'mud': 0.28, 'obsidian': 0.12}.get(ground, 0.7)
    g.data.materials.append(gm)
    for poly in g.data.polygons:
        poly.use_smooth = True
    if ground == 'mud':
        # standing water in the hollows
        bpy.ops.mesh.primitive_plane_add(size=120, location=(0, 0, -0.03))
        wtr = bpy.context.object
        wtr.name = 'stage_water'
        wm = flat_material('stage_water', (0.012, 0.016, 0.012), 0.04)
        wm.node_tree.nodes['Principled BSDF'].inputs['Transmission Weight'].default_value = 0.0
        wtr.data.materials.append(wm)
    if columns:
        count, ring_r, col_r, col_h = columns
        cm = bpy.data.materials.new('stage_column')
        cm.use_nodes = True
        cb = cm.node_tree.nodes['Principled BSDF']
        cb.inputs['Roughness'].default_value = 0.8
        scan_c = os.path.join(ASSETS, 'Art', 'Scanned', 'rustic_stone_wall_02', 'rustic_stone_wall_02_diff_2k.jpg')
        if os.path.exists(scan_c):
            nt4 = cm.node_tree
            tc4 = nt4.nodes.new('ShaderNodeTexCoord')
            mp4 = nt4.nodes.new('ShaderNodeMapping'); mp4.inputs['Scale'].default_value = (0.35, 0.35, 0.35)
            tb4 = nt4.nodes.new('ShaderNodeTexImage'); tb4.image = bpy.data.images.load(scan_c); tb4.projection = 'BOX'; tb4.projection_blend = 0.3
            nt4.links.new(tc4.outputs['Object'], mp4.inputs['Vector'])
            nt4.links.new(mp4.outputs['Vector'], tb4.inputs['Vector'])
            nt4.links.new(tb4.outputs['Color'], cb.inputs['Base Color'])
        for k in range(count):
            a = 2 * math.pi * (k + 0.5) / count
            broken = col_h * (0.45 + 0.55 * ((k * 7) % 5) / 4.0)
            bpy.ops.mesh.primitive_cylinder_add(vertices=20, radius=col_r, depth=broken, location=(ring_r * math.cos(a), ring_r * math.sin(a), broken / 2 - 0.2))
            c = bpy.context.object
            c.name = 'stage_column'
            c.data.materials.append(cm)
            bpy.ops.mesh.primitive_cube_add(size=1, location=(ring_r * math.cos(a), ring_r * math.sin(a), 0.25))
            pl = bpy.context.object
            pl.name = 'stage_plinth'
            pl.scale = (col_r * 2.6, col_r * 2.6, 0.5)
            pl.rotation_euler = (0, 0, a)
            pl.data.materials.append(cm)
    if fog > 0:
        bpy.ops.mesh.primitive_cube_add(size=1, location=(0, 0, 14))
        f = bpy.context.object
        f.name = 'stage_fog'
        f.scale = (120, 120, 30)
        fm = bpy.data.materials.new('stage_fog')
        fm.use_nodes = True
        nt3 = fm.node_tree
        nt3.nodes.clear()
        vs = nt3.nodes.new('ShaderNodeVolumeScatter')
        vs.inputs['Density'].default_value = fog
        vs.inputs['Anisotropy'].default_value = 0.35
        vs.inputs['Color'].default_value = (*tint, 1)
        o = nt3.nodes.new('ShaderNodeOutputMaterial')
        nt3.links.new(vs.outputs[0], o.inputs['Volume'])
        f.data.materials.append(fm)


def light(name, kind, loc, target, energy, color=(1, 1, 1), size=6.0):
    d = bpy.data.lights.new(name, kind)
    d.energy = energy
    d.color = color
    if kind == 'AREA':
        d.size = size
    if kind == 'SPOT':
        d.spot_size = math.radians(size)
        d.spot_blend = 0.6
    if kind == 'SUN':
        d.angle = math.radians(size)
    ob = link(bpy.data.objects.new(name, d))
    ob.location = loc
    ob.rotation_euler = (Vector(target) - Vector(loc)).to_track_quat('-Z', 'Y').to_euler()
    return ob


def shoot(path, eye, target, lens=40.0, res=(1920, 1080), samples=96, fstop=None, exposure=0.0, look='AgX - High Contrast'):
    sc = bpy.context.scene
    cam = bpy.data.cameras.new('cam')
    cam.lens = lens
    cam.clip_end = 2000
    ob = link(bpy.data.objects.new('cam', cam))
    ob.location = eye
    ob.rotation_euler = (Vector(target) - Vector(eye)).to_track_quat('-Z', 'Y').to_euler()
    if fstop:
        cam.dof.use_dof = True
        cam.dof.focus_distance = (Vector(target) - Vector(eye)).length
        cam.dof.aperture_fstop = fstop
    sc.camera = ob
    sc.render.engine = 'CYCLES'
    sc.cycles.device = 'CPU'
    sc.cycles.samples = samples
    sc.cycles.volume_bounces = 1
    sc.cycles.max_bounces = 6
    denoise = True
    try:
        sc.cycles.use_denoising = True
        sc.cycles.denoiser = 'OPENIMAGEDENOISE'
    except Exception:
        denoise = False
    if not denoise:
        # a Blender built without the denoiser (the distribution's package): say so and pay for it in samples
        sc.cycles.use_denoising = False
        sc.cycles.samples = samples * 4
        print('FORGE render: NOT DENOISED (this Blender has no OpenImageDenoise); samples raised to', samples * 4)
    sc.render.resolution_x, sc.render.resolution_y = res
    sc.render.resolution_percentage = 100
    sc.view_settings.view_transform = 'AgX'
    try:
        sc.view_settings.look = look
    except Exception:
        pass
    sc.view_settings.exposure = exposure
    sc.render.image_settings.file_format = 'PNG'
    sc.render.filepath = path
    bpy.ops.render.render(write_still=True)
    bpy.data.objects.remove(ob)
    print('WROTE', path)


def args():
    a = sys.argv[sys.argv.index('--') + 1:] if '--' in sys.argv else []
    opt = {'quick': '--quick' in a, 'out': os.path.join(ASSETS, 'Art', 'Creatures'), 'shots': os.path.join(ROOT, 'Docs', 'Images', 'Creatures'), 'tmp': None}
    for k in ('out', 'shots', 'tmp'):
        if '--' + k in a:
            opt[k] = a[a.index('--' + k) + 1]
    return opt


def done():
    """Leave without the bpy wheel's interpreter teardown (it can crash after everything is written)."""
    sys.stdout.flush()
    os._exit(0)
