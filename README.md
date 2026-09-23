# Dark Arisen

Single-player third-person action RPG built in Unreal Engine 5.8 with C++ as the primary language.

## Current delivery state

Milestone 0 establishes a compileable project boundary, a procedural greybox, Jake's baseline locomotion and stats, machine-enforced design-law constants, frame-time telemetry, self-hosted CI, and an automated Pixel Streaming 2 deployment path. The M1 source tranche adds committed combat timing, five posture states, four wound layers, camera-control policy, combat input, duration-bearing physical interaction and a native animation bridge. Current M2 source adds the markerless quest/journal foundation, four finite Rexa missions, the exact authored forty-person Las Raíces roster, schedule routing and protected-child combat response, Isabel Cruz encounter state, fort heat pressure, water breath/drowning and Cenote progress contracts. Exact scope and remaining gates are recorded in `Docs/M1_CORE_LOOP.md` and `Docs/M2_VERTICAL_SLICE.md`.

This is **not yet a playable Alpha or Beta**. An Alpha exists only after the complete M0–M8 game passes Windows and Linux UE compile/automation, native start-to-credits playthroughs, performance/content checks, iPad Pixel Streaming and the remaining gates in `Docs/ALPHA_DELIVERY_CHECKLIST.md` on the exact candidate hashes.

## O3DE evaluation (in progress)

An open-source engine evaluation on **O3DE 2605.0** (pinned in `Tools/o3de/O3DE_VERSION.lock`) runs alongside
the Unreal reference, which stays intact until O3DE proves Dark Arisen itself runs. Gameplay rules are being
moved into the engine-neutral `Core/` library (built and tested in CI); O3DE adapters live in
`Engine/O3DE/DarkArisen`. Status, evidence, blockers and gates: `Docs/O3DE_MIGRATION.md`.

## Engine and platform

- Unreal Engine **5.8**, with the project association, native operations checks, CI runner labels and Pixel Streaming integration aligned to that version.
- Windows x64 and Linux x86_64 are the two Alpha build targets. Native PlayStation 5 is deferred to Beta and requires authorised Sony tools and development/test hardware.
- 1920×1080 at a locked 60 fps floor; software Lumen and Nanite are the initial console-aligned rendering decisions.
- Git LFS is mandatory for Unreal binary assets.

## Local Windows build

Prerequisites: Unreal Engine 5.8 with C++ support, Visual Studio 2022, Git, and Git LFS.

```powershell
git lfs install
& "$env:UE_ROOT\Engine\Build\BatchFiles\GenerateProjectFiles.bat" `
  -Project="$PWD\DarkArisen.uproject" -Game
& "$env:UE_ROOT\Engine\Build\BatchFiles\Build.bat" DarkArisenOps Win64 Development `
  "-Project=$PWD\DarkArisen.uproject" -WaitMutex -WarningsAsErrors
./Binaries/Win64/DarkArisenOps.exe build "--root=$PWD" "--engine=$env:UE_ROOT" --automation
```

Opening `/Engine/Maps/Entry` uses `AGreyboxGameMode`, which constructs the M0 room in native code and spawns `AJakeCharacter`. No placeholder binary map is required.

## Local Linux build

Prerequisites: a licensed Unreal Engine 5.8 Linux installation, Epic's UE 5.8 Linux toolchain (clang 18), Git, Git LFS and Zip.

```bash
git lfs install
export UE_ROOT=/absolute/path/to/UnrealEngine-5.8
"$UE_ROOT/Engine/Build/BatchFiles/Linux/Build.sh" DarkArisenOps Linux Development \
  "-Project=$PWD/DarkArisen.uproject" -WaitMutex -WarningsAsErrors
./Binaries/Linux/DarkArisenOps build "--root=$PWD" "--engine=$UE_ROOT" --automation
```

## Verification

The native repository validator runs after building `DarkArisenOps`:

```bash
./Binaries/Linux/DarkArisenOps validate "--root=$PWD"
```

