# M0 Pixel Streaming operations

This directory prepares the no-surprise-cost deployment path for Unreal Engine 5.5 Pixel Streaming 2. It does not provision or start a paid GPU host.

## Locked architecture

- Unreal Engine 5.5 application, 1920×1080 at 60 fps, rendered offscreen through NVENC.
- Epic's `PixelStreamingInfrastructure` `UE5.5` branch, pinned to commit `c3e3abea6590a19e1c0ab4d2954efd6a1d949db3`.
- Signalling uses host ports 8888/8080 for the streamer and player, allows one player, and exposes no REST API. Windows Firewall blocks direct remote TCP access to 8080/8888/8889; tailnet-only Serve proxies only loopback 8080 to HTTPS 443.
- The selected AWS path uses Tailscale Serve for tailnet-only HTTPS. Funnel is disabled and a deny-by-default grant admits one exact Flo identity.
- coturn uses milestone-scoped long-term credentials on the private tailnet; no default credential exists and no TURN port is public.
- A provider adapter must deallocate the cloud VM after 30 minutes without a player. Merely stopping Windows is rejected because it may continue billing.
- The encrypted retained EBS data volume is the only host location for the repository, UE installation, releases, saves, and telemetry.

The older Caddy/password path remains available for another reviewed provider, but it is not selected for Flo's strict-private AWS Alpha. See `aws/README.md`.

## One-time host order

1. Complete the quote/approval and private AWS order in `aws/README.md`; no resource may be launched before it.
2. Apply the one-user tailnet policy, join the Windows Server 2022 NVIDIA host once, revoke the one-off key, and install the exact reviewed GRID driver.
3. Install UE 5.5, Git, Python, and NSSM on the retained `D:` volume.
4. Set `PIXEL_STREAMING_INFRA_ROOT`, `UE55_ROOT`, unique TURN secrets, and the chosen provider identity.
5. Run `bootstrap-infrastructure.ps1`.
6. Run `install-services.ps1 -PrivateTailnet` with the AWS shutdown adapter.
7. Register the host as a private GitHub self-hosted runner with labels `Windows`, `X64`, `ue5.5`, `dark-arisen`, and `dark-arisen-streaming`.
8. Dispatch `Deploy M0 Pixel Streaming` with the desired branch.

## Required secret environment values

- `DARKARISEN_STREAM_HOST`
- `DARKARISEN_TURN_HOST`
- `DARKARISEN_TURN_USERNAME`
- `DARKARISEN_TURN_CREDENTIAL`
- `DARKARISEN_TURN_REALM`
- `DARKARISEN_LOCAL_IP`
- `DARKARISEN_PRIVATE_OVERLAY=1`
- one provider resource identifier and credentials/managed identity

The merge gate remains closed until an iPad test proves touch, DualSense, Flo-only tailnet HTTPS, outside-tailnet denial, mobile-network private TURN, latency below 120 ms, locked 60 fps, and provider deallocation after 30 idle minutes.
