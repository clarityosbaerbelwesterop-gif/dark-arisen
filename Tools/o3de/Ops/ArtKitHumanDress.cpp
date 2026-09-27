// Human pipeline, part 2: dressing and grooming (clothes cut from the body surface, layered hair
// and beard shells, skirts, belts, hats and gear), texture baking into the MakeHuman UV layout,
// and the factory that turns a cast entry into a posed static figure.

#include "ArtKitHumanInternal.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <set>

namespace DarkArisen::Tools::Art::Humans
{
    namespace
    {
        // ------------------------------------------------------------------ noise and colour

        std::uint32_t Mix(std::uint32_t H)
        {
            H ^= H >> 16;
            H *= 0x7feb352du;
            H ^= H >> 15;
            H *= 0x846ca68bu;
            H ^= H >> 16;
            return H;
        }
        double Hash3(const int X, const int Y, const int Z, const std::uint32_t Seed)
        {
            const std::uint32_t H = Mix(static_cast<std::uint32_t>(X) * 0x8da6b343u ^ Mix(static_cast<std::uint32_t>(Y) * 0xd8163841u ^
                Mix(static_cast<std::uint32_t>(Z) * 0xcb1ab31fu ^ Seed * 0x9e3779b9u)));
            return static_cast<double>(H >> 8) / 16777216.0;
        }
        /** Value noise in [0, 1] (C1 smooth). */
        double Noise3(const V3& P, const std::uint32_t Seed)
        {
            const double Fx = std::floor(P.X), Fy = std::floor(P.Y), Fz = std::floor(P.Z);
            const int X = static_cast<int>(Fx), Y = static_cast<int>(Fy), Z = static_cast<int>(Fz);
            const double Tx = P.X - Fx, Ty = P.Y - Fy, Tz = P.Z - Fz;
            const double U = Tx * Tx * (3.0 - 2.0 * Tx), V = Ty * Ty * (3.0 - 2.0 * Ty), W = Tz * Tz * (3.0 - 2.0 * Tz);
            const double C000 = Hash3(X, Y, Z, Seed), C100 = Hash3(X + 1, Y, Z, Seed), C010 = Hash3(X, Y + 1, Z, Seed), C110 = Hash3(X + 1, Y + 1, Z, Seed);
            const double C001 = Hash3(X, Y, Z + 1, Seed), C101 = Hash3(X + 1, Y, Z + 1, Seed), C011 = Hash3(X, Y + 1, Z + 1, Seed), C111 = Hash3(X + 1, Y + 1, Z + 1, Seed);
            const double A = Lerp(Lerp(C000, C100, U), Lerp(C010, C110, U), V);
            const double B = Lerp(Lerp(C001, C101, U), Lerp(C011, C111, U), V);
            return Lerp(A, B, W);
        }
        double Fbm3(const V3& P, const int Octaves, const std::uint32_t Seed)
        {
            double Sum = 0, Amp = 0.5, Norm = 0, F = 1;
            for (int O = 0; O < Octaves; ++O)
            {
                Sum += Noise3(P * F, Seed + static_cast<std::uint32_t>(O) * 7919u) * Amp;
                Norm += Amp;
                Amp *= 0.5;
                F *= 2.0;
            }
            return Sum / Norm;
        }
        double Frac(const double X) { return X - std::floor(X); }

        Rgb operator*(const Rgb& C, const double S) { return {C.R * S, C.G * S, C.B * S}; }
        Rgb operator*(const Rgb& A, const Rgb& B) { return {A.R * B.R, A.G * B.G, A.B * B.B}; }
        Rgb MixRgb(const Rgb& A, const Rgb& B, const double T) { return {Lerp(A.R, B.R, T), Lerp(A.G, B.G, T), Lerp(A.B, B.B, T)}; }
        double Luma(const Rgb& C) { return 0.3 * C.R + 0.59 * C.G + 0.11 * C.B; }
        Rgb Desaturate(const Rgb& C, const double T) { const double L = Luma(C); return MixRgb(C, {L, L, L}, T); }

        std::uint8_t Byte(const double V) { return static_cast<std::uint8_t>(std::lround(Clamp(V, 0.0, 1.0) * 255.0)); }

        // ------------------------------------------------------------------ texture baking

        struct Canvas
        {
            Image Img;
            std::vector<std::uint8_t> Written;
            Canvas(const int W, const int H, const int Channels)
            {
                Img.Width = W;
                Img.Height = H;
                Img.Channels = Channels;
                Img.Pixels.assign(static_cast<std::size_t>(W * H * Channels), 0);
                Written.assign(static_cast<std::size_t>(W * H), 0);
            }
            void Put(const int X, const int Y, const Rgb& C, const double Alpha)
            {
                const auto I = static_cast<std::size_t>(Y * Img.Width + X);
                std::uint8_t* Px = &Img.Pixels[I * static_cast<std::size_t>(Img.Channels)];
                Px[0] = Byte(C.R);
                Px[1] = Byte(C.G);
                Px[2] = Byte(C.B);
                if (Img.Channels == 4) Px[3] = Byte(Alpha);
                Written[I] = 1;
            }
            /** Grows written texels outwards so filtering never reaches unwritten black. */
            void Dilate(const int Passes)
            {
                const int W = Img.Width, H = Img.Height, C = Img.Channels;
                for (int Pass = 0; Pass < Passes; ++Pass)
                {
                    const std::vector<std::uint8_t> Src = Img.Pixels;
                    const std::vector<std::uint8_t> Was = Written;
                    for (int Y = 0; Y < H; ++Y)
                    {
                        for (int X = 0; X < W; ++X)
                        {
                            const auto I = static_cast<std::size_t>(Y * W + X);
                            if (Was[I]) continue;
                            int Sum[4] = {0, 0, 0, 0}, Count = 0;
                            for (int Dy = -1; Dy <= 1; ++Dy)
                            {
                                for (int Dx = -1; Dx <= 1; ++Dx)
                                {
                                    const int Nx = X + Dx, Ny = Y + Dy;
                                    if (Nx < 0 || Ny < 0 || Nx >= W || Ny >= H) continue;
                                    const auto J = static_cast<std::size_t>(Ny * W + Nx);
                                    if (!Was[J]) continue;
                                    for (int K = 0; K < C; ++K) Sum[K] += Src[J * static_cast<std::size_t>(C) + static_cast<std::size_t>(K)];
                                    ++Count;
                                }
                            }
                            if (!Count) continue;
                            for (int K = 0; K < C; ++K) Img.Pixels[I * static_cast<std::size_t>(C) + static_cast<std::size_t>(K)] = static_cast<std::uint8_t>((Sum[K] + Count / 2) / Count);
                            Written[I] = 1;
                        }
                    }
                }
            }
        };

        /** Calls Shade(W0, W1, W2) for every texel centre inside the UV triangle. */
        template <class ShadeFn>
        void Raster(Canvas& Target, const std::array<double, 2>& A, const std::array<double, 2>& B, const std::array<double, 2>& C, ShadeFn&& Shade)
        {
            const double W = Target.Img.Width, H = Target.Img.Height;
            const double X0 = A[0] * W - 0.5, Y0 = A[1] * H - 0.5, X1 = B[0] * W - 0.5, Y1 = B[1] * H - 0.5, X2 = C[0] * W - 0.5, Y2 = C[1] * H - 0.5;
            const double Area = (X1 - X0) * (Y2 - Y0) - (X2 - X0) * (Y1 - Y0);
            if (std::abs(Area) < 1e-12) return;
            const int MinX = std::max(0, static_cast<int>(std::floor(std::min({X0, X1, X2}))));
            const int MaxX = std::min(Target.Img.Width - 1, static_cast<int>(std::ceil(std::max({X0, X1, X2}))));
            const int MinY = std::max(0, static_cast<int>(std::floor(std::min({Y0, Y1, Y2}))));
            const int MaxY = std::min(Target.Img.Height - 1, static_cast<int>(std::ceil(std::max({Y0, Y1, Y2}))));
            for (int Y = MinY; Y <= MaxY; ++Y)
            {
                for (int X = MinX; X <= MaxX; ++X)
                {
                    const double W0 = ((X1 - X) * (Y2 - Y) - (X2 - X) * (Y1 - Y)) / Area;
                    const double W1 = ((X2 - X) * (Y0 - Y) - (X0 - X) * (Y2 - Y)) / Area;
                    const double W2 = 1.0 - W0 - W1;
                    if (W0 < -1e-9 || W1 < -1e-9 || W2 < -1e-9) continue;
                    Shade(X, Y, W0, W1, W2);
                }
            }
        }

        // ------------------------------------------------------------------ coverage

        enum Slot { Stockings, Shirt, Legs, Feet, Vest, Coat, HeadCloth, SlotCount };

        /** Everything the shaders need per body vertex (interpolated per texel). */
        struct Attr
        {
            V3 P, N;
            std::array<double, SlotCount> S{};
            double Hair = -9, Beard = -9, Hidden = -9;  // hidden: under a long skirt
            double ArmT = 0, LegT = 0, Cavity = 0, Head = 0, Hand = 0, Leg = 0;
        };
        Attr Blend(const Attr& A, const Attr& B, const Attr& C, const double Wa, const double Wb, const double Wc)
        {
            Attr O;
            O.P = A.P * Wa + B.P * Wb + C.P * Wc;
            O.N = Normalize(A.N * Wa + B.N * Wb + C.N * Wc);
            for (std::size_t K = 0; K < O.S.size(); ++K) O.S[K] = A.S[K] * Wa + B.S[K] * Wb + C.S[K] * Wc;
            O.Hair = A.Hair * Wa + B.Hair * Wb + C.Hair * Wc;
            O.Beard = A.Beard * Wa + B.Beard * Wb + C.Beard * Wc;
            O.Hidden = A.Hidden * Wa + B.Hidden * Wb + C.Hidden * Wc;
            O.ArmT = A.ArmT * Wa + B.ArmT * Wb + C.ArmT * Wc;
            O.LegT = A.LegT * Wa + B.LegT * Wb + C.LegT * Wc;
            O.Cavity = A.Cavity * Wa + B.Cavity * Wb + C.Cavity * Wc;
            O.Head = A.Head * Wa + B.Head * Wb + C.Head * Wc;
            O.Hand = A.Hand * Wa + B.Hand * Wb + C.Hand * Wc;
            O.Leg = A.Leg * Wa + B.Leg * Wb + C.Leg * Wc;
            return O;
        }

        const Garment* SlotGarment(const PersonSpec& S, const int K)
        {
            switch (K)
            {
            case Stockings: return &S.Stockings;
            case Shirt: return &S.Shirt;
            case Legs: return &S.Legs;
            case Feet: return &S.Feet;
            case Vest: return &S.Vest;
            case Coat: return &S.Coat;
            default: return nullptr;
            }
        }

        /** Height of the leg at leg parameter T (0 hip, 1 knee, 2 ankle). */
        double LegY(const Body& B, const double T)
        {
            const double Hip = (B.Hip[0].Y + B.Hip[1].Y) * 0.5, Knee = (B.Knee[0].Y + B.Knee[1].Y) * 0.5, Ankle = (B.Ankle[0].Y + B.Ankle[1].Y) * 0.5;
            if (T <= 1.0) return Lerp(Hip, Knee, T);
            if (T <= 2.0) return Lerp(Knee, Ankle, T - 1.0);
            return Lerp(Ankle, 0.0, std::min(T - 2.0, 1.0));
        }

        /** Front-ness (-1 back .. +1 front) around the torso axis at P. */
        double Frontness(const Body& B, const V3& P)
        {
            const double Zc = Lerp((B.Hip[0].Z + B.Hip[1].Z) * 0.5, B.Shoulder[0].Z * 0.5 + B.Shoulder[1].Z * 0.5, Clamp(B.TorsoF(P.Y), 0.0, 1.0));
            const double Dx = P.X, Dz = P.Z - Zc;
            const double L = std::sqrt(Dx * Dx + Dz * Dz);
            return L > 0 ? Dz / L : 0.0;
        }

        double UpperCut(const Garment& G, const Body& B, const V3& P, const Anatomy& A)
        {
            const double C = Frontness(B, P);
            const double Dip = G.NeckFront * std::max(0.0, 1.0 - std::abs(P.X) / (0.35 + 0.6 * G.NeckFront)) * SmoothStep(0.0, 0.6, C);
            const double Neck = (B.NeckY + 0.12 - Dip) - P.Y;
            const double Hem = P.Y - B.TorsoY(G.Hem);
            double Torso = std::min(Neck, Hem);
            if (G.Open > 0.0 && C > 0.0)
            {
                const double F = B.TorsoF(P.Y);
                const double Width = G.Open * (1.0 + 1.2 * std::max(0.0, 0.55 - F)) + 0.25 * std::max(0.0, F - 0.8);
                Torso = std::min(Torso, (std::abs(P.X) - Width) + (1.0 - SmoothStep(0.0, 0.5, C)) * 2.0);
            }
            const double Arm = (G.Sleeve - A.ArmT) * 2.5;
            return (A.Head + A.Neck + A.Torso) * Torso + (A.Arm + A.Hand) * Arm + A.Leg * Hem + A.Foot * -2.0;
        }
        double LowerCut(const Garment& G, const Body& B, const V3& P, const Anatomy& A)
        {
            const double Torso = B.TorsoY(G.Waist) - P.Y;
            const double Leg = (G.LegEnd - A.LegT) * 2.5;
            return (A.Head + A.Neck + A.Torso) * Torso + (A.Leg + A.Foot) * Leg + (A.Arm + A.Hand) * -2.0;
        }
        double LegwearCut(const Garment& G, const Anatomy& A, const bool Footwear)
        {
            const double Leg = Footwear ? (A.LegT - G.LegStart) * 2.5 : std::min(A.LegT - G.LegStart, 3.5 - A.LegT) * 2.5;
            return (A.Leg + A.Foot) * Leg + (A.Head + A.Neck + A.Torso + A.Arm + A.Hand) * -2.0;
        }

        /** Azimuth around the head's vertical axis: 0 at the face, +pi/2 at the left ear. */
        double HeadAzimuth(const Body& B, const V3& P) { return Atan2(P.X - B.HeadCentre.X, P.Z - B.HeadCentre.Z); }

        /** Hairline: positive above it. Front, sides and nape heights relative to the eyes. */
        double HairlineCut(const Body& B, const V3& P, const double Front, const double Side, const double Back)
        {
            const double Theta = HeadAzimuth(B, P);
            const double C = Cos(Theta), C2 = Cos(2.0 * Theta);
            // Front = a0 + a1 + a2, Side = a0 - a2, Back = a0 - a1 + a2.
            const double A1 = (Front - Back) * 0.5;
            const double A0 = (Front + Back) * 0.25 + Side * 0.5;
            const double A2 = A0 - Side;
            const double Line = B.EyeMid.Y + A0 + A1 * C + A2 * C2;
            // The outer ears stay clear.
            const V3& EarAt = P.X >= B.HeadCentre.X ? B.Ear[0] : B.Ear[1];
            const double Ear = 1.0 - SmoothStep(0.22, 0.34, Length(P - EarAt));
            return P.Y - Line - Ear * 0.8;
        }

