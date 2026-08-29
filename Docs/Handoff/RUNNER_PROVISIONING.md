# Runner provisioning — the decision, costed

**Written:** 2026-08-28  
**Purpose:** remove every unknown from the one blocker that gates M2, M3, M4 and Alpha simultaneously, so the remaining question is a single yes/no for Flo.

The repository has no self-hosted runner carrying the labels the workflow requires. Until recently that meant both UE 5.5 jobs sat `queued` until they expired; since the `DarkArisenOps` migration it has been worse, because the workflow files stopped parsing and GitHub created no jobs at all — see §6. Either way, nothing on the ladder can move. No amount of additional source advances any milestone.

## 1. What the workflow actually demands

`.github/workflows/ci.yml` defines two jobs, both `timeout-minutes: 120`, both skipped for fork pull requests:

| Job | `runs-on` |
|---|---|
| `verify-windows` | `[self-hosted, Windows, X64, ue5.5, dark-arisen]` |
| `verify-linux` | `[self-hosted, Linux, X64, ue5.5, dark-arisen]` |

Label matching is conjunctive — a runner must carry **all** of them. `self-hosted`, the OS label and `X64` are applied automatically at configuration; `ue5.5` and `dark-arisen` are custom and must be supplied with `--labels` **at registration time**. The `config` script cannot retrofit labels onto an already-registered runner; adding them afterwards requires the repository UI or the REST API.

**Queued jobs expire after 24 hours.** This has already happened once in this repository: run #6 was created 2026-08-27T12:36:17Z and was cancelled at 2026-08-28T12:36:20Z. Expired runs do not resurrect when a runner appears — they must be re-run by hand.

### Preflight, before any compile step runs

One environment variable governs everything, with the same name on both platforms: **`UE55_ROOT`**, the absolute path to the directory *containing* `Engine/`. It must be set machine-scoped on Windows and in the **runner service** environment on Linux — an interactive shell export is not enough.

`DarkArisenOps runner-check` verifies: the platform and architecture; `UE55_ROOT`; the engine layout including `Engine/Build/Build.version` and the platform build script; that `Build.version` reports 5.5 and the `.uproject` names `EngineAssociation "5.5"`; Git and Git LFS; and at least 120 GB free on the project drive.

On Linux it additionally requires that **system `clang` reports major version exactly 18** (`BuildOps.cpp:366`) — see the defect note in §6, because that is not the compiler the build actually uses.

## 2. Licensing

**No per-machine entitlement, licence key, or activation exists.** The Unreal Engine EULA §2 "User License" states the Licensed Technology "is licensed to you for use by a single User" and that "**the User may store the Licensed Technology on any of the User's computers**". A build VM in Flo's own account is one of the User's computers.

Two consequences that matter operationally:

- **An Epic account is needed to *obtain* the engine, not to run it.** Once the bits are on disk, `Build.bat`/`Build.sh` and `UnrealEditor-Cmd -NullRHI` run headless with no sign-in and no licence check.
- **The account must not be shared.** The same EULA section says "You may not share or allow others to use your Account." If provisioning is delegated, the operator must never receive Flo's Epic credentials — Flo installs, or Flo produces the engine image, and the operator only attaches it.

Royalty terms are unchanged by any of this: games are free to use with a 5% royalty above the first USD 1,000,000 per product, reduced to 3.5% for titles released on the Epic Games Store at or before other stores. The seat-based USD 1,850/seat/year figure belongs to the separate Creators EULA and does not apply to a game.

**Copying an installed engine tree to a second machine Flo controls is storing, not Distributing.** But publishing that tree — a public AMI, a public container image, a public bucket object — would be Distribution of Engine Tools and is restricted. Any engine-bearing image must stay private. That happens to match `SECURITY.md`'s private-storage rule anyway.

### Linux is materially different, and is the real risk

**There is no Epic Games Launcher for Linux.** Two lawful paths:

**(a) Epic's pre-built Linux archive** — distribution-agnostic pre-compiled binaries, roughly 25 GB compressed and ~43 GB extracted, behind Epic account sign-in. Faster by hours. Two caveats: Epic does not keep every version listed indefinitely, so 5.5 may have aged off the page; and the archive is an *Installed Build*, which can only produce the configurations it was built with. `DarkArisenOps build` requires a `DarkArisen Linux Shipping` link, so **this must be proven by running it once, not assumed.**

