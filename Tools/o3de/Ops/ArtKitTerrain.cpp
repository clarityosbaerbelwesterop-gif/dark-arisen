// Ground for land levels (ArtKit.h, Kit::Terrain): a heightfield that carries the authored route on a
// flat corridor, rises into rolling country, moor or jungle beyond it and frames the space with hills,
// painted by biome (heather, dry grass, dirt roads, rock on slopes, mud, red earth), with ground cover
// cards near the route and shared plant models (trees, bushes, rocks, standing stones) placed on it.
// Only + - * / and sqrt reach a file (own sine and cosine), so the output is the same on every compiler.

#include "ArtKit.h"
#include "ArtKitLibrary.h"

#include <algorithm>
#include <cmath>
#include <map>

namespace DarkArisen::Tools::Art
{
    namespace
    {
        // ------------------------------------------------------------------ noise

        std::uint32_t Mix(std::uint32_t H)
        {
            H ^= H >> 16;
            H *= 0x7feb352du;
            H ^= H >> 15;
            H *= 0x846ca68bu;
            H ^= H >> 16;
            return H;
        }
        double Lattice(const std::int64_t X, const std::int64_t Y, const std::uint32_t Seed)
        {
            const std::uint32_t H = Mix(static_cast<std::uint32_t>(X) * 0x9e3779b1u ^ Mix(static_cast<std::uint32_t>(Y) * 0x85ebca77u ^ Seed));
            return static_cast<double>(H >> 8) / 16777216.0;
        }
        /** Smooth value noise in [0, 1) at a point (cells of size 1). */
        double Value(const double X, const double Y, const std::uint32_t Seed)
        {
            const double Fx = std::floor(X), Fy = std::floor(Y);
            const auto Ix = static_cast<std::int64_t>(Fx), Iy = static_cast<std::int64_t>(Fy);
            const double Tx = X - Fx, Ty = Y - Fy;
            const double Sx = Tx * Tx * (3.0 - 2.0 * Tx), Sy = Ty * Ty * (3.0 - 2.0 * Ty);
            const double A = Lattice(Ix, Iy, Seed), B = Lattice(Ix + 1, Iy, Seed), C = Lattice(Ix, Iy + 1, Seed), D = Lattice(Ix + 1, Iy + 1, Seed);
            return Lerp(Lerp(A, B, Sx), Lerp(C, D, Sx), Sy);
        }
        double Fractal(const double X, const double Y, const double Scale, const int Octaves, const std::uint32_t Seed)
        {
            double Sum = 0.0, Amp = 0.5, Norm = 0.0, F = 1.0 / Scale;
            for (int O = 0; O < Octaves; ++O)
            {
                Sum += Amp * Value(X * F, Y * F, Seed + static_cast<std::uint32_t>(O) * 101u);
                Norm += Amp;
                Amp *= 0.5;
                F *= 2.03;
            }
            return Sum / Norm;
        }

        // ------------------------------------------------------------------ plants

        /** Crossed alpha cards (Count planes through the vertical axis), foot at At. */
        void Tuft(Mesh& Out, const Material& Mt, const V3& At, const double Width, const double Height, const double Yaw, const int Count)
        {
            for (int K = 0; K < Count; ++K)
            {
                const double A = Yaw + Pi * K / Count;
                const V3 D{Cos(A) * Width * 0.5, Sin(A) * Width * 0.5, 0.0};
                const V3 Lean{Cos(A + 1.3) * Height * 0.12, Sin(A + 1.3) * Height * 0.12, 0.0};
                Out.Card(Mt, At - D, At + D, At + D + Lean + V3{0, 0, Height}, At - D + Lean + V3{0, 0, Height});
            }
        }

        /** A leafy cluster: three crossed cards, slightly tilted. */
        void Cluster(Mesh& Out, const Material& Mt, const V3& C, const double Size, const double Spin)
        {
            for (int K = 0; K < 3; ++K)
            {
                const double A = Spin + Pi * K / 3.0;
                const V3 D{Cos(A) * Size * 0.5, Sin(A) * Size * 0.5, 0.0};
                const V3 Tilt{-Sin(A) * Size * 0.18, Cos(A) * Size * 0.18, 0.0};
                const V3 Bottom = C - V3{0, 0, Size * 0.45};
                Out.Card(Mt, Bottom - D - Tilt, Bottom + D - Tilt, Bottom + D + Tilt + V3{0, 0, Size}, Bottom - D + Tilt + V3{0, 0, Size});
            }
        }

