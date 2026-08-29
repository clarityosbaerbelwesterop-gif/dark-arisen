// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Release/AlphaContentReadiness.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenAlphaContentReadinessSpec,
    "DarkArisen.Release.AlphaContentReadiness",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenAlphaContentReadinessSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Alpha content snapshot is structurally valid"), FAlphaContentReadiness::ValidateSnapshot(Errors));
    TestEqual(TEXT("Readiness snapshot has no structural errors"), Errors.Num(), 0);

    const TArray<FAlphaContentFamilyStatus> Snapshot = FAlphaContentReadiness::BuildCurrentSnapshot();
    TestEqual(TEXT("Fourteen Alpha content families are reconciled"), Snapshot.Num(), 14);

    TArray<FString> Blockers;
    TestFalse(TEXT("Source cannot self-certify complete Alpha production"), FAlphaContentReadiness::IsContentProductionComplete(Blockers));
    TestTrue(TEXT("Concrete Alpha production blockers are exposed"), Blockers.Num() > 0);

    const auto Find = [&Snapshot](const FName Id) -> const FAlphaContentFamilyStatus*
    {
        return Snapshot.FindByPredicate([Id](const FAlphaContentFamilyStatus& Entry) { return Entry.StableId == Id; });
    };

    const FAlphaContentFamilyStatus* Dungeons = Find(TEXT("alpha-content.named-dungeons"));
    TestNotNull(TEXT("Named dungeon status exists"), Dungeons);
    if (Dungeons)
    {
        TestEqual(TEXT("Forty of forty-one named dungeon identities are grounded"), Dungeons->SourceGroundedCount, 40);
        TestEqual(TEXT("No production dungeon map set is falsely claimed"), Dungeons->ProductionAssetCount, 0);
    }

    const FAlphaContentFamilyStatus* Turns = Find(TEXT("alpha-content.turns"));
    TestNotNull(TEXT("Turn status exists"), Turns);
    if (Turns)
    {
        TestEqual(TEXT("Three of 132 Turn identities are grounded"), Turns->SourceGroundedCount, 3);
    }

    const FAlphaContentFamilyStatus* Voices = Find(TEXT("alpha-content.core-crew-voice"));
    TestNotNull(TEXT("Core crew voice status exists"), Voices);
    if (Voices)
    {
        TestEqual(TEXT("Five core crew role identities are grounded"), Voices->SourceGroundedCount, 5);
        TestEqual(TEXT("Auditions are not counted as shipping voice assets"), Voices->ProductionAssetCount, 0);
    }

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
