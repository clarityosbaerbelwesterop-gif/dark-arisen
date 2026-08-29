// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Production/BossVisualAuthorityPolicy.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenBossVisualAuthoritySpec,
    "DarkArisen.Production.BossVisualAuthority",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenBossVisualAuthoritySpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Boss visual authority policy validates structurally"),
        FBossVisualAuthorityPolicy::Validate(Errors));
    TestEqual(TEXT("Boss visual authority policy has no structural errors"), Errors.Num(), 0);

    const TArray<FName> ConflictIds = FBossVisualAuthorityPolicy::BuildCurrentStoryConflictIds();
    TestEqual(TEXT("Exactly two legacy boss visuals conflict with current story authority"),
        ConflictIds.Num(), FBossVisualAuthorityPolicy::CurrentStoryConflictCount);
    TestFalse(TEXT("Legacy Ethan boss visual is provider-blocked"),
        FBossVisualAuthorityPolicy::IsProviderEligible(TEXT("boss-visual.ethan-harlow")));
    TestFalse(TEXT("Legacy Draven boss visual is provider-blocked"),
        FBossVisualAuthorityPolicy::IsProviderEligible(TEXT("boss-visual.draven-voss")));
    TestTrue(TEXT("Uncontested Ashen Wyrm visual remains usable as bounded reference"),
        FBossVisualAuthorityPolicy::IsProviderEligible(TEXT("boss-visual.ashen-wyrm")));
    TestEqual(TEXT("Nineteen deep-dive boss visuals remain provider-eligible"),
        FBossVisualAuthorityPolicy::ProviderEligibleVisualIdentityCount, 19);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