        /** Branching tree: tapered trunk, Limbs main branches, leaf clusters along their outer halves. */
        Mesh BroadleafTree(const std::string& Name, const double Height, const std::uint32_t Seed, const M BarkMat, const M LeafMat, const int Limbs,
            const double Spread, const double ClusterSize, const double Girth = 1.0)
        {
            Mesh Out(Name);
            Rng R(Seed);
            const double Lean = R.Range(-0.08, 0.08) * Height, LeanYaw = R.Range(0.0, 2.0 * Pi);
            const double TrunkTop = Height * R.Range(0.45, 0.6);
            const V3 TopP{Cos(LeanYaw) * Lean, Sin(LeanYaw) * Lean, TrunkTop};
            const double Base = (0.022 * Height + 0.06) * Girth;
            Out.Tube(Mat(BarkMat), {0, 0, -0.3}, V3{TopP.X * 0.5, TopP.Y * 0.5, TrunkTop * 0.5}, Base * 1.25, Base * 0.85, 9, false);
            Out.Tube(Mat(BarkMat), V3{TopP.X * 0.5, TopP.Y * 0.5, TrunkTop * 0.5}, TopP, Base * 0.85, Base * 0.6, 8, false);
            for (int L = 0; L < Limbs; ++L)
            {
                const double A = 2.0 * Pi * L / Limbs + R.Range(-0.4, 0.4);
                const double Up = R.Range(0.35, 0.75);
                const double Reach = Spread * R.Range(0.7, 1.0);
                const V3 From = TopP - V3{0, 0, R.Range(0.0, 0.25) * TrunkTop};
                const V3 Mid = From + V3{Cos(A) * Reach * 0.5, Sin(A) * Reach * 0.5, (Height - TrunkTop) * Up * 0.6};
                const V3 Tip = From + V3{Cos(A) * Reach, Sin(A) * Reach, (Height - TrunkTop) * (Up + 0.15)};
                Out.Tube(Mat(BarkMat), From, Mid, Base * 0.5, Base * 0.32, 6, false);
                Out.Tube(Mat(BarkMat), Mid, Tip, Base * 0.32, Base * 0.12, 5, false);
                const int Clusters = 5 + static_cast<int>(R.Unit() * 4);
                for (int K = 0; K < Clusters; ++K)
                {
                    const double T = R.Range(0.35, 1.05);
                    const V3 P = Mid + (Tip - Mid) * (T * 2.0 - 1.0 > 0 ? T * 2.0 - 1.0 : 0.0) + V3{R.Range(-0.6, 0.6), R.Range(-0.6, 0.6), R.Range(-0.3, 0.5)} * ClusterSize;
                    // draws are taken in statement order: argument evaluation order differs between compilers
                    const double Size = ClusterSize * R.Range(0.8, 1.25), Spin = R.Range(0.0, Pi);
                    Cluster(Out, Mat(LeafMat), T < 0.5 ? From + (Mid - From) * (T * 2.0) + V3{0, 0, ClusterSize * 0.3} : P, Size, Spin);
                }
            }
            // crown top
            for (int K = 0; K < 4; ++K)
            {
                const V3 At = TopP + V3{R.Range(-0.5, 0.5) * Spread * 0.5, R.Range(-0.5, 0.5) * Spread * 0.5, Height - TrunkTop - ClusterSize * 0.3};
                const double Spin = R.Range(0.0, Pi);
                Cluster(Out, Mat(LeafMat), At, ClusterSize * 1.2, Spin);
            }
            return Out;
        }

        /** Scots pine: tall bare trunk, a flattened crown of needle sprays near the top. */
        Mesh Pine(const std::string& Name, const double Height, const std::uint32_t Seed)
        {
            Mesh Out(Name);
            Rng R(Seed);
            const double Bend = R.Range(0.03, 0.09) * Height, BendYaw = R.Range(0.0, 2.0 * Pi);
            const auto Axis = [&](const double T) { return V3{Cos(BendYaw) * Bend * T * T, Sin(BendYaw) * Bend * T * T, Height * T}; };
            const double Base = 0.02 * Height + 0.08;
            for (int S = 0; S < 4; ++S)
            {
                const double T0 = S / 4.0, T1 = (S + 1) / 4.0;
                Out.Tube(Mat(M::Bark), Axis(T0) - V3{0, 0, S == 0 ? 0.3 : 0.0}, Axis(T1), Base * (1.0 - 0.6 * T0), Base * (1.0 - 0.6 * T1), 8, false);
            }
            const int Whorls = 6;
            for (int W = 0; W < Whorls; ++W)
            {
                const double T = 0.62 + 0.36 * W / (Whorls - 1);
                const V3 C = Axis(T);
                const int Arms = 4 + static_cast<int>(R.Unit() * 3);
                const double Reach = Height * 0.16 * (1.15 - 0.6 * (T - 0.62) / 0.36);
                for (int A = 0; A < Arms; ++A)
                {
                    const double Ang = 2.0 * Pi * A / Arms + R.Range(-0.3, 0.3) + W;
                    const V3 Dir{Cos(Ang), Sin(Ang), R.Range(0.05, 0.25)};
                    const V3 Tip = C + Dir * Reach;
                    Out.Tube(Mat(M::Bark), C, Tip, Base * 0.25, Base * 0.08, 4, false);
                    for (int K = 0; K < 3; ++K) Cluster(Out, Mat(M::Needles), C + (Tip - C) * (0.4 + 0.3 * K) + V3{0, 0, 0.2}, Reach * 0.85, Ang + K);
                }
            }
            return Out;
        }

        /** Low shrub: clusters on a dome. */
        Mesh Shrub(const std::string& Name, const double Radius, const double Height, const std::uint32_t Seed, const M LeafMat, const int Count)
        {
            Mesh Out(Name);
            Rng R(Seed);
            for (int K = 0; K < Count; ++K)
            {
                const double A = R.Range(0.0, 2.0 * Pi), Rr = Radius * R.Unit();
                const double H = Height * (0.35 + 0.65 * (1.0 - Rr / Radius)) * R.Range(0.7, 1.0);
                const double Size = Height * R.Range(0.7, 1.0), Spin = R.Range(0.0, Pi);
                Cluster(Out, Mat(LeafMat), {Cos(A) * Rr, Sin(A) * Rr, H * 0.55}, Size, Spin);
            }
            return Out;
        }

        /** Mangrove: arched prop roots lifting a short trunk, dark leaf clusters. */
        Mesh Mangrove(const std::string& Name, const double Height, const std::uint32_t Seed)
        {
            Mesh Out(Name);
            Rng R(Seed);
            const double Hub = Height * 0.28;
            for (int K = 0; K < 7; ++K)
            {
                const double A = 2.0 * Pi * K / 7 + R.Range(-0.3, 0.3), Reach = Height * R.Range(0.25, 0.4);
                const V3 Foot{Cos(A) * Reach, Sin(A) * Reach, -0.3};
                const V3 Knee{Cos(A) * Reach * 0.6, Sin(A) * Reach * 0.6, Hub * 0.9};
                Out.Tube(Mat(M::Bark), Foot, Knee, 0.05, 0.06, 5, false);
                Out.Tube(Mat(M::Bark), Knee, {0, 0, Hub}, 0.06, 0.09, 5, false);
            }
            Mesh Crown = BroadleafTree(Name, Height - Hub, Seed + 9u, M::Bark, M::LeavesDark, 6, Height * 0.35, Height * 0.22);
            Out.Push(Xf::Translate({0, 0, Hub}));
            Out.Append(Crown);
            Out.Pop();
            return Out;
        }

