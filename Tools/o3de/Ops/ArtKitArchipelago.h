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
        std::size_t Triangles = 0;
    };

    WorldTerrain BuildWorldTerrain(const ArchipelagoSpec& Spec, double TileSize, double TileStep, double CoarseStep, double TextureCell);
}
