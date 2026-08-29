// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Production/ExternalAssetProductionCatalog.h"

#include "Animation/AnimationProductionCatalog.h"
#include "ContentScale/AuthoredDungeonCatalog.h"
#include "ContentScale/AuthoredDungeonProductionProfile.h"
#include "ContentScale/AuthoredRewardCatalog.h"
#include "Presentation/PresentationProductionCatalog.h"
#include "World/AuthoredWorldRegionRegistry.h"
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

const TCHAR* BoolRead(const bool bValue)
{
    return bValue ? TEXT("yes") : TEXT("no");
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
                    TEXT("Non-shipping motion/performance reference only. Preserve this authored acceptance read literally: %s Do not add moves, beats, dialogue, camera language or timing that contradicts the cited source."),
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
                TEXT("Camera/performance previs only. Camera rule: %s Music rule: %s Control ownership remains authoritative in PresentationProductionCatalog. Do not turn protected playable material into a cinematic or invent dialogue."),
                *Cutscene.CameraRule,
                *Cutscene.MusicRule)));
    }

    for (const FAuthoredDungeonProductionProfile& Profile : FAuthoredDungeonProductionProfiles::BuildAllKnownProfiles())
    {
        FAuthoredDungeonCatalogEntry Dungeon;
        if (!FAuthoredDungeonCatalog::TryGetKnownSite(Profile.StableId, Dungeon))
        {
            continue;
        }

        const FString ImageRead = Profile.bImageWithheld
            ? TEXT("The unforgettable image is withheld/unresolved; do not invent it.")
            : FString::Printf(TEXT("Unforgettable image: %s"), *Profile.UnforgettableImage);
        const FString BossRead = Profile.bBossWithheldOrUnresolved
            ? TEXT("The boss/bottom identity is withheld or unresolved; do not invent it.")
            : FString::Printf(TEXT("Boss/bottom: %s"), *Profile.BossOrBottomDetail);
        const FString RewardRead = Profile.bRewardWithheldOrUnresolved
            ? TEXT("The reward is withheld or unresolved; do not invent it.")
            : FString::Printf(TEXT("Reward/evidence: %s"), *Profile.RewardDetail);

        Result.Add(HiggsfieldBrief(
            BriefId(TEXT("external.higgsfield.dungeon"), Profile.StableId),
            Profile.StableId,
            Dungeon.DisplayName,
            Profile.GoverningSource,
            EExternalAssetMediaKind::ConceptReferenceImage,
            FString::Printf(
                TEXT("Grounded named-dungeon production reference only. Access/discovery: %s Puzzle language/detail: %s Primary hazard: %s %s %s %s No extra rooms, bosses, characters, lore, rewards, coordinates, UI markers, child remains or ambient dungeon music may be invented."),
                *Profile.AccessAndDiscovery,
                *Profile.PuzzleLanguageDetail,
                *Profile.HazardDetail,
                *ImageRead,
                *BossRead,
                *RewardRead)));
    }

    for (const FAuthoredWorldRegionDefinition& Region : FAuthoredWorldRegionRegistry::BuildAll())
    {
        Result.Add(HiggsfieldBrief(
            BriefId(TEXT("external.higgsfield.region"), Region.StableId),
            Region.StableId,
            Region.DisplayName,
            Region.GoverningSource,
            EExternalAssetMediaKind::ConceptReferenceImage,
            FString::Printf(
                TEXT("Source-backed regional environment reference only. Registry coverage: settlements=%s, dungeons=%s, war=%s, naval=%s, houses=%s. Read the cited governing source for visual identity. Do not invent coordinates, unnamed settlements, landmarks, travel links, quest markers, minimap language or runtime map placement."),
                BoolRead(Region.bHasSettlements),
                BoolRead(Region.bHasDungeonContent),
                BoolRead(Region.bHasWarContent),
                BoolRead(Region.bHasNavalContent),
                BoolRead(Region.bHasHouseContent))));
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
                TEXT("Environment/landmark reference exploration only; no coordinates or runtime placement. Preserve the authored production read literally: %s"),
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
            TEXT("External motion/performance generation must not invent a canonical Jake/crew/major-NPC face or costume where no approved visual reference is registered. Character briefs require source-by-source visual extraction first."),
            TEXT("Docs/DesignAuthority.md; docs/design/style_bible.md; character/NPC design sources")
        },
        {
            TEXT("design-gap.external-assets.provider-3d-path"),
            TEXT("The connected Higgsfield model discovery currently exposes image/video production but did not return a usable 3D mesh model. Do not claim GLB/rigged-mesh generation until a supported provider action/model is actually available."),
            TEXT("Docs/PRE_RUNNER_ASSET_PRODUCTION_PLAN.md; connected-provider preflight 2026-08-30")
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
    int32 DungeonCount = 0;
    int32 RegionCount = 0;
    int32 HighmooreWorldCount = 0;
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
        else if (Id.StartsWith(TEXT("external.higgsfield.dungeon."))) ++DungeonCount;
        else if (Id.StartsWith(TEXT("external.higgsfield.region."))) ++RegionCount;
        else if (Id.StartsWith(TEXT("external.higgsfield.world."))) ++HighmooreWorldCount;
        else if (Id.StartsWith(TEXT("external.higgsfield.prop."))) ++PropCount;
        else OutErrors.Add(FString::Printf(TEXT("Unknown external brief family: %s"), *Id));

        const FString SourceId = Brief.SourceRequirementId.ToString();
        if (SourceId.StartsWith(TEXT("turn-gap."))
            || SourceId.StartsWith(TEXT("standing-gap."))
            || SourceId.Contains(TEXT("minor-slot")))
        {
            OutErrors.Add(FString::Printf(TEXT("Unauthored mission/minor-dungeon identity leaked into provider briefs: %s"), *SourceId));
        }
    }

    if (AnimationCount != NamedAnimationBriefCount + SystemAnimationBriefCount)
    {
        OutErrors.Add(TEXT("External animation brief coverage drifted from AnimationProductionCatalog."));
    }
    if (PresentationCount != ResolvedPresentationBriefCount)
    {
        OutErrors.Add(TEXT("External presentation brief coverage must include only the fourteen resolved cutscene identities."));
    }
    if (DungeonCount != GroundedDungeonBriefCount
        || GroundedDungeonBriefCount != FAuthoredDungeonProductionProfiles::BuildAllKnownProfiles().Num())
    {
        OutErrors.Add(TEXT("External dungeon-look coverage must track exactly the forty grounded named production profiles."));
    }
    if (RegionCount != WorldRegionBriefCount
        || WorldRegionBriefCount != FAuthoredWorldRegionRegistry::BuildAll().Num())
    {
        OutErrors.Add(TEXT("External region-look coverage must track exactly the eight authored world-region identities."));
    }
    if (HighmooreWorldCount != HighmooreWorldBriefCount)
    {
        OutErrors.Add(TEXT("External Highmoore world brief coverage drifted from the twelve source-backed anchors."));
    }
    if (PropCount != StateTreasureBriefCount + UniqueRewardBriefCount)
    {
        OutErrors.Add(TEXT("External prop brief coverage must remain nine State Treasures plus the currently grounded unique reward."));
    }

    if (UnresolvedPresentationIdentityCount != FPresentationProductionCatalog::RequiredCutsceneCount - FPresentationProductionCatalog::ResolvedCutsceneIdentityCount)
    {
        OutErrors.Add(TEXT("External provider boundary drifted from the five unresolved final-act cutscene identities."));
    }
    if (UnauthoredMinorDungeonIdentityCount != FAuthoredDungeonCatalog::RequiredMinorSiteCount)
    {
        OutErrors.Add(TEXT("External provider boundary drifted from the twenty unauthored minor-dungeon identities."));
    }

    if (AllowsProviderToCreateCanon()
        || AllowsGeneratedMediaToCountAsImportedAsset()
        || AllowsGeneratedMediaToCountAsRuntimeAccepted()
        || AllowsAutomaticProviderPurchaseOrUpgrade())
    {
        OutErrors.Add(TEXT("External provider authority may not create canon, self-promote media or purchase provider access."));
    }

    if (BuildDesignGaps().Num() != 5)
    {
        OutErrors.Add(TEXT("External asset production must retain all five character/3D/import/runtime/rights design gaps."));
    }

    return OutErrors.IsEmpty();
}
