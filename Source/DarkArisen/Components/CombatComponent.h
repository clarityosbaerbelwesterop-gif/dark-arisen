// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class UStaminaComponent;

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
    None,
    Cutlass,
    Flintlock,
    Musket,
    Bow,
    Throwable
};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
    Idle,
    LightAttacking,
    HeavyAttacking,
    Parrying,
    Dodging,
    Staggered,
    Dead
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatStateChanged, ECombatState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, EWeaponSlot, NewWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPostureBroken);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRacheMeterChanged, float, NewValue, float, MaximumValue);

/** Minimal M0 combat state. No action can cancel another action. */
UCLASS(ClassGroup = (DarkArisen), meta = (BlueprintSpawnableComponent))
class DARKARISEN_API UCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCombatComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Combat|Posture")
    void AddPostureDamage(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Combat|Moveset")
    bool PerformLightAttack();

    UFUNCTION(BlueprintCallable, Category = "Combat|Moveset")
    bool PerformHeavyAttack();

    UFUNCTION(BlueprintCallable, Category = "Combat|Moveset")
    bool PerformParry();

    UFUNCTION(BlueprintCallable, Category = "Combat|Moveset")
    bool PerformDodge(const FVector& Direction);

    UFUNCTION(BlueprintCallable, Category = "Combat|State")
    void FinishAction();

    UFUNCTION(BlueprintCallable, Category = "Combat|Loadout")
    void EquipWeapon(EWeaponSlot Slot);

    UFUNCTION(BlueprintCallable, Category = "Combat|Rache")
    bool StartRache();

    UFUNCTION(BlueprintCallable, Category = "Combat|Rache")
    void StopRache();

    UFUNCTION(BlueprintCallable, Category = "Combat|Rache")
    void AddRacheFuel(float Percent);

    UFUNCTION(BlueprintPure, Category = "Combat|Parry")
    bool IsDeflectionWindowOpen() const { return DeflectionWindowRemaining > 0.0f; }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Posture")
    float MaxPosture = 100.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Posture")
    float CurrentPosture = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Posture")
    float PoiseMultiplier = 1.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|State")
    ECombatState CurrentState = ECombatState::Idle;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Loadout")
    EWeaponSlot CurrentMelee = EWeaponSlot::Cutlass;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Loadout")
    EWeaponSlot CurrentRanged = EWeaponSlot::Flintlock;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Rache")
    float MaxRache = 100.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Rache")
    float CurrentRache = 0.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Rache")
    bool bRacheActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Rache", SaveGame)
    bool bRacheUnlocked = false;

    UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
    FOnCombatStateChanged OnStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
    FOnWeaponChanged OnWeaponChanged;

    UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
    FOnPostureBroken OnPostureBroken;

    UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
    FOnRacheMeterChanged OnRacheMeterChanged;

private:
    UPROPERTY()
    TObjectPtr<UStaminaComponent> CachedStamina;

    float DeflectionWindowRemaining = 0.0f;
    float RacheRealSecondsRemaining = 0.0f;

    void SetState(ECombatState NewState);
};
