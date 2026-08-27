// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraStateComponent.generated.h"

class UWoundStateComponent;

UENUM(BlueprintType)
enum class EPlayerCameraMode : uint8
{
    Free,
    Anchored,
    Authored
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnPlayerCameraModeChanged, EPlayerCameraMode, PreviousMode, EPlayerCameraMode, NewMode);

/** Owns camera-control policy and deliberately has no kill-cam or impact-shake path. */
UCLASS(ClassGroup = (DarkArisen), meta = (BlueprintSpawnableComponent))
class DARKARISEN_API UCameraStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCameraStateComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Camera|Mode") void EnterAnchored(float DurationSeconds);
    UFUNCTION(BlueprintCallable, Category = "Camera|Mode") void EnterDeathHold();
    UFUNCTION(BlueprintCallable, Category = "Camera|Mode") void EnterEmergenceAnchor();
    UFUNCTION(BlueprintCallable, Category = "Camera|Mode") void EnterAuthored();
    UFUNCTION(BlueprintCallable, Category = "Camera|Mode") void ReleaseToFree();

    UFUNCTION(BlueprintPure, Category = "Camera|Input")
    bool AllowsLookInput() const { return CurrentMode == EPlayerCameraMode::Free; }

    UFUNCTION(BlueprintPure, Category = "Camera|Input")
    bool AllowsMoveInput() const { return CurrentMode != EPlayerCameraMode::Authored; }

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Camera|Mode")
    EPlayerCameraMode CurrentMode = EPlayerCameraMode::Free;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Camera|Mode")
    float AnchoredTimeRemaining = 0.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Camera|Wounds")
    float WoundInstabilityAlpha = 0.0f;

    UPROPERTY(BlueprintAssignable, Category = "Camera|Events")
    FOnPlayerCameraModeChanged OnCameraModeChanged;

private:
    UPROPERTY()
    TObjectPtr<UWoundStateComponent> CachedWoundState;
    void SetMode(EPlayerCameraMode NewMode);
};
