// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Rexa/RexaSettlementResident.h"

#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "Components/HealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Rexa/RexaSettlementAnchor.h"
#include "UObject/ConstructorHelpers.h"

ARexaSettlementResident::ARexaSettlementResident()
{
    PrimaryActorTick.bCanEverTick = false;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    GetCapsuleComponent()->InitCapsuleSize(36.0f, 88.0f);

    UCharacterMovementComponent* Movement = GetCharacterMovement();
    Movement->bOrientRotationToMovement = true;
    Movement->RotationRate = FRotator(0.0f, 300.0f, 0.0f);
    Movement->MaxWalkSpeed = 180.0f;
    Movement->bEnablePhysicsInteraction = false;
    Movement->InitialPushForceFactor = 0.0f;
    Movement->PushForceFactor = 0.0f;
    Movement->TouchForceFactor = 0.0f;
    Movement->RepulsionForce = 0.0f;

    GreyboxBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GreyboxBody"));
    GreyboxBody->SetupAttachment(GetCapsuleComponent());
    GreyboxBody->SetRelativeLocation(FVector(0.0f, 0.0f, -8.0f));
    GreyboxBody->SetRelativeScale3D(FVector(0.58f, 0.58f, 1.55f));
    GreyboxBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMesh(
        TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (BodyMesh.Succeeded()) GreyboxBody->SetStaticMesh(BodyMesh.Object);

    Tags.AddUnique(TEXT("Rexa.NonCombatant"));
    SetCanBeDamaged(false);
}

void ARexaSettlementResident::LaunchCharacter(
    const FVector LaunchVelocity,
    const bool bXYOverride,
    const bool bZOverride)
{
    if (!bDefinitionInitialized || ResidentDefinition.bProtectedChild) return;
    Super::LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);
}

float ARexaSettlementResident::TakeDamage(
    const float DamageAmount,
    const FDamageEvent& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    if (!bDefinitionInitialized || ResidentDefinition.bProtectedChild) return 0.0f;
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

bool ARexaSettlementResident::InitializeFromDefinition(
    const FRexaResidentDefinition& Definition)
{
    const bool bIsChild = Definition.AgeBand == ERexaResidentAgeBand::Child;
    if (Definition.StableResidentId.IsNone() || Definition.DisplayName.IsEmpty() ||
        Definition.OccupationId.IsNone() || Definition.HomeOffsetCentimetres.ContainsNaN() ||
        Definition.DawnAnchorId.IsNone() || Definition.MiddayAnchorId.IsNone() ||
        Definition.EveningAnchorId.IsNone() || Definition.NightAnchorId.IsNone() ||
        Definition.bProtectedChild != bIsChild ||
        FindComponentByClass<UHealthComponent>() != nullptr ||
        FindComponentByClass<UCombatComponent>() != nullptr) return false;

    ResidentDefinition = Definition;
    bDefinitionInitialized = true;
    CurrentPurposeAnchorId = Definition.NightAnchorId;
    ApplyNonCombatantPolicy();
    return true;
}

FName ARexaSettlementResident::RefreshPurposeAnchor(const int64 GameMinute)
{
    CurrentPurposeAnchorId = bDefinitionInitialized
        ? ResidentDefinition.GetPurposeAnchorAtGameMinute(GameMinute)
        : NAME_None;
    return CurrentPurposeAnchorId;
}

bool ARexaSettlementResident::MoveToPurposeAnchor(
    const int64 GameMinute,
    ARexaSettlementAnchor* Anchor)
{
    if (!bDefinitionInitialized || GameMinute < 0 || !IsValid(Anchor) ||
        !Anchor->IsAuthoredAnchorValid()) return false;
    const FName RequiredAnchorId = ResidentDefinition.GetPurposeAnchorAtGameMinute(GameMinute);
    if (RequiredAnchorId.IsNone() || Anchor->AnchorId != RequiredAnchorId) return false;

    if (!GetController()) SpawnDefaultController();
    AAIController* ResidentController = Cast<AAIController>(GetController());
    if (!ResidentController) return false;
    const EPathFollowingRequestResult::Type Result = ResidentController->MoveToActor(
        Anchor,
        PurposeAnchorAcceptanceRadiusCentimetres,
        true,
        true,
        true,
        nullptr,
        true);
    if (Result == EPathFollowingRequestResult::Failed) return false;
    CurrentPurposeAnchorId = RequiredAnchorId;
    return true;
}

void ARexaSettlementResident::ClearPurposeRoute()
{
    if (AAIController* ResidentController = Cast<AAIController>(GetController()))
        ResidentController->StopMovement();
    CurrentPurposeAnchorId = NAME_None;
}

bool ARexaSettlementResident::IsProtectedChildRuntime() const
{
    if (!bDefinitionInitialized || !ResidentDefinition.bProtectedChild || CanBeDamaged())
        return false;
    return FindComponentByClass<UHealthComponent>() == nullptr &&
        FindComponentByClass<UCombatComponent>() == nullptr &&
        GreyboxBody && GreyboxBody->GetCollisionEnabled() == ECollisionEnabled::NoCollision &&
        GetCharacterMovement() && !GetCharacterMovement()->bEnablePhysicsInteraction;
}

void ARexaSettlementResident::ApplyNonCombatantPolicy()
{
    Tags.AddUnique(TEXT("Rexa.NonCombatant"));
    Tags.Remove(TEXT("Rexa.ProtectedChild"));
    if (ResidentDefinition.bProtectedChild) Tags.AddUnique(TEXT("Rexa.ProtectedChild"));
    SetCanBeDamaged(!ResidentDefinition.bProtectedChild);

    UCharacterMovementComponent* Movement = GetCharacterMovement();
    Movement->bEnablePhysicsInteraction = false;
    Movement->InitialPushForceFactor = 0.0f;
    Movement->PushForceFactor = 0.0f;
    Movement->TouchForceFactor = 0.0f;
    Movement->RepulsionForce = 0.0f;
    GreyboxBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
