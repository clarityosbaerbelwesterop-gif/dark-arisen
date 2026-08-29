// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ContentScale/AuthoredDungeonCatalog.h"

/**
 * A canonical Tier-A/minor slot from the Phase-11 global table.
 *
 * The corpus locks the regional counts but deliberately does not author individual names,
 * placements, encounters or rewards. These records therefore reserve deterministic content slots
 * without pretending that the missing authored content exists.
 */
struct FAuthoredMinorDungeonSlot
{
    FName StableId;
    EDungeonCatalogRegion Region = EDungeonCatalogRegion::RexaMoran;
    FString GoverningSource;

    /** Set only when a later governing document explicitly gives this slot a site identity. */
    bool bIdentityAuthored = false;

    /** Set only when a concrete world placement is authored. */
    bool bPlacementAuthored = false;

    /** Hard prohibition: these slots may never become random/radiant runtime generation. */
    bool bGeneratedAtRuntime = false;
};

/**
 * C++ authority for the twenty globally required Tier-A/minor slots.
 *
 * It encodes only what `dungeon system global.md` Section 2 actually locks:
 * Rexa/Moran 4, Fjordlund 3, Ashenmoor 4, Sea 3, Colonial 2, Region 06 1, Highmoore 3.
 */
class DARKARISEN_API FAuthoredMinorDungeonCatalog
{
public:
    static constexpr int32 RequiredSlotCount = 20;

    static TArray<FAuthoredMinorDungeonSlot> BuildSlots();
    static bool ValidateSlots(TArray<FString>& OutErrors);

    static int32 RequiredCountForRegion(EDungeonCatalogRegion Region);
};
