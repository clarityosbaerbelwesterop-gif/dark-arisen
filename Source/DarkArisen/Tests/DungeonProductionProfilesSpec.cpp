// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "ContentScale/AuthoredDungeonProductionProfile.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenDungeonProductionProfilesSpec,
    "DarkArisen.Content.Dungeons.ProductionProfiles",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenDungeonProductionProfilesSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("All grounded named dungeons have source-backed production profiles"),
        FAuthoredDungeonProductionProfiles::ValidateProfiles(Errors));
    TestEqual(TEXT("Dungeon production profiles have no structural errors"), Errors.Num(), 0);

    const TArray<FAuthoredDungeonProductionProfile> Profiles =
        FAuthoredDungeonProductionProfiles::BuildAllKnownProfiles();
    TestEqual(TEXT("Exactly forty grounded named sites have production profiles"), Profiles.Num(), 40);

    FAuthoredDungeonProductionProfile FirstHouse;
    TestTrue(TEXT("First House profile is available"),
        FAuthoredDungeonProductionProfiles::TryGetProfile(TEXT("dungeon.region06.first-house"), FirstHouse));
    TestTrue(TEXT("First House explicitly gives no reward"), FirstHouse.bRewardExplicitlyNone);
    TestFalse(TEXT("First House does not fabricate a reward"), !FirstHouse.RewardDetail.IsEmpty());

    FAuthoredDungeonProductionProfile ThirdBell;
    TestTrue(TEXT("Third Bell profile is available"),
        FAuthoredDungeonProductionProfiles::TryGetProfile(TEXT("dungeon.region06.third-bell"), ThirdBell));
    TestTrue(TEXT("Third Bell image remains withheld"), ThirdBell.bImageWithheld);
    TestTrue(TEXT("Third Bell boss remains unresolved"), ThirdBell.bBossWithheldOrUnresolved);
    TestTrue(TEXT("Third Bell reward remains unresolved"), ThirdBell.bRewardWithheldOrUnresolved);

    FAuthoredDungeonProductionProfile FifthConnection;
    TestTrue(TEXT("Fifth Connection profile is available"),
        FAuthoredDungeonProductionProfiles::TryGetProfile(TEXT("dungeon.cross-network.fifth-connection"), FifthConnection));
    TestTrue(TEXT("Fifth Connection keeps its non-combat-only resolution"),
        FifthConnection.bSupportsExplicitNonCombatResolution);
    TestTrue(TEXT("Fifth Connection image remains deliberately withheld"), FifthConnection.bImageWithheld);

    FAuthoredDungeonProductionProfile WyrmRest;
    TestTrue(TEXT("Wyrm-rest profile is available"),
        FAuthoredDungeonProductionProfiles::TryGetProfile(TEXT("dungeon.ashenmoor.thing-wyrm-left"), WyrmRest));
    TestTrue(TEXT("Wyrm-rest keeps an explicit non-combat resolution"),
        WyrmRest.bSupportsExplicitNonCombatResolution);

    FAuthoredDungeonProductionProfile VentShrine;
    TestTrue(TEXT("Vent-Shrine profile is available"),
        FAuthoredDungeonProductionProfiles::TryGetProfile(TEXT("dungeon.ashenmoor.vent-shrine"), VentShrine));
    TestTrue(TEXT("Vent-Shrine remains war-reactive"), VentShrine.bWarReactive);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
