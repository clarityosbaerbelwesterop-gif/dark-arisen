// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Components/CombatComponent.h"
#include "Components/WoundStateComponent.h"
#include "DarkArisenAnimInstance.generated.h"

class AJakeCharacter;

/** Native read-only bridge from gameplay state into authored animation graphs. */
UCLASS(Transient, Blueprintable)
class DARKARISEN_API UDarkArisenAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Animation|Locomotion")
    float GroundSpeedCentimetresPerSecond = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Animation|Locomotion")
    bool bIsInAir = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Animation|Locomotion")
    bool bIsAccelerating = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Animation|Combat")
    ECombatState CombatState = ECombatState::Idle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Animation|Combat")
    EWeaponWeightClass WeaponWeightClass = EWeaponWeightClass::Medium;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Animation|Combat")
    EPostureVisualState PostureVisualState = EPostureVisualState::Set;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Animation|Wounds")
    EWoundLayer WoundLayer = EWoundLayer::None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Animation|Wounds")
    FWoundPresentationProfile WoundPresentation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Animation|Wounds")
    bool bUseWoundedAnimationSet = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Animation|Wounds")
    bool bKatanaEquipped = false;

private:
    UPROPERTY(Transient)
    TObjectPtr<AJakeCharacter> CachedJake;
};
