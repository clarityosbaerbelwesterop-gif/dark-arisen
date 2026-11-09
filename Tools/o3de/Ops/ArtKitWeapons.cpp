// The weapon library: the weapon classes of docs/design/physics/combat_physics.md (2, 10) and the signature weapons the boss
// documents in docs/design/bosses describe. Every model is held at its origin: the middle of the grip (or of the haft
// where the hands sit) at (0, 0, 0), the blade or head toward +Z, a cutting edge toward +X, the flats facing +-Y.
// Only + - * / and sqrt (and the kit's series sine and cosine) reach the output, so every compiler writes the same bytes.

#include "ArtKit.h"
#include "ArtKitCore.h"
#include "ArtKitLibrary.h"

#include <algorithm>
#include <cmath>
#include <functional>

namespace DarkArisen::Tools::Art
{
    namespace
    {
        Material Plain(const char* Name, const double R, const double G, const double B, const double Roughness, const double Metallic)
        {
            Material M;
            M.Name = Name;
            M.BaseColor = {R, G, B, 1.0};
            M.Roughness = Roughness;
            M.Metallic = Metallic;
            return M;
        }

        Material Scanned(const char* Name, const char* Set, const double R, const double G, const double B, const double Tile, const double Rough, const double Metallic)
        {
            Material M;
            const std::string Base = std::string("Scanned/") + Set + "/" + Set;
            M.Name = Name;
            M.BaseColorTexture = Base + "_diff_2k.jpg";
            M.NormalTexture = Base + "_nor_gl_2k.jpg";
            M.OrmTexture = Base + "_arm_2k.jpg";
            M.BaseColor = {R, G, B, 1.0};
            M.Roughness = Rough;
            M.Metallic = Metallic;
            M.MetresPerTile = Tile;
            return M;
        }

        // the shared stock of materials
        struct Stock
        {
            Material Steel = Plain("M_Art_Weapon_Steel", 0.72, 0.72, 0.74, 0.28, 1.0);
            Material DarkSteel = Plain("M_Art_Weapon_DarkSteel", 0.2, 0.2, 0.22, 0.38, 1.0);
            Material Iron = Scanned("M_Art_Weapon_Iron", "rusty_metal_02", 0.5, 0.48, 0.46, 0.25, 0.85, 0.6);
            Material Brass = Plain("M_Art_Weapon_Brass", 0.78, 0.6, 0.3, 0.3, 1.0);
            Material Bronze = Plain("M_Art_Weapon_BellBronze", 0.6, 0.42, 0.24, 0.42, 1.0);
            Material Wood = Scanned("M_Art_Weapon_Wood", "rough_wood", 0.62, 0.46, 0.3, 0.3, 0.8, 0.0);
            Material DarkWood = Scanned("M_Art_Weapon_DarkWood", "rough_wood", 0.36, 0.25, 0.16, 0.3, 0.7, 0.0);
            Material Leather = Plain("M_Art_Weapon_GripLeather", 0.2, 0.12, 0.07, 0.7, 0.0);
            Material Cord = Plain("M_Art_Weapon_Cord", 0.62, 0.56, 0.44, 0.9, 0.0);
            Material Bone = Plain("M_Art_Weapon_Bone", 0.86, 0.8, 0.68, 0.55, 0.0);
            Material Planks = Scanned("M_Art_Weapon_ShieldPlanks", "weathered_planks", 0.7, 0.6, 0.48, 0.6, 0.9, 0.0);
        };

        /** A blade along +Z from Z0 to Z0 + Length: its width, thickness and the bend of its spine (+X) along the length. */
        struct BladeShape
        {
            double Z0 = 0.0, Length = 0.7;
            std::function<double(double)> Width, Thick, Bend;
            bool DoubleEdged = false;
        };

        /**
         * Lofts a blade: rings of six points round the cross-section (edge, two flats, the back), the tip closing to a point.
         * Single-edged blades keep a thick back at -X; double-edged ones are a lens with both edges keen.
         */
        void Blade(Mesh& Out, const Material& M, const BladeShape& S, const int Steps)
        {
            std::vector<std::vector<V3>> Rings;
            for (int K = 0; K <= Steps; ++K)
            {
                const double T = static_cast<double>(K) / Steps, Z = S.Z0 + S.Length * T;
                const double W = std::max(S.Width(T), 0.0004), H = std::max(S.Thick(T), 0.0003), X = S.Bend(T);
                const double Back = -W * 0.5;
                const double BackH = S.DoubleEdged ? 0.0004 : H * 0.5;
                Rings.push_back({{X + W * 0.5, 0.0, Z}, {X + W * 0.1, H * 0.5, Z}, {X + Back * 0.8, BackH, Z}, {X + Back, 0.0, Z},
                                 {X + Back * 0.8, -BackH, Z}, {X + W * 0.1, -H * 0.5, Z}});
            }
            Out.Loft(M, Rings, true);
        }

