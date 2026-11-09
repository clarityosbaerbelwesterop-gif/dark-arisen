# Third-party source data

Free, openly licensed datasets are the source for the people and the surfaces of Dark Arisen. The native art kit
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
    - the 34 expression units per ethnicity (`targets/expression/units/{african,asian,caucasian}/`), fetched 2026-11-08: every actor carries them as morph targets, visemes and emotions are weighted sets of them (`ContentSource/Animations/FaceShapes.json`)
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

Added 2026-11-08 for the gameplay motion library (`ContentSource/Animations/HumanMotionLibrary.json`); same excerpt rule.

| Clip | CMU description | Used for (library state) |
|---|---|---|
| 02_08 | swordplay | Jake CombatIdle (upper body), Jake CombatForward (upper body), Jake CombatBack (upper body), Jake CombatStrafeRight (upper body), Jake CombatStrafeLeft (upper body), Jake LightAttack1, Jake LightAttack2, Jake HeavyAttack |
| 02_09 | swordplay | Boarder HeavyAttack |
| 13_11 | forward jump | World JumpForward |
| 13_35 | climb 3 steps | Jake WaterExit, World ClimbSteps |
| 16_01 | jump | World Jump |
| 22_12 | A stumbles into B (2 subjects - subject A) | Jake Stagger |
| 75_01 | run jump | World RunJump |
| 79_96 | shooting a gun | World AimPistol |
| 81_12 | jump backwards off ledge | — |
| 82_02 | Jump off ledge | World FallLand |
| 90_16 | fall on face | — |
| 90_18 | RugPullFall | Jake Death, Boarder Death |
| 91_59 | 360 smallStumble | Jake HitReactionFront, Boarder HitReactionFront |
| 104_13 | StumbleWalk | Boarder Stagger |
| 111_02 | Bow | World Bow |
| 111_03 | Crawling | Jake BeachCrawl |
| 111_12 | Lay down | World LieDown |
| 111_17 | Pick up | Jake Pickup |
| 111_26 | Walk sideways | Jake CombatStrafeRight, Jake CombatStrafeLeft |
| 111_33 | Throwing | World Throw |
| 125_01 | Breast Stroke | Jake Swim |
| 125_05 | Butterfly | Jake StrugglingSwim |
| 125_06 | Free Style | — |
| 126_01 | Back Stroke | World SwimBackstroke |
| 127_03 | Run | — |
| 127_15 | Run Turn Left | Jake TurnLeft, Jake TurnRight |
| 127_16 | Run Turn Right | — |
| 127_23 | Run Dive Over Roll Run | Jake DodgeForward |
| 128_09 | Run Roll Underneath | — |
| 136_09 | Walk Crouched | World CrouchWalk |
| 137_33 | Old Man Walk | World OldWalk |
| 138_01 | Marching | World March |
| 139_01 | Looking Around | World LookAround |
| 139_05 | Pulling a Gun | World DrawPistol |
| 139_06 | Poking Around On The Ground | World SearchGround |
| 139_09 | Ducking | World Duck |
| 139_16 | Get Up From Ground | World GetUpFromFront |
| 139_19 | Walk Wounded Leg | World WoundedWalk |
| 139_25 | Giving Directions | World GiveDirections |
| 139_29 | Sneaking | World Sneak |
| 140_01 | Get Up Face Down | Jake BeachRecover |
| 140_05 | Motorcycle | Jake HelmIdle (upper body) |
| 140_06 | Idle | Jake Idle, Jake CombatIdle, Jake HelmIdle, Boarder Idle, Boarder CombatIdle |
| 140_08 | Get Up From Ground Laying on Back | World GetUpFromBack |
| 141_05 | Jump Sideways | — |
| 141_07 | Walk Up and Over | World ClimbStep |
| 141_16 | Wave Hello | World Wave |
| 141_21 | Shrug | World Shrug |
| 141_23 | Shake Hands | Jake InteractReach, World ShakeHands |
| 141_31 | Walk Backwards | Jake CombatBack, Jake Backstep, World WalkBackwards |
| 141_32 | Walk Sideways, Cross Legs | — |
| 143_01 | Run | — |
| 143_02 | Run to Stop | Jake Stop |
| 143_03 | Start to Run | — |
| 143_07 | Jumping Sideways | Jake DodgeRight, Jake DodgeLeft |
| 143_11 | Walk And Pick up Box | World PickUpBox |
| 143_18 | Sit Down And Get Up | World SitDown, World StandUp |
| 143_23 | Punching | World Punch |
| 143_24 | Kicking | World Kick |
| 15_06 | lean forward, reach for | reaching |

## Poly Haven (`PolyHaven/Scanned.lock`)

- **Source:** https://polyhaven.com. Fetched 2026-11-08 through the public API (`api.polyhaven.com/files/<asset>`), with downloads from `dl.polyhaven.org`.
- **Licence:** CC0 1.0 Universal (https://polyhaven.com/license). No attribution is required; the credits name Poly Haven and its scan authors anyway.
- **Files:** 15 scanned material sets at 2k: albedo, OpenGL normal, and ARM (occlusion, roughness, metal):

  | Surfaces | Sets |
  |---|---|
  | Wood | `weathered_planks`, `brown_planks_03`, `rough_wood`, `bark_brown_02` |
  | Building | `painted_plaster_wall`, `clay_roof_tiles_02`, `rustic_stone_wall_02`, `cobblestone_floor_08`, `rusty_metal_02` |
  | Ground | `sand_01`, `sparse_grass`, `forrest_ground_01`, `brown_mud_02` |
  | Rock | `rock_face_03`, `rock_pitted_mossy` |

  There is also one sky: `kloofendal_48d_partly_cloudy_puresky` (4k HDRI).
- **Not committed (about 144 MB).** `DarkArisenO3DE fetch-art` downloads every file into `Engine/O3DE/DarkArisen/Assets/Art/Scanned/` and checks it against the SHA-256 in the lock. The kit's materials reference them by path. Each tile size is the scan's real width, so the texel density matches the world.
- **Modifications:** none.
