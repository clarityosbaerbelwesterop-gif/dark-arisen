// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Dungeons/FirstHouseComponent.h"

#include "Dungeons/AuthoredDungeonSiteComponent.h"
#include "GameFramework/Actor.h"

UFirstHouseComponent::UFirstHouseComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UFirstHouseComponent::BeginPlay()
{
    Super::BeginPlay();
    BindSiteAuthority();
}

bool UFirstHouseComponent::BindSiteAuthority()
{
    SiteAuthority = GetOwner() ? GetOwner()->FindComponentByClass<UAuthoredDungeonSiteComponent>() : nullptr;
    if (!SiteAuthority)
    {
        UE_LOG(LogTemp, Error, TEXT("First House component requires UAuthoredDungeonSiteComponent on the same actor."));
        return false;
    }

    if (SiteAuthority->DungeonStableId != FName(TEXT("dungeon.region06.first-house")))
    {
        UE_LOG(LogTemp, Error, TEXT("First House component bound to wrong stable ID: %s"), *SiteAuthority->DungeonStableId.ToString());
        SiteAuthority = nullptr;
        return false;
    }

    return SiteAuthority->IsDefinitionValid() || SiteAuthority->InitializeDefinition();
}

bool UFirstHouseComponent::MarkEnteredThroughOpenDoor()
{
    if (!SiteAuthority && !BindSiteAuthority())
    {
        return false;
    }
    return SiteAuthority->MarkEntered();
}

bool UFirstHouseComponent::MarkHalfEatenMealWitnessed()
{
    if ((!SiteAuthority && !BindSiteAuthority()) || !SiteAuthority->bEntered)
    {
        return false;
    }
    return SiteAuthority->MarkUnforgettableImageWitnessed();
}

bool UFirstHouseComponent::ResolveEmptyBottom()
{
    if ((!SiteAuthority && !BindSiteAuthority()) || !SiteAuthority->bEntered)
    {
        return false;
    }
    return SiteAuthority->MarkBottomResolved(FName(TEXT("first-house.empty-bottom")));
}

bool UFirstHouseComponent::OpenMandatoryReturnShortcut()
{
    if (!SiteAuthority && !BindSiteAuthority())
    {
        return false;
    }
    return SiteAuthority->OpenReturnShortcutFromInside();
}

bool UFirstHouseComponent::IsDungeonComplete() const
{
    return SiteAuthority && SiteAuthority->IsStructurallyComplete();
}
