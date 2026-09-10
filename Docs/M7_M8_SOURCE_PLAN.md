# M7 / M8 source implementation plan

**Branch:** `feature/m7-m8-source-completion`
**Base:** PR #8 merge `e54d39af4534f026cc36525e19df9c9d20980abf`
**Scope:** M7 Content Scale-Out + M8 Alpha platform/release source contracts. UE 5.8 authored-runtime, packaging and operator acceptance remain evidence gates, not source claims.

## Governance / authority

Implementation order:

1. Flo's current written direction.
2. `HANDOVER.md` and `Docs/DesignAuthority.md`.
3. Locked design laws and newest narrow subsystem documents.
4. Phase master documents.
5. Older non-conflicting drafts.

`CLAUDE.md` is absent from `develop` and repository search does not locate an equivalent governance file. The current handover and DesignAuthority therefore remain the implementation authority; the missing file is recorded rather than silently reconstructed.

M8 follows the current DesignAuthority: Alpha targets **Windows x64 + Linux x86_64**. Native PS5 work is Beta-only and is not represented as M8 Alpha completion.

## Native C++ implementation rule

All executable Dark Arisen project logic is native C++.

- Gameplay, state, content validation, save/migration, accessibility and platform/release contracts live under `Source/DarkArisen` with native Unreal Automation Specs.
- Repository validation, runner preflight, build/package/promotion and Pixel Streaming operations live in the native `DarkArisenOps` C++ program under `Source/DarkArisenOps`.
- `.Target.cs` / `.Build.cs` remain Unreal Build Tool metadata only. Workflow YAML, `.uproject`, `.ini`, JSON/YAML/HUJSON/Caddy configuration and Markdown remain data/orchestration formats, not alternate implementation layers.
- `Source/` and `Tools/` may not contain Python, PowerShell, Bash, JavaScript or TypeScript implementation files.
- `UDataAsset` / `UObject` assets may carry authored data but cannot override governing C++ rules or become a second gameplay authority.

## Implementation checklist

### A. M7 authoritative content manifest

- Add one source-owned manifest contract covering exactly:
  - 61 dungeon sites = 41 named + 20 minor.
  - 17 Threads.
  - 132 Turns.
  - 147 authored Standing variants across the locked nine mission types: Escort 21, Convoy Raid 18, Recovery 24, Champion 12, Transport 16, Privateer Commission 14, Road Work 15, Hunt 13, Salvage 14.
  - nine Tier-1 bosses through `Docs/M7_TIER1_BOSS_REGISTER.md`.
  - 19 cutscenes and all 22 protected player-controlled moments from `cutscene catalog.md` Section 6.
- Fail closed on duplicate stable IDs, missing governing-source references, incorrect counts and any generated/radiant content.
- Do not fabricate narrative definitions where the bible does not individually name every entry. The contract validates authored definitions rather than creating filler.

### B. M7 system scale-out boundaries

- Add finite Standing-mission pool state. A region/type pool stops offering work when its authored variants are exhausted; there is no replacement generator.
- Same-region/type pay degrades to exactly 60% by the sixth run while Standing gain does not degrade. Canon does not define the intermediate percentages, so C++ validates a six-point authored monotonic curve instead of inventing them.
- Enforce dungeon laws: no markers, no ambient dungeon music, no child-remains flag, Tier-B+ return shortcut required, Crystal Caves the sole >90-minute carve-out and bounded to its authored 90–120 minute range.
- Protect the 22 Section-6 presentation moments from becoming Sequencer-owned cutscenes.
- Route M7 integration records through existing M2-M6 authorities for world time, war, progression/economy, Princess state and Rexa settlement state; no parallel authority is introduced.
- Add dialogue-production readiness records for dialogue lock, casting role, pronunciation reference and subtitle readiness. These are readiness metadata only; no voice/licence/vendor claim is invented.
- Add end-credit readiness requiring names/order, attribution, music source, licence, cost approval and non-reactive music before credits are production-ready.

### C. M8 Windows/Linux Alpha platform contracts

- Lock Alpha platform parity to exactly Windows x64 + Linux x86_64.
- Accessibility/difficulty contract includes remapping, subtitle sizing and colourblind presentation; difficulty may change damage/health only and can never alter the six-frame deflection timing.
- Save schema starts at explicit version 1. There is no fabricated historical migration. Unknown/pre-v1 and future schemas fail closed; future accepted migrations must be explicit contiguous one-version C++ steps.
- Release manifest requires both Shipping candidates to originate from the same 40-character source commit and content-manifest revision.
- Candidate metadata fails on deployment credentials, TURN secrets, private hostnames, public-link configuration or Sony/native-PS5 material.
- Source, Windows and Linux archives require SHA-256 identity without public artifact upload.
- Acceptance ledger keys evidence to the exact candidate hashes. Source code validates supplied evidence state but cannot self-certify playthrough, frame time, browser access or operator approval.

### D. Release / credits / completion gates

- Main-story completion may enter full credits only when the credits manifest is production-ready.
- Credits music must be explicitly approved, licensed and non-reactive. Missing approval/licence/cost data blocks readiness.
- No paid GPU, voice, asset, domain, runner or hosting action is authorized by this tranche.
- AI voice generation remains blocked until zero-cost and commercial-rights terms are verified and dialogue lock exists.

### E. Verification

- `DarkArisenOps validate` is the fail-closed deterministic repository/source validator. There is no Python validator layer.
- Native Unreal Automation Specs cover negative and positive cases for:
  - generated/radiant mission rejection and finite pool exhaustion,
  - exact Standing type counts and sixth-run 60% pay anchor,
  - dungeon markers/music/child-remains,
  - converting a protected playable moment to a cutscene,
  - PS5 Alpha leakage,
  - difficulty changing deflection timing,
  - unknown/future save schemas,
  - packaging candidates from different commits,
  - secrets/private host configuration in candidate manifests,
  - missing credits music licence/approval,
  - exact-hash Alpha acceptance gating.
- CI builds `DarkArisenOps` first, runs its validator/preflight, then compiles and executes native Unreal Automation on self-hosted UE 5.8 runners. Existing design/security gates are not disabled to force a merge.

### F. Documentation / handoff

- Keep `Docs/ALPHA_DELIVERY_CHECKLIST.md` runtime/content checkboxes open until their required evidence exists.
- Record source-closure boundaries in `Docs/M7_M8_SOURCE_COMPLETION.md`.
- Keep `HANDOVER.md` synchronized with PR #9, runtime blockers and the next engineering line.

## Merge policy for PR #9

PR #9 may be merged only after source diff audit and all available non-deferred validation gates pass. UE 5.8 runtime/packaging/performance/playthrough gates that cannot execute remain explicitly open and may not be re-labelled as passed. No branch protection, check, design-law or security gate may be weakened to force the merge.
