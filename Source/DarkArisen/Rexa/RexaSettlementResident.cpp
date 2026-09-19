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
    Movement->MaxWalkSpeed = RoutineWalkSpeedCentimetresPerSecond;
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
        (bIsChild && Definition.ChildSafetyAnchorId.IsNone()) ||
        (!bIsChild && !Definition.ChildSafetyAnchorId.IsNone()) ||
        FindComponentByClass<UHealthComponent>() != nullptr ||
        FindComponentByClass<UCombatComponent>() != nullptr) return false;

    ResidentDefinition = Definition;
    bDefinitionInitialized = true;
    CurrentPurposeAnchorId = Definition.NightAnchorId;
    CurrentSafetyState = ERexaResidentSafetyState::Routine;
    ApplyNonCombatantPolicy();
    return true;
}

FName ARexaSettlementResident::RefreshPurposeAnchor(const int64 GameMinute)
{
    if (CurrentSafetyState != ERexaResidentSafetyState::Routine)
        return CurrentPurposeAnchorId;
    CurrentPurposeAnchorId = bDefinitionInitialized
        ? ResidentDefinition.GetPurposeAnchorAtGameMinute(GameMinute)
        : NAME_None;
    return CurrentPurposeAnchorId;
}

bool ARexaSettlementResident::MoveToPurposeAnchor(
    const int64 GameMinute,
    ARexaSettlementAnchor* Anchor)
{
    if (!bDefinitionInitialized ||
        CurrentSafetyState != ERexaResidentSafetyState::Routine ||
        GameMinute < 0 || !IsValid(Anchor) ||
        !Anchor->IsAuthoredAnchorValid()) return false;
    const FName RequiredAnchorId = ResidentDefinition.GetPurposeAnchorAtGameMinute(GameMinute);
    if (RequiredAnchorId.IsNone() || Anchor->AnchorId != RequiredAnchorId) return false;

    if (!RequestMoveToAnchor(Anchor, PurposeAnchorAcceptanceRadiusCentimetres)) return false;
    CurrentPurposeAnchorId = RequiredAnchorId;
    return true;
}

void ARexaSettlementResident::ClearPurposeRoute()
{
    if (AAIController* ResidentController = Cast<AAIController>(GetController()))
        ResidentController->StopMovement();
    CurrentPurposeAnchorId = NAME_None;
}

bool ARexaSettlementResident::EnterProtectedChildFlee(
    const FVector& CombatLocation,
    ARexaSettlementAnchor* SafetyAnchor)
{
    if (!IsProtectedChildRuntime() ||
        CurrentSafetyState != ERexaResidentSafetyState::Routine ||
        CombatLocation.ContainsNaN() || !IsValid(SafetyAnchor) ||
        !SafetyAnchor->IsAuthoredAnchorValid() ||
        !SafetyAnchor->bChildSafetyDestination ||
        SafetyAnchor->AnchorId != ResidentDefinition.ChildSafetyAnchorId ||
        FVector::DistSquared(GetActorLocation(), CombatLocation) >
            FMath::Square(ChildCombatFleeRadiusCentimetres)) return false;

    GetCharacterMovement()->MaxWalkSpeed = ChildFleeSpeedCentimetresPerSecond;
    if (!RequestMoveToAnchor(SafetyAnchor, PurposeAnchorAcceptanceRadiusCentimetres))
    {
        GetCharacterMovement()->MaxWalkSpeed = RoutineWalkSpeedCentimetresPerSecond;
        return false;
    }
    CurrentPurposeAnchorId = SafetyAnchor->AnchorId;
    CurrentSafetyState = ERexaResidentSafetyState::FleeingCombat;
    return true;
}

