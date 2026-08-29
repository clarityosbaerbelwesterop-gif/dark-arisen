# Rules for AI agents working in this repository

**Applies to:** every AI agent, subagent, or automated session that reads or writes this repository — including agents spawned by another agent.

This is not general advice. Every rule below exists because the situation it describes has already happened here, or because a machine check in this repository already enforces it. Where a rule cites a file, that file is the authority, not this document.

## 1. The one rule that matters most

**Source existing is not a gate being met.**

This project's entire discipline is that a milestone is accepted only when both source *and* runtime evidence exist (`Docs/ALPHA_DELIVERY_CHECKLIST.md:9`). No runtime evidence exists for anything. No Unreal build has ever run. No automation test has ever executed. No CI job has ever been created, let alone completed.

Therefore:

- Never write that a milestone is complete, verified, passed, green, ready, or signed off.
- Never tick a checkbox that requires runtime evidence. If a fact is source-only, either add a separate source-only row or record it as prose under the milestone — that is the established convention.
- Never describe a static check as if it were a runtime result.
- When you cannot verify something, say so in the same sentence as the claim. "Not verified, and cannot be from here" is a complete and acceptable answer.

## 2. Read-only means the working tree as well

An agent instructed to be read-only recently ran a checkout that reverted four committed fixes in the working tree and staged the reversion. The commits survived only because they had already been pushed.

If you are told to investigate, audit, explore, or report:

- Do not run `git checkout`, `git reset`, `git stash`, `git restore`, `git clean`, or `git apply`.
- Do not stage anything. Do not touch the index.
- Do not edit, create, or delete files — including files you consider temporary.
- To read another revision, use `git show <ref>:<path>` or `git diff <ref>..<ref>`. Never switch the tree to read it.
- Leave `git status --porcelain` exactly as you found it. If it was empty, it must still be empty when you finish.

## 3. Re-fetch before you plan

Work has twice been planned against a `develop` that was 54 commits stale, producing a duplicate ship implementation that had to be discarded.

- Run `git fetch --all --prune` and re-read `origin/develop` immediately before planning, not at the start of a long session.
- Before writing a new subsystem, grep for it. If a type with that responsibility already exists, extend or report it — do not write a second one.
- If your plan is more than a few minutes old and other work may have landed, re-check before implementing it.

## 4. Never invent authored content

The design bible is the authority. Where it does not lock a value, the value is unlocked, and that is information — not a gap for you to fill.

- Mark every unlocked number with a `DESIGN-GAP:` comment immediately above its `UPROPERTY`, stating what the source does lock and what it does not. Make it `EditDefaultsOnly` so it is tunable without a recompile.
- Do not invent missions, nodes, names, dungeons, bosses, dialogue, or numbers to reach a required count. Encode the locked total as a constant and author only what the bible actually specifies. Forty-nine of the sixty-eight skill-tree nodes are unauthored; that is recorded, not filled in.
- Where two design sources conflict, resolve by the canon order in `Docs/DesignAuthority.md:6-17` and record the ruling. Where the conflict is unresolvable from the documents, escalate it — do not pick.
- `Docs/Handoff/ENGINEERING_HANDOFF.md` states it directly: never invent a HUD element, a marker, a meter, a cutscene, a music cue, a romance option, a respec, or a fast-travel point.

## 5. Machine-enforced prohibitions

These are checked by `Source/DarkArisenOps/Private/ValidationOps.cpp` and by `static_assert` in `Source/DarkArisen/DesignLaws.h`. Violating one fails the build, so this is not a matter of taste:

- **No language other than C++** in `Source/` and `Tools/`. `Docs/NATIVE_CPP_POLICY.md:31` bans `.py`, `.ps1`, `.sh`, `.bash`, `.js`, `.ts`, `.cmd`, `.bat` there. C# is permitted only for `.Target.cs` / `.Build.cs` metadata.
- **No GitHub-hosted runners.** Workflows must contain `self-hosted`; `ubuntu-latest`, `windows-latest` and `macos-latest` are rejected (`ValidationOps.cpp:359-363`).
- **No `actions/upload-artifact`** anywhere in a workflow (`ValidationOps.cpp:371`). Build outputs stay on the runner's disk.
- **No UMG and no Slate.** The module depends only on Core, CoreUObject, Engine, InputCore and AIModule. Posture is the only permitted combat-HUD element.
- **No randomness in authored systems.** `FMath::Rand` and `FRandomStream` are forbidden in content code; all content is authored.

Naming a prohibited concept in order to lock it false — `bFastTravelOverWaterExists = false` with a `static_assert` — is enforcement, not a violation. That is the correct pattern.

## 6. Cost, secrets and outward-facing actions

`SECURITY.md` is the authority and it reserves these to the owner:

- **Never start a billable resource.** No GPU, no VM, no domain, no paid voice or asset service, no quota increase, no removal of the idle watcher. A vendor free tier or credit offer is not authorisation.
- **Never put a secret in output.** Not in a file, not in command output, not in a screenshot, not in an issue or PR comment. Registration tokens and API keys are secrets. GitHub Actions secrets cannot be read back by design — do not attempt to, and do not build workarounds.
- **Never merge, close, or force-push without explicit instruction for that specific action.** Approval to open a pull request is not approval to merge it. Approval given once is not standing approval.
- Voice generation stays blocked until dialogue lock plus a verified zero-cost and commercial-rights check.

## 7. Working in parallel

- One agent owns a disjoint set of files. Shared files — `DesignLaws.h`, the test spec, the validator, workflows, checklists, `HANDOVER.md`, `README.md` — are integrated by the orchestrator afterwards, never edited by parallel agents.
- If your task would require touching a shared file, report what should change and let the orchestrator apply it.
- Expect to be interrupted by session limits. Write files as you complete them rather than holding everything until a final report, so partial work survives.

## 8. Reporting

- Quote `file:line`. A claim without a location is not checkable and will be treated as unverified.
- Distinguish what you executed from what you read. If you ran a validator, say so and give its output. If you only read the code, say that instead.
- Report contradictions you find even when they are outside your task. The stale `Tools/ci` references across `README.md` survived several tranches because each agent treated them as someone else's problem.
- When your finding contradicts the instruction you were given, say so plainly rather than completing the instruction as written.
