#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/DarkArisenInteractable.h"
#include "MainStoryRouteResolutionActor.generated.h"
class UStaticMeshComponent;
/** Authored physical route choice. Distinct placed actors represent negotiate/avoid/etc.; no menu-owned story state. */
UCLASS()
class DARKARISEN_API AMainStoryRouteResolutionActor : public AActor, public IDarkArisenInteractable
{
    GENERATED_BODY()
public:
    AMainStoryRouteResolutionActor();
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Visual;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName MissionId;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName OutcomeKey;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName OutcomeValue;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText InteractionLabel;
    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractionLabel_Implementation() const override;
    virtual void CompleteInteraction_Implementation(AActor* Interactor) override;
};
