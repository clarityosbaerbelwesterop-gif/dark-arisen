// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Components/InteractionComponent.h"

#include "Components/CameraStateComponent.h"
#include "CoreLoopTuning.h"
#include "DesignLaws.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

UInteractionComponent::UInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (bExaminePresentationActive && !ActiveExamineTarget.IsValid())
    {
        CloseExaminePresentation();
    }
    if (ActiveTarget.IsValid())
    {
        AActor* Target = ActiveTarget.Get();
        if (!Target->GetClass()->ImplementsInterface(UDarkArisenInteractable::StaticClass()) ||
            !IDarkArisenInteractable::Execute_CanInteract(Target, GetOwner()))
        {
            CancelActiveInteraction();
        }
        else
        {
            InteractionTimeRemaining = FMath::Max(0.0f, InteractionTimeRemaining - DeltaTime);
            if (InteractionTimeRemaining <= 0.0f) CompleteActiveInteraction();
        }
    }
    UpdateFocus(DeltaTime);
}

bool UInteractionComponent::TryBeginInteraction()
{
    if (bExaminePresentationActive)
    {
        CloseExaminePresentation();
        return true;
    }
    if (ActiveTarget.IsValid() || !FocusedTarget.IsValid()) return false;
    AActor* Target = FocusedTarget.Get();
    if (!Target->GetClass()->ImplementsInterface(UDarkArisenInteractable::StaticClass()) ||
        !IDarkArisenInteractable::Execute_CanInteract(Target, GetOwner()))
    {
        SetFocusedTarget(nullptr);
        return false;
    }
    ActiveTarget = Target;
    InteractionTimeRemaining = FMath::Max(
        0.0f,
        IDarkArisenInteractable::Execute_GetInteractionDuration(Target));
    SetPromptVisible(false);
    IDarkArisenInteractable::Execute_BeginInteraction(Target, GetOwner());
    OnInteractionStateChanged.Broadcast(Target, true);
    if (InteractionTimeRemaining <= 0.0f) CompleteActiveInteraction();
    return true;
}

void UInteractionComponent::CancelActiveInteraction()
{
    if (bExaminePresentationActive) CloseExaminePresentation();
    if (!ActiveTarget.IsValid()) return;
    AActor* Target = ActiveTarget.Get();
    if (Target->GetClass()->ImplementsInterface(UDarkArisenInteractable::StaticClass()))
    {
        IDarkArisenInteractable::Execute_CancelInteraction(Target, GetOwner());
    }
    ActiveTarget.Reset();
    InteractionTimeRemaining = 0.0f;
    OnInteractionStateChanged.Broadcast(Target, false);
}

AActor* UInteractionComponent::TraceForCandidate() const
{
    AActor* Owner = GetOwner();
    UWorld* World = GetWorld();
    if (!Owner || !World) return nullptr;

    FVector ViewLocation = Owner->GetActorLocation();
    FRotator ViewRotation = Owner->GetActorRotation();
    if (const APawn* Pawn = Cast<APawn>(Owner))
    {
        if (const AController* Controller = Pawn->GetController())
        {
            Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
        }
    }

    constexpr float CentimetresPerMetre = 100.0f;
    const float RangeCentimetres =
        DarkArisen::DesignLaws::InteractionPromptRangeMetres * CentimetresPerMetre;
    const FVector TraceEnd = ViewLocation + ViewRotation.Vector() * RangeCentimetres;
    FHitResult Hit;
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(DarkArisenInteraction), false, Owner);
    if (!World->LineTraceSingleByChannel(
        Hit, ViewLocation, TraceEnd, ECC_Visibility, QueryParams))
    {
        return nullptr;
    }
    AActor* Candidate = Hit.GetActor();
    if (!Candidate ||
        !Candidate->GetClass()->ImplementsInterface(UDarkArisenInteractable::StaticClass()) ||
        !IDarkArisenInteractable::Execute_CanInteract(Candidate, GetOwner()))
    {
        return nullptr;
    }
    return Candidate;
}