        /** Dead tree: grey bare trunk and a few broken limbs. */
        Mesh DeadTree(const std::string& Name, const double Height, const std::uint32_t Seed)
        {
            Mesh Out(Name);
            Rng R(Seed);
            const double Base = 0.04 * Height + 0.06;
            Out.Tube(Mat(M::Driftwood), {0, 0, -0.3}, {0, 0, Height}, Base, Base * 0.3, 7, true);
            for (int K = 0; K < 5; ++K)
            {
                const double A = R.Range(0.0, 2.0 * Pi), Z = Height * R.Range(0.35, 0.85), L = Height * R.Range(0.15, 0.35);
                Out.Tube(Mat(M::Driftwood), {0, 0, Z}, {Cos(A) * L, Sin(A) * L, Z + L * R.Range(0.2, 0.8)}, Base * 0.35, Base * 0.1, 4, true);
            }
            return Out;
        }

        /** Rough monolith, slightly leaning, lichen patched. */
        Mesh StandingStone(const std::string& Name, const double Height, const std::uint32_t Seed)
        {
            Mesh Out(Name);
            Rock(Out, Mat(M::Lichen), {0, 0, 0}, Height * 0.24, Height, Seed);
            return Out;
        }

        Mesh Boulder(const std::string& Name, const double Size, const std::uint32_t Seed, const M Stone)
        {
            Mesh Out(Name);
            Rock(Out, Mat(Stone), {0, 0, 0}, Size, Size * 0.65, Seed);
            Rng R(Seed + 3u);
            for (int K = 0; K < 2; ++K)
            {
                const double A = R.Range(0.0, 2.0 * Pi);
                Rock(Out, Mat(Stone), {Cos(A) * Size * 0.9, Sin(A) * Size * 0.9, 0}, Size * 0.4, Size * 0.3, Seed + 7u + static_cast<std::uint32_t>(K));
            }
            return Out;
        }

        enum class Species { Broadleaf, Pine, Birch, Shrub, Gorse, Fern, Reeds, Palm, Mangrove, Dead, Stone, Boulder };

        struct PlantKind
        {
            Species Kind;
            double Share;   // cumulative chance per planting cell
            bool Tree;      // needs gentle ground and room
        };

        struct BiomeRules
        {
            double Hills, EdgeRise;              // metres
            double CoverDensity;                 // tufts per 4 m2 near the route
            std::vector<PlantKind> Plants;
            double CellSize;                     // planting grid (m)
        };

        BiomeRules RulesFor(const Biome Kind)
        {
            switch (Kind)
            {
            case Biome::Moor:
                return {5.5, 16.0, 0.75,
                    {{Species::Stone, 0.025, false}, {Species::Pine, 0.07, true}, {Species::Birch, 0.14, true}, {Species::Gorse, 0.42, false},
                        {Species::Boulder, 0.52, false}},
                    8.0};
            case Biome::Tropical:
                return {8.0, 22.0, 0.6,
                    {{Species::Palm, 0.14, true}, {Species::Broadleaf, 0.26, true}, {Species::Shrub, 0.5, false}, {Species::Fern, 0.62, false},
                        {Species::Boulder, 0.68, false}},
                    7.0};
            case Biome::Mire:
                return {1.2, 5.0, 0.8,
                    {{Species::Mangrove, 0.18, true}, {Species::Dead, 0.26, true}, {Species::Reeds, 0.6, false}, {Species::Shrub, 0.68, false}},
                    6.5};
            case Biome::Grove:
                return {4.5, 18.0, 0.85,
                    {{Species::Broadleaf, 0.3, true}, {Species::Palm, 0.38, true}, {Species::Shrub, 0.58, false}, {Species::Fern, 0.78, false},
                        {Species::Boulder, 0.82, false}},
                    6.0};
            case Biome::Dream:
            default:
                return {2.5, 8.0, 0.05, {{Species::Dead, 0.06, true}, {Species::Boulder, 0.16, false}}, 9.0};
            }
        }

        const char* SpeciesName(const Species S)
        {
            switch (S)
            {
            case Species::Broadleaf: return "Broadleaf";
            case Species::Pine: return "Pine";
            case Species::Birch: return "Birch";
            case Species::Shrub: return "Shrub";
            case Species::Gorse: return "Gorse";
            case Species::Fern: return "Fern";
            case Species::Reeds: return "Reeds";
            case Species::Palm: return "Palm";
            case Species::Mangrove: return "Mangrove";
            case Species::Dead: return "DeadTree";
            case Species::Stone: return "StandingStone";
            case Species::Boulder: default: return "Boulder";
            }
        }

