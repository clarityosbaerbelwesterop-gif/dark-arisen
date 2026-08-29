// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "ContentScale/AuthoredMinorDungeonCatalog.h"

namespace
{
void AddRegionalSlots(
    TArray<FAuthoredMinorDungeonSlot>& OutSlots,
    const EDungeonCatalogRegion Region,
    const TCHAR* RegionSlug,
    const int32 Count)
{
    for (int32 Index = 1; Index <= Count; ++Index)
    {
        FAuthoredMinorDungeonSlot Slot;
        Slot.StableId = FName(*FString::Printf(TEXT("dungeon.minor.%s.slot-%02d"), RegionSlug, Index));
        Slot.Region = Region;
        Slot.GoverningSource = TEXT("dungeon system global.md Section 2; regional dungeon catalog.md Section 1.2");
        Slot.bIdentityAuthored = false;
        Slot.bPlacementAuthored = false;
        Slot.bGeneratedAtRuntime = false;
        OutSlots.Add(Slot);
    }
}
}

int32 FAuthoredMinorDungeonCatalog::RequiredCountForRegion(const EDungeonCatalogRegion Region)
{
    switch (Region)
    {
    case EDungeonCatalogRegion::RexaMoran:
        return 4;
    case EDungeonCatalogRegion::Fjordlund:
        return 3;
    case EDungeonCatalogRegion::Ashenmoor:
        return 4;
    case EDungeonCatalogRegion::Sea:
        return 3;
    case EDungeonCatalogRegion::Colonial:
        return 2;
    case EDungeonCatalogRegion::Region06:
        return 1;
    case EDungeonCatalogRegion::Highmoore:
        return 3;
    default:
        return 0;
    }
}

TArray<FAuthoredMinorDungeonSlot> FAuthoredMinorDungeonCatalog::BuildSlots()
{
    TArray<FAuthoredMinorDungeonSlot> Slots;
    Slots.Reserve(RequiredSlotCount);

    AddRegionalSlots(Slots, EDungeonCatalogRegion::RexaMoran, TEXT("rexa"), 4);
    AddRegionalSlots(Slots, EDungeonCatalogRegion::Fjordlund, TEXT("fjordlund"), 3);
    AddRegionalSlots(Slots, EDungeonCatalogRegion::Ashenmoor, TEXT("ashenmoor"), 4);
    AddRegionalSlots(Slots, EDungeonCatalogRegion::Sea, TEXT("sea"), 3);
    AddRegionalSlots(Slots, EDungeonCatalogRegion::Colonial, TEXT("colonial"), 2);
    AddRegionalSlots(Slots, EDungeonCatalogRegion::Region06, TEXT("region06"), 1);
    AddRegionalSlots(Slots, EDungeonCatalogRegion::Highmoore, TEXT("highmoore"), 3);

    return Slots;
}

bool FAuthoredMinorDungeonCatalog::ValidateSlots(TArray<FString>& OutErrors)
{
    OutErrors.Reset();

    const TArray<FAuthoredMinorDungeonSlot> Slots = BuildSlots();
    if (Slots.Num() != RequiredSlotCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Minor dungeon catalog requires exactly %d slots; found %d."), RequiredSlotCount, Slots.Num()));
    }

    TSet<FName> SeenIds;
    TMap<EDungeonCatalogRegion, int32> Counts;

    for (const FAuthoredMinorDungeonSlot& Slot : Slots)
    {
        if (Slot.StableId.IsNone())
        {
            OutErrors.Add(TEXT("Minor dungeon slot has no stable ID."));
            continue;
        }
        if (SeenIds.Contains(Slot.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate minor dungeon slot ID: %s"), *Slot.StableId.ToString()));
        }
        SeenIds.Add(Slot.StableId);

        if (Slot.GoverningSource.TrimStartAndEnd().IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("Minor dungeon slot has no governing source: %s"), *Slot.StableId.ToString()));
        }
        if (Slot.bGeneratedAtRuntime)
        {
            OutErrors.Add(FString::Printf(TEXT("Minor dungeon slot may not be random/radiant generated: %s"), *Slot.StableId.ToString()));
        }
        if (Slot.bIdentityAuthored != Slot.bPlacementAuthored)
        {
            OutErrors.Add(FString::Printf(TEXT("Minor dungeon identity/placement must advance together once authored: %s"), *Slot.StableId.ToString()));
        }

        Counts.FindOrAdd(Slot.Region)++;
    }

    const EDungeonCatalogRegion Regions[] = {
        EDungeonCatalogRegion::RexaMoran,
        EDungeonCatalogRegion::Fjordlund,
        EDungeonCatalogRegion::Ashenmoor,
        EDungeonCatalogRegion::Sea,
        EDungeonCatalogRegion::Colonial,
        EDungeonCatalogRegion::Region06,
        EDungeonCatalogRegion::Highmoore
    };

    for (const EDungeonCatalogRegion Region : Regions)
    {
        const int32 Required = RequiredCountForRegion(Region);
        const int32 Actual = Counts.FindRef(Region);
        if (Actual != Required)
        {
            OutErrors.Add(FString::Printf(
                TEXT("Minor dungeon regional quota mismatch for region %d: required %d, found %d."),
                static_cast<int32>(Region), Required, Actual));
        }
    }

    return OutErrors.IsEmpty();
}
