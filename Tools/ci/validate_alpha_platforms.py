#!/usr/bin/env python3
"""Fail-closed checks for the private Windows/Linux Alpha artifact contract."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path


REQUIRED_FILES = (
    "DarkArisen.uproject",
    ".github/workflows/ci.yml",
    "Tools/ci/verify-runner.ps1",
    "Tools/ci/verify-linux-runner.sh",
    "Tools/ci/build-unreal.ps1",
    "Tools/ci/build-unreal-linux.sh",
    "Tools/ci/package-alpha-windows.ps1",
    "Tools/ci/package-alpha-linux.sh",
    "Tools/ci/promote-alpha-candidates.py",
    "Docs/ALPHA_PLATFORM_ARTIFACTS.md",
    "Docs/ALPHA_DELIVERY_CHECKLIST.md",
)

FINAL_ARTIFACTS = (
    "DarkArisen-Alpha-Windows-Arcware.zip",
    "DarkArisen-Alpha-Linux-x86_64.zip",
)


def _read(root: Path, relative: str, errors: list[str]) -> str:
    path = root / relative
    if not path.is_file():
        errors.append(f"missing required Alpha platform file: {relative}")
        return ""
    try:
        return path.read_text(encoding="utf-8")
    except OSError as error:
        errors.append(f"cannot read {relative}: {error}")
        return ""


def validate(root: Path) -> list[str]:
    errors: list[str] = []
    for relative in REQUIRED_FILES:
        if not (root / relative).is_file():
            errors.append(f"missing required Alpha platform file: {relative}")

    project_text = _read(root, "DarkArisen.uproject", errors)
    if project_text:
        try:
            project = json.loads(project_text)
        except json.JSONDecodeError as error:
            errors.append(f"invalid DarkArisen.uproject: {error}")
        else:
            if project.get("EngineAssociation") != "5.5":
                errors.append("Alpha packaging must remain pinned to Unreal Engine 5.5")
            pixel_streaming = next(
                (
                    plugin
                    for plugin in project.get("Plugins", [])
                    if plugin.get("Name") == "PixelStreaming2"
                ),
                None,
            )
            if not pixel_streaming or pixel_streaming.get("Enabled") is not True:
                errors.append("PixelStreaming2 must be enabled for Alpha packages")
            elif set(pixel_streaming.get("TargetAllowList", [])) != {"Win64", "Linux"}:
                errors.append(
                    "PixelStreaming2 Alpha target allow-list must be exactly Win64 and Linux"
                )

    workflow = _read(root, ".github/workflows/ci.yml", errors)
    workflow_fragments = (
        "verify-windows:",
        "runs-on: [self-hosted, Windows, X64, ue5.5, dark-arisen]",
        "verify-linux:",
        "runs-on: [self-hosted, Linux, X64, ue5.5, dark-arisen]",
        "./Tools/ci/verify-runner.ps1",
        "./Tools/ci/verify-linux-runner.sh",
        "./Tools/ci/build-unreal.ps1 -RunAutomationTests",
        "./Tools/ci/build-unreal-linux.sh --run-automation-tests",
    )
    for fragment in workflow_fragments:
        if fragment not in workflow:
            errors.append(f"dual-platform CI requirement missing: {fragment}")
    if workflow.count("validate_alpha_platforms.py") != 2:
        errors.append("Alpha platform validation must run in both Windows and Linux jobs")
    if "actions/upload-artifact" in workflow.lower():
        errors.append("private game packages must never be uploaded as GitHub Actions artifacts")

    windows_package = _read(root, "Tools/ci/package-alpha-windows.ps1", errors)
    for fragment in (
        "BuildCookRun",
        "-platform=Win64",
        "-clientconfig=Shipping",
        "-allmaps",
        "-pak",
        "git.exe rev-parse HEAD",
        "DarkArisen-Candidate-Windows-Arcware-",
    ):
        if fragment not in windows_package:
            errors.append(f"Windows candidate packaging requirement missing: {fragment}")

    linux_package = _read(root, "Tools/ci/package-alpha-linux.sh", errors)
    for fragment in (
        "BuildCookRun",
        "-platform=Linux",
        "-clientconfig=Shipping",
        "-allmaps",
        "-pak",
        "git rev-parse HEAD",
        "DarkArisen-Candidate-Linux-x86_64-",
    ):
        if fragment not in linux_package:
            errors.append(f"Linux candidate packaging requirement missing: {fragment}")

    promoter = _read(root, "Tools/ci/promote-alpha-candidates.py", errors)
    for artifact in FINAL_ARTIFACTS:
        if artifact not in promoter:
            errors.append(f"final Alpha artifact name missing from promoter: {artifact}")

    platform_contract = _read(root, "Docs/ALPHA_PLATFORM_ARTIFACTS.md", errors)
    delivery_checklist = _read(root, "Docs/ALPHA_DELIVERY_CHECKLIST.md", errors)
    for artifact in FINAL_ARTIFACTS:
        if artifact not in platform_contract or artifact not in delivery_checklist:
            errors.append(f"final Alpha artifact is not documented consistently: {artifact}")
    for fragment in (
        "## Post-Alpha / Beta — native PS5",
        "authorised Sony tools",
        "not an Alpha deliverable",
    ):
        if fragment not in platform_contract + delivery_checklist:
            errors.append(f"PS5 Beta deferral requirement missing: {fragment}")

    m8_start = delivery_checklist.find("## M8 —")
    beta_start = delivery_checklist.find("## Post-Alpha / Beta —")
    if m8_start < 0 or beta_start < 0 or beta_start <= m8_start:
        errors.append("the Beta PS5 section must follow the M8 Alpha section")
    else:
        alpha_m8 = delivery_checklist[m8_start:beta_start]
        for forbidden in ("PS5 dev-kit", "native PS5 test package", "PS5 SDK"):
            if forbidden in alpha_m8:
                errors.append(f"PS5 work is forbidden inside Alpha M8: {forbidden}")

    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[2])
    args = parser.parse_args()
    errors = validate(args.root.resolve())
    if errors:
        print("Alpha platform validation failed:", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1
    print("Alpha platform validation passed: private Windows + Linux; PS5 deferred to Beta.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
