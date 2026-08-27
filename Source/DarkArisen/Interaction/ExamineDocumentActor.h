// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/DarkArisenInteractable.h"
#include "ExamineDocumentActor.generated.h"

class UStaticMeshComponent;

/** Player-paced Examine object: held in world, Anchored camera, surrounding world still visible. */
UCLASS()
class DARKARISEN_API AExamineDocumentActor : public AActor, public IDarkArisenInteractable
{
    GENERATED_BODY()

public:
    AExamineDocumentActor();
    virtual void Tick(float DeltaSeconds) override;

    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual EInteractionClass GetInteractionClass_Implementation() const override;
    virtual float GetInteractionDuration_Implementation() const override;
    virtual FText GetInteractionLabel_Implementation() const override;
    virtual FText GetExamineTitle_Implementation() const override;
    virtual FText GetExamineBody_Implementation() const override;
    virtual void BeginInteraction_Implementation(AActor* Interactor) override;
    virtual void CompleteInteraction_Implementation(AActor* Interactor) override;
    virtual void CancelInteraction_Implementation(AActor* Interactor) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Examine")
    FText DocumentTitle;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Examine", meta = (MultiLine = "true"))
    FText DocumentBody;

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> DocumentMesh;
    UPROPERTY()
    TObjectPtr<AActor> CurrentInteractor;
    FTransform RestingTransform;
    bool bMovingToHand = false;
    bool bPresented = false;
};
