# Pixel Streaming operations — native C++ owner

This directory now contains only configuration, provider templates and operational documentation. Executable behavior lives in the native `DarkArisenOps` C++ program under `Source/DarkArisenOps`.

## Locked architecture

- Unreal Engine 5.5 application, 1920×1080 at 60 fps, rendered offscreen through NVENC.
- Epic `PixelStreamingInfrastructure` `UE5.5` pinned to `c3e3abea6590a19e1c0ab4d2954efd6a1d949db3`.
- Epic's stock UE 5.5 frontend is used; Dark Arisen has no TypeScript/JavaScript frontend overlay.
- Signalling uses local 8888/8080/8889, one player and no REST API.
- Windows Firewall blocks direct remote TCP access to 8080/8888/8889.
- Strict-private AWS Alpha uses Tailscale Serve on HTTPS 443; Funnel is reset/disabled.
- coturn uses milestone-scoped long-term credentials; private mode exposes TURN only to tailnet ranges.
- `DarkArisenOps stream-idle-shutdown` invokes a provider stop/deallocation adapter after 30 minutes without a player; guest-OS shutdown alone is not accepted.
- Persistent provider storage holds repository, UE install, releases, saves and telemetry.

## Build the native owner

Windows:

`%UE55_ROOT%\Engine\Build\BatchFiles\Build.bat DarkArisenOps Win64 Development -Project=<repo>\DarkArisen.uproject -WaitMutex -WarningsAsErrors`

Linux:

`$UE55_ROOT/Engine/Build/BatchFiles/Linux/Build.sh DarkArisenOps Linux Development -Project=<repo>/DarkArisen.uproject -WaitMutex -WarningsAsErrors`

The resulting executable is `Binaries/Win64/DarkArisenOps.exe` or `Binaries/Linux/DarkArisenOps`.

## One-time Windows streaming host order

1. Complete the reviewed provider approval in `aws/README.md`; no paid resource is launched by this repository.
2. Build `DarkArisenOps`.
3. Apply the one-user tailnet policy.
4. Join the host with `DarkArisenOps tailnet-join --auth-key-parameter-arn=<exact-SSM-ARN> --region=<region>` and revoke/delete the one-off key immediately afterward.
5. Install the exact reviewed GRID package with `DarkArisenOps install-grid-driver --s3-key=<exact-key> --sha256=<64-hex> --accept-eula`.
6. Install UE 5.5, Git, NSSM and AWS CLI v2 on retained storage. Python and PowerShell modules are no longer project runtime dependencies.
7. Set `PIXEL_STREAMING_INFRA_ROOT`, `UE55_ROOT`, unique TURN secrets and provider identity.
8. Run `DarkArisenOps bootstrap-streaming`.
9. Run `DarkArisenOps stream-install-services --private-tailnet --provider=aws --nssm=<path> --infra=<path>`.
10. Register the private self-hosted GitHub runner with `Windows`, `X64`, `ue5.5`, `dark-arisen`, `dark-arisen-streaming`.
11. Dispatch `Deploy Native C++ Pixel Streaming`.
12. Capture evidence with `DarkArisenOps collect-host-evidence`.

## Required secret environment values

- `DARKARISEN_STREAM_HOST`
- `DARKARISEN_TURN_HOST`
- `DARKARISEN_TURN_USERNAME`
- `DARKARISEN_TURN_CREDENTIAL`
- `DARKARISEN_TURN_REALM`
- `DARKARISEN_LOCAL_IP`
- `DARKARISEN_PRIVATE_OVERLAY=1` for the selected strict-private Alpha
- provider-specific resource identifier (`DARKARISEN_AWS_INSTANCE_ID` or `DARKARISEN_AZURE_VM_RESOURCE_ID`)
- `DARKARISEN_SHUTDOWN_PROVIDER=aws|azure`

The merge gate remains closed until an iPad test proves touch, DualSense, approved-user-only tailnet HTTPS, outside-tailnet denial, mobile-network private TURN, latency below 120 ms, locked 60 fps and provider deallocation after the idle window.
