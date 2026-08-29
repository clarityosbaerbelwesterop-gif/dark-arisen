// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Dungeons/LongBarrowBraeheadComponent.h"

#include "Dungeons/AuthoredDungeonSiteComponent.h"
#include "GameFramework/Actor.h"

ULongBarrowBraeheadComponent::ULongBarrowBraeheadComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void ULongBarrowBraeheadComponent::BeginPlay()
{
    Super::BeginPlay();
    BindSiteAuthority();
}

bool ULongBarrowBraeheadComponent::BindSiteAuthority()
{
    SiteAuthority = GetOwner() ? GetOwner()->FindComponentByClass<UAuthoredDungeonSiteComponent>() : nullptr;
    if (!SiteAuthority)
    {
        UE_LOG(LogTemp, Error, TEXT("Long Barrow component requires UAuthoredDungeonSiteComponent on the same actor."));
        return false;
    }

    if (SiteAuthority->DungeonStableId != FName(TEXT("dungeon.highmoore.long-barrow-braehead")))
    {
        UE_LOG(LogTemp, Error, TEXT("Long Barrow component bound to wrong stable ID: %s"), *SiteAuthority->DungeonStableId.ToString());
        SiteAuthority = nullptr;
        return false;
    }

    return SiteAuthority->IsDefinitionValid() || SiteAuthority->InitializeDefinition();
}

bool ULongBarrowBraeheadComponent::MarkEnteredWithBraePermission(const bool bHouseBraeAccessGranted)
{
    if ((!SiteAuthority && !BindSiteAuthority()) || !bHouseBraeAccessGranted)
    {
        return false;
    }
    return SiteAuthority->MarkEntered();
}

bool ULongBarrowBraeheadComponent::ReachLegibleInteriorRecords()
{
    if ((!SiteAuthority && !BindSiteAuthority())
        || !SiteAuthority->bEntered
        || bInteriorRecordsReached
        || SiteAuthority->bUnforgettableImageWitnessed
        || SiteAuthority->bBottomResolved)
    {
        return false;
    }

    if (!SiteAuthority->MarkUnforgettableImageWitnessed())
    {
        return false;
    }
    if (!SiteAuthority->MarkBottomResolved(FName(TEXT("long-barrow.legible-records-bottom"))))
    {
        return false;
    }

    bInteriorRecordsReached = true;
    return true;
}

bool ULongBarrowBraeheadComponent::OpenMandatoryReturnShortcut()
{
    if (!SiteAuthority && !BindSiteAuthority())
    {
        return false;
    }
    return SiteAuthority->OpenReturnShortcutFromInside();
}

bool ULongBarrowBraeheadComponent::IsDungeonComplete() const
{
    return SiteAuthority && bInteriorRecordsReached && SiteAuthority->IsStructurallyComplete();
}
