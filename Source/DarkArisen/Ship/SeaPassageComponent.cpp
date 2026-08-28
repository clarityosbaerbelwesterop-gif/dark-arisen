// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Ship/SeaPassageComponent.h"

#include "Engine/World.h"
#include "World/DarkArisenWorldRulesSubsystem.h"

USeaPassageComponent::USeaPassageComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void USeaPassageComponent::BeginPlay()
{
    Super::BeginPlay();
    if (const AActor* Owner = GetOwner())
    {
        LastObservedLocation = Owner->GetActorLocation();
        bHasObservedLocation = true;
    }
}

void USeaPassageComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    const AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    const FVector Current = Owner->GetActorLocation();
    if (!bHasObservedLocation)
    {
        LastObservedLocation = Current;
        bHasObservedLocation = true;
        return;
    }

    if (bPassageActive)
    {
        const float DeltaCentimetres = FVector::Distance(Current, LastObservedLocation);
        if (FMath::IsFinite(DeltaCentimetres) && DeltaCentimetres > 0.0f)
        {
            SailedDistanceMetres += DeltaCentimetres / 100.0f;
        }
    }

    LastObservedLocation = Current;
}

bool USeaPassageComponent::BeginSeaPassage(
    const FName RouteId,
    const int32 MinimumGameMinutes,
    const float MinimumDistanceMetres)
{
    if (bPassageActive
        || RouteId.IsNone()
        || MinimumGameMinutes <= 0
        || !FMath::IsFinite(MinimumDistanceMetres)
        || MinimumDistanceMetres <= 0.0f)
    {
        return false;
    }

    ActiveRouteId = RouteId;
    PassageStartWorldMinute = GetCurrentWorldMinute();
    RequiredGameMinutes = MinimumGameMinutes;
    RequiredDistanceMetres = MinimumDistanceMetres;
    SailedDistanceMetres = 0.0f;
    bPassageActive = true;

    if (const AActor* Owner = GetOwner())
    {
        LastObservedLocation = Owner->GetActorLocation();
        bHasObservedLocation = true;
    }
    return true;
}

bool USeaPassageComponent::CanCompleteSeaPassage() const
{
    if (!bPassageActive)
    {
        return false;
    }

    const int64 ElapsedMinutes = GetCurrentWorldMinute() - PassageStartWorldMinute;
    return ElapsedMinutes >= RequiredGameMinutes
        && SailedDistanceMetres >= RequiredDistanceMetres;
}

bool USeaPassageComponent::CompleteSeaPassage()
{
    if (!CanCompleteSeaPassage())
    {
        return false;
    }

    bPassageActive = false;
    ActiveRouteId = NAME_None;
    RequiredGameMinutes = 0;
    RequiredDistanceMetres = 0.0f;
    SailedDistanceMetres = 0.0f;
    return true;
}

int64 USeaPassageComponent::GetCurrentWorldMinute() const
{
    const UWorld* World = GetWorld();
    const UDarkArisenWorldRulesSubsystem* Rules =
        World ? World->GetSubsystem<UDarkArisenWorldRulesSubsystem>() : nullptr;
    return Rules ? Rules->GetTotalWorldMinutes() : 0;
}
