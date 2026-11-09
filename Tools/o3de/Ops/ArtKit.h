#pragma once

// Dark Arisen art kit: hand-authored, procedural rough art that replaces the greybox *visuals*.
// Every asset is built from code: geometry from explicit construction (lofted hulls, planks,
// frames, limbs) and tileable textures from integer noise, so the output is byte-identical on
// every platform and reviewable in Git. The rough concept sheets supplied by the owner (regions,
// architecture, props, nature) are reference for palette and form only; no image from them is
// used in the game, and nothing here comes from a generative model.
//
// Frame: meshes are authored in the O3DE world frame (X forward, Y left/port, Z up, metres) and
// written as spec glTF that the project's import maps back to that frame, so an art model drops
// onto the greybox entity transform unchanged. Collision stays the greybox or layout authority
// unless an art collision is given (ships' walkable decks, the Rexa warehouse hall, cove jetty).

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace DarkArisen::Tools::Art
{
    class HumanFactory;

    /** Reads a repository-relative source file; nullptr when it is missing (the reader records the error). */
    using SourceReader = std::function<const std::string*(const std::string&)>;

    struct ArtFile
    {
        std::string ProjectRelative;  // e.g. "Assets/Art/Models/SM_Art_HarlowMerchantShip.gltf"
        std::string Content;          // glTF JSON or PNG bytes
    };

    /** Where the art hull of La Liberacion lies at the Galleon Cove berth (see Docs/O3DE_MIGRATION.md). */
    struct CoveBerth
    {
        double OffsetX = 4.5;         // from the ImpoundBerth anchor: her starboard boarding point over LaLiberacionBoarding
        double OffsetY = 5.8;
        double RailHalfWidth = 4.0;   // outside of the starboard rail at the boarding point, ship frame
        double RailTop = 3.72;        // cap rail top there (weather deck 2.6 + bulwark 1.05 + sheer + cap)
        double DeckZ = 2.6;
        double StairFrom = -1.7;      // the boarding stair starts this far before the boarding anchor (shore side)
        double HelmX = 10.2;          // helm interaction on the raised fore deck, ship frame
        double HelmZ = 3.0;
    };

    /** Ground character of a land level (ArtKitTerrain.cpp). */
    enum class Biome
    {
        Moor,      // Highmoore: heather, dry grass, gorse, birch and pine, standing stones, rock on slopes
        Tropical,  // Rexa and the archipelago: red earth, grass, palms, broadleaf trees, scrub, ferns
        Mire,      // back routes: mud, moss, mangroves, reeds, dead trees
        Grove,     // Moran groves: lush grass and moss, dense broadleaf and palms, ferns
        Dream      // memory spaces: ash and dark stone, a few dead trees
    };

    /** Ground for a land level, in O3DE world metres. */
    struct TerrainSpec
    {
        std::string Name;            // names the models (SM_Art_Terrain_<Name>)
        Biome Kind = Biome::Moor;
        double X0 = 0, Y0 = 0, X1 = 0, Y1 = 0;  // extent
        double BaseZ = 0;            // lowest walkable level
        struct Leg
        {
            double Ax, Ay, Az, Bx, By, Bz, HalfWidth;  // walkable corridor segment (tops), a pad when A == B
        };
        std::vector<Leg> Route;
        struct Block
        {
            double X0, Y0, X1, Y1, Top;  // authored geometry the ground stays under
        };
        std::vector<Block> Keep;
        std::uint32_t Seed = 1;
    };

    /** One material of a plant as its own model, so each takes its own (wind) material. */
    struct PlantPart
    {
        std::string Model;     // project-relative glTF
        std::string Material;  // project-relative DarkArisenFoliage .material; empty keeps the model's own (stone)
    };

    struct Planting
    {
        std::vector<PlantPart> Parts;  // shared plant models
        double X, Y, Z, Yaw, Scale;    // degrees
        bool Tree;                     // a tree (seen from afar) rather than undergrowth or a rock
    };

    struct TerrainArt
    {
        std::string Model;           // the ground
        std::string Material;        // its DarkArisenTerrain material (scanned layers, splat, macro colour)
        std::string Ponds;           // still water of the tarns and pools, empty when there are none
        std::string CollisionGltf;   // walkable ground triangles
        std::vector<Planting> Plants;
        std::vector<PlantPart> Cover;  // ground cover cards, one model per card material, placed at the origin
    };

    /** One open-world tile to plant (Kit::WorldVegetation): ground from the archipelago, the island's biome. */
    struct WorldTileSpec
    {
        Biome Kind = Biome::Tropical;
        double X0 = 0, Y0 = 0, Size = 500;               // world metres, X east, Y north
        std::function<double(double, double)> Height;    // ground height at a world point
        struct Clearing
        {
            double X, Y, Radius;                         // a place (town, fort, set piece): nothing grows inside
        };
        std::vector<Clearing> Clear;
        double TreeLine = 1e9;                           // no trees above this height (m)
        std::uint32_t Seed = 1;
    };

    /** A grass patch model for the runtime's ground cover: a few crossed cards on a 2 m square, foot at z = 0. */
    struct GrassPatch
    {
        std::string Card;      // GrassBlades, GrassDry, HeatherSprig, Reeds
        std::string Gltf;      // the patch model
        std::string Material;  // its DarkArisenFoliage material (ground-cover sway), paths relative to Assets/World/Materials
    };

    /** Building traditions of the open world's settlements (ArtKitSettlements.cpp). */
    enum class Tradition
    {
        Colonial,    // Rexa: plaster town houses under clay tiles
        Stilt,       // Ashenmoor and the Mire: raised timber houses, steep hipped roofs
        Nordic,      // the Quiet Coast: long timber houses on stone footings under turf
        Camp,        // camps and encampments: canvas tents
        Fort,        // curtain walls with towers
        Lighthouse,  // a whitewashed tower and its keeper's house
        Entrance,    // a dungeon's way in: a cave mouth in the rock or a carved doorway with steps down
        Civic,       // a town's landmark on its square: cathedral, market hall, warehouse, mansion, watchtower
        Harbour,     // the waterfront: stone quay, timber pier, fishing boat, merchant brig, rowing boat (afloat at z = 0)
        Street       // street furniture: gas lamp, market stall, bench
    };

    /** Variants of Tradition::Civic. */
    enum class Landmark
    {
        None = -1, Cathedral = 0, MarketHall = 1, Warehouse = 2, Mansion = 3, Watchtower = 4, Faro = 5
    };

    struct SettlementSpec
    {
        Tradition Kind = Tradition::Colonial;
        double X = 0, Y = 0, Radius = 100;               // world metres
        std::function<double(double, double)> Ground;     // the site's (levelled) ground
        double MinGround = 1.0;                           // nothing is built lower (shore, ponds)
        int MaxBuildings = 40;
        double Fill = 0.8;                                // share of street plots built on
        Landmark Centre = Landmark::None;                 // what stands on the square
        double StreetAngle = 0.0;                         // colonial towns: the grid's axis (degrees), shared by a city's districts
        std::uint32_t Seed = 1;
        int Waterfront = -1;                              // -1: where the sea is near; 0: never; 1: always
        bool Cluster = false;                             // colonial: organic clusters instead of the grid
        Tradition Houses = Tradition::Colonial;           // colonial: the houses' tradition (a district of timber shanties)
        double LandmarkU = 0, LandmarkW = 0;              // the landmark's offset in the grid's frame (u along the streets)
        double LandmarkYaw = -1e9;                        // its yaw (degrees); -1e9: the street angle
        double PlazaU = 0, PlazaW = 0, PlazaRadius = -1;  // the square kept clear; a negative radius: 0.22 of the radius at the centre
        double LampSpacing = 0;                           // gas lamps along both sides of the streets every so many metres (0: none)
        double StallRadius = 0, StallPitch = 5;           // market stalls in a ring round the square (0: none)
        int StallMax = 0;
        int Benches = 0;                                  // benches round the square's edge
        std::function<bool(double, double, double)> KeepOff;  // true where a road runs within the reach of (x, y)
    };

    /** One building of a settlement: a shared model, its placement and a box for collision. */
    struct Structure
    {
        std::string Model;                       // project-relative glTF
        double X, Y, Z, Yaw;                     // base centre, degrees about Z
        double HalfX, HalfY, Height;             // collision box in the model's frame
    };

    /** A named story box (its top is the floor the anchors mark), dressed by Kit::Authored. */
    struct AuthoredSpec
    {
        std::string Name;                     // geometry name from the contract, e.g. HoldingGate
        double SizeX = 0, SizeY = 0, SizeZ = 0;
        Biome Kind = Biome::Moor;
        unsigned Open = 0;                    // edges the route crosses (1 -X, 2 +X, 4 -Y, 8 +Y): no walls there
    };

    /** A scanned prop (Assets/Art/Scanned) placed in a set piece's frame: origin at the piece's base centre, Z up. */
    struct PropPlacement
    {
        std::string Model;            // project-relative glTF
        double X = 0, Y = 0, Z = 0;   // metres
        double Yaw = 0;               // degrees about Z
    };

    class Kit
    {
    public:
        /** Builds the fixed assets (textures, ships, set pieces, people, props); Reader supplies ContentSource/ThirdParty. */
        explicit Kit(SourceReader Reader);
        ~Kit();
        Kit(const Kit&) = delete;
        Kit& operator=(const Kit&) = delete;

        /** Every file produced so far (fixed assets plus everything requested since). */
        const std::vector<ArtFile>& Files() const { return Output; }
        /** Problems met while building (missing or unreadable source data); empty when all is well. */
        const std::vector<std::string>& Problems() const { return ProblemList; }

        /** Project-relative art model replacing a ContentSource greybox mesh, empty when none. */
        std::string ReplacementFor(std::string_view ContentSourceMesh) const;
        /** Art collision (spec glTF) for a ContentSource render or collision mesh, nullptr when the greybox stays. */
        const std::string* CollisionFor(std::string_view ContentSourceMesh) const;

        /**
         * A person for a placeholder figure (ArtKitHumans.h): the named cast get their own body, face,
         * clothes and pose; hostiles, soldiers and townsfolk one of the crowd archetypes.
         */
        std::string FigureFor(std::string_view EntityName);
        /** Art for a placeholder prop by its placeholder mesh name (crate, route marker, war marker, hostile hull). */
        std::string Prop(std::string_view PlaceholderName);
        /**
         * Dressing for a solid block (metres; X/Y footprint, Z depth below its top at TopHeight):
         * provisional ground becomes a timber deck on piles, authored geometry dressed stone.
         */
        std::string Block(double SizeX, double SizeY, double SizeZ, double TopHeight, bool Provisional, bool Land = false);
        /** Ground tile for land levels, GroundTileSize square, top at z = 0 (Style "paving" or "earth"). */
        std::string GroundTile(std::string_view Style);
        /** Heightfield ground carrying the route, painted and planted by biome (ArtKitTerrain.cpp). */
        TerrainArt Terrain(const TerrainSpec& Spec);
        /** Architecture for a named story box by what its name says it is (ArtKitAuthored.cpp). */
        std::string Authored(const AuthoredSpec& Spec);
        /** Scanned props for an authored story box by its style: guns on defences, stores in yards and on decks, a hearth
         * in a camp field, a table in an interior; only along closed edges and in corners, never on the route. */
        static std::vector<PropPlacement> AuthoredProps(const AuthoredSpec& Spec);
        static constexpr double GroundTileSize = 50.0;
        /**
         * Trees, shrubs, ferns and rocks of an open-world tile by its biome, clustered into woods and clearings;
         * scanned CC0 models where a game-ready one exists, else the kit's plants (which materialize emits).
         * Cache keeps the parts of each plant across tiles.
         */
        static std::vector<Planting> WorldVegetation(const WorldTileSpec& Spec, std::map<std::string, std::vector<PlantPart>, std::less<>>& Cache);
        /** How wooded the open world is at a point, 0 (open ground) to 1 (the heart of a wood): plantings and macro colour agree. */
        static double WorldWoods(double X, double Y, std::uint32_t Seed);
        /** Where a settlement's buildings stand on its ground (a pure function: the same streets on every build). */
        static std::vector<Structure> SettlementLayout(const SettlementSpec& Spec);
        /** One settlement model placed on its own (a roadside house, a lamp), with its collision box. */
        static Structure SettlementPiece(Tradition Kind, int Variant, double X, double Y, double Z, double Yaw);
        /** The ground-cover patches the runtime scatters near the player, one per card. */
        static std::vector<GrassPatch> WorldGrassPatches();
        /** Quay wall along a harbour edge (x = 0 is the quay face, water towards -X), Length along Y. */
        std::string HarborEdge(double Length);
        /** Row of town houses 6 m deep centred on the origin, Length along X; doors face +Y (north) or -Y. */
        std::string TownRow(double Length, bool FacingNorth);
        /** Generic dressing of a greybox set piece made of boxes (buildings, walls, platforms, posts); same volumes as the greybox. */
        std::string Dress(std::string_view SourceRelative, std::string_view Gltf);
        /** Terrain dressed from its greybox heightfield (source frame glTF): sand, wet sand, grass, rock, palms. */
        std::string DressTerrain(std::string_view Name, std::string_view GreyboxGltf, const std::vector<std::pair<double, double>>& KeepClear);
        /**
         * Boarding stair along +Y from a pier top (origin): up Rise over Distance to a landing on the
         * ship's rail, then down Drop onto her deck. Collision (spec glTF) is one solid box per step.
         */
        std::string BoardingStair(double Distance, double Rise, double Drop, std::string& OutCollisionGltf);

        static CoveBerth Cove() { return {}; }

    private:
        std::unique_ptr<HumanFactory> Humans;
        std::vector<std::string> ProblemList;
        std::vector<ArtFile> Output;
        std::map<std::string, std::string, std::less<>> Replacements;
        std::map<std::string, std::string, std::less<>> Collisions;
        std::map<std::string, std::string, std::less<>> Generated;
        std::map<std::string, std::vector<PlantPart>, std::less<>> PlantModels;

        std::string Emit(const std::string& Name, const std::string& Gltf);
        /** Every building variant SettlementLayout may place (ArtKitSettlements.cpp). */
        void BuildSettlementModels();
        /** The creature bosses of docs/design/bosses (ArtKitCreatures.cpp). */
        void BuildCreatureModels();
        void BuildWeaponModels();
    };
}
