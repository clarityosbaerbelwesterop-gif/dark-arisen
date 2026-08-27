# Dark Arisen

Single-player third-person action RPG built in Unreal Engine 5.5 with C++ as the primary language.

## Current delivery state

Milestone 0 establishes a compileable project boundary, a procedural greybox, Jake's baseline locomotion and stats, machine-enforced design-law constants, frame-time telemetry, self-hosted CI, and an automated Pixel Streaming 2 deployment path. The M1 source tranche adds committed combat timing, five posture states, four wound layers, camera-control policy, combat input, duration-bearing physical interaction and a native animation bridge. The first M2 source tranche adds a markerless six-trigger quest state and chronological, fallible journal foundation. Exact scope and remaining gates are recorded in `Docs/M1_CORE_LOOP.md` and `Docs/M2_VERTICAL_SLICE.md`.

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
python Tools/ci/validate_m1.py
python Tools/ci/validate_m2.py
python Tools/ci/design_law_audit.py
python -m unittest discover -s Tools/ci -p "test_*.py"
```

Unreal compilation and automation run only on a private self-hosted Windows runner labelled `Windows`, `X64`, `ue5.5`, and `dark-arisen`. This avoids GitHub-hosted Actions minutes; it does not bypass or weaken CI.

### Attach an existing Windows machine without cloud spend

1. Install the licensed Unreal Engine 5.5 build, Git LFS and Python on a dedicated Windows x64 machine.
2. Set the machine-level `UE55_ROOT` environment variable to that UE 5.5 installation.
3. In the private repository, open **Settings → Actions → Runners → New self-hosted runner** and follow GitHub's one-time Windows registration commands. Never commit or paste the registration token into a file or PR.
4. Add the custom labels `ue5.5` and `dark-arisen`; GitHub supplies `self-hosted`, `Windows`, and `X64`.
5. Run `./Tools/ci/verify-runner.ps1` locally. It fails closed on the wrong engine version, missing tools, non-Windows/non-x64 hosts, or insufficient disk.
6. Start the runner service. The queued PR #4 workflow will be claimed automatically.

This path uses existing hardware and no cloud GPU. It does not create a Pixel Streaming host; streaming remains a separate private and cost-gated deployment step.

## Pixel Streaming

`Tools/streaming/` pins Epic's free Pixel Streaming 2 software, builds the custom iPad-oriented frontend, configures HTTPS/password protection and TURN, packages atomically, proves NVENC activation, and installs a provider-level 30-minute idle shutdown.

The scripts do not provision or start a paid GPU machine. Host setup, secrets, and the six release gates are documented in `Tools/streaming/README.md` and `M0_CHECKLIST.md`.

## Design authority

The full 185-document repository corpus and both current handoff documents have been inventoried. `Docs/DesignAuthority.md` records canon order, conflicts, malformed paths, and superseded GDD decisions; `Docs/DesignSourceRegister.md` records coverage. `Source/DarkArisen/DesignLaws.h` exposes the twelve load-bearing laws to builds and tests; `Tools/ci/design_law_audit.py` rejects mechanically detectable violations.

The proposed GPU/persistence architecture is recorded in `Docs/GpuBackendDecision.md`. It is a decision record only: no infrastructure has been provisioned and no cost has been incurred.

## Branching and merge policy

Work moves from `feature/*` into `develop`; `main` receives milestone-complete changes only. Do not merge with a red, missing, queued, or unverified gate. Do not commit credentials, provider tokens, generated build outputs, or local streaming configuration.
