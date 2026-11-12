"""Jaw of the Mire, "the Swallowing King" (docs/design/bosses/jaw_of_the_mire.md, section 2).

Fourteen metres of alligator, so old he no longer looks like one: moss, mud, vines and coral-like growths over his back;
the left eye clouded, 40% of the left front teeth gone, a rusted harpoon head in the right shoulder from a hunt eighty
years ago; dark olive skin going near-black along the spine, a pale yellow belly, mottled brown-yellow teeth, blood-brown
streaks round the mouth; the good eye golden-amber. Faces +X, +Y is his left, z = 0 is the ground.
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
rng = np.random.default_rng(0x4A41)

# ------------------------------------------------------------------ anatomy tables

# x, centre z, half width, height above, depth below
SPINE = [(-7.0, 0.34, 0.05, 0.17, 0.10), (-6.0, 0.40, 0.14, 0.32, 0.18), (-5.0, 0.45, 0.24, 0.42, 0.25), (-4.0, 0.50, 0.36, 0.50, 0.32),
         (-3.0, 0.58, 0.50, 0.55, 0.40), (-2.0, 0.68, 0.68, 0.60, 0.50), (-1.0, 0.78, 0.88, 0.66, 0.60), (0.0, 0.85, 1.10, 0.70, 0.68),
         (1.0, 0.88, 1.28, 0.74, 0.72), (2.0, 0.90, 1.36, 0.76, 0.76), (3.0, 0.90, 1.30, 0.74, 0.72), (4.0, 0.90, 1.15, 0.70, 0.64),
         (4.6, 0.96, 0.96, 0.58, 0.52), (5.05, 1.04, 0.90, 0.48, 0.44)]
UPPER = [(4.95, 0.90, 0.42, 0.12), (5.3, 0.83, 0.40, 0.10), (5.7, 0.67, 0.27, 0.09), (6.2, 0.58, 0.21, 0.08), (6.7, 0.56, 0.20, 0.08),
         (6.95, 0.50, 0.21, 0.08), (7.08, 0.30, 0.12, 0.06)]
LOWER = [(4.85, 0.88, 0.08, 0.36), (5.4, 0.80, 0.07, 0.3), (5.9, 0.63, 0.06, 0.25), (6.4, 0.54, 0.06, 0.19), (6.85, 0.50, 0.06, 0.16),
         (7.0, 0.30, 0.05, 0.10)]


def spine_y(x):
    """The tail's slow S (the body is straight from the hips forward)."""
    t = np.clip(-np.asarray(x, dtype=float) / 7.0, 0.0, 1.0)
    return 0.95 * np.sin(t * math.pi * 1.15) * t


def interp(table, x, col):
    xs = [r[0] for r in table]
    return np.interp(x, xs, [r[col] for r in table])


def upper_z(x):
    return 1.10 + (np.asarray(x, dtype=float) - 5.0) * 0.14


def lower_z(x):
    return 0.98 - (np.asarray(x, dtype=float) - 5.0) * 0.16


def top_z(x, dy):
    """Height of the back at x, dy metres off the spine."""
    w, hu = interp(SPINE, x, 2), interp(SPINE, x, 3)
    q = np.clip(1 - (abs(dy) / w) ** 2.3, 0, 1) ** (1 / 2.3)
    return interp(SPINE, x, 1) + hu * q


# ------------------------------------------------------------------ 1. block-out

