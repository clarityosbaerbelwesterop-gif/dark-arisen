// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Dungeons/SecretDungeonResolutionComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenSecretDungeonResolutionSpec,
    "DarkArisen.Content.Dungeons.SecretResolution",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenSecretDungeonResolutionSpec::RunTest(const FString& Parameters)
{
    TestEqual(TEXT("Grove uses take-nothing rule"),
        USecretDungeonResolutionComponent::RuleForStableId(TEXT("dungeon.rexa.grove-beneath-grove")),
        ESecretDungeonResolutionRule::GroveTakeNothing);
    TestEqual(TEXT("One That Waited requires valued offering"),
        USecretDungeonResolutionComponent::RuleForStableId(TEXT("dungeon.fjordlund.one-that-waited")),
        ESecretDungeonResolutionRule::LeaveValuedOffering);
    TestEqual(TEXT("Wyrm site requires three-game-day wait"),
        USecretDungeonResolutionComponent::RuleForStableId(TEXT("dungeon.ashenmoor.thing-wyrm-left")),
        ESecretDungeonResolutionRule::WaitThreeGameDays);
    TestEqual(TEXT("Fifth Connection is Agreement only"),
        USecretDungeonResolutionComponent::RuleForStableId(TEXT("dungeon.cross-network.fifth-connection")),
        ESecretDungeonResolutionRule::AgreementNoFight);
    TestEqual(TEXT("Fourth Fortress remains withheld"),
        USecretDungeonResolutionComponent::RuleForStableId(TEXT("dungeon.region06.fourth-fortress")),
        ESecretDungeonResolutionRule::FourthFortressWithheld);
    TestEqual(TEXT("Harbour supports document-or-take-boat rule"),
        USecretDungeonResolutionComponent::RuleForStableId(TEXT("dungeon.sea.harbour-that-was-first")),
        ESecretDungeonResolutionRule::DocumentOrTakeBoat);

    TestFalse(TEXT("Unknown site has no secret resolution rule"),
        USecretDungeonResolutionComponent::RuleForStableId(TEXT("dungeon.unknown")) !=
            ESecretDungeonResolutionRule::Unknown);

    TestEqual(TEXT("Three-day wait is exactly 4320 game minutes"),
        USecretDungeonResolutionComponent::RequiredWyrmWaitGameMinutes,
        static_cast<int64>(4320));
    TestFalse(TEXT("Two game days do not satisfy Wyrm wait"),
        USecretDungeonResolutionComponent::IsThreeDayWaitComplete(100, 100 + (2 * 24 * 60)));
    TestFalse(TEXT("One minute short does not satisfy Wyrm wait"),
        USecretDungeonResolutionComponent::IsThreeDayWaitComplete(100, 100 + 4319));
    TestTrue(TEXT("Exactly three game days satisfies Wyrm wait"),
        USecretDungeonResolutionComponent::IsThreeDayWaitComplete(100, 100 + 4320));
    TestTrue(TEXT("Longer wait remains valid"),
        USecretDungeonResolutionComponent::IsThreeDayWaitComplete(100, 100 + 5000));
    TestFalse(TEXT("Clock rollback cannot satisfy Wyrm wait"),
        USecretDungeonResolutionComponent::IsThreeDayWaitComplete(1000, 999));

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
