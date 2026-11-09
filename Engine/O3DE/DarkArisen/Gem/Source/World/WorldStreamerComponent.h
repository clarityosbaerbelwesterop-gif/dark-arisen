#pragma once

#include <DarkArisen/WorldBus.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/EntityId.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/std/containers/unordered_map.h>
#include <AzCore/std/smart_ptr/unique_ptr.h>

#include <DarkArisen/Core/WorldStreaming.h>

namespace DarkArisen
{
    /**
     * Level entity of the open world (L_World): reads world-build's catalog, keeps Core::WorldStreaming
     * centred on the followed entity (Jake, or the ship he sails) and announces what comes and goes. Terrain
     * tiles stream within the load radius; plantings and ground heights within the smaller foliage radius.
     * Each tick starts at most a budget of loads, nearest first, so a frame never stalls.
     */
    class WorldStreamerComponent
        : public AZ::Component
        , public WorldStreamingRequests
        , protected AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(WorldStreamerComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);

        // WorldStreamingRequests
        void SetFocus(float x, float y) override;
        void GetFocus(float& x, float& y) const override;
        bool IsTileResident(int tile) const override;
        AZStd::size_t GetResidentTileCount() const override;
        const WorldTile* GetTile(int tile) const override;
        const WorldFoliage* GetFoliage(int tile) const override;
        const AZStd::vector<WorldPlant>& GetPlants() const override { return m_plants; }
        const AZStd::vector<WorldGrass>& GetGrass() const override { return m_grass; }
        const WorldBiomeGrass* GetBiomeGrass(const AZStd::string& biome) const override;
        bool GetGroundHeight(float x, float y, float& height) const override;
        float GetTileSize() const override { return m_tileSize; }
        int GetColumns() const override { return m_columns; }

        /** One streaming step at a point (what OnTick does at the followed entity); public for tests. */
        void Step(float x, float y);

    protected:
        void Activate() override;
        void Deactivate() override;
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

    private:
        bool LoadCatalog();
        bool LoadFoliage(const WorldTile& tile, WorldFoliage& out) const;

        // configuration
        AZStd::string m_catalog = "@products@/assets/world/worldtiles.json";
        AZ::EntityId m_follow;
        float m_loadRadius = 1500.0f;
        float m_unloadRadius = 1900.0f;
        float m_foliageRadius = 700.0f;
        int m_loadsPerTick = 4;

        // catalog
        float m_tileSize = 500.0f;
        float m_heightStep = 5.0f;
        int m_heightSamples = 101;
        int m_columns = 0, m_rows = 0;
        AZStd::vector<WorldTile> m_tiles;
        AZStd::unordered_map<int, AZStd::size_t> m_tileByIndex;
        AZStd::vector<WorldPlant> m_plants;
        AZStd::vector<WorldGrass> m_grass;
        AZStd::unordered_map<AZStd::string, WorldBiomeGrass> m_biomes;

        // streaming state
        AZStd::unique_ptr<Core::WorldStreaming> m_terrain;
        AZStd::unique_ptr<Core::WorldStreaming> m_foliageStreaming;
        AZStd::unordered_map<int, WorldFoliage> m_foliage;
        float m_focusX = 0.0f, m_focusY = 0.0f;
    };
}
