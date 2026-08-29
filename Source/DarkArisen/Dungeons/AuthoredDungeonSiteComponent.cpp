// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Dungeons/AuthoredDungeonSiteComponent.h"

UAuthoredDungeonSiteComponent::UAuthoredDungeonSiteComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAuthoredDungeonSiteComponent::BeginPlay()
{
    Super::BeginPlay();
    bDefinitionValid = FAuthoredDungeonCatalog::TryGetKnownSite(DungeonStableId, CachedDefinition);
    if (!bDefinitionValid)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("Authored dungeon site on %s has unknown/unresolved stable ID '%s'. It remains inert rather than inventing a definition."),
            *GetNameSafe(GetOwner()),
            *DungeonStableId.ToString());
    }
}

FString UAuthoredDungeonSiteComponent::GetAuthoredDisplayName() const
{
    return bDefinitionValid ? CachedDefinition.DisplayName : FString();
}

bool UAuthoredDungeonSiteComponent::IsSecretSite() const
{
    return bDefinitionValid && CachedDefinition.bSecretSite;
}

bool UAuthoredDungeonSiteComponent::IsNoBossSite() const
{
    return bDefinitionValid && CachedDefinition.BossState == EDungeonCatalogBossState::NoBoss;
}

bool UAuthoredDungeonSiteComponent::CanBeRecordedOnPhysicalMapBeforeEntry() const
{
    return bDefinitionValid && CachedDefinition.Discovery == EDungeonCatalogDiscovery::Cartographic;
}

bool UAuthoredDungeonSiteComponent::CanBeRecordedOnPhysicalMapNow() const
{
    return bDefinitionValid && (bEntered || CanBeRecordedOnPhysicalMapBeforeEntry());
}

bool UAuthoredDungeonSiteComponent::MarkEntered()
{
    if (!bDefinitionValid || bEntered)
    {
        return false;
    }
    bEntered = true;
    return true;
}

bool UAuthoredDungeonSiteComponent::MarkUnforgettableImageWitnessed()
{
    if (!bDefinitionValid || !bEntered || bUnforgettableImageWitnessed)
    {
        return false;
    }
    bUnforgettableImageWitnessed = true;
    return true;
}

bool UAuthoredDungeonSiteComponent::MarkBottomResolved(const FName ResolutionId)
{
    if (!bDefinitionValid || !bEntered || bBottomResolved || ResolutionId.IsNone())
    {
        return false;
    }
    bBottomResolved = true;
    BottomResolutionId = ResolutionId;
    return true;
}

bool UAuthoredDungeonSiteComponent::OpenReturnShortcutFromInside()
{
    if (!bDefinitionValid || !bEntered || bReturnShortcutOpened)
    {
        return false;
    }
    if (!CachedDefinition.bReturnShortcutFromInside)
    {
        // No level is allowed to silently add a Return requirement that the catalog does not own.
        return false;
    }
    bReturnShortcutOpened = true;
    return true;
}

bool UAuthoredDungeonSiteComponent::RecordArtifactDisposition(
    const EDungeonArtifactDisposition Disposition)
{
    if (!bDefinitionValid
        || !bEntered
        || Disposition == EDungeonArtifactDisposition::None
        || ArtifactDisposition != EDungeonArtifactDisposition::None)
    {
        return false;
    }

    ArtifactDisposition = Disposition;
    return true;
}

bool UAuthoredDungeonSiteComponent::IsStructurallyComplete() const
{
    if (!bDefinitionValid)
    {
        return false;
    }

    // The global design standard requires an unforgettable image and an inside-opened Return for
    // named Tier-B+ sites. "No boss" means no fight, not no bottom state, so bottom resolution is
    // still required. Rewards are deliberately not part of structural completion because First
    // House gives nothing and many sites leave the sell/keep/return question optional.
    return bEntered
        && bUnforgettableImageWitnessed
        && bBottomResolved
        && (!CachedDefinition.bReturnShortcutFromInside || bReturnShortcutOpened);
}
