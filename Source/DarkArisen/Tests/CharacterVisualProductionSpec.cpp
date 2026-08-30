// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Production/CharacterVisualProductionCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenCharacterVisualProductionSpec,
    "DarkArisen.Production.CharacterVisualSourceBoundary",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenCharacterVisualProductionSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Major-character visual catalog validates structurally"),
        FCharacterVisualProductionCatalog::Validate(Errors));
    TestEqual(TEXT("Major-character visual catalog has no structural errors"), Errors.Num(), 0);

    const TArray<FCharacterVisualProductionBrief> Briefs =
        FCharacterVisualProductionCatalog::BuildMajorCharacterBriefs();
    TestEqual(TEXT("Nine major-character source briefs exist"),
        Briefs.Num(), FCharacterVisualProductionCatalog::MajorCharacterBriefCount);

    int32 Ready = 0;
    int32 Blocked = 0;
    bool bElowenBlocked = false;
    bool bEthanBlocked = false;
    bool bDravenBlocked = false;
    bool bJakeFound = false;
    int32 CrewCount = 0;

    for (const FCharacterVisualProductionBrief& Character : Briefs)
    {
        if (Character.bProviderReferenceReady) ++Ready;
        else ++Blocked;

        TestFalse(TEXT("No character reference is falsely approved"), Character.bApprovedReferenceExists);
        TestTrue(TEXT("No approved character reference path is fabricated"), Character.ApprovedReferencePath.IsEmpty());

        if (Character.StableId == TEXT("character.elowen-arion"))
        {
            bElowenBlocked = !Character.bProviderReferenceReady
                && Character.ExplicitUnknowns.Contains(TEXT("Hair"), ESearchCase::CaseSensitive)
                && Character.ExplicitUnknowns.Contains(TEXT("facial structure"), ESearchCase::CaseSensitive);
        }
        else if (Character.StableId == TEXT("character.ethan-harlow"))
        {
            bEthanBlocked = !Character.bProviderReferenceReady
                && Character.GoverningSource.Contains(TEXT("DesignAuthority"), ESearchCase::CaseSensitive);
        }
        else if (Character.StableId == TEXT("character.draven-voss"))
        {
            bDravenBlocked = !Character.bProviderReferenceReady
                && Character.GoverningSource.Contains(TEXT("DesignAuthority"), ESearchCase::CaseSensitive);
        }
        else if (Character.StableId == TEXT("character.jake-harlow"))
        {
            bJakeFound = Character.PhysicalFacts.Contains(TEXT("1.75 m"), ESearchCase::CaseSensitive)
                && Character.PhysicalFacts.Contains(TEXT("green eyes"), ESearchCase::IgnoreCase);
        }

        if (Character.Role == ECharacterVisualProductionRole::CoreCrew)
        {
            ++CrewCount;
        }
    }

    TestEqual(TEXT("Six characters are current-authority ready for bounded reference generation"),
        Ready, FCharacterVisualProductionCatalog::ProviderReadyCharacterCount);
    TestEqual(TEXT("Exactly three major characters remain explicitly blocked"),
        Blocked, FCharacterVisualProductionCatalog::ExplicitlyBlockedCharacterCount);
    TestTrue(TEXT("Elowen remains blocked rather than receiving an invented canonical appearance"), bElowenBlocked);
    TestTrue(TEXT("Ethan remains blocked behind the Phase 11 rewrite decision"), bEthanBlocked);
    TestTrue(TEXT("Draven remains blocked behind the Phase 11 rewrite decision"), bDravenBlocked);
    TestTrue(TEXT("Jake physical brief preserves authored height/eyes"), bJakeFound);
    TestEqual(TEXT("Exactly five core crew visual briefs are present"), CrewCount, 5);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS