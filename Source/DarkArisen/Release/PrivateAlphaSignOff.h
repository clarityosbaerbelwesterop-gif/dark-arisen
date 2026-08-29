// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Release/AlphaReleaseContracts.h"
#include "Release/FullGameVerificationLedger.h"
#include "PrivateAlphaSignOff.generated.h"

USTRUCT(BlueprintType)
struct FPrivateAlphaCreditsApproval
{
    GENERATED_BODY()

    /** Sanitized revision/id for the complete credits names/order/attribution list. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName CreditsRevision;

    /** Sanitized source identifier for the approved end-credits cue. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString MusicSourceReference;

    /** Sanitized licence/rights record identifier; never a secret or billing token. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString MusicLicenceReference;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bCompleteAttributionApproved = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bMusicCommercialRightsApproved = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bMusicCostApproved = false;

    /** Locked design law: end-credits music is non-reactive. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bMusicIsNonReactive = false;

    bool Validate(TArray<FString>& OutErrors) const;
};

/**
 * M11 final private Alpha source authority.
 *
 * This object does not package, deploy, pay for infrastructure, or approve the game. It only
 * evaluates supplied immutable M8/M10 evidence plus explicit operator approval. Promotion of the
 * exact verified bytes remains owned by DarkArisenOps.
 */
UCLASS(BlueprintType)
class DARKARISEN_API UPrivateAlphaSignOff : public UObject
{
    GENERATED_BODY()

public:
    bool CanSignOff(
        const FAlphaReleaseManifest& ReleaseManifest,
        const FAlphaAcceptanceLedger& M8Acceptance,
        const UFullGameVerificationLedger* M10Verification,
        const FPrivateAlphaCreditsApproval& CreditsApproval,
        bool bOperatorApproved,
        TArray<FString>& OutErrors) const;
};
