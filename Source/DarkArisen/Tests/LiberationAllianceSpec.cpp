// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "ColonialWar/LiberationAllianceSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenLiberationAllianceConnectionsSpec,
    "DarkArisen.Content.Threads.LiberationAlliance.Connections",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenLiberationAllianceConnectionsSpec::RunTest(const FString& Parameters)
{
    ULiberationAllianceSubsystem* Alliance = NewObject<ULiberationAllianceSubsystem>();
    TestNotNull(TEXT("Alliance fixture exists"), Alliance);
    if (!Alliance)
    {
        return false;
    }

    const TArray<FAllianceConnectionDefinition> Definitions = Alliance->GetConnectionDefinitions();
    TestEqual(TEXT("All ten pairwise network slots are represented"), Definitions.Num(), 10);

    int32 Authored = 0;
    int32 FullVision = 0;
    for (const FAllianceConnectionDefinition& Definition : Definitions)
    {
        if (Definition.Authorship == EAllianceConnectionAuthorship::AuthoredQuest)
        {
            ++Authored;
        }
        else
        {
            ++FullVision;
        }
    }
    TestEqual(TEXT("Five connection quests are actually authored"), Authored, 5);
    TestEqual(TEXT("Five connections remain explicit Full Vision gaps"), FullVision, 5);
    TestEqual(TEXT("Gap query agrees with the source register"),
        Alliance->GetUnresolvedFullVisionConnectionCount(), 5);

    TestFalse(TEXT("A Full Vision pair cannot be completed as if its quest existed"),
        Alliance->RecordAuthoredConnectionCompleted(TEXT("alliance.connection.cultural-patient")));

    TestEqual(TEXT("Isolated networks begin at x1.00"), Alliance->GetConnectionMultiplier(), 1.00f);
    TestTrue(TEXT("Cultural ↔ Recovery is authored"),
        Alliance->RecordAuthoredConnectionCompleted(TEXT("alliance.connection.cultural-recovery")));
    TestEqual(TEXT("Two linked networks use x1.15"), Alliance->GetConnectionMultiplier(), 1.15f);

    TestTrue(TEXT("Cultural ↔ Mountain is authored"),
        Alliance->RecordAuthoredConnectionCompleted(TEXT("alliance.connection.cultural-mountain")));
    TestEqual(TEXT("Three linked networks use x1.25"), Alliance->GetConnectionMultiplier(), 1.25f);

    TestTrue(TEXT("Mountain ↔ Patient is authored"),
        Alliance->RecordAuthoredConnectionCompleted(TEXT("alliance.connection.mountain-patient")));
    TestEqual(TEXT("Four linked networks use x1.40"), Alliance->GetConnectionMultiplier(), 1.40f);

    TestTrue(TEXT("Patient ↔ Deed-Judgers is authored"),
        Alliance->RecordAuthoredConnectionCompleted(TEXT("alliance.connection.patient-deed")));
    TestEqual(TEXT("Four connections can genuinely link all five networks"),
        Alliance->GetConnectedNetworkCount(), 5);
    TestEqual(TEXT("Five linked networks use x1.60"), Alliance->GetConnectionMultiplier(), 1.60f);
    TestTrue(TEXT("Linking the fifth network raises the first-gathering event"),
        Alliance->IsFirstGatheringPending());
    TestTrue(TEXT("The authored first gathering can resolve once"),
        Alliance->RecordFirstGatheringCompleted());
    TestFalse(TEXT("The first gathering cannot resolve twice"),
        Alliance->RecordFirstGatheringCompleted());

    const FLiberationNetworkState Cultural = Alliance->GetNetworkState(ELiberationNetwork::CulturalWeb);
    const FLiberationNetworkState Recovery = Alliance->GetNetworkState(ELiberationNetwork::RecoveryUnderground);
    const FLiberationNetworkState Mountain = Alliance->GetNetworkState(ELiberationNetwork::MountainPaths);
    const FLiberationNetworkState Patient = Alliance->GetNetworkState(ELiberationNetwork::PatientCoast);
    const FLiberationNetworkState Deed = Alliance->GetNetworkState(ELiberationNetwork::DeedJudgers);
    TestEqual(TEXT("Cultural Web source contribution is 12"), Cultural.Strength, 12);
    TestEqual(TEXT("Recovery Underground source contribution is 8"), Recovery.Strength, 8);
    TestEqual(TEXT("Mountain Paths source contribution is 10"), Mountain.Strength, 10);
    TestEqual(TEXT("Patient Coast source contribution is 9"), Patient.Strength, 9);
    TestEqual(TEXT("Deed-Judgers source contribution is 6"), Deed.Strength, 6);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
