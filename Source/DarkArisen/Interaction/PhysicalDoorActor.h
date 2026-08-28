// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/DarkArisenInteractable.h"
#include "PhysicalDoorActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;

/** Interruptible physical door. Open and broken states are serializable and never fade. */
UCLASS()
class DARKARISEN_API APhysicalDoorActor : public AActor, public IDarkArisenInteractable
{
    GENERATED_BODY()

public:
    APhysicalDoorActor();
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

    UFUNCTION(BlueprintCallable, Category = "Door")
    void BreakDoor();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Persistence")
    FName PersistentId = TEXT("M1.GreyboxDoor");
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Door", SaveGame)
    bool bOpen = false;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Door", SaveGame)
    bool bBroken = false;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door")
    float OpenAngleDegrees = 95.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door")
    float InteractionDurationSeconds = 1.2f;

    static constexpr int32 OpenStateBit = 1;
    static constexpr int32 BrokenStateBit = 2;

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USceneComponent> SceneRoot;
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> DoorLeaf;
    bool bAnimating = false;
    float TargetYaw = 0.0f;
    float AngularSpeedDegreesPerSecond = 0.0f;

    void ApplyStableState();
};
