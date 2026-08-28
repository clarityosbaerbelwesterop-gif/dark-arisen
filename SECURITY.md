# Security and cost controls

## Scope

M0 exposes a single-player Unreal instance only inside a private Tailscale tailnet. The selected AWS security group exposes none of the game, HTTPS, TURN, RDP, SSH, WinRM, signalling, REST, streamer WebSocket, or SFU ports publicly. UDP 41641 is the sole inbound internet transport and carries authenticated Tailscale WireGuard packets, not a playable endpoint.

## Required controls

- Keep the GitHub repository and self-hosted runner private. The workflow rejects pull requests from forks.
- Run the CI runner and streaming services on dedicated machines or isolated service identities. Do not attach unrelated organization repositories to either runner.
- Require manual approval on the `m0-streaming` GitHub environment before any deployment workflow can start.
- Store GitHub deployment values in environment secrets and host runtime values as machine-scoped environment variables. Never place them in repository files, command output, screenshots, issues, or PR comments.
- Use a dedicated one-user Dark Arisen tailnet and apply the complete deny-by-default policy naming Flo's exact login and `tag:dark-arisen`; no wildcard source, destination, default allow-all reachability, shared machine, share link, or invited user is permitted.
- Use a one-off, pre-approved, tagged auth key. Read it once from an exact SSM SecureString ARN via a file reference, then revoke the key and delete the parameter. Never print or commit it.
- Keep Tailscale Funnel disabled. Tailscale Serve is the only HTTPS exposure and remains tailnet-only.
- Use a unique milestone-scoped TURN username/credential and rotate it after disclosure or at milestone close.
- Never launch Unreal with `-AllowPixelStreamingCommands`.
- Do not bypass HTTPS certificate validation for an iPad test.
- Preserve the inbound block rule for TCP 8080, 8888, and 8889. Provider security groups must not expose HTTPS or TURN publicly; Windows Firewall limits private ports to tailnet address ranges.
- Treat `C:\ProgramData\DarkArisen` as sensitive. Runtime TURN/signalling files are ACL-restricted to the service identity and SYSTEM.
- Keep the repository, packaged game, play URL, screenshots, evidence containing hostnames, and retained EBS volume private. Do not use public buckets, releases, artifacts, issues, or PR comments.

## Cost boundary

Repository scripts may build and validate without provisioning infrastructure. Creating or starting a billable GPU, purchasing a domain, enabling a paid voice/asset service, increasing quota, or removing the idle shutdown requires Flo's explicit prior approval.

The idle watcher must invoke a validated provider adapter that stops or deallocates the billed resource. A Windows shutdown is not sufficient. The final gate requires provider-side evidence after 30 minutes with zero players.

SkyPilot, GPU marketplaces, and vendor “free tier” or credit claims are not provisioning authorization. They require the same exact quote, Windows/NVENC/persistence/privacy review, and explicit approval before credentials or artifacts are provided.

## Reporting a problem

Do not open a public issue containing secrets or host details. Revoke/rotate the affected value first, stop public access if needed, preserve sanitized logs, and report privately to the repository owner.
