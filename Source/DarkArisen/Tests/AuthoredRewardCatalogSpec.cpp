// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "ContentScale/AuthoredRewardCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenAuthoredRewardCatalogSpec,
    "DarkArisen.Content.Rewards.AuthoredCatalog",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenAuthoredRewardCatalogSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Authored reward catalog validates"), FAuthoredRewardCatalog::Validate(Errors));
    TestEqual(TEXT("Reward catalog has no structural errors"), Errors.Num(), 0);

    const TArray<FAuthoredRewardBinding> DungeonRewards = FAuthoredRewardCatalog::BuildDungeonRewardBindings();
    TestEqual(TEXT("Forty grounded dungeon profiles expose reward/outcome bindings"), DungeonRewards.Num(), 40);

    const FAuthoredRewardBinding* FirstHouse = DungeonRewards.FindByPredicate([](const FAuthoredRewardBinding& Entry)
    {
        return Entry.SourceContentId == TEXT("dungeon.region06.first-house");
    });
    TestNotNull(TEXT("First House reward outcome exists"), FirstHouse);
    if (FirstHouse)
    {
        TestTrue(TEXT("First House explicitly rewards nothing"), FirstHouse->bExplicitlyNothing);
        TestEqual(TEXT("First House uses ExplicitNothing family"), FirstHouse->Family, EAuthoredRewardFamily::ExplicitNothing);
    }

    const FAuthoredRewardBinding* SiegeRoute = DungeonRewards.FindByPredicate([](const FAuthoredRewardBinding& Entry)
    {
        return Entry.SourceContentId == TEXT("dungeon.colonial.siege-tunnels-ciudadela");
    });
    TestNotNull(TEXT("La Ciudadela tunnel reward exists"), SiegeRoute);
    if (SiegeRoute)
    {
        TestEqual(TEXT("Undercity route is tactical access, not generic loot"), SiegeRoute->Family, EAuthoredRewardFamily::TacticalAccess);
    }

    const TArray<FAuthoredRewardBinding> StateTreasures = FAuthoredRewardCatalog::BuildStateTreasureSlots();
    TestEqual(TEXT("Nine state-treasure slots exist"), StateTreasures.Num(), 9);
    for (const FAuthoredRewardBinding& Treasure : StateTreasures)
    {
        TestFalse(TEXT("Unnamed state treasure is not falsely authored"), Treasure.bIdentityAuthored);
        TestTrue(TEXT("Unnamed state treasure stays unresolved"), Treasure.bWithheldOrUnresolved);
    }

    const TArray<FAuthoredRewardBinding> UniqueRewards = FAuthoredRewardCatalog::BuildNamedUniqueRewards();
    TestEqual(TEXT("One currently grounded named unique reward exists"), UniqueRewards.Num(), 1);
    TestEqual(TEXT("Crystal Katana remains the named unique reward"), UniqueRewards[0].DisplayName, FString(TEXT("Crystal Katana")));

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
