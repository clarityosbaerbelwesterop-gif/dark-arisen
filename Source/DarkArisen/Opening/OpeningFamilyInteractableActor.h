#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/DarkArisenInteractable.h"
#include "OpeningFamilyInteractableActor.generated.h"
class UStaticMeshComponent;
/** Physical family interaction; progression is still owned exclusively by UOpeningRuntimeComponent. */
UCLASS() class DARKARISEN_API AOpeningFamilyInteractableActor : public AActor, public IDarkArisenInteractable
{
    GENERATED_BODY()
public:
    AOpeningFamilyInteractableActor();
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) TObjectPtr<UStaticMeshComponent> PresentationMesh;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Opening|Family") FName CharacterId=TEXT("character.ethan");
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Opening|Family") FText CharacterName;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Opening|Family") FName DialogueId;
    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual EInteractionClass GetInteractionClass_Implementation() const override{return EInteractionClass::Use;}
    virtual float GetInteractionDuration_Implementation() const override{return 0.35f;}
    virtual FText GetInteractionLabel_Implementation() const override;
    virtual FName GetPersistentInteractionId_Implementation() const override{return CharacterId;}
    virtual void CompleteInteraction_Implementation(AActor* Interactor) override;
};
