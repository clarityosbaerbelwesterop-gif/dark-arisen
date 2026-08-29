// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EPresentationAnimationPriority : uint8
{
    NoneRequired,
    P1,
    P2,
    P3,
    Unresolved
};

enum class EPresentationControlOwnership : uint8
{
    SequencerOwned,
    BriefInsert,
    SlowPushOnly,
    PlayerControlled
};

struct FAuthoredCutsceneDefinition
{
    int32 CatalogNumber = 0;
    FName StableId;
    FString DisplayName;
    FString GoverningSource;
    FString CameraRule;
    FString MusicRule;
    float TargetSeconds = 0.0f;
    EPresentationAnimationPriority AnimationPriority = EPresentationAnimationPriority::Unresolved;
    EPresentationControlOwnership ControlOwnership = EPresentationControlOwnership::SequencerOwned;
    bool bIdentityResolved = true;
    bool bCameraMustRemainStatic = false;
    bool bMusicMustContinueUnchanged = false;
};

struct FProtectedPlayableMomentDefinition
{
    int32 CatalogNumber = 0;
    FName StableId;
    FString DisplayName;
    FString GoverningSource;
    FString Reason;
};

struct FPresentationDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Phase-10 production authority for the nineteen cutscene slots and twenty-two moments that may
 * never become cutscenes. It does not create Sequencer assets or claim capture/audio exists.
 */
class DARKARISEN_API FPresentationProductionCatalog
{
public:
    static constexpr int32 RequiredCutsceneCount = 19;
    static constexpr int32 ResolvedCutsceneIdentityCount = 14;
    static constexpr int32 UnresolvedEndgameCutsceneCount = 5;
    static constexpr int32 RequiredProtectedPlayableCount = 22;
    static constexpr int32 RequiredInsertCount = 5;
    static constexpr int32 RequiredSlowPushCount = 6;

    static TArray<FAuthoredCutsceneDefinition> BuildCutscenes();
    static TArray<FProtectedPlayableMomentDefinition> BuildProtectedPlayableMoments();
    static TArray<FName> BuildInsertIds();
    static TArray<FName> BuildSlowPushIds();
    static TArray<FPresentationDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);
};
