// The open world's geography (ContentSource/World/Archipelago.json) as a heightfield and a relief map:
// DarkArisenO3DE world-map. Islands are noisy ellipses rising to their peaks by shape (dome, ridge, volcano,
// low, coast); the sea floor falls away from the coasts. Only + - * / and sqrt reach the output, so every
// compiler writes the same bytes.

#include "ArtKitArchipelago.h"

#include "ArtKit.h"
#include "ArtKitCore.h"
#include "ArtKitLibrary.h"
#include "Json.h"
#include "O3deIds.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <set>

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
            {"tropical", {"leafy_grass", "sand_01", "rock_face_03", "forrest_ground_01"}, {2.0, 1.5, 2.7, 2.0}},
            {"mire", {"leafy_grass", "brown_mud_02", "rock_pitted_mossy", "forrest_ground_01"}, {2.0, 1.3, 1.0, 2.0}},
            {"moor", {"withered_grass", "forrest_ground_01", "rock_pitted_mossy", "brown_mud_02"}, {2.0, 2.0, 1.0, 1.3}},
            {"volcanic", {"brown_mud_02", "sand_01", "rock_face_03", "forrest_ground_01"}, {1.3, 1.5, 2.7, 2.0}},
            {"cold", {"withered_grass", "sand_01", "rock_pitted_mossy", "rock_face_03"}, {2.0, 1.5, 1.0, 2.7}},
            {"fortress", {"leafy_grass", "sand_01", "rock_face_03", "cobblestone_floor_08"}, {2.0, 1.5, 2.7, 2.0}},
            {"maw", {"brown_mud_02", "forrest_ground_01", "rock_pitted_mossy", "rock_face_03"}, {1.3, 2.0, 1.0, 2.7}}};

    }

    namespace
    {
        /** The nearest point of a road's centre line to (X, Y): its distance, the road's height there and the direction. */
        struct RoadPoint
        {
            double Distance = 1e30, Z = 0, Dx = 1, Dy = 0;
        };
        RoadPoint NearestOnRoad(const WorldRoad& Road, const double X, const double Y)
        {
            RoadPoint Best;
            for (std::size_t K = 0; K + 1 < Road.Points.size(); ++K)
            {
                const WorldRoad::Point& A = Road.Points[K];
                const WorldRoad::Point& B = Road.Points[K + 1];
                const double Ex = B.X - A.X, Ey = B.Y - A.Y, Len2 = Ex * Ex + Ey * Ey;
                const double T = Len2 > 0.0 ? Clamp(((X - A.X) * Ex + (Y - A.Y) * Ey) / Len2, 0.0, 1.0) : 0.0;
                const double Px = A.X + Ex * T, Py = A.Y + Ey * T;
                const double D = std::sqrt((X - Px) * (X - Px) + (Y - Py) * (Y - Py));
                if (D < Best.Distance)
                {
                    const double Len = std::sqrt(Len2);
                    Best = {D, A.Z + (B.Z - A.Z) * T, Len > 0.0 ? Ex / Len : 1.0, Len > 0.0 ? Ey / Len : 0.0};
                }
            }
            return Best;
        }
    }

    WorldTerrain BuildWorldTerrain(const ArchipelagoSpec& Spec, const std::vector<WorldPlaceMark>& Places, const std::vector<WorldLocation>& Locations,
        const std::vector<WorldRoad>& Roads, const double TileSize, const double TileStep, const double CoarseStep, const double TextureCell)
    {
        WorldTerrain Out;
        // each road's box (with its shoulders), so the ground only asks the roads it is near
        struct RoadBox { double X0, Y0, X1, Y1; };
        std::vector<RoadBox> RoadBoxes;
        for (const WorldRoad& Road : Roads)
        {
            RoadBox Box{1e30, 1e30, -1e30, -1e30};
            for (const WorldRoad::Point& P : Road.Points) Box = {std::min(Box.X0, P.X), std::min(Box.Y0, P.Y), std::max(Box.X1, P.X), std::max(Box.Y1, P.Y)};
            const double Reach = Road.Width / 2 + 12.0;
            RoadBoxes.push_back({Box.X0 - Reach, Box.Y0 - Reach, Box.X1 + Reach, Box.Y1 + Reach});
        }
        // settlements, camps, forts and lighthouses stand on levelled ground: a plateau at the site's own height,
        // easing into the hills over half its radius again (overlapping districts step down a slope as terraces)
        struct Site
        {
            const WorldLocation* Where;
            double Z;
            Tradition Kind;
        };
        std::vector<Site> Sites;
        for (const WorldLocation& L : Locations)
        {
            Tradition Kind;
            if (L.Kind == "district") Kind = Tradition::Colonial;
            else if (L.Kind == "settlement") Kind = L.Region == "QuietCoast" ? Tradition::Nordic : Tradition::Stilt;
            else if (L.Kind == "zone") Kind = Tradition::Stilt;
            else if (L.Kind == "camp") Kind = Tradition::Camp;
            else if (L.Kind == "fort") Kind = Tradition::Fort;
            else if (L.Kind == "lighthouse") Kind = Tradition::Lighthouse;
            else if (L.Kind == "dungeon") Kind = Tradition::Entrance;
            else continue;
            // a level design may set the terrace height (layout.md R-01); otherwise the natural ground, above the tide
            Sites.push_back({&L, L.Plan.HasGround ? L.Plan.GroundZ : std::max(ArchipelagoHeight(Spec, L.X, L.Y, nullptr), 1.5), Kind});
        }
        const auto Height = [&Spec, &Sites, &Roads, &RoadBoxes](double X, double Y)
        {
            double H = ArchipelagoHeight(Spec, X, Y, nullptr);
            for (const Site& S : Sites)
            {
                // the Mire's houses stand in the bog on posts; a dungeon's way in keeps its hillside
                if ((S.Kind == Tradition::Stilt && S.Where->Kind == "zone") || S.Kind == Tradition::Entrance) continue;
                const double Dx = X - S.Where->X, Dy = Y - S.Where->Y, R = S.Where->Radius;
                const double D2 = Dx * Dx + Dy * Dy;
                if (D2 >= 2.25 * R * R) continue;
                const double T = Smooth(1.5 * R, 0.95 * R, std::sqrt(D2));
                H = H + (S.Z - H) * T;
            }
            // the roads are graded into the ground: level across, at their own height along, with 10 m shoulders
            // (layout.md R-04); stairs sit on a ramp of the same grade
            for (std::size_t K = 0; K < Roads.size(); ++K)
            {
                const RoadBox& Box = RoadBoxes[K];
                if (X < Box.X0 || X > Box.X1 || Y < Box.Y0 || Y > Box.Y1) continue;
                const RoadPoint P = NearestOnRoad(Roads[K], X, Y);
                const double Half = Roads[K].Width / 2 + 1.0;
                if (P.Distance >= Half + 10.0) continue;
                H = H + (P.Z - H) * Smooth(Half + 10.0, Half, P.Distance);
            }
            return H;
        };
        /** How much of a road is under (X, Y): 1 on its surface, falling to 0 at its kerb plus Margin. */
        const auto OnRoad = [&Roads, &RoadBoxes](const double X, const double Y, const double Margin)
        {
            double Most = 0.0;
            for (std::size_t K = 0; K < Roads.size(); ++K)
            {
                const RoadBox& Box = RoadBoxes[K];
                if (X < Box.X0 || X > Box.X1 || Y < Box.Y0 || Y > Box.Y1) continue;
                const double Half = Roads[K].Width / 2;
                Most = std::max(Most, Smooth(Half + Margin, Half, NearestOnRoad(Roads[K], X, Y).Distance));
            }
            return Most;
        };
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
            M.SetUvSteps(1048576.0);  // world UVs: 1.3 cm over 14 km
            M.Indexed(Mat, V, I);
            Out.Triangles += I.size() / 3;
            return M;
        };

        // tiles a colonial district or a fort touches are towns: their accent layer is paving, laid where the town is
        const int TownColumns = static_cast<int>(Spec.Width / TileSize + 0.5);
        std::set<int> TownTiles;
        for (const Site& Si : Sites)
        {
            if (Si.Kind != Tradition::Colonial && Si.Kind != Tradition::Fort) continue;
            const double R = Si.Where->Radius;
            for (int Row = static_cast<int>((Si.Where->Y - R) / TileSize); Row <= static_cast<int>((Si.Where->Y + R) / TileSize); ++Row)
                for (int Column = static_cast<int>((Si.Where->X - R) / TileSize); Column <= static_cast<int>((Si.Where->X + R) / TileSize); ++Column)
                    TownTiles.insert(Row * TownColumns + Column);
        }

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
                ArchipelagoHeight(Spec, X, Y, &Owner);
                const double Z = Height(X, Y);
                const std::string Biome = Owner >= 0 ? Spec.Islands[static_cast<std::size_t>(Owner)].Biome : std::string("tropical");
                const double Peak = Owner >= 0 ? Spec.Islands[static_cast<std::size_t>(Owner)].Peak : 100.0;
                const V3 N = Normal(X, Y, TextureCell);
                // trodden ground in the settlements: the path layer (sand, mud or litter by biome) under the streets
                double Trodden = 0.0, Paved = 0.0;
                const bool InTown = TownTiles.count(static_cast<int>(Y / TileSize) * TownColumns + static_cast<int>(X / TileSize)) != 0;
                for (const Site& Si : Sites)
                {
                    if (Si.Kind == Tradition::Entrance) continue;
                    if (InTown && (Si.Kind == Tradition::Colonial || Si.Kind == Tradition::Fort))
                    {
                        const double Dx = X - Si.Where->X, Dy = Y - Si.Where->Y;
                        Paved = std::max(Paved, 0.9 * Smooth(Si.Where->Radius * 0.95, 0.75 * Si.Where->Radius, std::sqrt(Dx * Dx + Dy * Dy)));
                        continue;
                    }
                    const double Dx = X - Si.Where->X, Dy = Y - Si.Where->Y;
                    // towns are mostly street; villages and camps keep their grass between the paths
                    const double Worn = Si.Kind == Tradition::Colonial || Si.Kind == Tradition::Fort ? 0.7 : 0.3;
                    Trodden = std::max(Trodden, Worn * Smooth(Si.Where->Radius, 0.7 * Si.Where->Radius, std::sqrt(Dx * Dx + Dy * Dy)));
                }
                if (!Roads.empty())
                {
                    const double Road = OnRoad(X, Y, 1.5);
                    if (InTown) Paved = std::max(Paved, 0.9 * Road);
                    else Trodden = std::max(Trodden, 0.8 * Road);
                }
                const double Steep = Smooth(0.80, 0.62, N.Z);
                const double Shore = Smooth(3.0, 0.5, Z);
                const double Accent = Smooth(0.45, 0.62, Fractal(X, Y, 260.0, 3, Spec.Seed + 77u));
                double W1 = std::max(Biome == "mire" ? Smooth(4.0, 1.0, Z) : Shore, Trodden), W2 = Steep, W3 = Accent;
                if (Biome == "cold") W3 = Smooth(0.45, 0.8, Z / std::max(Peak, 1.0));  // bare rock towards the tops
                if (InTown) W3 = Paved;  // a town tile's accent is its paving: none outside the town
                W1 = std::clamp(W1, 0.0, 1.0) * (1.0 - W2);
                W3 = std::clamp(W3, 0.0, 1.0) * (1.0 - W2) * (InTown ? 1.0 : 1.0 - W1);
                if (InTown) W1 = W1 * (1.0 - W3);  // paving over the shore's sand
                const double W[4] = {std::clamp(1.0 - W1 - W2 - W3, 0.0, 1.0), W1, W2, W3};
                std::uint8_t* S = &Splat.Pixels[static_cast<std::size_t>((J * Tw + K) * 4)];
                for (int C = 0; C < 4; ++C) S[C] = static_cast<std::uint8_t>(std::lround(W[C] * 255.0));
                // macro colour: the biome's ground from shore to summit, woods darker and greener (from afar the trees are
                // this colour), sand along the shore, bare rock on steep ground, and a broad mottle so no slope is flat
                double Col[3] = {0.30, 0.34, 0.20};
                const double T = std::clamp(Z / std::max(Peak, 1.0), 0.0, 1.0);
                struct Pal { const char* B; double L[3], H[3], Wood[3]; };
                static const Pal Palette[] = {
                    {"tropical", {0.15, 0.29, 0.06}, {0.27, 0.30, 0.13}, {0.05, 0.13, 0.03}},
                    {"mire", {0.17, 0.21, 0.10}, {0.24, 0.26, 0.14}, {0.08, 0.12, 0.05}},
                    {"moor", {0.36, 0.27, 0.19}, {0.40, 0.31, 0.29}, {0.10, 0.15, 0.08}},
                    {"volcanic", {0.20, 0.18, 0.16}, {0.12, 0.11, 0.10}, {0.12, 0.13, 0.08}},
                    {"cold", {0.26, 0.31, 0.22}, {0.60, 0.62, 0.64}, {0.08, 0.13, 0.09}},
                    {"fortress", {0.22, 0.30, 0.11}, {0.34, 0.34, 0.21}, {0.07, 0.13, 0.05}},
                    {"maw", {0.16, 0.14, 0.15}, {0.10, 0.09, 0.10}, {0.10, 0.10, 0.09}}};
                const double Woods = Kit::WorldWoods(X, Y, Spec.Seed) * Smooth(2.5, 6.0, Z) * (1.0 - Steep);
                // a broad mottle everywhere, and on slopes the streaks of runoff and scree (a ridged noise), so a mountain
                // face reads from afar without a single tree on it
                const double Ridged = 1.0 - std::fabs(Fractal(X, Y, 55.0, 3, Spec.Seed + 123u) - 0.5) * 2.0;
                const double Slope = Smooth(0.97, 0.80, N.Z);
                const double Mottle = (0.9 + 0.2 * Fractal(X, Y, 140.0, 3, Spec.Seed + 91u)) * (1.0 - 0.22 * Slope * (Ridged * Ridged - 0.35));
                for (const Pal& P : Palette)
                    if (Biome == P.B)
                        for (int C = 0; C < 3; ++C) Col[C] = (P.L[C] + (P.H[C] - P.L[C]) * T) * Mottle * (1.0 - Woods) + P.Wood[C] * Woods;
                static const double Sand[3] = {0.55, 0.49, 0.36}, Rock[3] = {0.33, 0.32, 0.30};
                const double SandW = Biome == "mire" || Biome == "volcanic" || Biome == "maw" ? 0.0 : Shore;
                for (int C = 0; C < 3; ++C) Col[C] = Col[C] + (Sand[C] - Col[C]) * SandW * (1.0 - Steep);
                for (int C = 0; C < 3; ++C) Col[C] = Col[C] + (Rock[C] - Col[C]) * Steep;
                // the streets keep their own colour: weathered grey cobbles in the towns, bare earth on the village paths
                // (the macro re-tints every layer, so without this the paving takes the grass's green)
                static const double Cobble[3] = {0.35, 0.33, 0.30}, Path[3] = {0.36, 0.29, 0.21};
                for (int C = 0; C < 3; ++C) Col[C] = Col[C] + (Path[C] - Col[C]) * Trodden * (1.0 - Steep);
                for (int C = 0; C < 3; ++C) Col[C] = Col[C] + (Cobble[C] - Col[C]) * W[3] * (InTown ? 1.0 : 0.0);
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
            J += "        \"macro.strength\": 0.85,\n        \"macro.tiles\": [" + GltfBits::Num(L.Tile[0]) + ", " + GltfBits::Num(L.Tile[1]) + ", " + GltfBits::Num(L.Tile[2]) + ", " +
                 GltfBits::Num(L.Tile[3]) + "]\n    }\n}\n";
            Out.Files.push_back({std::string("Materials/M_World_") + L.Biome + ".material", J});
            // the town variant: the accent layer is cobblestone paving
            const std::string Accent = std::string("../../Art/Scanned/") + L.Layer[3] + "/" + L.Layer[3], Paving = "../../Art/Scanned/cobblestone_floor_08/cobblestone_floor_08";
            std::string Town = J;
            for (const char* Map : {"_arm_2k.jpg", "_diff_2k.jpg", "_nor_gl_2k.jpg"})
            {
                const std::string From = Accent + Map, To = Paving + Map;
                const std::size_t At = Town.find("\"layer3." + std::string(Map[1] == 'a' ? "arm" : Map[1] == 'd' ? "color" : "normal") + "\": \"" + From);
                if (At != std::string::npos) Town.replace(Town.find(From, At), From.size(), To);
            }
            Out.Files.push_back({std::string("Materials/M_World_") + L.Biome + "_town.material", Town});
        }

        Material Placeholder;
        Placeholder.Name = "M_World_Terrain";
        Placeholder.BaseColor = {0.4, 0.42, 0.3, 1.0};
        Placeholder.Roughness = 0.95;
        Out.Files.push_back({"SM_World_Coarse.gltf", WriteGltf(Grid("SM_World_Coarse", 0.0, 0.0, Spec.Width, Spec.Height, CoarseStep, 0.6, Placeholder), "Dark Arisen world-build")});

        // asset references as the runtime takes them: source UUID, product sub-id, product path (scan folder: the project)
        const auto ModelRef = [](const std::string& ProjectRelative)
        {
            const std::size_t Slash = ProjectRelative.find_last_of('/'), Dot = ProjectRelative.find_last_of('.');
            const std::string Stem = ProjectRelative.substr(Slash + 1, Dot - Slash - 1);
            std::string Hint = ProjectRelative.substr(0, Slash + 1) + Stem + ".azmodel";
            std::transform(Hint.begin(), Hint.end(), Hint.begin(), [](char C) { return C >= 'A' && C <= 'Z' ? static_cast<char>(C + 32) : C; });
            return "{\"id\": \"" + SourceAssetUuid(ProjectRelative) + "\", \"subId\": " + std::to_string(AtomModelSubId(Stem)) + ", \"hint\": \"" + Hint + "\"}";
        };
        const auto MaterialRef = [](const std::string& ProjectRelative)
        {
            if (ProjectRelative.empty()) return std::string("null");
            std::string Hint = ProjectRelative.substr(0, ProjectRelative.find_last_of('.')) + ".azmaterial";
            std::transform(Hint.begin(), Hint.end(), Hint.begin(), [](char C) { return C >= 'A' && C <= 'Z' ? static_cast<char>(C + 32) : C; });
            return "{\"id\": \"" + SourceAssetUuid(ProjectRelative) + "\", \"subId\": 0, \"hint\": \"" + Hint + "\"}";
        };
        const auto Cm = [](double V) { return std::to_string(static_cast<long long>(std::llround(V * 100.0))); };

        // the plants of the world: one table, each tile's plantings index into it
        std::map<std::string, std::vector<PlantPart>, std::less<>> PlantCache;
        std::map<std::string, std::size_t, std::less<>> PlantIndex;
        std::string PlantTable;
        std::vector<WorldTileSpec::Clearing> Clear;
        for (const WorldPlaceMark& P : Places) Clear.push_back({P.X, P.Y, P.Radius});
        for (const WorldLocation& L : Locations) Clear.push_back({L.X, L.Y, L.Radius});
        // no tree on a road or its verge
        for (const WorldRoad& Road : Roads)
            for (std::size_t K = 0; K + 1 < Road.Points.size(); ++K)
            {
                const WorldRoad::Point& A = Road.Points[K];
                const WorldRoad::Point& B = Road.Points[K + 1];
                const double Len = std::sqrt((B.X - A.X) * (B.X - A.X) + (B.Y - A.Y) * (B.Y - A.Y));
                for (double D = 0.0; D <= Len; D += 8.0)
                    Clear.push_back({A.X + (B.X - A.X) * D / std::max(Len, 1e-9), A.Y + (B.Y - A.Y) * D / std::max(Len, 1e-9), Road.Width / 2 + 6.0});
            }

        // the settlements' buildings, each on the tile under it; one model table for the world
        std::map<std::string, std::size_t, std::less<>> StructureIndex;
        std::string StructureTable;
        std::map<int, std::string> TileStructures;
        const auto AddStructure = [&](const Structure& B)
        {
            auto Known = StructureIndex.find(B.Model);
            if (Known == StructureIndex.end())
            {
                StructureTable += std::string(StructureIndex.empty() ? "" : ",") + "\n    {\"model\": " + ModelRef(B.Model) + ", \"material\": null, \"box\": [" +
                                  GltfBits::Num(B.HalfX) + ", " + GltfBits::Num(B.HalfY) + ", " + GltfBits::Num(B.Height) + "]}";
                Known = StructureIndex.emplace(B.Model, StructureIndex.size()).first;
            }
            const int Tile = static_cast<int>(B.Y / TileSize) * static_cast<int>(Spec.Width / TileSize + 0.5) + static_cast<int>(B.X / TileSize);
            std::string& List = TileStructures[Tile];
            List += std::string(List.empty() ? "" : ",") + "[" + std::to_string(Known->second) + "," + Cm(B.X) + "," + Cm(B.Y) + "," + Cm(B.Z) + "," +
                    std::to_string(static_cast<long long>(std::llround(B.Yaw))) + "]";
            ++Out.Structures;
        };
        for (const Site& Si : Sites)
        {
            SettlementSpec Settle;
            Settle.Kind = Si.Kind;
            Settle.X = Si.Where->X;
            Settle.Y = Si.Where->Y;
            Settle.Radius = Si.Where->Radius;
            Settle.Ground = Height;
            Settle.MinGround = Si.Where->Kind == "zone" ? 0.2 : 1.2;
            const bool Sparse = Si.Where->Kind == "zone" || Si.Kind == Tradition::Camp;
            Settle.MaxBuildings = Si.Kind == Tradition::Colonial ? 160 : Sparse ? 14 : 40;
            Settle.Fill = Si.Kind == Tradition::Colonial ? 0.92 : Sparse ? 0.35 : 0.75;
            // the level design's own parameters, where it has them (Locations.json "layout")
            const WorldLocation::Layout& Plan = Si.Where->Plan;
            if (Plan.Cap > 0) Settle.MaxBuildings = Plan.Cap;
            if (Plan.Fill >= 0.0) Settle.Fill = Plan.Fill;
            Settle.Waterfront = Plan.Waterfront;
            Settle.Cluster = Plan.Cluster;
            if (Plan.Tradition == "Stilt") Settle.Houses = Tradition::Stilt;
            if (Plan.HasLandmark) { Settle.LandmarkU = Plan.LandmarkU; Settle.LandmarkW = Plan.LandmarkW; Settle.LandmarkYaw = Plan.LandmarkYaw; }
            if (Plan.HasPlaza) { Settle.PlazaU = Plan.PlazaU; Settle.PlazaW = Plan.PlazaW; Settle.PlazaRadius = Plan.PlazaRadius; }
            Settle.LampSpacing = Plan.LampSpacing;
            Settle.StallRadius = Plan.StallRadius;
            Settle.StallPitch = Plan.StallPitch;
            Settle.StallMax = Plan.StallMax;
            Settle.Benches = Plan.Benches;
            Settle.Seed = 0;
            for (const char C : Si.Where->Id) Settle.Seed = Settle.Seed * 31u + static_cast<std::uint8_t>(C);
            // a town's landmark on its square (Rexa's districts by their documents, Ashenmoor's Kota Api, Jernholm's old fort)
            const std::string& Id = Si.Where->Id;
            const auto Ends = [&Id](const char* Tail) { const std::string T(Tail); return Id.size() >= T.size() && Id.compare(Id.size() - T.size(), T.size(), T) == 0; };
            if (Ends(".cathedral")) Settle.Centre = Landmark::Cathedral;
            else if (Ends(".market") || Ends(".graavig") || Ends(".kota_api")) Settle.Centre = Landmark::MarketHall;
            else if (Ends(".harbor") || Ends(".smugglers_reach")) Settle.Centre = Landmark::Warehouse;
            else if (Ends(".noble") || Ends(".plantation_masters_domain")) Settle.Centre = Landmark::Mansion;
            else if (Ends(".cliffside")) Settle.Centre = Landmark::Faro;  // the Old Spanish Lighthouse (region_02_rexa_city.md 2.6)
            else if (Ends(".jernholm") || Ends(".pirate_encampment")) Settle.Centre = Landmark::Watchtower;
            Settle.StreetAngle = Si.Kind == Tradition::Colonial ? 12.0 : static_cast<double>(Settle.Seed % 180u);  // Rexa's grid runs up from the harbour
            for (const Structure& B : Kit::SettlementLayout(Settle)) AddStructure(B);
        }
        // along the roads (layout.md 3.2-3.4): stairs where they climb, gas lamps at the kerb, and between the districts a
        // house every 35-60 m on alternating sides, set back 6 m, so the city reads as one
        for (const WorldRoad& Road : Roads)
        {
            if (Road.Points.size() < 2) continue;
            std::uint32_t Seed = 2166136261u;
            for (const char C : Road.Id) Seed = (Seed ^ static_cast<std::uint8_t>(C)) * 16777619u;
            Rng R(Seed);
            const bool Stairs = Road.Kind == "stairs";
            if (Stairs)
            {
                // one flight of steps the width of the stair, about 16 cm a step (E2: 328 steps), on the graded ramp
                const WorldRoad::Point A = Road.Points.front(), B = Road.Points.back();
                const double Dx = B.X - A.X, Dy = B.Y - A.Y, Run = std::sqrt(Dx * Dx + Dy * Dy), Rise = B.Z - A.Z;
                const int Steps = std::max(1, static_cast<int>(std::fabs(Rise) / 0.16 + 0.5));
                Mesh Flight("SM_World_Stairs_" + Road.Id.substr(Road.Id.find_last_of('.') + 1));
                for (int K = 0; K < Steps; ++K)
                {
                    const double T = (K + 0.5) / Steps, Top = Rise * (Rise > 0.0 ? (K + 1.0) : static_cast<double>(K)) / Steps;
                    OrientedBox(Flight, Mat(M::Stone), {Dx * T, Dy * T, Top - 0.35}, Dx, Dy, {Run / Steps / 2 + 0.01, Road.Width / 2, 0.35});
                }
                // low parapets either side, one block per twenty steps
                for (int K = 0; K < Steps; K += 20)
                {
                    const int Last = std::min(Steps, K + 20);
                    const double T = (K + Last) * 0.5 / Steps, Top = Rise * (K + Last) * 0.5 / Steps;
                    for (const double Side : {-1.0, 1.0})
                        OrientedBox(Flight, Mat(M::StoneDark), {Dx * T - Dy / Run * Side * (Road.Width / 2 + 0.25), Dy * T + Dx / Run * Side * (Road.Width / 2 + 0.25), Top + 0.1}, Dx, Dy,
                            {Run * (Last - K) / Steps / 2, 0.25, 0.7});
                }
                const std::string Name = Flight.Name();
                Out.Files.push_back({"Stairs/" + Name + ".gltf", WriteGltf(Flight, "Dark Arisen world-build")});
                AddStructure({"Assets/World/Stairs/" + Name + ".gltf", A.X, A.Y, A.Z, 0.0, 0.5, 0.5, 0.1});
            }
            double Walked = 0.0, NextLamp = 10.0, NextHouse = R.Range(20.0, 40.0);
            int LampSide = 1, HouseSide = 1;
            for (std::size_t K = 0; K + 1 < Road.Points.size(); ++K)
            {
                const WorldRoad::Point& A = Road.Points[K];
                const WorldRoad::Point& B = Road.Points[K + 1];
                const double Len = std::sqrt((B.X - A.X) * (B.X - A.X) + (B.Y - A.Y) * (B.Y - A.Y));
                if (Len < 1e-6) continue;
                const double Ux = (B.X - A.X) / Len, Uy = (B.Y - A.Y) / Len;
                for (double D = 0.0; D < Len; D += 1.0, Walked += 1.0)
                {
                    const double Cx = A.X + Ux * D, Cy = A.Y + Uy * D;
                    if (Walked >= NextLamp)
                    {
                        const double Off = Road.Width / 2 + 0.8;
                        const double X = Cx - Uy * Off * LampSide, Y = Cy + Ux * Off * LampSide;
                        AddStructure(Kit::SettlementPiece(Tradition::Street, 0, X, Y, Height(X, Y), 0.0));
                        NextLamp += Stairs ? 20.0 : 30.0;
                        LampSide = -LampSide;
                    }
                    if (!Stairs && Walked >= NextHouse)
                    {
                        NextHouse += R.Range(35.0, 60.0);
                        const int V = static_cast<int>(R.Unit() * 6.0);
                        const double Off = Road.Width / 2 + 6.0 + 5.0;
                        const double X = Cx - Uy * Off * HouseSide, Y = Cy + Ux * Off * HouseSide;
                        bool InDistrict = false;
                        for (const Site& Si : Sites)
                        {
                            const double Ex = X - Si.Where->X, Ey = Y - Si.Where->Y;
                            InDistrict = InDistrict || Ex * Ex + Ey * Ey < Si.Where->Radius * Si.Where->Radius;
                        }
                        const double Z = Height(X, Y);
                        // the door (-Y in the model) to the road
                        const double Yaw = Atan2(-Ux * HouseSide, Uy * HouseSide) * 180.0 / Pi - 270.0;
                        if (!InDistrict && Z > 1.0 && std::fabs(Z - Height(Cx, Cy)) < 2.5) AddStructure(Kit::SettlementPiece(Tradition::Colonial, V, X, Y, Z, Yaw));
                        HouseSide = -HouseSide;
                    }
                }
            }
        }
        // lava in the craters of the volcanoes the documents call active or erupting (Gunung Ibu, Gunung Anak): a molten
        // disc on the crater floor, found from the ground itself, drawn with the DarkArisenLava material
        bool AnyLava = false;
        for (const WorldLocation& L : Locations)
        {
            const bool Live = L.Kind == "volcano" && (L.Id.find("gunung_ibu") != std::string::npos || L.Id.find("gunung_anak") != std::string::npos);
            if (!Live) continue;
            int Owner = -1;
            ArchipelagoHeight(Spec, L.X, L.Y, &Owner);
            if (Owner < 0 || Spec.Islands[static_cast<std::size_t>(Owner)].Shape != "volcano") continue;  // a dome has no crater to fill
            const ArchipelagoIsland& I = Spec.Islands[static_cast<std::size_t>(Owner)];
            // the crater floor: the lowest ground within the summit's reach, then out until the wall rises 6 m above it
            double Cx = I.X, Cy = I.Y, Floor = Height(I.X, I.Y);
            for (double R = 0.0; R <= 120.0; R += 20.0)
                for (int K = 0; K < 8; ++K)
                {
                    const double X = I.X + R * Cos(Pi * K / 4.0), Y = I.Y + R * Sin(Pi * K / 4.0), Z = Height(X, Y);
                    if (Z < Floor) { Floor = Z; Cx = X; Cy = Y; }
                }
            double Reach = 0.0;
            for (double R = 5.0; R < 600.0; R += 5.0)
            {
                bool Inside = true;
                for (int K = 0; K < 16 && Inside; ++K) Inside = Height(Cx + R * Cos(Pi * K / 8.0), Cy + R * Sin(Pi * K / 8.0)) < Floor + 6.0;
                if (!Inside) break;
                Reach = R;
            }
            if (Reach < 15.0) continue;
            const std::string Name = "SM_World_Lava_" + L.Id.substr(L.Id.find_last_of('.') + 1);
            Mesh Lake(Name);
            std::vector<Vertex> LV{Vertex{V3{0.0, 0.0, 0.0}, V3{0, 0, 1}, 0.5, 0.5}};
            std::vector<std::uint32_t> LI;
            const int Segments = 48;
            for (int K = 0; K < Segments; ++K)
            {
                const double A = 2.0 * Pi * K / Segments, Rr = Reach * (0.9 + 0.08 * Sin(3.0 * A) + 0.04 * Cos(7.0 * A));
                LV.push_back(Vertex{V3{Rr * Cos(A), Rr * Sin(A), 0.0}, V3{0, 0, 1}, 0.5 + 0.5 * Cos(A), 0.5 - 0.5 * Sin(A)});
                LI.insert(LI.end(), {0u, static_cast<std::uint32_t>(1 + K), static_cast<std::uint32_t>(1 + (K + 1) % Segments)});
            }
            Material Molten;
            Molten.Name = "M_World_Lava";
            Molten.BaseColor = {1.0, 0.35, 0.05, 1.0};
            Lake.Indexed(Molten, LV, LI);
            Out.Files.push_back({"Lava/" + Name + ".gltf", WriteGltf(Lake, "Dark Arisen world-build")});
            const std::string Model = "Assets/World/Lava/" + Name + ".gltf";
            StructureTable += std::string(StructureIndex.empty() ? "" : ",") + "\n    {\"model\": " + ModelRef(Model) + ", \"material\": " +
                              MaterialRef("Assets/World/Materials/M_World_Lava.material") + ", \"box\": [0.5, 0.5, 0.1]}";
            const std::size_t LavaIndex = StructureIndex.emplace(Model, StructureIndex.size()).first->second;
            const int Tile = static_cast<int>(Cy / TileSize) * static_cast<int>(Spec.Width / TileSize + 0.5) + static_cast<int>(Cx / TileSize);
            std::string& List = TileStructures[Tile];
            List += std::string(List.empty() ? "" : ",") + "[" + std::to_string(LavaIndex) + "," + Cm(Cx) + "," + Cm(Cy) + "," + Cm(Floor + 1.5) + ",0]";
            ++Out.Structures;
            AnyLava = true;
        }
        if (AnyLava)
            Out.Files.push_back({"Materials/M_World_Lava.material",
                "{\n    \"materialType\": \"../../Materials/Types/DarkArisenLava.materialtype\",\n    \"materialTypeVersion\": 1,\n    \"propertyValues\": {\n"
                "        \"melt.moltenColor\": [1.0, 0.36, 0.06],\n        \"melt.glowIntensity\": 6.0,\n        \"crust.crustColor\": [0.05, 0.04, 0.035],\n"
                "        \"crust.coverage\": 0.55,\n        \"crust.featureSize\": 6.0,\n        \"crust.roughness\": 0.85\n    }\n}\n"});

        const auto BiomeOf = [](const std::string& B)
        {
            if (B == "tropical") return Biome::Tropical;
            if (B == "mire") return Biome::Mire;
            if (B == "volcanic" || B == "maw") return Biome::Dream;
            if (B == "fortress") return Biome::Grove;
            return Biome::Moor;  // moor, cold
        };

        const int Columns = static_cast<int>(Spec.Width / TileSize + 0.5), Rows = static_cast<int>(Spec.Height / TileSize + 0.5);
        const int Samples = static_cast<int>(TileSize / TileStep + 0.5) + 1;
        std::string Index = "{\n  \"tileSize\": " + GltfBits::Num(TileSize) + ",\n  \"columns\": " + std::to_string(Columns) + ",\n  \"rows\": " + std::to_string(Rows) +
                            ",\n  \"heightStep\": " + GltfBits::Num(TileStep) + ",\n  \"heightSamples\": " + std::to_string(Samples) + ",\n  \"tiles\": [";
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
                const ArchipelagoIsland* Island = Owner >= 0 ? &Spec.Islands[static_cast<std::size_t>(Owner)] : nullptr;
                const std::string Biome = Island ? Island->Biome : std::string("tropical");
                const std::string Suffix = std::to_string(Column) + "_" + std::to_string(Row);
                const std::string Name = "SM_World_Tile_" + Suffix;
                Out.Files.push_back({"Tiles/" + Name + ".gltf", WriteGltf(Grid(Name, X0, Y0, X0 + TileSize, Y0 + TileSize, TileStep, 0.0, Placeholder), "Dark Arisen world-build")});

                // plantings and the ground heights (cm, rows from the south) for the runtime's grass
                WorldTileSpec Plant;
                Plant.Kind = BiomeOf(Biome);
                Plant.X0 = X0;
                Plant.Y0 = Y0;
                Plant.Size = TileSize;
                Plant.Height = Height;
                Plant.Clear = Clear;
                Plant.TreeLine = Island ? Island->Peak * (Biome == "cold" ? 0.55 : Biome == "volcanic" ? 0.4 : 0.85) : 1e9;
                Plant.Seed = Spec.Seed;
                std::string Foliage = "{\"tile\": " + std::to_string(Row * Columns + Column) + ", \"x0\": " + GltfBits::Num(X0) + ", \"y0\": " + GltfBits::Num(Y0) +
                                      ", \"biome\": \"" + Biome + "\",\n\"plants\": [";
                std::size_t Count = 0;
                for (const Planting& P : Kit::WorldVegetation(Plant, PlantCache))
                {
                    std::string Key;
                    for (const PlantPart& Part : P.Parts) Key += Part.Model + "|" + Part.Material + ";";
                    auto Known = PlantIndex.find(Key);
                    if (Known == PlantIndex.end())
                    {
                        std::string Parts;
                        for (const PlantPart& Part : P.Parts)
                            Parts += std::string(Parts.empty() ? "" : ", ") + "{\"model\": " + ModelRef(Part.Model) + ", \"material\": " + MaterialRef(Part.Material) + "}";
                        PlantTable += std::string(PlantIndex.empty() ? "" : ",") + "\n    {\"tree\": " + (P.Tree ? "true" : "false") + ", \"parts\": [" + Parts + "]}";
                        Known = PlantIndex.emplace(Key, PlantIndex.size()).first;
                    }
                    Foliage += std::string(Count++ ? ",\n" : "\n") + "[" + std::to_string(Known->second) + "," + Cm(P.X) + "," + Cm(P.Y) + "," + Cm(P.Z) + "," +
                               std::to_string(static_cast<long long>(std::llround(P.Yaw))) + "," + std::to_string(static_cast<long long>(std::llround(P.Scale * 100.0))) + "]";
                }
                Foliage += "],\n\"heights\": [";
                for (int J = 0; J < Samples; ++J)
                {
                    Foliage += J ? ",\n" : "\n";
                    for (int K = 0; K < Samples; ++K) Foliage += (K ? "," : "") + Cm(Height(X0 + K * TileStep, Y0 + J * TileStep));
                }
                Foliage += "]}\n";
                Out.Plantings += Count;
                Out.Files.push_back({"Tiles/F_World_" + Suffix + ".json", Foliage});

                Index += std::string(First ? "" : ",") + "\n    {\"tile\": " + std::to_string(Row * Columns + Column) + ", \"column\": " + std::to_string(Column) + ", \"row\": " +
                         std::to_string(Row) + ", \"biome\": \"" + Biome + "\", \"model\": " + ModelRef("Assets/World/Tiles/" + Name + ".gltf") +
                         ", \"material\": " + MaterialRef("Assets/World/Materials/M_World_" + Biome + (TownTiles.count(Row * Columns + Column) ? "_town" : "") + ".material") +
                         ", \"foliage\": \"assets/world/tiles/f_world_" + Suffix + ".json\"" +
                         (TileStructures.count(Row * Columns + Column) ? ", \"structures\": [" + TileStructures[Row * Columns + Column] + "]" : std::string()) + "}";
                First = false;
                ++Out.Tiles;
            }
        Index += "\n  ],\n  \"plants\": [" + PlantTable + "\n  ],\n  \"structures\": [" + StructureTable + "\n  ],\n  \"grass\": [";
        bool FirstPatch = true;
        for (const GrassPatch& G : Kit::WorldGrassPatches())
        {
            const std::string Model = "Grass/SM_World_Grass_" + G.Card + ".gltf", Material = "Materials/M_World_Grass_" + G.Card + ".material";
            // the patch lives in Assets/World/Grass, the kit's card textures in Assets/Art/Textures
            std::string Gltf = G.Gltf;
            const std::string From = "\"uri\":\"../Textures/", To = "\"uri\":\"../../Art/Textures/";
            for (std::size_t At = Gltf.find(From); At != std::string::npos; At = Gltf.find(From, At + To.size())) Gltf.replace(At, From.size(), To);
            Out.Files.push_back({Model, Gltf});
            Out.Files.push_back({Material, G.Material});
            Index += std::string(FirstPatch ? "" : ",") + "\n    {\"card\": \"" + G.Card + "\", \"model\": " + ModelRef("Assets/World/" + Model) + ", \"material\": " +
                     MaterialRef("Assets/World/" + Material) + "}";
            FirstPatch = false;
        }
        // the towns' paving: no grass grows there
        std::string Paved;
        for (const Site& Si : Sites)
            if (Si.Kind == Tradition::Colonial || Si.Kind == Tradition::Fort)
                Paved += std::string(Paved.empty() ? "" : ", ") + "[" + GltfBits::Num(Si.Where->X) + ", " + GltfBits::Num(Si.Where->Y) + ", " +
                         GltfBits::Num(Si.Where->Radius * 0.9) + "]";
        // and the roads: a disc every few metres along each, as wide as the road
        for (const WorldRoad& Road : Roads)
            for (std::size_t K = 0; K + 1 < Road.Points.size(); ++K)
            {
                const WorldRoad::Point& A = Road.Points[K];
                const WorldRoad::Point& B = Road.Points[K + 1];
                const double Len = std::sqrt((B.X - A.X) * (B.X - A.X) + (B.Y - A.Y) * (B.Y - A.Y));
                for (double D = 0.0; D <= Len; D += Road.Width / 2)
                    Paved += std::string(Paved.empty() ? "" : ", ") + "[" + GltfBits::Num(A.X + (B.X - A.X) * D / std::max(Len, 1e-9)) + ", " +
                             GltfBits::Num(A.Y + (B.Y - A.Y) * D / std::max(Len, 1e-9)) + ", " + GltfBits::Num(Road.Width / 2 + 1.0) + "]";
            }
        Index += "\n  ],\n  \"paved\": [" + Paved;
        // which cards grow on each biome's open ground, and how densely (patches per 2 m cell)
        Index += "\n  ],\n  \"biomes\": {\n    \"tropical\": {\"grass\": [\"GrassBlades\"], \"density\": 0.85},\n    \"mire\": {\"grass\": [\"Reeds\", \"GrassBlades\"], \"density\": 0.6},\n"
                 "    \"moor\": {\"grass\": [\"HeatherSprig\", \"GrassDry\"], \"density\": 0.95},\n    \"cold\": {\"grass\": [\"GrassDry\", \"HeatherSprig\"], \"density\": 0.5},\n"
                 "    \"fortress\": {\"grass\": [\"GrassBlades\", \"GrassDry\"], \"density\": 0.65},\n    \"volcanic\": {\"grass\": [\"GrassDry\"], \"density\": 0.12},\n"
                 "    \"maw\": {\"grass\": [\"GrassDry\"], \"density\": 0.05}\n  }\n}\n";
        Out.Files.push_back({"WorldTiles.json", Index});
        return Out;
    }
}
