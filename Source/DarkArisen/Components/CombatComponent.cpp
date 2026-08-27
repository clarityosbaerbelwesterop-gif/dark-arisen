// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Components/CombatComponent.h"

#include "Components/StaminaComponent.h"
#include "DesignLaws.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

namespace
{
    constexpr float LightAttackStaminaCost = 15.0f;
    constexpr float HeavyAttackStaminaCost = 30.0f;
    constexpr float ParryStaminaCost = 10.0f;
    constexpr float DodgeStaminaCost = 25.0f;
    constexpr float PostureRecoveryPerSecond = 12.0f;
    constexpr float RacheDrainPerRealSecond = 20.0f;
}

UCombatComponent::UCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::BeginPlay()
{
    Super::BeginPlay();
    CachedStamina = GetOwner() ? GetOwner()->FindComponentByClass<UStaminaComponent>() : nullptr;
}

void UCombatComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopRache();
    Super::EndPlay(EndPlayReason);
}

void UCombatComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (DeflectionWindowRemaining > 0.0f)
    {
        DeflectionWindowRemaining = FMath::Max(0.0f, DeflectionWindowRemaining - DeltaTime);
        if (DeflectionWindowRemaining <= 0.0f && CurrentState == ECombatState::Parrying)
        {
            FinishAction();
        }
    }

    if (CurrentState == ECombatState::Idle && CurrentPosture > 0.0f)
    {
        CurrentPosture = FMath::Max(0.0f, CurrentPosture - PostureRecoveryPerSecond * DeltaTime);
    }

    if (!bRacheActive)
    {
        return;
    }

    const float TimeScale = UGameplayStatics::GetGlobalTimeDilation(this);
    const float RealDeltaTime = DeltaTime / FMath::Max(TimeScale, KINDA_SMALL_NUMBER);
    RacheRealSecondsRemaining = FMath::Max(0.0f, RacheRealSecondsRemaining - RealDeltaTime);
    CurrentRache = FMath::Max(0.0f, CurrentRache - RacheDrainPerRealSecond * RealDeltaTime);
    OnRacheMeterChanged.Broadcast(CurrentRache, MaxRache);

    if (RacheRealSecondsRemaining <= 0.0f || CurrentRache <= 0.0f)
    {
        StopRache();
    }
}

void UCombatComponent::AddPostureDamage(const float Amount)
{
    if (Amount <= 0.0f || CurrentState == ECombatState::Dead)
    {
        return;
    }

    CurrentPosture = FMath::Clamp(
        CurrentPosture + Amount * FMath::Max(0.0f, PoiseMultiplier),
        0.0f,
        MaxPosture);

    if (CurrentPosture >= MaxPosture)
    {
        SetState(ECombatState::Staggered);
        OnPostureBroken.Broadcast();
    }
}

bool UCombatComponent::PerformLightAttack()
{
    if (CurrentState != ECombatState::Idle || !CachedStamina ||
        !CachedStamina->ConsumeStamina(LightAttackStaminaCost))
    {
        return false;
    }

    SetState(ECombatState::LightAttacking);
    return true;
}

bool UCombatComponent::PerformHeavyAttack()
{
    if (CurrentState != ECombatState::Idle || !CachedStamina ||
        !CachedStamina->ConsumeStamina(HeavyAttackStaminaCost))
    {
        return false;
    }

    SetState(ECombatState::HeavyAttacking);
    return true;
}

bool UCombatComponent::PerformParry()
{
    if (CurrentState != ECombatState::Idle || !CachedStamina ||
        !CachedStamina->ConsumeStamina(ParryStaminaCost))
    {
        return false;
    }

    SetState(ECombatState::Parrying);
    DeflectionWindowRemaining = DarkArisen::DesignLaws::DeflectionWindowSeconds;
    return true;
}

bool UCombatComponent::PerformDodge(const FVector& Direction)
{
    if (CurrentState != ECombatState::Idle || Direction.ContainsNaN() || !CachedStamina ||
        !CachedStamina->ConsumeStamina(DodgeStaminaCost))
    {
        return false;
    }

    SetState(ECombatState::Dodging);
    return true;
}

void UCombatComponent::FinishAction()
{
    if (CurrentState != ECombatState::Dead)
    {
        DeflectionWindowRemaining = 0.0f;
        SetState(ECombatState::Idle);
    }
}

void UCombatComponent::EquipWeapon(const EWeaponSlot Slot)
{
    if (CurrentState != ECombatState::Idle || Slot == EWeaponSlot::None)
    {
        return;
    }

    if (Slot == EWeaponSlot::Cutlass)
    {
        CurrentMelee = Slot;
    }
    else
    {
        CurrentRanged = Slot;
    }

    OnWeaponChanged.Broadcast(Slot);
}

bool UCombatComponent::StartRache()
{
    if (!bRacheUnlocked || bRacheActive || CurrentRache <= 0.0f || !GetWorld())
    {
        return false;
    }

    bRacheActive = true;
    RacheRealSecondsRemaining = FMath::Min(
        DarkArisen::DesignLaws::RacheMaximumRealSeconds,
        CurrentRache / RacheDrainPerRealSecond);
    UGameplayStatics::SetGlobalTimeDilation(this, DarkArisen::DesignLaws::RacheTimeScale);
    return true;
}

void UCombatComponent::StopRache()
{
    if (!bRacheActive)
    {
        return;
    }

    bRacheActive = false;
    RacheRealSecondsRemaining = 0.0f;
    if (GetWorld())
    {
        UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
    }
}

void UCombatComponent::AddRacheFuel(const float Percent)
{
    if (Percent <= 0.0f)
    {
        return;
    }

    CurrentRache = FMath::Clamp(CurrentRache + MaxRache * Percent / 100.0f, 0.0f, MaxRache);
    OnRacheMeterChanged.Broadcast(CurrentRache, MaxRache);
}

void UCombatComponent::SetState(const ECombatState NewState)
{
    if (CurrentState == NewState)
    {
        return;
    }

    CurrentState = NewState;
    OnStateChanged.Broadcast(CurrentState);
}
