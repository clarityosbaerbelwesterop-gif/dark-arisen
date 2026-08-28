#!/usr/bin/env python3
"""Deterministic source-law validator for M5 Colonial War and M6 Highmoore."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

REQUIRED_FILES = (
    "Source/DarkArisen/ColonialWar/ColonialWarStateSubsystem.h",
    "Source/DarkArisen/ColonialWar/ColonialWarStateSubsystem.cpp",
    "Source/DarkArisen/ColonialWar/RetaliationSubsystem.h",
    "Source/DarkArisen/ColonialWar/RetaliationSubsystem.cpp",
    "Source/DarkArisen/ColonialWar/CastleSiegeComponent.h",
    "Source/DarkArisen/ColonialWar/CastleSiegeComponent.cpp",
    "Source/DarkArisen/ColonialWar/ArmyCampaignComponent.h",
    "Source/DarkArisen/ColonialWar/ArmyCampaignComponent.cpp",
    "Source/DarkArisen/ColonialWar/LargeBattleComponent.h",
    "Source/DarkArisen/ColonialWar/LargeBattleComponent.cpp",
    "Source/DarkArisen/Highmoore/CrystalCavesPassageComponent.h",
    "Source/DarkArisen/Highmoore/CrystalCavesPassageComponent.cpp",
    "Source/DarkArisen/Highmoore/CrystalGuardianComponent.h",
    "Source/DarkArisen/Highmoore/CrystalGuardianComponent.cpp",
    "Source/DarkArisen/Highmoore/CrystalKatanaComponent.h",
    "Source/DarkArisen/Highmoore/CrystalKatanaComponent.cpp",
    "Source/DarkArisen/Highmoore/HighmooreHorseComponent.h",
    "Source/DarkArisen/Highmoore/HighmooreHorseComponent.cpp",
    "Source/DarkArisen/Highmoore/PrincessQuestStateComponent.h",
    "Source/DarkArisen/Highmoore/PrincessQuestStateComponent.cpp",
    "Source/DarkArisen/ColonialWar/ColonyHoldingComponent.h",
    "Source/DarkArisen/ColonialWar/ColonyHoldingComponent.cpp",
)

PROHIBITED = (
    ("fast travel or teleport API", re.compile(r"\b(?:FastTravel|TeleportToWaypoint|OpenLevel|ServerTravel|SetActorLocation)\b")),
    ("compass/minimap/player-dot API", re.compile(r"\b(?:CompassWidget|CompassHeading|Minimap|PlayerDot)\b")),
    ("systemic romance/relationship meter", re.compile(r"\b(?:RomanceSystem|RomanceTree|AffinityMeter|RelationshipMeter|ApprovalMeter)\b")),
    ("systemic magic API", re.compile(r"\b(?:CastSpell|Spellbook|ManaPool|ManaMeter)\b")),
    ("illegal autosave helper", re.compile(r"\b(?:AutosaveBeforeDecision|AutosaveBeforeBoss)\b")),
    ("Belos undercroft rendering", re.compile(r"\b(?:RenderBelosUndercroft|UndercroftActor|BelosUndercroftLevel)\b")),
    ("Crystal Katana maintenance", re.compile(r"\b(?:RepairCrystalKatana|UpgradeCrystalKatana|SharpenCrystalKatana|KatanaDurability)\b")),
    ("Princess revive/slow-motion helper", re.compile(r"\b(?:RevivePrincess|SlowMotionArrow)\b")),
    ("RTS army UI/control", re.compile(r"\b(?:UnitCard|SelectionBox|FormationOrder|RTSPanel)\b")),
)


def _read(root: Path, relative: str, errors: list[str]) -> str:
    path = root / relative
    if not path.is_file():
        errors.append(f"missing M5/M6 source-contract file: {relative}")
        return ""
    return path.read_text(encoding="utf-8")


def _require(text: str, owner: str, fragments: tuple[str, ...], errors: list[str]) -> None:
    for fragment in fragments:
        if fragment not in text:
            errors.append(f"{owner} requirement missing: {fragment}")


def validate(root: Path) -> list[str]:
    errors: list[str] = []
    texts = {relative: _read(root, relative, errors) for relative in REQUIRED_FILES}

    war_h = texts[REQUIRED_FILES[0]]
    war_cpp = texts[REQUIRED_FILES[1]]
    retaliation_h = texts[REQUIRED_FILES[2]]
    retaliation_cpp = texts[REQUIRED_FILES[3]]
    siege_h = texts[REQUIRED_FILES[4]]
    siege_cpp = texts[REQUIRED_FILES[5]]
    army_h = texts[REQUIRED_FILES[6]]
    army_cpp = texts[REQUIRED_FILES[7]]
    battle_h = texts[REQUIRED_FILES[8]]
    battle_cpp = texts[REQUIRED_FILES[9]]
    caves_h = texts[REQUIRED_FILES[10]]
    caves_cpp = texts[REQUIRED_FILES[11]]
    guardian_h = texts[REQUIRED_FILES[12]]
    guardian_cpp = texts[REQUIRED_FILES[13]]
    katana_h = texts[REQUIRED_FILES[14]]
    katana_cpp = texts[REQUIRED_FILES[15]]
    horse_h = texts[REQUIRED_FILES[16]]
    horse_cpp = texts[REQUIRED_FILES[17]]
    princess_h = texts[REQUIRED_FILES[18]]
    princess_cpp = texts[REQUIRED_FILES[19]]
    holding_h = texts[REQUIRED_FILES[20]]
    holding_cpp = texts[REQUIRED_FILES[21]]

    _require(war_h, "ColonialWarStateSubsystem.h", (
        "Imperial,", "Albion,", "Liberation,", "Crimson",
        "Alpha,", "Beta,", "Gamma", "Support,", "Break,", "Liberate",
        "FRegionalAutonomousWarTick", "RecordAutonomousChapterTick",
    ), errors)
    _require(war_cpp, "ColonialWarStateSubsystem.cpp", (
        "ColonialControl < 30", "LiberationStrength >= 50", "CrimsonThreat >= 50",
        "Chapter <= 6", "Chapter <= 8", "*LastTickChapter >= CurrentChapter",
    ), errors)

    _require(retaliation_h, "RetaliationSubsystem.h", (
        "Unnoticed = 0", "Letter = 1", "Bounty = 2", "CounterAttack = 3",
        "Hostages = 4", "ScorchedEarth = 5", "bool bChild = false",
        "bool bCrew = false", "PassiveChapterDecay = 5",
    ), errors)
    _require(retaliation_cpp, "RetaliationSubsystem.cpp", (
        "Heat >= 90", "Heat >= 75", "Heat >= 60", "Heat >= 40", "Heat >= 20",
        "Chapter <= 6", "Chapter <= 8", "Candidate.bChild",
        "Candidate.bNarrativeProtected", "GetCurrentStage(Faction) < ERetaliationStage::Hostages",
    ), errors)
    if "Candidate.bCrew" in retaliation_cpp:
        errors.append("newer Phase-12 law violated: adult crew are incorrectly excluded from hostage selection")

    _require(siege_h, "CastleSiegeComponent.h", (
        "Approach,", "Investment,", "Breach,", "Assault,", "Wall,", "Back,", "Inside",
        "bPersistentBreachDamage",
    ), errors)
    _require(siege_cpp, "CastleSiegeComponent.cpp", (
        "Blind assault is legal", "bPersistentBreachDamage || bStructuralDamageInflicted",
        "InResolution == ESiegeResolution::HeldIntact",
    ), errors)

    _require(holding_h, "ColonyHoldingComponent.h", (
        "TradePost,", "Settlement,", "MilitaryStronghold,", "AllianceBastion",
        "Claimed = 0", "Functional = 1", "Established = 2", "Developed = 3", "Thriving = 4",
        "FDeliveredHoldingPerson", "VoyageId", "IsJakeOwned() const",
    ), errors)
    _require(holding_cpp, "ColonyHoldingComponent.cpp", (
        "bWasRazed", "bOldFortressForAlliance", "DeliveredPeople.IsEmpty()",
        "HoldingType == EHoldingType::AllianceBastion", "DeliveredPeople.Add(Person.PersonId, Person)",
        "PersistentDamageIds.Add", "RecordDamageRepaired",
    ), errors)

    _require(army_h, "ArmyCampaignComponent.h", (
        "Hired,", "SettlementMilitia,", "Alliance,", "FactionLevy,", "Crew",
        "Hold,", "Press,", "BreakOff", "JakeBaseCommandCapacity = 200",
        "CapacityPerNamedOfficer = 150", "HiredUpkeepPerMan = 14", "MilitiaUpkeepPerMan = 4",
    ), errors)
    _require(army_cpp, "ArmyCampaignComponent.cpp", (
        "Force->Type == ECampaignForceType::Alliance", "Rate *= 3", "Rate *= 2",
        "Force.bCompanyMarines && Unpaid >= 1", "Force.Type == ECampaignForceType::Hired && Unpaid >= 2",
        "Pair.Value.Type != ECampaignForceType::Crew",
    ), errors)

    _require(battle_h, "LargeBattleComponent.h", (
        "FieldAction,", "Assault,", "Defence,", "Rising",
        "FBattleSegmentRuntimeState", "Cohesion = 100", "MinimumSegments = 3", "MaximumSegments = 6",
    ), errors)
    _require(battle_cpp, "LargeBattleComponent.cpp", (
        "RecordJakeFallen", "bJakeFallen = true", "BattleType == ELargeBattleType::Rising",
        "Segment->Cohesion = FMath::Clamp", "Segment->Cohesion == 0", "AreAllSegmentsResolved()",
    ), errors)
    if "RecordJakeFallen" in battle_cpp and "bBattleActive = false" in battle_cpp.split("RecordJakeFallen", 1)[1].split("ResolveBattle", 1)[0]:
        errors.append("Jake falling must not end a large battle")

    _require(caves_h, "CrystalCavesPassageComponent.h", (
        "FirstPassageMinimumRealSeconds = 90.0f * 60.0f",
        "RepeatPassageMinimumRealSeconds = 4.0f * 60.0f",
        "RepeatPassageMaximumAuthoredRealSeconds = 6.0f * 60.0f",
    ), errors)
    _require(caves_cpp, "CrystalCavesPassageComponent.cpp", (
        "ElapsedPassageSeconds >= RequiredRealSeconds", "ObservedDistanceMetres >= RequiredDistanceMetres",
        "bHighmoorePermanentlyOpen = true",
    ), errors)

    _require(guardian_h, "CrystalGuardianComponent.h", (
        "Dormant,", "Threshold,", "Columns,", "Span,", "LastStand,", "Settling,", "Defeated,", "Bypassed",
        "RecordLightStimulus", "RecordGuardianBypassed", "DefeatHoldSeconds = 4.0f",
    ), errors)
    _require(guardian_cpp, "CrystalGuardianComponent.cpp", (
        "Phase != ECrystalGuardianPhase::Dormant", "Phase = ECrystalGuardianPhase::Bypassed",
        "SettleElapsedSeconds >= DefeatHoldSeconds", "RemainingPercent > 75.0f",
        "RemainingPercent > 45.0f", "RemainingPercent > 20.0f",
    ), errors)

    _require(katana_h, "CrystalKatanaComponent.h", (
        "BladeLengthCentimetres = 74.0f", "WeightKilograms = 1.1f",
        "ResolveFlatAttackStaminaCost", "IgnoresConventionalArmour",
    ), errors)
    _require(katana_cpp, "CrystalKatanaComponent.cpp", ("return AuthoredBaseCost;",), errors)

    _require(horse_h, "HighmooreHorseComponent.h", (
        "Unknown = 0", "Familiar = 1", "Trusted = 2", "Bonded = 3",
        "Walk,", "Trot,", "Canter,", "Gallop",
        "LakeRideRequiredSeconds = 9.0f * 60.0f", "ArionRideRequiredSeconds = 4.0f * 60.0f",
        "BelosRideRequiredSeconds = 6.0f * 60.0f", "ReturnWalkRequiredSeconds = 40.0f * 60.0f",
    ), errors)
    _require(horse_cpp, "HighmooreHorseComponent.cpp", (
        "EHorseGait::Trot", "EHorseGait::Gallop", "EHorseGait::Walk",
        "bStaminaSuspendedForAuthoredRide", "bLastingRevengeRideDamage = true",
    ), errors)

    _require(princess_h, "PrincessQuestStateComponent.h", (
        "AllowsArrowSlowMotion() const { return false; }",
        "AllowsArrowMusicCueChange() const { return false; }",
        "AllowsArrowCameraMove() const { return false; }",
        "AllowsArrowTelegraph() const { return false; }",
        "AllowsArrowRevivePrompt() const { return false; }",
        "RetainsPlayerControlAtArrow() const { return true; }",
        "KatanaAssaultInterrupted", "OrdinaryWeaponOverwhelmed",
    ), errors)
    _require(princess_cpp, "PrincessQuestStateComponent.cpp", (
        "BeginLakeToDockAutosaveSuppression", "EndLakeToDockAutosaveSuppression",
        "bIsCombatant && !bIsChild", "EPrincessQuestMovement::TurnedWest",
    ), errors)

    combined = "\n".join(texts.values())
    for label, pattern in PROHIBITED:
        matches = pattern.findall(combined)
        if matches:
            errors.append(f"prohibited {label} detected in M5/M6 source: {', '.join(sorted(set(matches)))}")

    return errors


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[2])
    args = parser.parse_args(argv)
    errors = validate(args.root.resolve())
    if errors:
        print("M5/M6 source validation failed:", file=sys.stderr)
        for error in errors:
            print(f" - {error}", file=sys.stderr)
        return 1
    print("M5/M6 source validation passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
