# Milestone 0 acceptance checklist

Milestone 0 is complete only when every required item below has reproducible evidence. “Implemented” is not equivalent to “verified.”

## Repository foundation

- [x] Unreal Engine association is pinned to 5.5.
- [x] Runtime module, editor/game targets, configuration, and Git LFS rules exist.
- [x] The greybox room and Jake pawn are created without an unreviewable placeholder map asset.
- [x] Baseline HP is 200 and stamina is 120.
- [x] The twelve laws are represented in `DesignLaws.h`; detectable prohibitions have CI checks.
- [x] Server-side frame-time windows report p50, p95, maximum, and the percentage over the 16.6 ms budget.
- [x] CI targets a private self-hosted UE 5.5 Windows runner and consumes no GitHub-hosted runner minutes.
- [x] Runner preflight fails closed on wrong OS/architecture, wrong engine, missing Git LFS/Python, or insufficient disk.

## Pixel Streaming implementation

- [x] Pixel Streaming 2 is enabled and the compatible Epic infrastructure revision is pinned.
- [x] Launch flags lock 1920×1080, 60 fps project output, offscreen rendering, and local signalling.
- [x] NVENC must be detected and proven in the Unreal log; software fallback fails deployment.
- [x] Signalling, TURN, gateway, and idle watcher have auto-restart services and rotated logs.
- [x] HTTPS and password authentication protect the public endpoint; signalling/API ports remain private.
- [x] One player is allowed; touch, native gamepad input, fullscreen, and a latency readout are enabled.
- [x] Deployment uses versioned releases, an atomic active pointer, exact-PID shutdown, and rollback without deleting the prior release.
- [x] A provider adapter must stop/deallocate billing after 30 idle minutes.
- [ ] A GPU host has been explicitly selected and approved by Flo. **No paid host has been provisioned.**

## Six mandatory merge gates

| Gate | Required evidence | Status |
|---|---|---|
| 1. Development + Shipping compile | UE 5.5 CI logs, zero errors and zero warnings | Not run — UE runner unavailable |
| 2. Automated tests | Python checks and Unreal `DarkArisen.*` automation report | Python: passed locally 2026-08-26 (8 tests + validator + audit); Unreal: not run |
| 3. In-engine smoke test | Greybox movement, camera, jump, sprint, death, Rache gate and frame telemetry recording | Not run |
| 4. iPad input | Safari video showing left-stick movement, right-drag look, and paired DualSense input | Not run |
| 5. Stream quality | HTTPS/password, mobile-network TURN, NVENC, 1080p60, visible latency under 120 ms | Not run |
| 6. Cost shutdown | Provider event/audit log proving deallocation after 30 minutes with zero players | Not run |

The feature PR must remain draft and unmerged while any cell above is not green. At milestone completion, merge into `develop`, re-run the full gate set, then merge/tag `main`.

## Design sections used by M0

- `technical targets.md` §1 and §3 — 60 fps floor and performance budget.
- `progression overview.md` §1.1–1.4 — three non-convertible axes.
- `interaction system.md` §1 — no highlight/fade and 1.4 m corner prompt.
- `camera system.md` §1.4 — prohibited camera vocabulary.
- `cutscene catalog.md` §6 and §7.2 — nineteen authored sequences and twenty-two playable moments.
- `travel system.md` §5 — real water travel and three authored land entries.
- `time and rest.md` §5 — chapter/rest autosaves and lake suppression.
- `skill tree.md` §7 — no conversion or respec.
- `docs/design/npcs/children_and_families.md` §1 and §10.1 — engine-level child protection.
- `social system.md` §7 — no systemic romance.
- `princess quest belos.md` §6 and §9.5 — the undercroft is never rendered.
- `animation system.md` §4 — no animation cancelling.

## Design-corpus evidence

- [x] All 185 repository documents were inventoried; the extensionless GDD was decoded as DOCX and read.
- [x] Both current handoff documents were added under `Docs/Handoff/`.
- [x] Canon conflicts and superseded GDD clauses are recorded in `Docs/DesignAuthority.md`.
- [x] The full path/title inventory is recorded in `Docs/DesignSourceRegister.md`.
- [x] The GPU and persistent-storage recommendation is recorded without provisioning in `Docs/GpuBackendDecision.md`.
