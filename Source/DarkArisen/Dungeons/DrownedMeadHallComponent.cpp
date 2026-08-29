// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Dungeons/DrownedMeadHallComponent.h"

#include "Dungeons/AuthoredDungeonSiteComponent.h"
#include "World/DarkArisenWorldRulesSubsystem.h"

namespace
{
int32 NormalizeWorldMinute(const int32 Minute)
{
    int32 Result = Minute % UDrownedMeadHallComponent::WorldMinutesPerDay;
    if (Result < 0)
    {
        Result += UDrownedMeadHallComponent::WorldMinutesPerDay;
    }
    return Result;
}

bool IsMinuteInsideWindow(const int32 Minute, const int32 Start, const int32 Duration)
{
    const int32 Delta = NormalizeWorldMinute(Minute - Start);
    return Delta >= 0 && Delta < Duration;
}
}

UDrownedMeadHallComponent::UDrownedMeadHallComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDrownedMeadHallComponent::BeginPlay()
{
    Super::BeginPlay();
    BindSiteAuthority();
}

bool UDrownedMeadHallComponent::BindSiteAuthority()
{
    SiteAuthority = GetOwner() ? GetOwner()->FindComponentByClass<UAuthoredDungeonSiteComponent>() : nullptr;
    if (!SiteAuthority)
    {
        UE_LOG(LogTemp, Error, TEXT("Drowned Mead Hall component requires UAuthoredDungeonSiteComponent on the same actor."));
        return false;
    }

    if (SiteAuthority->DungeonStableId != FName(TEXT("dungeon.fjordlund.drowned-mead-hall")))
    {
        UE_LOG(LogTemp, Error, TEXT("Drowned Mead Hall component bound to wrong dungeon stable ID: %s"), *SiteAuthority->DungeonStableId.ToString());
        SiteAuthority = nullptr;
        return false;
    }

    return SiteAuthority->IsDefinitionValid() || SiteAuthority->InitializeDefinition();
}

bool UDrownedMeadHallComponent::IsDryWindowMinute(
    const int32 MinuteOfDay,
    const int32 FirstWindowStartMinute)
{
    if (FirstWindowStartMinute < 0 || FirstWindowStartMinute >= WorldMinutesPerDay)
    {
        return false;
    }

    const int32 Minute = NormalizeWorldMinute(MinuteOfDay);
    const int32 FirstStart = NormalizeWorldMinute(FirstWindowStartMinute);
    const int32 SecondStart = NormalizeWorldMinute(FirstStart + DryWindowSeparationWorldMinutes);

    return IsMinuteInsideWindow(Minute, FirstStart, DryWindowDurationWorldMinutes)
        || IsMinuteInsideWindow(Minute, SecondStart, DryWindowDurationWorldMinutes);
}

bool UDrownedMeadHallComponent::IsDryWindowActive() const
{
    if (AuthoredFirstDryWindowStartMinute < 0)
    {
        return false;
    }

    const UWorld* World = GetWorld();
    const UDarkArisenWorldRulesSubsystem* Rules = World ? World->GetSubsystem<UDarkArisenWorldRulesSubsystem>() : nullptr;
    return Rules && IsDryWindowMinute(Rules->GetMinuteOfDay(), AuthoredFirstDryWindowStartMinute);
}

bool UDrownedMeadHallComponent::MarkEnteredAtLowTide()
{
    if (!SiteAuthority && !BindSiteAuthority())
    {
        return false;
    }
    if (!IsDryWindowActive())
    {
        return false;
    }
    return SiteAuthority->MarkEntered();
}

bool UDrownedMeadHallComponent::MarkDrainedHallImageWitnessed()
{
    if (!SiteAuthority && !BindSiteAuthority())
    {
        return false;
    }
    if (!IsDryWindowActive())
    {
        return false;
    }
    return SiteAuthority->MarkUnforgettableImageWitnessed();
}

bool UDrownedMeadHallComponent::ResolveLastGuest(const FName OutcomeId)
{
    if (!SiteAuthority && !BindSiteAuthority())
    {
        return false;
    }
    return SiteAuthority->MarkBottomResolved(OutcomeId);
}

bool UDrownedMeadHallComponent::OpenMandatoryReturnShortcut()
{
    if (!SiteAuthority && !BindSiteAuthority())
    {
        return false;
    }
    return SiteAuthority->OpenReturnShortcutFromInside();
}

bool UDrownedMeadHallComponent::RecoverDrinkingHorn()
{
    if (!SiteAuthority && !BindSiteAuthority())
    {
        return false;
    }
    if (!SiteAuthority->IsStructurallyComplete() || bDrinkingHornRecovered)
    {
        return false;
    }

    bDrinkingHornRecovered = true;
    return true;
}

bool UDrownedMeadHallComponent::IsDungeonStructurallyComplete() const
{
    return SiteAuthority && SiteAuthority->IsStructurallyComplete();
}
