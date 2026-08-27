# Dark Arisen

Single-player third-person action RPG built in Unreal Engine 5.5 with C++ as the primary language.

## Current delivery state

Milestone 0 establishes a compileable project boundary, a procedural greybox, Jake's baseline locomotion and stats, machine-enforced design-law constants, frame-time telemetry, self-hosted CI, and an automated Pixel Streaming 2 deployment path.

This is **not yet a playable Alpha or Beta**. A build becomes mergeable only after the Windows/UE compile, Unreal tests, in-engine smoke test, iPad controls, Pixel Streaming latency, and idle-deallocation gates in `M0_CHECKLIST.md` all have recorded green evidence.

## Engine and platform

- Unreal Engine **5.5**, pinned for M0 and matched to Epic's `PixelStreamingInfrastructure` `UE5.5` branch.
- Windows PC is the first build target. PlayStation 5 remains a later licensed platform target.
- 1920×1080 at a locked 60 fps floor; software Lumen and Nanite are the initial console-aligned rendering decisions.
- Git LFS is mandatory for Unreal binary assets.

## Local Windows build

Prerequisites: Unreal Engine 5.5 with C++ support, Visual Studio 2022, Python 3, Git, and Git LFS.

```powershell
git lfs install
& "$env:UE55_ROOT\Engine\Build\BatchFiles\GenerateProjectFiles.bat" `
  -Project="$PWD\DarkArisen.uproject" -Game
./Tools/ci/build-unreal.ps1 -RunAutomationTests
```

Opening `/Engine/Maps/Entry` uses `AGreyboxGameMode`, which constructs the M0 room in native code and spawns `AJakeCharacter`. No placeholder binary map is required.

## Verification

Cheap deterministic checks can run anywhere with Python:

```powershell
python Tools/ci/validate_m0.py
python Tools/ci/design_law_audit.py
python -m unittest discover -s Tools/ci -p "test_*.py"
```

Unreal compilation and automation run only on a private self-hosted Windows runner labelled `Windows`, `X64`, `ue5.5`, and `dark-arisen`. This avoids GitHub-hosted Actions minutes; it does not bypass or weaken CI.

## Pixel Streaming

`Tools/streaming/` pins Epic's free Pixel Streaming 2 software, builds the custom iPad-oriented frontend, configures Flo-only tailnet HTTPS and private TURN, packages atomically, proves NVENC activation, and installs a provider-level 30-minute idle shutdown. The hardened AWS template creates no public game or admin endpoint and cannot launch without a recorded cost acknowledgement.

The scripts do not provision or start a paid GPU machine. Host setup, secrets, and the six release gates are documented in `Tools/streaming/README.md` and `M0_CHECKLIST.md`.

## Design authority

The full 185-document repository corpus and both current handoff documents have been inventoried. `Docs/DesignAuthority.md` records canon order, conflicts, malformed paths, and superseded GDD decisions; `Docs/DesignSourceRegister.md` records coverage. `Source/DarkArisen/DesignLaws.h` exposes the twelve load-bearing laws to builds and tests; `Tools/ci/design_law_audit.py` rejects mechanically detectable violations.

The proposed GPU/persistence architecture is recorded in `Docs/GpuBackendDecision.md`. It is a decision record only: no infrastructure has been provisioned and no cost has been incurred.

## Branching and merge policy

Work moves from `feature/*` into `develop`; `main` receives milestone-complete changes only. Do not merge with a red, missing, queued, or unverified gate. Do not commit credentials, provider tokens, generated build outputs, or local streaming configuration.