bool ARexaSettlementResident::ShelterProtectedChild(
    ARexaSettlementAnchor* SafetyAnchor)
{
    if (!IsProtectedChildRuntime()) return false;
    ClearPurposeRoute();
    CurrentSafetyState = ERexaResidentSafetyState::ShelteredOffscreen;
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    if (IsValid(SafetyAnchor) && SafetyAnchor->IsAuthoredAnchorValid() &&
        SafetyAnchor->bChildSafetyDestination &&
        SafetyAnchor->AnchorId == ResidentDefinition.ChildSafetyAnchorId)
    {
        SetActorLocation(
            SafetyAnchor->GetActorLocation(),
            false,
            nullptr,
            ETeleportType::TeleportPhysics);
    }
    return true;
}

bool ARexaSettlementResident::RestoreProtectedChildAfterCombat(
    ARexaSettlementAnchor* SafetyAnchor)
{
    if (!IsProtectedChildRuntime() ||
        CurrentSafetyState == ERexaResidentSafetyState::Routine ||
        !IsValid(SafetyAnchor) || !SafetyAnchor->IsAuthoredAnchorValid() ||
        !SafetyAnchor->bChildSafetyDestination ||
        SafetyAnchor->AnchorId != ResidentDefinition.ChildSafetyAnchorId) return false;
    SetActorLocation(
        SafetyAnchor->GetActorLocation(),
        false,
        nullptr,
        ETeleportType::TeleportPhysics);
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    GetCharacterMovement()->MaxWalkSpeed = RoutineWalkSpeedCentimetresPerSecond;
    CurrentPurposeAnchorId = SafetyAnchor->AnchorId;
    CurrentSafetyState = ERexaResidentSafetyState::Routine;
    return true;
}

bool ARexaSettlementResident::EnterCivilianCombatFlee(const FVector& CombatLocation)
{
    if(!bDefinitionInitialized || ResidentDefinition.bProtectedChild ||
        CurrentSafetyState!=ERexaResidentSafetyState::Routine || CombatLocation.ContainsNaN()) return false;
    FVector Away=GetActorLocation()-CombatLocation;
    Away.Z=0.f;
    if(Away.IsNearlyZero()) Away=GetActorForwardVector();
    Away.Normalize();
    if(!GetController()) SpawnDefaultController();
    AAIController* ResidentController=Cast<AAIController>(GetController());
    if(!ResidentController) return false;
    GetCharacterMovement()->MaxWalkSpeed=FMath::Max(RoutineWalkSpeedCentimetresPerSecond,360.f);
    const EPathFollowingRequestResult::Type Result=ResidentController->MoveToLocation(
        GetActorLocation()+Away*1200.f,120.f,true,true,true,false,nullptr,true);
    if(Result==EPathFollowingRequestResult::Failed)
    {
        GetCharacterMovement()->MaxWalkSpeed=RoutineWalkSpeedCentimetresPerSecond;
        return false;
    }
    CurrentSafetyState=ERexaResidentSafetyState::FleeingCombat;
    return true;
}

bool ARexaSettlementResident::RestoreCivilianRoutine()
{
    if(!bDefinitionInitialized || ResidentDefinition.bProtectedChild ||
        CurrentSafetyState==ERexaResidentSafetyState::Routine) return false;
    if(AAIController* ResidentController=Cast<AAIController>(GetController()))
        ResidentController->StopMovement();
    GetCharacterMovement()->MaxWalkSpeed=RoutineWalkSpeedCentimetresPerSecond;
    CurrentSafetyState=ERexaResidentSafetyState::Routine;
    return true;
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

bool ARexaSettlementResident::RequestMoveToAnchor(
    ARexaSettlementAnchor* Anchor,
    const float AcceptanceRadius)
{
    if (!IsValid(Anchor) || AcceptanceRadius < 0.0f) return false;
    if (!GetController()) SpawnDefaultController();
    AAIController* ResidentController = Cast<AAIController>(GetController());
    if (!ResidentController) return false;
    const EPathFollowingRequestResult::Type Result = ResidentController->MoveToActor(
        Anchor,
        AcceptanceRadius,
        true,
        true,
        true,
        nullptr,
        true);
    return Result != EPathFollowingRequestResult::Failed;
}
