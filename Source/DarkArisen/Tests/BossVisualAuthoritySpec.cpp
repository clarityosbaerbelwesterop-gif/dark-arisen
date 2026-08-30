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

    TestEqual(TEXT("Twenty-one deep-dive visual identities are indexed"),
        FBossVisualAuthorityPolicy::DeepDiveVisualIdentityCount, 21);
    TestEqual(TEXT("Exactly two current-story conflicts remain"),
        FBossVisualAuthorityPolicy::CurrentStoryConflictCount, 2);
    TestEqual(TEXT("Exactly nineteen deep-dive visuals are provider eligible"),
        FBossVisualAuthorityPolicy::ProviderEligibleVisualIdentityCount, 19);

    TestFalse(TEXT("Legacy Ethan boss visual is provider blocked"),
        FBossVisualAuthorityPolicy::IsProviderEligible(TEXT("boss-visual.ethan-harlow")));
    TestFalse(TEXT("Legacy Draven boss visual is provider blocked"),
        FBossVisualAuthorityPolicy::IsProviderEligible(TEXT("boss-visual.draven-voss")));
    TestTrue(TEXT("Indexed uncontested Ashen Wyrm visual reference remains eligible"),
        FBossVisualAuthorityPolicy::IsProviderEligible(TEXT("boss-visual.ashen-wyrm")));
    TestFalse(TEXT("Unknown boss visual identity fails closed"),
        FBossVisualAuthorityPolicy::IsProviderEligible(TEXT("boss-visual.not-authored")));

    const FBossVisualAuthorityDecision Unknown =
        FBossVisualAuthorityPolicy::Evaluate(TEXT("boss-visual.not-authored"));
    TestEqual(TEXT("Unknown boss visual identity keeps explicit unknown authority state"),
        Unknown.State, EBossVisualAuthorityState::UnknownIdentity);
    TestFalse(TEXT("Unknown boss visual identity cannot enter reference production"),
        Unknown.bProviderEligible);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
