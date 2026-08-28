// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Ship/SeaPassageComponent.h"
#include "Ship/ShipHouseholdComponent.h"
#include "Systems/ChapterEconomyComponent.h"
#include "Systems/ProgressionEconomyComponent.h"
#include "Systems/SkillTreeCatalogDataAsset.h"
#include "World/DarkArisenWorldRulesSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM3HouseholdCompletionSpec,
    "DarkArisen.M3.HouseholdCompletion",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM3HouseholdCompletionSpec::RunTest(const FString& Parameters)
{
    UShipHouseholdComponent* Household = NewObject<UShipHouseholdComponent>();
    TestNotNull(TEXT("Household component constructs"), Household);
    if (!Household) return false;

    TestTrue(TEXT("Five named crew schedule from canonical time"), Household->ApplyGameMinute(6 * 60));
    const FCrewHouseholdRead Mira = Household->GetNamedCrewRead(TEXT("crew.mira"));
    TestEqual(TEXT("Mira exists in household schedule"), Mira.CrewId, FName(TEXT("crew.mira")));
    TestEqual(TEXT("Mira is on the weather deck at dawn"), Mira.Deck, EShipDeck::Weather);

    TestTrue(TEXT("Eight specialist berths are legal"), Household->SetSpecialistBerthsOccupied(8));
    TestFalse(TEXT("Ninth specialist berth is refused"), Household->SetSpecialistBerthsOccupied(9));
    TestTrue(TEXT("Twelve semi-named hands are legal"), Household->SetSemiNamedHands(12));
    TestFalse(TEXT("Thirteenth semi-named hand is refused"), Household->SetSemiNamedHands(13));

    Household->RecordCrewLoss(TEXT("hand.test"));
    TestEqual(TEXT("Recent loss is read through the ship, not a number"),
        Household->GetMoraleRead(), ECrewMoraleRead::RecentLoss);
    TestEqual(TEXT("Recent loss produces the empty-hammock world cue"),
        Household->GetVisibleMoraleCue(), ECrewMoraleCue::EmptyHammock);

    USeaPassageComponent* Passage = NewObject<USeaPassageComponent>();
    TestTrue(TEXT("Real passage can begin with time and distance floors"),
        Passage && Passage->BeginSeaPassage(TEXT("route.test"), 60, 1000.0f));
    TestFalse(TEXT("Passage cannot complete instantly"), Passage && Passage->CanCompleteSeaPassage());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM4CompletionSpec,
    "DarkArisen.M4.CompletionContracts",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM4CompletionSpec::RunTest(const FString& Parameters)
{
    UProgressionEconomyComponent* Progression = NewObject<UProgressionEconomyComponent>();
    UChapterEconomyComponent* ChapterEconomy = NewObject<UChapterEconomyComponent>();
    TestNotNull(TEXT("Progression component constructs"), Progression);
    TestNotNull(TEXT("Chapter economy component constructs"), ChapterEconomy);
    if (!Progression || !ChapterEconomy) return false;

    TestTrue(TEXT("Seed doubloons"), Progression->CreditCurrency(EDarkArisenCurrency::Doubloons, 1000));
    FChapterLedger Ledger;
    Ledger.HoldingIncome = 1000;
    Ledger.ArmyUpkeep = -500;
    Ledger.GarrisonUpkeep = 0;
    Ledger.ConstructionDraw = -200;
    TestTrue(TEXT("Valid chapter ledger accepted"), ChapterEconomy->SetPendingLedger(Ledger));
    TestTrue(TEXT("Chapter ledger resolves exactly once"), ChapterEconomy->ResolveChapterLedger(4, Progression));
    TestFalse(TEXT("Same chapter cannot compound"), ChapterEconomy->ResolveChapterLedger(4, Progression));
    TestEqual(TEXT("Ledger net applies without interest or compounding"),
        Progression->GetWallet().Doubloons, int64(1300));

    Progression->SetGreetingState(TEXT("social.test"), ESocialGreetingState::Known);
    TestTrue(TEXT("Drawing a weapon creates authored Wary memory"),
        Progression->RecordWeaponDrawnHere(TEXT("social.test"), 2));
    TestEqual(TEXT("Context is Wary"),
        Progression->GetGreetingState(TEXT("social.test")), ESocialGreetingState::Wary);
    Progression->AdvanceSocialChapter();
    TestEqual(TEXT("Wary persists across chapters"),
        Progression->GetGreetingState(TEXT("social.test")), ESocialGreetingState::Wary);
    Progression->AdvanceSocialChapter();
    TestEqual(TEXT("Prior greeting returns after authored memory"),
        Progression->GetGreetingState(TEXT("social.test")), ESocialGreetingState::Known);
    TestTrue(TEXT("Jake can sit without a time-skip API"), Progression->BeginSitting(TEXT("seat.test")));
    TestTrue(TEXT("Sitting state is active"), Progression->IsSitting());
    Progression->EndSitting();
    TestFalse(TEXT("Sitting state ends explicitly"), Progression->IsSitting());

    UDarkArisenWorldRulesSubsystem* Rules = NewObject<UDarkArisenWorldRulesSubsystem>();
    TestTrue(TEXT("Rest to midday is legal only through a rest location/daypart"),
        Rules && Rules->CompleteRest(EDarkArisenRestLocation::GreatCabin, EDarkArisenDaypart::Midday));
    TestEqual(TEXT("Midday rest advances canonical clock"), Rules->GetMinuteOfDay(), 12 * 60);
    TestTrue(TEXT("Rest queues one legal autosave"), Rules->ConsumePendingAutosaveRequest());
    Rules->BeginLakeToDockAutosaveSuppression();
    TestTrue(TEXT("Suppressed rest still advances time"),
        Rules->CompleteRest(EDarkArisenRestLocation::SafeHouse, EDarkArisenDaypart::Dusk));
    TestFalse(TEXT("Suppressed rest cannot queue autosave"), Rules->ConsumePendingAutosaveRequest());

    USkillTreeCatalogDataAsset* Catalog = NewObject<USkillTreeCatalogDataAsset>();
    TestNotNull(TEXT("Skill catalog data asset constructs"), Catalog);
    if (!Catalog) return false;

    const TArray<FName>& Teachers = UProgressionEconomyComponent::GetCanonicalTeacherIds();
    auto AddBranch = [Catalog, &Teachers](const TCHAR* Branch, int32 Count, int32& GlobalIndex)
    {
        for (int32 LocalIndex = 0; LocalIndex < Count; ++LocalIndex, ++GlobalIndex)
        {
            FSkillNodeDefinition Node;
            Node.NodeId = FName(*FString::Printf(TEXT("test.%s.%02d"), Branch, LocalIndex));
            Node.BranchId = FName(Branch);
            Node.MarkCost = GlobalIndex < 5 ? 3 : 2; // 68*2 + 5 = 141.
            if (GlobalIndex < 23)
            {
                Node.TeacherOptions.Add(Teachers[GlobalIndex % Teachers.Num()]);
            }
            if (GlobalIndex < 11)
            {
                Node.GateKind = ESkillGateKind::Standing;
                Node.RequiredGateId = FName(*FString::Printf(TEXT("standing.test.%02d"), GlobalIndex));
            }
            Catalog->Nodes.Add(Node);
        }
    };

    int32 GlobalIndex = 0;
    AddBranch(TEXT("Blade"), 16, GlobalIndex);
    AddBranch(TEXT("Shadow"), 12, GlobalIndex);
    AddBranch(TEXT("Sea"), 14, GlobalIndex);
    AddBranch(TEXT("Land"), 13, GlobalIndex);
    AddBranch(TEXT("Word"), 13, GlobalIndex);
    TestEqual(TEXT("Test catalog has sixty-eight nodes"), Catalog->Nodes.Num(), 68);
    TestTrue(TEXT("Exact 68-node structural contract validates"), Catalog->IsCatalogStructurallyValid());

    Catalog->Nodes[0].MarkCost = 4;
    TestFalse(TEXT("Wrong total Mark cost fails closed"), Catalog->IsCatalogStructurallyValid());
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
