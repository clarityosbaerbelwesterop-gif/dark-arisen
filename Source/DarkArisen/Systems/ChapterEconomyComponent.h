// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Systems/ProgressionEconomyComponent.h"
#include "ChapterEconomyComponent.generated.h"

USTRUCT(BlueprintType)
struct FChapterLedger
{
    GENERATED_BODY()

    /** Signed doubloon values. Income is normally positive; upkeep/draw values are <= 0. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int64 HoldingIncome = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int64 ArmyUpkeep = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int64 GarrisonUpkeep = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int64 ConstructionDraw = 0;

    int64 NetDoubloons() const
    {
        return HoldingIncome + ArmyUpkeep + GarrisonUpkeep + ConstructionDraw;
    }
};

/**
 * M4 economy cadence. Holdings/armies resolve per chapter rather than per day, preventing
 * time itself from becoming a resource meter. No interest, investment or compounding API exists.
 * Legendary work and Highmoore reconstruction remain explicit sinks, never passive upgrades.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UChapterEconomyComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UChapterEconomyComponent();

    UFUNCTION(BlueprintCallable, Category="Economy|Chapter")
    bool SetPendingLedger(const FChapterLedger& Ledger);

    UFUNCTION(BlueprintCallable, Category="Economy|Chapter")
    bool ResolveChapterLedger(int32 Chapter, UProgressionEconomyComponent* Economy);

    UFUNCTION(BlueprintCallable, Category="Economy|Legendary")
    bool PayLegendaryTierCost(
        UProgressionEconomyComponent* Economy,
        EDarkArisenCurrency Currency,
        int64 Amount,
        FName WorkId);

    UFUNCTION(BlueprintCallable, Category="Economy|Reconstruction")
    bool PayReconstructionSilver(
        UProgressionEconomyComponent* Economy,
        int64 SilverMarks,
        FName ProjectId);

    UFUNCTION(BlueprintCallable, Category="Economy|Crew")
    bool PayCrewShare(
        UProgressionEconomyComponent* Economy,
        int64 Doubloons,
        int32 Chapter);

    UFUNCTION(BlueprintPure, Category="Economy|Chapter")
    FChapterLedger GetPendingLedger() const { return PendingLedger; }

private:
    static bool IsValidLedger(const FChapterLedger& Ledger);

    UPROPERTY(SaveGame)
    FChapterLedger PendingLedger;

    UPROPERTY(SaveGame)
    int32 LastResolvedChapter = 0;

    UPROPERTY(SaveGame)
    TSet<FName> PaidLegendaryWork;

    UPROPERTY(SaveGame)
    TSet<FName> PaidReconstructionProjects;

    UPROPERTY(SaveGame)
    TSet<int32> PaidCrewShareChapters;
};
