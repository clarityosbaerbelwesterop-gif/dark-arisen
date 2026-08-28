#!/usr/bin/env python3
"""Deterministic source contract for completed M3/M4 source foundations."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

REQUIRED_FILES = (
    "Source/DarkArisen/Ship/ShipVoyageComponent.h",
    "Source/DarkArisen/Ship/ShipVoyageComponent.cpp",
    "Source/DarkArisen/Ship/LaLiberacionShip.h",
    "Source/DarkArisen/Ship/LaLiberacionShip.cpp",
    "Source/DarkArisen/Ship/ShipHouseholdComponent.h",
    "Source/DarkArisen/Ship/ShipHouseholdComponent.cpp",
    "Source/DarkArisen/Ship/SeaPassageComponent.h",
    "Source/DarkArisen/Ship/SeaPassageComponent.cpp",
    "Source/DarkArisen/Interaction/PhysicalMapActor.h",
    "Source/DarkArisen/Interaction/PhysicalMapActor.cpp",
    "Source/DarkArisen/Systems/ProgressionEconomyComponent.h",
    "Source/DarkArisen/Systems/ProgressionEconomyComponent.cpp",
    "Source/DarkArisen/Systems/ProgressionEconomySocial.cpp",
    "Source/DarkArisen/Systems/ChapterEconomyComponent.h",
    "Source/DarkArisen/Systems/ChapterEconomyComponent.cpp",
    "Source/DarkArisen/Systems/SkillTreeCatalogDataAsset.h",
    "Source/DarkArisen/Systems/SkillTreeCatalogDataAsset.cpp",
    "Source/DarkArisen/World/DarkArisenWorldRulesSubsystem.h",
    "Source/DarkArisen/World/DarkArisenWorldRulesSubsystem.cpp",
    "Source/DarkArisen/JakeCharacter.h",
    "Source/DarkArisen/JakeCharacter.cpp",
    "Source/DarkArisen/Rexa/RexaSettlementDirector.cpp",
    "Source/DarkArisen/Tests/M3M4SystemsSpec.cpp",
    "Source/DarkArisen/Tests/M3M4CompletionSpec.cpp",
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
    ship_actor_h = texts["Source/DarkArisen/Ship/LaLiberacionShip.h"]
    ship_actor_cpp = texts["Source/DarkArisen/Ship/LaLiberacionShip.cpp"]
    household_h = texts["Source/DarkArisen/Ship/ShipHouseholdComponent.h"]
    household_cpp = texts["Source/DarkArisen/Ship/ShipHouseholdComponent.cpp"]
    passage_h = texts["Source/DarkArisen/Ship/SeaPassageComponent.h"]
    passage_cpp = texts["Source/DarkArisen/Ship/SeaPassageComponent.cpp"]
    map_h = texts["Source/DarkArisen/Interaction/PhysicalMapActor.h"]
    map_cpp = texts["Source/DarkArisen/Interaction/PhysicalMapActor.cpp"]
    progression_h = texts["Source/DarkArisen/Systems/ProgressionEconomyComponent.h"]
    progression_cpp = texts["Source/DarkArisen/Systems/ProgressionEconomyComponent.cpp"]
    social_cpp = texts["Source/DarkArisen/Systems/ProgressionEconomySocial.cpp"]
    chapter_h = texts["Source/DarkArisen/Systems/ChapterEconomyComponent.h"]
    chapter_cpp = texts["Source/DarkArisen/Systems/ChapterEconomyComponent.cpp"]
    catalog_h = texts["Source/DarkArisen/Systems/SkillTreeCatalogDataAsset.h"]
    catalog_cpp = texts["Source/DarkArisen/Systems/SkillTreeCatalogDataAsset.cpp"]
    world_h = texts["Source/DarkArisen/World/DarkArisenWorldRulesSubsystem.h"]
    world_cpp = texts["Source/DarkArisen/World/DarkArisenWorldRulesSubsystem.cpp"]
    jake_h = texts["Source/DarkArisen/JakeCharacter.h"]
    jake_cpp = texts["Source/DarkArisen/JakeCharacter.cpp"]
    rexa_cpp = texts["Source/DarkArisen/Rexa/RexaSettlementDirector.cpp"]
    completion_spec = texts["Source/DarkArisen/Tests/M3M4CompletionSpec.cpp"]

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
        "Owner->AddActorWorldOffset(DeltaCentimetres, true)",
        "ForwardSpeedMetresPerSecond * 100.0f * DeltaTime",
    ), errors)
    for crew_id in CANONICAL_CREW:
        if crew_id not in ship_cpp:
            errors.append(f"ShipVoyageComponent.cpp missing canonical crew id: {crew_id}")
    if ship_cpp.count("AddCrew(TEXT(") != 5:
        errors.append("ShipVoyageComponent.cpp must author exactly five named crew entries")

    _require(ship_actor_h, "LaLiberacionShip.h", (
        "WeatherDeckRoot",
        "UpperDeckRoot",
        "MidDeckRoot",
        "HoldDeckRoot",
        "UShipHouseholdComponent",
        "USeaPassageComponent",
        "APhysicalMapActor",
        "RestGreatCabinToDaypart",
    ), errors)
    _require(ship_actor_cpp, "LaLiberacionShip.cpp", (
        'CreateDefaultSubobject<UShipHouseholdComponent>(TEXT("HouseholdComponent"))',
        'CreateDefaultSubobject<USeaPassageComponent>(TEXT("SeaPassageComponent"))',
        "SpawnPhysicalMap()",
        "EDarkArisenRestLocation::GreatCabin",
    ), errors)

    _require(household_h, "ShipHouseholdComponent.h", (
        "ECrewHouseholdActivity",
        "ECrewMoraleRead",
        "ECrewMoraleCue",
        "SpecialistBerthsOccupied",
        "SemiNamedHands = 12",
        "InternalMorale",
        "ApplyGameMinute",
    ), errors)
    _require(household_cpp, "ShipHouseholdComponent.cpp", (
        "NamedReads.Num() == 5",
        "Count > 8",
        "Count > 12",
        "ECrewMoraleCue::EmptyHammock",
        "ECrewMoraleCue::EmptyForecastleAtDusk",
        "World->GetSubsystem<UDarkArisenWorldRulesSubsystem>()",
    ), errors)
    for crew_id in CANONICAL_CREW:
        if crew_id not in household_cpp:
            errors.append(f"ShipHouseholdComponent.cpp missing scheduled crew id: {crew_id}")

    _require(passage_h, "SeaPassageComponent.h", (
        "BeginSeaPassage",
        "CanCompleteSeaPassage",
        "CompleteSeaPassage",
        "RequiredGameMinutes",
        "RequiredDistanceMetres",
        "SailedDistanceMetres",
        "never moves the vessel",
    ), errors)
    _require(passage_cpp, "SeaPassageComponent.cpp", (
        "FVector::Distance(Current, LastObservedLocation)",
        "ElapsedMinutes >= RequiredGameMinutes",
        "SailedDistanceMetres >= RequiredDistanceMetres",
    ), errors)
    for forbidden in ("OpenLevel(", "ServerTravel(", "SetActorLocation(", "TeleportToWaypoint("):
        if forbidden in passage_cpp:
            errors.append(f"SeaPassageComponent.cpp contains forbidden instant-travel path: {forbidden}")

    _require(map_h, "PhysicalMapActor.h", (
        "AExamineDocumentActor",
        "RecordSailedCoastline",
        "RecordVisitedSettlement",
        "RecordGivenRoute",
        "AddHandwrittenAnnotation",
        "no player position",
    ), errors)
    _require(map_cpp, "PhysicalMapActor.cpp", (
        'TEXT("Unfold map")',
        'TEXT("Coastlines sailed")',
        'TEXT("Settlements visited")',
        'TEXT("Routes given")',
        'TEXT("In Jake\'s hand")',
    ), errors)

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
        "RequiredTeacherGatedNodeCount = 23",
        "RequiredStandingGatedNodeCount = 11",
        "MaximumAvailableMarks = 94",
        "FullTreeMarkCost = 141",
        "MaximumHealth = 200",
        "MaximumStamina = 120",
        "MaximumPosture = 100",
        "CarryKilograms = 80.0f",
        "RecordWeaponDrawnHere",
        "AdvanceSocialChapter",
        "BeginSitting",
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
        "CompleteTeachingScene",
        "HasCompletedTeachingScene",
        "CreditCurrency",
        "SpendCurrency",
        "BeginListening",
        "InterruptListening",
        "CompleteListening",
        "The remaining authored nodes are not invented here",
    ), errors)
    _require(social_cpp, "ProgressionEconomySocial.cpp", (
        "GreetingBeforeWary",
        "WaryChaptersRemaining",
        "ESocialGreetingState::Wary",
        "InterruptListening();",
        "ActiveSeatId = SeatId",
    ), errors)
    for teacher_id in CANONICAL_TEACHERS:
        if teacher_id not in progression_cpp:
            errors.append(f"ProgressionEconomyComponent.cpp missing canonical teacher id: {teacher_id}")
    if progression_cpp.count('TEXT("teacher.') < 23:
        errors.append("ProgressionEconomyComponent.cpp must contain the complete 23-teacher catalog")

    _require(chapter_h, "ChapterEconomyComponent.h", (
        "FChapterLedger",
        "HoldingIncome",
        "ArmyUpkeep",
        "GarrisonUpkeep",
        "ConstructionDraw",
        "PayLegendaryTierCost",
        "PayReconstructionSilver",
        "PayCrewShare",
        "No interest, investment or compounding API exists",
    ), errors)
    _require(chapter_cpp, "ChapterEconomyComponent.cpp", (
        "Chapter <= LastResolvedChapter",
        "PendingLedger.NetDoubloons()",
        "EDarkArisenCurrency::SilverMarks",
        "PaidLegendaryWork",
        "PaidReconstructionProjects",
        "PaidCrewShareChapters",
    ), errors)

    _require(catalog_h, "SkillTreeCatalogDataAsset.h", (
        "USkillTreeCatalogDataAsset",
        "TArray<FSkillNodeDefinition> Nodes",
        "IsCatalogStructurallyValid",
        "RegisterInto",
        "68 / 16-12-14-13-13 / 141 / 23 teacher / 11 Standing",
    ), errors)
    _require(catalog_cpp, "SkillTreeCatalogDataAsset.cpp", (
        "Nodes.Num() != UProgressionEconomyComponent::RequiredSkillNodeCount",
        "Blade != 16 || Shadow != 12 || Sea != 14 || Land != 13 || Word != 13",
        "RequiredTeacherGatedNodeCount",
        "RequiredStandingGatedNodeCount",
        "!Ids.Contains(Prerequisite)",
        "Progression->IsSkillCatalogComplete()",
    ), errors)

    _require(world_h, "DarkArisenWorldRulesSubsystem.h", (
        "UTickableWorldSubsystem",
        "RealSecondsPerGameHour = 150.0f",
        "EDarkArisenRestLocation",
        "GreatCabin,",
        "SafeHouse",
        "EDarkArisenDaypart",
        "Dawn,",
        "Midday,",
        "Dusk,",
        "Night",
        "CompleteRest",
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
        "GetDaypartMinute",
        "DeltaMinutes += MinutesPerDay",
        "QueueLegalAutosaveRequest();",
        "bAutosaveSuppressed = true",
        "bPendingAutosaveRequest = false",
        "if (!bAutosaveSuppressed)",
    ), errors)

    _require(jake_h, "JakeCharacter.h", (
        "class UProgressionEconomyComponent;",
        "TObjectPtr<UProgressionEconomyComponent> ProgressionEconomyComponent;",
    ), errors)
    _require(jake_cpp, "JakeCharacter.cpp", (
        '#include "Systems/ProgressionEconomyComponent.h"',
        'CreateDefaultSubobject<UProgressionEconomyComponent>(TEXT("ProgressionEconomyComponent"))',
    ), errors)
    _require(rexa_cpp, "RexaSettlementDirector.cpp", (
        '#include "World/DarkArisenWorldRulesSubsystem.h"',
        "GetSubsystem<UDarkArisenWorldRulesSubsystem>()",
        "WorldRules->GetTotalWorldMinutes()",
        "CanonicalGameMinute != LastAppliedGameMinute",
    ), errors)

    _require(completion_spec, "M3M4CompletionSpec.cpp", (
        "DarkArisen.M3.HouseholdCompletion",
        "Passage cannot complete instantly",
        "Recent loss produces the empty-hammock world cue",
        "DarkArisen.M4.CompletionContracts",
        "Same chapter cannot compound",
        "Suppressed rest cannot queue autosave",
        "Exact 68-node structural contract validates",
        "Wrong total Mark cost fails closed",
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
