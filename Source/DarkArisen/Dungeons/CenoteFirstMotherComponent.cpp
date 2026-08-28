// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Dungeons/CenoteFirstMotherComponent.h"

UCenoteFirstMotherComponent::UCenoteFirstMotherComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UCenoteFirstMotherComponent::BeginPlay()
{
    Super::BeginPlay();
    RefreshTickState();
}

void UCenoteFirstMotherComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    GreenGoldWitnessSeconds = EvaluateGreenGoldWitnessSeconds(
        GreenGoldWitnessSeconds,
        DeltaTime,
        bPlayerInImageChamber,
        bGreenGoldSunlightWindowActive,
        RequiredImageWitnessSeconds);
    if (GreenGoldWitnessSeconds >= FMath::Max(0.1f, RequiredImageWitnessSeconds))
    {
        bGreenGoldImageWitnessed = true;
        RefreshTickState();
    }
}

bool UCenoteFirstMotherComponent::MarkEnteredAtSinkholeLip()
{
    if (bEntered) return false;
    bEntered = true;
    RefreshTickState();
    return true;
}

bool UCenoteFirstMotherComponent::MarkWaterRoutingSolved()
{
    if (!bEntered || bWaterRoutingSolved) return false;
    bWaterRoutingSolved = true;
    return true;
}

void UCenoteFirstMotherComponent::SetPlayerInImageChamber(const bool bInChamber)
{
    bPlayerInImageChamber = bInChamber;
    if (!bPlayerInImageChamber) GreenGoldWitnessSeconds = 0.0f;
    RefreshTickState();
}

void UCenoteFirstMotherComponent::SetGreenGoldSunlightWindowActive(const bool bActive)
{
    bGreenGoldSunlightWindowActive = bActive;
    if (!bGreenGoldSunlightWindowActive) GreenGoldWitnessSeconds = 0.0f;
    RefreshTickState();
}

bool UCenoteFirstMotherComponent::ResolveKeeperBelow(const FName OutcomeId)
{
    if (!bEntered || bKeeperEncounterResolved || OutcomeId.IsNone()) return false;
    bKeeperEncounterResolved = true;
    KeeperOutcomeId = OutcomeId;
    return true;
}

bool UCenoteFirstMotherComponent::OpenMandatoryReturnShortcut()
{
    if (!bKeeperEncounterResolved || bReturnShortcutOpened) return false;
    bReturnShortcutOpened = true;
    return true;
}

bool UCenoteFirstMotherComponent::IsDungeonComplete() const
{
    return bEntered && bWaterRoutingSolved && bGreenGoldImageWitnessed &&
        bKeeperEncounterResolved && bReturnShortcutOpened;
}

float UCenoteFirstMotherComponent::EvaluateGreenGoldWitnessSeconds(
    const float CurrentSeconds,
    const float DeltaSeconds,
    const bool bInImageChamber,
    const bool bSunlightWindowActive,
    const float RequiredSeconds)
{
    if (!bInImageChamber || !bSunlightWindowActive) return 0.0f;
    return FMath::Clamp(
        CurrentSeconds + FMath::Max(0.0f, DeltaSeconds),
        0.0f,
        FMath::Max(0.1f, RequiredSeconds));
}

void UCenoteFirstMotherComponent::RefreshTickState()
{
    SetComponentTickEnabled(
        bEntered && !bGreenGoldImageWitnessed && bPlayerInImageChamber &&
        bGreenGoldSunlightWindowActive);
}
