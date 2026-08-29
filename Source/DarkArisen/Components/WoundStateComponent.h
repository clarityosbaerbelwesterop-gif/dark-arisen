// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WoundStateComponent.generated.h"

class UHealthComponent;
class UStaminaComponent;

UENUM(BlueprintType)
enum class EWoundLayer : uint8
{
    None,
    Winded,
    Hurt,
    Bad,
    Failing
};

USTRUCT(BlueprintType)
struct FWoundPresentationProfile
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    float MovementSpeedScale = 1.0f;
    UPROPERTY(BlueprintReadOnly)
    float CameraInstabilityAlpha = 0.0f;
    UPROPERTY(BlueprintReadOnly)
    bool bAudibleBreathing = false;
    UPROPERTY(BlueprintReadOnly)
    bool bFavoursSide = false;
    UPROPERTY(BlueprintReadOnly)
    bool bLimp = false;
    UPROPERTY(BlueprintReadOnly)
    bool bStaggerRun = false;
    UPROPERTY(BlueprintReadOnly)
    bool bWeaponDrag = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnWoundLayerChanged, EWoundLayer, PreviousLayer, EWoundLayer, NewLayer);

/** Converts health and stamina into the authored deterioration layers. */
UCLASS(ClassGroup = (DarkArisen), meta = (BlueprintSpawnableComponent))
class DARKARISEN_API UWoundStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWoundStateComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION(BlueprintPure, Category = "Wounds")
    EWoundLayer GetWoundLayer() const { return CurrentLayer; }

    UFUNCTION(BlueprintPure, Category = "Wounds")
    bool IsSprintAllowed() const { return CurrentLayer != EWoundLayer::Failing; }

    UFUNCTION(BlueprintPure, Category = "Wounds|Animation")
    bool ShouldUseWoundedAnimationSet(bool bKatanaEquipped) const;

    UFUNCTION(BlueprintPure, Category = "Wounds|Camera")
    float GetCameraInstabilityAlpha() const;

    UFUNCTION(BlueprintPure, Category = "Wounds|Presentation")
    FWoundPresentationProfile GetPresentationProfile() const;

    static EWoundLayer EvaluateLayer(float HealthFraction, float StaminaFraction);
    static FWoundPresentationProfile GetPresentationProfileForLayer(EWoundLayer Layer);

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Wounds")
    EWoundLayer CurrentLayer = EWoundLayer::None;

    UPROPERTY(BlueprintAssignable, Category = "Wounds|Events")
    FOnWoundLayerChanged OnWoundLayerChanged;

    /** DESIGN-GAP: animation_system.md requires a visible Winded layer but does not lock a movement-speed multiplier. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wounds|Tuning", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float WindedMovementSpeedScale = 0.97f;

    /** DESIGN-GAP: animation_system.md requires a visible Hurt layer but does not lock a movement-speed multiplier. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wounds|Tuning", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float HurtMovementSpeedScale = 0.92f;

    /** DESIGN-GAP: animation_system.md requires a limp/stagger-run at Bad but does not lock a movement-speed multiplier. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wounds|Tuning", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float BadMovementSpeedScale = 0.80f;

    /** DESIGN-GAP: animation_system.md forbids sprint and requires weapon drag at Failing but does not lock a walk-speed multiplier. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wounds|Tuning", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float FailingMovementSpeedScale = 0.65f;

    /** DESIGN-GAP: camera_system.md requires instability to key only from deterioration but does not lock per-layer amplitude. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wounds|Tuning", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float WindedCameraInstabilityAlpha = 0.25f;

    /** DESIGN-GAP: camera_system.md requires monotonic deterioration-driven instability but does not lock the Hurt amplitude. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wounds|Tuning", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float HurtCameraInstabilityAlpha = 0.50f;

    /** DESIGN-GAP: camera_system.md requires monotonic deterioration-driven instability but does not lock the Bad amplitude. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wounds|Tuning", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float BadCameraInstabilityAlpha = 0.75f;

    /** DESIGN-GAP: 1.0 is a normalized drive convention, not a locked physical camera amplitude; JakeCharacter owns the separately tunable drift magnitude. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wounds|Tuning", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float FailingCameraInstabilityAlpha = 1.0f;

private:
    UPROPERTY()
    TObjectPtr<UHealthComponent> CachedHealth;

    UPROPERTY()
    TObjectPtr<UStaminaComponent> CachedStamina;

    UFUNCTION()
    void HandleHealthChanged(float NewHealth, float MaximumHealth, float Delta);

    UFUNCTION()
    void HandleStaminaChanged(float NewStamina, float MaximumStamina);

    FWoundPresentationProfile BuildPresentationProfileForLayer(EWoundLayer Layer) const;
    void RefreshLayer();
};
