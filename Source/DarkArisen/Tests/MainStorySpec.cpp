// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Components/MainStoryDirectorComponent.h"
#include "Components/QuestJournalComponent.h"
#include "Missions/MainStoryMissionCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenMainStoryCatalogSpec,
    "DarkArisen.Story.CurrentCanonCatalog",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenMainStoryCatalogSpec::RunTest(const FString& Parameters)
{
    const TArray<FMainStoryMissionDefinition> Missions =
        UMainStoryMissionCatalog::GetAuthoredMissions();
    TestEqual(TEXT("Critical path contains exactly 34 finite missions"), Missions.Num(), 34);
    TestTrue(TEXT("Critical-path catalog validates"),
        UMainStoryMissionCatalog::IsCatalogValid(Missions));

    int32 PerChapter[10] = {};
    bool bLegacyEthanBossLeak = false;
    for (const FMainStoryMissionDefinition& Mission : Missions)
    {
        if (Mission.Chapter >= 1 && Mission.Chapter <= 10) ++PerChapter[Mission.Chapter - 1];
        const FString Id = Mission.Activation.QuestId.ToString();
        bLegacyEthanBossLeak |= Id.Contains(TEXT("EthanBoss"), ESearchCase::IgnoreCase)
            || Id.Contains(TEXT("Betrayal"), ESearchCase::IgnoreCase);
    }

    static constexpr int32 Expected[10] = {4, 3, 3, 3, 3, 3, 3, 3, 4, 5};
    for (int32 Chapter = 0; Chapter < 10; ++Chapter)
    {
        TestEqual(*FString::Printf(TEXT("Chapter %d mission count"), Chapter + 1),
            PerChapter[Chapter], Expected[Chapter]);
    }
    TestFalse(TEXT("Legacy Ethan boss/betrayal branch cannot leak into current IDs"),
        bLegacyEthanBossLeak);

    FMainStoryMissionDefinition Taking;
    TestTrue(TEXT("The Taking exists"),
        UMainStoryMissionCatalog::TryGetMission(TEXT("Main.C01.03.TheTaking"), Taking));
    TestTrue(TEXT("The Taking persists Ethan's abduction"),
        Taking.PersistenceFlags.Contains(TEXT("Story.EthanAbducted")));
    TestTrue(TEXT("Marc dies in current opening canon"),
        Taking.PersistenceFlags.Contains(TEXT("Story.MarcDead")));
    TestTrue(TEXT("Denise dies in current opening canon"),
        Taking.PersistenceFlags.Contains(TEXT("Story.DeniseDead")));

    FMainStoryMissionDefinition Landfall;
    TestTrue(TEXT("Undertow landfall mission exists"),
        UMainStoryMissionCatalog::TryGetMission(TEXT("Main.C01.04.Undertow"), Landfall));
    TestEqual(TEXT("Jake physically strands at Driftwood Beach in Moran"), Landfall.RouteId,
        FName(TEXT("Route.Moran.OpenWater.OuterReef.DriftwoodBeach.DriftwoodCamp")));
    TestTrue(TEXT("Driftwood Beach arrival persists"),
        Landfall.PersistenceFlags.Contains(TEXT("World.DriftwoodBeachReached")));

    FMainStoryMissionDefinition CrewRoute;
    TestTrue(TEXT("Moran crew route exists"),
        UMainStoryMissionCatalog::TryGetMission(TEXT("Main.C02.01.NoCoinNoFlag"), CrewRoute));
    TestEqual(TEXT("Organic onboarding crosses Mira, mangroves, Koa and Galleon Cove"), CrewRoute.RouteId,
        FName(TEXT("Route.Moran.DriftwoodCamp.MirasCove.MangroveShallows.KoasTradingPost.GalleonCove")));

    FMainStoryMissionDefinition ShipTake;
    TestTrue(TEXT("La Liberacion acquisition exists"),
        UMainStoryMissionCatalog::TryGetMission(TEXT("Main.C02.02.AShipToTake"), ShipTake));
    TestTrue(TEXT("La Liberacion is acquired through Galleon Cove"),
        ShipTake.RouteId.ToString().Contains(TEXT("GalleonCove")));

    FMainStoryMissionDefinition FirstWake;
    TestTrue(TEXT("First voyage exists"),
        UMainStoryMissionCatalog::TryGetMission(TEXT("Main.C02.03.FirstWake"), FirstWake));
    TestEqual(TEXT("First authored voyage leaves Moran for Rexa Harbor"), FirstWake.RouteId,
        FName(TEXT("Route.Moran.GalleonCove.OpenSea.RexaHarbor")));
    TestTrue(TEXT("Rexa world entry persists"),
        FirstWake.PersistenceFlags.Contains(TEXT("World.RexaEntered")));

    FMainStoryMissionDefinition Brothers;
    TestTrue(TEXT("The Chapter-8 physical rescue exists"),
        UMainStoryMissionCatalog::TryGetMission(TEXT("Main.C08.03.Brothers"), Brothers));
    TestTrue(TEXT("The rescue persists Ethan recovered"),
        Brothers.PersistenceFlags.Contains(TEXT("Story.EthanRecovered")));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenMainStoryDirectorSpec,
    "DarkArisen.Story.OrderedStartToCreditsState",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenMainStoryDirectorSpec::RunTest(const FString& Parameters)
{
    UQuestJournalComponent* Journal = NewObject<UQuestJournalComponent>();
    UMainStoryDirectorComponent* Director = NewObject<UMainStoryDirectorComponent>();
    TestNotNull(TEXT("Quest journal created"), Journal);
    TestNotNull(TEXT("Story director created"), Director);
    if (!Journal || !Director) return false;

    TestTrue(TEXT("Main story registers into markerless journal"),
        UMainStoryMissionCatalog::RegisterAuthoredMissions(Journal));
    TestEqual(TEXT("Story starts at Home Water"), Director->GetCurrentMissionId(),
        FName(TEXT("Main.C01.01.HomeWater")));
    TestEqual(TEXT("Story starts in Chapter 1"), Director->GetCurrentChapter(), 1);
    TestTrue(TEXT("Fresh story state validates"), Director->HasValidStoryState());

    const TArray<FMainStoryMissionDefinition> Missions =
        UMainStoryMissionCatalog::GetAuthoredMissions();
    int64 GameMinute = 1;
    for (const FMainStoryMissionDefinition& Mission : Missions)
    {
        TestEqual(TEXT("Director never skips authored mission order"),
            Director->GetCurrentMissionId(), Mission.Activation.QuestId);
        TestTrue(TEXT("Current mission activates only through its authored trigger"),
            Director->TryActivateCurrentMission(
                Journal, Mission.Activation.Trigger, GameMinute++, false));
        TestTrue(TEXT("Current mission resolves through an authored outcome"),
            Director->TryResolveCurrentMission(
                Journal, Mission.AllowedOutcomeIds[0], GameMinute++));
    }

    TestTrue(TEXT("Finite critical path reaches story-complete state"), Director->IsStoryComplete());
    TestTrue(TEXT("Final persistent fact is present"),
        Director->IsStoryFactSet(TEXT("Story.MainComplete")));
    TestTrue(TEXT("Ethan was abducted in current canon"),
        Director->IsStoryFactSet(TEXT("Story.EthanAbducted")));
    TestTrue(TEXT("Ethan was physically recovered before the finale"),
        Director->IsStoryFactSet(TEXT("Story.EthanRecovered")));
    TestTrue(TEXT("Jake entered Moran through Driftwood Beach"),
        Director->IsStoryFactSet(TEXT("World.DriftwoodBeachReached")));
    TestTrue(TEXT("Jake reached Rexa by the first La Liberacion voyage"),
        Director->IsStoryFactSet(TEXT("World.RexaEntered")));
    TestTrue(TEXT("Completed story has no next mission"), Director->GetCurrentMissionId().IsNone());
    return true;
}

#endif
