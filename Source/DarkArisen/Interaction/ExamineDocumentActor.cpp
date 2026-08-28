// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Interaction/ExamineDocumentActor.h"

#include "Components/StaticMeshComponent.h"
#include "CoreLoopTuning.h"
#include "UObject/ConstructorHelpers.h"

AExamineDocumentActor::AExamineDocumentActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
    DocumentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DocumentMesh"));
    SetRootComponent(DocumentMesh);
    DocumentMesh->SetRelativeScale3D(FVector(0.32f, 0.22f, 0.015f));
    DocumentMesh->SetCollisionProfileName(TEXT("BlockAll"));
    DocumentTitle = NSLOCTEXT("DarkArisenInteraction", "GreyboxLetterTitle", "Salt-stained note");
    DocumentBody = NSLOCTEXT(
        "DarkArisenInteraction",
        "GreyboxLetterBody",
        "The ink has run at the edges. One line remains clear: do not mistake silence for safety.");
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (Cube.Succeeded()) DocumentMesh->SetStaticMesh(Cube.Object);
}

void AExamineDocumentActor::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!bMovingToHand || !IsValid(CurrentInteractor)) return;
    const FVector HandPoint = CurrentInteractor->GetActorLocation() +
        CurrentInteractor->GetActorForwardVector() * 65.0f + FVector(0.0f, 0.0f, 62.0f);
    SetActorLocation(FMath::VInterpTo(GetActorLocation(), HandPoint, DeltaSeconds, 10.0f));
}

bool AExamineDocumentActor::CanInteract_Implementation(AActor* Interactor) const
{
    return IsValid(Interactor) && !bPresented;
}

EInteractionClass AExamineDocumentActor::GetInteractionClass_Implementation() const
{
    return EInteractionClass::Examine;
}

float AExamineDocumentActor::GetInteractionDuration_Implementation() const
{
    return DarkArisen::CoreLoopTuning::DefaultTakeSeconds;
}

FText AExamineDocumentActor::GetInteractionLabel_Implementation() const
{
    return NSLOCTEXT("DarkArisenInteraction", "ExamineDocument", "Examine");
}

FText AExamineDocumentActor::GetExamineTitle_Implementation() const
{
    return DocumentTitle;
}

FText AExamineDocumentActor::GetExamineBody_Implementation() const
{
    return DocumentBody;
}

void AExamineDocumentActor::BeginInteraction_Implementation(AActor* Interactor)
{
    RestingTransform = GetActorTransform();
    CurrentInteractor = Interactor;
    bMovingToHand = true;
    DocumentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetActorTickEnabled(true);
}

void AExamineDocumentActor::CompleteInteraction_Implementation(AActor* Interactor)
{
    bMovingToHand = false;
    bPresented = true;
    SetActorTickEnabled(false);
    if (IsValid(Interactor))
    {
        AttachToActor(Interactor, FAttachmentTransformRules::KeepWorldTransform);
        SetActorRelativeLocation(FVector(65.0f, 18.0f, 62.0f));
        SetActorRelativeRotation(FRotator(8.0f, 88.0f, 2.0f));
    }
}

void AExamineDocumentActor::CancelInteraction_Implementation(AActor* /*Interactor*/)
{
    bMovingToHand = false;
    bPresented = false;
    CurrentInteractor = nullptr;
    SetActorTickEnabled(false);
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    SetActorTransform(RestingTransform);
    DocumentMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
