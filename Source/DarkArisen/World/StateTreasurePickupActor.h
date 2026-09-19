#pragma once
#include "CoreMinimal.h"
#include "Interaction/PhysicalPickupActor.h"
#include "StateTreasurePickupActor.generated.h"

/** Physical, duplicate-protected pickup for one of the nine authored State Treasures. */
UCLASS()
class DARKARISEN_API AStateTreasurePickupActor : public APhysicalPickupActor
{
    GENERATED_BODY()
public:
    AStateTreasurePickupActor();
    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual void CompleteInteraction_Implementation(AActor* Interactor) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Treasure")
    FName TreasureId;
};
