"""The Jungle Warden (docs/design/bosses/jungle_warden.md, section 2).

Four and a half metres, vaguely humanoid, built from twisted vines, ancient stone slabs and weathered armour fragments;
his skeleton is ironwood roots grown over and through stone artefacts. A carved wooden mask with no eyes, only two black
hollows. Moss and small flowers on his back and shoulders. The left arm much larger than the right. A hollow chest where a
green light pulses slowly. Vines run from his feet into the temple floor. Faces +X, +Y is his left, z = 0 is the floor.
"""
import math
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import bpy  # first: the bpy wheel provides mathutils
import numpy as np
from mathutils import Vector

import forge as F

OPT = F.args()
QUICK = OPT['quick']
rng = np.random.default_rng(0x57A2)
Z = Vector((0, 0, 1))

F.reset()
parts, SLABS = [], []


def stone(name, c, half, rot=(0, 0, 0)):
    ob, inv, hf = F.slab(name, c, half, rot)
    SLABS.append((inv, hf))
    parts.append(ob)


# legs of root, a slight stride; the feet's roots run out into the floor
LEG_L = [(0.0, 0.34, 2.2), (0.3, 0.46, 1.25), (0.08, 0.48, 0.2)]
LEG_R = [(0.0, -0.34, 2.2), (0.12, -0.46, 1.2), (-0.14, -0.48, 0.2)]
parts += F.bundle('leg', LEG_L, 5, 0.13, 0.1, 0.11, 0.085, 0.7, 11)
parts += F.bundle('leg', LEG_R, 5, 0.12, 0.1, 0.11, 0.085, -0.6, 12)
for ankle in (LEG_L[-1], LEG_R[-1]):
    for k in range(8):
        a = 2 * math.pi * k / 8 + rng.uniform(-0.25, 0.25)
        reach = rng.uniform(0.6, 1.15)
        ank = Vector(ankle)
        pts = [ank, ank + Vector((0.35 * reach * math.cos(a), 0.35 * reach * math.sin(a), -0.02)),
               ank + Vector((0.75 * reach * math.cos(a + 0.2), 0.75 * reach * math.sin(a + 0.2), -0.12)),
               ank + Vector((reach * math.cos(a + 0.3), reach * math.sin(a + 0.3), -0.3))]
        parts.append(F.tube('root', F.spline(pts, 4), [0.085 - 0.005 * i for i in range(13)], 10))
# the pelvis: temple blocks the roots have grown round
stone('pelvis', (-0.05, 0.12, 2.2), (0.3, 0.33, 0.2), (0.06, 0.1, 0.25))
stone('pelvis', (0.03, -0.27, 2.26), (0.22, 0.22, 0.16), (0.2, -0.12, -0.4))
parts += F.bundle('girdle', [(0.0, -0.58, 2.3), (0.36, 0.0, 2.18), (0.0, 0.58, 2.3)], 3, 0.04, 0.04, 0.06, 0.06, 1.0, 13)
# the trunk: a spine of roots, ribs round a hollow chest
parts += F.bundle('spine', [(-0.27, 0.0, 2.2), (-0.34, 0.0, 2.95), (-0.14, 0.0, 3.6), (0.2, 0.0, 3.98)], 5, 0.11, 0.08, 0.09, 0.07, 0.8, 30)
HEART = Vector((0.04, 0.0, 3.0))
for rib in range(6):
    z = 2.52 + 0.2 * rib
    lean = 0.22 * ((z - 2.25) / 1.6) ** 2
    depth, wide = 0.5 - 0.03 * rib, 0.56 + 0.045 * rib
    for s in (1, -1):
        arc = []
        for k in range(8):
            a = math.pi - (math.pi - 0.5) * k / 7
            arc.append((-0.07 + lean + depth * math.cos(a), s * wide * math.sin(a), z - 0.2 * (k / 7) ** 2))
        parts += F.bundle('rib', arc, 2, 0.03, 0.02, 0.06, 0.04, 0.5, 40 + rib * 2 + (s > 0), 3)
