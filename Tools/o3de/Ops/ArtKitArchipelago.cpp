// The open world's geography (ContentSource/World/Archipelago.json) as a heightfield and a relief map:
// DarkArisenO3DE world-map. Islands are noisy ellipses rising to their peaks by shape (dome, ridge, volcano,
// low, coast); the sea floor falls away from the coasts. Only + - * / and sqrt reach the output, so every
// compiler writes the same bytes.

#include "ArtKitArchipelago.h"

#include "ArtKitCore.h"
#include "Json.h"

#include <algorithm>
#include <cmath>

namespace DarkArisen::Tools::Art
{
    namespace
    {
        double Hash(std::int64_t X, std::int64_t Y, std::uint32_t Seed)
        {
            std::uint32_t H = static_cast<std::uint32_t>(X) * 374761393u + static_cast<std::uint32_t>(Y) * 668265263u + Seed * 2246822519u;
            H = (H ^ (H >> 13)) * 1274126177u;
            H ^= H >> 16;
            return static_cast<double>(H & 0xFFFFFFu) / 16777215.0;
        }

        double Noise(double X, double Y, std::uint32_t Seed)
        {
            const double Fx = std::floor(X), Fy = std::floor(Y);
            const auto Ix = static_cast<std::int64_t>(Fx), Iy = static_cast<std::int64_t>(Fy);
            double Tx = X - Fx, Ty = Y - Fy;
            Tx = Tx * Tx * (3.0 - 2.0 * Tx);
            Ty = Ty * Ty * (3.0 - 2.0 * Ty);
            const double A = Hash(Ix, Iy, Seed), B = Hash(Ix + 1, Iy, Seed), C = Hash(Ix, Iy + 1, Seed), D = Hash(Ix + 1, Iy + 1, Seed);
            return A + (B - A) * Tx + (C - A) * Ty + (A - B - C + D) * Tx * Ty;
        }

        double Fractal(double X, double Y, double Scale, int Octaves, std::uint32_t Seed)
        {
            double Sum = 0.0, Amp = 0.5, Norm = 0.0;
            for (int K = 0; K < Octaves; ++K)
            {
                Sum += Amp * Noise(X / Scale, Y / Scale, Seed + static_cast<std::uint32_t>(K) * 101u);
                Norm += Amp;
                Scale *= 0.5;
                Amp *= 0.5;
            }
            return Sum / Norm;
        }

        double Smooth(double E0, double E1, double X)
        {
            double T = (X - E0) / (E1 - E0);
            T = T < 0.0 ? 0.0 : (T > 1.0 ? 1.0 : T);
            return T * T * (3.0 - 2.0 * T);
        }

        /** Degrees to (cos, sin) by series: the same bits on every compiler. */
        void CosSin(double Degrees, double& C, double& S)
        {
            double A = Degrees * 3.14159265358979323846 / 180.0;
            while (A > 3.14159265358979323846) A -= 6.28318530717958647692;
            while (A < -3.14159265358979323846) A += 6.28318530717958647692;
            double Term = 1.0, Cs = 1.0, Sn = A, TermS = A;
            for (int K = 1; K < 12; ++K)
            {
                Term *= -A * A / ((2.0 * K - 1.0) * (2.0 * K));
                Cs += Term;
                TermS *= -A * A / ((2.0 * K) * (2.0 * K + 1.0));
                Sn += TermS;
            }
            C = Cs;
            S = Sn;
        }
    }

