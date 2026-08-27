// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Components/CameraStateComponent.h"

#include "Components/WoundStateComponent.h"
#include "CoreLoopTuning.h"

UCameraStateComponent::UCameraStateComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCameraStateComponent::BeginPlay()
{
    Super::BeginPlay();
    CachedWoundState = GetOwner()
        ? GetOwner()->FindComponentByClass<UWoundStateComponent>()
        : nullptr;
}

void UCameraStateComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    WoundInstabilityAlpha = CachedWoundState
        ? CachedWoundState->GetCameraInstabilityAlpha()
        : 0.0f;
    if (CurrentMode != EPlayerCameraMode::Anchored || AnchoredTimeRemaining < 0.0f) return;
    AnchoredTimeRemaining = FMath::Max(0.0f, AnchoredTimeRemaining - DeltaTime);
    if (AnchoredTimeRemaining <= 0.0f) ReleaseToFree();
}

void UCameraStateComponent::EnterAnchoredUntilReleased()
{
    AnchoredTimeRemaining = -1.0f;
    SetMode(EPlayerCameraMode::Anchored);
}

void UCameraStateComponent::EnterAnchored(const float DurationSeconds)
{
    if (DurationSeconds <= 0.0f)
    {
        ReleaseToFree();
        return;
    }
    AnchoredTimeRemaining = DurationSeconds;
    SetMode(EPlayerCameraMode::Anchored);
}

void UCameraStateComponent::EnterDeathHold()
{
    EnterAnchored(DarkArisen::CoreLoopTuning::DeathHoldSeconds);
}

void UCameraStateComponent::EnterEmergenceAnchor()
{
    EnterAnchored(DarkArisen::CoreLoopTuning::EmergenceAnchorSeconds);
}

void UCameraStateComponent::EnterAuthored()
{
    AnchoredTimeRemaining = 0.0f;
    SetMode(EPlayerCameraMode::Authored);
}

void UCameraStateComponent::ReleaseToFree()
{
    AnchoredTimeRemaining = 0.0f;
    SetMode(EPlayerCameraMode::Free);
}

void UCameraStateComponent::SetMode(const EPlayerCameraMode NewMode)
{
    if (CurrentMode == NewMode) return;
    const EPlayerCameraMode PreviousMode = CurrentMode;
    CurrentMode = NewMode;
    OnCameraModeChanged.Broadcast(PreviousMode, CurrentMode);
}
