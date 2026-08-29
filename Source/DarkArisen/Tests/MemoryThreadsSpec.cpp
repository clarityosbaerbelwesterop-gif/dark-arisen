// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Missions/MemoryThreadsSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenMemoryThreadsSpec,
    "DarkArisen.Content.Threads.Memory.NamedDeadAndEthan",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenMemoryThreadsSpec::RunTest(const FString& Parameters)
{
    UMemoryThreadsSubsystem* Memory = NewObject<UMemoryThreadsSubsystem>();
    TestNotNull(TEXT("Memory Thread fixture exists"), Memory);
    if (!Memory)
    {
        return false;
    }

    TestTrue(TEXT("Named dead memory registers with a concrete memorial"),
        Memory->RegisterNamedDeadMemory(
            TEXT("dead.test.sailor"),
            TEXT("memorial.drowned-cat.beam"),
            ENamedDeadMemorialTradition::SailorPirate));
    TestFalse(TEXT("A duplicate dead identity cannot be registered twice"),
        Memory->RegisterNamedDeadMemory(
            TEXT("dead.test.sailor"),
            TEXT("memorial.other"),
            ENamedDeadMemorialTradition::SailorPirate));
    TestTrue(TEXT("A memorial can be visited once"), Memory->RecordMemorialVisited(TEXT("dead.test.sailor")));
    TestFalse(TEXT("Repeated memorial visits create no completion/reward loop"),
        Memory->RecordMemorialVisited(TEXT("dead.test.sailor")));

    TestFalse(TEXT("Bjorn begins genuinely unresolved"), Memory->CanConfirmBjornStatus());
    TestFalse(TEXT("Unresolved cannot be submitted as an outcome"),
        Memory->ResolveBjornStatus(EBjornKnownStatus::Unresolved));
    TestTrue(TEXT("A source-authored Bjorn outcome may resolve the uncertainty once"),
        Memory->ResolveBjornStatus(EBjornKnownStatus::AliveCaptive));
    TestTrue(TEXT("Only after resolution may systems confirm Bjorn's status"), Memory->CanConfirmBjornStatus());
    TestFalse(TEXT("Bjorn status cannot be rerolled after resolution"),
        Memory->ResolveBjornStatus(EBjornKnownStatus::Dead));

    TestTrue(TEXT("Ethan physical trace accumulates"),
        Memory->RecordEthanPhysicalTrace(TEXT("ethan.trace.grove-note")));
    TestFalse(TEXT("The same Ethan trace cannot be farmed"),
        Memory->RecordEthanPhysicalTrace(TEXT("ethan.trace.grove-note")));
    TestTrue(TEXT("Dream-Ethan's one authored voice occurrence records once"), Memory->RecordDreamEthanVoiceHeard());
    TestFalse(TEXT("Dream-Ethan voice cannot recur"), Memory->RecordDreamEthanVoiceHeard());
    TestFalse(TEXT("Ethan Thread cannot structurally resolve before Chapter 9"),
        Memory->ResolveEthanThreadAtChapterNine(8));
    TestTrue(TEXT("Ethan Thread structurally resolves in Chapter 9 whether optional traces were pursued or not"),
        Memory->ResolveEthanThreadAtChapterNine(9));

    TestFalse(TEXT("The Named Dead has no ending"), Memory->NamedDeadThreadHasEnding());
    TestFalse(TEXT("Ethan never receives a closure mechanic"), Memory->EthanHasClosureMechanic());
    TestFalse(TEXT("There is no secret final Ethan letter"), Memory->EthanHasSecretFinalLetter());
    TestFalse(TEXT("Ethan never gets a clear full-face flashback"), Memory->AllowsClearEthanFlashbackFace());
    TestFalse(TEXT("Memorials never award gameplay rewards"), Memory->AwardsMemorialGameplayReward());

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
