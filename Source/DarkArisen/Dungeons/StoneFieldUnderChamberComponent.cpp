// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Dungeons/StoneFieldUnderChamberComponent.h"

#include "Dungeons/AuthoredDungeonSiteComponent.h"
#include "GameFramework/Actor.h"

UStoneFieldUnderChamberComponent::UStoneFieldUnderChamberComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UStoneFieldUnderChamberComponent::BeginPlay()
{
    Super::BeginPlay();
    BindSiteAuthority();
}

bool UStoneFieldUnderChamberComponent::BindSiteAuthority()
{
    SiteAuthority = GetOwner() ? GetOwner()->FindComponentByClass<UAuthoredDungeonSiteComponent>() : nullptr;
    if (!SiteAuthority)
    {
        UE_LOG(LogTemp, Error, TEXT("Stone Field Under-Chamber requires UAuthoredDungeonSiteComponent on the same actor."));
        return false;
    }

    if (SiteAuthority->DungeonStableId != FName(TEXT("dungeon.fjordlund.stone-field-under-chamber")))
    {
        UE_LOG(LogTemp, Error, TEXT("Stone Field Under-Chamber bound to wrong stable ID: %s"), *SiteAuthority->DungeonStableId.ToString());
        SiteAuthority = nullptr;
        return false;
    }

    return SiteAuthority->IsDefinitionValid() || SiteAuthority->InitializeDefinition();
}

bool UStoneFieldUnderChamberComponent::MarkEnteredUnderStandingStones()
{
    if (!SiteAuthority && !BindSiteAuthority())
    {
        return false;
    }
    return SiteAuthority->MarkEntered();
}

bool UStoneFieldUnderChamberComponent::ReachNamesWall(const bool bBjornDeadAccordingToAuthority)
{
    if ((!SiteAuthority && !BindSiteAuthority())
        || !SiteAuthority->bEntered
        || bNamesWallReached)
    {
        return false;
    }

    if (!SiteAuthority->MarkUnforgettableImageWitnessed())
    {
        return false;
    }
    if (!SiteAuthority->MarkBottomResolved(FName(TEXT("stone-field.names-only-bottom"))))
    {
        return false;
    }

    bNamesWallReached = true;
    bBjornsNamePresentAtWitness = bBjornDeadAccordingToAuthority;
    return true;
}

bool UStoneFieldUnderChamberComponent::OpenMandatoryReturnShortcut()
{
    if (!SiteAuthority && !BindSiteAuthority())
    {
        return false;
    }
    return SiteAuthority->OpenReturnShortcutFromInside();
}

bool UStoneFieldUnderChamberComponent::IsDungeonComplete() const
{
    return SiteAuthority && bNamesWallReached && SiteAuthority->IsStructurallyComplete();
}
