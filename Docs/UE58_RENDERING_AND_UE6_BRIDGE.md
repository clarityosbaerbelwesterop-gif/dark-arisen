# Dark Arisen — UE 5.8 Rendering Baseline and UE6 Bridge

**Authority date:** 2026-09-10
**Branch:** `feature/story-ue58-rendering-preview`
**Runtime target:** Windows + Linux Alpha, plus Xbox Series X|S only when legitimate Microsoft GDKX console tooling is available; 60 fps gameplay floor.
**Cinematic target:** optional path-traced capture/preview on supported hardware.

## Decision

Dark Arisen migrates from Unreal Engine 5.5 to **Unreal Engine 5.8** now.

Do **not** write `EngineAssociation: 6.0` until Epic ships a public production Unreal Engine 6 release compatible with the project. Epic describes 5.8 as the last planned major UE5 release while UE6 is being built. The project therefore treats 5.8 as the production bridge to UE6 rather than pretending a non-public production engine is installed.

## Rendering architecture

### Gameplay path — hard 60 fps floor

- Deferred renderer.
- Nanite enabled.
- Virtual Shadow Maps enabled.
- Lumen GI and reflections enabled.
- Hardware ray tracing support enabled and used when available.
- Compute Skin Cache compiled because hardware RT/path tracing require it.
- Shader Model 6 / DirectX 12 on Windows.
- Dynamic ray-tracing capability is compiled so quality settings may disable it on unsupported/slower machines.
- Path Tracing is **not** the normal gameplay renderer.

### Cinematic / preview path

- UE 5.8 Path Tracer support is compiled into the project.
- Path Tracing is used for Sequencer/Movie Render Queue reference frames, hero shots and visual ground truth where supported.
- Runtime path tracing may be exposed only as an explicit high-end preview/photo/cinematic mode after GPU-budget acceptance.
- A path-traced frame does not prove the normal gameplay path meets 60 fps.

This split preserves the design law: visual ambition may not silently replace the gameplay frame-rate floor.

## DLSS integration policy

### What can ship immediately

NVIDIA's public developer download path currently provides the official **DLSS 4.5 Unreal Engine plugin for UE 5.8**, including Super Resolution, DLAA, Ray Reconstruction, Reflex and Multi Frame Generation features. The plugin may be integrated once the official package is supplied to the build environment and its redistribution/licensing terms are accepted.

### DLSS 5

DLSS 5 / 3D-Guided Neural Rendering is a real released NVIDIA technology as of September 2026. However, the public NVIDIA Unreal Engine download page still exposes the DLSS 4.5 UE 5.8 package rather than a clearly downloadable DLSS 5 UE 5.8 plugin package.

Therefore the repository uses a **fail-closed DLSS 5 gate**:

1. accept only an official NVIDIA DLSS 5 Unreal plugin/SDK or official Streamline integration path;
2. record package version, supported UE build and checksum;
3. add plugin binaries/source under the licensed integration location or provision them on the private build runner;
4. enable only the exact plugin names delivered by NVIDIA;
5. compile Win64 Shipping and run a clean launch on RTX hardware;
6. test 60-fps gameplay without frame generation first;
7. test Super Resolution/Ray Reconstruction and latency;
8. then test DLSS 5 neural rendering on supported RTX hardware;
9. compare artifacting on faces, hair, sails, foliage, particles, water, UI and fast melee silhouettes;
10. retain a non-DLSS fallback path.

No placeholder `DLSS5` plugin entry is added to `DarkArisen.uproject`; doing so before the official package exists would make the project look integrated while actually failing to load.

## Preview quality modes

| Mode | Renderer | Upscaling / neural features | Acceptance purpose |
|---|---|---|---|
| `Gameplay60` | Lumen + hardware RT when available | TSR fallback; official DLSS once installed | hard gameplay acceptance |
| `GameplayQuality` | Lumen HWRT + higher reflection/shadow budget | official DLSS/RR | high-end RTX gameplay |
| `CinematicPathTrace` | UE Path Tracer | offline/capture denoising path | hero frames, Sequencer, visual target |
| `Compatibility` | Lumen software/fallback path | TSR | non-RTX / unsupported RT |

Frame generation never substitutes for the base-frame performance test. The 60-fps floor is evaluated on real rendered frames before generated frames are counted.

## UE6 migration gate

When Epic releases a production UE6 build, create a separate compatibility branch from the then-current accepted UE5.8 commit. The UE6 gate must prove:

- project generation and native C++ compile;
- Pixel Streaming replacement/compatibility;
- Nanite/Lumen/material parity;
- water and ship physics parity;
- animation/retargeting parity;
- quest/save serialization compatibility;
- Windows/Linux packaging;
- Xbox Series X|S packaging only through authorised Microsoft GDKX platform extensions;
- DLSS/Streamline plugin compatibility;
- exact 60-fps design-law tests;
- start-to-credits story-state migration.

Only after those gates pass should the production `EngineAssociation` move from 5.8 to UE6.

## Current blockers to a real executable preview

A repository edit is not an Unreal build. A real first gameplay preview requires a self-hosted Windows or Linux runner with Unreal Engine **5.8** installed and labelled for this repository. The current historic automation was built around UE 5.5 and must not be treated as proof of a 5.8 compile.

Higgsfield is an active Alpha production source when the connected plan permits it. Generated media still does not become a skeletal animation, Unreal asset, or shipping build by itself: it must be materialized, converted, imported, bound, tested, cooked, and licensed for the intended use.
