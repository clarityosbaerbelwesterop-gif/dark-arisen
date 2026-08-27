// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WaterBreathComponent.generated.h"

class UHealthComponent;

UENUM(BlueprintType)
enum class EWaterBreathTier : uint8
{
    Bare,
    Trained,
    Master,
    Equipment
};

/** Native breath and drowning state. It deliberately exposes no meter or warning event. */
UCLASS(ClassGroup = (DarkArisen), meta = (BlueprintSpawnableComponent))
class DARKARISEN_API UWaterBreathComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWaterBreathComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Water|Breath")
    void SetHeadSubmerged(bool bSubmerged);

    UFUNCTION(BlueprintCallable, Category = "Water|Breath")
    void SetBreathTier(EWaterBreathTier NewTier);

    UFUNCTION(BlueprintPure, Category = "Water|Breath")
    float GetRemainingBreathFraction() const;

    UFUNCTION(BlueprintPure, Category = "Water|Breath")
    bool HasIndefiniteBreath() const { return BreathTier == EWaterBreathTier::Equipment; }

    static float GetMaximumBreathSecondsForTier(EWaterBreathTier Tier);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame, Category = "Water|Breath")
    EWaterBreathTier BreathTier = EWaterBreathTier::Bare;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Water|Breath")
    float RemainingBreathSeconds = 30.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Water|Breath")
    bool bHeadSubmerged = false;

    /** DESIGN-GAP: the water document locks breath times, not drowning damage cadence. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Water|Tuning",
        meta = (ClampMin = "0.0"))
    float DrowningDamagePerSecond = 20.0f;

private:
    UPROPERTY()
    TObjectPtr<UHealthComponent> CachedHealth;

    float DrowningSeconds = 0.0f;
    void ResetBreath();
};