# the back: stone grown into him
stone('back', (-0.5, 0.18, 3.35), (0.1, 0.32, 0.36), (0.18, -0.32, 0.12))
stone('back', (-0.47, -0.24, 2.95), (0.09, 0.24, 0.28), (-0.25, -0.22, -0.1))
# shoulders and arms; the left grown far heavier over the centuries, an artefact caught in its forearm
SH_L, SH_R = Vector((0.12, 0.8, 3.64)), Vector((0.1, -0.64, 3.52))
stone('shoulder', SH_L + Vector((-0.05, 0.02, 0.2)), (0.32, 0.26, 0.1), (-0.45, 0.15, 0.3))
parts += F.bundle('yoke', [(0.0, 0.0, 3.62), SH_L], 4, 0.08, 0.12, 0.08, 0.1, 0.4, 31)
parts += F.bundle('yoke', [(0.0, 0.0, 3.56), SH_R], 3, 0.06, 0.07, 0.07, 0.065, -0.4, 32)
ARM_L = [SH_L, (0.4, 1.1, 2.62), (0.62, 1.04, 1.45), (0.68, 1.0, 1.02)]
parts += F.bundle('arm', ARM_L, 7, 0.19, 0.14, 0.12, 0.1, 0.9, 33)
stone('artefact', (0.52, 1.08, 2.02), (0.15, 0.21, 0.25), (0.4, -0.35, 0.6))
for k in range(5):
    a = -0.9 + 0.45 * k
    w = Vector(ARM_L[-1])
    kn = w + Vector((0.16 * math.cos(a), 0.16 * math.sin(a), -0.08))
    tip = w + Vector((0.42 * math.cos(a) + 0.06, 0.42 * math.sin(a), -0.6 + (0.25 if k == 0 else 0.0)))
    parts.append(F.tube('finger', F.spline([w, kn, kn.lerp(tip, 0.5) + Vector((0.07, 0, 0.06)), tip], 4), [0.11 - 0.006 * i for i in range(13)], 10))
ARM_R = [SH_R, (0.22, -0.84, 2.72), (0.48, -0.76, 2.08), (0.56, -0.72, 1.86)]
parts += F.bundle('arm', ARM_R, 3, 0.065, 0.05, 0.07, 0.055, -0.8, 34)
for k in range(4):
    a = -0.6 + 0.4 * k
    w = Vector(ARM_R[-1])
    parts.append(F.tube('finger', [w, w + Vector((0.08 * math.cos(a), 0.08 * math.sin(a), -0.12)), w + Vector((0.12 * math.cos(a), 0.14 * math.sin(a), -0.3))], [0.045, 0.035, 0.018], 8))
# the head: a knot of root behind the mask, roots swept back like a crown
FACE = Vector((0.58, 0.0, 4.16))
parts.append(F.blob('knot', (0.3, 0.0, 4.14), (0.3, 0.29, 0.36)))
for k in range(9):
    y = -0.26 + 0.065 * k + rng.uniform(-0.02, 0.02)
    ln = rng.uniform(0.7, 1.15)
    pts = [(0.4, y * 0.7, 4.36), (0.12, y * 1.2, 4.52 - 0.3 * abs(y)), (-0.2 * ln, y * 1.45, 4.32 - 0.25 * ln), (-0.32 * ln, y * 1.5, 3.98 - 0.3 * ln)]
    parts.append(F.tube('crown', F.spline(pts, 4), [0.05 - 0.003 * i for i in range(13)], 8))
# moss clods on the back and shoulders
BEDS = [(-0.58, 0.0, 3.74), (-0.6, 0.36, 3.66), (-0.58, -0.36, 3.6), (0.05, 0.86, 3.96), (0.12, -0.64, 3.76), (-0.34, 0.1, 3.92), (-0.5, 0.0, 3.3)]
for b in BEDS:
    parts.append(F.blob('clod', b, (rng.uniform(0.2, 0.3), rng.uniform(0.2, 0.3), rng.uniform(0.07, 0.12)), seg=14))

skin = F.fuse('warden_skin', parts, 0.035 if QUICK else 0.017, smooth=3)
low = F.retopo(skin, 'SM_Boss_JungleWarden', 30000 if QUICK else 85000)
high = F.subdivide(skin, 1 if QUICK else 2)
print('FORGE high verts', len(high.data.vertices), 'low tris', len(low.data.polygons))

# ------------------------------------------------------------------ sculpt and paint: bark, stone, moss

p = F.co(high)
n = F.normals(high)
is_stone = np.zeros(len(p))
for inv, hf in SLABS:
    is_stone = np.maximum(is_stone, F.box_mask(p, inv, hf, 0.03))
clod = np.zeros(len(p))
for b in BEDS:
    clod = np.maximum(clod, F.smoothstep(0.34, 0.2, np.linalg.norm((p - np.array(b)) / np.array([1.0, 1.0, 0.45]), axis=1)))
