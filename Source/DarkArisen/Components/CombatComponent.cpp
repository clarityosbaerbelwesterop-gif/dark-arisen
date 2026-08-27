// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Components/CombatComponent.h"

#include "Components/StaminaComponent.h"
#include "CoreLoopTuning.h"
#include "DesignLaws.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

namespace
{
    constexpr float LightAttackStaminaCost = 15.0f;
    constexpr float HeavyAttackStaminaCost = 30.0f;
    constexpr float ParryStaminaCost = 10.0f;
    constexpr float DodgeStaminaCost = 25.0f;
    constexpr float BackstepStaminaCost = 10.0f;
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
    CurrentPosture = FMath::Clamp(CurrentPosture, 0.0f, MaxPosture);
    RefreshPostureVisualState();
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
    }
    if (ActionCommitmentRemaining > 0.0f)
    {
        ActionCommitmentRemaining = FMath::Max(0.0f, ActionCommitmentRemaining - DeltaTime);
        if (ActionCommitmentRemaining <= 0.0f)
        {
            if (CurrentState == ECombatState::Staggered) CompleteStagger();
            else FinishAction();
        }
    }
    if (CurrentState == ECombatState::Idle && CurrentPosture > 0.0f)
    {
        PostureRegenDelayRemaining = FMath::Max(
            0.0f, PostureRegenDelayRemaining - DeltaTime);
        if (PostureRegenDelayRemaining <= 0.0f)
        {
            CurrentPosture = FMath::Max(
                0.0f,
                CurrentPosture - DarkArisen::CoreLoopTuning::PostureRegenPerSecond * DeltaTime);
            RefreshPostureVisualState();
        }
    }
    if (!bRacheActive) return;

    const float TimeScale = UGameplayStatics::GetGlobalTimeDilation(this);
    const float RealDeltaTime = DeltaTime / FMath::Max(TimeScale, KINDA_SMALL_NUMBER);
    RacheRealSecondsRemaining = FMath::Max(0.0f, RacheRealSecondsRemaining - RealDeltaTime);
    CurrentRache = FMath::Max(0.0f, CurrentRache - RacheDrainPerRealSecond * RealDeltaTime);
    OnRacheMeterChanged.Broadcast(CurrentRache, MaxRache);
    if (RacheRealSecondsRemaining <= 0.0f || CurrentRache <= 0.0f) StopRache();
}

void UCombatComponent::AddPostureDamage(const float Amount)
{
    if (Amount <= 0.0f || CurrentState == ECombatState::Dead ||
        CurrentState == ECombatState::Staggered) return;
    CurrentPosture = FMath::Clamp(
        CurrentPosture + Amount * FMath::Max(0.0f, PoiseMultiplier), 0.0f, MaxPosture);
    PostureRegenDelayRemaining = DarkArisen::CoreLoopTuning::PostureRegenDelaySeconds;
    RefreshPostureVisualState();
    if (PostureVisualState == EPostureVisualState::Broken)
    {
        DeflectionWindowRemaining = 0.0f;
        ActionCommitmentRemaining =
            DarkArisen::CoreLoopTuning::PostureBreakVulnerabilitySeconds;
        SetState(ECombatState::Staggered);
        OnPostureBroken.Broadcast();
    }
}

bool UCombatComponent::PerformLightAttack()
{
    return BeginCommittedAction(
        ECombatState::LightAttacking, LightAttackStaminaCost, GetMinimumCommitmentSeconds());
}

bool UCombatComponent::PerformHeavyAttack()
{
    return BeginCommittedAction(
        ECombatState::HeavyAttacking, HeavyAttackStaminaCost, GetMinimumCommitmentSeconds());
}

bool UCombatComponent::PerformParry()
{
    if (!BeginCommittedAction(
        ECombatState::Parrying,
        ParryStaminaCost,
        DarkArisen::DesignLaws::DeflectionWindowSeconds)) return false;
    DeflectionWindowRemaining = DarkArisen::DesignLaws::DeflectionWindowSeconds;
    return true;
}

bool UCombatComponent::PerformDodge(const FVector& Direction)
{
    if (Direction.ContainsNaN()) return false;
    const bool bBackstep = Direction.IsNearlyZero();
    return BeginCommittedAction(
        bBackstep ? ECombatState::Backstepping : ECombatState::Dodging,
        bBackstep ? BackstepStaminaCost : DodgeStaminaCost,
        DarkArisen::CoreLoopTuning::FramesToSeconds(GetRecoveryFrames()));
}

void UCombatComponent::FinishAction()
{
    if (CurrentState == ECombatState::Dead || ActionCommitmentRemaining > 0.0f ||
        DeflectionWindowRemaining > 0.0f) return;
    SetState(ECombatState::Idle);
}

void UCombatComponent::SetDead()
{
    StopRache();
    DeflectionWindowRemaining = 0.0f;
    ActionCommitmentRemaining = 0.0f;
    SetState(ECombatState::Dead);
}