        double HairCut(const PersonSpec& S, const Body& B, const V3& P, const Anatomy& A)
        {
            double Front = 0.62, Side = 0.1, Back = -0.95;
            switch (S.Hairdo)
            {
            case HairStyle::Bald: return -9.0;
            case HairStyle::Shaved: Front = 0.6; Side = 0.15; Back = -0.85; break;
            case HairStyle::Cropped: Front = 0.6; Side = 0.12; Back = -0.8; break;
            case HairStyle::Tonsure: Front = 0.62; Side = 0.05; Back = -0.9; break;
            case HairStyle::Tousled: Front = 0.45; Side = 0.02; Back = -1.0; break;
            case HairStyle::Braid: case HairStyle::PinnedUp: case HairStyle::LongLoose: Front = 0.58; Side = 0.0; Back = -1.05; break;
            default: break;
            }
            if (S.Age > 50.0 && S.Male > 0.5) Front += 0.12 * Clamp((S.Age - 50.0) / 25.0, 0.0, 1.0);
            double Cut = HairlineCut(B, P, Front, Side, Back);
            if (S.Hairdo == HairStyle::Tonsure)
            {
                const double Crown = Length(V3{P.X - B.HeadCentre.X, 0, P.Z - (B.HeadCentre.Z - 0.2)});
                Cut = std::min(Cut, (Crown - 0.55) + (B.HeadTop.Y - 0.9 > P.Y ? 9.0 : 0.0));
            }
            const double Region = A.Head + A.Neck * 0.5;
            return Region * Cut + (1.0 - Region) * -2.0;
        }

        /** Beard area (positive inside): jaw, chin, upper lip; lips and cheeks above the line stay clear. */
        double BeardCut(const PersonSpec& S, const Body& B, const V3& P, const Anatomy& A)
        {
            if (S.Beard == BeardStyle::None) return -9.0;
            const V3 F = P - B.EyeMid;
            const double Front = P.Z - B.HeadCentre.Z;
            const double Lateral = std::abs(F.X);
            // Cheek line from the sideburn (level with the eyes) to the mouth corner.
            const double MouthY = B.Mouth.Y - B.EyeMid.Y;
            const double Sideburn = S.Beard == BeardStyle::Full ? -0.12 : S.Beard == BeardStyle::Trimmed ? -0.3 : -0.2;
            const double CheekLine = Lerp(Sideburn, MouthY + (S.Beard == BeardStyle::Full ? 0.18 : 0.08), SmoothStep(0.62, 0.2, Lateral));
            double Cut = CheekLine - F.Y;
            // Throat: down to a little below the jaw line.
            const double Throat = P.Y - (B.Chin.Y - (S.Beard == BeardStyle::Full ? 0.55 : 0.35));
            Cut = std::min(Cut, Throat);
            // Behind the ear stays clear.
            Cut = std::min(Cut, (Front + 0.05) * 2.0);
            // Lips.
            const double Lx = (P.X - B.Mouth.X) / std::max(0.05, std::abs(B.MouthL.X - B.Mouth.X));
            const double Ly = (P.Y - B.Mouth.Y) / 0.13;
            const double Lip = 1.0 - (Lx * Lx + Ly * Ly);
            Cut = std::min(Cut, -Lip * 0.25 + (P.Z < B.Mouth.Z - 0.4 ? 0.3 : 0.0));
            if (S.Beard == BeardStyle::Mustache)
            {
                // Upper lip only.
                const double Upper = std::min(F.Y - (MouthY + 0.02), (MouthY + 0.28) - F.Y);
                Cut = std::min(Cut, Upper * 2.0);
                Cut = std::min(Cut, 0.33 - std::abs(P.X - B.Mouth.X));
            }
            const double Region = A.Head + A.Neck;
            return Region * Cut + (1.0 - Region) * -2.0;
        }

        double HeadClothCut(const PersonSpec& S, const Body& B, const V3& P, const Anatomy& A)
        {
            double Cut;
            switch (S.Hat)
            {
            case HatStyle::KnitCap: Cut = HairlineCut(B, P, 0.5, 0.3, -0.25); break;
            case HatStyle::Headscarf: Cut = HairlineCut(B, P, 0.48, 0.12, -0.7); break;
            case HatStyle::Headwrap: Cut = HairlineCut(B, P, 0.42, 0.2, -0.6); break;
            default: return -9.0;
            }
            const double Region = A.Head + A.Neck * 0.5;
            return Region * Cut + (1.0 - Region) * -2.0;
        }

        // ------------------------------------------------------------------ shading

        Rgb FabricShade(const Garment& G, const Attr& T, const std::uint32_t Seed, const double Edge)
        {
            Rgb C = G.Color;
            const V3 P = T.P;
            switch (G.Fabric)
            {
            case Cloth::Linen:
                C = C * (0.95 + 0.07 * Noise3(P * 22.0, Seed) + 0.05 * Noise3(V3{P.X * 3.0, P.Y * 90.0, P.Z * 3.0}, Seed + 1));
                break;
            case Cloth::Wool:
                C = C * (0.9 + 0.14 * Fbm3(P * 9.0, 3, Seed));
                break;
            case Cloth::Cotton:
                C = C * (0.95 + 0.07 * Noise3(P * 30.0, Seed));
                break;
            case Cloth::Leather:
            {
                const double Patch = Fbm3(P * 4.0, 3, Seed);
                const double Crease = SmoothStep(0.62, 0.7, Noise3(V3{P.X * 14.0, P.Y * 3.0, P.Z * 14.0}, Seed + 3));
                C = C * (0.78 + 0.35 * Patch) * (1.0 - 0.18 * Crease);
                C = MixRgb(C, C * 1.35, 0.25 * SmoothStep(0.7, 0.85, Fbm3(P * 11.0, 2, Seed + 5)));
                break;
            }
            case Cloth::Canvas:
                C = C * (0.9 + 0.1 * Noise3(P * 35.0, Seed) + 0.06 * Fbm3(P * 5.0, 2, Seed + 1));
                break;
            case Cloth::Silk:
                C = C * (0.97 + 0.05 * Noise3(P * 12.0, Seed));
                break;
            }
            // Pattern.
            if (G.Pattern == Print::Stripes)
            {
                const double Band = Frac(P.Y * 2.2);
                C = MixRgb(C, G.PatternColor, SmoothStep(0.52, 0.56, Band) * (1.0 - SmoothStep(0.86, 0.9, Band)));
            }
            else if (G.Pattern == Print::Pinstripe)
            {
                const double Line = Frac(Atan2(P.X, P.Z) * 6.4);
                C = MixRgb(C, G.PatternColor, 0.8 * (1.0 - SmoothStep(0.03, 0.07, std::abs(Line - 0.5))));
            }
            else if (G.Pattern == Print::Check)
            {
                const double Bx = Frac(P.X * 1.4 + P.Z * 1.4), By = Frac(P.Y * 1.4);
                const double Band = (Bx < 0.45 ? 0.5 : 0.0) + (By < 0.45 ? 0.5 : 0.0);
                C = MixRgb(C, G.PatternColor, Band * 0.9);
            }
            // Folds: soft darkening bands across limbs at elbows and knees, gathers at the waist.
            const double Across = Noise3(V3{P.X * 2.5, P.Y * 9.0 + Noise3(P * 3.0, Seed + 11) * 3.0, P.Z * 2.5}, Seed + 7);
            double Fold = 0.1 * (Across - 0.5);
            Fold -= 0.12 * (1.0 - SmoothStep(0.0, 0.35, std::abs(T.ArmT - 1.0))) * SmoothStep(0.4, 0.8, Noise3(P * 7.0, Seed + 13));
            Fold -= 0.1 * (1.0 - SmoothStep(0.0, 0.4, std::abs(T.LegT - 1.0))) * T.Leg * SmoothStep(0.4, 0.8, Noise3(P * 6.0, Seed + 17));
            Fold -= 0.12 * std::max(0.0, T.Cavity) * 3.0;
            C = C * (1.0 + Fold);
            // Hem: turned edge, slightly darker, with stitches.
            if (Edge >= 0.0 && Edge < 0.12)
            {
                const double Stitch = Frac((P.X + P.Z) * 30.0 + P.Y * 30.0) < 0.5 ? 0.9 : 1.0;
                C = C * (Edge < 0.03 ? 0.72 : 0.9 * (Edge > 0.08 && Edge < 0.095 ? Stitch : 1.0));
            }
            // Wear: grime towards the hem of the legs, salt bloom, sun fade on top.
            const double Grime = G.Wear * (SmoothStep(1.2, 2.2, T.LegT) * T.Leg * 0.35 + 0.25 * SmoothStep(0.55, 0.8, Fbm3(P * 3.0, 3, Seed + 19)));
            C = MixRgb(C, C * Rgb{0.72, 0.66, 0.58}, Grime);
            const double Salt = G.Wear * 0.3 * SmoothStep(0.72, 0.82, Fbm3(P * 5.0, 3, Seed + 23));
            C = MixRgb(C, Rgb{0.86, 0.84, 0.78}, Salt);
            C = MixRgb(C, Desaturate(C, 0.4) * 1.12, G.Wear * 0.3 * SmoothStep(0.3, 0.9, T.N.Y));
            return C;
        }

        Rgb HairShade(const PersonSpec& S, const Body& B, const Attr& T, const bool Beard, double& Density)
        {
            const V3 P = T.P;
            const std::uint32_t Seed = HashString(S.Id) + (Beard ? 101u : 0u);
            double Strand;
            if (Beard)
            {
                Strand = Noise3(V3{P.X * 55.0, P.Y * 3.0, P.Z * 55.0}, Seed);
            }
            else
            {
                const double Theta = HeadAzimuth(B, P);
                const bool Combed = S.Hairdo == HairStyle::SlickedBack || S.Hairdo == HairStyle::TiedBack || S.Hairdo == HairStyle::Braid ||
                    S.Hairdo == HairStyle::PinnedUp || S.Hairdo == HairStyle::LongLoose;
                const double Wobble = Fbm3(P * 2.0, 2, Seed + 3) * (Combed ? 0.6 : 2.5);
                Strand = Noise3(V3{(Theta + Wobble * 0.3) * 60.0, P.Y * (Combed ? 2.0 : 8.0), 0.0}, Seed);
                if (S.Hairdo == HairStyle::Short || S.Hairdo == HairStyle::Tousled || S.Hairdo == HairStyle::Cropped)
                {
                    Strand = Lerp(Strand, Noise3(P * 30.0 + V3{Wobble, 0, Wobble}, Seed + 5), 0.5);
                }
            }
            const double Clump = Fbm3(P * 6.0, 2, Seed + 9);
            Rgb C = S.Hair * (0.7 + 0.55 * Strand) * (0.85 + 0.3 * Clump);
            if (Hash3(static_cast<int>(std::floor(P.X * 80.0)), static_cast<int>(std::floor(P.Y * 6.0)), static_cast<int>(std::floor(P.Z * 80.0)), Seed + 13) < S.Gray)
            {
                C = MixRgb(C, Rgb{0.66, 0.65, 0.62} * (0.75 + 0.3 * Strand), S.Gray > 0.8 ? 0.9 : 0.35);
            }
            const double Edge = Beard ? T.Beard : T.Hair;
            const double Sparse = S.Hairdo == HairStyle::Shaved || (!Beard && S.Hairdo == HairStyle::Cropped) ? 0.55 : 1.0;
            Density = Clamp(SmoothStep(-0.08, 0.3, Edge) * (0.35 + 0.75 * Strand) * Sparse * (0.7 + 0.5 * Clump), 0.0, 1.0);
            return C;
        }

        struct FaceRig
        {
            const Body* B;
            double Brow(const V3& P, const V3& Eye, const double Side, double& Along) const
            {
                const double Bx = (P.X - Eye.X) * Side;   // outwards positive
                const double By = P.Y - Eye.Y;
                Along = (Bx + 0.17) / 0.5;
                if (Along < -0.1 || Along > 1.1 || P.Z < Eye.Z - 0.15) return 0.0;
                const double Centre = 0.27 + 0.07 * Sin(Clamp(Along, 0.0, 1.0) * Pi) - 0.03 * Along;
                const double Half = Lerp(0.055, 0.028, Clamp(Along, 0.0, 1.0));
                const double D = std::abs(By - Centre) / Half;
                return (1.0 - SmoothStep(0.55, 1.0, D)) * SmoothStep(-0.1, 0.08, Along) * (1.0 - SmoothStep(0.85, 1.08, Along));
            }
        };

