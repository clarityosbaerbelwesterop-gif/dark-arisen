// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Production/PreMainQuestReadinessCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenPreMainQuestReadinessSpec,
    "DarkArisen.Production.PreMainQuestReadiness",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenPreMainQuestReadinessSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Pre-main-quest readiness validates structurally"),
        FPreMainQuestReadinessCatalog::Validate(Errors));
    TestEqual(TEXT("Pre-main readiness has no structural validation errors"), Errors.Num(), 0);

    const TArray<FPreMainQuestReadinessRecord> Records = FPreMainQuestReadinessCatalog::BuildRecords();
    TestEqual(TEXT("Exactly seventeen pre-main production families are reconciled"),
        Records.Num(), FPreMainQuestReadinessCatalog::RequiredFamilyCount);

    const auto Find = [&Records](const TCHAR* Id) -> const FPreMainQuestReadinessRecord*
    {
        const FName Wanted(Id);
        return Records.FindByPredicate([Wanted](const FPreMainQuestReadinessRecord& Entry)
        {
            return Entry.StableId == Wanted;
        });
    };

    const FPreMainQuestReadinessRecord* Dungeons = Find(TEXT("pre-main.dungeons"));
    TestNotNull(TEXT("Dungeon closure record exists"), Dungeons);
    if (Dungeons != nullptr)
    {
        TestEqual(TEXT("Dungeon structure remains sixty-one sites"), Dungeons->RegisteredCount, 61);
        TestEqual(TEXT("Forty dungeon identities are grounded"), Dungeons->ProductionReadyCount, 40);
        TestEqual(TEXT("Twenty-one dungeon identities remain deliberately unresolved"), Dungeons->ExplicitGapCount, 21);
    }

    const FPreMainQuestReadinessRecord* Turns = Find(TEXT("pre-main.turns"));
    TestNotNull(TEXT("Turn closure record exists"), Turns);
    if (Turns != nullptr)
    {
        TestEqual(TEXT("Turns remain 132 total"), Turns->RegisteredCount, 132);
        TestEqual(TEXT("Three Turns are individually grounded"), Turns->ProductionReadyCount, 3);
        TestEqual(TEXT("129 Turns remain unauthored rather than generated"), Turns->ExplicitGapCount, 129);
    }

    const FPreMainQuestReadinessRecord* Standing = Find(TEXT("pre-main.standing"));
    TestNotNull(TEXT("Standing closure record exists"), Standing);
    if (Standing != nullptr)
    {
        TestEqual(TEXT("Standing variants remain 147 total"), Standing->RegisteredCount, 147);
        TestEqual(TEXT("One Standing variant is individually grounded"), Standing->ProductionReadyCount, 1);
        TestEqual(TEXT("146 Standing variants remain unauthored rather than generated"), Standing->ExplicitGapCount, 146);
    }

    TestEqual(TEXT("Turn plus Standing deliberate gap total remains 275"),
        FPreMainQuestReadinessCatalog::DeliberateMissionIdentityGapCount, 275);

    const FPreMainQuestReadinessRecord* Tier1Looks = Find(TEXT("pre-main.tier1-looks"));
    TestNotNull(TEXT("Current Tier-1 physical-look closure record exists"), Tier1Looks);
    if (Tier1Looks != nullptr)
    {
        TestEqual(TEXT("All nine current Tier-1 full looks remain explicit art-authority gaps"),
            Tier1Looks->ExplicitGapCount, 9);
    }

    const FPreMainQuestReadinessRecord* ThreeD = Find(TEXT("pre-main.3d-readiness"));
    TestNotNull(TEXT("3D readiness closure record exists"), ThreeD);
    if (ThreeD != nullptr)
    {
        TestEqual(TEXT("3D source-readiness register has 140 records"), ThreeD->RegisteredCount, 140);
        TestEqual(TEXT("105 records are candidate-geometry-ready"), ThreeD->ProductionReadyCount, 105);
        TestEqual(TEXT("14 records remain reference-only"), ThreeD->ReferenceOnlyCount, 14);
        TestEqual(TEXT("21 records remain blocked"), ThreeD->ExplicitGapCount, 21);
    }

    const FPreMainQuestReadinessRecord* Presentation = Find(TEXT("pre-main.presentation"));
    TestNotNull(TEXT("Presentation closure record exists"), Presentation);
    if (Presentation != nullptr)
    {
        TestEqual(TEXT("Nineteen cutscene slots remain the hard total"), Presentation->RegisteredCount, 19);
        TestEqual(TEXT("Fourteen cutscene identities are resolved"), Presentation->ProductionReadyCount, 14);
        TestEqual(TEXT("Five final-act cutscene identities remain unresolved"), Presentation->ExplicitGapCount, 5);
    }

    const FPreMainQuestReadinessRecord* Higgsfield = Find(TEXT("pre-main.higgsfield-previs"));
    TestNotNull(TEXT("Higgsfield closure record exists"), Higgsfield);
    if (Higgsfield != nullptr)
    {
        TestEqual(TEXT("Higgsfield is restricted to thirty-seven motion/cinematic briefs"),
            Higgsfield->RegisteredCount, 37);
        TestEqual(TEXT("All thirty-seven are source-ready briefs"), Higgsfield->ProductionReadyCount, 37);
    }

    const TArray<FPreMainQuestStopReason> StopReasons =
        FPreMainQuestReadinessCatalog::BuildMainQuestStopReasons();
    TestEqual(TEXT("Four explicit main-story authority blockers define the stop boundary"),
        StopReasons.Num(), 4);

    TestFalse(TEXT("Main quests may not be fabricated"),
        FPreMainQuestReadinessCatalog::AllowsMainQuestFabrication());
    TestFalse(TEXT("Generated filler cannot close finite content gaps"),
        FPreMainQuestReadinessCatalog::AllowsGapCountsToBeClosedWithGeneratedContent());
    TestFalse(TEXT("Source readiness is never runtime evidence"),
        FPreMainQuestReadinessCatalog::TreatsSourceReadinessAsRuntimeEvidence());

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
