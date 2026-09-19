# Dark Arisen — Open Generative Asset Production Pipeline

Status: IMPLEMENTED AS PRODUCTION AUTHORITY / RUNTIME VERIFICATION PENDING
Engine target: Unreal Engine 5.8

## Purpose

Dark Arisen uses a local/open generative production lane for source art, 3D reconstruction, texture/look-development, cinematic previs and asset candidates while native Unreal C++ remains the gameplay and story authority.

Approved upstream tools:

- Anil-matcha/Open-Generative-AI — image/video/look-development and reference generation frontend. Treat cloud-backed providers as optional; local generation is preferred when available.
- MrForExample/ComfyUI-3D-Pack — 3D reconstruction/generation/mesh/UV processing lane.
- OpenDCAI/GameFactory-3A — experimental game-asset/world generation lane, gated behind reproducibility and license/provenance review before generated outputs enter production.

These repositories are tooling dependencies/references. Do not vendor their full source into the game repository unless a later license/security review explicitly requires it.

## Hard boundaries

1. Generated media never owns mission, combat, AI, save, boss, naval, economy or progression authority.
2. Every accepted generated asset must retain provenance: tool/repository, model if known, source prompt/reference, generation date/version, license status, and human acceptance state.
3. No generated output is called FINAL until topology, UVs, materials, scale, collision, rig/animation compatibility, visual continuity and Unreal import have passed the relevant gates.
4. No generated character may silently change canonical identity. Jake, Ethan, Draven and recurring cast require locked identity references.
5. Real Ethan Harlow is alive, friendly after rescue and never a boss. Dream Ethan is a separate presentation/combat identity.
6. Do not copy protected characters, logos, music, recognizable proprietary assets or another game's art direction one-for-one.
7. Generated code/scripts/models are untrusted input until reviewed. Do not execute downloaded repository scripts or model files in production CI with repository/write secrets.
8. External model downloads must be pinned/checksummed where practical. Generated binary assets must not be granted execution permissions.
9. Unreal 5.8 is the target runtime. Source generation tools are not the playable runtime and are not substitutes for Windows Shipping/Pixel Streaming acceptance.

## Production flow

### A — Identity and look development

Open-Generative-AI produces controlled reference batches for characters, ships, regions, props, lighting, weather and cinematic keyframes. Keep stable prompt/reference packages under `ContentSource/Generation/` and final candidate media under the appropriate `ContentSource` category.

### B — 3D generation/reconstruction

ComfyUI-3D-Pack is the preferred open 3D processing lane for suitable reference sets. GameFactory-3A may be evaluated as a second generation lane. Generated meshes remain SOURCE CREATED until inspected.

### C — Asset conditioning

Before Unreal import, candidates must be conditioned for:

- real-world scale and stable pivots;
- sane topology/no catastrophic non-manifold geometry;
- UVs and material slots;
- PBR texture naming and color-space intent;
- LOD/Nanite suitability by asset type;
- collision strategy;
- skeletal/rig compatibility for characters;
- no hidden executable payloads;
- provenance sidecar manifest.

### D — Unreal import

Import into deterministic `/Game/...` destinations. Generated assets may replace Alpha proxy presentation only after the source candidate passes conditioning. Gameplay classes and canonical stable IDs remain unchanged.

### E — acceptance

Acceptance states are:

- SOURCE CREATED
- IMPORT READY
- IMPORTED / RUNTIME VERIFY PENDING
- VERIFIED
- REJECTED

Visual acceptance alone is not runtime acceptance. Shipping/Pixel Streaming must still be built and tested on a legitimate UE 5.8 host.

## Initial priority queue

1. Jake/Ethan/Draven identity locks and production character references.
2. Harlow merchant ship, La Liberacion and Draven flagship production geometry/material references.
3. Opening route production environments and opening raid cinematic assets.
4. Moran/Rexa/open-sea production environment kits.
5. Chapter 8 Ethan prison/rescue assets.
6. Ethan's Grove and separate Dream Ethan presentation.
7. Crimson Armada, Black Deck and Draven finale assets.
8. World props/settlement/vegetation/shoreline modular libraries.
9. Gameplay animation source/retarget candidates.
10. Cinematic/audio finishing and credits presentation.

## Integration contract

The asset lane must converge on the existing authored game rather than generate a second game. Mission IDs, map IDs, canonical character IDs and existing native systems are the integration keys. When generated material conflicts with `DesignAuthority`, `MAIN_STORY_AUTHORITY_2026_09`, native C++ policy or authored story contracts, the generated material is rejected.
