// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class UStaminaComponent;
class UHealthComponent;

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
    None,
    Cutlass,
    CrystalKatana,
    Flintlock,
    Musket,
    Bow,
    Throwable
};

UENUM(BlueprintType)
enum class EWeaponWeightClass : uint8
{
    Light,
    Medium,
    Heavy,
    Great,
    Polearm
};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
    Idle,
    LightAttacking,
    HeavyAttacking,
    Parrying,
    Dodging,
    Backstepping,
    Staggered,
    NonHostile,
    Dead
};

UENUM(BlueprintType)
enum class EPostureVisualState : uint8
{
    Set,
    Pressed,
    Failing,
    BrokenImminent,
    Broken
};

UENUM(BlueprintType)
enum class ECombatHitKind : uint8
{
    Light,
    Heavy,
    ParryStrike,
    Critical
};

USTRUCT(BlueprintType)
struct FCombatHitProfile
{
    GENERATED_BODY()

    FCombatHitProfile() = default;
    FCombatHitProfile(
        const float InHealthDamage,
        const float InPostureDamage,
        const float InDeflectedPostureDamage)
        : HealthDamage(InHealthDamage),
          PostureDamage(InPostureDamage),
          DeflectedPostureDamage(InDeflectedPostureDamage)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HealthDamage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PostureDamage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DeflectedPostureDamage = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatStateChanged, ECombatState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, EWeaponSlot, NewWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPostureBroken);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnPostureVisualStateChanged,
    EPostureVisualState,
    PreviousState,
    EPostureVisualState,
    NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnRacheMeterChanged, float, NewValue, float, MaximumValue);

/** M1 combat state with real action commitment and no action-cancel path. */
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

    /** Animation may request completion, but cannot bypass the commitment timer. */
    UFUNCTION(BlueprintCallable, Category = "Combat|State")
    void FinishAction();
    UFUNCTION(BlueprintCallable, Category = "Combat|State")
    void SetDead();
    /** Permanently leaves combat without fabricating a death. Used by authored mercy routes. */
    UFUNCTION(BlueprintCallable, Category = "Combat|State")
    void SetNonHostile();

    UFUNCTION(BlueprintCallable, Category = "Combat|Loadout")
    void EquipWeapon(EWeaponSlot Slot);
    UFUNCTION(BlueprintCallable, Category = "Combat|Loadout")
    void SetWeaponWeightClass(EWeaponWeightClass WeightClass);

    UFUNCTION(BlueprintCallable, Category = "Combat|Rache")
    bool StartRache();
    UFUNCTION(BlueprintCallable, Category = "Combat|Rache")
    void StopRache();
    UFUNCTION(BlueprintCallable, Category = "Combat|Rache")
    void AddRacheFuel(float Percent);

    UFUNCTION(BlueprintPure, Category = "Combat|Parry")
    bool IsDeflectionWindowOpen() const { return DeflectionWindowRemaining > 0.0f; }
    UFUNCTION(BlueprintPure, Category = "Combat|State")
    bool IsActionCommitted() const { return ActionCommitmentRemaining > 0.0f; }
    UFUNCTION(BlueprintPure, Category = "Combat|State")
    bool IsCombatTargetable() const
    {
        return CurrentState != ECombatState::Dead && CurrentState != ECombatState::NonHostile;
    }
    UFUNCTION(BlueprintPure, Category = "Combat|Timing")
    int32 GetStartupFrames() const;
    UFUNCTION(BlueprintPure, Category = "Combat|Timing")
    int32 GetRecoveryFrames() const;
    UFUNCTION(BlueprintPure, Category = "Combat|Timing")
    float GetMinimumCommitmentSeconds() const;
    UFUNCTION(BlueprintPure, Category = "Combat|Posture")
    float GetPostureRemainingFraction() const;
    UFUNCTION(BlueprintPure, Category = "Combat|Loadout")
    bool IsKatanaEquipped() const { return CurrentMelee == EWeaponSlot::CrystalKatana; }

    /** Resolves one authored melee contact. Returns false when the target is invalid. */
    UFUNCTION(BlueprintCallable, Category = "Combat|Hit")
    bool ResolveHitAgainst(AActor* Target, ECombatHitKind HitKind);

    UFUNCTION(BlueprintCallable, Category = "Combat|Hit")
    bool ResolveCriticalHit(AActor* Target);

    /** Animation notify path. Consumes the queued hit so the timing fallback cannot double-hit. */
    UFUNCTION(BlueprintCallable, Category = "Combat|Hit")
    bool ResolveQueuedMeleeHitFromAnimation(ECombatHitKind HitKind);

    /** Disables the greybox frame timer after an authored montage starts successfully. */
    UFUNCTION(BlueprintCallable, Category = "Combat|Hit")
    void RouteQueuedMeleeHitToAnimationNotify();

    static EPostureVisualState EvaluatePostureVisualState(float RemainingFraction);
    static FCombatHitProfile GetHitProfile(ECombatHitKind HitKind);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Posture")
    float MaxPosture = 100.0f;
    /** Damage accumulated: zero is fully set; MaxPosture is broken. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Posture")
    float CurrentPosture = 0.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Posture")
    float PoiseMultiplier = 1.0f;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Posture")
    EPostureVisualState PostureVisualState = EPostureVisualState::Set;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|State")
    ECombatState CurrentState = ECombatState::Idle;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|State")
    float ActionCommitmentRemaining = 0.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Loadout")
    EWeaponSlot CurrentMelee = EWeaponSlot::Cutlass;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Loadout")
    EWeaponSlot CurrentRanged = EWeaponSlot::Flintlock;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Loadout")
    EWeaponWeightClass CurrentWeightClass = EWeaponWeightClass::Medium;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Rache")
    float MaxRache = 100.0f;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Rache")
    float CurrentRache = 0.0f;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Rache")
    bool bRacheActive = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Rache", SaveGame)
    bool bRacheUnlocked = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Hit", meta = (ClampMin = "0.0"))
    float MeleeTraceStartCentimetres = 70.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Hit", meta = (ClampMin = "0.0"))
    float MeleeTraceEndCentimetres = 220.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Hit", meta = (ClampMin = "1.0"))
    float MeleeTraceRadiusCentimetres = 42.0f;

    UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
    FOnCombatStateChanged OnStateChanged;
    UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
    FOnWeaponChanged OnWeaponChanged;
    UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
    FOnPostureBroken OnPostureBroken;
    UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
    FOnPostureVisualStateChanged OnPostureVisualStateChanged;
    UPROPERTY(BlueprintAssignable, Category = "Combat|Events")
    FOnRacheMeterChanged OnRacheMeterChanged;

private:
    UPROPERTY()
    TObjectPtr<UStaminaComponent> CachedStamina;
    UPROPERTY()
    TObjectPtr<UHealthComponent> CachedHealth;
    float DeflectionWindowRemaining = 0.0f;
    float RacheRealSecondsRemaining = 0.0f;
    float PostureRegenDelayRemaining = 0.0f;
    float PendingHitDelayRemaining = 0.0f;
    ECombatHitKind PendingHitKind = ECombatHitKind::Light;
    bool bHasPendingHit = false;
    bool bPendingHitUsesFrameFallback = true;

    bool BeginCommittedAction(ECombatState NewState, float StaminaCost, float DurationSeconds);
    void QueueMeleeHit(ECombatHitKind HitKind);
    void ClearQueuedMeleeHit();
    bool TraceAndResolvePendingHit();
    void CompleteStagger();
    void SignalCombatActivity();
    void RefreshPostureVisualState();
    void SetState(ECombatState NewState);
};
