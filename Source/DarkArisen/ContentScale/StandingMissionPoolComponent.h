// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ContentScale/ContentScaleManifestDataAsset.h"
#include "StandingMissionPoolComponent.generated.h"

USTRUCT(BlueprintType)
struct FStandingMissionVariantDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName StableId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName RegionId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EM7StandingMissionType Type = EM7StandingMissionType::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
    int32 FullPay = 0;

    /** Standing gain never degrades with repetition. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
    int32 StandingGain = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString GoverningSource;
};

USTRUCT(BlueprintType)
struct FStandingMissionCompletionResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    bool bCompleted = false;

    UPROPERTY(BlueprintReadOnly)
    int32 PaidAmount = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 StandingGain = 0;

    UPROPERTY(BlueprintReadOnly)
    float AppliedPayMultiplier = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    int32 SameRegionTypeRunNumber = 0;
};

/**
 * Finite authored M7 Standing pool. It never synthesizes a mission. Once a region/type pool has no
 * unused authored variant, it is exhausted and must remain unavailable until authored state changes.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UStandingMissionPoolComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UStandingMissionPoolComponent();

    /** Exactly 147 approved authored variants are required for full M7 content readiness. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="M7|Standing")
    TArray<FStandingMissionVariantDefinition> AuthoredVariants;

    /**
     * Approved same-region/type pay curve for runs 1..6. Canon fixes only run 1 = 100% and run 6 =
     * 60%; intermediate values are authored tuning and are therefore not invented in C++.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="M7|Standing")
    TArray<float> SameRegionTypePayMultipliers;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category="M7|Standing")
    TSet<FName> CompletedVariantIds;

    UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category="M7|Standing")
    TMap<FName, int32> CompletedCountByRegionType;

    bool ValidateAuthoredPool(TArray<FString>& OutErrors) const;

    /** Returns null when the finite authored pool for this region/type is exhausted. */
    const FStandingMissionVariantDefinition* FindNextAvailableVariant(FName RegionId, EM7StandingMissionType Type) const;

    bool IsPoolExhausted(FName RegionId, EM7StandingMissionType Type) const;

    bool CompleteVariant(FName StableId, FStandingMissionCompletionResult& OutResult);

    float GetPayMultiplierForNextRun(FName RegionId, EM7StandingMissionType Type) const;

    int32 GetCompletedCount(FName RegionId, EM7StandingMissionType Type) const;

private:
    static FName MakeRegionTypeKey(FName RegionId, EM7StandingMissionType Type);
    const FStandingMissionVariantDefinition* FindDefinition(FName StableId) const;
    bool IsPayCurveValid() const;
};
