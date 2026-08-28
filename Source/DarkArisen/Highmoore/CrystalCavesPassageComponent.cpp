// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Highmoore/CrystalCavesPassageComponent.h"

#include "GameFramework/Actor.h"

UCrystalCavesPassageComponent::UCrystalCavesPassageComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCrystalCavesPassageComponent::BeginPlay()
{
    Super::BeginPlay();
    if (const AActor* Owner = GetOwner())
    {
        LastObservedLocation = Owner->GetActorLocation();
        bHasObservedLocation = true;
    }
}

void UCrystalCavesPassageComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (DeltaTime <= 0.0f || (State != ECrystalCavesPassageState::FirstPassage
        && State != ECrystalCavesPassageState::RepeatPassage))
    {
        return;
    }

    ElapsedPassageSeconds += DeltaTime;

    const AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    const FVector CurrentLocation = Owner->GetActorLocation();
    if (!bHasObservedLocation)
    {
        LastObservedLocation = CurrentLocation;
        bHasObservedLocation = true;
        return;
    }

    const float DeltaCentimetres = FVector::Distance(CurrentLocation, LastObservedLocation);
    if (FMath::IsFinite(DeltaCentimetres) && DeltaCentimetres > 0.0f)
    {
        ObservedDistanceMetres += DeltaCentimetres / 100.0f;
    }
    LastObservedLocation = CurrentLocation;
}

bool UCrystalCavesPassageComponent::BeginFirstPassage(const float AuthoredMinimumDistanceMetres)
{
    if (bHighmoorePermanentlyOpen
        || State != ECrystalCavesPassageState::Outside
        || !FMath::IsFinite(AuthoredMinimumDistanceMetres)
        || AuthoredMinimumDistanceMetres <= 0.0f)
    {
        return false;
    }

    State = ECrystalCavesPassageState::FirstPassage;
    RequiredRealSeconds = FirstPassageMinimumRealSeconds;
    RequiredDistanceMetres = AuthoredMinimumDistanceMetres;
    ResetObservedTraversal();
    return true;
}

bool UCrystalCavesPassageComponent::CompleteFirstPassage()
{
    if (State != ECrystalCavesPassageState::FirstPassage || !CanCompleteActivePassage())
    {
        return false;
    }

    bHighmoorePermanentlyOpen = true;
    State = ECrystalCavesPassageState::HighmooreOpen;
    ResetObservedTraversal();
    return true;
}

bool UCrystalCavesPassageComponent::BeginRepeatPassage(
    const float AuthoredMinimumDistanceMetres,
    const float AuthoredRepeatRealSeconds)
{
    if (!bHighmoorePermanentlyOpen
        || State != ECrystalCavesPassageState::HighmooreOpen
        || !FMath::IsFinite(AuthoredMinimumDistanceMetres)
        || AuthoredMinimumDistanceMetres <= 0.0f
        || !FMath::IsFinite(AuthoredRepeatRealSeconds)
        || AuthoredRepeatRealSeconds < RepeatPassageMinimumRealSeconds
        || AuthoredRepeatRealSeconds > RepeatPassageMaximumAuthoredRealSeconds)
    {
        return false;
    }

    State = ECrystalCavesPassageState::RepeatPassage;
    RequiredRealSeconds = AuthoredRepeatRealSeconds;
    RequiredDistanceMetres = AuthoredMinimumDistanceMetres;
    ResetObservedTraversal();
    return true;
}

bool UCrystalCavesPassageComponent::CompleteRepeatPassage()
{
    if (State != ECrystalCavesPassageState::RepeatPassage || !CanCompleteActivePassage())
    {
        return false;
    }

    State = ECrystalCavesPassageState::HighmooreOpen;
    ResetObservedTraversal();
    return true;
}

bool UCrystalCavesPassageComponent::CanCompleteActivePassage() const
{
    return ElapsedPassageSeconds >= RequiredRealSeconds
        && ObservedDistanceMetres >= RequiredDistanceMetres;
}

void UCrystalCavesPassageComponent::ResetObservedTraversal()
{
    ElapsedPassageSeconds = 0.0f;
    ObservedDistanceMetres = 0.0f;
    if (const AActor* Owner = GetOwner())
    {
        LastObservedLocation = Owner->GetActorLocation();
        bHasObservedLocation = true;
    }
    else
    {
        bHasObservedLocation = false;
    }
}
