// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Production/ThreeDAssetReadinessCatalog.h"

#include "ContentScale/AuthoredDungeonProductionProfile.h"
#include "ContentScale/AuthoredRewardCatalog.h"
#include "Production/BossVisualProductionCatalog.h"
#include "Production/CharacterVisualProductionCatalog.h"
#include "Production/FaunaVisualProductionCatalog.h"
#include "Production/FloraVisualProductionCatalog.h"
#include "Production/ShipVisualProductionCatalog.h"
#include "World/HighmooreWorldProductionCatalog.h"

namespace
{
FName RecordId(const TCHAR* Prefix, const FName SourceId)
{
    return FName(*FString::Printf(TEXT("%s.%s"), Prefix, *SourceId.ToString()));
}

FThreeDAssetReadinessRecord Record(
    const FName StableId,
    const FName SourceRequirementId,
    const FString& DisplayName,
    const FString& GoverningSource,
    const EThreeDAssetCandidateKind CandidateKind,
    const EThreeDSourceReadiness SourceReadiness,
    const FString& GeometryScope,
    const FString& Boundary,
    const bool bFinalGameplayIdentityAuthoritative = true,
    const bool bFinalLevelPlacementAuthoritative = true)
{
    FThreeDAssetReadinessRecord Result;
    Result.StableId = StableId;
    Result.SourceRequirementId = SourceRequirementId;
    Result.DisplayName = DisplayName;
    Result.GoverningSource = GoverningSource;
    Result.CandidateKind = CandidateKind;
    Result.SourceReadiness = SourceReadiness;
    Result.GeometryScope = GeometryScope;
    Result.EvidenceOrAuthorityBoundary = Boundary;
    Result.bFinalGameplayIdentityAuthoritative = bFinalGameplayIdentityAuthoritative;
    Result.bFinalLevelPlacementAuthoritative = bFinalLevelPlacementAuthoritative;
    return Result;
}

bool IsFaunaReferenceOnly3D(const FName StableId)
{
    static const TSet<FName> ReferenceOnly = {
        FName(TEXT("fauna.legendary.memory-stag")),
        FName(TEXT("fauna.legendary.threshold-fox")),
        FName(TEXT("fauna.legendary.deep-one"))
    };
    return ReferenceOnly.Contains(StableId);
}
}

