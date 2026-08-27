// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Components/HeatExposureComponent.h"

#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "GameFramework/Actor.h"

UHeatExposureComponent::UHeatExposureComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UHeatExposureComponent::BeginPlay()
{
    Super::BeginPlay();
    CachedHealth = GetOwner() ? GetOwner()->FindComponentByClass<UHealthComponent>() : nullptr;
    CachedStamina = GetOwner() ? GetOwner()->FindComponentByClass<UStaminaComponent>() : nullptr;
    CurrentHeatStress = FMath::Clamp(CurrentHeatStress, 0.0f, 1.0f);
    ApplyStaminaPressure();
}

void UHeatExposureComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (CachedStamina) CachedStamina->SetRegenMultiplier(1.0f);
    Super::EndPlay(EndPlayReason);
}

void UHeatExposureComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (DeltaTime <= 0.0f) return;

    CurrentHeatStress = EvaluateNextHeatStress(
        CurrentHeatStress,
        CurrentExposureZone,
        DeltaTime,
        ShadeAccumulationPerSecond,
        ExposedAccumulationPerSecond,
        CisternRecoveryPerSecond);
    ApplyStaminaPressure();

    const bool bSevereActiveHeat = IsHeatSimulationActive() && CachedHealth &&
        !CachedHealth->IsDead() && CurrentHeatStress >= EnvironmentalDamageThreshold;
    if (!bSevereActiveHeat)
    {
        SevereHeatSeconds = 0.0f;
        return;
    }

    SevereHeatSeconds += DeltaTime;
    const int32 WholeSeconds = FMath::FloorToInt(SevereHeatSeconds);
    if (WholeSeconds <= 0) return;
    SevereHeatSeconds -= static_cast<float>(WholeSeconds);
    CachedHealth->ApplyDamageWithRally(
        EnvironmentalDamagePerSecond * static_cast<float>(WholeSeconds),
        GetOwner(),
        ERallyDamageClass::Environmental);
}

void UHeatExposureComponent::SetExposureZone(const EHeatExposureZone NewZone)
{
    CurrentExposureZone = NewZone;
    SevereHeatSeconds = 0.0f;
    ApplyStaminaPressure();
}

float UHeatExposureComponent::EvaluateNextHeatStress(
    const float CurrentStress,
    const EHeatExposureZone Zone,
    const float DeltaSeconds,
    const float ShadeAccumulationRate,
    const float ExposedAccumulationRate,
    const float CoolingRate)
{
    const float SafeDelta = FMath::Max(0.0f, DeltaSeconds);
    float Rate = 0.0f;
    switch (Zone)
    {
    case EHeatExposureZone::ShadeGallery:
        Rate = FMath::Max(0.0f, ShadeAccumulationRate);
        break;
    case EHeatExposureZone::ExposedHeat:
        Rate = FMath::Max(0.0f, ExposedAccumulationRate);
        break;
    case EHeatExposureZone::CoolCistern:
    case EHeatExposureZone::Inactive:
        Rate = -FMath::Max(0.0f, CoolingRate);
        break;
    default:
        break;
    }
    return FMath::Clamp(CurrentStress + Rate * SafeDelta, 0.0f, 1.0f);
}

float UHeatExposureComponent::EvaluateStaminaRegenMultiplier(
    const float HeatStress,
    const float MinimumMultiplier)
{
    return FMath::Lerp(
        1.0f,
        FMath::Clamp(MinimumMultiplier, 0.0f, 1.0f),
        FMath::Clamp(HeatStress, 0.0f, 1.0f));
}

void UHeatExposureComponent::ApplyStaminaPressure() const
{
    if (!CachedStamina) return;
    const float Multiplier = IsHeatSimulationActive()
        ? EvaluateStaminaRegenMultiplier(CurrentHeatStress, MinimumStaminaRegenMultiplier)
        : 1.0f;
    CachedStamina->SetRegenMultiplier(Multiplier);
}
