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

    static EWoundLayer EvaluateLayer(float HealthFraction, float StaminaFraction);

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Wounds")
    EWoundLayer CurrentLayer = EWoundLayer::None;

    UPROPERTY(BlueprintAssignable, Category = "Wounds|Events")
    FOnWoundLayerChanged OnWoundLayerChanged;

private:
    UPROPERTY()
    TObjectPtr<UHealthComponent> CachedHealth;

    UPROPERTY()
    TObjectPtr<UStaminaComponent> CachedStamina;

    UFUNCTION()
    void HandleHealthChanged(float NewHealth, float MaximumHealth, float Delta);

    UFUNCTION()
    void HandleStaminaChanged(float NewStamina, float MaximumStamina);

    void RefreshLayer();
};