    bool ReadArchipelago(const std::string& Json, ArchipelagoSpec& Out, std::string& Problem)
    {
        JsonValue Doc;
        if (!JsonReader::Parse(Json, Doc, Problem)) return false;
        const JsonValue* World = Doc.Find("world");
        const JsonValue* Islands = Doc.Find("islands");
        if (!World || !Islands || !Islands->IsArray()) return (Problem = "Archipelago.json: world and islands required", false);
        const auto Num = [](const JsonValue& V, const char* Key, double Fallback) { const JsonValue* F = V.Find(Key); return F && F->IsNumber() ? F->Number : Fallback; };
        const auto Str = [](const JsonValue& V, const char* Key) { const JsonValue* F = V.Find(Key); return F && F->IsString() ? F->Text : std::string(); };
        Out.Width = Num(*World, "width", 0.0);
        Out.Height = Num(*World, "height", 0.0);
        Out.SeaFloor = Num(*World, "seaFloor", -60.0);
        Out.Seed = static_cast<std::uint32_t>(Num(*World, "seed", 1.0));
        if (Out.Width < 1000.0 || Out.Height < 1000.0) return (Problem = "Archipelago.json: world under 1 km", false);
        for (const JsonValue& I : Islands->Items)
        {
            ArchipelagoIsland Island{Str(I, "region"), Str(I, "name"), Num(I, "x", -1), Num(I, "y", -1), Num(I, "rx", 0), Num(I, "ry", 0),
                Num(I, "rotation", 0), Num(I, "peak", 0), Str(I, "shape"), Str(I, "biome")};
            if (Island.Name.empty() || Island.Rx <= 0 || Island.Ry <= 0 || Island.Peak <= 0 ||
                (Island.Shape != "dome" && Island.Shape != "ridge" && Island.Shape != "volcano" && Island.Shape != "low" && Island.Shape != "coast"))
                return (Problem = "Archipelago.json: bad island " + Island.Name, false);
            Out.Islands.push_back(std::move(Island));
        }
        return true;
    }

    double ArchipelagoHeight(const ArchipelagoSpec& Spec, double X, double Y, int* Owner)
    {
        double Best = -1e30;
        int Who = -1;
        for (std::size_t K = 0; K < Spec.Islands.size(); ++K)
        {
            const ArchipelagoIsland& I = Spec.Islands[K];
            double C = 1.0, S = 0.0;
            CosSin(-I.Rotation, C, S);
            const double Dx = X - I.X, Dy = Y - I.Y;
            const double Lx = (Dx * C - Dy * S) / I.Rx, Ly = (Dx * S + Dy * C) / I.Ry;
            const auto Seed = Spec.Seed + static_cast<std::uint32_t>(K) * 7919u;
            // coastline wobble at several scales, bays and headlands
            const double Wobble = 1.0 + 0.46 * (Fractal(X, Y, 1500.0, 4, Seed) - 0.5) * 2.0 + 0.12 * (Fractal(X, Y, 320.0, 3, Seed + 1u) - 0.5) * 2.0;
            const double D = std::sqrt(Lx * Lx + Ly * Ly) / Wobble;
            double H;
            if (D < 1.0)
            {
                const double In = 1.0 - D;  // 0 at the coast, 1 at the centre
                const double Detail = Fractal(X, Y, 900.0, 5, Seed + 3u);
                if (I.Shape == "volcano")
                {
                    const double Cone = In * In * (3.0 - 2.0 * In);
                    const double Crater = Smooth(0.86, 0.97, In) * 0.22;
                    H = I.Peak * (Cone - Crater) * (0.85 + 0.3 * Detail);
                }
                else if (I.Shape == "ridge")
                {
                    const double Ridge = 1.0 - std::fabs(Fractal(X, Y, 1600.0, 3, Seed + 5u) - 0.5) * 2.0;
                    H = I.Peak * Smooth(0.0, 0.55, In) * (0.35 + 0.65 * Ridge * Ridge) * (0.8 + 0.4 * Detail);
                }
                else if (I.Shape == "low")
                    H = I.Peak * Smooth(0.0, 0.3, In) * (0.4 + 0.6 * Detail);
                else if (I.Shape == "coast")
                    H = I.Peak * Smooth(0.0, 0.45, In) * (0.25 + 0.75 * Fractal(X, Y, 700.0, 5, Seed + 9u));
                else
                    H = I.Peak * Smooth(0.0, 0.8, In) * (0.55 + 0.45 * Detail);
                H += 1.5 * Smooth(0.0, 0.04, In);  // beach shelf above the tide
            }
            else
                H = -2.0 - (D - 1.0) * 55.0;  // the shelf falls off into the deep
            if (H > Best)
            {
                Best = H;
                Who = static_cast<int>(K);
            }
        }
        if (Owner) *Owner = Best > 0.0 ? Who : -1;
        return std::max(Best, Spec.SeaFloor);
    }

