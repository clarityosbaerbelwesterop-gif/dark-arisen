// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputCoreTypes.h"
#include "JakeCharacter.generated.h"

class UCameraComponent;
class UCombatComponent;
class UHealthComponent;
class USpringArmComponent;
class UStaminaComponent;

/** M0 player pawn: weighted third-person locomotion, gamepad input, and native two-zone touch. */
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

    void MoveForward(float Value);
    void MoveRight(float Value);
    void TurnAtRate(float Rate);
    void LookUpAtRate(float Rate);
    void StartSprint();
    void StopSprint();
    void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
    void TouchMoved(ETouchIndex::Type FingerIndex, FVector Location);
    void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

    UFUNCTION()
    void OnCharacterDied(AActor* DamageCauser);

    UFUNCTION()
    void OnStaminaDepleted();
};
