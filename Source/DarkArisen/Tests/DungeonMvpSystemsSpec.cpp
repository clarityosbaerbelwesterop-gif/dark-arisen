// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Dungeons/DrownedMeadHallComponent.h"
#include "Dungeons/HollowCityComponent.h"
#include "Dungeons/ObsidianVaultComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenDungeonMvpSystemsSpec,
    "DarkArisen.Content.Dungeons.MVPSystems",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenDungeonMvpSystemsSpec::RunTest(const FString& Parameters)
{
    // Drowned Mead Hall: exactly two forty-world-minute windows, twelve hours apart.
    TestFalse(TEXT("Un-authored tide start fails closed"),
        UDrownedMeadHallComponent::IsDryWindowMinute(100, -1));
    TestTrue(TEXT("First dry window opens at authored start"),
        UDrownedMeadHallComponent::IsDryWindowMinute(100, 100));
    TestTrue(TEXT("First dry window includes minute 39"),
        UDrownedMeadHallComponent::IsDryWindowMinute(139, 100));
    TestFalse(TEXT("First dry window closes at minute 40"),
        UDrownedMeadHallComponent::IsDryWindowMinute(140, 100));
    TestTrue(TEXT("Second dry window is twelve hours later"),
        UDrownedMeadHallComponent::IsDryWindowMinute(820, 100));
    TestTrue(TEXT("Second dry window includes its final minute"),
        UDrownedMeadHallComponent::IsDryWindowMinute(859, 100));
    TestFalse(TEXT("Second dry window closes after forty minutes"),
        UDrownedMeadHallComponent::IsDryWindowMinute(860, 100));
    TestTrue(TEXT("Dry window correctly wraps midnight"),
        UDrownedMeadHallComponent::IsDryWindowMinute(5, 1420));

    // Obsidian Vault: the authored image is exact, not an approximate spectacle trigger.
    TestTrue(TEXT("One hundred reflections with exactly one wrong is canonical"),
        UObsidianVaultComponent::IsCanonicalReflectionImage(100, 1));
    TestFalse(TEXT("Ninety-nine reflections are not the authored image"),
        UObsidianVaultComponent::IsCanonicalReflectionImage(99, 1));
    TestFalse(TEXT("Two wrong reflections violate the authored image"),
        UObsidianVaultComponent::IsCanonicalReflectionImage(100, 2));

    // Hollow City: four source-locked but unnamed sub-area slots, no invented names/counts.
    TestFalse(TEXT("Three Hollow City areas are incomplete"),
        UHollowCityComponent::HasAllSubAreaBits(0x07));
    TestTrue(TEXT("All four Hollow City areas satisfy the authored four-area contract"),
        UHollowCityComponent::HasAllSubAreaBits(0x0F));
    TestTrue(TEXT("Additional bookkeeping bits cannot invalidate the four required areas"),
        UHollowCityComponent::HasAllSubAreaBits(0x1F));

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