        /** A flat plate cut to a star-shaped outline in the XZ plane (seen from its centre C), thinning to its edges. */
        void Plate(Mesh& Out, const Material& M, const std::vector<V3>& Outline, const V3& C, const double HalfThick, const std::vector<double>& EdgeThick)
        {
            const std::size_t N = Outline.size();
            const auto Tex = [&](const V3& P) { return std::pair<double, double>{P.X / M.MetresPerTile, P.Z / M.MetresPerTile}; };
            for (std::size_t I = 0; I < N; ++I)
            {
                const V3& A = Outline[I];
                const V3& B = Outline[(I + 1) % N];
                const double Ha = EdgeThick.empty() ? HalfThick * 0.25 : EdgeThick[I], Hb = EdgeThick.empty() ? HalfThick * 0.25 : EdgeThick[(I + 1) % N];
                for (const double Side : {1.0, -1.0})
                {
                    const V3 Pc{C.X, Side * HalfThick, C.Z}, Pa{A.X, Side * Ha, A.Z}, Pb{B.X, Side * Hb, B.Z};
                    // wind each face so its geometric normal agrees with the side it faces, whichever way the outline runs
                    const V3 Nrm = Normalize(Cross(Pa - Pc, Pb - Pc));
                    const auto [Uc, Vc] = Tex(Pc);
                    const auto [Ua, Va] = Tex(Pa);
                    const auto [Ub, Vb] = Tex(Pb);
                    if (Nrm.Y * Side > 0)
                        Out.Triangle(M, Vertex{Pc, Nrm, Uc, Vc}, Vertex{Pa, Nrm, Ua, Va}, Vertex{Pb, Nrm, Ub, Vb});
                    else
                        Out.Triangle(M, Vertex{Pc, Nrm * -1.0, Uc, Vc}, Vertex{Pb, Nrm * -1.0, Ub, Vb}, Vertex{Pa, Nrm * -1.0, Ua, Va});
                }
                // the rim between the two faces
                Out.Quad(M, {A.X, Ha, A.Z}, {B.X, Hb, B.Z}, {B.X, -Hb, B.Z}, {A.X, -Ha, A.Z}, true);
            }
        }

        /** A wrapped grip from Z0 to Z1: a core with raised turns of the wrap. */
        void Grip(Mesh& Out, const Material& M, const double Z0, const double Z1, const double Radius)
        {
            Out.Tube(M, {0, 0, Z0}, {0, 0, Z1}, Radius, Radius * 0.96, 10);
            const int Turns = std::max(3, static_cast<int>((Z1 - Z0) / 0.018));
            for (int K = 0; K < Turns; ++K)
            {
                const double Z = Z0 + (Z1 - Z0) * (K + 0.5) / Turns;
                Out.Tube(M, {0, 0, Z - 0.004}, {0, 0, Z + 0.004}, Radius * 1.07, Radius * 1.07, 10);
            }
        }

        /** A closed ellipsoid at C (half sizes Rx, Ry, Rz). */
        void Knob(Mesh& Out, const Material& M, const V3& C, const double Rx, const double Ry, const double Rz)
        {
            std::vector<std::vector<V3>> Rings;
            for (int K = 0; K <= 8; ++K)
            {
                const double A = -Pi / 2 + Pi * K / 8.0, R = std::max(Cos(A), 0.004);
                std::vector<V3> Rg;
                for (int J = 0; J < 10; ++J)
                {
                    const double B = 2.0 * Pi * J / 10;
                    Rg.push_back({C.X + Rx * R * Cos(B), C.Y + Ry * R * Sin(B), C.Z + Rz * Sin(A)});
                }
                Rings.push_back(Rg);
            }
            Out.Loft(M, Rings, true);
        }

        /** A ring of tube (a loop of chain, a knuckle guard's ring) round Axis at C. */
        void Loop(Mesh& Out, const Material& M, const V3& C, const V3& U, const V3& V, const double R, const double Thick, const int Segments)
        {
            for (int K = 0; K < Segments; ++K)
            {
                const double A = 2.0 * Pi * K / Segments, B = 2.0 * Pi * (K + 1) / Segments;
                Out.Tube(M, C + U * (R * Cos(A)) + V * (R * Sin(A)), C + U * (R * Cos(B)) + V * (R * Sin(B)), Thick, Thick, 6);
            }
        }

