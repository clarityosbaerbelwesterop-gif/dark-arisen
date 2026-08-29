// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Dungeons/AuthoredDungeonSiteComponent.h"

UAuthoredDungeonSiteComponent::UAuthoredDungeonSiteComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAuthoredDungeonSiteComponent::BeginPlay()
{
    Super::BeginPlay();
    InitializeDefinition();
}

bool UAuthoredDungeonSiteComponent::InitializeDefinition()
{
    FAuthoredDungeonCatalogEntry Definition;
    FAuthoredDungeonProductionProfile ProductionProfile;

    const bool bHasCatalogDefinition =
        FAuthoredDungeonCatalog::TryGetKnownSite(DungeonStableId, Definition);
    const bool bHasProductionProfile =
        FAuthoredDungeonProductionProfiles::TryGetProfile(DungeonStableId, ProductionProfile);

    bDefinitionValid = bHasCatalogDefinition && bHasProductionProfile;
    if (!bDefinitionValid)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("Authored dungeon site on %s has incomplete/unknown stable ID '%s'. It remains inert rather than inventing a definition."),
            *GetNameSafe(GetOwner()),
            *DungeonStableId.ToString());
        return false;
    }

    CachedDefinition = MoveTemp(Definition);
    CachedProductionProfile = MoveTemp(ProductionProfile);
    return true;
}

FString UAuthoredDungeonSiteComponent::GetAuthoredDisplayName() const
{
    return bDefinitionValid ? CachedDefinition.DisplayName : FString();
}

FString UAuthoredDungeonSiteComponent::GetAuthoredAccessAndDiscovery() const
{
    return bDefinitionValid ? CachedProductionProfile.AccessAndDiscovery : FString();
}

FString UAuthoredDungeonSiteComponent::GetAuthoredPuzzleDetail() const
{
    return bDefinitionValid ? CachedProductionProfile.PuzzleLanguageDetail : FString();
}

FString UAuthoredDungeonSiteComponent::GetAuthoredHazardDetail() const
{
    return bDefinitionValid ? CachedProductionProfile.HazardDetail : FString();
}

FString UAuthoredDungeonSiteComponent::GetAuthoredImageBrief() const
{
    return bDefinitionValid ? CachedProductionProfile.UnforgettableImage : FString();
}

FString UAuthoredDungeonSiteComponent::GetAuthoredBossOrBottomDetail() const
{
    return bDefinitionValid ? CachedProductionProfile.BossOrBottomDetail : FString();
}

FString UAuthoredDungeonSiteComponent::GetAuthoredRewardDetail() const
{
    return bDefinitionValid ? CachedProductionProfile.RewardDetail : FString();
}

bool UAuthoredDungeonSiteComponent::IsSecretSite() const
{
    return bDefinitionValid && CachedDefinition.bSecretSite;
}

bool UAuthoredDungeonSiteComponent::IsNoBossSite() const
{
    return bDefinitionValid && CachedDefinition.BossState == EDungeonCatalogBossState::NoBoss;
}

bool UAuthoredDungeonSiteComponent::IsImageWithheld() const
{
    return bDefinitionValid && CachedProductionProfile.bImageWithheld;
}

bool UAuthoredDungeonSiteComponent::IsRewardExplicitlyNone() const
{
    return bDefinitionValid && CachedProductionProfile.bRewardExplicitlyNone;
}

bool UAuthoredDungeonSiteComponent::IsWarReactive() const
{
    return bDefinitionValid && CachedProductionProfile.bWarReactive;
}

bool UAuthoredDungeonSiteComponent::SupportsExplicitNonCombatResolution() const
{
    return bDefinitionValid && CachedProductionProfile.bSupportsExplicitNonCombatResolution;
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
    if (CachedProductionProfile.bImageWithheld)
    {
        // A withheld image is not a license for generic spectacle. It stays unresolved until canon
        // authors it, so common progression cannot self-certify the image beat.
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
    if (CachedProductionProfile.bBossWithheldOrUnresolved)
    {
        // Withheld/unresolved bottom content remains locked rather than accepting an invented result.
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
        || ArtifactDisposition != EDungeonArtifactDisposition::None
        || CachedProductionProfile.bRewardExplicitlyNone
        || CachedProductionProfile.bRewardWithheldOrUnresolved)
    {
        return false;
    }

    ArtifactDisposition = Disposition;
    return true;
}

bool UAuthoredDungeonSiteComponent::IsStructurallyComplete() const
{
    if (!bDefinitionValid
        || CachedProductionProfile.bImageWithheld
        || CachedProductionProfile.bBossWithheldOrUnresolved)
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
