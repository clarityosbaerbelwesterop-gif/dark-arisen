// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Components/CameraStateComponent.h"
#include "Components/CombatComponent.h"
#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "Components/WoundStateComponent.h"
#include "CoreLoopTuning.h"
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM1RallyTuningSpec,
    "DarkArisen.M1.RallyTuning",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM1RallyTuningSpec::RunTest(const FString& Parameters)
{
    TestTrue(TEXT("Standard damage is seventy percent recoverable"), FMath::IsNearlyEqual(
        UHealthComponent::GetRallyFractionForDamageClass(ERallyDamageClass::StandardEnemy),
        0.70f));
    TestTrue(TEXT("Boss damage is fifty percent recoverable"), FMath::IsNearlyEqual(
        UHealthComponent::GetRallyFractionForDamageClass(ERallyDamageClass::Boss),
        0.50f));
    TestTrue(TEXT("Poison cannot be rallied"), FMath::IsNearlyZero(
        UHealthComponent::GetRallyFractionForDamageClass(ERallyDamageClass::Poison)));
    TestTrue(TEXT("Light hit recovers fifteen percent"), FMath::IsNearlyEqual(
        UHealthComponent::GetRecoveryFractionForAction(ERallyRecoveryAction::LightHit),
        0.15f));
    TestTrue(TEXT("Critical recovers all available rally"), FMath::IsNearlyEqual(
        UHealthComponent::GetRecoveryFractionForAction(ERallyRecoveryAction::Critical),
        1.0f));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM1HitProfilesSpec,
    "DarkArisen.M1.HitProfiles",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM1HitProfilesSpec::RunTest(const FString& Parameters)
{
    const FCombatHitProfile Light = UCombatComponent::GetHitProfile(ECombatHitKind::Light);
    const FCombatHitProfile Heavy = UCombatComponent::GetHitProfile(ECombatHitKind::Heavy);
    const FCombatHitProfile Critical = UCombatComponent::GetHitProfile(ECombatHitKind::Critical);
    TestTrue(TEXT("Light damage is positive"), Light.HealthDamage > 0.0f);
    TestTrue(TEXT("Heavy exceeds light health damage"), Heavy.HealthDamage > Light.HealthDamage);
    TestTrue(TEXT("Heavy exceeds light posture damage"), Heavy.PostureDamage > Light.PostureDamage);
    TestTrue(TEXT("Critical breaks baseline posture"), Critical.PostureDamage >= 100.0f);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM1WoundLayersSpec,
    "DarkArisen.M1.WoundLayers",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM1WoundLayersSpec::RunTest(const FString& Parameters)
{
    TestEqual(TEXT("Healthy and rested"),
        UWoundStateComponent::EvaluateLayer(1.0f, 1.0f), EWoundLayer::None);
    TestEqual(TEXT("Stamina below thirty percent is winded"),
        UWoundStateComponent::EvaluateLayer(1.0f, 0.29f), EWoundLayer::Winded);
    TestEqual(TEXT("Health below sixty percent is hurt"),
        UWoundStateComponent::EvaluateLayer(0.59f, 1.0f), EWoundLayer::Hurt);
    TestEqual(TEXT("Health below thirty percent is bad"),
        UWoundStateComponent::EvaluateLayer(0.29f, 1.0f), EWoundLayer::Bad);
    TestEqual(TEXT("Health below twelve percent is failing"),
        UWoundStateComponent::EvaluateLayer(0.11f, 1.0f), EWoundLayer::Failing);

    UWoundStateComponent* Wounds = NewObject<UWoundStateComponent>();
    TestTrue(TEXT("Wound component created"), Wounds != nullptr);
    if (Wounds)
    {
        Wounds->CurrentLayer = EWoundLayer::Bad;
        TestTrue(TEXT("Ordinary weapons use wounded animations"),
            Wounds->ShouldUseWoundedAnimationSet(false));
        TestFalse(TEXT("Crystal Katana suppresses wounded animation swap"),
            Wounds->ShouldUseWoundedAnimationSet(true));
        Wounds->CurrentLayer = EWoundLayer::Failing;
        TestFalse(TEXT("Failing state blocks sprint"), Wounds->IsSprintAllowed());
    }
    const FWoundPresentationProfile Bad =
        UWoundStateComponent::GetPresentationProfileForLayer(EWoundLayer::Bad);
    const FWoundPresentationProfile Failing =
        UWoundStateComponent::GetPresentationProfileForLayer(EWoundLayer::Failing);
    TestTrue(TEXT("Bad layer visibly limps"), Bad.bLimp);
    TestTrue(TEXT("Bad layer uses stagger-run"), Bad.bStaggerRun);
    TestTrue(TEXT("Failing layer drags weapon"), Failing.bWeaponDrag);
    TestTrue(TEXT("Deterioration lowers movement speed"),
        Failing.MovementSpeedScale < Bad.MovementSpeedScale);
    TestTrue(TEXT("Failing reaches full wound camera drive"),
        FMath::IsNearlyEqual(Failing.CameraInstabilityAlpha, 1.0f));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM1PostureAndWeightSpec,
    "DarkArisen.M1.PostureAndWeight",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM1PostureAndWeightSpec::RunTest(const FString& Parameters)
{
    TestEqual(TEXT("Set posture"),
        UCombatComponent::EvaluatePostureVisualState(0.80f), EPostureVisualState::Set);
    TestEqual(TEXT("Pressed posture"),
        UCombatComponent::EvaluatePostureVisualState(0.79f), EPostureVisualState::Pressed);
    TestEqual(TEXT("Failing posture"),
        UCombatComponent::EvaluatePostureVisualState(0.54f), EPostureVisualState::Failing);
    TestEqual(TEXT("Broken-imminent posture"),
        UCombatComponent::EvaluatePostureVisualState(0.29f),
        EPostureVisualState::BrokenImminent);
    TestEqual(TEXT("Broken posture"),
        UCombatComponent::EvaluatePostureVisualState(0.09f), EPostureVisualState::Broken);

    UCombatComponent* Combat = NewObject<UCombatComponent>();
    TestTrue(TEXT("Combat component created"), Combat != nullptr);
    if (Combat)
    {
        TestEqual(TEXT("Medium startup is thirteen frames"), Combat->GetStartupFrames(), 13);
        TestEqual(TEXT("Medium recovery is twenty frames"), Combat->GetRecoveryFrames(), 20);
        Combat->SetWeaponWeightClass(EWeaponWeightClass::Great);
        TestEqual(TEXT("Great startup is twenty-six frames"), Combat->GetStartupFrames(), 26);
        TestEqual(TEXT("Great recovery is forty-four frames"), Combat->GetRecoveryFrames(), 44);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM1CameraPolicySpec,
    "DarkArisen.M1.CameraPolicy",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM1CameraPolicySpec::RunTest(const FString& Parameters)
{
    UCameraStateComponent* Camera = NewObject<UCameraStateComponent>();
    TestTrue(TEXT("Camera state component created"), Camera != nullptr);
    if (!Camera) return false;
    TestTrue(TEXT("Free accepts look"), Camera->AllowsLookInput());
    TestTrue(TEXT("Free accepts movement"), Camera->AllowsMoveInput());
    Camera->EnterDeathHold();
    TestEqual(TEXT("Death hold is anchored"), Camera->CurrentMode, EPlayerCameraMode::Anchored);
    TestFalse(TEXT("Anchored refuses look"), Camera->AllowsLookInput());
    TestTrue(TEXT("Anchored keeps movement"), Camera->AllowsMoveInput());
    TestTrue(TEXT("Death hold is four seconds"),
        FMath::IsNearlyEqual(Camera->AnchoredTimeRemaining, 4.0f));
    Camera->EnterAuthored();
    TestFalse(TEXT("Authored refuses look"), Camera->AllowsLookInput());
    TestFalse(TEXT("Authored refuses movement"), Camera->AllowsMoveInput());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM1InteractionContractSpec,
    "DarkArisen.M1.InteractionContract",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM1InteractionContractSpec::RunTest(const FString& Parameters)
{
    using namespace DarkArisen;
    TestTrue(TEXT("Interaction range is one metre forty"),
        FMath::IsNearlyEqual(DesignLaws::InteractionPromptRangeMetres, 1.4f));
    TestTrue(TEXT("Corner prompt lasts four seconds"),
        FMath::IsNearlyEqual(CoreLoopTuning::InteractionPromptSeconds, 4.0f));
    TestTrue(TEXT("Default take duration is six tenths"),
        FMath::IsNearlyEqual(CoreLoopTuning::DefaultTakeSeconds, 0.6f));
    return true;
}

#endif
