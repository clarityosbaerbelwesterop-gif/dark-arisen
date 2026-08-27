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
class UInteractionComponent;
class ULockOnComponent;
class USpringArmComponent;
class UStaminaComponent;

/** M1 player pawn: weighted locomotion, committed combat, wounds, camera policy, and touch. */
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

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jake|Camera")
    TObjectPtr<UCameraComponent> FollowCamera;

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