F.reset()
parts = []
parts.append(F.body('trunk', [dict(c=(x, float(spine_y(x)), z), w=w, hu=hu, hd=hd, right=(0, 1, 0), up=(0, 0, 1)) for x, z, w, hu, hd in SPINE]))
parts.append(F.body('skull', [dict(c=(x, 0, float(upper_z(x))), w=w, hu=hu, hd=hd, pd=5.0, right=(0, 1, 0), up=(0, 0, 1)) for x, w, hu, hd in UPPER]))
parts.append(F.body('mandible', [dict(c=(x, 0, float(lower_z(x))), w=w, hu=hu, hd=hd, pu=5.0, right=(0, 1, 0), up=(0, 0, 1)) for x, w, hu, hd in LOWER]))
parts.append(F.blob('throat', (4.75, 0, 0.72), (0.55, 0.7, 0.36)))
parts.append(F.blob('nose', (6.9, 0, 1.52), (0.17, 0.24, 0.1)))
for s in (-1, 1):
    parts.append(F.blob('jowl', (5.0, s * 0.72, 1.0), (0.5, 0.3, 0.36)))
    parts.append(F.blob('orbit', (5.32, s * 0.47, 1.49), (0.25, 0.17, 0.15)))
    parts.append(F.blob('brow', (5.34, s * 0.55, 1.6), (0.22, 0.15, 0.055), rot=(s * -0.35, 0, 0)))
    parts.append(F.blob('table', (5.02, s * 0.5, 1.5), (0.3, 0.22, 0.12)))
    parts.append(F.blob('lid_up', (5.36, s * 0.6, 1.585), (0.15, 0.085, 0.05), rot=(s * -0.5, 0, 0)))
    parts.append(F.blob('lid_low', (5.36, s * 0.61, 1.42), (0.14, 0.075, 0.045), rot=(s * 0.5, 0, 0)))
    # front leg: sprawled, elbow out, the hand planted
    front = [(4.0, s * 0.95, 0.85), (3.6, s * 2.0, 1.0), (4.22, s * 2.24, 0.22), (4.52, s * 2.26, 0.1)]
    parts.append(F.tube('foreleg', front, [0.44, 0.34, 0.24, 0.2]))
    parts.append(F.blob('shoulder', (3.95, s * 1.05, 1.0), (0.55, 0.5, 0.42)))
    for k in range(5):
        a = (-0.75 + 0.42 * k) * s
        d = Vector((math.cos(a), math.sin(a) * 1.0 + s * 0.25, 0)).normalized()
        base = Vector(front[-1]) + Vector((0.05, 0, -0.02))
        parts.append(F.tube('finger', [base, base + d * 0.3 + Vector((0, 0, 0.04)), base + d * 0.55 + Vector((0, 0, -0.03))], [0.1, 0.085, 0.06], 10))
    # hind leg: the knee forward and out, a long foot
    hind = [(0.3, s * 1.05, 0.85), (1.0, s * 2.05, 0.9), (0.2, s * 2.35, 0.24), (0.8, s * 2.42, 0.11)]
    parts.append(F.tube('hindleg', hind, [0.56, 0.44, 0.29, 0.22]))
    parts.append(F.blob('haunch', (0.35, s * 1.1, 1.0), (0.7, 0.55, 0.5)))
    for k in range(4):
        a = (-0.35 + 0.3 * k) * s
        d = Vector((math.cos(a), math.sin(a) + s * 0.1, 0)).normalized()
        base = Vector(hind[-1])
        parts.append(F.tube('toe', [base, base + d * 0.38 + Vector((0, 0, 0.03)), base + d * 0.7 + Vector((0, 0, -0.04))], [0.11, 0.09, 0.06], 10))

# the keeled scutes that give his back its saw edge: six files over the trunk, two crests on the tail merging into one
x = 4.55
row = 0
while x > 0.1:
    w = float(interp(SPINE, x, 2))
    for col in (-2.5, -1.5, -0.5, 0.5, 1.5, 2.5):
        dy = col * 0.3 * w / 1.36 * 1.25
        if abs(dy) > w * 0.82:
            continue
        j = rng.uniform(0.85, 1.2)
        parts.append(F.blob('scute', (x + rng.uniform(-0.03, 0.03), dy, float(top_z(x, dy)) + 0.01), (0.17 * j, 0.075, 0.085 * j), rot=(0, 0.25, 0), seg=12))
    x -= 0.36
    row += 1
