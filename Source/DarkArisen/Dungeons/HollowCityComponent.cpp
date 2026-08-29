// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Dungeons/HollowCityComponent.h"

#include "Dungeons/AuthoredDungeonSiteComponent.h"

UHollowCityComponent::UHollowCityComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHollowCityComponent::BeginPlay()
{
    Super::BeginPlay();
    BindSiteAuthority();
}

bool UHollowCityComponent::BindSiteAuthority()
{
    SiteAuthority = GetOwner() ? GetOwner()->FindComponentByClass<UAuthoredDungeonSiteComponent>() : nullptr;
    if (!SiteAuthority)
    {
        UE_LOG(LogTemp, Error, TEXT("Hollow City component requires UAuthoredDungeonSiteComponent on the same actor."));
        return false;
    }

    if (SiteAuthority->DungeonStableId != FName(TEXT("dungeon.rexa.hollow-city")))
    {
        UE_LOG(LogTemp, Error, TEXT("Hollow City component bound to wrong dungeon stable ID: %s"), *SiteAuthority->DungeonStableId.ToString());
        SiteAuthority = nullptr;
        return false;
    }

    return SiteAuthority->IsDefinitionValid() || SiteAuthority->InitializeDefinition();
}

bool UHollowCityComponent::MarkEnteredFromCulturalWebRoute()
{
    if (!SiteAuthority && !BindSiteAuthority())
    {
        return false;
    }
    return SiteAuthority->MarkEntered();
}

bool UHollowCityComponent::MarkSubAreaEntered(const int32 AuthoredIndex)
{
    if ((!SiteAuthority && !BindSiteAuthority())
        || !SiteAuthority->bEntered
        || AuthoredIndex < 0
        || AuthoredIndex >= RequiredSubAreaCount)
    {
        return false;
    }

    const uint8 Bit = static_cast<uint8>(1u << AuthoredIndex);
    if ((VisitedSubAreaMask & Bit) != 0)
    {
        return false;
    }

    VisitedSubAreaMask |= Bit;
    return true;
}

bool UHollowCityComponent::HasAllSubAreaBits(const uint8 Mask)
{
    return (Mask & RequiredSubAreaMask) == RequiredSubAreaMask;
}

bool UHollowCityComponent::HasVisitedAllFourSubAreas() const
{
    return HasAllSubAreaBits(VisitedSubAreaMask);
}

bool UHollowCityComponent::MarkCanopyHiddenCityImageWitnessed()
{
    if ((!SiteAuthority && !BindSiteAuthority()) || !HasVisitedAllFourSubAreas())
    {
        return false;
    }
    return SiteAuthority->MarkUnforgettableImageWitnessed();
}

void UHollowCityComponent::TryResolveBottomAuthority()
{
    if (SiteAuthority
        && SiteAuthority->bEntered
        && bWatcherAtGateResolved
        && bThingInReservoirResolved
        && !SiteAuthority->bBottomResolved)
    {
        SiteAuthority->MarkBottomResolved(FName(TEXT("hollow-city.two-bosses-resolved")));
    }
}

bool UHollowCityComponent::ResolveWatcherAtGate()
{
    if ((!SiteAuthority && !BindSiteAuthority()) || !SiteAuthority->bEntered || bWatcherAtGateResolved)
    {
        return false;
    }

    bWatcherAtGateResolved = true;
    TryResolveBottomAuthority();
    return true;
}

bool UHollowCityComponent::ResolveThingInReservoir()
{
    if ((!SiteAuthority && !BindSiteAuthority()) || !SiteAuthority->bEntered || bThingInReservoirResolved)
    {
        return false;
    }

    bThingInReservoirResolved = true;
    TryResolveBottomAuthority();
    return true;
}

bool UHollowCityComponent::OpenInterlockingShortcut(const FName ShortcutId)
{
    if ((!SiteAuthority && !BindSiteAuthority())
        || !SiteAuthority->bEntered
        || ShortcutId.IsNone()
        || OpenedInterlockingShortcutIds.Contains(ShortcutId))
    {
        return false;
    }

    OpenedInterlockingShortcutIds.Add(ShortcutId);
    return true;
}

bool UHollowCityComponent::OpenMandatoryReturnShortcut()
{
    if (!SiteAuthority && !BindSiteAuthority())
    {
        return false;
    }
    return SiteAuthority->OpenReturnShortcutFromInside();
}

bool UHollowCityComponent::RecoverAncientTierEMap()
{
    if ((!SiteAuthority && !BindSiteAuthority())
        || !SiteAuthority->IsStructurallyComplete()
        || bAncientTierEMapRecovered)
    {
        return false;
    }

    bAncientTierEMapRecovered = true;
    return true;
}

bool UHollowCityComponent::IsDungeonStructurallyComplete() const
{
    return SiteAuthority
        && HasVisitedAllFourSubAreas()
        && bWatcherAtGateResolved
        && bThingInReservoirResolved
        && SiteAuthority->IsStructurallyComplete();
}

bool UHollowCityComponent::IsContentChainComplete() const
{
    return IsDungeonStructurallyComplete() && bAncientTierEMapRecovered;
}
