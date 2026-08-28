// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Highmoore/HighmooreHorseComponent.h"

UHighmooreHorseComponent::UHighmooreHorseComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UHighmooreHorseComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bAlive || DeltaTime <= 0.0f)
    {
        return;
    }

    if (ActiveRide != EAuthoredRideSequence::None)
    {
        RideElapsedSeconds += DeltaTime;
    }

    if (bStaminaSuspendedForAuthoredRide)
    {
        return;
    }

    float DrainPerSecond = 0.0f;
    switch (Gait)
    {
    case EHorseGait::Gallop:
        DrainPerSecond = 2.0f;
        break;
    case EHorseGait::Canter:
        DrainPerSecond = 0.5f;
        break;
    case EHorseGait::Trot:
        DrainPerSecond = 0.1f;
        break;
    case EHorseGait::Walk:
    default:
        DrainPerSecond = 0.0f;
        break;
    }

    if (DrainPerSecond > 0.0f)
    {
        InternalStamina = FMath::Max(0.0f, InternalStamina - DrainPerSecond * DeltaTime);
    }
    else
    {
        InternalStamina = FMath::Min(180.0f, InternalStamina + 1.0f * DeltaTime);
    }
}

bool UHighmooreHorseComponent::SetBondLevel(const EHorseBondLevel NewBond)
{
    if (!bAlive || static_cast<uint8>(NewBond) < static_cast<uint8>(BondLevel))
    {
        return false;
    }
    BondLevel = NewBond;
    return true;
}

bool UHighmooreHorseComponent::SetGait(const EHorseGait NewGait)
{
    if (!bAlive)
    {
        return false;
    }

    if (ActiveRide == EAuthoredRideSequence::Lake
        && static_cast<uint8>(NewGait) > static_cast<uint8>(EHorseGait::Trot))
    {
        return false;
    }
    if ((ActiveRide == EAuthoredRideSequence::Arion || ActiveRide == EAuthoredRideSequence::Belos)
        && NewGait != EHorseGait::Gallop)
    {
        return false;
    }
    if (ActiveRide == EAuthoredRideSequence::ReturnWalk && NewGait != EHorseGait::Walk)
    {
        return false;
    }

    if (InternalStamina <= 0.0f && NewGait != EHorseGait::Walk && !bStaminaSuspendedForAuthoredRide)
    {
        return false;
    }

    Gait = NewGait;
    return true;
}

EHorseMoodRead UHighmooreHorseComponent::GetMoodRead() const
{
    if (!bAlive || bLastingRevengeRideDamage)
    {
        return EHorseMoodRead::Blown;
    }
    if (InternalStamina <= 20.0f)
    {
        return EHorseMoodRead::Tired;
    }
    if (BondLevel == EHorseBondLevel::Unknown)
    {
        return EHorseMoodRead::Frightened;
    }
    return BondLevel == EHorseBondLevel::Bonded ? EHorseMoodRead::Content : EHorseMoodRead::Calm;
}

bool UHighmooreHorseComponent::BeginLakeRide()
{
    return BeginRide(EAuthoredRideSequence::Lake, EHorseGait::Trot, LakeRideRequiredSeconds, false);
}

bool UHighmooreHorseComponent::BeginArionRide()
{
    return BeginRide(EAuthoredRideSequence::Arion, EHorseGait::Gallop, ArionRideRequiredSeconds, true);
}

bool UHighmooreHorseComponent::ContinueArionRideToBelos()
{
    if (!bAlive || ActiveRide != EAuthoredRideSequence::None || !bStaminaSuspendedForAuthoredRide)
    {
        return false;
    }

    ActiveRide = EAuthoredRideSequence::Belos;
    Gait = EHorseGait::Gallop;
    RideElapsedSeconds = 0.0f;
    ActiveRideRequiredSeconds = BelosRideRequiredSeconds;
    return true;
}

bool UHighmooreHorseComponent::BeginReturnWalk()
{
    if (!bAlive || ActiveRide != EAuthoredRideSequence::None)
    {
        return false;
    }

    bStaminaSuspendedForAuthoredRide = false;
    bLastingRevengeRideDamage = true;
    InternalStamina = FMath::Min(InternalStamina, 10.0f);
    return BeginRide(EAuthoredRideSequence::ReturnWalk, EHorseGait::Walk, ReturnWalkRequiredSeconds, false);
}

bool UHighmooreHorseComponent::CompleteAuthoredRide()
{
    if (ActiveRide == EAuthoredRideSequence::None || RideElapsedSeconds < GetRequiredRideSeconds())
    {
        return false;
    }

    const EAuthoredRideSequence CompletedRide = ActiveRide;
    ActiveRide = EAuthoredRideSequence::None;
    ActiveRideRequiredSeconds = 0.0f;
    RideElapsedSeconds = 0.0f;

    // Stamina suspension deliberately persists from Arion through the eastward Belos ride.
    if (CompletedRide == EAuthoredRideSequence::Belos)
    {
        bLastingRevengeRideDamage = true;
        InternalStamina = FMath::Min(InternalStamina, 10.0f);
        bStaminaSuspendedForAuthoredRide = false;
        Gait = EHorseGait::Walk;
    }
    else if (CompletedRide == EAuthoredRideSequence::Lake || CompletedRide == EAuthoredRideSequence::ReturnWalk)
    {
        bStaminaSuspendedForAuthoredRide = false;
    }

    return true;
}

bool UHighmooreHorseComponent::RecordPermanentDeath()
{
    if (!bAlive)
    {
        return false;
    }

    bAlive = false;
    ActiveRide = EAuthoredRideSequence::None;
    bStaminaSuspendedForAuthoredRide = false;
    InternalStamina = 0.0f;
    Gait = EHorseGait::Walk;
    return true;
}

bool UHighmooreHorseComponent::BeginRide(
    const EAuthoredRideSequence Ride,
    const EHorseGait RequiredGait,
    const float RequiredSeconds,
    const bool bSuspendStamina)
{
    if (!bAlive
        || ActiveRide != EAuthoredRideSequence::None
        || Ride == EAuthoredRideSequence::None
        || RequiredSeconds <= 0.0f)
    {
        return false;
    }

    ActiveRide = Ride;
    Gait = RequiredGait;
    RideElapsedSeconds = 0.0f;
    ActiveRideRequiredSeconds = RequiredSeconds;
    bStaminaSuspendedForAuthoredRide = bSuspendStamina;
    return true;
}

float UHighmooreHorseComponent::GetRequiredRideSeconds() const
{
    return ActiveRideRequiredSeconds;
}