x = 0.0
while x > -6.85:
    w = float(interp(SPINE, x, 2))
    sy = float(spine_y(x))
    tall = 0.16 + 0.1 * math.sin(min(1.0, -x / 3.0) * math.pi / 2)
    if x > -3.6:
        for s in (-1, 1):
            dy = s * w * 0.5
            parts.append(F.blob('crest', (x, sy + dy, float(top_z(x, dy)) + tall * 0.25), (0.17, 0.05, tall), rot=(s * -0.25, 0.35, 0), seg=12))
    else:
        t = (-x - 3.6) / 3.3
        parts.append(F.blob('crest', (x, sy, float(top_z(x, 0)) + tall * 0.2), (0.16, 0.04, tall * (1.15 - 0.7 * t)), rot=(0, 0.35, 0), seg=12))
    x -= 0.33

# a piece of the Mire that learned to eat: mounds of mud and moss grown fast to the back
MOUNDS = []
for k in range(16):
    x = rng.uniform(-2.6, 4.2)
    w = float(interp(SPINE, x, 2))
    dy = rng.uniform(-0.7, 0.7) * w
    c = (x, float(spine_y(x)) + dy, float(top_z(x, dy)) + 0.02)
    r = (rng.uniform(0.28, 0.6), rng.uniform(0.22, 0.45), rng.uniform(0.07, 0.15))
    MOUNDS.append((c, r))
    parts.append(F.blob('mound', c, r, rot=(0, 0, rng.uniform(0, 3.1)), seg=14))
# the flesh grown round the harpoon in the right shoulder
WOUND = Vector((4.05, -1.0, 1.42))
parts.append(F.blob('scar', WOUND, (0.3, 0.26, 0.2)))

# ------------------------------------------------------------------ 2. fuse, 3. sculpt

skin = F.fuse('jaw_skin', parts, 0.06 if QUICK else 0.035, smooth=5)
low = F.retopo(skin, 'SM_Boss_JawOfTheMire', 30000 if QUICK else 80000)
high = F.subdivide(skin, 1 if QUICK else 2)
print('FORGE high verts', len(high.data.vertices), 'low tris', len(low.data.polygons))

p = F.co(high)
n = F.normals(high)
X, Y, Z = p[:, 0], p[:, 1], p[:, 2]
DY = Y - spine_y(X)
W = interp(SPINE, X, 2)
in_head = F.smoothstep(4.95, 5.25, X)
limb = F.smoothstep(0.02, 0.3, np.abs(DY) - W) * (1 - in_head)
back = F.smoothstep(0.35, 0.75, n[:, 2]) * (1 - limb) * (1 - in_head)
belly = F.smoothstep(0.25, 0.65, -n[:, 2]) * (1 - limb) * F.smoothstep(1.0, 0.6, Z)
mouth = in_head * np.maximum(F.smoothstep(0.4, 0.8, -n[:, 2]) * F.smoothstep(0.1, 0.0, np.abs(Z - (upper_z(X) - 0.1))),
                             F.smoothstep(0.4, 0.8, n[:, 2]) * F.smoothstep(0.1, 0.0, np.abs(Z - (lower_z(X) + 0.07))))
mound = np.zeros(len(p))
for c, r in MOUNDS:
    d = np.sqrt(((X - c[0]) / (r[0] * 1.25)) ** 2 + ((Y - c[1]) / (r[1] * 1.25)) ** 2 + ((Z - c[2]) / (r[2] * 2.2)) ** 2)
    mound = np.maximum(mound, F.smoothstep(1.0, 0.6, d))

