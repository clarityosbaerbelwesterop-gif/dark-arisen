// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interaction/DarkArisenInteractable.h"
#include "InteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnInteractionPromptChanged, bool, bVisible, const FText&, Label);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnInteractionStateChanged, AActor*, Target, bool, bInteracting);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnExaminePresentationChanged,
    bool, bVisible,
    const FText&, Title,
    const FText&, Body);

/** Finds one looked-at target at arm's reach and runs its physical duration. */
UCLASS(ClassGroup = (DarkArisen), meta = (BlueprintSpawnableComponent))
class DARKARISEN_API UInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInteractionComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    bool TryBeginInteraction();

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void CancelActiveInteraction();

    UFUNCTION(BlueprintPure, Category = "Interaction")
    bool IsInteracting() const
    {
        return ActiveTarget.IsValid() || bExaminePresentationActive;
    }

    UFUNCTION(BlueprintPure, Category = "Interaction|Prompt")
    bool IsPromptVisible() const { return bPromptVisible; }

    UFUNCTION(BlueprintPure, Category = "Interaction|Prompt")
    FText GetPromptLabel() const { return VisiblePromptLabel; }

    UFUNCTION(BlueprintPure, Category = "Interaction|Examine")
    bool IsExamineVisible() const { return bExaminePresentationActive; }

    UFUNCTION(BlueprintPure, Category = "Interaction|Examine")
    FText GetExamineTitle() const { return ExamineTitle; }

    UFUNCTION(BlueprintPure, Category = "Interaction|Examine")
    FText GetExamineBody() const { return ExamineBody; }

    UFUNCTION(BlueprintPure, Category = "Interaction")
    AActor* GetFocusedTarget() const { return FocusedTarget.Get(); }

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Interaction")
    EInteractionClass FocusedInteractionClass = EInteractionClass::Take;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Interaction")
    float InteractionTimeRemaining = 0.0f;

    UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
    FOnInteractionPromptChanged OnPromptChanged;

    UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
    FOnInteractionStateChanged OnInteractionStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
    FOnExaminePresentationChanged OnExaminePresentationChanged;

private:
    TWeakObjectPtr<AActor> FocusedTarget;
    TWeakObjectPtr<AActor> ActiveTarget;
    TWeakObjectPtr<AActor> ActiveExamineTarget;
    float PromptTimeRemaining = 0.0f;
    bool bPromptVisible = false;
    bool bExaminePresentationActive = false;

    UPROPERTY()
    FText VisiblePromptLabel;
    UPROPERTY()
    FText ExamineTitle;
    UPROPERTY()
    FText ExamineBody;

    AActor* TraceForCandidate() const;
    void UpdateFocus(float DeltaTime);
    void SetFocusedTarget(AActor* NewTarget);
    void SetPromptVisible(bool bVisible);
    void CompleteActiveInteraction();
    void BeginExaminePresentation(AActor* Target);
    void CloseExaminePresentation();
};
