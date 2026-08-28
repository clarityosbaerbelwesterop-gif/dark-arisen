// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "World/CombatProximityResponder.h"
#include "RexaSettlementDirector.generated.h"

class ARexaSettlementResident;
class ARexaSettlementAnchor;
class USceneComponent;

/**
 * Authored population owner for Las Raíces. A Rexa level must place this actor deliberately;
 * the M0 room does not spawn the settlement.
 */
UCLASS()
class DARKARISEN_API ARexaSettlementDirector :
    public AActor,
    public ICombatProximityResponder
{
    GENERATED_BODY()

public:
    ARexaSettlementDirector();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void ReceiveCombatActivity_Implementation(const FVector& WorldLocation) override;

    UFUNCTION(BlueprintCallable, Category = "Rexa|Settlement")
    bool SpawnAuthoredSettlement();

    UFUNCTION(BlueprintCallable, Category = "Rexa|Settlement")
    void ClearSpawnedSettlement();

    UFUNCTION(BlueprintPure, Category = "Rexa|Settlement")
    int32 GetSpawnedResidentCount() const;

    /** Called by the future canonical world-time owner; this class invents no time scale. */
    UFUNCTION(BlueprintCallable, Category = "Rexa|Schedule")
    bool ApplyGameMinute(int64 GameMinute);

    UFUNCTION(BlueprintCallable, Category = "Rexa|Safety")
    int32 NotifyCombatActivity(const FVector& CombatLocation);

    UFUNCTION(BlueprintCallable, Category = "Rexa|Safety")
    int32 ShelterChildrenForPersistentCombat();

    UFUNCTION(BlueprintCallable, Category = "Rexa|Safety")
    bool EndCombatResponse();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Settlement")
    bool bSpawnOnBeginPlay = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rexa|Settlement")
    TSubclassOf<ARexaSettlementResident> ResidentClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Settlement")
    FName SettlementId = TEXT("Rexa.LasRaices");

    /** DESIGN-GAP: "combat persists" has no locked duration; tune only in M2 playtest. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rexa|Safety")
    float ChildSceneRemovalDelaySeconds = 2.0f;

    /** DESIGN-GAP: silence window for restoring children after the encounter. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rexa|Safety")
    float CombatSilenceBeforeReturnSeconds = 8.0f;

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(Transient)
    TArray<TObjectPtr<ARexaSettlementResident>> SpawnedResidents;

    bool bCombatResponseActive = false;
    bool bChildrenShelteredForPersistentCombat = false;
    float CombatElapsedSeconds = 0.0f;
    float CombatSilenceRemainingSeconds = 0.0f;
    int64 LastAppliedGameMinute = -1;

    bool BuildAnchorRegistry(TMap<FName, ARexaSettlementAnchor*>& OutAnchors) const;
};
