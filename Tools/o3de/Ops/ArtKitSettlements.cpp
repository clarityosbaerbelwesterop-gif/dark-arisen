// Settlements of the open world (ContentSource/World/Locations.json): building models in six traditions and
// their layout on a location's levelled ground. The models are a fixed set of variants the Kit emits once
// (materialize commits them); the layout is a pure function world-build calls, so a settlement's streets are
// the same on every build. Only + - * / and sqrt reach the output, so every compiler writes the same bytes.

#include "ArtKit.h"
#include "ArtKitCore.h"
#include "ArtKitLibrary.h"

#include <algorithm>
#include <cmath>

namespace DarkArisen::Tools::Art
{
    namespace
    {
        constexpr int Variants = 6;

        struct Size
        {
            double W, D, H;  // footprint along X and Y, wall height
        };

        Size VariantSize(const Tradition Kind, const int V)
        {
            switch (Kind)
            {
            case Tradition::Colonial: return {6.0 + 1.5 * (V % 3), 8.0 + 2.0 * (V / 3), 6.0 + 2.5 * (V % 2)};
            case Tradition::Stilt: return {5.0 + 1.0 * (V % 3), 6.0 + 1.5 * (V / 3), 2.6};
            case Tradition::Nordic: return {5.0 + 1.0 * (V % 2), 9.0 + 2.5 * (V % 3), 2.6};
            case Tradition::Camp: return {2.4 + 0.4 * (V % 3), 3.0 + 0.6 * (V % 2), 1.8};
            case Tradition::Fort: return {16.0 + 4.0 * (V % 3), 2.6, 7.0};
            case Tradition::Entrance: return {6.0 + 1.0 * (V % 3), 5.0, 4.0 + 0.5 * (V % 2)};
            case Tradition::Harbour:
            {
                // footprints along the model's Y (into the water); boats: length along Y, beam along X, deck height
                static const Size Harbour[] = {{8.0, 26.0, 1.6}, {3.0, 30.0, 1.5}, {2.8, 10.0, 1.2}, {6.4, 24.0, 2.0}, {1.6, 4.4, 0.6}};
                return Harbour[V % 5];
            }
            case Tradition::Civic:
            {
                static const Size Civic[] = {{16.0, 36.0, 14.0}, {18.0, 26.0, 6.0}, {14.0, 30.0, 8.0}, {22.0, 16.0, 11.0}, {7.0, 7.0, 16.0}};
                return Civic[V % 5];
            }
            case Tradition::Lighthouse: default: return {5.0, 5.0, 18.0};
            }
        }

        const char* TraditionName(const Tradition Kind)
        {
            switch (Kind)
            {
            case Tradition::Colonial: return "Colonial";
            case Tradition::Stilt: return "Stilt";
            case Tradition::Nordic: return "Nordic";
            case Tradition::Camp: return "Camp";
            case Tradition::Fort: return "Fort";
            case Tradition::Entrance: return "Entrance";
            case Tradition::Civic: return "Civic";
            case Tradition::Harbour: return "Harbour";
            case Tradition::Lighthouse: default: return "Lighthouse";
            }
        }

        std::string ModelName(const Tradition Kind, const int V) { return std::string("SM_Art_Settlement_") + TraditionName(Kind) + "_" + std::to_string(V); }

        /** Gable roof over [-W/2, W/2] x [-D/2, D/2] from eaves at Z, ridge along X, Rise tall, Over overhang. */
        void Gable(Mesh& Out, const Material& Roof, const Material& Gables, const double W, const double D, const double Z, const double Rise, const double Over)
        {
            const double X0 = -W / 2 - Over, X1 = W / 2 + Over, Y0 = -D / 2 - Over, Y1 = D / 2 + Over, Top = Z + Rise;
            Out.Quad(Roof, {X0, Y0, Z}, {X1, Y0, Z}, {X1, 0.0, Top}, {X0, 0.0, Top}, true);
            Out.Quad(Roof, {X1, Y1, Z}, {X0, Y1, Z}, {X0, 0.0, Top}, {X1, 0.0, Top}, true);
            for (const double X : {-W / 2, W / 2})
                Out.Fan(Gables, {{X, -D / 2, Z}, {X, D / 2, Z}, {X, 0.0, Top}}, {X < 0 ? -1.0 : 1.0, 0.0, 0.0});
        }

