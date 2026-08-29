// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Audio/AudioProductionCatalog.h"
#include "Audio/VoiceProductionCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenAudioProductionCatalogSpec,
    "DarkArisen.Content.Audio.ProductionCatalog",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenAudioProductionCatalogSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Audio production catalog validates"), FAudioProductionCatalog::Validate(Errors));
    TestEqual(TEXT("Audio catalog has no structural errors"), Errors.Num(), 0);

    const TArray<FAudioProductionRequirement> Requirements = FAudioProductionCatalog::BuildCoreRequirements();
    TestEqual(TEXT("Twelve core audio requirements are present"), Requirements.Num(), 12);

    for (const FAudioProductionRequirement& Requirement : Requirements)
    {
        TestFalse(TEXT("No production music reacts to gameplay events"), Requirement.bReactiveToGameplayEvent);
        TestFalse(TEXT("No absent audio is claimed as authored"), Requirement.bAssetAuthored);
        TestTrue(TEXT("No absent audio carries a fake asset path"), Requirement.AssetPath.IsEmpty());
    }

    TestEqual(TEXT("Three audio production gaps remain explicit"), FAudioProductionCatalog::BuildDesignGaps().Num(), 3);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenCoreVoiceProductionCatalogSpec,
    "DarkArisen.Content.Audio.CoreVoiceProductionCatalog",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenCoreVoiceProductionCatalogSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Core crew voice production catalog validates"), FVoiceProductionCatalog::Validate(Errors));
    TestEqual(TEXT("Voice catalog has no structural errors"), Errors.Num(), 0);

    const TArray<FVoiceRoleProductionDefinition> Roles = FVoiceProductionCatalog::BuildCoreCrewRoles();
    TestEqual(TEXT("Exactly five core crew voice roles are locked"), Roles.Num(), 5);
    for (const FVoiceRoleProductionDefinition& Role : Roles)
    {
        TestTrue(TEXT("Audition dialogue is locked"), Role.bDialogueLocked);
        TestTrue(TEXT("Audition subtitle is locked"), Role.bSubtitleReady);
        TestFalse(TEXT("Source catalog does not convert audition evidence into shipping acceptance"), Role.bCommercialRightsApproved);
        TestTrue(TEXT("No shipping audio path exists yet"), Role.ShippingAssetPath.IsEmpty());
    }
    TestEqual(TEXT("Three voice production gaps remain explicit"), FVoiceProductionCatalog::BuildDesignGaps().Num(), 3);
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
