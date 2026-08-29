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
enum class EM7StandingMissionType : uint8
{
    None,
    Escort,
    ConvoyRaid,
    Recovery,
    Champion,
    Transport,
    PrivateerCommission,
    RoadWork,
    Hunt,
    Salvage
};

UENUM(BlueprintType)
enum class EContentDungeonTier : uint8
{
    /** Canonical Tier A minor site. */
    Minor,

    /**
     * Legacy source value retained for serialization compatibility only. The design corpus defines
     * Tier A as the minor-site tier; new authored records must use Minor and validation rejects TierA.
     */
    TierA,
    TierB,
    TierC,
    TierD,
    TierE,

    /** Category-of-one passage, tracked separately from the 41 named + 20 minor global count. */
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

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EM7StandingMissionType StandingType = EM7StandingMissionType::None;

    /** Exact design source that authors this entry. Empty sources fail the manifest. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString GoverningSource;

    /** Hard prohibition. M7 content is authored and finite; no radiant/generated entry is accepted. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bGeneratedOrRadiant = false;
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

    /** Tier B+ and the Crystal Caves carve-out must open a route back from the inside. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bReturnShortcutFromInside = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bHasMapMarker = false;

    /** Ambient dungeon score is forbidden. Authored boss/image cues are outside this flag. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bHasAmbientMusic = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bContainsChildRemains = false;

    /**
     * DESIGN-GAP: most individual dungeons have only tier-level duration ranges, not exact per-site
     * maxima. Zero means no exact per-site maximum is authored; validation applies only global/tier
     * ceilings. Crystal Caves is the sole category with an explicit 90-120 minute range.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0"))
    float AuthoredMaximumMinutes = 0.0f;
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

    /** True only for one of the canonical twenty-two moments that must remain player-controlled. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bProtectedPlayableMoment = false;
};

USTRUCT(BlueprintType)
struct FContentAuthorityDependency
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName StableId;

    /** Existing M2-M6 state owner. This is an integration reference, never a parallel state system. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName RequiredAuthority;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString GoverningSource;
};

USTRUCT(BlueprintType)
struct FDialogueProductionReadinessRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName RoleId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName DialogueLockId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString CastingRole;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString PronunciationReference;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bDialogueLocked = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bSubtitleReady = false;
};

USTRUCT(BlueprintType)
struct FCreditsReadinessManifest
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bNamesAndOrderApproved = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bAttributionComplete = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString MusicSource;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString MusicLicenceReference;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bMusicSourceApproved = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bMusicLicenceApproved = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bMusicCostApproved = false;

    /** Dark Arisen score is non-reactive by design. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bMusicNonReactive = false;

    bool IsProductionReady() const;
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
    FName ManifestRevision;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="M7|Content")
    TArray<FScaledContentEntry> QuestAndMissionEntries;

    /** 41 named + 20 minor global sites, plus one separately counted Crystal Caves carve-out. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="M7|Content")
    TArray<FDungeonManifestEntry> Dungeons;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="M7|Content")
    TArray<FTier1BossManifestEntry> Tier1Bosses;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="M7|Content")
    TArray<FPresentationMomentManifestEntry> PresentationMoments;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="M7|Integration")
    TArray<FContentAuthorityDependency> AuthorityDependencies;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="M7|Dialogue")
    TArray<FDialogueProductionReadinessRecord> DialogueReadiness;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="M7|Credits")
    FCreditsReadinessManifest Credits;

    bool ValidateManifest(TArray<FString>& OutErrors) const;
    bool ValidateDialogueReadiness(TArray<FString>& OutErrors) const;

    static constexpr int32 RequiredNamedDungeons = 41;
    static constexpr int32 RequiredMinorDungeons = 20;
    static constexpr int32 RequiredDungeonTotal = 61;
    static constexpr int32 RequiredCrystalCavesCarveouts = 1;
    static constexpr int32 RequiredDungeonManifestRecords = RequiredDungeonTotal + RequiredCrystalCavesCarveouts;
    static constexpr int32 RequiredThreads = 17;
    static constexpr int32 RequiredTurns = 132;
    static constexpr int32 RequiredStandingVariants = 147;
    static constexpr int32 RequiredTier1Bosses = 9;
    static constexpr int32 RequiredCutscenes = 19;
    static constexpr int32 RequiredProtectedPlayableMoments = 22;

private:
    static bool IsTierBOrAbove(EContentDungeonTier Tier);
    static TSet<FName> GetCanonicalTier1BossIds();
    static TMap<EM7StandingMissionType, int32> GetRequiredStandingTypeCounts();
};
