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
        stage=dict(ground='obsidian', fog=0.006, hdri_strength=0.0, tint=(0.5, 0.42, 0.36), relief=0.08),
        lights=[('vent', 'SPOT', (6, -2, 60), (7, 0, 2), 250000, (0.62, 0.72, 1.0), 22),
                ('rim', 'AREA', (-14, 16, 9), (2, 0, 3), 9000, (1.0, 0.55, 0.25), 14.0),
                ('face', 'AREA', (19, -9, 5), (12, -0.4, 5.5), 2500, (0.7, 0.8, 1.0), 6.0)],
        shots=[('', (23.0, -13.0, 2.2), (6.5, 0.0, 4.2), 30), ('_Head', (17.5, -4.6, 4.6), (12.0, -0.4, 5.6), 50), ('_Side', (0.0, -30.0, 6.0), (0.0, 0.0, 3.0), 28)],
        exposure=0.8, file='Ashen_Wyrm'),
}


def run(name, opt, load=False):
    pre = PRESETS[name]
    if load:
        F.reset()
        bpy.ops.import_scene.gltf(filepath=os.path.join(opt['out'], 'SM_Boss_' + name + '.glb'))
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
