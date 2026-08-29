// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Dungeons/ObsidianVaultComponent.h"

#include "Dungeons/AuthoredDungeonSiteComponent.h"

UObsidianVaultComponent::UObsidianVaultComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UObsidianVaultComponent::BeginPlay()
{
    Super::BeginPlay();
    BindSiteAuthority();
}

bool UObsidianVaultComponent::BindSiteAuthority()
{
    SiteAuthority = GetOwner() ? GetOwner()->FindComponentByClass<UAuthoredDungeonSiteComponent>() : nullptr;
    if (!SiteAuthority)
    {
        UE_LOG(LogTemp, Error, TEXT("Obsidian Vault component requires UAuthoredDungeonSiteComponent on the same actor."));
        return false;
    }

    if (SiteAuthority->DungeonStableId != FName(TEXT("dungeon.ashenmoor.obsidian-vault")))
    {
        UE_LOG(LogTemp, Error, TEXT("Obsidian Vault component bound to wrong dungeon stable ID: %s"), *SiteAuthority->DungeonStableId.ToString());
        SiteAuthority = nullptr;
        return false;
    }

    return SiteAuthority->IsDefinitionValid() || SiteAuthority->InitializeDefinition();
}

bool UObsidianVaultComponent::MarkEnteredThroughLavaFissure()
{
    if (!SiteAuthority && !BindSiteAuthority())
    {
        return false;
    }
    return SiteAuthority->MarkEntered();
}

bool UObsidianVaultComponent::MarkHeatChannelSolved()
{
    if ((!SiteAuthority && !BindSiteAuthority()) || !SiteAuthority->bEntered || bHeatChannelSolved)
    {
        return false;
    }

    bHeatChannelSolved = true;
    return true;
}

bool UObsidianVaultComponent::IsCanonicalReflectionImage(
    const int32 VisibleJakeReflections,
    const int32 WrongMovingReflections)
{
    return VisibleJakeReflections == CanonicalVisibleJakeReflections
        && WrongMovingReflections == CanonicalWrongMovingReflections;
}

bool UObsidianVaultComponent::RecordReflectionImageObservation(
    const int32 VisibleJakeReflections,
    const int32 WrongMovingReflections)
{
    if ((!SiteAuthority && !BindSiteAuthority())
        || !bHeatChannelSolved
        || !IsCanonicalReflectionImage(VisibleJakeReflections, WrongMovingReflections))
    {
        return false;
    }

    return SiteAuthority->MarkUnforgettableImageWitnessed();
}

bool UObsidianVaultComponent::ResolveReflectionEncounter(const FName OutcomeId)
{
    if ((!SiteAuthority && !BindSiteAuthority()) || !bHeatChannelSolved)
    {
        return false;
    }
    return SiteAuthority->MarkBottomResolved(OutcomeId);
}

bool UObsidianVaultComponent::OpenMandatoryReturnShortcut()
{
    if (!SiteAuthority && !BindSiteAuthority())
    {
        return false;
    }
    return SiteAuthority->OpenReturnShortcutFromInside();
}

bool UObsidianVaultComponent::RecoverLegendaryObsidian()
{
    if ((!SiteAuthority && !BindSiteAuthority())
        || !SiteAuthority->IsStructurallyComplete()
        || bLegendaryObsidianRecovered)
    {
        return false;
    }

    bLegendaryObsidianRecovered = true;
    return true;
}

bool UObsidianVaultComponent::IsDungeonStructurallyComplete() const
{
    return SiteAuthority && bHeatChannelSolved && SiteAuthority->IsStructurallyComplete();
}
