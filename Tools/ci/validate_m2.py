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
    ), errors)

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
