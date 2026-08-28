// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/DarkArisenInteractable.h"
#include "PhysicalPickupActor.generated.h"

class UStaticMeshComponent;

/** A real-duration Take object. It moves into reach and remains taken after restore. */
UCLASS()
class DARKARISEN_API APhysicalPickupActor : public AActor, public IDarkArisenInteractable
{
    GENERATED_BODY()

public:
    APhysicalPickupActor();
    virtual void Tick(float DeltaSeconds) override;

    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual EInteractionClass GetInteractionClass_Implementation() const override;
    virtual float GetInteractionDuration_Implementation() const override;
    virtual FText GetInteractionLabel_Implementation() const override;
    virtual void BeginInteraction_Implementation(AActor* Interactor) override;
    virtual void CompleteInteraction_Implementation(AActor* Interactor) override;
    virtual void CancelInteraction_Implementation(AActor* Interactor) override;
    virtual FName GetPersistentInteractionId_Implementation() const override;
    virtual int32 CapturePersistentState_Implementation() const override;
    virtual void RestorePersistentState_Implementation(int32 StateBits) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup|Persistence")
    FName PersistentId = TEXT("M1.GreyboxPickup");
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Pickup", SaveGame)
    bool bTaken = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
    FText ItemName;

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> PickupMesh;
    UPROPERTY()
    TObjectPtr<AActor> CurrentInteractor;
    FTransform StartTransform;
    bool bMovingToHand = false;

    void ApplyTakenState();
};
