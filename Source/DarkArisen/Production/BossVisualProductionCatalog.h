// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EBossVisualProductionKind : uint8
{
    HumanDuelist,
    HumanCommander,
    PairedHumanEncounter,
    NavalCommander,
    MythicCreature,
    MonsterCreature,
    ConstructCreature,
    FinalNarrativeHuman
};

struct FBossVisualProductionBrief
{
    FName StableId;
    FString DisplayName;
    EBossVisualProductionKind Kind = EBossVisualProductionKind::HumanCommander;
    FString GoverningSource;
    FString VisualFacts;
    FString EquipmentOrSilhouette;
    FString ArenaVisualRead;
    FString PerformanceRead;
    FString ExplicitUnknowns;
    bool bProviderReferenceReady = false;
    bool bApprovedReferenceExists = false;
    FString ApprovedReferencePath;
};

struct FBossVisualProductionDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Visual/reference index over the twenty-one legacy/deep-dive boss specifications.
 * It is not a gameplay boss register. Current DesignAuthority blocks the legacy Ethan/Draven
 * final-act material until it is rewritten against Phase 11 or explicitly restored.
 */
class DARKARISEN_API FBossVisualProductionCatalog
{
public:
    static constexpr int32 DeepDiveBossBriefCount = 21;
    static constexpr int32 ProviderReadyBossBriefCount = 19;
    static constexpr int32 AuthorityBlockedBossBriefCount = 2;

    static TArray<FBossVisualProductionBrief> BuildDeepDiveBossBriefs();
    static TArray<FBossVisualProductionDesignGap> BuildDesignGaps();
    static bool Validate(TArray<FString>& OutErrors);
};