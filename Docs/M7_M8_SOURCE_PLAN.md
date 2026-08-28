# M7 / M8 source implementation plan

**Branch:** `feature/m7-m8-source-completion`  
**Base:** PR #8 merge `e54d39af4534f026cc36525e19df9c9d20980abf`  
**Scope:** M7 Content Scale-Out + M8 Alpha platform/release source contracts only. UE 5.5 authored-runtime, packaging and operator acceptance remain evidence gates, not source claims.

## Governance / authority

Implementation order:

1. Flo's current written direction.
2. `HANDOVER.md` and `Docs/DesignAuthority.md`.
3. Locked design laws and newest narrow subsystem documents.
4. Phase master documents.
5. Older non-conflicting drafts.

`CLAUDE.md` is absent from `develop` and repository search does not locate an equivalent governance file. This plan therefore treats the current handover and DesignAuthority as the active implementation authority and records the missing file as an unresolved repository-governance defect rather than silently inventing it.

M8 follows the current DesignAuthority: Alpha targets **Windows x64 + Linux x86_64**. Native PS5 work is Beta-only and is not represented as an M8 Alpha completion.

## Implementation checklist

### A. M7 authoritative content manifest

- Add one source-owned manifest contract covering exactly:
  - 61 dungeon sites = 41 named + 20 minor.
  - 17 Threads.
  - 132 Turns.
  - 147 authored Standing mission variants across the locked 9 mission types and exact per-type counts.
  - 9 Tier-1 bosses through a reconciled authoritative boss register.
  - 19 cutscenes and 22 explicitly playable non-cutscene moments.
- Fail closed on duplicate stable IDs, missing governing-source references, incorrect counts and prohibited generated/radiant content.
- Do **not** fabricate narrative definitions where the bible does not individually name every entry. The source contract validates completeness against authored definitions rather than generating filler.

### B. M7 system scale-out boundaries

- Add finite Standing-mission pool state with exhaustion and same-region/type pay degradation to 60% by the sixth run; Standing gain does not degrade.
- Add global dungeon-definition validation: no markers, no ambient dungeon music, no child-remains flag, Tier-B+ return shortcut required, Crystal Caves as the sole >90-minute carve-out.
- Add authoritative cutscene registry validation and a hard playable-moment prohibition set so Section-6 moments cannot be accidentally sequencer-owned.
- Integrate Assassin Network, Highmoore reconstruction, population/fauna and content-state dependencies through existing M2-M6 authorities; no parallel war/time/save/progression systems.
- Add dialogue-production readiness records: dialogue lock, casting role, pronunciation reference and subtitle readiness. These are readiness metadata only; no invented performance, voice, licence or vendor claim.
- Add end-credit readiness manifest requiring names/order/attribution/music-source/licence/cost approval before credits can be marked production-ready.

### C. M8 Windows/Linux Alpha platform contracts

- Add a two-platform parity manifest for Windows x64 and Linux x86_64 only.
- Add controller/input parity and accessibility contract:
  - remapping,
  - subtitle sizing,
  - colourblind presentation options,
  - permitted difficulty modifiers may affect damage/health only and never the six-frame deflection timing.
- Add save-schema version and deterministic migration registry. Unknown/future versions fail closed; migration paths are explicit and testable.
- Add packaging/release manifest requiring both Shipping candidates to originate from the same source commit and content-manifest revision.
- Add privacy/secrets scan contract for package manifests: no deployment credentials, TURN secrets, private hostnames, public-link configuration or Sony/native-PS5 material in Alpha candidates.
- Add SHA-256 sidecar contract for source, Windows and Linux candidate archives without uploading them publicly.
- Add start-to-credits acceptance ledger keyed by exact candidate hashes. Source code cannot self-certify playthrough, frame-time, browser access or operator approval.

### D. Release / credits / completion gates

- Main-story completion may enter full credits only when the credits manifest is production-ready.
- Credits music must be explicitly approved, licensed and non-reactive. Missing approval/licence/cost information blocks readiness.
- No paid GPU, voice, asset, domain, runner or hosting action is authorized by this tranche.
- AI voice generation remains blocked until zero-cost and commercial-rights terms are verified and the dialogue lock exists.

### E. Verification

- Add `Tools/ci/validate_m7_m8.py` as a fail-closed deterministic source validator.
- Add negative regression tests for:
  - radiant/infinite quest generation,
  - dungeon markers/music/child-remains,
  - converting a protected playable moment to a cutscene,
  - PS5 Alpha leakage,
  - difficulty changing deflection timing,
  - packaging candidates from different commits,
  - missing credits music licence/approval,
  - secret/host configuration in a candidate manifest.
- Add native Unreal Automation Specs for finite mission exhaustion/pay degradation, content-count structures, save migration, accessibility timing invariants and release-manifest parity.
- Extend CI additively from M0-M6 to M0-M8; no existing gate is removed or weakened.

### F. Documentation / handoff

- Update `Docs/ALPHA_DELIVERY_CHECKLIST.md` with source-closure prose while leaving runtime/content checkboxes open unless their required evidence exists.
- Add `Docs/M7_M8_SOURCE_COMPLETION.md` with exact implemented/deferred boundaries.
- Update `HANDOVER.md` to the PR #9 state, runtime blockers and exact next engineering line.

## Merge policy for PR #9

PR #9 may be merged only after source diff audit and available non-deferred validation gates pass. UE 5.5 runtime/packaging/performance/playthrough gates that cannot execute remain explicitly open and must not be re-labelled as passed. No branch protection, check, design-law or security gate may be disabled or weakened to force the merge.