        /** Hipped roof: a short ridge along X, eaves all round. */
        void Hip(Mesh& Out, const Material& Roof, const double W, const double D, const double Z, const double Rise, const double Over)
        {
            const double X0 = -W / 2 - Over, X1 = W / 2 + Over, Y0 = -D / 2 - Over, Y1 = D / 2 + Over, Top = Z + Rise;
            const double R = std::max(0.0, (X1 - X0) / 2 - (Y1 - Y0) / 2);
            Out.Quad(Roof, {X0, Y0, Z}, {X1, Y0, Z}, {R, 0.0, Top}, {-R, 0.0, Top}, true);
            Out.Quad(Roof, {X1, Y1, Z}, {X0, Y1, Z}, {-R, 0.0, Top}, {R, 0.0, Top}, true);
            Out.Quad(Roof, {X1, Y0, Z}, {X1, Y1, Z}, {R, 0.0, Top}, {R, 0.0, Top}, true);
            Out.Quad(Roof, {X0, Y1, Z}, {X0, Y0, Z}, {-R, 0.0, Top}, {-R, 0.0, Top}, true);
        }

        /** Door (dark inset) in the -Y wall and windows along both long walls. */
        void Openings(Mesh& Out, const double W, const double D, const double Z0, const double H, const Material& Frame)
        {
            Out.Box(Mat(M::TimberDark), {-0.55, -D / 2 - 0.06, Z0}, {0.55, -D / 2 + 0.02, Z0 + std::min(2.1, H - 0.3)});
            const int Count = std::max(1, static_cast<int>(W / 2.6));
            for (int K = 0; K < Count; ++K)
            {
                const double X = -W / 2 + (K + 0.5) * W / Count;
                if (std::fabs(X) < 1.0) continue;
                for (const double Y : {-D / 2 - 0.05, D / 2 + 0.05})
                {
                    Out.Box(Frame, {X - 0.45, Y - 0.03, Z0 + 1.0}, {X + 0.45, Y + 0.03, Z0 + std::min(2.1, H - 0.3)});
                    Out.Box(Mat(M::Glass), {X - 0.35, Y - 0.04, Z0 + 1.1}, {X + 0.35, Y + 0.04, Z0 + std::min(2.0, H - 0.4)});
                }
            }
        }

