// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "JakeCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "Components/HealthComponent.h"
#include "Components/InputComponent.h"
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
    Movement->MaxWalkSpeed = 420.0f;
    Movement->BrakingDecelerationWalking = 1400.0f;

    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
    StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

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
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
    PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AJakeCharacter::TurnAtRate);
    PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AJakeCharacter::LookUpAtRate);

    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &AJakeCharacter::StartSprint);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &AJakeCharacter::StopSprint);

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
    return AppliedDamage;
}

void AJakeCharacter::MoveForward(const float Value)
{
    if (!Controller || FMath::IsNearlyZero(Value) || HealthComponent->IsDead())
    {
        return;
    }

    const FRotator ControlRotation = Controller->GetControlRotation();
    const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);
    AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Value);
}

void AJakeCharacter::MoveRight(const float Value)
{
    if (!Controller || FMath::IsNearlyZero(Value) || HealthComponent->IsDead())
    {
        return;
    }

    const FRotator ControlRotation = Controller->GetControlRotation();
    const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);
    AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Value);
}

void AJakeCharacter::TurnAtRate(const float Rate)
{
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AJakeCharacter::LookUpAtRate(const float Rate)
{
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AJakeCharacter::StartSprint()
{
    if (!HealthComponent->IsDead() && StaminaComponent->CurrentStamina > 0.0f)
    {
        GetCharacterMovement()->MaxWalkSpeed = 620.0f;
        StaminaComponent->SetSprinting(true);
    }
}

void AJakeCharacter::StopSprint()
{
    GetCharacterMovement()->MaxWalkSpeed = 420.0f;
    StaminaComponent->SetSprinting(false);
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

    if (State.bMovementZone)
    {
        MovementTouch = FingerIndex;
    }
}

void AJakeCharacter::TouchMoved(const ETouchIndex::Type FingerIndex, const FVector Location)
{
    FTouchState* State = ActiveTouches.Find(FingerIndex);
    if (!State)
    {
        return;
    }

    const FVector2D Current(Location.X, Location.Y);
    if (State->bMovementZone)
    {
        constexpr float VirtualStickRadiusPixels = 120.0f;
        const FVector2D Delta = (Current - State->Start) / VirtualStickRadiusPixels;
        TouchMovement.X = FMath::Clamp(-Delta.Y, -1.0f, 1.0f);
        TouchMovement.Y = FMath::Clamp(Delta.X, -1.0f, 1.0f);
    }
    else
    {
        const FVector2D Delta = Current - State->Last;
        AddControllerYawInput(Delta.X * TouchLookSensitivity);
        AddControllerPitchInput(-Delta.Y * TouchLookSensitivity);
    }

    State->Last = Current;
}

void AJakeCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
    if (MovementTouch.IsSet() && MovementTouch.GetValue() == FingerIndex)
    {
        MovementTouch.Reset();
        TouchMovement = FVector2D::ZeroVector;
    }

    ActiveTouches.Remove(FingerIndex);
}

void AJakeCharacter::OnCharacterDied(AActor* DamageCauser)
{
    StopSprint();
    GetCharacterMovement()->DisableMovement();
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        DisableInput(PlayerController);
    }
}

void AJakeCharacter::OnStaminaDepleted()
{
    StopSprint();
}
