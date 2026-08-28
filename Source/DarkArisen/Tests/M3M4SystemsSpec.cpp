// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Ship/ShipVoyageComponent.h"
#include "Systems/ProgressionEconomyComponent.h"
#include "World/DarkArisenWorldRulesSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM3ShipContractSpec,
    "DarkArisen.M3.ShipContract",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM3ShipContractSpec::RunTest(const FString& Parameters)
{
    UShipVoyageComponent* Ship = NewObject<UShipVoyageComponent>();
    TestNotNull(TEXT("Ship voyage component is constructible"), Ship);
    if (!Ship) return false;

    TestEqual(TEXT("La Liberacion has exactly four required decks"),
        UShipVoyageComponent::GetRequiredDeckCount(), 4);

    Ship->SetActiveHands(120);
    TestEqual(TEXT("Hands never exceed the authored ninety-person ceiling"),
        Ship->GetActiveHands(), 90);
    Ship->SetActiveHands(-5);
    TestEqual(TEXT("Invalid negative hands fail closed to zero"), Ship->GetActiveHands(), 0);

    Ship->SetWind(0.0f, 6.0f);
    TestEqual(TEXT("Bow directly into wind is in irons"),
        Ship->GetPointOfSail(), EPointOfSail::InIrons);
    Ship->SetWind(90.0f, 6.0f);
    TestEqual(TEXT("Beam wind produces a beam reach"),
        Ship->GetPointOfSail(), EPointOfSail::BeamReach);

    TestFalse(TEXT("Unknown physical chart is not owned"),
        Ship->HasPhysicalChart(TEXT("chart.test")));
    TestTrue(TEXT("Physical chart can be acquired"),
        Ship->AcquirePhysicalChart(TEXT("chart.test")));
    TestTrue(TEXT("Acquired chart is owned"),
        Ship->HasPhysicalChart(TEXT("chart.test")));
    TestTrue(TEXT("Owned physical chart can be annotated"),
        Ship->AnnotatePhysicalChart(TEXT("chart.test")));
    TestFalse(TEXT("Unowned chart cannot be annotated"),
        Ship->AnnotatePhysicalChart(TEXT("chart.unowned")));

    Ship->SetJakeAtHelm(false);
    TestFalse(TEXT("Leaving the helm is represented without stopping the ship state"),
        Ship->IsJakeAtHelm());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM4ProgressionContractSpec,
    "DarkArisen.M4.ProgressionEconomySocialContract",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM4ProgressionContractSpec::RunTest(const FString& Parameters)
{
    UProgressionEconomyComponent* Progression = NewObject<UProgressionEconomyComponent>();
    TestNotNull(TEXT("Progression/economy component is constructible"), Progression);
    if (!Progression) return false;

    TestEqual(TEXT("Skill tree contract has sixty-eight nodes"),
        UProgressionEconomyComponent::RequiredSkillNodeCount, 68);
    TestEqual(TEXT("Teacher catalog contract has twenty-three named people"),
        UProgressionEconomyComponent::RequiredTeacherCount, 23);
    TestEqual(TEXT("Teacher-gated node count remains twenty-three"),
        UProgressionEconomyComponent::RequiredTeacherGatedNodeCount, 23);
    TestEqual(TEXT("Standing-gated node count remains eleven"),
        UProgressionEconomyComponent::RequiredStandingGatedNodeCount, 11);
    TestEqual(TEXT("Canonical teacher catalog contains exactly twenty-three people"),
        UProgressionEconomyComponent::GetCanonicalTeacherIds().Num(), 23);
    TestEqual(TEXT("Maximum available Marks remain ninety-four"),
        UProgressionEconomyComponent::MaximumAvailableMarks, 94);
    TestEqual(TEXT("Full tree still costs one hundred forty-one Marks"),
        UProgressionEconomyComponent::FullTreeMarkCost, 141);
    TestFalse(TEXT("Partial authored catalog cannot claim M4 completion"),
        Progression->IsSkillCatalogComplete());

    TestTrue(TEXT("Physician's Draught is an authored Body source"),
        Progression->ApplyPhysiciansDraught(TEXT("body.draught.test")));
    TestEqual(TEXT("Physician's Draught adds exactly fifteen HP"),
        Progression->GetMaximumHealth(), 215);
    TestFalse(TEXT("Body source cannot be consumed twice"),
        Progression->ApplyPhysiciansDraught(TEXT("body.draught.test")));

    TestTrue(TEXT("Deep-Water Pearl is an authored Body source"),
        Progression->ApplyDeepWaterPearl(TEXT("body.pearl.test")));
    TestEqual(TEXT("Deep-Water Pearl adds exactly five stamina"),
        Progression->GetMaximumStamina(), 125);

    FSkillNodeDefinition TaughtNode;
    TaughtNode.NodeId = TEXT("node.test.named_teacher_scene");
    TaughtNode.BranchId = TEXT("Sea");
    TaughtNode.MarkCost = 2;
    TaughtNode.TeacherOptions.Add(TEXT("teacher.mira"));
    TestTrue(TEXT("A valid authored teacher-gated node registers"),
        Progression->RegisterAuthoredNode(TaughtNode));
    TestTrue(TEXT("Authored Mark source can award the required Marks"),
        Progression->AwardMarks(TEXT("mark.test.teacher_scene"), 2));
    TestTrue(TEXT("Money may exist alongside progression"),
        Progression->CreditCurrency(EDarkArisenCurrency::Doubloons, 1000000));
    TestFalse(TEXT("Money cannot buy a technique without its named teacher"),
        Progression->TryLearnNode(TaughtNode.NodeId));
    Progression->RecordTeacherMet(TEXT("teacher.mira"));
    TestFalse(TEXT("Meeting a teacher is not the same as being taught"),
        Progression->TryLearnNode(TaughtNode.NodeId));
    TestTrue(TEXT("Named person can complete the authored teaching scene"),
        Progression->CompleteTeachingScene(TEXT("teacher.mira"), TaughtNode.NodeId));
    TestTrue(TEXT("Completed teaching scene plus Marks unlocks the technique"),
        Progression->TryLearnNode(TaughtNode.NodeId));
    TestTrue(TEXT("Learned technique remains learned"),
        Progression->HasLearnedNode(TaughtNode.NodeId));

    UProgressionEconomyComponent* PrerequisiteProgression = NewObject<UProgressionEconomyComponent>();
    TestNotNull(TEXT("Prerequisite test component is constructible"), PrerequisiteProgression);
    if (!PrerequisiteProgression) return false;

    FSkillNodeDefinition RootNode;
    RootNode.NodeId = TEXT("node.test.root");
    RootNode.BranchId = TEXT("Blade");
    RootNode.MarkCost = 1;
    FSkillNodeDefinition DependentNode;
    DependentNode.NodeId = TEXT("node.test.dependent");
    DependentNode.BranchId = TEXT("Blade");
    DependentNode.MarkCost = 1;
    DependentNode.PrerequisiteNodeIds.Add(RootNode.NodeId);
    TestTrue(TEXT("Root node registers"), PrerequisiteProgression->RegisterAuthoredNode(RootNode));
    TestTrue(TEXT("Dependent node registers"), PrerequisiteProgression->RegisterAuthoredNode(DependentNode));
    TestTrue(TEXT("Prerequisite test Marks are awarded"),
        PrerequisiteProgression->AwardMarks(TEXT("mark.test.prerequisites"), 2));
    TestFalse(TEXT("Dependent node cannot bypass its prerequisite"),
        PrerequisiteProgression->TryLearnNode(DependentNode.NodeId));
    TestTrue(TEXT("Root prerequisite can be learned first"),
        PrerequisiteProgression->TryLearnNode(RootNode.NodeId));
    TestTrue(TEXT("Dependent node opens only after its prerequisite"),
        PrerequisiteProgression->TryLearnNode(DependentNode.NodeId));

    TestTrue(TEXT("Pounds are a separate wallet"),
        Progression->CreditCurrency(EDarkArisenCurrency::Pounds, 12));
    TestTrue(TEXT("Silver Marks are a separate wallet"),
        Progression->CreditCurrency(EDarkArisenCurrency::SilverMarks, 7));
    const FCurrencyWallet Wallet = Progression->GetWallet();
    TestEqual(TEXT("Doubloon balance remains independent"), Wallet.Doubloons, int64(1000000));
    TestEqual(TEXT("Pound balance remains independent"), Wallet.Pounds, int64(12));
    TestEqual(TEXT("Silver Mark balance remains independent"), Wallet.SilverMarks, int64(7));

    Progression->SetGreetingState(TEXT("social.raices"), ESocialGreetingState::Known);
    TestEqual(TEXT("Greeting state is world-readable without an affinity meter"),
        Progression->GetGreetingState(TEXT("social.raices")), ESocialGreetingState::Known);
    TestTrue(TEXT("Listening begins from an authored conversation"),
        Progression->BeginListening(TEXT("conversation.test")));
    Progression->InterruptListening();
    TestFalse(TEXT("Interrupted conversation is not recorded as overheard"),
        Progression->HasOverheard(TEXT("conversation.test")));
    TestTrue(TEXT("Listening can begin again to a later occurrence"),
        Progression->BeginListening(TEXT("conversation.test")));
    TestTrue(TEXT("Uninterrupted listening completes"), Progression->CompleteListening());
    TestTrue(TEXT("Completed listening is remembered"),
        Progression->HasOverheard(TEXT("conversation.test")));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM4WorldRulesSpec,
    "DarkArisen.M4.WorldTimeAndSaveContract",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM4WorldRulesSpec::RunTest(const FString& Parameters)
{
    UDarkArisenWorldRulesSubsystem* Rules = NewObject<UDarkArisenWorldRulesSubsystem>();
    TestNotNull(TEXT("World rules subsystem is constructible for rule tests"), Rules);
    if (!Rules) return false;

    TestTrue(TEXT("Manual save remains unrestricted"), Rules->CanManualSave());
    TestTrue(TEXT("Rest is a legal autosave source"), Rules->NotifyRestCompleted());
    TestTrue(TEXT("Legal rest autosave request can be consumed"),
        Rules->ConsumePendingAutosaveRequest());
    TestFalse(TEXT("Autosave request is one-shot"), Rules->ConsumePendingAutosaveRequest());

    Rules->BeginLakeToDockAutosaveSuppression();
    TestTrue(TEXT("Lake-to-dock window suppresses autosave"), Rules->IsAutosaveSuppressed());
    TestTrue(TEXT("Rest can finish while autosave is suppressed"), Rules->NotifyRestCompleted());
    TestFalse(TEXT("Suppressed rest does not queue autosave"),
        Rules->ConsumePendingAutosaveRequest());
    TestTrue(TEXT("Manual save still works during authored autosave suppression"),
        Rules->CanManualSave());

    Rules->EndLakeToDockAutosaveSuppression();
    TestFalse(TEXT("Autosave suppression explicitly ends at the dock"),
        Rules->IsAutosaveSuppressed());
    TestTrue(TEXT("Chapter boundary advances canonical chapter state"),
        Rules->NotifyChapterBoundary(5));
    TestTrue(TEXT("Chapter boundary queues the second legal autosave source"),
        Rules->ConsumePendingAutosaveRequest());
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
