# Blender preview renderer for Dark Arisen (development tool, not project logic).
# blender -b --factory-startup -P .claude/skills/blender/scripts/preview.py -- <shot.json> <out.png>
# (or: <venv with bpy>/bin/python preview.py -- <shot.json> <out.png>); DARK_ARISEN_ROOT overrides the repository root.
# shot.json: {
#   "level": "L_RexaHarbor" | null,            materialised O3DE prefab to load (meshes, transforms, hierarchy)
#   "items": [{"gltf": "Assets/...gltf", "at": [x,y,z], "yaw": deg, "action": "name", "frame": n, "scale": s}],
#   "hide": ["substring", ...],                 entity names to skip (e.g. the level's own Jake)
#   "jake": [x,y,z], "look": [x,y,z], "pitch": deg, "arm": m, "fov": deg,   third-person rig as in the game
#   "eye": [x,y,z], "target": [x,y,z],         explicit camera instead of the rig
#   "sun": [azimuth, elevation], "sky": {...}, "engine": "EEVEE"|"CYCLES", "samples": n, "res": [w,h],
#   "sea": true, "exposure": ev, "fog": density, "dof": metres }
# All positions in the O3DE frame (metres, X forward, Y left, Z up). Blender is Z up as well.
import bpy, json, math, os, sys
from mathutils import Euler, Matrix, Vector

argv = sys.argv[sys.argv.index('--') + 1:]
shot = json.load(open(argv[0]))
out = argv[1]
ROOT = os.environ.get('DARK_ARISEN_ROOT') or os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '..'))
PROJ = os.path.join(ROOT, 'Engine', 'O3DE', 'DarkArisen')

bpy.ops.wm.read_factory_settings(use_empty=True)
scene = bpy.context.scene
world_root = bpy.data.objects.new('World', None)
scene.collection.objects.link(world_root)

# glTF (Y up) -> Blender import (x, -z, y); the project imports glTF as O3DE (-x, z, y): a half turn about Z.
GLTF_TO_O3DE = Matrix.Rotation(math.pi, 4, 'Z')
library = bpy.data.collections.new('Library')
scene.collection.children.link(library)
cache = {}

# ---------------------------------------------------------------- DarkArisenTerrain materials (mirror of the Atom shader)
MATERIAL_INDEX = {}
for root_, _, files_ in [w for base_ in ('Materials', 'World') for w in os.walk(os.path.join(PROJ, 'Assets', base_))]:
    for f_ in files_:
        if f_.endswith('.material'):
            rel_ = os.path.relpath(os.path.join(root_, f_), PROJ).replace(os.sep, '/')
            MATERIAL_INDEX[rel_.lower().replace('.material', '.azmaterial')] = os.path.join(root_, f_)

