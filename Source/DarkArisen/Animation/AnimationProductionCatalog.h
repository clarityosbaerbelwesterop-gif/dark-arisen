// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EAnimationProductionFamily : uint8
{
    Combat,
    WoundDeterioration,
    Locomotion,
    Traversal,
    Interaction,
    Mounted,
    PerformanceMoment
};

struct FAnimationProductionRequirement
{
    FName StableId;
    FString DisplayName;
    FString GoverningSource;
    EAnimationProductionFamily Family = EAnimationProductionFamily::PerformanceMoment;
    FString GameplayAuthority;
    FString AcceptanceRead;
    bool bAssetAuthored = false;
    FString AssetPath;
};

struct FAnimationProductionDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Production list for authored animation assets. Runtime combat/wound/posture state remains owned by
 * existing C++ components and UDarkArisenAnimInstance; this catalog never substitutes gameplay logic.
 */
class DARKARISEN_API FAnimationProductionCatalog
{
public:
    static constexpr int32 NamedBibleRequirementCount = 11;
    static constexpr int32 RequiredHorseGaitTransitionCount = 6;
    static constexpr int32 MinimumAttackTellFrames = 8;
    static constexpr int32 LockedDeflectionWindowFrames = 6;

    static TArray<FAnimationProductionRequirement> BuildNamedBibleRequirements();
    static TArray<FAnimationProductionRequirement> BuildSystemRequirements();
    static TArray<FAnimationProductionDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);
};
