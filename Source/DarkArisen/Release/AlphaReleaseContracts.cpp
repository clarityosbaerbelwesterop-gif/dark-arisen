// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Release/AlphaReleaseContracts.h"

namespace
{
bool IsLowerHex(const FString& Value, const int32 RequiredLength)
{
    if (Value.Len() != RequiredLength)
    {
        return false;
    }
    for (const TCHAR Character : Value)
    {
        if (!FChar::IsHexDigit(Character))
        {
            return false;
        }
    }
    return true;
}

void RequireGate(const bool bPassed, const TCHAR* Name, TArray<FString>& OutErrors)
{
    if (!bPassed)
    {
        OutErrors.Add(FString::Printf(TEXT("Alpha acceptance gate is not green: %s"), Name));
    }
}
}

bool FAccessibilityDifficultyContract::Validate(TArray<FString>& OutErrors) const
{
    OutErrors.Reset();

    if (!bInputRemappingEnabled)
    {
        OutErrors.Add(TEXT("M8 accessibility requires input remapping"));
    }
    if (!FMath::IsFinite(SubtitleScale) || SubtitleScale < 0.75f || SubtitleScale > 2.0f)
    {
        OutErrors.Add(TEXT("subtitle scale must remain within the approved 0.75-2.00 presentation range"));
    }
    if (!FMath::IsFinite(IncomingDamageMultiplier) || IncomingDamageMultiplier < 0.25f || IncomingDamageMultiplier > 4.0f)
    {
        OutErrors.Add(TEXT("difficulty damage multiplier is outside the permitted range"));
    }
    if (!FMath::IsFinite(PlayerHealthMultiplier) || PlayerHealthMultiplier < 0.25f || PlayerHealthMultiplier > 4.0f)
    {
        OutErrors.Add(TEXT("difficulty health multiplier is outside the permitted range"));
    }
    if (DeflectionWindowFrames != DarkArisen::DesignLaws::DeflectionWindowFrames
        || DeflectionWindowFrames != 6)
    {
        OutErrors.Add(TEXT("difficulty/accessibility may never change the six-frame deflection window"));
    }

    return OutErrors.IsEmpty();
}

bool FSaveSchemaMigrationRegistry::ValidateRegistry(
    const TArray<FSaveMigrationStep>& Steps,
    TArray<FString>& OutErrors)
{
    OutErrors.Reset();
    TSet<int32> SeenFromVersions;
    TSet<FName> SeenIds;

    for (const FSaveMigrationStep& Step : Steps)
    {
        if (Step.FromVersion < 1)
        {
            OutErrors.Add(TEXT("save migration may not target a pre-v1 fabricated schema"));
        }
        if (Step.ToVersion != Step.FromVersion + 1)
        {
            OutErrors.Add(FString::Printf(TEXT("save migration %d -> %d is not a contiguous one-version step"), Step.FromVersion, Step.ToVersion));
        }
        if (Step.ToVersion > CurrentSchemaVersion)
        {
            OutErrors.Add(FString::Printf(TEXT("save migration targets future schema version %d"), Step.ToVersion));
        }
        if (Step.StableMigrationId.IsNone())
        {
            OutErrors.Add(FString::Printf(TEXT("save migration %d -> %d has no stable id"), Step.FromVersion, Step.ToVersion));
        }
        else if (SeenIds.Contains(Step.StableMigrationId))
        {
            OutErrors.Add(FString::Printf(TEXT("duplicate save migration id: %s"), *Step.StableMigrationId.ToString()));
        }
        else
        {
            SeenIds.Add(Step.StableMigrationId);
        }
        if (SeenFromVersions.Contains(Step.FromVersion))
        {
            OutErrors.Add(FString::Printf(TEXT("multiple save migrations start at version %d"), Step.FromVersion));
        }
        else
        {
            SeenFromVersions.Add(Step.FromVersion);
        }
    }

    return OutErrors.IsEmpty();
}

