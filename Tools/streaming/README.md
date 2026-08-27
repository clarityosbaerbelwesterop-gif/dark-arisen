# M0 Pixel Streaming operations

This directory prepares the no-surprise-cost deployment path for Unreal Engine 5.5 Pixel Streaming 2. It does not provision or start a paid GPU host.

## Locked architecture

- Unreal Engine 5.5 application, 1920×1080 at 60 fps, rendered offscreen through NVENC.
- Epic's `PixelStreamingInfrastructure` `UE5.5` branch, pinned to commit `c3e3abea6590a19e1c0ab4d2954efd6a1d949db3`.
- Signalling listens privately on ports 8888/8080.
- Caddy owns public HTTPS and password protection. `/api/*` is not exposed.
- coturn uses milestone-scoped long-term credentials; no default credential exists.
- A provider adapter must deallocate the cloud VM after 30 minutes without a player. Merely stopping Windows is rejected because it may continue billing.

## One-time host order

1. Install UE 5.5, Git, Python, Caddy, and NSSM on a Windows Server 2022 NVIDIA host.
2. Set `PIXEL_STREAMING_INFRA_ROOT`, `UE55_ROOT`, the `DARKARISEN_*` secrets, public/local IPs, and the chosen provider identity.
3. Run `bootstrap-infrastructure.ps1`.
4. Run `install-services.ps1` with an Azure or AWS shutdown adapter.
5. Register the host as a GitHub self-hosted runner with labels `Windows`, `X64`, `ue5.5`, `dark-arisen`, and `dark-arisen-streaming`.
6. Dispatch `Deploy M0 Pixel Streaming` with the desired branch.

## Required secret environment values

- `DARKARISEN_STREAM_HOST`
- `DARKARISEN_STREAM_USER`
- `DARKARISEN_STREAM_PASSWORD_HASH` (Caddy bcrypt hash, never plaintext)
- `DARKARISEN_ACME_EMAIL`
- `DARKARISEN_TURN_HOST`
- `DARKARISEN_TURN_USERNAME`
- `DARKARISEN_TURN_CREDENTIAL`
- `DARKARISEN_TURN_REALM`
- `DARKARISEN_PUBLIC_IP`
- `DARKARISEN_LOCAL_IP`
- one provider resource identifier and credentials/managed identity

The merge gate remains closed until an iPad test proves touch, DualSense, HTTPS/password access, mobile-network TURN, latency below 120 ms, locked 60 fps, and provider deallocation after 30 idle minutes.

