// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HeatExposureComponent.generated.h"

class UHealthComponent;
class UStaminaComponent;

/** Authored arena volumes select one of these states; the component creates no HUD. */
UENUM(BlueprintType)
enum class EHeatExposureZone : uint8
{
    Inactive,
    CoolCistern,
    ShadeGallery,
    ExposedHeat
};

/**
 * Qualitative M2 heat pressure for the Fuerte San Rafael proof encounter.
 * Exact rates are editable DESIGN-GAP values until an in-engine tuning pass locks them.
 */
UCLASS(ClassGroup = (DarkArisen), meta = (BlueprintSpawnableComponent))
class DARKARISEN_API UHeatExposureComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHeatExposureComponent();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    /** Called by authored cistern, gallery and exposed-heat volumes. */
    UFUNCTION(BlueprintCallable, Category = "Heat")
    void SetExposureZone(EHeatExposureZone NewZone);

    UFUNCTION(BlueprintPure, Category = "Heat")
    float GetHeatStress() const { return CurrentHeatStress; }

    UFUNCTION(BlueprintPure, Category = "Heat")
    bool IsHeatSimulationActive() const
    {
        return CurrentExposureZone != EHeatExposureZone::Inactive;
    }

    static float EvaluateNextHeatStress(
        float CurrentStress,
        EHeatExposureZone Zone,
        float DeltaSeconds,
        float ShadeAccumulationRate,
        float ExposedAccumulationRate,
        float CoolingRate);

    static float EvaluateStaminaRegenMultiplier(
        float HeatStress,
        float MinimumMultiplier);

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category = "Heat",
        meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float CurrentHeatStress = 0.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Heat")
    EHeatExposureZone CurrentExposureZone = EHeatExposureZone::Inactive;

    /** DESIGN-GAP: qualitative source says shade still accumulates heat; rate is unlocked. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heat|Tuning",
        meta = (ClampMin = "0.0"))
    float ShadeAccumulationPerSecond = 0.01f;

    /** DESIGN-GAP: exposed spaces build pressure faster; rate is unlocked. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heat|Tuning",
        meta = (ClampMin = "0.0"))
    float ExposedAccumulationPerSecond = 0.035f;

    /** DESIGN-GAP: cisterns are the authored recovery rhythm; rate is unlocked. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heat|Tuning",
        meta = (ClampMin = "0.0"))
    float CisternRecoveryPerSecond = 0.1f;

    /** DESIGN-GAP: heat must impair stamina before damage, but the floor is unlocked. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heat|Tuning",
        meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MinimumStaminaRegenMultiplier = 0.35f;

    /** DESIGN-GAP: damage begins only at severe stress; threshold is unlocked. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heat|Tuning",
        meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float EnvironmentalDamageThreshold = 0.85f;

    /** DESIGN-GAP: environmental damage rate is unlocked. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heat|Tuning",
        meta = (ClampMin = "0.0"))
    float EnvironmentalDamagePerSecond = 5.0f;

private:
    UPROPERTY()
    TObjectPtr<UHealthComponent> CachedHealth;

    UPROPERTY()
    TObjectPtr<UStaminaComponent> CachedStamina;

    float SevereHeatSeconds = 0.0f;
    void ApplyStaminaPressure() const;
};
