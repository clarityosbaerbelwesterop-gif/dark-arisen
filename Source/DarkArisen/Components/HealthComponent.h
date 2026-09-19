// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiedSignature, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnHealthChangedSignature, float, NewHealth, float, MaximumHealth, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnRallyChangedSignature, float, RecoverableHealth, float, WindowSecondsRemaining);

UENUM(BlueprintType)
enum class EHealType : uint8
{
    Instant,
    OverTime
};

UENUM(BlueprintType)
enum class ERallyDamageClass : uint8
{
    StandardEnemy,
    EliteEnemy,
    Boss,
    Grab,
    Environmental,
    Fire,
    Poison,
    Fall,
    Bleed
};

UENUM(BlueprintType)
enum class ERallyRecoveryAction : uint8
{
    LightHit,
    HeavyHit,
    ParryStrike,
    Critical
};

/** Health never passively regenerates. Rally is an explicit, time-limited recovery path. */
UCLASS(ClassGroup = (DarkArisen), meta = (BlueprintSpawnableComponent))
class DARKARISEN_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHealthComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Health")
    void ApplyDamage(float Amount, AActor* DamageCauser);

    UFUNCTION(BlueprintCallable, Category = "Health|Rally")
    void ApplyDamageWithRally(
        float Amount,
        AActor* DamageCauser,
        ERallyDamageClass DamageClass);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void ApplyHeal(float Amount, EHealType HealType, float Duration = 0.0f);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void ResetForRespawn(float HealthFraction = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Health|Rally")
    float RecoverRally(ERallyRecoveryAction RecoveryAction);

    UFUNCTION(BlueprintCallable, Category = "Health|Rally")
    void ExpireRally();

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintPure, Category = "Health")
    bool IsDead() const { return bIsDead; }

    UFUNCTION(BlueprintPure, Category = "Health|Rally")
    bool HasActiveRally() const { return RallyAvailableHealth > 0.0f; }

    static float GetRallyFractionForDamageClass(ERallyDamageClass DamageClass);
    static float GetRecoveryFractionForAction(ERallyRecoveryAction RecoveryAction);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|Stats", meta = (ClampMin = "1.0"))
    float MaxHealth = 200.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health|Stats")
    float CurrentHealth = 200.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health|Stats")
    bool bIsDead = false;

    /** Canon-protected story/noncombat actors can opt out of every HealthComponent damage path. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|Stats")
    bool bDamageImmune = false;

    /** Enabled for Jake, disabled for ordinary combatants. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|Rally")
    bool bRallyEnabled = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|Rally")
    float RallyWindowSeconds = 3.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health|Rally")
    float RallyAvailableHealth = 0.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health|Rally")
    float RallyWindowRemaining = 0.0f;

    UPROPERTY(BlueprintAssignable, Category = "Health|Events")
    FOnDiedSignature OnDied;

    UPROPERTY(BlueprintAssignable, Category = "Health|Events")
    FOnHealthChangedSignature OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Health|Events")
    FOnRallyChangedSignature OnRallyChanged;

private:
    float PendingHeal = 0.0f;
    float HealTimeRemaining = 0.0f;

    void TickHealing(float DeltaTime);
    void ClampRallyToMissingHealth();
    void RefreshTickState();
};
