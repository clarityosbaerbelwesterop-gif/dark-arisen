// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "ColonialWar/ArmyCampaignComponent.h"
#include "ColonialWar/CastleSiegeComponent.h"
#include "ColonialWar/ColonialWarStateSubsystem.h"
#include "ColonialWar/LargeBattleComponent.h"
#include "ColonialWar/RetaliationSubsystem.h"
#include "Highmoore/CrystalCavesPassageComponent.h"
#include "Highmoore/CrystalGuardianComponent.h"
#include "Highmoore/CrystalKatanaComponent.h"
#include "Highmoore/HighmooreHorseComponent.h"
#include "Highmoore/PrincessQuestStateComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM5WarStateSpec,
    "DarkArisen.M5.WarStateAndRetaliation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM5WarStateSpec::RunTest(const FString& Parameters)
{
    UColonialWarStateSubsystem* War = NewObject<UColonialWarStateSubsystem>();
    TestNotNull(TEXT("War subsystem constructs"), War);
    if (!War) return false;

    TestTrue(TEXT("Imperial region registers"), War->RegisterRegion(TEXT("region.test"), EColonialFaction::Imperial));
    TestTrue(TEXT("Physical Break consequence is accepted"),
        War->RecordResolvedWarAction(TEXT("region.test"), EWarActionVerb::Break,
            EColonialFaction::Imperial, 50, 0, 0));
    TestEqual(TEXT("Region enters Crisis after colonial control drops below thirty"),
        War->GetRegionSnapshot(TEXT("region.test")).Outcome, ERegionalWarOutcome::Crisis);
    TestTrue(TEXT("Liberation work is distinct from breaking"),
        War->RecordResolvedWarAction(TEXT("region.test"), EWarActionVerb::Liberate,
            EColonialFaction::Liberation, 0, 50, 0));
    TestFalse(TEXT("Crisis alone is not liberation"), War->IsRegionLiberated(TEXT("region.test")));
    TestTrue(TEXT("Authored Fall assault can resolve"), War->RecordFallAssaultCompleted(TEXT("region.test")));
    TestTrue(TEXT("Fall plus Liberation Strength fifty resolves Liberation"),
        War->IsRegionLiberated(TEXT("region.test")));

    TestTrue(TEXT("Chapter five advances"), War->AdvanceChapter(5));
    for (int32 Chapter = 5; Chapter <= 10; ++Chapter)
    {
        if (Chapter > 5)
        {
            TestTrue(TEXT("Campaign chapter advances once"), War->AdvanceChapter(Chapter));
        }
        FRegionalAutonomousWarTick Tick;
        Tick.ImperialDelta = -1;
        Tick.AlbionDelta = 1;
        TestTrue(TEXT("War advances without Jake once per chapter"),
            War->RecordAutonomousChapterTick(TEXT("region.test"), Tick));
        TestFalse(TEXT("Duplicate autonomous tick in one chapter fails closed"),
            War->RecordAutonomousChapterTick(TEXT("region.test"), Tick));
    }
    TestEqual(TEXT("Chapter ten is Gamma momentum"), War->GetMomentumPhase(), EWarMomentumPhase::Gamma);

    URetaliationSubsystem* Retaliation = NewObject<URetaliationSubsystem>();
    TestNotNull(TEXT("Retaliation subsystem constructs"), Retaliation);
    if (!Retaliation) return false;
    TestTrue(TEXT("Imperial damage raises hidden Heat"),
        Retaliation->RecordFactionDamage(EColonialFaction::Imperial, 100));
    TestEqual(TEXT("Alpha phase caps retaliation at Bounty"),
        Retaliation->GetCurrentStage(EColonialFaction::Imperial), ERetaliationStage::Bounty);
    Retaliation->AdvanceChapter(7);
    TestEqual(TEXT("Beta phase caps retaliation at Hostages"),
        Retaliation->GetCurrentStage(EColonialFaction::Imperial), ERetaliationStage::Hostages);

    FHostageCandidate Child;
    Child.PersonId = TEXT("person.child");
    Child.RegionId = TEXT("region.test");
    Child.AttachmentScore = 100;
    Child.bChild = true;
    TestTrue(TEXT("Child may exist in relationship data"), Retaliation->RegisterHostageCandidate(Child));

    FHostageCandidate CrewAdult;
    CrewAdult.PersonId = TEXT("crew.mira");
    CrewAdult.RegionId = TEXT("region.test");
    CrewAdult.AttachmentScore = 90;
    CrewAdult.bCrew = true;
    TestTrue(TEXT("Adult crew candidate registers under newer Phase-12 law"),
        Retaliation->RegisterHostageCandidate(CrewAdult));
    const TArray<FName> Hostages = Retaliation->SelectHostages(
        EColonialFaction::Imperial, TEXT("region.test"), 2);
    TestFalse(TEXT("Children are never hostage candidates"), Hostages.Contains(Child.PersonId));
    TestTrue(TEXT("Adult crew remain eligible unless individually protected"), Hostages.Contains(CrewAdult.PersonId));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM5SiegeArmyBattleSpec,
    "DarkArisen.M5.SiegeArmyBattle",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM5SiegeArmyBattleSpec::RunTest(const FString& Parameters)
{
    UCastleSiegeComponent* Siege = NewObject<UCastleSiegeComponent>();
    TestTrue(TEXT("Castle config"), Siege->ConfigureCastle(TEXT("castle.test"), true));
    TestTrue(TEXT("Siege begins at Approach"), Siege->BeginSiege(false));
    TestTrue(TEXT("Blind Wall breach is legal"), Siege->CommitBreachDoor(ESiegeDoor::Wall));
    TestTrue(TEXT("Wall breach records persistent damage"), Siege->RecordBreachCompleted(true));
    TestTrue(TEXT("Commander resolves"), Siege->RecordCommanderResolved(TEXT("commander.test")));
    TestFalse(TEXT("Broken castle cannot be claimed intact"), Siege->ResolveSiege(ESiegeResolution::HeldIntact));
    TestTrue(TEXT("Broken castle can be held damaged"), Siege->ResolveSiege(ESiegeResolution::HeldDamaged));

    UArmyCampaignComponent* Army = NewObject<UArmyCampaignComponent>();
    FCampaignForceDefinition Hired;
    Hired.ForceId = TEXT("force.ridd");
    Hired.SourceId = TEXT("house.ridd");
    Hired.Type = ECampaignForceType::Hired;
    Hired.Men = 500;
    TestTrue(TEXT("Named hired force registers"), Army->RegisterForce(Hired));
    TestEqual(TEXT("Five hundred hired men cost seven thousand doubloons/chapter"),
        Army->CalculateChapterDoubloonUpkeep(), int64(7000));
    TestTrue(TEXT("Five hundred men exceed Jake-alone capacity"), Army->IsOverCommandCapacity());
    TestTrue(TEXT("Two named officers can raise capacity"), Army->SetNamedOfficerCount(2));
    TestFalse(TEXT("Officer capacity now covers the force"), Army->IsOverCommandCapacity());
    TestTrue(TEXT("HOLD is a legal order"), Army->IssueOrder(Hired.ForceId, EArmyOrder::Hold));
    Army->AdvanceChapter();
    TestTrue(TEXT("Hired force survives first unpaid chapter"), Army->IsForceActiveForTests(Hired.ForceId));
    Army->AdvanceChapter();
    TestFalse(TEXT("Hired force leaves after second unpaid chapter"), Army->IsForceActiveForTests(Hired.ForceId));

    ULargeBattleComponent* Battle = NewObject<ULargeBattleComponent>();
    TestTrue(TEXT("Battle begins"), Battle->BeginBattle(TEXT("battle.test")));
    TestTrue(TEXT("Jake may fall"), Battle->RecordJakeFallen());
    TestTrue(TEXT("Battle continues after Jake falls"), Battle->IsBattleActive());
    TestFalse(TEXT("Jake is no longer active after falling"), Battle->IsJakeActiveInBattle());
    TestTrue(TEXT("World simulation resolves battle later"), Battle->ResolveBattle(ELargeBattleOutcome::Defeat));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM6HighmooreSpec,
    "DarkArisen.M6.HighmooreCoreLaws",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM6HighmooreSpec::RunTest(const FString& Parameters)
{
    UCrystalCavesPassageComponent* Caves = NewObject<UCrystalCavesPassageComponent>();
    TestTrue(TEXT("First passage can start with authored physical distance"), Caves->BeginFirstPassage(1000.0f));
    TestFalse(TEXT("First passage cannot complete instantly"), Caves->CompleteFirstPassage());
    TestEqual(TEXT("First passage minimum remains ninety real minutes"),
        UCrystalCavesPassageComponent::FirstPassageMinimumRealSeconds, 5400.0f);
    TestEqual(TEXT("Repeat passage minimum remains four real minutes"),
        UCrystalCavesPassageComponent::RepeatPassageMinimumRealSeconds, 240.0f);

    UCrystalGuardianComponent* Guardian = NewObject<UCrystalGuardianComponent>();
    TestEqual(TEXT("Guardian starts dormant and does not attack first"),
        Guardian->GetPhase(), ECrystalGuardianPhase::Dormant);
    TestFalse(TEXT("Dormant Guardian ignores light until engaged"),
        Guardian->RecordLightStimulus(FVector::ZeroVector, 1.0f));
    TestTrue(TEXT("Guardian can be bypassed without combat"), Guardian->RecordGuardianBypassed());
    TestFalse(TEXT("Bypassed Guardian yields no Katana"), Guardian->CanYieldCrystalKatana());
    TestEqual(TEXT("Guardian defeat hold remains four seconds"),
        UCrystalGuardianComponent::DefeatHoldSeconds, 4.0f);

    UCrystalKatanaComponent* Katana = NewObject<UCrystalKatanaComponent>();
    TestFalse(TEXT("Katana cannot be taken from a bypassed Guardian"), Katana->AcquireFromDefeatedGuardian(false));
    TestTrue(TEXT("Katana can be acquired after defeated Guardian"), Katana->AcquireFromDefeatedGuardian(true));
    TestEqual(TEXT("Katana blade remains 74 cm"), Katana->GetBladeLengthCentimetres(), 74.0f);
    TestEqual(TEXT("Katana weight remains 1.1 kg"), Katana->GetWeightKilograms(), 1.1f);
    TestEqual(TEXT("Katana cost remains condition-independent"), Katana->ResolveFlatAttackStaminaCost(7.5f), 7.5f);

    UHighmooreHorseComponent* Horse = NewObject<UHighmooreHorseComponent>();
    TestTrue(TEXT("Lake ride begins"), Horse->BeginLakeRide());
    TestFalse(TEXT("Lake ride refuses canter above authored trot cap"), Horse->SetGait(EHorseGait::Canter));
    Horse->TickComponent(UHighmooreHorseComponent::LakeRideRequiredSeconds, LEVELTICK_All, nullptr);
    TestTrue(TEXT("Nine-minute lake ride completes after elapsed time"), Horse->CompleteAuthoredRide());
    TestTrue(TEXT("Arion ride begins as authored gallop"), Horse->BeginArionRide());
    TestFalse(TEXT("Arion ride cannot be slowed to trot"), Horse->SetGait(EHorseGait::Trot));
    Horse->TickComponent(UHighmooreHorseComponent::ArionRideRequiredSeconds, LEVELTICK_All, nullptr);
    TestTrue(TEXT("Four-minute Arion ride completes"), Horse->CompleteAuthoredRide());
    TestTrue(TEXT("Suspended-stamina ride continues east to Belos"), Horse->ContinueArionRideToBelos());
    Horse->TickComponent(UHighmooreHorseComponent::BelosRideRequiredSeconds, LEVELTICK_All, nullptr);
    TestTrue(TEXT("Belos ride completes"), Horse->CompleteAuthoredRide());
    TestTrue(TEXT("Revenge rides leave lasting horse damage"), Horse->HasLastingRevengeRideDamage());

    UPrincessQuestStateComponent* Princess = NewObject<UPrincessQuestStateComponent>();
    TestFalse(TEXT("Arrow has no slow motion"), Princess->AllowsArrowSlowMotion());
    TestFalse(TEXT("Arrow has no cue change"), Princess->AllowsArrowMusicCueChange());
    TestFalse(TEXT("Arrow has no camera move"), Princess->AllowsArrowCameraMove());
    TestFalse(TEXT("Arrow has no telegraph"), Princess->AllowsArrowTelegraph());
    TestFalse(TEXT("Arrow has no revive prompt"), Princess->AllowsArrowRevivePrompt());
    TestTrue(TEXT("Player control remains at arrow"), Princess->RetainsPlayerControlAtArrow());
    TestFalse(TEXT("Arion child is never damageable"), Princess->CanDamageArionActor(true, true));
    TestFalse(TEXT("Arion noncombatant is never damageable"), Princess->CanDamageArionActor(false, false));
    TestTrue(TEXT("Arion adult combatant remains combat-valid"), Princess->CanDamageArionActor(true, false));
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
