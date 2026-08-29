# Repository findings register

**Audited:** 2026-08-29 against `origin/develop` = `9698659` (identical to `main`)  
**Scope:** defects, false statements and enforcement gaps that the milestone checklist does not capture.

`Docs/ALPHA_DELIVERY_CHECKLIST.md` remains the authoritative progress index for milestone scope. This document does not duplicate it and does not compete with it. Everything here is a finding *about* the repository — something wrong, missing, or unenforced — not a milestone deliverable.

Severity: **A** = a false statement a reader would act on · **B** = a defect that silently weakens a gate · **C** = drift, staleness or inconsistency.

## A — false statements

| # | Finding | Location |
|---|---|---|
| A1 | **The only sentence in the repository asserting that a test gate passed.** "Python: passed locally 2026-08-27 (25 tests + M0/M1/M2 validators + audit)". The validators, tests and audit were all deleted the following day. The claim is unreproducible, and the file's own rule at `:3` says "'Implemented' is not equivalent to 'verified.'" | `M0_CHECKLIST.md:33` |
| A2 | **Nineteen of twenty `Tools/ci` references are false** — eighteen are actionable instructions or present-tense capability claims about deleted files. Eleven live in `README.md` alone, including six `python Tools/ci/*.py` commands under the heading "Cheap deterministic checks can run anywhere with Python", which is now false in exactly the direction that matters. | `README.md:26,38,39,47-52,63,72,81,85,88,98`; `HANDOVER.md:113`; `Docs/M3_M4_SOURCE_COMPLETION.md:86`; `Docs/M5_M6_SOURCE_COMPLETION.md:46` |
| A3 | **Three documents claim live CI validation that does not exist.** "`ci.yml` … now runs M0, M1, M2, M3/M4, M5/M6, Alpha-platform and design-law validators on both configured platforms." The workflow has exactly one validation step per platform: `DarkArisenOps validate`. | `HANDOVER.md:112`; `Docs/M5_M6_SOURCE_COMPLETION.md:49`; `Docs/M3_M4_SOURCE_COMPLETION.md:86` |
| A4 | **`README.md:92` asserts in the present indicative that the streaming path "proves NVENC activation" and "installs a provider-level 30-minute idle shutdown"** for a GPU host that `Docs/GpuBackendDecision.md:4` confirms has never been created. It also credits a custom iPad frontend that `Docs/NATIVE_CPP_POLICY.md:35` says was abandoned. | `README.md:92` |
| A5 | **All 24 ticked boxes violate the checklist's own rule.** `Docs/ALPHA_DELIVERY_CHECKLIST.md:9` requires source *and* runtime evidence; no runtime evidence exists for anything. The M3–M6 sections handle this correctly by recording source closure as prose with an explicit "why the checkboxes remain open" paragraph. M0, M1 and M2 do not. The document is internally inconsistent about its own discipline. | `Docs/ALPHA_DELIVERY_CHECKLIST.md:25-27,34-47,54-66` |
| A6 | **`M0_CHECKLIST.md` ticks 21 of 22 boxes**, including "frame-time windows report p50, p95, maximum" (no measurement has ever been taken), "NVENC must be detected and proven in the Unreal log" (never executed), and "both runner preflights fail closed on wrong OS/architecture" — which is refuted by findings B3 and B4 below. | `M0_CHECKLIST.md:11,12,14,20,24` |
| A7 | Stale run references: "Main workflow run #22 is queued". No such queued run exists; the workflows produced zero jobs. | `README.md:64`; `M0_CHECKLIST.md:32` |

## B — defects that silently weaken a gate

