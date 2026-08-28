// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RexaSettlementDirector.generated.h"

class ARexaSettlementResident;
class USceneComponent;

/**
 * Authored population owner for Las Raíces. A Rexa level must place this actor deliberately;
 * the M0 room does not spawn the settlement.
 */
UCLASS()
class DARKARISEN_API ARexaSettlementDirector : public AActor
{
    GENERATED_BODY()

public:
    ARexaSettlementDirector();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Rexa|Settlement")
    bool SpawnAuthoredSettlement();

    UFUNCTION(BlueprintCallable, Category = "Rexa|Settlement")
    void ClearSpawnedSettlement();

    UFUNCTION(BlueprintPure, Category = "Rexa|Settlement")
    int32 GetSpawnedResidentCount() const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Settlement")
    bool bSpawnOnBeginPlay = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rexa|Settlement")
    TSubclassOf<ARexaSettlementResident> ResidentClass;

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(Transient)
    TArray<TObjectPtr<ARexaSettlementResident>> SpawnedResidents;
};
