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
    if (bIsDead) ExpireRally();
    RefreshTickState();
}

void UHealthComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (bIsDead)
    {
        PendingHeal = 0.0f;
        HealTimeRemaining = 0.0f;
        ExpireRally();
        RefreshTickState();
        return;
    }

    TickHealing(DeltaTime);
    if (RallyAvailableHealth > 0.0f)
    {
        RallyWindowRemaining = FMath::Max(0.0f, RallyWindowRemaining - DeltaTime);
        if (RallyWindowRemaining <= 0.0f) ExpireRally();
        else OnRallyChanged.Broadcast(RallyAvailableHealth, RallyWindowRemaining);
    }
    RefreshTickState();
}

void UHealthComponent::ApplyDamage(const float Amount, AActor* DamageCauser)
{
    ApplyDamageWithRally(Amount, DamageCauser, ERallyDamageClass::StandardEnemy);
}

void UHealthComponent::ApplyDamageWithRally(
    const float Amount,
    AActor* DamageCauser,
    const ERallyDamageClass DamageClass)
{
    if (bIsDead || Amount <= 0.0f) return;
    const float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.0f, MaxHealth);
    const float AppliedDamage = PreviousHealth - CurrentHealth;
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, -AppliedDamage);

    if (CurrentHealth <= 0.0f)
    {
        bIsDead = true;
        PendingHeal = 0.0f;
        HealTimeRemaining = 0.0f;
        ExpireRally();
        RefreshTickState();
        OnDied.Broadcast(DamageCauser);
        return;
    }

    const float RallyFraction = bRallyEnabled
        ? GetRallyFractionForDamageClass(DamageClass) : 0.0f;
    if (RallyFraction > 0.0f && AppliedDamage > 0.0f)
    {
        RallyAvailableHealth = FMath::Min(
            MaxHealth - CurrentHealth,
            RallyAvailableHealth + AppliedDamage * RallyFraction);
        RallyWindowRemaining = RallyWindowSeconds;
        OnRallyChanged.Broadcast(RallyAvailableHealth, RallyWindowRemaining);
    }
    RefreshTickState();
}

void UHealthComponent::ApplyHeal(
    const float Amount,
    const EHealType HealType,
    const float Duration)
{
    if (bIsDead || Amount <= 0.0f || CurrentHealth >= MaxHealth) return;
    if (HealType == EHealType::OverTime && Duration > KINDA_SMALL_NUMBER)
    {
        PendingHeal += Amount;
        HealTimeRemaining = FMath::Max(HealTimeRemaining, Duration);
        RefreshTickState();
        return;
    }
    const float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Amount);
    const float AppliedHeal = CurrentHealth - PreviousHealth;
    ClampRallyToMissingHealth();
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, AppliedHeal);
    RefreshTickState();
}

float UHealthComponent::RecoverRally(const ERallyRecoveryAction RecoveryAction)
{
    if (bIsDead || RallyAvailableHealth <= 0.0f) return 0.0f;
    const float RecoveryFraction = GetRecoveryFractionForAction(RecoveryAction);
    const float RequestedRecovery = RallyAvailableHealth * RecoveryFraction;
    const float AppliedRecovery = FMath::Min(RequestedRecovery, MaxHealth - CurrentHealth);
    CurrentHealth += AppliedRecovery;
    RallyAvailableHealth = FMath::Max(0.0f, RallyAvailableHealth - AppliedRecovery);
    ClampRallyToMissingHealth();
    if (AppliedRecovery > 0.0f)
        OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, AppliedRecovery);
    OnRallyChanged.Broadcast(RallyAvailableHealth, RallyWindowRemaining);
    RefreshTickState();
    return AppliedRecovery;
}

void UHealthComponent::ExpireRally()
{
    const bool bHadRally = RallyAvailableHealth > 0.0f || RallyWindowRemaining > 0.0f;
    RallyAvailableHealth = 0.0f;
    RallyWindowRemaining = 0.0f;
    if (bHadRally) OnRallyChanged.Broadcast(0.0f, 0.0f);
}

float UHealthComponent::GetHealthPercent() const
{
    return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
}

float UHealthComponent::GetRallyFractionForDamageClass(const ERallyDamageClass DamageClass)
{
    switch (DamageClass)
    {
    case ERallyDamageClass::StandardEnemy: return 0.70f;
    case ERallyDamageClass::EliteEnemy: return 0.60f;
    case ERallyDamageClass::Boss: return 0.50f;
    case ERallyDamageClass::Grab: return 0.30f;
    case ERallyDamageClass::Environmental: return 0.40f;
    case ERallyDamageClass::Fire: return 0.25f;
    case ERallyDamageClass::Poison: return 0.0f;
    case ERallyDamageClass::Fall: return 0.50f;
    case ERallyDamageClass::Bleed: return 0.30f;
    default: return 0.0f;
    }
}

float UHealthComponent::GetRecoveryFractionForAction(const ERallyRecoveryAction RecoveryAction)
{
    switch (RecoveryAction)
    {
    case ERallyRecoveryAction::LightHit: return 0.15f;
    case ERallyRecoveryAction::HeavyHit: return 0.25f;
    case ERallyRecoveryAction::ParryStrike: return 0.40f;
    case ERallyRecoveryAction::Critical: return 1.0f;
    default: return 0.0f;
    }
}

void UHealthComponent::TickHealing(const float DeltaTime)
{
    if (PendingHeal <= 0.0f || HealTimeRemaining <= 0.0f) return;
    const float StepRatio = FMath::Clamp(DeltaTime / HealTimeRemaining, 0.0f, 1.0f);
    const float RequestedHeal = PendingHeal * StepRatio;
    const float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + RequestedHeal);
    const float AppliedHeal = CurrentHealth - PreviousHealth;
    PendingHeal = FMath::Max(0.0f, PendingHeal - RequestedHeal);
    HealTimeRemaining = FMath::Max(0.0f, HealTimeRemaining - DeltaTime);
    ClampRallyToMissingHealth();
    if (AppliedHeal > 0.0f) OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, AppliedHeal);
    if (CurrentHealth >= MaxHealth || PendingHeal <= KINDA_SMALL_NUMBER ||
        HealTimeRemaining <= 0.0f)
    {
        PendingHeal = 0.0f;
        HealTimeRemaining = 0.0f;
    }
}

void UHealthComponent::ClampRallyToMissingHealth()
{
    RallyAvailableHealth = FMath::Clamp(
        RallyAvailableHealth, 0.0f, FMath::Max(0.0f, MaxHealth - CurrentHealth));
    if (RallyAvailableHealth <= KINDA_SMALL_NUMBER)
    {
        RallyAvailableHealth = 0.0f;
        RallyWindowRemaining = 0.0f;
    }
}

void UHealthComponent::RefreshTickState()
{
    SetComponentTickEnabled(
        !bIsDead && (PendingHeal > 0.0f || RallyAvailableHealth > 0.0f));
}
