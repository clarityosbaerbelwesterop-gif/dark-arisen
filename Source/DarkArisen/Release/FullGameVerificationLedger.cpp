// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Release/FullGameVerificationLedger.h"

namespace
{
bool IsHexCharacter(const TCHAR Character)
{
    return (Character >= TEXT('0') && Character <= TEXT('9'))
        || (Character >= TEXT('a') && Character <= TEXT('f'))
        || (Character >= TEXT('A') && Character <= TEXT('F'));
}
}

bool UFullGameVerificationLedger::InitializeCandidateSet(
    const FString& InSourceCommit,
    const FString& InWindowsSha256,
    const FString& InLinuxSha256,
    const FName InContentManifestRevision,
    FString& OutError)
{
    if (bInitialized)
    {
        OutError = TEXT("M10 candidate set is immutable once initialized.");
        return false;
    }
    if (!IsHexOfLength(InSourceCommit, 40))
    {
        OutError = TEXT("M10 requires a complete 40-character source commit SHA.");
        return false;
    }
    if (!IsHexOfLength(InWindowsSha256, 64) || !IsHexOfLength(InLinuxSha256, 64))
    {
        OutError = TEXT("M10 requires valid Windows and Linux SHA-256 candidate identities.");
        return false;
    }
    if (InWindowsSha256.Equals(InLinuxSha256, ESearchCase::IgnoreCase))
    {
        OutError = TEXT("Windows and Linux candidate identities must be distinct immutable artifacts.");
        return false;
    }
    if (InContentManifestRevision.IsNone())
    {
        OutError = TEXT("M10 requires a content-manifest revision.");
        return false;
    }

    SourceCommit = InSourceCommit.ToLower();
    WindowsCandidateSha256 = InWindowsSha256.ToLower();
    LinuxCandidateSha256 = InLinuxSha256.ToLower();
    ContentManifestRevision = InContentManifestRevision;
    Evidence.Reset();
    bInitialized = true;
    return true;
}

bool UFullGameVerificationLedger::AppendEvidence(const FFullGameEvidenceRecord& Record, FString& OutError)
{
    if (!bInitialized)
    {
        OutError = TEXT("M10 candidate set must be initialized before evidence is appended.");
        return false;
    }
    if (!Record.SourceCommit.Equals(SourceCommit, ESearchCase::IgnoreCase)
        || !Record.WindowsCandidateSha256.Equals(WindowsCandidateSha256, ESearchCase::IgnoreCase)
        || !Record.LinuxCandidateSha256.Equals(LinuxCandidateSha256, ESearchCase::IgnoreCase)
        || Record.ContentManifestRevision != ContentManifestRevision)
    {
        OutError = TEXT("M10 evidence does not belong to the immutable configured candidate set.");
        return false;
    }
    if (!Record.bExternalOrRunnerEvidence)
    {
        OutError = TEXT("M10 runtime/runner/operator evidence may not be self-certified by source code.");
        return false;
    }
    if (!Record.bPassed)
    {
        OutError = TEXT("M10 stores only passed evidence; failed or unknown gates remain absent/open.");
        return false;
    }
    if (Record.EvidenceReference.TrimStartAndEnd().IsEmpty())
    {
        OutError = TEXT("M10 passed evidence requires a sanitized evidence reference.");
        return false;
    }
    for (const FFullGameEvidenceRecord& Existing : Evidence)
    {
        if (Existing.Gate == Record.Gate)
        {
            OutError = TEXT("M10 evidence is append-only; a gate may be recorded once per immutable candidate set.");
            return false;
        }
    }

    Evidence.Add(Record);
    return true;
}

bool UFullGameVerificationLedger::HasPassed(const EFullGameVerificationGate Gate) const
{
    for (const FFullGameEvidenceRecord& Record : Evidence)
    {
        if (Record.Gate == Gate && Record.bPassed)
        {
            return true;
        }
    }
    return false;
}

bool UFullGameVerificationLedger::IsComplete(TArray<FString>& OutErrors) const
{
    if (!bInitialized)
    {
        OutErrors.Add(TEXT("M10 verification ledger has no immutable candidate set."));
        return false;
    }

    for (const EFullGameVerificationGate Gate : RequiredGates())
    {
        if (!HasPassed(Gate))
        {
            OutErrors.Add(FString::Printf(TEXT("M10 verification gate remains open: %d"), static_cast<int32>(Gate)));
        }
    }
    return OutErrors.IsEmpty();
}

bool UFullGameVerificationLedger::IsHexOfLength(const FString& Value, const int32 Length)
{
    if (Value.Len() != Length)
    {
        return false;
    }
    for (const TCHAR Character : Value)
    {
        if (!IsHexCharacter(Character))
        {
            return false;
        }
    }
    return true;
}

const TArray<EFullGameVerificationGate>& UFullGameVerificationLedger::RequiredGates()
{
    static const TArray<EFullGameVerificationGate> Gates = {
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
    return Gates;
}
