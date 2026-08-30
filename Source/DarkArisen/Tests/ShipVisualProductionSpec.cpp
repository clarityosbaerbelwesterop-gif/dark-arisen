// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Production/ShipVisualProductionCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenShipVisualProductionSpec,
    "DarkArisen.Production.ShipVisualProduction",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenShipVisualProductionSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Ship visual production catalog validates structurally"),
        FShipVisualProductionCatalog::Validate(Errors));
    TestEqual(TEXT("Ship visual production catalog has no structural errors"), Errors.Num(), 0);

    const TArray<FShipVisualProductionBrief> Briefs = FShipVisualProductionCatalog::BuildLaLiberacionBriefs();
    TestEqual(TEXT("La Liberacion has six bounded visual production briefs"),
        Briefs.Num(), FShipVisualProductionCatalog::RequiredBriefCount);

    int32 Ready = 0;
    int32 Blocked = 0;
    for (const FShipVisualProductionBrief& Brief : Briefs)
    {
        if (Brief.bProviderReferenceReady) ++Ready;
        else ++Blocked;
    }

    TestEqual(TEXT("Five interior/deck references are provider-ready"),
        Ready, FShipVisualProductionCatalog::ProviderReadyBriefCount);
    TestEqual(TEXT("One exterior silhouette remains provider-blocked"),
        Blocked, FShipVisualProductionCatalog::ProviderBlockedBriefCount);

    const FShipVisualProductionBrief* Cabin = Briefs.FindByPredicate([](const FShipVisualProductionBrief& Brief)
    {
        return Brief.Kind == EShipVisualProductionKind::CaptainsCabin;
    });
    TestNotNull(TEXT("Captain cabin brief exists"), Cabin);
    if (Cabin != nullptr)
    {
        TestTrue(TEXT("Captain cabin explicitly removes superseded quest-marker UI"),
            Cabin->AntiInventionBoundary.Contains(TEXT("forbids quest markers")));
    }

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
