// Dressing of the authored story geometry on terrain levels (ArtKit.h, Kit::Authored). The story
// contracts give named boxes whose top is the floor the anchors mark: a road, a gate, a defence, an
// interior, a courtyard, a roof, a chart table, a cover, a ruin, a ship's deck. The name decides the
// architecture; walls and parapets only rise on edges the route does not cross, so the walkable top
// stays exactly the authored box (its collider) and nothing blocks the way.

#include "ArtKit.h"
#include "ArtKitLibrary.h"

#include <algorithm>
#include <cmath>

namespace DarkArisen::Tools::Art
{
    namespace
    {
        enum class Style { Road, Gate, Defence, Interior, Yard, Field, Roof, Table, Cover, Ruin, Deck, Masonry };

        /** True if a whole CamelCase word of Name is Part ("GroveTrail" has "trail", not "rail"). */
        bool Has(const std::string& Name, const char* Part)
        {
            std::string Word;
            const auto Done = [&] { const bool Hit = Word == Part; Word.clear(); return Hit; };
            for (const char C : Name)
            {
                const bool Upper = C >= 'A' && C <= 'Z', Lower = C >= 'a' && C <= 'z';
                if ((Upper || !Lower) && Done()) return true;
                if (Upper) Word += static_cast<char>(C + ('a' - 'A'));
                else if (Lower) Word += C;
            }
            return Done();
        }

        Style StyleOf(const std::string& Name)
        {
            if (Has(Name, "table")) return Style::Table;
            if (Has(Name, "roof")) return Style::Roof;
            if (Has(Name, "gate")) return Style::Gate;
            if (Has(Name, "deck") || Has(Name, "companionway") || Has(Name, "stairs") || Has(Name, "bridge") || Has(Name, "rail") || Has(Name, "boundary") ||
                (Has(Name, "hold") && Has(Name, "captive")))
            {
                return Style::Deck;
            }
            if (Has(Name, "road") || Has(Name, "route") || Has(Name, "trail") || Has(Name, "path") || Has(Name, "street")) return Style::Road;
            if (Has(Name, "wall") || Has(Name, "defence") || Has(Name, "barricade") || Has(Name, "breach") || Has(Name, "ridge") || Has(Name, "boom")) return Style::Defence;
            if (Has(Name, "interior") || Has(Name, "room") || Has(Name, "cabin") || Has(Name, "cell") || Has(Name, "cellar")) return Style::Interior;
            if (Has(Name, "courtyard") || Has(Name, "quay") || Has(Name, "port") || Has(Name, "place") || Has(Name, "arena")) return Style::Yard;
            if (Has(Name, "battlefield") || Has(Name, "ground") || Has(Name, "grove") || Has(Name, "screen") || Has(Name, "zone")) return Style::Field;
            if (Has(Name, "cover") || Has(Name, "stack") || Has(Name, "cut")) return Style::Cover;
            if (Has(Name, "ruin") || Has(Name, "echo") || Has(Name, "memory")) return Style::Ruin;
            return Style::Masonry;
        }

        /** Crenellated parapet of thickness T and height Hp on top of a wall line from A to B (at z = Z). */
        void Parapet(Mesh& Out, const V3& A, const V3& B, const double T, const double Hp, const M Stone)
        {
            const V3 D = B - A;
            const double L = std::sqrt(D.X * D.X + D.Y * D.Y);
            if (L < 0.5) return;
            const V3 U = D * (1.0 / L);
            const V3 Side{-U.Y * T * 0.5, U.X * T * 0.5, 0.0};
            const int Merlons = std::max(1, static_cast<int>(L / 1.6));
            for (int K = 0; K < Merlons; ++K)
            {
                const double S0 = L * K / Merlons, S1 = S0 + L / Merlons * 0.6;
                const V3 P0 = A + U * S0, P1 = A + U * S1;
                const V3 Lo{std::min(P0.X, P1.X) - std::fabs(Side.X), std::min(P0.Y, P1.Y) - std::fabs(Side.Y), A.Z};
                const V3 Hi{std::max(P0.X, P1.X) + std::fabs(Side.X), std::max(P0.Y, P1.Y) + std::fabs(Side.Y), A.Z + Hp};
                Out.Box(Mat(Stone), Lo, Hi);
            }
            // continuous base of the parapet
            const V3 Lo{std::min(A.X, B.X) - std::fabs(Side.X), std::min(A.Y, B.Y) - std::fabs(Side.Y), A.Z};
            const V3 Hi{std::max(A.X, B.X) + std::fabs(Side.X), std::max(A.Y, B.Y) + std::fabs(Side.Y), A.Z + Hp * 0.45};
            Out.Box(Mat(Stone), Lo, Hi);
        }

