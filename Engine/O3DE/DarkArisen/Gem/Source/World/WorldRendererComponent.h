#pragma once

#include <DarkArisen/WorldBus.h>

#include <Atom/Feature/Mesh/MeshFeatureProcessorInterface.h>
#include <Atom/RPI.Public/Material/Material.h>
#include <Atom/RPI.Reflect/Model/ModelAsset.h>
#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/std/containers/unordered_map.h>
#include <AzCore/std/containers/vector.h>

namespace DarkArisen
{
    /**
     * Draws what WorldStreamerComponent makes resident, straight through Atom's mesh feature processor (no
     * entity per planting): each terrain tile with its biome's DarkArisenTerrain material, each planting's
     * parts with their wind materials, and grass patches scattered on the ground heights in 16 m cells around
     * the player. Identical meshes instance on the GPU; undergrowth is culled earlier than trees.
     */
    class WorldRendererComponent
        : public AZ::Component
        , protected WorldStreamingNotificationBus::Handler
        , protected AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(WorldRendererComponent);

        WorldRendererComponent() = default;
        // mesh handles are move-only: the component is never copied (reflection must not offer a copy)
        WorldRendererComponent(const WorldRendererComponent&) = delete;
        WorldRendererComponent& operator=(const WorldRendererComponent&) = delete;

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

    protected:
        void Activate() override;
        void Deactivate() override;

        // WorldStreamingNotifications
        void OnTileLoaded(const WorldTile& tile) override;
        void OnTileUnloaded(int tile) override;
        void OnFoliageLoaded(const WorldFoliage& foliage) override;
        void OnFoliageUnloaded(int tile) override;

        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

    private:
        using MeshHandle = AZ::Render::MeshFeatureProcessorInterface::MeshHandle;

        AZ::Data::Asset<AZ::RPI::ModelAsset> Model(const WorldAsset& asset);
        AZ::Data::Instance<AZ::RPI::Material> Material(const WorldAsset& asset);
        MeshHandle Acquire(const WorldAsset& model, const WorldAsset& material, const AZ::Transform& transform, bool small);
        void BuildGrassCell(int cellX, int cellY, AZStd::vector<MeshHandle>& out);
        void ReleaseAll(AZStd::vector<MeshHandle>& handles);

        // configuration
        float m_grassRadius = 80.0f;            // grass patches around the player (m)
        float m_undergrowthCoverage = 1.0f / 300.0f;  // undergrowth is culled below this share of the screen

        AZ::Render::MeshFeatureProcessorInterface* m_meshes = nullptr;
        AZStd::unordered_map<AZ::Data::AssetId, AZ::Data::Asset<AZ::RPI::ModelAsset>> m_models;
        AZStd::unordered_map<AZ::Data::AssetId, AZ::Data::Instance<AZ::RPI::Material>> m_materials;
        AZStd::unordered_map<int, MeshHandle> m_tiles;
        AZStd::unordered_map<int, AZStd::vector<MeshHandle>> m_plants;
        AZStd::unordered_map<AZ::u64, AZStd::vector<MeshHandle>> m_grass;
        float m_grassClock = 0.0f;
    };
}