        Mesh BuildPlant(const Species S, const int Variant, const Biome Kind)
        {
            const std::uint32_t Seed = 9000u + static_cast<std::uint32_t>(S) * 97u + static_cast<std::uint32_t>(Variant) * 13u;
            const std::string Name = std::string("SM_Art_Veg_") + SpeciesName(S) + "_" + std::to_string(Variant);
            Rng R(Seed);
            switch (S)
            {
            case Species::Broadleaf:
            {
                const double Height = R.Range(7.0, 11.0), Spread = R.Range(2.8, 3.8);
                return BroadleafTree(Name, Height, Seed, M::Bark, Variant == 1 ? M::LeavesDark : M::Leaves, 7, Spread, 1.9, 1.1);
            }
            case Species::Pine: return Pine(Name, R.Range(9.0, 13.0), Seed);
            case Species::Birch:
            {
                const double Height = R.Range(6.5, 9.0), Spread = R.Range(1.6, 2.2);
                return BroadleafTree(Name, Height, Seed, M::BirchBark, M::LeavesPale, 6, Spread, 1.2, 0.5);
            }
            case Species::Shrub:
            {
                const double Height = R.Range(0.8, 1.4), Radius = R.Range(0.9, 1.4);
                return Shrub(Name, Height, Radius, Seed, Kind == Biome::Mire ? M::LeavesDark : M::Leaves, 9);
            }
            case Species::Gorse:
            {
                const double Height = R.Range(0.7, 1.2), Radius = R.Range(0.7, 1.1);
                return Shrub(Name, Height, Radius, Seed, M::Gorse, 10);
            }
            case Species::Fern:
            {
                Mesh Out(Name);
                for (int K = 0; K < 7; ++K)
                {
                    const double A = 2.0 * Pi * K / 7.0 + R.Range(-0.2, 0.2), L = R.Range(0.8, 1.2);
                    const V3 D{Cos(A), Sin(A), 0};
                    const V3 Side{-Sin(A) * 0.22, Cos(A) * 0.22, 0};
                    Out.Card(Mat(M::Fern), Side * -1.0, Side, D * L + Side + V3{0, 0, 0.55 * L}, D * L - Side + V3{0, 0, 0.55 * L});
                }
                return Out;
            }
            case Species::Reeds:
            {
                Mesh Out(Name);
                for (int K = 0; K < 6; ++K)
                {
                    const V3 At{R.Range(-0.6, 0.6), R.Range(-0.6, 0.6), -0.05};
                    const double Width = R.Range(0.7, 1.0), Height = R.Range(1.3, 2.0), Yaw = R.Range(0.0, Pi);
                    Tuft(Out, Mat(M::Reeds), At, Width, Height, Yaw, 2);
                }
                return Out;
            }
            case Species::Palm:
            {
                Mesh Out = BuildPalm(R.Range(6.0, 9.5), Seed);
                Mesh Named(Name);
                Named.Append(Out);
                return Named;
            }
            case Species::Mangrove: return Mangrove(Name, R.Range(5.0, 7.5), Seed);
            case Species::Dead: return DeadTree(Name, R.Range(4.5, 8.0), Seed);
            case Species::Stone: return StandingStone(Name, R.Range(2.2, 3.4), Seed);
            case Species::Boulder:
            default: return Boulder(Name, R.Range(0.7, 1.6), Seed, Kind == Biome::Dream ? M::StoneDark : M::Rock);
            }
        }
    }

    namespace
    {
        struct Wind
        {
            double Strength = 0.0, Speed = 0.0, Flutter = 0.0;  // Strength 0: the plant does not sway (stone)
        };

        Wind WindFor(const Species Kind)
        {
            switch (Kind)
            {
            case Species::Palm: return {0.35, 0.18, 0.3};
            case Species::Broadleaf: case Species::Birch: case Species::Mangrove: return {0.2, 0.22, 0.25};
            case Species::Pine: return {0.16, 0.18, 0.12};
            case Species::Dead: return {0.06, 0.2, 0.0};
            case Species::Shrub: case Species::Gorse: case Species::Fern: return {0.07, 0.45, 0.3};
            case Species::Reeds: return {0.18, 0.6, 0.2};
            case Species::Stone: case Species::Boulder: default: return {};
            }
        }

        /** DarkArisenFoliage material (Assets/Materials/Foliage) for one part of a plant. */
        std::string FoliageMaterialJson(const Material& Part, const Wind& W, const double BendHeight, const bool GroundCover = false)
        {
            using GltfBits::Num;
            const bool Leaf = Part.Mask;
            std::string J = "{\n";
            J += "    \"materialType\": \"../Types/DarkArisenFoliage.materialtype\",\n";
            J += "    \"materialTypeVersion\": 1,\n";
            J += "    \"propertyValues\": {\n";
            J += "        \"baseColor.color\": [" + Num(Part.BaseColor[0]) + ", " + Num(Part.BaseColor[1]) + ", " + Num(Part.BaseColor[2]) + "],\n";
            J += "        \"baseColor.textureMap\": \"../../Art/Textures/" + Part.BaseColorTexture + "\",\n";
            J += "        \"opacity.alphaCutoff\": " + Num(Leaf ? Part.AlphaCutoff : 0.0) + ",\n";
            J += "        \"surface.roughness\": " + Num(Part.Roughness) + ",\n";
            J += std::string("        \"translucency.color\": ") + (Leaf ? "[0.55, 0.75, 0.25]" : "[0, 0, 0]") + ",\n";
            J += "        \"wind.bendHeight\": " + Num(BendHeight) + ",\n";
            J += "        \"wind.flutter\": " + Num(Leaf ? W.Flutter : 0.0) + ",\n";
            if (GroundCover) J += "        \"wind.groundCover\": 1,\n";
            J += "        \"wind.speed\": " + Num(W.Speed) + ",\n";
            J += "        \"wind.strength\": " + Num(W.Strength) + "\n";
            J += "    }\n}\n";
            return J;
        }
    }

