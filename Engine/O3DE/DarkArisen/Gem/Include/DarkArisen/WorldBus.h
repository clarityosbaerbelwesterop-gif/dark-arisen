#pragma once

#include <DarkArisen/DarkArisenTypeIds.h>

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>
#include <AzCore/RTTI/RTTI.h>
#include <AzCore/std/containers/vector.h>
#include <AzCore/std/string/string.h>

/**
 * The open world as DarkArisenO3DE world-build wrote it (Assets/World/WorldTiles.json and one
 * Tiles/F_World_<column>_<row>.json per land tile), streamed around the player by Core::WorldStreaming.
 * The streamer decides what is resident; the renderer (and anything else that cares, e.g. spawning on the
 * ground) listens to WorldStreamingNotificationBus.
 */
namespace DarkArisen
{
    /** An asset as world-build names it: O3DE asset id (source UUID, product sub-id) and its product path. */
    struct WorldAsset
    {
        AZ::Data::AssetId Id;
        AZStd::string Hint;
    };

    /** A building of a settlement (world-build from Locations.json): a shared model and a box for collision. */
    struct WorldStructureModel
    {
        WorldAsset Model;
        float HalfX = 1, HalfY = 1, Height = 1;  // collision box in the model's frame, base at z = 0
    };

    struct WorldStructure
    {
        AZ::u32 Model = 0;                       // index into GetStructureModels()
        float X = 0, Y = 0, Z = 0, Yaw = 0;      // metres, degrees about Z
    };

    struct WorldTile
    {
        int Index = -1;               // row * columns + column
        int Column = 0, Row = 0;
        AZStd::string Biome;
        WorldAsset Model, Material;   // the ground and its DarkArisenTerrain material
        AZStd::string Foliage;        // product path of its plantings and ground heights
        AZStd::vector<WorldStructure> Structures;  // buildings standing on it (seen as far as the ground)
    };

    struct WorldPlant
    {
        struct Part
        {
            WorldAsset Model, Material;  // Material.Id invalid: the model keeps its own
        };
        bool Tree = false;               // seen from afar; undergrowth and rocks only near
        AZStd::vector<Part> Parts;
    };

    struct WorldPlanting
    {
        AZ::u32 Plant = 0;               // index into GetPlants()
        float X = 0, Y = 0, Z = 0;       // metres
        float Yaw = 0, Scale = 1;        // degrees about Z, uniform scale
    };

    /** A resident tile's plantings and its ground heights (metres, rows from the south, samples x samples). */
    struct WorldFoliage
    {
        int Tile = -1;
        float X0 = 0, Y0 = 0;
        AZStd::vector<WorldPlanting> Plantings;
        AZStd::vector<float> Heights;
    };

    struct WorldGrass
    {
        AZStd::string Card;              // GrassBlades, GrassDry, HeatherSprig, Reeds
        WorldAsset Model, Material;      // a 2 m patch, foot at z = 0
    };

    struct WorldBiomeGrass
    {
        AZStd::vector<AZ::u32> Grass;    // indices into GetGrass()
        float Density = 0;               // patches per 2 m cell of open, gentle ground
    };

    class WorldStreamingRequests
    {
    public:
        AZ_RTTI(WorldStreamingRequests, WorldStreamingRequestsTypeId);
        virtual ~WorldStreamingRequests() = default;

        /** Where streaming centres while no entity is followed (free camera, cutscenes, tests). */
        virtual void SetFocus(float x, float y) = 0;
        virtual void GetFocus(float& x, float& y) const = 0;

        virtual bool IsTileResident(int tile) const = 0;
        virtual AZStd::size_t GetResidentTileCount() const = 0;
        virtual const WorldTile* GetTile(int tile) const = 0;
        /** nullptr unless the tile's foliage is resident. */
        virtual const WorldFoliage* GetFoliage(int tile) const = 0;
        virtual const AZStd::vector<WorldPlant>& GetPlants() const = 0;
        virtual const AZStd::vector<WorldGrass>& GetGrass() const = 0;
        virtual const AZStd::vector<WorldStructureModel>& GetStructureModels() const = 0;
        virtual const WorldBiomeGrass* GetBiomeGrass(const AZStd::string& biome) const = 0;
        /** Ground height from the resident foliage tiles; false where none is resident (or at open sea). */
        virtual bool GetGroundHeight(float x, float y, float& height) const = 0;
        virtual float GetTileSize() const = 0;
        virtual int GetColumns() const = 0;
    };

    using WorldStreamingInterface = AZ::Interface<WorldStreamingRequests>;

    class WorldStreamingNotifications : public AZ::EBusTraits
    {
    public:
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        virtual ~WorldStreamingNotifications() = default;

        virtual void OnTileLoaded(const WorldTile& /*tile*/) {}
        virtual void OnTileUnloaded(int /*tile*/) {}
        virtual void OnFoliageLoaded(const WorldFoliage& /*foliage*/) {}
        virtual void OnFoliageUnloaded(int /*tile*/) {}
    };

    using WorldStreamingNotificationBus = AZ::EBus<WorldStreamingNotifications>;
}
