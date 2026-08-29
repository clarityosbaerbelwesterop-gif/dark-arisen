# Pre-runner asset production tranche

**Date:** 2026-08-30  
**Base:** `develop` at `04a34e37f4f0c2f982cf28b942e11e120d103d29`  
**Branch:** `feature/pre-runner-asset-production`  
**Stop boundary:** continue until the next required work would need unauthored Quest/Turn/Standing identities or a runner/editor fact that cannot be produced from repository authority.

## Operator direction

Continue on the Rockstar path. Do not optimise for a short route to an Alpha label. Use the already-adapted Claude Code Game Studios methodology and NVIDIA/Kimi only as bounded production assistants. Add Higgsfield as a non-canon external asset/previsualisation provider for work it can actually perform, without pretending a generated image/video/GLB is already an accepted Unreal asset. Final voice remains a later, separately gated production stage.

## Current repository fact check

- `develop` still points to PR #17 merge `04a34e37f4f0c2f982cf28b942e11e120d103d29`; no later GitHub branch/PR/commit was found at tranche start.
- `CLAUDE.md` now exists and configures the adapted `Donchitos/Claude-Code-Game-Studios` workflow.
- `.claude/agents/*`, `.claude/skills/team-level/SKILL.md` and `.github/workflows/game-studios-kimi.yml` already exist; do not duplicate them.
- Higgsfield workspace is on the free plan with 10 credits. Read-only preflight showed Seedance 2.0 Mini costs 4 credits for a 4 s / 480p clip, but actual submission returned `Requires basic plan or higher` before any job was created. Treat Higgsfield generation as provider-plan blocked until the operator changes that provider state; do not buy/upgrade automatically.

## Work order

1. **Governance reconciliation**
   - update `HANDOVER.md`, `Docs/ALPHA_DELIVERY_CHECKLIST.md` and stale agent statements to the PR #17 merge baseline;
   - preserve source/runtime/asset evidence separation;
   - record that `CLAUDE.md` and Game Studios are now present.

2. **External asset production authority**
   - create a native C++ production catalog for pre-runner asset families that maps existing canonical requirements to output/evidence classes;
   - support concept/reference image, motion-previs video, 3D GLB candidate, facial/performance reference and audio/voice audition as distinct non-shipping classes;
   - never mark an external generation as a UE production asset until actual import/review/runtime evidence exists.

3. **Higgsfield production bridge documentation**
   - document which Higgsfield capabilities are useful: images, reference-consistent video, 3D GLB, rigging/canned animation, video analysis;
   - document what it cannot replace: UE AnimMontage/AnimNotify ownership, retargeting, Sequencer integration, gameplay AI, navmesh, final level placement, save/load, performance or packaging;
   - require cost/plan/privacy/rights preflight before generation and record job identity/provenance when used.

4. **Animation/presentation production briefs**
   - derive provider-safe briefs from existing canonical animation/presentation catalogs;
   - do not invent Jake/crew visual identity where the repository has no approved visual reference;
   - for combat, provider video is motion reference only unless a real rigged animation artifact can be imported and reviewed.

5. **World/prop/3D production readiness**
   - identify source-backed world anchors, state treasures, unique objects and dungeon visual moments that can receive concept/3D candidate briefs without inventing layout coordinates or missing canon;
   - keep all `.umap`/`.uasset` claims false until real UE evidence exists.

6. **Quest boundary reconnaissance**
   - re-scan authored quest sources only to determine the exact next blocker;
   - do not create any of the 129 missing Turn identities or 146 missing Standing identities;
   - stop this tranche when further progression requires those unauthored identities, and report the exact gap count/source.

## Evidence classes

Every external-media record must use one of these states:

- `RequirementOnly` — canonical requirement exists, no external generation.
- `PrevisGenerated` — external image/video/reference exists, not a game asset.
- `CandidateAssetGenerated` — GLB/audio/etc. exists, still unimported/unreviewed.
- `ImportedForReview` — real Unreal import exists, but no runtime acceptance yet.
- `RuntimeAccepted` — runner/editor/runtime evidence exists on the exact commit.

Only the last two states imply actual Unreal content exists, and `RuntimeAccepted` requires evidence.

## Cost and rights law

- No provider upgrade, credit purchase, paid generation, licensing purchase or billable resource without explicit operator approval.
- Existing free credits do not override a provider-plan block.
- External output may be used as non-shipping previs before rights are cleared, but cannot be called shipping/commercially cleared until the provider terms and project use are explicitly accepted.
- Voice remains blocked for shipping until dialogue lock, casting role, pronunciation record, subtitle text and rights/cost approval exist.

## Merge boundary

This tranche may merge source/docs/tests that improve production authority and provenance even if Higgsfield itself remains provider-plan blocked. It may not claim any generated media, UE compile, automation, final assets, 60 fps, package, playthrough or Alpha acceptance that was not actually produced and evidenced.