def terrain_material(path):
    d = json.load(open(path))
    if 'DarkArisenTerrain' not in d.get('materialType', ''):
        return None
    v = d['propertyValues']; base = os.path.dirname(path)
    def img(key, data):
        im = bpy.data.images.load(os.path.normpath(os.path.join(base, v[key])), check_existing=True)
        if data: im.colorspace_settings.name = 'Non-Color'
        return im
    m = bpy.data.materials.new(os.path.basename(path)); m.use_nodes = True; nt_ = m.node_tree; N = nt_.nodes; Lk = nt_.links
    N.clear(); out_ = N.new('ShaderNodeOutputMaterial'); bsdf = N.new('ShaderNodeBsdfPrincipled'); Lk.new(bsdf.outputs[0], out_.inputs[0])
    uv = N.new('ShaderNodeTexCoord'); geo = N.new('ShaderNodeNewGeometry')
    def tex(image, vec, box=False):
        t = N.new('ShaderNodeTexImage'); t.image = image
        if box: t.projection = 'BOX'; t.projection_blend = 0.25
        Lk.new(vec, t.inputs['Vector']); return t
    splat = tex(img('macro.splat', True), uv.outputs['UV']); macro = tex(img('macro.color', False), uv.outputs['UV'])
    sep = N.new('ShaderNodeSeparateColor'); Lk.new(splat.outputs['Color'], sep.inputs[0])
    weights = [sep.outputs[0], sep.outputs[1], sep.outputs[2], splat.outputs['Alpha']]
    tiles = v.get('macro.tiles', [2, 2, 2.7, 2])
    col = rough = nrm = None
    mean = None
    for k in range(4):
        sc = N.new('ShaderNodeVectorMath'); sc.operation = 'SCALE'; Lk.new(geo.outputs['Position'], sc.inputs[0]); sc.inputs['Scale'].default_value = 1.0 / tiles[k]
        c = tex(img(f'layer{k}.color', False), sc.outputs[0], box=(k == 2))
        n = tex(img(f'layer{k}.normal', True), sc.outputs[0], box=(k == 2))
        a = tex(img(f'layer{k}.arm', True), sc.outputs[0], box=(k == 2))
        # the shader's whiteout blend in world space (the detail is projected on world X/Y, not on the mesh's UVs):
        # normalize(t.xy + N.xy, t.z * N.z) with t = 2 * colour - 1
        tv = N.new('ShaderNodeVectorMath'); tv.operation = 'MULTIPLY_ADD'; Lk.new(n.outputs['Color'], tv.inputs[0])
        tv.inputs[1].default_value = (2.0, 2.0, 2.0); tv.inputs[2].default_value = (-1.0, -1.0, -1.0)
        va = N.new('ShaderNodeVectorMath'); va.operation = 'ADD'; Lk.new(tv.outputs[0], va.inputs[0]); Lk.new(geo.outputs['Normal'], va.inputs[1])
        vm = N.new('ShaderNodeVectorMath'); vm.operation = 'MULTIPLY'; Lk.new(tv.outputs[0], vm.inputs[0]); Lk.new(geo.outputs['Normal'], vm.inputs[1])
        sa_ = N.new('ShaderNodeSeparateXYZ'); Lk.new(va.outputs[0], sa_.inputs[0]); sm_ = N.new('ShaderNodeSeparateXYZ'); Lk.new(vm.outputs[0], sm_.inputs[0])
        cn = N.new('ShaderNodeCombineXYZ'); Lk.new(sa_.outputs[0], cn.inputs[0]); Lk.new(sa_.outputs[1], cn.inputs[1]); Lk.new(sm_.outputs[2], cn.inputs[2])
        class _NM: pass
        nm = _NM(); nm.outputs = {'Normal': cn.outputs[0]}
        sa = N.new('ShaderNodeSeparateColor'); Lk.new(a.outputs['Color'], sa.inputs[0])
        def weighted(sock, w):
            m_ = N.new('ShaderNodeVectorMath'); m_.operation = 'SCALE'; Lk.new(sock, m_.inputs[0]); Lk.new(w, m_.inputs['Scale']); return m_.outputs[0]
        def add(acc, x):
            if acc is None: return x
            ad = N.new('ShaderNodeVectorMath'); ad.operation = 'ADD'; Lk.new(acc, ad.inputs[0]); Lk.new(x, ad.inputs[1]); return ad.outputs[0]
        col = add(col, weighted(c.outputs['Color'], weights[k]))
        # the layer's mean brightness (the shader reads its 1x1 mip)
        im_ = c.image; px_ = list(im_.pixels[:]) if im_.size[0] * im_.size[1] <= 4500000 else []
        if px_:
            n_ = len(px_) // 4; st_ = max(1, n_ // 20000)
            mk = sum(0.2126 * px_[4 * i] + 0.7152 * px_[4 * i + 1] + 0.0722 * px_[4 * i + 2] for i in range(0, n_, st_)) / len(range(0, n_, st_))
        else:
            mk = 0.3
        mm = N.new('ShaderNodeMath'); mm.operation = 'MULTIPLY'; Lk.new(weights[k], mm.inputs[0]); mm.inputs[1].default_value = mk
        if mean is None: mean = mm.outputs[0]
        else:
            ma = N.new('ShaderNodeMath'); ma.operation = 'ADD'; Lk.new(mean, ma.inputs[0]); Lk.new(mm.outputs[0], ma.inputs[1]); mean = ma.outputs[0]
        nrm = add(nrm, weighted(nm.outputs['Normal'], weights[k]))
        rough = add(rough, weighted(sa.outputs[1], weights[k]))
    nn = N.new('ShaderNodeVectorMath'); nn.operation = 'NORMALIZE'; Lk.new(nrm, nn.inputs[0])
    # macro re-tint: lerp(detail, macro * luma / 0.22, strength / 2)
    bw = N.new('ShaderNodeRGBToBW'); Lk.new(col, bw.inputs[0])
    mx_ = N.new('ShaderNodeMath'); mx_.operation = 'MAXIMUM'; Lk.new(mean, mx_.inputs[0]); mx_.inputs[1].default_value = 0.02
    sc2 = N.new('ShaderNodeMath'); sc2.operation = 'DIVIDE'; Lk.new(bw.outputs[0], sc2.inputs[0]); Lk.new(mx_.outputs[0], sc2.inputs[1])
    tint = N.new('ShaderNodeVectorMath'); tint.operation = 'SCALE'; Lk.new(macro.outputs['Color'], tint.inputs[0]); Lk.new(sc2.outputs[0], tint.inputs['Scale'])
    mix = N.new('ShaderNodeMix'); mix.data_type = 'RGBA'; mix.inputs['Factor'].default_value = shot.get('macro_override', v.get('macro.strength', 0.6))
    Lk.new(col, mix.inputs[6]); Lk.new(tint.outputs[0], mix.inputs[7])
    Lk.new(mix.outputs[2], bsdf.inputs['Base Color']); Lk.new(nn.outputs[0], bsdf.inputs['Normal'])
    rv = N.new('ShaderNodeSeparateXYZ'); Lk.new(rough, rv.inputs[0])
    # mirror of the shader: ground layers no rougher than 0.78 floor, rock (weight 2) keeps its own
    fl = N.new('ShaderNodeMath'); fl.operation = 'MAXIMUM'; Lk.new(rv.outputs[0], fl.inputs[0]); fl.inputs[1].default_value = 0.85
    rm = N.new('ShaderNodeMix'); rm.data_type = 'FLOAT'; Lk.new(weights[2], rm.inputs['Factor']); Lk.new(fl.outputs[0], rm.inputs[2]); Lk.new(rv.outputs[0], rm.inputs[3])
    Lk.new(rm.outputs[0], bsdf.inputs['Roughness'])
    # ground layers reflect half the usual (F0 0.02 = level 0.25), rock the usual 0.04
    sl = N.new('ShaderNodeMath'); sl.operation = 'MULTIPLY_ADD'; Lk.new(weights[2], sl.inputs[0]); sl.inputs[1].default_value = 0.25; sl.inputs[2].default_value = 0.25
    Lk.new(sl.outputs[0], bsdf.inputs['Specular IOR Level'])
    return m

