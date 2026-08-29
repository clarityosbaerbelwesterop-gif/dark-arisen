// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "ColonialWar/CastleSiegeComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenStateTreasureSiegeIntegrationSpec,
    "DarkArisen.Content.Rewards.StateTreasureSiegeIntegration",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenStateTreasureSiegeIntegrationSpec::RunTest(const FString& Parameters)
{
    UCastleSiegeComponent* Ciudadela = NewObject<UCastleSiegeComponent>();
    TestNotNull(TEXT("La Ciudadela siege component constructs"), Ciudadela);
    if (!Ciudadela) return false;

    TestTrue(TEXT("La Ciudadela config uses authored castle id"),
        Ciudadela->ConfigureCastle(TEXT("castle.la-ciudadela"), true));
    TestTrue(TEXT("La Ciudadela siege begins"), Ciudadela->BeginSiege(false));
    TestTrue(TEXT("Blind wall breach remains legal"), Ciudadela->CommitBreachDoor(ESiegeDoor::Wall));
    TestTrue(TEXT("Breach reaches assault phase"), Ciudadela->RecordBreachCompleted(false));

    TestFalse(TEXT("An arbitrary treasure id cannot become state treasure canon"),
        Ciudadela->RecordStateTreasureRecovered(TEXT("state-treasure.fake")));
    TestFalse(TEXT("A real state treasure cannot be recovered from the wrong castle"),
        Ciudadela->RecordStateTreasureRecovered(TEXT("state-treasure.labor-ledgers")));
    TestTrue(TEXT("La Ciudadela accepts its Conquest Archives"),
        Ciudadela->RecordStateTreasureRecovered(TEXT("state-treasure.conquest-archives-1651")));
    TestFalse(TEXT("The same state treasure cannot be recorded twice"),
        Ciudadela->RecordStateTreasureRecovered(TEXT("state-treasure.conquest-archives-1651")));

    UCastleSiegeComponent* Sterling = NewObject<UCastleSiegeComponent>();
    TestNotNull(TEXT("Sterling Bastion siege component constructs"), Sterling);
    if (!Sterling) return false;
    TestTrue(TEXT("Sterling Bastion config uses authored castle id"),
        Sterling->ConfigureCastle(TEXT("castle.sterling-bastion"), true));
    TestTrue(TEXT("Sterling siege begins"), Sterling->BeginSiege(false));
    TestTrue(TEXT("Sterling breach route commits"), Sterling->CommitBreachDoor(ESiegeDoor::Back));
    TestTrue(TEXT("Sterling breach reaches assault"), Sterling->RecordBreachCompleted(false));
    TestTrue(TEXT("Sterling Bastion accepts Sterling's Correspondence regardless of whether prior authored narrative acquired it by taking or voluntary handover"),
        Sterling->RecordStateTreasureRecovered(TEXT("state-treasure.sterlings-correspondence")));

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
