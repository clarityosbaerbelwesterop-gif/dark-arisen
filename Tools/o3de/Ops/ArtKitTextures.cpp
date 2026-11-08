#include "ArtKitLibrary.h"

#include <algorithm>
#include <cmath>

namespace DarkArisen::Tools::Art
{
    namespace
    {
        constexpr int S = 256;  // every texture tiles seamlessly at 256 x 256

        struct Surface
        {
            Image Color;
            std::vector<int> Height;
            explicit Surface(int Channels = 3)
            {
                Color.Width = Color.Height = S;
                Color.Channels = Channels;
                Color.Pixels.assign(static_cast<std::size_t>(S * S * Channels), 0);
                Height.assign(static_cast<std::size_t>(S * S), 32768);
            }
            void Put(int X, int Y, double R, double G, double B, double H, double A = 1.0)
            {
                const auto To8 = [](double V) { return static_cast<std::uint8_t>(std::lround(Clamp(V, 0.0, 1.0) * 255.0)); };
                std::uint8_t* P = &Color.Pixels[static_cast<std::size_t>((Y * S + X) * Color.Channels)];
                P[0] = To8(R);
                P[1] = To8(G);
                P[2] = To8(B);
                if (Color.Channels == 4) P[3] = To8(A);
                Height[static_cast<std::size_t>(Y * S + X)] = static_cast<int>(std::lround(Clamp(H, 0.0, 1.0) * 65535.0));
            }
        };

        double N01(int V) { return V / 65535.0; }
        double Hash01(std::uint32_t A, std::uint32_t B)
        {
            Rng R(A * 7919u + B * 104729u + 17u);
            return R.Unit();
        }

