// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Bosses/Tier1CommanderCatalog.h"
#include "Bosses/Tier1CommanderEncounterComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenTier1CommanderAvoidanceSpec,
    "DarkArisen.Content.Bosses.Tier1.FiniteAvoidanceRoutes",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenTier1CommanderAvoidanceSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("The Nine Who Hold catalog validates with finite authored avoidance routes"),
        FTier1CommanderCatalog::Validate(Errors));

    int32 AvoidanceRouteCount = 0;
    for (const FTier1CommanderDefinition& Definition : FTier1CommanderCatalog::BuildAll())
    {
        AvoidanceRouteCount += Definition.AllowedAvoidedRouteIds.Num();
    }
    TestEqual(TEXT("The nine commanders expose exactly twelve source-authored non-combat route ids"),
        AvoidanceRouteCount, 12);

    UTier1CommanderEncounterComponent* Herrera = NewObject<UTier1CommanderEncounterComponent>();
    TestNotNull(TEXT("Herrera fixture exists"), Herrera);
    if (Herrera)
    {
        Herrera->CommanderStableId = TEXT("boss.herrera");
        TestFalse(TEXT("An arbitrary avoidance string cannot become a Herrera resolution"),
            Herrera->ResolveAvoided(TEXT("avoid.herrera.invented-shortcut")));
        TestTrue(TEXT("The authored barrio-rising route can resolve Herrera without direct combat"),
            Herrera->ResolveAvoided(TEXT("avoid.herrera.barrio-rising")));
        TestEqual(TEXT("Herrera resolves as avoided"),
            Herrera->Resolution, ETier1CommanderResolution::Avoided);
    }

    UTier1CommanderEncounterComponent* Vega = NewObject<UTier1CommanderEncounterComponent>();
    TestNotNull(TEXT("Vega fixture exists"), Vega);
    if (Vega)
    {
        Vega->CommanderStableId = TEXT("boss.vega");
        TestFalse(TEXT("Vega cannot use the walk-away route before he actually refuses mercy"),
            Vega->ResolveAvoided(TEXT("avoid.vega.walk-away-after-spare-refusal")));
        TestTrue(TEXT("Vega direct encounter begins"), Vega->BeginEncounter());
        TestFalse(TEXT("Vega refuses a spare resolution"), Vega->TryResolveSpared());
        TestTrue(TEXT("The refusal is persisted"), Vega->bSpareRefusedObserved);
        TestTrue(TEXT("After refusal Jake may leave instead of fabricating a spare outcome"),
            Vega->ResolveAvoided(TEXT("avoid.vega.walk-away-after-spare-refusal")));
        TestEqual(TEXT("Vega walk-away is tracked as avoided, never spared"),
            Vega->Resolution, ETier1CommanderResolution::Avoided);
    }

    UTier1CommanderEncounterComponent* Sterling = NewObject<UTier1CommanderEncounterComponent>();
    TestNotNull(TEXT("Sterling fixture exists"), Sterling);
    if (Sterling)
    {
        Sterling->CommanderStableId = TEXT("boss.sterling");
        TestTrue(TEXT("Sterling's authored neutral standoff can avoid the naval fight"),
            Sterling->ResolveAvoided(TEXT("avoid.sterling.neutral-standoff")));
    }

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