    TerrainArt Kit::Terrain(const TerrainSpec& Spec)
    {
        // One model per material of a plant, each with its own wind material; all parts of a plant share the
        // bend height, so trunk and crown sway together.
        const auto EmitPlant = [this](const std::string& Key, const Species Kind, const Mesh& Plant)
        {
            const Wind W = WindFor(Kind);
            double Top = 0.05;
            bool Textured = true;
            for (const Mesh::Part& P : Plant.Parts())
            {
                for (const Vertex& V : P.Vertices) Top = std::max(Top, V.P.Z);
                Textured = Textured && !P.Mat.BaseColorTexture.empty();
            }
            const bool Sways = W.Strength > 0.0 && Textured;
            std::vector<PlantPart> Parts;
            for (const Mesh::Part& P : Plant.Parts())
            {
                std::string Suffix = P.Mat.Name.rfind("M_Art_", 0) == 0 ? P.Mat.Name.substr(6) : P.Mat.Name;
                const std::string Model = "SM_Art_Veg_" + Key + (Plant.Parts().size() > 1 ? "_" + Suffix : std::string());
                Mesh One(Model);
                One.Indexed(P.Mat, P.Vertices, P.Indices);
                PlantPart Part{Emit(Model, WriteGltf(One, "Dark Arisen art kit (terrain)")), std::string()};
                if (Sways)
                {
                    Part.Material = "Assets/Materials/Foliage/" + Model + ".material";
                    Output.push_back({Part.Material, FoliageMaterialJson(P.Mat, W, std::floor(Top * 100.0 + 0.5) / 100.0)});
                }
                Parts.push_back(std::move(Part));
            }
            return Parts;
        };
        TerrainArt Art;
        const BiomeRules Rules = RulesFor(Spec.Kind);
        const std::uint32_t Seed = Spec.Seed;

        // ---- height field
        struct Near
        {
            double D = 1e30, Z = 0.0, HalfWidth = 2.0;
        };
        const auto Nearest = [&Spec](const double X, const double Y)
        {
            Near Best;
            for (const TerrainSpec::Leg& L : Spec.Route)
            {
                const double Dx = L.Bx - L.Ax, Dy = L.By - L.Ay;
                const double Len2 = Dx * Dx + Dy * Dy;
                double T = Len2 > 1e-9 ? ((X - L.Ax) * Dx + (Y - L.Ay) * Dy) / Len2 : 0.0;
                T = Clamp(T, 0.0, 1.0);
                const double Px = L.Ax + Dx * T - X, Py = L.Ay + Dy * T - Y;
                const double D = std::sqrt(Px * Px + Py * Py) - L.HalfWidth;
                if (D < Best.D)
                {
                    Best.D = D;
                    Best.Z = L.Az + (L.Bz - L.Az) * T;
                    Best.HalfWidth = L.HalfWidth;
                }
            }
            return Best;
        };
        const double Ex0 = Spec.X0, Ey0 = Spec.Y0, Ex1 = Spec.X1, Ey1 = Spec.Y1;
        const auto Height = [&](const double X, const double Y, Near* Out)
        {
            const Near N = Nearest(X, Y);
            if (Out) *Out = N;
            const double Floor = N.Z - 0.08;
            const double Away = SmoothStep(2.0, 34.0, N.D);
            const double Hills = (Fractal(X, Y, 46.0, 4, Seed) - 0.42) * 2.0 * Rules.Hills * Away;
            const double Edge = std::min(std::min(X - Ex0, Ex1 - X), std::min(Y - Ey0, Ey1 - Y));
            const double Rise = SmoothStep(42.0, 0.0, Edge);
            const double Frame = Rules.EdgeRise * Rise * Rise * (0.65 + 0.7 * Fractal(X, Y, 30.0, 3, Seed + 7u));
            double H = Floor + std::max(Hills, -1.4 * Away) + Frame;
            H = Lerp(Floor, H, SmoothStep(0.6, 7.0, N.D));
            for (const TerrainSpec::Block& K : Spec.Keep)
            {
                const double Dx = std::max(std::max(K.X0 - X, X - K.X1), 0.0), Dy = std::max(std::max(K.Y0 - Y, Y - K.Y1), 0.0);
                const double D = std::sqrt(Dx * Dx + Dy * Dy);
                if (D < 6.0) H = std::min(H, Lerp(K.Top - 0.12, H, SmoothStep(1.0, 6.0, D)));
            }
            return std::max(H, Spec.BaseZ - 2.5);
        };

        const double Step = 2.0;
        const int Nx = static_cast<int>(std::ceil((Ex1 - Ex0) / Step)), Ny = static_cast<int>(std::ceil((Ey1 - Ey0) / Step));
        std::vector<double> H(static_cast<std::size_t>((Nx + 1) * (Ny + 1)));
        std::vector<Near> Info(H.size());
        for (int J = 0; J <= Ny; ++J)
        {
            for (int I = 0; I <= Nx; ++I)
            {
                const std::size_t K = static_cast<std::size_t>(J * (Nx + 1) + I);
                H[K] = Height(Ex0 + I * Step, Ey0 + J * Step, &Info[K]);
            }
        }
        const auto At = [&](const int I, const int J) { return H[static_cast<std::size_t>(J * (Nx + 1) + I)]; };
        const auto NormalAt = [&](const int I, const int J)
        {
            const double L = At(std::max(I - 1, 0), J), R = At(std::min(I + 1, Nx), J), D = At(I, std::max(J - 1, 0)), U = At(I, std::min(J + 1, Ny));
            return Normalize(V3{(L - R) / (2.0 * Step), (D - U) / (2.0 * Step), 1.0});
        };

        const auto Surface = [&](const double X, const double Y)
        {
            const double Fx = Clamp((X - Ex0) / Step, 0.0, Nx - 1e-9), Fy = Clamp((Y - Ey0) / Step, 0.0, Ny - 1e-9);
            const int I = static_cast<int>(Fx), J = static_cast<int>(Fy);
            const double Tx = Fx - I, Ty = Fy - J;
            return Lerp(Lerp(At(I, J), At(I + 1, J), Tx), Lerp(At(I, J + 1), At(I + 1, J + 1), Tx), Ty);
        };
        const auto InKeep = [&Spec](const double X, const double Y, const double Margin)
        {
            for (const TerrainSpec::Block& K : Spec.Keep)
            {
                if (X > K.X0 - Margin && X < K.X1 + Margin && Y > K.Y0 - Margin && Y < K.Y1 + Margin) return true;
            }
            return false;
        };

        // ---- tarns (moor) and pools (mire): bowls carved into level ground away from the route, holding still water
        struct Pond
        {
            double X, Y, R, Level;
        };
        std::vector<Pond> Ponds;
        const double PondDepth = 0.7;
        if (Spec.Kind == Biome::Moor || Spec.Kind == Biome::Mire)
        {
            const bool Mire = Spec.Kind == Biome::Mire;
            Rng R(Mix(Seed ^ 0x51ed270bu));
            for (int Try = 0; Try < 80 && static_cast<int>(Ponds.size()) < (Mire ? 7 : 3); ++Try)
            {
                const double Radius = Mire ? R.Range(5.0, 10.0) : R.Range(7.0, 13.0);
                const double X = R.Range(Ex0, Ex1), Y = R.Range(Ey0, Ey1);
                const double Edge = std::min(std::min(X - Ex0, Ex1 - X), std::min(Y - Ey0, Ey1 - Y));
                if (Edge < Radius + 24.0 || Nearest(X, Y).D < Radius + 7.0 || InKeep(X, Y, Radius + 4.0)) continue;
                bool Clear = true;
                for (const Pond& P : Ponds)
                {
                    const double Gap = P.R + Radius + 6.0;
                    Clear = Clear && (P.X - X) * (P.X - X) + (P.Y - Y) * (P.Y - Y) > Gap * Gap;
                }
                if (!Clear) continue;
                double Lo = 1e30, Hi = -1e30;
                for (int K = 0; K < 16; ++K)
                {
                    const double A = 2.0 * Pi * K / 16.0;
                    const double Z = Surface(X + Cos(A) * Radius, Y + Sin(A) * Radius);
                    Lo = std::min(Lo, Z);
                    Hi = std::max(Hi, Z);
                }
                if (Hi - Lo > 2.5) continue;  // a hillside would need a cut, not a hollow
                Ponds.push_back({X, Y, Radius, Lo - 0.15});
            }
            for (int J = 0; J <= Ny; ++J)
            {
                for (int I = 0; I <= Nx; ++I)
                {
                    const double X = Ex0 + I * Step, Y = Ey0 + J * Step;
                    double& Z = H[static_cast<std::size_t>(J * (Nx + 1) + I)];
                    for (const Pond& P : Ponds)
                    {
                        const double D2 = ((X - P.X) * (X - P.X) + (Y - P.Y) * (Y - P.Y)) / (P.R * P.R);
                        if (D2 < 1.0) Z = std::min(Z, P.Level + 0.3 - (PondDepth + 0.3) * (1.0 - D2));
                        else if (D2 < 4.0) Z = std::min(Z, P.Level + 0.3 + (std::sqrt(D2) - 1.0) * P.R * 0.4);  // bank slope, no step into a rise
                    }
                }
            }
        }
        /** Distance to the nearest pond centre in pond radii (large when there is none). */
        const auto PondAt = [&Ponds](const double X, const double Y)
        {
            double Best = 1e30;
            for (const Pond& P : Ponds) Best = std::min(Best, std::sqrt((X - P.X) * (X - P.X) + (Y - P.Y) * (Y - P.Y)) / P.R);
            return Best;
        };


        // ---- layers per cell, surface mesh and collision
        Mesh Ground("SM_Art_Terrain_" + Spec.Name);
        Mesh Collide("SM_Art_Terrain_" + Spec.Name + "_Collision");
        const auto LayerAt = [&](const double X, const double Y, const double Z, const double Nz, const Near& N) -> M
        {
            const double P = Fractal(X, Y, 14.0, 3, Seed + 11u), Q = Fractal(X, Y, 5.0, 2, Seed + 13u);
            const bool Road = N.D < 0.9 + 0.5 * Q;
            switch (Spec.Kind)
            {
            case Biome::Moor:
                if (Road) return M::Dirt;
                if (Nz < 0.8) return P > 0.5 ? M::Lichen : M::Rock;
                if (P > 0.62) return M::DryGrass;
                if (P < 0.3) return M::Moss;
                return M::Heather;
            case Biome::Tropical:
                if (Road) return M::RedEarth;
                if (Nz < 0.78) return M::Rock;
                if (Z < Spec.BaseZ + 0.2 && N.D > 14.0) return M::Sand;
                return P > 0.66 ? M::RedEarth : M::Grass;
            case Biome::Mire:
                if (Road || Z < Spec.BaseZ - 0.35) return M::Mud;
                return P > 0.52 ? M::Moss : M::Grass;
            case Biome::Grove:
                if (Road) return M::Dirt;
                if (Nz < 0.78) return M::Rock;
                return P > 0.6 ? M::Moss : M::Grass;
            case Biome::Dream:
            default:
                return Nz < 0.8 ? M::StoneDark : M::Ash;
            }
        };
        // One unique colour texture over the whole ground (0.5 m texels), soft layer blends baked in by biome.
        const double Texel = 0.5;
        const int Tw = std::min(1024, static_cast<int>(std::ceil((Ex1 - Ex0) / Texel))), Th = std::min(1024, static_cast<int>(std::ceil((Ey1 - Ey0) / Texel)));
        const auto Bilinear = [&](const double X, const double Y)
        {
            const double Fx = Clamp((X - Ex0) / Step, 0.0, Nx - 1e-9), Fy = Clamp((Y - Ey0) / Step, 0.0, Ny - 1e-9);
            const int I = static_cast<int>(Fx), J = static_cast<int>(Fy);
            const double Tx = Fx - I, Ty = Fy - J;
            const double Z = Lerp(Lerp(At(I, J), At(I + 1, J), Tx), Lerp(At(I, J + 1), At(I + 1, J + 1), Tx), Ty);
            const V3 Nn = Normalize(NormalAt(I, J) * ((1 - Tx) * (1 - Ty)) + NormalAt(I + 1, J) * (Tx * (1 - Ty)) + NormalAt(I, J + 1) * ((1 - Tx) * Ty) +
                NormalAt(I + 1, J + 1) * (Tx * Ty));
            return std::make_pair(Z, Nn);
        };
        using C3 = std::array<double, 3>;
        const auto Blend = [](const C3& A, const C3& B, const double T) { return C3{Lerp(A[0], B[0], T), Lerp(A[1], B[1], T), Lerp(A[2], B[2], T)}; };
        const C3 Heather{0.40, 0.29, 0.22}, DryGrass{0.58, 0.49, 0.29}, Moss{0.23, 0.30, 0.13}, Dirt{0.42, 0.34, 0.25}, RockC{0.44, 0.43, 0.41},
            LichenC{0.50, 0.51, 0.43}, GrassC{0.26, 0.35, 0.13}, RedEarth{0.52, 0.31, 0.20}, SandC{0.74, 0.66, 0.50}, MudC{0.20, 0.17, 0.13},
            AshC{0.31, 0.30, 0.29}, Dark{0.24, 0.23, 0.22};
        Image Colour;
        Colour.Width = Tw;
        Colour.Height = Th;
        Colour.Channels = 3;
        Colour.Pixels.assign(static_cast<std::size_t>(Tw * Th * 3), 0);
        for (int Ty = 0; Ty < Th; ++Ty)
        {
            for (int Tx = 0; Tx < Tw; ++Tx)
            {
                const double X = Ex0 + (Tx + 0.5) * (Ex1 - Ex0) / Tw, Y = Ey1 - (Ty + 0.5) * (Ey1 - Ey0) / Th;  // image rows run from +Y down
                const auto [Z, Nn] = Bilinear(X, Y);
                const Near N = Nearest(X, Y);
                const double P = 0.65 * Fractal(X, Y, 30.0, 3, Seed + 11u) + 0.35 * Fractal(X, Y, 8.0, 2, Seed + 17u);
                const double Q = Fractal(X, Y, 5.0, 2, Seed + 13u);
                const double Road = 1.0 - SmoothStep(-0.6, 1.0 + 0.6 * Q, N.D);
                const double Steep = SmoothStep(0.88, 0.72, Nn.Z);
                C3 C{};
                switch (Spec.Kind)
                {
                case Biome::Moor:
                    C = Blend(Blend(Heather, DryGrass, SmoothStep(0.44, 0.6, P)), Moss, SmoothStep(0.33, 0.24, P));
                    C = Blend(C, Blend(RockC, LichenC, SmoothStep(0.4, 0.6, Q)), Steep);
                    C = Blend(C, Dirt, Road);
                    break;
                case Biome::Tropical:
                    C = Blend(Blend(GrassC, RedEarth, SmoothStep(0.58, 0.72, P)), SandC, SmoothStep(Spec.BaseZ + 0.4, Spec.BaseZ - 0.1, Z) * SmoothStep(10.0, 16.0, N.D));
                    C = Blend(C, RockC, Steep);
                    C = Blend(C, Blend(RedEarth, Dirt, 0.4), Road);
                    break;
                case Biome::Mire:
                    C = Blend(Blend(GrassC, Moss, SmoothStep(0.45, 0.6, P)), MudC, SmoothStep(Spec.BaseZ - 0.1, Spec.BaseZ - 0.5, Z));
                    C = Blend(C, MudC, Road);
                    break;
                case Biome::Grove:
                    C = Blend(Blend(GrassC, Moss, SmoothStep(0.52, 0.66, P)), Blend(GrassC, DryGrass, 0.35), SmoothStep(0.3, 0.2, P));
                    C = Blend(C, RockC, Steep);
                    C = Blend(C, Dirt, Road);
                    break;
                case Biome::Dream:
                default:
                    C = Blend(Blend(AshC, Dark, SmoothStep(0.4, 0.7, P)), Dark, Steep);
                    break;
                }
                if (!Ponds.empty())
                {
                    const double Bank = SmoothStep(1.12, 0.86, PondAt(X, Y));
                    C = Blend(C, Blend(MudC, Moss, 0.35 + 0.3 * Q), Bank * 0.8);
                }
                const double Grain = 0.9 + 0.2 * Value(X * 1.6, Y * 1.6, Seed + 23u) + 0.08 * (Lattice(Tx, Ty, Seed) - 0.5);
                std::uint8_t* Px = &Colour.Pixels[static_cast<std::size_t>((Ty * Tw + Tx) * 3)];
                for (int K = 0; K < 3; ++K) Px[K] = static_cast<std::uint8_t>(std::lround(Clamp(C[static_cast<std::size_t>(K)] * Grain, 0.0, 1.0) * 255.0));
            }
        }
        const std::string TextureName = "T_Terrain_" + Spec.Name + ".png";
        Output.push_back({"Assets/Art/Textures/" + TextureName, EncodePng(Colour)});
        Material GroundMat;
        GroundMat.Name = "M_Art_Terrain_" + Spec.Name;
        GroundMat.BaseColorTexture = TextureName;
        GroundMat.Roughness = 0.93;
        std::vector<Vertex> GV;
        std::vector<std::uint32_t> CI;
        for (int J = 0; J <= Ny; ++J)
        {
            for (int I = 0; I <= Nx; ++I)
            {
                const double X = Ex0 + I * Step, Y = Ey0 + J * Step;
                GV.push_back(Vertex{V3{X, Y, At(I, J)}, NormalAt(I, J), (X - Ex0) / (Ex1 - Ex0), (Ey1 - Y) / (Ey1 - Ey0)});
            }
        }
        for (int J = 0; J < Ny; ++J)
        {
            for (int I = 0; I < Nx; ++I)
            {
                const auto A = static_cast<std::uint32_t>(J * (Nx + 1) + I), B = A + 1u, C = A + static_cast<std::uint32_t>(Nx + 2), D = A + static_cast<std::uint32_t>(Nx + 1);
                CI.insert(CI.end(), {A, B, C, A, C, D});
            }
        }
        Ground.Indexed(GroundMat, GV, CI);
        Collide.Indexed(Mat(M::Rock), GV, CI);

        // ---- ground cover near the route: its own model per card material, swaying as a meadow
        Mesh Cover("SM_Art_Cover_" + Spec.Name);
        for (int J = 0; J < Ny; ++J)
        {
            for (int I = 0; I < Nx; ++I)
            {
                const std::uint32_t Cell = static_cast<std::uint32_t>(J * 7919 + I * 104729) ^ Seed;
                Rng R(Mix(Cell));
                const double X = Ex0 + (I + R.Unit()) * Step, Y = Ey0 + (J + R.Unit()) * Step;
                const Near N = Nearest(X, Y);
                if (N.D < 0.9 || InKeep(X, Y, 0.5)) continue;
                const double Wet = PondAt(X, Y);
                if (Wet < 0.8) continue;
                const bool Shore = Wet < 1.15;
                const double Density = Shore ? 0.85 : Rules.CoverDensity * (N.D < 28.0 ? 1.0 : 0.25);
                if (R.Unit() > Density) continue;
                const double Z = Surface(X, Y);
                if (Shore)
                {
                    const double Width = R.Range(0.7, 1.1), Height = R.Range(0.9, 1.5), Yaw = R.Range(0.0, Pi);
                    Tuft(Cover, Mat(M::Reeds), {X, Y, Z - 0.04}, Width, Height, Yaw, 3);
                    continue;
                }
                const V3 Nn = NormalAt(I, J);
                if (Nn.Z < 0.82) continue;
                const M Layer = LayerAt(X, Y, Z, Nn.Z, N);
                M Card = M::GrassBlades;
                double W = R.Range(0.6, 1.0), Hh = R.Range(0.3, 0.6);
                switch (Layer)
                {
                case M::Heather: Card = M::HeatherSprig; Hh = R.Range(0.28, 0.45); break;
                case M::DryGrass: Card = M::GrassDry; Hh = R.Range(0.35, 0.7); break;
                case M::Moss: Card = Spec.Kind == Biome::Mire ? M::Reeds : M::GrassBlades; break;
                case M::Mud: Card = M::Reeds; Hh = R.Range(0.8, 1.4); break;
                case M::Ash: case M::StoneDark: case M::Rock: case M::Lichen: case M::Sand: case M::Dirt: case M::RedEarth: continue;
                default: break;
                }
                Tuft(Cover, Mat(Card), {X, Y, Z - 0.04}, W, Hh, R.Range(0.0, Pi), 2);
            }
        }

        // ---- plants: shared models placed on the ground
        const double CellSize = Rules.CellSize;
        const int Cx = static_cast<int>((Ex1 - Ex0) / CellSize), Cy = static_cast<int>((Ey1 - Ey0) / CellSize);
        for (int J = 0; J < Cy; ++J)
        {
            for (int I = 0; I < Cx; ++I)
            {
                Rng R(Mix(static_cast<std::uint32_t>(J * 92821 + I * 68917) ^ (Seed * 31u)));
                const double X = Ex0 + (I + 0.15 + 0.7 * R.Unit()) * CellSize, Y = Ey0 + (J + 0.15 + 0.7 * R.Unit()) * CellSize;
                const double Roll = R.Unit();
                const PlantKind* Pick = nullptr;
                for (const PlantKind& P : Rules.Plants)
                {
                    if (Roll < P.Share)
                    {
                        Pick = &P;
                        break;
                    }
                }
                if (!Pick) continue;
                const Near N = Nearest(X, Y);
                if (N.D < (Pick->Tree ? 4.5 : 2.5) || InKeep(X, Y, Pick->Tree ? 3.0 : 1.5)) continue;
                if (Pick->Kind == Species::Stone && N.D < 10.0) continue;
                if (PondAt(X, Y) < (Pick->Tree ? 1.5 : 1.2)) continue;
                const double Z = Surface(X, Y);
                const double Fx = Clamp((X - Ex0) / Step, 0.0, Nx - 1.0), Fy = Clamp((Y - Ey0) / Step, 0.0, Ny - 1.0);
                if (Pick->Tree && NormalAt(static_cast<int>(Fx), static_cast<int>(Fy)).Z < 0.86) continue;
                const int Variant = static_cast<int>(R.Unit() * 3.0);
                const std::string Key = std::string(SpeciesName(Pick->Kind)) + "_" + std::to_string(Variant);
                auto Known = PlantModels.find(Key);
                if (Known == PlantModels.end()) Known = PlantModels.emplace(Key, EmitPlant(Key, Pick->Kind, BuildPlant(Pick->Kind, Variant, Spec.Kind))).first;
                Art.Plants.push_back({Known->second, X, Y, Z - 0.05, R.Range(0.0, 360.0), R.Range(0.85, 1.2)});
            }
        }

        // ---- still water: one disc per pond, its rim buried in the bank
        for (const Pond& P : Ponds)
        {
            const double Rw = P.R * std::sqrt(1.0 - 0.15 / (PondDepth + 0.3));
            std::vector<Vertex> WV{Vertex{V3{P.X, P.Y, P.Level}, V3{0, 0, 1}, 0.5, 0.5}};
            std::vector<std::uint32_t> WI;
            const int Segments = 28;
            for (int K = 0; K < Segments; ++K)
            {
                const double A = 2.0 * Pi * K / Segments;
                WV.push_back(Vertex{V3{P.X + Cos(A) * Rw, P.Y + Sin(A) * Rw, P.Level}, V3{0, 0, 1}, 0.5 + 0.5 * Cos(A), 0.5 - 0.5 * Sin(A)});
                WI.insert(WI.end(), {0u, static_cast<std::uint32_t>(1 + K), static_cast<std::uint32_t>(1 + (K + 1) % Segments)});
            }
            Ground.Indexed(Mat(M::PondWater), WV, WI);
        }

        for (const Mesh::Part& P : Cover.Parts())
        {
            const std::string Suffix = P.Mat.Name.rfind("M_Art_", 0) == 0 ? P.Mat.Name.substr(6) : P.Mat.Name;
            const std::string Model = Cover.Name() + "_" + Suffix;
            Mesh One(Model);
            One.Indexed(P.Mat, P.Vertices, P.Indices);
            PlantPart Part{Emit(Model, WriteGltf(One, "Dark Arisen art kit (terrain)")), "Assets/Materials/Foliage/" + Model + ".material"};
            const Wind Grass{0.09, 0.55, 0.25};
            Output.push_back({Part.Material, FoliageMaterialJson(P.Mat, Grass, 0.6, true)});
            Art.Cover.push_back(std::move(Part));
        }

        Art.Model = Emit(Ground.Name(), WriteGltf(Ground, "Dark Arisen art kit (terrain)"));
        Art.CollisionGltf = WriteCollisionGltf(Collide);
        return Art;
    }
}