warp = p + (F.fbm(p * 1.3, 3)[:, None] - 0.5) * 0.25
dome_c, crack_c, id_c = F.cells(warp / 0.13, 0.12)              # flank scales
dome_f, crack_f, id_f = F.cells(warp / 0.07, 0.16)         # limb, jaw and throat scales
dome_h, crack_h, id_h = F.cells(warp / 0.17, 0.1)        # the skull's plates
# the back: transverse rows of square keeled plates between the scutes
u = X / 0.36
v = DY / (0.375 * np.maximum(W, 0.2) / 1.36 * 1.25) + 0.5
fu, fv = u - np.floor(u), v - np.floor(v)
edge = np.minimum(np.minimum(fu, 1 - fu) * 0.36, np.minimum(fv, 1 - fv) * 0.3)
plate = (1 - (2 * fu - 1) ** 4) * (1 - (2 * fv - 1) ** 4)
groove_b = 1 - F.smoothstep(0.0, 0.035, edge)
h_back = plate * 0.02 - groove_b * 0.022
# the belly: flat transverse plates
ub, vb = X / 0.21, DY / 0.3
fub = ub - np.floor(ub)
fvb = vb + 0.5 * (np.floor(ub) % 2)
fvb = fvb - np.floor(fvb)
edge_b = np.minimum(np.minimum(fub, 1 - fub) * 0.21, np.minimum(fvb, 1 - fvb) * 0.3)
groove_y = 1 - F.smoothstep(0.0, 0.022, edge_b)
h_belly = -groove_y * 0.012
h_flank = dome_c * 0.014 - crack_c * 0.014
h_fine = dome_f * 0.009 - crack_f * 0.01
h_head = dome_h * 0.008 - crack_h * 0.016 + (F.fbm(p * 22, 3) - 0.5) * 0.008
fine_w = np.clip(limb + in_head * F.smoothstep(0.2, -0.3, n[:, 2]) + F.smoothstep(4.3, 4.9, X) * (1 - in_head) * 0.6, 0, 1)
h = h_flank * (1 - fine_w) + h_fine * fine_w
h = h * (1 - back) + h_back * back
h = h * (1 - belly) + h_belly * belly
head_top = in_head * F.smoothstep(-0.1, 0.4, n[:, 2])
h = h * (1 - head_top) + h_head * head_top
# old age: folds at the neck and the roots of the legs, pits everywhere, and the mounds' own clodded surface
fold = np.abs(F.fbm(p * np.array([1.2, 5.0, 5.0]), 4) - 0.5) * 2
h += -(1 - fold) ** 3 * 0.02 * np.clip(F.smoothstep(4.2, 4.7, X) * (1 - in_head) + limb * 0.5, 0, 1)
h += (F.fbm(p * 38, 3) - 0.5) * 0.004
clod = (F.fbm(p * 9, 4) - 0.5) * 0.05 + (F.fbm(p * 30, 3) - 0.5) * 0.012
h = h * (1 - mound) + clod * mound
h = h * (1 - mouth) + (F.fbm(p * 14, 3) - 0.5) * 0.012 * mouth
crack = np.clip((crack_c * (1 - fine_w) + crack_f * fine_w) * (1 - back) * (1 - belly) + groove_b * back + groove_y * belly, 0, 1)
crack = (crack * (1 - head_top) + crack_h * head_top) * (1 - mound) * (1 - mouth)
F.push(high, h)

# ------------------------------------------------------------------ 4. paint

