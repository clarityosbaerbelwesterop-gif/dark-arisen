// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Bosses/IsabelCruzCharacter.h"

#include "Components/CameraStateComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "Components/HealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/StaminaComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AIsabelCruzCharacter::AIsabelCruzCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

    UCharacterMovementComponent* Movement = GetCharacterMovement();
    Movement->bOrientRotationToMovement = true;
    Movement->RotationRate = FRotator(0.0f, 420.0f, 0.0f);
    Movement->MaxWalkSpeed = 290.0f;
    Movement->BrakingDecelerationWalking = 1600.0f;

    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
    // DESIGN-GAP: proof health and posture await controller tuning in the authored arena.
    HealthComponent->MaxHealth = 650.0f;
    HealthComponent->CurrentHealth = 650.0f;
    StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
    CombatComponent->MaxPosture = 150.0f;

    GreyboxBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GreyboxBody"));
    GreyboxBody->SetupAttachment(GetCapsuleComponent());
    GreyboxBody->SetRelativeLocation(FVector(0.0f, 0.0f, -8.0f));
    GreyboxBody->SetRelativeScale3D(FVector(0.78f, 0.78f, 1.8f));
    GreyboxBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMesh(
        TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (BodyMesh.Succeeded()) GreyboxBody->SetStaticMesh(BodyMesh.Object);
}

void AIsabelCruzCharacter::BeginPlay()
{
    Super::BeginPlay();
    HealthComponent->OnHealthChanged.AddDynamic(this, &AIsabelCruzCharacter::OnHealthChanged);
    HealthComponent->OnDied.AddDynamic(this, &AIsabelCruzCharacter::OnCruzDied);

    if (Resolution == EBossEncounterResolution::Killed || HealthComponent->IsDead())
    {
        Resolution = EBossEncounterResolution::Killed;
        SetPhase(EIsabelCruzPhase::Resolved);
        CombatComponent->SetDead();
        GetCharacterMovement()->DisableMovement();
        SetActorTickEnabled(false);
        return;
    }
    if (Resolution == EBossEncounterResolution::Spared ||
        Resolution == EBossEncounterResolution::Avoided)
    {
        SetPhase(EIsabelCruzPhase::Resolved);
        CombatComponent->SetNonHostile();
        GetCharacterMovement()->DisableMovement();
        SetActorTickEnabled(false);
        return;
    }

    SetPhase(EvaluatePhase(HealthComponent->GetHealthPercent()));
    TargetCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
}

void AIsabelCruzCharacter::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (Resolution != EBossEncounterResolution::Unresolved || HealthComponent->IsDead()) return;

    AttackCooldownRemaining = FMath::Max(0.0f, AttackCooldownRemaining - DeltaSeconds);
    if (!IsValid(TargetCharacter))
        TargetCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (!IsValid(TargetCharacter)) return;

    const FVector Offset = TargetCharacter->GetActorLocation() - GetActorLocation();
    const float Distance = Offset.Size2D();
    if (!bEncounterActive)
    {
        if (Distance > AwarenessRangeCentimetres) return;
        if (!BeginEncounter(TargetCharacter)) return;
    }

    if (CombatComponent->CurrentState != ECombatState::Idle) return;
    if (Distance > AttackRangeCentimetres)
    {
        AddMovementInput(Offset.GetSafeNormal2D());
        return;
    }

    const FRotator Facing = Offset.Rotation();
    SetActorRotation(FRotator(0.0f, Facing.Yaw, 0.0f));
    if (AttackCooldownRemaining <= 0.0f && CombatComponent->PerformLightAttack())
        AttackCooldownRemaining = AttackCooldownSeconds;
}

bool AIsabelCruzCharacter::BeginEncounter(ACharacter* PlayerCharacter)
{
    if (Resolution != EBossEncounterResolution::Unresolved || bEncounterActive ||
        HealthComponent->IsDead() || !IsValid(PlayerCharacter)) return false;
    TargetCharacter = PlayerCharacter;
    bEncounterActive = true;
    return true;
}

bool AIsabelCruzCharacter::TrySpare()
{
    if (!CanBeSpared()) return false;
    ResolveEncounter(EBossEncounterResolution::Spared);
    return true;
}

bool AIsabelCruzCharacter::ResolveAvoided(const bool bFortSurrenderedWithoutCombat)
{
    if (!bFortSurrenderedWithoutCombat || bEncounterActive ||
        Resolution != EBossEncounterResolution::Unresolved || HealthComponent->IsDead())
        return false;
    ResolveEncounter(EBossEncounterResolution::Avoided);
    return true;
}

bool AIsabelCruzCharacter::CanBeSpared() const
{
    return bEncounterActive && Resolution == EBossEncounterResolution::Unresolved &&
        !HealthComponent->IsDead() &&
        CombatComponent->CurrentState == ECombatState::Staggered;
}

EIsabelCruzPhase AIsabelCruzCharacter::EvaluatePhase(const float HealthFraction)
{
    const float Remaining = FMath::Clamp(HealthFraction, 0.0f, 1.0f);
    if (Remaining > ListPhaseHealthFraction) return EIsabelCruzPhase::Duty;
    if (Remaining > EighteenFortySixPhaseHealthFraction) return EIsabelCruzPhase::List;
    return EIsabelCruzPhase::EighteenFortySix;
}

void AIsabelCruzCharacter::OnHealthChanged(
    const float NewHealth,
    const float MaximumHealth,
    const float /*Delta*/)
{
    if (Resolution != EBossEncounterResolution::Unresolved || MaximumHealth <= 0.0f) return;
    SetPhase(EvaluatePhase(NewHealth / MaximumHealth));
}

void AIsabelCruzCharacter::OnCruzDied(AActor* /*DamageCauser*/)
{
    if (Resolution != EBossEncounterResolution::Unresolved) return;
    ResolveEncounter(EBossEncounterResolution::Killed);
    if (ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0))
    {
        if (UCameraStateComponent* CameraState =
            PlayerCharacter->FindComponentByClass<UCameraStateComponent>())
            CameraState->EnterDeathHold();
    }
}

void AIsabelCruzCharacter::SetPhase(const EIsabelCruzPhase NewPhase)
{
    if (CurrentPhase == NewPhase) return;
    const EIsabelCruzPhase PreviousPhase = CurrentPhase;
    CurrentPhase = NewPhase;
    OnPhaseChanged.Broadcast(PreviousPhase, CurrentPhase);
}

void AIsabelCruzCharacter::ResolveEncounter(const EBossEncounterResolution NewResolution)
{
    if (Resolution != EBossEncounterResolution::Unresolved ||
        NewResolution == EBossEncounterResolution::Unresolved) return;
    Resolution = NewResolution;
    bEncounterActive = false;
    TargetCharacter = nullptr;
    AttackCooldownRemaining = 0.0f;
    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();
    if (Resolution == EBossEncounterResolution::Killed) CombatComponent->SetDead();
    else CombatComponent->SetNonHostile();
    SetPhase(EIsabelCruzPhase::Resolved);
    SetActorTickEnabled(false);
    OnEncounterResolved.Broadcast(Resolution);
}
