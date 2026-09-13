#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/DarkArisenInteractable.h"
#include "OpeningCrewRecruitmentActor.generated.h"
class UStaticMeshComponent;
/** Physical authored recruitment actor. Each interaction advances at most one existing crew-state gate. */
UCLASS() class DARKARISEN_API AOpeningCrewRecruitmentActor : public AActor, public IDarkArisenInteractable
{
    GENERATED_BODY()
public:
    AOpeningCrewRecruitmentActor();
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) TObjectPtr<UStaticMeshComponent> PresentationMesh;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Opening|Crew") FName CrewId=TEXT("crew.mira");
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Opening|Crew") FText DisplayName;
    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual EInteractionClass GetInteractionClass_Implementation() const override{return EInteractionClass::Use;}
    virtual float GetInteractionDuration_Implementation() const override{return 0.45f;}
    virtual FText GetInteractionLabel_Implementation() const override;
    virtual FName GetPersistentInteractionId_Implementation() const override{return CrewId;}
    virtual void CompleteInteraction_Implementation(AActor* Interactor) override;
};
