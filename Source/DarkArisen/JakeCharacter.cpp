// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "JakeCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CameraStateComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "Components/HealthComponent.h"
#include "Components/InputComponent.h"
#include "Components/InteractionComponent.h"
#include "Components/StaminaComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "UnrealClient.h"

AJakeCharacter::AJakeCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    UCharacterMovementComponent* Movement = GetCharacterMovement();
    Movement->bOrientRotationToMovement = true;
    Movement->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
    Movement->JumpZVelocity = 520.0f;
    Movement->AirControl = 0.25f;
    Movement->MaxWalkSpeed = RunSpeedCentimetresPerSecond;
    Movement->BrakingDecelerationWalking = 1400.0f;

    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
    StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
    WoundStateComponent = CreateDefaultSubobject<UWoundStateComponent>(TEXT("WoundStateComponent"));
    CameraStateComponent = CreateDefaultSubobject<UCameraStateComponent>(TEXT("CameraStateComponent"));
    InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 360.0f;
    CameraBoom->SocketOffset = FVector(0.0f, 45.0f, 70.0f);
    CameraBoom->bUsePawnControlRotation = true;
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
}

void AJakeCharacter::BeginPlay()
{
    Super::BeginPlay();
    HealthComponent->OnDied.AddDynamic(this, &AJakeCharacter::OnCharacterDied);
    StaminaComponent->OnStaminaDepleted.AddDynamic(this, &AJakeCharacter::OnStaminaDepleted);
    WoundStateComponent->OnWoundLayerChanged.AddDynamic(
        this, &AJakeCharacter::OnWoundLayerChanged);
}

void AJakeCharacter::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!TouchMovement.IsNearlyZero())
    {
        MoveForward(TouchMovement.X);
        MoveRight(TouchMovement.Y);
    }
}

void AJakeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    check(PlayerInputComponent);
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AJakeCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AJakeCharacter::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AJakeCharacter::Turn);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AJakeCharacter::LookUp);
    PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AJakeCharacter::TurnAtRate);
    PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AJakeCharacter::LookUpAtRate);
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AJakeCharacter::StartJump);
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &AJakeCharacter::StopJump);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &AJakeCharacter::StartSprint);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &AJakeCharacter::StopSprint);
    PlayerInputComponent->BindAction(TEXT("LightAttack"), IE_Pressed, this, &AJakeCharacter::PerformLightAttack);
    PlayerInputComponent->BindAction(TEXT("HeavyAttack"), IE_Pressed, this, &AJakeCharacter::PerformHeavyAttack);
    PlayerInputComponent->BindAction(TEXT("Parry"), IE_Pressed, this, &AJakeCharacter::PerformParry);
    PlayerInputComponent->BindAction(TEXT("Dodge"), IE_Pressed, this, &AJakeCharacter::PerformDodge);
    PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AJakeCharacter::TryInteract);
    PlayerInputComponent->BindTouch(IE_Pressed, this, &AJakeCharacter::TouchStarted);
    PlayerInputComponent->BindTouch(IE_Repeat, this, &AJakeCharacter::TouchMoved);
    PlayerInputComponent->BindTouch(IE_Released, this, &AJakeCharacter::TouchStopped);
}

float AJakeCharacter::TakeDamage(
    const float DamageAmount,
    const FDamageEvent& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    const float AppliedDamage = Super::TakeDamage(
        DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    HealthComponent->ApplyDamage(AppliedDamage, DamageCauser);
    if (AppliedDamage > 0.0f) InteractionComponent->CancelActiveInteraction();
    return AppliedDamage;
}

void AJakeCharacter::MoveForward(const float Value)
{
    if (!Controller || FMath::IsNearlyZero(Value) || HealthComponent->IsDead() ||
        !CameraStateComponent->AllowsMoveInput()) return;
    InteractionComponent->CancelActiveInteraction();
    const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
    AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Value);
}

void AJakeCharacter::MoveRight(const float Value)
{
    if (!Controller || FMath::IsNearlyZero(Value) || HealthComponent->IsDead() ||
        !CameraStateComponent->AllowsMoveInput()) return;
    InteractionComponent->CancelActiveInteraction();
    const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
    AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Value);
}

void AJakeCharacter::Turn(const float Value)
{
    if (CameraStateComponent->AllowsLookInput()) AddControllerYawInput(Value);
}

void AJakeCharacter::LookUp(const float Value)
{
    if (CameraStateComponent->AllowsLookInput()) AddControllerPitchInput(Value);
}

