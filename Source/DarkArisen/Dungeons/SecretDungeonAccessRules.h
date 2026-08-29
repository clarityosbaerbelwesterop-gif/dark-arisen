// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** Canonical five Liberation Alliance networks. This file owns identifiers, not network state. */
enum class ELiberationNetwork : uint8
{
    CulturalWeb,
    RecoveryUnderground,
    MountainPaths,
    PatientCoast,
    DeedJudgers
};

/** Canonical Trust Tiers from indigenous_liberation_alliance.md Section 3.5. */
enum class ELiberationTrustTier : uint8
{
    Watched = 0,
    Tolerated = 1,
    Trusted = 2,
    Kin = 3,
    Blood = 4
};

struct FSecretDungeonAccessEvidence
{
    /** Supplied by the real Alliance authority. Dungeon code does not mutate trust. */
    TMap<ELiberationNetwork, ELiberationTrustTier> TrustByNetwork;

    /** Stable authored knowledge/artifact/route facts supplied by their owning systems. */
    TSet<FName> EvidenceIds;

    bool bFifthConnectionGatheringCompleted = false;
    bool bRegion06Reached = false;
    bool bRecoveryReturnThresholdSatisfied = false;
    bool bEmpirePath = false;
};

struct FSecretDungeonAccessRequirement
{
    FName DungeonStableId;
    FString GoverningSource;

    TArray<ELiberationNetwork> RequiredBloodNetworks;
    bool bAnyBloodNetworkSatisfies = false;
    bool bRequiresAllFiveBloodNetworks = false;

    TArray<FName> RequiredEvidenceIds;
    bool bRequiresFifthConnectionGathering = false;
    bool bRequiresRegion06Reached = false;
    bool bRequiresRecoveryReturnThreshold = false;

    /** Secret content is simply absent on the Empire path; no missed-content UI is emitted. */
    bool bEmpirePathBlind = true;
};

/**
 * Pure evidence evaluator for the six authored Tier-E/secret sites.
 * It performs no trust progression and exposes no map marker or checklist to the player.
 */
class DARKARISEN_API FSecretDungeonAccessRules
{
public:
    static TArray<FSecretDungeonAccessRequirement> BuildRequirements();

    static bool CanAccess(
        FName DungeonStableId,
        const FSecretDungeonAccessEvidence& Evidence,
        FString& OutFailureReason);

    static bool HasBloodTrust(
        const FSecretDungeonAccessEvidence& Evidence,
        ELiberationNetwork Network);
};
