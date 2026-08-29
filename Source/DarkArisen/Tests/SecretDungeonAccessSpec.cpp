// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Dungeons/SecretDungeonAccessRules.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenSecretDungeonAccessSpec,
    "DarkArisen.Content.Dungeons.SecretAccess",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

namespace
{
void GiveBlood(FSecretDungeonAccessEvidence& Evidence, const ELiberationNetwork Network)
{
    Evidence.TrustByNetwork.Add(Network, ELiberationTrustTier::Blood);
}
}

bool FDarkArisenSecretDungeonAccessSpec::RunTest(const FString& Parameters)
{
    const TArray<FSecretDungeonAccessRequirement> Requirements = FSecretDungeonAccessRules::BuildRequirements();
    TestEqual(TEXT("Exactly six secret-site access contracts exist"), Requirements.Num(), 6);

    TSet<FName> SeenIds;
    for (const FSecretDungeonAccessRequirement& Requirement : Requirements)
    {
        TestFalse(TEXT("Secret-site access IDs are unique"), SeenIds.Contains(Requirement.DungeonStableId));
        SeenIds.Add(Requirement.DungeonStableId);
        TestTrue(TEXT("Secret-site access has governing source"), !Requirement.GoverningSource.IsEmpty());
        TestTrue(TEXT("Empire path blindness remains enabled"), Requirement.bEmpirePathBlind);
    }

    FString Failure;

    // Grove Beneath the Grove: Blood + map + route + returned artifact.
    FSecretDungeonAccessEvidence Grove;
    GiveBlood(Grove, ELiberationNetwork::CulturalWeb);
    Grove.EvidenceIds = {
        TEXT("knowledge.rexa.secret-grove-map"),
        TEXT("route.rexa.deep-canopy-open"),
        TEXT("artifact.rexa.ceremonial-piece-returned")
    };
    TestTrue(TEXT("Complete Grove evidence opens the secret site"),
        FSecretDungeonAccessRules::CanAccess(TEXT("dungeon.rexa.grove-beneath-grove"), Grove, Failure));
    Grove.EvidenceIds.Remove(TEXT("artifact.rexa.ceremonial-piece-returned"));
    TestFalse(TEXT("Selling/not-returning the Grove artifact keeps the site closed"),
        FSecretDungeonAccessRules::CanAccess(TEXT("dungeon.rexa.grove-beneath-grove"), Grove, Failure));

    // Fifth Connection: all five Blood plus the gathering plus assembled knowledge/route.
    FSecretDungeonAccessEvidence Fifth;
    GiveBlood(Fifth, ELiberationNetwork::CulturalWeb);
    GiveBlood(Fifth, ELiberationNetwork::RecoveryUnderground);
    GiveBlood(Fifth, ELiberationNetwork::MountainPaths);
    GiveBlood(Fifth, ELiberationNetwork::PatientCoast);
    GiveBlood(Fifth, ELiberationNetwork::DeedJudgers);
    Fifth.bFifthConnectionGatheringCompleted = true;
    Fifth.EvidenceIds = {
        TEXT("knowledge.cross-network.five-map-languages-assembled"),
        TEXT("route.cross-network.fifth-site-assembled")
    };
    TestTrue(TEXT("All-five Blood evidence opens Fifth Connection"),
        FSecretDungeonAccessRules::CanAccess(TEXT("dungeon.cross-network.fifth-connection"), Fifth, Failure));
    Fifth.TrustByNetwork.Add(ELiberationNetwork::DeedJudgers, ELiberationTrustTier::Kin);
    TestFalse(TEXT("Kin is insufficient where all five must be Blood"),
        FSecretDungeonAccessRules::CanAccess(TEXT("dungeon.cross-network.fifth-connection"), Fifth, Failure));

    // Fourth Fortress: any one Blood network plus physical Region-06 reachability.
    FSecretDungeonAccessEvidence Fortress;
    GiveBlood(Fortress, ELiberationNetwork::PatientCoast);
    Fortress.bRegion06Reached = true;
    TestTrue(TEXT("Any Blood network plus Region 06 can open Fourth Fortress knowledge"),
        FSecretDungeonAccessRules::CanAccess(TEXT("dungeon.region06.fourth-fortress"), Fortress, Failure));

    // Harbour That Was First: hidden returned-item threshold remains an externally supplied fact.
    FSecretDungeonAccessEvidence Harbour;
    GiveBlood(Harbour, ELiberationNetwork::RecoveryUnderground);
    Harbour.EvidenceIds = {
        TEXT("knowledge.sea.deep-harbour-charts"),
        TEXT("route.sea.deepest-feared-waters")
    };
    Harbour.bRecoveryReturnThresholdSatisfied = false;
    TestFalse(TEXT("Unknown/unsatisfied sacred-return threshold fails closed"),
        FSecretDungeonAccessRules::CanAccess(TEXT("dungeon.sea.harbour-that-was-first"), Harbour, Failure));
    Harbour.bRecoveryReturnThresholdSatisfied = true;
    TestTrue(TEXT("Authority-supplied threshold evidence opens Harbour That Was First"),
        FSecretDungeonAccessRules::CanAccess(TEXT("dungeon.sea.harbour-that-was-first"), Harbour, Failure));

    // Empire path never surfaces any secret site.
    Harbour.bEmpirePath = true;
    TestFalse(TEXT("Empire path remains blind to secret sites"),
        FSecretDungeonAccessRules::CanAccess(TEXT("dungeon.sea.harbour-that-was-first"), Harbour, Failure));

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
