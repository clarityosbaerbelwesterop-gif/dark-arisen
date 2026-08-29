// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Missions/ArchipelagoThreadsComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenArchipelagoThreadsSpec,
    "DarkArisen.Content.Threads.Archipelago.SourceGroundedState",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenArchipelagoThreadsSpec::RunTest(const FString& Parameters)
{
    UArchipelagoThreadsComponent* Threads = NewObject<UArchipelagoThreadsComponent>();
    TestNotNull(TEXT("Archipelago Thread fixture exists"), Threads);
    if (!Threads)
    {
        return false;
    }

    TestTrue(TEXT("Kessler activates from established initial trust"),
        Threads->ActivateKesslerInvestigation(true));
    TestTrue(TEXT("Kessler bribe-officer investigation records"),
        Threads->RecordKesslerBribeOfficerInvestigated());
    TestTrue(TEXT("Kessler noble network investigation records"),
        Threads->RecordKesslerNobleNetworkInvestigated());
    TestTrue(TEXT("Kessler Crimson protection investigation records"),
        Threads->RecordKesslerCrimsonProtectionInvestigated());
    TestTrue(TEXT("Castillo Dorado bribe ledgers terminate Kessler's Thread"),
        Threads->RecordCastilloDoradoBribeLedgersRecovered());
    TestEqual(TEXT("Kessler resolves after the ledgers"),
        Threads->GetKesslerLifecycle(), EArchipelagoThreadLifecycle::Resolved);
    TestFalse(TEXT("Resolved Kessler Thread cannot be retroactively expired"),
        Threads->RecordKesslerExpiryWorldState(true, false));

    UArchipelagoThreadsComponent* KesslerExpiry = NewObject<UArchipelagoThreadsComponent>();
    TestNotNull(TEXT("Kessler expiry fixture exists"), KesslerExpiry);
    if (KesslerExpiry)
    {
        TestTrue(TEXT("De Silva death expires unresolved Kessler content immediately"),
            KesslerExpiry->RecordKesslerExpiryWorldState(false, true));
        TestEqual(TEXT("Kessler expiry carries no timer state"),
            KesslerExpiry->GetKesslerLifecycle(), EArchipelagoThreadLifecycle::Expired);
    }

    TestTrue(TEXT("Drowned Plantation ledger records"),
        Threads->RecordLedgerRecovered(ELedgerTrilogyRecord::DrownedPlantation));
    TestTrue(TEXT("Valdena Mine rolls record"),
        Threads->RecordLedgerRecovered(ELedgerTrilogyRecord::ValdenaMine));
    TestTrue(TEXT("Fuerte Esperanza ledger records"),
        Threads->RecordLedgerRecovered(ELedgerTrilogyRecord::FuerteEsperanza));
    TestFalse(TEXT("Duplicate labour records cannot pad the chain"),
        Threads->RecordLedgerRecovered(ELedgerTrilogyRecord::FuerteEsperanza));
    TestTrue(TEXT("Company Works books complete the four-location chain"),
        Threads->RecordLedgerRecovered(ELedgerTrilogyRecord::CompanyWorks));
    TestTrue(TEXT("Ledger Trilogy resolves only after all four source locations"),
        Threads->IsLedgerTrilogyResolved());

    TestTrue(TEXT("Schreiber research relationship opens"), Threads->ActivateSchreiberDocumentation(true));
    TestTrue(TEXT("Schreiber ethical documentation method records"),
        Threads->RecordSchreiberEthicalMethodEstablished());
    TestTrue(TEXT("Harbour That Was First is reached"), Threads->RecordHarbourThatWasFirstReached());
    TestTrue(TEXT("Ancient boats are documented and deliberately left untaken"),
        Threads->RecordFirstHarbourBoatsDocumentedAndLeftUntaken());
    TestTrue(TEXT("The datable first-harbour record resolves Schreiber's documentation Thread"),
        Threads->RecordFirstHarbourDatableRecordRecovered());
    TestEqual(TEXT("Schreiber Thread resolves"),
        Threads->GetSchreiberLifecycle(), EArchipelagoThreadLifecycle::Resolved);

    TestTrue(TEXT("Vasquez's captain-peer Thread identity can open"), Threads->ActivateVasquezThread(true));
    TestEqual(TEXT("Under-authored Vasquez stage chain fails closed instead of generating filler"),
        Threads->GetVasquezLifecycle(), EArchipelagoThreadLifecycle::AwaitingAuthoredDetail);
    TestTrue(TEXT("Assassin-network evidence may record Vasquez as a private engager"),
        Threads->RecordVasquezPrivateEngagementEvidence(true));
    TestTrue(TEXT("Vasquez evidence persists"), Threads->HasVasquezPrivateEngagementEvidence());
    TestTrue(TEXT("Vasquez being sunk expires the unresolved Thread"), Threads->RecordVasquezSunk());
    TestEqual(TEXT("Vasquez expiry is world-state based"),
        Threads->GetVasquezLifecycle(), EArchipelagoThreadLifecycle::Expired);

    TestFalse(TEXT("Old Fortress cannot count without Alliance access"),
        Threads->RecordOldFortressReclaimed(EOldFortressSite::LaGuardiaVieja, false, true));
    TestTrue(TEXT("La Guardia Vieja reclamation records"),
        Threads->RecordOldFortressReclaimed(EOldFortressSite::LaGuardiaVieja, true, true));
    TestTrue(TEXT("Drowned Hold reclamation records"),
        Threads->RecordOldFortressReclaimed(EOldFortressSite::DrownedHold, true, true));
    TestTrue(TEXT("Ash Bastion reclamation records"),
        Threads->RecordOldFortressReclaimed(EOldFortressSite::AshBastion, true, true));
    TestTrue(TEXT("Fourth Fortress reclamation completes the exact four-site Thread"),
        Threads->RecordOldFortressReclaimed(EOldFortressSite::FourthFortress, true, true));
    TestEqual(TEXT("Old Fortresses resolves at four physical reclamations"),
        Threads->GetOldFortressesLifecycle(), EArchipelagoThreadLifecycle::Resolved);

    TestFalse(TEXT("No missing archipelago Thread stages are generated"),
        Threads->AllowsGeneratedThreadStages());
    TestFalse(TEXT("World-state expiration never creates a timer/warning UI contract"),
        Threads->AllowsQuestExpiryTimerUI());

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
