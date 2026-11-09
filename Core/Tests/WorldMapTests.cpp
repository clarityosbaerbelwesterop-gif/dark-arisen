#include "TestHarness.h"

#include "DarkArisen/Core/WorldMap.h"

#include <string>

using namespace DarkArisen::Core;

namespace
{
    WorldMap Archipelago()
    {
        MapSettings Settings;
        Settings.Width = 2000;
        Settings.Height = 1000;
        Settings.RevealFromVista = 800;
        std::vector<MapPlace> Places = {
            {"loc.test.harbour", MapMarkKind::Harbour, 200, 200, 300},
            {"loc.test.faro", MapMarkKind::Vista, 900, 500, 400},
            {"loc.test.far_cove", MapMarkKind::Landmark, 1800, 900, 200},
        };
        // west half one region, east half another
        return WorldMap(Settings, Places, [](double X, double) { return std::string(X < 1000 ? "West" : "East"); });
    }
}

TEST_CASE("Map: the ground reveals itself around Jake, farther from the deck (ship_navigation.md 1.4)")
{
    WorldMap Map = Archipelago();
    CHECK(!Map.IsRevealed(210, 210));
    Map.Travel(200, 200, false, false);
    CHECK(Map.IsRevealed(210, 210));
    CHECK(!Map.IsRevealed(500, 200));
    Map.Travel(200, 600, true, false);
    CHECK(Map.IsRevealed(700, 600));
}

TEST_CASE("Map: a vista reveals a large area and the places in sight (region_02_rexa_city.md 2.6)")
{
    WorldMap Map = Archipelago();
    Map.ReachVista(900, 500);
    CHECK(Map.IsRevealed(1600, 500));
    CHECK(!Map.IsRevealed(1800, 900));
    CHECK(Map.Knows("loc.test.faro"));
    CHECK(Map.Knows("loc.test.harbour"));
    CHECK(!Map.Knows("loc.test.far_cove"));
    CHECK(Map.Explored("West") > 0.6);
}

TEST_CASE("Map: a place named in conversation is known unseen (spoken directions)")
{
    WorldMap Map = Archipelago();
    CHECK(Map.Learn("loc.test.far_cove"));
    CHECK(Map.Knows("loc.test.far_cove"));
    CHECK(!Map.IsRevealed(1800, 900));
    CHECK(!Map.Learn("loc.test.unknown"));
}

TEST_CASE("Map: under an overcast sky at sea the position is only estimated, until a fix (6.3)")
{
    WorldMap Map = Archipelago();
    Map.Travel(100, 500, true, false);
    CHECK(Map.Uncertainty() == 0.0);
    Map.Travel(600, 500, true, true);
    CHECK(Map.Uncertainty() > 10.0);
    const double Grown = Map.Uncertainty();
    Map.Travel(900, 500, true, true);
    CHECK(Map.Uncertainty() > Grown);
    Map.Travel(950, 500, true, false);
    CHECK(Map.Uncertainty() == 0.0);
}

TEST_CASE("Map: the minimap shows only known places near Jake; nothing else can be put on it")
{
    WorldMap Map = Archipelago();
    Map.Travel(200, 220, false, false);
    const auto Near = Map.MinimapPlaces();
    CHECK(Near.size() == 1);
    CHECK(Near.size() == 1 && Near[0]->Id == "loc.test.harbour");
    // the kinds of mark are the places themselves: no objective, enemy or collectible kind exists
    CHECK(static_cast<int>(MapMarkKind::Road) == 5);
}

TEST_CASE("Map: exploration per region and the revealed cells survive a save")
{
    WorldMap Map = Archipelago();
    Map.Travel(1500, 500, true, false);
    CHECK(Map.Explored("East") > 0.2);
    CHECK(Map.Explored("West") < 0.05);
    const auto Bytes = Map.SaveRevealed();
    WorldMap Loaded = Archipelago();
    CHECK(Loaded.LoadRevealed(Bytes));
    CHECK(Loaded.IsRevealed(1500, 500));
    CHECK(!Loaded.LoadRevealed({1, 2, 3}));
}