def load_gltf(rel):
    """Imports a glTF once into a hidden collection; returns the collection to instance."""
    if rel in cache:
        return cache[rel]
    before = set(bpy.data.objects)
    bpy.ops.import_scene.gltf(filepath=os.path.join(PROJ, rel), import_shading='NORMALS')
    new = [o for o in bpy.data.objects if o not in before]
    col = bpy.data.collections.new(os.path.basename(rel))
    library.children.link(col)
    for o in new:
        for c in o.users_collection:
            c.objects.unlink(o)
        col.objects.link(o)
        if o.parent is None:
            o.matrix_world = GLTF_TO_O3DE @ o.matrix_world
    cache[rel] = col
    return col

def instance(rel, name, matrix, parent=None):
    col = load_gltf(rel)
    inst = bpy.data.objects.new(name, None)
    inst.instance_type = 'COLLECTION'
    inst.instance_collection = col
    scene.collection.objects.link(inst)
    inst.parent = parent or world_root
    inst.matrix_parent_inverse = Matrix.Identity(4)
    inst.matrix_basis = matrix
    return inst

def trs(t, r=(0, 0, 0), s=1.0):
    return Matrix.Translation(Vector(t)) @ Euler([math.radians(a) for a in r], 'XYZ').to_matrix().to_4x4() @ Matrix.Scale(s, 4)

# ---------------------------------------------------------------- level
gltf_index = {}
for root, _, files in os.walk(os.path.join(PROJ, 'Assets')):
    for f in files:
        if f.lower().endswith('.gltf'):
            full = os.path.join(root, f)
            gltf_index[os.path.relpath(full, PROJ)[:-5].lower()] = os.path.relpath(full, PROJ)

