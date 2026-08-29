// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Dungeons/SecretDungeonAccessRules.h"

namespace
{
FSecretDungeonAccessRequirement Requirement(
    const TCHAR* SiteId,
    const TCHAR* Source)
{
    FSecretDungeonAccessRequirement R;
    R.DungeonStableId = FName(SiteId);
    R.GoverningSource = Source;
    return R;
}
}

TArray<FSecretDungeonAccessRequirement> FSecretDungeonAccessRules::BuildRequirements()
{
    TArray<FSecretDungeonAccessRequirement> Requirements;
    Requirements.Reserve(6);

    {
        FSecretDungeonAccessRequirement R = Requirement(
            TEXT("dungeon.rexa.grove-beneath-grove"),
            TEXT("secret bosses.md Section 3"));
        R.RequiredBloodNetworks = {ELiberationNetwork::CulturalWeb};
        R.RequiredEvidenceIds = {
            TEXT("knowledge.rexa.secret-grove-map"),
            TEXT("route.rexa.deep-canopy-open"),
            TEXT("artifact.rexa.ceremonial-piece-returned")
        };
        Requirements.Add(R);
    }

    {
        FSecretDungeonAccessRequirement R = Requirement(
            TEXT("dungeon.fjordlund.one-that-waited"),
            TEXT("secret bosses.md Section 4"));
        R.RequiredBloodNetworks = {ELiberationNetwork::PatientCoast};
        R.RequiredEvidenceIds = {
            TEXT("knowledge.fjordlund.untold-saga-map"),
            TEXT("route.fjordlund.northern-winter-waters"),
            TEXT("artifact.fjordlund.drinking-horn-returned")
        };
        Requirements.Add(R);
    }

    {
        FSecretDungeonAccessRequirement R = Requirement(
            TEXT("dungeon.ashenmoor.thing-wyrm-left"),
            TEXT("secret bosses.md Section 5"));
        R.RequiredBloodNetworks = {ELiberationNetwork::MountainPaths};
        R.RequiredEvidenceIds = {
            TEXT("knowledge.ashenmoor.singing-caverns-star-map"),
            TEXT("route.ashenmoor.high-volcanic-slopes-night"),
            TEXT("artifact.ashenmoor.1846-patrol-reports-returned")
        };
        Requirements.Add(R);
    }

    {
        FSecretDungeonAccessRequirement R = Requirement(
            TEXT("dungeon.cross-network.fifth-connection"),
            TEXT("secret bosses.md Section 6"));
        R.bRequiresAllFiveBloodNetworks = true;
        R.bRequiresFifthConnectionGathering = true;
        R.RequiredEvidenceIds = {
            TEXT("knowledge.cross-network.five-map-languages-assembled"),
            TEXT("route.cross-network.fifth-site-assembled")
        };
        Requirements.Add(R);
    }

    {
        FSecretDungeonAccessRequirement R = Requirement(
            TEXT("dungeon.region06.fourth-fortress"),
            TEXT("secret bosses.md Section 7; regional dungeon catalog.md Section 7.4"));
        R.bAnyBloodNetworkSatisfies = true;
        R.bRequiresRegion06Reached = true;
        Requirements.Add(R);
    }

    {
        FSecretDungeonAccessRequirement R = Requirement(
            TEXT("dungeon.sea.harbour-that-was-first"),
            TEXT("secret bosses.md Section 8"));
        R.RequiredBloodNetworks = {ELiberationNetwork::RecoveryUnderground};
        R.RequiredEvidenceIds = {
            TEXT("knowledge.sea.deep-harbour-charts"),
            TEXT("route.sea.deepest-feared-waters")
        };
        R.bRequiresRecoveryReturnThreshold = true;
        Requirements.Add(R);
    }

    return Requirements;
}

bool FSecretDungeonAccessRules::HasBloodTrust(
    const FSecretDungeonAccessEvidence& Evidence,
    const ELiberationNetwork Network)
{
    const ELiberationTrustTier* Tier = Evidence.TrustByNetwork.Find(Network);
    return Tier && *Tier == ELiberationTrustTier::Blood;
}

bool FSecretDungeonAccessRules::CanAccess(
    const FName DungeonStableId,
    const FSecretDungeonAccessEvidence& Evidence,
    FString& OutFailureReason)
{
    OutFailureReason.Reset();

    const TArray<FSecretDungeonAccessRequirement> Requirements = BuildRequirements();
    const FSecretDungeonAccessRequirement* Match = nullptr;
    for (const FSecretDungeonAccessRequirement& RequirementEntry : Requirements)
    {
        if (RequirementEntry.DungeonStableId == DungeonStableId)
        {
            Match = &RequirementEntry;
            break;
        }
    }

    if (!Match)
    {
        OutFailureReason = TEXT("No authored secret-site access requirement exists for this stable ID.");
        return false;
    }

    if (Match->bEmpirePathBlind && Evidence.bEmpirePath)
    {
        OutFailureReason = TEXT("Secret site is absent on the Empire path.");
        return false;
    }

    if (Match->bRequiresAllFiveBloodNetworks)
    {
        const ELiberationNetwork AllNetworks[] = {
            ELiberationNetwork::CulturalWeb,
            ELiberationNetwork::RecoveryUnderground,
            ELiberationNetwork::MountainPaths,
            ELiberationNetwork::PatientCoast,
            ELiberationNetwork::DeedJudgers
        };
        for (const ELiberationNetwork Network : AllNetworks)
        {
            if (!HasBloodTrust(Evidence, Network))
            {
                OutFailureReason = TEXT("All five networks must be at Blood trust for this site.");
                return false;
            }
        }
    }
    else if (Match->bAnyBloodNetworkSatisfies)
    {
        const ELiberationNetwork AllNetworks[] = {
            ELiberationNetwork::CulturalWeb,
            ELiberationNetwork::RecoveryUnderground,
            ELiberationNetwork::MountainPaths,
            ELiberationNetwork::PatientCoast,
            ELiberationNetwork::DeedJudgers
        };
        bool bAnyBlood = false;
        for (const ELiberationNetwork Network : AllNetworks)
        {
            bAnyBlood |= HasBloodTrust(Evidence, Network);
        }
        if (!bAnyBlood)
        {
            OutFailureReason = TEXT("At least one Alliance network must be at Blood trust for this site.");
            return false;
        }
    }
    else
    {
        for (const ELiberationNetwork Network : Match->RequiredBloodNetworks)
        {
            if (!HasBloodTrust(Evidence, Network))
            {
                OutFailureReason = TEXT("Required Alliance network has not reached Blood trust.");
                return false;
            }
        }
    }

    for (const FName EvidenceId : Match->RequiredEvidenceIds)
    {
        if (!Evidence.EvidenceIds.Contains(EvidenceId))
        {
            OutFailureReason = FString::Printf(TEXT("Missing authored access evidence: %s"), *EvidenceId.ToString());
            return false;
        }
    }

    if (Match->bRequiresFifthConnectionGathering && !Evidence.bFifthConnectionGatheringCompleted)
    {
        OutFailureReason = TEXT("The five-network connection gathering has not occurred.");
        return false;
    }

    if (Match->bRequiresRegion06Reached && !Evidence.bRegion06Reached)
    {
        OutFailureReason = TEXT("Region 06 has not been physically reached.");
        return false;
    }

    if (Match->bRequiresRecoveryReturnThreshold && !Evidence.bRecoveryReturnThresholdSatisfied)
    {
        // The source says the lock is a hidden number but does not author the threshold here.
        OutFailureReason = TEXT("Recovery Underground sacred-return threshold is not satisfied.");
        return false;
    }

    return true;
}
