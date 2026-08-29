// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EAudioProductionKind : uint8
{
    Environmental,
    DiegeticMusic,
    NonDiegeticCue,
    WeaponResonance,
    SilenceContract
};

struct FAudioProductionRequirement
{
    FName StableId;
    FString DisplayName;
    FString GoverningSource;
    EAudioProductionKind Kind = EAudioProductionKind::Environmental;
    FString ProductionRule;
    bool bReactiveToGameplayEvent = false;
    bool bAssetAuthored = false;
    FString AssetPath;
};

struct FAudioProductionDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Source-backed audio/music production authority. Dark Arisen's current locked law is non-reactive
 * music: cues may be authored to scenes/contexts, but gameplay events do not dynamically add/remove
 * layers, change intensity or fire stingers.
 */
class DARKARISEN_API FAudioProductionCatalog
{
public:
    static constexpr int32 CoreRequirementCount = 12;

    static TArray<FAudioProductionRequirement> BuildCoreRequirements();
    static TArray<FAudioProductionDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);
};
