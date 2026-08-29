// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Missions/CrewPersonalThreadsComponent.h"
#include "Systems/ProgressionEconomyComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenCrewPersonalThreadsSpec,
    "DarkArisen.Content.Threads.Crew.SourceGroundedState",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenCrewPersonalThreadsSpec::RunTest(const FString& Parameters)
{
    UCrewPersonalThreadsComponent* Threads = NewObject<UCrewPersonalThreadsComponent>();
    UProgressionEconomyComponent* Progression = NewObject<UProgressionEconomyComponent>();
    TestNotNull(TEXT("Crew Thread fixture exists"), Threads);
    TestNotNull(TEXT("Progression fixture exists"), Progression);
    if (!Threads || !Progression)
    {
        return false;
    }

    TestFalse(TEXT("Ines cannot activate before Chapter 5"), Threads->ActivateInesThread(4, 3));
    TestFalse(TEXT("Ines cannot activate before the third meeting"), Threads->ActivateInesThread(5, 2));
    TestTrue(TEXT("Ines activates at Chapter 5+ on the third meeting"), Threads->ActivateInesThread(5, 3));
    TestTrue(TEXT("Ines portrait stage records"), Threads->RecordInesPortraitShared());
    TestTrue(TEXT("Ines first lead records"), Threads->RecordInesFirstLead());
    TestTrue(TEXT("Ines Imperial records stage records"), Threads->RecordInesImperialRecordsFound());
    TestTrue(TEXT("Ines Crimson trail stage records"), Threads->RecordInesCrimsonTrailFound());
    TestTrue(TEXT("Ines household stage records"), Threads->RecordInesHouseholdLocated());
    TestFalse(TEXT("Ines cannot resolve without an authored Esperanza outcome"),
        Threads->ResolveInesTruth(EEsperanzaOutcome::None));
    TestTrue(TEXT("Ines can resolve to the source-authored servitude outcome"),
        Threads->ResolveInesTruth(EEsperanzaOutcome::AliveInServitude));
    TestEqual(TEXT("Ines Thread resolves"),
        Threads->GetLifecycle(ECrewPersonalThread::InesEsperanza), ECrewThreadLifecycle::Resolved);
    TestTrue(TEXT("Resolved Ines Thread awards exactly its authored Mark source once"),
        Threads->ClaimCompletionMarks(ECrewPersonalThread::InesEsperanza, Progression));
    TestFalse(TEXT("Crew Thread Marks cannot be claimed twice"),
        Threads->ClaimCompletionMarks(ECrewPersonalThread::InesEsperanza, Progression));

    TestFalse(TEXT("Mira cannot activate outside the Fjordlund overheard trigger"),
        Threads->ActivateMiraThread(false, false));
    TestTrue(TEXT("Mira activates from the authored Fjordlund overheard event"),
        Threads->ActivateMiraThread(true, false));
    TestTrue(TEXT("Mira first lead records"), Threads->RecordMiraFirstLead());
    TestTrue(TEXT("Mira search records"), Threads->RecordMiraSearchUnderway());
    TestTrue(TEXT("Mira crucial lead records"), Threads->RecordMiraCrucialLead());
    TestTrue(TEXT("Mira Armada investigation records"), Threads->RecordMiraArmadaInvestigation());
    TestTrue(TEXT("Mira can resolve to the source-authored captive outcome"),
        Threads->ResolveMiraTruth(EMiraBrotherOutcome::AliveCaptive));
    TestFalse(TEXT("Resolved Mira Thread cannot later expire"), Threads->RecordFjordlundControlResolved());

    UCrewPersonalThreadsComponent* ExpiryFixture = NewObject<UCrewPersonalThreadsComponent>();
    TestNotNull(TEXT("Mira expiry fixture exists"), ExpiryFixture);
    if (ExpiryFixture)
    {
        TestFalse(TEXT("Resolved Fjordlund control blocks late Mira activation"),
            ExpiryFixture->ActivateMiraThread(true, true));
        TestEqual(TEXT("Late Mira Thread is recorded as expired without a timer/warning state"),
            ExpiryFixture->GetLifecycle(ECrewPersonalThread::MirasCoast), ECrewThreadLifecycle::Expired);
    }

    TestTrue(TEXT("Big Tom activates only from the authored recogniser-leaves absence"),
        Threads->ActivateBigTomThread(true));
    TestTrue(TEXT("Big Tom service story records"), Threads->RecordBigTomServiceStory());
    TestTrue(TEXT("Big Tom tracking records"), Threads->RecordBigTomTrackingReinhardt());
    TestTrue(TEXT("Big Tom choice stage records"), Threads->RecordBigTomChoiceFormed());
    TestTrue(TEXT("Big Tom confrontation stage records"), Threads->RecordBigTomConfrontationReached());
    TestTrue(TEXT("Big Tom walk-away outcome is a full source-authored resolution"),
        Threads->ResolveBigTomService(EBigTomServiceOutcome::WalkAway));

    TestFalse(TEXT("Salvio cannot activate without Jake having spared someone"),
        Threads->ActivateSalvioParishThread(false, true));
    TestTrue(TEXT("Salvio's later parish identity activates from its exact source trigger"),
        Threads->ActivateSalvioParishThread(true, true));
    TestEqual(TEXT("Under-authored Salvio parish content fails closed after activation"),
        Threads->GetLifecycle(ECrewPersonalThread::FatherSalviosParish),
        ECrewThreadLifecycle::AwaitingAuthoredDetail);

    TestTrue(TEXT("Esteban's physical chart in his quarters activates the later Thread identity"),
        Threads->ActivateEstebanLastChartThread(true));
    TestEqual(TEXT("Under-authored last-chart destination/content fails closed after activation"),
        Threads->GetLifecycle(ECrewPersonalThread::EstebansLastChart),
        ECrewThreadLifecycle::AwaitingAuthoredDetail);

    TestFalse(TEXT("Crew Thread system never generates missing quest stages"),
        Threads->AllowsGeneratedCrewQuestStages());
    TestFalse(TEXT("Crew Thread system does not introduce systemic romance"),
        Threads->AllowsSystemicRomance());

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
