// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Dungeons/FirstHouseComponent.h"
#include "Dungeons/LongBarrowBraeheadComponent.h"
#include "Dungeons/SecretDungeonResolutionComponent.h"
#include "Dungeons/StoneFieldUnderChamberComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenDungeonNoBossCarveoutsSpec,
    "DarkArisen.Content.Dungeons.NoBossCarveouts",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenDungeonNoBossCarveoutsSpec::RunTest(const FString& Parameters)
{
    UFirstHouseComponent* FirstHouse = NewObject<UFirstHouseComponent>();
    TestFalse(TEXT("First House has no hazard"), FirstHouse->HasHazard());
    TestFalse(TEXT("First House has no boss"), FirstHouse->HasBoss());
    TestFalse(TEXT("First House has no reward"), FirstHouse->HasReward());

    UStoneFieldUnderChamberComponent* StoneField = NewObject<UStoneFieldUnderChamberComponent>();
    TestFalse(TEXT("Stone Field Under-Chamber has no boss"), StoneField->HasBoss());

    ULongBarrowBraeheadComponent* LongBarrow = NewObject<ULongBarrowBraeheadComponent>();
    TestFalse(TEXT("Long Barrow at Braehead has no boss"), LongBarrow->HasBoss());
    TestFalse(TEXT("Long Barrow does not invent an unauthored reward"), LongBarrow->HasAuthoredReward());

    TestEqual(TEXT("Fifth Connection uses the no-fight Agreement rule"),
        USecretDungeonResolutionComponent::RuleForStableId(TEXT("dungeon.cross-network.fifth-connection")),
        ESecretDungeonResolutionRule::AgreementNoFight);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
