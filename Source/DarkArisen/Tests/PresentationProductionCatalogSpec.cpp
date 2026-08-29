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

    const auto FindCutscene = [&Cutscenes](const TCHAR* StableId) -> const FAuthoredCutsceneDefinition*
    {
        return Cutscenes.FindByPredicate([StableId](const FAuthoredCutsceneDefinition& Entry)
        {
            return Entry.StableId == FName(StableId);
        });
    };

    const FAuthoredCutsceneDefinition* Ejection = FindCutscene(TEXT("cutscene.highmoore.ejection"));
    TestNotNull(TEXT("Ejection cutscene exists"), Ejection);
    if (Ejection)
    {
        TestEqual(TEXT("Narrow #9 entry remains P1 despite summary-count conflict"), Ejection->AnimationPriority, EPresentationAnimationPriority::P1);
        TestEqual(TEXT("Ejection remains full authored ownership"), Ejection->ControlOwnership, EPresentationControlOwnership::SequencerOwned);
    }

    const FAuthoredCutsceneDefinition* Grove = FindCutscene(TEXT("cutscene.archipelago.the-grove"));
    TestNotNull(TEXT("Grove cutscene exists"), Grove);
    if (Grove)
    {
        TestEqual(TEXT("Grove uses anchored camera while movement stays with player"),
            Grove->ControlOwnership, EPresentationControlOwnership::AnchoredPlayerMovement);
    }

    const FAuthoredCutsceneDefinition* Emergence = FindCutscene(TEXT("cutscene.highmoore.emergence"));
    TestNotNull(TEXT("Emergence cutscene exists"), Emergence);
    if (Emergence)
    {
        TestEqual(TEXT("Emergence uses anchored player-movement ownership"),
            Emergence->ControlOwnership, EPresentationControlOwnership::AnchoredPlayerMovement);
        TestEqual(TEXT("Emergence anchor remains six seconds"), Emergence->TargetSeconds, 6.0f);
    }

    const FAuthoredCutsceneDefinition* VoiceFromBehind = FindCutscene(TEXT("cutscene.highmoore.voice-from-behind"));
    TestNotNull(TEXT("Voice From Behind cutscene exists"), VoiceFromBehind);
    if (VoiceFromBehind)
    {
        TestEqual(TEXT("Voice From Behind is only a brief authored takeover"),
            VoiceFromBehind->ControlOwnership, EPresentationControlOwnership::BriefAuthoredTakeover);
    }

    const FAuthoredCutsceneDefinition* Arrow = FindCutscene(TEXT("cutscene.highmoore.arrow"));
    TestNotNull(TEXT("Arrow cutscene exists"), Arrow);
    if (Arrow)
    {
        TestTrue(TEXT("Arrow camera cannot move"), Arrow->bCameraMustRemainStatic);
        TestTrue(TEXT("Arrow music cannot react"), Arrow->bMusicMustContinueUnchanged);
        TestEqual(TEXT("Arrow takes control for exactly 1.5 seconds"), Arrow->TargetSeconds, 1.5f);
        TestEqual(TEXT("Arrow is a brief takeover, not a full cinematic lock"),
            Arrow->ControlOwnership, EPresentationControlOwnership::BriefAuthoredTakeover);
    }

    const FAuthoredCutsceneDefinition* RealLetter = FindCutscene(TEXT("cutscene.highmoore.real-letter"));
    TestNotNull(TEXT("Real Letter cutscene exists"), RealLetter);
    if (RealLetter)
    {
        TestEqual(TEXT("Real Letter owns only the eight-second slow push"),
            RealLetter->ControlOwnership, EPresentationControlOwnership::SlowPushOnly);
    }

    const TArray<FProtectedPlayableMomentDefinition> Protected =
        FPresentationProductionCatalog::BuildProtectedPlayableMoments();
    TestEqual(TEXT("Exactly twenty-two moments remain player-controlled"), Protected.Num(), 22);

    TSet<FName> ProtectedIds;
    for (const FProtectedPlayableMomentDefinition& Entry : Protected)
    {
        ProtectedIds.Add(Entry.StableId);
    }
    TestTrue(TEXT("Thirty seconds after arrow remain protected gameplay"),
        ProtectedIds.Contains(TEXT("playable.arrow.after-thirty-seconds")));
    TestTrue(TEXT("Every boss death remains protected gameplay"),
        ProtectedIds.Contains(TEXT("playable.boss-death.hold")));
    TestTrue(TEXT("Liberation aftermath remains free-camera gameplay"),
        ProtectedIds.Contains(TEXT("playable.liberation.aftermath")));

    TestEqual(TEXT("Exactly five inserts remain"), FPresentationProductionCatalog::BuildInsertIds().Num(), 5);
    TestEqual(TEXT("Exactly six slow pushes remain"), FPresentationProductionCatalog::BuildSlowPushIds().Num(), 6);
    TestEqual(TEXT("Three presentation production gaps remain explicit"), FPresentationProductionCatalog::BuildDesignGaps().Num(), 3);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
