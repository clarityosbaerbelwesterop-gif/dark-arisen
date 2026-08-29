// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Missions/QuestCoverageRegister.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenQuestCoverageRegisterSpec,
    "DarkArisen.Content.Missions.QuestCoverage",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenQuestCoverageRegisterSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Quest coverage register validates"), FQuestCoverageRegister::Validate(Errors));
    TestEqual(TEXT("Quest coverage validation has no structural errors"), Errors.Num(), 0);

    const TArray<FQuestCoverageSlot> Coverage = FQuestCoverageRegister::BuildCoverage();
    const TArray<FQuestCoverageSlot> Missing = FQuestCoverageRegister::BuildMissingCoverage();

    TestEqual(TEXT("Finite Turn + Standing coverage is 279"), Coverage.Num(), 279);
    TestEqual(TEXT("Exactly 275 identities remain deliberately unauthored"), Missing.Num(), 275);
    TestEqual(TEXT("Exactly 129 Turn identities remain unauthored"), FQuestCoverageRegister::MissingTurns, 129);
    TestEqual(TEXT("Exactly 146 Standing identities remain unauthored"), FQuestCoverageRegister::MissingStanding, 146);

    for (const FQuestCoverageSlot& Slot : Missing)
    {
        TestTrue(TEXT("Unauthored slots never acquire invented quest identities"), Slot.AuthoredQuestId.IsNone());
        TestFalse(TEXT("Unauthored slots retain a neutral coverage id"), Slot.CoverageId.IsNone());
        TestFalse(TEXT("Unauthored slots retain a source bucket"), Slot.BucketId.IsNone());
    }

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