        Mesh BuildVariant(const Tradition Kind, const int V)
        {
            const Size S = VariantSize(Kind, V);
            const std::string Name = ModelName(Kind, V);
            switch (Kind)
            {
            case Tradition::Colonial:
                // the kit's dressed town house: plaster, tiled roof, doors on -Y
                return DressBoxes(Name, {{{-S.W / 2, -S.D / 2, 0.0}, {S.W / 2, S.D / 2, S.H}}});
            case Tradition::Stilt:
            {
                // a raised timber house on posts above flood and damp, steep tiled hip roof, a ladder to the porch
                Mesh Out(Name);
                const double Floor = 1.7;
                for (const double X : {-S.W / 2 + 0.2, 0.0, S.W / 2 - 0.2})
                    for (const double Y : {-S.D / 2 + 0.2, S.D / 2 - 0.2})
                        Out.Tube(Mat(M::TimberDark), {X, Y, -0.6}, {X, Y, Floor}, 0.13, 0.11, 8);
                Out.Box(Mat(M::Deck), {-S.W / 2 - 0.2, -S.D / 2 - 1.4, Floor}, {S.W / 2 + 0.2, S.D / 2 + 0.2, Floor + 0.18});
                Out.Box(V % 2 ? Mat(M::Timber) : Mat(M::PlasterOchre), {-S.W / 2, -S.D / 2, Floor + 0.18}, {S.W / 2, S.D / 2, Floor + 0.18 + S.H});
                Openings(Out, S.W, S.D, Floor + 0.18, S.H, Mat(M::TimberDark));
                Hip(Out, Mat(M::RoofTile), S.W, S.D, Floor + 0.18 + S.H, 2.4 + 0.2 * (V % 3), 0.9);
                for (int K = 0; K < 6; ++K)
                {
                    const double Y = -S.D / 2 - 1.4 - 0.25 * (K + 1), Z = Floor - 0.28 * (K + 1);
                    Out.Box(Mat(M::TimberDark), {-0.5, Y - 0.12, Z}, {0.5, Y + 0.12, Z + 0.06});
                }
                return Out;
            }
            case Tradition::Nordic:
            {
                // a long timber house on a stone footing under a steep turf roof, a stone chimney at one end
                Mesh Out(Name);
                Out.Box(Mat(M::StoneDark), {-S.W / 2 - 0.15, -S.D / 2 - 0.15, -0.5}, {S.W / 2 + 0.15, S.D / 2 + 0.15, 0.45});
                Out.Box(Mat(M::TimberDark), {-S.W / 2, -S.D / 2, 0.45}, {S.W / 2, S.D / 2, 0.45 + S.H});
                Mesh Rotated(Name + "_Openings");
                Openings(Rotated, S.D, S.W, 0.45, S.H, Mat(M::Timber));  // door and windows on the long side
                Out.Push(Xf::RotateZ(Pi / 2));
                Out.Append(Rotated);
                Out.Pop();
                Out.Push(Xf::RotateZ(Pi / 2));
                Mesh Roof(Name + "_Roof");
                Gable(Roof, Mat(M::Moss), Mat(M::TimberDark), S.D, S.W, 0.45 + S.H, 0.9 * S.W, 0.5);
                Out.Append(Roof);
                Out.Pop();
                Out.Box(Mat(M::Stone), {-0.5, S.D / 2 - 1.6, 0.45}, {0.5, S.D / 2 - 0.6, 0.45 + S.H + 0.9 * S.W + 0.6});
                return Out;
            }
            case Tradition::Camp:
            {
                // an A-frame tent of tarred canvas on two poles
                Mesh Out(Name);
                const double Half = S.W / 2, Top = S.H;
                Out.Quad(Mat(V % 2 ? M::Canvas : M::CanvasBlack), {-Half, -S.D / 2, 0.0}, {-Half, S.D / 2, 0.0}, {0.0, S.D / 2, Top}, {0.0, -S.D / 2, Top}, true);
                Out.Quad(Mat(V % 2 ? M::Canvas : M::CanvasBlack), {Half, S.D / 2, 0.0}, {Half, -S.D / 2, 0.0}, {0.0, -S.D / 2, Top}, {0.0, S.D / 2, Top}, true);
                for (const double Y : {-S.D / 2, S.D / 2}) Out.Tube(Mat(M::TimberDark), {0.0, Y, 0.0}, {0.0, Y, Top + 0.15}, 0.04, 0.04, 6);
                return Out;
            }
            case Tradition::Fort:
            {
                // a stretch of curtain wall with a squat tower at its end
                return DressFort(Name, {{{-S.W / 2, -S.D / 2, 0.0}, {S.W / 2, S.D / 2, S.H}}, {{S.W / 2 - 2.5, -3.5, 0.0}, {S.W / 2 + 4.5, 3.5, S.H + 3.0}}});
            }
            case Tradition::Harbour:
            {
                Mesh Out(Name);
                if (V == 0)
                {
                    // a stone quay running out from the shore (+Y), its top 1.6 m above the sea, bollards along its edges
                    Out.Box(Mat(M::StoneDark), {-S.W / 2, 0.0, -4.0}, {S.W / 2, S.D, S.H - 0.2});
                    Out.Box(Mat(M::Paving), {-S.W / 2 - 0.1, -2.0, S.H - 0.2}, {S.W / 2 + 0.1, S.D + 0.1, S.H});
                    for (double Y = 3.0; Y < S.D; Y += 6.0)
                        for (const double X : {-S.W / 2 + 0.5, S.W / 2 - 0.5}) Out.Tube(Mat(M::Iron), {X, Y, S.H}, {X, Y, S.H + 0.6}, 0.18, 0.15, 8);
                }
                else if (V == 1)
                {
                    // a timber pier on piles, plank deck 1.5 m above the sea
                    for (double Y = 1.0; Y < S.D; Y += 3.0)
                        for (const double X : {-S.W / 2 + 0.2, S.W / 2 - 0.2}) Out.Tube(Mat(M::TimberDark), {X, Y, -4.0}, {X, Y, S.H + 0.6}, 0.14, 0.12, 7);
                    Out.Box(Mat(M::Deck), {-S.W / 2, -2.0, S.H - 0.15}, {S.W / 2, S.D, S.H});
                }
                else
                {
                    // boats: a single-masted fishing boat, a two-masted merchant brig, a rowing boat; bow to +Y
                    ShipSpec Boat;
                    Boat.Name = Name;
                    Boat.Bowsprit = 0.0;
                    Boat.Cargo = V == 3;
                    if (V == 2)
                    {
                        Boat.Stern = -4.6; Boat.Bow = 5.2; Boat.HalfBeam = 1.35; Boat.Deck = 0.9; Boat.Keel = -0.8; Boat.Bulwark = 0.45; Boat.Sheer = 0.35;
                        Boat.Masts = {0.6}; Boat.MastHeights = {7.5}; Boat.Topsides = M::HullBlack; Boat.Band = M::HullBandRed; Boat.SailsSet = false; Boat.Bowsprit = 1.8;
                    }
                    else if (V == 3)
                    {
                        Boat.Stern = -11.0; Boat.Bow = 11.5; Boat.HalfBeam = 3.1; Boat.Deck = 1.8; Boat.Keel = -2.0; Boat.Bulwark = 0.9;
                        Boat.Masts = {-3.5, 4.0}; Boat.MastHeights = {15.0, 16.5}; Boat.SailsSet = false; Boat.Bowsprit = 6.0; Boat.Hatches = {{-1.0, 1.5}};
                    }
                    else
                    {
                        Boat.Stern = -2.1; Boat.Bow = 2.3; Boat.HalfBeam = 0.75; Boat.Deck = 0.35; Boat.Keel = -0.35; Boat.Bulwark = 0.25; Boat.Sheer = 0.15;
                        Boat.SternHalf = 0.6;
                    }
                    Mesh Hull = BuildShip(Boat, nullptr);
                    Out.Push(Xf::RotateZ(Pi / 2));  // the kit builds bow to +X
                    Out.Append(Hull);
                    Out.Pop();
                }
                return Out;
            }
            case Tradition::Civic:
            {
                Mesh Out(Name);
                switch (V)
                {
                case 0:
                {
                    // the cathedral: a nave under a tiled gable, a bell tower over the west door (-Y), an apse
                    Out.Box(Mat(M::PlasterOchre), {-S.W / 2, -S.D / 2, 0.0}, {S.W / 2, S.D / 2, S.H});
                    Mesh Nave(Name + "_Nave");
                    Gable(Nave, Mat(M::RoofTile), Mat(M::PlasterOchre), S.D, S.W, S.H, 6.0, 0.6);
                    Out.Push(Xf::RotateZ(Pi / 2));  // the ridge runs the nave's length (Y)
                    Out.Append(Nave);
                    Out.Pop();
                    Out.Box(Mat(M::Plaster), {-4.0, -S.D / 2 - 7.0, 0.0}, {4.0, -S.D / 2 + 1.0, S.H + 14.0});
                    Hip(Out, Mat(M::RoofTile), 8.0, 8.0, S.H + 14.0, 5.0, 0.3);
                    Out.Tube(Mat(M::PlasterOchre), {0.0, S.D / 2, 0.0}, {0.0, S.D / 2, S.H * 0.85}, 6.5, 6.5, 16);
                    Out.Box(Mat(M::TimberDark), {-1.4, -S.D / 2 - 7.08, 0.0}, {1.4, -S.D / 2 - 6.9, 4.6});
                    for (int K = 0; K < 5; ++K)
                        for (const double X : {-S.W / 2 - 0.05, S.W / 2 + 0.05})
                            Out.Box(Mat(M::Glass), {X - 0.05, -S.D / 2 + 4.0 + K * 6.0, 4.0}, {X + 0.05, -S.D / 2 + 5.8 + K * 6.0, 9.5});
                    break;
                }
                case 1:
                {
                    // the market hall: an open arcade of stone pillars under one great tiled roof
                    for (double X = -S.W / 2; X <= S.W / 2 + 0.01; X += S.W / 4)
                        for (double Y = -S.D / 2; Y <= S.D / 2 + 0.01; Y += S.D / 5)
                            Out.Box(Mat(M::Stone), {X - 0.4, Y - 0.4, 0.0}, {X + 0.4, Y + 0.4, S.H});
                    Out.Box(Mat(M::Paving), {-S.W / 2 - 1.0, -S.D / 2 - 1.0, -0.4}, {S.W / 2 + 1.0, S.D / 2 + 1.0, 0.15});
                    Hip(Out, Mat(M::RoofTile), S.W, S.D, S.H, 4.0, 1.2);
                    for (int K = 0; K < 6; ++K) Crate(Out, {-S.W / 2 + 2.0 + 2.5 * K, -2.0, 0.15}, {-S.W / 2 + 3.4 + 2.5 * K, -0.8, 1.0});
                    break;
                }
                case 2:
                {
                    // the warehouse: a long store of dark timber on stone, wide doors on both long sides
                    Out.Box(Mat(M::StoneDark), {-S.W / 2, -S.D / 2, -0.4}, {S.W / 2, S.D / 2, 1.2});
                    Out.Box(Mat(M::TimberDark), {-S.W / 2, -S.D / 2, 1.2}, {S.W / 2, S.D / 2, S.H});
                    Gable(Out, Mat(M::RoofTile), Mat(M::TimberDark), S.W, S.D, S.H, 4.5, 0.6);
                    for (int K = 0; K < 3; ++K)
                        for (const double Y : {-S.D / 2 - 0.06, S.D / 2 - 0.02})
                            Out.Box(Mat(M::Timber), {-S.W / 2 + 2.0 + K * 4.5, Y, 0.0}, {-S.W / 2 + 5.0 + K * 4.5, Y + 0.08, 4.0});
                    break;
                }
                case 3:
                {
                    // the mansion: a dressed great house behind a garden wall
                    Out.Append(DressBoxes(Name + "_House", {{{-S.W / 2, -S.D / 2, 0.0}, {S.W / 2, S.D / 2, S.H}}}));
                    for (const double Y : {-S.D / 2 - 8.0, S.D / 2 + 4.0}) Out.Box(Mat(M::Plaster), {-S.W / 2 - 4.0, Y, 0.0}, {S.W / 2 + 4.0, Y + 0.6, 2.4});
                    for (const double X : {-S.W / 2 - 4.0, S.W / 2 + 3.4}) Out.Box(Mat(M::Plaster), {X, -S.D / 2 - 8.0, 0.0}, {X + 0.6, S.D / 2 + 4.6, 2.4});
                    Out.Box(Mat(M::Iron), {-1.6, -S.D / 2 - 8.05, 0.0}, {1.6, -S.D / 2 - 7.35, 2.6});
                    break;
                }
                default:
                {
                    // the watchtower: a square stone tower with a crenellated top and a flag
                    Out.Box(Mat(M::Stone), {-S.W / 2, -S.D / 2, -0.5}, {S.W / 2, S.D / 2, S.H});
                    for (int K = 0; K < 4; ++K)
                    {
                        const double A = Pi / 2 * K;
                        OrientedBox(Out, Mat(M::Stone), {Cos(A) * (S.W / 2 - 0.3), Sin(A) * (S.W / 2 - 0.3), S.H + 0.6}, -Sin(A), Cos(A), {S.W / 2, 0.3, 0.6});
                    }
                    Out.Tube(Mat(M::TimberDark), {0.0, 0.0, S.H}, {0.0, 0.0, S.H + 6.0}, 0.08, 0.06, 6);
                    Out.Quad(Mat(M::HullBandRed), {0.0, 0.0, S.H + 6.0}, {0.0, 2.2, S.H + 6.0}, {0.0, 2.2, S.H + 4.6}, {0.0, 0.0, S.H + 4.6}, true);
                    Out.Box(Mat(M::TimberDark), {-0.6, -S.D / 2 - 0.06, 0.0}, {0.6, -S.D / 2 + 0.02, 2.2});
                    break;
                }
                }
                return Out;
            }
            case Tradition::Entrance:
            {
                Mesh Out(Name);
                if (V < 3)
                {
                    // a cave mouth: a heap of rock with a dark way in on its -Y face
                    for (int K = 0; K < 7; ++K)
                    {
                        const double A = Pi * (0.15 + 0.7 * K / 6.0);
                        Rock(Out, Mat(M::Rock), {Cos(A) * S.W * 0.55, Sin(A) * S.D * 0.35, -0.3}, S.W * 0.32, S.H * (0.8 + 0.25 * (K % 3)), 911u * (V + 1) + K);
                    }
                    Rock(Out, Mat(M::Rock), {0.0, 0.6, S.H * 0.55}, S.W * 0.45, S.H * 0.6, 37u * (V + 3));
                    Out.Box(Mat(M::StoneDark), {-1.2, -0.4, -0.2}, {1.2, 1.8, 2.6});
                }
                else
                {
                    // a carved doorway: two pillars and a lintel over steps going down into the dark
                    for (const double X : {-1.6, 1.6}) Out.Box(Mat(M::Stone), {X - 0.4, -0.4, 0.0}, {X + 0.4, 0.4, 3.4});
                    Out.Box(Mat(M::Stone), {-2.3, -0.5, 3.4}, {2.3, 0.5, 4.1});
                    Out.Box(Mat(M::StoneDark), {-1.2, -0.3, 0.0}, {1.2, 0.3, 3.4});
                    for (int K = 0; K < 5; ++K) Out.Box(Mat(M::Stone), {-1.25, -1.6 + 0.3 * K, -0.2 - 0.2 * K}, {1.25, -1.3 + 0.3 * K, -0.05 - 0.2 * K});
                    for (int K = 0; K < 4; ++K) Rock(Out, Mat(M::Lichen), {-3.4 + 2.2 * K, 1.2, -0.2}, 0.7, 0.6 + 0.2 * (K % 2), 53u * (V + K));
                }
                return Out;
            }
            case Tradition::Lighthouse:
            default:
            {
                // a whitewashed tapering tower, iron gallery and glazed lantern
                Mesh Out(Name);
                Out.Tube(Mat(M::Plaster), {0.0, 0.0, -0.5}, {0.0, 0.0, S.H}, S.W / 2, S.W / 2 * 0.62, 20);
                Out.Tube(Mat(M::StoneDark), {0.0, 0.0, -0.5}, {0.0, 0.0, 1.2}, S.W / 2 + 0.3, S.W / 2 + 0.2, 20);
                Out.Tube(Mat(M::Iron), {0.0, 0.0, S.H}, {0.0, 0.0, S.H + 0.25}, S.W / 2 * 0.62 + 0.6, S.W / 2 * 0.62 + 0.6, 20);
                Out.Tube(Mat(M::Glass), {0.0, 0.0, S.H + 0.25}, {0.0, 0.0, S.H + 2.4}, 1.1, 1.1, 12);
                Out.Tube(Mat(M::Iron), {0.0, 0.0, S.H + 2.4}, {0.0, 0.0, S.H + 3.6}, 1.3, 0.1, 12);
                Out.Box(Mat(M::TimberDark), {-0.55, -S.W / 2 - 0.08, 0.0}, {0.55, -S.W / 2 + 0.3, 2.2});
                return Out;
            }
            }
        }
    }

