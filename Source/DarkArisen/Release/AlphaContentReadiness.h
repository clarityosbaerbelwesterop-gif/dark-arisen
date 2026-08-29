// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FAlphaContentFamilyStatus
{
    FName StableId;
    FString DisplayName;
    int32 RequiredCount = 0;
    int32 SourceGroundedCount = 0;
    int32 ProductionAssetCount = 0;
    FString GoverningSource;
    FString BlockingReason;

    bool IsSourceIdentityComplete() const { return SourceGroundedCount == RequiredCount; }
    bool IsProductionComplete() const { return ProductionAssetCount == RequiredCount; }
};

/**
 * Immutable source snapshot for the current authored-content tranche. It deliberately separates
 * source-grounded identity from real UE/audio production assets and can never self-certify runtime.
 */
class DARKARISEN_API FAlphaContentReadiness
{
public:
    static TArray<FAlphaContentFamilyStatus> BuildCurrentSnapshot();

    /** True only when every required family has both complete identities and production assets. */
    static bool IsContentProductionComplete(TArray<FString>& OutBlockers);

    static bool ValidateSnapshot(TArray<FString>& OutErrors);
};
