// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Highmoore/CrystalGuardianComponent.h"

UCrystalGuardianComponent::UCrystalGuardianComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCrystalGuardianComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (Phase != ECrystalGuardianPhase::Settling || DeltaTime <= 0.0f)
    {
        return;
    }

    SettleElapsedSeconds += DeltaTime;
    if (SettleElapsedSeconds >= DefeatHoldSeconds)
    {
        Phase = ECrystalGuardianPhase::Defeated;
        bHasActiveLightStimulus = false;
    }
}

bool UCrystalGuardianComponent::EngageGuardian()
{
    if (Phase != ECrystalGuardianPhase::Dormant)
    {
        return false;
    }

    Phase = ECrystalGuardianPhase::Threshold;
    return true;
}

bool UCrystalGuardianComponent::RecordLightStimulus(const FVector WorldLocation, const float Intensity)
{
    if (Phase == ECrystalGuardianPhase::Dormant
        || Phase == ECrystalGuardianPhase::Defeated
        || Phase == ECrystalGuardianPhase::Bypassed
        || Phase == ECrystalGuardianPhase::Settling
        || !WorldLocation.IsFinite()
        || !FMath::IsFinite(Intensity)
        || Intensity <= 0.0f)
    {
        return false;
    }

    LastLightStimulusLocation = WorldLocation;
    bHasActiveLightStimulus = true;
    return true;
}

bool UCrystalGuardianComponent::ApplyGuardianHealthPercent(const float RemainingPercent)
{
    if (Phase == ECrystalGuardianPhase::Dormant
        || Phase == ECrystalGuardianPhase::Defeated
        || Phase == ECrystalGuardianPhase::Bypassed
        || Phase == ECrystalGuardianPhase::Settling
        || !FMath::IsFinite(RemainingPercent)
        || RemainingPercent <= 0.0f
        || RemainingPercent > HealthPercent)
    {
        return false;
    }

    HealthPercent = FMath::Clamp(RemainingPercent, 0.0f, 100.0f);
    Phase = ResolveCombatPhase(HealthPercent);
    return true;
}

bool UCrystalGuardianComponent::RecordGuardianBypassed()
{
    if (Phase != ECrystalGuardianPhase::Dormant)
    {
        return false;
    }

    Phase = ECrystalGuardianPhase::Bypassed;
    return true;
}

bool UCrystalGuardianComponent::BeginDefeatSettle()
{
    if (Phase == ECrystalGuardianPhase::Dormant
        || Phase == ECrystalGuardianPhase::Defeated
        || Phase == ECrystalGuardianPhase::Bypassed
        || Phase == ECrystalGuardianPhase::Settling)
    {
        return false;
    }

    HealthPercent = 0.0f;
    Phase = ECrystalGuardianPhase::Settling;
    SettleElapsedSeconds = 0.0f;
    bHasActiveLightStimulus = false;
    return true;
}

ECrystalGuardianPhase UCrystalGuardianComponent::ResolveCombatPhase(const float RemainingPercent)
{
    if (RemainingPercent > 75.0f)
    {
        return ECrystalGuardianPhase::Threshold;
    }
    if (RemainingPercent > 45.0f)
    {
        return ECrystalGuardianPhase::Columns;
    }
    if (RemainingPercent > 20.0f)
    {
        return ECrystalGuardianPhase::Span;
    }
    return ECrystalGuardianPhase::LastStand;
}
