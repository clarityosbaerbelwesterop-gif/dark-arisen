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
    bool IsInteracting() const { return ActiveTarget.IsValid(); }

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

private:
    TWeakObjectPtr<AActor> FocusedTarget;
    TWeakObjectPtr<AActor> ActiveTarget;
    float PromptTimeRemaining = 0.0f;
    bool bPromptVisible = false;

    AActor* TraceForCandidate() const;
    void UpdateFocus(float DeltaTime);
    void SetFocusedTarget(AActor* NewTarget);
    void SetPromptVisible(bool bVisible);
    void CompleteActiveInteraction();
};
