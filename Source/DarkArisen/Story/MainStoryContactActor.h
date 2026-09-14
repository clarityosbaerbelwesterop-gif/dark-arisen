#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/DarkArisenInteractable.h"
#include "MainStoryContactActor.generated.h"
class UStaticMeshComponent;
/** Authored story contact. Dialogue text stays presentation-only; mission authority remains UMainStorySubsystem. */
UCLASS()
class DARKARISEN_API AMainStoryContactActor : public AActor, public IDarkArisenInteractable
{
    GENERATED_BODY()
public:
    AMainStoryContactActor();
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Visual;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName ContactId;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName DialogueId;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName MissionId;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MultiLine="true")) FText DialogueText;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bActivateMissionOnInteraction=false;
    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual EInteractionClass GetInteractionClass_Implementation() const override;
    virtual FText GetInteractionLabel_Implementation() const override;
    virtual FText GetExamineTitle_Implementation() const override;
    virtual FText GetExamineBody_Implementation() const override;
    virtual void CompleteInteraction_Implementation(AActor* Interactor) override;
};
