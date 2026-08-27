// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Animation/DarkArisenAnimInstance.h"
#include "Bosses/IsabelCruzCharacter.h"
#include "Components/CameraStateComponent.h"
#include "Components/CombatComponent.h"
#include "Components/HealthComponent.h"
#include "Components/HeatExposureComponent.h"
#include "Components/LockOnComponent.h"
#include "Components/QuestJournalComponent.h"
#include "Components/StaminaComponent.h"
#include "Components/WoundStateComponent.h"
#include "CoreLoopTuning.h"
#include "DesignLaws.h"
#include "Interaction/InteractionPersistence.h"
#include "Interaction/PhysicalDoorActor.h"

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
    FDarkArisenM1AnimationBridgeDefaultsSpec,
    "DarkArisen.M1.AnimationBridgeDefaults",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM1AnimationBridgeDefaultsSpec::RunTest(const FString& Parameters)
{
    const UDarkArisenAnimInstance* Animation = GetDefault<UDarkArisenAnimInstance>();
    TestTrue(TEXT("Animation bridge created"), Animation != nullptr);
    TestEqual(TEXT("Default combat state is idle"),
        Animation ? Animation->CombatState : ECombatState::Dead, ECombatState::Idle);
    TestEqual(TEXT("Default weight class is medium"),
        Animation ? Animation->WeaponWeightClass : EWeaponWeightClass::Great,
        EWeaponWeightClass::Medium);
    TestEqual(TEXT("Default posture reads set"),
        Animation ? Animation->PostureVisualState : EPostureVisualState::Broken,
        EPostureVisualState::Set);
    TestEqual(TEXT("Default wound layer is none"),
        Animation ? Animation->WoundLayer : EWoundLayer::Failing, EWoundLayer::None);
    TestFalse(TEXT("Default animation set is not wounded"),
        Animation && Animation->bUseWoundedAnimationSet);
    TestFalse(TEXT("Default loadout is not the Katana"),
        Animation && Animation->bKatanaEquipped);
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
    Camera->EnterAnchoredUntilReleased();
    TestEqual(TEXT("Player-paced Examine is anchored"),
        Camera->CurrentMode, EPlayerCameraMode::Anchored);
    TestTrue(TEXT("Player-paced anchor has no timer"), Camera->AnchoredTimeRemaining < 0.0f);
    TestTrue(TEXT("Player-paced Examine preserves movement"), Camera->AllowsMoveInput());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM1LockOnLeashSpec,
    "DarkArisen.M1.LockOnLeash",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM1LockOnLeashSpec::RunTest(const FString& Parameters)
{
    const ULockOnComponent* LockOn = NewObject<ULockOnComponent>();
    TestTrue(TEXT("Lock-on component created"), LockOn != nullptr);
    TestTrue(TEXT("Acquisition range is twenty metres"), LockOn && FMath::IsNearlyEqual(
        LockOn->MaximumAcquisitionRangeCentimetres, 2000.0f));
    TestTrue(TEXT("Retention leash is twenty-five metres"), LockOn && FMath::IsNearlyEqual(
        LockOn->MaximumRetentionRangeCentimetres, 2500.0f));
    TestTrue(TEXT("Retention leash exceeds acquisition range"), LockOn &&
        LockOn->MaximumRetentionRangeCentimetres > LockOn->MaximumAcquisitionRangeCentimetres);
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
    TestEqual(TEXT("Door open state uses first bit"), APhysicalDoorActor::OpenStateBit, 1);
    TestEqual(TEXT("Door broken state uses second bit"), APhysicalDoorActor::BrokenStateBit, 2);
    UInteractionStateSnapshot* Snapshot = NewObject<UInteractionStateSnapshot>();
    TestTrue(TEXT("Interaction snapshot created"), Snapshot != nullptr);
    if (Snapshot)
    {
        Snapshot->StateByPersistentId.Add(TEXT("M1.TestDoor"),
            APhysicalDoorActor::OpenStateBit | APhysicalDoorActor::BrokenStateBit);
        TestEqual(TEXT("Interaction snapshot preserves state bits"),
            Snapshot->StateByPersistentId.FindRef(TEXT("M1.TestDoor")), 3);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM2MarkerlessQuestFoundationSpec,
    "DarkArisen.M2.MarkerlessQuestFoundation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM2MarkerlessQuestFoundationSpec::RunTest(const FString& Parameters)
{
    TestEqual(TEXT("Exactly six authored activation triggers"),
        static_cast<uint8>(EQuestActivationTrigger::Count), static_cast<uint8>(6));
    UQuestJournalComponent* Journal = NewObject<UQuestJournalComponent>();
    TestTrue(TEXT("Quest journal component created"), Journal != nullptr);
    if (!Journal) return false;

    FQuestActivationDefinition Conversation;
    Conversation.QuestId = TEXT("M2.Test.Conversation");
    Conversation.Trigger = EQuestActivationTrigger::Conversation;
    Conversation.InitialEntryId = TEXT("M2.Entry.Conversation");
    Conversation.InitialJakeText = FText::FromString(TEXT("She mentioned the old ford."));
    Conversation.InitialDirections = FText::FromString(TEXT("East of the split cedar."));
    Conversation.MutuallyExclusiveQuestIds.Add(TEXT("M2.Test.OtherPath"));

    FQuestActivationDefinition OtherPath;
    OtherPath.QuestId = TEXT("M2.Test.OtherPath");
    OtherPath.Trigger = EQuestActivationTrigger::Object;
    OtherPath.bSilentAtActivation = true;

    TestTrue(TEXT("Conversation definition registered"),
        Journal->RegisterQuestDefinition(Conversation));
    TestTrue(TEXT("Excluded definition registered"),
        Journal->RegisterQuestDefinition(OtherPath));
    TestTrue(TEXT("Conversation activates without an accept button"), Journal->ActivateQuest(
        Conversation.QuestId, EQuestActivationTrigger::Conversation, 100, false));
    TestEqual(TEXT("Normal activation writes exactly one chronological note"),
        Journal->GetJournalEntries().Num(), 1);

    FQuestRuntimeState RuntimeState;
    TestTrue(TEXT("Conversation state can be read"),
        Journal->TryGetQuestState(Conversation.QuestId, RuntimeState));
    TestEqual(TEXT("Conversation is active"), RuntimeState.Lifecycle, EQuestLifecycle::Active);
    TestTrue(TEXT("Excluded path state can be read"),
        Journal->TryGetQuestState(OtherPath.QuestId, RuntimeState));
    TestEqual(TEXT("Mutual exclusion is silent and persistent"),
        RuntimeState.Lifecycle, EQuestLifecycle::Unavailable);

    UQuestJournalComponent* ReverseJournal = NewObject<UQuestJournalComponent>();
    TestTrue(TEXT("Reverse exclusion journal created"), ReverseJournal != nullptr);
    if (ReverseJournal)
    {
        TestTrue(TEXT("Reverse conversation definition registered"),
            ReverseJournal->RegisterQuestDefinition(Conversation));
        TestTrue(TEXT("Reverse other-path definition registered"),
            ReverseJournal->RegisterQuestDefinition(OtherPath));
        TestTrue(TEXT("Either side can establish a mutual exclusion"),
            ReverseJournal->ActivateQuest(
                OtherPath.QuestId, EQuestActivationTrigger::Object, 100, false));
        TestTrue(TEXT("Reverse excluded state can be read"),
            ReverseJournal->TryGetQuestState(Conversation.QuestId, RuntimeState));
        TestEqual(TEXT("Mutual exclusion is symmetric"),
            RuntimeState.Lifecycle, EQuestLifecycle::Unavailable);
    }

    FQuestActivationDefinition Silent;
    Silent.QuestId = TEXT("M2.Test.Silent");
    Silent.Trigger = EQuestActivationTrigger::Proximity;
    Silent.bSilentAtActivation = true;
    TestTrue(TEXT("Silent definition registered"), Journal->RegisterQuestDefinition(Silent));
    TestTrue(TEXT("Proximity activation succeeds silently"), Journal->ActivateQuest(
        Silent.QuestId, EQuestActivationTrigger::Proximity, 120, false));
    TestEqual(TEXT("Silent activation adds no journal entry"),
        Journal->GetJournalEntries().Num(), 1);
    TestTrue(TEXT("Later knowledge reveals the silent quest"), Journal->RevealSilentQuest(
        Silent.QuestId,
        TEXT("M2.Entry.Rumour"),
        FText::FromString(TEXT("They said the bridge was north.")),
        FText::FromString(TEXT("North of the flooded field.")),
        360,
        true));
    TestTrue(TEXT("A later correction appends instead of erasing the rumour"),
        Journal->AppendJournalCorrection(
            Silent.QuestId,
            TEXT("M2.Entry.Correction"),
            FText::FromString(TEXT("The bridge is west, not north.")),
            FText::FromString(TEXT("West of the flooded field.")),
            420));
    const TArray<FQuestJournalEntry> Entries = Journal->GetJournalEntries();
    TestEqual(TEXT("Rumour and correction both remain"), Entries.Num(), 3);
    if (Entries.Num() >= 3)
    {
        TestTrue(TEXT("Rumour remains marked distorted"), Entries[1].bDistorted);
        TestTrue(TEXT("Correction is explicitly chronological"),
            Entries[2].bCorrection && Entries[2].Sequence > Entries[1].Sequence);
    }
    TestEqual(TEXT("Plain notebook search finds the correction"),
        Journal->SearchJournal(TEXT("west")).Num(), 1);

    FQuestActivationDefinition Contract;
    Contract.QuestId = TEXT("M2.Test.Contract");
    Contract.Trigger = EQuestActivationTrigger::Conversation;
    Contract.bRequiresSpokenAgreement = true;
    Contract.InitialEntryId = TEXT("M2.Entry.Contract");
    Contract.InitialJakeText = FText::FromString(TEXT("I gave my word."));
    TestTrue(TEXT("Contract definition registered"),
        Journal->RegisterQuestDefinition(Contract));
    TestFalse(TEXT("Contract cannot start before the spoken agreement"), Journal->ActivateQuest(
        Contract.QuestId, EQuestActivationTrigger::Conversation, 500, false));
    TestTrue(TEXT("Spoken agreement starts the contract"), Journal->ActivateQuest(
        Contract.QuestId, EQuestActivationTrigger::Conversation, 500, true));

    FQuestActivationDefinition Expiring;
    Expiring.QuestId = TEXT("M2.Test.Expiring");
    Expiring.Trigger = EQuestActivationTrigger::WorldState;
    Expiring.bSilentAtActivation = true;
    Expiring.ExpirationDurationMinutes = 30;
    TestTrue(TEXT("Expiring definition registered"),
        Journal->RegisterQuestDefinition(Expiring));
    TestTrue(TEXT("World-state quest activates"), Journal->ActivateQuest(
        Expiring.QuestId, EQuestActivationTrigger::WorldState, 600, false));
    TestEqual(TEXT("No early expiry"), Journal->ProcessExpirations(629), 0);
    TestEqual(TEXT("Expiry resolves without a warning entry"),
        Journal->ProcessExpirations(630), 1);
    TestTrue(TEXT("Expired state can be read"),
        Journal->TryGetQuestState(Expiring.QuestId, RuntimeState));
    TestEqual(TEXT("Expiry is an outcome, not a failure state"),
        RuntimeState.Lifecycle, EQuestLifecycle::Resolved);
    TestEqual(TEXT("Expiry added no journal notification"),
        Journal->GetJournalEntries().Num(), 4);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM2HeatRhythmSpec,
    "DarkArisen.M2.HeatRhythm",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM2HeatRhythmSpec::RunTest(const FString& Parameters)
{
    const float Shade = UHeatExposureComponent::EvaluateNextHeatStress(
        0.5f, EHeatExposureZone::ShadeGallery, 10.0f, 0.01f, 0.035f, 0.1f);
    const float Exposed = UHeatExposureComponent::EvaluateNextHeatStress(
        0.5f, EHeatExposureZone::ExposedHeat, 10.0f, 0.01f, 0.035f, 0.1f);
    const float Cistern = UHeatExposureComponent::EvaluateNextHeatStress(
        0.5f, EHeatExposureZone::CoolCistern, 10.0f, 0.01f, 0.035f, 0.1f);
    TestTrue(TEXT("Shade accumulates heat"), Shade > 0.5f);
    TestTrue(TEXT("Exposed heat builds faster than shade"), Exposed > Shade);
    TestTrue(TEXT("Cistern recovers heat stress"), Cistern < 0.5f);
    TestTrue(TEXT("Heat stress clamps at one"), FMath::IsNearlyEqual(
        UHeatExposureComponent::EvaluateNextHeatStress(
            0.99f, EHeatExposureZone::ExposedHeat, 10.0f, 0.01f, 0.035f, 0.1f),
        1.0f));
    TestTrue(TEXT("Heat stress clamps at zero"), FMath::IsNearlyZero(
        UHeatExposureComponent::EvaluateNextHeatStress(
            0.01f, EHeatExposureZone::CoolCistern, 10.0f, 0.01f, 0.035f, 0.1f)));
    TestTrue(TEXT("Severe heat reduces stamina regeneration"),
        UHeatExposureComponent::EvaluateStaminaRegenMultiplier(1.0f, 0.35f) <
        UHeatExposureComponent::EvaluateStaminaRegenMultiplier(0.0f, 0.35f));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM2IsabelCruzStateSpec,
    "DarkArisen.M2.IsabelCruzState",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM2IsabelCruzStateSpec::RunTest(const FString& Parameters)
{
    TestTrue(TEXT("List threshold is locked at sixty-five percent"), FMath::IsNearlyEqual(
        AIsabelCruzCharacter::ListPhaseHealthFraction, 0.65f));
    TestTrue(TEXT("1846 threshold is locked at thirty percent"), FMath::IsNearlyEqual(
        AIsabelCruzCharacter::EighteenFortySixPhaseHealthFraction, 0.30f));
    TestEqual(TEXT("Above sixty-five percent is Duty"),
        AIsabelCruzCharacter::EvaluatePhase(0.6501f), EIsabelCruzPhase::Duty);
    TestEqual(TEXT("Sixty-five percent enters List"),
        AIsabelCruzCharacter::EvaluatePhase(0.65f), EIsabelCruzPhase::List);
    TestEqual(TEXT("Above thirty percent remains List"),
        AIsabelCruzCharacter::EvaluatePhase(0.3001f), EIsabelCruzPhase::List);
    TestEqual(TEXT("Thirty percent enters 1846"),
        AIsabelCruzCharacter::EvaluatePhase(0.30f), EIsabelCruzPhase::EighteenFortySix);

    UCombatComponent* Combat = NewObject<UCombatComponent>();
    TestTrue(TEXT("Combat component created for mercy state"), Combat != nullptr);
    if (Combat)
    {
        TestTrue(TEXT("Living combatant begins targetable"), Combat->IsCombatTargetable());
        Combat->SetNonHostile();
        TestEqual(TEXT("Mercy has a distinct non-hostile state"),
            Combat->CurrentState, ECombatState::NonHostile);
        TestFalse(TEXT("Spared combatant cannot be locked or hit"),
            Combat->IsCombatTargetable());
        Combat->SetDead();
        TestEqual(TEXT("Death remains distinct from mercy"),
            Combat->CurrentState, ECombatState::Dead);
    }
    return true;
}

#endif
