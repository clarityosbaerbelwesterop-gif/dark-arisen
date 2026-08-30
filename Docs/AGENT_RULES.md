# Rules for AI agents working in this repository

**Applies to:** every AI agent, subagent, or automated session that reads or writes this repository — including agents spawned by another agent.

This is not general advice. Every rule below exists because the situation it describes has already happened here, or because a machine check in this repository already enforces it. Where a rule cites a file, that file is the authority, not this document.

## 1. The one rule that matters most

**Source existing is not a gate being met.**

This project's entire discipline is that a milestone is accepted only when both required source and required runtime/production evidence exist. Source work has advanced substantially, and GitHub Actions now parse and create Windows/Linux jobs, but the matching UE 5.5 self-hosted runners are not assigned/online. The observed jobs remain queued with no runner and therefore have executed no compile, Unreal Automation, runtime, performance, packaging or playthrough gate. **Queued is not passed.**

Therefore:

- Never write that a milestone is complete, verified, passed, green, ready, or signed off merely because source exists or a workflow run was created.
- Never tick a checkbox that requires runtime evidence. If a fact is source-only, either add a separate source-only row or record it as prose under the milestone — that is the established convention.
- Never describe a static check, authored Automation spec, queued workflow or external-media generation as if it were a runtime result.
- When you cannot verify something, say so in the same sentence as the claim.

## 2. Read-only means the working tree as well

An agent instructed to be read-only recently ran a checkout that reverted four committed fixes in the working tree and staged the reversion. The commits survived only because they had already been pushed.

If you are told to investigate, audit, explore, or report:

- Do not run `git checkout`, `git reset`, `git stash`, `git restore`, `git clean`, or `git apply`.
- Do not stage anything. Do not touch the index.
- Do not edit, create, or delete files — including files you consider temporary.
- To read another revision, use `git show <ref>:<path>` or `git diff <ref>..<ref>`. Never switch the tree to read it.
- Leave `git status --porcelain` exactly as you found it. If it was empty, it must still be empty when you finish.

## 3. Re-fetch before you plan

Work has twice been planned against a stale `develop`, producing duplicate work that had to be discarded.

- Re-read current `origin/develop` / connected-repository head immediately before planning, not only at the start of a long session.
- Before writing a new subsystem, search for it. If a type with that responsibility already exists, extend or report it — do not write a second one.
- If your plan is more than a few minutes old and other work may have landed, re-check before implementing it.

## 4. Never invent authored content

The design bible is the authority. Where it does not lock a value, the value is unlocked, and that is information — not a gap for you to fill.

- Mark every unlocked number with a `DESIGN-GAP:` comment immediately above its `UPROPERTY`, stating what the source does lock and what it does not. Make it `EditDefaultsOnly` so it is tunable without a recompile.
- Do not invent missions, nodes, names, dungeons, bosses, dialogue, or numbers to reach a required count. Encode the locked total as a constant and author only what the bible actually specifies.
- Where two design sources conflict, resolve by the canon order in `Docs/DesignAuthority.md` and record the ruling. Where the conflict is unresolvable from the documents, escalate it — do not pick.
- `Docs/Handoff/ENGINEERING_HANDOFF.md` states it directly: never invent a HUD element, a marker, a meter, a cutscene, a music cue, a romance option, a respec, or a fast-travel point.

## 5. Machine-enforced prohibitions

These are checked by `Source/DarkArisenOps/Private/ValidationOps.cpp` / tranche-specific native validators and by `static_assert` in `Source/DarkArisen/DesignLaws.h`. Violating one fails the relevant source/build gate, so this is not a matter of taste:

- **No language other than C++** in `Source/` and `Tools/`. `Docs/NATIVE_CPP_POLICY.md` bans `.py`, `.ps1`, `.sh`, `.bash`, `.js`, `.ts`, `.cmd`, `.bat` there. C# is permitted only for `.Target.cs` / `.Build.cs` metadata.
- **No GitHub-hosted runners.** Workflows must contain `self-hosted`; `ubuntu-latest`, `windows-latest` and `macos-latest` are rejected.
- **No `actions/upload-artifact`** anywhere in a workflow. Build outputs stay on the runner's disk.
- **No UMG and no Slate.** The module dependency boundary remains native/minimal and posture is the only permitted combat-HUD element.
- **No randomness in authored systems.** `FMath::Rand` and `FRandomStream` are forbidden in content code; all content is authored.