        /** The four top edges (counter-clockwise from -Y) with their open-mask bit. */
        struct Edge
        {
            V3 A, B;
            unsigned Bit;
        };
        std::array<Edge, 4> Edges(const double Hx, const double Hy, const double Z)
        {
            return {Edge{{-Hx, -Hy, Z}, {Hx, -Hy, Z}, 4u}, Edge{{Hx, -Hy, Z}, {Hx, Hy, Z}, 2u}, Edge{{Hx, Hy, Z}, {-Hx, Hy, Z}, 8u}, Edge{{-Hx, Hy, Z}, {-Hx, -Hy, Z}, 1u}};
        }
    }

    std::string Kit::Authored(const AuthoredSpec& Spec)
    {
        const auto Cm = [](const double Metres) { return std::to_string(std::lround(Metres * 100.0)); };
        const std::string ModelName = "SM_Art_Authored_" + Spec.Name + "_" + Cm(Spec.SizeX) + "x" + Cm(Spec.SizeY) + "x" + Cm(Spec.SizeZ) + "_" +
            std::to_string(static_cast<int>(Spec.Kind)) + "_" + std::to_string(Spec.Open);
        if (const auto Found = Generated.find(ModelName); Found != Generated.end()) return Found->second;
        Mesh Out(ModelName);
        const double Hx = Spec.SizeX / 2.0, Hy = Spec.SizeY / 2.0, Z = Spec.SizeZ;
        const bool Tropic = Spec.Kind == Biome::Tropical || Spec.Kind == Biome::Grove || Spec.Kind == Biome::Mire;
        const M Wall = Spec.Kind == Biome::Dream ? M::StoneDark : (Tropic ? M::Stone : M::StoneDark);
        const M Earth = Spec.Kind == Biome::Tropical ? M::RedEarth : (Spec.Kind == Biome::Mire ? M::Mud : M::Dirt);
        const auto Body = [&](const M Side, const M Top, const double TopThickness)
        {
            // battered sides from the foot to just under the top, then the top course
            const double B = std::min(0.4, Z * 0.06);
            const V3 Lo[4] = {{-Hx - B, -Hy - B, 0.0}, {Hx + B, -Hy - B, 0.0}, {Hx + B, Hy + B, 0.0}, {-Hx - B, Hy + B, 0.0}};
            const V3 Hi[4] = {{-Hx, -Hy, Z - TopThickness}, {Hx, -Hy, Z - TopThickness}, {Hx, Hy, Z - TopThickness}, {-Hx, Hy, Z - TopThickness}};
            for (int K = 0; K < 4; ++K) Out.Quad(Mat(Side), Lo[K], Lo[(K + 1) % 4], Hi[(K + 1) % 4], Hi[K]);
            Out.Box(Mat(Top), {-Hx, -Hy, Z - TopThickness}, {Hx, Hy, Z});
        };
        Rng R(HashString(Spec.Name));
        switch (StyleOf(Spec.Name))
        {
        case Style::Road:
        {
            Body(Earth, Spec.Kind == Biome::Tropical ? M::Paving : Earth, 0.12);
            for (const Edge& E : Edges(Hx, Hy, Z))
            {
                if (Spec.Open & E.Bit) continue;
                // kerb of set stones
                const V3 D = E.B - E.A;
                const double L = std::sqrt(D.X * D.X + D.Y * D.Y);
                for (double S = 0.3; S < L - 0.3; S += 0.9)
                {
                    const V3 P = E.A + D * (S / L);
                    Out.Box(Mat(M::StoneDark), {P.X - 0.3, P.Y - 0.3, Z - 0.1}, {P.X + 0.3, P.Y + 0.3, Z + 0.12});
                }
            }
            break;
        }
        case Style::Gate:
        {
            Body(Wall, M::Paving, 0.15);
            // flanking walls on the closed edges, towers at the corners, an arch over the passage
            const double Wh = 5.5;
            for (const Edge& E : Edges(Hx, Hy, Z))
            {
                if (Spec.Open & E.Bit) continue;
                const V3 D = E.B - E.A;
                const V3 In = Normalize(V3{-D.Y, D.X, 0.0}) * 0.5;
                const V3 Lo{std::min(E.A.X, E.B.X) + std::min(In.X, 0.0) * 2.0, std::min(E.A.Y, E.B.Y) + std::min(In.Y, 0.0) * 2.0, Z};
                const V3 Hi{std::max(E.A.X, E.B.X) + std::max(In.X, 0.0) * 2.0, std::max(E.A.Y, E.B.Y) + std::max(In.Y, 0.0) * 2.0, Z + Wh};
                Out.Box(Mat(Wall), Lo, Hi);
                Parapet(Out, (E.A + In) + V3{0, 0, Wh}, (E.B + In) + V3{0, 0, Wh}, 1.0, 1.1, Wall);
            }
            for (const Edge& E : Edges(Hx, Hy, Z))
            {
                const V3 C = E.A;
                Out.Box(Mat(Wall), {C.X - 1.6, C.Y - 1.6, 0.0}, {C.X + 1.6, C.Y + 1.6, Z + Wh + 2.0});
                Parapet(Out, C + V3{-1.6, -1.6, Wh + 2.0}, C + V3{1.6, -1.6, Wh + 2.0}, 0.6, 1.0, Wall);
                Parapet(Out, C + V3{-1.6, 1.6, Wh + 2.0}, C + V3{1.6, 1.6, Wh + 2.0}, 0.6, 1.0, Wall);
            }
            // arch over the passage across the short axis
            const bool AlongX = Spec.SizeX >= Spec.SizeY;
            const double Span = AlongX ? Hy : Hx;
            for (int K = 0; K <= 12; ++K)
            {
                const double A0 = Pi * K / 13.0, A1 = Pi * (K + 1) / 13.0;
                const double U0 = -Cos(A0) * Span, U1 = -Cos(A1) * Span, V0 = Sin(A0) * 1.6, V1 = Sin(A1) * 1.6;
                const double Base = Z + Wh - 1.8;
                if (AlongX)
                    Out.Box(Mat(Wall), {-0.8, std::min(U0, U1), Base + std::min(V0, V1)}, {0.8, std::max(U0, U1), Z + Wh + 0.6});
                else
                    Out.Box(Mat(Wall), {std::min(U0, U1), -0.8, Base + std::min(V0, V1)}, {std::max(U0, U1), 0.8, Z + Wh + 0.6});
            }
            break;
        }
        case Style::Defence:
        {
            Body(Wall, Has(Spec.Name, "barricade") ? Earth : M::Paving, 0.15);
            if (Has(Spec.Name, "barricade") || Has(Spec.Name, "boom"))
            {
                // timber stakes and crates along the closed edges
                for (const Edge& E : Edges(Hx, Hy, Z))
                {
                    if (Spec.Open & E.Bit) continue;
                    const V3 D = E.B - E.A;
                    const double L = std::sqrt(D.X * D.X + D.Y * D.Y);
                    for (double S = 0.4; S < L - 0.4; S += 0.5)
                    {
                        const V3 P = E.A + D * (S / L);
                        Out.Tube(Mat(M::TimberDark), {P.X, P.Y, Z - 0.2}, {P.X + R.Range(-0.15, 0.15), P.Y + R.Range(-0.15, 0.15), Z + R.Range(1.2, 1.8)}, 0.09, 0.03, 5, true);
                    }
                    for (int K = 0; K < 3; ++K)
                    {
                        const V3 P = E.A + D * R.Range(0.15, 0.85) + Normalize(V3{-D.Y, D.X, 0.0}) * 0.9;
                        Crate(Out, {P.X - 0.45, P.Y - 0.45, Z}, {P.X + 0.45, P.Y + 0.45, Z + 0.9});
                    }
                }
            }
            else
            {
                for (const Edge& E : Edges(Hx, Hy, Z))
                {
                    if (Spec.Open & E.Bit) continue;
                    const V3 In = Normalize(V3{-(E.B - E.A).Y, (E.B - E.A).X, 0.0}) * 0.45;
                    Parapet(Out, E.A + In, E.B + In, 0.9, 1.3, Wall);
                }
                if (Has(Spec.Name, "breach"))
                {
                    for (int K = 0; K < 9; ++K)
                    {
                        // draws in statement order: argument evaluation order differs between compilers
                        const V3 At{R.Range(-Hx, Hx) * 0.8, R.Range(-Hy, Hy) * 0.8, Z - 0.05};
                        const double Size = R.Range(0.3, 0.8), Flat = R.Range(0.2, 0.5);
                        Rock(Out, Mat(Wall), At, Size, Flat, 400u + static_cast<std::uint32_t>(K));
                    }
                }
            }
            break;
        }
        case Style::Interior:
        {
            Body(Wall, Tropic ? M::Deck : M::Paving, 0.15);
            // roofless room: walls with window gaps on the closed edges, timber posts at the corners
            const double Wh = 2.6;
            for (const Edge& E : Edges(Hx, Hy, Z))
            {
                const V3 C = E.A;
                Out.Box(Mat(M::TimberDark), {C.X - 0.18, C.Y - 0.18, Z}, {C.X + 0.18, C.Y + 0.18, Z + Wh + 0.3});
                if (Spec.Open & E.Bit) continue;
                const V3 D = E.B - E.A;
                const double L = std::sqrt(D.X * D.X + D.Y * D.Y);
                const V3 U = D * (1.0 / L), In = Normalize(V3{-D.Y, D.X, 0.0}) * 0.2;
                for (double S = 0.2; S < L - 0.2; S += 2.4)
                {
                    const double S1 = std::min(L - 0.2, S + 1.6);
                    const V3 P0 = E.A + U * S + In, P1 = E.A + U * S1 + In;
                    Out.Box(Mat(Tropic ? M::Plaster : Wall), {std::min(P0.X, P1.X) - 0.2, std::min(P0.Y, P1.Y) - 0.2, Z}, {std::max(P0.X, P1.X) + 0.2, std::max(P0.Y, P1.Y) + 0.2, Z + Wh});
                    const V3 Q0 = E.A + U * S1 + In, Q1 = E.A + U * std::min(L - 0.2, S + 2.4) + In;  // window bay: sill and lintel
                    Out.Box(Mat(Tropic ? M::Plaster : Wall), {std::min(Q0.X, Q1.X) - 0.2, std::min(Q0.Y, Q1.Y) - 0.2, Z}, {std::max(Q0.X, Q1.X) + 0.2, std::max(Q0.Y, Q1.Y) + 0.2, Z + 0.9});
                    Out.Box(Mat(M::TimberDark), {std::min(Q0.X, Q1.X) - 0.2, std::min(Q0.Y, Q1.Y) - 0.2, Z + Wh - 0.4}, {std::max(Q0.X, Q1.X) + 0.2, std::max(Q0.Y, Q1.Y) + 0.2, Z + Wh});
                }
            }
            break;
        }
        case Style::Yard:
            Body(Wall, M::Paving, 0.15);
            for (const Edge& E : Edges(Hx, Hy, Z))
            {
                if (Spec.Open & E.Bit) continue;
                const V3 In = Normalize(V3{-(E.B - E.A).Y, (E.B - E.A).X, 0.0}) * 0.35;
                const V3 A = E.A + In, B = E.B + In;
                Out.Box(Mat(Wall), {std::min(A.X, B.X) - 0.35, std::min(A.Y, B.Y) - 0.35, Z}, {std::max(A.X, B.X) + 0.35, std::max(A.Y, B.Y) + 0.35, Z + 0.95});
            }
            break;
        case Style::Field:
            Body(Earth, Spec.Kind == Biome::Moor ? M::DryGrass : M::Grass, 0.2);
            if (Has(Spec.Name, "battlefield"))
            {
                for (int K = 0; K < 14; ++K)
                {
                    const V3 P{R.Range(-Hx, Hx) * 0.9, R.Range(-Hy, Hy) * 0.9, Z};
                    Out.Tube(Mat(M::TimberDark), P, P + V3{R.Range(-0.4, 0.4), R.Range(-0.4, 0.4), R.Range(0.8, 1.6)}, 0.06, 0.02, 5, true);  // broken pikes
                }
            }
            break;
        case Style::Roof:
        {
            Body(Tropic ? M::Plaster : Wall, M::RoofTile, 0.25);
            for (const Edge& E : Edges(Hx, Hy, Z))
            {
                const V3 Out2 = Normalize(V3{(E.B - E.A).Y, -(E.B - E.A).X, 0.0}) * 0.5;
                const V3 A = E.A + Out2, B = E.B + Out2;
                Out.Box(Mat(M::TimberDark), {std::min(A.X, B.X) - 0.1, std::min(A.Y, B.Y) - 0.1, Z - 0.35}, {std::max(A.X, B.X) + 0.1, std::max(A.Y, B.Y) + 0.1, Z - 0.15});
            }
            break;
        }
        case Style::Table:
            Body(M::Timber, M::Deck, 0.08);
            Out.Box(Mat(M::Canvas), {-Hx * 0.7, -Hy * 0.6, Z}, {Hx * 0.7, Hy * 0.6, Z + 0.01});  // spread chart
            break;
        case Style::Cover:
            Body(Earth, Earth, 0.1);
            for (int K = 0; K < 8; ++K)
            {
                const V3 P{R.Range(-Hx, Hx) * 0.8, R.Range(-Hy, Hy) * 0.8, Z};
                const double S = R.Range(0.4, 0.6);
                Crate(Out, {P.X - S, P.Y - S, Z}, {P.X + S, P.Y + S, Z + S * 2.0});
            }
            break;
        case Style::Ruin:
            Body(Wall, M::Paving, 0.15);
            for (const Edge& E : Edges(Hx, Hy, Z))
            {
                if (Spec.Open & E.Bit) continue;
                const V3 D = E.B - E.A;
                const double L = std::sqrt(D.X * D.X + D.Y * D.Y);
                for (double S = 0.0; S < L; S += 1.2)
                {
                    const double Tall = R.Range(0.3, 2.8);
                    const double H = Tall * (R.Unit() < 0.3 ? 0.3 : 1.0);
                    const V3 P = E.A + D * (S / L);
                    Out.Box(Mat(Wall), {P.X - 0.6, P.Y - 0.6, Z - 0.1}, {P.X + 0.6, P.Y + 0.6, Z + H});
                }
            }
            for (int K = 0; K < 10; ++K)
            {
                const V3 At{R.Range(-Hx, Hx) * 0.85, R.Range(-Hy, Hy) * 0.85, Z - 0.05};
                const double Size = R.Range(0.25, 0.6), Flat = R.Range(0.15, 0.4);
                Rock(Out, Mat(Wall), At, Size, Flat, 500u + static_cast<std::uint32_t>(K));
            }
            break;
        case Style::Deck:
            Body(M::HullBlack, M::Deck, 0.15);
            for (const Edge& E : Edges(Hx, Hy, Z))
            {
                if (Spec.Open & E.Bit) continue;
                const V3 In = Normalize(V3{-(E.B - E.A).Y, (E.B - E.A).X, 0.0}) * 0.15;
                const V3 A = E.A + In, B = E.B + In;
                Out.Box(Mat(M::TimberDark), {std::min(A.X, B.X) - 0.15, std::min(A.Y, B.Y) - 0.15, Z}, {std::max(A.X, B.X) + 0.15, std::max(A.Y, B.Y) + 0.15, Z + 1.05});
            }
            break;
        case Style::Masonry:
        default:
            Body(M::Stone, M::Paving, 0.14);
            break;
        }
        return Generated[ModelName] = Emit(ModelName, WriteGltf(Out, "Dark Arisen art kit (authored geometry)"));
    }