olive = np.array([0.105, 0.125, 0.055])
black = np.array([0.018, 0.02, 0.016])
belly_c = np.array([0.56, 0.47, 0.24])
col = np.tile(olive, (len(p), 1))
mottle = F.fbm(p * 2.2, 4)
col *= (0.65 + 0.7 * mottle)[:, None]
col = F.mix(col, np.array([0.16, 0.13, 0.06]), F.smoothstep(0.55, 0.8, F.fbm(p * 5.0 + 9.1, 3)) * 0.6)          # brown mottling
spine_dark = F.smoothstep(0.75, 0.05, np.abs(DY) / np.maximum(W, 0.2)) * F.smoothstep(0.0, 0.6, n[:, 2]) * (1 - limb)
col = F.mix(col, black, np.clip(spine_dark * 0.92 + in_head * F.smoothstep(0.2, 0.7, n[:, 2]) * 0.55, 0, 1))
under = np.clip(belly + F.smoothstep(0.3, 0.8, -n[:, 2]) * in_head + limb * F.smoothstep(0.2, 0.7, -n[:, 2]) * 0.8, 0, 1)
col = F.mix(col, belly_c * (0.8 + 0.4 * F.fbm(p * 6, 3))[:, None], under)
plate_tone = (0.82 + 0.36 * id_c)[:, None]
col *= plate_tone * (1 - fine_w)[:, None] + (0.85 + 0.3 * id_f)[:, None] * fine_w[:, None]
col *= (1 - 0.72 * crack)[:, None]                                                                             # dirt in every crack
dust = F.smoothstep(0.5, 0.9, n[:, 2]) * F.smoothstep(0.4, 0.75, F.fbm(p * 3.1 + 4.0, 4)) * 0.35
col = F.mix(col, np.array([0.2, 0.19, 0.13]), dust)                                                            # dried silt on what faces the sky
moss_n = F.fbm(p * 7.0 + 2.0, 4)
moss = np.clip(mound * F.smoothstep(0.3, 0.55, moss_n) + back * F.smoothstep(0.48, 0.66, F.fbm(p * 1.7 + 30, 4)) * 0.9, 0, 1)
col = F.mix(col, np.array([0.09, 0.16, 0.035]) * (0.6 + 0.9 * F.fbm(p * 25, 2))[:, None], moss)
mud = np.clip(mound * (1 - F.smoothstep(0.35, 0.6, moss_n)) + F.smoothstep(0.75, 0.2, Z) * F.smoothstep(0.35, 0.7, F.fbm(p * 2.6 + 11, 4)), 0, 1)
col = F.mix(col, np.array([0.11, 0.08, 0.05]) * (0.7 + 0.6 * F.fbm(p * 18, 2))[:, None], mud * 0.9)
col = F.mix(col, np.array([0.34, 0.14, 0.13]) * (0.7 + 0.6 * F.fbm(p * 12, 3))[:, None], mouth)                # the mouth's flesh
# blood-brown streaks round the mouth, run down from the lips
lip = in_head * F.smoothstep(0.22, 0.0, np.minimum(np.abs(Z - (upper_z(X) - 0.08)), np.abs(Z - (lower_z(X) + 0.05)))) * (1 - mouth)
streak = F.smoothstep(0.45, 0.7, F.fbm(p * np.array([9.0, 9.0, 1.2]), 3))
col = F.mix(col, np.array([0.13, 0.035, 0.02]), np.clip(lip * (0.45 + 0.55 * streak), 0, 1) * 0.85)
wound = F.smoothstep(0.42, 0.1, np.linalg.norm(p - np.array(WOUND), axis=1))
col = F.mix(col, np.array([0.2, 0.07, 0.06]), wound * 0.8)                                                     # the old wound, never closed
F.set_attr(high, 'Col', col)
wet = F.smoothstep(0.9, 0.25, Z)
rough = 0.62 - 0.3 * wet + 0.3 * moss + 0.12 * dust - 0.3 * mouth + 0.1 * crack
F.set_attr(high, 'Rough', np.repeat(np.clip(rough, 0.12, 1.0)[:, None], 3, 1))

# ------------------------------------------------------------------ 5-6. bake

base, normal, orm = F.bake_maps(high, low, 1024 if QUICK else 4096, 0.07, 8 if QUICK else 32, OPT['tmp'])
F.assign(low, F.game_material('M_Boss_Jaw_Skin', base, normal, orm))
bpy.data.objects.remove(high)

# ------------------------------------------------------------------ what stays real geometry

