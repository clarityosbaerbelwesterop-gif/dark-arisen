// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Interaction/PhysicalDoorActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

APhysicalDoorActor::APhysicalDoorActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);
    DoorLeaf = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorLeaf"));
    DoorLeaf->SetupAttachment(SceneRoot);
    DoorLeaf->SetRelativeLocation(FVector(0.0f, 50.0f, 100.0f));
    DoorLeaf->SetRelativeScale3D(FVector(0.18f, 1.0f, 2.0f));
    DoorLeaf->SetCollisionProfileName(TEXT("BlockAll"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (Cube.Succeeded()) DoorLeaf->SetStaticMesh(Cube.Object);
}

void APhysicalDoorActor::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!bAnimating || bBroken) return;
    const float CurrentYaw = DoorLeaf->GetRelativeRotation().Yaw;
    const float NewYaw = FMath::FInterpConstantTo(
        CurrentYaw, TargetYaw, DeltaSeconds, AngularSpeedDegreesPerSecond);
    DoorLeaf->SetRelativeRotation(FRotator(0.0f, NewYaw, 0.0f));
    if (FMath::IsNearlyEqual(NewYaw, TargetYaw, 0.1f))
    {
        bAnimating = false;
        SetActorTickEnabled(false);
    }
}

bool APhysicalDoorActor::CanInteract_Implementation(AActor* Interactor) const
{
    return IsValid(Interactor) && !bBroken;
}

EInteractionClass APhysicalDoorActor::GetInteractionClass_Implementation() const
{
    return EInteractionClass::Use;
}

float APhysicalDoorActor::GetInteractionDuration_Implementation() const
{
    return InteractionDurationSeconds;
}

FText APhysicalDoorActor::GetInteractionLabel_Implementation() const
{
    return bOpen
        ? NSLOCTEXT("DarkArisenInteraction", "CloseDoor", "Close")
        : NSLOCTEXT("DarkArisenInteraction", "OpenDoor", "Open");
}

void APhysicalDoorActor::BeginInteraction_Implementation(AActor* /*Interactor*/)
{
    TargetYaw = bOpen ? 0.0f : OpenAngleDegrees;
    const float RemainingDegrees = FMath::Abs(
        FMath::FindDeltaAngleDegrees(DoorLeaf->GetRelativeRotation().Yaw, TargetYaw));
    AngularSpeedDegreesPerSecond = RemainingDegrees /
        FMath::Max(InteractionDurationSeconds, KINDA_SMALL_NUMBER);
    bAnimating = true;
    SetActorTickEnabled(true);
}

void APhysicalDoorActor::CompleteInteraction_Implementation(AActor* /*Interactor*/)
{
    bOpen = TargetYaw != 0.0f;
    DoorLeaf->SetRelativeRotation(FRotator(0.0f, TargetYaw, 0.0f));
    bAnimating = false;
    SetActorTickEnabled(false);
}

void APhysicalDoorActor::CancelInteraction_Implementation(AActor* /*Interactor*/)
{
    bAnimating = false;
    SetActorTickEnabled(false);
}

FName APhysicalDoorActor::GetPersistentInteractionId_Implementation() const
{
    return PersistentId;
}

int32 APhysicalDoorActor::CapturePersistentState_Implementation() const
{
    return (bOpen ? OpenStateBit : 0) | (bBroken ? BrokenStateBit : 0);
}

void APhysicalDoorActor::RestorePersistentState_Implementation(const int32 StateBits)
{
    bOpen = (StateBits & OpenStateBit) != 0;
    bBroken = (StateBits & BrokenStateBit) != 0;
    ApplyStableState();
}

void APhysicalDoorActor::BreakDoor()
{
    bBroken = true;
    bAnimating = false;
    SetActorTickEnabled(false);
    DoorLeaf->SetCollisionProfileName(TEXT("PhysicsActor"));
    DoorLeaf->SetSimulatePhysics(true);
}

void APhysicalDoorActor::ApplyStableState()
{
    bAnimating = false;
    SetActorTickEnabled(false);
    DoorLeaf->SetSimulatePhysics(false);
    DoorLeaf->SetCollisionProfileName(TEXT("BlockAll"));
    DoorLeaf->SetRelativeRotation(FRotator(0.0f, bOpen ? OpenAngleDegrees : 0.0f, 0.0f));
    if (bBroken) BreakDoor();
}
