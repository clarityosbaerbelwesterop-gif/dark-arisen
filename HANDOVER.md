# Engineering handover

**Updated:** 2026-08-27  
**Milestone:** M0 — Foundation and remote play path  
**Merge state:** Do not merge; runtime gates have not executed.

## Completed in the feature branch

- Converted the documentation-only repository into an Unreal Engine 5.5 C++ project boundary.
- Replaced the four isolated headers with matching implementations and corrected baseline HP/stamina.
- Added a native third-person pawn, camera, keyboard/gamepad input, two-zone touch input, sprint, damage/death, stamina, posture, attacks, dodge, six-frame parry, and unlock-gated Rache.
- Added a procedural greybox and JSON frame-time telemetry against the 16.6 ms budget.
- Encoded the twelve load-bearing design laws in constants, static assertions, Unreal automation tests, and a deterministic source audit.
- Added private self-hosted CI so the repo does not rely on exhausted GitHub-hosted Actions minutes.
- Added the complete M0 Pixel Streaming automation path: pinned Epic frontend/infrastructure, secure signalling/private TURN, iPad UI, NVENC proof, versioned deployment/rollback, services, firewall rules, evidence collection, and provider-level idle stop.
- Read and indexed all 185 repository documents, including the extensionless DOCX GDD, and added the two current handoff sources to the repository.
- Recorded the canon hierarchy, legacy conflicts, malformed paths, and five unresolved explicit cross-references.
- Selected AWS G6.2xlarge in Frankfurt as the provisional M0 host architecture with EBS-backed saves; added a strict-private Windows Server 2022 CloudFormation/Tailscale implementation that cannot launch without cost approval. This is not a provisioning authorization.
- Audited the supplied AWS UE4 sample and SkyPilot. Neither supplies free GPU capacity; SkyPilot is BYOC orchestration and is not the selected Windows UE runtime.
- Added fail-closed validation for the private AWS/Tailscale contract, including public-port, wildcard-identity, Funnel, charge-acknowledgement, retained-storage, and private-service checks.
- Hardened the M1 greybox path: broken posture cancels a queued strike, invalid Examine targets cannot strand the camera, death enters the authored four-second Hold, lock-on uses a 20 m acquisition/25 m retention leash, and Rache has its locked keyboard/stick-combination input gate.

## Deliberately not done

- No GPU host, VM, domain, certificate, TURN endpoint, or paid service was provisioned or started.
- No credential or voice-generation job was created. Flo expects the connected AI Voice Generator to be free, but generation remains blocked until dialogue lock and a verified zero-cost/commercial-rights check.
- No Unreal compile, package, browser stream, iPad input test, or provider deallocation test is claimed as passed.
- No Alpha/Beta or full-content build is claimed. M0 is the prerequisite for visible iteration, not the full game.
- UE 5.8 was not adopted during M0 because the approved handoff and Pixel Streaming infrastructure are pinned to 5.5; an upgrade requires a separate compatibility milestone.

## Next operator actions

1. Attach a private Windows UE 5.5 self-hosted runner and let the feature PR execute compile/tests.
2. Resolve any real UE compiler or automation failures; do not suppress checks.
3. Flo selects and operates the GPU host and creates the private browser link. Repository work supplies the reviewed project, packaging/deployment scripts, and eventual private ZIP; it does not provision or publish the endpoint.
4. On Flo's selected host, apply the exact one-user access policy and configure machine-scoped secrets/provider identity according to the relevant private deployment guide.
5. Deploy the exact PR revision, run `collect-host-evidence.ps1`, and capture all six gates in `M0_CHECKLIST.md` without placing the private URL or password in GitHub.
6. Merge only after all evidence is green, first to `develop` and then to `main` at milestone completion.

## Known external blockers

- The repository currently exposes no active Unreal/GPU host or self-hosted runner. Flo has taken ownership of the GPU and private-link step; the evidence is still pending.
- GitHub-hosted Actions availability is account-scoped; this repo had no workflows or historical runs before M0. The new workflow intentionally requires a self-hosted machine.
- Pixel Streaming Infrastructure is free software hosted on GitHub. Epic's documentation does not include a free external GPU computer with it; if Flo has a separate Epic-host entitlement, its concrete instance/dashboard URL is required.
