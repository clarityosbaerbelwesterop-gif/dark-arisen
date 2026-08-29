// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Release/PrivateAlphaSignOff.h"

bool FPrivateAlphaCreditsApproval::Validate(TArray<FString>& OutErrors) const
{
    if (CreditsRevision.IsNone())
    {
        OutErrors.Add(TEXT("M11 credits require a stable complete-attribution revision."));
    }
    if (MusicSourceReference.TrimStartAndEnd().IsEmpty())
    {
        OutErrors.Add(TEXT("M11 credits require an approved music source reference."));
    }
    if (MusicLicenceReference.TrimStartAndEnd().IsEmpty())
    {
        OutErrors.Add(TEXT("M11 credits require a music licence/rights reference."));
    }
    if (!bCompleteAttributionApproved)
    {
        OutErrors.Add(TEXT("M11 complete credits attribution is not approved."));
    }
    if (!bMusicCommercialRightsApproved)
    {
        OutErrors.Add(TEXT("M11 credits music commercial rights are not approved."));
    }
    if (!bMusicCostApproved)
    {
        OutErrors.Add(TEXT("M11 credits music cost is not approved."));
    }
    if (!bMusicIsNonReactive)
    {
        OutErrors.Add(TEXT("M11 credits music must obey the non-reactive music design law."));
    }
    return OutErrors.IsEmpty();
}

bool UPrivateAlphaSignOff::CanSignOff(
    const FAlphaReleaseManifest& ReleaseManifest,
    const FAlphaAcceptanceLedger& M8Acceptance,
    const UFullGameVerificationLedger* M10Verification,
    const FPrivateAlphaCreditsApproval& CreditsApproval,
    const bool bOperatorApproved,
    TArray<FString>& OutErrors) const
{
    TArray<FString> ReleaseErrors;
    if (!ReleaseManifest.Validate(ReleaseErrors))
    {
        OutErrors.Append(ReleaseErrors);
    }

    TArray<FString> AcceptanceErrors;
    if (!M8Acceptance.IsAcceptedForPromotion(ReleaseManifest, AcceptanceErrors))
    {
        OutErrors.Append(AcceptanceErrors);
    }

    if (!M10Verification)
    {
        OutErrors.Add(TEXT("M11 requires the exact M10 verification ledger for the candidate set."));
    }
    else
    {
        const FAlphaCandidateDescriptor* Windows = ReleaseManifest.Find(EAlphaTargetPlatform::Win64);
        const FAlphaCandidateDescriptor* Linux = ReleaseManifest.Find(EAlphaTargetPlatform::LinuxX64);
        if (!Windows || !Linux)
        {
            OutErrors.Add(TEXT("M11 requires both Windows and Linux candidate descriptors."));
        }
        else
        {
            if (!M10Verification->GetSourceCommit().Equals(ReleaseManifest.SourceCommit, ESearchCase::IgnoreCase)
                || !M10Verification->GetWindowsCandidateSha256().Equals(Windows->Sha256, ESearchCase::IgnoreCase)
                || !M10Verification->GetLinuxCandidateSha256().Equals(Linux->Sha256, ESearchCase::IgnoreCase)
                || M10Verification->GetContentManifestRevision() != ReleaseManifest.ContentManifestRevision)
            {
                OutErrors.Add(TEXT("M11 M10 evidence does not belong to the exact M8 release manifest candidates."));
            }
        }

        TArray<FString> VerificationErrors;
        if (!M10Verification->IsComplete(VerificationErrors))
        {
            OutErrors.Append(VerificationErrors);
        }
    }

    TArray<FString> CreditsErrors;
    if (!CreditsApproval.Validate(CreditsErrors))
    {
        OutErrors.Append(CreditsErrors);
    }

    if (!bOperatorApproved)
    {
        OutErrors.Add(TEXT("M11 requires explicit operator Alpha approval."));
    }

    return OutErrors.IsEmpty();
}
