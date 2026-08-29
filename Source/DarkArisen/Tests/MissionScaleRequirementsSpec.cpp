// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Missions/MissionScaleRequirements.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenMissionScaleRequirementsSpec,
    "DarkArisen.Content.Missions.ScaleRequirements",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenMissionScaleRequirementsSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Finite mission production quotas validate"), FMissionScaleRequirements::Validate(Errors));
    TestEqual(TEXT("Mission production quota validation has no structural errors"), Errors.Num(), 0);

    const TArray<FTurnProductionRequirement> Turns = FMissionScaleRequirements::BuildTurnRequirements();
    int32 TurnTotal = 0;
    int32 AuthoredTurnTotal = 0;
    for (const FTurnProductionRequirement& Requirement : Turns)
    {
        TurnTotal += Requirement.RequiredCount;
        AuthoredTurnTotal += Requirement.AuthoredIdentityCount;
    }
    TestEqual(TEXT("Turn slots remain exactly 132"), TurnTotal, 132);
    TestEqual(TEXT("Only three Turn identities are currently grounded"), AuthoredTurnTotal, 3);

    const TArray<FStandingTypeProductionRequirement> Standing = FMissionScaleRequirements::BuildStandingTypeRequirements();
    int32 StandingTotal = 0;
    int32 AuthoredStandingTotal = 0;
    for (const FStandingTypeProductionRequirement& Requirement : Standing)
    {
        StandingTotal += Requirement.RequiredCount;
        AuthoredStandingTotal += Requirement.AuthoredIdentityCount;
    }
    TestEqual(TEXT("Standing slots remain exactly 147"), StandingTotal, 147);
    TestEqual(TEXT("Only San Telmo Bell is a complete native Standing identity today"), AuthoredStandingTotal, 1);

    TestEqual(TEXT("Unknown mission slot is not silently treated as authored"),
        FMissionScaleRequirements::ResolveKnownIdentity(TEXT("Standing.Generated.Filler")),
        EAuthoredMissionSlotState::ReservedUnauthored);
    TestEqual(TEXT("Known San Telmo Bell variant is authored"),
        FMissionScaleRequirements::ResolveKnownIdentity(TEXT("Rexa.Standing.Salvage.SanTelmoBell")),
        EAuthoredMissionSlotState::AuthoredIdentity);

    TestEqual(TEXT("Four mission design gaps remain explicit"), FMissionScaleRequirements::BuildDesignGaps().Num(), 4);
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
