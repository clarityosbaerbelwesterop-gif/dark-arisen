// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Components/HealthComponent.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
    bIsDead = CurrentHealth <= 0.0f;
}

void UHealthComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsDead || PendingHeal <= 0.0f || HealTimeRemaining <= 0.0f)
    {
        PendingHeal = 0.0f;
        HealTimeRemaining = 0.0f;
        SetComponentTickEnabled(false);
        return;
    }

    const float StepRatio = FMath::Clamp(DeltaTime / HealTimeRemaining, 0.0f, 1.0f);
    const float RequestedHeal = PendingHeal * StepRatio;
    const float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + RequestedHeal);
    const float AppliedHeal = CurrentHealth - PreviousHealth;

    PendingHeal = FMath::Max(0.0f, PendingHeal - RequestedHeal);
    HealTimeRemaining = FMath::Max(0.0f, HealTimeRemaining - DeltaTime);

    if (AppliedHeal > 0.0f)
    {
        OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, AppliedHeal);
    }

    if (CurrentHealth >= MaxHealth || PendingHeal <= KINDA_SMALL_NUMBER || HealTimeRemaining <= 0.0f)
    {
        PendingHeal = 0.0f;
        HealTimeRemaining = 0.0f;
        SetComponentTickEnabled(false);
    }
}

void UHealthComponent::ApplyDamage(const float Amount, AActor* DamageCauser)
{
    if (bIsDead || Amount <= 0.0f)
    {
        return;
    }

    const float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, CurrentHealth - PreviousHealth);

    if (CurrentHealth <= 0.0f)
    {
        bIsDead = true;
        PendingHeal = 0.0f;
        HealTimeRemaining = 0.0f;
        SetComponentTickEnabled(false);
        OnDied.Broadcast(DamageCauser);
    }
}

void UHealthComponent::ApplyHeal(const float Amount, const EHealType HealType, const float Duration)
{
    if (bIsDead || Amount <= 0.0f || CurrentHealth >= MaxHealth)
    {
        return;
    }

    if (HealType == EHealType::OverTime && Duration > KINDA_SMALL_NUMBER)
    {
        PendingHeal = Amount;
        HealTimeRemaining = Duration;
        SetComponentTickEnabled(true);
        return;
    }

    const float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Amount);
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, CurrentHealth - PreviousHealth);
}

float UHealthComponent::GetHealthPercent() const
{
    return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
}

