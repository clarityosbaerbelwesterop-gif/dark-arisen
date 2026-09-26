#pragma once

#include "ArtKit.h"
#include "ArtKitCore.h"

#include <string>
#include <vector>

namespace DarkArisen::Tools::Art
{
    /** Every texture of the kit as PNG files under Assets/Art/Textures. */
    std::vector<ArtFile> BuildTextures();

    enum class M
    {
        Deck, DeckWorn, Hull, HullBlack, HullBelow, HullBand, HullBandRed, Timber, TimberDark, Driftwood, Canvas, CanvasBlack,
        Rope, Iron, Brass, Glass, Plaster, PlasterOchre, PlasterBlue, RoofTile, Stone, StoneDark, Paving, Sand, WetSand, Grass,
        Rock, Bark, Frond, Coral, CoralRed, Kelp, HarborWater,
        Linen, WoolBlue, WoolBrown, WoolGrey, WoolBlack, Sash, Ochre, DressGreen, Leather, Skin, SkinDark, Hair, HairGrey,
        Count
    };
    const Material& Mat(M Id);

    // ------------------------------------------------------------------ shared construction helpers

    using Ring = std::vector<V3>;
    /** Counter-clockwise (seen from +Z) ellipse at height Z; lofting such rings upwards faces outwards. */
    Ring Ellipse(double Cx, double Cy, double Z, double Rx, double Ry, int Sides);
    /** Box rotated about Z by the direction (Dx, Dy) (unnormalised), centred on Centre. */
    void OrientedBox(Mesh& Out, const Material& Mt, const V3& Centre, double Dx, double Dy, const V3& HalfSize);
    /** Lumpy rock: a squashed noisy sphere standing on z = 0 (Radius across, Height tall). */
    void Rock(Mesh& Out, const Material& Mt, const V3& At, double Radius, double Height, std::uint32_t Seed);
    void Barrel(Mesh& Out, const V3& At, double Height, double Radius);
    void Crate(Mesh& Out, const V3& Min, const V3& Max);
    double SegmentDistance(double Px, double Py, double Ax, double Ay, double Bx, double By);
    /** Carriage gun at At (on its deck), barrel pointing to Side * +Y. */
    void Cannon(Mesh& Out, const V3& At, double Side);

    // ------------------------------------------------------------------ ships

    struct ShipSpec
    {
        std::string Name;
        double Stern = -13.5, Bow = 14.0;  // hull ends (x) at deck level; bow at +X (layout forward)
        double HalfBeam = 3.9;
        double Deck = 2.2;                 // walkable weather deck (z), the layout authority
        double Keel = -2.3;
        double Bulwark = 1.05;             // rail height above the deck
        double SternHalf = 0.78;           // transom half width, fraction of HalfBeam
        double Sheer = 0.5;                // rail rise towards the ends
        double BowTaperFrom = 0.64;        // fraction of the length where the bow entry starts
        M Topsides = M::Hull, Band = M::HullBand, Bottom = M::HullBelow;
        std::vector<double> Masts;         // x, stern to bow
        std::vector<double> MastHeights;   // above the deck
        bool SailsSet = true;
        bool BlackSails = false;
        bool SpankerOnAftMast = false;     // fore-and-aft sail on the aftmost mast
        int GunsPerSide = 0;
        std::vector<double> GunX;
        double Bowsprit = 8.0;
        double RaisedFrom = 1e9;           // raised fore deck from this x to the bow
        double RaisedDeck = 0.0;           // its height (z)
        double HelmX = 1e9;                // ship's wheel (hub 1.2 m above its deck)
        std::vector<std::pair<double, double>> Hatches;  // x ranges of hatch coamings
        double CapstanX = 1e9;
        bool Cargo = true;                 // barrels and crates along the bulwarks
    };

    /** Hull, deck, rig and fittings; Collision receives the walkable decks, bulwarks, masts and hull shell. */
    Mesh BuildShip(const ShipSpec& Spec, Mesh* Collision);

    // ------------------------------------------------------------------ people

    /** Standing person, feet on z = 0, facing +X. Variant: jake, boarder, merchant, officer, draven, woman, sailor, dockworker, elder, bigman. */
    Mesh BuildFigure(std::string_view Variant, std::uint32_t Seed);

    // ------------------------------------------------------------------ world

    Mesh BuildPalm(double Height, std::uint32_t Seed);
    Mesh BuildWreckage();
    Mesh BuildOuterReef();
    Mesh BuildCoveJetty();
    /** Rexa warehouse quarter over the greybox volumes; Collision gets walls, floors and the open passages. */
    Mesh BuildRexaWarehouses(Mesh* Collision);
    /** Generic dressing of a greybox made of boxes (source-frame min/max pairs already in world frame). */
    Mesh DressBoxes(const std::string& Name, const std::vector<std::pair<V3, V3>>& Boxes);
    /**
     * Colonial stone fort over the same boxes: battered curtain walls with a cordon, embrasured
     * parapets and a rampart walk, sentry boxes (garitas) at the wall ends, a false gate, gun
     * platforms with carriage guns, a flag. Volumes stay inside the greybox, so its collision holds.
     */
    Mesh DressFort(const std::string& Name, const std::vector<std::pair<V3, V3>>& Boxes);
}