sun_rot = None
sea = shot.get('sea', False)
if shot.get('level'):
    lvl = shot['level']
    prefab = json.load(open(os.path.join(PROJ, 'Levels', lvl, lvl + '.prefab')))
    nodes, parents = {}, {}
    hide = shot.get('hide', [])
    for eid, e in prefab['Entities'].items():
        if e.get('IsRuntimeActive') is False:
            continue
        t, r, s, parent, mesh, override = [0, 0, 0], [0, 0, 0], 1.0, None, None, None
        for c in e['Components'].values():
            ty = c.get('$type', '')
            if 'TransformComponent' in ty:
                d = c.get('Transform Data', {})
                t, r, s = d.get('Translate', t), d.get('Rotate', r), d.get('UniformScale', 1.0)
                p = c.get('Parent Entity')
                parent = None if p in (None, 'ContainerEntity') else p
            if ty.endswith('EditorMeshComponent'):
                hint = c['Controller']['Configuration']['ModelAsset']['assetHint'].replace('.azmodel', '')
                if 'sm_oceantile' in hint:
                    sea = True
                    continue
                mesh = gltf_index.get(hint)
            if ty.endswith('EditorMaterialComponent'):
                for ov in c.get('Controller', {}).get('Configuration', {}).get('materials', []):
                    mhint = ov.get('Value', {}).get('MaterialAsset', {}).get('assetHint', '')
                    if mhint in MATERIAL_INDEX: override = MATERIAL_INDEX[mhint]
        if e['Name'] == 'Sun':
            sun_rot = r
        if any(h in e['Name'] for h in hide):
            continue
        if mesh and override:
            tm = terrain_material(override)
            if tm:
                for ob in load_gltf(mesh).all_objects:
                    if ob.type == 'MESH':
                        ob.data.materials.clear(); ob.data.materials.append(tm)
        if mesh:
            nodes[eid] = instance(mesh, e['Name'], trs(t, r, s))
        else:
            o = bpy.data.objects.new(e['Name'], None)
            scene.collection.objects.link(o)
            o.matrix_basis = trs(t, r, s)
            nodes[eid] = o
        parents[eid] = parent
    for eid, p in parents.items():
        if p in nodes:
            nodes[eid].parent = nodes[p]
        else:
            nodes[eid].parent = world_root

