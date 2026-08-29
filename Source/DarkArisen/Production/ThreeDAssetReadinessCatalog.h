// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EThreeDAssetCandidateKind : uint8
{
    RiggedCharacter,
    RiggedCreature,
    StaticProp,
    ModularEnvironmentKit,
    ShipInteriorKit,
    ShipExteriorHull,
    FoliageOrVegetation
};

enum class EThreeDSourceReadiness : uint8
{
    CandidateGeometryReady,
    ReferenceOnly,
    Blocked
};

struct FThreeDAssetReadinessRecord
{
    FName StableId;
    FName SourceRequirementId;
    FString DisplayName;
    FString GoverningSource;
    EThreeDAssetCandidateKind CandidateKind = EThreeDAssetCandidateKind::StaticProp;
    EThreeDSourceReadiness SourceReadiness = EThreeDSourceReadiness::Blocked;

    /** What a future 3D provider/modeler is allowed to construct from current source. */
    FString GeometryScope;

    /** Why the record is not a final shipping/runtime asset, or why it is blocked. */
    FString EvidenceOrAuthorityBoundary;

    /**
     * Current connected-provider fact. A source can be geometry-ready while there is no verified
     * connected 3D generation action. These flags may advance only from real provider/UE evidence.
     */
    bool bVerifiedConnectedProvider3DAction = false;
    bool bCandidateGenerated = false;
    bool bImportedForReview = false;
    bool bRuntimeAccepted = false;

    /** False where the visual source is useful but does not itself select final gameplay identity. */
    bool bFinalGameplayIdentityAuthoritative = true;

    /** False for dungeon/world kits because source-backed modular geometry is not reviewed .umap placement. */
    bool bFinalLevelPlacementAuthoritative = true;
};

struct FThreeDAssetReadinessDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Cross-family source-readiness register for later 3D production. It deliberately separates
 * sufficient geometry description from provider capability, Unreal import and runtime acceptance.
 */
class DARKARISEN_API FThreeDAssetReadinessCatalog
{
public:
    static constexpr int32 CharacterRecordCount = 9;
    static constexpr int32 BossVisualRecordCount = 21;
    static constexpr int32 ShipRecordCount = 6;
    static constexpr int32 FaunaRecordCount = 24;
    static constexpr int32 FloraRecordCount = 9;
    static constexpr int32 PropRecordCount = 10;
    static constexpr int32 DungeonEnvironmentRecordCount = 40;
    static constexpr int32 HighmooreAnchorRecordCount = 12;
    static constexpr int32 RequiredRecordCount =
        CharacterRecordCount
        + BossVisualRecordCount
        + ShipRecordCount
        + FaunaRecordCount
        + FloraRecordCount
        + PropRecordCount
        + DungeonEnvironmentRecordCount
        + HighmooreAnchorRecordCount;

    static constexpr int32 CandidateGeometryReadyCount = 105;
    static constexpr int32 ReferenceOnlyCount = 14;
    static constexpr int32 BlockedCount = 12;

    static TArray<FThreeDAssetReadinessRecord> BuildRecords();
    static TArray<FThreeDAssetReadinessDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);

    /** Current connected Higgsfield preflight did not expose a usable 3D-generation action. */
    static bool HasVerifiedConnectedProvider3DAction() { return false; }
};
