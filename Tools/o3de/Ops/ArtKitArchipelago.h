#pragma once

#include <cstdint>
#include <string>
#include <vector>

/** The open world's geography: ContentSource/World/Archipelago.json to a heightfield and a relief map. */
namespace DarkArisen::Tools::Art
{
    struct ArchipelagoIsland
    {
        std::string Region, Name;
        double X = 0, Y = 0, Rx = 0, Ry = 0, Rotation = 0, Peak = 0;  // metres, degrees
        std::string Shape;   // dome, ridge, volcano, low, coast
        std::string Biome;   // tropical, mire, moor, volcanic, cold, fortress, maw
    };

    struct ArchipelagoSpec
    {
        double Width = 0, Height = 0, SeaFloor = -60;
        std::uint32_t Seed = 1;
        std::vector<ArchipelagoIsland> Islands;
    };

    bool ReadArchipelago(const std::string& Json, ArchipelagoSpec& Out, std::string& Problem);

    /** Ground height in metres at world (X east, Y north); Owner is the island it belongs to, -1 at sea. */
    double ArchipelagoHeight(const ArchipelagoSpec& Spec, double X, double Y, int* Owner);

    struct ArchipelagoMaps
    {
        std::string HeightPng;   // 16-bit greyscale, rows from the north
        int HeightWidth = 0, HeightHeight = 0;
        double MinHeight = 0, MaxHeight = 0;
        std::string ReliefPng;   // shaded relief, biome colours, sea by depth
        double LandSquareKm = 0;
    };

    ArchipelagoMaps BuildArchipelagoMaps(const ArchipelagoSpec& Spec, double HeightCell, double ReliefCell);
}

namespace DarkArisen::Tools::Art
{
    /** The world's terrain as models: a coarse mesh of the whole archipelago (always resident, the horizon) and
     * detail tiles for the streamer, plus world-wide macro colour and splat textures and one DarkArisenTerrain
     * material per biome. Paths are relative to Assets/World. */
    struct WorldTerrain
    {
        struct File
        {
            std::string Path, Content;
        };
        std::vector<File> Files;
        int Tiles = 0;
        std::size_t Triangles = 0, Plantings = 0, Structures = 0;
    };

    /** A place of the story in the world (ContentSource/World/Places.json): the ground there is left clear of plants. */
    struct WorldPlaceMark
    {
        double X = 0, Y = 0, Radius = 0;
    };

    /** A location of the design (ContentSource/World/Locations.json): settlements level their ground and get buildings. */
    struct WorldLocation
    {
        std::string Id, Kind, Region;
        double X = 0, Y = 0, Radius = 0;
        /** Optional "layout" block of Locations.json (a level design's generator parameters, e.g. layout.md 4.1). */
        struct Layout
        {
            bool HasGround = false;
            double GroundZ = 0;        // the levelled ground's height instead of the natural one
            int Cap = 0;               // buildings at most (0: the kind's default)
            double Fill = -1;          // share of plots built on (negative: the kind's default)
            int Waterfront = -1;       // -1: wherever the sea is near; 0: never; 1: always
            bool Cluster = false;      // organic clusters along the contour instead of a street grid
            std::string Tradition;     // building tradition instead of the kind's (e.g. "Stilt")
            bool HasLandmark = false;
            double LandmarkU = 0, LandmarkW = 0, LandmarkYaw = 0;  // landmark offset in the street grid's frame and its yaw
            bool HasPlaza = false;
            double PlazaU = 0, PlazaW = 0, PlazaRadius = 0;        // the square kept clear
            double LampSpacing = 0;                                // street lamps, metres apart on each side
            double StallRadius = 0, StallPitch = 5;                // market stalls round the square
            int StallMax = 0;
            int Benches = 0;
        } Plan;
    };

    /**
     * Also plants the land tiles (Kit::WorldVegetation): each tile's plantings and its ground heights go to
     * Tiles/F_World_<c>_<r>.json for the runtime (trees and rocks as instances, grass patches scattered near the
     * player on the heights), the plant models and grass patches to WorldTiles.json with their O3DE asset ids.
     */
    /** A route between places (ContentSource/World/Roads.json): graded into the terrain, paved, lit, built along. */
    struct WorldRoad
    {
        std::string Id, Kind;          // Kind: "avenue", "street" or "stairs"
        double Width = 7;
        struct Point { double X = 0, Y = 0, Z = 0; };
        std::vector<Point> Points;     // x, y and the road's height z at each point
    };

    WorldTerrain BuildWorldTerrain(const ArchipelagoSpec& Spec, const std::vector<WorldPlaceMark>& Places, const std::vector<WorldLocation>& Locations,
        const std::vector<WorldRoad>& Roads, double TileSize, double TileStep, double CoarseStep, double TextureCell);
}
