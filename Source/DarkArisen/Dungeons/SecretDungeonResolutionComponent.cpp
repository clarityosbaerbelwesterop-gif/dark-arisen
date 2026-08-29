// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Dungeons/SecretDungeonResolutionComponent.h"

#include "Dungeons/AuthoredDungeonSiteComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "World/DarkArisenWorldRulesSubsystem.h"

USecretDungeonResolutionComponent::USecretDungeonResolutionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USecretDungeonResolutionComponent::BeginPlay()
{
    Super::BeginPlay();
    BindSiteAuthority();
}

ESecretDungeonResolutionRule USecretDungeonResolutionComponent::RuleForStableId(const FName StableId)
{
    if (StableId == FName(TEXT("dungeon.rexa.grove-beneath-grove")))
    {
        return ESecretDungeonResolutionRule::GroveTakeNothing;
    }
    if (StableId == FName(TEXT("dungeon.fjordlund.one-that-waited")))
    {
        return ESecretDungeonResolutionRule::LeaveValuedOffering;
    }
    if (StableId == FName(TEXT("dungeon.ashenmoor.thing-wyrm-left")))
    {
        return ESecretDungeonResolutionRule::WaitThreeGameDays;
    }
    if (StableId == FName(TEXT("dungeon.cross-network.fifth-connection")))
    {
        return ESecretDungeonResolutionRule::AgreementNoFight;
    }
    if (StableId == FName(TEXT("dungeon.region06.fourth-fortress")))
    {
        return ESecretDungeonResolutionRule::FourthFortressWithheld;
    }
    if (StableId == FName(TEXT("dungeon.sea.harbour-that-was-first")))
    {
        return ESecretDungeonResolutionRule::DocumentOrTakeBoat;
    }
    return ESecretDungeonResolutionRule::Unknown;
}

bool USecretDungeonResolutionComponent::BindSiteAuthority()
{
    SiteAuthority = GetOwner() ? GetOwner()->FindComponentByClass<UAuthoredDungeonSiteComponent>() : nullptr;
    if (!SiteAuthority)
    {
        UE_LOG(LogTemp, Error, TEXT("Secret dungeon resolution requires UAuthoredDungeonSiteComponent on the same actor."));
        return false;
    }

    if (!(SiteAuthority->IsDefinitionValid() || SiteAuthority->InitializeDefinition())
        || !SiteAuthority->IsSecretSite())
    {
        UE_LOG(LogTemp, Error, TEXT("Secret dungeon resolution is attached to an invalid or non-secret site."));
        SiteAuthority = nullptr;
        ResolutionRule = ESecretDungeonResolutionRule::Unknown;
        return false;
    }

    ResolutionRule = RuleForStableId(SiteAuthority->DungeonStableId);
    if (ResolutionRule == ESecretDungeonResolutionRule::Unknown)
    {
        UE_LOG(LogTemp, Error, TEXT("Secret dungeon has no authored resolution rule: %s"), *SiteAuthority->DungeonStableId.ToString());
        SiteAuthority = nullptr;
        return false;
    }
    return true;
}

bool USecretDungeonResolutionComponent::IsCombatVariantAllowed() const
{
    return ResolutionRule == ESecretDungeonResolutionRule::GroveTakeNothing
        || ResolutionRule == ESecretDungeonResolutionRule::LeaveValuedOffering
        || ResolutionRule == ESecretDungeonResolutionRule::WaitThreeGameDays
        || ResolutionRule == ESecretDungeonResolutionRule::DocumentOrTakeBoat;
}

bool USecretDungeonResolutionComponent::ResolveCombatVariant(const FName AuthoredCombatResolutionId)
{
    if ((!SiteAuthority && !BindSiteAuthority())
        || !SiteAuthority->bEntered
        || !IsCombatVariantAllowed()
        || AuthoredCombatResolutionId.IsNone())
    {
        return false;
    }

    return SiteAuthority->MarkBottomResolved(AuthoredCombatResolutionId);
}

