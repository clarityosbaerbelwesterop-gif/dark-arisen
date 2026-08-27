// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Components/LockOnComponent.h"

#include "Components/CameraStateComponent.h"
#include "Components/CombatComponent.h"
#include "Components/HealthComponent.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

ULockOnComponent::ULockOnComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void ULockOnComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!IsLockedOn()) return;
    if (!IsEligibleTarget(CurrentTarget, false))
    {
        ReleaseTarget();
        return;
    }

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter || !OwnerCharacter->GetController())
    {
        ReleaseTarget();
        return;
    }
    const UCameraStateComponent* CameraState =
        OwnerCharacter->FindComponentByClass<UCameraStateComponent>();
    if (CameraState && CameraState->CurrentMode != EPlayerCameraMode::Free) return;

    const FVector AimPoint = CurrentTarget->GetActorLocation() + FVector(0.0f, 0.0f, 55.0f);
    const FRotator Desired = (AimPoint - OwnerCharacter->GetActorLocation()).Rotation();
    const FRotator Current = OwnerCharacter->GetController()->GetControlRotation();
    const FRotator Smoothed = FMath::RInterpTo(Current, Desired, DeltaTime, RotationSpeed);
    OwnerCharacter->GetController()->SetControlRotation(
        FRotator(Smoothed.Pitch, Smoothed.Yaw, 0.0f));
}

bool ULockOnComponent::ToggleLockOn()
{
    if (IsLockedOn())
    {
        ReleaseTarget();
        return false;
    }
    return AcquireBestTarget();
}

bool ULockOnComponent::AcquireBestTarget()
{
    AActor* Owner = GetOwner();
    UWorld* World = GetWorld();
    if (!Owner || !World) return false;

    AActor* BestTarget = nullptr;
    float BestScore = TNumericLimits<float>::Max();
    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* Candidate = *It;
        if (!IsEligibleTarget(Candidate, true)) continue;
        const FVector Offset = Candidate->GetActorLocation() - Owner->GetActorLocation();
        const float DistanceScore = Offset.SizeSquared();
        const float FacingPenalty =
            (1.0f - FVector::DotProduct(Owner->GetActorForwardVector(), Offset.GetSafeNormal())) *
            FMath::Square(MaximumRangeCentimetres);
        const float Score = DistanceScore + FacingPenalty;
        if (Score < BestScore)
        {
            BestScore = Score;
            BestTarget = Candidate;
        }
    }
    if (!BestTarget) return false;

    CurrentTarget = BestTarget;
    ApplyFacingPolicy(true);
    OnLockTargetChanged.Broadcast(CurrentTarget);
    return true;
}

void ULockOnComponent::ReleaseTarget()
{
    if (!CurrentTarget) return;
    CurrentTarget = nullptr;
    ApplyFacingPolicy(false);
    OnLockTargetChanged.Broadcast(nullptr);
}

bool ULockOnComponent::IsEligibleTarget(AActor* Candidate, const bool bRequireFacing) const
{
    const AActor* Owner = GetOwner();
    const UWorld* World = GetWorld();
    if (!Owner || !World || !IsValid(Candidate) || Candidate == Owner) return false;
    const UHealthComponent* Health = Candidate->FindComponentByClass<UHealthComponent>();
    const UCombatComponent* Combat = Candidate->FindComponentByClass<UCombatComponent>();
    if (!Health || !Combat || Health->IsDead() || Combat->CurrentState == ECombatState::Dead)
        return false;

    const FVector Offset = Candidate->GetActorLocation() - Owner->GetActorLocation();
    if (Offset.SizeSquared() > FMath::Square(MaximumRangeCentimetres)) return false;
    if (bRequireFacing && FVector::DotProduct(
        Owner->GetActorForwardVector(), Offset.GetSafeNormal()) < MinimumFacingDot) return false;

    FHitResult SightHit;
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(DarkArisenLockOnSight), false, Owner);
    QueryParams.AddIgnoredActor(Owner);
    const bool bBlocked = World->LineTraceSingleByChannel(
        SightHit,
        Owner->GetActorLocation() + FVector(0.0f, 0.0f, 55.0f),
        Candidate->GetActorLocation() + FVector(0.0f, 0.0f, 55.0f),
        ECC_Visibility,
        QueryParams);
    return !bBlocked || SightHit.GetActor() == Candidate;
}

void ULockOnComponent::ApplyFacingPolicy(const bool bLocked)
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter) return;
    OwnerCharacter->bUseControllerRotationYaw = bLocked;
    if (UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement())
        Movement->bOrientRotationToMovement = !bLocked;
}
