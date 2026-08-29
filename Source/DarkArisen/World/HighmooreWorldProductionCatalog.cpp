// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "World/HighmooreWorldProductionCatalog.h"

namespace
{
FHighmooreWorldProductionAnchor Anchor(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const EHighmooreWorldAnchorKind Kind,
    const TCHAR* SubRegion,
    const TCHAR* Source,
    const TCHAR* ProductionRead)
{
    FHighmooreWorldProductionAnchor Result;
    Result.StableId = StableId;
    Result.DisplayName = DisplayName;
    Result.Kind = Kind;
    Result.SubRegion = SubRegion;
    Result.GoverningSource = Source;
    Result.ProductionRead = ProductionRead;
    return Result;
}
}

TArray<FHighmooreWorldProductionAnchor> FHighmooreWorldProductionCatalog::BuildNamedAnchors()
{
    return {
        Anchor(TEXT("world.highmoore.castle.arion"), TEXT("Arion"), EHighmooreWorldAnchorKind::Castle, TEXT("The Folds"), TEXT("highmoore region.md Section 4.2; house arion.md"), TEXT("White and gold on rising ground, the furthest and most beautiful castle in the emergence view; it is deliberately the skyline target the eye chooses.")),
        Anchor(TEXT("world.highmoore.castle.belos"), TEXT("Belos"), EHighmooreWorldAnchorKind::Castle, TEXT("Stonecountry"), TEXT("highmoore region.md Section 4.2; house belos.md"), TEXT("Iron-grey and squat in the Stonecountry; silhouette must read differently from Arion at long range.")),
        Anchor(TEXT("world.highmoore.village.ashby-under-moor"), TEXT("Ashby Under Moor"), EHighmooreWorldAnchorKind::Village, TEXT("The Folds"), TEXT("highmoore settlements.md Section 3.2; the lesser houses.md Section 3.1"), TEXT("Most-changed-hands village; boundary stone bears eleven cuts and settlement presentation must read repeated political turnover rather than generic damage.")),
        Anchor(TEXT("world.highmoore.village.lynnwater"), TEXT("Lynnwater"), EHighmooreWorldAnchorKind::Village, TEXT("The Folds"), TEXT("highmoore settlements.md Section 3.2; the lesser houses.md Section 4.4"), TEXT("Mill village; Tam's authored regional presence belongs here.")),
        Anchor(TEXT("world.highmoore.village.halford-cross"), TEXT("Halford Cross"), EHighmooreWorldAnchorKind::Village, TEXT("The Folds"), TEXT("highmoore settlements.md Sections 3.2 and 5"), TEXT("Abbey village whose settlement read is inseparable from the region's only abbey and school.")),
        Anchor(TEXT("world.highmoore.village.stonecroft"), TEXT("Stonecroft"), EHighmooreWorldAnchorKind::Village, TEXT("Stonecountry"), TEXT("highmoore settlements.md Section 3.2; highmoore smiths.md"), TEXT("Quarrying and iron-working village; roughly half the village works iron, so production must visibly read industry rather than a generic rural hamlet.")),
        Anchor(TEXT("world.highmoore.settlements.mere-villages"), TEXT("The Mere Villages"), EHighmooreWorldAnchorKind::SettlementGroup, TEXT("Waterlands"), TEXT("highmoore settlements.md Section 3.2; the lesser houses.md Section 2.2; water physics.md"), TEXT("Pile-built settlements reached by causeway; access is physically water-dependent and passable four hours in six.")),
        Anchor(TEXT("world.highmoore.town.coldbeck"), TEXT("Coldbeck"), EHighmooreWorldAnchorKind::MarketTown, TEXT("The Folds"), TEXT("highmoore settlements.md Section 4"), TEXT("Largest of Highmoore's three real towns and the location of the Drover's Rest where the Wizard is met.")),
        Anchor(TEXT("world.highmoore.town.dunford"), TEXT("Dunford"), EHighmooreWorldAnchorKind::MarketTown, TEXT("The Folds / Stonecountry crossing"), TEXT("highmoore settlements.md Sections 4 and 7"), TEXT("The ford town at Highmoore's reliable river crossing; its graveyard is visibly too large for a forty-household village-scale community.")),
        Anchor(TEXT("world.highmoore.town.wynnes-cross"), TEXT("Wynne's Cross"), EHighmooreWorldAnchorKind::MarketTown, TEXT("The Folds"), TEXT("highmoore settlements.md Section 4.1; mounted travel.md Section 2.3"), TEXT("Horse-market town; stable and horse culture must be legible in world production.")),
        Anchor(TEXT("world.highmoore.abbey.halford-cross"), TEXT("Halford Cross Abbey"), EHighmooreWorldAnchorKind::Abbey, TEXT("The Folds"), TEXT("highmoore settlements.md Section 5"), TEXT("Highmoore's only abbey and only school; stone/high-vault acoustics, records and shelter function are authored parts of the place.")),
        Anchor(TEXT("world.highmoore.pub.drovers-rest"), TEXT("The Drover's Rest"), EHighmooreWorldAnchorKind::PublicHouse, TEXT("Coldbeck"), TEXT("highmoore settlements.md Section 4.3; wizard questline.md Section 2"), TEXT("Low-beamed dark public house with coal fire and settle benches; Huw Pennant shapes the room and the Wizard occupies the avoided corner table."))
    };
}

