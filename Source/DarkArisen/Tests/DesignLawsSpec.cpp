// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "DesignLaws.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenDesignLawsSpec,
    "DarkArisen.DesignLaws.Constants",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenDesignLawsSpec::RunTest(const FString& Parameters)
{
    using namespace DarkArisen::DesignLaws;

    TestEqual(TEXT("Frame-rate floor"), TargetFramesPerSecond, 60);
    TestEqual(TEXT("Deflection window"), DeflectionWindowFrames, 6);
    TestTrue(TEXT("Deflection seconds"), FMath::IsNearlyEqual(DeflectionWindowSeconds, 0.1f));
    TestEqual(TEXT("Combat-HUD concession"), PermittedCombatHudElements, 1);
    TestTrue(TEXT("Rache scale"), FMath::IsNearlyEqual(RacheTimeScale, 0.3f));
    TestTrue(TEXT("Rache duration"), FMath::IsNearlyEqual(RacheMaximumRealSeconds, 5.0f));
    TestEqual(TEXT("Autosave trigger count"), static_cast<uint8>(EAutosaveTrigger::Count), 2);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenBaselineStatsSpec,
    "DarkArisen.Foundation.BaselineStats",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenBaselineStatsSpec::RunTest(const FString& Parameters)
{
    const UHealthComponent* Health = NewObject<UHealthComponent>();
    const UStaminaComponent* Stamina = NewObject<UStaminaComponent>();

    TestTrue(TEXT("Health component created"), Health != nullptr);
    TestTrue(TEXT("Stamina component created"), Stamina != nullptr);
    TestTrue(TEXT("Baseline HP is 200"), Health && FMath::IsNearlyEqual(Health->MaxHealth, 200.0f));
    TestTrue(TEXT("Baseline stamina is 120"), Stamina && FMath::IsNearlyEqual(Stamina->MaxStamina, 120.0f));
    TestTrue(TEXT("Stamina regenerates at 20 per second"),
        Stamina && FMath::IsNearlyEqual(Stamina->BaseRegenRate, 20.0f));
    TestTrue(TEXT("Normal stamina delay is one second"),
        Stamina && FMath::IsNearlyEqual(Stamina->RegenDelay, 1.0f));
    TestTrue(TEXT("Exhaustion stamina delay is three seconds"),
        Stamina && FMath::IsNearlyEqual(Stamina->ExhaustionRegenDelay, 3.0f));
    TestTrue(TEXT("Sprint drains ten stamina per second"),
        Stamina && FMath::IsNearlyEqual(Stamina->SprintDrainPerSecond, 10.0f));
    return true;
}

#endif
