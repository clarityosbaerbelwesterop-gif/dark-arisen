#pragma once

#include "CoreMinimal.h"
#include "DuelingEnemyCharacter.h"
#include "MainStoryHolderBossCharacter.generated.h"

/**
 * Reusable native bridge for a physically defeated Holder boss.
 * Combat stays in the existing dueling enemy foundation; story/save ownership stays in MainStorySubsystem.
 */
UCLASS()
class DARKARISEN_API AMainStoryHolderBossCharacter : public ADuelingEnemyCharacter
{
    GENERATED_BODY()

public:
    AMainStoryHolderBossCharacter();
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Holder")
    FName BossId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Holder")
    FName MissionId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Holder")
    FName OutcomeKey;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Holder")
    FName OutcomeValue = TEXT("Defeated");

private:
    UFUNCTION()
    void HandleHolderDied(AActor* DamageCauser);
};
