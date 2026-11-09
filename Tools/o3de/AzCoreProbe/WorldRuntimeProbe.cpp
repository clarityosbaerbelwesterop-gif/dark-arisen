// Runs the Dark Arisen open-world streamer (WorldStreamerComponent) inside a real O3DE 2605.0
// AZ::ComponentApplication: the catalog and plantings are read through AzFramework::LocalFileIO from
// a small world in world-build's format, Core::WorldStreaming decides what is resident, and the
// tile and foliage events arrive over a real EBus. It proves the adapter runs on O3DE's framework;
// drawing the tiles (WorldRendererComponent, Atom) needs a full engine build.
#include <AzCore/Component/ComponentApplication.h>
#include <AzCore/Component/Entity.h>
#include <AzCore/IO/FileIO.h>
#include <AzFramework/IO/LocalFileIO.h>
#include <DarkArisen/WorldBus.h>
#include "World/WorldStreamerComponent.h"

#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace
{
    int g_failures = 0;
    void Check(const bool condition, const char* what)
    {
        std::printf("[%s] %s\n", condition ? " ok " : "FAIL", what);
        g_failures += condition ? 0 : 1;
    }

    struct Listener : DarkArisen::WorldStreamingNotificationBus::Handler
    {
        std::vector<std::string> events;
        void OnTileLoaded(const DarkArisen::WorldTile& tile) override { events.push_back("tile+" + std::to_string(tile.Index)); }
        void OnTileUnloaded(const int tile) override { events.push_back("tile-" + std::to_string(tile)); }
        void OnFoliageLoaded(const DarkArisen::WorldFoliage& foliage) override { events.push_back("plants+" + std::to_string(foliage.Tile)); }
        void OnFoliageUnloaded(const int tile) override { events.push_back("plants-" + std::to_string(tile)); }
        int Count(const std::string& prefix) const
        {
            int n = 0;
            for (const std::string& e : events) n += e.rfind(prefix, 0) == 0 ? 1 : 0;
            return n;
        }
    };

    const char* Asset(const char* uuid, unsigned subId, const char* hint, std::string& out)
    {
        out = std::string("{\"id\": \"") + uuid + "\", \"subId\": " + std::to_string(subId) + ", \"hint\": \"" + hint + "\"}";
        return out.c_str();
    }

    /** A 4 x 3 world of 500 m tiles (land at columns 0..1 of rows 0..1, the rest sea), heights rising 1 m per 10 m east. */
    void WriteWorld(const std::filesystem::path& root)
    {
        std::filesystem::create_directories(root / "assets/world/tiles");
        std::string model, material, plant;
        std::string catalog = "{\n  \"tileSize\": 500,\n  \"columns\": 4,\n  \"rows\": 3,\n  \"heightStep\": 50,\n  \"heightSamples\": 11,\n  \"tiles\": [";
        bool first = true;
        for (int row = 0; row < 2; ++row)
            for (int column = 0; column < 2; ++column)
            {
                const int index = row * 4 + column;
                const std::string suffix = std::to_string(column) + "_" + std::to_string(row);
                catalog += std::string(first ? "" : ",") + "\n    {\"tile\": " + std::to_string(index) + ", \"column\": " + std::to_string(column) + ", \"row\": " +
                           std::to_string(row) + ", \"biome\": \"tropical\", \"model\": " +
                           Asset("{11111111-2222-3333-4444-555555555555}", 268435457u + static_cast<unsigned>(index), "assets/world/tiles/t.azmodel", model) +
                           ", \"material\": " + Asset("{66666666-7777-8888-9999-AAAAAAAAAAAA}", 0, "assets/world/materials/m.azmaterial", material) +
                           ", \"foliage\": \"assets/world/tiles/f_world_" + suffix + ".json\"" +
                           (index == 0 ? std::string(", \"structures\": [[0,12000,13000,1250,90],[7,0,0,0,0]]") : std::string()) + "}";
                first = false;
                std::string foliage = "{\"tile\": " + std::to_string(index) + ", \"x0\": " + std::to_string(column * 500) + ", \"y0\": " + std::to_string(row * 500) +
                                      ", \"biome\": \"tropical\",\n\"plants\": [[0," + std::to_string(column * 50000 + 25000) + "," + std::to_string(row * 50000 + 25000) +
                                      ",1200,90,110],[1,100,100,5,0,100]],\n\"heights\": [";
                for (int j = 0; j <= 10; ++j)
                    for (int k = 0; k <= 10; ++k)
                    {
                        const double x = column * 500.0 + k * 50.0;
                        foliage += std::string(j || k ? "," : "") + std::to_string(static_cast<long long>(std::llround(x * 10.0)));  // cm: x / 10 m
                    }
                foliage += "]}\n";
                std::ofstream(root / ("assets/world/tiles/f_world_" + suffix + ".json")) << foliage;
            }
        std::string building;
        catalog += std::string("\n  ],\n  \"structures\": [\n    {\"model\": ") +
                   Asset("{ABCDEF00-1111-2222-3333-999999999999}", 268435463u, "assets/art/models/sm_art_settlement_nordic_0.azmodel", building) +
                   ", \"box\": [3.3, 6.0, 7.5]}\n  ],\n  \"paved\": [[400, 300, 50]],";
        catalog += std::string("\n  \"plants\": [\n    {\"tree\": true, \"parts\": [{\"model\": ") +
                   Asset("{ABCDEF00-1111-2222-3333-444444444444}", 268435460u, "assets/art/scanned/game/island_tree_01_0.azmodel", plant) + ", \"material\": null}]},\n" +
                   "    {\"tree\": false, \"parts\": [{\"model\": " + Asset("{ABCDEF00-1111-2222-3333-555555555555}", 268435461u, "assets/art/models/x.azmodel", plant) +
                   ", \"material\": " + Asset("{ABCDEF00-1111-2222-3333-666666666666}", 0, "assets/materials/foliage/x.azmaterial", material) + "}]}\n  ],\n" +
                   "  \"grass\": [\n    {\"card\": \"GrassBlades\", \"model\": " + Asset("{ABCDEF00-1111-2222-3333-777777777777}", 268435462u, "assets/world/grass/g.azmodel", model) +
                   ", \"material\": " + Asset("{ABCDEF00-1111-2222-3333-888888888888}", 0, "assets/world/materials/g.azmaterial", material) + "}\n  ],\n" +
                   "  \"biomes\": {\"tropical\": {\"grass\": [\"GrassBlades\"], \"density\": 0.7}}\n}\n";
        std::ofstream(root / "assets/world/worldtiles.json") << catalog;
    }
}

