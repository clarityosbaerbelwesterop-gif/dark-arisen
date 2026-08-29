// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Missions/AuthoredQuestCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenAuthoredQuestCatalogSpec,
    "DarkArisen.Content.Quests.AuthoredCatalog",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenAuthoredQuestCatalogSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Source-backed Thread/mission coverage catalog validates"), FAuthoredQuestCatalog::ValidateCatalog(Errors));
    TestEqual(TEXT("Quest catalog structural validation has no errors"), Errors.Num(), 0);

    const TArray<FAuthoredThreadCatalogEntry> Threads = FAuthoredQuestCatalog::BuildThreads();
    TestEqual(TEXT("Exactly seventeen named Threads are authored"), Threads.Num(), 17);

    int32 CrewCount = 0;
    int32 CrewMarks = 0;
    int32 HardWorldStateExpirations = 0;
    TSet<FName> ThreadIds;
    for (const FAuthoredThreadCatalogEntry& Entry : Threads)
    {
        TestFalse(TEXT("Thread stable IDs are unique"), ThreadIds.Contains(Entry.StableId));
        ThreadIds.Add(Entry.StableId);
        if (Entry.Region == EAuthoredThreadRegion::Crew)
        {
            ++CrewCount;
            CrewMarks += Entry.MarkReward;
        }
        HardWorldStateExpirations += Entry.bExpiresByWorldState ? 1 : 0;
    }
    TestEqual(TEXT("Five crew Threads"), CrewCount, 5);
    TestEqual(TEXT("Five crew Threads grant two Marks each"), CrewMarks, 10);
    TestEqual(TEXT("Three Threads have hard world-state closure in this catalog; Light Elves never fully expires"), HardWorldStateExpirations, 3);

    TestTrue(TEXT("Ines/Esperanza Thread is present"), ThreadIds.Contains(TEXT("thread.crew.ines-esperanza")));
    TestTrue(TEXT("Assassin Network Thread is present"), ThreadIds.Contains(TEXT("thread.archipelago.assassin-network")));
    TestTrue(TEXT("Princess Thread is present"), ThreadIds.Contains(TEXT("thread.highmoore.princess")));
    TestTrue(TEXT("Named Dead Thread is present"), ThreadIds.Contains(TEXT("thread.cross.named-dead")));
    TestTrue(TEXT("Ethan Thread is present without reviving legacy betrayal canon"), ThreadIds.Contains(TEXT("thread.cross.ethan")));

    int32 RegionalTurnTotal = 0;
    for (const FTurnDistributionRequirement& Region : FAuthoredQuestCatalog::BuildTurnDistribution())
    {
        RegionalTurnTotal += Region.RequiredCount;
    }
    TestEqual(TEXT("Regional Turn distribution is exactly 132"), RegionalTurnTotal, 132);

    int32 KnownTurns = 0;
    int32 KnownStanding = 0;
    for (const FKnownMissionIdentity& Mission : FAuthoredQuestCatalog::BuildKnownMissionIdentities())
    {
        Mission.bStanding ? ++KnownStanding : ++KnownTurns;
    }
    TestEqual(TEXT("Only three Turn identities are currently concretely authored"), KnownTurns, 3);
    TestEqual(TEXT("Only one Standing identity is currently concretely authored"), KnownStanding, 1);
    TestEqual(TEXT("Missing identities remain explicit instead of generated"), FAuthoredQuestCatalog::BuildDesignGaps().Num(), 3);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