bool FSaveSchemaMigrationRegistry::TryBuildMigrationPath(
    const int32 SavedVersion,
    const TArray<FSaveMigrationStep>& Steps,
    TArray<FSaveMigrationStep>& OutPath,
    FString& OutError)
{
    OutPath.Reset();
    OutError.Reset();

    TArray<FString> RegistryErrors;
    if (!ValidateRegistry(Steps, RegistryErrors))
    {
        OutError = FString::Join(RegistryErrors, TEXT("; "));
        return false;
    }
    if (SavedVersion < 1)
    {
        OutError = TEXT("unknown/pre-v1 save schema fails closed");
        return false;
    }
    if (SavedVersion > CurrentSchemaVersion)
    {
        OutError = FString::Printf(TEXT("future save schema %d fails closed; current is %d"), SavedVersion, CurrentSchemaVersion);
        return false;
    }
    if (SavedVersion == CurrentSchemaVersion)
    {
        return true;
    }

    int32 Version = SavedVersion;
    while (Version < CurrentSchemaVersion)
    {
        const FSaveMigrationStep* Step = Steps.FindByPredicate([Version](const FSaveMigrationStep& Candidate)
        {
            return Candidate.FromVersion == Version && Candidate.ToVersion == Version + 1;
        });
        if (!Step)
        {
            OutPath.Reset();
            OutError = FString::Printf(TEXT("no explicit migration registered from save schema %d"), Version);
            return false;
        }
        OutPath.Add(*Step);
        Version = Step->ToVersion;
    }
    return Version == CurrentSchemaVersion;
}

bool FAlphaCandidateDescriptor::IsPrivateAlphaSafe(TArray<FString>& OutErrors) const
{
    if (!IsLowerHex(SourceCommit, 40))
    {
        OutErrors.Add(TEXT("candidate source commit must be a complete 40-character SHA"));
    }
    if (ContentManifestRevision.IsNone())
    {
        OutErrors.Add(TEXT("candidate content manifest revision is required"));
    }
    if (!IsLowerHex(Sha256, 64))
    {
        OutErrors.Add(TEXT("candidate SHA-256 must contain 64 hexadecimal characters"));
    }
    if (!bShipping)
    {
        OutErrors.Add(TEXT("Alpha candidate must use Shipping configuration"));
    }
    if (bContainsDeploymentCredentials)
    {
        OutErrors.Add(TEXT("candidate contains deployment credentials"));
    }
    if (bContainsTurnSecret)
    {
        OutErrors.Add(TEXT("candidate contains a TURN secret"));
    }
    if (bContainsPrivateHostname)
    {
        OutErrors.Add(TEXT("candidate contains a private host name"));
    }
    if (bContainsPublicLinkConfiguration)
    {
        OutErrors.Add(TEXT("candidate contains public-link configuration"));
    }
    if (bContainsSonyOrNativePs5Material)
    {
        OutErrors.Add(TEXT("native PS5/Sony material is forbidden in the Alpha candidate"));
    }
    return OutErrors.IsEmpty();
}

const FAlphaCandidateDescriptor* FAlphaReleaseManifest::Find(const EAlphaTargetPlatform Platform) const
{
    return Candidates.FindByPredicate([Platform](const FAlphaCandidateDescriptor& Candidate)
    {
        return Candidate.Platform == Platform;
    });
}