        /** A haft from Z0 to Z1 with iron collars. */
        void Haft(Mesh& Out, const Stock& K, const Material& M, const double Z0, const double Z1, const double Radius, const std::vector<double>& Collars)
        {
            Out.Tube(M, {0, 0, Z0}, {0, 0, Z1}, Radius, Radius * 0.92, 10);
            for (const double Z : Collars) Out.Tube(K.Iron, {0, 0, Z - 0.012}, {0, 0, Z + 0.012}, Radius * 1.12, Radius * 1.12, 10);
        }

        // ------------------------------------------------------------------ swords and knives

        /** A one-handed blade with a grip, a guard and a pommel; the guard style: 0 cross, 1 knuckle bow, 2 swept rapier hilt. */
        Mesh Sword(const char* Name, const Stock& K, const Material& BladeMat, const BladeShape& S, const int Guard, const double GripLength, const Material& Fittings)
        {
            Mesh Out(Name);
            Blade(Out, BladeMat, S, 24);
            Grip(Out, K.Leather, -GripLength * 0.5, GripLength * 0.5, 0.014);
            const double Top = GripLength * 0.5;
            if (Guard == 0)
            {
                Out.Tube(Fittings, {-0.075, 0, Top + 0.01}, {0.075, 0, Top + 0.01}, 0.009, 0.009, 8);
                for (const double X : {-0.08, 0.08}) Knob(Out, Fittings, {X, 0, Top + 0.01}, 0.012, 0.012, 0.012);
            }
            else if (Guard == 1)
            {
                // the knuckle bow: from the guard round the fingers to the pommel, a shell on the guard
                Out.Tube(Fittings, {-0.03, 0, Top + 0.008}, {0.05, 0, Top + 0.008}, 0.008, 0.008, 8);
                const V3 Bow[] = {{0.05, 0, Top + 0.008}, {0.065, 0, Top - 0.04}, {0.06, 0, -Top + 0.02}, {0.02, 0, -Top - 0.01}};
                for (int I = 0; I < 3; ++I) Out.Tube(Fittings, Bow[I], Bow[I + 1], 0.006, 0.006, 6);
                Knob(Out, Fittings, {0.012, 0, Top + 0.014}, 0.04, 0.03, 0.007);
            }
            else
            {
                // swept hilt: quillons, a ring each side of the ricasso, a bow, a small cup
                Out.Tube(Fittings, {-0.1, 0, Top + 0.012}, {0.1, 0, Top + 0.012}, 0.006, 0.006, 6);
                for (const double Y : {-1.0, 1.0}) Loop(Out, Fittings, {0, Y * 0.025, Top + 0.035}, {1, 0, 0}, {0, 0, 1}, 0.03, 0.004, 10);
                const V3 Bow[] = {{0.0, 0, Top + 0.012}, {0.06, 0, Top - 0.02}, {0.055, 0, -Top + 0.02}, {0.015, 0, -Top - 0.01}};
                for (int I = 0; I < 3; ++I) Out.Tube(Fittings, Bow[I], Bow[I + 1], 0.005, 0.005, 6);
                Knob(Out, Fittings, {0, 0, Top + 0.02}, 0.04, 0.04, 0.012);
            }
            Knob(Out, Fittings, {0, 0, -GripLength * 0.5 - 0.018}, 0.02, 0.02, 0.022);
            return Out;
        }

        BladeShape Straight(const double Length, const double Width, const double Thick, const bool Double)
        {
            BladeShape S;
            S.Z0 = 0.06;
            S.Length = Length;
            S.Width = [=](double T) { return T < 0.82 ? Width * (1.0 - 0.25 * T) : Width * 0.8 * (1.0 - (T - 0.82) / 0.18); };
            S.Thick = [=](double T) { return Thick * (1.0 - 0.6 * T); };
            S.Bend = [](double) { return 0.0; };
            S.DoubleEdged = Double;
            return S;
        }

        BladeShape Curved(const double Length, const double Width, const double Thick, const double Sweep, const double ClipFrom)
        {
            BladeShape S;
            S.Z0 = 0.06;
            S.Length = Length;
            S.Width = [=](double T) { return T < ClipFrom ? Width * (1.0 + 0.15 * T) : Width * 1.15 * (1.0 - (T - ClipFrom) / (1.0 - ClipFrom)); };
            S.Thick = [=](double T) { return Thick * (1.0 - 0.5 * T); };
            S.Bend = [=](double T) { return -Sweep * T * T; };
            return S;
        }

        // ------------------------------------------------------------------ hafted weapons

