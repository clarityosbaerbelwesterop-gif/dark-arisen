# Private AWS Windows GPU host

This is the hardened replacement for the public `aws-samples/deploying-unreal-engine-pixel-streaming-server-on-ec2` defaults. It is deployment-ready infrastructure code, but it intentionally does **not** run by itself and no AWS resource has been created.

## What the supplied AWS sample actually is

The sample is an Unreal Engine 4-era CloudFormation automation project, not an Epic or AWS free-GPU entitlement. Its template creates billable EC2 and S3 resources and includes defaults that are unacceptable for Dark Arisen: broad public CIDRs, open RDP/game ports, a fixed example administrator password, old Windows/Node generations, and broad IAM permissions. None of those defaults are inherited here.

## Selected privacy model

- Windows Server 2022 on `g6.2xlarge` in Frankfurt, with `g5.2xlarge` as the reviewed fallback.
- No public game, HTTPS, TURN, RDP, SSH, or WinRM ingress.
- UDP 41641 is the sole public transport rule. It carries authenticated Tailscale WireGuard traffic, not the game endpoint.
- Browser HTTPS uses Tailscale Serve, which is reachable inside the tailnet only. Tailscale Funnel is explicitly reset and must remain disabled.
- A deny-by-default tailnet grant admits one exact Flo login to one tagged host. TURN is private inside the same tailnet.
- Administration uses AWS Systems Manager, so no public admin port or EC2 key pair is created.
- The host accepts one Pixel Streaming player, and provider-level idle logic stops the exact tagged EC2 instance after 30 minutes.
- The root EBS volume is encrypted. A separate encrypted 500 GiB gp3 data volume is retained on stack deletion/replacement and holds UE, project data, releases, saves, and telemetry.
- A dynamic public IPv4 address is used only for outbound control/download traffic and Tailscale transport. No Elastic IP, public DNS name, public bucket, database, load balancer, or paid add-on is created.

## Hard cost gate

`private-single-player.yaml` cannot be launched without both an `APPROVED-*` record and the literal charge acknowledgement. This is an additional guard, not a substitute for reviewing AWS Pricing Calculator output. Before launch, Flo must approve:

1. exact regional Windows EC2 hourly price;
2. gp3 monthly price for both volumes;
3. public IPv4 and expected egress;
4. maximum monthly spend;
5. selected GRID driver/license terms.

CloudFormation creation, change-set execution, quota increases, Marketplace acceptance, and instance start are forbidden before that approval.

## One-time private setup order

1. Use a dedicated one-user Dark Arisen tailnet. Replace the placeholder in `tailnet-policy.example.hujson` with Flo's exact login and apply it as the full policy. Preview the effective rules; confirm there is no default allow-all rule, invited user, shared machine, share link, or wildcard reaching `tag:dark-arisen`.
2. Generate a **one-off, pre-approved, non-ephemeral** Tailscale auth key tagged `tag:dark-arisen`. Put it in the same AWS account/region as an SSM Parameter Store `SecureString` using the AWS-managed `aws/ssm` key. Record only the parameter ARN, never the value.
3. From the official Tailscale stable package page, record the pinned MSI SHA-256 (`.sha256`). In the AWS NVIDIA bucket, select one exact Windows GRID `latest/*.exe` key compatible with G6/Windows Server 2022 and record its SHA-256.
4. Produce an AWS Pricing Calculator export. After Flo approves it, create a CloudFormation change set from `private-single-player.yaml`; inspect it before executing.
5. Wait for Systems Manager to report the instance online. Run `join-private-tailnet.ps1 -ValidateOnly`, then run it once with the exact SecureString parameter ARN. Revoke the one-off Tailscale key and delete its SSM parameter immediately after the node joins.
6. Run `install-nvidia-grid-driver.ps1 -ValidateOnly`, review the NVIDIA GRID EULA, then install the exact reviewed driver. After restart, collect `nvidia-smi` evidence.
7. Install UE 5.5, Git, Python, NSSM, AWS CLI v2, the `AWS.Tools.SimpleSystemsManagement` and `AWS.Tools.S3` PowerShell modules, and the private GitHub runner on `D:\DarkArisen`; do not place the repository or saves on instance-store NVMe.
8. Configure unique milestone TURN credentials as machine-scoped values, then run `install-services.ps1 -PrivateTailnet` with the AWS shutdown adapter.
9. Package/deploy the M0 build and execute every gate in `M0_CHECKLIST.md` from Flo's Tailscale-authenticated iPad.

## Link semantics

The final bookmark will be the `https://<host>.<tailnet>.ts.net` value written to `C:\ProgramData\DarkArisen\private-tailnet.json`. It is not a public play link: Safari must have the Tailscale app connected as Flo's allowed identity. Do not share this URL or publish it in CI logs, issues, PR comments, screenshots, or documentation.

## Required official references

- AWS sample: https://github.com/aws-samples/deploying-unreal-engine-pixel-streaming-server-on-ec2
- Windows AMI public parameters: https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/finding-an-ami-parameter-store.html
- Session Manager: https://docs.aws.amazon.com/systems-manager/latest/userguide/session-manager.html
- AWS NVIDIA GRID drivers: https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/nvidia-GRID-driver.html
- Tailscale auth-key file handling: https://tailscale.com/docs/reference/tailscale-cli/up
- Tailscale Serve: https://tailscale.com/docs/reference/tailscale-cli/serve
- Tailscale Funnel: https://tailscale.com/docs/reference/tailscale-cli/funnel
- Tailscale grants: https://tailscale.com/docs/features/access-control/grants
