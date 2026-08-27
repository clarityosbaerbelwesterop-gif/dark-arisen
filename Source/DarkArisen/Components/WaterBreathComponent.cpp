// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Components/WaterBreathComponent.h"

#include "Components/HealthComponent.h"
#include "GameFramework/Actor.h"

UWaterBreathComponent::UWaterBreathComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UWaterBreathComponent::BeginPlay()
{
    Super::BeginPlay();
    CachedHealth = GetOwner() ? GetOwner()->FindComponentByClass<UHealthComponent>() : nullptr;
    ResetBreath();
}

void UWaterBreathComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!bHeadSubmerged || HasIndefiniteBreath() || DeltaTime <= 0.0f ||
        (CachedHealth && CachedHealth->IsDead())) return;

    RemainingBreathSeconds = FMath::Max(0.0f, RemainingBreathSeconds - DeltaTime);
    if (RemainingBreathSeconds > 0.0f)
    {
        DrowningSeconds = 0.0f;
        return;
    }

    DrowningSeconds += DeltaTime;
    const int32 WholeSeconds = FMath::FloorToInt(DrowningSeconds);
    if (WholeSeconds <= 0 || !CachedHealth) return;
    DrowningSeconds -= static_cast<float>(WholeSeconds);
    CachedHealth->ApplyDamageWithRally(
        DrowningDamagePerSecond * static_cast<float>(WholeSeconds),
        GetOwner(),
        ERallyDamageClass::Environmental);
}

void UWaterBreathComponent::SetHeadSubmerged(const bool bSubmerged)
{
    if (bHeadSubmerged == bSubmerged) return;
    bHeadSubmerged = bSubmerged;
    DrowningSeconds = 0.0f;
    if (!bHeadSubmerged) ResetBreath();
}

void UWaterBreathComponent::SetBreathTier(const EWaterBreathTier NewTier)
{
    BreathTier = NewTier;
    ResetBreath();
}

float UWaterBreathComponent::GetRemainingBreathFraction() const
{
    if (HasIndefiniteBreath()) return 1.0f;
    const float Maximum = GetMaximumBreathSecondsForTier(BreathTier);
    return Maximum > 0.0f ? FMath::Clamp(RemainingBreathSeconds / Maximum, 0.0f, 1.0f) : 0.0f;
}

float UWaterBreathComponent::GetMaximumBreathSecondsForTier(const EWaterBreathTier Tier)
{
    switch (Tier)
    {
    case EWaterBreathTier::Bare: return 30.0f;
    case EWaterBreathTier::Trained: return 60.0f;
    case EWaterBreathTier::Master: return 90.0f;
    case EWaterBreathTier::Equipment: return 0.0f;
    default: return 30.0f;
    }
}

void UWaterBreathComponent::ResetBreath()
{
    RemainingBreathSeconds = GetMaximumBreathSecondsForTier(BreathTier);
    DrowningSeconds = 0.0f;
}
