#pragma once

#include "CoreMinimal.h"
#include "ColonialWar/ColonialWarStateSubsystem.h"
#include "GameFramework/Actor.h"
#include "Interaction/DarkArisenInteractable.h"
#include "MainStoryWarActionActor.generated.h"

class UStaticMeshComponent;

/**
 * Thin physical adapter between an authored main-story interaction and the existing Colonial War authority.
 * It does not own war state, reputation, mission state, or UI.
 */
UCLASS()
class DARKARISEN_API AMainStoryWarActionActor : public AActor, public IDarkArisenInteractable
{
    GENERATED_BODY()

public:
    AMainStoryWarActionActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> Visual;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Story")
    FName MissionId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="War")
    FName RegionId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="War")
    EColonialFaction InitialController = EColonialFaction::Imperial;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="War")
    EWarActionVerb Verb = EWarActionVerb::Break;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="War")
    EColonialFaction TargetFaction = EColonialFaction::Imperial;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="War", meta=(ClampMin="0"))
    int32 ControlDelta = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="War", meta=(ClampMin="0"))
    int32 LiberationDelta = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="War", meta=(ClampMin="0"))
    int32 CrimsonDelta = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Story")
    FName OutcomeKey;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Story")
    FName OutcomeValue;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction")
    FText InteractionLabel;

    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual FText GetInteractionLabel_Implementation() const override;
    virtual void CompleteInteraction_Implementation(AActor* Interactor) override;
};
