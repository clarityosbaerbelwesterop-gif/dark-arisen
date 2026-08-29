// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Animation/AnimationProductionCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenAnimationProductionCatalogSpec,
    "DarkArisen.Content.Animation.ProductionCatalog",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenAnimationProductionCatalogSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Animation production catalog validates"), FAnimationProductionCatalog::Validate(Errors));
    TestEqual(TEXT("Animation catalog has no structural errors"), Errors.Num(), 0);

    const TArray<FAnimationProductionRequirement> Named = FAnimationProductionCatalog::BuildNamedBibleRequirements();
    TestEqual(TEXT("All eleven named Bible animation requirements are present"), Named.Num(), 11);

    TSet<FName> Ids;
    for (const FAnimationProductionRequirement& Requirement : Named)
    {
        Ids.Add(Requirement.StableId);
        TestFalse(TEXT("No named animation is falsely claimed as authored asset"), Requirement.bAssetAuthored);
        TestTrue(TEXT("No absent animation carries a fake asset path"), Requirement.AssetPath.IsEmpty());
    }

    TestTrue(TEXT("Bad catch requirement present"), Ids.Contains(TEXT("anim.named.bad-catch")));
    TestTrue(TEXT("Guardian settling requirement present"), Ids.Contains(TEXT("anim.named.guardian-settling")));
    TestTrue(TEXT("Vega stand-up requirement present"), Ids.Contains(TEXT("anim.named.vega-stands-again")));
    TestTrue(TEXT("Bailiffs leaving requirement present"), Ids.Contains(TEXT("anim.named.bailiffs-leaving")));
    TestTrue(TEXT("Three mount attempts requirement present"), Ids.Contains(TEXT("anim.named.mount-three-attempts")));

    TestEqual(TEXT("Attack tell remains at least eight frames"), FAnimationProductionCatalog::MinimumAttackTellFrames, 8);
    TestEqual(TEXT("Deflection window remains exactly six frames"), FAnimationProductionCatalog::LockedDeflectionWindowFrames, 6);
    TestEqual(TEXT("Four gaits require six adjacent transition pairs"), FAnimationProductionCatalog::RequiredHorseGaitTransitionCount, 6);
    TestEqual(TEXT("Three animation production gaps remain explicit"), FAnimationProductionCatalog::BuildDesignGaps().Num(), 3);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