        Rgb SkinShade(const PersonSpec& S, const Body& B, const Attr& T)
        {
            const V3 P = T.P;
            const std::uint32_t Seed = HashString(S.Id) + 7u;
            Rgb C = S.Skin;
            // Mottling and fine grain.
            C = C * (0.94 + 0.1 * Fbm3(P * 1.6, 3, Seed) + 0.03 * (Noise3(P * 60.0, Seed + 1) - 0.5));
            const V3 F = P - B.EyeMid;
            const double Face = T.Head * SmoothStep(-0.2, 0.3, P.Z - B.HeadCentre.Z);
            // Warm zones: cheeks, nose, ears, lips' surround; knuckles, elbows, knees.
            const double CheekL = 1.0 - SmoothStep(0.0, 0.38, Length(V3{F.X - 0.42, F.Y + 0.5, 0.0}));
            const double CheekR = 1.0 - SmoothStep(0.0, 0.38, Length(V3{F.X + 0.42, F.Y + 0.5, 0.0}));
            const double Nose = 1.0 - SmoothStep(0.0, 0.28, Length(P - B.NoseTip));
            const double Ears = T.Head * SmoothStep(0.62, 0.75, std::abs(P.X - B.HeadCentre.X)) * (1.0 - SmoothStep(0.3, 0.6, std::abs(F.Y + 0.1)));
            const double Joints = T.Hand * SmoothStep(2.4, 2.9, T.ArmT) * 0.5;
            const double Red = Clamp((CheekL + CheekR) * Face * (0.35 + S.Ruddy) + Nose * (0.3 + S.Ruddy * 0.5) + Ears * 0.45 + Joints, 0.0, 1.0);
            C = MixRgb(C, C * Rgb{1.06, 0.8, 0.76}, Red * 0.45);
            // Weathering: sun-darkened face, neck, hands.
            C = MixRgb(C, C * Rgb{0.86, 0.78, 0.7}, S.Weathered * 0.35 * (T.Head + T.Hand) * (0.6 + 0.4 * Fbm3(P * 3.0, 2, Seed + 3)));
            // Eye sockets, lid creases and the lash line along the upper lid.
            for (const V3& Eye : {B.EyeL, B.EyeR})
            {
                const double D = Length(P - Eye);
                C = MixRgb(C, C * Rgb{0.8, 0.72, 0.74}, 0.35 * (1.0 - SmoothStep(0.16, 0.34, D)) * T.Head);
                const double Front = SmoothStep(-0.05, 0.05, P.Z - Eye.Z);
                const double Upper = SmoothStep(-0.03, 0.03, P.Y - Eye.Y);
                const double Rim = (1.0 - SmoothStep(0.012, 0.03, std::abs(D - 0.14))) * Front * T.Head;
                C = MixRgb(C, MixRgb(S.Hair * 0.4, Rgb{0.03, 0.025, 0.02}, 0.6), Rim * (0.25 + 0.55 * Upper));
                const double Crease = (1.0 - SmoothStep(0.01, 0.03, std::abs(D - 0.21))) * Upper * Front * T.Head;
                C = C * (1.0 - 0.18 * Crease);
            }
            // Lips.
            {
                const double Half = std::max(0.08, std::abs(B.MouthL.X - B.Mouth.X));
                const double Lx = (P.X - B.Mouth.X) / Half;
                const double Up = P.Y - B.Mouth.Y;
                const double Ly = Up > 0 ? Up / (0.12 - 0.035 * Lx * Lx) : -Up / 0.13;
                const double Lip = (1.0 - SmoothStep(0.7, 1.0, Lx * Lx * (1.0 + 0.3 * Ly * Ly) + Ly * Ly)) * SmoothStep(-0.3, -0.1, P.Z - B.Mouth.Z) * T.Head;
                const Rgb LipColour = S.Skin * Rgb{0.86, 0.6, 0.6} * (S.Uncanny ? 1.05 : 1.0);
                C = MixRgb(C, LipColour, Lip * 0.75);
                C = C * (1.0 - 0.35 * Lip * (1.0 - SmoothStep(0.0, 0.025, std::abs(Up))));  // parting line
            }
            // Brows.
            {
                const FaceRig Rig{&B};
                const Rgb BrowColour = MixRgb(S.Hair * 0.85, Rgb{0.7, 0.7, 0.68}, S.Gray * 0.8);
                for (int Side = 0; Side < 2; ++Side)
                {
                    double Along = 0;
                    const double M = Rig.Brow(P, Side == 0 ? B.EyeL : B.EyeR, Side == 0 ? 1.0 : -1.0, Along) * T.Head;
                    if (M <= 0.0) continue;
                    const double Hairs = Noise3(V3{P.X * 90.0 + P.Y * 40.0, P.Y * 20.0, 0.0}, Seed + 5);
                    C = MixRgb(C, BrowColour, M * (0.45 + 0.5 * Hairs));
                }
            }
            // Beard shadow and stubble.
            const double Shadow = SmoothStep(-0.1, 0.15, T.Beard);
            if (S.Beard != BeardStyle::None || S.Male > 0.5)
            {
                const double Amount = S.Beard == BeardStyle::Stubble ? 0.6 : S.Beard == BeardStyle::None ? 0.22 : 0.5;
                const double Dots = SmoothStep(0.45, 0.75, Noise3(P * 120.0, Seed + 9));
                const Rgb Dark = MixRgb(S.Hair * 0.7, Rgb{0.6, 0.6, 0.6}, S.Gray);
                C = MixRgb(C, MixRgb(C * Rgb{0.82, 0.84, 0.88}, Dark, Dots * 0.6), Shadow * Amount);
            }
            // Scalp under hair reads as hair; shaved scalps get stubble.
            if (T.Hair > -0.05)
            {
                const double Under = SmoothStep(-0.05, 0.2, T.Hair);
                const double Short = S.Hairdo == HairStyle::Shaved ? 0.5 : 0.85;
                C = MixRgb(C, S.Hair * (0.8 + 0.3 * Noise3(P * 80.0, Seed + 11)), Under * Short);
            }
            // Age: forehead lines, crow's feet, nasolabial folds, spots.
            if (S.Wrinkles > 0.0)
            {
                const double Forehead = Face * SmoothStep(0.35, 0.5, F.Y) * (1.0 - SmoothStep(0.75, 0.95, F.Y)) * (1.0 - SmoothStep(0.3, 0.5, std::abs(F.X)));
                const double Lines = SmoothStep(0.8, 0.95, Sin(F.Y * 95.0 + 3.0 * Noise3(P * 3.0, Seed + 13)) * 0.5 + 0.5);
                double Crease = Forehead * Lines;
                for (const double Side : {1.0, -1.0})
                {
                    const V3 Corner = (Side > 0 ? B.EyeL : B.EyeR) + V3{Side * 0.2, 0.0, -0.1};
                    const double D = Length(P - Corner);
                    const double Rays = SmoothStep(0.75, 0.95, Sin(Atan2(P.Y - Corner.Y, (P.X - Corner.X) * Side) * 14.0) * 0.5 + 0.5);
                    Crease += Face * Rays * (1.0 - SmoothStep(0.05, 0.22, D)) * SmoothStep(0.03, 0.06, D);
                    // Nasolabial: nose wing to mouth corner.
                    const V3 Wing = B.NoseTip + V3{Side * 0.2, -0.12, -0.25};
                    const V3 Corner2 = (Side > 0 ? B.MouthL : B.MouthR) + V3{Side * 0.05, -0.02, 0.0};
                    const V3 Seg = Corner2 - Wing;
                    const double Tt = Clamp(Dot(P - Wing, Seg) / Dot(Seg, Seg), 0.0, 1.0);
                    const double Ds = Length(P - (Wing + Seg * Tt));
                    Crease += Face * (1.0 - SmoothStep(0.0, 0.035, Ds)) * 0.8;
                }
                C = C * (1.0 - 0.28 * S.Wrinkles * Clamp(Crease, 0.0, 1.0));
                const double Spots = SmoothStep(0.78, 0.84, Noise3(P * 9.0, Seed + 15)) * (T.Head + T.Hand);
                C = MixRgb(C, C * Rgb{0.8, 0.7, 0.6}, Spots * S.Wrinkles * 0.5);
            }
            if (S.Freckles > 0.0)
            {
                const double Dots = SmoothStep(0.7, 0.78, Noise3(P * 45.0, Seed + 17)) * (Face + T.Hand * 0.5);
                C = MixRgb(C, C * Rgb{0.78, 0.62, 0.5}, Dots * S.Freckles);
            }
            for (const Scar& Sc : S.Scars)
            {
                const V3 A = Sc.Face ? B.EyeMid + Sc.A : Sc.A, Bb = Sc.Face ? B.EyeMid + Sc.B : Sc.B;
                const V3 Seg = Bb - A;
                const double Tt = Clamp(Dot(P - A, Seg) / Dot(Seg, Seg), 0.0, 1.0);
                const double D = Length(P - (A + Seg * Tt));
                const double Core = 1.0 - SmoothStep(Sc.Width * 0.3, Sc.Width, D);
                C = MixRgb(C, MixRgb(C, Rgb{0.92, 0.72, 0.68}, 0.6) * 1.08, Core * 0.8);
                C = C * (1.0 - 0.12 * (SmoothStep(Sc.Width * 0.6, Sc.Width, D) * (1.0 - SmoothStep(Sc.Width, Sc.Width * 1.6, D))));
            }
            // Creases and cavities.
            C = C * (1.0 - 0.35 * Clamp(T.Cavity * 2.5, 0.0, 1.0)) * (1.0 + 0.05 * Clamp(-T.Cavity * 2.0, 0.0, 1.0));
            if (S.Uncanny) C = MixRgb(Desaturate(C, 0.7), Rgb{0.62, 0.68, 0.76}, 0.25);
            return C;
        }

        Rgb EyeShade(const PersonSpec& S, const V3& Dir, const std::uint32_t Seed)
        {
            const double CosA = Dir.Z;
            const double Psi = Atan2(Dir.Y, Dir.X);
            const double R = std::sqrt(std::max(0.0, 1.0 - CosA * CosA));  // sine of the angle from the gaze
            Rgb Sclera{0.78, 0.75, 0.7};
            Sclera = MixRgb(Sclera, Rgb{0.85, 0.62, 0.6}, SmoothStep(0.55, 0.95, R) * 0.35);
            if (CosA < 0.0) return Sclera * 0.8;
            const double Iris = 0.5, Pupil = 0.19;
            if (R > Iris) return MixRgb(Sclera, Sclera * 0.75, 1.0 - SmoothStep(Iris, Iris + 0.06, R));
            const double Fibres = Noise3(V3{Psi * 9.0, R * 6.0, 0.0}, Seed);
            Rgb C = S.Eyes * (0.95 + 0.8 * Fibres);
            C = MixRgb(C, S.Eyes * 1.5, (1.0 - SmoothStep(0.2, 0.3, R)) * 0.35);           // collarette
            C = C * (1.0 - 0.55 * SmoothStep(Iris - 0.1, Iris, R));                         // limbal ring
            C = MixRgb(C, Rgb{0.02, 0.02, 0.02}, 1.0 - SmoothStep(Pupil - 0.02, Pupil + 0.01, R));
            if (S.Uncanny) C = MixRgb(C, Rgb{0.75, 0.8, 0.85}, 0.5);
            return C;
        }

        // ------------------------------------------------------------------ geometry helpers

        /** Support ring: for each of Count directions around (Cx, Cz), the furthest extent of the points. */
        std::vector<double> SupportRing(const std::vector<V3>& Points, const double Cx, const double Cz, const int Count)
        {
            std::vector<double> R(static_cast<std::size_t>(Count), 0.3);
            for (int K = 0; K < Count; ++K)
            {
                const double A = 2.0 * Pi * K / Count;
                const double Sx = Sin(A), Cz2 = Cos(A);
                double Best = 0.05;
                for (const V3& P : Points) Best = std::max(Best, (P.X - Cx) * Sx + (P.Z - Cz) * Cz2);
                R[static_cast<std::size_t>(K)] = Best;
            }
            return R;
        }

        /** Convex hull (counter-clockwise seen from +Y) of points projected onto XZ, Andrew's monotone chain. */
        std::vector<V3> HullXZ(std::vector<V3> Points)
        {
            std::sort(Points.begin(), Points.end(), [](const V3& A, const V3& B) { return A.X != B.X ? A.X < B.X : A.Z < B.Z; });
            const auto Turn = [](const V3& O, const V3& A, const V3& B) { return (A.X - O.X) * (B.Z - O.Z) - (A.Z - O.Z) * (B.X - O.X); };
            std::vector<V3> H;
            for (int Pass = 0; Pass < 2; ++Pass)
            {
                const std::size_t Start = H.size();
                for (std::size_t K = 0; K < Points.size(); ++K)
                {
                    const V3& P = Pass == 0 ? Points[K] : Points[Points.size() - 1 - K];
                    while (H.size() >= Start + 2 && Turn(H[H.size() - 2], H.back(), P) >= 0.0) H.pop_back();
                    H.push_back(P);
                }
                H.pop_back();
            }
            return H;
        }

        struct Builder
        {
            FigurePart* Part;
            const Source* Data;
            const Body* B;
            std::uint32_t Add(const V3& P, const V3& N, const double U, const double V)
            {
                SkinVertex Sv;
                Sv.P = P;
                Sv.N = Normalize(N);
                Sv.U = U;
                Sv.V = V;
                Nearest(P, Sv);
                Part->Vertices.push_back(Sv);
                return static_cast<std::uint32_t>(Part->Vertices.size() - 1);
            }
            /** Skin influences of the nearest body vertex. */
            void Nearest(const V3& P, SkinVertex& Out) const
            {
                double Best = 1e30;
                std::size_t Found = 0;
                for (std::size_t I = 0; I < B->Rest.size(); ++I)
                {
                    if (!B->IsBody[I]) continue;
                    const V3 D = B->Rest[I] - P;
                    const double L = Dot(D, D);
                    if (L < Best)
                    {
                        Best = L;
                        Found = I;
                    }
                }
                InfluencesOf(*Data, static_cast<int>(Found), Out.Joint, Out.Weight);
            }
            void Tri(const std::uint32_t A, const std::uint32_t Bv, const std::uint32_t C)
            {
                Part->Indices.push_back(A);
                Part->Indices.push_back(Bv);
                Part->Indices.push_back(C);
            }
            /** Grid of rings (Rows x Cols points, row-major); Closed wraps the columns. Normals from the grid. */
            void Grid(const std::vector<std::vector<V3>>& Rings, const bool Closed, const double UScale, const double VScale)
            {
                const std::size_t Rows = Rings.size();
                if (Rows < 2) return;
                const std::size_t Cols = Rings[0].size();
                const auto Base = static_cast<std::uint32_t>(Part->Vertices.size());
                const std::size_t Wrap = Closed ? Cols + 1 : Cols;
                double VAcc = 0;
                for (std::size_t R = 0; R < Rows; ++R)
                {
                    if (R > 0) VAcc += Length(Rings[R][0] - Rings[R - 1][0]);
                    double UAcc = 0;
                    for (std::size_t C = 0; C < Wrap; ++C)
                    {
                        const std::size_t Ci = C % Cols;
                        if (C > 0) UAcc += Length(Rings[R][Ci] - Rings[R][(C - 1) % Cols]);
                        const V3 Du = Rings[R][(Ci + 1) % Cols] - Rings[R][(Ci + Cols - 1) % Cols];
                        const V3 Dv = Rings[std::min(R + 1, Rows - 1)][Ci] - Rings[R > 0 ? R - 1 : 0][Ci];
                        const V3 Dun = Closed || (Ci > 0 && Ci + 1 < Cols) ? Du : (Ci == 0 ? Rings[R][1] - Rings[R][0] : Rings[R][Cols - 1] - Rings[R][Cols - 2]);
                        Add(Rings[R][Ci], Cross(Dv, Dun), UAcc / UScale, VAcc / VScale);
                    }
                }
                for (std::size_t R = 0; R + 1 < Rows; ++R)
                {
                    for (std::size_t C = 0; C + 1 < Wrap; ++C)
                    {
                        const auto A = static_cast<std::uint32_t>(Base + R * Wrap + C);
                        const auto Bq = static_cast<std::uint32_t>(A + 1);
                        const auto Cq = static_cast<std::uint32_t>(A + Wrap);
                        const auto Dq = static_cast<std::uint32_t>(Cq + 1);
                        Tri(A, Cq, Bq);
                        Tri(Bq, Cq, Dq);
                    }
                }
            }
            /** Tube along a path with per-point radii (Sides around), end caps. */
            void Tube(const std::vector<V3>& Path, const std::vector<double>& Radius, const int Sides, const double Flatten, const V3& Hint)
            {
                std::vector<std::vector<V3>> Rings;
                for (std::size_t I = 0; I < Path.size(); ++I)
                {
                    const V3 Dir = Normalize(Path[std::min(I + 1, Path.size() - 1)] - Path[I > 0 ? I - 1 : 0]);
                    V3 Side = Cross(Dir, Hint);
                    if (Length(Side) < 1e-6) Side = Cross(Dir, V3{1, 0, 0});
                    Side = Normalize(Side);
                    const V3 Up = Normalize(Cross(Side, Dir));
                    std::vector<V3> Ring;
                    for (int K = 0; K < Sides; ++K)
                    {
                        const double A = 2.0 * Pi * K / Sides;
                        Ring.push_back(Path[I] + Side * (Cos(A) * Radius[I]) + Up * (Sin(A) * Radius[I] * Flatten));
                    }
                    Rings.push_back(std::move(Ring));
                }
                Grid(Rings, true, 0.5, 0.5);
                // caps
                for (const std::size_t End : {std::size_t{0}, Rings.size() - 1})
                {
                    V3 Centre;
                    for (const V3& P : Rings[End]) Centre = Centre + P;
                    Centre = Centre * (1.0 / Sides);
                    const V3 Out = End == 0 ? Normalize(Rings[0][0] - Rings[1][0]) : Normalize(Rings[End][0] - Rings[End - 1][0]);
                    const std::uint32_t Hub = Add(Centre, Out, 0.5, 0.5);
                    std::vector<std::uint32_t> Rim;
                    for (const V3& P : Rings[End]) Rim.push_back(Add(P, Out, 0.5, 0.5));
                    for (int K = 0; K < Sides; ++K)
                    {
                        const auto A = Rim[static_cast<std::size_t>(K)], C = Rim[static_cast<std::size_t>((K + 1) % Sides)];
                        if (Dot(Cross(Rings[End][static_cast<std::size_t>(K)] - Centre, Rings[End][static_cast<std::size_t>((K + 1) % Sides)] - Centre), Out) > 0) Tri(Hub, A, C);
                        else Tri(Hub, C, A);
                    }
                }
            }
            void Box(const V3& Centre, const V3& AxisX, const V3& AxisY, const V3& AxisZ)
            {
                const V3 Axes[3] = {AxisX, AxisY, AxisZ};
                for (int F = 0; F < 6; ++F)
                {
                    const int A = F / 2;
                    const double Sg = (F % 2) ? -1.0 : 1.0;
                    const V3 N = Axes[A] * Sg;
                    const V3 U = Axes[(A + 1) % 3], V = Axes[(A + 2) % 3];
                    const V3 C = Centre + N;
                    const std::uint32_t I0 = Add(C - U - V, N, 0, 0), I1 = Add(C + U - V, N, 1, 0), I2 = Add(C + U + V, N, 1, 1), I3 = Add(C - U + V, N, 0, 1);
                    if (Dot(Cross(U, V), N) > 0)
                    {
                        Tri(I0, I1, I2);
                        Tri(I0, I2, I3);
                    }
                    else
                    {
                        Tri(I0, I2, I1);
                        Tri(I0, I3, I2);
                    }
                }
            }
            void Sphere(const V3& Centre, const V3& Radii, const int Rows, const int Cols)
            {
                std::vector<std::vector<V3>> Rings;
                for (int R = 0; R <= Rows; ++R)
                {
                    const double Ph = Pi * R / Rows;
                    std::vector<V3> Ring;
                    for (int C = 0; C < Cols; ++C)
                    {
                        const double Th = 2.0 * Pi * C / Cols;
                        Ring.push_back(Centre + V3{Sin(Ph) * Sin(Th) * Radii.X, Cos(Ph) * Radii.Y, Sin(Ph) * Cos(Th) * Radii.Z});
                    }
                    Rings.push_back(std::move(Ring));
                }
                Grid(Rings, true, 1.0, 1.0);
            }
        };