Unreal compilation and automation run only on private self-hosted runners. Windows requires `Windows`, `X64`, `ue5.8`, `dark-arisen`; Linux requires `Linux`, `X64`, `ue5.8`, `dark-arisen`. This avoids GitHub-hosted Actions minutes and public build artifacts; it does not bypass or weaken CI.

### Attach an existing Windows machine without cloud spend

1. Install the licensed Unreal Engine 5.8 build, Git LFS and Python on a dedicated Windows x64 machine.
2. Set the machine-level `UE_ROOT` environment variable to that UE 5.8 installation.
3. In the private repository, open **Settings → Actions → Runners → New self-hosted runner** and follow GitHub's one-time Windows registration commands. Never commit or paste the registration token into a file or PR.
4. Add the custom labels `ue5.8` and `dark-arisen`; GitHub supplies `self-hosted`, `Windows`, and `X64`.
5. Run `DarkArisenOps.exe runner-check --root=<repo> --engine=%UE_ROOT%` locally. It fails closed on the wrong engine version, missing tools, non-Windows/non-x64 hosts, or insufficient disk.
6. Start the runner service. Main workflow run #22 and successor-PR workflows will be claimed automatically when their exact required labels match.

This path uses existing hardware and no cloud GPU. It does not create a Pixel Streaming host; streaming remains a separate private and cost-gated deployment step.

### Attach an existing Linux machine without cloud spend

1. Install the licensed UE 5.8 Linux build/toolchain, Git LFS, Python 3 and Zip on a dedicated x86_64 machine.
2. Set `UE_ROOT`, register it as a private self-hosted repository runner and add `ue5.8` plus `dark-arisen`; GitHub supplies `self-hosted`, `Linux` and `X64`.
3. Run `DarkArisenOps runner-check --root=<repo> --engine=$UE_ROOT`, then start the runner service. It fails closed on an invalid OS, architecture, engine, clang version, dependency set or disk capacity.

No cloud provider, credit or paid runner is started by these instructions.

## Private Alpha candidates

After all content is implemented and the exact commit is clean, the two platform runners create local, non-uploaded candidates:

```powershell
./Binaries/Win64/DarkArisenOps.exe package-alpha --revision=<40-character-commit> "--engine=$env:UE_ROOT"
```

```bash
./Binaries/Linux/DarkArisenOps package-alpha --revision=<40-character-commit> "--engine=$UE_ROOT"
```

They are not Alpha releases until the same immutable candidate hashes pass the complete acceptance record and `Tools/ci/promote-alpha-candidates.py` creates `DarkArisen-Alpha-Windows-Arcware.zip` and `DarkArisen-Alpha-Linux-x86_64.zip`. The full contract is in `Docs/ALPHA_PLATFORM_ARTIFACTS.md`.

## Pixel Streaming

`Tools/streaming/` pins Epic's free Pixel Streaming 2 software, builds the custom iPad-oriented frontend, configures HTTPS/password protection and TURN, packages atomically, proves NVENC activation, and installs a provider-level 30-minute idle shutdown.

The scripts do not provision or start a paid GPU machine. Host setup, secrets, and the six release gates are documented in `Tools/streaming/README.md` and `M0_CHECKLIST.md`.

## Design authority

The full 185-document repository corpus and both current handoff documents have been inventoried. `Docs/DesignAuthority.md` records canon order, conflicts, malformed paths, and superseded GDD decisions; `Docs/DesignSourceRegister.md` records coverage. `Source/DarkArisen/DesignLaws.h` exposes the twelve load-bearing laws to builds and tests; `Tools/ci/design_law_audit.py` rejects mechanically detectable violations.

The proposed GPU/persistence architecture is recorded in `Docs/GpuBackendDecision.md`. It is a decision record only: no infrastructure has been provisioned and no cost has been incurred.

## Branching and merge policy

Work moves from `feature/*` into `develop`; `main` should receive milestone-complete changes only. PR #4/#5 source was merged before runtime gates executed, so that history is not milestone acceptance. Do not repeat the exception: never describe a red, missing, queued, or unverified revision as accepted. Do not commit credentials, provider tokens, generated build outputs, or local streaming configuration.
