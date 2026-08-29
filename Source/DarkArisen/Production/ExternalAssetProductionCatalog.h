// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** External providers are production assistants. None of them is design authority. */
enum class EExternalAssetProvider : uint8
{
    Higgsfield,
    AIVoiceGenerator
};

/** What an external output is useful for before Unreal acceptance. */
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

/**
 * Evidence state is deliberately monotonic. A provider generation cannot jump directly to
 * ImportedForReview or RuntimeAccepted: those require actual Unreal-side evidence.
 */
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

    /** What the provider may help communicate; never a replacement for the governing source. */
    FString ProductionPurpose;

    /** Empty until a real provider job is created. */
    FString ProviderJobId;

    /** Empty until real external media exists. Never store credentials or signed upload URLs. */
    FString MediaReference;

    /** Empty until an actual UE import exists on the repository/runtime side. */
    FString UnrealAssetPath;

    /** Rights/cost clearance is independent from whether a preview was generated. */
    bool bProviderCostApproved = false;
    bool bShippingRightsCleared = false;
};

struct FExternalAssetProductionDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Finite provider-facing brief catalog built from existing Dark Arisen source authorities.
 * It does not call providers, create files, import assets or mutate runtime state.
 */
class DARKARISEN_API FExternalAssetProductionCatalog
{
public:
    static constexpr int32 NamedAnimationBriefCount = 11;
    static constexpr int32 SystemAnimationBriefCount = 12;
    static constexpr int32 ResolvedPresentationBriefCount = 14;
    static constexpr int32 HighmooreWorldBriefCount = 12;
    static constexpr int32 StateTreasureBriefCount = 9;
    static constexpr int32 UniqueRewardBriefCount = 1;
    static constexpr int32 RequiredHiggsfieldBriefCount =
        NamedAnimationBriefCount
        + SystemAnimationBriefCount
        + ResolvedPresentationBriefCount
        + HighmooreWorldBriefCount
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