# ---------------------------------------------------------------- open world (Assets/World from DarkArisenO3DE world-build)
if shot.get('world'):
    import random
    wc = shot['world']; WD = os.path.join(PROJ, 'Assets', 'World')
    idx = json.load(open(os.path.join(WD, 'WorldTiles.json')))
    eye_ = Vector(shot['eye']); tgt_ = Vector(shot['target'])
    fwd_ = Vector((tgt_.x - eye_.x, tgt_.y - eye_.y, 0)).normalized()
    half_ = math.radians(shot.get('hfov', 75) * 0.5 + 12)
    def visible(x, y, slack):
        d = Vector((x - eye_.x, y - eye_.y, 0)); L = d.length
        if L < slack: return True
        return d.normalized().dot(fwd_) > math.cos(half_) - slack / max(L, 1.0)
    mats_ = {}
    def tmat(path):
        if path not in mats_: mats_[path] = terrain_material(path)
        return mats_[path]
    def dress(mesh, mpath):
        tm = tmat(mpath)
        print('DRESS', mesh, mpath, tm.name if tm else None, len(tm.node_tree.nodes) if tm else 0, [im.filepath[-40:] + ':' + str(im.has_data) + str(tuple(im.size)) for im in bpy.data.images][:3])
        for ob in load_gltf(mesh).all_objects:
            if ob.type == 'MESH': ob.data.materials.clear(); ob.data.materials.append(tm)
    coarse = 'Assets/World/SM_World_Coarse.gltf'
    dress(coarse, os.path.join(WD, 'Materials', 'M_World_' + wc.get('coarse_biome', 'tropical') + '.material'))
    instance(coarse, 'WorldCoarse', Matrix.Identity(4))
    src = lambda h: gltf_index.get(h.replace('.azmodel', ''))
    ts = idx['tileSize']; step = idx['heightStep']; ns = idx['heightSamples']
    grass_models = {g['card']: src(g['model']['hint']) for g in idx['grass']}
    rng = random.Random(7)
    counts = [0, 0, 0]
    for t in idx['tiles']:
        x0, y0 = t['column'] * ts, t['row'] * ts
        cx, cy = x0 + ts / 2, y0 + ts / 2
        d = math.hypot(cx - eye_.x, cy - eye_.y)
        if d > wc.get('tiles', 4000) or not visible(cx, cy, ts): continue
        mesh = src(t['model']['hint'])
        dress(mesh, MATERIAL_INDEX[t['material']['hint']])
        instance(mesh, 'Tile_%d' % t['tile'], Matrix.Identity(4)); counts[0] += 1
        for (si, x, y, z, yaw) in t.get('structures', []):
            x, y, z = x / 100.0, y / 100.0, z / 100.0
            if not visible(x, y, 40): continue
            st_ = idx['structures'][si]; rel_ = src(st_['model']['hint'])
            if st_.get('material') and 'lava' in st_['material']['hint'] and rel_ not in cache:
                lm = bpy.data.materials.new('Lava'); lm.use_nodes = True; ln = lm.node_tree.nodes; ll = lm.node_tree.links
                em = ln.new('ShaderNodeEmission'); em.inputs['Strength'].default_value = 18.0
                vn = ln.new('ShaderNodeTexVoronoi'); vn.inputs['Scale'].default_value = 0.08; cr = ln.new('ShaderNodeValToRGB')
                cr.color_ramp.elements[0].color = (0.02, 0.01, 0.01, 1); cr.color_ramp.elements[1].color = (1.0, 0.32, 0.04, 1); cr.color_ramp.elements[0].position = 0.35
                tc = ln.new('ShaderNodeTexCoord'); ll.new(tc.outputs['Object'], vn.inputs['Vector']); ll.new(vn.outputs['Distance'], cr.inputs[0])
                ll.new(cr.outputs[0], em.inputs['Color']); ll.new(em.outputs[0], ln['Material Output'].inputs['Surface']); ln.remove(ln['Principled BSDF'])
                for ob in load_gltf(rel_).all_objects:
                    if ob.type == 'MESH': ob.data.materials.clear(); ob.data.materials.append(lm)
            instance(rel_, 'Building', trs((x, y, z), (0, 0, yaw)))
        if d > wc.get('plants', 1200) + ts: continue
        f = json.load(open(os.path.join(WD, 'Tiles', 'F_World_%d_%d.json' % (t['column'], t['row']))))
        for (i, x, y, z, yaw, sc) in f['plants']:
            x, y, z = x / 100.0, y / 100.0, z / 100.0
            pl = idx['plants'][i]
            reach = wc.get('plants', 1200) if pl['tree'] else wc.get('small', 350)
            if math.hypot(x - eye_.x, y - eye_.y) > reach or not visible(x, y, 15): continue
            for part in pl['parts']:
                instance(src(part['model']['hint']), 'Plant', trs((x, y, z), (0, 0, yaw), sc / 100.0))
            counts[1] += 1
        gr = wc.get('grass', 0)
        if gr and d < gr + ts:
            H = f['heights']; biome = idx['biomes'][t['biome']]
            def hgt(x, y):
                fx = min(max((x - x0) / step, 0), ns - 1.001); fy = min(max((y - y0) / step, 0), ns - 1.001)
                i_, j_ = int(fx), int(fy); u, v = fx - i_, fy - j_
                a = H[j_ * ns + i_] * (1 - u) + H[j_ * ns + i_ + 1] * u; b = H[(j_ + 1) * ns + i_] * (1 - u) + H[(j_ + 1) * ns + i_ + 1] * u
                return (a * (1 - v) + b * v) / 100.0
            for gy in range(int(y0), int(y0 + ts), 2):
                for gx in range(int(x0), int(x0 + ts), 2):
                    if math.hypot(gx - eye_.x, gy - eye_.y) > gr or not visible(gx, gy, 6): continue
                    if rng.random() > biome['density']: continue
                    if any((gx - px_) ** 2 + (gy - py_) ** 2 < pr_ ** 2 for px_, py_, pr_ in idx.get('paved', [])): continue
                    x, y = gx + rng.random() * 2, gy + rng.random() * 2
                    z = hgt(x, y)
                    if z < 1.6: continue
                    nx_ = (hgt(x - 1, y) - hgt(x + 1, y)) / 2.0; ny_ = (hgt(x, y - 1) - hgt(x, y + 1)) / 2.0
                    if 1.0 / math.sqrt(1 + nx_ * nx_ + ny_ * ny_) < 0.9: continue
                    card = biome['grass'][rng.randrange(len(biome['grass']))]
                    m_ = trs((x, y, z), (math.degrees(math.atan(-ny_)), math.degrees(math.atan(nx_)), rng.random() * 360), 0.8 + rng.random() * 0.5)
                    instance(grass_models[card], 'Grass', m_); counts[2] += 1
    print('WORLD tiles %d plants %d grass %d' % tuple(counts))

