// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "World/AuthoredTravelProductionCatalog.h"
#include "World/HighmooreWorldProductionCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenTravelWorldProductionSpec,
    "DarkArisen.Content.World.TravelAndHighmooreProduction",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenTravelWorldProductionSpec::RunTest(const FString& Parameters)
{
    TArray<FString> TravelErrors;
    TestTrue(TEXT("Authored travel production catalog validates"), FAuthoredTravelProductionCatalog::Validate(TravelErrors));
    TestEqual(TEXT("Travel production has no structural errors"), TravelErrors.Num(), 0);

    const TArray<FAuthoredTravelProductionRoute> Routes = FAuthoredTravelProductionCatalog::BuildCoreRoutes();
    TestEqual(TEXT("Six core travel law routes are represented"), Routes.Num(), 6);
    TestFalse(TEXT("Water fast travel remains impossible"), FAuthoredTravelProductionCatalog::AllowsInstantWaterTravel());
    TestFalse(TEXT("Map-click movement remains impossible"), FAuthoredTravelProductionCatalog::AllowsMapClickMovement());
    TestFalse(TEXT("Compass remains absent"), FAuthoredTravelProductionCatalog::AllowsCompass());
    TestFalse(TEXT("Mini-map remains absent"), FAuthoredTravelProductionCatalog::AllowsMiniMapDisplay());
    TestFalse(TEXT("Player dot remains absent"), FAuthoredTravelProductionCatalog::AllowsPlayerDot());
    TestEqual(TEXT("Six feared-water regions remain trust/guide opened"), FAuthoredTravelProductionCatalog::FearedWaterRegionCount, 6);

    const FAuthoredTravelProductionRoute* FirstCaves = Routes.FindByPredicate([](const FAuthoredTravelProductionRoute& Entry){ return Entry.Mode == EAuthoredTravelMode::CrystalCavesFirstPassage; });
    TestNotNull(TEXT("First Crystal Caves passage route exists"), FirstCaves);
    if (FirstCaves)
    {
        TestEqual(TEXT("First Crystal passage minimum is ninety real minutes"), FirstCaves->MinimumRealMinutes, 90);
        TestEqual(TEXT("First Crystal passage maximum is one hundred twenty real minutes"), FirstCaves->MaximumRealMinutes, 120);
        TestTrue(TEXT("First Crystal passage requires physical traversal"), FirstCaves->bRequiresPhysicalTraversal);
    }

    const FAuthoredTravelProductionRoute* RepeatCaves = Routes.FindByPredicate([](const FAuthoredTravelProductionRoute& Entry){ return Entry.Mode == EAuthoredTravelMode::CrystalCavesRepeatPassage; });
    TestNotNull(TEXT("Repeat Crystal Caves passage route exists"), RepeatCaves);
    if (RepeatCaves)
    {
        TestEqual(TEXT("Repeat Crystal passage minimum is four real minutes"), RepeatCaves->MinimumRealMinutes, 4);
        TestEqual(TEXT("Repeat Crystal passage maximum is six real minutes"), RepeatCaves->MaximumRealMinutes, 6);
        TestTrue(TEXT("Repeat Crystal passage remains traversal rather than a menu"), RepeatCaves->bRequiresPhysicalTraversal && !RepeatCaves->bMenuTravelPermitted);
    }

    const FAuthoredTravelProductionRoute* Stable = Routes.FindByPredicate([](const FAuthoredTravelProductionRoute& Entry){ return Entry.Mode == EAuthoredTravelMode::HorseStableRelocation; });
    TestNotNull(TEXT("Highmoore stable relocation route exists"), Stable);
    if (Stable)
    {
        TestFalse(TEXT("Stable relocation never moves Jake"), Stable->bMovesJake);
        TestTrue(TEXT("Stable relocation moves only the horse"), Stable->bMovesHorseOnly);
    }

    TArray<FString> HighmooreErrors;
    TestTrue(TEXT("Highmoore world production catalog validates"), FHighmooreWorldProductionCatalog::Validate(HighmooreErrors));
    TestEqual(TEXT("Highmoore world catalog has no structural errors"), HighmooreErrors.Num(), 0);

    const TArray<FHighmooreWorldProductionAnchor> Anchors = FHighmooreWorldProductionCatalog::BuildNamedAnchors();
    TestEqual(TEXT("Twelve source-backed Highmoore named anchors are registered"), Anchors.Num(), 12);
    TestEqual(TEXT("At least six castles remain visible from the emergence shelf"), FHighmooreWorldProductionCatalog::MinimumVisibleCastlesFromEmergence, 6);
    TestEqual(TEXT("Emergence switchback minimum remains eight minutes on foot"), FHighmooreWorldProductionCatalog::EmergenceSwitchbackMinimumMinutesOnFoot, 8);
    TestEqual(TEXT("Emergence switchback maximum remains ten minutes on foot"), FHighmooreWorldProductionCatalog::EmergenceSwitchbackMaximumMinutesOnFoot, 10);
    TestEqual(TEXT("Highmoore end-to-end canter remains roughly twenty-five minutes"), FHighmooreWorldProductionCatalog::ApproximateEndToEndCanterMinutes, 25);
    TestFalse(TEXT("Highmoore discovery never gains question-mark markers"), FHighmooreWorldProductionCatalog::AllowsQuestionMarkDiscoveryMarkers());
    TestFalse(TEXT("Highmoore castles never gain generic substitutions"), FHighmooreWorldProductionCatalog::AllowsGenericCastleSubstitution());

    TSet<FName> AnchorIds;
    const FHighmooreWorldProductionAnchor* Coldbeck = nullptr;
    for (const FHighmooreWorldProductionAnchor& Entry : Anchors)
    {
        AnchorIds.Add(Entry.StableId);
        TestFalse(TEXT("No Highmoore anchor falsely claims reviewed runtime placement"), Entry.bRuntimePlacementAuthored);
        TestTrue(TEXT("No absent Highmoore runtime placement carries a fake actor path"), Entry.RuntimeActorPath.IsEmpty());
        if (Entry.StableId == FName(TEXT("world.highmoore.town.coldbeck"))) Coldbeck = &Entry;
    }
    TestTrue(TEXT("Arion skyline anchor exists"), AnchorIds.Contains(TEXT("world.highmoore.castle.arion")));
    TestTrue(TEXT("Belos skyline anchor exists"), AnchorIds.Contains(TEXT("world.highmoore.castle.belos")));
    TestTrue(TEXT("Drover's Rest anchor exists"), AnchorIds.Contains(TEXT("world.highmoore.pub.drovers-rest")));
    TestNotNull(TEXT("Coldbeck town anchor exists"), Coldbeck);
    if (Coldbeck) TestEqual(TEXT("Coldbeck display identity is exact"), Coldbeck->DisplayName, FString(TEXT("Coldbeck")));

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
