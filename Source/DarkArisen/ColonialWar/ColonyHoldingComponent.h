// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ColonyHoldingComponent.generated.h"

UENUM(BlueprintType)
enum class EHoldingType : uint8
{
    Undeclared,
    TradePost,
    Settlement,
    MilitaryStronghold,
    AllianceBastion
};

UENUM(BlueprintType)
enum class EHoldingDevelopmentStage : uint8
{
    Claimed = 0,
    Functional = 1,
    Established = 2,
    Developed = 3,
    Thriving = 4
};

UENUM(BlueprintType)
enum class ESettlementSpecialisation : uint8
{
    None,
    Plantation,
    Craft,
    FreeSettlement,
    Port
};

UENUM(BlueprintType)
enum class EHoldingPopulationKind : uint8
{
    Settler,
    FreedLabourer,
    Refugee,
    Specialist,
    CrewFamily
};

USTRUCT(BlueprintType)
struct FDeliveredHoldingPerson
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName PersonId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EHoldingPopulationKind Kind = EHoldingPopulationKind::Settler;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName VoyageId;
};

/**
 * M5 holding source boundary. Growth is person-by-person: a population record must come from an
 * authored delivered person/voyage rather than a numeric population purchase. Razed castles are
 * never ownable. Alliance Bastions are explicitly not Jake's: no tax, sale, conversion or hired
 * garrison path exists.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UColonyHoldingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UColonyHoldingComponent();

    UFUNCTION(BlueprintCallable, Category="ColonialWar|Holding")
    bool ClaimHolding(FName InHoldingId, bool bCastleTakenIntact, bool bOldFortressForAlliance,
        bool bAbandonedSite, bool bWasRazed);

    UFUNCTION(BlueprintCallable, Category="ColonialWar|Holding")
    bool DeclareHoldingType(EHoldingType NewType);

    /** External economy/building systems prove the authored requirements; this state only advances once. */
    UFUNCTION(BlueprintCallable, Category="ColonialWar|Holding")
    bool AdvanceDevelopment(bool bAuthoredRequirementsSatisfied);

    UFUNCTION(BlueprintCallable, Category="ColonialWar|Holding")
    bool ChooseSettlementSpecialisation(ESettlementSpecialisation NewSpecialisation);

    UFUNCTION(BlueprintCallable, Category="ColonialWar|Holding|Population")
    bool RecordDeliveredPerson(const FDeliveredHoldingPerson& Person);

    UFUNCTION(BlueprintCallable, Category="ColonialWar|Holding|Damage")
    bool RecordPersistentDamage(FName DamageId);

    UFUNCTION(BlueprintCallable, Category="ColonialWar|Holding|Damage")
    bool RecordDamageRepaired(FName DamageId);

    UFUNCTION(BlueprintPure, Category="ColonialWar|Holding")
    EHoldingType GetHoldingType() const { return HoldingType; }

    UFUNCTION(BlueprintPure, Category="ColonialWar|Holding")
    EHoldingDevelopmentStage GetDevelopmentStage() const { return DevelopmentStage; }

    UFUNCTION(BlueprintPure, Category="ColonialWar|Holding")
    int32 GetDeliveredPopulationCount() const { return DeliveredPeople.Num(); }

    UFUNCTION(BlueprintPure, Category="ColonialWar|Holding")
    bool IsJakeOwned() const { return bClaimed && HoldingType != EHoldingType::AllianceBastion; }

    UFUNCTION(BlueprintPure, Category="ColonialWar|Holding")
    bool HasUnrepairedDamage() const { return PersistentDamageIds.Num() > 0; }

private:
    UPROPERTY(SaveGame)
    FName HoldingId;

    UPROPERTY(SaveGame)
    bool bClaimed = false;

    UPROPERTY(SaveGame)
    EHoldingType HoldingType = EHoldingType::Undeclared;

    UPROPERTY(SaveGame)
    EHoldingDevelopmentStage DevelopmentStage = EHoldingDevelopmentStage::Claimed;

    UPROPERTY(SaveGame)
    ESettlementSpecialisation SettlementSpecialisation = ESettlementSpecialisation::None;

    UPROPERTY(SaveGame)
    TMap<FName, FDeliveredHoldingPerson> DeliveredPeople;

    UPROPERTY(SaveGame)
    TSet<FName> PersistentDamageIds;
};
