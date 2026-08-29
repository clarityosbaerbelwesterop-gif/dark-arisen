// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ContentScale/ContentScaleManifestDataAsset.h"

/** Phase-11 regions used by the exact 132-Turn distribution. */
enum class ETurnProductionRegion : uint8
{
    RexaMoran,
    Fjordlund,
    Ashenmoor,
    QuietCoast,
    PaleIsleSilvera,
    AtSea,
    Highmoore,
    Region06
};

/** A production slot is not a quest identity until a governing source authors it. */
enum class EAuthoredMissionSlotState : uint8
{
    ReservedUnauthored,
    AuthoredIdentity
};

struct FTurnProductionRequirement
{
    ETurnProductionRegion Region = ETurnProductionRegion::RexaMoran;
    FName RegionId;
    int32 RequiredCount = 0;
    int32 AuthoredIdentityCount = 0;
};

struct FStandingTypeProductionRequirement
{
    EM7StandingMissionType Type = EM7StandingMissionType::None;
    int32 RequiredCount = 0;
    int32 AuthoredIdentityCount = 0;
    FString GoverningSource;
};

struct FMissionScaleDesignGap
{
    FName StableId;
    FString Description;
    FString GoverningSource;
};

/**
 * Exact finite production contract for Phase 11. It never constructs a mission, person, place,
 * outcome, pay value, dialogue line or region/type allocation that the design corpus does not author.
 */
class DARKARISEN_API FMissionScaleRequirements
{
public:
    static constexpr int32 RequiredTurnCount = 132;
    static constexpr int32 RequiredStandingCount = 147;
    static constexpr int32 KnownTurnIdentityCount = 3;
    static constexpr int32 KnownStandingIdentityCount = 1;

    static TArray<FTurnProductionRequirement> BuildTurnRequirements();
    static TArray<FStandingTypeProductionRequirement> BuildStandingTypeRequirements();
    static TArray<FMissionScaleDesignGap> BuildDesignGaps();

    /** True only for the four identities already grounded by current native source. */
    static EAuthoredMissionSlotState ResolveKnownIdentity(FName QuestId);

    static bool Validate(TArray<FString>& OutErrors);
};
