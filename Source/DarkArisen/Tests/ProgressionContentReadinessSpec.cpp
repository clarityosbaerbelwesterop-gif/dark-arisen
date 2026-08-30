// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Production/ProgressionContentReadinessCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenProgressionContentReadinessSpec,
    "DarkArisen.Production.ProgressionContentReadiness",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenProgressionContentReadinessSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Progression content readiness validates structurally"),
        FProgressionContentReadinessCatalog::Validate(Errors));
    TestEqual(TEXT("Progression readiness has no structural validation errors"), Errors.Num(), 0);

    const TArray<FProgressionContentReadinessRecord> Records =
        FProgressionContentReadinessCatalog::BuildRecords();
    TestEqual(TEXT("Exactly six finite progression readiness families are tracked"),
        Records.Num(), FProgressionContentReadinessCatalog::RequiredReadinessFamilyCount);

    TestEqual(TEXT("CRAFT remains exactly sixty-eight nodes"),
        FProgressionContentReadinessCatalog::RequiredSkillNodeCount, 68);
    TestEqual(TEXT("Only nineteen skill-node identities are individually grounded"),
        FProgressionContentReadinessCatalog::GroundedSkillNodeCount, 19);
    TestEqual(TEXT("Forty-nine skill-node identities remain deliberately unauthored"),
        FProgressionContentReadinessCatalog::SkillNodeIdentityGapCount, 49);

    TestEqual(TEXT("All twenty-three teacher identities are grounded"),
        FProgressionContentReadinessCatalog::GroundedTeacherCount,
        FProgressionContentReadinessCatalog::RequiredTeacherCount);

    TestEqual(TEXT("Nine of twelve Physician's Draught placements are source-assigned"),
        FProgressionContentReadinessCatalog::GroundedPhysiciansDraughtPlacementCount, 9);
    TestEqual(TEXT("Three Physician's Draught dungeon placements remain unresolved"),
        FProgressionContentReadinessCatalog::PhysiciansDraughtPlacementGapCount, 3);

    TestEqual(TEXT("No individual Deep-Water Pearl placement is invented"),
        FProgressionContentReadinessCatalog::GroundedDeepWaterPearlPlacementCount, 0);
    TestEqual(TEXT("All sixteen Deep-Water Pearl placements remain authored-placement gaps"),
        FProgressionContentReadinessCatalog::DeepWaterPearlPlacementGapCount, 16);

    TestEqual(TEXT("Nine Tier-1 posture sources are unambiguous"),
        FProgressionContentReadinessCatalog::UncontestedTier1DeflectionSourceCount, 9);
    TestEqual(TEXT("Six remaining posture-source slots stay unresolved rather than selecting hunters"),
        FProgressionContentReadinessCatalog::PostureDeflectionSelectionGapCount, 6);

    TestEqual(TEXT("Six of twelve one-off Mark acts are explicitly listed"),
        FProgressionContentReadinessCatalog::GroundedSpecificMarkActCount, 6);
    TestEqual(TEXT("Six one-off Mark acts remain unidentified"),
        FProgressionContentReadinessCatalog::SpecificMarkActGapCount, 6);

    const TArray<FProgressionContentDesignGap> Gaps =
        FProgressionContentReadinessCatalog::BuildDesignGaps();
    TestEqual(TEXT("Five progression authoring gap families remain explicit"), Gaps.Num(), 5);

    for (const FProgressionContentReadinessRecord& Entry : Records)
    {
        TestFalse(TEXT("Progression readiness never fabricates an empty stable id"), Entry.StableId.IsNone());
        TestFalse(TEXT("Progression readiness always cites governing source"), Entry.GoverningSource.IsEmpty());
        TestEqual(TEXT("Each finite readiness record closes without hidden filler"),
            Entry.GroundedCount + Entry.ExplicitGapCount, Entry.RequiredCount);
    }

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
