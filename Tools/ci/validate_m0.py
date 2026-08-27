#!/usr/bin/env python3
"""Cheap, deterministic checks for the M0 repository contract."""

from __future__ import annotations

import argparse
import json
import re
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
    "Tools/streaming/aws/tailnet-policy.example.hujson",
    "Tools/streaming/aws/join-private-tailnet.ps1",
    "Tools/streaming/install-services.ps1",
    "Tools/ci/verify-runner.ps1",
    ".github/workflows/ci.yml",
    ".github/workflows/deploy-streaming.yml",
)

LFS_PATTERNS = ("*.uasset", "*.umap", "*.fbx", "*.wav")


def _text_section(text: str, start: str, end: str) -> str:
    """Return one indentation-delimited contract section, or an empty string."""
    start_index = text.find(start)
    if start_index < 0:
        return ""
    end_index = text.find(end, start_index + len(start))
    return text[start_index:end_index if end_index >= 0 else len(text)]


def validate_private_aws_text(
    template: str,
    policy: str,
    join_script: str,
    install_services: str,
) -> list[str]:
    """Fail closed when the optional AWS path stops being private or cost gated."""
    errors: list[str] = []

    ingress = _text_section(
        template,
        "SecurityGroupIngress:",
        "SecurityGroupEgress:",
    )
    if not ingress:
        errors.append("private AWS template is missing SecurityGroupIngress")
    else:
        ports = {
            int(match)
            for match in re.findall(r"(?:FromPort|ToPort):\s*(\d+)", ingress)
        }
        for port in sorted(ports - {41641}):
            errors.append(f"private AWS ingress exposes forbidden public port {port}")
        for fragment in (
            "IpProtocol: udp",
            "FromPort: 41641",
            "ToPort: 41641",
            "CidrIp: 0.0.0.0/0",
        ):
            if fragment not in ingress:
                errors.append(f"private AWS WireGuard ingress requirement missing: {fragment}")

    for fragment in (
        "AllowedPattern: \"^APPROVED-",
        "I_ACKNOWLEDGE_AWS_CHARGES",
        "HttpTokens: required",
        "DeletionPolicy: Retain",
        "UpdateReplacePolicy: Retain",
        "Encrypted: true",
        "NO_PUBLIC_GAME_RDP_SSH_WINRM_HTTPS_OR_TURN_INGRESS",
    ):
        if fragment not in template:
            errors.append(f"private AWS safety requirement missing: {fragment}")
    for forbidden in (
        "AWS::EC2::KeyPair",
        "KeyName:",
        "MasterUserPassword",
    ):
        if forbidden in template:
            errors.append(f"private AWS template contains forbidden credential/admin path: {forbidden}")

    if re.search(r'"(?:src|dst)"\s*:\s*\[\s*"\*"', policy):
        errors.append("private tailnet policy contains a wildcard source or destination")
    if any(
        identity in policy
        for identity in (
            "autogroup:member",
            "autogroup:members",
            "autogroup:everyone",
            "autogroup:shared",
        )
    ):
        errors.append("private tailnet policy contains a broad identity")
    for fragment in (
        '"tag:dark-arisen"',
        '"dst": ["tag:dark-arisen"]',
        '"tcp:443"',
        '"udp:49160-49200"',
    ):
        if fragment not in policy:
            errors.append(f"private tailnet policy requirement missing: {fragment}")

    funnel_enablement = re.compile(
        r"(?im)^\s*&\s+\$[A-Za-z][A-Za-z0-9]*\s+funnel\s+(?!reset\b)"
    )
    if funnel_enablement.search(join_script) or funnel_enablement.search(install_services):
        errors.append("Tailscale Funnel must never be enabled for the private game link")
    for fragment in (
        '"--auth-key=file:$AuthKeyPath"',
        "$TailscaleExe funnel reset",
        "$TailscaleExe serve --bg --https=443 http://127.0.0.1:8080",
        "Remove-Item -LiteralPath $AuthKeyPath -Force",
        '"DARKARISEN_PRIVATE_OVERLAY", "1", "Machine"',
    ):
        if fragment not in join_script:
            errors.append(f"private tailnet join requirement missing: {fragment}")

    for fragment in (
        "[switch]$PrivateTailnet",
        'if ($PrivateTailnet)',
        '"100.64.0.0/10"',
        '"fd7a:115c:a1e0::/48"',
        "$TailscalePath funnel reset",
        '-LocalPort "8080,8888,8889"',
    ):
        if fragment not in install_services:
            errors.append(f"private streaming service requirement missing: {fragment}")

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

    runner_preflight_path = root / "Tools/ci/verify-runner.ps1"
    if runner_preflight_path.is_file():
        runner_preflight = runner_preflight_path.read_text(encoding="utf-8")
        for fragment in (
            "$IsWindows",
            "Is64BitOperatingSystem",
            "MajorVersion -ne 5",
            "MinorVersion -ne 5",
            "git.exe lfs version",
            "MinimumFreeDiskGb",
        ):
            if fragment not in runner_preflight:
                errors.append(f"runner preflight requirement missing: {fragment}")

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

    private_aws_paths = (
        root / "Tools/streaming/aws/private-single-player.yaml",
        root / "Tools/streaming/aws/tailnet-policy.example.hujson",
        root / "Tools/streaming/aws/join-private-tailnet.ps1",
        root / "Tools/streaming/install-services.ps1",
    )
    if all(path.is_file() for path in private_aws_paths):
        errors.extend(validate_private_aws_text(*(
            path.read_text(encoding="utf-8") for path in private_aws_paths
        )))

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
