#include "WorldStreamerComponent.h"

#include <AzCore/Component/TransformBus.h>
#include <AzCore/IO/FileIO.h>
#include <AzCore/JSON/document.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/std/algorithm.h>

#include <cmath>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(WorldStreamerComponent, "WorldStreamerComponent", WorldStreamerComponentTypeId);

    namespace
    {
        bool ReadText(const AZStd::string& path, AZStd::string& out)
        {
            AZ::IO::FileIOBase* fileIO = AZ::IO::FileIOBase::GetInstance();
            AZ::IO::HandleType handle = AZ::IO::InvalidHandle;
            if (!fileIO || !fileIO->Open(path.c_str(), AZ::IO::OpenMode::ModeRead | AZ::IO::OpenMode::ModeBinary, handle))
            {
                return false;
            }
            AZ::u64 size = 0;
            bool ok = fileIO->Size(handle, size);
            out.resize_no_construct(static_cast<size_t>(size));
            ok = ok && (size == 0 || fileIO->Read(handle, out.data(), size, true));
            fileIO->Close(handle);
            return ok;
        }

        /** {"id": "{UUID}", "subId": n, "hint": "assets/..."} or null. */
        WorldAsset ReadAsset(const rapidjson::Value& value)
        {
            WorldAsset asset;
            if (!value.IsObject() || !value.HasMember("id") || !value["id"].IsString())
            {
                return asset;
            }
            const AZ::Uuid uuid = AZ::Uuid::CreateString(value["id"].GetString(), value["id"].GetStringLength());
            const AZ::u32 subId = value.HasMember("subId") && value["subId"].IsUint() ? value["subId"].GetUint() : 0;
            asset.Id = AZ::Data::AssetId(uuid, subId);
            if (value.HasMember("hint") && value["hint"].IsString())
            {
                asset.Hint = value["hint"].GetString();
            }
            return asset;
        }

        float Number(const rapidjson::Value& object, const char* name, const float fallback)
        {
            return object.HasMember(name) && object[name].IsNumber() ? static_cast<float>(object[name].GetDouble()) : fallback;
        }
    }

    void WorldStreamerComponent::Reflect(AZ::ReflectContext* context)
    {
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }
        serializeContext->Class<WorldStreamerComponent, AZ::Component>()
            ->Version(1)
            ->Field("Catalog", &WorldStreamerComponent::m_catalog)
            ->Field("Follow", &WorldStreamerComponent::m_follow)
            ->Field("LoadRadius", &WorldStreamerComponent::m_loadRadius)
            ->Field("UnloadRadius", &WorldStreamerComponent::m_unloadRadius)
            ->Field("FoliageRadius", &WorldStreamerComponent::m_foliageRadius)
            ->Field("LoadsPerTick", &WorldStreamerComponent::m_loadsPerTick);
        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
        {
            editContext->Class<WorldStreamerComponent>("Dark Arisen World Streamer", "Streams the open world's terrain tiles and plantings around the player.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Level"))
                ->DataElement(AZ::Edit::UIHandlers::Default, &WorldStreamerComponent::m_catalog, "Catalog", "WorldTiles.json product path")
                ->DataElement(AZ::Edit::UIHandlers::Default, &WorldStreamerComponent::m_follow, "Follow", "Streaming centres on this entity (Jake)")
                ->DataElement(AZ::Edit::UIHandlers::Default, &WorldStreamerComponent::m_loadRadius, "Load Radius", "Terrain tiles closer than this come in (m)")
                ->DataElement(AZ::Edit::UIHandlers::Default, &WorldStreamerComponent::m_unloadRadius, "Unload Radius", "...and leave only beyond this (m)")
                ->DataElement(AZ::Edit::UIHandlers::Default, &WorldStreamerComponent::m_foliageRadius, "Foliage Radius", "Plantings and ground heights (m)")
                ->DataElement(AZ::Edit::UIHandlers::Default, &WorldStreamerComponent::m_loadsPerTick, "Loads Per Tick", "New tiles started per frame");
        }
    }

    void WorldStreamerComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("DarkArisenWorldStreamingService"));
    }

    void WorldStreamerComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("DarkArisenWorldStreamingService"));
    }

    bool WorldStreamerComponent::LoadCatalog()
    {
        AZStd::string text;
        if (!ReadText(m_catalog, text))
        {
            AZ_Error("WorldStreamer", false, "Cannot read the world catalog %s (run DarkArisenO3DE world-build).", m_catalog.c_str());
            return false;
        }
        rapidjson::Document document;
        document.Parse(text.c_str(), text.size());
        if (document.HasParseError() || !document.IsObject() || !document.HasMember("tiles") || !document["tiles"].IsArray())
        {
            AZ_Error("WorldStreamer", false, "The world catalog %s is not a world-build catalog.", m_catalog.c_str());
            return false;
        }
        m_tileSize = Number(document, "tileSize", 500.0f);
        m_heightStep = Number(document, "heightStep", 5.0f);
        m_heightSamples = static_cast<int>(Number(document, "heightSamples", 101.0f));
        m_columns = static_cast<int>(Number(document, "columns", 0.0f));
        m_rows = static_cast<int>(Number(document, "rows", 0.0f));

        AZStd::vector<int> land;
        for (const rapidjson::Value& entry : document["tiles"].GetArray())
        {
            WorldTile tile;
            tile.Index = static_cast<int>(Number(entry, "tile", -1.0f));
            tile.Column = static_cast<int>(Number(entry, "column", 0.0f));
            tile.Row = static_cast<int>(Number(entry, "row", 0.0f));
            tile.Biome = entry.HasMember("biome") && entry["biome"].IsString() ? entry["biome"].GetString() : "tropical";
            tile.Model = entry.HasMember("model") ? ReadAsset(entry["model"]) : WorldAsset{};
            tile.Material = entry.HasMember("material") ? ReadAsset(entry["material"]) : WorldAsset{};
            tile.Foliage = entry.HasMember("foliage") && entry["foliage"].IsString() ? AZStd::string("@products@/") + entry["foliage"].GetString() : AZStd::string();
            if (entry.HasMember("structures") && entry["structures"].IsArray())
            {
                for (const rapidjson::Value& row : entry["structures"].GetArray())
                {
                    // [model, x cm, y cm, z cm, yaw degrees]
                    if (!row.IsArray() || row.Size() < 5 || !row[0].IsUint())
                    {
                        continue;
                    }
                    WorldStructure structure;
                    structure.Model = row[0].GetUint();
                    structure.X = static_cast<float>(row[1].GetDouble() * 0.01);
                    structure.Y = static_cast<float>(row[2].GetDouble() * 0.01);
                    structure.Z = static_cast<float>(row[3].GetDouble() * 0.01);
                    structure.Yaw = static_cast<float>(row[4].GetDouble());
                    tile.Structures.push_back(structure);
                }
            }
            if (tile.Index < 0)
            {
                continue;
            }
            m_tileByIndex[tile.Index] = m_tiles.size();
            land.push_back(tile.Index);
            m_tiles.push_back(AZStd::move(tile));
        }
        if (document.HasMember("plants") && document["plants"].IsArray())
        {
            for (const rapidjson::Value& entry : document["plants"].GetArray())
            {
                WorldPlant plant;
                plant.Tree = entry.HasMember("tree") && entry["tree"].IsBool() && entry["tree"].GetBool();
                if (entry.HasMember("parts") && entry["parts"].IsArray())
                {
                    for (const rapidjson::Value& part : entry["parts"].GetArray())
                    {
                        WorldPlant::Part p;
                        p.Model = part.HasMember("model") ? ReadAsset(part["model"]) : WorldAsset{};
                        p.Material = part.HasMember("material") ? ReadAsset(part["material"]) : WorldAsset{};
                        plant.Parts.push_back(AZStd::move(p));
                    }
                }
                m_plants.push_back(AZStd::move(plant));
            }
        }
        if (document.HasMember("structures") && document["structures"].IsArray())
        {
            for (const rapidjson::Value& entry : document["structures"].GetArray())
            {
                WorldStructureModel model;
                model.Model = entry.HasMember("model") ? ReadAsset(entry["model"]) : WorldAsset{};
                model.Material = entry.HasMember("material") ? ReadAsset(entry["material"]) : WorldAsset{};
                if (entry.HasMember("box") && entry["box"].IsArray() && entry["box"].Size() == 3)
                {
                    model.HalfX = static_cast<float>(entry["box"][0].GetDouble());
                    model.HalfY = static_cast<float>(entry["box"][1].GetDouble());
                    model.Height = static_cast<float>(entry["box"][2].GetDouble());
                }
                m_structureModels.push_back(AZStd::move(model));
            }
        }
        // a structure naming a model the table lacks would index past it: drop it rather than draw garbage
        for (WorldTile& tile : m_tiles)
        {
            tile.Structures.erase(
                AZStd::remove_if(tile.Structures.begin(), tile.Structures.end(),
                    [this](const WorldStructure& s) { return s.Model >= m_structureModels.size(); }),
                tile.Structures.end());
        }
        if (document.HasMember("paved") && document["paved"].IsArray())
        {
            for (const rapidjson::Value& disc : document["paved"].GetArray())
            {
                if (disc.IsArray() && disc.Size() == 3 && disc[0].IsNumber() && disc[1].IsNumber() && disc[2].IsNumber())
                {
                    m_paved.emplace_back(static_cast<float>(disc[0].GetDouble()), static_cast<float>(disc[1].GetDouble()), static_cast<float>(disc[2].GetDouble()));
                }
            }
        }
        if (document.HasMember("grass") && document["grass"].IsArray())
        {
            for (const rapidjson::Value& entry : document["grass"].GetArray())
            {
                WorldGrass grass;
                grass.Card = entry.HasMember("card") && entry["card"].IsString() ? entry["card"].GetString() : "";
                grass.Model = entry.HasMember("model") ? ReadAsset(entry["model"]) : WorldAsset{};
                grass.Material = entry.HasMember("material") ? ReadAsset(entry["material"]) : WorldAsset{};
                m_grass.push_back(AZStd::move(grass));
            }
        }
        if (document.HasMember("biomes") && document["biomes"].IsObject())
        {
            for (auto member = document["biomes"].MemberBegin(); member != document["biomes"].MemberEnd(); ++member)
            {
                WorldBiomeGrass biome;
                biome.Density = Number(member->value, "density", 0.0f);
                if (member->value.HasMember("grass") && member->value["grass"].IsArray())
                {
                    for (const rapidjson::Value& card : member->value["grass"].GetArray())
                    {
                        for (AZ::u32 index = 0; index < m_grass.size(); ++index)
                        {
                            if (card.IsString() && m_grass[index].Card == card.GetString())
                            {
                                biome.Grass.push_back(index);
                            }
                        }
                    }
                }
                m_biomes[member->name.GetString()] = AZStd::move(biome);
            }
        }

        Core::StreamingSettings terrain;
        terrain.WorldWidth = static_cast<double>(m_columns) * m_tileSize;
        terrain.WorldHeight = static_cast<double>(m_rows) * m_tileSize;
        terrain.TileSize = m_tileSize;
        terrain.LoadRadius = m_loadRadius;
        terrain.UnloadRadius = m_unloadRadius;
        terrain.LoadsPerUpdate = m_loadsPerTick;
        Core::StreamingSettings foliage = terrain;
        foliage.LoadRadius = m_foliageRadius;
        foliage.UnloadRadius = m_foliageRadius + (m_unloadRadius - m_loadRadius) * 0.5;
        foliage.LoadsPerUpdate = AZStd::max(1, m_loadsPerTick / 2);
        const std::vector<int> landTiles(land.begin(), land.end());
        m_terrain = AZStd::make_unique<Core::WorldStreaming>(terrain);
        m_terrain->SetLandTiles(landTiles);
        m_foliageStreaming = AZStd::make_unique<Core::WorldStreaming>(foliage);
        m_foliageStreaming->SetLandTiles(landTiles);
        return true;
    }

    bool WorldStreamerComponent::LoadFoliage(const WorldTile& tile, WorldFoliage& out) const
    {
        AZStd::string text;
        if (tile.Foliage.empty() || !ReadText(tile.Foliage, text))
        {
            AZ_Warning("WorldStreamer", false, "Cannot read the plantings of tile %d (%s).", tile.Index, tile.Foliage.c_str());
            return false;
        }
        rapidjson::Document document;
        document.Parse(text.c_str(), text.size());
        if (document.HasParseError() || !document.IsObject())
        {
            return false;
        }
        out.Tile = tile.Index;
        out.X0 = Number(document, "x0", tile.Column * m_tileSize);
        out.Y0 = Number(document, "y0", tile.Row * m_tileSize);
        if (document.HasMember("plants") && document["plants"].IsArray())
        {
            out.Plantings.reserve(document["plants"].Size());
            for (const rapidjson::Value& row : document["plants"].GetArray())
            {
                // [plant, x cm, y cm, z cm, yaw degrees, scale %]
                if (!row.IsArray() || row.Size() < 6 || !row[0].IsUint() || row[0].GetUint() >= m_plants.size())
                {
                    continue;
                }
                WorldPlanting planting;
                planting.Plant = row[0].GetUint();
                planting.X = static_cast<float>(row[1].GetDouble() * 0.01);
                planting.Y = static_cast<float>(row[2].GetDouble() * 0.01);
                planting.Z = static_cast<float>(row[3].GetDouble() * 0.01);
                planting.Yaw = static_cast<float>(row[4].GetDouble());
                planting.Scale = static_cast<float>(row[5].GetDouble() * 0.01);
                out.Plantings.push_back(planting);
            }
        }
        if (document.HasMember("heights") && document["heights"].IsArray())
        {
            out.Heights.reserve(document["heights"].Size());
            for (const rapidjson::Value& height : document["heights"].GetArray())
            {
                out.Heights.push_back(height.IsNumber() ? static_cast<float>(height.GetDouble() * 0.01) : 0.0f);
            }
        }
        if (out.Heights.size() != static_cast<size_t>(m_heightSamples) * static_cast<size_t>(m_heightSamples))
        {
            out.Heights.clear();  // no heights rather than wrong ones
        }
        return true;
    }

    void WorldStreamerComponent::Activate()
    {
        if (!LoadCatalog())
        {
            return;
        }
        WorldStreamingInterface::Register(this);
        AZ::TickBus::Handler::BusConnect();
    }

    void WorldStreamerComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        if (WorldStreamingInterface::Get() == this)
        {
            WorldStreamingInterface::Unregister(this);
        }
        // everything resident leaves, so listeners release what they hold
        for (const auto& [index, foliage] : m_foliage)
        {
            WorldStreamingNotificationBus::Broadcast(&WorldStreamingNotifications::OnFoliageUnloaded, index);
        }
        if (m_terrain)
        {
            for (const WorldTile& tile : m_tiles)
            {
                if (m_terrain->IsTileResident(tile.Index))
                {
                    WorldStreamingNotificationBus::Broadcast(&WorldStreamingNotifications::OnTileUnloaded, tile.Index);
                }
            }
        }
        m_foliage.clear();
        m_terrain.reset();
        m_foliageStreaming.reset();
        m_tiles.clear();
        m_tileByIndex.clear();
        m_plants.clear();
        m_grass.clear();
        m_structureModels.clear();
        m_paved.clear();
        m_biomes.clear();
    }

    void WorldStreamerComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        if (m_follow.IsValid())
        {
            AZ::Vector3 position = AZ::Vector3::CreateZero();
            AZ::TransformBus::EventResult(position, m_follow, &AZ::TransformBus::Events::GetWorldTranslation);
            m_focusX = position.GetX();
            m_focusY = position.GetY();
        }
        Step(m_focusX, m_focusY);
    }

    void WorldStreamerComponent::Step(const float x, const float y)
    {
        if (!m_terrain || !m_foliageStreaming)
        {
            return;
        }
        m_focusX = x;
        m_focusY = y;
        // foliage first out, terrain last out; terrain first in, foliage after: plants never stand on missing ground
        const Core::StreamingChange plants = m_foliageStreaming->Update(x, y);
        for (const int index : plants.UnloadTiles)
        {
            m_foliage.erase(index);
            WorldStreamingNotificationBus::Broadcast(&WorldStreamingNotifications::OnFoliageUnloaded, index);
        }
        const Core::StreamingChange ground = m_terrain->Update(x, y);
        for (const int index : ground.UnloadTiles)
        {
            WorldStreamingNotificationBus::Broadcast(&WorldStreamingNotifications::OnTileUnloaded, index);
        }
        for (const int index : ground.LoadTiles)
        {
            if (const WorldTile* tile = GetTile(index))
            {
                WorldStreamingNotificationBus::Broadcast(&WorldStreamingNotifications::OnTileLoaded, *tile);
            }
        }
        for (const int index : plants.LoadTiles)
        {
            const WorldTile* tile = GetTile(index);
            WorldFoliage foliage;
            if (!tile || !LoadFoliage(*tile, foliage))
            {
                continue;
            }
            const WorldFoliage& stored = m_foliage[index] = AZStd::move(foliage);
            WorldStreamingNotificationBus::Broadcast(&WorldStreamingNotifications::OnFoliageLoaded, stored);
        }
    }

    void WorldStreamerComponent::SetFocus(const float x, const float y)
    {
        m_focusX = x;
        m_focusY = y;
    }

    void WorldStreamerComponent::GetFocus(float& x, float& y) const
    {
        x = m_focusX;
        y = m_focusY;
    }

    bool WorldStreamerComponent::IsTileResident(const int tile) const
    {
        return m_terrain && m_terrain->IsTileResident(tile);
    }

    AZStd::size_t WorldStreamerComponent::GetResidentTileCount() const
    {
        return m_terrain ? m_terrain->ResidentTileCount() : 0;
    }

    const WorldTile* WorldStreamerComponent::GetTile(const int tile) const
    {
        const auto found = m_tileByIndex.find(tile);
        return found == m_tileByIndex.end() ? nullptr : &m_tiles[found->second];
    }

    const WorldFoliage* WorldStreamerComponent::GetFoliage(const int tile) const
    {
        const auto found = m_foliage.find(tile);
        return found == m_foliage.end() ? nullptr : &found->second;
    }

    const WorldBiomeGrass* WorldStreamerComponent::GetBiomeGrass(const AZStd::string& biome) const
    {
        const auto found = m_biomes.find(biome);
        return found == m_biomes.end() ? nullptr : &found->second;
    }

    bool WorldStreamerComponent::IsPaved(const float x, const float y) const
    {
        for (const AZ::Vector3& disc : m_paved)
        {
            const float dx = x - disc.GetX(), dy = y - disc.GetY();
            if (dx * dx + dy * dy < disc.GetZ() * disc.GetZ())
            {
                return true;
            }
        }
        return false;
    }

    bool WorldStreamerComponent::GetGroundHeight(const float x, const float y, float& height) const
    {
        if (m_columns <= 0 || x < 0.0f || y < 0.0f)
        {
            return false;
        }
        const int column = static_cast<int>(x / m_tileSize), row = static_cast<int>(y / m_tileSize);
        const WorldFoliage* foliage = GetFoliage(row * m_columns + column);
        if (!foliage || foliage->Heights.empty())
        {
            return false;
        }
        // the tile mesh's own grid, bilinear within a cell
        const int last = m_heightSamples - 1;
        const float fx = AZStd::clamp((x - foliage->X0) / m_heightStep, 0.0f, static_cast<float>(last) - 0.001f);
        const float fy = AZStd::clamp((y - foliage->Y0) / m_heightStep, 0.0f, static_cast<float>(last) - 0.001f);
        const int i = static_cast<int>(fx), j = static_cast<int>(fy);
        const float u = fx - static_cast<float>(i), v = fy - static_cast<float>(j);
        const auto at = [&](const int a, const int b) { return foliage->Heights[static_cast<size_t>(b * m_heightSamples + a)]; };
        const float south = at(i, j) + (at(i + 1, j) - at(i, j)) * u;
        const float north = at(i, j + 1) + (at(i + 1, j + 1) - at(i, j + 1)) * u;
        height = south + (north - south) * v;
        return true;
    }
}
