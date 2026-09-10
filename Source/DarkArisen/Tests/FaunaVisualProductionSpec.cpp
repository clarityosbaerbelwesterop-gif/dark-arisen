// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Production/FaunaVisualProductionCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenFaunaVisualProductionSpec,
    "DarkArisen.Production.FaunaVisualAuthority",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenFaunaVisualProductionSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Fauna visual production catalog validates structurally"),
        FFaunaVisualProductionCatalog::Validate(Errors));
    TestEqual(TEXT("Fauna visual production catalog has no structural validation errors"), Errors.Num(), 0);

    const TArray<FFaunaVisualProductionBrief> Legendary =
        FFaunaVisualProductionCatalog::BuildLegendaryCreatureBriefs();
    const TArray<FFaunaVisualProductionBrief> Highmoore =
        FFaunaVisualProductionCatalog::BuildHighmooreMvpBriefs();
    const TArray<FFaunaVisualProductionBrief> All =
        FFaunaVisualProductionCatalog::BuildAllBriefs();

    TestEqual(TEXT("Nineteen individually grounded legendary fauna identities are indexed"),
        Legendary.Num(), FFaunaVisualProductionCatalog::LegendaryCreatureBriefCount);
    TestEqual(TEXT("Five Highmoore MVP fauna identities are indexed"),
        Highmoore.Num(), FFaunaVisualProductionCatalog::HighmooreMvpBriefCount);
    TestEqual(TEXT("Fauna visual catalog does not expand Phase-4 target counts with invented species"),
        All.Num(), FFaunaVisualProductionCatalog::RequiredBriefCount);

    int32 Ready = 0;
    int32 Blocked = 0;
    for (const FFaunaVisualProductionBrief& Brief : All)
    {
        if (Brief.bProviderReferenceReady) ++Ready;
        else ++Blocked;

        TestFalse(TEXT("No fauna brief falsely claims an approved external reference"), Brief.bApprovedReferenceExists);
        TestTrue(TEXT("No fauna brief fabricates an approved reference path"), Brief.ApprovedReferencePath.IsEmpty());
    }

    TestEqual(TEXT("Exactly the source-bounded fauna subset is provider-ready"),
        Ready, FFaunaVisualProductionCatalog::ProviderReadyBriefCount);
    TestEqual(TEXT("Under-specified or save-dependent fauna remain provider-blocked"),
        Blocked, FFaunaVisualProductionCatalog::ProviderBlockedBriefCount);

    const FFaunaVisualProductionBrief* FinalWolf = All.FindByPredicate([](const FFaunaVisualProductionBrief& Brief)
    {
        return Brief.StableId == TEXT("fauna.legendary.final-wolf");
    });
    TestNotNull(TEXT("Final Wolf visual identity exists"), FinalWolf);
    if (FinalWolf != nullptr)
    {
        TestFalse(TEXT("Final Wolf cannot be frozen into one provider-generated appearance"), FinalWolf->bProviderReferenceReady);
    }

    const FFaunaVisualProductionBrief* DeepOne = All.FindByPredicate([](const FFaunaVisualProductionBrief& Brief)
    {
        return Brief.StableId == TEXT("fauna.legendary.deep-one");
    });
    TestNotNull(TEXT("Deep One glimpse identity exists"), DeepOne);
    if (DeepOne != nullptr)
    {
        TestTrue(TEXT("Deep One allows only a bounded glimpse reference"), DeepOne->bProviderReferenceReady);
        TestTrue(TEXT("Deep One full body stays deliberately undefined"),
            DeepOne->AntiInventionBoundary.Contains(TEXT("Do not render a complete creature")));
    }

    const FFaunaVisualProductionBrief* HighmooreRedDeer = All.FindByPredicate([](const FFaunaVisualProductionBrief& Brief)
    {
        return Brief.StableId == TEXT("fauna.highmoore.red-deer");
    });
    TestNotNull(TEXT("Highmoore Red Deer visual identity exists"), HighmooreRedDeer);
    if (HighmooreRedDeer != nullptr)
    {
        TestTrue(TEXT("Highmoore Red Deer is bounded at ordinary species level"), HighmooreRedDeer->bProviderReferenceReady);
    }

    TestEqual(TEXT("Fauna source gaps stay explicit"),
        FFaunaVisualProductionCatalog::BuildDesignGaps().Num(), 3);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
