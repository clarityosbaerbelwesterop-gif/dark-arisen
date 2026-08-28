// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnStaminaChangedSignature, float, NewStamina, float, MaximumStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaDepletedSignature);

UCLASS(ClassGroup = (DarkArisen), meta = (BlueprintSpawnableComponent))
class DARKARISEN_API UStaminaComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UStaminaComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintPure, Category = "Stamina")
    bool CanAfford(float Cost) const;

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    bool ConsumeStamina(float Cost);

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    void RestoreFromPerfectParry(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    void SetSprinting(bool bSprintActive);

    UFUNCTION(BlueprintPure, Category = "Stamina")
    bool IsSprinting() const { return bSprinting; }

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    void SetRegenMultiplier(float Multiplier);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina|Stats", meta = (ClampMin = "1.0"))
    float MaxStamina = 120.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Stamina|Stats")
    float CurrentStamina = 120.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina|Regen", meta = (ClampMin = "0.0"))
    float BaseRegenRate = 20.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina|Regen", meta = (ClampMin = "0.0"))
    float RegenDelay = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina|Regen", meta = (ClampMin = "0.0"))
    float ExhaustionRegenDelay = 3.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Stamina|Regen")
    float RegenMultiplier = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina|Sprint", meta = (ClampMin = "0.0"))
    float SprintDrainPerSecond = 10.0f;

    UPROPERTY(BlueprintAssignable, Category = "Stamina|Events")
    FOnStaminaChangedSignature OnStaminaChanged;

    UPROPERTY(BlueprintAssignable, Category = "Stamina|Events")
    FOnStaminaDepletedSignature OnStaminaDepleted;

private:
    float RegenLockTimer = 0.0f;
    bool bSprinting = false;
    bool bDepletionBroadcast = false;
};