TArray<FThreeDAssetReadinessRecord> FThreeDAssetReadinessCatalog::BuildRecords()
{
    TArray<FThreeDAssetReadinessRecord> Result;
    Result.Reserve(RequiredRecordCount);

    for (const FCharacterVisualProductionBrief& Character : FCharacterVisualProductionCatalog::BuildMajorCharacterBriefs())
    {
        EThreeDSourceReadiness Readiness = EThreeDSourceReadiness::Blocked;
        FString Boundary = Character.ExplicitUnknowns;

        if (Character.StableId == FName(TEXT("character.jake-harlow")))
        {
            Readiness = EThreeDSourceReadiness::ReferenceOnly;
            Boundary = TEXT("Jake's physical body reference is grounded, but the canonical primary costume sheet is incomplete. A full final rigged-character candidate would silently choose clothing canon; keep 3D production to body/proportion reference until the costume authority is locked.");
        }
        else if (Character.bProviderReferenceReady)
        {
            Readiness = EThreeDSourceReadiness::CandidateGeometryReady;
            Boundary = TEXT("Source is sufficient for a non-shipping rigged visual candidate. Skeleton topology, deformation, facial rig, groom, cloth setup, LODs and Unreal retarget acceptance remain separate production evidence.");
        }

        Result.Add(Record(
            RecordId(TEXT("3d.character"), Character.StableId),
            Character.StableId,
            Character.DisplayName,
            Character.GoverningSource,
            EThreeDAssetCandidateKind::RiggedCharacter,
            Readiness,
            FString::Printf(TEXT("Physical facts: %s Wardrobe/objects: %s"), *Character.PhysicalFacts, *Character.WardrobeAndObjects),
            Boundary));
    }

    for (const FBossVisualProductionBrief& Boss : FBossVisualProductionCatalog::BuildDeepDiveBossBriefs())
    {
        const EThreeDSourceReadiness Readiness = Boss.bProviderReferenceReady
            ? EThreeDSourceReadiness::CandidateGeometryReady
            : EThreeDSourceReadiness::Blocked;

        Result.Add(Record(
            RecordId(TEXT("3d.boss-visual"), Boss.StableId),
            Boss.StableId,
            Boss.DisplayName,
            Boss.GoverningSource,
            EThreeDAssetCandidateKind::RiggedCreature,
            Readiness,
            FString::Printf(TEXT("Visual candidate scope: %s Equipment/silhouette: %s"), *Boss.VisualFacts, *Boss.EquipmentOrSilhouette),
            Boss.bProviderReferenceReady
                ? TEXT("A rigged/sculpt candidate may preserve the visual source, but the deep-dive visual catalog is not boss-count/gameplay authority. No mesh candidate can promote itself into the current Tier-1, dungeon, secret or final-act gameplay registers.")
                : Boss.ExplicitUnknowns,
            false));
    }

    for (const FShipVisualProductionBrief& Ship : FShipVisualProductionCatalog::BuildLaLiberacionBriefs())
    {
        const bool bExterior = Ship.Kind == EShipVisualProductionKind::ExteriorHull;
        Result.Add(Record(
            RecordId(TEXT("3d.ship"), Ship.StableId),
            Ship.StableId,
            Ship.DisplayName,
            Ship.GoverningSource,
            bExterior ? EThreeDAssetCandidateKind::ShipExteriorHull : EThreeDAssetCandidateKind::ShipInteriorKit,
            Ship.bProviderReferenceReady ? EThreeDSourceReadiness::CandidateGeometryReady : EThreeDSourceReadiness::Blocked,
            FString::Printf(TEXT("Authored ship geometry scope: %s Functional objects: %s"), *Ship.AuthoredVisualFacts, *Ship.FunctionalObjects),
            Ship.AntiInventionBoundary,
            true,
            false));
    }

    for (const FFaunaVisualProductionBrief& Fauna : FFaunaVisualProductionCatalog::BuildAllBriefs())
    {
        EThreeDSourceReadiness Readiness = EThreeDSourceReadiness::Blocked;
        FString Boundary = Fauna.AntiInventionBoundary;
        if (Fauna.bProviderReferenceReady)
        {
            if (IsFaunaReferenceOnly3D(Fauna.StableId))
            {
                Readiness = EThreeDSourceReadiness::ReferenceOnly;
                Boundary = FString::Printf(
                    TEXT("2D/reference language is usable, but one final full-body mesh would collapse authored variability or concealment. %s"),
                    *Fauna.AntiInventionBoundary);
            }
            else
            {
                Readiness = EThreeDSourceReadiness::CandidateGeometryReady;
                Boundary = FString::Printf(
                    TEXT("A non-shipping creature mesh/rig candidate is source-grounded. Final anatomy review, skeleton, locomotion, hit/collision shapes, materials, LODs and runtime ecology remain separate. %s"),
                    *Fauna.AntiInventionBoundary);
            }
        }

        Result.Add(Record(
            RecordId(TEXT("3d.fauna"), Fauna.StableId),
            Fauna.StableId,
            Fauna.DisplayName,
            Fauna.GoverningSource,
            EThreeDAssetCandidateKind::RiggedCreature,
            Readiness,
            FString::Printf(TEXT("Visual facts: %s Motion read: %s"), *Fauna.AuthoredVisualFacts, *Fauna.BehaviorAndMotionRead),
            Boundary));
    }

    for (const FFloraVisualProductionBrief& Flora : FFloraVisualProductionCatalog::BuildBriefs())
    {
        EThreeDSourceReadiness Readiness = EThreeDSourceReadiness::Blocked;
        FString Boundary = Flora.AntiInventionBoundary;
        if (Flora.bProviderReferenceReady)
        {
            if (Flora.StableId == FName(TEXT("flora.region06.infinite-branch")))
            {
                Readiness = EThreeDSourceReadiness::ReferenceOnly;
                Boundary = TEXT("Impossible-perspective concept work is source-backed, but the source explicitly refuses a single stable full silhouette/dimension set. Do not bake the Infinite Branch into one canonical static mesh.");
            }
            else
            {
                Readiness = EThreeDSourceReadiness::CandidateGeometryReady;
                Boundary = FString::Printf(
                    TEXT("A non-shipping vegetation/static candidate is source-grounded. Wind response, procedural variation, materials, Nanite/foliage settings, collision and runtime phenomena remain Unreal-side evidence. %s"),
                    *Flora.AntiInventionBoundary);
            }
        }

        Result.Add(Record(
            RecordId(TEXT("3d.flora"), Flora.StableId),
            Flora.StableId,
            Flora.DisplayName,
            Flora.GoverningSource,
            EThreeDAssetCandidateKind::FoliageOrVegetation,
            Readiness,
            FString::Printf(TEXT("Visual facts: %s Environment: %s"), *Flora.AuthoredVisualFacts, *Flora.EnvironmentRead),
            Boundary,
            true,
            false));
    }

    for (const FAuthoredRewardBinding& Treasure : FAuthoredRewardCatalog::BuildStateTreasureSlots())
    {
        Result.Add(Record(
            RecordId(TEXT("3d.prop"), Treasure.StableId),
            Treasure.StableId,
            Treasure.DisplayName,
            Treasure.GoverningSource,
            EThreeDAssetCandidateKind::StaticProp,
            EThreeDSourceReadiness::ReferenceOnly,
            Treasure.AuthoredOutcome,
            TEXT("The document identity and physical origin are authored, but cover/binding/paper dimensions, wear pattern, handwriting/print layout and other unique prop appearance are not locked. Build a concept/reference sheet before unique final geometry/materials.")));
    }

    for (const FAuthoredRewardBinding& Reward : FAuthoredRewardCatalog::BuildNamedUniqueRewards())
    {
        Result.Add(Record(
            RecordId(TEXT("3d.prop"), Reward.StableId),
            Reward.StableId,
            Reward.DisplayName,
            Reward.GoverningSource,
            EThreeDAssetCandidateKind::StaticProp,
            EThreeDSourceReadiness::CandidateGeometryReady,
            Reward.AuthoredOutcome,
            TEXT("The Crystal Katana has a separate physical source sheet locking its form, 74 cm blade, 1.1 kg mass, translucent crystal-adjacent blade, no visible bevel and motion-dependent light. Mesh/material candidate still requires UE material, collision, socket and weapon integration review.")));
    }

    for (const FAuthoredDungeonProductionProfile& Dungeon : FAuthoredDungeonProductionProfiles::BuildAllKnownProfiles())
    {
        Result.Add(Record(
            RecordId(TEXT("3d.dungeon-kit"), Dungeon.StableId),
            Dungeon.StableId,
            Dungeon.StableId.ToString(),
            Dungeon.GoverningSource,
            EThreeDAssetCandidateKind::ModularEnvironmentKit,
            EThreeDSourceReadiness::CandidateGeometryReady,
            FString::Printf(
                TEXT("Modular environment-kit scope only. Puzzle language/detail: %s Primary hazard: %s Unforgettable image: %s"),
                *Dungeon.PuzzleLanguageDetail,
                *Dungeon.HazardDetail,
                Dungeon.bImageWithheld ? TEXT("withheld — do not invent") : *Dungeon.UnforgettableImage),
            TEXT("This readiness covers reusable architecture/prop/material kit candidates only. It does not define room coordinates, final layout, encounter placement, navmesh, lighting, streaming cells or a reviewed .umap."),
            true,
            false));
    }

    for (const FHighmooreWorldProductionAnchor& Anchor : FHighmooreWorldProductionCatalog::BuildNamedAnchors())
    {
        Result.Add(Record(
            RecordId(TEXT("3d.highmoore-kit"), Anchor.StableId),
            Anchor.StableId,
            Anchor.DisplayName,
            Anchor.GoverningSource,
            EThreeDAssetCandidateKind::ModularEnvironmentKit,
            EThreeDSourceReadiness::CandidateGeometryReady,
            Anchor.ProductionRead,
            TEXT("Source is sufficient for a non-shipping architecture/environment kit direction, not coordinates or reviewed level placement. Generic lesser-House substitution remains forbidden."),
            true,
            false));
    }

    return Result;
}

