#include "WorldRendererComponent.h"

#include <Atom/RPI.Public/Scene.h>
#include <Atom/RPI.Reflect/Asset/AssetUtils.h>
#include <AzCore/Asset/AssetManager.h>
#include <AzCore/Math/MathUtils.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>

#include <cmath>
#include <cstdlib>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(WorldRendererComponent, "WorldRendererComponent", WorldRendererComponentTypeId);

    namespace
    {
        constexpr float GrassCell = 16.0f;  // metres; 8 x 8 patches of 2 m
        constexpr int GrassPerSide = 8;

        AZ::u64 CellKey(const int x, const int y)
        {
            return (static_cast<AZ::u64>(static_cast<AZ::u32>(x)) << 32) | static_cast<AZ::u32>(y);
        }

        /** Deterministic per patch, so a cell rebuilt later grows the same grass. */
        float Hash01(AZ::u32 h)
        {
            h ^= h >> 16;
            h *= 0x7feb352du;
            h ^= h >> 15;
            h *= 0x846ca68bu;
            h ^= h >> 16;
            return static_cast<float>(h & 0xFFFFFFu) / 16777216.0f;
        }
    }

    void WorldRendererComponent::Reflect(AZ::ReflectContext* context)
    {
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }
        serializeContext->Class<WorldRendererComponent, AZ::Component>()
            ->Version(1)
            ->Field("GrassRadius", &WorldRendererComponent::m_grassRadius)
            ->Field("UndergrowthCoverage", &WorldRendererComponent::m_undergrowthCoverage);
        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
        {
            editContext->Class<WorldRendererComponent>("Dark Arisen World Renderer", "Draws the streamed open world: terrain tiles, plantings and grass.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Level"))
                ->DataElement(AZ::Edit::UIHandlers::Default, &WorldRendererComponent::m_grassRadius, "Grass Radius", "Grass patches around the player (m)")
                ->DataElement(AZ::Edit::UIHandlers::Default, &WorldRendererComponent::m_undergrowthCoverage, "Undergrowth Coverage",
                    "Undergrowth is culled below this share of the screen");
        }
    }

    void WorldRendererComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("DarkArisenWorldRendererService"));
    }

    void WorldRendererComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("DarkArisenWorldStreamingService"));
    }

    void WorldRendererComponent::Activate()
    {
        m_meshes = AZ::RPI::Scene::GetFeatureProcessorForEntity<AZ::Render::MeshFeatureProcessorInterface>(GetEntityId());
        AZ_Error("WorldRenderer", m_meshes, "No mesh feature processor in the entity's scene: the open world is not drawn.");
        if (!m_meshes)
        {
            return;
        }
        // materials are few (seven biomes, the plants' wind materials, four grass cards): load them up front, not mid-walk
        if (const WorldStreamingRequests* world = WorldStreamingInterface::Get())
        {
            for (const WorldPlant& plant : world->GetPlants())
            {
                for (const WorldPlant::Part& part : plant.Parts)
                {
                    Material(part.Material);
                }
            }
            for (const WorldGrass& grass : world->GetGrass())
            {
                Material(grass.Material);
            }
        }
        WorldStreamingNotificationBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
    }

    void WorldRendererComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        WorldStreamingNotificationBus::Handler::BusDisconnect();
        if (m_meshes)
        {
            for (auto& [index, handle] : m_tiles)
            {
                m_meshes->ReleaseMesh(handle);
            }
            for (auto& [index, handles] : m_plants)
            {
                ReleaseAll(handles);
            }
            for (auto& [index, handles] : m_structures)
            {
                ReleaseAll(handles);
            }
            for (auto& [key, handles] : m_grass)
            {
                ReleaseAll(handles);
            }
        }
        m_tiles.clear();
        m_structures.clear();
        m_plants.clear();
        m_grass.clear();
        m_models.clear();
        m_materials.clear();
        m_meshes = nullptr;
    }

    AZ::Data::Asset<AZ::RPI::ModelAsset> WorldRendererComponent::Model(const WorldAsset& asset)
    {
        auto found = m_models.find(asset.Id);
        if (found == m_models.end())
        {
            // queued: the feature processor draws the mesh once its model has loaded
            auto model = AZ::Data::AssetManager::Instance().GetAsset<AZ::RPI::ModelAsset>(asset.Id, AZ::Data::AssetLoadBehavior::QueueLoad);
            found = m_models.emplace(asset.Id, AZStd::move(model)).first;
        }
        return found->second;
    }

    AZ::Data::Instance<AZ::RPI::Material> WorldRendererComponent::Material(const WorldAsset& asset)
    {
        if (!asset.Id.IsValid())
        {
            return {};
        }
        auto found = m_materials.find(asset.Id);
        if (found == m_materials.end())
        {
            AZ::Data::Instance<AZ::RPI::Material> instance;
            auto material = AZ::RPI::AssetUtils::LoadAssetById<AZ::RPI::MaterialAsset>(asset.Id, AZ::RPI::AssetUtils::TraceLevel::Warning);
            if (material.IsReady())
            {
                instance = AZ::RPI::Material::FindOrCreate(material);
            }
            found = m_materials.emplace(asset.Id, instance).first;
        }
        return found->second;
    }

    WorldRendererComponent::MeshHandle WorldRendererComponent::Acquire(
        const WorldAsset& model, const WorldAsset& material, const AZ::Transform& transform, const bool small)
    {
        AZ::Render::MeshHandleDescriptor descriptor(Model(model));
        if (const auto instance = Material(material))
        {
            descriptor.m_customMaterials[AZ::Render::DefaultCustomMaterialId] = AZ::Render::CustomMaterialInfo{ instance, {} };
        }
        descriptor.m_isRayTracingEnabled = !small;
        MeshHandle handle = m_meshes->AcquireMesh(descriptor);
        m_meshes->SetTransform(handle, transform);
        if (small)
        {
            AZ::RPI::Cullable::LodConfiguration lods;
            lods.m_minimumScreenCoverage = m_undergrowthCoverage;
            m_meshes->SetMeshLodConfiguration(handle, lods);
        }
        return handle;
    }

    void WorldRendererComponent::ReleaseAll(AZStd::vector<MeshHandle>& handles)
    {
        for (MeshHandle& handle : handles)
        {
            m_meshes->ReleaseMesh(handle);
        }
        handles.clear();
    }

    void WorldRendererComponent::OnTileLoaded(const WorldTile& tile)
    {
        if (m_tiles.find(tile.Index) != m_tiles.end())
        {
            return;
        }
        // tile models are authored in world metres: identity transform
        m_tiles[tile.Index] = Acquire(tile.Model, tile.Material, AZ::Transform::CreateIdentity(), false);
        // the settlement buildings on it are seen as far as the ground
        if (const WorldStreamingRequests* world = WorldStreamingInterface::Get(); world && !tile.Structures.empty())
        {
            AZStd::vector<MeshHandle>& handles = m_structures[tile.Index];
            for (const WorldStructure& structure : tile.Structures)
            {
                const AZ::Transform transform = AZ::Transform::CreateFromQuaternionAndTranslation(
                    AZ::Quaternion::CreateRotationZ(AZ::DegToRad(structure.Yaw)), AZ::Vector3(structure.X, structure.Y, structure.Z));
                handles.push_back(Acquire(world->GetStructureModels()[structure.Model].Model, WorldAsset{}, transform, false));
            }
        }
    }

    void WorldRendererComponent::OnTileUnloaded(const int tile)
    {
        const auto found = m_tiles.find(tile);
        if (found != m_tiles.end())
        {
            m_meshes->ReleaseMesh(found->second);
            m_tiles.erase(found);
        }
        const auto buildings = m_structures.find(tile);
        if (buildings != m_structures.end())
        {
            ReleaseAll(buildings->second);
            m_structures.erase(buildings);
        }
    }

    void WorldRendererComponent::OnFoliageLoaded(const WorldFoliage& foliage)
    {
        const WorldStreamingRequests* world = WorldStreamingInterface::Get();
        if (!world || m_plants.find(foliage.Tile) != m_plants.end())
        {
            return;
        }
        AZStd::vector<MeshHandle>& handles = m_plants[foliage.Tile];
        const AZStd::vector<WorldPlant>& plants = world->GetPlants();
        for (const WorldPlanting& planting : foliage.Plantings)
        {
            const WorldPlant& plant = plants[planting.Plant];
            AZ::Transform transform = AZ::Transform::CreateFromQuaternionAndTranslation(
                AZ::Quaternion::CreateRotationZ(AZ::DegToRad(planting.Yaw)), AZ::Vector3(planting.X, planting.Y, planting.Z));
            transform.MultiplyByUniformScale(planting.Scale);
            for (const WorldPlant::Part& part : plant.Parts)
            {
                handles.push_back(Acquire(part.Model, part.Material, transform, !plant.Tree));
            }
        }
    }

    void WorldRendererComponent::OnFoliageUnloaded(const int tile)
    {
        const auto found = m_plants.find(tile);
        if (found != m_plants.end())
        {
            ReleaseAll(found->second);
            m_plants.erase(found);
        }
    }

    void WorldRendererComponent::BuildGrassCell(const int cellX, const int cellY, AZStd::vector<MeshHandle>& out)
    {
        const WorldStreamingRequests* world = WorldStreamingInterface::Get();
        if (!world)
        {
            return;
        }
        const float x0 = static_cast<float>(cellX) * GrassCell, y0 = static_cast<float>(cellY) * GrassCell;
        const int column = static_cast<int>(x0 / world->GetTileSize()), row = static_cast<int>(y0 / world->GetTileSize());
        const WorldTile* tile = world->GetTile(row * world->GetColumns() + column);
        const WorldBiomeGrass* biome = tile ? world->GetBiomeGrass(tile->Biome) : nullptr;
        if (!biome || biome->Grass.empty())
        {
            return;
        }
        const AZStd::vector<WorldGrass>& cards = world->GetGrass();
        for (int k = 0; k < GrassPerSide * GrassPerSide; ++k)
        {
            const AZ::u32 seed = static_cast<AZ::u32>(cellX) * 73856093u ^ static_cast<AZ::u32>(cellY) * 19349663u ^ static_cast<AZ::u32>(k) * 83492791u;
            if (Hash01(seed) > biome->Density)
            {
                continue;
            }
            const float x = x0 + 2.0f * static_cast<float>(k % GrassPerSide) + 2.0f * Hash01(seed + 1u);
            const float y = y0 + 2.0f * static_cast<float>(k / GrassPerSide) + 2.0f * Hash01(seed + 2u);
            float z = 0.0f, east = 0.0f, west = 0.0f, north = 0.0f, south = 0.0f;
            if (!world->GetGroundHeight(x, y, z) || z < 1.6f || !world->GetGroundHeight(x + 1.0f, y, east) || !world->GetGroundHeight(x - 1.0f, y, west) ||
                !world->GetGroundHeight(x, y + 1.0f, north) || !world->GetGroundHeight(x, y - 1.0f, south))
            {
                continue;  // sea, beach, or no heights resident
            }
            const AZ::Vector3 normal = AZ::Vector3((west - east) * 0.5f, (south - north) * 0.5f, 1.0f).GetNormalized();
            if (normal.GetZ() < 0.9f)
            {
                continue;  // steep ground is rock
            }
            const WorldGrass& card = cards[biome->Grass[static_cast<size_t>(Hash01(seed + 3u) * static_cast<float>(biome->Grass.size())) % biome->Grass.size()]];
            const AZ::Quaternion rotation = AZ::Quaternion::CreateShortestArc(AZ::Vector3::CreateAxisZ(), normal) *
                AZ::Quaternion::CreateRotationZ(Hash01(seed + 4u) * AZ::Constants::TwoPi);
            AZ::Transform transform = AZ::Transform::CreateFromQuaternionAndTranslation(rotation, AZ::Vector3(x, y, z));
            transform.MultiplyByUniformScale(0.8f + 0.5f * Hash01(seed + 5u));
            out.push_back(Acquire(card.Model, card.Material, transform, true));
        }
    }

    void WorldRendererComponent::OnTick(const float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        const WorldStreamingRequests* world = WorldStreamingInterface::Get();
        m_grassClock += deltaTime;
        if (!world || m_grassClock < 0.2f)
        {
            return;
        }
        m_grassClock = 0.0f;
        float px = 0.0f, py = 0.0f;
        world->GetFocus(px, py);
        // cells beyond the radius plus a margin leave; new ones within the radius come in, a few per update
        const float leave = m_grassRadius + GrassCell * 1.5f;
        for (auto it = m_grass.begin(); it != m_grass.end();)
        {
            const int cx = static_cast<int>(static_cast<AZ::s32>(it->first >> 32)), cy = static_cast<int>(static_cast<AZ::s32>(it->first & 0xFFFFFFFFu));
            const float dx = (static_cast<float>(cx) + 0.5f) * GrassCell - px, dy = (static_cast<float>(cy) + 0.5f) * GrassCell - py;
            if (dx * dx + dy * dy > leave * leave)
            {
                ReleaseAll(it->second);
                it = m_grass.erase(it);
            }
            else
            {
                ++it;
            }
        }
        const int reach = static_cast<int>(std::ceil(m_grassRadius / GrassCell));
        const int ox = static_cast<int>(std::floor(px / GrassCell)), oy = static_cast<int>(std::floor(py / GrassCell));
        int budget = 6;
        for (int ring = 0; ring <= reach && budget > 0; ++ring)
        {
            for (int cy = oy - ring; cy <= oy + ring && budget > 0; ++cy)
            {
                for (int cx = ox - ring; cx <= ox + ring && budget > 0; ++cx)
                {
                    if (AZStd::max(std::abs(cx - ox), std::abs(cy - oy)) != ring || cx < 0 || cy < 0)
                    {
                        continue;  // nearest rings first
                    }
                    const float dx = (static_cast<float>(cx) + 0.5f) * GrassCell - px, dy = (static_cast<float>(cy) + 0.5f) * GrassCell - py;
                    if (dx * dx + dy * dy > m_grassRadius * m_grassRadius || m_grass.find(CellKey(cx, cy)) != m_grass.end())
                    {
                        continue;
                    }
                    BuildGrassCell(cx, cy, m_grass[CellKey(cx, cy)]);
                    --budget;
                }
            }
        }
    }
}
