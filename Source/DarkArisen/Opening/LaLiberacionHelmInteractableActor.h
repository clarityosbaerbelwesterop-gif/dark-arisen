#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/DarkArisenInteractable.h"
#include "LaLiberacionHelmInteractableActor.generated.h"
class ALaLiberacionShip;
class UStaticMeshComponent;
/** Helm interaction bridges the physical ship to PR #23 gates; it never fabricates ownership. */
UCLASS() class DARKARISEN_API ALaLiberacionHelmInteractableActor : public AActor, public IDarkArisenInteractable
{
    GENERATED_BODY()
public:
    ALaLiberacionHelmInteractableActor();
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) TObjectPtr<UStaticMeshComponent> HelmMesh;
    UPROPERTY(EditInstanceOnly,BlueprintReadOnly,Category="Opening|Ship") TObjectPtr<ALaLiberacionShip> Ship;
    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual EInteractionClass GetInteractionClass_Implementation() const override{return EInteractionClass::Use;}
    virtual float GetInteractionDuration_Implementation() const override{return 0.6f;}
    virtual FText GetInteractionLabel_Implementation() const override{return NSLOCTEXT("DarkArisenOpening","TakeHelm","Take the helm");}
    virtual void CompleteInteraction_Implementation(AActor* Interactor) override;
};