| # | Finding | Location |
|---|---|---|
| B1 | **The workflows do not parse.** `run:` opens a YAML double-quoted scalar with `"%GITHUB_WORKSPACE%\Binaries\…`, where `\B`, `\W` and `\D` are illegal escapes. GitHub creates **zero jobs**; every run completes as `failure` within the same second. The repository moved from "queued because no runner exists" to "cannot start even if one did". *Fixed on `feature/m3-m4-systems`.* | `ci.yml:36,40,45,50`; `deploy-streaming.yml:39` |
| B2 | **The automation failure gate could never fire.** `Log.Contains(TEXT("Result={Fail}"))` is a literal substring test; Unreal writes `Result={Failed}`, which does not contain it. `Automation Test Failed` is not emitted verbatim either. The entire gate rested on one alternative. The deleted shell scripts had the same defect as a regex and it was carried into C++ unchanged. *Fixed on `feature/m3-m4-systems`.* | `BuildOps.cpp:114` |
| B3 | **No Windows toolchain check.** `runner-check` verifies the engine layout but nothing verifies the MSVC toolchain and Windows SDK that `Build.bat` invokes. A machine with UE 5.5 and no C++ workload prints "Runner preflight passed" and dies inside UnrealBuildTool minutes later. | `BuildOps.cpp` (absent) |
| B4 | **The Linux compiler check validates the wrong compiler and rejects the supported OS.** It requires system `clang` to report major version 18, but the build uses the engine's bundled `v23_clang-18.1.0-rockylinux8`. Ubuntu 22.04 — UE 5.5's supported host — ships clang 14 and is refused. | `BuildOps.cpp:366` |
| B5 | **Unreal automation never runs on `push`.** Compile is gated `if: github.event_name == 'push'`, automation `if: … == 'pull_request'`. Merges to `develop` and `main` run zero engine tests, so the design-law enforcement the project calls its core guarantee has an open path around it. Compile-only is strictly cheaper than the automation job, so the asymmetry looks inverted rather than deliberate. | `ci.yml:43,48` |
| B6 | **Fork pull requests report as green.** Both jobs are skipped for forks, and GitHub reports skipped required checks as satisfied unless branch protection distinguishes them. The guard itself is correct and must not be weakened. | `ci.yml:20,54` |
| B7 | **The forbidden-identifier scan is case-sensitive `Contains` over concatenated source.** `FASTTRAVEL`, `Fast_Travel` and `TeleportToWayPoint` all pass. Conversely a *comment* naming a banned token fails the build — `ShipVoyageComponent.h:87` deliberately hyphenates "fast-travel" to avoid this. Six of the twelve laws have no token at all: 3, 4, 5, 6, 10, 12. Missing entirely: `SetGlobalTimeDilation` (law 4), any music-reactivity token (law 5), `Undercroft` (law 12), `RomanceSystem`/`GiftMenu` (law 11 — only the two "…Meter" spellings are caught). | `ValidationOps.cpp:240-263` |
| B8 | **`ForbidFragments` returns silently when a file cannot be read**, so a deleted file passes the check. | `ValidationOps.cpp:49-52` |
| B9 | **Alpha promotion gates are unsigned operator self-attestation.** Hashes and manifests are verified rigorously; the acceptance claims — start-to-credits playthrough, 60 fps floor — are read from a hand-authored file. Arguably by design, but it should be stated rather than implied. | `promote-alpha` |

## Design-law enforcement — the asymmetry worth naming

Twenty-one `static_assert`s exist repo-wide (nineteen in `DesignLaws.h`, one in `CoreLoopTuning.h`, one in `PostureOnlyHUD.cpp`). Their distribution does not track the weight of the laws:

| Law | Asserts | Runtime guard | Test | Validator token | Held by |
|---|---|---|---|---|---|
| 9 — no conversion, no respec | **8** | structural | ✅ | 2 | asserts + absent API |
| 3 — 60 fps / six frames | 4 | ✅ | ✅ | ✅ | the *floor* itself is log-only |
| 8 — autosave | 1 | **best in repo** — private method, three legal callers | ✅ ×2 | 2 | genuinely well built |
| 10 — **no hostile path reaches a child** | **0** | **best in repo** | ✅ ×3 | **0** | structural absence only |
| 11 — no systemic romance | 0 | 0 | 0 | 2 meter-names | absence; a `RomanceComponent` avoiding the word "meter" passes everything |
| 12 — **undercroft never rendered** | 0 | 0 | 0 | 0 | **one comment line, `DesignLaws.h:22`** |

Law 10 survives only because the structural work was done properly: `ARexaSettlementResident` never creates a Health or Combat component, and `LockOnComponent.cpp:105-107` requires both, so residents are *unreachable* by targeting rather than merely rejected. That is the right pattern and it should be the model for laws 11 and 12.