TArray<FThreeDAssetReadinessDesignGap> FThreeDAssetReadinessCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.3d.connected-provider-action"),
            TEXT("Current connected Higgsfield discovery did not expose a usable 3D generation action/model. Source geometry readiness does not imply a provider can generate a GLB today."),
            TEXT("Docs/PRE_RUNNER_ASSET_PRODUCTION_PLAN.md; connected-provider preflight 2026-08-30")
        },
        {
            TEXT("design-gap.3d.character-final-sheets"),
            TEXT("Jake lacks a complete primary costume sheet; Elowen lacks a canonical physical sheet; Ethan and Draven remain blocked by Phase-11 story authority. These are explicit full-rig blockers rather than invitations to invent."),
            TEXT("Production/CharacterVisualProductionCatalog; Docs/DesignAuthority.md")
        },
        {
            TEXT("design-gap.3d.boss-gameplay-authority"),
            TEXT("Nineteen legacy/deep-dive boss visuals are detailed enough for non-shipping sculpt/rig candidates, but that catalog is not boss gameplay/count authority and cannot select final encounters."),
            TEXT("Production/BossVisualProductionCatalog; Docs/M7_TIER1_BOSS_REGISTER.md; Docs/DesignAuthority.md")
        },
        {
            TEXT("design-gap.3d.variable-or-hidden-geometry"),
            TEXT("Memory Stag, Threshold Fox, Deep One and Infinite Branch allow bounded visual reference but not one final canonical full-body/static geometry. Final Wolf is even more strongly player-history dependent and remains blocked."),
            TEXT("Production/FaunaVisualProductionCatalog; Production/FloraVisualProductionCatalog")
        },
        {
            TEXT("design-gap.3d.state-treasure-prop-appearance"),
            TEXT("Nine State Treasure document identities and origins are authored, but their unique physical appearance sheets are not. They remain reference-only for final prop geometry/material production."),
            TEXT("ContentScale/AuthoredRewardCatalog; treasure system.md Section 4")
        },
        {
            TEXT("design-gap.3d.level-placement"),
            TEXT("Forty dungeon modular kits and twelve Highmoore anchor kits can be prepared from source, but no such kit is a reviewed level. Coordinates, final layout, navmesh, lighting, streaming, collision and performance remain Unreal/editor evidence."),
            TEXT("ContentScale/AuthoredDungeonProductionProfile; World/HighmooreWorldProductionCatalog; Docs/ALPHA_DELIVERY_CHECKLIST.md")
        }
    };
}

bool FThreeDAssetReadinessCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();
    const TArray<FThreeDAssetReadinessRecord> Records = BuildRecords();

    if (Records.Num() != RequiredRecordCount)
    {
        OutErrors.Add(FString::Printf(TEXT("3D readiness register requires %d records; found %d."), RequiredRecordCount, Records.Num()));
    }

    int32 CandidateReady = 0;
    int32 ReferenceOnly = 0;
    int32 Blocked = 0;
    TSet<FName> Seen;

    for (const FThreeDAssetReadinessRecord& Entry : Records)
    {
        if (Entry.StableId.IsNone()
            || Entry.SourceRequirementId.IsNone()
            || Entry.DisplayName.IsEmpty()
            || Entry.GoverningSource.IsEmpty()
            || Entry.GeometryScope.IsEmpty()
            || Entry.EvidenceOrAuthorityBoundary.IsEmpty())
        {
            OutErrors.Add(TEXT("Every 3D readiness record requires identity, source, geometry scope and an evidence/authority boundary."));
        }

        if (Seen.Contains(Entry.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate 3D readiness id: %s"), *Entry.StableId.ToString()));
        }
        Seen.Add(Entry.StableId);

        switch (Entry.SourceReadiness)
        {
        case EThreeDSourceReadiness::CandidateGeometryReady: ++CandidateReady; break;
        case EThreeDSourceReadiness::ReferenceOnly: ++ReferenceOnly; break;
        case EThreeDSourceReadiness::Blocked: ++Blocked; break;
        }

        if (Entry.bVerifiedConnectedProvider3DAction
            || Entry.bCandidateGenerated
            || Entry.bImportedForReview
            || Entry.bRuntimeAccepted)
        {
            OutErrors.Add(FString::Printf(TEXT("3D record %s falsely claims provider/generation/import/runtime evidence."), *Entry.StableId.ToString()));
        }
    }

    if (CandidateReady != CandidateGeometryReadyCount
        || ReferenceOnly != ReferenceOnlyCount
        || Blocked != BlockedCount)
    {
        OutErrors.Add(FString::Printf(
            TEXT("3D readiness split must remain %d candidate-ready / %d reference-only / %d blocked; found %d / %d / %d."),
            CandidateGeometryReadyCount,
            ReferenceOnlyCount,
            BlockedCount,
            CandidateReady,
            ReferenceOnly,
            Blocked));
    }

    const auto FindBySource = [&Records](const TCHAR* SourceId) -> const FThreeDAssetReadinessRecord*
    {
        const FName Wanted(SourceId);
        return Records.FindByPredicate([Wanted](const FThreeDAssetReadinessRecord& Entry)
        {
            return Entry.SourceRequirementId == Wanted;
        });
    };

    for (const TCHAR* BlockedSource : {
        TEXT("character.elowen-arion"),
        TEXT("character.ethan-harlow"),
        TEXT("character.draven-voss"),
        TEXT("boss-visual.ethan-harlow"),
        TEXT("boss-visual.draven-voss"),
        TEXT("ship-visual.la-liberacion.exterior"),
        TEXT("fauna.legendary.final-wolf"),
        TEXT("flora.story.ethans-grove")})
    {
        const FThreeDAssetReadinessRecord* Entry = FindBySource(BlockedSource);
        if (Entry == nullptr || Entry->SourceReadiness != EThreeDSourceReadiness::Blocked)
        {
            OutErrors.Add(FString::Printf(TEXT("Authority-blocked 3D source leaked into candidate readiness: %s"), BlockedSource));
        }
    }

    for (const TCHAR* ReferenceOnlySource : {
        TEXT("character.jake-harlow"),
        TEXT("fauna.legendary.memory-stag"),
        TEXT("fauna.legendary.threshold-fox"),
        TEXT("fauna.legendary.deep-one"),
        TEXT("flora.region06.infinite-branch")})
    {
        const FThreeDAssetReadinessRecord* Entry = FindBySource(ReferenceOnlySource);
        if (Entry == nullptr || Entry->SourceReadiness != EThreeDSourceReadiness::ReferenceOnly)
        {
            OutErrors.Add(FString::Printf(TEXT("Variable/incomplete 3D source must remain reference-only: %s"), ReferenceOnlySource));
        }
    }

    for (const FThreeDAssetReadinessRecord& Entry : Records)
    {
        const FString Id = Entry.StableId.ToString();
        if ((Id.StartsWith(TEXT("3d.dungeon-kit.")) || Id.StartsWith(TEXT("3d.highmoore-kit.")))
            && Entry.bFinalLevelPlacementAuthoritative)
        {
            OutErrors.Add(FString::Printf(TEXT("Environment kit %s may not self-promote into final level placement."), *Id));
        }
    }

    if (HasVerifiedConnectedProvider3DAction())
    {
        OutErrors.Add(TEXT("3D readiness catalog may not claim a connected provider action until provider discovery is re-verified and recorded."));
    }

    if (BuildDesignGaps().Num() != 6)
    {
        OutErrors.Add(TEXT("3D readiness must retain provider, character, boss, variable-geometry, prop and level-placement design gaps."));
    }

    return OutErrors.IsEmpty();
}
