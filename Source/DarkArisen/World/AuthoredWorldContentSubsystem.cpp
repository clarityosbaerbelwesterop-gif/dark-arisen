// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "World/AuthoredWorldContentSubsystem.h"

#include "World/AuthoredWorldRegionRegistry.h"

bool UAuthoredWorldContentSubsystem::BindLoadedRegion(
    const FName RegionStableId,
    const FString& RuntimeMapPackage)
{
    if (RegionStableId.IsNone() || RuntimeMapPackage.TrimStartAndEnd().IsEmpty())
    {
        return false;
    }

    FAuthoredWorldRegionDefinition Definition;
    if (!FAuthoredWorldRegionRegistry::TryGet(RegionStableId, Definition))
    {
        return false;
    }

    if (LoadedRegion.bSourceRegistryKnown)
    {
        return false;
    }

    LoadedRegion.RegionStableId = RegionStableId;
    LoadedRegion.RuntimeMapPackage = RuntimeMapPackage;
    LoadedRegion.bSourceRegistryKnown = true;
    return true;
}

void UAuthoredWorldContentSubsystem::ClearLoadedRegion()
{
    LoadedRegion = FLoadedRegionBinding();
}