    ArchipelagoMaps BuildArchipelagoMaps(const ArchipelagoSpec& Spec, double HeightCell, double ReliefCell)
    {
        ArchipelagoMaps Out;
        // heightfield: 16-bit, metres = MinHeight + value / 65535 * (MaxHeight - MinHeight)
        Out.MinHeight = Spec.SeaFloor;
        Out.MaxHeight = 1000.0;
        Out.HeightWidth = static_cast<int>(Spec.Width / HeightCell) + 1;
        Out.HeightHeight = static_cast<int>(Spec.Height / HeightCell) + 1;
        std::vector<std::uint16_t> Px(static_cast<std::size_t>(Out.HeightWidth * Out.HeightHeight));
        std::size_t Land = 0;
        for (int J = 0; J < Out.HeightHeight; ++J)
            for (int I = 0; I < Out.HeightWidth; ++I)
            {
                const double Z = ArchipelagoHeight(Spec, I * HeightCell, Spec.Height - J * HeightCell, nullptr);  // rows from the north
                if (Z > 0.0) ++Land;
                const double T = std::clamp((Z - Out.MinHeight) / (Out.MaxHeight - Out.MinHeight), 0.0, 1.0);
                Px[static_cast<std::size_t>(J * Out.HeightWidth + I)] = static_cast<std::uint16_t>(std::lround(T * 65535.0));
            }
        Out.LandSquareKm = static_cast<double>(Land) * HeightCell * HeightCell / 1e6;
        Out.HeightPng = EncodePng16Gray(Out.HeightWidth, Out.HeightHeight, Px);

        // relief map: biome colour, height tint, hillshade from the north-west, sea by depth
        const int W = static_cast<int>(Spec.Width / ReliefCell), H = static_cast<int>(Spec.Height / ReliefCell);
        Image Relief;
        Relief.Width = W;
        Relief.Height = H;
        Relief.Channels = 3;
        Relief.Pixels.assign(static_cast<std::size_t>(W * H * 3), 0);
        const auto Biome = [](const std::string& B, double T, double Out3[3])
        {
            struct Pair { const char* Name; double Low[3], High[3]; };
            static const Pair Table[] = {{"tropical", {0.36, 0.52, 0.22}, {0.48, 0.44, 0.30}}, {"mire", {0.30, 0.38, 0.22}, {0.36, 0.40, 0.26}},
                {"moor", {0.46, 0.42, 0.28}, {0.52, 0.42, 0.42}}, {"volcanic", {0.30, 0.28, 0.26}, {0.20, 0.18, 0.17}},
                {"cold", {0.38, 0.44, 0.38}, {0.90, 0.92, 0.94}}, {"fortress", {0.55, 0.50, 0.40}, {0.60, 0.55, 0.45}}, {"maw", {0.22, 0.20, 0.24}, {0.14, 0.12, 0.16}}};
            for (const Pair& P : Table)
                if (B == P.Name)
                {
                    for (int K = 0; K < 3; ++K) Out3[K] = P.Low[K] + (P.High[K] - P.Low[K]) * T;
                    return;
                }
            Out3[0] = Out3[1] = Out3[2] = 0.5;
        };
        for (int J = 0; J < H; ++J)
            for (int I = 0; I < W; ++I)
            {
                const double X = (I + 0.5) * ReliefCell, Y = Spec.Height - (J + 0.5) * ReliefCell;
                int Owner = -1;
                const double Z = ArchipelagoHeight(Spec, X, Y, &Owner);
                double C[3];
                if (Z > 0.0 && Owner >= 0)
                {
                    const ArchipelagoIsland& Isl = Spec.Islands[static_cast<std::size_t>(Owner)];
                    Biome(Isl.Biome, std::clamp(Z / std::max(Isl.Peak, 1.0), 0.0, 1.0), C);
                    if (Z < 2.0)
                        for (double& V : C) V = V * 0.4 + 0.84 * 0.6;  // sand
                    const double Zx = ArchipelagoHeight(Spec, X + ReliefCell, Y, nullptr) - Z, Zy = ArchipelagoHeight(Spec, X, Y + ReliefCell, nullptr) - Z;
                    const double Nx = -Zx / ReliefCell, Ny = -Zy / ReliefCell, Len = std::sqrt(Nx * Nx + Ny * Ny + 1.0);
                    const double Shade = std::clamp(((-Nx * 0.6 + Ny * 0.6) + 0.55) / Len + 0.25, 0.35, 1.25);
                    for (double& V : C) V *= Shade;
                }
                else
                {
                    const double Depth = std::clamp(-Z / 60.0, 0.0, 1.0);
                    const double Shallow[3] = {0.40, 0.70, 0.72}, Deep[3] = {0.06, 0.18, 0.30};
                    for (int K = 0; K < 3; ++K) C[K] = Shallow[K] + (Deep[K] - Shallow[K]) * std::sqrt(Depth);
                }
                std::uint8_t* P = &Relief.Pixels[static_cast<std::size_t>((J * W + I) * 3)];
                for (int K = 0; K < 3; ++K) P[K] = static_cast<std::uint8_t>(std::lround(std::clamp(C[K], 0.0, 1.0) * 255.0));
            }
        Out.ReliefPng = EncodePng(Relief);
        return Out;
    }