# the library only feeds collection instances: keep it out of the rendered layer
for lc in bpy.context.view_layer.layer_collection.children:
    if lc.collection == library:
        lc.exclude = True

# ---------------------------------------------------------------- extra items (actors with a pose, props)
for k, it in enumerate(shot.get('items', [])):
    before = set(bpy.data.objects)
    bpy.ops.import_scene.gltf(filepath=os.path.join(PROJ, it['gltf']), import_shading='NORMALS')
    new = [o for o in bpy.data.objects if o not in before]
    holder = bpy.data.objects.new('Item%d' % k, None)
    scene.collection.objects.link(holder)
    holder.matrix_basis = trs(it.get('at', [0, 0, 0]), (0, 0, it.get('yaw', 0)), it.get('scale', 1.0)) @ GLTF_TO_O3DE
    for o in new:
        if o.parent is None:
            o.parent = holder
    if it.get('motion'):
        # apply an action from a motion glTF onto this item's armature
        mb = set(bpy.data.actions)
        bpy.ops.import_scene.gltf(filepath=os.path.join(PROJ, it['motion']))
        acts = [a for a in bpy.data.actions if a not in mb]
        junk = [o for o in bpy.data.objects if o not in before and o not in new]
        for arm in [o for o in new if o.type == 'ARMATURE']:
            arm.animation_data_create()
            arm.animation_data.action = acts[0]
        for o in junk:
            bpy.data.objects.remove(o, do_unlink=True)
    for arm in [o for o in new if o.type == 'ARMATURE' and o.animation_data and o.animation_data.action]:
        if 'action' in it:
            arm.animation_data.action = bpy.data.actions[it['action']]
    if 'frame' in it:
        scene.frame_set(it['frame'])
    if it.get('shapes'):
        for o in new:
            if o.type == 'MESH' and o.data.shape_keys:
                for kb in o.data.shape_keys.key_blocks:
                    if kb.name in it['shapes']:
                        kb.value = it['shapes'][kb.name]

# ---------------------------------------------------------------- environment
eng = shot.get('engine', 'EEVEE')
scene.render.engine = 'CYCLES' if eng == 'CYCLES' else 'BLENDER_EEVEE'
w, h = shot.get('res', [1920, 1080])
scene.render.resolution_x, scene.render.resolution_y = w, h
scene.render.film_transparent = False
scene.view_settings.view_transform = 'AgX' if 'AgX' in [v.identifier for v in bpy.types.ColorManagedViewSettings.bl_rna.properties['view_transform'].enum_items] else 'Filmic'
try:
    scene.view_settings.look = shot.get('look_cm', 'None')
except TypeError:
    scene.view_settings.look = 'None'
scene.view_settings.exposure = shot.get('exposure', 0.0)

if 'sun' in shot:
    az, el = shot['sun']
else:
    az, el = 210.0, 35.0
    if sun_rot:
        # DefaultLevel Sun: forward +Y rotated by its Euler angles (ZYX as in scene.js) = direction light travels
        d = Euler([math.radians(a) for a in sun_rot], 'XYZ').to_matrix() @ Vector((0, 1, 0))
        to_sun = -d
        el = math.degrees(math.asin(max(-1, min(1, to_sun.z))))
        az = math.degrees(math.atan2(to_sun.y, to_sun.x))
