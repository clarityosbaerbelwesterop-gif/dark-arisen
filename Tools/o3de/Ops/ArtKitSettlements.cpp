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
        for (const Tradition Kind : {Tradition::Colonial, Tradition::Stilt, Tradition::Nordic, Tradition::Camp, Tradition::Fort, Tradition::Lighthouse})
            for (int V = 0; V < (Kind == Tradition::Lighthouse ? 1 : Variants); ++V)
                Emit(ModelName(Kind, V), WriteGltf(BuildVariant(Kind, V), "Dark Arisen art kit (settlements)"));
    }

    std::vector<Structure> Kit::SettlementLayout(const SettlementSpec& Spec)
    {
        std::vector<Structure> Out;
        Rng R(Spec.Seed * 2654435761u + 977u);
        const auto Place = [&](const Tradition Kind, const int V, const double X, const double Y, const double Yaw)
        {
            const Size S = VariantSize(Kind, V);
            const double Z = Spec.Ground(X, Y);
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

        // streets in rings round a square: buildings face the centre, a lane between rings; denser near the middle
        const double Plaza = Spec.Radius * (Spec.Kind == Tradition::Camp ? 0.15 : 0.2);
        const int Wanted = Spec.MaxBuildings;
        for (double Ring = Plaza + 8.0; Ring < Spec.Radius * 0.92 && static_cast<int>(Out.size()) < Wanted; Ring += Spec.Kind == Tradition::Camp ? 6.0 : 15.0)
        {
            const double Start = R.Range(0.0, 2.0 * Pi);
            double Along = 0.0;
            const double Circumference = 2.0 * Pi * Ring;
            while (Along < Circumference - 6.0 && static_cast<int>(Out.size()) < Wanted)
            {
                const int V = static_cast<int>(R.Unit() * Variants);
                const Size S = VariantSize(Spec.Kind, V);
                const double Gap = Spec.Kind == Tradition::Colonial ? R.Range(0.3, 1.5) : R.Range(3.0, 9.0);
                const double A = Start + (Along + S.W / 2) / Ring;
                const double X = Spec.X + Ring * Cos(A), Y = Spec.Y + Ring * Sin(A);
                // the door (-Y in the model) faces the square
                const double Yaw = A * 180.0 / Pi - 90.0;
                const double Skip = R.Unit();
                if (Skip > Spec.Fill || !Free(X, Y, S.W / 2 + 1.0))
                {
                    Along += S.W + Gap;
                    continue;
                }
                Place(Spec.Kind, V, X, Y, Yaw);
                Along += S.W + Gap;
            }
        }
        return Out;
    }
}
