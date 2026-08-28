// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Interaction/PhysicalPickupActor.h"

#include "Components/StaticMeshComponent.h"
#include "CoreLoopTuning.h"
#include "UObject/ConstructorHelpers.h"

APhysicalPickupActor::APhysicalPickupActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
    PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
    SetRootComponent(PickupMesh);
    PickupMesh->SetRelativeScale3D(FVector(0.18f));
    PickupMesh->SetCollisionProfileName(TEXT("BlockAll"));
    ItemName = NSLOCTEXT("DarkArisenInteraction", "GreyboxKeepsake", "Weathered keepsake");
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (Sphere.Succeeded()) PickupMesh->SetStaticMesh(Sphere.Object);
}

void APhysicalPickupActor::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!bMovingToHand || !IsValid(CurrentInteractor)) return;
    const FVector HandPoint = CurrentInteractor->GetActorLocation() +
        CurrentInteractor->GetActorForwardVector() * 55.0f + FVector(0.0f, 0.0f, 65.0f);
    SetActorLocation(FMath::VInterpTo(GetActorLocation(), HandPoint, DeltaSeconds, 12.0f));
}

bool APhysicalPickupActor::CanInteract_Implementation(AActor* Interactor) const
{
    return IsValid(Interactor) && !bTaken;
}

EInteractionClass APhysicalPickupActor::GetInteractionClass_Implementation() const
{
    return EInteractionClass::Take;
}

float APhysicalPickupActor::GetInteractionDuration_Implementation() const
{
    return DarkArisen::CoreLoopTuning::DefaultTakeSeconds;
}

FText APhysicalPickupActor::GetInteractionLabel_Implementation() const
{
    return FText::Format(
        NSLOCTEXT("DarkArisenInteraction", "TakeNamedItem", "Take {0}"), ItemName);
}

void APhysicalPickupActor::BeginInteraction_Implementation(AActor* Interactor)
{
    StartTransform = GetActorTransform();
    CurrentInteractor = Interactor;
    bMovingToHand = true;
    PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetActorTickEnabled(true);
}

void APhysicalPickupActor::CompleteInteraction_Implementation(AActor* /*Interactor*/)
{
    bTaken = true;
    bMovingToHand = false;
    CurrentInteractor = nullptr;
    SetActorTickEnabled(false);
    ApplyTakenState();
}

void APhysicalPickupActor::CancelInteraction_Implementation(AActor* /*Interactor*/)
{
    if (bTaken) return;
    bMovingToHand = false;
    CurrentInteractor = nullptr;
    SetActorTickEnabled(false);
    SetActorTransform(StartTransform);
    PickupMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

FName APhysicalPickupActor::GetPersistentInteractionId_Implementation() const
{
    return PersistentId;
}

int32 APhysicalPickupActor::CapturePersistentState_Implementation() const
{
    return bTaken ? 1 : 0;
}

void APhysicalPickupActor::RestorePersistentState_Implementation(const int32 StateBits)
{
    bTaken = (StateBits & 1) != 0;
    ApplyTakenState();
}

void APhysicalPickupActor::ApplyTakenState()
{
    SetActorHiddenInGame(bTaken);
    SetActorEnableCollision(!bTaken);
}