    void Kit::BuildSettlementModels()
    {
        for (const Tradition Kind : {Tradition::Colonial, Tradition::Stilt, Tradition::Nordic, Tradition::Camp, Tradition::Fort, Tradition::Lighthouse, Tradition::Entrance,
                 Tradition::Civic, Tradition::Harbour})
            for (int V = 0; V < (Kind == Tradition::Lighthouse ? 1 : Kind == Tradition::Civic || Kind == Tradition::Harbour ? 5 : Variants); ++V)
                Emit(ModelName(Kind, V), WriteGltf(BuildVariant(Kind, V), "Dark Arisen art kit (settlements)"));
    }

    std::vector<Structure> Kit::SettlementLayout(const SettlementSpec& Spec)
    {
        std::vector<Structure> Out;
        Rng R(Spec.Seed * 2654435761u + 977u);
        const auto Place = [&](const Tradition Kind, const int V, const double X, const double Y, const double Yaw)
        {
            const Size S = VariantSize(Kind, V);
            const double Z = Kind == Tradition::Harbour ? 0.0 : Spec.Ground(X, Y);  // the waterfront floats at sea level
            const double Lift = Kind == Tradition::Stilt ? 1.7 + 2.0 : 0.0;
            Out.push_back({"Assets/Art/Models/" + ModelName(Kind, V) + ".gltf", X, Y, Z, Yaw, S.W / 2 + 0.3, S.D / 2 + 0.3, S.H + Lift + 2.0});
        };
        const auto Free = [&](const double X, const double Y, const double Clear)
        {
            if (Spec.Ground(X, Y) < Spec.MinGround) return false;  // the shore, a pond, the sea
            for (const Structure& S : Out)
                if ((S.X - X) * (S.X - X) + (S.Y - Y) * (S.Y - Y) < (Clear + S.HalfX) * (Clear + S.HalfX)) return false;
            return true;
        };

        if (Spec.Kind == Tradition::Entrance)
        {
            // a cave mouth or a carved doorway, facing downhill (where the approach is)
            const double Dx = Spec.Ground(Spec.X - 4.0, Spec.Y) - Spec.Ground(Spec.X + 4.0, Spec.Y), Dy = Spec.Ground(Spec.X, Spec.Y - 4.0) - Spec.Ground(Spec.X, Spec.Y + 4.0);
            // the downhill direction to the nearest of eight (no libm: every compiler places it alike)
            const double Ax = std::fabs(Dx), Ay = std::fabs(Dy);
            double Facing = 0.0;
            if (Ax > 2.414 * Ay) Facing = Dx >= 0.0 ? 0.0 : 180.0;
            else if (Ay > 2.414 * Ax) Facing = Dy >= 0.0 ? 90.0 : 270.0;
            else Facing = Dx >= 0.0 ? (Dy >= 0.0 ? 45.0 : 315.0) : (Dy >= 0.0 ? 135.0 : 225.0);
            Place(Tradition::Entrance, static_cast<int>(Spec.Seed % Variants), Spec.X, Spec.Y, Facing + 90.0);
            return Out;
        }
        if (Spec.Kind == Tradition::Lighthouse)
        {
            Place(Tradition::Lighthouse, 0, Spec.X, Spec.Y, 0.0);
            Place(Tradition::Nordic, 0, Spec.X + 9.0, Spec.Y - 4.0, 90.0);  // the keeper's house
            return Out;
        }
        if (Spec.Kind == Tradition::Fort)
        {
            // a curtain wall round the site, stretch after stretch, each with its tower; a gap for the gate
            const double Ring = Spec.Radius * 0.8, Circumference = 2.0 * Pi * Ring;
            const double Gate = R.Range(0.0, Circumference);
            int V = 0;
            for (double Along = 0.0; Along < Circumference - 8.0;)
            {
                const double W = VariantSize(Tradition::Fort, V).W;
                const double A = (Along + W / 2) / Ring;
                if (std::fabs(Along + W / 2 - Gate) > 12.0)
                    Place(Tradition::Fort, V, Spec.X + Ring * Cos(A), Spec.Y + Ring * Sin(A), A * 180.0 / Pi + 90.0);
                Along += W + 2.5;  // the tower's width past the stretch
                V = (V + 1) % 3;
            }
            return Out;
        }

        // the waterfront: where the site meets the sea, piers or a quay run out from the shore with boats alongside
        if (Spec.Kind != Tradition::Camp && Spec.Kind != Tradition::Fort)
        {
            int Best = -1;
            double BestShore = 1e30;
            for (int K = 0; K < 16; ++K)
            {
                const double Ux = Cos(Pi * K / 8.0), Uy = Sin(Pi * K / 8.0);
                for (double D = 10.0; D < Spec.Radius * (Spec.Kind == Tradition::Colonial ? 2.6 : 1.8); D += 10.0)
                {
                    if (Spec.Ground(Spec.X + Ux * D, Spec.Y + Uy * D) < -1.5)
                    {
                        if (D < BestShore) { BestShore = D; Best = K; }
                        break;
                    }
                }
            }
            if (Best >= 0)
            {
                const double Ux = Cos(Pi * Best / 8.0), Uy = Sin(Pi * Best / 8.0);
                double Shore = 0.0;  // the waterline along that bearing
                for (double D = 0.0; D < BestShore; D += 2.0)
                    if (Spec.Ground(Spec.X + Ux * D, Spec.Y + Uy * D) < 0.3) { Shore = D; break; }
                const double Yaw = Pi * Best / 8.0 * 180.0 / Pi - 90.0;  // the model's +Y out to sea
                const bool Town = Spec.Kind == Tradition::Colonial;
                const int Piers = Town ? 3 : Spec.Kind == Tradition::Lighthouse ? 1 : 2;
                for (int P = 0; P < Piers; ++P)
                {
                    const double Side = (P - (Piers - 1) / 2.0) * (Town ? 30.0 : 22.0);  // along the shore
                    const double Bx = Spec.X + Ux * (Shore - 3.0) - Uy * Side, By = Spec.Y + Uy * (Shore - 3.0) + Ux * Side;
                    const int Kind = Town ? 0 : 1;
                    Place(Tradition::Harbour, Kind, Bx, By, Yaw);
                    // boats moored alongside: brigs at a town's quays, fishing boats and rowing boats at a village's piers
                    const double Along = VariantSize(Tradition::Harbour, Kind).D * 0.6, Off = VariantSize(Tradition::Harbour, Kind).W / 2 + (Town ? 4.5 : 2.2);
                    const int Boat = Town ? (P == 1 ? 2 : 3) : (P % 2 ? 4 : 2);
                    Place(Tradition::Harbour, Boat, Bx + Ux * Along - Uy * Off, By + Uy * Along + Ux * Off, Yaw);
                    if (!Town) Place(Tradition::Harbour, 4, Bx + Ux * (Along * 0.5) + Uy * Off, By + Uy * (Along * 0.5) - Ux * Off, Yaw + 8.0);
                }
            }
        }

        // the landmark on the square
        if (Spec.Centre != Landmark::None) Place(Tradition::Civic, static_cast<int>(Spec.Centre), Spec.X, Spec.Y, Spec.StreetAngle);

        if (Spec.Kind == Tradition::Colonial)
        {
            // a colonial grid: blocks of 40 x 26 m between 7 m streets on the city's axis, each block lined on its long
            // sides with terraced houses wall to wall, doors on the street; the square at the middle stays open
            const double Block = 40.0, Deep = 26.0, Street = 7.0;
            const double Ca = Cos(Spec.StreetAngle * Pi / 180.0), Sa = Sin(Spec.StreetAngle * Pi / 180.0);
            const double Square = Spec.Radius * 0.22;
            const int Reach = static_cast<int>(Spec.Radius / (Deep + Street)) + 1;
            for (int J = -Reach; J <= Reach && static_cast<int>(Out.size()) < Spec.MaxBuildings; ++J)
            {
                for (int I = -Reach; I <= Reach && static_cast<int>(Out.size()) < Spec.MaxBuildings; ++I)
                {
                    // block centre in the grid's frame (u along the street axis), offset half a block so the square is a crossing
                    const double U = (I + 0.5) * (Block + Street), W = (J + 0.5) * (Deep + Street);
                    if (U * U + W * W > Spec.Radius * 0.9 * Spec.Radius * 0.9 || U * U + W * W < Square * Square) continue;
                    if (R.Unit() > Spec.Fill) continue;  // a garden, a yard, a ruin
                    for (const double Side : {-1.0, 1.0})
                    {
                        double Along = -Block / 2;
                        while (Along < Block / 2 - 5.0)
                        {
                            const int V = static_cast<int>(R.Unit() * Variants);
                            const Size S = VariantSize(Tradition::Colonial, V);
                            if (Along + S.W > Block / 2) break;
                            const double Lu = U + Along + S.W / 2, Lw = W + Side * (Deep / 2 - S.D / 2);
                            const double X = Spec.X + Lu * Ca - Lw * Sa, Y = Spec.Y + Lu * Sa + Lw * Ca;
                            if (Spec.Ground(X, Y) >= Spec.MinGround)
                                Place(Tradition::Colonial, V, X, Y, Spec.StreetAngle + (Side < 0 ? 0.0 : 180.0));  // doors (-Y) on the street
                            Along += S.W + 0.05;
                        }
                    }
                }
            }
            return Out;
        }

        // villages and camps grow, they are not drawn: houses in loose clusters, denser towards the middle, each set
        // along the hillside (long side on the contour) with a little give, a path's width from its neighbours
        const double Ridge = Spec.StreetAngle;
        for (int Tries = 0; Tries < Spec.MaxBuildings * 30 && static_cast<int>(Out.size()) < Spec.MaxBuildings; ++Tries)
        {
            const double A = R.Range(0.0, 2.0 * Pi);
            const double Rr = Spec.Radius * 0.9 * std::sqrt(R.Unit()) * (0.35 + 0.65 * R.Unit());
            const double X = Spec.X + Rr * Cos(A), Y = Spec.Y + Rr * Sin(A);
            const int V = static_cast<int>(R.Unit() * Variants);
            const Size S = VariantSize(Spec.Kind, V);
            const double Jitter = R.Range(-14.0, 14.0);
            if (R.Unit() > Spec.Fill || !Free(X, Y, std::max(S.W, S.D) / 2 + (Spec.Kind == Tradition::Camp ? 2.0 : 4.0))) continue;
            // the contour: across the downhill direction (from the ground either side), else the village's own axis
            const double Gx = Spec.Ground(X + 3.0, Y) - Spec.Ground(X - 3.0, Y), Gy = Spec.Ground(X, Y + 3.0) - Spec.Ground(X, Y - 3.0);
            double Along = Ridge;
            if (Gx * Gx + Gy * Gy > 0.04) Along = std::fabs(Gx) > std::fabs(Gy) ? 0.0 : 90.0;  // long side (Y in the model) along the slope's contour
            Place(Spec.Kind, V, X, Y, Along + Jitter + (R.Unit() < 0.5 ? 180.0 : 0.0));
        }
        return Out;
    }
}
