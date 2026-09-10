# GPU backend decision — M0 remote play

**Decision date:** 2026-08-26
**State:** Architecture selected; no account resource has been created, started, reserved, or purchased.

## Recommendation

Use one on-demand **AWS EC2 `g6.2xlarge` Windows Server 2022** instance in **Europe (Frankfurt), `eu-central-1`**, subject to quota and a price quote approved by Flo.

Why this is the current first choice:

- One full NVIDIA L4 GPU with 24 GB class memory, hardware H.264 encoding, and two video encoders is a good M0 fit for 1080p60 game streaming.
- Eight vCPUs and 32 GiB system RAM are materially safer for UE packaging and the signalling stack than the 4-vCPU/16-GiB `g6.xlarge`.
- AWS lists G6 for graphics, real-time rendering, and game streaming and lists Frankfurt availability.
- AWS provides supported GRID/gaming-driver paths for G6 on Windows.
- The existing provider adapter already validates identity and stops the exact EC2 instance after 30 idle minutes.

Official references:

- https://aws.amazon.com/ec2/instance-types/g6/
- https://aws.amazon.com/about-aws/whats-new/2026/05/amazon-ec2-g6-aws-european-sovereign-cloud/
- https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/install-nvidia-driver.html
- https://aws.amazon.com/ec2/pricing/on-demand/

## Strict-private access decision

The M0 play endpoint must not be publicly playable or discoverable as an open service. The selected implementation is therefore:

1. No public game, HTTPS, TURN, RDP, SSH, or WinRM security-group ingress.
2. UDP 41641 is the only public inbound rule and carries authenticated Tailscale WireGuard transport, not Pixel Streaming itself.
3. Tailscale Serve terminates HTTPS inside Flo's tailnet; Tailscale Funnel is explicitly disabled.
4. A deny-by-default tailnet grant permits one exact Flo login to `tag:dark-arisen` on HTTPS and private TURN ports only.
5. AWS Systems Manager performs administration without an inbound admin port or EC2 key pair.
6. Direct signalling ports remain blocked, Serve proxies only loopback player port 8080, `max_players` remains one, the REST API is disabled, and TURN binds to the tailnet address.
7. The browser bookmark is never placed in CI output, issues, PR comments, screenshots, or public documentation.

This makes the eventual `https://<host>.<tailnet>.ts.net` bookmark usable only while the iPad is authenticated to the allowed tailnet identity. A hostname is not proof of authorization; the tailnet policy and a real outside-tailnet denial test remain mandatory gates.

Private-host code and the fail-closed launch order are under `Tools/streaming/aws/`.

Official references:

- https://docs.aws.amazon.com/systems-manager/latest/userguide/session-manager.html
- https://tailscale.com/docs/reference/tailscale-cli/serve
- https://tailscale.com/docs/reference/tailscale-cli/funnel
- https://tailscale.com/docs/features/access-control/grants
- https://tailscale.com/docs/reference/tailscale-cli/up

## Audit of the supplied AWS sample

`aws-samples/deploying-unreal-engine-pixel-streaming-server-on-ec2` is useful as a historical infrastructure reference, but it is **not** a free or pre-allocated GPU. Its own files target Unreal Engine 4-era packaged builds, G3/G4/G4dn, Windows Server through 2019, an old Node runtime, and CloudFormation-created EC2/S3 resources. The sample defaults also include public CIDRs, public game/admin ports, a fixed example administrator password, broad IAM access, and error-ignoring bootstrap behavior.

Dark Arisen does not deploy or fork those defaults. The replacement template uses Windows Server 2022, G6/G5, IMDSv2, exact IAM resources/actions, encrypted retained storage, no public game/admin ingress, and explicit charge/approval parameters.

- https://github.com/aws-samples/deploying-unreal-engine-pixel-streaming-server-on-ec2

## Audit of SkyPilot

SkyPilot is a useful open-source optimizer/orchestrator across existing clouds and clusters, not a source of free GPU capacity. Its official README states that it is BYOC and launches resources inside the user's cloud accounts, VPCs, and clusters; its GPU example requires GPU-instance access. GPU Compass compares prices and availability, and the SkyPilot team explicitly says it does not sell compute.

SkyPilot is also built around Linux/SSH/AI workload images by default. It does not replace the reviewed Windows Server 2022, WDDM/NVENC, persistent EBS, private-tailnet, and provider-stop requirements for UE 5.8 Pixel Streaming. It may later be used read-only to compare eligible offerings, but it must not receive cloud credentials, provision a cluster, upload the private repository, or open ports without a separate security review and Flo's cost approval.

- https://github.com/skypilot-org/skypilot
- https://skypilot.ai/blog/gpu-compass
- https://docs.skypilot.co/en/latest/getting-started/installation.html

## Persistent storage layout

The EC2 local NVMe instance store is scratch space only. AWS documents that instance-store data is erased when an instance is stopped, hibernated, or terminated.