        /** An axe head (star-shaped outline in XZ about Eye), thinning to its edge; Edge marks the outline points on the bit. */
        void AxeHead(Mesh& Out, const Material& M, const std::vector<V3>& Outline, const V3& Eye, const double Thick, const std::vector<bool>& Keen)
        {
            std::vector<double> Edge;
            for (std::size_t I = 0; I < Outline.size(); ++I) Edge.push_back(Keen[I] ? 0.0008 : Thick * 0.45);
            Plate(Out, M, Outline, Eye, Thick * 0.5, Edge);
        }
    }

    void Kit::BuildWeaponModels()
    {
        const Stock K;
        const auto Save = [&](const Mesh& M) { Emit(M.Name(), WriteGltf(M, "Dark Arisen art kit (weapons)")); };

        // ---- the classes (combat_physics.md 2.1-2.3, 10.1-10.5)
        {
            BladeShape S = Straight(0.24, 0.026, 0.006, true);
            S.Z0 = 0.05;
            Save(Sword("SM_Art_Weapon_Dagger", K, K.Steel, S, 0, 0.1, K.Iron));
        }
        {
            BladeShape S = Straight(0.15, 0.02, 0.004, true);
            S.Z0 = 0.04;
            Mesh M = Sword("SM_Art_Weapon_ThrowingKnife", K, K.Steel, S, 0, 0.08, K.DarkSteel);
            Save(M);
        }
        Save(Sword("SM_Art_Weapon_Cutlass", K, K.Steel, Curved(0.66, 0.036, 0.006, 0.05, 0.86), 1, 0.11, K.Brass));
        Save(Sword("SM_Art_Weapon_Rapier", K, K.Steel, Straight(0.98, 0.016, 0.006, true), 2, 0.1, K.Steel));
        Save(Sword("SM_Art_Weapon_Longsword", K, K.Steel, Straight(0.92, 0.045, 0.007, true), 0, 0.22, K.Iron));
        Save(Sword("SM_Art_Weapon_Greatsword", K, K.Steel, Straight(1.25, 0.055, 0.008, true), 0, 0.38, K.Iron));
        Save(Sword("SM_Art_Weapon_OfficerSaber", K, K.Steel, Curved(0.84, 0.03, 0.006, 0.04, 0.9), 1, 0.12, K.Brass));
        {
            // hand axe and boarding axe: a short haft; the boarding axe a spike behind its bit (a sailor's tool)
            for (const bool Boarding : {false, true})
            {
                Mesh M(Boarding ? "SM_Art_Weapon_BoardingAxe" : "SM_Art_Weapon_HandAxe");
                Haft(M, K, K.Wood, -0.3, 0.2, 0.016, {-0.28});
                AxeHead(M, K.Iron, {{-0.02, 0, 0.15}, {0.03, 0, 0.16}, {0.1, 0, 0.21}, {0.12, 0, 0.17}, {0.12, 0, 0.09}, {0.1, 0, 0.06}, {0.03, 0, 0.11}, {-0.02, 0, 0.11}},
                        {0.0, 0, 0.13}, 0.03, {false, false, true, true, true, true, false, false});
                if (Boarding) M.Tube(K.Iron, {-0.02, 0, 0.13}, {-0.13, 0, 0.12}, 0.012, 0.002, 6);
                Save(M);
            }
        }
        {
            Mesh M("SM_Art_Weapon_BattleAxe");
            Haft(M, K, K.Wood, -0.55, 0.45, 0.02, {-0.5, 0.0});
            for (const double S : {1.0, -1.0})
                AxeHead(M, K.Iron, {{0, 0, 0.32}, {S * 0.06, 0, 0.36}, {S * 0.2, 0, 0.47}, {S * 0.23, 0, 0.36}, {S * 0.2, 0, 0.24}, {S * 0.06, 0, 0.3}},
                        {0, 0, 0.34}, 0.035, {false, false, true, true, true, false});
            Save(M);
        }
        {
            Mesh M("SM_Art_Weapon_WarHammer");
            Haft(M, K, K.Wood, -0.45, 0.32, 0.018, {-0.42});
            M.Box(K.Iron, {-0.03, -0.03, 0.3}, {0.09, 0.03, 0.36});
            M.Tube(K.Iron, {-0.03, 0, 0.33}, {-0.14, 0, 0.31}, 0.022, 0.003, 6);
            M.Tube(K.Iron, {0, 0, 0.36}, {0, 0, 0.44}, 0.012, 0.002, 6);
            Save(M);
        }
        {
            Mesh M("SM_Art_Weapon_Mace");
            Haft(M, K, K.Wood, -0.22, 0.24, 0.016, {-0.2});
            for (int F = 0; F < 7; ++F)
            {
                const double A = 2.0 * Pi * F / 7;
                M.Push(Xf::Translate({0, 0, 0.27}) * Xf::RotateZ(A));
                M.Box(K.Iron, {0.0, -0.005, -0.045}, {0.05, 0.005, 0.045});
                M.Pop();
            }
            Knob(M, K.Iron, {0, 0, 0.27}, 0.025, 0.025, 0.05);
            Save(M);
        }
        {
            Mesh M("SM_Art_Weapon_Spear");
            Haft(M, K, K.Wood, -1.0, 0.9, 0.016, {0.88});
            BladeShape S;
            S.Z0 = 0.9; S.Length = 0.28; S.DoubleEdged = true;
            S.Width = [](double T) { return 0.05 * Sin(Pi * std::min(0.35 + T, 1.0)) * (1.0 - T) + 0.004; };
            S.Thick = [](double T) { return 0.012 * (1.0 - T); };
            S.Bend = [](double) { return 0.0; };
            Blade(M, K.Steel, S, 16);
            Save(M);
        }
        {
            Mesh M("SM_Art_Weapon_Halberd");
            Haft(M, K, K.Wood, -1.1, 0.9, 0.018, {0.75, 0.9});
            AxeHead(M, K.Iron, {{0, 0, 0.72}, {0.08, 0, 0.7}, {0.22, 0, 0.66}, {0.24, 0, 0.82}, {0.2, 0, 0.95}, {0.08, 0, 0.9}, {0, 0, 0.9}},
                    {0.02, 0, 0.8}, 0.03, {false, false, true, true, true, false, false});
            M.Tube(K.Iron, {0, 0, 0.82}, {-0.16, 0, 0.86}, 0.014, 0.002, 6);
            BladeShape S = Straight(0.26, 0.03, 0.01, true);
            S.Z0 = 0.9;
            Blade(M, K.Steel, S, 12);
            Save(M);
        }
        {
            // flintlock pistol, musket and blunderbuss: barrel, stock, lock, cock, frizzen, trigger guard, brass furniture
            struct Gun { const char* Name; double Barrel, Bore, Flare, Stock; };
            for (const Gun& G : {Gun{"SM_Art_Weapon_Pistol", 0.26, 0.011, 0.0, 0.16}, Gun{"SM_Art_Weapon_Musket", 1.05, 0.012, 0.0, 0.42},
                                 Gun{"SM_Art_Weapon_Blunderbuss", 0.48, 0.016, 0.03, 0.34}})
            {
                Mesh M(G.Name);
                // the grip at the origin; the barrel forward along +Z above it, the stock back and down
                const double BarrelY = 0.0, BarrelZ0 = 0.06, BarrelX = 0.045;
                M.Tube(K.DarkSteel, {BarrelX, BarrelY, BarrelZ0}, {BarrelX, BarrelY, BarrelZ0 + G.Barrel}, G.Bore + 0.006, G.Bore + 0.005 + G.Flare, 10);
                M.Tube(K.DarkWood, {BarrelX - 0.012, 0, BarrelZ0}, {BarrelX - 0.012, 0, BarrelZ0 + G.Barrel * 0.85}, 0.014, 0.012, 8);
                M.Tube(K.DarkWood, {BarrelX - 0.005, 0, BarrelZ0 + 0.01}, {0.0, 0, 0.0}, 0.02, 0.018, 8);
                M.Tube(K.DarkWood, {0.0, 0, 0.0}, {-0.05, 0, -G.Stock}, 0.018, 0.026, 8);
                Knob(M, K.Brass, {-0.05, 0, -G.Stock - 0.005}, 0.03, 0.022, 0.015);
                M.Box(K.DarkSteel, {BarrelX - 0.02, 0.012, BarrelZ0 - 0.01}, {BarrelX + 0.008, 0.02, BarrelZ0 + 0.06});
                M.Tube(K.DarkSteel, {BarrelX + 0.005, 0.02, BarrelZ0 + 0.01}, {BarrelX + 0.03, 0.02, BarrelZ0 + 0.0}, 0.004, 0.003, 6);
                M.Tube(K.DarkSteel, {BarrelX + 0.03, 0.02, BarrelZ0}, {BarrelX + 0.035, 0.02, BarrelZ0 + 0.015}, 0.003, 0.003, 6);
                M.Box(K.DarkSteel, {BarrelX + 0.004, 0.014, BarrelZ0 + 0.035}, {BarrelX + 0.02, 0.02, BarrelZ0 + 0.045});
                const V3 Guard[] = {{0.02, 0, 0.05}, {-0.012, 0, 0.045}, {-0.02, 0, 0.01}, {0.0, 0, -0.01}};
                for (int I = 0; I < 3; ++I) M.Tube(K.Brass, Guard[I], Guard[I + 1], 0.003, 0.003, 6);
                M.Tube(K.DarkSteel, {0.01, 0, 0.03}, {-0.006, 0, 0.025}, 0.003, 0.002, 5);
                M.Tube(K.Brass, {BarrelX, 0, BarrelZ0 + G.Barrel - 0.02}, {BarrelX, 0, BarrelZ0 + G.Barrel - 0.005}, G.Bore + 0.008 + G.Flare, G.Bore + 0.008 + G.Flare, 10);
                M.Tube(K.DarkSteel, {BarrelX - 0.02, 0, BarrelZ0 + 0.04}, {BarrelX - 0.02, 0, BarrelZ0 + G.Barrel * 0.8}, 0.003, 0.003, 5);  // the ramrod
                Save(M);
            }
        }
        {
            // a recurve bow strung at rest: limbs from the grip, recurved tips, the string straight between them
            Mesh M("SM_Art_Weapon_Bow");
            std::vector<V3> Tips;
            for (const double S : {1.0, -1.0})
            {
                V3 Prev{0.0, 0, S * 0.05};
                for (int K2 = 1; K2 <= 10; ++K2)
                {
                    const double T = K2 / 10.0, Z = S * (0.05 + 0.6 * T), X = -0.12 * Sin(Pi * 0.5 * T) + (T > 0.8 ? 1.2 * (T - 0.8) * (T - 0.8) : 0.0);
                    const V3 P{X, 0, Z};
                    M.Tube(K.DarkWood, Prev, P, Lerp(0.016, 0.006, T - 0.1), Lerp(0.016, 0.006, T), 7);
                    Prev = P;
                }
                Tips.push_back(Prev);
            }
            Grip(M, K.Leather, -0.06, 0.06, 0.018);
            M.Tube(K.Cord, Tips[0], Tips[1], 0.0015, 0.0015, 4);
            Save(M);
        }
        {
            // a round shield of planks with iron bands, an iron rim and a domed iron boss, held at the boss's grip
            Mesh M("SM_Art_Weapon_RoundShield");
            const double R = 0.42;
            M.Tube(K.Planks, {0, -0.012, 0}, {0, 0.012, 0}, R, R, 28);
            Loop(M, K.Iron, {0, 0, 0}, {1, 0, 0}, {0, 0, 1}, R, 0.012, 28);
            for (const double X : {-0.16, 0.16}) M.Box(K.Iron, {X - 0.02, 0.012, -std::sqrt(R * R - X * X) + 0.01}, {X + 0.02, 0.016, std::sqrt(R * R - X * X) - 0.01});
            M.Push(Xf::Translate({0, 0.012, 0}) * Xf::RotateX(-Pi / 2));
            M.Tube(K.Iron, {0, 0, 0}, {0, 0, 0.001}, 0.1, 0.1, 16);
            M.Pop();
            Knob(M, K.Iron, {0, 0.012, 0}, 0.085, 0.06, 0.085);
            M.Tube(K.Wood, {-0.08, -0.04, 0}, {0.08, -0.04, 0}, 0.014, 0.014, 8);
            Save(M);
        }

        // ---- the bosses' own (docs/design/bosses)
        {
            // Brother Cleaver's Great Cleaver: 80 cm, forged from his church's bell, the parish cross stamped in the blade
            Mesh M("SM_Art_Weapon_GreatCleaver");
            Grip(M, K.DarkWood, -0.14, 0.12, 0.02);
            Knob(M, K.Bronze, {0, 0, -0.155}, 0.025, 0.025, 0.02);
            Plate(M, K.Bronze, {{-0.06, 0, 0.12}, {0.0, 0, 0.12}, {0.17, 0, 0.13}, {0.19, 0, 0.4}, {0.18, 0, 0.66}, {0.1, 0, 0.67}, {-0.06, 0, 0.62}},
                  {0.04, 0, 0.38}, 0.007, {0.006, 0.006, 0.0008, 0.0008, 0.0008, 0.004, 0.006});
            for (const double Y : {0.0072, -0.0072})
            {
                M.Box(K.Bronze, {0.035, Y - 0.0015, 0.38}, {0.05, Y + 0.0015, 0.5});
                M.Box(K.Bronze, {0.0, Y - 0.0015, 0.45}, {0.085, Y + 0.0015, 0.465});
            }
            Save(M);
        }
        {
            // the six ritual knives on his apron: small cleavers, all alike
            Mesh M("SM_Art_Weapon_RitualCleaver");
            Grip(M, K.DarkWood, -0.05, 0.05, 0.011);
            Plate(M, K.DarkSteel, {{-0.02, 0, 0.05}, {0.06, 0, 0.05}, {0.07, 0, 0.17}, {0.03, 0, 0.18}, {-0.02, 0, 0.16}}, {0.02, 0, 0.11}, 0.003, {0.003, 0.0006, 0.0006, 0.002, 0.003});
            Save(M);
        }
        {
            // the bone censer: a human pelvis made a bowl, swung on a chain
            Mesh M("SM_Art_Weapon_BoneCenser");
            for (int L = 0; L < 14; ++L)
            {
                const V3 C{0, 0, -0.04 * L};
                if (L % 2) Loop(M, K.Iron, C, {0, 1, 0}, {0, 0, 1}, 0.022, 0.004, 8);
                else Loop(M, K.Iron, C, {1, 0, 0}, {0, 0, 1}, 0.022, 0.004, 8);
            }
            const double Hang = -0.6;
            std::vector<std::vector<V3>> Bowl;
            for (int K2 = 0; K2 <= 6; ++K2)
            {
                const double T = K2 / 6.0, R = 0.05 + 0.08 * Sin(Pi * 0.5 * T), Z = Hang - 0.12 + 0.12 * T;
                std::vector<V3> Rg;
                for (int J = 0; J < 14; ++J)
                {
                    const double A = 2.0 * Pi * J / 14, Rim = T > 0.99 ? 1.0 + 0.12 * Sin(3.0 * A) : 1.0;
                    Rg.push_back({R * Rim * Cos(A) * 1.25, R * Rim * Sin(A) * 0.85, Z});
                }
                Bowl.push_back(Rg);
            }
            M.Loft(K.Bone, Bowl, true);
            for (const double S : {1.0, -1.0})
            {
                M.Push(Xf::Translate({S * 0.13, 0, Hang + 0.02}) * Xf::RotateY(S * 0.5));
                Knob(M, K.Bone, {0, 0, 0}, 0.07, 0.015, 0.05);
                M.Pop();
                M.Tube(K.Iron, {S * 0.12, 0, Hang + 0.01}, {0, 0, Hang + 0.05}, 0.003, 0.003, 5);
            }
            M.Tube(K.Iron, {0, 0, Hang + 0.05}, {0, 0, -0.04 * 13}, 0.003, 0.003, 5);
            Save(M);
        }
        {
            // Corazon's Toledo dueling sabres (85 cm, lighter than standard) and her 20 cm parrying dagger
            Save(Sword("SM_Art_Weapon_ToledoSaber", K, K.Steel, Curved(0.85, 0.024, 0.005, 0.03, 0.9), 1, 0.11, K.Steel));
            BladeShape S = Straight(0.2, 0.022, 0.005, true);
            S.Z0 = 0.05;
            Mesh M = Sword("SM_Art_Weapon_ParryingDagger", K, K.Steel, S, 0, 0.09, K.Steel);
            Loop(M, K.Steel, {0, 0.02, 0.07}, {1, 0, 0}, {0, 0, 1}, 0.018, 0.0035, 10);
            Save(M);
        }
        {
            // the Scarred Twins: Kira's 70 cm shortswords of dark steel
            Save(Sword("SM_Art_Weapon_TwinShortsword", K, K.DarkSteel, Straight(0.58, 0.04, 0.007, true), 0, 0.12, K.DarkSteel));
        }
        {
            // Silvano's Tide-Staff: two metres of polished dragon-bone capped with an iron wave
            Mesh M("SM_Art_Weapon_TideStaff");
            M.Tube(K.Bone, {0, 0, -1.0}, {0, 0, 0.9}, 0.016, 0.019, 10);
            Knob(M, K.Bone, {0, 0, -1.0}, 0.018, 0.018, 0.01);
            // the wave: a crest curling over itself above a collar
            M.Tube(K.Iron, {0, 0, 0.9}, {0, 0, 0.94}, 0.024, 0.024, 10);
            V3 Prev{-0.04, 0, 0.94};
            for (int K2 = 1; K2 <= 14; ++K2)
            {
                const double A = Pi * 1.6 * K2 / 14, R = 0.07 * (1.0 - 0.55 * K2 / 14);
                const V3 P{0.02 - R * Cos(A), 0, 1.0 + R * Sin(A)};
                M.Tube(K.Iron, Prev, P, Lerp(0.02, 0.006, (K2 - 1) / 14.0), Lerp(0.02, 0.006, K2 / 14.0), 8);
                Prev = P;
            }
            Save(M);
        }
        {
            // the Red Lieutenant's dragon-bone saber: 90 cm, pale cream-white with dark veining, faintly glowing orange in the dark
            Material Dragonbone = Scanned("M_Art_Weapon_Dragonbone", "painted_plaster_wall", 0.95, 0.9, 0.78, 0.15, 0.35, 0.0);
            Dragonbone.Emissive = {0.3, 0.1, 0.02};  // a faint orange only the dark shows; cream-white by day
            Save(Sword("SM_Art_Weapon_DragonboneSaber", K, Dragonbone, Curved(0.9, 0.032, 0.007, 0.045, 0.9), 1, 0.12, K.Brass));
        }
        {
            // the Twin Hooks' boarding hooks: 1.2 m of Fjordlund steel, a curved primary hook and a smaller secondary spike
            Mesh M("SM_Art_Weapon_BoardingHook");
            Haft(M, K, K.DarkWood, -0.55, 0.45, 0.017, {-0.52, 0.1, 0.42});
            V3 Prev{0, 0, 0.45};
            for (int K2 = 1; K2 <= 10; ++K2)
            {
                const double A = Pi * 0.95 * K2 / 10;
                const V3 P{0.1 * (1.0 - Cos(A)), 0, 0.45 + 0.12 * Sin(A) + 0.12 * K2 / 10.0};
                M.Tube(K.DarkSteel, Prev, P, Lerp(0.018, 0.003, (K2 - 1) / 10.0), Lerp(0.018, 0.003, K2 / 10.0), 8);
                Prev = P;
            }
            M.Tube(K.DarkSteel, {0, 0, 0.47}, {-0.11, 0, 0.53}, 0.012, 0.002, 6);
            M.Tube(K.DarkSteel, {0, 0, 0.45}, {0, 0, 0.66}, 0.012, 0.002, 6);
            Save(M);
        }
        {
            // Ulfar's Brytari, "Breaker": a massive bearded axe, two-handed
            Mesh M("SM_Art_Weapon_Brytari");
            Haft(M, K, K.Wood, -0.62, 0.38, 0.021, {-0.6, -0.2, 0.36});
            AxeHead(M, K.Iron, {{-0.03, 0, 0.27}, {0.04, 0, 0.29}, {0.2, 0, 0.36}, {0.25, 0, 0.3}, {0.26, 0, 0.16}, {0.22, 0, 0.02}, {0.15, 0, 0.0}, {0.12, 0, 0.12},
                                {0.04, 0, 0.2}, {-0.03, 0, 0.2}},
                    {0.03, 0, 0.24}, 0.04, {false, false, true, true, true, true, true, false, false, false});
            Save(M);
        }
        {
            // Viktor's flare pistols: short, wide-bored, brass
            Mesh M("SM_Art_Weapon_FlarePistol");
            M.Tube(K.Brass, {0.04, 0, 0.05}, {0.04, 0, 0.24}, 0.022, 0.024, 12);
            M.Tube(K.DarkWood, {0.03, 0, 0.05}, {0.0, 0, 0.0}, 0.02, 0.018, 8);
            M.Tube(K.DarkWood, {0.0, 0, 0.0}, {-0.04, 0, -0.12}, 0.018, 0.024, 8);
            const V3 Guard[] = {{0.02, 0, 0.05}, {-0.012, 0, 0.045}, {-0.02, 0, 0.01}, {0.0, 0, -0.01}};
            for (int I = 0; I < 3; ++I) M.Tube(K.Brass, Guard[I], Guard[I + 1], 0.003, 0.003, 6);
            M.Tube(K.DarkSteel, {0.04, 0.02, 0.06}, {0.065, 0.02, 0.05}, 0.004, 0.003, 6);
            Save(M);
        }
        {
            // Mira's composite recurve bow (the Scarred Twins): heavier limbs, horn-dark tips
            Mesh M("SM_Art_Weapon_CompositeBow");
            std::vector<V3> Tips;
            for (const double S : {1.0, -1.0})
            {
                V3 Prev{0.0, 0, S * 0.06};
                for (int K2 = 1; K2 <= 12; ++K2)
                {
                    const double T = K2 / 12.0, Z = S * (0.06 + 0.66 * T), X = -0.14 * Sin(Pi * 0.6 * T) + (T > 0.75 ? 0.9 * (T - 0.75) * (T - 0.75) : 0.0);
                    const V3 P{X, 0, Z};
                    M.Tube(T > 0.8 ? K.DarkSteel : K.Leather, Prev, P, Lerp(0.02, 0.007, T - 1.0 / 12), Lerp(0.02, 0.007, T), 7);
                    Prev = P;
                }
                Tips.push_back(Prev);
            }
            Grip(M, K.Cord, -0.07, 0.07, 0.02);
            M.Tube(K.Cord, Tips[0], Tips[1], 0.0016, 0.0016, 4);
            Save(M);
        }
    }
}
