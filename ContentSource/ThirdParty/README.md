# Third-party source data

Two free datasets are the source for the people in Dark Arisen. The native art kit
(`Tools/o3de/Ops/ArtKitHumans*.cpp`) turns them into the game's cast: bodies, clothes, hair,
textures, poses and animation clips. Nothing here was made by a generative model, and nothing
was bought. `ThirdPartyManifest.json` records the SHA-256 hash of every file.

## MakeHuman (`MakeHuman/`)

- **Source:** https://github.com/makehumancommunity/makehuman, branch `master`, folder `makehuman/data/`. Fetched 2026-09-26.
- **Licence:** CC0 1.0 Universal. The full text is in `MakeHuman/LICENSE.ASSETS.md`, and each file carries its own CC0 header.
- **Copyright holders at the CC0 release:** Data Collection AB, Joel Palmius, Jonas Hauquier.
- **Files used:**
  - `3dobjs/base.obj`: the base mesh, including its helper geometry.
  - `rigs/default.mhskel` and `rigs/default_weights.mhw`: the default skeleton (163 bones) and its skin weights.
  - Targets:
    - the macro targets for gender, age, ethnicity, muscle and weight
    - ideal proportions
    - breast size
    - a selection of face and body detail targets
- **Modifications:** none. Every file is a byte-identical copy of upstream.

## CMU Graphics Lab Motion Capture Database (`CMU/`)

- **Source:** http://mocap.cs.cmu.edu, in the MotionBuilder-friendly BVH conversion by B. Hahne. Fetched from the mirror https://github.com/una-dinosauria/cmu-mocap (`data/`).
- **Licence** (see `CMU/READMEFIRST.txt`):
  - The motions may be used freely, including in commercial products.
  - The raw data itself may not be resold.
- **Required acknowledgement:** "The data used in this project was obtained from mocap.cs.cmu.edu. The database was created with funding from NSF EIA-0196217."
- **Modification:** each clip is an excerpt.
  - The header and frame 0 (the T-pose added by the conversion) are kept verbatim.
  - After that, only every 4th original frame is kept (frames 1, 5, 9, …), up to original frame 2400.
  - The `Frame Time` is therefore 1/30 s. Every kept frame line is verbatim.

| Clip | CMU description | Used for |
|---|---|---|
| 02_01 | walk | walk cycle |
| 09_01 | run | run cycle |
| 17_03 | walk stealthily | stealth walk |
| 77_02 | standing | idle stance |
| 77_01 | looking around | watchful idle |
| 40_10 | wait for bus | waiting idle |
| 18_08 | conversation, explain with hand gestures | talking |
| 18_10 | quarrel, angry hand gestures | arguing |
| 13_26 | direct traffic, wave | waving, pointing |
| 62_07 | hammering a nail | smith, carpenter |
| 13_23 | sweep floor | deck work |
| 70_03 | carry suitcase | carrying |
| 69_69 | walk, pick up and carry an object | loading cargo |
| 26_10 | bend, lift | lifting |
| 75_18 | high sit | sitting on a crate or rail |
| 82_05 | sitting on ground relaxing | resting |
| 13_04 | sit on stepstool, chin in hand | brooding |
| 14_30 | sit on stepstool, ankle on knee, hand on chin | sitting at ease |
| 02_07 | swordplay | saber stance and strikes |
| 15_13 | boxing | brawling |
| 13_33 | climb ladder | rigging and ladders |
| 13_09 | drink | drinking |
| 81_07 | pull heavy object | hauling lines |
| 91_09 | drunk walk | tavern drunk |
| 15_06 | lean forward, reach for | reaching |
