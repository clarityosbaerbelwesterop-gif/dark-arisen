// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ThreadRuntimeCoverage.generated.h"

UENUM(BlueprintType)
enum class EThreadNativeCoverageState : uint8
{
    NativeContractPresent,
    NativeContractWithAuthorshipGaps
};

USTRUCT(BlueprintType)
struct FThreadNativeCoverageEntry
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName ThreadId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString NativeOwner;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString GoverningSource;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EThreadNativeCoverageState State = EThreadNativeCoverageState::NativeContractPresent;

    /** Stable production gaps only. These are not quest IDs and cannot be executed as content. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<FName> AuthorshipGapIds;
};

/**
 * Source-level coverage register for all seventeen Phase-11 Threads.
 *
 * "Native contract present" means only that source/runtime ownership exists. It is not evidence of
 * UE compilation, authored maps/assets/dialogue/audio, runtime completion or Alpha acceptance.
 */
struct DARKARISEN_API FThreadRuntimeCoverage
{
    static constexpr int32 RequiredThreadCount = 17;

    static TArray<FThreadNativeCoverageEntry> Build();
    static bool Validate(TArray<FString>& OutErrors);
};
