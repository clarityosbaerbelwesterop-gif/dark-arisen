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

    TSet<FName> SystemIds;
    for (const FAnimationProductionRequirement& Requirement : FAnimationProductionCatalog::BuildSystemRequirements())
    {
        SystemIds.Add(Requirement.StableId);
        TestFalse(TEXT("System animation contract never pretends the binary asset exists"), Requirement.bAssetAuthored);
        TestTrue(TEXT("System animation contract carries no fake binary path"), Requirement.AssetPath.IsEmpty());
    }
    TestTrue(TEXT("Non-cancellable recovery is an explicit production requirement"),
        SystemIds.Contains(TEXT("anim.system.non-cancellable-recovery")));
    TestTrue(TEXT("People carrying is an explicit full-body requirement"),
        SystemIds.Contains(TEXT("anim.system.people-carried")));
    TestTrue(TEXT("Horse mood has a body-language requirement"),
        SystemIds.Contains(TEXT("anim.system.horse-mood-body-read")));
    TestTrue(TEXT("Katana clean-set exception is explicit"),
        SystemIds.Contains(TEXT("anim.system.katana-clean-under-wounds")));

    const TArray<FAnimationWeightTiming> Timings = FAnimationProductionCatalog::BuildWeightTimings();
    TestEqual(TEXT("Five exact weapon weight timing classes exist"), Timings.Num(), 5);
    if (Timings.Num() == 5)
    {
        TestEqual(TEXT("Light startup is 9 frames"), Timings[0].StartupFrames, 9);
        TestEqual(TEXT("Light recovery is 14 frames"), Timings[0].RecoveryFrames, 14);
        TestEqual(TEXT("Medium startup is 13 frames"), Timings[1].StartupFrames, 13);
        TestEqual(TEXT("Medium recovery is 20 frames"), Timings[1].RecoveryFrames, 20);
        TestEqual(TEXT("Heavy startup is 19 frames"), Timings[2].StartupFrames, 19);
        TestEqual(TEXT("Heavy recovery is 31 frames"), Timings[2].RecoveryFrames, 31);
        TestEqual(TEXT("Great startup is 26 frames"), Timings[3].StartupFrames, 26);
        TestEqual(TEXT("Great recovery is 44 frames"), Timings[3].RecoveryFrames, 44);
        TestEqual(TEXT("Polearm startup is 15 frames"), Timings[4].StartupFrames, 15);
        TestEqual(TEXT("Polearm recovery is 24 frames"), Timings[4].RecoveryFrames, 24);

        for (const FAnimationWeightTiming& Timing : Timings)
        {
            TestTrue(TEXT("Every weapon class preserves the eight-frame attack readability floor"),
                Timing.StartupFrames >= FAnimationProductionCatalog::MinimumAttackTellFrames);
            TestTrue(TEXT("Every weapon class preserves non-zero authored recovery commitment"),
                Timing.RecoveryFrames > 0);
        }
    }

    const TArray<FAnimationWoundLayerRequirement> Wounds = FAnimationProductionCatalog::BuildWoundLayerRequirements();
    TestEqual(TEXT("Four source-authored wound animation layers exist"), Wounds.Num(), 4);
    if (Wounds.Num() == 4)
    {
        TestTrue(TEXT("Winded trigger remains Stamina <30%"), Wounds[0].Trigger.Contains(TEXT("30%")));
        TestTrue(TEXT("Hurt trigger remains HP <60%"), Wounds[1].Trigger.Contains(TEXT("60%")));
        TestTrue(TEXT("Bad trigger remains HP <30%"), Wounds[2].Trigger.Contains(TEXT("30%")));
        TestTrue(TEXT("Failing trigger remains HP <12%"), Wounds[3].Trigger.Contains(TEXT("12%")));
        TestTrue(TEXT("Failing layer keeps the no-sprint law"), Wounds[3].RequiredBodyRead.Contains(TEXT("Cannot sprint")));
    }

    TestEqual(TEXT("Attack tell remains at least eight frames"), FAnimationProductionCatalog::MinimumAttackTellFrames, 8);
    TestEqual(TEXT("Deflection window remains exactly six frames"), FAnimationProductionCatalog::LockedDeflectionWindowFrames, 6);
    TestEqual(TEXT("Four gaits require six adjacent transition pairs"), FAnimationProductionCatalog::RequiredHorseGaitTransitionCount, 6);
    TestEqual(TEXT("Three animation production gaps remain explicit"), FAnimationProductionCatalog::BuildDesignGaps().Num(), 3);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