to_sun = Vector((math.cos(math.radians(az)) * math.cos(math.radians(el)), math.sin(math.radians(az)) * math.cos(math.radians(el)), math.sin(math.radians(el))))

wd = bpy.data.worlds.new('Sky')
scene.world = wd
wd.use_nodes = True
nt = wd.node_tree
nt.nodes.clear()
sky = nt.nodes.new('ShaderNodeTexSky')
sky.sky_type = 'NISHITA'
sky.sun_elevation = math.radians(max(el, 0.5))
sky.sun_rotation = math.radians(90 - az) if False else math.atan2(to_sun.x, to_sun.y)
sky.altitude = 10
skyo = shot.get('sky', {})
sky.air_density = skyo.get('air', 1.0)
sky.dust_density = skyo.get('dust', 1.5)
sky.ozone_density = skyo.get('ozone', 1.0)
sky.sun_disc = True
sky.sun_intensity = skyo.get('sun_disc', 0.4)
bg = nt.nodes.new('ShaderNodeBackground')
bg.inputs['Strength'].default_value = skyo.get('strength', 0.35)
o_ = nt.nodes.new('ShaderNodeOutputWorld')
nt.links.new(sky.outputs['Color'], bg.inputs['Color'])
nt.links.new(bg.outputs['Background'], o_.inputs['Surface'])
if shot.get('hdri'):
    # CC0 HDRI sky (Assets/Art/Scanned/hdri) instead of the analytic sky; the sun stays a separate light
    env = nt.nodes.new('ShaderNodeTexEnvironment')
    env.image = bpy.data.images.load(os.path.join(PROJ, 'Assets/Art/Scanned/hdri', shot['hdri']))
    tc = nt.nodes.new('ShaderNodeTexCoord'); mp = nt.nodes.new('ShaderNodeMapping')
    mp.inputs['Rotation'].default_value[2] = math.radians(shot.get('hdri_rot', 0.0))
    nt.links.new(tc.outputs['Generated'], mp.inputs['Vector']); nt.links.new(mp.outputs['Vector'], env.inputs['Vector'])
    nt.links.new(env.outputs['Color'], bg.inputs['Color'])
    bg.inputs['Strength'].default_value = shot.get('hdri_strength', 1.0)

sun = bpy.data.lights.new('Sun', 'SUN')
sun.energy = shot.get('sun_energy', 4.0)
sun.angle = math.radians(1.2)
sun.color = shot.get('sun_color', [1.0, 0.95, 0.88])
sun_obj = bpy.data.objects.new('Sun', sun)
scene.collection.objects.link(sun_obj)
sun_obj.rotation_euler = (-to_sun).to_track_quat('-Z', 'Y').to_euler()

if sea:
    # the open world: a 6 km sea with waves around the camera, a flat one to the horizon beyond
    sea_at = (shot['eye'][0], shot['eye'][1], 0) if shot.get('world') else (0, 0, 0)
    bpy.ops.mesh.primitive_plane_add(size=6000, location=sea_at)
    plane = bpy.context.object
    plane.name = 'Sea'
    bpy.ops.object.modifier_add(type='OCEAN')
    m = plane.modifiers[-1]
    m.geometry_mode = 'DISPLACE'
    m.resolution = 12
    m.size = 1.0
    m.spatial_size = 60
    m.wave_scale = shot.get('wave', 0.6)
    m.wind_velocity = 14
    m.repeat_x = m.repeat_y = 100
    bpy.ops.object.modifier_add(type='SUBSURF')
    plane.modifiers[-1].levels = 6
    plane.modifiers[-1].render_levels = 6
    plane.modifiers[-1].subdivision_type = 'SIMPLE'
    mat = bpy.data.materials.new('Water')
    mat.use_nodes = True
    b = mat.node_tree.nodes['Principled BSDF']
    b.inputs['Base Color'].default_value = shot.get('water', [0.01, 0.05, 0.06, 1])
    b.inputs['Roughness'].default_value = 0.03
    b.inputs['IOR'].default_value = 1.33
    for name in ('Transmission Weight', 'Transmission'):
        if name in b.inputs:
            b.inputs[name].default_value = 0.0
    plane.data.materials.append(mat)
    if shot.get('world'):
        bpy.ops.mesh.primitive_plane_add(size=60000, location=(sea_at[0], sea_at[1], -0.15))
        far = bpy.context.object; far.name = 'SeaFar'; far.data.materials.append(mat)

