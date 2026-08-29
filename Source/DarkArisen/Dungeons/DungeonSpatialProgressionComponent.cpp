// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Dungeons/DungeonSpatialProgressionComponent.h"

#include "Dungeons/AuthoredDungeonSiteComponent.h"
#include "GameFramework/Actor.h"

UDungeonSpatialProgressionComponent::UDungeonSpatialProgressionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDungeonSpatialProgressionComponent::BeginPlay()
{
    Super::BeginPlay();
    BindSiteAuthority();
}

bool UDungeonSpatialProgressionComponent::BindSiteAuthority()
{
    SiteAuthority = GetOwner() ? GetOwner()->FindComponentByClass<UAuthoredDungeonSiteComponent>() : nullptr;
    if (!SiteAuthority)
    {
        UE_LOG(LogTemp, Error, TEXT("Dungeon spatial progression requires UAuthoredDungeonSiteComponent on the same actor."));
        return false;
    }
    return SiteAuthority->IsDefinitionValid() || SiteAuthority->InitializeDefinition();
}

bool UDungeonSpatialProgressionComponent::IsLegalNextBeat(
    const EDungeonSpatialBeat Current,
    const EDungeonSpatialBeat Proposed)
{
    const uint8 CurrentValue = static_cast<uint8>(Current);
    const uint8 ProposedValue = static_cast<uint8>(Proposed);
    return ProposedValue == CurrentValue + 1;
}

bool UDungeonSpatialProgressionComponent::RecordPhysicalBeat(const EDungeonSpatialBeat Beat)
{
    if ((!SiteAuthority && !BindSiteAuthority())
        || !SiteAuthority->bEntered
        || !IsLegalNextBeat(CurrentBeat, Beat))
    {
        return false;
    }

    if (Beat == EDungeonSpatialBeat::ReturnOpened)
    {
        if (CurrentBeat != EDungeonSpatialBeat::Floor
            || !SiteAuthority->OpenReturnShortcutFromInside())
        {
            return false;
        }
    }

    CurrentBeat = Beat;
    return true;
}
