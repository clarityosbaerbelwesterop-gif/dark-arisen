// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Presentation/PresentationProductionCatalog.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenPresentationProductionCatalogSpec,
    "DarkArisen.Content.Presentation.ProductionCatalog",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenPresentationProductionCatalogSpec::RunTest(const FString& Parameters)
{
    TArray<FString> Errors;
    TestTrue(TEXT("Presentation production catalog validates"), FPresentationProductionCatalog::Validate(Errors));
    TestEqual(TEXT("Presentation catalog has no structural errors"), Errors.Num(), 0);

    const TArray<FAuthoredCutsceneDefinition> Cutscenes = FPresentationProductionCatalog::BuildCutscenes();
    TestEqual(TEXT("Exactly nineteen cutscene slots exist"), Cutscenes.Num(), 19);

    int32 Resolved = 0;
    int32 Unresolved = 0;
    for (const FAuthoredCutsceneDefinition& Entry : Cutscenes)
    {
        Resolved += Entry.bIdentityResolved ? 1 : 0;
        Unresolved += Entry.bIdentityResolved ? 0 : 1;
    }
    TestEqual(TEXT("Fourteen cutscene identities are grounded"), Resolved, 14);
    TestEqual(TEXT("Five endgame cutscene identities remain unresolved"), Unresolved, 5);

    const FAuthoredCutsceneDefinition* Ejection = Cutscenes.FindByPredicate([](const FAuthoredCutsceneDefinition& Entry)
    {
        return Entry.StableId == FName(TEXT("cutscene.highmoore.ejection"));
    });
    TestNotNull(TEXT("Ejection cutscene exists"), Ejection);
    if (Ejection)
    {
        TestEqual(TEXT("Narrow #9 entry remains P1 despite summary-count conflict"), Ejection->AnimationPriority, EPresentationAnimationPriority::P1);
    }

    const FAuthoredCutsceneDefinition* Arrow = Cutscenes.FindByPredicate([](const FAuthoredCutsceneDefinition& Entry)
    {
        return Entry.StableId == FName(TEXT("cutscene.highmoore.arrow"));
    });
    TestNotNull(TEXT("Arrow cutscene exists"), Arrow);
    if (Arrow)
    {
        TestTrue(TEXT("Arrow camera cannot move"), Arrow->bCameraMustRemainStatic);
        TestTrue(TEXT("Arrow music cannot react"), Arrow->bMusicMustContinueUnchanged);
        TestEqual(TEXT("Arrow takes control for exactly 1.5 seconds"), Arrow->TargetSeconds, 1.5f);
    }

    TestEqual(TEXT("Exactly twenty-two moments remain player-controlled"),
        FPresentationProductionCatalog::BuildProtectedPlayableMoments().Num(), 22);
    TestEqual(TEXT("Exactly five inserts remain"), FPresentationProductionCatalog::BuildInsertIds().Num(), 5);
    TestEqual(TEXT("Exactly six slow pushes remain"), FPresentationProductionCatalog::BuildSlowPushIds().Num(), 6);
    TestEqual(TEXT("Three presentation production gaps remain explicit"), FPresentationProductionCatalog::BuildDesignGaps().Num(), 3);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
