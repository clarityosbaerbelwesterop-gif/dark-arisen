// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WoundStateComponent.h"
#include "GameFramework/Character.h"
#include "InputCoreTypes.h"
#include "JakeCharacter.generated.h"

class UCameraComponent;
class UCameraStateComponent;
class UCombatComponent;
class UHealthComponent;
class UHeatExposureComponent;
class UInteractionComponent;
class ULockOnComponent;
class UAnimMontage;
class UProgressionEconomyComponent;
class UQuestJournalComponent;
class USpringArmComponent;
class UStaminaComponent;
class UWaterBreathComponent;

/** M1 player pawn: weighted locomotion, committed combat, wounds, camera policy, touch, and persistent authored state. */
UCLASS()
class DARKARISEN_API AJakeCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AJakeCharacter();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

    /** Restores a dead Jake at a resolved authored checkpoint anchor. */
    UFUNCTION(BlueprintCallable, Category = "Jake|Persistence")
    bool RestoreAtCheckpoint(const FTransform& SpawnTransform);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Components")
    TObjectPtr<UHealthComponent> HealthComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Components")
    TObjectPtr<UStaminaComponent> StaminaComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Components")
    TObjectPtr<UCombatComponent> CombatComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Components")
    TObjectPtr<UWoundStateComponent> WoundStateComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Components")
    TObjectPtr<UCameraStateComponent> CameraStateComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Components")
    TObjectPtr<UInteractionComponent> InteractionComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Components")
    TObjectPtr<ULockOnComponent> LockOnComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Components")
    TObjectPtr<UQuestJournalComponent> QuestJournalComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Components")
    TObjectPtr<UHeatExposureComponent> HeatExposureComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Components")
    TObjectPtr<UWaterBreathComponent> WaterBreathComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Components")
    TObjectPtr<UProgressionEconomyComponent> ProgressionEconomyComponent;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Camera")
    TObjectPtr<UCameraComponent> FollowCamera;

    /** Authored assets are assigned in Jake's Blueprint; native state remains authoritative. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jake|Animation")
    TObjectPtr<UAnimMontage> LightAttackMontage;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jake|Animation")
    TObjectPtr<UAnimMontage> HeavyAttackMontage;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jake|Animation")
    TObjectPtr<UAnimMontage> ParryMontage;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jake|Animation")
    TObjectPtr<UAnimMontage> DodgeMontage;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jake|Animation")
    TObjectPtr<UAnimMontage> BackstepMontage;

private:
    struct FTouchState
    {
        FVector2D Start = FVector2D::ZeroVector;
        FVector2D Last = FVector2D::ZeroVector;
        bool bMovementZone = false;
    };

    TMap<ETouchIndex::Type, FTouchState> ActiveTouches;
    FVector2D TouchMovement = FVector2D::ZeroVector;
    TOptional<ETouchIndex::Type> MovementTouch;

    float BaseTurnRate = 45.0f;
    float BaseLookUpRate = 45.0f;
    float TouchLookSensitivity = 0.12f;
    float RunSpeedCentimetresPerSecond = 330.0f;
    float SprintSpeedCentimetresPerSecond = 600.0f;
    FVector BaseCameraSocketOffset = FVector(0.0f, 45.0f, 70.0f);
    float WoundCameraPhase = 0.0f;

    /** DESIGN-GAP: amplitude is a tuning lever in camera_system.md with no locked value. */
    UPROPERTY(EditDefaultsOnly, Category = "Jake|Camera", meta = (ClampMin = "0.0"))
    float MaximumWoundCameraDriftCentimetres = 3.0f;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void TurnAtRate(float Rate);
    void LookUpAtRate(float Rate);
    void StartSprint();
    void StopSprint();
    void StartJump();
    void StopJump();
    void PerformLightAttack();
    void PerformHeavyAttack();
    void PerformParry();
    void PerformDodge();
    void TryInteract();
    void ToggleLockOn();
    void TryActivateRache();
    bool TryPlayActionMontage(UAnimMontage* Montage);
    void UpdateWoundPresentation(float DeltaSeconds);
    void ApplyWoundLocomotion();
    void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
    void TouchMoved(ETouchIndex::Type FingerIndex, FVector Location);
    void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

    UFUNCTION()
    void OnCharacterDied(AActor* DamageCauser);
    UFUNCTION()
    void OnStaminaDepleted();
    UFUNCTION()
    void OnWoundLayerChanged(EWoundLayer PreviousLayer, EWoundLayer NewLayer);
};
