// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ContentScaleManifestDataAsset.generated.h"

UENUM(BlueprintType)
enum class EScaledContentKind : uint8
{
    Thread,
    Turn,
    StandingVariant
};

UENUM(BlueprintType)
enum class EContentDungeonTier : uint8
{
    Minor,
    TierA,
    TierB,
    TierC,
    TierD,
    TierE,
    CrystalCaves
};

USTRUCT(BlueprintType)
struct FScaledContentEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName StableId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EScaledContentKind Kind = EScaledContentKind::Turn;

    /** Exact design source that authors this entry. Empty sources fail the manifest. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString GoverningSource;
};

USTRUCT(BlueprintType)
struct FDungeonManifestEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName StableId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EContentDungeonTier Tier = EContentDungeonTier::Minor;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString GoverningSource;

    /** Tier B+ must open a route back from the inside. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bReturnShortcutFromInside = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bHasMapMarker = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bHasAmbientMusic = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bContainsChildRemains = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1.0"))
    float AuthoredMaximumMinutes = 45.0f;
};

USTRUCT(BlueprintType)
struct FTier1BossManifestEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName StableId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString GoverningSource;
};

USTRUCT(BlueprintType)
struct FPresentationMomentManifestEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName StableId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString GoverningSource;

    /** True only for one of the canonical nineteen authored cutscenes. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bSequencerOwnedCutscene = false;
};

/**
 * M7 completeness authority. The asset validates authored definitions; it does not generate missing
 * content. A structurally incomplete catalog fails closed rather than inventing filler missions,
 * dungeons, bosses or cinematics.
 */
UCLASS(BlueprintType)
class DARKARISEN_API UContentScaleManifestDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="M7|Content")
    TArray<FScaledContentEntry> QuestAndMissionEntries;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="M7|Content")
    TArray<FDungeonManifestEntry> Dungeons;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="M7|Content")
    TArray<FTier1BossManifestEntry> Tier1Bosses;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="M7|Content")
    TArray<FPresentationMomentManifestEntry> PresentationMoments;

    bool ValidateManifest(TArray<FString>& OutErrors) const;

    static constexpr int32 RequiredNamedDungeons = 41;
    static constexpr int32 RequiredMinorDungeons = 20;
    static constexpr int32 RequiredDungeonTotal = 61;
    static constexpr int32 RequiredThreads = 17;
    static constexpr int32 RequiredTurns = 132;
    static constexpr int32 RequiredStandingVariants = 147;
    static constexpr int32 RequiredTier1Bosses = 9;
    static constexpr int32 RequiredCutscenes = 19;
    static constexpr int32 RequiredProtectedPlayableMoments = 22;

private:
    static bool IsTierBOrAbove(EContentDungeonTier Tier);
    static TSet<FName> GetCanonicalTier1BossIds();
};
