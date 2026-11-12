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
                static const Size Civic[] = {{27.0, 50.0, 31.0},  // the cathedral's box spans its aisles, towers and dome (layout.md R-08)
                                               {18.0, 26.0, 6.0}, {14.0, 30.0, 8.0}, {22.0, 16.0, 11.0}, {7.0, 7.0, 16.0}, {9.0, 9.0, 42.0},
                                               {18.0, 16.0, 9.0}, {24.0, 18.0, 10.0},  // LM6 La Sirena Negra, LM5 the Guild (layout.md 4)
                                               {12.0, 1.0, 4.5}, {12.0, 1.0, 4.5}, {6.0, 6.0, 8.0}, {1.3, 1.3, 5.6},  // W1, W1 with its lamp, W3, a gate pier
                                               {11.0, 12.6, 8.0}, {34.0, 40.0, 8.0}, {44.0, 36.0, 11.0}, {20.0, 14.0, 7.0}, {16.0, 16.0, 0.9},  // TC2, TC4 lot, LM10 lot, LM11, N2
                                               {8.0, 2.5, 6.5}, {4.6, 0.8, 1.7}};  // W2 the seam arch, a stair's low gate (layout.md 3.3)
                return Civic[V % 19];
            }
            case Tradition::Street:
            {
                static const Size Street[] = {{0.7, 0.7, 4.2}, {3.0, 3.0, 3.0}, {1.8, 0.6, 0.9}};  // gas lamp, market stall, bench
                return Street[V % 3];
            }
            case Tradition::Shanty:
            {
                // TC6 SH0-SH5 (layout.md 4): shanties, two of them two-storey with a 0.6 m jetty, and the long tenement
                static const Size Shanty[] = {{3.5, 4.5, 2.6}, {4.0, 5.0, 2.8}, {5.0, 6.0, 5.4}, {4.5, 7.0, 5.8}, {6.0, 8.0, 3.2}, {8.0, 22.0, 6.5}};
                return Shanty[V % 6];
            }
            case Tradition::Ruin: return {6.0 + 1.5 * (V % 3), 8.0, 3.6};  // TC7 R0-R2 (layout.md 4)
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
            case Tradition::Street: return "Street";
            case Tradition::Shanty: return "Shanty";
            case Tradition::Ruin: return "Ruin";
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
            // both faces of each gable (a quad folded to a triangle): seen from outside and, under the eaves, from below
            for (const double X : {-W / 2, W / 2}) Out.Quad(Gables, {X, -D / 2, Z}, {X, D / 2, Z}, {X, 0.0, Top}, {X, 0.0, Top}, true);
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

        // ---------------------------------------------------------------- masonry details (walls face -Y in the local frame)

        /** Round-headed panel on the wall plane y = Y facing -Y: a rectangle W wide from Z0 to the springing at Z0 + H, then a semicircle. */
        void ArchPanel(Mesh& Out, const Material& M, const double X, const double Y, const double Z0, const double W, const double H)
        {
            Out.Box(M, {X - W / 2, Y - 0.02, Z0}, {X + W / 2, Y, Z0 + H});
            std::vector<V3> Head;
            for (int K = 0; K <= 10; ++K)
            {
                const double A = Pi * K / 10.0;
                Head.push_back({X + W / 2 * Cos(A), Y - 0.02, Z0 + H + W / 2 * Sin(A)});
            }
            Out.Fan(M, Head, {0.0, -1.0, 0.0});
        }

        /** A round-headed window or door: a stone surround standing proud of the wall and the opening inside it. */
        void ArchedOpening(Mesh& Out, const Material& Surround, const Material& Fill, const double X, const double Z0, const double W, const double H)
        {
            ArchPanel(Out, Surround, X, -0.03, Z0 - 0.2, W + 0.5, H + 0.2);
            ArchPanel(Out, Fill, X, -0.06, Z0, W, H);
            Out.Box(Surround, {X - W / 2 - 0.35, -0.22, Z0 - 0.3}, {X + W / 2 + 0.35, 0.0, Z0 - 0.12});  // the sill
        }

        /** Round window (oculus) of radius R centred at (X, Z) with a moulded ring. */
        void Oculus(Mesh& Out, const Material& Ring, const Material& Fill, const double X, const double Z, const double R)
        {
            Out.Push(Xf::Translate({X, 0.0, Z}) * Xf::RotateX(Pi / 2));
            Out.Tube(Ring, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.25}, R + 0.3, R + 0.3, 20);
            Out.Pop();
            std::vector<V3> Disc;
            for (int K = 0; K < 20; ++K)
            {
                const double A = 2.0 * Pi * K / 20.0;
                Disc.push_back({X + R * Cos(A), -0.27, Z + R * Sin(A)});
            }
            Out.Fan(Fill, Disc, {0.0, -1.0, 0.0});
        }

        /** Projecting course (cornice, string course or plinth) round a W x D block at Z. */
        void Course(Mesh& Out, const Material& M, const double W, const double D, const double Z, const double Height, const double Projection)
        {
            Out.Box(M, {-W / 2 - Projection, -D / 2 - Projection, Z}, {W / 2 + Projection, D / 2 + Projection, Z + Height});
        }

        /** Dressed corner stones (quoins) up the four corners of a W x D block, alternating long and short. */
        void Quoins(Mesh& Out, const Material& M, const double W, const double D, const double Z0, const double Z1)
        {
            int K = 0;
            for (double Z = Z0; Z + 0.45 <= Z1; Z += 0.5, ++K)
                for (const double Sx : {-1.0, 1.0})
                    for (const double Sy : {-1.0, 1.0})
                    {
                        const double Lx = K % 2 ? 0.9 : 0.5, Ly = K % 2 ? 0.5 : 0.9;
                        const double Cx = Sx * W / 2, Cy = Sy * D / 2;
                        Out.Box(M, {std::min(Cx, Cx - Sx * Lx) - (Sx < 0 ? 0.05 : 0.0), std::min(Cy, Cy - Sy * Ly) - (Sy < 0 ? 0.05 : 0.0), Z},
                                {std::max(Cx, Cx - Sx * Lx) + (Sx > 0 ? 0.05 : 0.0), std::max(Cy, Cy - Sy * Ly) + (Sy > 0 ? 0.05 : 0.0), Z + 0.45});
                    }
        }

        /** Dome of radius R springing at Z (Rise tall) over (X, Y), with Rings bands so it shades smoothly. */
        void Dome(Mesh& Out, const Material& M, const double X, const double Y, const double Z, const double R, const double Rise, const int Sides)
        {
            std::vector<Ring> Rings;
            for (int K = 0; K <= 8; ++K)
            {
                const double A = Pi / 2 * K / 8.0;
                const double Radius = std::max(R * Cos(A), 0.04);
                Rings.push_back(Ellipse(X, Y, Z + Rise * Sin(A), Radius, Radius, Sides));
            }
            Out.Loft(M, Rings, true);
        }

        /** Iron (or gilt) cross standing on (X, Y, Z). */
        void CrossOn(Mesh& Out, const Material& M, const double X, const double Y, const double Z, const double Height)
        {
            Out.Box(M, {X - 0.08, Y - 0.08, Z}, {X + 0.08, Y + 0.08, Z + Height});
            Out.Box(M, {X - Height * 0.3, Y - 0.07, Z + Height * 0.62}, {X + Height * 0.3, Y + 0.07, Z + Height * 0.74});
        }

        /** Bell tower: a quoined shaft, a string course, a belfry pierced on every face, a cornice, an octagonal cupola. */
        void BellTower(Mesh& Out, const double X, const double Y, const double Side, const double Shaft, const double Belfry)
        {
            Out.Push(Xf::Translate({X, Y, 0.0}));
            Out.Box(Mat(M::Plaster), {-Side / 2, -Side / 2, 0.0}, {Side / 2, Side / 2, Shaft + Belfry});
            Quoins(Out, Mat(M::Stone), Side, Side, 1.0, Shaft);
            Course(Out, Mat(M::Stone), Side, Side, Shaft, 0.5, 0.25);
            for (int Face = 0; Face < 4; ++Face)
            {
                // every face of the belfry: an arched opening onto the bells' dark
                Out.Push(Xf::RotateZ(Pi / 2 * Face) * Xf::Translate({0.0, -Side / 2, 0.0}));
                ArchedOpening(Out, Mat(M::Stone), Mat(M::StoneDark), 0.0, Shaft + 1.2, Side * 0.42, Belfry - 2.4 - Side * 0.21);
                Out.Pop();
            }
            Course(Out, Mat(M::Stone), Side, Side, Shaft + Belfry, 0.7, 0.4);
            const double Top = Shaft + Belfry + 0.7;
            Out.Tube(Mat(M::Plaster), {0.0, 0.0, Top}, {0.0, 0.0, Top + 2.6}, Side * 0.4, Side * 0.4, 8);
            Out.Tube(Mat(M::Stone), {0.0, 0.0, Top + 2.6}, {0.0, 0.0, Top + 2.9}, Side * 0.44, Side * 0.44, 8);
            Dome(Out, Mat(M::RoofTile), 0.0, 0.0, Top + 2.9, Side * 0.42, Side * 0.36, 8);
            Out.Tube(Mat(M::Brass), {0.0, 0.0, Top + 2.9 + Side * 0.36 - 0.1}, {0.0, 0.0, Top + 2.9 + Side * 0.36 + 0.9}, 0.35, 0.18, 8);
            CrossOn(Out, Mat(M::Iron), 0.0, 0.0, Top + 3.7 + Side * 0.36, 1.8);
            // the corners of the cornice carry small pinnacles
            for (const double Sx : {-1.0, 1.0})
                for (const double Sy : {-1.0, 1.0})
                    Out.Tube(Mat(M::Stone), {Sx * (Side / 2 - 0.1), Sy * (Side / 2 - 0.1), Top}, {Sx * (Side / 2 - 0.1), Sy * (Side / 2 - 0.1), Top + 1.6}, 0.32, 0.03, 6);
            Out.Pop();
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

        /**
         * One salvaged-timber shanty storey (TC6): walls of odd boards (driftwood, ship's planking still painted, dark
         * timber) nailed over a frame, patched; a doorway with a sailcloth curtain on -Y, a shuttered hole for a window.
         */
        void ShantyStorey(Mesh& Out, Rng& R, const double X0, const double X1, const double Y0, const double Y1, const double Z0, const double Z1, const bool Door)
        {
            const M Boards[] = {M::Driftwood, M::TimberDark, M::Hull, M::Deck, M::HullBand, M::DeckWorn};
            const struct { V3 A, B, N; } Walls[4] = {{{X0, Y0, 0}, {X1, Y0, 0}, {0, -1, 0}}, {{X1, Y0, 0}, {X1, Y1, 0}, {1, 0, 0}}, {{X1, Y1, 0}, {X0, Y1, 0}, {0, 1, 0}}, {{X0, Y1, 0}, {X0, Y0, 0}, {-1, 0, 0}}};
            for (int F = 0; F < 4; ++F)
            {
                const V3 Along = Normalize(Walls[F].B - Walls[F].A);
                const double Len = Length(Walls[F].B - Walls[F].A);
                double At = 0.0;
                while (At < Len - 0.01)
                {
                    const double Wd = std::min(R.Range(0.22, 0.42), Len - At);
                    const double Mid = At + Wd / 2;
                    const bool Gap = Door && F == 0 && std::fabs(Mid - Len / 2) < 0.5;
                    const V3 P = Walls[F].A + Along * Mid;
                    const double Top = Z1 - R.Range(0.0, 0.08);
                    if (!Gap)
                        OrientedBox(Out, Mat(Boards[R.Next() % 6]), {P.X, P.Y, (Z0 + Top) / 2}, Along.X, Along.Y, {Wd / 2 - 0.01, 0.025, (Top - Z0) / 2});
                    else if (Z1 - Z0 > 2.2)
                        OrientedBox(Out, Mat(Boards[R.Next() % 6]), {P.X, P.Y, (Z0 + 2.0 + Top) / 2}, Along.X, Along.Y, {Wd / 2 - 0.01, 0.025, (Top - Z0 - 2.0) / 2});
                    At += Wd;
                }
                // a patch board across the planks, and on the long walls a shuttered window hole
                const V3 Patch = Walls[F].A + Along * (Len * R.Range(0.2, 0.8)) + Walls[F].N * 0.04;
                OrientedBox(Out, Mat(Boards[R.Next() % 6]), {Patch.X, Patch.Y, Z0 + R.Range(0.6, std::max(0.7, Z1 - Z0 - 0.6))}, Along.X, Along.Y, {R.Range(0.4, 0.8), 0.02, 0.1});
                if (Len > 4.0 && F % 2 == 1)
                {
                    const V3 W = Walls[F].A + Along * (Len * 0.5) + Walls[F].N * 0.04;
                    OrientedBox(Out, Mat(M::HullBelow), {W.X, W.Y, Z0 + 1.55}, Along.X, Along.Y, {0.35, 0.02, 0.3});
                    const V3 Sh = W + Along * 0.62 + Walls[F].N * 0.03;
                    OrientedBox(Out, Mat(M::TimberDark), {Sh.X, Sh.Y, Z0 + 1.55}, Along.X, Along.Y, {0.26, 0.02, 0.32});
                }
            }
            for (const V3& C : {V3{X0, Y0, 0}, V3{X1, Y0, 0}, V3{X1, Y1, 0}, V3{X0, Y1, 0}}) Out.Box(Mat(M::TimberDark), {C.X - 0.07, C.Y - 0.07, Z0}, {C.X + 0.07, C.Y + 0.07, Z1});
            if (Door)
            {
                const double Xm = (X0 + X1) / 2;
                Out.Box(Mat(M::HullBelow), {Xm - 0.5, Y0 + 0.02, Z0}, {Xm + 0.5, Y0 + 0.06, Z0 + 2.0});
                Out.Quad(Mat(M::Canvas), {Xm - 0.55, Y0 - 0.06, Z0 + 0.15}, {Xm + 0.1, Y0 - 0.08, Z0 + 0.1}, {Xm + 0.1, Y0 - 0.06, Z0 + 2.0}, {Xm - 0.55, Y0 - 0.06, Z0 + 2.0}, true);
            }
        }

        /**
         * A shed roof of tarred planks from the top of the walls (Z) rising Fall to the front, sailcloth patches on it; the
         * front raised on a fascia board and the sides closed with boards, so no gap shows under it.
         */
        void ShantyRoof(Mesh& Out, Rng& R, const double X0, const double X1, const double Y0, const double Y1, const double Z, const double Fall)
        {
            Out.Box(Mat(M::TimberDark), {X0, Y0 - 0.03, Z - 0.05}, {X1, Y0 + 0.03, Z + Fall});
            Out.Fan(Mat(M::Driftwood), {{X0, Y0, Z}, {X0, Y1, Z}, {X0, Y0, Z + Fall}}, {-1, 0, 0});
            Out.Fan(Mat(M::Driftwood), {{X1, Y1, Z}, {X1, Y0, Z}, {X1, Y0, Z + Fall}}, {1, 0, 0});
            const double O = 0.3;
            const double Top = Z + Fall + Fall * O / (Y1 - Y0);
            const double Low = Z - Fall * O / (Y1 - Y0);
            for (double X = X0 - O; X < X1 + O - 0.01; X += 0.27)
            {
                const double Xe = std::min(X + 0.31, X1 + O), Lift = R.Range(0.0, 0.03);
                Out.Quad(Mat(M::HullBlack), {X, Y0 - O, Top + Lift}, {Xe, Y0 - O, Top + Lift}, {Xe, Y1 + O, Low + Lift}, {X, Y1 + O, Low + Lift}, true);
            }
            for (int K = 0; K < 2; ++K)
            {
                const double Px = R.Range(X0, X1 - 1.2), Py = R.Range(Y0, Y1 - 1.4), Slope = Fall / (Y1 - Y0);
                const double Zp = Z + Fall - Slope * (Py - Y0) + 0.06;
                Out.Quad(Mat(M::Canvas), {Px, Py, Zp}, {Px + 1.2, Py, Zp}, {Px + 1.2, Py + 1.4, Zp - Slope * 1.4}, {Px, Py + 1.4, Zp - Slope * 1.4}, true);
            }
        }

        /** A laundry line from Start to End sagging between, with washing pegged out. */
        void Laundry(Mesh& Out, Rng& R, const V3& Start, const V3& End)
        {
            const V3 Mid = (Start + End) * 0.5 - V3{0, 0, 0.25};
            Out.Tube(Mat(M::Rope), Start, Mid, 0.008, 0.008, 4, false);
            Out.Tube(Mat(M::Rope), Mid, End, 0.008, 0.008, 4, false);
            const M Cloth[] = {M::Linen, M::WoolBlue, M::Canvas, M::Ochre, M::DressGreen, M::Sash};
            const int Count = static_cast<int>(Length(End - Start) / 0.9);
            for (int K = 1; K < Count; ++K)
            {
                const double T = static_cast<double>(K) / Count, Sag = 0.25 * (1.0 - (2 * T - 1) * (2 * T - 1));
                const V3 P = Start + (End - Start) * T - V3{0, 0, Sag};
                const V3 Along = Normalize(End - Start) * R.Range(0.2, 0.32);
                const double Drop = R.Range(0.4, 0.8);
                Out.Quad(Mat(Cloth[R.Next() % 6]), P - Along, P + Along, P + Along - V3{0, 0, Drop}, P - Along - V3{0, 0, Drop * R.Range(0.85, 1.0)}, true);
            }
        }

        Mesh BuildVariant(const Tradition Kind, const int V)
        {
            const Size S = VariantSize(Kind, V);
            const std::string Name = ModelName(Kind, V);
            switch (Kind)
            {
            case Tradition::Colonial:
            {
                // the kit's dressed town house: plaster, tiled roof, doors on -Y; the street-level features of TC1
                // (production/levels/rexa-city/layout.md 4): rejas on the ground windows, a wooden balcony on two-storey houses
                Mesh Out = DressBoxes(Name, {{{-S.W / 2, -S.D / 2, 0.0}, {S.W / 2, S.D / 2, S.H}}});
                const Material& Iron = Mat(M::Iron);
                // the ground windows lie where the dressed house puts them: Len / 2.6 per face, clear of the door on -Y
                const struct { V3 A, B, N; } Faces[4] = {{{-S.W / 2, -S.D / 2, 0}, {S.W / 2, -S.D / 2, 0}, {0, -1, 0}}, {{S.W / 2, -S.D / 2, 0}, {S.W / 2, S.D / 2, 0}, {1, 0, 0}},
                                                         {{S.W / 2, S.D / 2, 0}, {-S.W / 2, S.D / 2, 0}, {0, 1, 0}}, {{-S.W / 2, S.D / 2, 0}, {-S.W / 2, -S.D / 2, 0}, {-1, 0, 0}}};
                for (int F = 0; F < 4; ++F)
                {
                    const V3 Along = Normalize(Faces[F].B - Faces[F].A);
                    const double Len = Length(Faces[F].B - Faces[F].A);
                    const int Count = std::max(1, static_cast<int>(Len / 2.6));
                    for (int I = 0; I < Count; ++I)
                    {
                        const double At = Len * (I + 0.5) / Count;
                        if (F == 0 && At > S.W / 2 - 1.4 && At < S.W / 2 + 1.4) continue;  // the door
                        const V3 C = Faces[F].A + Along * At + Faces[F].N * 0.1;
                        // a reja 0.9 x 1.4: five bars, two rails, set out from the wall
                        for (int Bar = 0; Bar < 5; ++Bar)
                        {
                            const V3 P = C + Along * (-0.4 + 0.2 * Bar);
                            Out.Tube(Iron, {P.X, P.Y, 1.05}, {P.X, P.Y, 2.45}, 0.012, 0.012, 5);
                        }
                        for (const double Z : {1.1, 2.4})
                            Out.Tube(Iron, V3{C.X, C.Y, Z} - Along * 0.47, V3{C.X, C.Y, Z} + Along * 0.47, 0.016, 0.016, 5);
                    }
                }
                if (S.H > 7.0)
                {
                    // the balcony: 0.6 of the frontage wide, 1.1 deep, at +3.4 m, on brackets, a railing; a door onto it
                    const double Half = S.W * 0.3, Y = -S.D / 2, Z = 3.4;
                    Out.Box(Mat(M::Deck), {-Half, Y - 1.1, Z - 0.12}, {Half, Y, Z});
                    for (const double X : {-Half + 0.2, 0.0, Half - 0.2})
                        Out.Tube(Mat(M::TimberDark), {X, Y, Z - 0.9}, {X, Y - 1.0, Z - 0.13}, 0.06, 0.05, 4);
                    const int Posts = static_cast<int>(2.0 * Half / 0.18);
                    for (int K = 0; K <= Posts; ++K)
                    {
                        const double X = -Half + 2.0 * Half * K / Posts;
                        Out.Tube(Mat(M::TimberDark), {X, Y - 1.05, Z}, {X, Y - 1.05, Z + 0.95}, 0.022, 0.022, 4);
                    }
                    for (const double Side : {-1.0, 1.0})
                        for (int K = 0; K <= 5; ++K)
                        {
                            const double Yk = Y - 1.05 * K / 5.0;
                            Out.Tube(Mat(M::TimberDark), {Side * Half, Yk, Z}, {Side * Half, Yk, Z + 0.95}, 0.022, 0.022, 4);
                        }
                    Out.Box(Mat(M::TimberDark), {-Half - 0.03, Y - 1.1, Z + 0.95}, {Half + 0.03, Y - 1.0, Z + 1.03});
                    for (const double Side : {-1.0, 1.0}) Out.Box(Mat(M::TimberDark), {Side * Half - 0.04, Y - 1.1, Z + 0.95}, {Side * Half + 0.04, Y, Z + 1.03});
                    Out.Box(Mat(M::TimberDark), {-0.5, Y - 0.05, Z}, {0.5, Y + 0.02, Z + 2.2});
                }
                return Out;
            }
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
            case Tradition::Street:
            {
                Mesh Out(Name);
                if (V == 0)
                {
                    // a gas lamp ("gas-lit streets", region_02_rexa_city.md 1): a cast-iron post on a stone base, the
                    // lamplighter's crossbar, a glazed lantern under a hood
                    Out.Box(Mat(M::Stone), {-0.3, -0.3, -0.3}, {0.3, 0.3, 0.35});
                    Out.Tube(Mat(M::Iron), {0.0, 0.0, 0.35}, {0.0, 0.0, 0.9}, 0.14, 0.09, 10);
                    Out.Tube(Mat(M::Iron), {0.0, 0.0, 0.9}, {0.0, 0.0, 3.4}, 0.06, 0.05, 10);
                    Out.Box(Mat(M::Iron), {-0.38, -0.03, 3.05}, {0.38, 0.03, 3.11});
                    Out.Tube(Mat(M::Iron), {0.0, 0.0, 3.4}, {0.0, 0.0, 3.5}, 0.16, 0.16, 8);
                    Out.Tube(Mat(M::Glass), {0.0, 0.0, 3.5}, {0.0, 0.0, 3.95}, 0.2, 0.26, 4);
                    for (int K = 0; K < 4; ++K)
                    {
                        const double A = Pi / 4 + Pi / 2 * K;
                        Out.Tube(Mat(M::Iron), {0.2 * Cos(A), 0.2 * Sin(A), 3.5}, {0.26 * Cos(A), 0.26 * Sin(A), 3.95}, 0.012, 0.012, 4, false);
                    }
                    Out.Tube(Mat(M::Iron), {0.0, 0.0, 3.95}, {0.0, 0.0, 4.15}, 0.32, 0.05, 4);
                    Out.Tube(Mat(M::Iron), {0.0, 0.0, 4.15}, {0.0, 0.0, 4.25}, 0.03, 0.03, 6);
                }
                else if (V == 1)
                {
                    // a market stall (layout.md 5.2, 3 x 3 x 3): four posts, a counter to the front (-Y), a canvas awning
                    for (const double X : {-1.4, 1.4})
                        for (const double Y : {-1.4, 1.4}) Out.Tube(Mat(M::Timber), {X, Y, 0.0}, {X, Y, Y < 0 ? 2.6 : 2.2}, 0.05, 0.05, 6);
                    Out.Box(Mat(M::Timber), {-1.45, -1.5, 0.85}, {1.45, -0.9, 0.95});
                    Out.Box(Mat(M::TimberDark), {-1.4, -1.4, 0.0}, {1.4, -1.3, 0.85});
                    Out.Quad(Mat(M::Canvas), {-1.6, -1.9, 2.55}, {1.6, -1.9, 2.55}, {1.6, 1.6, 2.2}, {-1.6, 1.6, 2.2}, true);
                    for (int K = 0; K < 3; ++K) Crate(Out, {-1.2 + 0.9 * K, 0.3, 0.0}, {-0.5 + 0.9 * K, 1.0, 0.6});
                }
                else
                {
                    // a bench of stone ends and a timber seat
                    for (const double X : {-0.75, 0.75}) Out.Box(Mat(M::Stone), {X - 0.1, -0.22, 0.0}, {X + 0.1, 0.22, 0.42});
                    Out.Box(Mat(M::Timber), {-0.9, -0.25, 0.42}, {0.9, 0.25, 0.48});
                    Out.Box(Mat(M::Timber), {-0.9, 0.2, 0.55}, {0.9, 0.26, 0.85});
                }
                return Out;
            }
            case Tradition::Ruin:
            {
                // TC7 (layout.md 4, 8): a roofless shell, some 60% of its walls still standing, broken to ragged tops; plaster
                // fallen to the stone beneath; vines down the walls, a rubble apron round it; in R2 a tree has grown up
                // through where the roof was
                Mesh Out(Name);
                Rng R(HashString(Name));
                const double X0 = -S.W / 2, X1 = S.W / 2, Y0 = -S.D / 2, Y1 = S.D / 2, T = 0.45;
                Out.Box(Mat(M::StoneDark), {X0 - 0.1, Y0 - 0.1, -0.5}, {X1 + 0.1, Y1 + 0.1, 0.12});
                Out.Box(Mat(M::Dirt), {X0 + T, Y0 + T, 0.12}, {X1 - T, Y1 - T, 0.16});
                const struct { V3 A, B, N; } Walls[4] = {{{X0, Y0, 0}, {X1, Y0, 0}, {0, -1, 0}}, {{X1, Y0, 0}, {X1, Y1, 0}, {1, 0, 0}}, {{X1, Y1, 0}, {X0, Y1, 0}, {0, 1, 0}}, {{X0, Y1, 0}, {X0, Y0, 0}, {-1, 0, 0}}};
                for (int F = 0; F < 4; ++F)
                {
                    const V3 Along = Normalize(Walls[F].B - Walls[F].A);
                    const double Len = Length(Walls[F].B - Walls[F].A);
                    double Top = S.H * R.Range(0.6, 1.0);
                    for (double At = 0.0; At < Len - 0.01; At += 0.5)
                    {
                        // the top wanders; a third of the wall is down to its stump
                        Top = Clamp(Top + R.Range(-0.7, 0.6), 0.3, S.H);
                        const bool Down = (static_cast<int>(At / 0.5) + F * 3) % 9 < 3 && R.Unit() < 0.85;
                        const double H = Down ? R.Range(0.2, 0.7) : Top;
                        const bool Door = F == 0 && std::fabs(At + 0.25 - Len / 2) < 0.6;
                        if (Door) continue;
                        const V3 P = Walls[F].A + Along * (At + 0.25) - Walls[F].N * (T / 2);
                        OrientedBox(Out, Mat(M::Stone), {P.X, P.Y, 0.12 + H / 2}, Along.X, Along.Y, {0.255, T / 2, H / 2});
                        if (!Down && H > 1.2 && R.Unit() < 0.55)
                        {
                            const V3 Q = P + Walls[F].N * (T / 2 + 0.012);
                            const double Hp = H * R.Range(0.4, 0.9);
                            OrientedBox(Out, Mat(R.Unit() < 0.5 ? M::PlasterOchre : M::Plaster), {Q.X, Q.Y, 0.12 + Hp / 2}, Along.X, Along.Y, {0.25, 0.012, Hp / 2});
                        }
                        if (!Down && R.Unit() < 0.3)
                        {
                            const V3 Q = P + Walls[F].N * (T / 2 + 0.05);
                            Out.Tube(Mat(M::LeavesDark), {Q.X, Q.Y, 0.12 + H}, {Q.X + R.Range(-0.3, 0.3), Q.Y, R.Range(0.2, 0.8)}, 0.1, 0.05, 5);
                        }
                    }
                }
                for (int K = 0; K < 14; ++K)
                {
                    const double A = 2.0 * Pi * K / 14 + R.Range(-0.2, 0.2), Rr = std::max(S.W, S.D) / 2 + R.Range(-0.4, 1.2);
                    Rock(Out, Mat(M::Stone), {Rr * Cos(A) * S.W / std::max(S.W, S.D), Rr * Sin(A) * S.D / std::max(S.W, S.D), 0.0}, R.Range(0.3, 0.65), R.Range(0.35, 0.7), 500u + static_cast<std::uint32_t>(K));
                }
                if (V == 2)
                {
                    Out.Tube(Mat(M::Bark), {0.6, 0.8, 0.0}, {0.4, 0.5, 4.5}, 0.32, 0.22, 10);
                    Out.Tube(Mat(M::Bark), {0.4, 0.5, 4.5}, {1.6, 0.2, 7.2}, 0.18, 0.1, 8);
                    Out.Tube(Mat(M::Bark), {0.4, 0.5, 4.5}, {-1.0, 1.4, 7.0}, 0.16, 0.08, 8);
                    for (const V3& C : {V3{0.4, 0.5, 7.4}, V3{1.9, 0.0, 7.6}, V3{-1.2, 1.6, 7.3}, V3{0.6, 2.0, 8.1}})
                        Rock(Out, Mat(M::Leaves), C - V3{0, 0, 1.0}, R.Range(1.8, 2.6), R.Range(1.6, 2.2), 700u + static_cast<std::uint32_t>(C.X * 10.0 + 50.0));
                    for (int K = 0; K < 6; ++K)
                    {
                        const double A = 2.0 * Pi * K / 6;
                        Out.Tube(Mat(M::Bark), {0.6, 0.8, 0.2}, {0.6 + 1.4 * Cos(A), 0.8 + 1.4 * Sin(A), -0.1}, 0.14, 0.05, 6);
                    }
                }
                return Out;
            }
            case Tradition::Shanty:
            {
                // TC6 (layout.md 4): salvaged ship timber, sailcloth awnings, plank-and-tar roofs, laundry lines; SH2 and SH3
                // two storeys with a 0.6 m jetty over the street, SH5 the long tenement with a gallery and an outside stair
                Mesh Out(Name);
                Rng R(HashString(Name));
                const double X0 = -S.W / 2, X1 = S.W / 2, Y0 = -S.D / 2, Y1 = S.D / 2;
                Out.Box(Mat(M::StoneDark), {X0 - 0.05, Y0 - 0.05, -0.4}, {X1 + 0.05, Y1 + 0.05, 0.15});
                if (V == 5)
                {
                    // the tenement: two storeys of rooms, five doors below, a gallery along the front above, a stair up
                    ShantyStorey(Out, R, X0, X1, Y0, Y1, 0.15, 3.1, false);
                    ShantyStorey(Out, R, X0, X1, Y0, Y1, 3.1, S.H - 0.4, false);
                    for (int K = 0; K < 5; ++K)
                    {
                        const double Yd = Y0 + S.D * (K + 0.5) / 5;
                        for (const double Z : {0.15, 3.25})
                        {
                            Out.Box(Mat(M::HullBelow), {X0 - 0.06, Yd - 0.45, Z}, {X0 - 0.02, Yd + 0.45, Z + 1.95});
                            Out.Quad(Mat(M::Canvas), {X0 - 0.08, Yd - 0.5, Z + 0.1}, {X0 - 0.08, Yd + 0.1, Z + 0.05}, {X0 - 0.08, Yd + 0.1, Z + 1.95}, {X0 - 0.08, Yd - 0.5, Z + 1.95}, true);
                        }
                    }
                    Out.Box(Mat(M::Deck), {X0 - 1.3, Y0, 3.0}, {X0, Y1, 3.12});
                    for (double Y = Y0 + 0.2; Y < Y1; Y += 2.2) Out.Tube(Mat(M::Driftwood), {X0 - 1.2, Y, 0.0}, {X0 - 1.2, Y, 4.1}, 0.07, 0.06, 6);
                    Out.Tube(Mat(M::TimberDark), {X0 - 1.25, Y0, 4.1}, {X0 - 1.25, Y1, 4.1}, 0.04, 0.04, 4);
                    for (int K = 0; K < 10; ++K)
                    {
                        const double T = K / 10.0;
                        Out.Box(Mat(M::Deck), {X0 - 1.25, Y1 + 2.8 * (1.0 - T) - 0.15, 3.0 * T}, {X0 - 0.05, Y1 + 2.8 * (1.0 - T) + 0.15, 3.0 * T + 0.08});
                    }
                    ShantyRoof(Out, R, X0, X1, Y0, Y1, S.H - 0.4, 0.9);
                    for (int K = 0; K < 3; ++K)
                    {
                        const double Y = Y0 + 3.0 + 7.0 * K;
                        Laundry(Out, R, {X0 - 1.25, Y, 4.0}, {X0 - 1.25, Y + 5.5, 4.0});
                    }
                }
                else
                {
                    const bool Upper = S.H > 4.0;
                    const double Ground = Upper ? 2.7 : S.H - 0.35;
                    ShantyStorey(Out, R, X0, X1, Y0, Y1, 0.15, Ground, true);
                    if (Upper)
                    {
                        // the jetty: the upper storey hangs 0.6 m out over the street on brackets
                        Out.Box(Mat(M::Deck), {X0, Y0 - 0.6, Ground - 0.12}, {X1, Y1, Ground});
                        for (const double X : {X0 + 0.2, X1 - 0.2}) Out.Tube(Mat(M::TimberDark), {X, Y0, Ground - 0.7}, {X, Y0 - 0.55, Ground - 0.12}, 0.05, 0.05, 4);
                        ShantyStorey(Out, R, X0, X1, Y0 - 0.6, Y1, Ground, S.H - 0.4, false);
                    }
                    ShantyRoof(Out, R, X0, X1, Upper ? Y0 - 0.6 : Y0, Y1, S.H - 0.4, 0.6);
                    // the sailcloth awning over the door on two poles
                    const double Xa = 1.4;
                    for (const double X : {-Xa, Xa}) Out.Tube(Mat(M::Driftwood), {X, Y0 - 1.5, 0.0}, {X, Y0 - 1.5, 2.0}, 0.04, 0.035, 5);
                    Out.Quad(Mat(M::Canvas), {-Xa - 0.1, Y0 - 1.55, 2.0}, {Xa + 0.1, Y0 - 1.55, 2.0}, {Xa + 0.1, Y0 - 0.02, 2.35}, {-Xa - 0.1, Y0 - 0.02, 2.35}, true);
                    if (V % 2 == 0) Laundry(Out, R, {X1, Y0 + 0.4, 2.2}, {X1 + 3.2, Y0 - 1.4, 2.1});
                    if (V == 4) Barrel(Out, {X0 - 0.5, Y0 - 0.6, 0.0}, 0.9, 0.3);
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
                    // La Catedral de los Huesos (region_02_rexa_city.md 2.3: "quiet, oppressive, holy; painted gold over
                    // rot"): a Spanish colonial basilica. Twin bell towers flank a baroque frontispiece on the west front
                    // (-Y); a tall nave between lower aisles, a dome on a drum over the crossing, a half-round apse. Its
                    // plaster is ochre gone grey at the foot; the only gold is the lanterns and the finials.
                    const double Front = -S.D / 2, Back = S.D / 2, Nave = 6.0, Aisle = 13.0, NaveH = 16.0, AisleH = 10.0;
                    constexpr int Bays = 6;
                    const double Bay = (Back - 6.0 - (Front + 7.0) - 1.2) / Bays, Crossing = Back - 12.0;
                    // plinth all round, the stone gone dark with damp
                    Out.Box(Mat(M::StoneDark), {-Aisle - 0.3, Front + 6.7, -0.6}, {Aisle + 0.3, Back - 5.7, 1.1});
                    // nave and aisles
                    Out.Box(Mat(M::PlasterOchre), {-Nave, Front + 1.0, 0.0}, {Nave, Back - 6.0, NaveH});
                    for (const double Side : {-1.0, 1.0})
                    {
                        Out.Box(Mat(M::PlasterOchre), {Side > 0 ? Nave : -Aisle, Front + 7.0, 0.0}, {Side > 0 ? Aisle : -Nave, Back - 6.0, AisleH});
                        // the aisle's lean-to roof, from under the clerestory down over its wall
                        const double In = Side * Nave, OutX = Side * (Aisle + 0.5);
                        Out.Quad(Mat(M::RoofTile), {In, Front + 6.6, AisleH + 2.2}, {In, Back - 5.6, AisleH + 2.2}, {OutX, Back - 5.6, AisleH - 0.15},
                                 {OutX, Front + 6.6, AisleH - 0.15}, true);
                        Out.Box(Mat(M::Stone), {Side > 0 ? Aisle - 0.05 : -Aisle - 0.45, Front + 6.8, AisleH - 0.6}, {Side > 0 ? Aisle + 0.45 : -Aisle + 0.05, Back - 5.8, AisleH - 0.15});
                        // the aisle wall: a pilaster between every bay and a round-headed window in each
                        Out.Push(Xf::Translate({Side * Aisle, 0.0, 0.0}) * Xf::RotateZ(Side * Pi / 2));
                        for (int K = 0; K <= Bays; ++K)
                        {
                            const double Along = Side * (Front + 7.6 + K * Bay - (Front + Back) / 2);
                            Out.Box(Mat(M::Plaster), {Along - 0.45, -0.35, 1.1}, {Along + 0.45, 0.0, AisleH - 0.6});
                            if (K < Bays) ArchedOpening(Out, Mat(M::Stone), Mat(M::Glass), Along + Side * Bay / 2, 3.2, 1.5, 3.4);
                        }
                        Out.Pop();
                        // clerestory: an oculus per bay above the aisle roof
                        Out.Push(Xf::Translate({Side * Nave, 0.0, 0.0}) * Xf::RotateZ(Side * Pi / 2));
                        for (int K = 0; K < Bays; ++K) Oculus(Out, Mat(M::Stone), Mat(M::Glass), Side * (Front + 7.6 + (K + 0.5) * Bay - (Front + Back) / 2), 14.0, 0.7);
                        Out.Pop();
                    }
                    Out.Box(Mat(M::Stone), {-Nave - 0.4, Front + 1.0, NaveH - 0.7}, {Nave + 0.4, Back - 6.0, NaveH});
                    Mesh Roof(Name + "_Roof");
                    Gable(Roof, Mat(M::RoofTile), Mat(M::PlasterOchre), Back - Front - 7.0, 2.0 * Nave, NaveH, 4.5, 0.4);
                    Out.Push(Xf::Translate({0.0, (Front + 1.0 + Back - 6.0) / 2, 0.0}) * Xf::RotateZ(Pi / 2));  // ridge along the nave
                    Out.Append(Roof);
                    Out.Pop();
                    // the crossing: an octagonal drum with windows, a tiled dome, a gilt lantern
                    Out.Tube(Mat(M::Plaster), {0.0, Crossing, NaveH}, {0.0, Crossing, NaveH + 5.0}, 5.4, 5.4, 16);
                    for (int K = 0; K < 8; ++K)
                    {
                        const double A = Pi / 4 * K + Pi / 8;
                        Out.Push(Xf::Translate({5.45 * Cos(A), Crossing + 5.45 * Sin(A), 0.0}) * Xf::RotateZ(A + Pi / 2));
                        ArchPanel(Out, Mat(M::Glass), 0.0, 0.0, NaveH + 1.2, 1.1, 2.2);
                        Out.Pop();
                    }
                    Out.Tube(Mat(M::Stone), {0.0, Crossing, NaveH + 5.0}, {0.0, Crossing, NaveH + 5.6}, 5.9, 5.9, 16);
                    Dome(Out, Mat(M::RoofTile), 0.0, Crossing, NaveH + 5.6, 5.6, 5.2, 24);
                    Out.Tube(Mat(M::Brass), {0.0, Crossing, NaveH + 10.6}, {0.0, Crossing, NaveH + 12.4}, 1.0, 1.0, 8);
                    Dome(Out, Mat(M::Brass), 0.0, Crossing, NaveH + 12.4, 1.15, 1.1, 8);
                    CrossOn(Out, Mat(M::Brass), 0.0, Crossing, NaveH + 13.4, 2.4);
                    // the apse: half round under a conical roof
                    Out.Tube(Mat(M::PlasterOchre), {0.0, Back - 6.0, 0.0}, {0.0, Back - 6.0, NaveH - 2.0}, 6.0, 6.0, 20);
                    Out.Tube(Mat(M::Stone), {0.0, Back - 6.0, NaveH - 2.6}, {0.0, Back - 6.0, NaveH - 2.0}, 6.4, 6.4, 20);
                    Out.Tube(Mat(M::RoofTile), {0.0, Back - 6.0, NaveH - 2.0}, {0.0, Back - 6.0, NaveH + 2.6}, 6.6, 0.2, 20, false);
                    // west front: two bell towers and between them the frontispiece
                    for (const double Side : {-1.0, 1.0}) BellTower(Out, Side * (Aisle - 3.5), Front + 3.5, 7.0, 17.5, 7.0);
                    Out.Box(Mat(M::Plaster), {-Nave - 0.5, Front + 0.3, 0.0}, {Nave + 0.5, Front + 1.3, 19.5});
                    Out.Push(Xf::Translate({0.0, Front + 0.3, 0.0}));
                    Out.Box(Mat(M::StoneDark), {-Nave - 0.6, -0.15, -0.6}, {Nave + 0.6, 0.05, 1.1});
                    // the portal: a stone arch on paired columns, the great door, a niche either side, the rose above
                    ArchedOpening(Out, Mat(M::Stone), Mat(M::TimberDark), 0.0, 1.1, 3.4, 4.4);
                    for (const double X : {-2.6, 2.6})
                        Out.Tube(Mat(M::Stone), {X, -0.45, 1.1}, {X, -0.45, 7.4}, 0.32, 0.28, 10);
                    Out.Box(Mat(M::Stone), {-3.3, -0.8, 7.4}, {3.3, 0.0, 8.0});
                    for (const double X : {-4.4, 4.4}) ArchedOpening(Out, Mat(M::Stone), Mat(M::StoneDark), X, 2.4, 1.1, 2.4);
                    Out.Box(Mat(M::Stone), {-Nave - 0.6, -0.35, 9.0}, {Nave + 0.6, 0.0, 9.5});
                    Oculus(Out, Mat(M::Stone), Mat(M::Glass), 0.0, 13.0, 2.0);
                    Out.Box(Mat(M::Stone), {-Nave - 0.6, -0.4, 17.0}, {Nave + 0.6, 0.0, 17.6});
                    // a curved baroque crown over the frontispiece, scrolls at its feet, a gilt cross on top
                    std::vector<V3> Crown;
                    for (int K = 0; K <= 12; ++K)
                    {
                        const double A = Pi * K / 12.0;
                        Crown.push_back({4.2 * Cos(A), -0.02, 19.5 + 2.6 * Sin(A)});
                    }
                    Out.Fan(Mat(M::Plaster), Crown, {0.0, -1.0, 0.0});
                    Out.Push(Xf::Translate({0.0, 1.0, 0.0}) * Xf::RotateZ(Pi));
                    Out.Fan(Mat(M::Plaster), Crown, {0.0, -1.0, 0.0});
                    Out.Pop();
                    for (const double X : {-5.4, 5.4})
                    {
                        Out.Push(Xf::Translate({X, 0.5, 19.5}) * Xf::RotateY(Pi / 2));
                        Out.Tube(Mat(M::Stone), {0.0, 0.0, -0.6}, {0.0, 0.0, 0.6}, 0.9, 0.9, 12);
                        Out.Pop();
                    }
                    CrossOn(Out, Mat(M::Brass), 0.0, 0.5, 22.1, 2.0);
                    // three steps up to the doors
                    for (int K = 0; K < 3; ++K) Out.Box(Mat(M::Stone), {-7.0 + K * 0.4, -2.4 + K * 0.6, -0.6}, {7.0 - K * 0.4, 0.0, 0.35 * (K + 1)});
                    Out.Pop();
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
                case 5:
                {
                    // the Old Spanish Lighthouse of the Cliffside (region_02_rexa_city.md 2.6; layout.md LM4, 5.6): a
                    // stone plinth, a tapering masonry shaft to a gallery at +34 m, a glazed lantern and its cap. The
                    // door rotted shut long ago, so the climb is outside: a buttress, a timber scaffold, a broken
                    // exterior stair with three gaps, and an iron ladder to the gallery hatch.
                    Out.Box(Mat(M::StoneDark), {-4.5, -4.5, -0.8}, {4.5, 4.5, 6.0});
                    Course(Out, Mat(M::Stone), 9.0, 9.0, 5.6, 0.5, 0.25);
                    Out.Tube(Mat(M::Stone), {0.0, 0.0, 6.0}, {0.0, 0.0, 34.0}, 3.7, 2.6, 24);
                    for (const double Z : {13.5, 21.5, 28.0})
                    {
                        const double Rz = 3.7 - 1.1 * (Z - 6.0) / 28.0;
                        Out.Tube(Mat(M::StoneDark), {0.0, 0.0, Z}, {0.0, 0.0, Z + 0.35}, Rz + 0.18, Rz + 0.18, 24);
                    }
                    // the rotted door on the plinth's -Y face, and the buttress the climb starts on
                    Out.Box(Mat(M::TimberDark), {-0.8, -4.58, 0.0}, {0.8, -4.4, 2.6});
                    for (int K = 0; K < 6; ++K) Out.Box(Mat(M::Stone), {3.0 - 0.0, -1.2, -0.8}, {4.5 + 1.0 * (6 - K) / 6.0 * 2.0, 1.2, K + 1.0});
                    // R1: timber scaffold round the shaft from +6 to +14, a 0.9 m ledge of planks every 2 m
                    for (int K = 0; K < 4; ++K)
                    {
                        const double Z = 7.5 + 2.0 * K, Rz = 3.7 - 1.1 * (Z - 6.0) / 28.0;
                        Out.Tube(Mat(M::Deck), {0.0, 0.0, Z}, {0.0, 0.0, Z + 0.12}, Rz + 0.9, Rz + 0.9, 24);
                    }
                    for (int K = 0; K < 6; ++K)
                    {
                        const double A = Pi / 3 * K;
                        Out.Tube(Mat(M::TimberDark), {4.5 * Cos(A), 4.5 * Sin(A), 6.0}, {4.4 * Cos(A), 4.4 * Sin(A), 14.2}, 0.1, 0.09, 6);
                    }
                    // R2: the broken exterior stair from +14 to +22 winding once round the shaft, with three 1.5 m gaps
                    for (int K = 0; K < 40; ++K)
                    {
                        if (K == 9 || K == 10 || K == 21 || K == 22 || K == 31 || K == 32) continue;
                        const double A = 2.0 * Pi * K / 40.0, Z = 14.2 + 0.2 * K, Rz = 3.7 - 1.1 * (Z - 6.0) / 28.0;
                        OrientedBox(Out, Mat(M::Stone), {(Rz + 0.55) * Cos(A), (Rz + 0.55) * Sin(A), Z}, -Sin(A), Cos(A), {0.32, 0.6, 0.1});
                    }
                    // R3: a collapsed bay in the shaft between +22 and +28 (the dark of the interior stair), R4: the ladder
                    Out.Push(Xf::Translate({0.0, -2.95, 0.0}));
                    ArchPanel(Out, Mat(M::StoneDark), 0.0, -0.05, 22.4, 1.6, 4.0);
                    Out.Pop();
                    for (const double X : {-0.25, 0.25}) Out.Box(Mat(M::Iron), {X - 0.03, 2.75, 28.0}, {X + 0.03, 2.82, 34.4});
                    for (int K = 0; K < 20; ++K) Out.Box(Mat(M::Iron), {-0.25, 2.76, 28.2 + 0.3 * K}, {0.25, 2.81, 28.24 + 0.3 * K});
                    // the gallery: a stone deck on corbels, an iron rail 1.1 m high
                    Out.Tube(Mat(M::Stone), {0.0, 0.0, 33.6}, {0.0, 0.0, 34.0}, 4.2, 4.2, 24);
                    for (int K = 0; K < 12; ++K)
                    {
                        const double A = Pi / 6 * K;
                        OrientedBox(Out, Mat(M::Stone), {3.4 * Cos(A), 3.4 * Sin(A), 33.0}, Cos(A), Sin(A), {0.7, 0.18, 0.4});
                    }
                    Out.Tube(Mat(M::Iron), {0.0, 0.0, 35.05}, {0.0, 0.0, 35.12}, 4.15, 4.15, 24, false);
                    for (int K = 0; K < 24; ++K)
                    {
                        const double A = Pi / 12 * K;
                        Out.Tube(Mat(M::Iron), {4.15 * Cos(A), 4.15 * Sin(A), 34.0}, {4.15 * Cos(A), 4.15 * Sin(A), 35.1}, 0.03, 0.03, 4, false);
                    }
                    // the lantern: glass between iron mullions, an iron cap with a brass ball and a vane
                    Out.Tube(Mat(M::StoneDark), {0.0, 0.0, 34.0}, {0.0, 0.0, 34.9}, 2.3, 2.3, 16);
                    Out.Tube(Mat(M::Glass), {0.0, 0.0, 34.9}, {0.0, 0.0, 39.2}, 2.2, 2.2, 12);
                    for (int K = 0; K < 12; ++K)
                    {
                        const double A = Pi / 6 * K;
                        Out.Tube(Mat(M::Iron), {2.22 * Cos(A), 2.22 * Sin(A), 34.9}, {2.22 * Cos(A), 2.22 * Sin(A), 39.2}, 0.05, 0.05, 4, false);
                    }
                    Out.Tube(Mat(M::Iron), {0.0, 0.0, 39.2}, {0.0, 0.0, 39.5}, 2.5, 2.5, 16);
                    Dome(Out, Mat(M::Iron), 0.0, 0.0, 39.5, 2.4, 1.8, 16);
                    Dome(Out, Mat(M::Brass), 0.0, 0.0, 41.2, 0.35, 0.35, 8);
                    Out.Tube(Mat(M::Iron), {0.0, 0.0, 41.4}, {0.0, 0.0, 42.6}, 0.04, 0.04, 4);
                    Out.Quad(Mat(M::Iron), {0.0, 0.0, 42.4}, {0.9, 0.0, 42.4}, {0.9, 0.0, 42.0}, {0.0, 0.0, 42.0}, true);
                    break;
                }
                case 4:
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
                case 6:
                {
                    // LM6, the tavern La Sirena Negra (layout.md 4, B1): 18 x 16 x 9, a wrap balcony, the black mermaid
                    // figurehead on a 1.2 m bracket at +4.5 over the door, a 2.4 x 1.6 painted sign; lanterns at the door.
                    // Its terrace on the apron is the harbour's to furnish.
                    Out.Append(DressBoxes(Name + "_House", {{{-S.W / 2, -S.D / 2, 0.0}, {S.W / 2, S.D / 2, S.H}}}));
                    const double F = -S.D / 2, Z = 3.4;
                    // the wrap balcony: down both sides and along the front, open over the door where the mermaid hangs
                    const auto Rail = [&](const V3& A, const V3& B, const V3& Out_)
                    {
                        const V3 Along = B - A;
                        const double Len = Length(Along);
                        const V3 Lo = A + Out_ * 1.1, Hi = B + Out_ * 1.1;
                        const V3 Min{std::min(A.X, Hi.X), std::min(A.Y, Hi.Y), Z - 0.12}, Max{std::max(A.X, Hi.X), std::max(A.Y, Hi.Y), Z};
                        Out.Box(Mat(M::Deck), Min, Max);
                        const int Posts = static_cast<int>(Len / 0.2);
                        for (int K = 0; K <= Posts; ++K)
                        {
                            const V3 P = Lo + (Hi - Lo) * (static_cast<double>(K) / Posts);
                            Out.Tube(Mat(M::TimberDark), {P.X, P.Y, Z}, {P.X, P.Y, Z + 0.95}, 0.022, 0.022, 4);
                        }
                        Out.Tube(Mat(M::TimberDark), {Lo.X, Lo.Y, Z + 1.0}, {Hi.X, Hi.Y, Z + 1.0}, 0.05, 0.05, 4);
                        for (int K = 0; K <= static_cast<int>(Len / 2.5); ++K)
                        {
                            const V3 P = A + Along * (std::min(1.0, 2.5 * K / Len));
                            Out.Tube(Mat(M::TimberDark), V3{P.X, P.Y, Z - 0.9}, V3{P.X, P.Y, Z - 0.12} + Out_ * 0.95, 0.06, 0.05, 4);
                        }
                    };
                    Rail({-S.W / 2, F, 0}, {-1.6, F, 0}, {0, -1, 0});
                    Rail({1.6, F, 0}, {S.W / 2, F, 0}, {0, -1, 0});
                    Rail({-S.W / 2, F, 0}, {-S.W / 2, S.D / 2 - 1.0, 0}, {-1, 0, 0});
                    Rail({S.W / 2, F, 0}, {S.W / 2, S.D / 2 - 1.0, 0}, {1, 0, 0});
                    for (const double X : {-4.5, 4.5}) Out.Box(Mat(M::TimberDark), {X - 0.5, F - 0.05, Z}, {X + 0.5, F + 0.02, Z + 2.2});
                    // the double door and its lanterns
                    Out.Box(Mat(M::TimberDark), {-1.0, F - 0.07, 0.0}, {1.0, F + 0.02, 2.6});
                    Out.Box(Mat(M::Iron), {-0.03, F - 0.09, 0.2}, {0.03, F - 0.06, 2.5});
                    Material Lamp = Mat(M::Glass);
                    Lamp.Name = "M_Art_Tavern_Lantern";
                    Lamp.BaseColor = {1.0, 0.78, 0.42, 1.0};
                    Lamp.Emissive = {1.0, 0.7, 0.35};
                    Lamp.EmissiveStrength = 3.0;
                    for (const double X : {-1.6, 1.6})
                    {
                        Out.Tube(Mat(M::Iron), {X, F, 2.9}, {X, F - 0.35, 2.9}, 0.02, 0.02, 4);
                        Out.Box(Lamp, {X - 0.12, F - 0.47, 2.5}, {X + 0.12, F - 0.23, 2.85});
                        Out.Box(Mat(M::Iron), {X - 0.14, F - 0.49, 2.85}, {X + 0.14, F - 0.21, 2.92});
                    }
                    // the black mermaid: a ship's figurehead carved and painted black, leaning out from her bracket
                    Material Black = Mat(M::HullBlack);
                    Black.Name = "M_Art_Tavern_Mermaid";
                    Black.BaseColor = {0.035, 0.035, 0.04, 1.0};
                    Black.Roughness = 0.35;
                    Out.Box(Mat(M::TimberDark), {-0.12, F - 1.2, 4.45}, {0.12, F, 4.65});
                    Out.Tube(Mat(M::TimberDark), {0.0, F, 3.7}, {0.0, F - 1.0, 4.45}, 0.07, 0.07, 4);
                    const V3 Waist{0.0, F - 1.25, 4.05}, Chest{0.0, F - 1.5, 4.55}, Head{0.0, F - 1.62, 4.86};
                    Out.Tube(Black, Waist, Chest, 0.15, 0.19, 10);
                    Out.Tube(Black, Chest, Head - V3{0, 0, 0.12}, 0.19, 0.07, 10);
                    Dome(Out, Black, Head.X, Head.Y, Head.Z - 0.02, 0.12, 0.14, 10);
                    Out.Push(Xf::Translate(Head - V3{0, 0, 0.02}) * Xf::Scale({1.0, 1.0, -1.0}));
                    Dome(Out, Black, 0.0, 0.0, 0.0, 0.12, 0.12, 10);
                    Out.Pop();
                    for (int K = 0; K < 7; ++K)
                    {
                        const double Xh = -0.12 + 0.04 * K;
                        Out.Tube(Black, {Xh, Head.Y + 0.05, Head.Z + 0.06}, {Xh * 1.6, F - 1.25, 4.5}, 0.03, 0.02, 5);
                        Out.Tube(Black, {Xh * 1.6, F - 1.25, 4.5}, {Xh * 1.8, F - 1.05, 4.1}, 0.02, 0.008, 5);
                    }
                    for (const double Sd : {-1.0, 1.0})
                    {
                        Out.Tube(Black, Chest + V3{Sd * 0.17, 0.02, 0.0}, Chest + V3{Sd * 0.3, -0.12, -0.32}, 0.05, 0.04, 6);
                        Out.Tube(Black, Chest + V3{Sd * 0.3, -0.12, -0.32}, Chest + V3{Sd * 0.12, -0.2, -0.5}, 0.04, 0.03, 6);
                    }
                    const V3 Tail[] = {Waist, {0.0, F - 1.15, 3.75}, {0.0, F - 0.9, 3.55}, {0.0, F - 0.6, 3.6}, {0.0, F - 0.42, 3.82}};
                    for (int K = 0; K < 4; ++K) Out.Tube(Black, Tail[K], Tail[K + 1], Lerp(0.15, 0.04, K / 4.0), Lerp(0.15, 0.04, (K + 1) / 4.0), 10);
                    for (const double Sd : {-1.0, 1.0}) Out.Quad(Black, Tail[4], Tail[4] + V3{Sd * 0.22, 0.05, 0.25}, Tail[4] + V3{Sd * 0.08, 0.12, 0.18}, Tail[4], true);
                    // the painted sign, square to the street, high on the left of the front: cream boards and a black mermaid
                    {
                        const double X = -S.W / 2 + 1.2, Y0 = F - 0.3, Y1 = F - 2.7, Zs = 6.9;
                        Out.Tube(Mat(M::Iron), {X, F, Zs + 0.15}, {X, Y1 - 0.1, Zs + 0.15}, 0.03, 0.03, 4);
                        Out.Tube(Mat(M::Iron), {X, F, Zs - 0.9}, {X, (Y0 + Y1) / 2.0, Zs + 0.12}, 0.025, 0.025, 4);
                        for (const double Y : {Y0 + 0.1, Y1 - 0.0}) Out.Tube(Mat(M::Iron), {X, Y, Zs + 0.15}, {X, Y, Zs}, 0.012, 0.012, 4);
                        Material Board;
                        Board.Name = "M_Art_Tavern_SignBoard";
                        Board.BaseColor = {0.78, 0.7, 0.54, 1.0};
                        Board.Roughness = 0.8;
                        Out.Box(Board, {X - 0.03, Y1, Zs - 1.6}, {X + 0.03, Y0, Zs});
                        Out.Box(Mat(M::TimberDark), {X - 0.04, Y1 - 0.04, Zs - 1.64}, {X + 0.04, Y0 + 0.04, Zs - 1.56});
                        Out.Box(Mat(M::TimberDark), {X - 0.04, Y1 - 0.04, Zs - 0.04}, {X + 0.04, Y0 + 0.04, Zs + 0.04});
                        for (const double Side : {-1.0, 1.0})
                        {
                            const double Xs = X + Side * 0.035, Yc = (Y0 + Y1) / 2.0, Zc = Zs - 0.8;
                            const V3 Sil[] = {{Xs, Yc + 0.55, Zc + 0.5}, {Xs, Yc + 0.3, Zc + 0.15}, {Xs, Yc + 0.05, Zc - 0.15}, {Xs, Yc - 0.35, Zc - 0.35}, {Xs, Yc - 0.7, Zc - 0.2}};
                            for (int K = 0; K < 4; ++K) Out.Tube(Black, Sil[K], Sil[K + 1], Lerp(0.12, 0.04, K / 4.0), Lerp(0.12, 0.04, (K + 1) / 4.0), 6);
                            Out.Tube(Black, {Xs, Yc + 0.62, Zc + 0.66}, {Xs, Yc + 0.62, Zc + 0.67}, 0.09, 0.09, 8);
                            Out.Quad(Black, Sil[4], Sil[4] + V3{0, -0.2, 0.22}, Sil[4] + V3{0, -0.28, 0.0}, Sil[4] + V3{0, -0.2, -0.2}, true);
                        }
                    }
                    break;
                }
                case 7:
                {
                    // LM5, the Bounty Hunters Guild (layout.md 4, H2): a fortified casa 24 x 18 x 10 with a 4 x 4 tower to
                    // 16 m at (+10, +7), an iron-studded door, the wanted-poster wall, a shield-and-manacle sign.
                    const double F = -S.D / 2;
                    Out.Box(Mat(M::StoneDark), {-S.W / 2, F, -0.5}, {S.W / 2, S.D / 2, 4.5});
                    Out.Box(Mat(M::Plaster), {-S.W / 2 + 0.1, F + 0.1, 4.5}, {S.W / 2 - 0.1, S.D / 2 - 0.1, S.H});
                    Course(Out, Mat(M::Stone), S.W, S.D, 4.4, 0.3, 0.12);
                    Out.Box(Mat(M::Stone), {-S.W / 2 - 0.1, F - 0.1, S.H}, {S.W / 2 + 0.1, S.D / 2 + 0.1, S.H + 0.3});
                    const auto Merlons = [&](const double X0, const double X1, const double Y0, const double Y1, const double Zt)
                    {
                        for (double X = X0 + 0.4; X < X1 - 0.3; X += 1.5)
                            for (const double Y : {Y0, Y1}) Out.Box(Mat(M::Stone), {X, Y - 0.25, Zt}, {X + 0.8, Y + 0.25, Zt + 0.7});
                        for (double Y = Y0 + 1.5; Y < Y1 - 0.3; Y += 1.5)
                            for (const double X : {X0, X1}) Out.Box(Mat(M::Stone), {X - 0.25, Y, Zt}, {X + 0.25, Y + 0.8, Zt + 0.7});
                    };
                    Merlons(-S.W / 2, S.W / 2, F, S.D / 2, S.H + 0.3);
                    // the tower
                    Out.Box(Mat(M::Stone), {8.0, 5.0, -0.5}, {12.0, 9.0, 16.0});
                    Merlons(8.0, 12.0, 5.0, 9.0, 16.0);
                    Out.Tube(Mat(M::TimberDark), {10.0, 7.0, 16.0}, {10.0, 7.0, 20.0}, 0.06, 0.05, 6);
                    Out.Quad(Mat(M::HullBandRed), {10.0, 7.0, 20.0}, {11.6, 7.0, 20.0}, {11.6, 7.0, 19.0}, {10.0, 7.0, 19.0}, true);
                    for (const double Zw : {6.5, 9.5, 12.5}) Out.Box(Mat(M::TimberDark), {9.6, 4.94, Zw}, {10.4, 5.02, Zw + 1.0});
                    // windows: slits below, barred above
                    for (const double X : {-9.0, -6.0, 6.0, 9.0})
                    {
                        Out.Box(Mat(M::TimberDark), {X - 0.18, F - 0.04, 1.6}, {X + 0.18, F + 0.02, 2.8});
                        Out.Box(Mat(M::TimberDark), {X - 0.55, F - 0.04, 6.2}, {X + 0.55, F + 0.02, 7.8});
                        for (int Bar = 0; Bar < 4; ++Bar) Out.Tube(Mat(M::Iron), {X - 0.36 + 0.24 * Bar, F - 0.1, 6.15}, {X - 0.36 + 0.24 * Bar, F - 0.1, 7.85}, 0.02, 0.02, 4);
                    }
                    // the iron-studded door
                    Out.Box(Mat(M::StoneDark), {-1.5, F - 0.25, 0.0}, {1.5, F + 0.02, 3.6});
                    Out.Box(Mat(M::TimberDark), {-1.0, F - 0.3, 0.0}, {1.0, F - 0.2, 3.0});
                    for (int Row = 0; Row < 7; ++Row)
                        for (int Col = 0; Col < 5; ++Col)
                        {
                            const double X = -0.8 + 0.4 * Col, Zd = 0.3 + 0.42 * Row;
                            Out.Box(Mat(M::Iron), {X - 0.035, F - 0.34, Zd - 0.035}, {X + 0.035, F - 0.29, Zd + 0.035});
                        }
                    for (const double Zb : {0.5, 1.5, 2.5}) Out.Box(Mat(M::Iron), {-1.0, F - 0.32, Zb - 0.04}, {1.0, F - 0.29, Zb + 0.04});
                    // the wanted-poster wall: a timber board left of the door, papers pinned all over it
                    Out.Box(Mat(M::TimberDark), {-5.6, F - 0.12, 0.9}, {-2.4, F + 0.02, 2.9});
                    Rng Pin(0x9E11u);
                    Material Paper;
                    Paper.Name = "M_Art_Guild_Poster";
                    Paper.BaseColor = {0.86, 0.81, 0.68, 1.0};
                    Paper.Roughness = 0.9;
                    Material Ink;
                    Ink.Name = "M_Art_Guild_PosterInk";
                    Ink.BaseColor = {0.12, 0.1, 0.08, 1.0};
                    Ink.Roughness = 0.9;
                    for (int K = 0; K < 22; ++K)
                    {
                        const double Cx = Pin.Range(-5.3, -2.7), Cz = Pin.Range(1.15, 2.65), Hw = Pin.Range(0.16, 0.24), Hh = Pin.Range(0.2, 0.3), Tilt = Pin.Range(-0.12, 0.12);
                        const double Yp = F - 0.13 - 0.002 * K;
                        const auto At = [&](const double U, const double W) { return V3{Cx + U * Cos(Tilt) - W * Sin(Tilt), Yp, Cz + U * Sin(Tilt) + W * Cos(Tilt)}; };
                        Out.Quad(Paper, At(-Hw, -Hh), At(Hw, -Hh), At(Hw, Hh), At(-Hw, Hh));  // facing the street (-Y)
                        // a face-shaped smudge and lines of text, black on the paper
                        const auto AtInk = [&](const double U, const double W) { return At(U, W) - V3{0, 0.001, 0}; };
                        Out.Quad(Ink, AtInk(-Hw * 0.4, Hh * 0.05), AtInk(Hw * 0.4, Hh * 0.05), AtInk(Hw * 0.4, Hh * 0.7), AtInk(-Hw * 0.4, Hh * 0.7));
                        for (int L = 0; L < 3; ++L)
                            Out.Quad(Ink, AtInk(-Hw * 0.75, -Hh * (0.2 + 0.22 * L)), AtInk(Hw * 0.75, -Hh * (0.2 + 0.22 * L)), AtInk(Hw * 0.75, -Hh * (0.14 + 0.22 * L)),
                                     AtInk(-Hw * 0.75, -Hh * (0.14 + 0.22 * L)));
                    }
                    // the sign: a shield on a bracket with a manacle hanging under it
                    {
                        const double X = 3.2, Ys = F - 1.0, Zs = 4.0;
                        Out.Tube(Mat(M::Iron), {X, F, Zs + 0.6}, {X, F - 1.6, Zs + 0.6}, 0.03, 0.03, 4);
                        Out.Fan(Mat(M::HullBandRed), {{X, Ys - 0.5, Zs + 0.5}, {X, Ys + 0.5, Zs + 0.5}, {X, Ys + 0.5, Zs - 0.1}, {X, Ys, Zs - 0.6}, {X, Ys - 0.5, Zs - 0.1}}, {1, 0, 0});
                        Out.Fan(Mat(M::HullBandRed), {{X, Ys - 0.5, Zs + 0.5}, {X, Ys - 0.5, Zs - 0.1}, {X, Ys, Zs - 0.6}, {X, Ys + 0.5, Zs - 0.1}, {X, Ys + 0.5, Zs + 0.5}}, {-1, 0, 0});
                        for (const double Dy : {-0.5, 0.5}) Out.Tube(Mat(M::Iron), {X, Ys + Dy, Zs + 0.6}, {X, Ys + Dy, Zs + 0.5}, 0.01, 0.01, 4);
                        for (const double Dy : {-0.14, 0.14})
                            for (int K = 0; K < 8; ++K)
                            {
                                const double A = 2.0 * Pi * K / 8, B = 2.0 * Pi * (K + 1) / 8;
                                Out.Tube(Mat(M::Iron), {X, Ys + Dy + 0.1 * Cos(A), Zs - 0.85 + 0.1 * Sin(A)}, {X, Ys + Dy + 0.1 * Cos(B), Zs - 0.85 + 0.1 * Sin(B)}, 0.018, 0.018, 4);
                            }
                        Out.Tube(Mat(M::Iron), {X, Ys - 0.04, Zs - 0.85}, {X, Ys + 0.04, Zs - 0.85}, 0.012, 0.012, 4);
                        Out.Tube(Mat(M::Iron), {X, Ys, Zs - 0.6}, {X, Ys, Zs - 0.75}, 0.01, 0.01, 4);
                    }
                    break;
                }
                case 8:
                case 9:
                case 11:
                {
                    // W1, the Muro noble (layout.md 4): 12 x 1.0 x 4.5 of pale plaster on a stone footing, tile coping and
                    // iron spikes, a 0.6 m pilaster at its +X end; every fourth carries a finial lamp. A gate pier (11)
                    // stands at each side of a gap.
                    Material Pale = Mat(M::Plaster);
                    Pale.Name = "M_Art_NobleWall_Plaster";
                    Pale.BaseColor = {0.94, 0.91, 0.84, 1.0};
                    Material Lamp = Mat(M::Glass);
                    Lamp.Name = "M_Art_NobleWall_Lamp";
                    Lamp.BaseColor = {1.0, 0.82, 0.5, 1.0};
                    Lamp.Emissive = {1.0, 0.74, 0.4};
                    Lamp.EmissiveStrength = 3.0;
                    const auto Finial = [&](const double X, const double Z)
                    {
                        Out.Tube(Mat(M::Iron), {X, 0.0, Z}, {X, 0.0, Z + 0.7}, 0.04, 0.03, 6);
                        Out.Box(Lamp, {X - 0.16, -0.16, Z + 0.7}, {X + 0.16, 0.16, Z + 1.15});
                        Out.Box(Mat(M::Iron), {X - 0.2, -0.2, Z + 1.15}, {X + 0.2, 0.2, Z + 1.22});
                        Out.Tube(Mat(M::Iron), {X, 0.0, Z + 1.22}, {X, 0.0, Z + 1.5}, 0.05, 0.005, 6);
                    };
                    if (V == 11)
                    {
                        Out.Box(Mat(M::Stone), {-0.65, -0.65, -0.8}, {0.65, 0.65, 0.6});
                        Out.Box(Pale, {-0.55, -0.55, 0.6}, {0.55, 0.55, 4.9});
                        Out.Box(Mat(M::Stone), {-0.68, -0.68, 4.9}, {0.68, 0.68, 5.15});
                        Dome(Out, Mat(M::Stone), 0.0, 0.0, 5.15, 0.4, 0.35, 10);
                        Finial(0.0, 5.45);
                        break;
                    }
                    Out.Box(Mat(M::Stone), {-6.0, -0.55, -0.8}, {6.0, 0.55, 0.5});
                    Out.Box(Pale, {-6.0, -0.5, 0.5}, {6.0, 0.5, 4.3});
                    Out.Box(Mat(M::RoofTile), {-6.0, -0.62, 4.3}, {6.0, 0.62, 4.42});
                    Out.Quad(Mat(M::RoofTile), {-6.0, -0.62, 4.42}, {6.0, -0.62, 4.42}, {6.0, 0.0, 4.55}, {-6.0, 0.0, 4.55}, true);
                    Out.Quad(Mat(M::RoofTile), {6.0, 0.62, 4.42}, {-6.0, 0.62, 4.42}, {-6.0, 0.0, 4.55}, {6.0, 0.0, 4.55}, true);
                    for (double X = -5.85; X < 5.6; X += 0.3) Out.Tube(Mat(M::Iron), {X, 0.0, 4.55}, {X, 0.0, 4.85}, 0.012, 0.002, 4);
                    Out.Box(Pale, {5.7, -0.65, -0.2}, {6.3, 0.65, 4.8});
                    Out.Box(Mat(M::Stone), {5.65, -0.7, 4.8}, {6.35, 0.7, 4.98});
                    if (V == 9) Finial(6.0, 4.98);
                    // the mould line of the salt air at the foot
                    Material Mould = Mat(M::StoneDark);
                    Mould.Name = "M_Art_NobleWall_Mould";
                    Mould.BaseColor = {0.42, 0.42, 0.36, 1.0};
                    for (const double Y : {-0.505, 0.505}) Out.Box(Mould, {-6.0, Y - 0.004, 0.5}, {6.0, Y + 0.004, 0.75});
                    break;
                }
                case 10:
                {
                    // W3, a gatehouse (layout.md 4, 5.4 N7): 6 x 6 x 8 of plaster on a stone storey, a hipped tile roof, a
                    // door and barred windows, a lamp; the guards' post at the Noble gates
                    Out.Box(Mat(M::Stone), {-3.0, -3.0, -0.6}, {3.0, 3.0, 3.0});
                    Out.Box(Mat(M::Plaster), {-2.95, -2.95, 3.0}, {2.95, 2.95, 6.8});
                    Course(Out, Mat(M::Stone), 6.0, 6.0, 2.9, 0.25, 0.1);
                    Hip(Out, Mat(M::RoofTile), 5.9, 5.9, 6.8, 1.9, 0.45);
                    Out.Box(Mat(M::TimberDark), {-0.6, -3.06, 0.0}, {0.6, -2.98, 2.3});
                    for (const double X : {-1.6, 1.6})
                    {
                        Out.Box(Mat(M::TimberDark), {X - 0.4, -3.04, 4.2}, {X + 0.4, -2.98, 5.6});
                        for (int Bar = 0; Bar < 4; ++Bar) Out.Tube(Mat(M::Iron), {X - 0.3 + 0.2 * Bar, -3.1, 4.15}, {X - 0.3 + 0.2 * Bar, -3.1, 5.65}, 0.015, 0.015, 4);
                    }
                    for (const double Y : {-1.5, 1.5})
                        for (const double X : {-3.04, 3.04}) Out.Box(Mat(M::TimberDark), {X - 0.04, Y - 0.15, 1.4}, {X + 0.04, Y + 0.15, 2.2});
                    Material Lamp = Mat(M::Glass);
                    Lamp.Name = "M_Art_NobleWall_Lamp";
                    Lamp.BaseColor = {1.0, 0.82, 0.5, 1.0};
                    Lamp.Emissive = {1.0, 0.74, 0.4};
                    Lamp.EmissiveStrength = 3.0;
                    Out.Tube(Mat(M::Iron), {1.0, -3.0, 2.7}, {1.0, -3.4, 2.7}, 0.02, 0.02, 4);
                    Out.Box(Lamp, {0.88, -3.52, 2.3}, {1.12, -3.28, 2.62});
                    break;
                }
                case 12:
                {
                    // TC2, the casa de portal (layout.md 4; N5): 11 x 10 x 8 behind a ground-floor arcade 2.6 deep and 3.0
                    // clear, 0.5 m piers every 3.5 m; three balcony doors above on wrought-iron balconies
                    const double Y0 = -S.D / 2 + 2.6;
                    Out.Append(DressBoxes(Name + "_House", {{{-S.W / 2, Y0, 0.0}, {S.W / 2, S.D / 2, S.H}}}));
                    Out.Box(Mat(M::Plaster), {-S.W / 2, -S.D / 2, 3.0}, {S.W / 2, Y0, S.H});
                    Out.Box(Mat(M::Paving), {-S.W / 2, -S.D / 2, 0.0}, {S.W / 2, Y0, 0.12});
                    Out.Box(Mat(M::TimberDark), {-S.W / 2, -S.D / 2 + 0.05, 2.85}, {S.W / 2, Y0, 3.0});
                    for (double X = -S.W / 2 + 0.25; X <= S.W / 2 - 0.2; X += 3.5)
                    {
                        Out.Box(Mat(M::Stone), {X - 0.25, -S.D / 2, 0.0}, {X + 0.25, -S.D / 2 + 0.5, 3.0});
                        Out.Box(Mat(M::StoneDark), {X - 0.32, -S.D / 2 - 0.05, 2.8}, {X + 0.32, -S.D / 2 + 0.55, 3.05});
                    }
                    for (const double X : {-3.5, 0.0, 3.5})
                    {
                        Out.Box(Mat(M::TimberDark), {X - 0.5, -S.D / 2 - 0.04, 3.6}, {X + 0.5, -S.D / 2 + 0.02, 5.8});
                        Out.Box(Mat(M::StoneDark), {X - 0.75, -S.D / 2 - 0.7, 3.45}, {X + 0.75, -S.D / 2, 3.6});
                        for (int Bar = 0; Bar <= 7; ++Bar)
                            Out.Tube(Mat(M::Iron), {X - 0.7 + 0.2 * Bar, -S.D / 2 - 0.66, 3.6}, {X - 0.7 + 0.2 * Bar, -S.D / 2 - 0.66, 4.55}, 0.012, 0.012, 4);
                        Out.Tube(Mat(M::Iron), {X - 0.72, -S.D / 2 - 0.66, 4.55}, {X + 0.72, -S.D / 2 - 0.66, 4.55}, 0.02, 0.02, 4);
                    }
                    break;
                }
                case 13:
                {
                    // TC4, the casa-patio villa on its walled lot (layout.md 4; N6): the house 16 x 18 x 8 at the back of a 34 x 40
                    // lot behind a 2.8 m wall with a 3.5 m gate; a 3.2 m zaguan with its iron gate; a garden with a fountain
                    Out.Append(DressBoxes(Name + "_House", {{{-8.0, 0.0, 0.0}, {8.0, 18.0, 8.0}}}));
                    Out.Box(Mat(M::TimberDark), {-1.6, -0.06, 0.0}, {1.6, 0.02, 3.4});
                    for (int Bar = 0; Bar <= 10; ++Bar) Out.Tube(Mat(M::Iron), {-1.5 + 0.3 * Bar, -0.12, 0.0}, {-1.5 + 0.3 * Bar, -0.12, 3.3}, 0.02, 0.02, 4);
                    const double X0 = -17.0, X1 = 17.0, Y0 = -20.0, Y1 = 20.0;
                    for (const auto& [A, B] : {std::pair<V3, V3>{{X0, Y1 - 0.3, 0.0}, {X1, Y1, 2.8}}, {{X0, Y0, 0.0}, {X0 + 0.3, Y1, 2.8}}, {{X1 - 0.3, Y0, 0.0}, {X1, Y1, 2.8}},
                                               {{X0, Y0, 0.0}, {-1.75, Y0 + 0.3, 2.8}}, {{1.75, Y0, 0.0}, {X1, Y0 + 0.3, 2.8}}})
                    {
                        Out.Box(Mat(M::Plaster), {A.X, A.Y, -0.4}, B);
                        Out.Box(Mat(M::RoofTile), {A.X - 0.06, A.Y - 0.06, 2.8}, {B.X + 0.06, B.Y + 0.06, 2.95});
                    }
                    for (const double X : {-1.75, 1.75}) Out.Box(Mat(M::Stone), {X - 0.4, Y0 - 0.1, -0.4}, {X + 0.4, Y0 + 0.4, 3.4});
                    for (int Bar = 0; Bar <= 11; ++Bar) Out.Tube(Mat(M::Iron), {-1.35 + 0.245 * Bar, Y0 + 0.15, 0.0}, {-1.35 + 0.245 * Bar, Y0 + 0.15, 2.6}, 0.02, 0.02, 4);
                    Out.Box(Mat(M::Paving), {-1.6, Y0, 0.0}, {1.6, 0.0, 0.08});
                    Out.Tube(Mat(M::Stone), {0.0, -10.0, 0.0}, {0.0, -10.0, 0.6}, 2.2, 2.2, 20);
                    Out.Tube(Mat(M::HarborWater), {0.0, -10.0, 0.5}, {0.0, -10.0, 0.55}, 2.0, 2.0, 20);
                    Out.Tube(Mat(M::Stone), {0.0, -10.0, 0.5}, {0.0, -10.0, 1.6}, 0.25, 0.18, 10);
                    break;
                }
                case 14:
                {
                    // LM10, Salazar's public estate (layout.md 4; N3): 40 x 26 x 11 of white plaster with pale gilt ironwork, its
                    // azotea behind a 1.1 m parapet with four watch posts, on a 44 x 36 lot with a forecourt facing the square
                    Material White = Mat(M::Plaster);
                    White.Name = "M_Art_Salazar_WhitePlaster";
                    White.BaseColor = {0.97, 0.96, 0.92, 1.0};
                    Material Gilt = Mat(M::Brass);
                    Gilt.Name = "M_Art_Salazar_Gilt";
                    Gilt.BaseColor = {0.86, 0.78, 0.55, 1.0};
                    const double Y0 = -8.0;  // the house behind a 10 m forecourt (26 + 15 would not fit the 36 m lot)
                    Out.Box(Mat(M::Stone), {-20.0, Y0, -0.6}, {20.0, Y0 + 26.0, 0.8});
                    Out.Box(White, {-20.0, Y0, 0.8}, {20.0, Y0 + 26.0, 11.0});
                    Course(Out, Mat(M::Stone), 40.0, 26.0, 4.5, 0.3, 0.15);
                    Out.Box(White, {-20.15, Y0 - 0.15, 11.0}, {20.15, Y0 + 26.15, 12.1});
                    Out.Box(Mat(M::Stone), {-20.25, Y0 - 0.25, 12.1}, {20.25, Y0 + 26.25, 12.25});
                    for (const double X : {-19.0, 19.0})
                        for (const double Y : {Y0 + 1.0, Y0 + 25.0})
                        {
                            Out.Box(White, {X - 1.2, Y - 1.2, 11.0}, {X + 1.2, Y + 1.2, 14.0});
                            Out.Box(Mat(M::RoofTile), {X - 1.4, Y - 1.4, 14.0}, {X + 1.4, Y + 1.4, 14.25});
                        }
                    for (double X = -17.0; X <= 17.0; X += 3.4)
                        for (const double Z : {2.0, 6.5})
                        {
                            Out.Box(Mat(M::TimberDark), {X - 0.55, Y0 - 0.05, Z}, {X + 0.55, Y0 + 0.02, Z + 2.2});
                            for (int Bar = 0; Bar < 5; ++Bar) Out.Tube(Gilt, {X - 0.4 + 0.2 * Bar, Y0 - 0.12, Z - 0.05}, {X - 0.4 + 0.2 * Bar, Y0 - 0.12, Z + 2.25}, 0.018, 0.018, 4);
                        }
                    Out.Box(Mat(M::TimberDark), {-1.6, Y0 - 0.08, 0.8}, {1.6, Y0 + 0.02, 4.4});
                    Out.Box(Mat(M::Paving), {-22.0, -18.0, 0.0}, {22.0, Y0, 0.1});
                    for (const double X : {-22.0, 21.6}) Out.Box(White, {X, -18.0, -0.4}, {X + 0.4, Y0, 2.4});
                    for (int Bar = 0; Bar <= 40; ++Bar)
                        if (std::fabs(-20.0 + Bar) > 2.2) Out.Tube(Gilt, {-20.0 + Bar, -18.0, 0.0}, {-20.0 + Bar, -18.0, 2.2}, 0.025, 0.012, 4);
                    Out.Tube(Gilt, {-22.0, -18.0, 2.2}, {22.0, -18.0, 2.2}, 0.03, 0.03, 4);
                    break;
                }
                case 15:
                {
                    // LM11, the guard barracks at the Noble gate (layout.md 4; N4): 20 x 14 x 7, plain and strong
                    Out.Box(Mat(M::StoneDark), {-10.0, -7.0, -0.5}, {10.0, 7.0, 3.2});
                    Out.Box(Mat(M::Plaster), {-9.95, -6.95, 3.2}, {9.95, 6.95, 7.0});
                    Hip(Out, Mat(M::RoofTile), 20.0, 14.0, 7.0, 2.2, 0.4);
                    for (double X = -8.0; X <= 8.0; X += 4.0)
                    {
                        Out.Box(Mat(M::TimberDark), {X - 0.4, -7.04, 4.2}, {X + 0.4, -6.98, 5.8});
                        for (int Bar = 0; Bar < 4; ++Bar) Out.Tube(Mat(M::Iron), {X - 0.3 + 0.2 * Bar, -7.1, 4.15}, {X - 0.3 + 0.2 * Bar, -7.1, 5.85}, 0.015, 0.015, 4);
                    }
                    Out.Box(Mat(M::TimberDark), {-1.2, -7.06, 0.0}, {1.2, -6.98, 2.8});
                    Out.Tube(Mat(M::TimberDark), {-6.0, -7.0, 7.0}, {-6.0, -7.0, 11.0}, 0.06, 0.05, 6);
                    Out.Quad(Mat(M::HullBandRed), {-6.0, -7.0, 11.0}, {-6.0, -8.4, 11.0}, {-6.0, -8.4, 10.1}, {-6.0, -7.0, 10.1}, true);
                    for (const double X : {4.0, 6.5}) Out.Box(Mat(M::Timber), {X - 0.9, -9.0, 0.0}, {X + 0.9, -8.6, 1.1});  // the hitching rail
                    break;
                }
                case 16:
                {
                    // N2, the duel platform of the Duelist's Square (layout.md 5.4): 14 x 14 x 0.5 with a brass inlay, three
                    // 0.3 m steps on its east and west sides
                    Material Pale = Mat(M::Paving);
                    Pale.Name = "M_Art_DuelPlatform";
                    Pale.BaseColor = {0.92, 0.9, 0.84, 1.0};
                    Out.Box(Pale, {-7.0, -7.0, -0.3}, {7.0, 7.0, 0.5});
                    for (int K = 0; K < 3; ++K)
                        for (const double Side : {-1.0, 1.0})
                        {
                            const double X0 = Side * (7.0 + 0.35 * K), X1 = Side * (7.0 + 0.35 * (K + 1));
                            Out.Box(Pale, {std::min(X0, X1), -5.0, -0.3}, {std::max(X0, X1), 5.0, 0.5 - 0.17 * (K + 1)});
                        }
                    Material Inlay = Mat(M::Brass);
                    Inlay.Name = "M_Art_DuelInlay";
                    for (int K = 0; K < 24; ++K)
                    {
                        const double A = 2.0 * Pi * K / 24, B = 2.0 * Pi * (K + 1) / 24;
                        Out.Box(Inlay, {std::min(5.0 * Cos(A), 5.0 * Cos(B)) - 0.06, std::min(5.0 * Sin(A), 5.0 * Sin(B)) - 0.06, 0.5},
                                {std::max(5.0 * Cos(A), 5.0 * Cos(B)) + 0.06, std::max(5.0 * Sin(A), 5.0 * Sin(B)) + 0.06, 0.505});
                    }
                    for (const double A : {0.0, Pi / 2})
                        Out.Box(Inlay, {-5.0 * std::fabs(Cos(A)) - 0.05, -5.0 * std::fabs(Sin(A)) - 0.05, 0.5}, {5.0 * std::fabs(Cos(A)) + 0.05, 5.0 * std::fabs(Sin(A)) + 0.05, 0.505});
                    break;
                }
                case 17:
                {
                    // W2, a seam arch (layout.md 3.3): 8 x 2.5 x 6.5, an opening 5.5 x 4.5 under a segmental arch, two grille
                    // leaves folded back against the piers (they close for a 5-star lockdown), a painted name board on the lintel
                    // (the lettering is the district's, left to the sign pass), a 3 x 3 x 3.5 guard post beside it and a
                    // fingerpost 5 m outside (+Y, along the road away from the district)
                    Out.Box(Mat(M::Stone), {-4.0, -1.25, -0.6}, {-2.75, 1.25, 6.5});
                    Out.Box(Mat(M::Stone), {2.75, -1.25, -0.6}, {4.0, 1.25, 6.5});
                    Out.Box(Mat(M::Plaster), {-2.75, -1.25, 5.2}, {2.75, 1.25, 6.5});
                    for (int K = 0; K < 8; ++K)
                    {
                        const double A0 = Pi * K / 8, A1 = Pi * (K + 1) / 8;
                        for (const double Y : {-1.15, 1.15})
                            Out.Tube(Mat(M::Stone), {2.75 * Cos(A0), Y, 4.5 + 0.7 * Sin(A0)}, {2.75 * Cos(A1), Y, 4.5 + 0.7 * Sin(A1)}, 0.14, 0.14, 6);
                        OrientedBox(Out, Mat(M::Plaster), {2.75 * (Cos(A0) + Cos(A1)) / 2, 0.0, 4.5 + 0.7 * (Sin(A0) + Sin(A1)) / 2 + 0.4}, 1.0, 0.0, {0.18, 1.2, 0.42});
                    }
                    Out.Box(Mat(M::StoneDark), {-4.15, -1.4, 6.5}, {4.15, 1.4, 6.8});
                    for (const double Y : {-1.27, 1.27}) Out.Box(Mat(M::TimberDark), {-2.4, Y - 0.03, 5.45}, {2.4, Y + 0.03, 6.35});
                    for (const double Side : {-1.0, 1.0})
                        for (int Bar = 0; Bar <= 7; ++Bar)
                        {
                            const double Y = -1.1 + 0.3 * Bar;
                            Out.Tube(Mat(M::Iron), {Side * 2.6, Y, 0.0}, {Side * 2.6, Y, 4.3}, 0.02, 0.02, 4);
                            if (Bar == 0) for (const double Z : {0.3, 2.2, 4.2}) Out.Tube(Mat(M::Iron), {Side * 2.6, -1.1, Z}, {Side * 2.6, 1.0, Z}, 0.025, 0.025, 4);
                        }
                    // the guard post beside the east pier
                    Out.Box(Mat(M::Stone), {4.3, -1.5, -0.5}, {7.3, 1.5, 3.5});
                    Out.Push(Xf::Translate({5.8, 0.0, 0.0}));
                    Hip(Out, Mat(M::RoofTile), 3.0, 3.0, 3.5, 1.0, 0.25);
                    Out.Pop();
                    Out.Box(Mat(M::TimberDark), {5.3, 1.46, 0.0}, {6.3, 1.54, 2.1});
                    // the fingerpost outside
                    Out.Tube(Mat(M::TimberDark), {-5.2, 5.0, 0.0}, {-5.2, 5.0, 3.0}, 0.06, 0.05, 6);
                    for (const double Dir : {-1.0, 1.0}) Out.Box(Mat(M::Timber), {-5.2 + (Dir < 0 ? -0.9 : 0.0), 4.97, 2.45 + (Dir < 0 ? 0.0 : 0.3)}, {-5.2 + (Dir < 0 ? 0.0 : 0.9), 5.03, 2.65 + (Dir < 0 ? 0.0 : 0.3)});
                    break;
                }
                case 18:
                {
                    // a stair's low gate (layout.md 3.3): a 3 m grille between two stone posts at the foot of the stair, open
                    for (const double X : {-1.9, 1.9})
                    {
                        Out.Box(Mat(M::Stone), {X - 0.35, -0.35, -0.4}, {X + 0.35, 0.35, 1.6});
                        Out.Box(Mat(M::StoneDark), {X - 0.42, -0.42, 1.6}, {X + 0.42, 0.42, 1.72});
                    }
                    for (const double Side : {-1.0, 1.0})
                        for (int Bar = 0; Bar <= 6; ++Bar)
                        {
                            const double Y = 0.1 + 0.22 * Bar;
                            Out.Tube(Mat(M::Iron), {Side * 1.5, Y, 0.05}, {Side * 1.5, Y, 1.4}, 0.016, 0.016, 4);
                            if (Bar == 0) for (const double Z : {0.15, 1.35}) Out.Tube(Mat(M::Iron), {Side * 1.5, 0.1, Z}, {Side * 1.5, 1.45, Z}, 0.02, 0.02, 4);
                        }
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
                 Tradition::Civic, Tradition::Harbour, Tradition::Street, Tradition::Shanty, Tradition::Ruin})
            for (int V = 0; V < (Kind == Tradition::Lighthouse ? 1 : Kind == Tradition::Civic ? 19 : Kind == Tradition::Harbour ? 5 : Kind == Tradition::Street ? 3 : Kind == Tradition::Ruin ? 3 : Variants); ++V)
                Emit(ModelName(Kind, V), WriteGltf(BuildVariant(Kind, V), "Dark Arisen art kit (settlements)"));
    }

    Structure Kit::SettlementPiece(const Tradition Kind, const int Variant, const double X, const double Y, const double Z, const double Yaw)
    {
        const Size S = VariantSize(Kind, Variant);
        return {"Assets/Art/Models/" + ModelName(Kind, Variant) + ".gltf", X, Y, Z, Yaw, S.W / 2 + 0.3, S.D / 2 + 0.3, S.H + 2.0};
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
        // life at the doors: a scanned barrel, crate or lantern beside a house's door (-Y in its frame). Props are kept
        // apart and added at the end, so they never count against a settlement's buildings.
        std::vector<Structure> Props;
        const auto Dress = [&](const Tradition Kind, const int V, const double X, const double Y, const double Yaw)
        {
            static const char* const Kinds[] = {"Assets/Art/Scanned/models/Barrel_01/Barrel_01_2k.gltf", "Assets/Art/Scanned/models/Barrel_02/Barrel_02_2k.gltf",
                "Assets/Art/Scanned/models/barrel_03/barrel_03_2k.gltf", "Assets/Art/Scanned/models/wooden_crate_01/wooden_crate_01_2k.gltf",
                "Assets/Art/Scanned/models/wooden_lantern_01/wooden_lantern_01_2k.gltf"};
            const Size S = VariantSize(Kind, V);
            const int Count = static_cast<int>(R.Unit() * 3.0);
            const double Rad = Yaw * Pi / 180.0, Ca = Cos(Rad), Sa = Sin(Rad);
            for (int K = 0; K < Count; ++K)
            {
                const double Lx = (K % 2 ? 1.0 : -1.0) * R.Range(1.0, std::max(1.2, S.W / 2 - 0.4)), Ly = -S.D / 2 - R.Range(0.6, 1.4);
                const double Px = X + Lx * Ca - Ly * Sa, Py = Y + Lx * Sa + Ly * Ca;
                const int Pick = static_cast<int>(R.Unit() * 5.0);
                const double Turn = R.Range(0.0, 360.0);
                Props.push_back({Kinds[Pick], Px, Py, Spec.Ground(Px, Py), Turn, 0.4, 0.4, 1.0});
            }
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
        // (a level design can switch it off, e.g. Rexa's districts above the harbour: layout.md R-05)
        if (Spec.Kind != Tradition::Camp && Spec.Kind != Tradition::Fort && Spec.Waterfront != 0)
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

        // the landmark on the square, or where the level design puts it, facing the way it says (layout.md R-03)
        const double Ca = Cos(Spec.StreetAngle * Pi / 180.0), Sa = Sin(Spec.StreetAngle * Pi / 180.0);
        const auto GridToWorld = [&](const double U, const double W) { return std::pair<double, double>{Spec.X + U * Ca - W * Sa, Spec.Y + U * Sa + W * Ca}; };
        if (Spec.Centre != Landmark::None)
        {
            const auto [Lx, Ly] = GridToWorld(Spec.LandmarkU, Spec.LandmarkW);
            Place(Tradition::Civic, static_cast<int>(Spec.Centre), Lx, Ly, Spec.LandmarkYaw > -1e8 ? Spec.LandmarkYaw : Spec.StreetAngle);
        }
        // the level design's own buildings at their world positions (layout.md: the tavern B1, the Guild H2)
        for (const auto& [V, X, Y, Yaw] : Spec.Civic) Place(Tradition::Civic, V, X, Y, Yaw);
        // a walled district's ring (layout.md 5.4: W1 every 12.6 m round the centre, every fourth with its finial lamp, gaps
        // for the gates with a pier on each side)
        const std::size_t BeforeWall = Out.size();
        if (Spec.WallRadius > 0.0)
        {
            const double Step = 12.6;
            const int Segments = std::max(8, static_cast<int>(2.0 * Pi * Spec.WallRadius / Step + 0.5));
            const auto InGate = [&](const double A, const double Reach)
            {
                for (const auto& [Bearing, Gap] : Spec.WallGates)
                {
                    double D = A - Bearing * Pi / 180.0;
                    while (D > Pi) D -= 2.0 * Pi;
                    while (D < -Pi) D += 2.0 * Pi;
                    if (std::fabs(D) * Spec.WallRadius < Gap / 2.0 + Reach) return true;
                }
                return false;
            };
            for (int K = 0; K < Segments; ++K)
            {
                const double A = 2.0 * Pi * (K + 0.5) / Segments;
                if (InGate(A, 6.3)) continue;
                Place(Tradition::Civic, K % 4 == 0 ? 9 : 8, Spec.X + Spec.WallRadius * Cos(A), Spec.Y + Spec.WallRadius * Sin(A), A * 180.0 / Pi + 90.0);
            }
            for (const auto& [Bearing, Gap] : Spec.WallGates)
                for (const double Side : {-1.0, 1.0})
                {
                    const double A = Bearing * Pi / 180.0 + Side * (Gap / 2.0 + 0.65) / Spec.WallRadius;
                    Place(Tradition::Civic, 11, Spec.X + Spec.WallRadius * Cos(A), Spec.Y + Spec.WallRadius * Sin(A), A * 180.0 / Pi + 90.0);
                }
        }
        // the wall's pieces do not count against the district's buildings
        const int MaxBuildings = Spec.MaxBuildings + static_cast<int>(Out.size() - BeforeWall);
        // what a house may not stand on: the landmarks and quays placed so far, each as the circle round its box
        const std::vector<Structure> Reserved = Out;
        const auto Clear = [&](const double X, const double Y, const double Reach)
        {
            for (const Structure& S : Reserved)
            {
                const double Keep = std::sqrt(S.HalfX * S.HalfX + S.HalfY * S.HalfY) + Reach;
                if ((S.X - X) * (S.X - X) + (S.Y - Y) * (S.Y - Y) < Keep * Keep) return false;
            }
            return true;
        };
        const Tradition Houses = Spec.Kind == Tradition::Colonial ? Spec.Houses : Spec.Kind;
        // the jungle reclaims a town's edge (layout.md 8): a quarter of the houses in band B1 (R_f - 20 .. R_f) are ruins, and
        // ruins at half fill continue out through B2 (R_f .. R_f + 25). Decided from the position, not the layout's stream,
        // so nothing else in the town moves.
        const double Rim = Spec.Radius * 0.9;
        const auto Chance = [](const double X, const double Y, const std::uint32_t Salt)
        {
            Rng H(static_cast<std::uint32_t>(std::llround(X * 10.0)) * 73856093u ^ static_cast<std::uint32_t>(std::llround(Y * 10.0)) * 19349663u ^ Salt);
            return H.Unit();
        };
        const auto PlaceHouse = [&](const int V, const double X, const double Y, const double Yaw)
        {
            const double D = std::sqrt((X - Spec.X) * (X - Spec.X) + (Y - Spec.Y) * (Y - Spec.Y));
            if (Spec.Kind == Tradition::Colonial && D > Rim - 20.0 && D <= Rim && Chance(X, Y, 1u) < 0.25)
                Place(Tradition::Ruin, static_cast<int>(Chance(X, Y, 2u) * 3.0), X, Y, Yaw);
            else
                Place(Houses, V, X, Y, Yaw);
        };
        const auto EdgeRuins = [&]()
        {
            if (Spec.Kind != Tradition::Colonial || Spec.WallRadius > 0.0) return;  // a walled quarter keeps a clean edge
            for (const double Ring : {Rim + 8.0, Rim + 19.0})
            {
                const int Steps = static_cast<int>(2.0 * Pi * Ring / 18.0);
                for (int K = 0; K < Steps; ++K)
                {
                    const double A = 2.0 * Pi * (K + (Ring > Rim + 10.0 ? 0.5 : 0.0)) / Steps;
                    const double X = Spec.X + Ring * Cos(A), Y = Spec.Y + Ring * Sin(A);
                    if (Chance(X, Y, 3u) > 0.5) continue;
                    const int V = static_cast<int>(Chance(X, Y, 4u) * 3.0);
                    const Size S = VariantSize(Tradition::Ruin, V);
                    if (!Clear(X, Y, std::max(S.W, S.D) / 2 + 2.0)) continue;
                    if (Spec.KeepOff && Spec.KeepOff(X, Y, std::max(S.W, S.D) / 2 + 2.0)) continue;
                    if (std::fabs(Spec.Ground(X + 4.0, Y) - Spec.Ground(X - 4.0, Y)) > 3.0 || std::fabs(Spec.Ground(X, Y + 4.0) - Spec.Ground(X, Y - 4.0)) > 3.0) continue;
                    if (Spec.Ground(X, Y) < 1.0) continue;
                    bool Near = false;
                    for (const Structure& O : Out)
                        Near = Near || (O.X - X) * (O.X - X) + (O.Y - Y) * (O.Y - Y) < (O.HalfX + O.HalfY + 5.0) * (O.HalfX + O.HalfY + 5.0) * 0.5;
                    if (Near) continue;
                    Place(Tradition::Ruin, V, X, Y, A * 180.0 / Pi + 90.0 + (Chance(X, Y, 5u) - 0.5) * 30.0);
                }
            }
        };
        // street furniture (layout.md R-09): kept with the props, so it never counts against the buildings
        const auto Furnish = [&](const int V, const double X, const double Y, const double Yaw)
        {
            const Size S = VariantSize(Tradition::Street, V);
            if (V != 0 && Spec.KeepOff && Spec.KeepOff(X, Y, std::max(S.W, S.D) / 2)) return;
            Props.push_back({"Assets/Art/Models/" + ModelName(Tradition::Street, V) + ".gltf", X, Y, Spec.Ground(X, Y), Yaw, S.W / 2, S.D / 2, S.H});
        };
        {
            // market stalls in a ring round the square, counters to the middle; benches round its edge, facing in
            const double Pu = Spec.PlazaRadius >= 0.0 ? Spec.PlazaU : 0.0, Pw = Spec.PlazaRadius >= 0.0 ? Spec.PlazaW : 0.0;
            const auto Ring = [&](const int V, const int Count, const double Radius, const double Phase)
            {
                for (int K = 0; K < Count; ++K)
                {
                    const double A = 2.0 * Pi * (K + Phase) / Count;
                    const auto [X, Y] = GridToWorld(Pu + Radius * Cos(A), Pw + Radius * Sin(A));
                    // the model's front (-Y) towards the square's middle
                    Furnish(V, X, Y, Spec.StreetAngle + A * 180.0 / Pi - 90.0);
                }
            };
            if (Spec.StallRadius > 0.0 && Spec.StallMax > 0)
                Ring(1, std::min(Spec.StallMax, static_cast<int>(2.0 * Pi * Spec.StallRadius / std::max(Spec.StallPitch, 3.2))), Spec.StallRadius, 0.0);
            if (Spec.Benches > 0 && Spec.PlazaRadius > 4.0) Ring(2, Spec.Benches, Spec.PlazaRadius - 2.5, 0.5);
        }

        if (Spec.Kind == Tradition::Colonial && !Spec.Cluster)
        {
            // a colonial grid: blocks of 40 x 26 m between 7 m streets on the city's axis, each block lined on its long
            // sides with terraced houses wall to wall, doors on the street; the square stays open. Blocks fill from the
            // centre outwards (layout.md R-02), so a district's cap leaves its edge thin, never one side empty.
            const double Block = 40.0, Deep = 26.0, Street = 7.0;
            const double Square = Spec.Radius * 0.22;
            const int Reach = static_cast<int>(Spec.Radius / (Deep + Street)) + 1;
            struct Plot { double Key; int I, J; };
            std::vector<Plot> Plots;
            for (int J = -Reach; J <= Reach; ++J)
                for (int I = -Reach; I <= Reach; ++I)
                {
                    // block centre in the grid's frame (u along the street axis), offset half a block so the square is a crossing
                    const double U = (I + 0.5) * (Block + Street), W = (J + 0.5) * (Deep + Street);
                    if (U * U + W * W > Spec.Radius * 0.9 * Spec.Radius * 0.9) continue;
                    if (Spec.PlazaRadius < 0.0 && U * U + W * W < Square * Square) continue;
                    Plots.push_back({U * U + W * W, I, J});
                }
            std::sort(Plots.begin(), Plots.end(), [](const Plot& A, const Plot& B) { return A.Key != B.Key ? A.Key < B.Key : A.J != B.J ? A.J < B.J : A.I < B.I; });
            for (const Plot& P : Plots)
            {
                if (static_cast<int>(Out.size()) >= MaxBuildings) break;
                const double U = (P.I + 0.5) * (Block + Street), W = (P.J + 0.5) * (Deep + Street);
                if (R.Unit() > Spec.Fill) continue;  // a garden, a yard, a ruin
                for (const double Side : {-1.0, 1.0})
                {
                    double Along = -Block / 2;
                    while (Along < Block / 2 - 5.0 && static_cast<int>(Out.size()) < MaxBuildings)
                    {
                        const int V = static_cast<int>(R.Unit() * Variants);
                        const Size S = VariantSize(Houses, V);
                        if (Along + S.W > Block / 2) break;
                        const double Lu = U + Along + S.W / 2, Lw = W + Side * (Deep / 2 - S.D / 2);
                        const auto [X, Y] = GridToWorld(Lu, Lw);
                        const double Half = std::sqrt(S.W * S.W + S.D * S.D) / 2;
                        const bool OffPlaza = Spec.PlazaRadius < 0.0 ||
                            (Lu - Spec.PlazaU) * (Lu - Spec.PlazaU) + (Lw - Spec.PlazaW) * (Lw - Spec.PlazaW) > (Spec.PlazaRadius + Half) * (Spec.PlazaRadius + Half);
                        const bool OffRoad = !Spec.KeepOff || !Spec.KeepOff(X, Y, Half);
                        if (Spec.Ground(X, Y) >= Spec.MinGround && OffPlaza && OffRoad && Clear(X, Y, Half + 3.0))
                        {
                            PlaceHouse(V, X, Y, Spec.StreetAngle + (Side < 0 ? 0.0 : 180.0));  // doors (-Y) on the street
                            if (R.Unit() < 0.3) Dress(Houses, V, X, Y, Spec.StreetAngle + (Side < 0 ? 0.0 : 180.0));
                        }
                        Along += S.W + 0.05;
                    }
                    // gas lamps at the kerb along this side of the block ("gas-lit streets"), so every street has a
                    // row on both sides from the two blocks that face it
                    if (Spec.LampSpacing > 0.0)
                        for (double Lu = U - Block / 2 + Spec.LampSpacing / 2; Lu < U + Block / 2; Lu += Spec.LampSpacing)
                        {
                            const double Lw = W + Side * (Deep / 2 + 1.0);
                            const auto [X, Y] = GridToWorld(Lu, Lw);
                            const bool OffPlaza = Spec.PlazaRadius < 0.0 ||
                                (Lu - Spec.PlazaU) * (Lu - Spec.PlazaU) + (Lw - Spec.PlazaW) * (Lw - Spec.PlazaW) > Spec.PlazaRadius * Spec.PlazaRadius;
                            if (Spec.Ground(X, Y) >= Spec.MinGround && OffPlaza && Clear(X, Y, 1.0)) Furnish(0, X, Y, Spec.StreetAngle);
                        }
                }
            }
            EdgeRuins();
            Out.insert(Out.end(), Props.begin(), Props.end());
            return Out;
        }

        // villages and camps grow, they are not drawn: houses in loose clusters, denser towards the middle, each set
        // along the hillside (long side on the contour) with a little give, a path's width from its neighbours
        // a camp gathers round its fire (placed first, so the tents keep their distance)
        if (Spec.Kind == Tradition::Camp)
            Out.push_back({"Assets/Art/Scanned/models/stone_fire_pit/stone_fire_pit_2k.gltf", Spec.X, Spec.Y, Spec.Ground(Spec.X, Spec.Y), 0.0, 3.0, 3.0, 0.6});
        const double Ridge = Spec.StreetAngle;
        for (int Tries = 0; Tries < Spec.MaxBuildings * 30 && static_cast<int>(Out.size()) < MaxBuildings; ++Tries)
        {
            const double A = R.Range(0.0, 2.0 * Pi);
            const double Rr = Spec.Radius * 0.9 * std::sqrt(R.Unit()) * (0.35 + 0.65 * R.Unit());
            const double X = Spec.X + Rr * Cos(A), Y = Spec.Y + Rr * Sin(A);
            const int V = static_cast<int>(R.Unit() * Variants);
            const Size S = VariantSize(Houses, V);
            const double Jitter = R.Range(-14.0, 14.0);
            if (R.Unit() > Spec.Fill || !Free(X, Y, std::max(S.W, S.D) / 2 + (Spec.Kind == Tradition::Camp ? 2.0 : 4.0))) continue;
            if (Spec.KeepOff && Spec.KeepOff(X, Y, std::max(S.W, S.D) / 2)) continue;
            if (Spec.PlazaRadius >= 0.0)
            {
                const auto [Px, Py] = GridToWorld(Spec.PlazaU, Spec.PlazaW);
                if ((X - Px) * (X - Px) + (Y - Py) * (Y - Py) < (Spec.PlazaRadius + std::max(S.W, S.D)) * (Spec.PlazaRadius + std::max(S.W, S.D))) continue;
            }
            // the contour: across the downhill direction (from the ground either side), else the village's own axis
            const double Gx = Spec.Ground(X + 3.0, Y) - Spec.Ground(X - 3.0, Y), Gy = Spec.Ground(X, Y + 3.0) - Spec.Ground(X, Y - 3.0);
            double Along = Ridge;
            if (Gx * Gx + Gy * Gy > 0.04) Along = std::fabs(Gx) > std::fabs(Gy) ? 0.0 : 90.0;  // long side (Y in the model) along the slope's contour
            const double Yaw = Along + Jitter + (R.Unit() < 0.5 ? 180.0 : 0.0);
            PlaceHouse(V, X, Y, Yaw);
            if (Spec.Kind != Tradition::Camp && R.Unit() < 0.6) Dress(Houses, V, X, Y, Yaw);
        }

        EdgeRuins();
        Out.insert(Out.end(), Props.begin(), Props.end());
        return Out;
    }
}
