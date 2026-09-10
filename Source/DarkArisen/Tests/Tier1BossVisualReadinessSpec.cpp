// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Production/Tier1BossVisualReadinessCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenTier1BossVisualReadinessSpec,
    "DarkArisen.Production.Tier1BossVisualReadiness",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenTier1BossVisualReadinessSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Current Tier-1 boss visual readiness validates structurally"),
        FTier1BossVisualReadinessCatalog::Validate(Errors));
    TestEqual(TEXT("Current Tier-1 boss visual readiness has no structural errors"), Errors.Num(), 0);

    const TArray<FTier1BossVisualReadiness> Bosses = FTier1BossVisualReadinessCatalog::BuildAll();
    TestEqual(TEXT("Exactly the Nine Who Hold are tracked"),
        Bosses.Num(), FTier1BossVisualReadinessCatalog::RequiredTier1BossCount);

    int32 FullLookReady = 0;
    for (const FTier1BossVisualReadiness& Boss : Bosses)
    {
        TestTrue(TEXT("Each current Tier-1 boss retains current authority"), Boss.bCurrentTier1Authority);
        TestFalse(TEXT("No current Tier-1 full look is fabricated"), Boss.bProviderFullLookReady);
        TestFalse(TEXT("Missing physical authority is recorded"), Boss.MissingPhysicalAuthority.IsEmpty());
        if (Boss.bProviderFullLookReady) ++FullLookReady;
    }
    TestEqual(TEXT("No Nine-Who-Hold full face/body look is provider-ready yet"),
        FullLookReady, FTier1BossVisualReadinessCatalog::ProviderFullLookReadyCount);
    TestEqual(TEXT("Nine explicit current-boss visual design gaps remain"),
        FTier1BossVisualReadinessCatalog::BuildDesignGaps().Num(),
        FTier1BossVisualReadinessCatalog::ProviderBlockedFullLookCount);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
