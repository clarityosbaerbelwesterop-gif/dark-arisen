// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Production/PreMainQuestReadinessCatalog.h"

#include "Production/CharacterVisualProductionCatalog.h"
#include "Production/ExternalAssetProductionCatalog.h"
#include "Production/FaunaVisualProductionCatalog.h"
#include "Production/FloraVisualProductionCatalog.h"
#include "Production/ShipVisualProductionCatalog.h"
#include "Production/ThreeDAssetReadinessCatalog.h"
#include "Production/Tier1BossVisualReadinessCatalog.h"

namespace
{
FPreMainQuestReadinessRecord Record(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const TCHAR* GoverningSource,
    const int32 Registered,
    const int32 Ready,
    const int32 ReferenceOnly,
    const int32 Gaps,
    const TCHAR* Boundary)
{
    FPreMainQuestReadinessRecord Result;
    Result.StableId = StableId;
    Result.DisplayName = DisplayName;
    Result.GoverningSource = GoverningSource;
    Result.RegisteredCount = Registered;
    Result.ProductionReadyCount = Ready;
    Result.ReferenceOnlyCount = ReferenceOnly;
    Result.ExplicitGapCount = Gaps;
    Result.Boundary = Boundary;
    return Result;
}

void AppendSubcatalogErrors(
    const TCHAR* Label,
    const TArray<FString>& SubErrors,
    TArray<FString>& OutErrors)
{
    for (const FString& Error : SubErrors)
    {
        OutErrors.Add(FString::Printf(TEXT("%s: %s"), Label, *Error));
    }
}
}