stump_m = F.flat_material('M_Boss_Jaw_ToothStump', (0.2, 0.12, 0.06), 0.5)
claw_m = F.flat_material('M_Boss_Jaw_Claw', (0.05, 0.04, 0.03), 0.3)
rust_m = F.flat_material('M_Boss_Jaw_Rust', (0.2, 0.08, 0.035), 0.85, 0.6)
coral_m = F.flat_material('M_Boss_Jaw_Coral', (0.2, 0.17, 0.12), 0.85)
vine_m = F.flat_material('M_Boss_Jaw_Vine', (0.035, 0.06, 0.02), 0.7)
leaf_m = F.flat_material('M_Boss_Jaw_Leaf', (0.06, 0.13, 0.03), 0.55)

teeth, stumps = [], []
for s in (-1, 1):
    for jaw, table, zf, sign in (('u', UPPER, upper_z, -1), ('l', LOWER, lower_z, 1)):
        x = 5.3
        k = 0
        while x < 6.98:
            w = float(interp(table, x, 1))
            size = (0.09 + 0.06 * math.sin(k * 1.9) ** 2) * (1.5 if k in (9, 10) else 1.0) * rng.uniform(0.85, 1.15)
            z0 = float(zf(x)) + (-0.07 if jaw == 'u' else 0.05)
            b = Vector((x, s * (w - 0.05), z0))
            missing = s > 0 and x > 6.05 and rng.random() < 0.4          # 40% of the left front teeth
            if missing:
                stumps.append(F.cone('stump', b, b + Vector((0, 0, sign * 0.035)), 0.045, 8))
            else:
                teeth.append(F.cone('tooth', b, b + Vector((0.02, s * 0.02, sign * size)), 0.03 + size * 0.12, 8, 0.015, (-1, 0, 0)))
            x += 0.085 + 0.02 * math.sin(k * 2.3)
            k += 1
for jaw, zf, sign in (('u', upper_z, -1), ('l', lower_z, 1)):               # the front of the snout
    for k in range(5):
        y = -0.22 + 0.11 * k
        b = Vector((7.02 if jaw == 'u' else 6.95, y, float(zf(7.0)) + (-0.05 if jaw == 'u' else 0.04)))
        if y > 0.05 and rng.random() < 0.5:
            stumps.append(F.cone('stump', b, b + Vector((0, 0, sign * 0.03)), 0.04, 8))
        else:
            teeth.append(F.cone('tooth', b, b + Vector((0.03, 0, sign * 0.12)), 0.038, 8))


def paint_teeth(q, nq):
    """Old ivory: stained dark at the gum, yellow-brown up the crown, paler at the worn tip; enamel ridges along the tooth."""
    du = (upper_z(q[:, 0]) - 0.07) - q[:, 2]
    dl = q[:, 2] - (lower_z(q[:, 0]) + 0.05)
    t = np.clip(np.minimum(np.abs(du), np.abs(dl)) / 0.13, 0, 1)
    ridge = F.fbm(q * np.array([70.0, 70.0, 9.0]), 3)
    stain = F.fbm(q * 14.0 + 3.0, 4)
    c = F.mix(np.tile(np.array([0.1, 0.06, 0.03]), (len(q), 1)), np.array([0.36, 0.26, 0.11]), F.smoothstep(0.0, 0.35, t))
    c = F.mix(c, np.array([0.55, 0.47, 0.3]), F.smoothstep(0.55, 1.0, t) * 0.8)
    c *= (0.7 + 0.5 * stain)[:, None] * (0.85 + 0.3 * ridge)[:, None]
    c = F.mix(c, np.array([0.12, 0.03, 0.02]), F.smoothstep(0.62, 0.8, F.fbm(q * 6.0 + 40.0, 3)) * (1 - t) * 0.7)   # old blood at the gum
    return (ridge - 0.5) * 0.004 * (1 - t * 0.5), c, 0.5 - 0.22 * t + 0.15 * (stain - 0.5)


