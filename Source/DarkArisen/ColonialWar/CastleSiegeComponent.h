// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CastleSiegeComponent.generated.h"

UENUM(BlueprintType)
enum class ESiegePhase : uint8
{
    None,
    Approach,
    Investment,
    Breach,
    Assault,
    Resolved
};

UENUM(BlueprintType)
enum class ESiegeDoor : uint8
{
    Wall,
    Back,
    Inside
};

UENUM(BlueprintType)
enum class ESiegeResolution : uint8
{
    None,
    HeldIntact,
    HeldDamaged,
    Razed,
    Defended,
    Lost
};

/**
 * Four-phase M5 castle siege state. A siege is a physical mission pipeline, not an ownership
 * button. The Wall/Back/Inside routes are distinct authored doors. Breaches and structural
 * damage persist so taking a castle by force means owning the damage afterward.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UCastleSiegeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCastleSiegeComponent();

    UFUNCTION(BlueprintCallable, Category="Siege|Authoring")
    bool ConfigureCastle(FName InCastleId, bool bInCoastal);

    UFUNCTION(BlueprintCallable, Category="Siege")
    bool BeginSiege(bool bDefensiveSiege);

    UFUNCTION(BlueprintCallable, Category="Siege|Approach")
    bool RecordApproachIntel(FName IntelId);

    UFUNCTION(BlueprintCallable, Category="Siege|Investment")
    bool RecordInvestmentAction(FName ActionId);

    UFUNCTION(BlueprintCallable, Category="Siege|Breach")
    bool CommitBreachDoor(ESiegeDoor Door);

    /** Physical breach/infiltration/conspiracy reports completion after gameplay. */
    UFUNCTION(BlueprintCallable, Category="Siege|Breach")
    bool RecordBreachCompleted(bool bStructuralDamageInflicted);

    UFUNCTION(BlueprintCallable, Category="Siege|Assault")
    bool RecordCommanderResolved(FName CommanderId);

    UFUNCTION(BlueprintCallable, Category="Siege|Assault")
    bool RecordStateTreasureRecovered(FName TreasureId);

    UFUNCTION(BlueprintCallable, Category="Siege|Resolution")
    bool ResolveSiege(ESiegeResolution Resolution);

    UFUNCTION(BlueprintPure, Category="Siege|WorldRead")
    ESiegePhase GetPhase() const { return Phase; }

    UFUNCTION(BlueprintPure, Category="Siege|WorldRead")
    bool HasPersistentBreachDamage() const { return bPersistentBreachDamage; }

    UFUNCTION(BlueprintPure, Category="Siege|WorldRead")
    ESiegeResolution GetResolution() const { return Resolution; }

    UFUNCTION(BlueprintPure, Category="Siege|WorldRead")
    bool IsDefensiveSiege() const { return bDefensive; }

private:
    UPROPERTY(SaveGame)
    FName CastleId;

    UPROPERTY(SaveGame)
    bool bCoastal = false;

    UPROPERTY(SaveGame)
    bool bDefensive = false;

    UPROPERTY(SaveGame)
    ESiegePhase Phase = ESiegePhase::None;

    UPROPERTY(SaveGame)
    ESiegeDoor Door = ESiegeDoor::Wall;

    UPROPERTY(SaveGame)
    bool bDoorCommitted = false;

    UPROPERTY(SaveGame)
    bool bBreachCompleted = false;

    UPROPERTY(SaveGame)
    bool bPersistentBreachDamage = false;

    UPROPERTY(SaveGame)
    bool bCommanderResolved = false;

    UPROPERTY(SaveGame)
    ESiegeResolution Resolution = ESiegeResolution::None;

    UPROPERTY(SaveGame)
    TSet<FName> ApproachIntel;

    UPROPERTY(SaveGame)
    TSet<FName> InvestmentActions;

    UPROPERTY(SaveGame)
    TSet<FName> RecoveredStateTreasures;
};
