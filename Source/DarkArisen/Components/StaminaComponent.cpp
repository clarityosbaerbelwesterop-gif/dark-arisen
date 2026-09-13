// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Components/StaminaComponent.h"

UStaminaComponent::UStaminaComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UStaminaComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
    bDepletionBroadcast = CurrentStamina <= 0.0f;
}

void UStaminaComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bSprinting && CurrentStamina > 0.0f)
    {
        ConsumeStamina(SprintDrainPerSecond * DeltaTime);
        return;
    }

    RegenLockTimer = FMath::Max(0.0f, RegenLockTimer - DeltaTime);
    if (RegenLockTimer > 0.0f || CurrentStamina >= MaxStamina)
    {
        return;
    }

    const float PreviousStamina = CurrentStamina;
    CurrentStamina = FMath::Min(
        MaxStamina,
        CurrentStamina + BaseRegenRate * FMath::Max(0.0f, RegenMultiplier) * DeltaTime);

    if (!FMath::IsNearlyEqual(PreviousStamina, CurrentStamina))
    {
        bDepletionBroadcast = false;
        OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
    }
}

bool UStaminaComponent::CanAfford(const float Cost) const
{
    return Cost >= 0.0f && CurrentStamina >= Cost;
}

bool UStaminaComponent::ConsumeStamina(const float Cost)
{
    if (Cost <= 0.0f)
    {
        return true;
    }

    if (!CanAfford(Cost))
    {
        return false;
    }

    CurrentStamina = FMath::Max(0.0f, CurrentStamina - Cost);
    RegenLockTimer = CurrentStamina <= 0.0f ? ExhaustionRegenDelay : RegenDelay;
    OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);

    if (CurrentStamina <= 0.0f && !bDepletionBroadcast)
    {
        bDepletionBroadcast = true;
        OnStaminaDepleted.Broadcast();
    }

    return true;
}

void UStaminaComponent::RestoreFromPerfectParry(const float Amount)
{
    if (Amount <= 0.0f)
    {
        return;
    }

    CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + Amount);
    RegenLockTimer = 0.0f;
    bDepletionBroadcast = false;
    OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void UStaminaComponent::SetSprinting(const bool bSprintActive)
{
    bSprinting = bSprintActive && CurrentStamina > 0.0f;
}

void UStaminaComponent::ResetForRespawn(const float StaminaFraction)
{
    bSprinting = false;
    RegenLockTimer = 0.0f;
    bDepletionBroadcast = false;
    CurrentStamina = MaxStamina * FMath::Clamp(StaminaFraction, 0.0f, 1.0f);
    OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void UStaminaComponent::SetRegenMultiplier(const float Multiplier)
{
    RegenMultiplier = FMath::Max(0.0f, Multiplier);
}