clod *= 1 - is_stone * 0.5
# bark: long furrows down the grain, plates that crack across, knots
furrow = np.abs(F.fbm(p * np.array([22.0, 22.0, 3.5]), 4) - 0.5) * 2
plate_d, plate_c, plate_id = F.cells(p * np.array([9.0, 9.0, 3.0]), 0.2)
knot = F.smoothstep(0.75, 0.9, F.fbm(p * 3.5 + 7.0, 3))
h_bark = -(1 - furrow) ** 2 * 0.014 + plate_d * 0.006 - plate_c * 0.008 + knot * 0.015 + (F.fbm(p * 60, 2) - 0.5) * 0.003
# stone: worn faces, chipped arrises, old carving lines, pits
chip = F.smoothstep(0.62, 0.8, F.fbm(p * 6.0 + 3.0, 4))
carve = F.smoothstep(0.035, 0.0, np.abs(((p[:, 2] * 5.5 + p[:, 1] * 1.2) % 1.0) - 0.5)) * F.smoothstep(0.4, 0.6, F.fbm(p * 1.5 + 20, 2))
h_stone = -chip * 0.03 - carve * 0.012 + (F.fbm(p * 30, 3) - 0.5) * 0.006 - F.smoothstep(0.7, 0.85, F.fbm(p * 45, 2)) * 0.006
h_clod = (F.fbm(p * 14, 4) - 0.5) * 0.05 + (F.fbm(p * 50, 2) - 0.5) * 0.012
h = h_bark * (1 - is_stone) + h_stone * is_stone
h = h * (1 - clod) + h_clod * clod
F.push(high, h)

bark = np.tile(np.array([0.075, 0.052, 0.035]), (len(p), 1)) * (0.6 + 0.9 * F.fbm(p * 4, 4))[:, None]
bark = F.mix(bark, np.array([0.2, 0.15, 0.1]), furrow ** 2 * 0.55)                                   # weathered ridges
bark *= (1 - 0.6 * (1 - furrow) ** 3)[:, None]
grey = np.tile(np.array([0.3, 0.31, 0.27]), (len(p), 1)) * (0.7 + 0.6 * F.fbm(p * 3, 4))[:, None]
grey = F.mix(grey, np.array([0.1, 0.11, 0.09]), np.clip(chip * 0.7 + carve * 0.8, 0, 1))
grey = F.mix(grey, np.array([0.52, 0.54, 0.4]), F.smoothstep(0.62, 0.7, F.fbm(p * 11 + 5, 3)) * 0.7)   # lichen
col = F.mix(bark, grey, is_stone)
up = F.smoothstep(0.2, 0.8, n[:, 2])
high_up = F.smoothstep(2.6, 3.6, p[:, 2])
moss = np.clip(clod + up * high_up * F.smoothstep(0.4, 0.6, F.fbm(p * 5 + 9, 4)) + up * F.smoothstep(0.6, 0.75, F.fbm(p * 2.5 + 1, 3)) * 0.7, 0, 1)
col = F.mix(col, np.array([0.07, 0.13, 0.03]) * (0.6 + 0.9 * F.fbm(p * 35, 2))[:, None], moss)
damp = F.smoothstep(0.6, 0.0, p[:, 2])
col *= (1 - 0.4 * damp)[:, None]
# the heart's light on everything that faces it inside the chest
to_heart = np.array(HEART) - p
dist = np.linalg.norm(to_heart, axis=1)
facing = np.clip(np.sum(n * to_heart, axis=1) / np.maximum(dist, 1e-6), 0, 1)
bathed = F.smoothstep(0.85, 0.2, dist) * (0.25 + 0.75 * facing)
col = F.mix(col, np.array([0.2, 0.7, 0.2]), bathed * 0.5)
F.set_attr(high, 'Col', col)
F.set_attr(high, 'Rough', np.repeat(np.clip(0.75 + 0.2 * moss - 0.15 * is_stone * (1 - moss) - 0.2 * damp, 0.2, 1)[:, None], 3, 1))
F.set_attr(high, 'Emit', np.outer(bathed ** 2, np.array([0.25, 1.0, 0.22])))

maps = F.bake_maps(high, low, 1024 if QUICK else 4096, 0.03, 8 if QUICK else 32, OPT['tmp'])
F.assign(low, F.game_material('M_Boss_Warden_Body', *maps[:3], emit=maps[3], emit_strength=1.5))
bpy.data.objects.remove(high)

