// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SecretDungeonResolutionComponent.generated.h"

class UAuthoredDungeonSiteComponent;

UENUM(BlueprintType)
enum class ESecretDungeonResolutionRule : uint8
{
    Unknown,
    GroveTakeNothing,
    LeaveValuedOffering,
    WaitThreeGameDays,
    AgreementNoFight,
    FourthFortressWithheld,
    DocumentOrTakeBoat
};

/**
 * Choice-resolution authority for the six authored secret sites.
 *
 * Trust/map/route/artifact ACCESS is owned by FSecretDungeonAccessRules. This component begins only
 * after physical entry and owns the site's final combat/non-combat choice without inventing rewards.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API USecretDungeonResolutionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USecretDungeonResolutionComponent();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintPure, Category="Dungeon|Secret")
    ESecretDungeonResolutionRule GetResolutionRule() const { return ResolutionRule; }

    /** Combat is legal only at sites whose canon explicitly offers a combat variant. */
    UFUNCTION(BlueprintCallable, Category="Dungeon|Secret")
    bool ResolveCombatVariant(FName AuthoredCombatResolutionId);

    /** Grove Beneath the Grove: leave without taking anything. */
    UFUNCTION(BlueprintCallable, Category="Dungeon|Secret")
    bool ResolveGroveByLeavingUntouched();

    /** One That Waited: the source accepts something Jake genuinely values, not arbitrary trash. */
    UFUNCTION(BlueprintCallable, Category="Dungeon|Secret")
    bool ResolveByLeavingValuedOffering(FName InventoryItemId, bool bInventoryAuthorityConfirmsGenuineValue);

    /** Thing the Wyrm Left: begin the exact three-game-day non-combat wait. */
    UFUNCTION(BlueprintCallable, Category="Dungeon|Secret")
    bool BeginWyrmThreeDayWait();

    UFUNCTION(BlueprintCallable, Category="Dungeon|Secret")
    bool CompleteWyrmThreeDayWait();

    /** Fifth Connection Site: the only legal bottom is the Agreement; there is never combat. */
    UFUNCTION(BlueprintCallable, Category="Dungeon|Secret")
    bool ResolveFifthConnectionAgreement();

    /** Harbour That Was First: document/map the boats and leave them. */
    UFUNCTION(BlueprintCallable, Category="Dungeon|Secret")
    bool ResolveHarbourByDocumentingAndLeaving();

    UFUNCTION(BlueprintPure, Category="Dungeon|Secret")
    bool IsCombatVariantAllowed() const;

    static ESecretDungeonResolutionRule RuleForStableId(FName StableId);
    static bool IsThreeDayWaitComplete(int64 StartWorldMinutes, int64 CurrentWorldMinutes);

    static constexpr int64 RequiredWyrmWaitGameMinutes = 3 * 24 * 60;

private:
    bool BindSiteAuthority();
    bool CanResolveNonCombat(ESecretDungeonResolutionRule RequiredRule) const;

    UPROPERTY(Transient)
    TObjectPtr<UAuthoredDungeonSiteComponent> SiteAuthority;

    UPROPERTY(VisibleInstanceOnly, Category="Dungeon|Secret")
    ESecretDungeonResolutionRule ResolutionRule = ESecretDungeonResolutionRule::Unknown;

    UPROPERTY(SaveGame)
    bool bWyrmWaitStarted = false;

    UPROPERTY(SaveGame)
    int64 WyrmWaitStartWorldMinutes = 0;

    UPROPERTY(SaveGame)
    FName ValuedOfferingItemId;
};