bool FAlphaReleaseManifest::Validate(TArray<FString>& OutErrors) const
{
    OutErrors.Reset();

    if (!IsLowerHex(SourceCommit, 40))
    {
        OutErrors.Add(TEXT("release source commit must be a complete 40-character SHA"));
    }
    if (ContentManifestRevision.IsNone())
    {
        OutErrors.Add(TEXT("release content manifest revision is required"));
    }
    if (!IsLowerHex(SourceArchiveSha256, 64))
    {
        OutErrors.Add(TEXT("source archive SHA-256 must contain 64 hexadecimal characters"));
    }
    if (Candidates.Num() != 2)
    {
        OutErrors.Add(FString::Printf(TEXT("Alpha release requires exactly two candidates; found %d"), Candidates.Num()));
    }

    bool bHasWindows = false;
    bool bHasLinux = false;
    TSet<EAlphaTargetPlatform> SeenPlatforms;
    for (const FAlphaCandidateDescriptor& Candidate : Candidates)
    {
        if (SeenPlatforms.Contains(Candidate.Platform))
        {
            OutErrors.Add(TEXT("Alpha release contains duplicate platform candidate"));
        }
        SeenPlatforms.Add(Candidate.Platform);
        bHasWindows |= Candidate.Platform == EAlphaTargetPlatform::Win64;
        bHasLinux |= Candidate.Platform == EAlphaTargetPlatform::LinuxX64;

        if (!Candidate.SourceCommit.Equals(SourceCommit, ESearchCase::IgnoreCase))
        {
            OutErrors.Add(TEXT("Windows/Linux candidate source commit does not match release commit"));
        }
        if (Candidate.ContentManifestRevision != ContentManifestRevision)
        {
            OutErrors.Add(TEXT("Windows/Linux candidate content manifest revision does not match release manifest"));
        }
        Candidate.IsPrivateAlphaSafe(OutErrors);
    }

    if (!bHasWindows || !bHasLinux)
    {
        OutErrors.Add(TEXT("Alpha release platform set must be exactly Win64 + Linux x86_64"));
    }

    return OutErrors.IsEmpty();
}

bool FAlphaAcceptanceLedger::IsAcceptedForPromotion(
    const FAlphaReleaseManifest& Manifest,
    TArray<FString>& OutErrors) const
{
    OutErrors.Reset();

    TArray<FString> ManifestErrors;
    if (!Manifest.Validate(ManifestErrors))
    {
        OutErrors.Append(ManifestErrors);
    }

    if (!CandidateCommit.Equals(Manifest.SourceCommit, ESearchCase::IgnoreCase))
    {
        OutErrors.Add(TEXT("acceptance ledger commit does not match the release manifest"));
    }
    const FAlphaCandidateDescriptor* Windows = Manifest.Find(EAlphaTargetPlatform::Win64);
    const FAlphaCandidateDescriptor* Linux = Manifest.Find(EAlphaTargetPlatform::LinuxX64);
    if (!Windows || !WindowsCandidateSha256.Equals(Windows->Sha256, ESearchCase::IgnoreCase))
    {
        OutErrors.Add(TEXT("acceptance ledger Windows hash does not match the exact candidate"));
    }
    if (!Linux || !LinuxCandidateSha256.Equals(Linux->Sha256, ESearchCase::IgnoreCase))
    {
        OutErrors.Add(TEXT("acceptance ledger Linux hash does not match the exact candidate"));
    }

    RequireGate(bFullContentM0M8, TEXT("full_content_m0_m8"), OutErrors);
    RequireGate(bWindowsUe55BuildAndAutomation, TEXT("windows_ue55_build_and_automation"), OutErrors);
    RequireGate(bLinuxUe55BuildAndAutomation, TEXT("linux_ue55_build_and_automation"), OutErrors);
    RequireGate(bWindowsStartToCreditsPlaythrough, TEXT("windows_start_to_credits_playthrough"), OutErrors);
    RequireGate(bLinuxStartToCreditsPlaythrough, TEXT("linux_start_to_credits_playthrough"), OutErrors);
    RequireGate(bContentManifestReconciled, TEXT("content_manifest_reconciled"), OutErrors);
    RequireGate(bPerformanceFloor60Fps, TEXT("performance_floor_60_fps"), OutErrors);
    RequireGate(bPrivacyAccessDenied, TEXT("privacy_access_denied"), OutErrors);
    RequireGate(bCreditsMusicAndLicencesApproved, TEXT("credits_music_and_licences_approved"), OutErrors);
    RequireGate(bFloAlphaApproval, TEXT("flo_alpha_approval"), OutErrors);

    return OutErrors.IsEmpty();
}
