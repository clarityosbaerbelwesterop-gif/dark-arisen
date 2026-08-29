// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** External providers are production assistants. None of them is design authority. */
enum class EExternalAssetProvider : uint8
{
    Higgsfield,
    AIVoiceGenerator
};

enum class EExternalAssetMediaKind : uint8
{
    ConceptReferenceImage,
    MotionPrevisVideo,
    CinematicPrevisVideo,
    FacialPerformanceReference,
    CandidateMesh3D,
    CandidateRiggedMesh3D,
    CandidateMotionClip,
    VoiceAudition
};

enum class EExternalAssetEvidenceState : uint8
{
    RequirementOnly,
    PrevisGenerated,
    CandidateAssetGenerated,
    ImportedForReview,
    RuntimeAccepted
};

struct FExternalAssetProductionBrief
{
    FName StableId;
    FName SourceRequirementId;
    FString DisplayName;
    FString GoverningSource;
    EExternalAssetProvider Provider = EExternalAssetProvider::Higgsfield;
    EExternalAssetMediaKind MediaKind = EExternalAssetMediaKind::ConceptReferenceImage;
    EExternalAssetEvidenceState EvidenceState = EExternalAssetEvidenceState::RequirementOnly;
    FString ProductionPurpose;
    FString ProviderJobId;
    FString MediaReference;
    FString UnrealAssetPath;
    bool bProviderCostApproved = false;
    bool bShippingRightsCleared = false;
};

struct FExternalAssetProductionDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

class DARKARISEN_API FExternalAssetProductionCatalog
{
public:
    static constexpr int32 NamedAnimationBriefCount = 11;
    static constexpr int32 SystemAnimationBriefCount = 12;
    static constexpr int32 ResolvedPresentationBriefCount = 14;
    static constexpr int32 GroundedDungeonBriefCount = 40;
    static constexpr int32 WorldRegionBriefCount = 8;
    static constexpr int32 HighmooreWorldBriefCount = 12;
    static constexpr int32 ProviderReadyCharacterBriefCount = 8;
    static constexpr int32 StateTreasureBriefCount = 9;
    static constexpr int32 UniqueRewardBriefCount = 1;
    static constexpr int32 UnresolvedPresentationIdentityCount = 5;
    static constexpr int32 UnauthoredMinorDungeonIdentityCount = 20;
    static constexpr int32 DeliberateTurnStandingIdentityGapCount = 275;

    static constexpr int32 RequiredHiggsfieldBriefCount =
        NamedAnimationBriefCount
        + SystemAnimationBriefCount
        + ResolvedPresentationBriefCount
        + GroundedDungeonBriefCount
        + WorldRegionBriefCount
        + HighmooreWorldBriefCount
        + ProviderReadyCharacterBriefCount
        + StateTreasureBriefCount
        + UniqueRewardBriefCount;

    static TArray<FExternalAssetProductionBrief> BuildHiggsfieldBriefs();
    static TArray<FExternalAssetProductionDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);

    static bool AllowsProviderToCreateCanon() { return false; }
    static bool AllowsGeneratedMediaToCountAsImportedAsset() { return false; }
    static bool AllowsGeneratedMediaToCountAsRuntimeAccepted() { return false; }
    static bool AllowsAutomaticProviderPurchaseOrUpgrade() { return false; }
};
