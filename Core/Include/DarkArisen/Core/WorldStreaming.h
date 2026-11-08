#pragma once

#include <cstdint>
#include <string>
#include <vector>

/**
 * Which parts of the open world are resident around the player (engine-neutral).
 *
 * The archipelago (ContentSource/World/Archipelago.json) is cut into square terrain tiles; places (towns,
 * forts, mission set pieces) are streamed by their own radius. Each update asks for what should be resident:
 * everything inside the load radius, nearest first, at most a budget of new loads per update so a frame never
 * stalls. Unloading waits until something is beyond the unload radius (larger than the load radius), so walking
 * along a border does not make tiles flicker in and out. The engine adapter spawns and despawns what it is told.
 */
namespace DarkArisen::Core
{
    struct StreamedPlace
    {
        std::string Id;
        double X = 0.0, Y = 0.0;      // metres, X east, Y north
        double Radius = 300.0;        // resident while the player is within this distance (plus hysteresis to leave)
    };

    struct StreamingSettings
    {
        double WorldWidth = 14000.0, WorldHeight = 12000.0;
        double TileSize = 500.0;
        double LoadRadius = 1500.0;   // tiles whose nearest point is closer than this come in
        double UnloadRadius = 1900.0; // and leave only beyond this
        int LoadsPerUpdate = 4;       // new tiles or places started per update
    };

    struct StreamingChange
    {
        std::vector<int> LoadTiles;          // tile index = row * columns + column, nearest first
        std::vector<int> UnloadTiles;
        std::vector<std::string> LoadPlaces;
        std::vector<std::string> UnloadPlaces;
    };

    class WorldStreaming
    {
    public:
        explicit WorldStreaming(StreamingSettings Settings, std::vector<StreamedPlace> Places = {});

        int Columns() const { return ColumnCount; }
        int Rows() const { return RowCount; }
        /** Tiles that hold land (the rest is open sea, drawn by the ocean): only these ever load. */
        void SetLandTiles(const std::vector<int>& Tiles);

        StreamingChange Update(double PlayerX, double PlayerY);

        bool IsTileResident(int Tile) const;
        bool IsPlaceResident(const std::string& Id) const;
        std::size_t ResidentTileCount() const;

        /** Squared distance from a point to a tile's nearest point (0 inside). */
        double TileDistanceSquared(int Tile, double X, double Y) const;

    private:
        StreamingSettings Settings;
        std::vector<StreamedPlace> Places;
        int ColumnCount = 0, RowCount = 0;
        std::vector<std::uint8_t> Land;      // per tile: 1 when it holds land
        std::vector<std::uint8_t> Resident;  // per tile
        std::vector<std::uint8_t> PlaceResident;
    };
}