bool USecretDungeonResolutionComponent::CanResolveNonCombat(
    const ESecretDungeonResolutionRule RequiredRule) const
{
    return SiteAuthority
        && SiteAuthority->bEntered
        && !SiteAuthority->bBottomResolved
        && ResolutionRule == RequiredRule;
}

bool USecretDungeonResolutionComponent::ResolveGroveByLeavingUntouched()
{
    if (!CanResolveNonCombat(ESecretDungeonResolutionRule::GroveTakeNothing)
        || SiteAuthority->ArtifactDisposition != EDungeonArtifactDisposition::None)
    {
        return false;
    }

    return SiteAuthority->MarkBottomResolved(TEXT("secret.grove.left-untouched"));
}

bool USecretDungeonResolutionComponent::ResolveByLeavingValuedOffering(
    const FName InventoryItemId,
    const bool bInventoryAuthorityConfirmsGenuineValue)
{
    if (!CanResolveNonCombat(ESecretDungeonResolutionRule::LeaveValuedOffering)
        || InventoryItemId.IsNone()
        || !bInventoryAuthorityConfirmsGenuineValue
        || !ValuedOfferingItemId.IsNone())
    {
        return false;
    }

    ValuedOfferingItemId = InventoryItemId;
    return SiteAuthority->MarkBottomResolved(TEXT("secret.steersman.valued-offering-left"));
}

bool USecretDungeonResolutionComponent::IsThreeDayWaitComplete(
    const int64 StartWorldMinutes,
    const int64 CurrentWorldMinutes)
{
    return StartWorldMinutes >= 0
        && CurrentWorldMinutes >= StartWorldMinutes
        && (CurrentWorldMinutes - StartWorldMinutes) >= RequiredWyrmWaitGameMinutes;
}

bool USecretDungeonResolutionComponent::BeginWyrmThreeDayWait()
{
    if (!CanResolveNonCombat(ESecretDungeonResolutionRule::WaitThreeGameDays)
        || bWyrmWaitStarted)
    {
        return false;
    }

    UWorld* World = GetWorld();
    UDarkArisenWorldRulesSubsystem* Rules = World ? World->GetSubsystem<UDarkArisenWorldRulesSubsystem>() : nullptr;
    if (!Rules)
    {
        return false;
    }

    bWyrmWaitStarted = true;
    WyrmWaitStartWorldMinutes = Rules->GetTotalWorldMinutes();
    return true;
}

bool USecretDungeonResolutionComponent::CompleteWyrmThreeDayWait()
{
    if (!CanResolveNonCombat(ESecretDungeonResolutionRule::WaitThreeGameDays)
        || !bWyrmWaitStarted)
    {
        return false;
    }

    UWorld* World = GetWorld();
    UDarkArisenWorldRulesSubsystem* Rules = World ? World->GetSubsystem<UDarkArisenWorldRulesSubsystem>() : nullptr;
    if (!Rules
        || !IsThreeDayWaitComplete(WyrmWaitStartWorldMinutes, Rules->GetTotalWorldMinutes()))
    {
        return false;
    }

    return SiteAuthority->MarkBottomResolved(TEXT("secret.wyrm.waited-three-game-days"));
}

bool USecretDungeonResolutionComponent::ResolveFifthConnectionAgreement()
{
    if (!CanResolveNonCombat(ESecretDungeonResolutionRule::AgreementNoFight))
    {
        return false;
    }
    return SiteAuthority->MarkBottomResolved(TEXT("secret.fifth-connection.agreement"));
}

bool USecretDungeonResolutionComponent::ResolveHarbourByDocumentingAndLeaving()
{
    if (!CanResolveNonCombat(ESecretDungeonResolutionRule::DocumentOrTakeBoat)
        || SiteAuthority->ArtifactDisposition != EDungeonArtifactDisposition::None)
    {
        return false;
    }
    return SiteAuthority->MarkBottomResolved(TEXT("secret.harbour.documented-and-left"));
}
