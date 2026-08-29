// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "ContentScale/AuthoredRewardCatalog.h"
#include "ContentScale/TreasureStateComponent.h"

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
    TestEqual(TEXT("Exactly nine state treasures exist"), StateTreasures.Num(), 9);
    TSet<FString> Names;
    for (const FAuthoredRewardBinding& Treasure : StateTreasures)
    {
        TestTrue(TEXT("Every state treasure is concretely authored"), Treasure.bIdentityAuthored);
        TestFalse(TEXT("No named state treasure remains falsely unresolved"), Treasure.bWithheldOrUnresolved);
        TestEqual(TEXT("Every state treasure is a document"), Treasure.TreasureClass, EAuthoredTreasureClass::Document);
        TestFalse(TEXT("Every state treasure has an authored castle origin"), Treasure.OriginStableId.IsNone());
        Names.Add(Treasure.DisplayName);
    }
    TestTrue(TEXT("Conquest Archives are present"), Names.Contains(TEXT("The Conquest Archives, 1651")));
    TestTrue(TEXT("1846 Patrol Reports are present"), Names.Contains(TEXT("The 1846 Patrol Reports")));
    TestTrue(TEXT("Sterling correspondence is present"), Names.Contains(TEXT("Sterling's Correspondence")));
    TestTrue(TEXT("Thorne dispatches are present"), Names.Contains(TEXT("Thorne's Dispatches")));

    TestEqual(TEXT("Treasure corpus has forty return-question artifacts"), FAuthoredRewardCatalog::ReturnQuestionArtifactCount, 40);
    TestEqual(TEXT("Sixteen buried hoards remain required"), FAuthoredRewardCatalog::BuriedHoardCount, 16);
    TestEqual(TEXT("Twelve buried hoards belong to the archipelago"), FAuthoredRewardCatalog::ArchipelagoBuriedHoardCount, 12);
    TestEqual(TEXT("Four buried hoards belong to Highmoore"), FAuthoredRewardCatalog::HighmooreBuriedHoardCount, 4);
    TestFalse(TEXT("Random loot tables remain prohibited"), FAuthoredRewardCatalog::AllowsRandomLootTables());
    TestFalse(TEXT("Rarity colours remain prohibited"), FAuthoredRewardCatalog::AllowsRarityColourCoding());
    TestFalse(TEXT("Returned-count UI remains prohibited"), FAuthoredRewardCatalog::AllowsReturnedCountUI());

    const TArray<FAuthoredRewardBinding> UniqueRewards = FAuthoredRewardCatalog::BuildNamedUniqueRewards();
    TestEqual(TEXT("One currently grounded named unique reward exists"), UniqueRewards.Num(), 1);
    TestEqual(TEXT("Crystal Katana remains the named unique reward"), UniqueRewards[0].DisplayName, FString(TEXT("Crystal Katana")));

    UTreasureStateComponent* State = NewObject<UTreasureStateComponent>();
    TestNotNull(TEXT("Treasure state component created"), State);
    if (State)
    {
        TestTrue(TEXT("Return-question artifact A registers"), State->RegisterArtifact(TEXT("artifact.test.a"), true));
        TestTrue(TEXT("Return-question artifact B registers"), State->RegisterArtifact(TEXT("artifact.test.b"), true));
        TestTrue(TEXT("Return-question artifact C registers"), State->RegisterArtifact(TEXT("artifact.test.c"), true));

        const FTreasureDecisionResult FirstReturn = State->ResolveArtifact(TEXT("artifact.test.a"), ETreasureDisposition::Returned);
        TestTrue(TEXT("Return decision is accepted"), FirstReturn.bAccepted);
        TestEqual(TEXT("Returning an authored community artifact yields +5 network-strength delta for the owning authority"),
            FirstReturn.NetworkStrengthDelta, 5);
        TestTrue(TEXT("One returned versus zero non-returned satisfies the hidden return bias"),
            FirstReturn.bRecoveryReturnThresholdSatisfied);

        const FTreasureDecisionResult Kept = State->ResolveArtifact(TEXT("artifact.test.b"), ETreasureDisposition::Kept);
        TestTrue(TEXT("Keep decision is accepted"), Kept.bAccepted);
        TestFalse(TEXT("One returned versus one non-returned no longer satisfies the hidden return bias"),
            Kept.bRecoveryReturnThresholdSatisfied);

        const FTreasureDecisionResult SecondReturn = State->ResolveArtifact(TEXT("artifact.test.c"), ETreasureDisposition::Returned);
        TestTrue(TEXT("Second return is accepted"), SecondReturn.bAccepted);
        TestTrue(TEXT("Two returned versus one non-returned restores the hidden return bias"),
            SecondReturn.bRecoveryReturnThresholdSatisfied);

        const FTreasureDecisionResult Duplicate = State->ResolveArtifact(TEXT("artifact.test.c"), ETreasureDisposition::Sold);
        TestFalse(TEXT("A resolved artifact cannot be re-decided"), Duplicate.bAccepted);
    }

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
