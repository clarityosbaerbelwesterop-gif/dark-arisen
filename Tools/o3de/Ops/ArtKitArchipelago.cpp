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
}