    std::vector<PropPlacement> Kit::AuthoredProps(const AuthoredSpec& Spec)
    {
        const auto Scan = [](const char* Id) { return std::string("Assets/Art/Scanned/models/") + Id + "/" + Id + "_2k.gltf"; };
        const double Hx = Spec.SizeX / 2.0, Hy = Spec.SizeY / 2.0, Z = Spec.SizeZ;
        std::vector<PropPlacement> Out;
        Rng R(HashString(Spec.Name));
        // corners whose two edges are both closed are off the route; fall back to any corner when none is
        struct Corner
        {
            double X, Y;
            unsigned Edges;
        };
        const std::array<Corner, 4> Corners{Corner{-Hx, -Hy, 1u | 4u}, Corner{Hx, -Hy, 2u | 4u}, Corner{Hx, Hy, 2u | 8u}, Corner{-Hx, Hy, 1u | 8u}};
        std::vector<Corner> Quiet;
        for (const Corner& C : Corners)
            if ((Spec.Open & C.Edges) == 0) Quiet.push_back(C);
        if (Quiet.empty()) Quiet.assign(Corners.begin(), Corners.end());
        const auto Inset = [](const Corner& C, double By) { return std::array<double, 2>{C.X - (C.X > 0 ? By : -By), C.Y - (C.Y > 0 ? By : -By)}; };
        const auto Stores = [&](const Corner& C)
        {
            if (Hx < 2.0 || Hy < 2.0) return;
            const auto P = Inset(C, 1.0);
            const double Yaw = R.Range(0.0, 360.0);
            Out.push_back({Scan("Barrel_01"), P[0], P[1], Z, Yaw});
            const auto Q = Inset(C, 1.7);
            Out.push_back({Scan("Barrel_02"), Q[0], P[1], Z, Yaw + 40.0});
            Out.push_back({Scan("wooden_crate_01"), P[0], Q[1], Z, Yaw + 90.0});
            Out.push_back({Scan("wooden_lantern_01"), P[0], Q[1], Z + 0.35, Yaw});
        };
        switch (StyleOf(Spec.Name))
        {
        case Style::Defence:
            for (const Edge& E : Edges(Hx, Hy, Z))
            {
                if (Spec.Open & E.Bit) continue;
                const V3 D = E.B - E.A;
                const double L = std::sqrt(D.X * D.X + D.Y * D.Y);
                if (L < 6.0 || Hx < 2.5 || Hy < 2.5) continue;
                const V3 U = D * (1.0 / L);
                const V3 Outward{U.Y, -U.X, 0.0};
                const double Yaw = std::atan2(-Outward.X, Outward.Y) * 180.0 / Pi;  // the gun's length lies along +Y
                const int Guns = std::max(1, static_cast<int>((L - 2.0) / 7.0));
                for (int K = 0; K < Guns; ++K)
                {
                    const V3 P = E.A + U * (L * (K + 0.5) / Guns) - Outward * 1.6;
                    Out.push_back({Scan("cannon_01"), P.X, P.Y, Z, Yaw});
                }
            }
            if (!Quiet.empty()) Stores(Quiet.front());
            break;
        case Style::Yard:
        case Style::Deck:
            for (std::size_t K = 0; K < Quiet.size() && K < 2; ++K) Stores(Quiet[K]);
            break;
        case Style::Field:
            if (Hx >= 4.0 && Hy >= 4.0)
            {
                const auto P = Inset(Quiet.front(), 3.0);
                Out.push_back({Scan("stone_fire_pit"), P[0], P[1], Z, R.Range(0.0, 360.0)});
                Stores(Quiet.back());
            }
            break;
        case Style::Interior:
            if (Hx >= 2.0 && Hy >= 1.5)
            {
                const auto P = Inset(Quiet.front(), 1.2);
                const double Yaw = std::fabs(P[0]) > std::fabs(P[1]) ? 90.0 : 0.0;
                Out.push_back({Scan("WoodenTable_01"), P[0], P[1], Z, Yaw});
                Out.push_back({Scan("jug_01"), P[0] + 0.2, P[1], Z + 0.55, R.Range(0.0, 360.0)});
                Out.push_back({Scan("ceramic_pot"), P[0] - 0.3, P[1] + 0.1, Z + 0.55, R.Range(0.0, 360.0)});
                if (Quiet.size() > 1) Stores(Quiet[1]);
            }
            break;
        default:
            break;
        }
        return Out;
    }
}
