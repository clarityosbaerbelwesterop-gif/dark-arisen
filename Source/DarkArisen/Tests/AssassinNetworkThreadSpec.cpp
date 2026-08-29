// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "ColonialWar/RetaliationSubsystem.h"
#include "Missions/AssassinNetworkThreadComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenRetaliationHistorySpec,
    "DarkArisen.Content.Threads.AssassinNetwork.RetaliationHistory",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenRetaliationHistorySpec::RunTest(const FString& Parameters)
{
    URetaliationSubsystem* Retaliation = NewObject<URetaliationSubsystem>();
    TestNotNull(TEXT("Retaliation fixture exists"), Retaliation);
    if (!Retaliation)
    {
        return false;
    }

    TestTrue(TEXT("Heat may reach raw Stage 3 while Alpha cap holds it at Bounty"),
        Retaliation->RecordFactionDamage(EColonialFaction::Imperial, 60));
    TestEqual(TEXT("Alpha chapter cap remains Stage 2"),
        Retaliation->GetCurrentStage(EColonialFaction::Imperial),
        ERetaliationStage::Bounty);
    TestFalse(TEXT("Counter-Attack was not yet genuinely active under the cap"),
        Retaliation->HasReachedStageAtLeastOnce(
            EColonialFaction::Imperial,
            ERetaliationStage::CounterAttack));

    Retaliation->AdvanceChapter(7);
    TestEqual(TEXT("Beta phase permits the stored Heat to become Stage 3"),
        Retaliation->GetCurrentStage(EColonialFaction::Imperial),
        ERetaliationStage::CounterAttack);
    TestTrue(TEXT("Stage 3 history is retained once genuinely active"),
        Retaliation->HasReachedStageAtLeastOnce(
            EColonialFaction::Imperial,
            ERetaliationStage::CounterAttack));

    TestTrue(TEXT("Heat can later de-escalate"),
        Retaliation->RecordDeEscalation(EColonialFaction::Imperial, 60));
    TestEqual(TEXT("Current stage may return to Unnoticed"),
        Retaliation->GetCurrentStage(EColonialFaction::Imperial),
        ERetaliationStage::Unnoticed);
    TestTrue(TEXT("Historical Stage 3 proof survives de-escalation"),
        Retaliation->HasReachedStageAtLeastOnce(
            EColonialFaction::Imperial,
            ERetaliationStage::CounterAttack));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenAssassinNetworkBranchSpec,
    "DarkArisen.Content.Threads.AssassinNetwork.Branches",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenAssassinNetworkBranchSpec::RunTest(const FString& Parameters)
{
    UAssassinNetworkThreadComponent* Thread = NewObject<UAssassinNetworkThreadComponent>();
    TestNotNull(TEXT("Assassin Network fixture exists"), Thread);
    if (!Thread)
    {
        return false;
    }

    // Entry itself depends on world subsystems; this fixture starts after the authored cabin scene.
    Thread->ThreadState = EAssassinThreadState::Investigation;

    TestTrue(TEXT("First Cofradia hunter is recorded"), Thread->RecordNamedHunterOutcome(
        TEXT("hunter.cofradia.one"), EAssassinHunterOrganisation::Cofradia, EAssassinHunterOutcome::Spared));
    TestTrue(TEXT("Second Cofradia hunter is recorded"), Thread->RecordNamedHunterOutcome(
        TEXT("hunter.cofradia.two"), EAssassinHunterOrganisation::Cofradia, EAssassinHunterOutcome::Killed));
    TestTrue(TEXT("Third Cofradia hunter is recorded"), Thread->RecordNamedHunterOutcome(
        TEXT("hunter.cofradia.three"), EAssassinHunterOrganisation::Cofradia, EAssassinHunterOutcome::Killed));

    TestTrue(TEXT("First chapter-house mark is retained"), Thread->RecordCofradiaChapterMark(
        TEXT("evidence.candle.one"), TEXT("hunter.cofradia.one"), TEXT("mark.one")));
    TestTrue(TEXT("Second chapter-house mark is retained"), Thread->RecordCofradiaChapterMark(
        TEXT("evidence.candle.two"), TEXT("hunter.cofradia.two"), TEXT("mark.two")));
    TestTrue(TEXT("Third chapter-house mark is retained"), Thread->RecordCofradiaChapterMark(
        TEXT("evidence.candle.three"), TEXT("hunter.cofradia.three"), TEXT("mark.three")));
    TestTrue(TEXT("Three distinct marks identify the chapter houses"), Thread->IdentifyCofradiaChapterHouses());

    TestTrue(TEXT("San Benito infiltration records"),
        Thread->RecordCofradiaHouseInfiltrated(ECofradiaChapterHouse::SanBenito));
    TestTrue(TEXT("Valdena-en-Mar infiltration records"),
        Thread->RecordCofradiaHouseInfiltrated(ECofradiaChapterHouse::ValdenaEnMar));
    TestTrue(TEXT("Pale Isle infiltration records"),
        Thread->RecordCofradiaHouseInfiltrated(ECofradiaChapterHouse::PaleIsle));
    TestTrue(TEXT("Writ ledger follows all three houses"), Thread->RecordWritLedgerRecovered());
    TestTrue(TEXT("Primera reveal follows the ledger"), Thread->RecordPrimeraRevealedAsVidal());

    TestFalse(TEXT("Vidal withdrawal is impossible without the earlier real confession"),
        Thread->ResolveCofradiaByWithdrawal(true));
    TestTrue(TEXT("Earlier Vidal confession can be recorded"), Thread->RecordVidalConfessionTaken());
    TestTrue(TEXT("Confession opens the authored withdrawal route"), Thread->ResolveCofradiaByWithdrawal(true));
    TestFalse(TEXT("Cofradia hunters cease after withdrawal"), Thread->AreCofradiaHuntersActive());

    TestTrue(TEXT("Regulator references decode"), Thread->RecordRegulatorFileReferencesDecoded());
    TestTrue(TEXT("Coventry House station is infiltrated"), Thread->RecordCoventryHouseStationInfiltrated());
    TestTrue(TEXT("Register and unactioned contract are recovered"),
        Thread->RecordEngagementRegisterRecovered(true));
    TestTrue(TEXT("Kettle meeting remains a recorded playable dialogue event"),
        Thread->RecordKettleMeetingCompleted());

    TestFalse(TEXT("Partial payment cannot turn the Regulators"),
        Thread->ResolveRegulatorsBySettlement(true, true, false));
    TestTrue(TEXT("All contracts + operating costs + retainer close the engagements"),
        Thread->ResolveRegulatorsBySettlement(true, true, true));
    TestFalse(TEXT("Regulator hunters cease after settlement"), Thread->AreRegulatorHuntersActive());
    TestTrue(TEXT("Turned Regulators expose the warning network"), Thread->IsRegulatorWarningNetworkAvailable());
    TestEqual(TEXT("Both branches resolve the Thread"), Thread->ThreadState, EAssassinThreadState::Resolved);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
