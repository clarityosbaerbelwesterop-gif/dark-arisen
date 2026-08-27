#!/usr/bin/env python3
"""Cheap, deterministic checks for the M0 repository contract."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path


REQUIRED_FILES = (
    ".gitattributes",
    ".gitignore",
    "DarkArisen.uproject",
    "Config/DefaultEngine.ini",
    "Config/DefaultGame.ini",
    "Config/DefaultInput.ini",
    "Source/DarkArisen.Target.cs",
    "Source/DarkArisenEditor.Target.cs",
    "Source/DarkArisen/DarkArisen.Build.cs",
    "Source/DarkArisen/DarkArisen.cpp",
    "Source/DarkArisen/DesignLaws.h",
    "Source/DarkArisen/GreyboxGameMode.cpp",
    "Source/DarkArisen/FrameTimeTelemetryActor.cpp",
    "Source/DarkArisen/Tests/DesignLawsSpec.cpp",
    "README.md",
    "HANDOVER.md",
    "M0_CHECKLIST.md",
    "SECURITY.md",
    "Docs/DesignAuthority.md",
    "Docs/DesignSourceRegister.md",
    "Docs/GpuBackendDecision.md",
    "Docs/Handoff/ENGINEERING_HANDOFF.md",
    "Docs/Handoff/PIXEL_STREAMING_SETUP.md",
    "Tools/streaming/launch-stream.ps1",
    "Tools/streaming/deploy.ps1",
    "Tools/streaming/collect-host-evidence.ps1",
    "Tools/streaming/aws/private-single-player.yaml",
    "Tools/streaming/aws/join-private-tailnet.ps1",
    "Tools/streaming/aws/install-nvidia-grid-driver.ps1",
    "Tools/streaming/aws/tailnet-policy.example.hujson",
    "Tools/streaming/aws/README.md",
    ".github/workflows/ci.yml",
    ".github/workflows/deploy-streaming.yml",
)

LFS_PATTERNS = ("*.uasset", "*.umap", "*.fbx", "*.wav")


def validate_private_aws_text(
    template: str,
    policy: str,
    join_script: str,
    install_services: str,
) -> list[str]:
    """Validate the fail-closed AWS/tailnet deployment contract without dependencies."""
    errors: list[str] = []

    required_template_fragments = (
        "Windows_Server-2022-English-Full-Base",
        "Default: g6.2xlarge",
        "I_ACKNOWLEDGE_AWS_CHARGES",
        'AllowedPattern: "^APPROVED-',
        "HttpTokens: required",
        "DeletionPolicy: Retain",
        "UpdateReplacePolicy: Retain",
        "AmazonSSMManagedInstanceCore",
        "ec2:ResourceTag/DarkArisenManaged",
        "FromPort: 41641",
        "SecurityPosture:",
        "NO_PUBLIC_GAME_RDP_SSH_WINRM_HTTPS_OR_TURN_INGRESS",
    )
    for fragment in required_template_fragments:
        if fragment not in template:
            errors.append(f"private AWS template requirement missing: {fragment}")

    security_group_start = template.find("  PrivateGameSecurityGroup:")
    role_start = template.find("  GpuHostRole:")
    if security_group_start < 0 or role_start <= security_group_start:
        errors.append("private AWS security group block cannot be located")
    else:
        security_group_block = template[security_group_start:role_start]
        egress_start = security_group_block.find("SecurityGroupEgress:")
        ingress_block = (
            security_group_block[:egress_start]
            if egress_start >= 0
            else security_group_block
        )
        for forbidden_port in ("FromPort: 22", "FromPort: 80", "FromPort: 443", "FromPort: 3389", "FromPort: 3478", "FromPort: 8443"):
            if forbidden_port in ingress_block:
                errors.append(f"public game/admin ingress is forbidden: {forbidden_port}")
        if ingress_block.count("SecurityGroupIngress:") != 1 or ingress_block.count("FromPort: 41641") != 1:
            errors.append("the sole inbound security-group transport must be UDP 41641")
        if ingress_block.count("IpProtocol: udp") != 1 or "ToPort: 41641" not in ingress_block:
            errors.append("the sole inbound Tailscale transport must be UDP 41641 only")

    outputs_start = template.find("Outputs:")
    if outputs_start < 0:
        errors.append("private AWS template outputs cannot be located")
    else:
        outputs_block = template[outputs_start:]
        for exposed_output in ("PublicIp", "PublicDns", "https://", "ts.net"):
            if exposed_output in outputs_block:
                errors.append(f"private endpoint output is forbidden: {exposed_output}")

    for legacy_or_broad_value in (
        "Ch4ng3M3!",
        "AdministratorPassword",
        "Windows_Server-2019",
        "g4dn.4xlarge",
        "ec2:*",
        "s3:*",
        "ssm:*",
        "logs:*",
        "iam:*",
    ):
        if legacy_or_broad_value in template:
            errors.append(f"legacy or broad AWS sample value is forbidden: {legacy_or_broad_value}")

    for policy_fragment in (
        '"tag:dark-arisen"',
        '"tag:dark-arisen": ["FLO_TAILSCALE_LOGIN@example.invalid"]',
        '"src": ["FLO_TAILSCALE_LOGIN@example.invalid"]',
        '"dst": ["tag:dark-arisen"]',
        '"tcp:443"',
        '"tcp:3478"',
        '"udp:3478"',
        '"udp:49160-49200"',
    ):
        if policy_fragment not in policy:
            errors.append(f"tailnet policy requirement missing: {policy_fragment}")
    for wildcard_rule in ('"src": ["*"]', '"dst": ["*"]'):
        if wildcard_rule in policy:
            errors.append(f"tailnet wildcard access is forbidden: {wildcard_rule}")
    for broad_identity in ("autogroup:member", "autogroup:admin", "autogroup:owner"):
        if broad_identity in policy:
            errors.append(f"broad tailnet identity is forbidden: {broad_identity}")

    for join_fragment in (
        "Get-SSMParameterValue",
        "--auth-key=file:",
        "funnel reset",
        "serve --bg --https=443 http://127.0.0.1:8080",
        'DARKARISEN_PRIVATE_OVERLAY", "1"',
    ):
        if join_fragment not in join_script:
            errors.append(f"private tailnet join control missing: {join_fragment}")
    if "Write-Host $AuthKey" in join_script or "Write-Output $AuthKey" in join_script:
        errors.append("the Tailscale auth key must never be printed")

    for service_fragment in (
        "[switch]$PrivateTailnet",
        "100.64.0.0/10",
        "fd7a:115c:a1e0::/48",
        "-RemoteAddress $TailnetRanges",
        'Dark Arisen Private TCP',
        "funnel reset",
    ):
        if service_fragment not in install_services:
            errors.append(f"private service control missing: {service_fragment}")
    if "funnel --bg" in join_script or "funnel --bg" in install_services:
        errors.append("Tailscale Funnel must never be enabled")

    return errors


def validate(root: Path) -> list[str]:
    errors: list[str] = []

    for relative in REQUIRED_FILES:
        if not (root / relative).is_file():
            errors.append(f"missing required file: {relative}")

    project_path = root / "DarkArisen.uproject"
    if project_path.is_file():
        try:
            project = json.loads(project_path.read_text(encoding="utf-8"))
        except (json.JSONDecodeError, OSError) as error:
            errors.append(f"invalid DarkArisen.uproject: {error}")
        else:
            if project.get("EngineAssociation") != "5.5":
                errors.append("DarkArisen.uproject must remain pinned to UE 5.5 for M0")
            modules = {module.get("Name") for module in project.get("Modules", [])}
            if "DarkArisen" not in modules:
                errors.append("DarkArisen runtime module is not declared")
            plugins = {
                plugin.get("Name"): plugin.get("Enabled")
                for plugin in project.get("Plugins", [])
            }
            if plugins.get("PixelStreaming2") is not True:
                errors.append("PixelStreaming2 must be enabled")

    attributes_path = root / ".gitattributes"
    if attributes_path.is_file():
        attributes = attributes_path.read_text(encoding="utf-8")
        for pattern in LFS_PATTERNS:
            if f"{pattern} filter=lfs diff=lfs merge=lfs -text" not in attributes:
                errors.append(f"Git LFS rule is missing for {pattern}")

    ignore_path = root / ".gitignore"
    if ignore_path.is_file():
        ignore_text = ignore_path.read_text(encoding="utf-8")
        for fragment in (
            "Tools/streaming/aws/*.local.*",
            "Tools/streaming/aws/parameters*.json",
            "Tools/streaming/aws/change-set*.json",
        ):
            if fragment not in ignore_text:
                errors.append(f"local AWS deployment artifact is not ignored: {fragment}")

    engine_config = root / "Config/DefaultEngine.ini"
    if engine_config.is_file():
        config = engine_config.read_text(encoding="utf-8")
        if "t.MaxFPS=60" not in config:
            errors.append("the 60 fps frame cap is missing")
        if "GlobalDefaultGameMode=/Script/DarkArisen.GreyboxGameMode" not in config:
            errors.append("the M0 greybox game mode is not the default")
        if "r.Lumen.HardwareRayTracing=False" not in config:
            errors.append("console-aligned software Lumen decision is missing")

    laws_path = root / "Source/DarkArisen/DesignLaws.h"
    if laws_path.is_file():
        laws = laws_path.read_text(encoding="utf-8")
        locked_fragments = (
            "TargetFramesPerSecond = 60",
            "DeflectionWindowFrames = 6",
            "RacheTimeScale = 0.30f",
            "RacheMaximumRealSeconds = 5.0f",
            "ProhibitedCutsceneMomentCount = 22",
        )
        for fragment in locked_fragments:
            if fragment not in laws:
                errors.append(f"locked design constant missing: {fragment}")

    workflow_directory = root / ".github/workflows"
    if workflow_directory.is_dir():
        workflow_text = "\n".join(
            path.read_text(encoding="utf-8")
            for path in sorted(workflow_directory.glob("*.yml"))
        )
        if "self-hosted" not in workflow_text:
            errors.append("workflows must remain self-hosted while hosted minutes are unavailable")
        for hosted_label in ("ubuntu-latest", "windows-latest", "macos-latest"):
            if hosted_label in workflow_text:
                errors.append(f"GitHub-hosted runner label is forbidden for M0: {hosted_label}")

    launch_path = root / "Tools/streaming/launch-stream.ps1"
    if launch_path.is_file():
        launch = launch_path.read_text(encoding="utf-8")
        for flag in (
            "-PixelStreamingURL=ws://127.0.0.1:8888",
            "-RenderOffscreen",
            "-Unattended",
            "-NoTextureStreaming",
            "-ResX=1920",
            "-ResY=1080",
            "-PixelStreamingWebRTCMaxFps=60",
            "-UserDir=",
        ):
            if flag not in launch:
                errors.append(f"required streaming launch flag is missing: {flag}")
        if "NVENC" not in launch:
            errors.append("launch-stream.ps1 must prove NVENC and reject software fallback")

    streaming_directory = root / "Tools/streaming"
    if streaming_directory.is_dir():
        streaming_text = "\n".join(
            path.read_text(encoding="utf-8")
            for path in sorted(streaming_directory.rglob("*"))
            if path.is_file()
        )
        for provisioning_command in ("az vm create", "aws ec2 run-instances"):
            if provisioning_command in streaming_text:
                errors.append(f"paid infrastructure provisioning is forbidden: {provisioning_command}")
        if "c3e3abea6590a19e1c0ab4d2954efd6a1d949db3" not in streaming_text:
            errors.append("Pixel Streaming Infrastructure must remain pinned for UE 5.5")
        if "-ValidateOnly" not in streaming_text:
            errors.append("provider shutdown adapters must support a non-destructive validation mode")

    private_aws_path = root / "Tools/streaming/aws/private-single-player.yaml"
    private_policy_path = root / "Tools/streaming/aws/tailnet-policy.example.hujson"
    private_join_path = root / "Tools/streaming/aws/join-private-tailnet.ps1"
    install_services_path = root / "Tools/streaming/install-services.ps1"
    if all(path.is_file() for path in (
        private_aws_path,
        private_policy_path,
        private_join_path,
        install_services_path,
    )):
        errors.extend(validate_private_aws_text(
            private_aws_path.read_text(encoding="utf-8"),
            private_policy_path.read_text(encoding="utf-8"),
            private_join_path.read_text(encoding="utf-8"),
            install_services_path.read_text(encoding="utf-8"),
        ))

    signalling_path = root / "Tools/streaming/launch-signalling.ps1"
    if signalling_path.is_file():
        signalling = signalling_path.read_text(encoding="utf-8")
        if "rest_api = $false" not in signalling:
            errors.append("the signalling REST API must remain disabled")

    host_evidence_path = root / "Tools/streaming/collect-host-evidence.ps1"
    if host_evidence_path.is_file():
        host_evidence = host_evidence_path.read_text(encoding="utf-8")
        if "stream_host_sha256" not in host_evidence:
            errors.append("stream host evidence must store a hash instead of the private hostname")
        if "public_host =" in host_evidence or "stream_host =" in host_evidence:
            errors.append("stream host evidence must not store the private hostname")

    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[2])
    args = parser.parse_args()
    root = args.root.resolve()

    errors = validate(root)
    if errors:
        print("M0 validation failed:", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1

    print("M0 repository validation passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