        Material Solid(const std::string& Name, const Rgb& C, const double Roughness, const double Metallic = 0.0)
        {
            Material M;
            M.Name = Name;
            M.BaseColor = {Clamp(C.R, 0.0, 1.0), Clamp(C.G, 0.0, 1.0), Clamp(C.B, 0.0, 1.0), 1.0};
            M.Roughness = Roughness;
            M.Metallic = Metallic;
            return M;
        }
        /** Tinted shared tileable texture (texture mean 0.85, so the factor stays within 0..1). */
        Material Tinted(const std::string& Name, const std::string& Texture, const Rgb& C, const double Roughness)
        {
            Material M = Solid(Name, C * (1.0 / 0.85), Roughness);
            M.BaseColorTexture = Texture;
            M.DoubleSided = true;
            return M;
        }

        const char* ClothTexture(const Cloth C)
        {
            return C == Cloth::Leather ? "T_Hum_Leather_BaseColor.png" : "T_Hum_Cloth_BaseColor.png";
        }

        /** Shared tileable textures used by skirts, hats and gear. */
        std::vector<ArtFile> SharedTextures()
        {
            std::vector<ArtFile> Files;
            const int S = 256;
            const auto Make = [&](const char* Name, auto&& Fn, const int Channels)
            {
                Image Img;
                Img.Width = S;
                Img.Height = S;
                Img.Channels = Channels;
                Img.Pixels.resize(static_cast<std::size_t>(S * S * Channels));
                for (int Y = 0; Y < S; ++Y)
                {
                    for (int X = 0; X < S; ++X)
                    {
                        double A = 1.0;
                        const double V = Fn(X, Y, A);
                        std::uint8_t* Px = &Img.Pixels[static_cast<std::size_t>((Y * S + X) * Channels)];
                        Px[0] = Px[1] = Px[2] = Byte(V);
                        if (Channels == 4) Px[3] = Byte(A);
                    }
                }
                Files.push_back({std::string("Assets/Art/Textures/") + Name, EncodePng(Img)});
            };
            // Cloth: plain weave with slubs.
            Make("T_Hum_Cloth_BaseColor.png", [&](int X, int Y, double&)
            {
                const double Weave = ((X / 2 + Y / 2) % 2 ? 0.03 : -0.03);
                const double Slub = (Noise(X, Y * 8, S, 32, 71) / 65535.0 - 0.5) * 0.06;
                const double Mottle = (Fbm(X, Y, S, 4, 3, 72) / 65535.0 - 0.5) * 0.08;
                return 0.85 + Weave + Slub + Mottle;
            }, 3);
            Make("T_Hum_Leather_BaseColor.png", [&](int X, int Y, double&)
            {
                const double Grain = (Noise(X, Y, S, 64, 81) / 65535.0 - 0.5) * 0.08;
                const double Patch = (Fbm(X, Y, S, 4, 4, 82) / 65535.0 - 0.5) * 0.22;
                return 0.85 + Grain + Patch;
            }, 3);
            Make("T_Hum_Strands_BaseColor.png", [&](int X, int Y, double&)
            {
                const double Strand = Noise(X * 4, Y / 8, S, 64, 91) / 65535.0;
                const double Clump = Fbm(X, Y, S, 4, 2, 92) / 65535.0;
                return 0.62 + 0.3 * Strand + 0.12 * (Clump - 0.5);
            }, 3);
            Make("T_Hum_HairCard_BaseColor.png", [&](int X, int Y, double& Alpha)
            {
                // Strands run down the card (v); each column ends at its own length, roots dense, tips sparse.
                const double V = (Y + 0.5) / S;
                const double Column = Noise(X * 6, 0, S, 64, 97) / 65535.0;
                const double Length = 0.55 + 0.45 * (Noise(X * 3, 7, S, 32, 98) / 65535.0);
                const double Present = Column > 0.3 + 0.35 * V ? 1.0 : 0.0;
                const double Edge = std::min((X + 0.5) / 18.0, (S - X - 0.5) / 18.0);
                Alpha = Present * (V < Length ? 1.0 : 0.0) * Clamp(Edge, 0.0, 1.0);
                return 0.72 + 0.26 * Column - 0.1 * V;
            }, 4);
            Make("T_Hum_Straw_BaseColor.png", [&](int X, int Y, double&)
            {
                const bool Over = ((X / 8) + (Y / 8)) % 2 == 0;
                const double Fibre = Noise(Over ? X : X * 6, Over ? Y * 6 : Y, S, 64, 95) / 65535.0;
                return 0.8 + 0.18 * (Fibre - 0.5) + (Over ? 0.04 : -0.04);
            }, 3);
            return Files;
        }
    }

}

namespace DarkArisen::Tools::Art
{
    using namespace Humans;

    // ------------------------------------------------------------------ factory

    struct HumanFactory::Impl
    {
        SourceReader Reader;
        Source Data;
        bool Loaded = false;
        bool Failed = false;
        std::string LoadProblem;
        std::set<std::string> TexturesDone;

        bool Ensure(std::string& Problem)
        {
            if (!Loaded && !Failed)
            {
                Failed = !LoadSource(Reader, Data, LoadProblem);
                Loaded = !Failed;
            }
            if (Failed) Problem = LoadProblem;
            return Loaded;
        }

        /** Builds the dressed, groomed person in the rest frame, plus its textures. */
        bool Dress(const PersonSpec& S, Body& B, std::vector<FigurePart>& Parts, std::vector<ArtFile>& Files, std::string& Problem);
    };

    HumanFactory::HumanFactory(SourceReader Reader) : P(std::make_unique<Impl>())
    {
        P->Reader = std::move(Reader);
    }
    HumanFactory::~HumanFactory() = default;

    std::vector<std::string> HumanFactory::MotionClips()
    {
        return {"02_01", "09_01", "17_03", "77_02", "77_01", "40_10", "18_08", "18_10", "13_26", "62_07", "13_23", "70_03", "69_69",
            "26_10", "75_18", "82_05", "13_04", "14_30", "02_07", "15_13", "13_33", "13_09", "81_07", "91_09", "15_06"};
    }

