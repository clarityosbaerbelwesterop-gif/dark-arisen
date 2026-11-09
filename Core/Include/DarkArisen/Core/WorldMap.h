#pragma once

#include <cstdint>
#include <functional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

/**
 * The world map and the minimap. The owner's Alpha navigation override (Docs/DesignAuthority.md, 2026-09-19) requires
 * both and forbids every objective aid: no quest marker, route line, enemy, collectible or objective arrow ever reaches
 * them, so this model has no way to hold one. What it holds follows docs/design/mechanics/ship_navigation.md:
 * - the map reveals itself as Jake travels (1.4) and through vistas that "reveal a large minimap area" (the Faro,
 *   region_02_rexa_city.md 2.6; the Great Tree, region_01_moran.md; the Great Terraces, region_03_ashenmoor_settlements.md);
 * - known places are added when seen or when someone tells Jake of them (spoken directions), and stay (12.4);
 * - the current position, exact on land and under a clear sky, estimated by dead reckoning at sea when the sky is
 *   overcast (6.3: "errors possible"), corrected at the next fix (a landmark sighted, the sky clearing, landfall);
 * - the share of each region explored (12.4).
 *
 * Reveal radii, the cell size and the dead-reckoning drift are tuning values, not design numbers.
 */
namespace DarkArisen::Core
{
    /** What the map may show besides the revealed ground and Jake. Nothing else exists to be shown. */
    enum class MapMarkKind : std::uint8_t { Settlement, Landmark, Harbour, Dungeon, Vista, Road };

    struct MapPlace
    {
        std::string Id;
        MapMarkKind Kind = MapMarkKind::Landmark;
        double X = 0, Y = 0;
        double SeenFrom = 300.0;  // metres: within this range Jake sees it and it goes on the map
    };

    struct MapSettings
    {
        double Width = 12000, Height = 10000;  // the archipelago (Docs/ALPHA_PLAN_O3DE_2026_11.md)
        double Cell = 50;                       // fog-of-war resolution
        double RevealOnLand = 120;              // metres around Jake on foot
        double RevealAtSea = 600;               // from the deck the horizon is far
        double RevealFromVista = 1500;          // the top of a vista
        double MinimapRadius = 150;             // the minimap shows this far around Jake
        double DriftPerMetre = 0.03;            // dead-reckoning error grows with distance sailed under overcast
    };

    class WorldMap
    {
    public:
        using RegionAt = std::function<std::string(double X, double Y)>;

        WorldMap(MapSettings Settings, std::vector<MapPlace> Places, RegionAt Region);

        /** Jake's position this frame; at sea under an overcast sky the map only estimates it. */
        void Travel(double X, double Y, bool AtSea, bool Overcast);
        /** At the top of a vista. */
        void ReachVista(double X, double Y);
        /** A place named in conversation (spoken directions): known, even unseen. */
        bool Learn(std::string_view PlaceId);

        bool IsRevealed(double X, double Y) const;
        bool Knows(std::string_view PlaceId) const { return Known.count(std::string(PlaceId)) != 0; }
        const std::set<std::string>& KnownPlaces() const { return Known; }
        /** The known places within the minimap's reach of Jake. */
        std::vector<const MapPlace*> MinimapPlaces() const;
        /** Share (0..1) of a region's cells revealed. */
        double Explored(std::string_view RegionId) const;

        double MarkedX() const { return EstimateX; }
        double MarkedY() const { return EstimateY; }
        /** Radius of doubt round the marked position (0 when Jake's position is known). */
        double Uncertainty() const { return Doubt; }

        /** The revealed cells, packed eight to a byte, for the save. */
        std::vector<std::uint8_t> SaveRevealed() const;
        bool LoadRevealed(const std::vector<std::uint8_t>& Bytes);

    private:
        void Reveal(double X, double Y, double Radius);
        int Columns() const;
        int Rows() const;

        MapSettings Settings;
        std::vector<MapPlace> Places;
        RegionAt Region;
        std::vector<bool> Revealed;
        std::set<std::string> Known;
        double TrueX = 0, TrueY = 0, EstimateX = 0, EstimateY = 0, Doubt = 0;
        bool Started = false;
    };
}
