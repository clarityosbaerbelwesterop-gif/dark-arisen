// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Production/ShipVisualProductionCatalog.h"

namespace
{
FShipVisualProductionBrief ShipBrief(
    const TCHAR* StableId,
    const TCHAR* DisplayName,
    const EShipVisualProductionKind Kind,
    const TCHAR* Source,
    const TCHAR* VisualFacts,
    const TCHAR* FunctionalObjects,
    const TCHAR* Boundary,
    const bool bReady)
{
    FShipVisualProductionBrief Result;
    Result.StableId = StableId;
    Result.DisplayName = DisplayName;
    Result.Kind = Kind;
    Result.GoverningSource = Source;
    Result.AuthoredVisualFacts = VisualFacts;
    Result.FunctionalObjects = FunctionalObjects;
    Result.AntiInventionBoundary = Boundary;
    Result.bProviderReferenceReady = bReady;
    return Result;
}
}

TArray<FShipVisualProductionBrief> FShipVisualProductionCatalog::BuildLaLiberacionBriefs()
{
    return {
        ShipBrief(
            TEXT("ship-visual.la-liberacion.exterior"),
            TEXT("La Liberacion — Exterior Hull"),
            EShipVisualProductionKind::ExteriorHull,
            TEXT("docs/design/mechanics/ship_management.md; docs/design/physics/ship_physics.md; docs/design/mechanics/ship_interior.md"),
            TEXT("Age-of-sail living vessel with an open weather deck, helm at stern, main deck work area, foredeck, crow's nest, broadside cannon positions and a visible figurehead. Base battery is exactly twelve cannons, six per side. The ship must visibly support persistent damage, cargo-dependent waterline and later authored upgrade states."),
            TEXT("Helm, rigging, sails, broadside battery, boarding deck, cargo access and figurehead location must exist as functional visual zones."),
            TEXT("DESIGN-GAP: the governing sources do not lock the final hull class, exact length, beam, mast count, sail plan, exterior color scheme, figurehead identity or ornament package. Do not let a provider choose those canonical facts."),
            false),
        ShipBrief(
            TEXT("ship-visual.la-liberacion.weather-deck"),
            TEXT("La Liberacion — Weather Deck"),
            EShipVisualProductionKind::WeatherDeck,
            TEXT("docs/design/mechanics/ship_interior.md Section 2.1"),
            TEXT("Open-air working deck divided into stern helm, central main deck, foredeck and accessible crow's nest. The deck should feel used rather than ceremonial: practical timber surfaces, work traffic and ocean exposure."),
            TEXT("Ship wheel and navigation tools at the stern; rope coils, barrels and crates in the central work area; cannons along both sides; lookout/foredeck work zone; rigging climb to crow's nest; spyglass at crow's nest."),
            TEXT("Do not add decorative fantasy machinery, quest markers, minimap devices or non-authored deck rooms. Exact timber species, paint, rigging ornament and figurehead appearance remain unresolved."),
            true),
        ShipBrief(
            TEXT("ship-visual.la-liberacion.main-interior"),
            TEXT("La Liberacion — Main Deck Interior"),
            EShipVisualProductionKind::MainDeckInterior,
            TEXT("docs/design/mechanics/ship_interior.md Section 2.2"),
            TEXT("Interior immediately below the weather deck with a substantial but practical Age-of-Sail wood construction. Spaces are functional and lived-in rather than luxury-showcase rooms."),
            TEXT("Stern port captain's cabin; stern starboard officer's mess with long table seating eight; center-port navigation room with charts, astrolabes and sextants; center-starboard armory with weapon/boarding/ammunition storage and Big Tom's adjacent work area."),
            TEXT("Do not invent extra cabins, secret passages, mission UI surfaces or modern navigation equipment. Final corridor widths, joinery language and decoration are not locked here."),
            true),
        ShipBrief(
            TEXT("ship-visual.la-liberacion.lower-deck"),
            TEXT("La Liberacion — Lower Crew Deck"),
            EShipVisualProductionKind::LowerDeck,
            TEXT("docs/design/mechanics/ship_interior.md Section 2.3"),
            TEXT("Dim communal crew level that should communicate constant habitation and limited shipboard space rather than a clean museum vessel."),
            TEXT("Bow crew quarters with twelve hammocks and personal chests; center galley; stern wardroom for senior crew/card games; small port-side chapel with altar and candles."),
            TEXT("Do not enlarge the chapel into a cathedral, invent private luxury suites or add unrelated crew identities. Lighting fixtures and decorative motifs remain source-bounded rather than provider-authored canon."),
            true),
        ShipBrief(
            TEXT("ship-visual.la-liberacion.cargo-workshop"),
            TEXT("La Liberacion — Cargo Hold & Workshop"),
            EShipVisualProductionKind::CargoWorkshop,
            TEXT("docs/design/mechanics/ship_interior.md Section 2.4; docs/design/mechanics/ship_management.md"),
            TEXT("Lowest working/storage layer with visible load discipline and a practical separation between bulk cargo, weapons stores and active craft work. It should visually support weight, flooding risk and persistent maintenance."),
            TEXT("Bulk food/water/supply/trade storage; isolated gunpowder magazine; cannonball stores; spare weapons; Big Tom forge/work area; Ines alchemy station; Father Salvio ritual workspace."),
            TEXT("Do not create impossible full-size industrial workshops, exposed powder beside open flame or fantasy alchemy machinery. Exact compartment dimensions and upgrade-state layouts remain unresolved."),
            true),
        ShipBrief(
            TEXT("ship-visual.la-liberacion.captains-cabin"),
            TEXT("La Liberacion — Jake's Captain Cabin"),
            EShipVisualProductionKind::CaptainsCabin,
            TEXT("docs/design/mechanics/ship_interior.md Section 3; Docs/DesignAuthority.md"),
            TEXT("Private 6 m x 4 m stern cabin, wood-paneled and substantially larger than ordinary crew spaces, designed as Jake's quiet home/memory space rather than a throne room."),
            TEXT("Bed beneath large paned stern windows; substantial brass-fitted wooden desk on port wall; central map table; starboard bookshelf; wardrobe; display shelves; journal/writing materials and father's old compass on desk."),
            TEXT("Higher authority forbids quest markers/objective UI. Therefore the older map-table language about highlighted destinations, pins as UI and strategic marker overlays must not appear in provider briefs. Trophy identities are only used when separately grounded by current reward authority."),
            true)
    };
}

