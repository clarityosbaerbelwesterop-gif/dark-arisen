// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Production/ExternalAssetProductionCatalog.h"

#include "Animation/AnimationProductionCatalog.h"
#include "ContentScale/AuthoredRewardCatalog.h"
#include "Presentation/PresentationProductionCatalog.h"
#include "World/HighmooreWorldProductionCatalog.h"

namespace
{
FName BriefId(const TCHAR* Prefix, const FName SourceId)
{
    return FName(*FString::Printf(TEXT("%s.%s"), Prefix, *SourceId.ToString()));
}

FExternalAssetProductionBrief HiggsfieldBrief(
    const FName StableId,
    const FName SourceRequirementId,
    const FString& DisplayName,
    const FString& GoverningSource,
    const EExternalAssetMediaKind MediaKind,
    const FString& Purpose)
{
    FExternalAssetProductionBrief Result;
    Result.StableId = StableId;
    Result.SourceRequirementId = SourceRequirementId;
    Result.DisplayName = DisplayName;
    Result.GoverningSource = GoverningSource;
    Result.Provider = EExternalAssetProvider::Higgsfield;
    Result.MediaKind = MediaKind;
    Result.EvidenceState = EExternalAssetEvidenceState::RequirementOnly;
    Result.ProductionPurpose = Purpose;
    return Result;
}

EExternalAssetMediaKind AnimationMediaKind(const EAnimationProductionFamily Family)
{
    switch (Family)
    {
    case EAnimationProductionFamily::PerformanceMoment:
        return EExternalAssetMediaKind::FacialPerformanceReference;
    default:
        return EExternalAssetMediaKind::MotionPrevisVideo;
    }
}
}

TArray<FExternalAssetProductionBrief> FExternalAssetProductionCatalog::BuildHiggsfieldBriefs()
{
    TArray<FExternalAssetProductionBrief> Result;
    Result.Reserve(RequiredHiggsfieldBriefCount);

    const auto AddAnimationRequirements = [&Result](const TArray<FAnimationProductionRequirement>& Requirements)
    {
        for (const FAnimationProductionRequirement& Requirement : Requirements)
        {
            Result.Add(HiggsfieldBrief(
                BriefId(TEXT("external.higgsfield.animation"), Requirement.StableId),
                Requirement.StableId,
                Requirement.DisplayName,
                Requirement.GoverningSource,
                AnimationMediaKind(Requirement.Family),
                FString::Printf(
                    TEXT("Non-shipping motion/performance reference only. Preserve this authored acceptance read: %s"),
                    *Requirement.AcceptanceRead)));
        }
    };

    AddAnimationRequirements(FAnimationProductionCatalog::BuildNamedBibleRequirements());
    AddAnimationRequirements(FAnimationProductionCatalog::BuildSystemRequirements());

    for (const FAuthoredCutsceneDefinition& Cutscene : FPresentationProductionCatalog::BuildCutscenes())
    {
        if (!Cutscene.bIdentityResolved)
        {
            continue;
        }

        Result.Add(HiggsfieldBrief(
            BriefId(TEXT("external.higgsfield.presentation"), Cutscene.StableId),
            Cutscene.StableId,
            Cutscene.DisplayName,
            Cutscene.GoverningSource,
            EExternalAssetMediaKind::CinematicPrevisVideo,
            FString::Printf(
                TEXT("Camera/performance previs only. Camera rule: %s Music rule: %s Control ownership remains authoritative in PresentationProductionCatalog."),
                *Cutscene.CameraRule,
                *Cutscene.MusicRule)));
    }

    for (const FHighmooreWorldProductionAnchor& Anchor : FHighmooreWorldProductionCatalog::BuildNamedAnchors())
    {
        Result.Add(HiggsfieldBrief(
            BriefId(TEXT("external.higgsfield.world"), Anchor.StableId),
            Anchor.StableId,
            Anchor.DisplayName,
            Anchor.GoverningSource,
            EExternalAssetMediaKind::ConceptReferenceImage,
            FString::Printf(
                TEXT("Environment/landmark reference exploration only; no coordinates or runtime placement. Source production read: %s"),
                *Anchor.ProductionRead)));
    }

    for (const FAuthoredRewardBinding& Treasure : FAuthoredRewardCatalog::BuildStateTreasureSlots())
    {
        Result.Add(HiggsfieldBrief(
            BriefId(TEXT("external.higgsfield.prop"), Treasure.StableId),
            Treasure.StableId,
            Treasure.DisplayName,
            Treasure.GoverningSource,
            EExternalAssetMediaKind::ConceptReferenceImage,
            FString::Printf(
                TEXT("Physical evidence/prop reference exploration only. Preserve authored origin %s and outcome: %s"),
                *Treasure.OriginStableId.ToString(),
                *Treasure.AuthoredOutcome)));
    }

    for (const FAuthoredRewardBinding& Reward : FAuthoredRewardCatalog::BuildNamedUniqueRewards())
    {
        Result.Add(HiggsfieldBrief(
            BriefId(TEXT("external.higgsfield.prop"), Reward.StableId),
            Reward.StableId,
            Reward.DisplayName,
            Reward.GoverningSource,
            EExternalAssetMediaKind::ConceptReferenceImage,
            FString::Printf(
                TEXT("Unique-item visual reference only. Preserve the authored physical/gameplay identity: %s"),
                *Reward.AuthoredOutcome)));
    }

    return Result;
}