    namespace
    {
        struct BiomeLayers
        {
            const char* Biome;
            const char* Layer[4];
            double Tile[4];
        };
        const BiomeLayers Layers[] = {
            {"tropical", {"sparse_grass", "sand_01", "rock_face_03", "forrest_ground_01"}, {2.0, 1.5, 2.7, 2.0}},
            {"mire", {"sparse_grass", "brown_mud_02", "rock_pitted_mossy", "forrest_ground_01"}, {2.0, 1.3, 1.0, 2.0}},
            {"moor", {"sparse_grass", "forrest_ground_01", "rock_pitted_mossy", "brown_mud_02"}, {2.0, 2.0, 1.0, 1.3}},
            {"volcanic", {"brown_mud_02", "sand_01", "rock_face_03", "forrest_ground_01"}, {1.3, 1.5, 2.7, 2.0}},
            {"cold", {"sparse_grass", "sand_01", "rock_pitted_mossy", "rock_face_03"}, {2.0, 1.5, 1.0, 2.7}},
            {"fortress", {"sparse_grass", "sand_01", "rock_face_03", "cobblestone_floor_08"}, {2.0, 1.5, 2.7, 2.0}},
            {"maw", {"brown_mud_02", "forrest_ground_01", "rock_pitted_mossy", "rock_face_03"}, {1.3, 2.0, 1.0, 2.7}}};

    }