        /** Plank field with the boards running along V (texture rows). */
        Surface Planks(int PlankWidth, int SegmentLength, std::uint32_t Seed, double Base, double Variation, double SeamDark)
        {
            Surface Out;
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    const int Column = X / PlankWidth;
                    const int Within = X % PlankWidth;
                    const int Offset = static_cast<int>(Hash01(Seed, static_cast<std::uint32_t>(Column)) * S);
                    const int Along = (Y - Offset + S * 4) % S;
                    const int Segment = Along / SegmentLength;
                    const int SegmentPos = Along % SegmentLength;
                    const double Tint = (Hash01(Seed + 1u, static_cast<std::uint32_t>(Column * 8 + Segment)) - 0.5) * Variation;
                    const double Grain = N01(Noise((X * 8) % S, Y, S, 16, Seed + 2u));
                    const double Fine = N01(Noise((X * 16) % S, (Y * 2) % S, S, 32, Seed + 3u));
                    const double Wear = N01(Fbm(X, Y, S, 4, 3, Seed + 4u));
                    double Value = Base * (0.82 + 0.22 * Grain + 0.08 * Fine) + Tint;
                    double H = 0.72 + 0.12 * Grain + 0.05 * Fine;
                    const int EdgeDistance = std::min(Within, PlankWidth - 1 - Within);
                    if (EdgeDistance == 0 || SegmentPos == 0)
                    {
                        Value *= SeamDark;
                        H = 0.08;
                    }
                    else if (EdgeDistance == 1)
                    {
                        Value *= 0.9;
                        H -= 0.18;
                    }
                    // Treenails near every butt joint.
                    const int NailRow = (SegmentPos + 3) % SegmentLength;
                    if (NailRow < 3 && (Within == PlankWidth / 4 || Within == PlankWidth * 3 / 4))
                    {
                        Value *= 0.72;
                        H -= 0.2;
                    }
                    Out.Put(X, Y, Value, Value, Value, H + 0.06 * (Wear - 0.5));
                }
            }
            return Out;
        }

        void Colorize(Surface& Surf, double R, double G, double B, double GreyR, double GreyG, double GreyB, std::uint32_t Seed, double WeatherAmount)
        {
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    std::uint8_t* P = &Surf.Color.Pixels[static_cast<std::size_t>((Y * S + X) * Surf.Color.Channels)];
                    const double V = P[0] / 255.0;
                    const double Weather = SmoothStep(0.45, 0.8, N01(Fbm(X, Y, S, 4, 4, Seed))) * WeatherAmount;
                    const auto Mix = [&](double C, double Grey) { return Lerp(V * C / 0.55, V * Grey / 0.55, Weather); };
                    const auto To8 = [](double C) { return static_cast<std::uint8_t>(std::lround(Clamp(C, 0.0, 1.0) * 255.0)); };
                    P[0] = To8(Mix(R, GreyR));
                    P[1] = To8(Mix(G, GreyG));
                    P[2] = To8(Mix(B, GreyB));
                }
            }
        }

        Surface WoodDeck()
        {
            Surface Out = Planks(32, 128, 11u, 0.55, 0.12, 0.25);
            Colorize(Out, 0.43, 0.33, 0.23, 0.50, 0.48, 0.44, 12u, 0.55);
            return Out;
        }

        Surface HullPlanks()
        {
            // Neutral light boards: the hull materials tint them (tar brown, ochre band, red band).
            Surface Out = Planks(24, 256, 21u, 0.62, 0.08, 0.4);
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    // Salt streaks run down the ship's side: across the boards in texture space.
                    const double Salt = SmoothStep(0.62, 0.85, N01(Noise(X, (Y * 8) % S, S, 16, 23u)));
                    std::uint8_t* P = &Out.Color.Pixels[static_cast<std::size_t>((Y * S + X) * 3)];
                    for (int C = 0; C < 3; ++C)
                    {
                        P[C] = static_cast<std::uint8_t>(std::lround(Clamp(P[C] / 255.0 + Salt * 0.07, 0.0, 1.0) * 255.0));
                    }
                }
            }
            return Out;
        }

        Surface Timber()
        {
            Surface Out;
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    const double Grain = N01(Noise((X * 8) % S, Y, S, 16, 31u));
                    const double Rings = N01(Noise((X * 16) % S, (Y * 2) % S, S, 32, 32u));
                    const double Wear = N01(Fbm(X, Y, S, 4, 3, 33u));
                    const double V = 0.55 * (0.8 + 0.25 * Grain + 0.1 * Rings) - 0.06 * SmoothStep(0.6, 0.9, Wear);
                    Out.Put(X, Y, V, V, V, 0.5 + 0.2 * Grain);
                }
            }
            return Out;
        }

        Surface Canvas()
        {
            Surface Out;
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    const double Weave = ((X & 1) ^ (Y & 1)) ? 0.018 : -0.018;
                    const double Stain = SmoothStep(0.55, 0.85, N01(Fbm(X, Y, S, 4, 4, 41u)));
                    const double Fine = N01(Noise(X, Y, S, 64, 42u)) - 0.5;
                    double R = 0.80 + Weave + 0.03 * Fine, G = 0.75 + Weave + 0.03 * Fine, B = 0.64 + Weave + 0.03 * Fine;
                    R = Lerp(R, 0.70, Stain * 0.35);
                    G = Lerp(G, 0.62, Stain * 0.35);
                    B = Lerp(B, 0.48, Stain * 0.35);
                    if (X % 32 == 0 || X % 32 == 1)  // panel seams with stitching
                    {
                        const double Stitch = (Y % 6 < 3) ? 0.78 : 0.88;
                        R *= Stitch;
                        G *= Stitch;
                        B *= Stitch;
                    }
                    Out.Put(X, Y, R, G, B, 0.5 + Weave * 4);
                }
            }
            return Out;
        }

        Surface Rope()
        {
            Surface Out;
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    const int T = (X + Y * 2) % 32;
                    const double Twist = T < 16 ? 0.5 + T / 32.0 : 1.0 - (T - 16) / 32.0;
                    const double Fibre = N01(Noise((X * 4) % S, Y, S, 32, 51u)) - 0.5;
                    const double V = 0.6 + 0.35 * Twist + 0.08 * Fibre;
                    Out.Put(X, Y, 0.56 * V, 0.46 * V, 0.31 * V, Twist);
                }
            }
            return Out;
        }

        Surface Plaster()
        {
            Surface Out;
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    const double Patch = N01(Fbm(X, Y, S, 4, 4, 61u));
                    const double Fine = N01(Noise(X, Y, S, 64, 62u)) - 0.5;
                    const double Streak = SmoothStep(0.6, 0.9, N01(Noise((X * 8) % S, Y, S, 8, 63u)));
                    double R = 0.84 + 0.03 * Fine, G = 0.80 + 0.03 * Fine, B = 0.71 + 0.03 * Fine, H = 0.7 + 0.05 * Fine;
                    R -= 0.08 * Streak;
                    G -= 0.08 * Streak;
                    B -= 0.07 * Streak;
                    if (Patch > 0.8)  // plaster fallen away: the brick underneath
                    {
                        const int Row = Y / 12;
                        const int BrickX = (X + (Row & 1) * 12) % 24;
                        const bool Mortar = (Y % 12 == 0) || BrickX == 0;
                        const double Tint = Hash01(64u, static_cast<std::uint32_t>(Row * 16 + (X + (Row & 1) * 12) / 24)) * 0.08;
                        R = Mortar ? 0.62 : 0.55 + Tint;
                        G = Mortar ? 0.58 : 0.36 + Tint * 0.5;
                        B = Mortar ? 0.52 : 0.26;
                        H = Mortar ? 0.3 : 0.45;
                    }
                    else if (Patch > 0.77)
                    {
                        R *= 0.86;
                        G *= 0.84;
                        B *= 0.82;
                        H = 0.6;
                    }
                    Out.Put(X, Y, R, G, B, H);
                }
            }
            return Out;
        }

        Surface RoofTiles()
        {
            Surface Out;
            constexpr int TileW = 32, TileH = 24;
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    const int Row = Y / TileH;
                    const int Shift = (Row & 1) * (TileW / 2);
                    const int Col = ((X + Shift) % S) / TileW;
                    const int DX = (X + Shift) % TileW;
                    const int DY = Y % TileH;
                    const double Across = (DX - TileW / 2.0) / (TileW / 2.0);
                    const double Bulge = 1.0 - Across * Across;
                    const double Tint = Hash01(71u, static_cast<std::uint32_t>(Row * 64 + Col)) - 0.5;
                    const double Moss = SmoothStep(0.72, 0.85, N01(Fbm(X, Y, S, 8, 3, 72u)));
                    double Shade = 0.62 + 0.38 * Bulge;
                    if (DY > TileH - 5) Shade *= 0.62;  // shadow of the next course
                    const double Fine = N01(Noise(X, Y, S, 64, 73u)) - 0.5;
                    double R = (0.60 + 0.08 * Tint + 0.03 * Fine) * Shade, G = (0.29 + 0.04 * Tint + 0.02 * Fine) * Shade,
                           B = (0.17 + 0.03 * Tint) * Shade;
                    R = Lerp(R, 0.40 * Shade, Moss * 0.7);
                    G = Lerp(G, 0.40 * Shade, Moss * 0.7);
                    B = Lerp(B, 0.20 * Shade, Moss * 0.7);
                    Out.Put(X, Y, R, G, B, 0.2 + 0.6 * Bulge + 0.2 * (1.0 - DY / static_cast<double>(TileH)));
                }
            }
            return Out;
        }

        Surface Stone()
        {
            Surface Out;
            constexpr int RowH = 32;
            for (int Y = 0; Y < S; ++Y)
            {
                const int Row = Y / RowH;
                // Block boundaries of this course, deterministic and wrapping at the tile edge.
                std::vector<int> Joints;
                int At = static_cast<int>(Hash01(81u, static_cast<std::uint32_t>(Row)) * 40);
                while (At < S + At % 1)
                {
                    Joints.push_back(At);
                    At += 40 + static_cast<int>(Hash01(82u, static_cast<std::uint32_t>(Row * 32 + static_cast<int>(Joints.size()))) * 48);
                    if (At >= S) break;
                }
                for (int X = 0; X < S; ++X)
                {
                    int Block = 0, Left = Joints.back() - S, Right = Joints.front();
                    for (std::size_t I = 0; I < Joints.size(); ++I)
                    {
                        if (X >= Joints[I])
                        {
                            Block = static_cast<int>(I) + 1;
                            Left = Joints[I];
                            Right = I + 1 < Joints.size() ? Joints[I + 1] : Joints.front() + S;
                        }
                    }
                    const int DY = Y % RowH;
                    const int Edge = std::min({X - Left, Right - X, DY, RowH - 1 - DY});
                    const double Tint = Hash01(83u, static_cast<std::uint32_t>(Row * 64 + Block % static_cast<int>(Joints.size()))) - 0.5;
                    const double Surf = N01(Fbm(X, Y, S, 16, 3, 84u));
                    const double Pit = N01(Noise(X, Y, S, 64, 85u));
                    double V = 0.50 + 0.10 * Tint + 0.12 * (Surf - 0.5) - 0.05 * SmoothStep(0.7, 0.9, Pit);
                    double H = 0.8 + 0.12 * (Surf - 0.5);
                    if (Edge < 2)
                    {
                        V = 0.58;  // lime mortar
                        H = 0.2;
                    }
                    else if (Edge < 4)
                    {
                        V *= 0.86;
                        H -= 0.25;
                    }
                    Out.Put(X, Y, V * 1.02, V * 0.97, V * 0.88, H);
                }
            }
            return Out;
        }

        Surface Sand()
        {
            Surface Out;
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    const double Grain = N01(Noise(X, Y, S, 128, 91u)) - 0.5;
                    const double Warp = N01(Noise(X, Y, S, 8, 92u));
                    const int Phase = (Y * 3 + static_cast<int>(Warp * 40.0)) % 32;
                    const double Ripple = Phase < 16 ? Phase / 16.0 : (32 - Phase) / 16.0;
                    const double Patch = N01(Fbm(X, Y, S, 4, 3, 93u)) - 0.5;
                    const bool Shell = Hash01(94u, static_cast<std::uint32_t>(Y * S + X)) > 0.9975;
                    double V = 1.0 + 0.06 * Grain - 0.025 * (1.0 - Ripple) + 0.06 * Patch;
                    if (Shell) V = 1.18;
                    Out.Put(X, Y, 0.79 * V, 0.70 * V, 0.53 * V, 0.35 + 0.35 * Ripple + 0.1 * Grain);
                }
            }
            return Out;
        }

        Surface Grass()
        {
            Surface Out;
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    const double Mix = N01(Fbm(X, Y, S, 4, 4, 101u));
                    const double Blades = N01(Noise((X * 4) % S, (Y * 16) % S, S, 64, 102u));
                    const double Dirt = SmoothStep(0.62, 0.78, Mix);
                    double R = Lerp(0.24, 0.35, Blades), G = Lerp(0.31, 0.40, Blades), B = Lerp(0.10, 0.15, Blades);
                    R = Lerp(R, 0.38, Dirt);
                    G = Lerp(G, 0.31, Dirt);
                    B = Lerp(B, 0.21, Dirt);
                    Out.Put(X, Y, R, G, B, 0.5 + 0.2 * Blades);
                }
            }
            return Out;
        }

        Surface Rock()
        {
            Surface Out;
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    const double Base = N01(Fbm(X, Y, S, 4, 5, 111u));
                    const double Ridge = 1.0 - std::fabs(2.0 * N01(Fbm(X, Y, S, 8, 3, 112u)) - 1.0);
                    const double Crack = SmoothStep(0.9, 0.98, Ridge);
                    const double Lichen = SmoothStep(0.78, 0.86, N01(Noise(X, Y, S, 16, 113u)));
                    double V = 0.34 + 0.16 * Base - 0.06 * Crack;
                    double R = V * 1.05, G = V, B = V * 0.92;
                    R = Lerp(R, 0.50, Lichen * 0.25);
                    G = Lerp(G, 0.48, Lichen * 0.25);
                    B = Lerp(B, 0.34, Lichen * 0.25);
                    Out.Put(X, Y, R, G, B, 0.6 * Base + 0.3 * (1.0 - Crack));
                }
            }
            return Out;
        }

        Surface Bark()
        {
            Surface Out;
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    const int T = Y % 22;
                    const double Band = T < 3 ? 0.55 : 0.85 + 0.15 * (T / 22.0);
                    const double Fibre = N01(Noise((X * 8) % S, Y, S, 32, 121u));
                    const double V = Band * (0.85 + 0.2 * Fibre);
                    Out.Put(X, Y, 0.44 * V, 0.37 * V, 0.29 * V, T < 3 ? 0.2 : 0.55 + 0.3 * T / 22.0);
                }
            }
            return Out;
        }

        Surface Frond()
        {
            Surface Out(4);
            for (int Y = 0; Y < S; ++Y)
            {
                const double T = Y / 255.0;                       // 0 tip .. 1 base
                const double Half = 118.0 * 4.0 * T * (1.0 - T) * (0.55 + 0.45 * (1.0 - T));
                for (int X = 0; X < S; ++X)
                {
                    const double Off = std::fabs(X - 127.5);
                    const double Diagonal = Y + Off * 0.9;
                    const double Frac = Diagonal - std::floor(Diagonal / 9.0) * 9.0;
                    const bool Rib = Off < 2.5 && T > 0.02;
                    const bool Leaf = Off < Half && Frac < 6.5 && Off > 1.5;
                    const double Edge = Half > 1.0 ? Off / Half : 1.0;
                    const double Shade = 0.8 + 0.2 * (1.0 - Frac / 6.5);
                    const double Dry = SmoothStep(0.75, 1.0, Edge) * 0.6 + SmoothStep(0.8, 1.0, N01(Noise(X, Y, S, 16, 131u))) * 0.4;
                    double R = Lerp(0.19, 0.50, Dry) * Shade, G = Lerp(0.33, 0.44, Dry) * Shade, B = Lerp(0.09, 0.20, Dry) * Shade;
                    if (Rib)
                    {
                        R = 0.46;
                        G = 0.43;
                        B = 0.24;
                    }
                    Out.Put(X, Y, R, G, B, 0.5, (Rib || Leaf) ? 1.0 : 0.0);
                }
            }
            return Out;
        }

        Surface Fabric()
        {
            Surface Out;
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    const double Weave = (((X / 2) ^ (Y / 2)) & 1) ? 0.025 : -0.025;
                    const double Fine = N01(Noise(X, Y, S, 64, 141u)) - 0.5;
                    const double Wear = SmoothStep(0.6, 0.85, N01(Fbm(X, Y, S, 4, 3, 142u)));
                    const double V = 0.80 + Weave + 0.05 * Fine + 0.06 * Wear;
                    Out.Put(X, Y, V, V, V, 0.5);
                }
            }
            return Out;
        }

        Surface Iron()
        {
            Surface Out;
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    const double Rust = SmoothStep(0.58, 0.75, N01(Fbm(X, Y, S, 4, 4, 151u)));
                    const double Pit = N01(Noise(X, Y, S, 64, 152u)) - 0.5;
                    const double R = Lerp(0.14, 0.38, Rust) + 0.02 * Pit, G = Lerp(0.14, 0.20, Rust) + 0.02 * Pit,
                                 B = Lerp(0.15, 0.10, Rust) + 0.02 * Pit;
                    Out.Put(X, Y, R, G, B, 0.5 + 0.1 * Pit);
                }
            }
            return Out;
        }
    }

    namespace
    {
        /** Ground texture from three mottled tones (Fbm patches) plus fine grain; Height for the normal map. */
        Surface Ground(const std::array<double, 3>& A, const std::array<double, 3>& B, const std::array<double, 3>& C, const std::uint32_t Seed,
            const int GrainPeriod, const double Speckle)
        {
            Surface Out;
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    const double P1 = N01(Fbm(X, Y, S, 4, 4, Seed));
                    const double P2 = N01(Fbm(X, Y, S, 8, 3, Seed + 1u));
                    const double Grain = N01(Noise(X, Y, S, GrainPeriod, Seed + 2u)) - 0.5;
                    const double Fine = N01(Noise(X, Y, S, 128, Seed + 3u)) - 0.5;
                    const double Mab = SmoothStep(0.35, 0.65, P1), Mc = SmoothStep(0.55, 0.8, P2);
                    double R = Lerp(Lerp(A[0], B[0], Mab), C[0], Mc), G = Lerp(Lerp(A[1], B[1], Mab), C[1], Mc), Bl = Lerp(Lerp(A[2], B[2], Mab), C[2], Mc);
                    const double V = 1.0 + 0.16 * Grain + Speckle * Fine;
                    Out.Put(X, Y, R * V, G * V, Bl * V, 0.45 + 0.25 * Grain + 0.15 * P2);
                }
            }
            return Out;
        }

        /** Heather: low woody tufts, purple-brown out of bloom with rust and fawn, dark gaps between plants. */
        Surface HeatherGround()
        {
            Surface Out;
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    const double Tuft = N01(Noise(X, Y, S, 32, 201u));
                    const double Fine = N01(Noise(X, Y, S, 128, 202u));
                    const double Patch = N01(Fbm(X, Y, S, 4, 3, 203u));
                    const double Gap = SmoothStep(0.25, 0.45, Tuft);
                    double R = Lerp(0.30, 0.42, Fine), G = Lerp(0.20, 0.27, Fine), B = Lerp(0.20, 0.22, Fine);   // purple-brown
                    R = Lerp(R, 0.50, SmoothStep(0.55, 0.8, Patch));                                               // rust
                    G = Lerp(G, 0.36, SmoothStep(0.55, 0.8, Patch));
                    B = Lerp(B, 0.20, SmoothStep(0.55, 0.8, Patch));
                    const double Shade = Lerp(0.45, 1.0, Gap);
                    Out.Put(X, Y, R * Shade, G * Shade, B * Shade, 0.3 + 0.5 * Gap * Tuft);
                }
            }
            return Out;
        }

        /**
         * Alpha card of a vegetation sprig: Count blades or leaves rising from the bottom edge (Kind 0 grass blades,
         * 1 broadleaf cluster, 2 pine needles on twigs, 3 fern frond, 4 reeds, 5 heather sprigs with tiny flowers).
         */
        Surface Sprig(const int Kind, const std::uint32_t Seed)
        {
            Surface Out(4);
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X) Out.Put(X, Y, 0.2, 0.25, 0.1, 0.5, 0.0);
            }
            Rng R(Seed);
            const auto Disc = [&](double Cx, double Cy, double Rx, double Ry, double Angle, double Cr, double Cg, double Cb)
            {
                const double Ca = Cos(Angle), Sa = Sin(Angle);
                const double Ext = std::max(Rx, Ry) + 1.0;
                for (int Y = static_cast<int>(Cy - Ext); Y <= static_cast<int>(Cy + Ext); ++Y)
                {
                    for (int X = static_cast<int>(Cx - Ext); X <= static_cast<int>(Cx + Ext); ++X)
                    {
                        if (X < 0 || Y < 0 || X >= S || Y >= S) continue;
                        const double Dx = X - Cx, Dy = Y - Cy;
                        const double U = (Dx * Ca + Dy * Sa) / Rx, V = (-Dx * Sa + Dy * Ca) / Ry;
                        const double D = U * U + V * V;
                        if (D > 1.0) continue;
                        const double Shade = 0.78 + 0.22 * (1.0 - D) + 0.1 * (V > 0 ? 0.0 : -V);
                        Out.Put(X, Y, Cr * Shade, Cg * Shade, Cb * Shade, 0.5, 1.0);
                    }
                }
            };
            const auto Line = [&](double X0, double Y0, double X1, double Y1, double W0, double W1, double Cr, double Cg, double Cb)
            {
                const int Steps = static_cast<int>(std::sqrt((X1 - X0) * (X1 - X0) + (Y1 - Y0) * (Y1 - Y0))) + 1;
                for (int K = 0; K <= Steps; ++K)
                {
                    const double T = static_cast<double>(K) / Steps;
                    Disc(Lerp(X0, X1, T), Lerp(Y0, Y1, T), Lerp(W0, W1, T), Lerp(W0, W1, T), 0.0, Cr * (0.8 + 0.2 * T), Cg * (0.8 + 0.2 * T), Cb * (0.8 + 0.2 * T));
                }
            };
            // texture row 0 is the top of the card, row 255 its foot
            if (Kind == 0 || Kind == 4 || Kind == 6)
            {
                const bool Golden = Kind == 6;
                const int Count = Kind == 4 ? 14 : 26;
                for (int I = 0; I < Count; ++I)
                {
                    const double Bx = 30 + R.Unit() * 196, Lean = (R.Unit() - 0.5) * (Kind != 4 ? 90 : 30);
                    const double Top = Kind != 4 ? 20 + R.Unit() * 120 : 4 + R.Unit() * 50;
                    const double Dry = Golden ? 0.75 + 0.25 * R.Unit() : R.Unit();
                    const double Cr = Kind != 4 ? Lerp(0.24, 0.66, Dry * Dry) : Lerp(0.42, 0.62, Dry);
                    const double Cg = Kind != 4 ? Lerp(0.36, 0.55, Dry) : Lerp(0.44, 0.55, Dry);
                    const double Cb = Kind != 4 ? Lerp(0.12, 0.30, Dry) : Lerp(0.22, 0.30, Dry);
                    Line(Bx, 255, Bx + Lean, Top, Kind != 4 ? 2.6 : 3.4, 0.6, Cr, Cg, Cb);
                    if (Kind == 4 && R.Unit() < 0.5) Disc(Bx + Lean, Top + 10, 3.5, 14, 0.0, 0.33, 0.24, 0.14);  // seed head
                }
            }
            else if (Kind == 1)
            {
                Line(128, 255, 128, 120, 4, 2, 0.32, 0.24, 0.16);
                for (int I = 0; I < 60; ++I)
                {
                    const double A = R.Unit() * 6.283, Rad = 30 + R.Unit() * 85;
                    const double Cx = 128 + Cos(A) * Rad, Cy = 118 + Sin(A) * Rad * 0.85;
                    const double Tone = R.Unit();
                    const double Long = 13 + R.Unit() * 6, Wide = 7 + R.Unit() * 3;  // in order: argument evaluation order differs between compilers
                    Disc(Cx, Cy, Long, Wide, A, Lerp(0.16, 0.30, Tone), Lerp(0.28, 0.42, Tone), Lerp(0.08, 0.14, Tone));
                }
            }
            else if (Kind == 2)
            {
                Line(128, 255, 128, 10, 3, 1.5, 0.30, 0.22, 0.15);
                for (int I = 0; I < 9; ++I)
                {
                    const double Y0 = 230 - I * 24, Side = (I % 2) ? 1.0 : -1.0;
                    Line(128, Y0, 128 + Side * (70 - I * 5), Y0 - 30, 2, 1, 0.30, 0.22, 0.15);
                    for (int K = 0; K < 90; ++K)
                    {
                        const double T = R.Unit();
                        const double Px = 128 + Side * (70 - I * 5) * T, Py = Y0 - 30 * T;
                        const double A = (R.Unit() - 0.5) * 2.4 - 1.57;
                        Line(Px, Py, Px + Cos(A) * 18, Py + Sin(A) * 18, 2.2, 1.0, 0.13, 0.24, 0.12);
                    }
                }
            }
            else if (Kind == 3)
            {
                Line(128, 255, 128, 8, 2.5, 1, 0.25, 0.32, 0.12);
                for (int I = 0; I < 22; ++I)
                {
                    const double Y0 = 240 - I * 10.5, L = 95 * (1.0 - I / 24.0) * (0.4 + 0.6 * Sin(3.1416 * (I + 2) / 25.0));
                    for (const double Side : {-1.0, 1.0}) Disc(128 + Side * L / 2, Y0 - 6, L / 2, 4.5, Side * 0.35, 0.20, 0.38, 0.12);
                }
            }
            else
            {
                for (int I = 0; I < 12; ++I)
                {
                    const double Bx = 40 + R.Unit() * 176, Lean = (R.Unit() - 0.5) * 70, Top = 60 + R.Unit() * 120;
                    Line(Bx, 255, Bx + Lean, Top, 3, 1.2, 0.33, 0.27, 0.20);
                    for (int K = 0; K < 26; ++K)
                    {
                        const double T = 0.15 + R.Unit() * 0.85;
                        const bool Bloom = R.Unit() < 0.35;
                        Disc(Lerp(Bx, Bx + Lean, T) + (R.Unit() - 0.5) * 12, Lerp(255, Top, T), 3.5, 3.5, 0.0, Bloom ? 0.52 : 0.30, Bloom ? 0.30 : 0.33, Bloom ? 0.48 : 0.17);
                    }
                }
            }
            return Out;
        }
    }

        /** Birch bark: chalk white with dark horizontal lenticels and black patches near the foot of the texture. */
        Surface BirchBark()
        {
            Surface Out;
            for (int Y = 0; Y < S; ++Y)
            {
                for (int X = 0; X < S; ++X)
                {
                    const double Fine = N01(Noise(X, Y, S, 64, 281u)) - 0.5;
                    const double Band = N01(Noise((X * 1) % S, (Y * 8) % S, S, 16, 282u));
                    const bool Lenticel = Band > 0.72 && N01(Noise((X * 4) % S, Y, S, 32, 283u)) > 0.55;
                    const double Patch = SmoothStep(0.68, 0.8, N01(Fbm(X, Y, S, 4, 3, 284u)));
                    double V = 0.86 + 0.06 * Fine;
                    if (Lenticel) V = 0.22;
                    V = Lerp(V, 0.16, Patch * 0.85);
                    Out.Put(X, Y, V, V * 0.98, V * 0.94, Lenticel ? 0.2 : 0.6 - 0.3 * Patch);
                }
            }
            return Out;
        }
    std::vector<ArtFile> BuildTextures()
    {
        std::vector<ArtFile> Files;
        const auto Add = [&Files](const std::string& Name, const Surface& Surf, int NormalStrength)
        {
            Files.push_back({"Assets/Art/Textures/" + Name + "_BaseColor.png", EncodePng(Surf.Color)});
            if (NormalStrength > 0)
            {
                Files.push_back({"Assets/Art/Textures/" + Name + "_Normal.png", EncodePng(NormalFromHeight(Surf.Height, S, NormalStrength))});
            }
        };
        Add("T_WoodDeck", WoodDeck(), 260);
        Add("T_HullPlanks", HullPlanks(), 240);
        Add("T_Timber", Timber(), 0);
        Add("T_Canvas", Canvas(), 0);
        Add("T_Rope", Rope(), 0);
        Add("T_Plaster", Plaster(), 150);
        Add("T_RoofTile", RoofTiles(), 260);
        Add("T_Stone", Stone(), 300);
        Add("T_Sand", Sand(), 55);
        Add("T_Grass", Grass(), 0);
        Add("T_Rock", Rock(), 320);
        Add("T_Bark", Bark(), 200);
        Add("T_Frond", Frond(), 0);
        Add("T_Fabric", Fabric(), 0);
        Add("T_Iron", Iron(), 0);
        // terrain and vegetation
        Add("T_Heather", HeatherGround(), 120);
        Add("T_DryGrass", Ground({0.55, 0.47, 0.28}, {0.47, 0.42, 0.25}, {0.62, 0.55, 0.36}, 211u, 64, 0.1), 60);
        Add("T_Dirt", Ground({0.42, 0.34, 0.25}, {0.36, 0.29, 0.21}, {0.50, 0.43, 0.33}, 221u, 96, 0.14), 160);
        Add("T_Mud", Ground({0.22, 0.19, 0.15}, {0.17, 0.15, 0.12}, {0.28, 0.25, 0.18}, 231u, 64, 0.06), 90);
        Add("T_RedEarth", Ground({0.52, 0.30, 0.19}, {0.44, 0.26, 0.17}, {0.60, 0.41, 0.27}, 241u, 96, 0.12), 140);
        Add("T_Moss", Ground({0.22, 0.30, 0.12}, {0.18, 0.25, 0.10}, {0.30, 0.34, 0.16}, 251u, 64, 0.12), 70);
        Add("T_Ash", Ground({0.30, 0.29, 0.28}, {0.22, 0.21, 0.21}, {0.40, 0.39, 0.37}, 261u, 128, 0.1), 80);
        Add("T_SprigGrass", Sprig(0, 271u), 0);
        Add("T_SprigLeaves", Sprig(1, 272u), 0);
        Add("T_SprigNeedles", Sprig(2, 273u), 0);
        Add("T_SprigFern", Sprig(3, 274u), 0);
        Add("T_SprigReeds", Sprig(4, 275u), 0);
        Add("T_SprigHeather", Sprig(5, 276u), 0);
        Add("T_SprigDryGrass", Sprig(6, 277u), 0);
        Add("T_BirchBark", BirchBark(), 160);
        return Files;
    }

    const Material& Mat(const M Id)
    {
        static const std::vector<Material> Table = []
        {
            std::vector<Material> T(static_cast<std::size_t>(M::Count));
            const auto Set = [&T](M Id, const char* Name, const char* Texture, bool Normal, std::array<double, 4> Color, double Rough,
                                 double Tile, double Metal = 0.0, bool Mask = false, bool TwoSided = false)
            {
                Material& Mt = T[static_cast<std::size_t>(Id)];
                Mt.Name = Name;
                if (Texture)
                {
                    Mt.BaseColorTexture = std::string(Texture) + "_BaseColor.png";
                    if (Normal) Mt.NormalTexture = std::string(Texture) + "_Normal.png";
                }
                Mt.BaseColor = Color;
                Mt.Roughness = Rough;
                Mt.MetresPerTile = Tile;
                Mt.Metallic = Metal;
                Mt.Mask = Mask;
                Mt.DoubleSided = TwoSided || Mask;
            };
            Set(M::Deck, "M_Art_Deck", "T_WoodDeck", true, {1, 1, 1, 1}, 0.8, 2.0);
            Set(M::DeckWorn, "M_Art_DeckWorn", "T_WoodDeck", true, {0.86, 0.87, 0.88, 1}, 0.85, 2.0);
            Set(M::Hull, "M_Art_Hull", "T_HullPlanks", true, {0.58, 0.4, 0.27, 1}, 0.72, 2.5);
            Set(M::HullBlack, "M_Art_HullBlack", "T_HullPlanks", true, {0.22, 0.2, 0.19, 1}, 0.62, 2.0);
            Set(M::HullBelow, "M_Art_HullBelow", "T_HullPlanks", true, {0.7, 0.42, 0.3, 1}, 0.8, 2.0);
            Set(M::HullBand, "M_Art_HullBand", "T_HullPlanks", true, {1.0, 0.86, 0.45, 1}, 0.6, 2.0);
            Set(M::HullBandRed, "M_Art_HullBandRed", "T_HullPlanks", true, {0.8, 0.2, 0.13, 1}, 0.6, 2.0);
            Set(M::Timber, "M_Art_Timber", "T_Timber", false, {0.95, 0.7, 0.46, 1}, 0.78, 1.5);
            Set(M::TimberDark, "M_Art_TimberDark", "T_Timber", false, {0.52, 0.37, 0.26, 1}, 0.75, 1.5);
            Set(M::Driftwood, "M_Art_Driftwood", "T_Timber", false, {1.0, 0.97, 0.9, 1}, 0.92, 1.5);
            Set(M::Canvas, "M_Art_Canvas", "T_Canvas", false, {1, 1, 1, 1}, 0.92, 3.0, 0.0, false, true);
            Set(M::CanvasBlack, "M_Art_CanvasBlack", "T_Canvas", false, {0.17, 0.16, 0.15, 1}, 0.9, 3.0, 0.0, false, true);
            Set(M::Rope, "M_Art_Rope", "T_Rope", false, {1, 1, 1, 1}, 0.95, 0.35);
            Set(M::Iron, "M_Art_Iron", "T_Iron", false, {1, 1, 1, 1}, 0.62, 1.0, 0.7);
            Set(M::Brass, "M_Art_Brass", nullptr, false, {0.72, 0.56, 0.28, 1}, 0.35, 1.0, 1.0);
            Set(M::Glass, "M_Art_Glass", nullptr, false, {0.05, 0.07, 0.08, 1}, 0.12, 1.0, 0.0);
            Set(M::Plaster, "M_Art_Plaster", "T_Plaster", true, {1, 1, 1, 1}, 0.9, 2.5);
            Set(M::PlasterOchre, "M_Art_PlasterOchre", "T_Plaster", true, {0.96, 0.79, 0.54, 1}, 0.9, 2.5);
            Set(M::PlasterBlue, "M_Art_PlasterBlue", "T_Plaster", true, {0.66, 0.75, 0.84, 1}, 0.9, 2.5);
            Set(M::RoofTile, "M_Art_RoofTile", "T_RoofTile", true, {1, 1, 1, 1}, 0.75, 2.0);
            Set(M::Stone, "M_Art_Stone", "T_Stone", true, {1, 1, 1, 1}, 0.85, 2.5);
            Set(M::StoneDark, "M_Art_StoneDark", "T_Stone", true, {0.62, 0.60, 0.58, 1}, 0.85, 2.5);
            Set(M::Paving, "M_Art_Paving", "T_Stone", true, {0.93, 0.9, 0.84, 1}, 0.8, 2.0);
            Set(M::Sand, "M_Art_Sand", "T_Sand", true, {1, 1, 1, 1}, 0.92, 3.0);
            Set(M::WetSand, "M_Art_WetSand", "T_Sand", true, {0.62, 0.56, 0.47, 1}, 0.38, 3.0);
            Set(M::Grass, "M_Art_Grass", "T_Grass", false, {1, 1, 1, 1}, 0.9, 3.0);
            Set(M::Rock, "M_Art_Rock", "T_Rock", true, {1, 1, 1, 1}, 0.86, 3.0);
            Set(M::Bark, "M_Art_Bark", "T_Bark", true, {1, 1, 1, 1}, 0.88, 1.2);
            Set(M::Frond, "M_Art_Frond", "T_Frond", false, {1, 1, 1, 1}, 0.7, 1.0, 0.0, true);
            Set(M::Coral, "M_Art_Coral", "T_Sand", true, {0.95, 0.72, 0.64, 1}, 0.8, 1.0);
            Set(M::CoralRed, "M_Art_CoralRed", "T_Sand", true, {0.92, 0.46, 0.38, 1}, 0.8, 1.0);
            Set(M::Kelp, "M_Art_Kelp", "T_Frond", false, {0.62, 0.55, 0.28, 1}, 0.7, 1.0, 0.0, true);
            Set(M::HarborWater, "M_Art_HarborWater", nullptr, false, {0.1, 0.2, 0.22, 1}, 0.08, 1.0);
            Set(M::Linen, "M_Art_Linen", "T_Fabric", false, {1.0, 0.97, 0.86, 1}, 0.9, 0.6);
            Set(M::WoolBlue, "M_Art_WoolBlue", "T_Fabric", false, {0.26, 0.32, 0.48, 1}, 0.92, 0.6);
            Set(M::WoolBrown, "M_Art_WoolBrown", "T_Fabric", false, {0.52, 0.36, 0.23, 1}, 0.92, 0.6);
            Set(M::WoolGrey, "M_Art_WoolGrey", "T_Fabric", false, {0.5, 0.5, 0.52, 1}, 0.92, 0.6);
            Set(M::WoolBlack, "M_Art_WoolBlack", "T_Fabric", false, {0.16, 0.16, 0.17, 1}, 0.9, 0.6);
            Set(M::Sash, "M_Art_Sash", "T_Fabric", false, {0.76, 0.15, 0.1, 1}, 0.85, 0.6);
            Set(M::Ochre, "M_Art_Ochre", "T_Fabric", false, {0.86, 0.63, 0.28, 1}, 0.9, 0.6);
            Set(M::DressGreen, "M_Art_DressGreen", "T_Fabric", false, {0.36, 0.48, 0.37, 1}, 0.9, 0.6);
            Set(M::Leather, "M_Art_Leather", "T_Fabric", false, {0.45, 0.28, 0.16, 1}, 0.55, 0.6);
            Set(M::Skin, "M_Art_Skin", nullptr, false, {0.8, 0.6, 0.47, 1}, 0.55, 1.0);
            Set(M::SkinDark, "M_Art_SkinDark", nullptr, false, {0.5, 0.34, 0.24, 1}, 0.55, 1.0);
            Set(M::Hair, "M_Art_Hair", nullptr, false, {0.24, 0.16, 0.1, 1}, 0.6, 1.0);
            Set(M::HairGrey, "M_Art_HairGrey", nullptr, false, {0.50, 0.48, 0.45, 1}, 0.6, 1.0);
            Set(M::Heather, "M_Art_Heather", "T_Heather", true, {1, 1, 1, 1}, 0.95, 3.0);
            Set(M::DryGrass, "M_Art_DryGrass", "T_DryGrass", true, {1, 1, 1, 1}, 0.93, 3.5);
            Set(M::Dirt, "M_Art_Dirt", "T_Dirt", true, {1, 1, 1, 1}, 0.9, 3.0);
            Set(M::Mud, "M_Art_Mud", "T_Mud", true, {1, 1, 1, 1}, 0.45, 3.0);
            Set(M::RedEarth, "M_Art_RedEarth", "T_RedEarth", true, {1, 1, 1, 1}, 0.9, 3.0);
            Set(M::Moss, "M_Art_Moss", "T_Moss", true, {1, 1, 1, 1}, 0.92, 2.5);
            Set(M::Ash, "M_Art_Ash", "T_Ash", true, {1, 1, 1, 1}, 0.95, 3.0);
            Set(M::Leaves, "M_Art_Leaves", "T_SprigLeaves", false, {1, 1, 1, 1}, 0.75, 1.0, 0.0, true);
            Set(M::LeavesDark, "M_Art_LeavesDark", "T_SprigLeaves", false, {0.62, 0.7, 0.6, 1}, 0.75, 1.0, 0.0, true);
            Set(M::LeavesPale, "M_Art_LeavesPale", "T_SprigLeaves", false, {1.0, 1.0, 0.72, 1}, 0.75, 1.0, 0.0, true);
            Set(M::Needles, "M_Art_Needles", "T_SprigNeedles", false, {1, 1, 1, 1}, 0.8, 1.0, 0.0, true);
            Set(M::GrassBlades, "M_Art_GrassBlades", "T_SprigGrass", false, {1, 1, 1, 1}, 0.85, 1.0, 0.0, true);
            Set(M::GrassDry, "M_Art_GrassDry", "T_SprigDryGrass", false, {1, 1, 1, 1}, 0.9, 1.0, 0.0, true);
            Set(M::HeatherSprig, "M_Art_HeatherSprig", "T_SprigHeather", false, {1, 1, 1, 1}, 0.9, 1.0, 0.0, true);
            Set(M::Fern, "M_Art_Fern", "T_SprigFern", false, {1, 1, 1, 1}, 0.8, 1.0, 0.0, true);
            Set(M::Reeds, "M_Art_Reeds", "T_SprigReeds", false, {1, 1, 1, 1}, 0.85, 1.0, 0.0, true);
            Set(M::Gorse, "M_Art_Gorse", "T_SprigNeedles", false, {0.75, 0.95, 0.6, 1}, 0.85, 1.0, 0.0, true);
            Set(M::BirchBark, "M_Art_BirchBark", "T_BirchBark", true, {1, 1, 1, 1}, 0.8, 1.2);
            Set(M::Lichen, "M_Art_Lichen", "T_Rock", true, {0.85, 0.88, 0.72, 1}, 0.9, 2.0);
            Set(M::PondWater, "M_Art_PondWater", nullptr, false, {0.05, 0.07, 0.06, 1}, 0.05, 1.0);
            return T;
        }();
        return Table[static_cast<std::size_t>(Id)];
    }
}