TArray<FExternalAssetProductionDesignGap> FExternalAssetProductionCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.external-assets.approved-character-visual-references"),
            TEXT("External motion/performance generation must not invent a canonical Jake/crew/major-NPC face or costume where no approved visual reference is registered. Neutral mannequins or already-approved reference media are required."),
            TEXT("Docs/DesignAuthority.md; Docs/ANIMATION_PRESENTATION_SOURCE_COMPLETION.md")
        },
        {
            TEXT("design-gap.external-assets.unreal-import"),
            TEXT("No external generation may be counted as a production asset until an actual Unreal import exists and skeleton/material/collision/retarget/Sequencer settings are reviewed as applicable."),
            TEXT("Docs/PRE_RUNNER_ASSET_PRODUCTION_PLAN.md; Docs/ALPHA_DELIVERY_CHECKLIST.md")
        },
        {
            TEXT("design-gap.external-assets.runtime-acceptance"),
            TEXT("ImportedForReview is still not RuntimeAccepted. Runtime acceptance requires evidence from the exact UE 5.5 commit/build, including gameplay ownership and performance where relevant."),
            TEXT("Docs/AGENT_RULES.md; Docs/ALPHA_DELIVERY_CHECKLIST.md")
        },
        {
            TEXT("design-gap.external-assets.shipping-rights"),
            TEXT("Provider output remains non-shipping until cost, licence, commercial-use rights and project provenance requirements are explicitly accepted. Existing credits or a generated preview do not clear shipping rights."),
            TEXT("SECURITY.md; CLAUDE.md")
        }
    };
}

bool FExternalAssetProductionCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    const TArray<FExternalAssetProductionBrief> Briefs = BuildHiggsfieldBriefs();
    if (Briefs.Num() != RequiredHiggsfieldBriefCount)
    {
        OutErrors.Add(FString::Printf(
            TEXT("External Higgsfield brief catalog requires exactly %d source-derived briefs; found %d."),
            RequiredHiggsfieldBriefCount,
            Briefs.Num()));
    }

    int32 AnimationCount = 0;
    int32 PresentationCount = 0;
    int32 WorldCount = 0;
    int32 PropCount = 0;
    TSet<FName> SeenBriefIds;

    for (const FExternalAssetProductionBrief& Brief : Briefs)
    {
        if (Brief.StableId.IsNone()
            || Brief.SourceRequirementId.IsNone()
            || Brief.DisplayName.IsEmpty()
            || Brief.GoverningSource.IsEmpty()
            || Brief.ProductionPurpose.IsEmpty())
        {
            OutErrors.Add(TEXT("Every external asset brief requires stable/source identity, display name, source and production purpose."));
        }

        if (SeenBriefIds.Contains(Brief.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate external asset brief id: %s"), *Brief.StableId.ToString()));
        }
        SeenBriefIds.Add(Brief.StableId);

        if (Brief.Provider != EExternalAssetProvider::Higgsfield)
        {
            OutErrors.Add(FString::Printf(TEXT("Higgsfield brief %s changed provider."), *Brief.StableId.ToString()));
        }

        if (Brief.EvidenceState != EExternalAssetEvidenceState::RequirementOnly
            || !Brief.ProviderJobId.IsEmpty()
            || !Brief.MediaReference.IsEmpty()
            || !Brief.UnrealAssetPath.IsEmpty()
            || Brief.bProviderCostApproved
            || Brief.bShippingRightsCleared)
        {
            OutErrors.Add(FString::Printf(
                TEXT("Brief %s falsely claims generation/import/rights evidence. Current source catalog must stay RequirementOnly until real evidence is recorded."),
                *Brief.StableId.ToString()));
        }

        const FString Id = Brief.StableId.ToString();
        if (Id.StartsWith(TEXT("external.higgsfield.animation."))) ++AnimationCount;
        else if (Id.StartsWith(TEXT("external.higgsfield.presentation."))) ++PresentationCount;
        else if (Id.StartsWith(TEXT("external.higgsfield.world."))) ++WorldCount;
        else if (Id.StartsWith(TEXT("external.higgsfield.prop."))) ++PropCount;
        else OutErrors.Add(FString::Printf(TEXT("Unknown external brief family: %s"), *Id));
    }

    if (AnimationCount != NamedAnimationBriefCount + SystemAnimationBriefCount)
    {
        OutErrors.Add(TEXT("External animation brief coverage drifted from AnimationProductionCatalog."));
    }
    if (PresentationCount != ResolvedPresentationBriefCount)
    {
        OutErrors.Add(TEXT("External presentation brief coverage must include only the fourteen resolved cutscene identities."));
    }
    if (WorldCount != HighmooreWorldBriefCount)
    {
        OutErrors.Add(TEXT("External Highmoore world brief coverage drifted from the twelve source-backed anchors."));
    }
    if (PropCount != StateTreasureBriefCount + UniqueRewardBriefCount)
    {
        OutErrors.Add(TEXT("External prop brief coverage must remain nine State Treasures plus the currently grounded unique reward."));
    }

    if (AllowsProviderToCreateCanon()
        || AllowsGeneratedMediaToCountAsImportedAsset()
        || AllowsGeneratedMediaToCountAsRuntimeAccepted()
        || AllowsAutomaticProviderPurchaseOrUpgrade())
    {
        OutErrors.Add(TEXT("External provider authority may not create canon, self-promote media or purchase provider access."));
    }

    if (BuildDesignGaps().Num() != 4)
    {
        OutErrors.Add(TEXT("External asset production must retain all four import/runtime/rights/reference design gaps."));
    }

    return OutErrors.IsEmpty();
}