Law 12 is the sharpest gap. The Belos *place* is modelled — `EPrincessQuestMovement::Belos`, `ResolveBelosAssault`, `BelosRideRequiredSeconds`, and two canonical protected moments — while the word `undercroft` appears zero times in `Source/`. Nothing distinguishes "deliberately excluded" from "not built yet."

**Correction to a claim made in PR #12:** a `static_assert` here is *not* engine-independent. `DesignLaws.h` includes `CoreMinimal.h`, and `DarkArisenOps.Target.cs` sets `bCompileAgainstEngine = false` and reads the header only as text. There is no non-UE build path, and `ValidationOps.cpp:64-91` forbids adding a script-based one. The asserts still fire strictly earlier than the tests and the validator — during the module compile — but every mechanism in this repository, compile-time included, needs a licensed UE 5.5 install.

## C — drift and inconsistency

- **`Docs/Handoff/ENGINEERING_HANDOFF.md` has never been revised**, yet three documents nominate it as a primary authority. It still names the wrong platforms (Windows + PS5; Linux appears nowhere), the wrong repository layout (`/Content`, `/Tests`, `/Source/DarkArisenEditor` — none exist), and a corpus size of "63 documents, roughly 400,000 words" against the register's 185 documents / 4.2 M characters.
- **Two milestone ladders.** Everything on `develop` treats M8 as terminal, including the CI workflow name and the checklist section "M8 — Platform, packaging and **Alpha sign-off**". PR #11 adds M9–M11 with "M11 — Private Alpha Release & Sign-off". Two milestones own the same terminal gate; nothing reconciles them. The plan doc is honest that M9–M11 are new and add no canon.
- **`NATIVE_CPP_POLICY.md` forbids what other documents instruct.** `Docs/Handoff/PIXEL_STREAMING_SETUP.md:56,84` carry *open, unchecked action items* telling a reader to create `.ps1` files, and `Tools/streaming/aws/README.md:39,40,42` instructs running three deleted ones.
- **M7/M8 is the only merged tranche with no source-closure record.** `Docs/M7_M8_SOURCE_COMPLETION.md` does not exist despite being required by `Docs/M7_M8_SOURCE_PLAN.md:92`, and the checklist has no M7/M8 prose.
- **`HANDOVER.md:3-6` is two milestones stale** — it names PR #8 as the active Draft; PR #8 and PR #9 have both merged.
- **Two of three character bibles are unreachable at their canonical path.** `docs/characters/docs/characters/ethan_harlow.md` is two levels deep and `docs/characters/docs/characters/docs/characters/draven_voss.md` three. Unchanged since the first audit.
- **`ANIMATIONS.md`** in the repository root is a German-language React/CSS specification for a mobile incremental game. Quarantined in both registers; still present.

## Content — the distinction that matters most

The contract layer and the content layer are at very different stages, and conflating them is the easiest mistake to make when reading this repository.

**18,677 lines of C++** across 125 files, with zero TODOs, zero empty method bodies and 28 automation tests. The validators are real: hard count checks, uniqueness sets, `return false` on every distinct failure path.

But the code models content that does not exist:

| | Required | Authored |
|---|---|---|
| Dungeons | 61 | 0 |
| Threads | 17 | 0 |
| Turns | 132 | **3** |
| Standing mission variants | 147 | **1** |
| Tier-1 bosses | 9 (all named with stable IDs) | **1 with a class** |
| Cutscenes | 19 | 0 |
| Protected playable moments | 22 (all enumerated by ID) | 0 |
| `.uasset` / `.umap` / any asset | — | **0** |

`UContentScaleManifestDataAsset` can *verify* every one of those counts. No populated manifest instance exists; the only place one is ever built is a test fixture. The forty-person Las Raíces roster is the sole genuinely authored content in the repository — and it is reachable only through `ARexaSettlementDirector`, which nothing places.

**Twenty-one classes are declared but never instantiated outside tests**, including all six Colonial War types, all five Highmoore types, both ContentScale types, and `ALaLiberacionShip` — which has zero references anywhere, tests included. Roughly **7,557 lines are unreachable from a running game**.

The live path is: `Config/DefaultEngine.ini:5` → `AGreyboxGameMode` → seven engine cube instances, two lights, `AJakeCharacter` with eleven components, one HUD, one duelling enemy and four interaction props. That is the entire playable world.
