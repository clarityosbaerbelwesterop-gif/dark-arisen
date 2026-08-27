#!/usr/bin/env python3
"""Fail CI when code introduces a mechanically detectable design-law breach."""

from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path


SOURCE_SUFFIXES = {".h", ".hpp", ".c", ".cc", ".cpp"}
RACHE_IMPLEMENTATION = Path("Source/DarkArisen/Components/CombatComponent.cpp")


@dataclass(frozen=True)
class Finding:
    path: Path
    line: int
    rule: str
    excerpt: str


PROHIBITED_IDENTIFIERS: tuple[tuple[str, re.Pattern[str]], ...] = (
    ("Combat HUD remains posture-only", re.compile(
        r"\b(?:HealthBarWidget|StaminaBarWidget|AmmoCounterWidget|CombatMiniMap|DamageNumber)\b",
        re.I,
    )),
    ("No interaction highlight", re.compile(
        r"\b(?:SetRenderCustomDepth|RenderCustomDepth|HighlightNearbyLoot|FocusModeHighlight)\b",
        re.I,
    )),
    ("No generic loot-container shortcut", re.compile(
        r"\b(?:TakeAllLoot|GenericLootContainer|LootBarrel)\b",
        re.I,
    )),
    ("No action cancellation", re.compile(
        r"\b(?:CancelIntoDodge|CancelIntoBlock|CanCancelAttack)\b",
        re.I,
    )),
    ("No cinematic action vocabulary", re.compile(
        r"\b(?:KillCam|FinisherCamera|SpeedRamp|DutchAngle|ShakeOnDamage|CameraShakeOnImpact)\b",
        re.I,
    )),
    ("No dynamic music", re.compile(r"\b(?:DynamicMusic|AdaptiveMusic|MusicStinger)\b", re.I)),
    ("No fast travel over water", re.compile(r"\b(?:WaterFastTravel|FastTravelAcrossWater)\b", re.I)),
    ("No interface navigation aids", re.compile(r"\b(?:MiniMap|QuestMarker|DamageNumber)\b", re.I)),
    ("No systemic romance", re.compile(r"\bRomanceSystem\b", re.I)),
    ("The undercroft is never authored", re.compile(r"\b(?:UndercroftMap|UndercroftLevel)\b", re.I)),
)


def _strip_comments(text: str) -> str:
    """Remove C/C++ comments while preserving newlines for accurate reports."""
    text = re.sub(r"/\*.*?\*/", lambda match: "\n" * match.group(0).count("\n"), text, flags=re.S)
    return re.sub(r"//[^\n]*", "", text)


def audit(root: Path) -> list[Finding]:
    findings: list[Finding] = []
    source_root = root / "Source"

    for path in sorted(source_root.rglob("*")):
        if not path.is_file() or path.suffix.lower() not in SOURCE_SUFFIXES:
            continue

        relative = path.relative_to(root)
        content = _strip_comments(path.read_text(encoding="utf-8"))
        lines = content.splitlines()

        for rule, pattern in PROHIBITED_IDENTIFIERS:
            for index, line in enumerate(lines, start=1):
                if pattern.search(line):
                    findings.append(Finding(relative, index, rule, line.strip()))

        for index, line in enumerate(lines, start=1):
            if not re.search(r"\b(?:SetGlobalTimeDilation|SetTimeDilation|CustomTimeDilation)\b", line):
                continue
            if relative != RACHE_IMPLEMENTATION or "SetGlobalTimeDilation" not in line:
                findings.append(Finding(
                    relative,
                    index,
                    "Time dilation is confined to Rache",
                    line.strip(),
                ))

        if relative == RACHE_IMPLEMENTATION:
            if "DesignLaws::RacheTimeScale" not in content:
                findings.append(Finding(
                    relative,
                    1,
                    "Rache must use the locked 30% constant",
                    "DesignLaws::RacheTimeScale is missing",
                ))
            if content.count("SetGlobalTimeDilation") != 2:
                findings.append(Finding(
                    relative,
                    1,
                    "Rache owns exactly one enter and one restore call",
                    f"found {content.count('SetGlobalTimeDilation')} calls",
                ))

    return findings


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[2])
    args = parser.parse_args()
    root = args.root.resolve()

    findings = audit(root)
    if findings:
        print("Design-law audit failed:", file=sys.stderr)
        for finding in findings:
            print(
                f"- {finding.path}:{finding.line}: {finding.rule}: {finding.excerpt}",
                file=sys.stderr,
            )
        return 1

    print("Design-law audit passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
