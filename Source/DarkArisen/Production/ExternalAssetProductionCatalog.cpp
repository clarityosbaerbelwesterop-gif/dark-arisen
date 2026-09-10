// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Production/ExternalAssetProductionCatalog.h"

#include "Animation/AnimationProductionCatalog.h"
#include "Presentation/PresentationProductionCatalog.h"

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
    return Family == EAnimationProductionFamily::PerformanceMoment
        ? EExternalAssetMediaKind::FacialPerformanceReference
        : EExternalAssetMediaKind::MotionPrevisVideo;
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
                    TEXT("Non-shipping motion/performance reference only. Preserve the authored acceptance read literally: %s Do not add moves, beats, dialogue, camera language or timing that contradicts the cited source. This output is not an AnimMontage, AnimNotify, retargeted clip or runtime animation."),
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
                TEXT("Camera/performance previs only. Camera rule: %s Music rule: %s Control ownership remains authoritative in PresentationProductionCatalog. Do not turn protected playable material into a cinematic, invent dialogue, create a missing final-act identity or treat the video as a Sequencer asset."),
                *Cutscene.CameraRule,
                *Cutscene.MusicRule)));
    }

    return Result;
}

TArray<FExternalAssetProductionDesignGap> FExternalAssetProductionCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.external-assets.higgsfield-plan-access"),
            TEXT("The connected Higgsfield workspace is Free with 10 credits and no Unlimited access. Two Seedance 2.0 Mini submissions were rejected before job creation with 'Requires basic plan or higher'. No upgrade or purchase is authorised."),
            TEXT("Docs/PRE_RUNNER_ASSET_PRODUCTION_PLAN.md; connected-provider preflight 2026-08-30")
        },
        {
            TEXT("design-gap.external-assets.final-act-presentation"),
            TEXT("Five final-act cutscene identities remain unresolved by current presentation authority and are excluded from Higgsfield. A provider cannot invent their story beats merely to reach nineteen."),
            TEXT("Presentation/PresentationProductionCatalog; Docs/DesignAuthority.md")
        },
        {
            TEXT("design-gap.external-assets.unreal-import"),
            TEXT("No Higgsfield motion/cinematic previs may be counted as a production asset until actual Unreal animation/Sequencer import or recreation exists and ownership/timing/settings are reviewed."),
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
            TEXT("Higgsfield previs catalog requires %d briefs; found %d."),
            RequiredHiggsfieldBriefCount,
            Briefs.Num()));
    }

    TSet<FName> StableIds;
    int32 MotionOrPerformance = 0;
    int32 Cinematic = 0;

    for (const FExternalAssetProductionBrief& Brief : Briefs)
    {
        if (Brief.StableId.IsNone() || Brief.SourceRequirementId.IsNone())
        {
            OutErrors.Add(TEXT("External previs brief has an empty stable/source requirement id."));
            continue;
        }

        if (StableIds.Contains(Brief.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate external previs stable id: %s"), *Brief.StableId.ToString()));
        }
        StableIds.Add(Brief.StableId);

        if (Brief.GoverningSource.IsEmpty() || Brief.ProductionPurpose.IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("External previs brief %s lacks source/purpose."), *Brief.StableId.ToString()));
        }

        if (Brief.Provider != EExternalAssetProvider::Higgsfield)
        {
            OutErrors.Add(FString::Printf(TEXT("Higgsfield previs catalog contains non-Higgsfield provider: %s"), *Brief.StableId.ToString()));
        }

        if (Brief.EvidenceState != EExternalAssetEvidenceState::RequirementOnly
            || !Brief.ProviderJobId.IsEmpty()
            || !Brief.MediaReference.IsEmpty()
            || !Brief.UnrealAssetPath.IsEmpty()
            || Brief.bProviderCostApproved
            || Brief.bShippingRightsCleared)
        {
            OutErrors.Add(FString::Printf(TEXT("External previs brief %s fabricates evidence/cost/rights state."), *Brief.StableId.ToString()));
        }

        const FString SourceId = Brief.SourceRequirementId.ToString();
        if (SourceId.StartsWith(TEXT("turn-gap."))
            || SourceId.StartsWith(TEXT("standing-gap."))
            || SourceId.Contains(TEXT("minor-slot")))
        {
            OutErrors.Add(FString::Printf(TEXT("Unauthored content leaked into Higgsfield previs: %s"), *SourceId));
        }

        switch (Brief.MediaKind)
        {
        case EExternalAssetMediaKind::MotionPrevisVideo:
        case EExternalAssetMediaKind::FacialPerformanceReference:
            ++MotionOrPerformance;
            break;
        case EExternalAssetMediaKind::CinematicPrevisVideo:
            ++Cinematic;
            break;
        default:
            OutErrors.Add(FString::Printf(
                TEXT("Higgsfield is motion/presentation-only; disallowed media kind on %s."),
                *Brief.StableId.ToString()));
            break;
        }
    }

    if (MotionOrPerformance != NamedAnimationBriefCount + SystemAnimationBriefCount)
    {
        OutErrors.Add(FString::Printf(
            TEXT("Expected %d animation/performance briefs; found %d."),
            NamedAnimationBriefCount + SystemAnimationBriefCount,
            MotionOrPerformance));
    }

    if (Cinematic != ResolvedPresentationBriefCount)
    {
        OutErrors.Add(FString::Printf(
            TEXT("Expected %d resolved cinematic briefs; found %d."),
            ResolvedPresentationBriefCount,
            Cinematic));
    }

    return OutErrors.IsEmpty();
}
