# Dark Arisen — RTX / 4K Alpha Rendering Acceptance

Status: **IMPLEMENTED CONFIGURATION / GPU RUNTIME VERIFICATION PENDING**

## Alpha rendering contract

The Unreal Engine 5.8 build remains the rendering authority. The browser/hosting surface must never replace the Unreal renderer with generated frames, a JavaScript recreation, or a second game implementation.

Default alpha profile:
- 3840x2160 desktop target.
- DX12 + Shader Model 6.
- Nanite.
- Lumen GI/reflections with hardware ray tracing enabled.
- UE path-tracing support compiled/enabled for supported high-quality capture/presentation workflows.
- Epic scalability defaults.
- 60 fps cap until real GPU profiling establishes a safer quality/performance matrix.
- Pixel Streaming may encode a lower transport resolution only as an explicit fallback; the Unreal render profile remains authoritative.

## DLSS

As of 2026-09-18 NVIDIA's public UE 5.8 download is **DLSS 4.5**. It exposes Super Resolution, Ray Reconstruction, DLAA, Reflex and Dynamic Multi Frame Generation. NVIDIA has announced DLSS 5 3D-Guided Neural Rendering, but a generally downloadable UE 5.8 DLSS 5 plugin is not currently listed on the public UE plugin page.

Therefore:
- do not label Dark Arisen as DLSS 5 until an authorized UE 5.8 DLSS 5 SDK/plugin is actually available and integrated;
- use the latest official UE 5.8 DLSS 4.5 plugin for the alpha once its plugin package is supplied/installed under its NVIDIA terms;
- upgrade to DLSS 5 only through NVIDIA's official package, never through an unofficial binary;
- do not commit NVIDIA redistributables unless their license explicitly permits that distribution.

## Path tracing

`r.PathTracing=True` enables the engine's path-tracing feature support. It is not a promise that every live gameplay frame will use the offline/reference path-traced view. Normal gameplay uses UE5.8's real-time renderer with hardware ray tracing/Lumen; path tracing is reserved for modes/workflows where UE and the target GPU support it without breaking playability.

## Higgsfield hosting boundary

Higgsfield may own the public web/launcher URL and the generated asset pipeline. The official Higgsfield game deployment contract is a browser-game application. It does not execute a packaged UE5.8 Win64 GPU renderer by itself.

A compliant Dark Arisen alpha URL therefore has this topology:

Higgsfield public launcher URL -> authenticated/session-safe player page -> UE5.8 Pixel Streaming signalling/WebRTC -> GPU host running the exact packaged Dark Arisen revision.

This keeps the visible entry point on Higgsfield while preserving Unreal Engine rendering, gameplay, physics, native C++ authority, Nanite, Lumen, ray tracing and DLSS. A pure Higgsfield browser rewrite is forbidden.

## No-AI-slop acceptance gate

Generated assets are source candidates, not automatically shippable content. Hero characters, ships, environments and animations must pass identity, silhouette, topology, UV, PBR, scale, collision, rig, retarget, animation cleanup, material, lighting, LOD/Nanite and in-engine continuity review. Failed assets are rejected rather than shipped to fill a slot.

## Required runtime evidence before sharing the Alpha URL

1. UE5.8 Development/Shipping build succeeds for the exact revision.
2. Story materialization and cook succeed.
3. New Game -> credits traversal passes.
4. Save/load/continue passes across process restart.
5. Armada/Draven/Ethan/Dream-Ethan contracts pass.
6. 4K profile launches on the acceptance GPU; fallback profile is measured.
7. No critical render/RHI/Pixel Streaming errors.
8. Public Higgsfield launcher reaches the real UE Pixel Streaming session.
9. Keyboard/mouse and iPad/touch path accepted.
10. URL tested anonymously/in the intended access mode before it is sent to the player.