    WorldTerrain BuildWorldTerrain(const ArchipelagoSpec& Spec, const double TileSize, const double TileStep, const double CoarseStep, const double TextureCell)
    {
        WorldTerrain Out;
        const auto Height = [&Spec](double X, double Y) { return ArchipelagoHeight(Spec, X, Y, nullptr); };
        const auto Normal = [&](double X, double Y, double Step)
        {
            const double Dx = (Height(X + Step, Y) - Height(X - Step, Y)) / (2.0 * Step), Dy = (Height(X, Y + Step) - Height(X, Y - Step)) / (2.0 * Step);
            return Normalize(V3{-Dx, -Dy, 1.0});
        };
        // a grid mesh over [X0, X1] x [Y0, Y1]; UVs address the world textures (V = 0 at the north edge)
        const auto Grid = [&](const std::string& Name, double X0, double Y0, double X1, double Y1, double Step, double Lower, const Material& Mat)
        {
            const int Nx = static_cast<int>((X1 - X0) / Step + 0.5), Ny = static_cast<int>((Y1 - Y0) / Step + 0.5);
            std::vector<Vertex> V;
            std::vector<std::uint32_t> I;
            V.reserve(static_cast<std::size_t>((Nx + 1) * (Ny + 1)));
            for (int J = 0; J <= Ny; ++J)
                for (int K = 0; K <= Nx; ++K)
                {
                    const double X = X0 + K * Step, Y = Y0 + J * Step;
                    V.push_back(Vertex{V3{X, Y, Height(X, Y) - Lower}, Normal(X, Y, Step), X / Spec.Width, (Spec.Height - Y) / Spec.Height});
                }
            for (int J = 0; J < Ny; ++J)
                for (int K = 0; K < Nx; ++K)
                {
                    const auto A = static_cast<std::uint32_t>(J * (Nx + 1) + K), B = A + 1u, C = A + static_cast<std::uint32_t>(Nx + 2), D = A + static_cast<std::uint32_t>(Nx + 1);
                    // skip cells entirely in the deep, where only the ocean is seen
                    if (V[A].P.Z < -20.0 && V[B].P.Z < -20.0 && V[C].P.Z < -20.0 && V[D].P.Z < -20.0) continue;
                    I.insert(I.end(), {A, B, C, A, C, D});
                }
            Mesh M(Name);
            M.Indexed(Mat, V, I);
            Out.Triangles += I.size() / 3;
            return M;
        };

        // world textures: macro colour (biome by height) and splat (ground, path/sand/mud, rock, accent)
        const int Tw = static_cast<int>(Spec.Width / TextureCell), Th = static_cast<int>(Spec.Height / TextureCell);
        Image Macro, Splat;
        Macro.Width = Splat.Width = Tw;
        Macro.Height = Splat.Height = Th;
        Macro.Channels = 3;
        Splat.Channels = 4;
        Macro.Pixels.assign(static_cast<std::size_t>(Tw * Th * 3), 0);
        Splat.Pixels.assign(static_cast<std::size_t>(Tw * Th * 4), 0);
        for (int J = 0; J < Th; ++J)
            for (int K = 0; K < Tw; ++K)
            {
                const double X = (K + 0.5) * TextureCell, Y = Spec.Height - (J + 0.5) * TextureCell;
                int Owner = -1;
                const double Z = ArchipelagoHeight(Spec, X, Y, &Owner);
                const std::string Biome = Owner >= 0 ? Spec.Islands[static_cast<std::size_t>(Owner)].Biome : std::string("tropical");
                const double Peak = Owner >= 0 ? Spec.Islands[static_cast<std::size_t>(Owner)].Peak : 100.0;
                const V3 N = Normal(X, Y, TextureCell);
                const double Steep = Smooth(0.80, 0.62, N.Z);
                const double Shore = Smooth(3.0, 0.5, Z);
                const double Accent = Smooth(0.45, 0.62, Fractal(X, Y, 260.0, 3, Spec.Seed + 77u));
                double W1 = Biome == "mire" ? Smooth(4.0, 1.0, Z) : Shore, W2 = Steep, W3 = Accent;
                if (Biome == "cold") W3 = Smooth(0.45, 0.8, Z / std::max(Peak, 1.0));  // bare rock towards the tops
                W1 = std::clamp(W1, 0.0, 1.0) * (1.0 - W2);
                W3 = std::clamp(W3, 0.0, 1.0) * (1.0 - W2) * (1.0 - W1);
                const double W[4] = {std::clamp(1.0 - W1 - W2 - W3, 0.0, 1.0), W1, W2, W3};
                std::uint8_t* S = &Splat.Pixels[static_cast<std::size_t>((J * Tw + K) * 4)];
                for (int C = 0; C < 4; ++C) S[C] = static_cast<std::uint8_t>(std::lround(W[C] * 255.0));
                double Col[3] = {0.45, 0.5, 0.35};
                const double T = std::clamp(Z / std::max(Peak, 1.0), 0.0, 1.0);
                struct Pal { const char* B; double L[3], H[3]; };
                static const Pal Palette[] = {{"tropical", {0.30, 0.42, 0.16}, {0.40, 0.38, 0.24}}, {"mire", {0.24, 0.30, 0.16}, {0.30, 0.33, 0.20}},
                    {"moor", {0.42, 0.34, 0.24}, {0.46, 0.36, 0.34}}, {"volcanic", {0.27, 0.25, 0.23}, {0.18, 0.16, 0.15}}, {"cold", {0.34, 0.40, 0.32}, {0.62, 0.64, 0.66}},
                    {"fortress", {0.40, 0.42, 0.26}, {0.48, 0.45, 0.32}}, {"maw", {0.22, 0.20, 0.22}, {0.15, 0.13, 0.15}}};
                for (const Pal& P : Palette)
                    if (Biome == P.B)
                        for (int C = 0; C < 3; ++C) Col[C] = P.L[C] + (P.H[C] - P.L[C]) * T;
                std::uint8_t* M = &Macro.Pixels[static_cast<std::size_t>((J * Tw + K) * 3)];
                for (int C = 0; C < 3; ++C) M[C] = static_cast<std::uint8_t>(std::lround(std::clamp(Col[C], 0.0, 1.0) * 255.0));
            }
        Out.Files.push_back({"T_World_Macro.png", EncodePng(Macro)});
        Out.Files.push_back({"T_World_Splat.png", EncodePng(Splat)});

        // one material per biome; tiles and the coarse mesh take their biome's (the coarse mesh the tropical one)
        for (const BiomeLayers& L : Layers)
        {
            std::string J = "{\n    \"materialType\": \"../../Materials/Types/DarkArisenTerrain.materialtype\",\n    \"materialTypeVersion\": 1,\n    \"propertyValues\": {\n";
            for (int K = 0; K < 4; ++K)
            {
                const std::string Base = std::string("../../Art/Scanned/") + L.Layer[K] + "/" + L.Layer[K];
                const std::string G = "        \"layer" + std::to_string(K);
                J += G + ".arm\": \"" + Base + "_arm_2k.jpg\",\n" + G + ".color\": \"" + Base + "_diff_2k.jpg\",\n" + G + ".normal\": \"" + Base + "_nor_gl_2k.jpg\",\n";
            }
            J += "        \"macro.blendSharpness\": 3,\n        \"macro.color\": \"../T_World_Macro.png\",\n        \"macro.splat\": \"../T_World_Splat.png\",\n";
            J += "        \"macro.strength\": 0.8,\n        \"macro.tiles\": [" + GltfBits::Num(L.Tile[0]) + ", " + GltfBits::Num(L.Tile[1]) + ", " + GltfBits::Num(L.Tile[2]) + ", " +
                 GltfBits::Num(L.Tile[3]) + "]\n    }\n}\n";
            Out.Files.push_back({std::string("Materials/M_World_") + L.Biome + ".material", J});
        }

        Material Placeholder;
        Placeholder.Name = "M_World_Terrain";
        Placeholder.BaseColor = {0.4, 0.42, 0.3, 1.0};
        Placeholder.Roughness = 0.95;
        Out.Files.push_back({"SM_World_Coarse.gltf", WriteGltf(Grid("SM_World_Coarse", 0.0, 0.0, Spec.Width, Spec.Height, CoarseStep, 0.6, Placeholder), "Dark Arisen world-build")});

        const int Columns = static_cast<int>(Spec.Width / TileSize + 0.5), Rows = static_cast<int>(Spec.Height / TileSize + 0.5);
        std::string Index = "{\n  \"tileSize\": " + GltfBits::Num(TileSize) + ",\n  \"columns\": " + std::to_string(Columns) + ",\n  \"rows\": " + std::to_string(Rows) + ",\n  \"tiles\": [";
        bool First = true;
        for (int Row = 0; Row < Rows; ++Row)
            for (int Column = 0; Column < Columns; ++Column)
            {
                const double X0 = Column * TileSize, Y0 = Row * TileSize;
                double Top = -1e30;
                int Owner = -1, Votes = 0;
                for (double Y = Y0; Y <= Y0 + TileSize; Y += TileSize / 8.0)
                    for (double X = X0; X <= X0 + TileSize; X += TileSize / 8.0)
                    {
                        int O = -1;
                        Top = std::max(Top, ArchipelagoHeight(Spec, X, Y, &O));
                        if (O >= 0 && Votes == 0) { Owner = O; Votes = 1; }
                    }
                if (Top < -2.0) continue;  // open sea: the ocean alone
                const std::string Biome = Owner >= 0 ? Spec.Islands[static_cast<std::size_t>(Owner)].Biome : std::string("tropical");
                const std::string Name = "SM_World_Tile_" + std::to_string(Column) + "_" + std::to_string(Row);
                Out.Files.push_back({"Tiles/" + Name + ".gltf", WriteGltf(Grid(Name, X0, Y0, X0 + TileSize, Y0 + TileSize, TileStep, 0.0, Placeholder), "Dark Arisen world-build")});
                Index += std::string(First ? "" : ",") + "\n    {\"tile\": " + std::to_string(Row * Columns + Column) + ", \"column\": " + std::to_string(Column) + ", \"row\": " +
                         std::to_string(Row) + ", \"model\": \"Tiles/" + Name + ".gltf\", \"material\": \"Materials/M_World_" + Biome + ".material\"}";
                First = false;
                ++Out.Tiles;
            }
        Out.Files.push_back({"WorldTiles.json", Index + "\n  ]\n}\n"});
        return Out;
    }
}