    bool HumanFactory::Impl::Dress(const PersonSpec& S, Body& B, std::vector<FigurePart>& Parts, std::vector<ArtFile>& Files, std::string& Problem)
    {
        if (!BuildBody(Reader, Data, S, B, Problem)) return false;
        const std::size_t N = B.Rest.size();
        const int BodyGroup = Data.Group("body");
        const std::string Tag = "Hum_" + S.Id;
        const std::uint32_t Seed = HashString(S.Id);

        // Per-vertex attributes.
        const double SkirtHemY = S.Dress.On ? LegY(B, S.Dress.Hem) : 1e9;
        std::vector<Attr> At(N);
        for (std::size_t I = 0; I < N; ++I)
        {
            Attr& A = At[I];
            A.P = B.Rest[I];
            A.N = B.Normals[I];
            if (!B.IsBody[I]) continue;
            const Anatomy& An = B.Parts[I];
            A.ArmT = An.ArmT;
            A.LegT = An.LegT;
            A.Cavity = An.Cavity;
            A.Head = An.Head + An.Neck * 0.3;
            A.Hand = An.Hand;
            A.Leg = An.Leg + An.Foot;
            for (int K = 0; K < SlotCount; ++K) A.S[static_cast<std::size_t>(K)] = -9.0;
            if (S.Stockings.On) A.S[Stockings] = LegwearCut(S.Stockings, An, false);
            if (S.Shirt.On) A.S[Shirt] = UpperCut(S.Shirt, B, A.P, An);
            if (S.Legs.On) A.S[Legs] = LowerCut(S.Legs, B, A.P, An);
            if (S.Feet.On) A.S[Feet] = LegwearCut(S.Feet, An, true);
            if (S.Vest.On) A.S[Vest] = UpperCut(S.Vest, B, A.P, An);
            if (S.Coat.On) A.S[Coat] = UpperCut(S.Coat, B, A.P, An);
            A.S[HeadCloth] = HeadClothCut(S, B, A.P, An);
            A.Hair = HairCut(S, B, A.P, An);
            A.Beard = BeardCut(S, B, A.P, An);
            if (S.Dress.On && S.Dress.OpenFront <= 0.0)
            {
                // Legs under a closed skirt: hidden above the hem.
                const double Under = std::min(A.P.Y - SkirtHemY - 0.25, B.TorsoY(S.Dress.From) - A.P.Y);
                A.Hidden = (An.Leg + An.Foot + An.Torso) * Under + (An.Head + An.Neck + An.Arm + An.Hand) * -2.0;
            }
        }
        const auto Covering = [&](const Attr& A)
        {
            double Best = -9.0;
            for (int K = 0; K < SlotCount; ++K) Best = std::max(Best, A.S[static_cast<std::size_t>(K)]);
            return Best;
        };

        const int Size = S.TextureSize;
        const std::string SkinTexture = "T_" + Tag + "_Skin.png", OutfitTexture = "T_" + Tag + "_Outfit.png", EyeTexture = "T_" + Tag + "_Eyes.png";

        // Body faces: split into triangles with their UVs.
        struct Tri3
        {
            std::array<int, 3> V, T;
        };
        std::vector<Tri3> BodyTris;
        for (const Face& F : Data.Faces)
        {
            if (F.Group != BodyGroup) continue;
            for (int K = 1; K + 1 < F.Count; ++K)
            {
                BodyTris.push_back({{F.V[0], F.V[static_cast<std::size_t>(K)], F.V[static_cast<std::size_t>(K + 1)]},
                    {F.T[0], F.T[static_cast<std::size_t>(K)], F.T[static_cast<std::size_t>(K + 1)]}});
            }
        }

        // ---- textures
        if (!TexturesDone.count(SkinTexture))
        {
            Canvas Skin(Size, Size, 3), Outfit(Size, Size, 4);
            for (const Tri3& T : BodyTris)
            {
                const Attr& A0 = At[static_cast<std::size_t>(T.V[0])];
                const Attr& A1 = At[static_cast<std::size_t>(T.V[1])];
                const Attr& A2 = At[static_cast<std::size_t>(T.V[2])];
                const auto& U0 = Data.Uvs[static_cast<std::size_t>(T.T[0])];
                const auto& U1 = Data.Uvs[static_cast<std::size_t>(T.T[1])];
                const auto& U2 = Data.Uvs[static_cast<std::size_t>(T.T[2])];
                Raster(Skin, U0, U1, U2, [&](int X, int Y, double W0, double W1, double W2)
                {
                    Skin.Put(X, Y, SkinShade(S, B, Blend(A0, A1, A2, W0, W1, W2)), 1.0);
                });
                const double Near = std::max({Covering(A0), Covering(A1), Covering(A2), A0.Hair, A1.Hair, A2.Hair, A0.Beard, A1.Beard, A2.Beard});
                if (Near < -0.6) continue;
                Raster(Outfit, U0, U1, U2, [&](int X, int Y, double W0, double W1, double W2)
                {
                    const Attr T3 = Blend(A0, A1, A2, W0, W1, W2);
                    // Topmost garment covering the texel (head cloth, coat, vest, footwear, legs, shirt, stockings);
                    // otherwise the nearest one, written transparent so filtering at cloth edges keeps its colour.
                    static constexpr int Order[] = {HeadCloth, Coat, Vest, Feet, Legs, Shirt, Stockings};
                    int Which = -1;
                    double BestS = -8.0;
                    bool Covered = false;
                    for (const int K : Order)
                    {
                        const double Sk = T3.S[static_cast<std::size_t>(K)];
                        if (Sk >= 0.0)
                        {
                            Which = K;
                            BestS = Sk;
                            Covered = true;
                            break;
                        }
                        if (Sk > BestS)
                        {
                            BestS = Sk;
                            Which = K;
                        }
                    }
                    const bool HairNear = T3.Hair > -0.3 || T3.Beard > -0.3;
                    if (Which >= 0 && (Covered || !HairNear))
                    {
                        Rgb C;
                        if (Which == HeadCloth)
                        {
                            Garment Scarf;
                            Scarf.Color = S.HatColor;
                            Scarf.Fabric = S.Hat == HatStyle::KnitCap ? Cloth::Wool : Cloth::Cotton;
                            Scarf.Wear = 0.3;
                            C = FabricShade(Scarf, T3, Seed + 40u, BestS);
                            if (S.Hat == HatStyle::KnitCap) C = C * (0.9 + 0.2 * SmoothStep(0.3, 0.7, Frac(HeadAzimuth(B, T3.P) * 12.0)));
                        }
                        else
                        {
                            const Garment& G = *SlotGarment(S, Which);
                            C = FabricShade(G, T3, Seed + static_cast<std::uint32_t>(Which) * 31u, BestS);
                            if (G.Trimmed && BestS >= 0.0 && BestS < 0.1) C = G.Trim;
                            // Buttons down the front edge (or the centre line of a closed garment).
                            if (G.Buttons > 0 && (Which == Vest || Which == Coat || Which == Shirt))
                            {
                                const double Top = B.NeckY - G.NeckFront - 0.15, Bottom = std::max(B.TorsoY(G.Hem) + 0.3, B.TorsoY(0.1));
                                const double Column = G.Open > 0.0 ? G.Open + 0.14 : 0.0;
                                const double Spacing = (Top - Bottom) / std::max(1, G.Buttons - 1);
                                for (int Bi = 0; Bi < G.Buttons; ++Bi)
                                {
                                    const double By = Top - Spacing * Bi;
                                    for (const double Side : {1.0, -1.0})
                                    {
                                        if (Column == 0.0 && Side < 0) continue;
                                        const double D = Length(V3{std::abs(T3.P.X) - Column, T3.P.Y - By, 0.0});
                                        if (D < 0.075 && Frontness(B, T3.P) > 0.5 && (G.Open > 0.0 || Side > 0))
                                        {
                                            const double Dome = 1.0 - D / 0.075;
                                            C = G.ButtonColor * (0.65 + 0.5 * Dome);
                                        }
                                    }
                                }
                            }
                        }
                        if (S.Uncanny) C = MixRgb(Desaturate(C, 0.75), Rgb{0.6, 0.66, 0.74}, 0.3);
                        Outfit.Put(X, Y, C, Covered ? 1.0 : 0.0);
                        return;
                    }
                    // Hair and beard (their shells use the same texture).
                    double Density = 0;
                    const bool IsBeard = T3.Beard > T3.Hair;
                    const Rgb C = HairShade(S, B, T3, IsBeard, Density);
                    Outfit.Put(X, Y, S.Uncanny ? Desaturate(C, 0.6) : C, Density);
                });
            }
            Skin.Dilate(6);
            Outfit.Dilate(6);
            Files.push_back({"Assets/Art/Textures/" + SkinTexture, EncodePng(Skin.Img)});
            Files.push_back({"Assets/Art/Textures/" + OutfitTexture, EncodePng(Outfit.Img)});

            // Eyes: both eyeball islands side by side.
            Canvas Eyes(128, 64, 3);
            for (int E = 0; E < 2; ++E)
            {
                const int G = Data.Group(E == 0 ? "helper-l-eye" : "helper-r-eye");
                const V3 Centre = E == 0 ? B.EyeL : B.EyeR;
                double U0 = 1e9, V0 = 1e9, U1 = -1e9, V1 = -1e9;
                for (const Face& F : Data.Faces)
                {
                    if (F.Group != G) continue;
                    for (int K = 0; K < F.Count; ++K)
                    {
                        const auto& Uv = Data.Uvs[static_cast<std::size_t>(F.T[static_cast<std::size_t>(K)])];
                        U0 = std::min(U0, Uv[0]); U1 = std::max(U1, Uv[0]); V0 = std::min(V0, Uv[1]); V1 = std::max(V1, Uv[1]);
                    }
                }
                for (const Face& F : Data.Faces)
                {
                    if (F.Group != G) continue;
                    for (int K = 1; K + 1 < F.Count; ++K)
                    {
                        const int Ix[3] = {0, K, K + 1};
                        std::array<double, 2> Uv[3];
                        V3 Dir[3];
                        for (int J = 0; J < 3; ++J)
                        {
                            const auto& Src = Data.Uvs[static_cast<std::size_t>(F.T[static_cast<std::size_t>(Ix[J])])];
                            Uv[J] = {(E * 0.5) + (Src[0] - U0) / (U1 - U0) * 0.5, (Src[1] - V0) / (V1 - V0)};
                            Dir[J] = B.Rest[static_cast<std::size_t>(F.V[static_cast<std::size_t>(Ix[J])])] - Centre;
                        }
                        Raster(Eyes, Uv[0], Uv[1], Uv[2], [&](int X, int Y, double W0, double W1, double W2)
                        {
                            Eyes.Put(X, Y, EyeShade(S, Normalize(Dir[0] * W0 + Dir[1] * W1 + Dir[2] * W2), Seed + 3u), 1.0);
                        });
                    }
                }
            }
            Eyes.Dilate(4);
            Files.push_back({"Assets/Art/Textures/" + EyeTexture, EncodePng(Eyes.Img)});
            TexturesDone.insert(SkinTexture);
        }

        // ---- meshes from the body topology
        const auto BodyVertex = [&](const int V, const int T, const V3& Offset)
        {
            SkinVertex Sv;
            Sv.P = B.Rest[static_cast<std::size_t>(V)] + Offset;
            Sv.N = B.Normals[static_cast<std::size_t>(V)];
            Sv.U = Data.Uvs[static_cast<std::size_t>(T)][0];
            Sv.V = Data.Uvs[static_cast<std::size_t>(T)][1];
            InfluencesOf(Data, V, Sv.Joint, Sv.Weight);
            return Sv;
        };
        /** Adds the triangles of the groups that pass Keep(v0, v1, v2), each vertex displaced by Offset(v). */
        const auto Surface = [&](FigurePart& Part, const std::vector<int>& Groups, auto&& Keep, auto&& Offset, const bool RemapEyes)
        {
            std::map<std::pair<int, int>, std::uint32_t> Index;
            for (const int G : Groups)
            {
                double U0 = 1e9, V0 = 1e9, U1 = -1e9, V1 = -1e9;
                if (RemapEyes)
                {
                    for (const Face& F : Data.Faces)
                    {
                        if (F.Group != G) continue;
                        for (int K = 0; K < F.Count; ++K)
                        {
                            const auto& Uv = Data.Uvs[static_cast<std::size_t>(F.T[static_cast<std::size_t>(K)])];
                            U0 = std::min(U0, Uv[0]); U1 = std::max(U1, Uv[0]); V0 = std::min(V0, Uv[1]); V1 = std::max(V1, Uv[1]);
                        }
                    }
                }
                const bool Right = RemapEyes && Data.Groups[static_cast<std::size_t>(G)] == "helper-r-eye";
                for (const Face& F : Data.Faces)
                {
                    if (F.Group != G) continue;
                    for (int K = 1; K + 1 < F.Count; ++K)
                    {
                        const std::size_t Ix[3] = {0, static_cast<std::size_t>(K), static_cast<std::size_t>(K + 1)};
                        if (!Keep(F.V[Ix[0]], F.V[Ix[1]], F.V[Ix[2]])) continue;
                        for (const std::size_t J : Ix)
                        {
                            const int V = F.V[J], T = F.T[J];
                            const auto Key = std::make_pair(V, T);
                            auto Found = Index.find(Key);
                            if (Found == Index.end())
                            {
                                SkinVertex Sv = BodyVertex(V, T, Offset(V));
                                if (RemapEyes)
                                {
                                    Sv.U = (Right ? 0.5 : 0.0) + (Sv.U - U0) / (U1 - U0) * 0.5;
                                    Sv.V = (Sv.V - V0) / (V1 - V0);
                                }
                                Part.Vertices.push_back(Sv);
                                Found = Index.emplace(Key, static_cast<std::uint32_t>(Part.Vertices.size() - 1)).first;
                            }
                            Part.Indices.push_back(Found->second);
                        }
                    }
                }
            }
        };
        const auto Any = [&](auto&& Test) { return [&, Test](int A, int Bv, int C) { return Test(At[static_cast<std::size_t>(A)]) || Test(At[static_cast<std::size_t>(Bv)]) || Test(At[static_cast<std::size_t>(C)]); }; };
        const auto All = [](int, int, int) { return true; };
        const auto Flat = [](int) { return V3{}; };

        const auto Opaque = [&](const Attr& A)
        {
            double Best = A.Hidden;
            for (int K = 0; K < SlotCount; ++K) Best = std::max(Best, A.S[static_cast<std::size_t>(K)]);
            return Best;
        };

        // Skin: every triangle with a corner that is not deep under cloth.
        {
            FigurePart Skin;
            Skin.Mat = Solid(Tag + "_Skin", {1, 1, 1}, 0.55);
            Skin.Mat.BaseColorTexture = SkinTexture;
            Skin.Ground = true;
            Surface(Skin, {BodyGroup}, Any([&](const Attr& A) { return Opaque(A) < 0.12; }), Flat, false);
            Parts.push_back(std::move(Skin));
        }
        // Eyes (lashes are painted into the skin: the helper strips read as heavy liner).
        {
            FigurePart Eyes;
            Eyes.Mat = Solid(Tag + "_Eyes", {1, 1, 1}, 0.12);
            Eyes.Mat.BaseColorTexture = EyeTexture;
            Surface(Eyes, {Data.Group("helper-l-eye"), Data.Group("helper-r-eye")}, All, Flat, true);
            Parts.push_back(std::move(Eyes));
        }
        // Loose trouser legs and shirt sleeves (built below as tubes) hide the shell underneath.
        const bool LegsUnderSkirt = S.Dress.On && S.Dress.OpenFront <= 0.0;
        const bool LegTubes = S.Legs.On && !LegsUnderSkirt;
        const bool LegsTucked = S.Feet.On && S.Feet.LegStart < S.Legs.LegEnd;
        const double LegTubeEnd = LegsTucked ? S.Feet.LegStart + 0.12 : S.Legs.LegEnd;
        const bool ShirtOuterOnArms = S.Shirt.On && !(S.Coat.On && S.Coat.Sleeve > 0.5) && !(S.Vest.On && S.Vest.Sleeve > 0.5);
        const bool SleeveTubes = ShirtOuterOnArms && S.Shirt.Sleeve > 1.0 && S.Shirt.Fabric != Cloth::Wool;
        const auto UnderTube = [&](const int V)
        {
            const Anatomy& A = B.Parts[static_cast<std::size_t>(V)];
            if (LegTubes && A.Leg + A.Foot > 0.9 && A.LegT > 0.3 && A.LegT < LegTubeEnd - 0.1) return true;
            return SleeveTubes && A.Arm > 0.9 && A.ArmT > 0.35 && A.ArmT < S.Shirt.Sleeve - 0.15;
        };

        // Outfit shell: triangles touching cloth, lifted by the outermost garment's thickness.
        {
            const auto Thick = [&](int V)
            {
                const Attr& A = At[static_cast<std::size_t>(V)];
                double T = 0.0;
                for (int K = 0; K < SlotCount; ++K)
                {
                    double Th = 0.0;
                    if (K == HeadCloth) Th = S.Hat == HatStyle::KnitCap ? 0.16 : S.Hat == HatStyle::Headwrap ? 0.3 : 0.07;
                    else if (const Garment* G = SlotGarment(S, K); G && G->On) Th = G->Thick;
                    T = std::max(T, Th * SmoothStep(-0.12, 0.06, A.S[static_cast<std::size_t>(K)]));
                }
                // Hair under a head cloth lifts it.
                if (A.S[HeadCloth] > -0.1 && A.Hair > 0.0 && S.Hairdo != HairStyle::Bald && S.Hairdo != HairStyle::Shaved) T += 0.1 * SmoothStep(0.0, 0.3, A.Hair);
                return T;
            };
            // Drape: relax the lifted surface so cloth bridges hollows (under the bust, the small of the
            // back, between the toes) instead of clinging; it never sinks below its thickness.
            std::vector<V3> Draped(N);
            std::vector<double> Lift(N, 0.0);
            std::vector<std::uint8_t> Cloth(N, 0);
            for (std::size_t I = 0; I < N; ++I)
            {
                if (!B.IsBody[I]) continue;
                Lift[I] = Thick(static_cast<int>(I));
                Draped[I] = B.Rest[I] + B.Normals[I] * Lift[I];
                const Attr& A = At[I];
                double Best = -9.0;
                for (int K = 0; K < SlotCount; ++K) Best = std::max(Best, A.S[static_cast<std::size_t>(K)]);
                Cloth[I] = Best > 0.05 && A.Head < 0.5 && A.Hand < 0.5 ? 1 : 0;
            }
            // Footwear keeps relaxing much longer: a boot is a closed last, not a sock over the toes.
            for (int Pass = 0; Pass < 40; ++Pass)
            {
                const std::vector<V3> Src = Draped;
                for (std::size_t I = 0; I < N; ++I)
                {
                    if (!Cloth[I] || B.Neighbours[I].empty()) continue;
                    if (Pass >= 10 && !(B.Parts[I].Foot > 0.3 && At[I].S[Feet] > 0.0)) continue;
                    V3 Avg;
                    for (const int J : B.Neighbours[I]) Avg = Avg + Src[static_cast<std::size_t>(J)];
                    Avg = Avg * (1.0 / static_cast<double>(B.Neighbours[I].size()));
                    V3 P = Src[I] + (Avg - Src[I]) * 0.5;
                    const double Out = Dot(P - B.Rest[I], B.Normals[I]);
                    if (Out < Lift[I]) P = P + B.Normals[I] * (Lift[I] - Out);
                    Draped[I] = P;
                }
            }
            FigurePart Outfit;
            Outfit.Mat = Solid(Tag + "_Outfit", {1, 1, 1}, 0.8);
            Outfit.Mat.BaseColorTexture = OutfitTexture;
            Outfit.Mat.Mask = true;
            Outfit.Ground = true;
            const auto Touches = Any([&](const Attr& A) { double Best = -9.0; for (int K = 0; K < SlotCount; ++K) Best = std::max(Best, A.S[static_cast<std::size_t>(K)]); return Best > -0.02; });
            Surface(Outfit, {BodyGroup}, [&](int A, int Bv, int C) { return Touches(A, Bv, C) && !(UnderTube(A) && UnderTube(Bv) && UnderTube(C)); },
                [&](int V) { return Draped[static_cast<std::size_t>(V)] - B.Rest[static_cast<std::size_t>(V)]; }, false);
            if (!Outfit.Indices.empty()) Parts.push_back(std::move(Outfit));
        }
        // Hair and beard: layered shells with rising alpha cut-offs (sparser outwards).
        const auto Shells = [&](const bool Beard, const int Layers, const double Base, const double Step)
        {
            for (int L = 0; L < Layers; ++L)
            {
                FigurePart Shell;
                Shell.Mat = Solid(Tag + (Beard ? "_Beard" : "_Hair") + std::to_string(L), {1, 1, 1}, 0.72);
                Shell.Mat.BaseColorTexture = OutfitTexture;
                Shell.Mat.Mask = true;
                Shell.Mat.DoubleSided = true;
                Shell.Mat.AlphaCutoff = Layers == 1 ? 0.4 : 0.25 + 0.5 * L / (Layers - 1);
                const auto Offset = [&](int V)
                {
                    const Attr& A = At[static_cast<std::size_t>(V)];
                    const double Edge = Beard ? A.Beard : A.Hair;
                    double Lift = (Base + Step * L) * SmoothStep(-0.1, 0.35, Edge);
                    if (Beard)
                    {
                        const double Below = Clamp((B.Mouth.Y - A.P.Y) / std::max(0.2, B.Mouth.Y - B.Chin.Y + 0.3), 0.0, 1.2);
                        Lift *= 0.35 + Below * (S.Beard == BeardStyle::Full ? 1.6 : 0.8);
                    }
                    else
                    {
                        const double Top = Clamp((A.P.Y - B.EyeMid.Y) / std::max(0.2, B.HeadTop.Y - B.EyeMid.Y), 0.0, 1.0);
                        Lift *= (0.55 + 0.45 * Top) * S.HairVolume;
                        Lift *= 0.85 + 0.3 * Noise3(A.P * 2.5, Seed + 77u);
                    }
                    return Lift;
                };
                const auto Keep = [&](int A0, int A1, int A2)
                {
                    bool Some = false, Covered = false;
                    for (const int V : {A0, A1, A2})
                    {
                        const Attr& A = At[static_cast<std::size_t>(V)];
                        Some = Some || (Beard ? A.Beard : A.Hair) > -0.05;
                        Covered = Covered || A.S[HeadCloth] > 0.05;
                    }
                    return Some && !Covered;
                };
                Surface(Shell, {BodyGroup}, Keep, [&](int V) { return B.Normals[static_cast<std::size_t>(V)] * Offset(V); }, false);
                if (!Shell.Indices.empty()) Parts.push_back(std::move(Shell));
            }
        };
        switch (S.Hairdo)
        {
        case HairStyle::Bald: break;
        case HairStyle::Shaved: Shells(false, 1, 0.01, 0.0); break;
        case HairStyle::Cropped: Shells(false, 2, 0.02, 0.03); break;
        case HairStyle::Tonsure: Shells(false, 2, 0.02, 0.04); break;
        case HairStyle::Tousled: Shells(false, 3, 0.04, 0.1); break;
        default: Shells(false, 3, 0.03, 0.08); break;
        }
        switch (S.Beard)
        {
        case BeardStyle::None: case BeardStyle::Stubble: break;
        case BeardStyle::Mustache: Shells(true, 2, 0.02, 0.04); break;
        case BeardStyle::Trimmed: Shells(true, 3, 0.02, S.BeardLength * 0.4); break;
        case BeardStyle::Full: Shells(true, 3, 0.03, S.BeardLength * 0.45); break;
        }

        // ---- loose and rigid pieces
        std::vector<V3> TorsoPoints, LowerPoints, HeadPoints, NeckPoints;
        for (std::size_t I = 0; I < N; ++I)
        {
            if (!B.IsBody[I]) continue;
            const Anatomy& A = B.Parts[I];
            if (A.Torso + A.Leg + A.Foot > 0.6) LowerPoints.push_back(B.Rest[I]);
            if (A.Torso > 0.6) TorsoPoints.push_back(B.Rest[I]);
            if (A.Head > 0.6) HeadPoints.push_back(B.Rest[I]);
            if (A.Neck + A.Head > 0.5 && B.Rest[I].Y < B.Chin.Y) NeckPoints.push_back(B.Rest[I]);
        }
        const double Cz = (B.Hip[0].Z + B.Hip[1].Z) * 0.5;
        const auto Slice = [](const std::vector<V3>& Points, const double Y, const double Half)
        {
            std::vector<V3> Out;
            for (const V3& P : Points)
            {
                if (std::abs(P.Y - Y) <= Half) Out.push_back(P);
            }
            return Out;
        };
        const int Around = 48;
        const auto Dir = [](const double A) { return V3{Sin(A), 0.0, Cos(A)}; };
        const double OuterThick = std::max({S.Coat.On ? S.Coat.Thick : 0.0, S.Vest.On ? S.Vest.Thick : 0.0, S.Shirt.On ? S.Shirt.Thick : 0.0, S.Legs.On ? S.Legs.Thick : 0.0});

        /** Skirt-like hull from the torso fraction From down to leg parameter Hem. */
        const auto Hull = [&](FigurePart& Part, const Skirt& Sk, const double Ease)
        {
            const double Top = B.TorsoY(Sk.From), Bottom = LegY(B, Sk.Hem);
            const int Rows = std::max(3, static_cast<int>((Top - Bottom) / 0.35));
            const double Gap = Sk.OpenFront;
            const int Cols = Gap > 0.0 ? Around - static_cast<int>(Around * Gap / Pi) : Around;
            std::vector<std::vector<V3>> Rings;
            std::vector<double> Previous;
            for (int R = 0; R <= Rows; ++R)
            {
                const double T = static_cast<double>(R) / Rows;
                const double Y = Lerp(Top, Bottom, T);
                std::vector<V3> Pts = Slice(LowerPoints, Y, 0.25);
                if (Pts.empty()) Pts = Slice(LowerPoints, Y, 0.6);
                const std::vector<double> Support = SupportRing(Pts, 0.0, Cz, Around);
                std::vector<double> Radius(static_cast<std::size_t>(Around));
                for (int K = 0; K < Around; ++K)
                {
                    double Rr = Support[static_cast<std::size_t>(K)] + Ease + Sk.Thick;
                    if (!Previous.empty()) Rr = std::max(Rr, Previous[static_cast<std::size_t>(K)] + Sk.Flare * (Top - Bottom) / Rows);
                    Radius[static_cast<std::size_t>(K)] = Rr;
                }
                Previous = Radius;
                std::vector<V3> Ring;
                for (int K = 0; K < Cols; ++K)
                {
                    const double A = Gap > 0.0 ? Gap + (2.0 * Pi - 2.0 * Gap) * K / (Cols - 1) : 2.0 * Pi * K / Around;
                    const int Src = static_cast<int>(std::lround(A / (2.0 * Pi) * Around)) % Around;
                    const double Fold = 1.0 + 0.06 * T * Sin(A * 9.0 + 2.0 * Noise3(V3{A * 2.0, 0.0, 0.0}, Seed + 55u));
                    Ring.push_back(V3{0.0, Y, Cz} + Dir(A) * (Radius[static_cast<std::size_t>(Src)] * Fold));
                }
                Rings.push_back(std::move(Ring));
            }
            Builder Bd{&Part, &Data, &B};
            Bd.Grid(Rings, Gap <= 0.0, 2.0, 2.0);
        };

        const auto AddPart = [&](FigurePart&& Part) { if (!Part.Indices.empty()) Parts.push_back(std::move(Part)); };

        if (S.Dress.On)
        {
            FigurePart Skirt;
            Skirt.Mat = Tinted(Tag + "_Skirt", ClothTexture(S.Dress.Fabric), S.Dress.Color, 0.85);
            Skirt.Ground = true;
            Hull(Skirt, S.Dress, 0.05);
            AddPart(std::move(Skirt));
        }
        if (S.Tails.On)
        {
            FigurePart Tails;
            Tails.Mat = Tinted(Tag + "_Tails", ClothTexture(S.Tails.Fabric), S.Tails.Color, S.Tails.Fabric == Cloth::Leather ? 0.6 : 0.85);
            Hull(Tails, S.Tails, OuterThick + 0.05);
            AddPart(std::move(Tails));
        }
        if (S.Front.On)
        {
            // Apron: the front sector of a hull from the bib to the hem.
            FigurePart Apron;
            Apron.Mat = Tinted(Tag + "_Apron", ClothTexture(S.Front.Fabric), S.Front.Color, S.Front.Fabric == Cloth::Leather ? 0.55 : 0.85);
            const double Top = B.TorsoY(S.Front.Top), Bottom = LegY(B, S.Front.Hem);
            const int Rows = std::max(4, static_cast<int>((Top - Bottom) / 0.3));
            std::vector<std::vector<V3>> Rings;
            for (int R = 0; R <= Rows; ++R)
            {
                const double Y = Lerp(Top, Bottom, static_cast<double>(R) / Rows);
                std::vector<V3> Pts = Slice(LowerPoints, Y, 0.25);
                const std::vector<double> Support = SupportRing(Pts, 0.0, Cz, Around);
                const double Narrow = Y > B.TorsoY(0.35) ? 0.75 : 1.0;
                std::vector<V3> Ring;
                for (int K = 0; K <= 16; ++K)
                {
                    const double A = S.Front.HalfAngle * Narrow * (static_cast<double>(K) / 8.0 - 1.0);
                    const int Src = (static_cast<int>(std::lround(A / (2.0 * Pi) * Around)) + Around) % Around;
                    Ring.push_back(V3{0.0, Y, Cz} + Dir(A) * (Support[static_cast<std::size_t>(Src)] + OuterThick + 0.12));
                }
                Rings.push_back(std::move(Ring));
            }
            Builder Bd{&Apron, &Data, &B};
            Bd.Grid(Rings, false, 2.0, 2.0);
            AddPart(std::move(Apron));
        }

        // Loose sleeves and trouser legs: tubes around the limb's axis that hang straight below the
        // knee instead of clinging, eased more towards the hem (the cut of 1830s slops and linen shirts).
        const auto LimbTube = [&](const Garment& G, const bool Leg, const int Side, const double From, const double To,
                                  const double EaseTop, const double EaseBottom, const bool Straight, const bool Tuck, const std::string& Name,
                                  const bool CapEnd = false)
        {
            const V3 Chain[4] = {Leg ? B.Hip[Side] : B.Shoulder[Side], Leg ? B.Knee[Side] : B.Elbow[Side], Leg ? B.Ankle[Side] : B.Wrist[Side],
                Leg ? B.ToeTip[Side] : B.FingerTip[Side]};
            const auto At2 = [&](const double T)
            {
                const int Seg = std::clamp(static_cast<int>(std::floor(T)), 0, 2);
                return Chain[Seg] + (Chain[Seg + 1] - Chain[Seg]) * (T - Seg);
            };
            std::vector<V3> Limb;
            const double SideSign = Side == 0 ? 1.0 : -1.0;
            for (std::size_t I = 0; I < N; ++I)
            {
                if (!B.IsBody[I] || B.Parts[I].Side != SideSign) continue;
                const Anatomy& A = B.Parts[I];
                if (Leg ? (A.Leg + A.Foot > 0.5) : (A.Arm + A.Hand > 0.5)) Limb.push_back(B.Rest[I]);
            }
            const int Sides = 20;
            std::vector<std::vector<V3>> Rings;
            std::vector<double> Previous;
            const int Steps = std::max(3, static_cast<int>((To - From) / 0.08));
            for (int K = 0; K <= Steps; ++K)
            {
                const double T = From + (To - From) * K / Steps;
                const V3 C = At2(T);
                const V3 D = Normalize(At2(std::min(T + 0.05, 2.99)) - At2(std::max(T - 0.05, 0.0)));
                V3 U = V3{0, 0, 1} - D * D.Z;
                if (Length(U) < 1e-3) U = V3{1, 0, 0} - D * D.X;
                U = Normalize(U);
                const V3 W = Cross(D, U);
                std::vector<double> Radius(static_cast<std::size_t>(Sides), 0.2);
                for (const V3& P : Limb)
                {
                    const V3 R = P - C;
                    if (std::abs(Dot(R, D)) > 0.12) continue;
                    for (int A = 0; A < Sides; ++A)
                    {
                        const double Ang = 2.0 * Pi * A / Sides;
                        Radius[static_cast<std::size_t>(A)] = std::max(Radius[static_cast<std::size_t>(A)], Dot(R, U * Cos(Ang) + W * Sin(Ang)));
                    }
                }
                const double Frac2 = static_cast<double>(K) / Steps;
                double Ease = Lerp(EaseTop, EaseBottom, SmoothStep(0.0, 0.6, Frac2));
                if (Tuck) Ease *= 1.0 - 0.85 * SmoothStep(0.7, 1.0, Frac2);  // gathered into a boot or a cuff
                Ease += G.Thick;
                for (int A = 0; A < Sides; ++A)
                {
                    double& R = Radius[static_cast<std::size_t>(A)];
                    R += Ease;
                    if (Straight && !Tuck && !Previous.empty() && T > 1.0) R = std::max(R, Previous[static_cast<std::size_t>(A)] * 0.985);
                }
                Previous = Radius;
                std::vector<V3> Ring;
                for (int A = 0; A < Sides; ++A)
                {
                    const double Ang = 2.0 * Pi * A / Sides;
                    const double Fold = 1.0 + 0.05 * Frac2 * Sin(Ang * 5.0 + T * 7.0);
                    Ring.push_back(C + (U * Cos(Ang) + W * Sin(Ang)) * (Radius[static_cast<std::size_t>(A)] * Fold));
                }
                if (Dot(Cross(Ring[1] - Ring[0], D), Ring[0] - C) < 0.0) std::reverse(Ring.begin(), Ring.end());
                Rings.push_back(std::move(Ring));
            }
            FigurePart Tube;
            Tube.Mat = Tinted(Tag + "_" + Name, ClothTexture(G.Fabric), G.Color, G.Roughness);
            Builder Bd{&Tube, &Data, &B};
            Bd.Grid(Rings, true, 1.5, 1.5);
            if (CapEnd)
            {
                // Rounded toe: the last ring closes onto a point a little ahead of it.
                const std::vector<V3>& Last = Rings.back();
                V3 Centre;
                for (const V3& P : Last) Centre = Centre + P;
                Centre = Centre * (1.0 / static_cast<double>(Last.size()));
                const V3 Ahead = Normalize(Centre - At2(To - 0.1));
                const std::uint32_t Hub = Bd.Add(Centre + Ahead * 0.12, Ahead, 0.5, 0.5);
                std::vector<std::uint32_t> Rim;
                for (const V3& P : Last) Rim.push_back(Bd.Add(P, Normalize(P - Centre + Ahead * 0.5), 0.5, 0.5));
                for (std::size_t K = 0; K < Rim.size(); ++K)
                {
                    const std::uint32_t A = Rim[K], C = Rim[(K + 1) % Rim.size()];
                    if (Dot(Cross(Last[K] - Centre, Last[(K + 1) % Last.size()] - Centre), Ahead) > 0.0) Bd.Tri(A, C, Hub);
                    else Bd.Tri(C, A, Hub);
                }
            }
            Tube.Ground = Leg && CapEnd;
            if (!Tube.Indices.empty()) Parts.push_back(std::move(Tube));
        };
        if (LegTubes)
        {
            // Breeches gather at the knee band; trousers tucked into tall boots end above the boot top.
            const bool Breeches = S.Legs.LegEnd < 1.3;
            for (int Side = 0; Side < 2; ++Side)
            {
                LimbTube(S.Legs, true, Side, 0.12, LegTubeEnd, 0.02, Breeches ? 0.1 : 0.24, !Breeches, LegsTucked || Breeches, Side == 0 ? "LegL" : "LegR");
            }
        }
        // Footwear uppers: a closed last from the ankle to the toe tip over the (draped) foot.
        if (S.Feet.On)
        {
            for (int Side = 0; Side < 2; ++Side)
            {
                LimbTube(S.Feet, true, Side, 1.9, 2.97, 0.05, 0.08, false, false, Side == 0 ? "UpperL" : "UpperR", true);
            }
        }
        if (SleeveTubes)
        {
            for (int Side = 0; Side < 2; ++Side)
            {
                LimbTube(S.Shirt, false, Side, 0.22, S.Shirt.Sleeve - 0.04, 0.04, 0.2, false, S.Shirt.Sleeve > 1.8, Side == 0 ? "SleeveL" : "SleeveR");
            }
        }

        // Belt and sash.
        for (const Band* Bt : {&S.Belt, &S.Sash})
        {
            if (!Bt->On) continue;
            const bool IsSash = Bt == &S.Sash;
            FigurePart Belt;
            Belt.Mat = Tinted(Tag + (IsSash ? "_Sash" : "_Belt"), IsSash ? "T_Hum_Cloth_BaseColor.png" : "T_Hum_Leather_BaseColor.png", Bt->Color, IsSash ? 0.85 : 0.5);
            const double Y = B.TorsoY(Bt->At);
            const std::vector<double> Support = SupportRing(Slice(TorsoPoints, Y, 0.3), 0.0, Cz, Around);
            std::vector<std::vector<V3>> Rings;
            for (const double Dy : {-0.5, -0.45, 0.45, 0.5})
            {
                std::vector<V3> Ring;
                for (int K = 0; K < Around; ++K)
                {
                    const double A = 2.0 * Pi * K / Around;
                    const double Lift = std::abs(Dy) > 0.46 ? 0.0 : 0.05;
                    Ring.push_back(V3{0.0, Y + Dy * Bt->Height, Cz} + Dir(A) * (Support[static_cast<std::size_t>(K)] + OuterThick + 0.04 + Lift));
                }
                Rings.push_back(std::move(Ring));
            }
            std::reverse(Rings.begin(), Rings.end());
            Builder Bd{&Belt, &Data, &B};
            Bd.Grid(Rings, true, 2.0, 1.0);
            if (Bt->Knot)
            {
                const double A = 1.3;
                const V3 At0 = V3{0.0, Y, Cz} + Dir(A) * (Support[static_cast<std::size_t>(std::lround(A / (2.0 * Pi) * Around)) % Around] + OuterThick + 0.15);
                Bd.Sphere(At0, V3{0.2, 0.22, 0.18}, 6, 10);
                for (const double Spread : {-0.08, 0.1})
                {
                    std::vector<V3> Path;
                    std::vector<double> Radius;
                    for (int K = 0; K <= 6; ++K)
                    {
                        const double T = K / 6.0;
                        Path.push_back(At0 + V3{Spread + 0.1 * T, -2.4 * T, 0.12 * T * (Spread > 0 ? 1 : -1)});
                        Radius.push_back(0.18 - 0.03 * T);
                    }
                    Bd.Tube(Path, Radius, 6, 0.25, V3{0, 0, 1});
                }
            }
            AddPart(std::move(Belt));
            if (Bt->Buckle && !IsSash)
            {
                FigurePart Buckle;
                Buckle.Mat = Solid(Tag + "_Buckle", Bt->BuckleColor, 0.35, 0.9);
                Builder Bk{&Buckle, &Data, &B};
                const V3 Front = V3{0.0, Y, Cz + Support[0] + OuterThick + 0.12};
                Bk.Box(Front, V3{0.3, 0, 0}, V3{0, Bt->Height * 0.6, 0}, V3{0, 0, 0.04});
                AddPart(std::move(Buckle));
            }
        }

        // Boot cuffs (tall boots) and a turned-down coat collar.
        if (S.Feet.On && S.Feet.LegStart < 1.6)
        {
            FigurePart Cuffs;
            Cuffs.Mat = Tinted(Tag + "_Cuffs", "T_Hum_Leather_BaseColor.png", S.Feet.Color, S.Feet.Roughness);
            Builder Bd{&Cuffs, &Data, &B};
            for (int Side = 0; Side < 2; ++Side)
            {
                const double Y = LegY(B, S.Feet.LegStart);
                const double T = Clamp(S.Feet.LegStart - 1.0, 0.0, 1.0);
                const V3 Axis = B.Knee[Side] + (B.Ankle[Side] - B.Knee[Side]) * T;
                std::vector<V3> Pts;
                for (std::size_t I = 0; I < N; ++I)
                {
                    if (B.IsBody[I] && B.Parts[I].Leg > 0.6 && B.Parts[I].Side == (Side == 0 ? 1.0 : -1.0) && std::abs(B.Rest[I].Y - Y) < 0.2) Pts.push_back(B.Rest[I]);
                }
                const std::vector<double> Support = SupportRing(Pts, Axis.X, Axis.Z, 24);
                std::vector<std::vector<V3>> Rings;
                for (const auto& [Dy, Grow] : {std::pair{0.3, 0.2}, std::pair{0.05, 0.1}, std::pair{-0.25, 0.06}})
                {
                    std::vector<V3> Ring;
                    for (int K = 0; K < 24; ++K)
                    {
                        const double A = 2.0 * Pi * K / 24;
                        Ring.push_back(V3{Axis.X, Y + Dy, Axis.Z} + Dir(A) * (Support[static_cast<std::size_t>(K)] + S.Feet.Thick + Grow));
                    }
                    Rings.push_back(std::move(Ring));
                }
                Bd.Grid(Rings, true, 1.0, 1.0);
            }
            AddPart(std::move(Cuffs));
        }
        if (S.Coat.On && S.Coat.NeckFront > 0.0)
        {
            FigurePart Collar;
            Collar.Mat = Tinted(Tag + "_Collar", ClothTexture(S.Coat.Fabric), S.Coat.Trimmed ? MixRgb(S.Coat.Color, S.Coat.Trim, 0.25) : S.Coat.Color * 0.92, std::max(0.8, S.Coat.Roughness));
            const std::vector<double> Support = SupportRing(NeckPoints.empty() ? TorsoPoints : NeckPoints, 0.0, (B.Shoulder[0].Z + B.Shoulder[1].Z) * 0.5, Around);
            const double Gap = 0.55;
            std::vector<std::vector<V3>> Rings;
            for (const auto& [Dy, Grow] : {std::pair{0.45, 0.12}, std::pair{0.3, 0.22}, std::pair{-0.15, 0.5}})
            {
                std::vector<V3> Ring;
                for (int K = 0; K <= 36; ++K)
                {
                    const double A = Gap + (2.0 * Pi - 2.0 * Gap) * K / 36;
                    const int Src = static_cast<int>(std::lround(A / (2.0 * Pi) * Around)) % Around;
                    Ring.push_back(V3{0.0, B.NeckY + Dy, (B.Shoulder[0].Z + B.Shoulder[1].Z) * 0.5} + Dir(A) * (Support[static_cast<std::size_t>(Src)] + Grow));
                }
                Rings.push_back(std::move(Ring));
            }
            Builder Bd{&Collar, &Data, &B};
            Bd.Grid(Rings, false, 1.0, 1.0);
            AddPart(std::move(Collar));
        }

        // Soles (and heels) under footwear.
        if (S.Feet.On)
        {
            FigurePart Soles;
            Soles.Mat = Tinted(Tag + "_Soles", "T_Hum_Leather_BaseColor.png", S.Feet.Color * 0.55, 0.7);
            Soles.Ground = true;
            Builder Bd{&Soles, &Data, &B};
            const bool Heeled = S.Feet.LegStart < 1.9;
            for (int Side = 0; Side < 2; ++Side)
            {
                std::vector<V3> Bottom;
                double MinY = 1e9;
                for (std::size_t I = 0; I < N; ++I)
                {
                    if (!B.IsBody[I] || B.Parts[I].Foot < 0.5 || B.Parts[I].Side != (Side == 0 ? 1.0 : -1.0)) continue;
                    MinY = std::min(MinY, B.Rest[I].Y);
                    if (B.Normals[I].Y < -0.3) Bottom.push_back(B.Rest[I]);
                }
                if (Bottom.size() < 3) continue;
                std::vector<V3> Hull = HullXZ(Bottom);
                V3 C;
                for (const V3& P : Hull) C = C + P;
                C = C * (1.0 / static_cast<double>(Hull.size()));
                const double Top = MinY + 0.08, Base = MinY - 0.18;
                const double HeelZ = B.Ankle[Side].Z + 0.15;
                std::vector<V3> Upper, Lower;
                for (const V3& P : Hull)
                {
                    const V3 Out = Normalize(V3{P.X - C.X, 0.0, P.Z - C.Z}) * (S.Feet.Thick + 0.06);
                    const double Heel = Heeled && P.Z < HeelZ ? 0.22 : 0.0;
                    Upper.push_back(V3{P.X + Out.X, Top, P.Z + Out.Z});
                    Lower.push_back(V3{P.X + Out.X, Base - Heel, P.Z + Out.Z});
                }
                std::reverse(Upper.begin(), Upper.end());
                std::reverse(Lower.begin(), Lower.end());
                Bd.Grid({Upper, Lower}, true, 1.0, 1.0);
                const std::uint32_t Hub = Bd.Add(V3{C.X, Base - (Heeled ? 0.11 : 0.0), C.Z}, V3{0, -1, 0}, 0.5, 0.5);
                std::vector<std::uint32_t> Rim;
                for (const V3& P : Lower) Rim.push_back(Bd.Add(P, V3{0, -1, 0}, 0.5, 0.5));
                for (std::size_t K = 0; K < Rim.size(); ++K) Bd.Tri(Hub, Rim[(K + 1) % Rim.size()], Rim[K]);
            }
            AddPart(std::move(Soles));
        }

        // Hair and beard cards: strips of strands rooted in the scalp and jaw, following the style's flow.
        const auto Cards = [&](const bool Beard)
        {
            FigurePart Part;
            Part.Mat = Tinted(Tag + (Beard ? "_BeardCards" : "_HairCards"), "T_Hum_HairCard_BaseColor.png", MixRgb(S.Hair, Rgb{0.7, 0.69, 0.66}, S.Gray * (S.Gray > 0.8 ? 0.6 : 0.25)), 0.7);
            Part.Mat.Mask = true;
            Part.Mat.AlphaCutoff = 0.4;
            Builder Bd{&Part, &Data, &B};
            const double HatBand = (S.Hat == HatStyle::Tricorne || S.Hat == HatStyle::Bicorne || S.Hat == HatStyle::Straw) ? B.EyeMid.Y + 0.5 : 1e9;
            const bool Combed = S.Hairdo == HairStyle::SlickedBack || S.Hairdo == HairStyle::TiedBack || S.Hairdo == HairStyle::Braid || S.Hairdo == HairStyle::PinnedUp;
            double Reach = 0.8, Density = 0.22, Scatter = 0.35;
            if (Beard) { Reach = std::max(0.3, S.BeardLength * 2.2); Density = 0.35; Scatter = 0.25; }
            else if (S.Hairdo == HairStyle::Short) { Reach = 0.55; Density = 0.2; }
            else if (S.Hairdo == HairStyle::Tousled) { Reach = 1.05; Density = 0.28; Scatter = 0.8; }
            else if (S.Hairdo == HairStyle::LongLoose) { Reach = S.HairLength; Density = 0.3; Scatter = 0.2; }
            else if (Combed) { Reach = 0.9; Density = 0.2; Scatter = 0.15; }
            const V3 Crown = V3{B.HeadCentre.X, B.HeadTop.Y, B.HeadCentre.Z - 0.2};
            for (std::size_t I = 0; I < N; ++I)
            {
                if (!B.IsBody[I]) continue;
                const Attr& A = At[I];
                const double Edge = Beard ? A.Beard : A.Hair;
                if (Edge < 0.12 || A.S[HeadCloth] > -0.05) continue;
                if (Beard && A.P.Y > B.Mouth.Y + 0.05) continue;
                if (!Beard && A.P.Y > HatBand) continue;
                if (Hash3(static_cast<int>(I), 17, 3, Seed + (Beard ? 5u : 1u)) > Density) continue;
                const V3 Normal = B.Normals[I];
                V3 Flow = Beard ? V3{0.0, -1.0, 0.25} : Combed ? V3{0.0, -0.35, -1.0} : Normalize(A.P - Crown);
                const double Jx = Hash3(static_cast<int>(I), 1, 9, Seed) - 0.5, Jy = Hash3(static_cast<int>(I), 2, 9, Seed) - 0.5, Jz = Hash3(static_cast<int>(I), 3, 9, Seed) - 0.5;
                Flow = Flow + V3{Jx, Jy * 0.5, Jz} * Scatter;
                Flow = Flow - Normal * Dot(Flow, Normal);
                if (Reach > 1e-9 && Dot(Flow, Flow) < 1e-6) Flow = V3{0, -1, 0};
                Flow = Normalize(Flow);
                const double Len = Reach * (0.7 + 0.6 * Hash3(static_cast<int>(I), 4, 9, Seed));
                const double Lift = (Beard ? 0.03 : 0.06 * S.HairVolume) + 0.02;
                const V3 Width = Normalize(Cross(Flow, Normal)) * (Beard ? 0.09 : 0.13);
                std::vector<std::vector<V3>> Rows;
                V3 P = A.P + Normal * Lift, Dir = Flow;
                const double RootRadius = Length(A.P - B.HeadCentre);
                for (int K = 0; K <= 4; ++K)
                {
                    Rows.push_back({P - Width, P + Width});
                    Dir = Normalize(Dir + V3{0.0, Beard ? -0.25 : -0.35, 0.0});
                    P = P + Dir * (Len / 4.0);
                    // Stay off the scalp while above the ears.
                    if (!Beard && P.Y > B.EyeMid.Y - 0.3)
                    {
                        const V3 D = P - B.HeadCentre;
                        const double R = Length(D);
                        const double Want = RootRadius + Lift * (0.8 + 0.1 * K);
                        if (R < Want) P = B.HeadCentre + D * (Want / R);
                    }
                }
                Bd.Grid(Rows, false, 0.26, Len);
                // Card UVs: u across, v root-to-tip.
                const std::size_t First = Part.Vertices.size() - 10;
                for (std::size_t K = 0; K < 10; ++K)
                {
                    Part.Vertices[First + K].U = static_cast<double>(K % 2);
                    Part.Vertices[First + K].V = static_cast<double>(K / 2) / 4.0;
                }
            }
            Part.Mat.DoubleSided = true;
            AddPart(std::move(Part));
        };
        if (S.Hairdo != HairStyle::Bald && S.Hairdo != HairStyle::Shaved && S.Hairdo != HairStyle::Cropped && S.Hairdo != HairStyle::Tonsure) Cards(false);
        if (S.Beard == BeardStyle::Full) Cards(true);

        // Hanging hair: braid, queue, bun.
        if (S.Hairdo == HairStyle::Braid || S.Hairdo == HairStyle::TiedBack || S.Hairdo == HairStyle::PinnedUp || S.Hairdo == HairStyle::LongLoose)
        {
            FigurePart Hang;
            Hang.Mat = Tinted(Tag + "_HairHang", "T_Hum_Strands_BaseColor.png", MixRgb(S.Hair, Rgb{0.75, 0.74, 0.7}, S.Gray * 0.6), 0.6);
            Builder Bd{&Hang, &Data, &B};
            const V3 Nape{B.HeadCentre.X, B.EyeMid.Y - 0.35, B.HeadCentre.Z - 0.85};
            if (S.Hairdo == HairStyle::PinnedUp)
            {
                Bd.Sphere(Nape + V3{0.0, 0.45, -0.12}, V3{0.42, 0.34, 0.3}, 8, 14);
            }
            else
            {
                const bool Braid = S.Hairdo == HairStyle::Braid;
                const double Length = S.HairLength;
                std::vector<V3> Path;
                std::vector<double> Radius;
                const int Steps = std::max(6, static_cast<int>(Length / 0.12));
                for (int K = 0; K <= Steps; ++K)
                {
                    const double T = static_cast<double>(K) / Steps;
                    const double Y = Nape.Y - Length * T;
                    // Follow the back: furthest-back body point at this height, plus clearance.
                    double BackZ = Nape.Z;
                    for (const V3& P : Slice(TorsoPoints, Y, 0.2)) BackZ = std::min(BackZ, P.Z);
                    const double Z = std::min(Nape.Z - 0.05 * T, BackZ - (OuterThick + 0.18));
                    Path.push_back(V3{Nape.X + 0.03 * Sin(T * 6.0), Y, Z});
                    double R = Braid ? Lerp(0.2, 0.1, T) * (1.0 + 0.28 * std::abs(Sin(T * Length * 6.0))) : Lerp(0.16, 0.08, T);
                    if (S.Hairdo == HairStyle::LongLoose) R = Lerp(0.5, 0.3, T);
                    Radius.push_back(R);
                }
                Bd.Tube(Path, Radius, Braid ? 8 : 10, S.Hairdo == HairStyle::LongLoose ? 0.35 : 1.0, V3{1, 0, 0});
                if (!Braid)
                {
                    FigurePart Ribbon;
                    Ribbon.Mat = Solid(Tag + "_Ribbon", Rgb{0.08, 0.07, 0.07}, 0.7);
                    Builder Rb{&Ribbon, &Data, &B};
                    Rb.Tube({Path[1] + V3{0, 0.05, 0}, Path[1] - V3{0, 0.1, 0}}, {Radius[1] * 1.15, Radius[1] * 1.15}, 10, 1.0, V3{1, 0, 0});
                    AddPart(std::move(Ribbon));
                }
            }
            AddPart(std::move(Hang));
        }

        // Hats with brims.
        if (S.Hat == HatStyle::Tricorne || S.Hat == HatStyle::Bicorne || S.Hat == HatStyle::Straw)
        {
            const double BandY = B.EyeMid.Y + (S.Hat == HatStyle::Straw ? 0.55 : 0.6);
            const std::vector<double> Head = SupportRing(Slice(HeadPoints, BandY, 0.15), B.HeadCentre.X, B.HeadCentre.Z, Around);
            const bool Straw = S.Hat == HatStyle::Straw;
            FigurePart Hat;
            Hat.Mat = Straw ? Tinted(Tag + "_Hat", "T_Hum_Straw_BaseColor.png", S.HatColor, 0.9) : Tinted(Tag + "_Hat", "T_Hum_Cloth_BaseColor.png", S.HatColor, 0.9);
            const double Lift = (S.Hairdo == HairStyle::Bald || S.Hairdo == HairStyle::Shaved) ? 0.03 : 0.12;
            std::vector<std::vector<V3>> Crown;
            for (int R = 0; R <= 5; ++R)
            {
                const double T = R / 5.0;
                std::vector<V3> Ring;
                for (int K = 0; K < Around; ++K)
                {
                    const double A = 2.0 * Pi * K / Around;
                    const double Shrink = Straw ? (1.0 - 0.18 * T * T) : (1.0 - 0.3 * T * T * T);
                    Ring.push_back(V3{B.HeadCentre.X, BandY + T * (Straw ? 0.95 : 1.05), B.HeadCentre.Z} + Dir(A) * ((Head[static_cast<std::size_t>(K)] + Lift) * Shrink));
                }
                Crown.push_back(std::move(Ring));
            }
            std::reverse(Crown.begin(), Crown.end());
            Builder Bd{&Hat, &Data, &B};
            Bd.Grid(Crown, true, 2.0, 2.0);
            // Top cap.
            {
                const std::vector<V3>& Top = Crown.front();
                V3 Centre;
                for (const V3& P : Top) Centre = Centre + P;
                Centre = Centre * (1.0 / Around) + V3{0, 0.04, 0};
                const std::uint32_t Hub = Bd.Add(Centre, V3{0, 1, 0}, 0.5, 0.5);
                std::vector<std::uint32_t> Rim;
                for (const V3& P : Top) Rim.push_back(Bd.Add(P, V3{0, 1, 0}, 0.5 + (P.X - Centre.X) * 0.5, 0.5 + (P.Z - Centre.Z) * 0.5));
                for (int K = 0; K < Around; ++K) Bd.Tri(Hub, Rim[static_cast<std::size_t>(K)], Rim[static_cast<std::size_t>((K + 1) % Around)]);
            }
            // Brim: from the band out to the outline, turned up by Wall(angle).
            std::vector<std::vector<V3>> Brim;
            for (int R = 0; R <= 4; ++R)
            {
                const double T = R / 4.0;
                std::vector<V3> Ring;
                for (int K = 0; K < Around; ++K)
                {
                    const double A = 2.0 * Pi * K / Around;
                    double Outline, Wall;
                    if (S.Hat == HatStyle::Tricorne)
                    {
                        Outline = 1.62 * (1.0 + 0.2 * Cos(3.0 * A));
                        Wall = 0.62 + 0.12 * Cos(3.0 * A);
                    }
                    else if (S.Hat == HatStyle::Bicorne)
                    {
                        const double Side = std::abs(Sin(A));
                        Outline = Lerp(1.0, 2.35, Side * Side);
                        Wall = Lerp(1.25, 0.2, Side * Side);
                    }
                    else
                    {
                        Outline = 2.25;
                        Wall = -0.12;
                    }
                    const double Inner = Head[static_cast<std::size_t>(K)] + Lift;
                    const double Radius = Lerp(Inner, std::max(Outline, Inner + 0.3), std::min(1.0, T * 1.6));
                    const double Up = Straw ? Wall * T * T : Wall * SmoothStep(0.45, 1.0, T);
                    Ring.push_back(V3{B.HeadCentre.X, BandY + Up, B.HeadCentre.Z} + Dir(A) * Radius);
                }
                Brim.push_back(std::move(Ring));
            }
            Bd.Grid(Brim, true, 2.0, 2.0);
            if (S.HatTrimmed)
            {
                FigurePart Trim;
                Trim.Mat = Solid(Tag + "_HatTrim", S.HatTrim, 0.4, 0.6);
                Builder Tb{&Trim, &Data, &B};
                std::vector<std::vector<V3>> Edge{Brim[3], Brim[4]};
                for (auto& Ring : Edge)
                {
                    for (V3& P : Ring) P.Y += 0.015;
                }
                Tb.Grid(Edge, true, 1.0, 1.0);
                AddPart(std::move(Trim));
            }
            Hat.Mat.DoubleSided = true;
            AddPart(std::move(Hat));
        }
        if (S.Hat == HatStyle::KnitCap || S.Hat == HatStyle::Headscarf || S.Hat == HatStyle::Headwrap)
        {
            // Rolled rim (knit cap) or knot at the back (scarves).
            FigurePart Extra;
            Extra.Mat = Tinted(Tag + "_HeadCloth", "T_Hum_Cloth_BaseColor.png", S.HatColor, 0.9);
            Builder Bd{&Extra, &Data, &B};
            if (S.Hat == HatStyle::KnitCap)
            {
                std::vector<V3> Path;
                std::vector<double> Radius;
                for (int K = 0; K <= Around; ++K)
                {
                    const double A = 2.0 * Pi * K / Around;
                    const double Y = B.EyeMid.Y + Lerp(0.42, -0.2, (1.0 - Cos(A)) * 0.5) + 0.02;
                    const std::vector<V3> Pts = Slice(HeadPoints, Y, 0.12);
                    double R = 0.3;
                    for (const V3& P : Pts) R = std::max(R, (P.X - B.HeadCentre.X) * Sin(A) + (P.Z - B.HeadCentre.Z) * Cos(A));
                    Path.push_back(V3{B.HeadCentre.X, Y, B.HeadCentre.Z} + Dir(A) * (R + 0.16));
                    Radius.push_back(0.1);
                }
                Bd.Tube(Path, Radius, 8, 1.0, V3{0, 1, 0});
            }
            else
            {
                const V3 Knot{B.HeadCentre.X, B.EyeMid.Y - 0.1, B.HeadCentre.Z - 1.0};
                Bd.Sphere(Knot, V3{0.18, 0.15, 0.14}, 6, 10);
                for (const double Spread : {-0.12, 0.12})
                {
                    std::vector<V3> Path;
                    std::vector<double> Radius;
                    for (int K = 0; K <= 4; ++K)
                    {
                        const double T = K / 4.0;
                        Path.push_back(Knot + V3{Spread * (1.0 + T), -1.1 * T, -0.15 * T});
                        Radius.push_back(Lerp(0.14, 0.08, T));
                    }
                    Bd.Tube(Path, Radius, 6, 0.3, V3{0, 0, 1});
                }
            }
            AddPart(std::move(Extra));
        }

        // Gear.
        if (S.Cutlass || S.Pistol)
        {
            const double Y = B.TorsoY(S.Belt.On ? S.Belt.At : 0.3);
            const std::vector<double> Support = SupportRing(Slice(TorsoPoints, Y, 0.3), 0.0, Cz, Around);
            const auto Surf = [&](const double A) { return V3{0.0, Y, Cz} + Dir(A) * (Support[static_cast<std::size_t>((std::lround(A / (2.0 * Pi) * Around) % Around + Around) % Around)] + OuterThick + 0.15); };
            FigurePart Leather, Steel, Brass, Wood;
            Leather.Mat = Tinted(Tag + "_Scabbard", "T_Hum_Leather_BaseColor.png", Rgb{0.16, 0.1, 0.07}, 0.5);
            Steel.Mat = Solid(Tag + "_Steel", Rgb{0.55, 0.55, 0.56}, 0.35, 1.0);
            Brass.Mat = Solid(Tag + "_Brass", Rgb{0.72, 0.56, 0.28}, 0.35, 1.0);
            Wood.Mat = Solid(Tag + "_Wood", Rgb{0.32, 0.2, 0.12}, 0.6);
            Builder Lb{&Leather, &Data, &B}, Sb{&Steel, &Data, &B}, Bb{&Brass, &Data, &B}, Wb{&Wood, &Data, &B};
            if (S.Cutlass)
            {
                const V3 Hang = Surf(1.75) + V3{0.08, 0.0, 0.0};
                const V3 Down = Normalize(V3{0.12, -1.0, -0.55});
                std::vector<V3> Path;
                std::vector<double> Radius;
                for (int K = 0; K <= 8; ++K)
                {
                    const double T = K / 8.0;
                    Path.push_back(Hang + Down * (6.6 * T) + V3{0.0, 0.0, -0.35 * T * T});
                    Radius.push_back(Lerp(0.2, 0.12, T));
                }
                Lb.Tube(Path, Radius, 8, 0.3, V3{1, 0, 0});
                const V3 Up = Normalize(V3{-0.05, 1.0, 0.75});
                const V3 GripA = Hang + Up * 0.15, GripB = Hang + Up * 1.05;
                Wb.Tube({GripA, GripB}, {0.085, 0.075}, 8, 1.0, V3{1, 0, 0});
                Bb.Sphere(GripA + Up * 0.05, V3{0.34, 0.16, 0.3}, 6, 12);
                Bb.Sphere(GripB + Up * 0.06, V3{0.09, 0.09, 0.09}, 5, 8);
            }
            if (S.Pistol)
            {
                const V3 At0 = Surf(-0.45);
                const V3 Along = Normalize(V3{-0.35, -1.0, 0.1});
                Sb.Tube({At0, At0 + Along * 2.4}, {0.07, 0.06}, 8, 1.0, V3{0, 0, 1});
                Wb.Box(At0 + V3{0.05, 0.35, 0.08}, V3{0.1, 0, 0}, V3{0, 0.45, 0.1}, V3{0, -0.05, 0.12});
            }
            AddPart(std::move(Leather));
            AddPart(std::move(Steel));
            AddPart(std::move(Brass));
            AddPart(std::move(Wood));
        }
        if (S.Necklace || S.Cross)
        {
            FigurePart Cord;
            Cord.Mat = Solid(Tag + (S.Cross ? "_Rosary" : "_Chain"), S.Cross ? Rgb{0.3, 0.2, 0.12} : Rgb{0.7, 0.62, 0.45}, 0.5, S.Cross ? 0.0 : 0.9);
            Builder Bd{&Cord, &Data, &B};
            const double Cs = (B.Shoulder[0].Z + B.Shoulder[1].Z) * 0.5;
            const std::vector<double> Support = SupportRing(Slice(TorsoPoints, B.NeckY - 0.25, 0.25), 0.0, Cs, Around);
            std::vector<V3> Path;
            std::vector<double> Radius;
            const double DropFront = S.Cross ? 1.4 : 0.9;
            for (int K = 0; K <= Around; ++K)
            {
                const double A = 2.0 * Pi * K / Around;
                const double Drop = DropFront * SmoothStep(0.0, 1.0, (1.0 + Cos(A)) * 0.5);
                double Rr = Support[static_cast<std::size_t>(K % Around)] + OuterThick + 0.05;
                // Follow the chest front where the cord hangs lower.
                if (Drop > 0.2)
                {
                    double Front = -1e9;
                    for (const V3& P : Slice(TorsoPoints, B.NeckY - 0.1 - Drop, 0.2)) Front = std::max(Front, P.Z - Cs);
                    if (Front > -1e8) Rr = std::max(Rr * (1.0 - 0.3 * Drop), Front + OuterThick + 0.06);
                }
                Path.push_back(V3{0.0, B.NeckY - 0.05 - Drop, Cs} + Dir(A) * Rr);
                Radius.push_back(0.018);
            }
            Bd.Tube(Path, Radius, 5, 1.0, V3{0, 1, 0});
            const V3 Pendant = Path.front() + V3{0.0, -0.18, 0.02};
            if (S.Cross)
            {
                Bd.Box(Pendant, V3{0.03, 0, 0}, V3{0, 0.2, 0}, V3{0, 0, 0.02});
                Bd.Box(Pendant + V3{0, 0.07, 0}, V3{0.12, 0, 0}, V3{0, 0.03, 0}, V3{0, 0, 0.02});
            }
            else
            {
                Bd.Sphere(Pendant, V3{0.06, 0.06, 0.02}, 4, 8);
            }
            AddPart(std::move(Cord));
        }
        return true;
    }

