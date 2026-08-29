// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TreasureStateComponent.generated.h"

UENUM(BlueprintType)
enum class ETreasureDisposition : uint8
{
    Unresolved,
    Kept,
    Sold,
    Returned
};

USTRUCT(BlueprintType)
struct FTreasureDecisionResult
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bAccepted = false;

    /** Apply through the real Liberation Alliance/network authority; this component does not own network strength. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 NetworkStrengthDelta = 0;

    /** Boolean only by design. The hidden cumulative count is never exposed to UI. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bRecoveryReturnThresholdSatisfied = false;
};

/**
 * Hidden persistence for the authored sell / keep / return question.
 *
 * It does not grant inventory items, money, trust, network strength or secret-site access. Those
 * systems report/consume outcomes through their existing authorities. The cumulative return tally
 * is intentionally private because treasure system.md forbids displaying it.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UTreasureStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    static constexpr int32 ReturnedArtifactNetworkStrengthDelta = 5;

    UFUNCTION(BlueprintCallable, Category="Treasure|Authored")
    bool RegisterArtifact(FName ArtifactId, bool bCountsTowardReturnQuestion);

    UFUNCTION(BlueprintCallable, Category="Treasure|Authored")
    FTreasureDecisionResult ResolveArtifact(FName ArtifactId, ETreasureDisposition Disposition);

    UFUNCTION(BlueprintPure, Category="Treasure|Authored")
    bool HasResolvedArtifact(FName ArtifactId) const;

    /**
     * Supplies SecretDungeonAccessEvidence::bRecoveryReturnThresholdSatisfied without exposing the
     * underlying number. To reconcile treasure_system.md §7.3 with secret_bosses.md §8, a return
     * bias means returned artifacts outnumber all resolved non-returned artifacts (kept + fenced).
     */
    UFUNCTION(BlueprintPure, Category="Treasure|Authored")
    bool IsRecoveryReturnThresholdSatisfied() const;

    /** The exact hidden return count intentionally has no public/UI getter. */

private:
    UPROPERTY(SaveGame)
    TMap<FName, ETreasureDisposition> DispositionByArtifact;

    UPROPERTY(SaveGame)
    TArray<FName> ReturnQuestionArtifactIds;
};
