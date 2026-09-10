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
    EExternalAssetMediaKind MediaKind = EExternalAssetMediaKind::MotionPrevisVideo;
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

/**
 * Provider-facing previs catalog.
 *
 * Operator direction limits Higgsfield to motion/performance/cinematic previs. Character looks,
 * boss looks, dungeon/world looks, fauna/flora, ships, props and 3D source-readiness remain in
 * their native Dark Arisen production catalogs and are not submitted to Higgsfield by this owner.
 */
class DARKARISEN_API FExternalAssetProductionCatalog
{
public:
    static constexpr int32 NamedAnimationBriefCount = 11;
    static constexpr int32 SystemAnimationBriefCount = 12;
    static constexpr int32 ResolvedPresentationBriefCount = 14;
    static constexpr int32 RequiredHiggsfieldBriefCount =
        NamedAnimationBriefCount
        + SystemAnimationBriefCount
        + ResolvedPresentationBriefCount;

    static constexpr int32 UnresolvedPresentationIdentityCount = 5;
    static constexpr int32 UnauthoredMinorDungeonIdentityCount = 20;
    static constexpr int32 DeliberateTurnStandingIdentityGapCount = 275;

    static TArray<FExternalAssetProductionBrief> BuildHiggsfieldBriefs();
    static TArray<FExternalAssetProductionDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);

    /** Provider/source authority boundaries. */
    static bool AllowsProviderToCreateCanon() { return false; }
    static bool AllowsGeneratedMediaToCountAsImportedAsset() { return false; }
    static bool AllowsGeneratedMediaToCountAsRuntimeAccepted() { return false; }
    static bool AllowsAutomaticProviderPurchaseOrUpgrade() { return false; }

    /** Higgsfield is deliberately a motion/presentation-previs lane, not a look-development owner. */
    static bool AllowsHiggsfieldStaticVisualProduction() { return false; }
    static bool AllowsHiggsfieldCharacterOrWorldLookProduction() { return false; }
    static bool AllowsHiggsfieldThreeDProductionWithoutVerifiedAction() { return false; }

    /** Mandatory NON_AI_SLOP production boundaries. */
    static bool AllowsGenericFillerPrompt() { return false; }
    static bool AllowsProviderToResolveDesignGap() { return false; }
    static bool AllowsUngroundedDecorativeCompletion() { return false; }
    static bool AllowsUnreviewedProviderDefaultToBecomeIdentity() { return false; }
    static bool RequiresSourceSpecificity() { return true; }
    static bool RequiresFunctionalLogicReview() { return true; }
    static bool RequiresRestraintAndRepetitionReview() { return true; }
};
