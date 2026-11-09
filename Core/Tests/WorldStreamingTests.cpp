#include "TestHarness.h"

#include "DarkArisen/Core/WorldStreaming.h"

using namespace DarkArisen::Core;

TEST_CASE("WorldStreaming: the archipelago cuts into 28 x 24 tiles of 500 m")
{
    const WorldStreaming World(StreamingSettings{});
    CHECK(World.Columns() == 28);
    CHECK(World.Rows() == 24);
    CHECK_NEAR(World.TileDistanceSquared(0, 250.0, 250.0), 0.0, 1e-12);       // inside
    CHECK_NEAR(World.TileDistanceSquared(0, 800.0, 250.0), 300.0 * 300.0, 1e-9);
}

TEST_CASE("WorldStreaming: nearest tiles first, within the per-update budget, until the radius is full")
{
    WorldStreaming World(StreamingSettings{});
    const StreamingChange First = World.Update(7000.0, 6000.0);
    CHECK(First.LoadTiles.size() == 4);  // budget
    CHECK(World.TileDistanceSquared(First.LoadTiles.front(), 7000.0, 6000.0) <= World.TileDistanceSquared(First.LoadTiles.back(), 7000.0, 6000.0));
    std::size_t Total = First.LoadTiles.size();
    for (int K = 0; K < 100; ++K) Total += World.Update(7000.0, 6000.0).LoadTiles.size();
    // every tile whose nearest point is within 1500 m: a 1500 m disc grown by the tile size
    CHECK(Total == World.ResidentTileCount());
    CHECK(Total > 30 && Total < 60);
    CHECK(World.Update(7000.0, 6000.0).LoadTiles.empty());  // settled
}

TEST_CASE("WorldStreaming: hysteresis keeps tiles while walking along a border")
{
    WorldStreaming World(StreamingSettings{});
    for (int K = 0; K < 100; ++K) World.Update(7000.0, 6000.0);
    const std::size_t Before = World.ResidentTileCount();
    // a few hundred metres back and forth unloads nothing
    for (int K = 0; K < 20; ++K)
    {
        const StreamingChange C = World.Update(7000.0 + (K % 2 ? 300.0 : -300.0), 6000.0);
        CHECK(C.UnloadTiles.empty());
    }
    CHECK(World.ResidentTileCount() >= Before);
    // a long way off: everything behind leaves
    const StreamingChange Far = World.Update(1000.0, 1000.0);
    CHECK(!Far.UnloadTiles.empty());
}

TEST_CASE("WorldStreaming: open sea never loads; places stream by their own radius")
{
    WorldStreaming World(StreamingSettings{}, {{"Rexa Harbor", 5600.0, 5200.0, 400.0}, {"Fort Carrion", 1700.0, 7700.0, 600.0}});
    World.SetLandTiles({World.Columns() * 10 + 11});  // a single land tile at (5500..6000, 5000..5500)
    for (int K = 0; K < 50; ++K) World.Update(5600.0, 5300.0);
    CHECK(World.ResidentTileCount() == 1);
    CHECK(World.IsPlaceResident("Rexa Harbor"));
    CHECK(!World.IsPlaceResident("Fort Carrion"));
    // leaving the harbour: it stays until beyond its radius plus the hysteresis (400 + 400 m)
    World.Update(5600.0, 5900.0);
    CHECK(World.IsPlaceResident("Rexa Harbor"));
    const StreamingChange Gone = World.Update(5600.0, 6300.0);
    CHECK(Gone.UnloadPlaces.size() == 1);
    CHECK(!World.IsPlaceResident("Rexa Harbor"));
}
