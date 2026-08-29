// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class ETier1CommanderEncounterMode : uint8
{
    HumanDuel,
    NegotiationPursuit,
    VerticalPursuit,
    NavalEngagement
};

enum class ETier1CommanderResolutionRule : uint8
{
    StandardKilledSparedAvoided,
    SpareRefusedLeaveOnly
};

struct FTier1CommanderDefinition
{
    FName StableId;
    FString DisplayName;
    FString Colony;
    FString Castle;
    FString GoverningSource;
    ETier1CommanderEncounterMode EncounterMode = ETier1CommanderEncounterMode::HumanDuel;
    ETier1CommanderResolutionRule ResolutionRule = ETier1CommanderResolutionRule::StandardKilledSparedAvoided;

    /** Health-fraction boundaries for authored three-band duel phases. Empty for non-health encounters. */
    TArray<float> PhaseHealthFractions;
    TArray<FName> PhaseIds;

    FString ArenaMechanic;
    FString AvoidedPath;
    FString PrimaryReward;

    /** Finite technical IDs for only the non-combat resolutions explicitly authored in the source. */
    TArray<FName> AllowedAvoidedRouteIds;

    bool bDirectCombatant = true;
    bool bHasUndercityVariant = false;
    bool bHasBoardingVariant = false;
    bool bHasAllianceAvoidancePath = false;
    bool bObjectiveCanBeDestroyedByFire = false;
    bool bGarrisonWeakensOverTime = false;
};

/**
 * Engineering authority for the nine Phase-7 Tier-1 commanders. It is a source-backed encounter
 * registry, not a replacement for combat/ship/siege/war systems and not an AI behaviour generator.
 */
class DARKARISEN_API FTier1CommanderCatalog
{
public:
    static constexpr int32 RequiredCommanderCount = 9;

    static TArray<FTier1CommanderDefinition> BuildAll();
    static bool TryGet(FName StableId, FTier1CommanderDefinition& OutDefinition);
    static bool Validate(TArray<FString>& OutErrors);
};
