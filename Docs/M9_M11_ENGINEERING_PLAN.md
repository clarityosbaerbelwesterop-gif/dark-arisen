# M9-M11 engineering plan

**Status:** operator-defined engineering continuation after M0-M8  
**Branch:** `feature/m9-m11-alpha-completion`  
**Base:** `develop` at PR #9 merge `969865973b59079cc163aeece976574b095dfc44`  
**Engine:** Unreal Engine 5.5  
**Implementation language:** native C++ for executable project logic

## Authority

M9-M11 did not exist as named repository milestones before 2026-08-28. They are created under the operator's current instruction to continue beyond M8. They do **not** add new story, quest, character, world, economy, combat, platform or presentation canon. They only reorganize the already-open Alpha completion work into three engineering milestones.

Canon precedence remains:

1. current operator direction;
2. `HANDOVER.md` and `Docs/DesignAuthority.md`;
3. the twelve locked design laws;
4. newest narrow subsystem source;
5. phase masters and older non-conflicting sources.

No missing authored narrative content may be invented to satisfy these milestones. Missing assets, dialogue, animation, level geometry, music/licence data or runtime evidence remain explicit open requirements.

## M9 — Runtime & Authored Content Integration

Purpose: turn the M0-M8 C++ system boundaries into one auditable authored-runtime integration surface without creating a second gameplay authority.

### Source deliverables

- Add a native C++ authored-runtime manifest and readiness registry.
- Track required runtime integration classes without pretending that a referenced asset has been authored or tested:
  - level/world partition content;
  - navigation and AI placement;
  - interaction anchors;
  - animation/montage/notify bindings;
  - audio and non-reactive music bindings;
  - NPC/population/fauna placement;
  - dungeons, bosses, ship, colonial war and Highmoore authored presentation;
  - cutscene bindings for the canonical nineteen only;
  - explicit protection of the twenty-two playable non-cutscenes.
- Stable IDs and governing-source references are mandatory.
- Runtime references fail closed when duplicated, source-less, missing a required class, or mapped to a prohibited presentation mode.
- No Blueprint-only gameplay authority. Assets may provide meshes, animation, sound, levels, data and presentation only.
- No runtime gate is marked passed merely because a manifest entry exists.

### Acceptance boundary

M9 source closure means the native integration contract exists. M9 runtime acceptance still requires the authored content to exist in UE 5.5 and be observed in engine.

## M10 — Full-Game Verification

Purpose: define one C++ evidence authority for the exact candidate build rather than scattered manual claims.

### Source deliverables

- Add a native verification ledger keyed by full source commit and candidate SHA-256.
- Evidence categories:
  - Windows UE 5.5 compile;
  - Linux UE 5.5 compile;
  - Unreal Automation suite;
  - content-manifest reconciliation;
  - save-schema/migration verification;
  - Windows/Linux save compatibility;
  - controller/input parity;
  - accessibility parity;
  - full start-to-credits Windows playthrough;
  - full start-to-credits Linux playthrough;
  - 60-fps floor evidence for required benchmark segments;
  - no design-law regression;
  - privacy/access-denial evidence.
- Evidence is append-only per immutable candidate identity and cannot be copied between hashes.
- Runtime or human evidence cannot be self-certified by source code.
- Unknown/missing evidence blocks completion rather than defaulting true.

### Acceptance boundary

M10 source closure means the evidence model and fail-closed gates exist. M10 acceptance requires the actual Windows/Linux runner and playthrough/performance evidence.

## M11 — Private Alpha Release & Sign-off

Purpose: connect the M8 release contracts and DarkArisenOps packaging logic to one final private-Alpha acceptance state.

### Source deliverables

- Reuse `AlphaReleaseContracts` and `DarkArisenOps`; do not create duplicate packaging logic.
- Add a native final sign-off coordinator requiring:
  - one exact 40-character source commit;
  - immutable Windows and Linux candidate SHA-256 values;
  - same content-manifest revision;
  - all M10 evidence gates green for those exact hashes;
  - complete credits attribution;
  - approved/licensed non-reactive credits cue;
  - privacy/access-denial evidence;
  - explicit operator approval.
- Final promotion may rename/copy verified candidate bytes but may not rebuild or recompress them.
- No GitHub public release, Actions artifact, public bucket, public game URL or repository-hosted secret.
- Native PS5 stays Beta-only and is not an M11 Alpha condition.
- No paid GPU, hosting, voice, music, asset, domain or platform purchase is authorized by this milestone.

### Acceptance boundary

M11 source closure means the final acceptance authority exists. Actual Alpha sign-off remains impossible until real immutable candidates and external/operator evidence exist.

## Verification strategy

- Add native Unreal Automation Specs for M9-M11 state contracts.
- Extend `DarkArisenOps validate` to require the M9-M11 C++ owners and reject duplicate/non-C++ authority.
- Keep M0-M8 laws and validation intact.
- Self-hosted runner jobs remain deferred while unavailable. Their absence does not block source authoring, but no compile/runtime/performance/package result may be reported as passed.

## Documentation cleanup in this tranche

- Repair the stale top-level `HANDOVER.md` status so it reflects PR #9 as the integrated baseline.
- Update Alpha checklist wording from the removed Python validator stack to native C++/Unreal Automation.
- Add an M9-M11 source-completion record only after implementation exists.

## Merge policy

PR #10 may be merged only by explicit operator authorization. A source merge does not equal runtime, packaging or Alpha acceptance.
