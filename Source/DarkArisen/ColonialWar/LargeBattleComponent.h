// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LargeBattleComponent.generated.h"

UENUM(BlueprintType)
enum class ELargeBattleOutcome : uint8
{
    Unresolved,
    Victory,
    Defeat,
    Withdrawal
};

/**
 * M5 large-battle source boundary. Jake participates as one combatant/captain; his fall removes
 * him from direct control but does not end the battle. Final battle outcome must be reported by
 * authored force/world simulation, never inferred from player death.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API ULargeBattleComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    ULargeBattleComponent();

    UFUNCTION(BlueprintCallable, Category="Battle")
    bool BeginBattle(FName InBattleId);

    UFUNCTION(BlueprintCallable, Category="Battle")
    bool RecordJakeFallen();

    UFUNCTION(BlueprintCallable, Category="Battle")
    bool ResolveBattle(ELargeBattleOutcome Outcome);

    UFUNCTION(BlueprintPure, Category="Battle")
    bool IsBattleActive() const { return bBattleActive; }

    UFUNCTION(BlueprintPure, Category="Battle")
    bool IsJakeActiveInBattle() const { return bBattleActive && !bJakeFallen; }

    UFUNCTION(BlueprintPure, Category="Battle")
    ELargeBattleOutcome GetOutcome() const { return BattleOutcome; }

private:
    UPROPERTY(SaveGame)
    FName BattleId;

    UPROPERTY(SaveGame)
    bool bBattleActive = false;

    UPROPERTY(SaveGame)
    bool bJakeFallen = false;

    UPROPERTY(SaveGame)
    ELargeBattleOutcome BattleOutcome = ELargeBattleOutcome::Unresolved;
};
