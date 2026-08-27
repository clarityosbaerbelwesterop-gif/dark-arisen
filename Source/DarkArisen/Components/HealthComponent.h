// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiedSignature, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnHealthChangedSignature, float, NewHealth, float, MaximumHealth, float, Delta);

UENUM(BlueprintType)
enum class EHealType : uint8
{
    Instant,
    OverTime
};

/** Health has no passive regeneration. Every increase is an explicit authored action. */
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

    UFUNCTION(BlueprintCallable, Category = "Health")
    void ApplyHeal(float Amount, EHealType HealType, float Duration = 0.0f);

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintPure, Category = "Health")
    bool IsDead() const { return bIsDead; }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|Stats", meta = (ClampMin = "1.0"))
    float MaxHealth = 200.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health|Stats")
    float CurrentHealth = 200.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health|Stats")
    bool bIsDead = false;

    UPROPERTY(BlueprintAssignable, Category = "Health|Events")
    FOnDiedSignature OnDied;

    UPROPERTY(BlueprintAssignable, Category = "Health|Events")
    FOnHealthChangedSignature OnHealthChanged;

private:
    float PendingHeal = 0.0f;
    float HealTimeRemaining = 0.0f;
};