TArray<FHighmooreWorldDesignGap> FHighmooreWorldProductionCatalog::BuildDesignGaps()
{
    return {
        {TEXT("design-gap.highmoore.anchor-coordinates"), TEXT("The sources lock geography, subregions, silhouette/readability and travel relationships but do not provide production coordinates for these anchors. Coordinates must be authored in the reviewed Highmoore .umap, not invented in C++."), TEXT("highmoore region.md; highmoore settlements.md")},
        {TEXT("design-gap.highmoore.lesser-castle-placement"), TEXT("At least six castles must be visible from the emergence shelf, but only Arion and Belos are individually fixed in the region source. The remaining skyline castles must be selected from approved lesser-House authority during level production rather than represented by generic castles."), TEXT("highmoore region.md Section 4; the lesser houses.md")},
        {TEXT("design-gap.highmoore.runtime-assets"), TEXT("No reviewed Highmoore production .umap, settlement actor placement, castle mesh set, navmesh, lighting, crowd, weather or streaming-cell evidence exists yet."), TEXT("Docs/NEXT_ROCKSTAR_TRANCHE.md; repository asset evidence")}
    };
}

bool FHighmooreWorldProductionCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();
    const TArray<FHighmooreWorldProductionAnchor> Anchors = BuildNamedAnchors();
    if (Anchors.Num() != RequiredNamedAnchorCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Highmoore world production requires exactly %d current source-backed named anchors; found %d."), RequiredNamedAnchorCount, Anchors.Num()));
    }
    TSet<FName> Seen;
    for (const FHighmooreWorldProductionAnchor& Entry : Anchors)
    {
        if (Entry.StableId.IsNone() || Entry.DisplayName.IsEmpty() || Entry.SubRegion.IsEmpty() || Entry.GoverningSource.IsEmpty() || Entry.ProductionRead.IsEmpty()) OutErrors.Add(TEXT("Every Highmoore world anchor requires id, name, subregion, source and production read."));
        if (Seen.Contains(Entry.StableId)) OutErrors.Add(FString::Printf(TEXT("Duplicate Highmoore world anchor: %s"), *Entry.StableId.ToString()));
        Seen.Add(Entry.StableId);
        if (Entry.bRuntimePlacementAuthored || !Entry.RuntimeActorPath.IsEmpty()) OutErrors.Add(FString::Printf(TEXT("Highmoore anchor %s may not claim reviewed Unreal placement before asset evidence exists."), *Entry.StableId.ToString()));
    }
    if (MinimumVisibleCastlesFromEmergence != 6 || EmergenceSwitchbackMinimumMinutesOnFoot != 8 || EmergenceSwitchbackMaximumMinutesOnFoot != 10 || ApproximateEndToEndCanterMinutes != 25) OutErrors.Add(TEXT("Highmoore emergence/scale constants drifted from highmoore region.md."));
    if (AllowsQuestionMarkDiscoveryMarkers() || AllowsGenericCastleSubstitution()) OutErrors.Add(TEXT("Highmoore production may not gain question-mark discovery markers or generic castle substitution."));
    if (BuildDesignGaps().Num() != 3) OutErrors.Add(TEXT("Highmoore world production must retain the three current placement/asset design gaps."));
    return OutErrors.IsEmpty();
}