TArray<FPreMainQuestReadinessRecord> FPreMainQuestReadinessCatalog::BuildRecords()
{
    return {
        Record(
            TEXT("pre-main.dungeons"),
            TEXT("Dungeon identities"),
            TEXT("dungeon system global.md; regional dungeon catalog.md; AuthoredDungeonCatalog"),
            RequiredDungeonSites,
            GroundedDungeonSites,
            0,
            DungeonIdentityGaps,
            TEXT("Exactly 40 named identities are grounded. Twenty minor identities plus one named Region-06 identity remain deliberately unauthored; Crystal Caves is tracked separately as the category-of-one passage contract.")),

        Record(
            TEXT("pre-main.threads"),
            TEXT("Named Threads"),
            TEXT("side quest catalog.md; Thread runtime owners"),
            RequiredThreads,
            GroundedThreads,
            0,
            0,
            TEXT("All seventeen named Threads have source/runtime owners; individual thread sub-gaps remain fail-closed where their governing documents withhold details.")),

        Record(
            TEXT("pre-main.turns"),
            TEXT("Turns"),
            TEXT("side quest catalog.md; MissionContentAuthority"),
            RequiredTurns,
            GroundedTurns,
            0,
            TurnIdentityGaps,
            TEXT("Only three Turn identities are individually authored enough for production. The remaining 129 receive neutral gap IDs only and are never generated as filler.")),

        Record(
            TEXT("pre-main.standing"),
            TEXT("Standing variants"),
            TEXT("side quest catalog.md; Standing mission authority"),
            RequiredStandingVariants,
            GroundedStandingVariants,
            0,
            StandingIdentityGaps,
            TEXT("Only Rexa.Standing.Salvage.SanTelmoBell is individually complete. The other 146 identities remain deliberately unauthored.")),

        Record(
            TEXT("pre-main.tier1-gameplay"),
            TEXT("Tier-1 gameplay identities"),
            TEXT("Docs/M7_TIER1_BOSS_REGISTER.md; colonial war bosses.md"),
            RequiredTier1Bosses,
            RequiredTier1Bosses,
            0,
            0,
            TEXT("The Nine Who Hold are the authoritative Tier-1 set; kill/spare/avoid special cases remain source-owned.")),

        Record(
            TEXT("pre-main.tier1-looks"),
            TEXT("Tier-1 full physical look sheets"),
            TEXT("Tier1BossVisualReadinessCatalog; colonial war bosses.md"),
            RequiredTier1Bosses,
            0,
            0,
            CurrentTier1FullLookGaps,
            TEXT("Current boss sources lock role, weapons, arena and performance but do not lock nine complete face/body/hair/wardrobe sheets. No unrelated legacy boss look may fill these gaps.")),

        Record(
            TEXT("pre-main.world-regions"),
            TEXT("World-region identities"),
            TEXT("AuthoredWorldRegionRegistry"),
            WorldRegionCount,
            WorldRegionCount,
            0,
            0,
            TEXT("All eight production-region identities are source-registered. Final maps, coordinates, streaming, navmesh and art remain later Unreal production evidence.")),

        Record(
            TEXT("pre-main.highmoore-anchors"),
            TEXT("Highmoore named anchors"),
            TEXT("HighmooreWorldProductionCatalog"),
            HighmooreAnchorCount,
            HighmooreAnchorCount,
            0,
            0,
            TEXT("Twelve named anchors are source-grounded as production direction; no coordinates or generic lesser-House substitutes are invented.")),

        Record(
            TEXT("pre-main.physical-rewards"),
            TEXT("State Treasures + named unique reward"),
            TEXT("AuthoredRewardCatalog; treasure system.md; crystal katana.md"),
            StateTreasureCount + NamedUniqueRewardCount,
            StateTreasureCount + NamedUniqueRewardCount,
            0,
            0,
            TEXT("Nine State Treasure identities/origins and the current named unique reward are authored. Unique State Treasure appearance remains reference-only in the 3D layer where physical sheets are missing.")),

        Record(
            TEXT("pre-main.characters"),
            TEXT("Major character full-geometry readiness"),
            TEXT("CharacterVisualProductionCatalog; Docs/DesignAuthority.md"),
            MajorCharacterRecordCount,
            MajorCharacterGeometryReady,
            MajorCharacterReferenceOnly,
            MajorCharacterBlocked,
            TEXT("Five crew rigs are source-ready, Jake is reference-only pending a locked primary costume, Elowen lacks a complete physical sheet, and Ethan/Draven remain blocked by current story-authority conflict.")),

        Record(
            TEXT("pre-main.ship"),
            TEXT("La Liberación visual geometry readiness"),
            TEXT("ShipVisualProductionCatalog"),
            ShipVisualRecordCount,
            ShipGeometryReady,
            0,
            ShipBlocked,
            TEXT("Five deck/interior spaces are geometry-ready as source packages. The exterior is blocked because hull class, dimensions, mast/sail plan, colors and figurehead are not locked.")),

        Record(
            TEXT("pre-main.fauna"),
            TEXT("Fauna geometry readiness"),
            TEXT("FaunaVisualProductionCatalog"),
            FaunaRecordCount,
            FaunaGeometryReady,
            FaunaReferenceOnly,
            FaunaBlocked,
            TEXT("Variable/hidden anatomy and under-specified Highmoore variants remain unresolved instead of being normalized into generic creature meshes.")),

        Record(
            TEXT("pre-main.flora"),
            TEXT("Flora geometry readiness"),
            TEXT("FloraVisualProductionCatalog"),
            FloraRecordCount,
            FloraGeometryReady,
            FloraReferenceOnly,
            FloraBlocked,
            TEXT("Infinite Branch remains variable; Ethan's Grove remains current-story-authority blocked; broad flora scale targets do not authorize filler species.")),

        Record(
            TEXT("pre-main.3d-readiness"),
            TEXT("Cross-family 3D source readiness"),
            TEXT("ThreeDAssetReadinessCatalog"),
            ThreeDRecordCount,
            ThreeDGeometryReady,
            ThreeDReferenceOnly,
            ThreeDBlocked,
            TEXT("This is readiness for future modeling, not generated meshes. Higgsfield is not the general 3D owner and no connected 3D action is claimed.")),

        Record(
            TEXT("pre-main.animation"),
            TEXT("Animation production requirements"),
            TEXT("AnimationProductionCatalog"),
            AnimationRequirementCount,
            AnimationRequirementCount,
            0,
            0,
            TEXT("Eleven named + twelve system requirements are source-grounded. Actual AnimSequences/Montages/Notifies/retarget/runtime evidence remain later Unreal work.")),

        Record(
            TEXT("pre-main.presentation"),
            TEXT("Cutscene identities"),
            TEXT("cutscene catalog.md; PresentationProductionCatalog"),
            RequiredCutsceneCount,
            ResolvedCutsceneCount,
            0,
            UnresolvedFinalActCutsceneCount,
            TEXT("Fourteen identities are resolved. Final-act slots 15-19 remain unresolved and cannot be invented by Higgsfield or a source implementation.")),

        Record(
            TEXT("pre-main.higgsfield-previs"),
            TEXT("Higgsfield motion/cinematic previs briefs"),
            TEXT("ExternalAssetProductionCatalog; Docs/HIGGSFIELD_ASSET_PRODUCTION_MATRIX.md"),
            HiggsfieldPrevisBriefCount,
            HiggsfieldPrevisBriefCount,
            0,
            0,
            TEXT("Exactly 37 RequirementOnly briefs exist: 23 animation/performance + 14 resolved cinematic. Provider plan currently blocks actual Seedance job creation without upgrade; zero generated media is claimed."))
    };
}

