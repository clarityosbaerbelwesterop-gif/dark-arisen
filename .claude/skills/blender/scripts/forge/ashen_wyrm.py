"""The Ashen Wyrm (docs/design/bosses/ashen_wyrm.md, section 2).

Thirty metres from snout to tail tip, forty across the wings unfurled; a quadrupedal serpentine dragon, nearer the Eastern
dragon than the Western, her massive wings folded against her body; black obsidian scales overlaid with glowing cracks of
molten gold where her inner fire seeps through; a long, horned, ancient head, two curved horns arching backward, eyes like
twin suns burning in deep sockets; wings leather-black with gold veins. Tired, not angry. Her floor is black obsidian
polished by her body (section 3). Faces +X, z = 0 is the floor.
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
rng = np.random.default_rng(0xA5E7)
Z = Vector((0, 0, 1))

# ------------------------------------------------------------------ the spine: tail tip to the base of the skull

CTRL = [(-12.5, 5.5, 0.35), (-10.5, 2.2, 0.5), (-7.5, -1.4, 0.85), (-4.2, -1.6, 1.35), (-0.4, 0.4, 1.8), (3.4, 1.1, 2.05),
        (6.0, 0.5, 3.1), (7.6, -0.2, 5.0), (9.0, -0.55, 6.25), (10.6, -0.5, 6.55)]
PATH = F.spline(CTRL, 7)
FR, LENGTH = F.frames(PATH)
TS = np.array([f[3] for f in FR])
# arc fraction -> half width, height above, depth below
T_KEY = [0.0, 0.08, 0.2, 0.33, 0.41, 0.5, 0.6, 0.66, 0.74, 0.86, 1.0]
W_KEY = [0.07, 0.26, 0.6, 1.05, 1.4, 1.55, 1.45, 1.1, 0.82, 0.72, 0.68]
HU_KEY = [0.09, 0.3, 0.6, 0.95, 1.2, 1.3, 1.25, 1.0, 0.8, 0.7, 0.62]
HD_KEY = [0.07, 0.24, 0.5, 0.85, 1.1, 1.25, 1.2, 0.95, 0.75, 0.66, 0.55]
T_HIP, T_SHOULDER = 0.41, 0.6


def at(t):
    """Point and frame on the spine at arc fraction t."""
    k = int(np.clip(np.searchsorted(TS, t), 1, len(TS) - 1))
    a = (t - TS[k - 1]) / max(TS[k] - TS[k - 1], 1e-9)
    p = PATH[k - 1].lerp(PATH[k], a)
    f, left, up, _ = FR[k]
    return p, f, left, up


F.reset()
parts = []
parts.append(F.body('trunk', [dict(c=PATH[k], w=float(np.interp(TS[k], T_KEY, W_KEY)), hu=float(np.interp(TS[k], T_KEY, HU_KEY)),
                                   hd=float(np.interp(TS[k], T_KEY, HD_KEY)), right=FR[k][1], up=FR[k][2]) for k in range(len(PATH))], 32))

# the skull, long and ancient, looking down and ahead; the jaw a little open
HB = PATH[-1]
HF = Vector((2.5, 0.25, -1.0)).normalized()
HL = Z.cross(HF).normalized()
HU = HF.cross(HL).normalized()
JAW = math.radians(11)
MF = (HF * math.cos(JAW) - HU * math.sin(JAW)).normalized()
MU = MF.cross(HL).normalized()
SKULL = [(-0.3, 0.66, 0.6, 0.3), (0.3, 0.66, 0.6, 0.2), (0.9, 0.6, 0.5, 0.16), (1.5, 0.46, 0.34, 0.13), (2.2, 0.37, 0.27, 0.11), (2.8, 0.33, 0.25, 0.1), (3.05, 0.2, 0.15, 0.07)]
MAND = [(-0.2, 0.6, 0.07, 0.36), (0.7, 0.52, 0.06, 0.28), (1.5, 0.4, 0.05, 0.19), (2.3, 0.31, 0.05, 0.14), (2.85, 0.19, 0.04, 0.08)]


def skull_p(s, side=0.0, up=0.0):
    return HB + HF * s + HL * side + HU * (0.12 + up)


def mand_p(s, side=0.0, up=0.0):
    return HB + HF * 0.1 + MF * s + HL * side + MU * (-0.16 + up)


parts.append(F.body('skull', [dict(c=skull_p(s), w=w, hu=hu, hd=hd, pd=5.0, right=HL, up=HU) for s, w, hu, hd in SKULL]))
parts.append(F.body('mandible', [dict(c=mand_p(s), w=w, hu=hu, hd=hd, pu=5.0, right=HL, up=MU) for s, w, hu, hd in MAND]))
parts.append(F.blob('snout', skull_p(2.85, 0, 0.2), (0.3, 0.3, 0.14)))
for k in range(7):                                                                     # the ridge of the nose, plate behind plate
    parts.append(F.blob('nasal', skull_p(1.25 + 0.22 * k, 0, 0.36 - 0.02 * k), (0.16, 0.1 - 0.005 * k, 0.07), rot=(0, -0.25, 0), seg=12))
for k in range(3):                                                                     # spines under the chin
    b = mand_p(0.9 + 0.5 * k, 0, -0.2)
    parts.append(F.cone('chin', b, b - MU * (0.34 - 0.06 * k) - HF * 0.2, 0.08, 8))
for s in (-1, 1):
    parts.append(F.blob('brow', skull_p(0.95, s * 0.5, 0.52), (0.5, 0.2, 0.13)))       # the deep sockets are made by what overhangs them
    parts.append(F.blob('cheek', skull_p(0.8, s * 0.6, 0.12), (0.45, 0.16, 0.2)))
    parts.append(F.blob('jowl', skull_p(0.1, s * 0.6, -0.1), (0.5, 0.22, 0.4)))
    for k in range(3):                                                                 # horned brows over the sockets
        b = skull_p(0.75 + 0.2 * k, s * (0.52 - 0.03 * k), 0.6 - 0.03 * k)
        parts.append(F.cone('browhorn', b, b + (-HF * 0.5 + HU * 0.32 + HL * s * 0.18) * (1.0 - 0.2 * k), 0.09, 8))
    parts.append(F.cone('cheekhorn', skull_p(0.55, s * 0.68, 0.08), skull_p(0.55, s * 0.68, 0.08) + (-HF * 1.0 + HL * s * 0.5 - HU * 0.1), 0.14, 10))
    parts.append(F.blob('nostril', skull_p(2.86, s * 0.15, 0.3), (0.13, 0.09, 0.07)))
    for k in range(4):                                                                 # a fringe of spines along the jaw's hinge
        b = skull_p(0.0 + 0.22 * k, s * 0.66, 0.15 - 0.1 * k)
        parts.append(F.cone('spine', b, b + (-HF * 0.9 + HL * s * 0.45 + HU * (0.25 - 0.1 * k)) * (0.9 - 0.12 * k), 0.12, 10))

# the ridge down her back
t = 0.05
while t < 0.99:
    p, f, left, up = at(t)
    hu = float(np.interp(t, T_KEY, HU_KEY))
    tall = 0.25 + 0.55 * math.sin(min(t / 0.5, 1.0) * math.pi / 2) * (1.0 if t < 0.7 else 1.0 - (t - 0.7) * 1.6)
    base = p + up * (hu - 0.06)
    parts.append(F.cone('ridge', base, base + up * tall - f * tall * 0.75, 0.16 + tall * 0.12, 10, 0.06, tuple(f)))
    t += 0.42 / LENGTH * (0.8 + 0.5 * tall)

# four legs under her, heavy at the shoulder and haunch
FEET = []
for s in (-1, 1):
    p, f, left, up = at(T_SHOULDER)
    sh = p + left * s * 1.25 - Z * 0.2
    front = [sh, sh + left * s * 1.0 - f * 0.55 + Z * (1.25 - sh.z), sh + left * s * 1.15 + f * 0.75 + Z * (0.45 - sh.z), sh + left * s * 1.2 + f * 1.25 + Z * (0.18 - sh.z)]
    parts.append(F.tube('foreleg', front, [0.6, 0.46, 0.3, 0.26]))
    parts.append(F.blob('shoulder', sh + Z * 0.25, (0.8, 0.65, 0.7)))
    p, f, left, up = at(T_HIP)
    hp = p + left * s * 1.2 - Z * 0.1
    hind = [hp, hp + left * s * 0.95 + f * 1.0 + Z * (1.45 - hp.z), hp + left * s * 1.15 - f * 0.5 + Z * (0.6 - hp.z), hp + left * s * 1.2 + f * 0.45 + Z * (0.18 - hp.z)]
    parts.append(F.tube('hindleg', hind, [0.75, 0.56, 0.34, 0.28]))
    parts.append(F.blob('haunch', hp + Z * 0.2, (1.0, 0.75, 0.85)))
    for leg, fwd in ((front, f), (hind, f)):
        for k in range(4):
            a = (-0.45 + 0.3 * k) * s
            d = (fwd * math.cos(a) + left * math.sin(a)).normalized()
            b = leg[-1]
            parts.append(F.tube('toe', [b, b + d * 0.45 + Z * 0.06, b + d * 0.8 - Z * 0.03], [0.15, 0.12, 0.09], 10))
            FEET.append((b + d * 0.75 - Z * 0.02, d))
    # the folded wing's arm: up and back from the shoulder, then forward again to a wrist high over her back
    p, f, left, up = at(T_SHOULDER - 0.02)
    w0 = p + left * s * 0.9 + up * 1.0
    w1 = w0 - f * 3.8 + left * s * 1.0 + Z * 1.9
    w2 = w1 + f * 4.4 + left * s * 0.35 + Z * 1.1
    parts.append(F.tube('wing_arm', [w0, w0.lerp(w1, 0.5) + Z * 0.15, w1, w1.lerp(w2, 0.5), w2], [0.42, 0.36, 0.3, 0.24, 0.2]))
    parts.append(F.cone('thumb', w2, w2 + f * 0.7 + Z * 0.45, 0.14, 8, 0.1, (0, 0, -1)))

skin = F.fuse('wyrm_skin', parts, 0.1 if QUICK else 0.055, smooth=4)
low = F.retopo(skin, 'SM_Boss_AshenWyrm', 30000 if QUICK else 90000)
high = F.subdivide(skin, 1 if QUICK else 2)
print('FORGE high verts', len(high.data.vertices), 'low tris', len(low.data.polygons), 'length', round(LENGTH + 3.05, 1))

# ------------------------------------------------------------------ sculpt: obsidian plates, molten cracks

p = F.co(high)
n = F.normals(high)
warp = p + (F.fbm(p * 0.6, 3)[:, None] - 0.5) * 0.5
dome_a, crack_a, id_a = F.cells(warp / 0.36, 0.1)       # the great plates
dome_b, crack_b, id_b = F.cells(warp / 0.15, 0.14)      # the scales between and on the limbs and face
under = F.smoothstep(0.2, 0.7, -n[:, 2])
fine = np.clip(F.smoothstep(0.45, 0.6, F.fbm(p * 0.9 + 5.0, 3)) + under * 0.6, 0, 1)
facet = (id_a - 0.5) * 0.02                              # each plate struck at its own level, like knapped glass
h = (dome_a * 0.035 + facet - crack_a * 0.05) * (1 - fine) + (dome_b * 0.016 - crack_b * 0.022) * fine
h += (F.fbm(p * 16, 3) - 0.5) * 0.008
crack = crack_a * (1 - fine) + crack_b * fine
F.push(high, h)
# where the inner fire seeps through: along the cracks, in veins that wander, strongest under the belly and at the throat
vein = F.smoothstep(0.6, 0.72, F.fbm(p * 0.45 + 11.0, 4))
fire = np.clip(crack ** 2 * np.clip(vein + under * 0.2, 0, 1), 0, 1)
for s in (-1, 1):                                                                      # the nostrils: black pits with a breath of fire in them
    pit = F.smoothstep(0.075, 0.03, np.linalg.norm(p - np.array(skull_p(2.93, s * 0.15, 0.3)), axis=1))
    fire = np.maximum(fire, pit * 0.35)
black = np.array([0.012, 0.012, 0.016])
col = np.tile(black, (len(p), 1)) * (0.6 + 0.9 * id_a)[:, None]
col = F.mix(col, np.array([0.035, 0.028, 0.04]), F.smoothstep(0.5, 0.8, F.fbm(p * 2.5, 3)) * 0.5)      # a violet sheen in the glass
col = F.mix(col, np.array([0.05, 0.045, 0.04]), under * 0.5)
col *= (1 - 0.6 * crack)[:, None]
col = F.mix(col, np.array([0.9, 0.4, 0.06]), fire)
ember = F.smoothstep(0.0, 0.5, dome_a) * 0                                                                   # (plates themselves stay cold)
F.set_attr(high, 'Col', col)
F.set_attr(high, 'Rough', np.repeat(np.clip(0.16 + 0.5 * crack + 0.25 * fine + 0.1 * (F.fbm(p * 9, 2) - 0.5), 0.06, 1)[:, None], 3, 1))
F.set_attr(high, 'Emit', np.outer(fire ** 1.5, np.array([1.0, 0.5, 0.09])))

maps = F.bake_maps(high, low, 1024 if QUICK else 4096, 0.1, 8 if QUICK else 32, OPT['tmp'])
F.assign(low, F.game_material('M_Boss_Wyrm_Scales', *maps[:3], emit=maps[3], emit_strength=3.0))
bpy.data.objects.remove(high)

# ------------------------------------------------------------------ horns, teeth, claws, eyes

extra = []


def paint_horn(bases, tips):
    def paint(q, nq):
        t = F.along_axes(q, bases, tips)
        rings = F.fbm(np.stack([t * 40, q[:, 0] * 0.5, q[:, 1] * 0.5], 1), 3)
        grain = F.fbm(q * np.array([9.0, 9.0, 30.0]), 3)
        c = F.mix(np.tile(np.array([0.02, 0.018, 0.02]), (len(q), 1)), np.array([0.1, 0.085, 0.07]), F.smoothstep(0.3, 1.0, t) * 0.8)
        c *= (0.6 + 0.8 * grain)[:, None]
        glow = F.smoothstep(0.1, 0.0, t) * F.smoothstep(0.6, 0.75, rings) * 0.6                             # fire in the roots
        c = F.mix(c, np.array([1.0, 0.5, 0.1]), glow)
        return (rings - 0.5) * 0.02 * (1 - t) + (grain - 0.5) * 0.006, c, 0.3 + 0.3 * grain, np.outer(glow, np.array([1.0, 0.45, 0.08]))
    return paint


horns, hb, ht = [], [], []
for s in (-1, 1):
    b = skull_p(0.2, s * 0.42, 0.55)
    pts = [b, b + (-HF * 1.3 + HU * 0.95 + HL * s * 0.35), b + (-HF * 2.9 + HU * 1.2 + HL * s * 0.6), b + (-HF * 4.0 + HU * 0.75 + HL * s * 0.5)]
    sp = F.spline(pts, 5)
    horns.append(F.tube('horn', sp, [0.27 * (1 - k / (len(sp) - 1)) ** 0.8 + 0.012 for k in range(len(sp))], 14))
    hb.append(b)
    ht.append(pts[-1])
    b2 = skull_p(0.55, s * 0.55, 0.5)                                                                        # a lesser pair beneath
    pts2 = [b2, b2 + (-HF * 0.8 + HU * 0.4 + HL * s * 0.45), b2 + (-HF * 1.7 + HU * 0.35 + HL * s * 0.7)]
    sp2 = F.spline(pts2, 5)
    horns.append(F.tube('horn', sp2, [0.15 * (1 - k / (len(sp2) - 1)) ** 0.8 + 0.01 for k in range(len(sp2))], 12))
    hb.append(b2)
    ht.append(pts2[-1])
extra.append(F.bake_part(F.join('horns', horns), 'M_Boss_Wyrm_Horn', paint_horn(hb, ht), 512 if QUICK else 2048, 2, 0.03, OPT['tmp'], emit_strength=1.0))

teeth, tb, tt = [], [], []
for s in (-1, 1):
    for kind, pf, table, updir, sign in (('u', skull_p, SKULL, HU, -1), ('l', mand_p, MAND, MU, 1)):
        x = 0.95
        k = 0
        while x < (2.95 if kind == 'u' else 2.8):
            w = float(np.interp(x, [r[0] for r in table], [r[1] for r in table]))
            size = (0.14 + 0.1 * math.sin(k * 1.7) ** 2) * (1.5 if k % 5 == 2 else 1.0)
            b = pf(x, s * (w - 0.05), -0.1 if kind == 'u' else 0.05)
            tip = b + updir * sign * size + HF * 0.03
            teeth.append(F.cone('tooth', b, tip, 0.035 + size * 0.14, 8, 0.02, tuple(-HF)))
            tb.append(b)
            tt.append(tip)
            x += 0.14 + 0.03 * math.sin(k * 2.3)
            k += 1


def paint_teeth(q, nq):
    t = F.along_axes(q, tb, tt)
    ridge = F.fbm(q * np.array([50.0, 50.0, 50.0]), 3)
    c = F.mix(np.tile(np.array([0.025, 0.015, 0.01]), (len(q), 1)), np.array([0.2, 0.15, 0.08]), F.smoothstep(0.0, 0.5, t))
    c = F.mix(c, np.array([0.36, 0.3, 0.2]), F.smoothstep(0.6, 1.0, t) * 0.7)
    c *= (0.6 + 0.7 * ridge)[:, None] * (0.7 + 0.5 * F.fbm(q * 7.0, 3))[:, None]
    return (ridge - 0.5) * 0.006, c, 0.55 - 0.2 * t


extra.append(F.bake_part(F.join('teeth', teeth), 'M_Boss_Wyrm_Teeth', paint_teeth, 512 if QUICK else 1024, 2, 0.012, OPT['tmp']))

claw_m = F.flat_material('M_Boss_Wyrm_Claw', (0.01, 0.01, 0.012), 0.08)
extra.append(F.assign(F.join('claws', [F.cone('claw', b, b + d * 0.5 - Z * 0.14, 0.09, 8, 0.1, (0, 0, 1)) for b, d in FEET]), claw_m))
EYES = [np.array(skull_p(0.98, s * 0.47, 0.36)) for s in (-1, 1)]


def paint_eyes(q, nq):
    """Twin suns: a corona of gold round a slit of night."""
    d = np.minimum(np.linalg.norm(q - EYES[0], axis=1), np.linalg.norm(q - EYES[1], axis=1))
    near = np.where((np.linalg.norm(q - EYES[0], axis=1) < np.linalg.norm(q - EYES[1], axis=1))[:, None], q - EYES[0], q - EYES[1])
    along = near @ np.array(HF)
    upw = near @ np.array(HU)
    r = np.sqrt(along ** 2 + upw ** 2) / 0.1
    ang = np.arctan2(upw, along)
    flare = F.fbm(np.stack([np.cos(ang) * 5, np.sin(ang) * 5, r * 3], 1), 4)
    c = F.mix(np.tile(np.array([1.0, 0.8, 0.35]), (len(q), 1)), np.array([0.9, 0.35, 0.04]), F.smoothstep(0.3, 1.0, r))
    c *= (0.6 + 0.8 * flare)[:, None]
    slit = F.smoothstep(0.16, 0.08, np.abs(along) / 0.1 / np.maximum(1 - (upw / 0.085) ** 2, 0.03) ** 0.5)
    c = F.mix(c, np.array([0.0, 0.0, 0.0]), slit)
    return None, c, np.full(len(q), 0.08), c * (1 - slit)[:, None]


eyes = F.join('eyes', [F.blob('eye', tuple(e), (0.13, 0.075, 0.09), seg=20) for e in EYES])
extra.append(F.bake_part(eyes, 'M_Boss_Wyrm_Eye', paint_eyes, 256 if QUICK else 512, 3, 0.006, OPT['tmp'], emit_strength=9.0))

# ------------------------------------------------------------------ the folded wings: fingers laid back along her flanks, leather between

bone_m = F.flat_material('M_Boss_Wyrm_WingBone', (0.012, 0.012, 0.015), 0.22)
bones, sheets = [], []
for s in (-1, 1):
    p0, f, left, up = at(T_SHOULDER - 0.02)
    w0 = p0 + left * s * 0.9 + up * 1.0
    w1 = w0 - f * 3.8 + left * s * 1.0 + Z * 1.9
    wrist = w1 + f * 4.4 + left * s * 0.35 + Z * 1.1
    tips = []
    for back, drop, out in ((15.5, 3.4, 0.6), (13.0, 4.6, 1.0), (9.5, 5.3, 1.2), (5.5, 5.4, 1.0)):
        tips.append(wrist - f * back - Z * drop + left * s * out)
    tips.append(w1 - Z * 2.2 + left * s * 0.5)                           # the last panel closes to the elbow and her flank
    for tip in tips[:4]:
        mid = wrist.lerp(tip, 0.5) + Z * 0.5 + left * s * 0.25
        sp = F.spline([wrist, mid, tip], 6)
        bones.append(F.tube('finger', sp, [0.17 * (1 - k / (len(sp) - 1)) ** 0.7 + 0.02 for k in range(len(sp))], 8))
    for k in range(4):
        a, b = tips[k], tips[k + 1]
        ma = wrist.lerp(a, 0.5) + Z * 0.5 + left * s * 0.25
        mb = wrist.lerp(b, 0.5) + Z * (0.5 if k < 3 else 0.0) + left * s * 0.25
        nu, nv = 7, 9
        verts, faces = [], []
        for i in range(nu + 1):
            u = i / nu
            for j in range(nv + 1):
                v = j / nv * (1.0 - 0.14 * math.sin(math.pi * u))            # the trailing edge scalloped between the fingers
                pa = (1 - v) ** 2 * wrist + 2 * (1 - v) * v * ma + v * v * a
                pb = (1 - v) ** 2 * wrist + 2 * (1 - v) * v * mb + v * v * b
                q = pa.lerp(pb, u)
                q = q - Z * (0.55 * math.sin(math.pi * u) * v) + left * s * (0.12 * math.sin(math.pi * u * 3) * v)   # hanging folds
                verts.append(q)
        for i in range(nu):
            for j in range(nv):
                c0 = i * (nv + 1) + j
                faces.append((c0, c0 + 1, c0 + nv + 2, c0 + nv + 1))
        sheets.append(F.new_obj('membrane', verts, faces))
extra.append(F.assign(F.join('wing_bones', bones), bone_m))


def paint_membrane(q, nq):
    fold = F.fbm(q * np.array([0.9, 0.9, 3.5]), 4)
    wrinkle = F.fbm(q * 7.0, 3)
    veins = F.smoothstep(0.012, 0.0, np.abs(F.fbm(q * 0.55 + 3.0, 3) - 0.5))            # gold veins branching through the leather
    veins = np.maximum(veins, F.smoothstep(0.008, 0.0, np.abs(F.fbm(q * 1.4 + 9.0, 3) - 0.5)) * 0.6)
    c = np.tile(np.array([0.016, 0.013, 0.013]), (len(q), 1)) * (0.6 + 0.9 * wrinkle)[:, None]
    c = F.mix(c, np.array([0.9, 0.5, 0.1]), veins * 0.9)
    return (fold - 0.5) * 0.12 + (wrinkle - 0.5) * 0.02, c, 0.55 + 0.2 * wrinkle - 0.2 * veins, np.outer(veins, np.array([1.0, 0.5, 0.09]))


extra.append(F.bake_part(F.join('wings', sheets), 'M_Boss_Wyrm_Wing', paint_membrane, 512 if QUICK else 2048, 3, 0.2, OPT['tmp'], emit_strength=1.2, two_sided=True))

boss = F.join('SM_Boss_AshenWyrm', [low] + extra)
size = F.export_gltf(OPT['out'], 'SM_Boss_AshenWyrm', [boss])
print('FORGE gltf bytes', size, 'tris', sum(len(q.vertices) - 2 for q in boss.data.polygons))

# ------------------------------------------------------------------ present (redo alone with present.py)

import present

present.run('AshenWyrm', OPT)
F.done()