**(b) Source build from `github.com/EpicGames/UnrealEngine`, branch `5.5`** — this **does** require linking a GitHub account to an Epic account, then accepting an emailed `@EpicGames` organisation invitation within seven days. Verified: an unauthenticated fetch of that repository returns **HTTP 404**, which is the normal symptom of an incomplete link rather than a wrong URL. `Setup.sh` then downloads the `v23_clang-18.1.0-rockylinux8` native toolchain, guaranteeing a compiler matching Epic's codebase, and a source build supports Shipping unconditionally.

**Do the Epic↔GitHub link before provisioning day.** It has a seven-day invitation window and involves a human email step only Flo can perform.

## 3. Hardware

**No GPU is required.** Every CI step is UnrealBuildTool, and the automation pass runs `-NullRHI`. This is the single most cost-relevant fact in the document.

| | Windows x64 | Linux x86_64 |
|---|---|---|
| CPU | 8 physical cores min, 16 vCPU recommended | same |
| RAM | 32 GB min, **64 GB recommended** — the monolithic Shipping link is the peak | same |
| Engine on disk | ~100–130 GB installed | ~43 GB from the archive; 150–250 GB if source-built |
| Project + Intermediate | 20–60 GB today, growing with `Content/` under LFS | same |
| Derived data cache | 20–100 GB+; UE 5.4+ defaults to a Zen local DDC | same |
| Preflight hard floor | **120 GB free on the project drive** | same |
| Disk total | **500 GB NVMe minimum, 1 TB recommended** | 500 GB min, 700 GB–1 TB if source-building |
| GPU | **none** | **none** |
| OS | Windows Server 2022/2025 or Windows 10/11 x64 | **Ubuntu 24.04 LTS** — it ships clang 18.1.3, which is what the preflight demands |
| Also required | Git, Git LFS, VS 2022 17.8+ with MSVC ≥14.38 and a Windows SDK, .NET 8 | Git, Git LFS, `clang` 18 as the unversioned name, `zip` |

**GitHub-hosted runners are structurally impossible here**, independent of the label constraint: they provide 14 GB of disk. The engine alone is 3–9× that.

Note the preflight measures free space on the drive of the *project root*. Putting the engine and the runner work directory on the same 500 GB volume will trip the 120 GB floor once the DDC grows. Prefer separate volumes, or size to 1 TB.

## 4. Registration

Repository → Settings → Actions → Runners → New self-hosted runner. The registration token is valid about one hour and is a secret — never commit, echo, screenshot, or paste it into an issue or PR.

**Windows** (runner v2.337.0), after installing VS 2022 with the C++ workload, Git and Git LFS, and setting `UE55_ROOT` machine-scoped. PowerShell 7 and Python are **no longer required** — the migration to `DarkArisenOps` removed every script dependency:

```powershell
./config.cmd --url https://github.com/clarityosbaerbelwesterop-gif/dark-arisen `
             --token <REGISTRATION_TOKEN> `
             --name dark-arisen-win-01 `
             --labels ue5.5,dark-arisen `
             --work D:\ghwork `
             --runasservice
"%GITHUB_WORKSPACE%\Binaries\Win64\DarkArisenOps.exe" runner-check --root=. --engine=%UE55_ROOT%
```

**Linux**, after installing the engine (executable bits set on `UnrealEditor-Cmd`, `Build.sh` and `RunUAT.sh`), clang 18 reachable as `clang`, and `UE55_ROOT` in the service environment:

```bash
./config.sh --url https://github.com/clarityosbaerbelwesterop-gif/dark-arisen \
            --token <REGISTRATION_TOKEN> \
            --name dark-arisen-linux-01 \
            --labels ue5.5,dark-arisen \
            --work /data/ghwork
sudo ./svc.sh install && sudo ./svc.sh start
"$GITHUB_WORKSPACE/Binaries/Linux/DarkArisenOps" runner-check --root=. --engine=$UE55_ROOT
```

**Verify the label set resolves** before trusting anything:

```bash
gh api repos/clarityosbaerbelwesterop-gif/dark-arisen/actions/runners \
  --jq '.runners[] | {name, status, labels: [.labels[].name]}'
