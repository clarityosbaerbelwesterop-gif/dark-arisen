// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Components/WoundStateComponent.h"

#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "CoreLoopTuning.h"

UWoundStateComponent::UWoundStateComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UWoundStateComponent::BeginPlay()
{
    Super::BeginPlay();
    if (AActor* Owner = GetOwner())
    {
        CachedHealth = Owner->FindComponentByClass<UHealthComponent>();
        CachedStamina = Owner->FindComponentByClass<UStaminaComponent>();
    }
    if (CachedHealth)
    {
        CachedHealth->OnHealthChanged.AddDynamic(this, &UWoundStateComponent::HandleHealthChanged);
    }
    if (CachedStamina)
    {
        CachedStamina->OnStaminaChanged.AddDynamic(this, &UWoundStateComponent::HandleStaminaChanged);
    }
    RefreshLayer();
}

void UWoundStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (CachedHealth)
    {
        CachedHealth->OnHealthChanged.RemoveDynamic(this, &UWoundStateComponent::HandleHealthChanged);
    }
    if (CachedStamina)
    {
        CachedStamina->OnStaminaChanged.RemoveDynamic(this, &UWoundStateComponent::HandleStaminaChanged);
    }
    Super::EndPlay(EndPlayReason);
}

bool UWoundStateComponent::ShouldUseWoundedAnimationSet(const bool bKatanaEquipped) const
{
    return !bKatanaEquipped && CurrentLayer != EWoundLayer::None;
}

float UWoundStateComponent::GetCameraInstabilityAlpha() const
{
    return GetPresentationProfile().CameraInstabilityAlpha;
}

FWoundPresentationProfile UWoundStateComponent::GetPresentationProfile() const
{
    return GetPresentationProfileForLayer(CurrentLayer);
}

FWoundPresentationProfile UWoundStateComponent::GetPresentationProfileForLayer(
    const EWoundLayer Layer)
{
    FWoundPresentationProfile Profile;
    switch (Layer)
    {
    case EWoundLayer::Winded:
        Profile.MovementSpeedScale = 0.97f;
        Profile.CameraInstabilityAlpha = 0.25f;
        Profile.bAudibleBreathing = true;
        break;
    case EWoundLayer::Hurt:
        Profile.MovementSpeedScale = 0.92f;
        Profile.CameraInstabilityAlpha = 0.50f;
        Profile.bFavoursSide = true;
        break;
    case EWoundLayer::Bad:
        Profile.MovementSpeedScale = 0.80f;
        Profile.CameraInstabilityAlpha = 0.75f;
        Profile.bFavoursSide = true;
        Profile.bLimp = true;
        Profile.bStaggerRun = true;
        break;
    case EWoundLayer::Failing:
        Profile.MovementSpeedScale = 0.65f;
        Profile.CameraInstabilityAlpha = 1.0f;
        Profile.bFavoursSide = true;
        Profile.bLimp = true;
        Profile.bWeaponDrag = true;
        break;
    default:
        break;
    }
    return Profile;
}

EWoundLayer UWoundStateComponent::EvaluateLayer(
    const float HealthFraction,
    const float StaminaFraction)
{
    using namespace DarkArisen::CoreLoopTuning;
    const float Health = FMath::Clamp(HealthFraction, 0.0f, 1.0f);
    const float Stamina = FMath::Clamp(StaminaFraction, 0.0f, 1.0f);
    if (Health < FailingHealthThreshold) return EWoundLayer::Failing;
    if (Health < BadHealthThreshold) return EWoundLayer::Bad;
    if (Health < HurtHealthThreshold) return EWoundLayer::Hurt;
    if (Stamina < WindedStaminaThreshold) return EWoundLayer::Winded;
    return EWoundLayer::None;
}

void UWoundStateComponent::HandleHealthChanged(
    const float /*NewHealth*/,
    const float /*MaximumHealth*/,
    const float /*Delta*/)
{
    RefreshLayer();
}

void UWoundStateComponent::HandleStaminaChanged(
    const float /*NewStamina*/,
    const float /*MaximumStamina*/)
{
    RefreshLayer();
}

void UWoundStateComponent::RefreshLayer()
{
    const float HealthFraction = CachedHealth ? CachedHealth->GetHealthPercent() : 1.0f;
    const float StaminaFraction = CachedStamina && CachedStamina->MaxStamina > 0.0f
        ? CachedStamina->CurrentStamina / CachedStamina->MaxStamina
        : 1.0f;
    const EWoundLayer NewLayer = EvaluateLayer(HealthFraction, StaminaFraction);
    if (NewLayer == CurrentLayer) return;
    const EWoundLayer PreviousLayer = CurrentLayer;
    CurrentLayer = NewLayer;
    OnWoundLayerChanged.Broadcast(PreviousLayer, CurrentLayer);
}
