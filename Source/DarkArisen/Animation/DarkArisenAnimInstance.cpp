// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Animation/DarkArisenAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "JakeCharacter.h"

void UDarkArisenAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    CachedJake = Cast<AJakeCharacter>(TryGetPawnOwner());
}

void UDarkArisenAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (!IsValid(CachedJake))
    {
        CachedJake = Cast<AJakeCharacter>(TryGetPawnOwner());
    }
    if (!IsValid(CachedJake)) return;

    GroundSpeedCentimetresPerSecond = CachedJake->GetVelocity().Size2D();
    if (const UCharacterMovementComponent* Movement = CachedJake->GetCharacterMovement())
    {
        bIsInAir = Movement->IsFalling();
        bIsAccelerating = !Movement->GetCurrentAcceleration().IsNearlyZero();
    }

    if (const UCombatComponent* Combat = CachedJake->CombatComponent)
    {
        CombatState = Combat->CurrentState;
        WeaponWeightClass = Combat->CurrentWeightClass;
        PostureVisualState = Combat->PostureVisualState;
        bKatanaEquipped = Combat->IsKatanaEquipped();
    }
    if (const UWoundStateComponent* Wounds = CachedJake->WoundStateComponent)
    {
        WoundLayer = Wounds->GetWoundLayer();
        WoundPresentation = Wounds->GetPresentationProfile();
        bUseWoundedAnimationSet = Wounds->ShouldUseWoundedAnimationSet(bKatanaEquipped);
    }
}
