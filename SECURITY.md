# Security and cost controls

## Scope

M0 exposes a single-player Unreal instance to one authenticated browser. The public surface is Caddy on TCP 80/443 and coturn on TCP/UDP 3478 plus UDP 49160–49200. Signalling, its REST API, streamer WebSocket, and SFU ports remain blocked from external ingress.

## Required controls

- Keep the GitHub repository and self-hosted runner private. The workflow rejects pull requests from forks.
- Run the CI runner and streaming services on dedicated machines or isolated service identities. Do not attach unrelated organization repositories to either runner.
- Require manual approval on the `m0-streaming` GitHub environment before any deployment workflow can start.
- Store GitHub deployment values in environment secrets and host runtime values as machine-scoped environment variables. Never place them in repository files, command output, screenshots, issues, or PR comments.
- Use a unique Caddy bcrypt password hash and unique milestone-scoped TURN username/credential. Rotate both after disclosure or at milestone close.
- Never launch Unreal with `-AllowPixelStreamingCommands`.
- Keep Caddy automatic HTTPS enabled; do not bypass certificate validation for an iPad test.
- Preserve the inbound block rule for TCP 8080, 8888, and 8889. Provider security groups must independently allow only HTTPS and the documented TURN ports.
- Treat `C:\ProgramData\DarkArisen` as sensitive. Runtime TURN/signalling files are ACL-restricted to the service identity and SYSTEM.

## Cost boundary

Repository scripts may build and validate without provisioning infrastructure. Creating or starting a billable GPU, purchasing a domain, enabling a paid voice/asset service, increasing quota, or removing the idle shutdown requires Flo's explicit prior approval.

The idle watcher must invoke a validated provider adapter that stops or deallocates the billed resource. A Windows shutdown is not sufficient. The final gate requires provider-side evidence after 30 minutes with zero players.

## Reporting a problem

Do not open a public issue containing secrets or host details. Revoke/rotate the affected value first, stop public access if needed, preserve sanitized logs, and report privately to the repository owner.