    // ------------------------------------------------------------------ public entry points

    namespace
    {
        /** MakeHuman frame (dm) to O3DE world (m). */
        V3 ToWorld(const V3& P) { return V3{P.Z * 0.1, P.X * 0.1, P.Y * 0.1}; }
        V3 ToWorldDir(const V3& N) { return V3{N.Z, N.X, N.Y}; }
    }

    bool HumanFactory::BuildFigure(const std::string& PersonId, Mesh& Out, std::vector<ArtFile>& Files, std::string& Problem)
    {
        const PersonSpec* Spec = FindPerson(PersonId);
        if (!Spec)
        {
            Problem = "unknown person " + PersonId;
            return false;
        }
        if (!P->Ensure(Problem)) return false;
        if (!P->TexturesDone.count("shared"))
        {
            for (ArtFile& F : SharedTextures()) Files.push_back(std::move(F));
            P->TexturesDone.insert("shared");
        }
        Body B;
        std::vector<FigurePart> Parts;
        if (!P->Dress(*Spec, B, Parts, Files, Problem)) return false;
        const Motion* Clip = LoadMotion(P->Reader, P->Data, Spec->Clip, Problem);
        if (!Clip) return false;
        const int Frame = std::clamp(static_cast<int>(std::lround(Spec->Time / Clip->FrameTime)), 1, static_cast<int>(Clip->Frames.size()) - 1);
        const Pose Posed = PoseFromMotion(P->Data, B, *Clip, Frame, Frame, Spec->Upright, 0.35);

        std::vector<std::vector<Vertex>> Verts(Parts.size());
        double Ground = 1e30;
        for (std::size_t I = 0; I < Parts.size(); ++I)
        {
            for (const SkinVertex& V : Parts[I].Vertices)
            {
                Vertex W;
                W.P = ToWorld(SkinPoint(B, Posed, V));
                W.N = ToWorldDir(SkinNormal(Posed, V));
                W.U = V.U;
                W.V = V.V;
                if (Parts[I].Ground) Ground = std::min(Ground, W.P.Z);
                Verts[I].push_back(W);
            }
        }
        for (std::size_t I = 0; I < Parts.size(); ++I)
        {
            for (Vertex& V : Verts[I]) V.P.Z -= Ground;
            Out.Indexed(Parts[I].Mat, Verts[I], Parts[I].Indices);
        }
        return true;
    }