void UInteractionComponent::UpdateFocus(const float DeltaTime)
{
    AActor* Candidate = (ActiveTarget.IsValid() || bExaminePresentationActive)
        ? nullptr : TraceForCandidate();
    if (Candidate != FocusedTarget.Get()) SetFocusedTarget(Candidate);
    if (!bPromptVisible) return;
    PromptTimeRemaining = FMath::Max(0.0f, PromptTimeRemaining - DeltaTime);
    if (PromptTimeRemaining <= 0.0f) SetPromptVisible(false);
}

void UInteractionComponent::SetFocusedTarget(AActor* NewTarget)
{
    FocusedTarget = NewTarget;
    PromptTimeRemaining = NewTarget
        ? DarkArisen::CoreLoopTuning::InteractionPromptSeconds
        : 0.0f;
    if (!NewTarget)
    {
        SetPromptVisible(false);
        return;
    }
    FocusedInteractionClass = IDarkArisenInteractable::Execute_GetInteractionClass(NewTarget);
    SetPromptVisible(true);
}

void UInteractionComponent::SetPromptVisible(const bool bVisible)
{
    if (bPromptVisible == bVisible) return;
    bPromptVisible = bVisible;
    VisiblePromptLabel = bPromptVisible && FocusedTarget.IsValid()
        ? IDarkArisenInteractable::Execute_GetInteractionLabel(FocusedTarget.Get())
        : FText::GetEmpty();
    OnPromptChanged.Broadcast(bPromptVisible, VisiblePromptLabel);
}

void UInteractionComponent::CompleteActiveInteraction()
{
    if (!ActiveTarget.IsValid()) return;
    AActor* Target = ActiveTarget.Get();
    EInteractionClass InteractionClass = EInteractionClass::Take;
    if (Target->GetClass()->ImplementsInterface(UDarkArisenInteractable::StaticClass()))
    {
        InteractionClass = IDarkArisenInteractable::Execute_GetInteractionClass(Target);
        IDarkArisenInteractable::Execute_CompleteInteraction(Target, GetOwner());
    }
    ActiveTarget.Reset();
    InteractionTimeRemaining = 0.0f;
    OnInteractionStateChanged.Broadcast(Target, false);
    if (InteractionClass == EInteractionClass::Examine) BeginExaminePresentation(Target);
}

void UInteractionComponent::BeginExaminePresentation(AActor* Target)
{
    if (!IsValid(Target) ||
        !Target->GetClass()->ImplementsInterface(UDarkArisenInteractable::StaticClass())) return;
    ActiveExamineTarget = Target;
    bExaminePresentationActive = true;
    ExamineTitle = IDarkArisenInteractable::Execute_GetExamineTitle(Target);
    ExamineBody = IDarkArisenInteractable::Execute_GetExamineBody(Target);
    if (UCameraStateComponent* Camera =
        GetOwner()->FindComponentByClass<UCameraStateComponent>())
        Camera->EnterAnchoredUntilReleased();
    OnExaminePresentationChanged.Broadcast(true, ExamineTitle, ExamineBody);
}

void UInteractionComponent::CloseExaminePresentation()
{
    if (!bExaminePresentationActive) return;
    AActor* Target = ActiveExamineTarget.Get();
    if (IsValid(Target) &&
        Target->GetClass()->ImplementsInterface(UDarkArisenInteractable::StaticClass()))
        IDarkArisenInteractable::Execute_CancelInteraction(Target, GetOwner());
    bExaminePresentationActive = false;
    ActiveExamineTarget.Reset();
    ExamineTitle = FText::GetEmpty();
    ExamineBody = FText::GetEmpty();
    if (UCameraStateComponent* Camera =
        GetOwner()->FindComponentByClass<UCameraStateComponent>())
    {
        if (Camera->CurrentMode == EPlayerCameraMode::Anchored)
            Camera->ReleaseToFree();
    }
    OnExaminePresentationChanged.Broadcast(false, ExamineTitle, ExamineBody);
}
