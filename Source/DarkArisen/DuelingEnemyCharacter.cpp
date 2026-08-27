// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "DuelingEnemyCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "Components/HealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/StaminaComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ADuelingEnemyCharacter::ADuelingEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

    UCharacterMovementComponent* Movement = GetCharacterMovement();
    Movement->bOrientRotationToMovement = true;
    Movement->RotationRate = FRotator(0.0f, 420.0f, 0.0f);
    Movement->MaxWalkSpeed = 280.0f;
    Movement->BrakingDecelerationWalking = 1600.0f;

    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
    HealthComponent->MaxHealth = 180.0f;
    HealthComponent->CurrentHealth = 180.0f;
    StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
    CombatComponent->MaxPosture = 100.0f;

    GreyboxBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GreyboxBody"));
    GreyboxBody->SetupAttachment(GetCapsuleComponent());
    GreyboxBody->SetRelativeLocation(FVector(0.0f, 0.0f, -8.0f));
    GreyboxBody->SetRelativeScale3D(FVector(0.72f, 0.72f, 1.75f));
    GreyboxBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (BodyMesh.Succeeded()) GreyboxBody->SetStaticMesh(BodyMesh.Object);
}

void ADuelingEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();
    HealthComponent->OnDied.AddDynamic(this, &ADuelingEnemyCharacter::OnEnemyDied);
    TargetCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
}

void ADuelingEnemyCharacter::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (HealthComponent->IsDead()) return;
    AttackCooldownRemaining = FMath::Max(0.0f, AttackCooldownRemaining - DeltaSeconds);
    if (!IsValid(TargetCharacter)) TargetCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (!IsValid(TargetCharacter)) return;

    const FVector Offset = TargetCharacter->GetActorLocation() - GetActorLocation();
    const float Distance = Offset.Size2D();
    if (Distance > AwarenessRangeCentimetres) return;

    if (Distance > AttackRangeCentimetres)
    {
        if (CombatComponent->CurrentState == ECombatState::Idle)
            AddMovementInput(Offset.GetSafeNormal2D());
        return;
    }

    const FRotator Facing = Offset.Rotation();
    SetActorRotation(FRotator(0.0f, Facing.Yaw, 0.0f));

    if (AttackCooldownRemaining <= 0.0f && CombatComponent->PerformLightAttack())
        AttackCooldownRemaining = AttackCooldownSeconds;
}

void ADuelingEnemyCharacter::OnEnemyDied(AActor* /*DamageCauser*/)
{
    CombatComponent->SetDead();
    GetCharacterMovement()->DisableMovement();
    SetLifeSpan(4.0f);
}