if shot.get('fog') and shot.get('engine', 'EEVEE') != 'CYCLES':
    vol = nt.nodes.new('ShaderNodeVolumePrincipled')
    vol.inputs['Density'].default_value = shot['fog']
    vol.inputs['Color'].default_value = shot.get('fog_color', [0.75, 0.8, 0.85, 1])
    nt.links.new(vol.outputs['Volume'], o_.inputs['Volume'])
elif shot.get('fog'):
    # Cycles: an unbounded world volume absorbs the sky; haze lives in a bounded box around the scene instead
    bpy.ops.mesh.primitive_cube_add(size=2.0 * shot.get('fog_extent', 1500.0), location=(shot['eye'][0], shot['eye'][1], 0) if shot.get('world') else (0, 0, 0))
    box = bpy.context.object; box.name = 'Haze'
    fm = bpy.data.materials.new('Haze'); fm.use_nodes = True; fn = fm.node_tree
    fn.nodes.clear(); fv = fn.nodes.new('ShaderNodeVolumePrincipled'); fo = fn.nodes.new('ShaderNodeOutputMaterial')
    fv.inputs['Density'].default_value = shot['fog']; fv.inputs['Color'].default_value = shot.get('fog_color', [0.75, 0.8, 0.85, 1])
    fn.links.new(fv.outputs['Volume'], fo.inputs['Volume']); box.data.materials.append(fm)

# ---------------------------------------------------------------- camera
cam = bpy.data.cameras.new('Cam')
cam.lens_unit = 'FOV'
cam.angle = math.radians(shot.get('fov', 58.7) * w / h if False else shot.get('hfov', 90.0))
cam.clip_start = 0.05
cam.clip_end = 20000
cam_obj = bpy.data.objects.new('Cam', cam)
scene.collection.objects.link(cam_obj)
scene.camera = cam_obj
if 'eye' in shot:
    eye, target = Vector(shot['eye']), Vector(shot['target'])
else:
    jake, look = Vector(shot['jake']), Vector(shot['look'])
    flat = Vector((look.x - jake.x, look.y - jake.y, 0)).normalized()
    p = math.radians(shot.get('pitch', -6))
    fwd = Vector((flat.x * math.cos(p), flat.y * math.cos(p), math.sin(p)))
    right = Vector((flat.y, -flat.x, 0))
    pivot = jake + Vector((0, 0, 0.96))
    eye = pivot - fwd * shot.get('arm', 3.6) + right * 0.45 + Vector((0, 0, 0.7))
    target = eye + fwd
cam_obj.location = eye
cam_obj.rotation_euler = (target - eye).to_track_quat('-Z', 'Y').to_euler()
if shot.get('dof'):
    cam.dof.use_dof = True
    cam.dof.focus_distance = shot['dof']
    cam.dof.aperture_fstop = shot.get('fstop', 2.8)

if scene.render.engine == 'CYCLES':
    scene.cycles.samples = shot.get('samples', 96)
    scene.cycles.use_adaptive_sampling = True
    scene.cycles.max_bounces = 6
    scene.cycles.device = 'CPU'
    scene.cycles.use_denoising = bool(shot.get('denoise', True))
    try:
        scene.cycles.denoiser = 'OPENIMAGEDENOISE'
    except TypeError:
        scene.cycles.use_denoising = False
    scene.render.threads_mode = 'AUTO'
else:
    ee = scene.eevee
    ee.taa_render_samples = shot.get('samples', 64)
    ee.use_gtao = True
    ee.gtao_distance = 1.2
    ee.use_ssr = True
    ee.use_ssr_refraction = True
    ee.use_bloom = shot.get('bloom', False)
    ee.use_soft_shadows = True
    ee.shadow_cascade_size = '4096'
    ee.shadow_cube_size = '1024'
    ee.use_volumetric_shadows = True
    ee.volumetric_end = shot.get('vol_end', 400)
    sun.shadow_cascade_max_distance = shot.get('shadow_dist', 120)
    sun.shadow_cascade_count = 4

scene.render.filepath = out
bpy.ops.render.render(write_still=True)
print('WROTE', out)