# ------------------------------------------------------------------ the mask, the armour, the heart

extra = []
TALL, WIDE, DEEP = 0.36, 0.26, 0.14


def paint_mask(q, nq):
    """Carved wood in the temple's manner: no eyes, two black hollows; a brow band, a nose ridge, a closed mouth, ringed cheeks."""
    y, z = q[:, 1] - FACE.y, q[:, 2] - FACE.z
    front = F.smoothstep(-0.02, 0.05, q[:, 0] - FACE.x)
    hollow = np.zeros(len(q))
    rings = np.zeros(len(q))
    for s in (-1, 1):
        d = np.sqrt(((y - s * 0.1) / 0.058) ** 2 + ((z - 0.055) / 0.085) ** 2)
        hollow = np.maximum(hollow, F.smoothstep(1.0, 0.75, d))
        rings = np.maximum(rings, F.smoothstep(0.35, 0.0, np.abs(((d * 1.6) % 1.0) - 0.5)) * F.smoothstep(3.2, 1.2, d) * (d > 1.05))
    brow = F.smoothstep(0.03, 0.01, np.abs(z - (0.17 + 0.02 * np.cos(y * 12)))) * (np.abs(y) < 0.22)
    nose = F.smoothstep(0.035, 0.012, np.abs(y)) * F.smoothstep(0.16, 0.12, z) * F.smoothstep(-0.12, -0.08, z)
    mouth = F.smoothstep(0.012, 0.004, np.abs(z + 0.19)) * (np.abs(y) < 0.085)
    chev = F.smoothstep(0.3, 0.0, np.abs((((z + np.abs(y) * 0.8) * 22) % 1.0) - 0.5)) * (z < -0.06) * (z > -0.3) * (np.abs(y) > 0.05)
    grain = F.fbm(q * np.array([8.0, 8.0, 60.0]), 4)
    crackw = F.smoothstep(0.02, 0.0, np.abs(F.fbm(q * np.array([3.0, 14.0, 1.5]) + 4.0, 3) - 0.5))
    h = (brow * 0.012 + nose * 0.022 - rings * 0.006 - chev * 0.005 - mouth * 0.012 - hollow * 0.07 + (grain - 0.5) * 0.004 - crackw * 0.006) * front
    c = np.tile(np.array([0.3, 0.2, 0.11]), (len(q), 1)) * (0.6 + 0.8 * grain)[:, None]
    c = F.mix(c, np.array([0.1, 0.06, 0.035]), np.clip(rings * 0.8 + chev * 0.7 + mouth + crackw * 0.8, 0, 1))       # dirt in the cuts
    c = F.mix(c, np.array([0.45, 0.36, 0.22]), np.clip(brow + nose, 0, 1) * 0.5)                                    # worn high points
    c = F.mix(c, np.array([0.0, 0.0, 0.0]), hollow)
    c = F.mix(c, np.array([0.08, 0.13, 0.04]), F.smoothstep(0.6, 0.75, F.fbm(q * 9 + 2, 3)) * (1 - hollow) * 0.6)   # moss creeping on
    return h, c, 0.6 + 0.3 * grain - 0.2 * hollow


mask = F.blob('mask', FACE, (DEEP, WIDE, TALL), seg=32)
extra.append(F.bake_part(mask, 'M_Boss_Warden_Mask', paint_mask, 512 if QUICK else 2048, 3 if QUICK else 4, 0.02, OPT['tmp']))


def paint_rust(q, nq):
    """Armour fragments centuries in the damp: hammered iron, pitted, rust blooming over what is left of it."""
    dents = F.fbm(q * 9.0, 4)
    pits = F.smoothstep(0.6, 0.8, F.fbm(q * 40.0, 3))
    bloom = F.smoothstep(0.38, 0.62, F.fbm(q * 5.0 + 13.0, 4))
    c = F.mix(np.tile(np.array([0.07, 0.07, 0.075]), (len(q), 1)), np.array([0.16, 0.07, 0.035]), bloom)
    c = F.mix(c, np.array([0.3, 0.14, 0.05]), F.smoothstep(0.6, 0.8, F.fbm(q * 16 + 2, 3)) * bloom)
    c = F.mix(c, np.array([0.06, 0.1, 0.03]), F.smoothstep(0.3, 0.8, nq[:, 2]) * F.smoothstep(0.5, 0.65, F.fbm(q * 7 + 30, 3)) * 0.8)
    return (dents - 0.5) * 0.012 - pits * 0.006, c, 0.45 + 0.45 * bloom


