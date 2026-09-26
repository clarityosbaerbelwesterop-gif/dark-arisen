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
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace DarkArisen::Tools::Art
{
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

    class Kit
    {
    public:
        /** Builds the fixed assets (textures, ships, set pieces, people, props). */
        Kit();

        /** Every file produced so far (fixed assets plus everything requested since). */
        const std::vector<ArtFile>& Files() const { return Output; }

        /** Project-relative art model replacing a ContentSource greybox mesh, empty when none. */
        std::string ReplacementFor(std::string_view ContentSourceMesh) const;
        /** Art collision (spec glTF) for a ContentSource render or collision mesh, nullptr when the greybox stays. */
        const std::string* CollisionFor(std::string_view ContentSourceMesh) const;

        /** A person for a placeholder figure: named cast get their own look, others a townsfolk variant. */
        std::string FigureFor(std::string_view EntityName);
        /** Art for a placeholder prop by its placeholder mesh name (crate, route marker, war marker, hostile hull). */
        std::string Prop(std::string_view PlaceholderName);
        /**
         * Dressing for a solid block (metres; X/Y footprint, Z depth below its top at TopHeight):
         * provisional ground becomes a timber deck on piles, authored geometry dressed stone.
         */
        std::string Block(double SizeX, double SizeY, double SizeZ, double TopHeight, bool Provisional);
        /** Ground tile for land levels, GroundTileSize square, top at z = 0 (Style "paving" or "earth"). */
        std::string GroundTile(std::string_view Style);
        static constexpr double GroundTileSize = 50.0;
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
        std::vector<ArtFile> Output;
        std::map<std::string, std::string, std::less<>> Replacements;
        std::map<std::string, std::string, std::less<>> Collisions;
        std::map<std::string, std::string, std::less<>> Generated;

        std::string Emit(const std::string& Name, const std::string& Gltf);
    };
}
