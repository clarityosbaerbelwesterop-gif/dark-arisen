// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ContentScale/AuthoredDungeonCatalog.h"

bool FAuthoredDungeonCatalog::TryGetKnownSite(
    const FName StableId,
    FAuthoredDungeonCatalogEntry& OutEntry)
{
    if (StableId.IsNone())
    {
        return false;
    }

    const TArray<FAuthoredDungeonCatalogEntry> Sites = BuildKnownNamedSites();
    for (const FAuthoredDungeonCatalogEntry& Site : Sites)
    {
        if (Site.StableId == StableId)
        {
            OutEntry = Site;
            return true;
        }
    }
    return false;
}