armour = [F.blob('pauldron', SH_R + Vector((0.0, -0.06, 0.06)), (0.3, 0.27, 0.2), rot=(0.5, 0, 0), seg=20),
          F.slab('breast', (0.36, 0.38, 3.2), (0.03, 0.24, 0.3), (0.12, -0.3, 0.35))[0],
          F.slab('greave', (0.34, 0.47, 0.72), (0.035, 0.16, 0.3), (0.0, -0.18, 0.0))[0],
          F.tube('band', [(0.05, -0.37, 1.62), (0.07, -0.4, 1.9)], [0.27, 0.28], 18)]
extra.append(F.bake_part(F.join('armour', armour), 'M_Boss_Warden_Armour', paint_rust, 512 if QUICK else 2048, 2 if QUICK else 3, 0.02, OPT['tmp']))

heart_m = F.flat_material('M_Boss_Warden_Heart', (0.2, 0.8, 0.2), 0.3, emit=(0.25, 1.0, 0.22), strength=7.0)
extra.append(F.assign(F.blob('heart', HEART, (0.13, 0.13, 0.16), seg=18), heart_m))

# small flowers on the moss, vines draped from the shoulders
petal = [F.flat_material('M_Boss_Warden_FlowerPale', (0.85, 0.8, 0.5), 0.6), F.flat_material('M_Boss_Warden_FlowerViolet', (0.5, 0.36, 0.7), 0.6)]
stem_m = F.flat_material('M_Boss_Warden_Vine', (0.04, 0.07, 0.02), 0.7)
leaf_m = F.flat_material('M_Boss_Warden_Leaf', (0.06, 0.14, 0.03), 0.55)
stems, blooms, leaves = [], [[], []], []
for bi, b in enumerate(BEDS):
    for f in range(6):
        base = Vector(b) + Vector((rng.uniform(-0.2, 0.2), rng.uniform(-0.2, 0.2), 0.05))
        head = base + Vector((rng.uniform(-0.03, 0.03), rng.uniform(-0.03, 0.03), rng.uniform(0.07, 0.14)))
        stems.append(F.tube('stem', [base, head], [0.006, 0.004], 5))
        for k in range(5):
            a = 2 * math.pi * k / 5
            d = Vector((math.cos(a), math.sin(a), 0.25)).normalized() * 0.03
            sd = Vector((-math.sin(a), math.cos(a), 0)) * 0.012
            blooms[(bi + f) % 2].append(F.new_obj('petal', [head, head + d * 0.5 + sd, head + d, head + d * 0.5 - sd], [(0, 1, 2, 3)]))
for k in range(10):
    s = 1 if k % 2 else -1
    x = rng.uniform(-0.45, 0.1)
    pts = [(x, s * rng.uniform(0.2, 0.5), 3.78), (x + 0.1, s * rng.uniform(0.6, 0.8), rng.uniform(3.2, 3.45)), (x + rng.uniform(0.0, 0.25), s * rng.uniform(0.55, 0.7), rng.uniform(2.3, 2.9))]
    sp = F.spline(pts, 6)
    stems.append(F.tube('vine', sp, [0.022 - 0.0012 * i for i in range(len(sp))], 6))
    for q in sp[3::3]:
        d = Vector((rng.uniform(-1, 1), rng.uniform(-1, 1), rng.uniform(-0.6, 0.1))).normalized() * 0.11
        sd = d.cross(Z).normalized() * 0.04
        leaves.append(F.new_obj('leaf', [q, q + d * 0.5 + sd, q + d, q + d * 0.5 - sd], [(0, 1, 2, 3)]))
extra.append(F.assign(F.join('stems', stems), stem_m))
extra.append(F.assign(F.join('leaves', leaves), leaf_m))
for k in (0, 1):
    extra.append(F.assign(F.join('petals', blooms[k]), petal[k]))

boss = F.join('SM_Boss_JungleWarden', [low] + extra)
size = F.export_gltf(OPT['out'], 'SM_Boss_JungleWarden', [boss])
print('FORGE gltf bytes', size, 'tris', sum(len(q.vertices) - 2 for q in boss.data.polygons))

# ------------------------------------------------------------------ present (redo alone with present.py)

import present

present.run('JungleWarden', OPT)
F.done()
