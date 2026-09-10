// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Production/FloraVisualProductionCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenFloraVisualProductionSpec,
    "DarkArisen.Production.FloraVisualAuthority",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenFloraVisualProductionSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Flora visual production catalog validates structurally"),
        FFloraVisualProductionCatalog::Validate(Errors));
    TestEqual(TEXT("Flora visual production catalog has no structural validation errors"), Errors.Num(), 0);

    const TArray<FFloraVisualProductionBrief> Briefs = FFloraVisualProductionCatalog::BuildBriefs();
    TestEqual(TEXT("Nine individually grounded legendary/story flora identities are indexed"),
        Briefs.Num(), FFloraVisualProductionCatalog::RequiredBriefCount);

    int32 Ready = 0;
    int32 Blocked = 0;
    for (const FFloraVisualProductionBrief& Brief : Briefs)
    {
        if (Brief.bProviderReferenceReady) ++Ready;
        else ++Blocked;
        TestFalse(TEXT("No flora brief falsely claims an approved provider reference"), Brief.bApprovedReferenceExists);
        TestTrue(TEXT("No flora brief fabricates an approved reference path"), Brief.ApprovedReferencePath.IsEmpty());
    }

    TestEqual(TEXT("Eight source-bounded flora references are provider-ready"),
        Ready, FFloraVisualProductionCatalog::ProviderReadyBriefCount);
    TestEqual(TEXT("One story-dependent flora identity remains provider-blocked"),
        Blocked, FFloraVisualProductionCatalog::ProviderBlockedBriefCount);

    const FFloraVisualProductionBrief* EthanGrove = Briefs.FindByPredicate([](const FFloraVisualProductionBrief& Brief)
    {
        return Brief.StableId == TEXT("flora.story.ethans-grove");
    });
    TestNotNull(TEXT("Ethan's Grove source identity exists"), EthanGrove);
    if (EthanGrove != nullptr)
    {
        TestFalse(TEXT("Ethan's Grove is not provider-ready while Phase-11 Ethan story authority is unresolved"),
            EthanGrove->bProviderReferenceReady);
    }

    const FFloraVisualProductionBrief* DeepGeometry = Briefs.FindByPredicate([](const FFloraVisualProductionBrief& Brief)
    {
        return Brief.StableId == TEXT("flora.region06.infinite-branch");
    });
    TestNotNull(TEXT("Infinite Branch source identity exists"), DeepGeometry);
    if (DeepGeometry != nullptr)
    {
        TestTrue(TEXT("Infinite Branch supports bounded impossible-perspective reference work"), DeepGeometry->bProviderReferenceReady);
        TestTrue(TEXT("Infinite Branch never locks one final silhouette"),
            DeepGeometry->AntiInventionBoundary.Contains(TEXT("must not establish one canonical full silhouette")));
    }

    TestEqual(TEXT("Flora source inconsistencies and story boundaries remain explicit"),
        FFloraVisualProductionCatalog::BuildDesignGaps().Num(), 3);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