| Layer | Initial layout | Persistence rule |
|---|---|---|
| Source and design | Private GitHub repository + Git LFS | Canonical source; never rely on the VM as the only copy |
| OS and tools | 160-256 GiB encrypted EBS gp3 root volume | Survives stop/start |
| Project, UE install, releases | 500 GiB encrypted EBS gp3 data volume | Versioned releases and project data survive deallocation |
| Player saves and telemetry | `<RuntimeRoot>\UserData` on the EBS data volume | `launch-stream.ps1` passes an explicit Unreal `-UserDir` |
| Derived-data cache and packaging scratch | Local NVMe instance store | Rebuildable; safe to lose |
| Backup | Optional encrypted EBS snapshot | Disabled until Flo approves its recurring storage cost |

AWS persistence references:

- https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/instance-store-lifetime.html
- https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/Storage.html
- https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/Stop_Start.html
- https://aws.amazon.com/ebs/pricing/

## Cost boundary

The only safe number before account, region, Windows image, disk, public IPv4, and tax are selected is a budget envelope, not a quote.

- **Compute envelope:** reserve roughly USD 1.20-1.70 per running hour for a Frankfurt Windows G6-class host until the AWS calculator supplies the exact current quote.
- **Example first-playthrough use:** 20 running hours would therefore be roughly USD 24-34 for compute.
- **Persistent 500 GiB gp3 storage:** budget roughly USD 40-60 per month even while the VM is stopped; exact Frankfurt pricing must be quoted.
- **Network egress:** a 12-25 Mbit/s stream transfers roughly 5.4-11.3 GB per hour. AWS currently documents 100 GB/month of aggregated free internet egress, after which regional charges apply.
- **Other possible charges:** Windows licensing in the instance price, public IPv4, snapshots if approved, DNS/domain, and tax.

Before provisioning, the exact calculator export becomes the approval artifact. No Spot, Savings Plan, Reserved Instance, snapshot schedule, domain purchase, or paid add-on is allowed for M0 without separate approval.

## Alternatives reviewed

| Option | Result | Reason |
|---|---|---|
| A concrete Epic-provided GPU entitlement | Preferred if it actually exists in Flo's account | Epic Pixel Streaming itself is software/reference infrastructure, not proof of a free GPU allocation. A dashboard or instance link is required. |
| SkyPilot over 20+ clouds | Rejected as the M0 runtime | BYOC orchestration, not free compute; default Linux/SSH workload assumptions do not satisfy the Windows UE 5.8 path. |
| Azure `NVadsA10_v5`, Germany West Central/West Europe | Valid fallback | Windows/GRID support is strong, but the affordable sizes expose only fractions of an A10: 4, 8, or 12 GB. A full 24 GB A10 starts at `NV36ads_A10_v5`, which is substantially oversized for M0. |
| AWS `g5.2xlarge`, Frankfurt | Safe fallback | Full 24 GB A10G and NVENC; older and generally less attractive than G6/L4 for this single-stream target. |
| Paperspace A4000, Amsterdam | Rejected for a new account | DigitalOcean states that new Paperspace users cannot create Windows templates after 2024-07-01. Linux remains possible but diverges from the approved Windows build/deploy path. |
| Consumer-GPU rental marketplaces | Deferred | Lower headline cost, but Windows licensing, persistent-disk semantics, public IP continuity, and reliable stop/deallocate APIs vary too much for the first acceptance gate. |

Azure and Paperspace references:

- https://learn.microsoft.com/azure/virtual-machines/sizes/gpu-accelerated/nvadsa10v5-series
- https://learn.microsoft.com/azure/virtual-machines/overview
- https://docs.digitalocean.com/products/paperspace/machines/details/machine-types/
- https://docs.digitalocean.com/products/paperspace/machines/details/availability/

Epic Pixel Streaming references:

- https://dev.epicgames.com/documentation/unreal-engine/pixel-streaming-in-unreal-engine
- https://dev.epicgames.com/documentation/unreal-engine/hosting-and-networking-guide-for-pixel-streaming-in-unreal-engine
- https://dev.epicgames.com/documentation/unreal-engine/pixel-streaming-infrastructure

## Backend scope for M0

A database cluster is unnecessary for one player and one Unreal process. The strong backend M0 needs is:

1. GPU compute close to Germany.
2. Persistent encrypted block storage separated from ephemeral scratch.
3. GitHub as source authority.
4. Tailnet-identity HTTPS, private TURN, and a single-player limit.
5. Atomic releases and rollback.
6. Provider-level idle deallocation.
7. Evidence collection for NVENC, latency, frame time, firewall, and cost shutdown.

Object storage, a user database, session orchestration, and multi-instance autoscaling become relevant only after a real concurrent-user requirement exists.

## Provisioning gate

Provision only after Flo approves all of the following in one explicit decision:

- provider and account;
- region and instance type;
- exact hourly compute quote;
- monthly persistent-disk estimate;
- maximum monthly spend;
- whether a domain or snapshot is allowed;
- Flo's exact Tailscale login and acceptance of the free Personal-plan terms, if that plan remains eligible at launch time;
- the reviewed one-off auth-key workflow and outside-tailnet denial test.

Until then, repository work may validate scripts but must not create or start billable infrastructure.
