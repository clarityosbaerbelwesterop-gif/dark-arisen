// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DuelingEnemyCharacter.generated.h"

class UCombatComponent;
class UHealthComponent;
class UStaticMeshComponent;
class UStaminaComponent;

/** M1 greybox opponent: pursuit, committed attacks, health, posture, deflection and death. */
UCLASS()
class DARKARISEN_API ADuelingEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ADuelingEnemyCharacter();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Components")
    TObjectPtr<UHealthComponent> HealthComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Components")
    TObjectPtr<UStaminaComponent> StaminaComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Components")
    TObjectPtr<UCombatComponent> CombatComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Components")
    TObjectPtr<UStaticMeshComponent> GreyboxBody;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Behaviour")
    float AwarenessRangeCentimetres = 900.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Behaviour")
    float AttackRangeCentimetres = 210.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Behaviour")
    float AttackCooldownSeconds = 1.15f;

private:
    UPROPERTY()
    TObjectPtr<ACharacter> TargetCharacter;
    float AttackCooldownRemaining = 0.0f;

    UFUNCTION()
    void OnEnemyDied(AActor* DamageCauser);
};
