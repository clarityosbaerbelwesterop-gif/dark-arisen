// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ContentScale/AuthoredDungeonProductionProfile.h"

TArray<FAuthoredDungeonProductionProfile> FAuthoredDungeonProductionProfiles::BuildAllKnownProfiles()
{
    TArray<FAuthoredDungeonProductionProfile> Profiles;
    Profiles.Reserve(40);

    AppendRexaMoran(Profiles);
    AppendFjordlund(Profiles);
    AppendAshenmoor(Profiles);
    AppendSea(Profiles);
    AppendColonial(Profiles);
    AppendRegion06AndCrossNetwork(Profiles);
    AppendHighmoore(Profiles);

    return Profiles;
}

bool FAuthoredDungeonProductionProfiles::TryGetProfile(
    const FName StableId,
    FAuthoredDungeonProductionProfile& OutProfile)
{
    if (StableId.IsNone())
    {
        return false;
    }

    const TArray<FAuthoredDungeonProductionProfile> Profiles = BuildAllKnownProfiles();
    for (const FAuthoredDungeonProductionProfile& Profile : Profiles)
    {
        if (Profile.StableId == StableId)
        {
            OutProfile = Profile;
            return true;
        }
    }
    return false;
}

bool FAuthoredDungeonProductionProfiles::ValidateProfiles(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    const TArray<FAuthoredDungeonProductionProfile> Profiles = BuildAllKnownProfiles();
    if (Profiles.Num() != 40)
    {
        OutErrors.Add(FString::Printf(TEXT("Dungeon production profile set requires exactly 40 grounded named sites; found %d."), Profiles.Num()));
    }

    TSet<FName> SeenIds;
    for (const FAuthoredDungeonProductionProfile& Profile : Profiles)
    {
        if (Profile.StableId.IsNone())
        {
            OutErrors.Add(TEXT("Dungeon production profile has no stable ID."));
            continue;
        }
        if (SeenIds.Contains(Profile.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate dungeon production profile ID: %s"), *Profile.StableId.ToString()));
        }
        SeenIds.Add(Profile.StableId);

        FAuthoredDungeonCatalogEntry CatalogEntry;
        if (!FAuthoredDungeonCatalog::TryGetKnownSite(Profile.StableId, CatalogEntry))
        {
            OutErrors.Add(FString::Printf(TEXT("Dungeon production profile has no grounded catalog entry: %s"), *Profile.StableId.ToString()));
        }

        if (Profile.GoverningSource.TrimStartAndEnd().IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Dungeon production profile has no governing source: %s"), *Profile.StableId.ToString()));
        }
        if (Profile.AccessAndDiscovery.TrimStartAndEnd().IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Dungeon production profile has no access/discovery contract: %s"), *Profile.StableId.ToString()));
        }
        if (Profile.PuzzleLanguageDetail.TrimStartAndEnd().IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Dungeon production profile has no puzzle-language contract: %s"), *Profile.StableId.ToString()));
        }
        if (Profile.HazardDetail.TrimStartAndEnd().IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Dungeon production profile has no hazard contract: %s"), *Profile.StableId.ToString()));
        }
        if (!Profile.bImageWithheld && Profile.UnforgettableImage.TrimStartAndEnd().IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Dungeon production profile is missing its authored unforgettable image: %s"), *Profile.StableId.ToString()));
        }
        if (!Profile.bBossWithheldOrUnresolved && Profile.BossOrBottomDetail.TrimStartAndEnd().IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Dungeon production profile is missing its authored boss/bottom detail: %s"), *Profile.StableId.ToString()));
        }
        if (!Profile.bRewardExplicitlyNone
            && !Profile.bRewardWithheldOrUnresolved
            && Profile.RewardDetail.TrimStartAndEnd().IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Dungeon production profile is missing its authored reward: %s"), *Profile.StableId.ToString()));
        }
        if (Profile.bRewardExplicitlyNone && !Profile.RewardDetail.TrimStartAndEnd().IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Explicit no-reward site may not invent a reward summary: %s"), *Profile.StableId.ToString()));
        }
    }

    const TArray<FAuthoredDungeonCatalogEntry> Catalog = FAuthoredDungeonCatalog::BuildKnownNamedSites();
    for (const FAuthoredDungeonCatalogEntry& Entry : Catalog)
    {
        if (!SeenIds.Contains(Entry.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Grounded named dungeon lacks a production profile: %s"), *Entry.StableId.ToString()));
        }
    }

    return OutErrors.IsEmpty();
}
