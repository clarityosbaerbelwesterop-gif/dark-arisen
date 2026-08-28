#!/usr/bin/env python3
"""Deterministic source contract for the M3 ship and M4 systems foundations."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

REQUIRED_FILES = (
    "Source/DarkArisen/Ship/ShipVoyageComponent.h",
    "Source/DarkArisen/Ship/ShipVoyageComponent.cpp",
    "Source/DarkArisen/Systems/ProgressionEconomyComponent.h",
    "Source/DarkArisen/Systems/ProgressionEconomyComponent.cpp",
    "Source/DarkArisen/World/DarkArisenWorldRulesSubsystem.h",
    "Source/DarkArisen/World/DarkArisenWorldRulesSubsystem.cpp",
)

CANONICAL_TEACHERS = (
    "teacher.mira",
    "teacher.big_tom",
    "teacher.ines",
    "teacher.father_salvio",
    "teacher.esteban",
    "teacher.mama_jacinta",
    "teacher.mbah_seruni",
    "teacher.mormor_astrid",
    "teacher.don_alejandro",
    "teacher.mateusz_voltari",
    "teacher.cassandra_wells",
    "teacher.dr_eleanor_whitcomb",
    "teacher.lt_marcus_sandford",
    "teacher.yara_sun_walker",
    "teacher.pak_setyo",
    "teacher.bu_wira",
    "teacher.isolde_thorn",
    "teacher.gruffydd",
    "teacher.maeve_donovan",
    "teacher.margarethe_fitzmueller",
    "teacher.admiral_sterling",
    "teacher.bram_kettle",
    "teacher.wooden_tooth_holloway",
)

CANONICAL_CREW = (
    "crew.mira",
    "crew.big_tom",
    "crew.esteban",
    "crew.ines",
    "crew.father_salvio",
)

PROHIBITED_SOURCE_PATTERNS = (
    ("fast travel API", re.compile(r"\b(?:FastTravel|TeleportToWaypoint)\b")),
    ("compass API", re.compile(r"\b(?:CompassHeading|CompassWidget|Minimap)\b")),
    ("respec API", re.compile(r"\b(?:Respec|ResetSkillTree|RefundMarks)\b")),
    ("generic currency conversion API", re.compile(r"\b(?:ConvertCurrency|ExchangeCurrency|ExchangeRate)\b")),
    ("relationship meter", re.compile(r"\b(?:AffinityMeter|RelationshipMeter|ApprovalMeter)\b")),
    ("autosave-before-choice API", re.compile(r"\b(?:AutosaveBeforeDecision|AutosaveBeforeBoss)\b")),
)


def _read(root: Path, relative: str, errors: list[str]) -> str:
    path = root / relative
    if not path.is_file():
        errors.append(f"missing M3/M4 source-contract file: {relative}")
        return ""
    return path.read_text(encoding="utf-8")


def _require(text: str, owner: str, fragments: tuple[str, ...], errors: list[str]) -> None:
    for fragment in fragments:
        if fragment not in text:
            errors.append(f"{owner} requirement missing: {fragment}")


def validate(root: Path) -> list[str]:
    errors: list[str] = []
    texts = {relative: _read(root, relative, errors) for relative in REQUIRED_FILES}

    ship_h = texts["Source/DarkArisen/Ship/ShipVoyageComponent.h"]
    ship_cpp = texts["Source/DarkArisen/Ship/ShipVoyageComponent.cpp"]
    progression_h = texts["Source/DarkArisen/Systems/ProgressionEconomyComponent.h"]
    progression_cpp = texts["Source/DarkArisen/Systems/ProgressionEconomyComponent.cpp"]
    world_h = texts["Source/DarkArisen/World/DarkArisenWorldRulesSubsystem.h"]
    world_cpp = texts["Source/DarkArisen/World/DarkArisenWorldRulesSubsystem.cpp"]

    _require(ship_h, "ShipVoyageComponent.h", (
        "enum class EShipDeck",
        "Weather,",
        "Upper,",
        "Mid,",
        "Hold",
        "enum class EPointOfSail",
        "InIrons,",
        "CloseHauled,",
        "CloseReach,",
        "BeamReach,",
        "BroadReach,",
        "Running",
        "FPhysicalChartRecord",
        "ActiveHands = 40",
        "GetRequiredDeckCount() { return 4; }",
        "InternalMorale",
        "no water fast-travel API exists here",
    ), errors)
    _require(ship_cpp, "ShipVoyageComponent.cpp", (
        "RecalculatePointOfSail();",
        "GetCrewHandlingFactor()",
        "SetHelmCommandDegrees",
        "AcquirePhysicalChart",
        "AnnotatePhysicalChart",
        "FMath::FindDeltaAngleDegrees",
        "ActiveHands = FMath::Clamp(ActiveHands, 0, 90)",
    ), errors)
    for crew_id in CANONICAL_CREW:
        if crew_id not in ship_cpp:
            errors.append(f"ShipVoyageComponent.cpp missing canonical crew id: {crew_id}")
    if ship_cpp.count("AddCrew(TEXT(") != 5:
        errors.append("ShipVoyageComponent.cpp must author exactly five named crew entries")

    _require(progression_h, "ProgressionEconomyComponent.h", (
        "Doubloons,",
        "Pounds,",
        "SilverMarks",
        "Stranger,",
        "Known,",
        "Owed,",
        "Wary",
        "RequiredSkillNodeCount = 68",
        "RequiredTeacherCount = 23",
        "MaximumAvailableMarks = 94",
        "FullTreeMarkCost = 141",
        "MaximumHealth = 200",
        "MaximumStamina = 120",
        "MaximumPosture = 100",
        "CarryKilograms = 80.0f",
        "no respec path exists",
        "no generic conversion API",
    ), errors)
    _require(progression_cpp, "ProgressionEconomyComponent.cpp", (
        "MaximumHealth = FMath::Min(380, MaximumHealth + 15)",
        "MaximumStamina = FMath::Min(200, MaximumStamina + 5)",
        "MaximumPosture = FMath::Min(175, MaximumPosture + 5)",
        "AuthoredCarryKg > 130.0f",
        "MarksEarned > MaximumAvailableMarks - Amount",
        "SkillNodeDefinitions.Num() >= RequiredSkillNodeCount",
        "DefinitionCanBeLearned",
        "TeachersMet.Contains",
        "WorldFlags.Contains",
        "CreditCurrency",
        "SpendCurrency",
        "BeginListening",
        "InterruptListening",
        "CompleteListening",
        "The remaining authored nodes are not invented here",
    ), errors)
    for teacher_id in CANONICAL_TEACHERS:
        if teacher_id not in progression_cpp:
            errors.append(f"ProgressionEconomyComponent.cpp missing canonical teacher id: {teacher_id}")
    if progression_cpp.count('TEXT("teacher.') < 23:
        errors.append("ProgressionEconomyComponent.cpp must contain the complete 23-teacher catalog")

    _require(world_h, "DarkArisenWorldRulesSubsystem.h", (
        "UTickableWorldSubsystem",
        "RealSecondsPerGameHour = 150.0f",
        "NotifyChapterBoundary",
        "NotifyRestCompleted",
        "BeginLakeToDockAutosaveSuppression",
        "EndLakeToDockAutosaveSuppression",
        "CanManualSave() const { return true; }",
        "ConsumePendingAutosaveRequest",
        "performs no disk IO",
    ), errors)
    _require(world_cpp, "DarkArisenWorldRulesSubsystem.cpp", (
        "GameMinutesPerRealSecond",
        "QueueLegalAutosaveRequest();",
        "bAutosaveSuppressed = true",
        "bPendingAutosaveRequest = false",
        "if (!bAutosaveSuppressed)",
    ), errors)

    combined = "\n".join(texts.values())
    for label, pattern in PROHIBITED_SOURCE_PATTERNS:
        matches = pattern.findall(combined)
        if matches:
            errors.append(f"prohibited {label} detected in M3/M4 source: {', '.join(sorted(set(matches)))}")

    if "ConvertCurrency(" in progression_h or "ConvertCurrency(" in progression_cpp:
        errors.append("M4 economy exposes a generic conversion path")

    return errors


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[2])
    args = parser.parse_args(argv)

    errors = validate(args.root.resolve())
    if errors:
        print("M3/M4 source validation failed:", file=sys.stderr)
        for error in errors:
            print(f" - {error}", file=sys.stderr)
        return 1

    print("M3/M4 source validation passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