teeth_ob = F.bake_part(F.join('teeth', teeth), 'M_Boss_Jaw_Teeth', paint_teeth, 512 if QUICK else 2048, 2, 0.01, OPT['tmp'])
extra = [teeth_ob, F.assign(F.join('stumps', stumps), stump_m)]

claws = []
for s in (-1, 1):
    for k in range(5):
        a = (-0.75 + 0.42 * k) * s
        d = Vector((math.cos(a), math.sin(a) + s * 0.25, 0)).normalized()
        b = Vector((4.57, s * 2.26, 0.08)) + d * 0.5
        claws.append(F.cone('claw', b, b + d * 0.3 + Vector((0, 0, -0.08)), 0.055, 8, 0.05, (0, 0, 1)))
    for k in range(4):
        a = (-0.35 + 0.3 * k) * s
        d = Vector((math.cos(a), math.sin(a) + s * 0.1, 0)).normalized()
        b = Vector((0.8, s * 2.42, 0.08)) + d * 0.64
        claws.append(F.cone('claw', b, b + d * 0.36 + Vector((0, 0, -0.08)), 0.06, 8, 0.06, (0, 0, 1)))
extra.append(F.assign(F.join('claws', claws), claw_m))



def paint_eye(centre, side, blind):
    def paint(q, nq):
        d = (q - np.array(centre)) / 0.09
        out = np.clip(d[:, 1] * side, -1, 1)                       # 1 where the eye looks out of the head
        r = np.sqrt(np.clip(1 - out * out, 0, 1))                   # 0 at the pole of the gaze
        ang = np.arctan2(d[:, 2], d[:, 0])
        fibre = F.fbm(np.stack([np.cos(ang) * 6, np.sin(ang) * 6, r * 2.5], 1), 4)
        iris = np.tile(np.array([0.62, 0.36, 0.05]), (len(q), 1)) * (0.55 + 0.9 * fibre)[:, None]
        iris = F.mix(iris, np.array([0.9, 0.62, 0.12]), F.smoothstep(0.5, 0.15, r) * 0.6)          # golden round the pupil
        iris = F.mix(iris, np.array([0.1, 0.05, 0.01]), F.smoothstep(0.62, 0.8, r))                # the dark limbal ring
        c = F.mix(iris, np.array([0.05, 0.03, 0.02]), F.smoothstep(0.8, 0.95, r))                 # what little white a reptile shows
        slit = F.smoothstep(0.1, 0.06, np.abs(d[:, 0]) / np.maximum(1 - (d[:, 2] / 0.62) ** 2, 0.02) ** 0.5) * (np.abs(d[:, 2]) < 0.62)
        c = F.mix(c, np.array([0.0, 0.0, 0.0]), slit)
        rough = np.full(len(q), 0.04)
        if blind:
            cloud = F.fbm(d * 3.0 + 7.0, 4)
            c = F.mix(c, np.array([0.55, 0.6, 0.63]) * (0.75 + 0.5 * cloud)[:, None], np.clip(0.75 + 0.3 * cloud, 0, 1) * (out > -0.2))
            rough = 0.18 + 0.2 * cloud
        c[out < -0.1] = np.array([0.03, 0.02, 0.02])
        return None, c, rough
    return paint


extra.append(F.bake_part(F.blob('eye_r', (5.36, -0.6, 1.5), 0.09, seg=24), 'M_Boss_Jaw_EyeAmber', paint_eye((5.36, -0.6, 1.5), -1, False), 512, 3, 0.004, OPT['tmp']))
extra.append(F.bake_part(F.blob('eye_l', (5.36, 0.6, 1.5), 0.09, seg=24), 'M_Boss_Jaw_EyeMilky', paint_eye((5.36, 0.6, 1.5), 1, True), 512, 3, 0.004, OPT['tmp']))