int main(int argc, char** argv)
{
    const std::filesystem::path products = std::filesystem::temp_directory_path() / "darkarisen-o3de-world-probe";
    std::filesystem::remove_all(products);
    WriteWorld(products);

    AZ::ComponentApplication app(argc, argv);
    AZ::ComponentApplication::Descriptor descriptor;
    AZ::ComponentApplication::StartupParameters startup;
    startup.m_loadSettingsRegistry = false;
    startup.m_loadStaticModules = false;
    startup.m_loadDynamicModules = false;
    startup.m_loadAssetCatalog = false;
    AZ::Entity* systemEntity = app.Create(descriptor, startup);
    Check(systemEntity != nullptr, "O3DE ComponentApplication created");

    AZ::IO::LocalFileIO fileIO;
    AZ::IO::FileIOBase::SetInstance(nullptr);
    AZ::IO::FileIOBase::SetInstance(&fileIO);
    fileIO.SetAlias("@products@", products.string().c_str());

    app.RegisterComponentDescriptor(DarkArisen::WorldStreamerComponent::CreateDescriptor());
    Listener listener;
    listener.BusConnect();

    auto* level = aznew AZ::Entity("L_World");
    auto* streamer = level->CreateComponent<DarkArisen::WorldStreamerComponent>();
    level->Init();
    level->Activate();
    Check(level->GetState() == AZ::Entity::State::Active, "level entity with WorldStreamerComponent active");
    auto* world = DarkArisen::WorldStreamingInterface::Get();
    Check(world != nullptr, "WorldStreamingInterface registered through AZ::Interface");
    if (!world)
    {
        return 1;
    }
    Check(world->GetColumns() == 4 && std::fabs(world->GetTileSize() - 500.0f) < 1e-3f, "catalog: 4 columns of 500 m tiles");
    Check(world->GetPlants().size() == 2 && world->GetPlants()[0].Tree && !world->GetPlants()[1].Tree, "catalog: plant table with a tree and undergrowth");
    Check(world->GetPlants()[0].Parts.size() == 1 && world->GetPlants()[0].Parts[0].Model.Id.m_subId == 268435460u &&
              !world->GetPlants()[0].Parts[0].Material.Id.IsValid() && world->GetPlants()[1].Parts[0].Material.Id.IsValid(),
        "catalog: asset ids with sub-ids; a missing material keeps the model's own");
    const DarkArisen::WorldTile* first = world->GetTile(0);
    Check(world->GetStructureModels().size() == 1 && std::fabs(world->GetStructureModels()[0].HalfY - 6.0f) < 1e-4f &&
              world->GetStructureModels()[0].Model.Id.m_subId == 268435463u,
        "catalog: building models with their collision boxes");
    Check(first && first->Structures.size() == 1 && std::fabs(first->Structures[0].X - 120.0f) < 1e-3f && std::fabs(first->Structures[0].Z - 12.5f) < 1e-3f &&
              std::fabs(first->Structures[0].Yaw - 90.0f) < 1e-3f,
        "catalog: a tile's buildings in metres and degrees; one naming a missing model is dropped");
    Check(world->IsPaved(420.0f, 310.0f) && !world->IsPaved(460.0f, 300.0f), "a town's paving is known (no grass there)");
    const DarkArisen::WorldBiomeGrass* grass = world->GetBiomeGrass("tropical");
    Check(grass && grass->Grass.size() == 1 && world->GetGrass()[grass->Grass[0]].Card == "GrassBlades" && std::fabs(grass->Density - 0.7f) < 1e-4f,
        "catalog: the biome's grass cards resolve to patches");

    // standing on the first tile: the four land tiles come in, two per tick at most for plantings, sea never
    for (int tick = 0; tick < 10; ++tick)
    {
        streamer->Step(250.0f, 250.0f);
    }
    Check(world->GetResidentTileCount() == 4, "all four land tiles resident within 1500 m; the sea tiles never load");
    Check(listener.Count("tile+") == 4 && listener.Count("plants+") == 4, "tile and planting events over WorldStreamingNotificationBus");
    Check(!listener.events.empty() && listener.events.front() == "tile+0", "the tile underfoot comes first");
    const DarkArisen::WorldFoliage* here = world->GetFoliage(0);
    Check(here && here->Plantings.size() == 2 && std::fabs(here->Plantings[0].X - 250.0f) < 1e-3f && std::fabs(here->Plantings[0].Z - 12.0f) < 1e-3f &&
              std::fabs(here->Plantings[0].Yaw - 90.0f) < 1e-3f && std::fabs(here->Plantings[0].Scale - 1.1f) < 1e-4f,
        "plantings read in metres, degrees and scale");
    float height = 0.0f;
    Check(world->GetGroundHeight(123.0f, 77.0f, height) && std::fabs(height - 12.3f) < 1e-3f, "ground height interpolated from the tile's grid");
    Check(world->GetGroundHeight(740.0f, 260.0f, height) && std::fabs(height - 74.0f) < 1e-3f, "ground height on the neighbouring tile");
    Check(!world->GetGroundHeight(1800.0f, 250.0f, height), "no ground height at open sea");

    // sailing far east: plantings leave before their ground, then the ground
    listener.events.clear();
    streamer->Step(3900.0f, 1400.0f);
    Check(listener.Count("plants-") == 4 && listener.Count("tile-") == 4, "everything behind unloads past the unload radius");
    Check(listener.events.front().rfind("plants-", 0) == 0 && listener.events.back().rfind("tile-", 0) == 0, "plants leave before the ground they stand on");
    Check(world->GetResidentTileCount() == 0 && world->GetFoliage(0) == nullptr, "nothing resident at sea");

    // back again, then the level ends: listeners are told to release everything
    for (int tick = 0; tick < 10; ++tick)
    {
        streamer->Step(250.0f, 250.0f);
    }
    listener.events.clear();
    level->Deactivate();
    Check(listener.Count("plants-") == 4 && listener.Count("tile-") == 4, "deactivation unloads every resident tile and planting");
    Check(DarkArisen::WorldStreamingInterface::Get() == nullptr, "interface unregistered on deactivation");
    delete level;

    listener.BusDisconnect();
    app.Destroy();
    std::printf("%s\n", g_failures == 0 ? "world runtime probe passed" : "world runtime probe FAILED");
    return g_failures == 0 ? 0 : 1;
}
