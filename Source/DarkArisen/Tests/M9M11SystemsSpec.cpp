// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Release/AlphaReleaseContracts.h"
#include "Release/FullGameVerificationLedger.h"
#include "Release/PrivateAlphaSignOff.h"
#include "Runtime/RuntimeIntegrationManifest.h"

namespace
{
URuntimeIntegrationManifest* BuildValidM9Manifest()
{
    URuntimeIntegrationManifest* Manifest = NewObject<URuntimeIntegrationManifest>();
    Manifest->ManifestRevision = TEXT("m9.synthetic.runtime");

    const TArray<ERuntimeIntegrationKind> RuntimeKinds = {
        ERuntimeIntegrationKind::Level,
        ERuntimeIntegrationKind::Navigation,
        ERuntimeIntegrationKind::InteractionAnchor,
        ERuntimeIntegrationKind::Animation,
        ERuntimeIntegrationKind::Audio,
        ERuntimeIntegrationKind::Population,
        ERuntimeIntegrationKind::Fauna,
        ERuntimeIntegrationKind::Dungeon,
        ERuntimeIntegrationKind::Boss,
        ERuntimeIntegrationKind::Ship,
        ERuntimeIntegrationKind::ColonialWar,
        ERuntimeIntegrationKind::Highmoore
    };
    for (int32 Index = 0; Index < RuntimeKinds.Num(); ++Index)
    {
        FRuntimeIntegrationEntry Entry;
        Entry.StableId = FName(*FString::Printf(TEXT("runtime.%02d"), Index + 1));
        Entry.GoverningSource = TEXT("automation synthetic authored-runtime fixture");
        Entry.Kind = RuntimeKinds[Index];
        Entry.AssetPath = FSoftObjectPath(FString::Printf(
            TEXT("/Game/Test/Runtime_%02d.Runtime_%02d"), Index + 1, Index + 1));
        Manifest->Entries.Add(Entry);
    }

    for (int32 Index = 0; Index < 19; ++Index)
    {
        FRuntimeIntegrationEntry Entry;
        Entry.StableId = FName(*FString::Printf(TEXT("cutscene.%02d"), Index + 1));
        Entry.GoverningSource = TEXT("cutscene catalog.md");
        Entry.Kind = ERuntimeIntegrationKind::Cutscene;
        Entry.AssetPath = FSoftObjectPath(FString::Printf(TEXT("/Game/Test/Cutscene_%02d.Cutscene_%02d"), Index + 1, Index + 1));
        Entry.bSequencerOwned = true;
        Manifest->Entries.Add(Entry);
    }

    for (int32 Index = 0; Index < 22; ++Index)
    {
        FRuntimeIntegrationEntry Entry;
        Entry.StableId = FName(*FString::Printf(TEXT("playable.%02d"), Index + 1));
        Entry.GoverningSource = TEXT("cutscene catalog.md Section 6");
        Entry.Kind = ERuntimeIntegrationKind::ProtectedPlayableMoment;
        Entry.AssetPath = FSoftObjectPath(FString::Printf(TEXT("/Game/Test/Playable_%02d.Playable_%02d"), Index + 1, Index + 1));
        Entry.bSequencerOwned = false;
        Manifest->Entries.Add(Entry);
    }
    return Manifest;
}

FAlphaReleaseManifest BuildReleaseManifestForM11()
{
    FAlphaReleaseManifest Manifest;
    Manifest.SourceCommit = FString::ChrN(40, TEXT('a'));
    Manifest.ContentManifestRevision = TEXT("m11.synthetic.content");
    Manifest.SourceArchiveSha256 = FString::ChrN(64, TEXT('1'));

    FAlphaCandidateDescriptor Windows;
    Windows.Platform = EAlphaTargetPlatform::Win64;
    Windows.SourceCommit = Manifest.SourceCommit;
    Windows.ContentManifestRevision = Manifest.ContentManifestRevision;
    Windows.Sha256 = FString::ChrN(64, TEXT('2'));
    Windows.bShipping = true;
    Manifest.Candidates.Add(Windows);

    FAlphaCandidateDescriptor Linux;
    Linux.Platform = EAlphaTargetPlatform::LinuxX64;
    Linux.SourceCommit = Manifest.SourceCommit;
    Linux.ContentManifestRevision = Manifest.ContentManifestRevision;
    Linux.Sha256 = FString::ChrN(64, TEXT('3'));
    Linux.bShipping = true;
    Manifest.Candidates.Add(Linux);
    return Manifest;
}

FAlphaAcceptanceLedger BuildAcceptedM8Ledger(const FAlphaReleaseManifest& Manifest)
{
    FAlphaAcceptanceLedger Ledger;
    Ledger.CandidateCommit = Manifest.SourceCommit;
    Ledger.WindowsCandidateSha256 = Manifest.Find(EAlphaTargetPlatform::Win64)->Sha256;
    Ledger.LinuxCandidateSha256 = Manifest.Find(EAlphaTargetPlatform::LinuxX64)->Sha256;
    Ledger.bFullContentM0M8 = true;
    Ledger.bWindowsUe55BuildAndAutomation = true;
    Ledger.bLinuxUe55BuildAndAutomation = true;
    Ledger.bWindowsStartToCreditsPlaythrough = true;
    Ledger.bLinuxStartToCreditsPlaythrough = true;
    Ledger.bContentManifestReconciled = true;
    Ledger.bPerformanceFloor60Fps = true;
    Ledger.bPrivacyAccessDenied = true;
    Ledger.bCreditsMusicAndLicencesApproved = true;
    Ledger.bFloAlphaApproval = true;
    return Ledger;
}

UFullGameVerificationLedger* BuildCompleteM10Ledger(const FAlphaReleaseManifest& Manifest)
{
    UFullGameVerificationLedger* Ledger = NewObject<UFullGameVerificationLedger>();
    FString Error;
    Ledger->InitializeCandidateSet(
        Manifest.SourceCommit,
        Manifest.Find(EAlphaTargetPlatform::Win64)->Sha256,
        Manifest.Find(EAlphaTargetPlatform::LinuxX64)->Sha256,
        Manifest.ContentManifestRevision,
        Error);

    const TArray<EFullGameVerificationGate> Gates = {
        EFullGameVerificationGate::WindowsUe55Compile,
        EFullGameVerificationGate::LinuxUe55Compile,
        EFullGameVerificationGate::UnrealAutomation,
        EFullGameVerificationGate::ContentManifestReconciled,
        EFullGameVerificationGate::SaveMigrationVerified,
        EFullGameVerificationGate::CrossPlatformSaveCompatibility,
        EFullGameVerificationGate::ControllerInputParity,
        EFullGameVerificationGate::AccessibilityParity,
        EFullGameVerificationGate::WindowsStartToCredits,
        EFullGameVerificationGate::LinuxStartToCredits,
        EFullGameVerificationGate::PerformanceFloor60Fps,
        EFullGameVerificationGate::DesignLawsRegressionFree,
        EFullGameVerificationGate::PrivacyAccessDenied
    };

    for (const EFullGameVerificationGate Gate : Gates)
    {
        FFullGameEvidenceRecord Record;
        Record.Gate = Gate;
        Record.SourceCommit = Manifest.SourceCommit;
        Record.WindowsCandidateSha256 = Manifest.Find(EAlphaTargetPlatform::Win64)->Sha256;
        Record.LinuxCandidateSha256 = Manifest.Find(EAlphaTargetPlatform::LinuxX64)->Sha256;
        Record.ContentManifestRevision = Manifest.ContentManifestRevision;
        Record.EvidenceReference = FString::Printf(TEXT("synthetic-evidence-%d"), static_cast<int32>(Gate));
        Record.bPassed = true;
        Record.bExternalOrRunnerEvidence = true;
        Ledger->AppendEvidence(Record, Error);
    }
    return Ledger;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM9RuntimeIntegrationSpec,
    "DarkArisen.M9.RuntimeIntegrationManifest",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM9RuntimeIntegrationSpec::RunTest(const FString& Parameters)
{
    URuntimeIntegrationManifest* Manifest = BuildValidM9Manifest();
    TestNotNull(TEXT("M9 manifest constructs"), Manifest);
    if (!Manifest) return false;

    TArray<FString> Errors;
    TestTrue(TEXT("Every runtime category plus exact 19/22 presentation bindings validate"), Manifest->ValidateDefinition(Errors));

    const int32 FirstProtectedIndex = 12 + 19;
    Manifest->Entries[FirstProtectedIndex].bSequencerOwned = true;
    Errors.Reset();
    TestFalse(TEXT("Protected playable moment cannot become Sequencer-owned"), Manifest->ValidateDefinition(Errors));
    Manifest->Entries[FirstProtectedIndex].bSequencerOwned = false;

    Manifest->Entries[0].bBlueprintOwnsGameplayLogic = true;
    Errors.Reset();
    TestFalse(TEXT("Blueprint gameplay authority fails M9 closed"), Manifest->ValidateDefinition(Errors));
    Manifest->Entries[0].bBlueprintOwnsGameplayLogic = false;

    Manifest->Entries.RemoveAt(0);
    Errors.Reset();
    TestFalse(TEXT("A missing authored runtime category fails M9 closed"), Manifest->ValidateDefinition(Errors));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM10VerificationLedgerSpec,
    "DarkArisen.M10.ImmutableVerificationLedger",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM10VerificationLedgerSpec::RunTest(const FString& Parameters)
{
    const FAlphaReleaseManifest Manifest = BuildReleaseManifestForM11();
    UFullGameVerificationLedger* Ledger = NewObject<UFullGameVerificationLedger>();
    FString Error;
    TestTrue(TEXT("Immutable candidate set initializes"), Ledger->InitializeCandidateSet(
        Manifest.SourceCommit,
        Manifest.Find(EAlphaTargetPlatform::Win64)->Sha256,
        Manifest.Find(EAlphaTargetPlatform::LinuxX64)->Sha256,
        Manifest.ContentManifestRevision,
        Error));

    FFullGameEvidenceRecord Record;
    Record.Gate = EFullGameVerificationGate::WindowsUe55Compile;
    Record.SourceCommit = Manifest.SourceCommit;
    Record.WindowsCandidateSha256 = Manifest.Find(EAlphaTargetPlatform::Win64)->Sha256;
    Record.LinuxCandidateSha256 = Manifest.Find(EAlphaTargetPlatform::LinuxX64)->Sha256;
    Record.ContentManifestRevision = Manifest.ContentManifestRevision;
    Record.EvidenceReference = TEXT("synthetic-runner-evidence");
    Record.bPassed = true;
    Record.bExternalOrRunnerEvidence = true;
    TestTrue(TEXT("Matching evidence appends"), Ledger->AppendEvidence(Record, Error));
    TestFalse(TEXT("Duplicate gate cannot overwrite append-only evidence"), Ledger->AppendEvidence(Record, Error));

    FFullGameEvidenceRecord WrongHash = Record;
    WrongHash.Gate = EFullGameVerificationGate::LinuxUe55Compile;
    WrongHash.WindowsCandidateSha256 = FString::ChrN(64, TEXT('f'));
    TestFalse(TEXT("Evidence cannot migrate between candidate hashes"), Ledger->AppendEvidence(WrongHash, Error));

    FFullGameEvidenceRecord SelfCertified = Record;
    SelfCertified.Gate = EFullGameVerificationGate::LinuxUe55Compile;
    SelfCertified.bExternalOrRunnerEvidence = false;
    TestFalse(TEXT("Source code cannot self-certify runtime evidence"), Ledger->AppendEvidence(SelfCertified, Error));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDarkArisenM11PrivateAlphaSignOffSpec,
    "DarkArisen.M11.PrivateAlphaSignOff",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDarkArisenM11PrivateAlphaSignOffSpec::RunTest(const FString& Parameters)
{
    const FAlphaReleaseManifest Manifest = BuildReleaseManifestForM11();
    const FAlphaAcceptanceLedger M8 = BuildAcceptedM8Ledger(Manifest);
    UFullGameVerificationLedger* M10 = BuildCompleteM10Ledger(Manifest);
    UPrivateAlphaSignOff* SignOff = NewObject<UPrivateAlphaSignOff>();

    FPrivateAlphaCreditsApproval Credits;
    Credits.CreditsRevision = TEXT("credits.synthetic.approved");
    Credits.MusicSourceReference = TEXT("music-source-approved");
    Credits.MusicLicenceReference = TEXT("music-licence-approved");
    Credits.bCompleteAttributionApproved = true;
    Credits.bMusicCommercialRightsApproved = true;
    Credits.bMusicCostApproved = true;
    Credits.bMusicIsNonReactive = true;

    TArray<FString> Errors;
    TestFalse(TEXT("M11 cannot self-approve without explicit operator approval"),
        SignOff->CanSignOff(Manifest, M8, M10, Credits, false, Errors));

    FPrivateAlphaCreditsApproval ReactiveCredits = Credits;
    ReactiveCredits.bMusicIsNonReactive = false;
    Errors.Reset();
    TestFalse(TEXT("Reactive credits music can never satisfy M11"),
        SignOff->CanSignOff(Manifest, M8, M10, ReactiveCredits, true, Errors));

    Errors.Reset();
    TestTrue(TEXT("M11 accepts exact immutable evidence plus explicit operator approval"),
        SignOff->CanSignOff(Manifest, M8, M10, Credits, true, Errors));
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