    bool HumanFactory::BuildActor(const std::string& PersonId, std::string& OutGltf, std::vector<ArtFile>& Files, std::string& Problem)
    {
        const PersonSpec* Spec = FindPerson(PersonId);
        if (!Spec)
        {
            Problem = "unknown person " + PersonId;
            return false;
        }
        if (!P->Ensure(Problem)) return false;
        Body B;
        std::vector<FigurePart> Parts;
        if (!P->Dress(*Spec, B, Parts, Files, Problem)) return false;
        OutGltf = WriteActorGltf(P->Data, B, "SK_Art_" + Spec->Id, Parts);
        return true;
    }

    bool HumanFactory::BuildMotion(const std::string& PersonId, const std::string& ClipId, std::string& OutGltf, std::string& Problem)
    {
        const PersonSpec* Spec = FindPerson(PersonId);
        if (!Spec)
        {
            Problem = "unknown person " + PersonId;
            return false;
        }
        if (!P->Ensure(Problem)) return false;
        Body B;
        if (!BuildBody(P->Reader, P->Data, *Spec, B, Problem)) return false;
        const Motion* Clip = LoadMotion(P->Reader, P->Data, ClipId, Problem);
        if (!Clip) return false;
        // Six seconds from the first second on (the excerpts run up to 20 s at 30 fps).
        const int Last = static_cast<int>(Clip->Frames.size()) - 1;
        const int First = std::min(30, Last);
        OutGltf = WriteMotionGltf(P->Data, B, "AN_Art_Human_" + ClipId, *Clip, First, std::min(Last, First + 180));
        return true;
    }
}
