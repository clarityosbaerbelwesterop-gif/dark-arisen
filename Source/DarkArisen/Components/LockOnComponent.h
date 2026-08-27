// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLockTargetChanged, AActor*, NewTarget);

/** Target-facing camera aid. It deliberately creates no marker, outline, widget or HUD state. */
UCLASS(ClassGroup = (DarkArisen), meta = (BlueprintSpawnableComponent))
class DARKARISEN_API ULockOnComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    ULockOnComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "LockOn")
    bool ToggleLockOn();

    UFUNCTION(BlueprintCallable, Category = "LockOn")
    bool AcquireBestTarget();

    UFUNCTION(BlueprintCallable, Category = "LockOn")
    void ReleaseTarget();

    UFUNCTION(BlueprintPure, Category = "LockOn")
    bool IsLockedOn() const { return IsValid(CurrentTarget); }

    UFUNCTION(BlueprintPure, Category = "LockOn")
    AActor* GetCurrentTarget() const { return CurrentTarget; }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LockOn", meta = (ClampMin = "100.0"))
    float MaximumAcquisitionRangeCentimetres = 2000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LockOn", meta = (ClampMin = "100.0"))
    float MaximumRetentionRangeCentimetres = 2500.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LockOn", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MinimumFacingDot = 0.35f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LockOn", meta = (ClampMin = "1.0"))
    float RotationSpeed = 12.0f;

    UPROPERTY(BlueprintAssignable, Category = "LockOn|Events")
    FOnLockTargetChanged OnLockTargetChanged;

private:
    UPROPERTY()
    TObjectPtr<AActor> CurrentTarget;

    bool IsEligibleTarget(AActor* Candidate, bool bRequireFacing) const;
    void ApplyFacingPolicy(bool bLocked);
};
