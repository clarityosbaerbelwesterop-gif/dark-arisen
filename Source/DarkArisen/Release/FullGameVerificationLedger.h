// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FullGameVerificationLedger.generated.h"

UENUM(BlueprintType)
enum class EFullGameVerificationGate : uint8
{
    WindowsUe55Compile,
    LinuxUe55Compile,
    UnrealAutomation,
    ContentManifestReconciled,
    SaveMigrationVerified,
    CrossPlatformSaveCompatibility,
    ControllerInputParity,
    AccessibilityParity,
    WindowsStartToCredits,
    LinuxStartToCredits,
    PerformanceFloor60Fps,
    DesignLawsRegressionFree,
    PrivacyAccessDenied
};

USTRUCT(BlueprintType)
struct FFullGameEvidenceRecord
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EFullGameVerificationGate Gate = EFullGameVerificationGate::WindowsUe55Compile;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString SourceCommit;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString WindowsCandidateSha256;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString LinuxCandidateSha256;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ContentManifestRevision;

    /** Sanitized private evidence locator or evidence ID. Never a secret/public gameplay URL. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString EvidenceReference;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bPassed = false;

    /** Runtime/runner/operator observations cannot be synthesized by source code. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bExternalOrRunnerEvidence = false;
};

/**
 * M10 immutable-candidate verification authority.
 * Evidence may only be appended for the exact configured source commit and Windows/Linux hashes.
 */
UCLASS(BlueprintType)
class DARKARISEN_API UFullGameVerificationLedger : public UObject
{
    GENERATED_BODY()

public:
    bool InitializeCandidateSet(
        const FString& InSourceCommit,
        const FString& InWindowsSha256,
        const FString& InLinuxSha256,
        FName InContentManifestRevision,
        FString& OutError);

    bool AppendEvidence(const FFullGameEvidenceRecord& Record, FString& OutError);

    UFUNCTION(BlueprintPure, Category="M10|Verification")
    bool HasPassed(EFullGameVerificationGate Gate) const;

    bool IsComplete(TArray<FString>& OutErrors) const;

    UFUNCTION(BlueprintPure, Category="M10|Verification")
    const TArray<FFullGameEvidenceRecord>& GetEvidence() const { return Evidence; }

    UFUNCTION(BlueprintPure, Category="M10|Verification")
    FString GetSourceCommit() const { return SourceCommit; }

    UFUNCTION(BlueprintPure, Category="M10|Verification")
    FString GetWindowsCandidateSha256() const { return WindowsCandidateSha256; }

    UFUNCTION(BlueprintPure, Category="M10|Verification")
    FString GetLinuxCandidateSha256() const { return LinuxCandidateSha256; }

    UFUNCTION(BlueprintPure, Category="M10|Verification")
    FName GetContentManifestRevision() const { return ContentManifestRevision; }

private:
    UPROPERTY(VisibleAnywhere, Category="M10|Verification")
    FString SourceCommit;

    UPROPERTY(VisibleAnywhere, Category="M10|Verification")
    FString WindowsCandidateSha256;

    UPROPERTY(VisibleAnywhere, Category="M10|Verification")
    FString LinuxCandidateSha256;

    UPROPERTY(VisibleAnywhere, Category="M10|Verification")
    FName ContentManifestRevision;

    UPROPERTY(VisibleAnywhere, Category="M10|Verification")
    TArray<FFullGameEvidenceRecord> Evidence;

    bool bInitialized = false;

    static bool IsHexOfLength(const FString& Value, int32 Length);
    static const TArray<EFullGameVerificationGate>& RequiredGates();
};