Naming a prohibited concept in order to lock it false can be valid enforcement. Do not introduce callable gameplay APIs that implement the prohibited behavior.

## 6. Cost, secrets and outward-facing actions

`SECURITY.md` is the authority and it reserves these to the owner:

- **Never start a billable resource or change a provider plan.** No GPU, VM, domain, paid voice/asset generation, subscription upgrade, credit purchase, quota increase or removal of the idle watcher without explicit operator approval. Existing free credits are not authorisation to upgrade a provider account or spend credits on generation.
- **Never put a secret in output.** Not in a file, command output, screenshot, issue or PR comment. Registration tokens and API keys are secrets. GitHub Actions secrets cannot be read back by design — do not attempt to, and do not build workarounds.
- **Never merge, close, or force-push without explicit instruction for that specific action.** Approval given for an earlier PR/tranche is not standing approval for a later one.
- Shipping voice generation stays blocked until dialogue lock, casting role, pronunciation/subtitle lock and verified cost/commercial-rights acceptance.
- External provider generation must perform plan/cost/rights preflight and remain non-shipping evidence until real import/review/runtime acceptance exists.

## 7. External AI, asset providers and the non-AI-slop gate

- `CLAUDE.md` configures the adapted Game Studios methodology and is present in the repository.
- `Docs/NON_AI_SLOP_STANDARD.md` is mandatory for authored content, provider briefs and external-media review. It is a rejection gate, not optional art-direction prose.
- Before accepting an important proposal/candidate, apply its ordered review passes: canon, function, silhouette/composition, physical credibility, restraint, repetition and runtime role.
- A mood-only/style-adjective-only prompt is rejected before provider submission. Every provider brief needs real governing sources, source-specific anchors, explicit unknowns and a declared production purpose.
- Game Studios profiles and Kimi K3 are production methodology/review tools, never canon authority.
- Kimi output cannot auto-merge, create missing authored missions/content, resolve a `DESIGN-GAP`, or silently add decorative completion.

### Current Higgsfield scope

Higgsfield is currently approved only for:

- animation/motion reference;
- body/facial performance reference for already-authored beats;
- cinematic/cutscene previs for resolved presentation identities.

The current Higgsfield queue is exactly 23 animation/performance briefs plus 14 resolved cinematic briefs. It does **not** include character looks, boss looks, dungeons, worlds, Highmoore anchors, ships, fauna, flora, props, general 3D/GLBs, dialogue or missing story.

A generated video is not an AnimSequence, AnimMontage, AnimNotify, retargeted animation, Sequencer asset or runtime evidence. Provider output that fills a withheld fact or design gap is rejected even when visually strong. Current internal character/boss/ship/fauna/flora/3D readiness catalogs remain production-planning authorities, not Higgsfield job queues.

The tested Higgsfield Seedance path is currently plan-gated before job creation; no provider upgrade may be performed automatically.

### Voice

A voice audition is not a shipping/commercially-cleared voice asset. AI Voice Generator stays a separate late-stage lane after dialogue/casting/pronunciation/subtitle/cost/rights locks.

## 8. Main-story boundary

After the pre-main production families are reconciled, `Docs/MAIN_QUEST_AUTHORITY_GAP.md` is the stop boundary. Do not infer complete main missions from `main story structure.md` chapter summaries. Do not restore the legacy Ethan/Draven final-boss branch unless current design authority explicitly resolves or restores it.

## 9. Working in parallel

- One agent owns a disjoint set of files. Shared files — `DesignLaws.h`, test specs, validators, workflows, checklists, `HANDOVER.md`, `README.md` — are integrated by the orchestrator afterwards, never edited by parallel agents.
- If your task would require touching a shared file, report what should change and let the orchestrator apply it.
- Expect to be interrupted by session limits. Write files as you complete them rather than holding everything until a final report, so partial work survives.

## 10. Reporting

- Quote concrete file paths/lines when evidence is available. A claim without a location is not checkable and should be treated as unverified.
- Distinguish what you executed from what you read. If you ran a validator, say so and give its output. If you only read the code/spec, say that instead.
- Report contradictions you find even when they are outside your task.
- When your finding contradicts the instruction you were given, say so plainly rather than completing the instruction as written.