void AJakeCharacter::TurnAtRate(const float Rate)
{
    if (CameraStateComponent->AllowsLookInput())
        AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AJakeCharacter::LookUpAtRate(const float Rate)
{
    if (CameraStateComponent->AllowsLookInput())
        AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AJakeCharacter::StartSprint()
{
    if (!HealthComponent->IsDead() && StaminaComponent->CurrentStamina > 0.0f &&
        WoundStateComponent->IsSprintAllowed() &&
        CombatComponent->CurrentState == ECombatState::Idle &&
        !InteractionComponent->IsInteracting())
    {
        GetCharacterMovement()->MaxWalkSpeed = SprintSpeedCentimetresPerSecond;
        StaminaComponent->SetSprinting(true);
    }
}

void AJakeCharacter::StopSprint()
{
    GetCharacterMovement()->MaxWalkSpeed = RunSpeedCentimetresPerSecond;
    StaminaComponent->SetSprinting(false);
}

void AJakeCharacter::StartJump()
{
    constexpr float JumpStaminaCost = 12.0f;
    if (!HealthComponent->IsDead() && CameraStateComponent->AllowsMoveInput() &&
        CombatComponent->CurrentState == ECombatState::Idle &&
        !InteractionComponent->IsInteracting() &&
        StaminaComponent->ConsumeStamina(JumpStaminaCost)) Jump();
}

void AJakeCharacter::StopJump()
{
    StopJumping();
}

void AJakeCharacter::PerformLightAttack()
{
    if (!HealthComponent->IsDead() && !InteractionComponent->IsInteracting())
        CombatComponent->PerformLightAttack();
}

void AJakeCharacter::PerformHeavyAttack()
{
    if (!HealthComponent->IsDead() && !InteractionComponent->IsInteracting())
        CombatComponent->PerformHeavyAttack();
}

void AJakeCharacter::PerformParry()
{
    if (!HealthComponent->IsDead() && !InteractionComponent->IsInteracting())
        CombatComponent->PerformParry();
}

void AJakeCharacter::PerformDodge()
{
    if (HealthComponent->IsDead() || InteractionComponent->IsInteracting()) return;
    CombatComponent->PerformDodge(GetLastMovementInputVector().GetSafeNormal());
}

void AJakeCharacter::TryInteract()
{
    if (!HealthComponent->IsDead() && CombatComponent->CurrentState == ECombatState::Idle &&
        CameraStateComponent->CurrentMode != EPlayerCameraMode::Authored)
        InteractionComponent->TryBeginInteraction();
}

void AJakeCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
    FVector2D ViewportSize(1920.0f, 1080.0f);
    if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
    {
        const FIntPoint Size = GEngine->GameViewport->Viewport->GetSizeXY();
        ViewportSize = FVector2D(static_cast<float>(Size.X), static_cast<float>(Size.Y));
    }
    FTouchState State;
    State.Start = FVector2D(Location.X, Location.Y);
    State.Last = State.Start;
    State.bMovementZone = Location.X < ViewportSize.X * 0.5f && !MovementTouch.IsSet();
    ActiveTouches.Add(FingerIndex, State);
    if (State.bMovementZone) MovementTouch = FingerIndex;
}

void AJakeCharacter::TouchMoved(const ETouchIndex::Type FingerIndex, const FVector Location)
{
    FTouchState* State = ActiveTouches.Find(FingerIndex);
    if (!State) return;
    const FVector2D Current(Location.X, Location.Y);
    if (State->bMovementZone)
    {
        constexpr float VirtualStickRadiusPixels = 120.0f;
        const FVector2D Delta = (Current - State->Start) / VirtualStickRadiusPixels;
        TouchMovement.X = FMath::Clamp(-Delta.Y, -1.0f, 1.0f);
        TouchMovement.Y = FMath::Clamp(Delta.X, -1.0f, 1.0f);
    }
    else if (CameraStateComponent->AllowsLookInput())
    {
        const FVector2D Delta = Current - State->Last;
        AddControllerYawInput(Delta.X * TouchLookSensitivity);
        AddControllerPitchInput(-Delta.Y * TouchLookSensitivity);
    }
    State->Last = Current;
}

void AJakeCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector /*Location*/)
{
    if (MovementTouch.IsSet() && MovementTouch.GetValue() == FingerIndex)
    {
        MovementTouch.Reset();
        TouchMovement = FVector2D::ZeroVector;
    }
    ActiveTouches.Remove(FingerIndex);
}

void AJakeCharacter::OnCharacterDied(AActor* /*DamageCauser*/)
{
    StopSprint();
    InteractionComponent->CancelActiveInteraction();
    CombatComponent->SetDead();
    GetCharacterMovement()->DisableMovement();
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
        DisableInput(PlayerController);
}

void AJakeCharacter::OnStaminaDepleted()
{
    StopSprint();
}

void AJakeCharacter::OnWoundLayerChanged(
    const EWoundLayer /*PreviousLayer*/,
    const EWoundLayer /*NewLayer*/)
{
    if (!WoundStateComponent->IsSprintAllowed()) StopSprint();
}