void UCombatComponent::EquipWeapon(const EWeaponSlot Slot)
{
    if (CurrentState != ECombatState::Idle || Slot == EWeaponSlot::None) return;
    if (Slot == EWeaponSlot::Cutlass || Slot == EWeaponSlot::CrystalKatana)
    {
        CurrentMelee = Slot;
        CurrentWeightClass = Slot == EWeaponSlot::CrystalKatana
            ? EWeaponWeightClass::Light : EWeaponWeightClass::Medium;
    }
    else CurrentRanged = Slot;
    OnWeaponChanged.Broadcast(Slot);
}

void UCombatComponent::SetWeaponWeightClass(const EWeaponWeightClass WeightClass)
{
    if (CurrentState == ECombatState::Idle) CurrentWeightClass = WeightClass;
}

bool UCombatComponent::StartRache()
{
    if (!bRacheUnlocked || bRacheActive || CurrentRache <= 0.0f || !GetWorld()) return false;
    bRacheActive = true;
    RacheRealSecondsRemaining = FMath::Min(
        DarkArisen::DesignLaws::RacheMaximumRealSeconds,
        CurrentRache / RacheDrainPerRealSecond);
    UGameplayStatics::SetGlobalTimeDilation(this, DarkArisen::DesignLaws::RacheTimeScale);
    return true;
}

void UCombatComponent::StopRache()
{
    if (!bRacheActive) return;
    bRacheActive = false;
    RacheRealSecondsRemaining = 0.0f;
    if (GetWorld()) UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
}

void UCombatComponent::AddRacheFuel(const float Percent)
{
    if (Percent <= 0.0f) return;
    CurrentRache = FMath::Clamp(CurrentRache + MaxRache * Percent / 100.0f, 0.0f, MaxRache);
    OnRacheMeterChanged.Broadcast(CurrentRache, MaxRache);
}

int32 UCombatComponent::GetStartupFrames() const
{
    using namespace DarkArisen::CoreLoopTuning;
    switch (CurrentWeightClass)
    {
    case EWeaponWeightClass::Light: return LightStartupFrames;
    case EWeaponWeightClass::Heavy: return HeavyStartupFrames;
    case EWeaponWeightClass::Great: return GreatStartupFrames;
    case EWeaponWeightClass::Polearm: return PolearmStartupFrames;
    default: return MediumStartupFrames;
    }
}

int32 UCombatComponent::GetRecoveryFrames() const
{
    using namespace DarkArisen::CoreLoopTuning;
    switch (CurrentWeightClass)
    {
    case EWeaponWeightClass::Light: return LightRecoveryFrames;
    case EWeaponWeightClass::Heavy: return HeavyRecoveryFrames;
    case EWeaponWeightClass::Great: return GreatRecoveryFrames;
    case EWeaponWeightClass::Polearm: return PolearmRecoveryFrames;
    default: return MediumRecoveryFrames;
    }
}

float UCombatComponent::GetMinimumCommitmentSeconds() const
{
    return DarkArisen::CoreLoopTuning::FramesToSeconds(GetStartupFrames() + GetRecoveryFrames());
}

float UCombatComponent::GetPostureRemainingFraction() const
{
    return MaxPosture > 0.0f
        ? FMath::Clamp(1.0f - CurrentPosture / MaxPosture, 0.0f, 1.0f) : 0.0f;
}

EPostureVisualState UCombatComponent::EvaluatePostureVisualState(const float RemainingFraction)
{
    using namespace DarkArisen::CoreLoopTuning;
    const float Remaining = FMath::Clamp(RemainingFraction, 0.0f, 1.0f);
    if (Remaining >= PostureSetThreshold) return EPostureVisualState::Set;
    if (Remaining >= PosturePressedThreshold) return EPostureVisualState::Pressed;
    if (Remaining >= PostureFailingThreshold) return EPostureVisualState::Failing;
    if (Remaining >= PostureBrokenImminentThreshold) return EPostureVisualState::BrokenImminent;
    return EPostureVisualState::Broken;
}

bool UCombatComponent::BeginCommittedAction(
    const ECombatState NewState,
    const float StaminaCost,
    const float DurationSeconds)
{
    if (CurrentState != ECombatState::Idle || DurationSeconds <= 0.0f || !CachedStamina ||
        !CachedStamina->ConsumeStamina(StaminaCost)) return false;
    ActionCommitmentRemaining = DurationSeconds;
    SetState(NewState);
    return true;
}

void UCombatComponent::CompleteStagger()
{
    CurrentPosture = MaxPosture * (1.0f - DarkArisen::CoreLoopTuning::PostureResetFraction);
    PostureRegenDelayRemaining = DarkArisen::CoreLoopTuning::PostureRegenDelaySeconds;
    RefreshPostureVisualState();
    SetState(ECombatState::Idle);
}

void UCombatComponent::RefreshPostureVisualState()
{
    const EPostureVisualState NewState =
        EvaluatePostureVisualState(GetPostureRemainingFraction());
    if (NewState == PostureVisualState) return;
    const EPostureVisualState PreviousState = PostureVisualState;
    PostureVisualState = NewState;
    OnPostureVisualStateChanged.Broadcast(PreviousState, PostureVisualState);
}

void UCombatComponent::SetState(const ECombatState NewState)
{
    if (CurrentState == NewState) return;
    CurrentState = NewState;
    OnStateChanged.Broadcast(CurrentState);
}
