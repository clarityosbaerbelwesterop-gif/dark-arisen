// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DesignLaws.h"
#include "AlphaReleaseContracts.generated.h"

UENUM(BlueprintType)
enum class EAlphaTargetPlatform : uint8
{
    Win64,
    LinuxX64
};

UENUM(BlueprintType)
enum class EColourblindPresentationMode : uint8
{
    Off,
    Deuteranopia,
    Protanopia,
    Tritanopia
};

USTRUCT(BlueprintType)
struct FAccessibilityDifficultyContract
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bInputRemappingEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.75", ClampMax="2.00"))
    float SubtitleScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EColourblindPresentationMode ColourblindMode = EColourblindPresentationMode::Off;

    /** Permitted difficulty tuning surface: damage only. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.25", ClampMax="4.00"))
    float IncomingDamageMultiplier = 1.0f;

    /** Permitted difficulty tuning surface: health only. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.25", ClampMax="4.00"))
    float PlayerHealthMultiplier = 1.0f;

    /** Must remain exactly the global six-frame design law in every difficulty mode. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 DeflectionWindowFrames = DarkArisen::DesignLaws::DeflectionWindowFrames;

    bool Validate(TArray<FString>& OutErrors) const;
};

USTRUCT(BlueprintType)
struct FSaveMigrationStep
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 FromVersion = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ToVersion = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName StableMigrationId;
};

/**
 * M8 save schema authority. There is no fabricated historical migration: version 1 is the first
 * accepted schema. Future migrations must be explicitly registered as contiguous one-version steps.
 */
struct DARKARISEN_API FSaveSchemaMigrationRegistry
{
    static constexpr int32 CurrentSchemaVersion = 1;

    static bool ValidateRegistry(const TArray<FSaveMigrationStep>& Steps, TArray<FString>& OutErrors);
    static bool TryBuildMigrationPath(
        int32 SavedVersion,
        const TArray<FSaveMigrationStep>& Steps,
        TArray<FSaveMigrationStep>& OutPath,
        FString& OutError);
};

USTRUCT(BlueprintType)
struct FAlphaCandidateDescriptor
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EAlphaTargetPlatform Platform = EAlphaTargetPlatform::Win64;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString SourceCommit;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ContentManifestRevision;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString Sha256;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bShipping = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bContainsDeploymentCredentials = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bContainsTurnSecret = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bContainsPrivateHostname = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bContainsPublicLinkConfiguration = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bContainsSonyOrNativePs5Material = false;

    bool IsPrivateAlphaSafe(TArray<FString>& OutErrors) const;
};

USTRUCT(BlueprintType)
struct FAlphaReleaseManifest
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString SourceCommit;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ContentManifestRevision;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString SourceArchiveSha256;

    /** Exactly Win64 + Linux x86_64, from the same commit and manifest revision. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FAlphaCandidateDescriptor> Candidates;

    bool Validate(TArray<FString>& OutErrors) const;
    const FAlphaCandidateDescriptor* Find(EAlphaTargetPlatform Platform) const;
};

USTRUCT(BlueprintType)
struct FAlphaAcceptanceLedger
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString CandidateCommit;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString WindowsCandidateSha256;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString LinuxCandidateSha256;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bFullContentM0M8 = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bWindowsUe55BuildAndAutomation = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bLinuxUe55BuildAndAutomation = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bWindowsStartToCreditsPlaythrough = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bLinuxStartToCreditsPlaythrough = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bContentManifestReconciled = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bPerformanceFloor60Fps = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bPrivacyAccessDenied = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bCreditsMusicAndLicencesApproved = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bFloAlphaApproval = false;

    /** Source cannot self-certify these observations; this only validates supplied evidence state. */
    bool IsAcceptedForPromotion(const FAlphaReleaseManifest& Manifest, TArray<FString>& OutErrors) const;
};