# the harpoon head: eighty years in the shoulder, the shaft long broken off
tip = WOUND + Vector((-0.05, 0.12, -0.1))
out = Vector((0.25, -0.75, 0.6)).normalized()
iron = [F.tube('shaft', [tip, tip + out * 0.75], [0.035, 0.03], 8)]
for a in (0.0, 2.1, 4.2):
    side = out.cross(Vector((math.cos(a), math.sin(a), 0.3))).normalized()
    iron.append(F.cone('barb', tip + out * 0.32, tip + out * 0.12 + side * 0.13, 0.025, 6))
iron.append(F.blob('ring', tip + out * 0.75, (0.06, 0.06, 0.03)))
extra.append(F.assign(F.join('harpoon', iron), rust_m))

# coral-like growths: knots of branching fingers on the mounds
coral = []
for c, r in MOUNDS[::2]:
    for f in range(5):
        a = f * 1.3 + rng.uniform(0, 1)
        b0 = Vector(c) + Vector((math.cos(a) * r[0] * 0.3, math.sin(a) * r[1] * 0.3, r[2] * 0.5))
        b1 = b0 + Vector((math.cos(a) * 0.1, math.sin(a) * 0.1, rng.uniform(0.1, 0.22)))
        coral.append(F.tube('coral', [b0, b0.lerp(b1, 0.55) + Vector((rng.uniform(-0.04, 0.04), rng.uniform(-0.04, 0.04), 0)), b1], [0.05, 0.036, 0.022], 7))
        for t in (0.45, 0.7):
            q = b0.lerp(b1, t)
            e = q + Vector((math.cos(a + 2.0 * t * 5), math.sin(a + 2.0 * t * 5), 0.9)).normalized() * rng.uniform(0.08, 0.15)
            coral.append(F.tube('coral', [q, e], [0.028, 0.016], 6))
extra.append(F.assign(F.join('coral', coral), coral_m))

# vines trailing from the back down the flanks, a few leaves on each
vines, leaves = [], []
for k in range(12):
    x = rng.uniform(-2.2, 4.0)
    s = 1 if k % 2 else -1
    w = float(interp(SPINE, x, 2))
    sy = float(spine_y(x))
    path = []
    for t in np.linspace(0.15, 1.12, 9):
        dy = s * w * min(t, 1.0)
        q = max(1 - min(t, 1.0) ** 2.3, 0) ** (1 / 2.3)
        z = float(interp(SPINE, x, 1)) + float(interp(SPINE, x, 3)) * q if t <= 1.0 else float(interp(SPINE, x, 1)) - (t - 1.0) * 4.0
        path.append(Vector((x + 0.25 * math.sin(t * 5 + k), sy + dy * 1.03 + s * 0.03, max(z + 0.03, 0.12))))
    vines.append(F.tube('vine', path, [0.03] * 5 + [0.024, 0.02, 0.016, 0.01], 6))
    for q in path[2::2]:
        d = Vector((rng.uniform(-1, 1), rng.uniform(-1, 1), rng.uniform(-0.2, 0.5))).normalized() * 0.16
        sd = d.cross(Vector((0, 0, 1))).normalized() * 0.055
        leaves.append(F.new_obj('leaf', [q, q + d * 0.5 + sd, q + d, q + d * 0.5 - sd], [(0, 1, 2, 3)]))
extra.append(F.assign(F.join('vines', vines), vine_m))
extra.append(F.assign(F.join('leaves', leaves), leaf_m))

boss = F.join('SM_Boss_JawOfTheMire', [low] + extra)
size = F.export_glb(os.path.join(OPT['out'], 'SM_Boss_JawOfTheMire.glb'), [boss])
print('FORGE glb bytes', size, 'tris', sum(len(q.vertices) - 2 for q in boss.data.polygons))

# ------------------------------------------------------------------ present (redo alone with present.py)

import present

present.run('JawOfTheMire', OPT)
F.done()
