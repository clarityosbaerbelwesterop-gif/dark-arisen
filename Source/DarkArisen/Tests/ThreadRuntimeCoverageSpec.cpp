// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Missions/ThreadRuntimeCoverage.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenThreadRuntimeCoverageSpec,
    "DarkArisen.Content.Threads.ExactNativeCoverage",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenThreadRuntimeCoverageSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("All seventeen authored Thread identities have explicit native source owners"),
        FThreadRuntimeCoverage::Validate(Errors));
    TestEqual(TEXT("Thread runtime coverage remains exactly seventeen"),
        FThreadRuntimeCoverage::Build().Num(), FThreadRuntimeCoverage::RequiredThreadCount);

    int32 GapBearingThreads = 0;
    int32 GapIds = 0;
    for (const FThreadNativeCoverageEntry& Entry : FThreadRuntimeCoverage::Build())
    {
        if (Entry.State == EThreadNativeCoverageState::NativeContractWithAuthorshipGaps)
        {
            ++GapBearingThreads;
            GapIds += Entry.AuthorshipGapIds.Num();
        }
    }

    TestEqual(TEXT("Five Thread identities currently retain explicit authorship gaps"), GapBearingThreads, 5);
    TestEqual(TEXT("Those five Threads expose seven stable unresolved authorship gaps"), GapIds, 7);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
