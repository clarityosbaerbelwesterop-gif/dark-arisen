# Dark Arisen — O3DE Migration Record

**Branch:** `claude/dark-arisen-o3de-migration-002z1w` (serves as the `engine/o3de-cutover` line)
**Base:** `main` @ `7ca98058` + PR #52 (`98d745b7`) merged in
**Date:** 2026-09-23
**Status:** O3DE evaluation IN PROGRESS. Engine decision NOT made. Unreal stays the reference implementation.

Status words follow the Alpha brief: VERIFIED, IMPLEMENTED, IMPLEMENTED / RUNTIME VERIFY PENDING,
SOURCE CREATED, IMPORT READY, PARTIAL, BLOCKED, NOT DONE.

## 1. Engine pin

| Item | Value | Evidence |
|---|---|---|
| Repository | https://github.com/o3de/o3de | `git ls-remote --tags` |
| Release | `2605.0` (engine.json `version` 2.6.0) | tag exists upstream; CI re-checks it |
| Commit | `3db6943249d8bd7960b9ed7e9aee310b7668586e` | `Tools/o3de/O3DE_VERSION.lock` |
| License | Apache-2.0 OR MIT (at the user's option) | `LICENSE.txt` in the pinned checkout |
| Project pin | `"engine": "o3de==2.6.0"` | enforced by the engine's `o3deConfigVersion.cmake` |

## 2. What O3DE 2605.0 actually ships (read from the pinned source)

| Need | O3DE 2605.0 | Consequence |
|---|---|---|
| Large terrain | `Terrain` Gem (+ `GradientSignal`, `SurfaceData`) | usable; streaming/precision to be proven per region |
| Dense vegetation | `Vegetation` Gem | usable |
| Sky, clouds, atmosphere, fog | `SkyAtmosphere`, `Stars`, Atom deferred height fog (noise-textured) | no volumetric-cloud system found; storm skies need custom work |
| Global illumination | `DiffuseProbeGrid` | usable, hardware RT path |
| Hardware ray tracing | Atom `RayTracing`, `SpecularReflections` features | present; cost must be measured |
| **Ocean / water** | **no water or ocean Gem** | custom work required. Started: `Core::OceanSurface` (8 Gerstner waves from wind, Pierson-Moskowitz Hs, loop-free, invertible sampling, hull heave/pitch/roll), `OceanComponent`, ship riding the shared sea (VERIFIED on AzCore), and `GerstnerOcean.azsli` compiled by O3DE azslc 1.8.22 to HLSL and by glslang to SPIR-V. Still NOT DONE: Atom render pass/material, shore foam, underwater, reflections, visual acceptance on a GPU. |
| DLSS / FSR 2+ / XeSS | **none integrated** (only FidelityFX SPD downsampler) | any upscaler is a separate, legally checked integration |
| Characters/animation | `EMotionFX`, `MotionMatching`, `NvCloth`, `AtomTressFX` | usable |
| AI navigation | `RecastNavigation` | usable |
| Cinematics | `Maestro` (Track View) | usable for authored inserts |
| Physics | `PhysX5` | usable (character controller, triggers, scene queries) |
| glTF/GLB import | Asset Processor (Assimp) | Higgsfield GLB is a native input format |

## 3. Environment evidence (this cloud container)

`DarkArisenO3DE doctor` output, 2026-09-23:

- `o3de-package-server` **FAIL**: `https://d3t6xeg4fgfoum.cloudfront.net` is denied by the environment
  network policy. O3DE CMake configure downloads all third-party packages from this host
  (`cmake/3rdPartyPackages.cmake`), so **no O3DE configure or build is possible here**.
- `disk` **FAIL**: 29.5 GiB available; O3DE documents 100+ GB for source builds.
- `git-lfs` **FAIL** (installable).
- `vulkan` warn: no GPU; Editor and GameLauncher cannot render here even after a build.

Therefore ENGINE BUILD, EDITOR LAUNCH and GAME LAUNCHER are **BLOCKED in this environment**, not failed
by O3DE.

**Framework-level runtime evidence (VERIFIED, 2026-09-23):** `DarkArisenO3DE probe` builds O3DE 2605.0
**AzCore from the pinned engine sources** (Clang 18, O3DE's own Linux defines and flags) with the
dependencies pinned in `Tools/o3de/AzCoreProbe/DEPENDENCIES.lock`, and runs Dark Arisen's
`CampaignSystemComponent` inside a real `AZ::ComponentApplication`: `AZ::Interface` registration, EBus
notifications, O3DE `LocalFileIO`, Harlow raid to Driftwood Camp, chapter-boundary autosave on disk,
load/migrate/validate, corrupt-save rejection with untouched state, path-traversal rejection, manual save,
autosave suppression. Result: 23/23 checks passed; reproduced from an empty dependency folder.
A second probe runs the gameplay adapters on entities (`CombatantComponent`, `EnemyBrainComponent`,
`ShipVoyageComponent`) with O3DE's real AzFramework physics types, reflection and service resolution:
boarder telegraph and attack over the buses, parry/deflect, Rache driving `AZ::ITime` to 0.30 and back
within five real seconds, player capture/restore scoped to its level, real-Ethan hit rejection, ship
ownership gate and physical sailing on TickBus, the shared ocean (ship heave and tilt), and Chapter 1
swimming: `SwimmerComponent` and `WaterVolumeComponent` with the ContentSource Outer Reef Current.A,
PhysX character gravity suspended in water and restored on the shallow walk-out, buoyancy on the sampled
sea, casual stroke losing and sprint beating the current, exhaustion, 30 s breath and 20 drowning damage
per whole second, breath refill on surfacing. Result: 35/35 passed (2026-09-25). The remaining Gem
sources (`JakeInputComponent`, `StoryTriggerComponent`, the module) are compiled against the pinned
engine headers in the same run, so every adapter source is compiler-checked on each PR.

**Materialised Chapter 1 levels (VERIFIED on AzCore, 2026-09-25):** `DarkArisenO3DE materialize` turns
ContentSource into `Levels/L_HarlowOpening`, `L_DriftwoodBeach` and `L_DriftwoodCamp` prefabs plus
O3DE-frame greybox meshes (20 files, byte-identical from GCC and Clang builds, `--check` in CI). A third
probe loads those prefabs, deserialises every Dark Arisen game component with O3DE's JSON serializer
(unknown or mistyped fields fail; entity references resolved like prefab aliases) and plays the chapter
on the real adapters: family talk, cargo manifest, black sails, three boarders released and defeated,
Draven, The Taking (Marc and Denise fall, Ethan seized alive), map transition, arrival in the water,
Outer Reef swim, walk-out at wading depth, beach, camp smoke (chapter-boundary autosave on disk) and
travel to Driftwood Camp, plus two deaths: drowning in Undertow returns Jake to the overboard arrival,
swimming again with full vitals; dying after the recovery returns him to the beach checkpoint. Result: 41/41. Stand-ins, stated in the probe: transforms, a character
controller that integrates velocity over a beach ground profile, point-in-box triggers on the
materialised trigger colliders, the game entity context and the `LoadLevel` console command.
Editor loading, Asset Processor output and PhysX behaviour of these prefabs are RUNTIME VERIFY PENDING.

**Atom shader path (VERIFIED at compile level, 2026-09-25):** `DarkArisenO3DE shader-check` runs the
Asset Processor's shader chain without the Asset Processor: preprocess with the engine's Atom shader
include roots, AZSL -> HLSL with O3DE's azslc 1.8.22, then HLSL -> SPIR-V (Vulkan) and DXIL (DX12)
with DXC 1.8.2505.1, the compiler release O3DE 2605.0 pins (downloaded from Microsoft's release,
SHA-256 pinned in `Tools/o3de/ShaderCheck/ARCHIVES.lock`). O3DE's own `Unlit.azsl` is compiled first
as the calibration reference; then the ocean forward pass (VS/PS) and depth pass (VS). 6 entry points
compile. Rendering on a GPU is RUNTIME VERIFY PENDING. No PhysX scene exists in the probe,
so the melee sweep logs its documented error and hits are resolved directly. The CI job
`verify-o3de-framework` repeats it on every PR. Findings on the way: O3DE needs its patched RapidXML
(`isError/getError`), a RapidJSON newer than the 3p recipe commit, and `O3DE_DISABLE_CONDITIONAL_EXPLICIT`
for Clang 18 (O3DE's CMake sets the latter itself). Required build host: 100+ GB free, 8+ cores (2 GB RAM per build thread), 32 GB RAM,
DX12/Vulkan GPU, network access to the host above, GitHub and git-lfs.

## 4. Architecture decision

```
ContentSource (engine-neutral data)          Core/ (C++20, no engine)
        │                                           │  CampaignRuntime, SaveRules, SaveCodec,
        │                                           │  OpeningRuntime, Combat, ShipVoyage,
        │                                           │  EnemyBrain, Ocean, Water, EntityPolicy
        ▼                                           ▼
Tools/o3de (native ops + asset ingest)   Engine/O3DE/DarkArisen/Gem  ── thin AZ::Component adapters
        │                                           │  CampaignSystemComponent, SaveSlotStore,
        ▼                                           │  CombatantComponent, ShipVoyageComponent,
Engine/O3DE/DarkArisen/Assets ──► Asset Processor   │  StoryTriggerComponent, JakeInputComponent
                                                    ▼
                                             O3DE 2605.0 (Atom, PhysX5, EMotionFX)
```

- Gameplay authority lives in `Core/`. It compiles with GCC 13, Clang 18 (`-Werror`) and is built on
  GitHub-hosted Ubuntu and Windows runners by the `verify-engine-neutral` CI job.
- No fake Unreal types. Adapters are real `AZ::Component`s with `AZ_COMPONENT_DECL/IMPL`, EBus/`AZ::Interface`.
- The campaign is a **system component**: it survives level loads, so state captured before travel is
  in memory before the destination's entities activate (PR #52 ordering). Entities read their state in
  `Activate()`; map-local transforms apply only when the saved source level matches.
- Native C++ policy: Python/shell/PowerShell are replaced by `DarkArisenO3DE` (C++).

## 5. System audit matrix

| System | UE source | Class | O3DE status |
|---|---|---|---|
| 34-mission catalog | `Story/MainStoryMissionCatalog.cpp` | engine-independent | IMPLEMENTED in Core, parity-tested against UE source |
| Story authority, facts, crew, bosses, checkpoints | `Story/MainStorySubsystem*.cpp` | engine-independent logic, UE-coupled shell | IMPLEMENTED (Core); campaign adapter VERIFIED on O3DE AzCore runtime (probe) |
| Save schema v9, migration, validation (PR #52) | `Persistence/DarkArisenSaveGame.h`, `ValidateState` | engine-independent rules | IMPLEMENTED + checksummed codec; quest journal, economy, living-NPC snapshots NOT DONE |
| Cross-map persistence (PR #52) | `CaptureWorldState/RestoreWorldState` | UE-coupled | capture/restore notifications VERIFIED on AzCore; level loading via Editor/launcher RUNTIME VERIFY PENDING |
| Opening Ch. 1–2 runtime | `Opening/OpeningRuntimeComponent.cpp` | engine-independent logic | IMPLEMENTED (Core), triggers IMPLEMENTED / RUNTIME VERIFY PENDING |
| Combat, stamina, health/rally, damage | `Components/*`, `Combat/DamagePipeline.cpp` | engine-independent math | IMPLEMENTED (Core), parity-tested |
| Melee sweep, targeting | `CombatComponent::TraceAndResolvePendingHit` | UE-coupled | IMPLEMENTED (PhysX sphere cast) / PhysX RUNTIME VERIFY PENDING; combat adapter VERIFIED on AzCore |
| Jake movement/input | `JakeCharacter.cpp` | UE-coupled | PARTIAL: input, run/sprint, combat, surface swimming; camera, lock-on, diving NOT DONE |
| Visible ocean (Atom) | none in Unreal source (no water system shipped in O3DE 2605.0 either) | rendering | IMPLEMENTED: `DarkArisenOcean.materialtype`, forward + depth shaders displacing tiled grids with the gameplay Gerstner waves on `SceneSrg::m_time` (the clock `OceanComponent` now reads); compiles to SPIR-V and DXIL; material waves = live `OceanComponent` surface (level probe); foam, refraction, underwater fog and runtime weather changes on the material NOT DONE; GPU RUNTIME VERIFY PENDING |
| Swimming, currents, breath, drowning | `Components/WaterBreathComponent.cpp`, `Opening/OpeningWaterCurrentVolume.cpp` | engine-independent rules, UE-coupled movement | IMPLEMENTED (Core `BreathModel`, `SwimModel`, design speeds 1/1.5/2 m/s); adapters VERIFIED on AzCore; PhysX trigger volumes RUNTIME VERIFY PENDING; swim stamina values PROVISIONAL (section 6, item 9) |
| Player death and checkpoint respawn | `AJakeCharacter::RestoreAtCheckpoint` (only reachable through an unplaced trigger) | UE-coupled | IMPLEMENTED: 2 s death beat, checkpoint or legal level spawn, full vitals, back in the water when placed inside a volume, no failure screen; VERIFIED on AzCore (level probe) |
| Enemy AI (boarders, Holders, Dream Ethan, Draven) | `AI/*`, `Bosses/*`, `DuelingEnemyCharacter` | engine-independent decisions | IMPLEMENTED (Core `EnemyBrain`, phases, telegraphs, boss defeat to story); adapter VERIFIED on AzCore; PhysX movement/line of sight RUNTIME VERIFY PENDING; Draven still lacks distinct moves beyond phases (gap carried over from UE) |
| La Liberación voyage | `Ship/ShipVoyageComponent.cpp` | engine-independent model | IMPLEMENTED (Core); adapter VERIFIED on AzCore (ownership gate, sailing); buoyancy/collision sweep NOT DONE |
| Ethan canon guards | scattered validators | engine-independent | IMPLEMENTED: real Ethan can never be hostile, damaged or a boss; `boss.dream_ethan` is separate |
| Chapter 1 levels (Harlow, Driftwood Beach with Outer Reef, Driftwood Camp) | `DarkArisenMaterializeAlphaCommandlet.cpp` | content build | IMPLEMENTED (`materialize`): prefabs, O3DE-frame greybox, PhysX collision manifests, opening director, map transitions, spawn rules; VERIFIED on AzCore (level probe 38/38); Editor/Launcher RUNTIME VERIFY PENDING |
| Opening presentation (black sails, Draven, The Taking) | never wired in Unreal | presentation | IMPLEMENTED as timed beats + subtitles bus (Core `OpeningDirector`); cameras, animation and the not-authored `Cinematic.Opening.JakeOverboard` NOT DONE |
| Chapter 3–10 physical contracts | `ContentSource/Story/Chapter03..10` (27 JSON) | CONTENT SOURCE | SOURCE CREATED; O3DE prefab materialiser for them NOT DONE |
| Credits | `ContentSource/Story/Credits/CreditsAuthority.json` | CONTENT SOURCE | SOURCE CREATED; O3DE UI NOT DONE |
| Higgsfield GLB ingest | `Tools/higgsfield/import_3d_jutsu_glb.py` | tooling | IMPLEMENTED natively (`import-glb`), IMPORT READY once the GLBs are supplied |
| Pixel Streaming | `DarkArisenOps/StreamingOps.cpp` | UE-only | replaced by plan in section 8 |
| Colonial War, economy, living NPCs, dungeons, Highmoore | many UE subsystems | mixed | NOT DONE |

## 6. Defects found while porting

1. **Deflection window one frame too long (UE reference).** Six ticks of `1/60 s` leave ~1e-9 s of the
   `0.1 s` window, holding it open for a seventh frame; the same applies to the 8-frame dodge i-frames.
   Core timers now expire within 0.1 ms (tests: exactly 6 and 8 frames). The UE code has the same float
   accumulation and should receive the same fix.
2. **Authority facts never set.** 11 Chapter 1–2 facts required by `MAIN_STORY_AUTHORITY_2026_09.md`
   section 4 (`Story.FamilyIntact`, `Story.EthanPresent`, `Story.DravenIdentified`, `Story.JakeOverboard`,
   `Story.JakeSurvived`, `Story.EthanAliveLead`, `Chapter.01.Complete`, `Crew.*Known`,
   `Crew.CoreThreeJoined`, `Chapter.02.Complete`, `Travel.FreeSailingSeeded`) were never set by the UE
   runtime. Core sets them. Naming gap kept for save compatibility: authority `Draven.Killed/Captured`
   vs runtime `Story.DravenKilled/Captured`.
3. **Moran softlock.** UE allowed walking from Driftwood Beach inland before the beach recovery; recovery
   then required being on the beach and could never complete. Core requires recovery first.
4. **Double posture penalty on deflect (UE reference, kept for parity).** A deflected light hit costs the
   attacker `1.5 × 18` (pipeline) `+ 32` (hit profile) = 59 posture. Needs a design ruling.
5. **Minimap conflict.** `Docs/DesignAuthority.md` (owner override) requires an Alpha map/minimap;
   `MORAN_OPENING_WORLD_AUTHORITY` and the current brief forbid a minimap. Needs an owner ruling;
   nothing was changed.
6. **Policy drift.** `Tools/validation/*.py`, `Tools/content/*.py`, `Tools/higgsfield/*.py|.sh` violate
   `NATIVE_CPP_POLICY.md` (the UE `ValidationOps` rejects them). New tooling is C++.
7. **Higgsfield sources are not in the repository.** Only manifests (project id, revision, size, ETag)
   are committed; the `.gltf` files in `ContentSource/Ships` are greybox placeholders without normals/UV.
   The exact GLB revisions must be downloaded from the owner's Higgsfield account, then:
   `DarkArisenO3DE import-glb --manifest=ContentSource/Ships/Harlow/Higgsfield3DSource.json --glb=<file>`.
8. **Drowning over-counted on the breath-out tick (UE reference).** `UWaterBreathComponent` adds the whole
   tick to drowning time on the tick breath reaches zero, so the first damage arrives up to one tick early
   and depends on frame rate. Core counts only the overflow (test: identical damage at 30/60/144 Hz).
9. **Swimming had no design values in UE.** `JakeCharacter` never sets `MaxSwimSpeed`, so UE swam at the
    engine default 3 m/s (design: 1 / 1.5 / 2 m/s) with no stamina cost, and every `bShallowExit` volume
    switched to walking on any exit, including seaward into deep water. The port uses the design speeds,
    walks out only at wading depth, and marks the unspecified numbers PROVISIONAL for a design ruling:
    water drag 1/s (Current.A 130 cm/s² therefore drifts 1.3 m/s), active stroke 3/s, swim sprint 15/s
    (land sprint 10/s), 4/s per m/s of opposing current, in-water regen ×0.25, exhaustion until 25 %.
10. **Chapter 1 could not be played through in the Unreal maps.** The materialiser placed no trigger
    for the fleet, nothing called `StartEncounter` or `SignalDravenArrived`, nothing listened to
    `OnCinematicRequested`, no map transition left Harlow, Driftwood had no beach, recovery or camp
    beats, and Jake spawned on the beach although Undertow starts in the water. The O3DE build wires
    each step (cargo manifest per the family manifest's gate, `OpeningDirector`, `MapTransitionComponent`,
    `PlayerSpawnComponent` with an overboard arrival) and the level probe plays it end to end.
11. **Unreal ignored the authored Outer Reef data.** `BuildDriftwood` hard-codes currents and extents
    (130/20, 80/-35, 45/0 cm/s²) that differ from `OuterReef_GameplayLayout.json` (130/15, 90/30, 45/0);
    the O3DE materialiser uses the layout.
12. **Greybox meshes are Z-up inside glTF.** The ContentSource greybox glTF files carry Unreal-frame,
    Z-up metres, while glTF is Y-up and the project imports glTF with the root transform. The
    materialiser rewrites them (O3DE `(x, -y, z)`, winding reversed): forward and up are kept and
    port stays port (Harlow boarders on the left of the bow in the probe).
13. **PROVISIONAL authoring defaults** (no size in ContentSource): beach, smoke and camp-route beat
    volumes 6 × 6 × 4 m; family members use a generated placeholder figure and the cargo manifest a
    placeholder crate (no greybox exists); Jake and the boarders use static bind poses of their skinned
    greyboxes; the follow camera is a child at the Unreal boom length (camera system NOT DONE).
14. **Death was a dead end in Unreal.** `OnCharacterDied` disables input and movement; recovery exists
    only as an `OpeningEventTrigger` action that no map places. O3DE: `PlayerSpawnComponent` respawns
    after `DesignLaws::DeathToRespawnSeconds` (traversal.md 7.4) at the checkpoint, never shortcutting
    the swim (Outer Reef still counts as the sea), and `SwimmerComponent::RefreshWaterVolumes` puts a
    body placed inside water back into swimming, since trigger events only report crossings.
15. **Windows.** The first GitHub run on Windows (manual dispatch, 2026-09-25) found the tool tests
    deleting a directory with a file still open; fixed, and the materialiser closes files before writing.
16. **CI.** PR #52 made the UE jobs unconditional on self-hosted `ue5.8` runners that do not exist, so
   those jobs can only queue or be cancelled. The new `verify-engine-neutral` job gives real, executed
   C++ evidence on every PR.

## 7. Gates before an engine decision

O3DE passes only with evidence on the pinned build host:

1. `DarkArisenO3DE doctor` all ok → `bootstrap` → `configure` → `build` (Editor, AssetProcessor,
   DarkArisen.GameLauncher) → Editor and launcher start.
2. Gem links and `CampaignSystemComponent` activates; save/load round trip on disk; travel between two
   levels keeps vitals and never moves the ship across maps.
3. Harlow-to-Driftwood slice: imported Harlow GLB, Jake with PhysX character controller, two boarders,
   deflect/stagger, The Taking, overboard swim, Outer Reef, Driftwood Beach, Driftwood Camp.
4. Custom ocean prototype that is not a flat plane; 1080p/60 fps on the target GPU; crash-free 30 min.
5. Side-by-side with the Unreal reference (visuals, fps, memory, build and import times).

A real fundamental blocker (renderer, large world, asset pipeline, platform, stability) triggers the
Wicked Engine fallback evaluation, with the evidence recorded here.

## 8. Remote iPad testing (no purchase)

Recommended path: **Sunshine** (open-source game-stream host, NVENC/AMF/QSV) on the existing Windows GPU
machine + **Moonlight** (free iPad client) over the existing **Tailscale** tailnet (the repo already uses
a private tailnet for streaming). No cloud GPU, no inbound port exposure. Pixel Streaming remains the
Unreal-only path until the engine decision.

## 9. Commands

```
cmake -S Tools/o3de -B Build/ops && cmake --build Build/ops
Build/ops/DarkArisenO3DE doctor                 # prerequisites, package host, disk
Build/ops/DarkArisenO3DE bootstrap              # clone + verify pinned SHA + LFS + register engine
Build/ops/DarkArisenO3DE configure && Build/ops/DarkArisenO3DE build
Build/ops/DarkArisenO3DE test                   # Core tests always; O3DE tests when configured
Build/ops/DarkArisenO3DE import-glb --manifest=... --glb=...
Build/ops/DarkArisenO3DE probe                  # AzCore from pinned sources + campaign/gameplay/level runtime probes
Build/ops/DarkArisenO3DE materialize [--check]  # ContentSource -> Chapter 1 level prefabs + O3DE-frame greybox
Build/ops/DarkArisenO3DE shader-check --engine=PATH  # azslc 1.8.22 + DXC 1.8.2505.1: AZSL -> HLSL -> SPIR-V + DXIL
```
`O3DE_ENGINE_ROOT` overrides the engine location (`/opt/dark-arisen/o3de`, `C:\DarkArisenEngine\o3de`).
