"""Arena shots of a forged boss, from its GLB alone (so a render can be redone without rebuilding the boss).

  <python with bpy> present.py -- <Name> [--quick] [--out DIR] [--shots DIR]
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import bpy  # first: the bpy wheel provides mathutils

import forge as F

# stage, lights (name, kind, location, target, energy, colour, size), shots (file suffix, eye, target, lens), exposure
PRESETS = {
    'JawOfTheMire': dict(
        stage=dict(ground='mud', fog=0.004, hdri_strength=0.22, tint=(0.42, 0.5, 0.46)),
        lights=[('rim', 'SUN', (-30, 24, 14), (0, 0, 1), 4.0, (1.0, 0.8, 0.58), 2.0),
                ('fill', 'AREA', (12, -9, 5), (5, 0, 1), 350, (0.6, 0.75, 1.0), 9.0),
                ('eye', 'SPOT', (9.5, -5.5, 2.4), (5.4, -0.6, 1.5), 900, (1.0, 0.9, 0.75), 18)],
        shots=[('', (11.6, -5.2, 1.25), (5.2, 0.1, 1.15), 32), ('_Side', (1.5, -15.5, 3.2), (0.5, 0, 0.9), 35), ('_Head', (8.3, -2.6, 1.9), (5.9, -0.3, 1.25), 50)],
        exposure=0.0, file='Jaw_of_the_Mire'),
    'AshenWyrm': dict(
        stage=dict(ground='obsidian', fog=0.003, hdri_strength=0.0, tint=(0.5, 0.42, 0.36), relief=0.08),
        lights=[('vent', 'SPOT', (6, -2, 60), (7, 0, 2), 120000, (0.62, 0.72, 1.0), 20),
                ('rim', 'AREA', (-14, 16, 9), (2, 0, 3), 5000, (1.0, 0.55, 0.25), 14.0),
                ('face', 'AREA', (19, -9, 5), (12, -0.4, 5.5), 1800, (0.7, 0.8, 1.0), 6.0)],
        shots=[('', (23.0, -13.0, 2.2), (6.5, 0.0, 4.2), 30), ('_Head', (17.5, -4.6, 4.6), (12.0, -0.4, 5.6), 50), ('_Side', (0.0, -30.0, 6.0), (0.0, 0.0, 3.0), 28)],
        exposure=0.3, file='Ashen_Wyrm'),
    # the Inner Sanctum (jungle_warden.md 3): cracked polished stone, vine-hung pillars, shafts of gold through the broken roof
    'JungleWarden': dict(
        stage=dict(ground='rock', fog=0.02, hdri_strength=0.03, tint=(0.6, 0.62, 0.45), relief=0.06, columns=(9, 9.5, 0.7, 11.0)),
        lights=[('shaft', 'SPOT', (-5, 7, 22), (0.5, -0.5, 1.5), 90000, (1.0, 0.86, 0.55), 16),
                ('shaft2', 'SPOT', (6, -9, 24), (4, -4, 0), 40000, (1.0, 0.86, 0.55), 9),
                ('fill', 'AREA', (7, -4, 3), (0, 0, 2.5), 260, (0.55, 0.75, 0.7), 6.0)],
        shots=[('', (6.8, -3.4, 1.5), (0.1, 0.1, 2.5), 32), ('_Mask', (2.6, -0.9, 4.0), (0.5, 0.0, 3.85), 50), ('_Back', (-6.0, 4.0, 2.6), (0, 0, 2.4), 35)],
        exposure=0.4, file='Jungle_Warden'),
}


def run(name, opt, load=False):
    pre = PRESETS[name]
    if load:
        F.reset()
        base = os.path.join(opt['out'], 'SM_Boss_' + name)
        path = os.path.join(base, 'SM_Boss_' + name + '.gltf')
        bpy.ops.import_scene.gltf(filepath=path if os.path.exists(path) else base + '.glb')
    F.stage(**pre['stage'])
    for args in pre['lights']:
        F.light(*args)
    quick = opt['quick']
    for suffix, eye, target, lens in pre['shots'][:1 if quick else None]:
        out = os.path.join(opt['shots'], pre['file'] + suffix + '.png')
        if opt.get('only') and suffix not in opt['only']:
            continue
        F.shoot(out, eye, target, lens, (960, 540) if quick else (1920, 1080), 24 if quick else 96, exposure=pre['exposure'])


if __name__ == '__main__':
    a = sys.argv[sys.argv.index('--') + 1:]
    run(a[0], F.args(), load=True)
    F.done()