```

The definitive test is that a queued job leaves `queued` within seconds of the service starting. If it does not, the cause is almost always a typo in `--labels`, registration at the user/org level instead of the repository, or a runner configured before `--labels` was supplied.

## 5. Cost

All figures are AWS on-demand, shared tenancy, EU (Frankfurt) `eu-central-1`, USD excluding tax, taken from the AWS Price List bulk offer file published 2026-08-27. These are not estimates.

| Instance | vCPU | RAM | Linux $/h | Windows $/h |
|---|---|---|---|---|
| `c7i.4xlarge` | 16 | 32 GiB | 0.8148 | 1.5508 |
| **`m7i.4xlarge`** | **16** | **64 GiB** | **0.9660** | **1.7020** |
| `c7i.8xlarge` | 32 | 64 GiB | 1.6296 | 3.1016 |
| `g6.2xlarge` (L4 GPU) | 8 | 32 GiB | 1.2225 | 1.5905 |

The Windows premium is exactly **$0.046 per vCPU-hour** across every instance — $0.736/h extra at 16 vCPU, purely for the OS licence. Frankfurt gp3 storage is **$0.0952 / GB-month**, and **EBS bills while the instance is stopped**. Billing is per-second with a one-minute minimum on both Linux and Windows.

Self-hosted runner minutes are **free today**. GitHub proposed a $0.002/min charge on self-hosted usage from 2026-03-01 and then **postponed it**. Treat it as a live risk, not a current cost.

| Option | Cash/month | Time to green | Approval needed | Security fit |
|---|---|---|---|---|
| **(a) Owner hardware** | **$0** | **Hours** | **None** — creates no billable resource | Good, if dedicated |
| (c1) AWS start/stop, 400 GiB gp3 | ~$130 (~$43 if snapshot-parked) | 1–2 days | **Yes** | Good |
| (c2) AWS ephemeral + private AMI | ~$70 | 1–2 weeks | **Yes**, plus a new secret surface | Best |
| (b) AWS always-on pair | **~$2,043** | 1 day | **Yes** | Wasteful |

In (c1) the standing **EBS cost dominates**: keeping the engine on live volumes is ~$76/month at zero builds. Parking it as snapshots drops that to ~$22 per 400 GiB at the price of a restore step and a cold DDC every run.

**Recommendation: (a).** It is the only option that consumes none of Flo's cost-approval budget, it needs no new infrastructure and no new secret surface, and it is the fastest path to clearing the queue before more runs expire. Escalate to (c1) only if no suitable hardware exists.

### The streaming host is not the build runner

`Docs/GpuBackendDecision.md` selects a `g6.2xlarge` Windows instance in Frankfurt for Pixel Streaming. That remains the right streaming choice and should not be revisited. It is the wrong build runner on five independent counts:

1. `SECURITY.md` forbids it outright — "Run the CI runner and streaming services on dedicated machines or isolated service identities." Co-locating them also puts the runner's `_work` tree, into which arbitrary branch content is checked out, on the host holding `C:\ProgramData\DarkArisen`, which that document classifies as sensitive.
2. The GPU is dead weight; the automation pass is explicitly `-NullRHI`.
3. It is the wrong shape. 8 vCPU / 32 GiB, where `m7i.4xlarge` gives 2× the cores and 2× the RAM for $0.11/h more — strictly cheaper per build.
4. Its 450 GB scratch is instance store, which AWS erases on stop. A runner that stops between jobs would lose its DDC every time and push toward the 120-minute timeout.
5. It breaks the cost gate: every push would wake a billable GPU, and a resident runner service complicates the zero-player idle-shutdown evidence.

## 6. Defects in the preflight and build tooling

The Python/PowerShell/Bash tooling these were originally found in was deleted and reimplemented as the native C++ program `Source/DarkArisenOps/`. Each finding below was re-checked against that rewrite rather than carried over.

**Fixed on this branch:**

- **Both workflows could not parse, so no CI job has been created since the rewrite.** `.github/workflows/ci.yml` lines 36/40/45/50 and `deploy-streaming.yml:39` began a YAML double-quoted scalar with `"%GITHUB_WORKSPACE%\Binaries\...`, where `\B`, `\W` and `\D` are illegal escapes. Every run since the `Tools/ci` → `DarkArisenOps` migration therefore completed as `failure` within the same second and created **zero jobs** — a workflow startup failure, not a queued job. The repository went from "queued forever because no runner exists" to "cannot start at all", which is strictly worse because attaching a runner would not have helped. The Windows `run:` values are now single-quoted, matching how the Linux steps were already written; the commands themselves are byte-identical.

