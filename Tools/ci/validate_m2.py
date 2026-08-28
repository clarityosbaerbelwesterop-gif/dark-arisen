#!/usr/bin/env python3
"""Deterministic source contract for the native M2 quest, environment and encounter tranche."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path


REQUIRED_FILES = (
    "Source/DarkArisen/Components/QuestJournalComponent.h",
    "Source/DarkArisen/Components/QuestJournalComponent.cpp",
    "Source/DarkArisen/Components/HeatExposureComponent.h",
    "Source/DarkArisen/Components/HeatExposureComponent.cpp",
    "Source/DarkArisen/Bosses/IsabelCruzCharacter.h",
    "Source/DarkArisen/Bosses/IsabelCruzCharacter.cpp",
    "Source/DarkArisen/Components/WaterBreathComponent.h",
    "Source/DarkArisen/Components/WaterBreathComponent.cpp",
    "Source/DarkArisen/Dungeons/CenoteFirstMotherComponent.h",
    "Source/DarkArisen/Dungeons/CenoteFirstMotherComponent.cpp",
    "Source/DarkArisen/Missions/RexaM2MissionCatalog.h",
    "Source/DarkArisen/Missions/RexaM2MissionCatalog.cpp",
    "Source/DarkArisen/Interaction/PhysicalJournalActor.h",
    "Source/DarkArisen/Interaction/PhysicalJournalActor.cpp",
    "Source/DarkArisen/Rexa/RexaSettlementRoster.h",
    "Source/DarkArisen/Rexa/RexaSettlementRoster.cpp",
    "Source/DarkArisen/Rexa/RexaSettlementResident.h",
    "Source/DarkArisen/Rexa/RexaSettlementResident.cpp",
    "Source/DarkArisen/Rexa/RexaSettlementDirector.h",
    "Source/DarkArisen/Rexa/RexaSettlementDirector.cpp",
    "Docs/M2_VERTICAL_SLICE.md",
)


def _require_fragments(path: Path, fragments: tuple[str, ...], errors: list[str]) -> None:
    if not path.is_file():
        return
    text = path.read_text(encoding="utf-8")
    for fragment in fragments:
        if fragment not in text:
            errors.append(f"{path.name} requirement missing: {fragment}")


def validate(root: Path) -> list[str]:
    errors: list[str] = []
    for relative in REQUIRED_FILES:
        if not (root / relative).is_file():
            errors.append(f"missing M2 source-contract file: {relative}")

    header = root / "Source/DarkArisen/Components/QuestJournalComponent.h"
    source = root / "Source/DarkArisen/Components/QuestJournalComponent.cpp"
    _require_fragments(header, (
        "Conversation,",
        "Overheard,",
        "Object,",
        "Proximity,",
        "WorldState,",
        "Absence,",
        "bSilentAtActivation",
        "bRequiresSpokenAgreement",
        "bInitialEntryDistorted",
        "ExpirationDurationMinutes",
        "MutuallyExclusiveQuestIds",
        "SearchJournal",
        "SaveGame",
    ), errors)
    _require_fragments(source, (
        "RuntimeState->Lifecycle != EQuestLifecycle::Dormant",
        "Definition->bRequiresSpokenAgreement && !bSpokenAgreementConfirmed",
        "Definition->bSilentAtActivation",
        "ApplyMutualExclusions",
        "AppendJournalCorrection",
        "Entry.Sequence = NextJournalSequence++",
        "ProcessExpirations",
        "RuntimeState.OutcomeId = ExpiredOutcomeId",
        "ESearchCase::IgnoreCase",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/JakeCharacter.cpp", (
        'CreateDefaultSubobject<UQuestJournalComponent>(TEXT("QuestJournalComponent"))',
        'CreateDefaultSubobject<UHeatExposureComponent>(TEXT("HeatExposureComponent"))',
        'CreateDefaultSubobject<UWaterBreathComponent>(TEXT("WaterBreathComponent"))',
    ), errors)
    heat_header = root / "Source/DarkArisen/Components/HeatExposureComponent.h"
    heat_source = root / "Source/DarkArisen/Components/HeatExposureComponent.cpp"
    _require_fragments(heat_header, (
        "Inactive,",
        "CoolCistern,",
        "ShadeGallery,",
        "ExposedHeat",
        "CurrentHeatStress",
        "SaveGame",
        "DESIGN-GAP",
        "EvaluateNextHeatStress",
        "EvaluateStaminaRegenMultiplier",
    ), errors)
    _require_fragments(heat_source, (
        "ERallyDamageClass::Environmental",
        "CurrentHeatStress >= EnvironmentalDamageThreshold",
        "CachedStamina->SetRegenMultiplier(1.0f)",
        "EHeatExposureZone::CoolCistern",
        "EHeatExposureZone::ShadeGallery",
        "EHeatExposureZone::ExposedHeat",
    ), errors)

    cruz_header = root / "Source/DarkArisen/Bosses/IsabelCruzCharacter.h"
    cruz_source = root / "Source/DarkArisen/Bosses/IsabelCruzCharacter.cpp"
    _require_fragments(cruz_header, (
        "Duty,",
        "List,",
        "EighteenFortySix",
        "Killed,",
        "Spared,",
        "Avoided",
        "ResolveAvoided",
        "TrySpare",
        "SaveGame",
    ), errors)
    _require_fragments(cruz_source, (
        "Remaining > ListPhaseHealthFraction",
        "Remaining > EighteenFortySixPhaseHealthFraction",
        "CombatComponent->CurrentState == ECombatState::Staggered",
        "CombatComponent->SetNonHostile()",
        "CameraState->EnterDeathHold()",
        "bFortSurrenderedWithoutCombat",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/Components/CombatComponent.h", (
        "NonHostile,",
        "IsCombatTargetable",
        "SetNonHostile",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/Tests/DesignLawsSpec.cpp", (
        "DarkArisen.M2.MarkerlessQuestFoundation",
        "Exactly six authored activation triggers",
        "Silent activation adds no journal entry",
        "Expiry is an outcome, not a failure state",
        "DarkArisen.M2.HeatRhythm",
        "Exposed heat builds faster than shade",
        "DarkArisen.M2.IsabelCruzState",
        "Sixty-five percent enters List",
        "Spared combatant cannot be locked or hit",
        "DarkArisen.M2.CenoteContract",
        "Bare breath is thirty seconds",
        "Image timing resets outside the sunlight hour",
        "Mandatory Return opens after the floor",
        "DarkArisen.M2.RexaAuthoredMissions",
        "Exactly three authored Turn quests",
        "Exactly one authored Standing variant",
        "Standing variant is finite authored salvage",
        "DarkArisen.M2.RexaSettlementRoster",
        "Las Raices has exactly forty authored residents",
        "Five authored children are protected",
        "Residents have no combat component and cannot be locked on",
    ), errors)

    mission_header = root / "Source/DarkArisen/Missions/RexaM2MissionCatalog.h"
    mission_source = root / "Source/DarkArisen/Missions/RexaM2MissionCatalog.cpp"
    physical_journal_header = root / "Source/DarkArisen/Interaction/PhysicalJournalActor.h"
    physical_journal_source = root / "Source/DarkArisen/Interaction/PhysicalJournalActor.cpp"
    _require_fragments(mission_header, (
        "EQuestStructuralTier",
        "EStandingMissionType",
        "FLocalDirectionVariant",
        "FRexaM2MissionDefinition",
        "GetAuthoredMissions",
        "AppendLocalDirection",
        "AuthoredOutcomeIds",
    ), errors)
    _require_fragments(mission_source, (
        'TEXT("Rexa.Turn.EmptyHammock")',
        'TEXT("Rexa.Turn.ThreeCutsInStone")',
        'TEXT("Rexa.Turn.SaltLedger")',
        'TEXT("Rexa.Standing.Salvage.SanTelmoBell")',
        "Mission.Activation.bRequiresSpokenAgreement = true",
        "Mission.StandingType = EStandingMissionType::Salvage",
        "return TurnCount == 3 && StandingCount == 1",
    ), errors)
    _require_fragments(physical_journal_header, (
        "APhysicalJournalActor",
        "SetSearchQuery",
        "BuildChronologicalPages",
    ), errors)
    _require_fragments(physical_journal_source, (
        '"Jake\'s notebook"',
        "Journal->GetJournalEntries()",
        "Journal->SearchJournal(SearchQuery)",
        'TEXT("Day %lld — %02lld:%02lld\\n")',
    ), errors)
    _require_fragments(root / "Source/DarkArisen/JakeCharacter.cpp", (
        "URexaM2MissionCatalog::RegisterAuthoredMissions(QuestJournalComponent)",
    ), errors)
    _require_fragments(root / "Source/DarkArisen/GreyboxGameMode.cpp", (
        "World->SpawnActor<APhysicalJournalActor>",
    ), errors)

    roster_header = root / "Source/DarkArisen/Rexa/RexaSettlementRoster.h"
    roster_source = root / "Source/DarkArisen/Rexa/RexaSettlementRoster.cpp"
    resident_header = root / "Source/DarkArisen/Rexa/RexaSettlementResident.h"
    resident_source = root / "Source/DarkArisen/Rexa/RexaSettlementResident.cpp"
    director_header = root / "Source/DarkArisen/Rexa/RexaSettlementDirector.h"
    director_source = root / "Source/DarkArisen/Rexa/RexaSettlementDirector.cpp"
    _require_fragments(roster_header, (
        "RequiredResidentCount = 40",
        "RequiredIndigenousCount = 16",
        "RequiredMixedCount = 12",
        "RequiredImperialCount = 8",
        "RequiredSailorTraderCount = 4",
        "GetPurposeAnchorAtGameMinute",
        "KnowledgeIds",
        "bProtectedChild",
        "GetAuthoredResidents",
        "IsRosterValid",
    ), errors)
    _require_fragments(roster_source, (
        "Indigenous Rexan — sixteen residents (40%)",
        "Mixed Rexan — twelve residents (30%)",
        "Imperial colonists — eight residents (20%)",
        "Sailors and traders — four residents (10%)",
        "ChildCount == 5",
        'TEXT("Raices.RiverChild")',
        'TEXT("Raices.DockWorker")',
        'TEXT("Raices.WreckDiver")',
    ), errors)
    if roster_source.is_file():
        roster_text = roster_source.read_text(encoding="utf-8")
        resident_count = roster_text.count("Residents.Add(Resident(")
        if resident_count != 40:
            errors.append(
                f"RexaSettlementRoster.cpp must author exactly 40 residents; found {resident_count}")
        expected_census = {
            "Heritage::IndigenousRexan": 16,
            "Heritage::MixedRexan": 12,
            "Heritage::ImperialColonist": 8,
            "Heritage::SailorTrader": 4,
            "Age::Child": 5,
        }
        for token, expected_count in expected_census.items():
            actual_count = roster_text.count(token)
            if actual_count != expected_count:
                errors.append(
                    f"RexaSettlementRoster.cpp census requires {expected_count} {token}; "
                    f"found {actual_count}")
    _require_fragments(resident_header, (
        "ARexaSettlementResident",
        "LaunchCharacter",
        "TakeDamage",
        "InitializeFromDefinition",
        "RefreshPurposeAnchor",
        "IsProtectedChildRuntime",
    ), errors)
    _require_fragments(resident_source, (
        'Tags.AddUnique(TEXT("Rexa.NonCombatant"))',
        'Tags.AddUnique(TEXT("Rexa.ProtectedChild"))',
        "if (!bDefinitionInitialized || ResidentDefinition.bProtectedChild) return",
        "if (!bDefinitionInitialized || ResidentDefinition.bProtectedChild) return 0.0f",
        "SetCanBeDamaged(!ResidentDefinition.bProtectedChild)",
        "FindComponentByClass<UHealthComponent>() == nullptr",
        "FindComponentByClass<UCombatComponent>() == nullptr",
        "Movement->bEnablePhysicsInteraction = false",
        "GreyboxBody->SetCollisionEnabled(ECollisionEnabled::NoCollision)",
    ), errors)
    _require_fragments(director_header, (
        "ARexaSettlementDirector",
        "SpawnAuthoredSettlement",
        "ClearSpawnedSettlement",
        "GetSpawnedResidentCount",
    ), errors)
    _require_fragments(director_source, (
        "URexaSettlementRoster::GetAuthoredResidents()",
        "URexaSettlementRoster::IsRosterValid(Residents)",
        "World->SpawnActor<ARexaSettlementResident>",
        "Spawned->InitializeFromDefinition(Definition)",
        "ClearSpawnedSettlement()",
        "URexaSettlementRoster::RequiredResidentCount",
    ), errors)
    settlement_text = "\n".join(
        path.read_text(encoding="utf-8")
        for path in (
            roster_header,
            roster_source,
            resident_header,
            resident_source,
            director_header,
            director_source,
        )
        if path.is_file()
    )
    for forbidden in (
        "FMath::Rand",
        "FRandomStream",
        "GenerateResident",
        "CreateDefaultSubobject<UHealthComponent>",
        "CreateDefaultSubobject<UCombatComponent>",
        "SetSimulatePhysics(true)",
    ):
        if forbidden in settlement_text:
            errors.append(f"authored Rexa settlement source forbids: {forbidden}")

    quest_text = "\n".join(
        path.read_text(encoding="utf-8")
        for path in (header, source)
        if path.is_file()
    )
    for forbidden in (
        "OnQuestActivated",
        "OnJournalEntryAdded",
        "Quest Added",
        "Quest Failed",
        "AddQuestMarker",
        "GenerateRadiantQuest",
        "SetViewTarget",
        "PlaySound",
        "JournalEntries.Sort",
    ):
        if forbidden in quest_text:
            errors.append(f"markerless quest foundation forbids: {forbidden}")

    authored_mission_text = "\n".join(
        path.read_text(encoding="utf-8")
        for path in (
            mission_header,
            mission_source,
            physical_journal_header,
            physical_journal_source,
        )
        if path.is_file()
    )
    for forbidden in (
        "GenerateRadiantQuest",
        "AddQuestMarker",
        "Quest Added",
        "Quest Failed",
        "ObjectiveCheckbox",
        "CompletionPercentage",
        "OnMissionActivated",
    ):
        if forbidden in authored_mission_text:
            errors.append(f"authored Rexa mission content forbids: {forbidden}")

    mechanics_text = "\n".join(
        path.read_text(encoding="utf-8")
        for path in (heat_header, heat_source, cruz_header, cruz_source)
        if path.is_file()
    )
    for forbidden in (
        "HeatWidget",
        "OnHeatChanged",
        "SetLifeSpan",
        "SetViewTarget",
        "PlaySound",
        "SlowMotion",
        "KillCam",
    ):
        if forbidden in mechanics_text:
            errors.append(f"M2 heat/boss source contract forbids: {forbidden}")

    breath_header = root / "Source/DarkArisen/Components/WaterBreathComponent.h"
    breath_source = root / "Source/DarkArisen/Components/WaterBreathComponent.cpp"
    cenote_header = root / "Source/DarkArisen/Dungeons/CenoteFirstMotherComponent.h"
    cenote_source = root / "Source/DarkArisen/Dungeons/CenoteFirstMotherComponent.cpp"
    _require_fragments(breath_header, (
        "Bare,",
        "Trained,",
        "Master,",
        "Equipment",
        "exposes no meter or warning event",
        "DrowningDamagePerSecond",
        "DESIGN-GAP",
    ), errors)
    _require_fragments(breath_source, (
        "case EWaterBreathTier::Bare: return 30.0f",
        "case EWaterBreathTier::Trained: return 60.0f",
        "case EWaterBreathTier::Master: return 90.0f",
        "ERallyDamageClass::Environmental",
    ), errors)
    _require_fragments(cenote_header, (
        "MarkEnteredAtSinkholeLip",
        "MarkWaterRoutingSolved",
        "SetGreenGoldSunlightWindowActive",
        "ResolveKeeperBelow",
        "OpenMandatoryReturnShortcut",
        "IsDungeonComplete",
        "SaveGame",
        "DESIGN-GAP",
    ), errors)
    _require_fragments(cenote_source, (
        "bPlayerInImageChamber",
        "bGreenGoldSunlightWindowActive",
        "bKeeperEncounterResolved && bReturnShortcutOpened",
        "if (!bKeeperEncounterResolved || bReturnShortcutOpened) return false",
    ), errors)
    cenote_text = "\n".join(
        path.read_text(encoding="utf-8")
        for path in (breath_header, breath_source, cenote_header, cenote_source)
        if path.is_file()
    )
    for forbidden in (
        "QuestMarker",
        "MapMarker",
        "AudioLog",
        "OnBreathChanged",
        "OnDungeonCompleted",
        "PlaySound",
        "MusicCue",
    ):
        if forbidden in cenote_text:
            errors.append(f"M2 Cenote source contract forbids: {forbidden}")
    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[2])
    args = parser.parse_args()
    errors = validate(args.root.resolve())
    if errors:
        print("M2 quest/environment/encounter validation failed:", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1
    print("M2 quest, environment and encounter source validation passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
