// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ContentScale/ContentScaleManifestDataAsset.h"

/**
 * Authored region ownership. CrossNetwork is reserved for the Fifth Connection Site because the
 * governing source deliberately defines it through all five networks rather than one region.
 */
enum class EDungeonCatalogRegion : uint8
{
    RexaMoran,
    Fjordlund,
    Ashenmoor,
    Sea,
    Colonial,
    Region06,
    Highmoore,
    CrossNetwork,
    CrystalPassage
};

enum class EDungeonCatalogDiscovery : uint8
{
    Environmental,
    Cartographic,
    Documentary,
    Relational,
    Hybrid,
    WithheldOrUnresolved
};

enum class EDungeonCatalogPuzzleLanguage : uint8
{
    None,
    Rexan,
    Fjordlund,
    Ashenmoor,
    Colonial,
    HighmoorePractices,
    Region06Unreliable,
    Hybrid,
    WithheldOrUnresolved
};

enum class EDungeonCatalogBossState : uint8
{
    Authored,
    MultipleAuthored,
    Environmental,
    SurvivorConditional,
    WarReactive,
    NoBoss,
    Withheld,
    Unresolved
};

struct FAuthoredDungeonCatalogEntry
{
    FName StableId;
    FString DisplayName;
    EDungeonCatalogRegion Region = EDungeonCatalogRegion::RexaMoran;
    EContentDungeonTier Tier = EContentDungeonTier::TierB;
    FString GoverningSource;
    FString SecondarySource;
    EDungeonCatalogDiscovery Discovery = EDungeonCatalogDiscovery::WithheldOrUnresolved;
    EDungeonCatalogPuzzleLanguage PuzzleLanguage = EDungeonCatalogPuzzleLanguage::WithheldOrUnresolved;
    EDungeonCatalogBossState BossState = EDungeonCatalogBossState::Unresolved;
    FString BossOrEncounterName;

    /** True for the six trust/map/route/artifact sites from `secret bosses.md`. */
    bool bSecretSite = false;

    /** True for Old Fortress content that resolves into an Alliance Bastion rather than a normal clear. */
    bool bOldFortress = false;

    /** Canon requires Tier B+ to open a route back from the inside. */
    bool bReturnShortcutFromInside = false;

    /** These are hard global prohibitions, represented positively so drift is obvious in tests. */
    bool bHasMapMarker = false;
    bool bHasAmbientDungeonMusic = false;
    bool bContainsChildRemains = false;

    /**
     * The regional catalog calls the Fourth Fortress Tier D, while `secret bosses.md` includes the
     * same physical site among the six Tier E secret sites. No local ruling is invented here.
     */
    bool bTierClassificationConflict = false;

    /**
     * Exact per-site durations are generally not authored. Tier standards live in the design
     * philosophy; zero means there is no exact per-site value to hard-code.
     */
    float ExactMaximumMinutes = 0.0f;
};

struct FDungeonCatalogDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * C++ authority for what the Bible actually names. It deliberately does not synthesize the
 * remaining Region 06 Tier-E slot or twenty unnamed minor-site definitions.
 */
class DARKARISEN_API FAuthoredDungeonCatalog
{
public:
    static constexpr int32 RequiredNamedSiteCount = 41;
    static constexpr int32 RequiredMinorSiteCount = 20;
    static constexpr int32 RequiredGlobalDungeonCount = 61;
    static constexpr int32 RequiredCrystalCavesCarveoutCount = 1;

    /** Forty uniquely named regional/cross-network sites that can be grounded today. */
    static TArray<FAuthoredDungeonCatalogEntry> BuildKnownNamedSites();

    /** Crystal Caves is represented separately because the newest global document calls it a category of one. */
    static FAuthoredDungeonCatalogEntry BuildCrystalCavesCarveout();

    /** Canon gaps that must be resolved rather than filled by AI or procedural generation. */
    static TArray<FDungeonCatalogDesignGap> BuildDesignGaps();

    static bool ValidateKnownCatalog(TArray<FString>& OutErrors);
};
