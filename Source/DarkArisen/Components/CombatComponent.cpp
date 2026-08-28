// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Components/CombatComponent.h"

#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "CoreLoopTuning.h"
#include "DesignLaws.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "World/CombatProximitySubsystem.h"

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
    CachedHealth = GetOwner() ? GetOwner()->FindComponentByClass<UHealthComponent>() : nullptr;
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
    if (bHasPendingHit && bPendingHitUsesFrameFallback)
    {
        PendingHitDelayRemaining = FMath::Max(0.0f, PendingHitDelayRemaining - DeltaTime);
        if (PendingHitDelayRemaining <= 0.0f)
        {
            TraceAndResolvePendingHit();
            ClearQueuedMeleeHit();
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
    if (Amount <= 0.0f || !IsCombatTargetable() ||
        CurrentState == ECombatState::Staggered) return;
    CurrentPosture = FMath::Clamp(
        CurrentPosture + Amount * FMath::Max(0.0f, PoiseMultiplier), 0.0f, MaxPosture);
    PostureRegenDelayRemaining = DarkArisen::CoreLoopTuning::PostureRegenDelaySeconds;
    RefreshPostureVisualState();
    if (PostureVisualState == EPostureVisualState::Broken)
    {
        DeflectionWindowRemaining = 0.0f;
        ClearQueuedMeleeHit();
        ActionCommitmentRemaining =
            DarkArisen::CoreLoopTuning::PostureBreakVulnerabilitySeconds;
        SetState(ECombatState::Staggered);
        OnPostureBroken.Broadcast();
    }
}

bool UCombatComponent::PerformLightAttack()
{
    if (!BeginCommittedAction(
        ECombatState::LightAttacking, LightAttackStaminaCost, GetMinimumCommitmentSeconds()))
        return false;
    SignalCombatActivity();
    QueueMeleeHit(ECombatHitKind::Light);
    return true;
}

bool UCombatComponent::PerformHeavyAttack()
{
    if (!BeginCommittedAction(
        ECombatState::HeavyAttacking, HeavyAttackStaminaCost, GetMinimumCommitmentSeconds()))
        return false;
    SignalCombatActivity();
    QueueMeleeHit(ECombatHitKind::Heavy);
    return true;
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
    if (!IsCombatTargetable() || ActionCommitmentRemaining > 0.0f ||
        DeflectionWindowRemaining > 0.0f) return;
    ClearQueuedMeleeHit();
    SetState(ECombatState::Idle);
}

void UCombatComponent::SetDead()
{
    StopRache();
    DeflectionWindowRemaining = 0.0f;
    ActionCommitmentRemaining = 0.0f;
    ClearQueuedMeleeHit();
    SetState(ECombatState::Dead);
}

void UCombatComponent::SetNonHostile()
{
    if (CurrentState == ECombatState::Dead) return;
    StopRache();
    DeflectionWindowRemaining = 0.0f;
    ActionCommitmentRemaining = 0.0f;
    ClearQueuedMeleeHit();
    SetState(ECombatState::NonHostile);
}

bool UCombatComponent::ResolveHitAgainst(AActor* Target, const ECombatHitKind HitKind)
{
    AActor* Owner = GetOwner();
    if (!IsValid(Owner) || !IsValid(Target) || Target == Owner ||
        !IsCombatTargetable()) return false;

    UHealthComponent* TargetHealth = Target->FindComponentByClass<UHealthComponent>();
    UCombatComponent* TargetCombat = Target->FindComponentByClass<UCombatComponent>();
    if (!TargetHealth || !TargetCombat || TargetHealth->IsDead() ||
        !TargetCombat->IsCombatTargetable()) return false;

    const FCombatHitProfile Profile = GetHitProfile(HitKind);
    if (TargetCombat->IsDeflectionWindowOpen() && HitKind != ECombatHitKind::Critical)
    {
        AddPostureDamage(Profile.DeflectedPostureDamage);
        if (UHealthComponent* DefenderHealth = Target->FindComponentByClass<UHealthComponent>())
            DefenderHealth->RecoverRally(ERallyRecoveryAction::ParryStrike);
        return true;
    }

    TargetHealth->ApplyDamageWithRally(
        Profile.HealthDamage,
        Owner,
        ERallyDamageClass::StandardEnemy);
    TargetCombat->AddPostureDamage(Profile.PostureDamage);

    if (CachedHealth)
    {
        ERallyRecoveryAction RecoveryAction = ERallyRecoveryAction::LightHit;
        switch (HitKind)
        {
        case ECombatHitKind::Heavy: RecoveryAction = ERallyRecoveryAction::HeavyHit; break;
        case ECombatHitKind::ParryStrike: RecoveryAction = ERallyRecoveryAction::ParryStrike; break;
        case ECombatHitKind::Critical: RecoveryAction = ERallyRecoveryAction::Critical; break;
        default: break;
        }
        CachedHealth->RecoverRally(RecoveryAction);
    }
    return true;
}

bool UCombatComponent::ResolveCriticalHit(AActor* Target)
{
    return ResolveHitAgainst(Target, ECombatHitKind::Critical);
}

bool UCombatComponent::ResolveQueuedMeleeHitFromAnimation(const ECombatHitKind HitKind)
{
    if (!bHasPendingHit || PendingHitKind != HitKind) return false;
    const bool bResolved = TraceAndResolvePendingHit();
    ClearQueuedMeleeHit();
    return bResolved;
}

void UCombatComponent::RouteQueuedMeleeHitToAnimationNotify()
{
    if (bHasPendingHit) bPendingHitUsesFrameFallback = false;
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

FCombatHitProfile UCombatComponent::GetHitProfile(const ECombatHitKind HitKind)
{
    switch (HitKind)
    {
    case ECombatHitKind::Light: return {24.0f, 18.0f, 32.0f};
    case ECombatHitKind::Heavy: return {42.0f, 34.0f, 48.0f};
    case ECombatHitKind::ParryStrike: return {36.0f, 45.0f, 0.0f};
    case ECombatHitKind::Critical: return {72.0f, 100.0f, 0.0f};
    default: return {};
    }
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

void UCombatComponent::QueueMeleeHit(const ECombatHitKind HitKind)
{
    PendingHitKind = HitKind;
    PendingHitDelayRemaining = DarkArisen::CoreLoopTuning::FramesToSeconds(GetStartupFrames());
    bHasPendingHit = true;
    bPendingHitUsesFrameFallback = true;
}

void UCombatComponent::ClearQueuedMeleeHit()
{
    bHasPendingHit = false;
    bPendingHitUsesFrameFallback = true;
    PendingHitDelayRemaining = 0.0f;
}

bool UCombatComponent::TraceAndResolvePendingHit()
{
    AActor* Owner = GetOwner();
    UWorld* World = GetWorld();
    if (!Owner || !World) return false;

    const FVector Forward = Owner->GetActorForwardVector().GetSafeNormal();
    const FVector Start = Owner->GetActorLocation() + Forward * MeleeTraceStartCentimetres;
    const FVector End = Owner->GetActorLocation() + Forward * MeleeTraceEndCentimetres;
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(DarkArisenMeleeHit), false, Owner);
    TArray<FHitResult> Hits;
    if (!World->SweepMultiByChannel(
        Hits,
        Start,
        End,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(MeleeTraceRadiusCentimetres),
        QueryParams)) return false;

    Hits.Sort([Owner](const FHitResult& Left, const FHitResult& Right)
    {
        return FVector::DistSquared(Owner->GetActorLocation(), Left.ImpactPoint) <
            FVector::DistSquared(Owner->GetActorLocation(), Right.ImpactPoint);
    });
    for (const FHitResult& Hit : Hits)
    {
        if (ResolveHitAgainst(Hit.GetActor(), PendingHitKind)) return true;
    }
    return false;
}

void UCombatComponent::CompleteStagger()
{
    CurrentPosture = MaxPosture * (1.0f - DarkArisen::CoreLoopTuning::PostureResetFraction);
    PostureRegenDelayRemaining = DarkArisen::CoreLoopTuning::PostureRegenDelaySeconds;
    RefreshPostureVisualState();
    SetState(ECombatState::Idle);
}

void UCombatComponent::SignalCombatActivity()
{
    AActor* Owner = GetOwner();
    UWorld* World = GetWorld();
    if (!Owner || !World) return;
    if (UCombatProximitySubsystem* Proximity =
        World->GetSubsystem<UCombatProximitySubsystem>())
        Proximity->BroadcastCombatActivity(Owner->GetActorLocation());
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