TArray<FShipVisualProductionDesignGap> FShipVisualProductionCatalog::BuildDesignGaps()
{
    return {
        {
            TEXT("design-gap.ship-visual.final-exterior-silhouette"),
            TEXT("La Liberacion has a detailed functional deck/interior structure but the final exterior hull class, dimensions, mast/sail plan, color language and figurehead identity are not locked by the current governing sources. Exterior provider generation remains blocked until those facts are authored or an approved reference exists."),
            TEXT("docs/design/mechanics/ship_interior.md; docs/design/mechanics/ship_management.md; docs/design/physics/ship_physics.md")
        },
        {
            TEXT("design-gap.ship-visual.upgrade-state-art"),
            TEXT("Ship-management sources define upgrade categories and gameplay trade-offs, but they do not lock final visual packages for each hull/sail/cannon/capacity/navigation/luxury tier. Do not invent canonical upgrade silhouettes."),
            TEXT("docs/design/mechanics/ship_management.md Section 2")
        }
    };
}

bool FShipVisualProductionCatalog::Validate(TArray<FString>& OutErrors)
{
    OutErrors.Reset();
    const TArray<FShipVisualProductionBrief> Briefs = BuildLaLiberacionBriefs();

    if (Briefs.Num() != RequiredBriefCount)
    {
        OutErrors.Add(FString::Printf(TEXT("Ship visual catalog requires exactly %d briefs; found %d."), RequiredBriefCount, Briefs.Num()));
    }

    int32 Ready = 0;
    int32 Blocked = 0;
    TSet<FName> Seen;
    for (const FShipVisualProductionBrief& Brief : Briefs)
    {
        if (Brief.StableId.IsNone()
            || Brief.DisplayName.IsEmpty()
            || Brief.GoverningSource.IsEmpty()
            || Brief.AuthoredVisualFacts.IsEmpty()
            || Brief.FunctionalObjects.IsEmpty()
            || Brief.AntiInventionBoundary.IsEmpty())
        {
            OutErrors.Add(TEXT("Every ship visual brief requires identity, source, visual facts, functional objects and anti-invention boundary."));
        }
        if (Seen.Contains(Brief.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate ship visual id: %s"), *Brief.StableId.ToString()));
        }
        Seen.Add(Brief.StableId);

        if (Brief.bProviderReferenceReady) ++Ready;
        else ++Blocked;
    }

    if (Ready != ProviderReadyBriefCount || Blocked != ProviderBlockedBriefCount)
    {
        OutErrors.Add(FString::Printf(
            TEXT("Ship visual provider boundary requires %d ready and %d blocked briefs; found %d ready and %d blocked."),
            ProviderReadyBriefCount,
            ProviderBlockedBriefCount,
            Ready,
            Blocked));
    }

    const FShipVisualProductionBrief* Exterior = Briefs.FindByPredicate([](const FShipVisualProductionBrief& Brief)
    {
        return Brief.Kind == EShipVisualProductionKind::ExteriorHull;
    });
    if (Exterior == nullptr || Exterior->bProviderReferenceReady || !Exterior->AntiInventionBoundary.Contains(TEXT("DESIGN-GAP")))
    {
        OutErrors.Add(TEXT("La Liberacion exterior must remain provider-blocked behind an explicit silhouette design gap."));
    }

    if (BuildDesignGaps().Num() != 2)
    {
        OutErrors.Add(TEXT("Ship visual production must preserve exterior-silhouette and upgrade-art design gaps."));
    }

    return OutErrors.IsEmpty();
}