TArray<FPreMainQuestStopReason> FPreMainQuestReadinessCatalog::BuildMainQuestStopReasons()
{
    return {
        {
            TEXT("main-quest-gap.structure-is-scaffold"),
            TEXT("Phase-11 main story structure explicitly defines chapter architecture rather than complete mission content. Chapter titles/functions/unlocks are insufficient to author a mission's stable identity, trigger, playable actions, physical route, completion and persistence consequences."),
            TEXT("main story structure.md")
        },
        {
            TEXT("main-quest-gap.ethan-draven-authority-conflict"),
            TEXT("Current Phase-11 structure ends Chapter 1 with Ethan dead and later uses Ethan's Grove, while older Ethan/Draven character and boss material describes the superseded captured/betrayal/final-boss branch. DesignAuthority requires a rewrite or explicit restoration before that material can drive current main-story implementation."),
            TEXT("Docs/DesignAuthority.md; main story structure.md")
        },
        {
            TEXT("main-quest-gap.crimson-armada-final-act-source"),
            TEXT("main story structure.md routes Chapters 8 and 10 to bosses/crimson_armada.md for detailed endgame plot, but repository search does not provide a current matching authoritative Crimson Armada final-act document. Do not reconstruct it from legacy boss files."),
            TEXT("main story structure.md; Docs/DesignAuthority.md")
        },
        {
            TEXT("main-quest-gap.final-act-cutscenes"),
            TEXT("Five of the nineteen authored cutscene slots remain unresolved final-act identities. Their absence is presentation evidence that the endgame mission sequence is not yet sufficiently authored for implementation."),
            TEXT("cutscene catalog.md; PresentationProductionCatalog")
        }
    };
}

bool FPreMainQuestReadinessCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    // The closure register is an aggregate gate. Validate the source/readiness catalogs it summarizes
    // so a child catalog cannot drift while the top-level hard-coded accounting still appears closed.
    TArray<FString> SubErrors;

    if (!FCharacterVisualProductionCatalog::Validate(SubErrors))
    {
        AppendSubcatalogErrors(TEXT("CharacterVisualProductionCatalog"), SubErrors, OutErrors);
    }
    SubErrors.Reset();

    if (!FShipVisualProductionCatalog::Validate(SubErrors))
    {
        AppendSubcatalogErrors(TEXT("ShipVisualProductionCatalog"), SubErrors, OutErrors);
    }
    SubErrors.Reset();

    if (!FFaunaVisualProductionCatalog::Validate(SubErrors))
    {
        AppendSubcatalogErrors(TEXT("FaunaVisualProductionCatalog"), SubErrors, OutErrors);
    }
    SubErrors.Reset();

    if (!FFloraVisualProductionCatalog::Validate(SubErrors))
    {
        AppendSubcatalogErrors(TEXT("FloraVisualProductionCatalog"), SubErrors, OutErrors);
    }
    SubErrors.Reset();

    if (!FTier1BossVisualReadinessCatalog::Validate(SubErrors))
    {
        AppendSubcatalogErrors(TEXT("Tier1BossVisualReadinessCatalog"), SubErrors, OutErrors);
    }
    SubErrors.Reset();

    if (!FThreeDAssetReadinessCatalog::Validate(SubErrors))
    {
        AppendSubcatalogErrors(TEXT("ThreeDAssetReadinessCatalog"), SubErrors, OutErrors);
    }
    SubErrors.Reset();

    if (!FExternalAssetProductionCatalog::Validate(SubErrors))
    {
        AppendSubcatalogErrors(TEXT("ExternalAssetProductionCatalog"), SubErrors, OutErrors);
    }

    if (FCharacterVisualProductionCatalog::BuildMajorCharacterBriefs().Num() != MajorCharacterRecordCount)
    {
        OutErrors.Add(TEXT("Major-character readiness count drifted from CharacterVisualProductionCatalog."));
    }

    if (FShipVisualProductionCatalog::BuildLaLiberacionBriefs().Num() != ShipVisualRecordCount)
    {
        OutErrors.Add(TEXT("La Liberación readiness count drifted from ShipVisualProductionCatalog."));
    }

    if (FFaunaVisualProductionCatalog::BuildAllBriefs().Num() != FaunaRecordCount)
    {
        OutErrors.Add(TEXT("Fauna readiness count drifted from FaunaVisualProductionCatalog."));
    }

    if (FFloraVisualProductionCatalog::BuildBriefs().Num() != FloraRecordCount)
    {
        OutErrors.Add(TEXT("Flora readiness count drifted from FloraVisualProductionCatalog."));
    }

    if (FTier1BossVisualReadinessCatalog::BuildAll().Num() != RequiredTier1Bosses)
    {
        OutErrors.Add(TEXT("Current Tier-1 visual readiness count drifted from the Nine Who Hold."));
    }

    if (FThreeDAssetReadinessCatalog::BuildRecords().Num() != ThreeDRecordCount)
    {
        OutErrors.Add(TEXT("Cross-family 3D readiness count drifted from ThreeDAssetReadinessCatalog."));
    }

    if (FExternalAssetProductionCatalog::BuildHiggsfieldBriefs().Num() != HiggsfieldPrevisBriefCount)
    {
        OutErrors.Add(TEXT("Higgsfield motion/cinematic brief count drifted from ExternalAssetProductionCatalog."));
    }

    const TArray<FPreMainQuestReadinessRecord> Records = BuildRecords();
    if (Records.Num() != RequiredFamilyCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Pre-main readiness requires %d families; found %d."), RequiredFamilyCount, Records.Num()));
    }

    TSet<FName> StableIds;
    for (const FPreMainQuestReadinessRecord& Entry : Records)
    {
        if (Entry.StableId.IsNone() || Entry.GoverningSource.IsEmpty() || Entry.Boundary.IsEmpty())
        {
            OutErrors.Add(TEXT("Pre-main readiness entry lacks stable id, source or boundary."));
            continue;
        }

        if (StableIds.Contains(Entry.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate pre-main readiness id: %s"), *Entry.StableId.ToString()));
        }
        StableIds.Add(Entry.StableId);

        if (Entry.RegisteredCount < 0
            || Entry.ProductionReadyCount < 0
            || Entry.ReferenceOnlyCount < 0
            || Entry.ExplicitGapCount < 0)
        {
            OutErrors.Add(FString::Printf(TEXT("Negative readiness count on %s."), *Entry.StableId.ToString()));
        }

        if (Entry.ProductionReadyCount + Entry.ReferenceOnlyCount + Entry.ExplicitGapCount != Entry.RegisteredCount)
        {
            OutErrors.Add(FString::Printf(
                TEXT("Pre-main readiness count does not close for %s: registered=%d ready=%d reference=%d gaps=%d."),
                *Entry.StableId.ToString(),
                Entry.RegisteredCount,
                Entry.ProductionReadyCount,
                Entry.ReferenceOnlyCount,
                Entry.ExplicitGapCount));
        }
    }

    if (DeliberateMissionIdentityGapCount != 275)
    {
        OutErrors.Add(TEXT("Turn + Standing deliberate gap total must remain 275."));
    }

    if (DungeonIdentityGaps != 21)
    {
        OutErrors.Add(TEXT("Dungeon identity gaps must remain 20 minor + one named Region-06 identity = 21."));
    }

    if (ThreeDGeometryReady + ThreeDReferenceOnly + ThreeDBlocked != ThreeDRecordCount)
    {
        OutErrors.Add(TEXT("3D readiness split must close to 140 records."));
    }

    if (HiggsfieldPrevisBriefCount != 37)
    {
        OutErrors.Add(TEXT("Higgsfield scope must remain exactly 37 animation/cinematic previs briefs."));
    }

    if (BuildMainQuestStopReasons().Num() != 4)
    {
        OutErrors.Add(TEXT("Exactly four current main-quest stop reasons must remain explicit until authority changes."));
    }

    return OutErrors.IsEmpty();
}