- **The automation failure gate could never fail.** `BuildOps.cpp:114` tested `Log.Contains(TEXT("Result={Fail}"))` — a literal substring, not a pattern. Unreal writes `Result={Failed}`, and `Result={Fail}` is not a substring of it, because after `Fail` the log has `e` rather than `}`. `Automation Test Failed` is not a string Unreal emits verbatim either, so the whole gate rested on the single `LogAutomationController: Error` alternative. The identical defect existed in the deleted shell scripts and was carried into C++ unchanged. Both spellings are now accepted, plus a reported test failure.
- **The disk gate could be switched off by a bad argument.** `FCString::Atoi64` returns 0 for a non-numeric value, so `--min-disk-gb=abc` disabled the check entirely. The 120 GB floor may now be raised, never lowered.

**Open, and needing an owner decision:**

- **P0 — Unreal automation never runs on `push`.** In `.github/workflows/ci.yml` the compile step is gated `if: github.event_name == 'push'` and the automation step `if: github.event_name == 'pull_request'`. A merge to `develop` or `main`, a direct push, an admin merge or a merge-queue commit compiles the targets and runs **zero** engine tests. The design-law enforcement this project treats as its core guarantee has an open path around it, and since compile-only is strictly cheaper than the automation job, the asymmetry looks inverted rather than deliberate.
- **P0 — no Windows toolchain check.** `runner-check` verifies the engine layout but nothing verifies the MSVC toolchain and Windows SDK that `Build.bat` actually invokes. A machine with UE 5.5 and no Visual Studio C++ workload prints "Runner preflight passed" and then dies inside UnrealBuildTool minutes later. This remains the most likely first-operator experience.
- **P1 — the Linux clang check validates the wrong compiler and rejects the supported OS.** `BuildOps.cpp:366` requires system `clang --version` to report `version 18`. The build itself uses the engine's bundled `v23_clang-18.1.0-rockylinux8` toolchain, which this never inspects. Ubuntu 22.04, which is UE 5.5's supported host, ships clang 14 and is refused; Ubuntu 24.04 passes by coincidence. A machine carrying only `clang-18` without the unversioned symlink also fails.
- **P1 — fork pull requests read as green.** Both jobs are skipped for forks, and GitHub reports skipped required checks as satisfied unless branch protection distinguishes them. The guard itself is correct and must not be weakened — it is what makes a persistent self-hosted runner safe here — but the reporting consequence should be understood.
- **P2 — Alpha promotion gates are operator self-attestation.** `promote-alpha` verifies hashes and manifests rigorously, but the acceptance gates, including the start-to-credits playthrough and the 60 fps floor, are read from an unsigned hand-authored evidence file. Arguably by design, but it should be stated plainly rather than implied.

Findings that no longer apply, because the scripts holding them were deleted: the PowerShell 5.1 `$IsWindows` hard error, the Microsoft Store `python.exe` stub satisfying `Get-Command`, the `-f` versus `-x` mismatch on the Linux engine binaries, and the `DARKARISEN_MINIMUM_FREE_DISK_GB` environment-variable hole.

**Before trusting the first green run**, deliberately break a `TestEqual` in `Source/DarkArisen/Tests/M3M4SystemsSpec.cpp` and confirm the job goes red. A gate that has never been observed to fail is not yet a gate — and until the fix above, this one could not have failed at all.

## 7. Not verified — check these first

- **Whether UE 5.5 is still downloadable at all**, on either platform. `unrealengine.com` and `dev.epicgames.com` were unreachable from the environment this document was written in. This decides between the Linux archive and a source build, and it is the highest-value thing to check first.
- Whether Epic's pre-built Linux 5.5 archive includes the Shipping configuration. Prove it by running `DarkArisenOps build` once.
- Epic's official hardware specification numbers and the engine install sizes quoted above are community measurements, not figures read from Epic's page.
- UE 5.5's exact required Visual Studio, MSVC, Windows SDK and .NET versions are from secondary sources.
- Actual per-run wall-clock time, which drives every figure in §5. Measure the first successful run and re-cost.
