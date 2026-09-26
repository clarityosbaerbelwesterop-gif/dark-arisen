#include "ArtKitLibrary.h"

#include <algorithm>
#include <cmath>

namespace DarkArisen::Tools::Art
{
    // ------------------------------------------------------------------ shared helpers

    Ring Ellipse(const double Cx, const double Cy, const double Z, const double Rx, const double Ry, const int Sides)
    {
        Ring R;
        for (int I = 0; I < Sides; ++I)
        {
            const double A = 2.0 * Pi * I / Sides;
            R.push_back({Cx + Rx * Cos(A), Cy + Ry * Sin(A), Z});
        }
        return R;
    }

    void OrientedBox(Mesh& Out, const Material& Mt, const V3& Centre, const double Dx, const double Dy, const V3& HalfSize)
    {
        const double Len = std::sqrt(Dx * Dx + Dy * Dy);
        const double C = Len > 1e-9 ? Dx / Len : 1.0, S = Len > 1e-9 ? Dy / Len : 0.0;
        Xf T;
        T.M = {C, -S, 0, Centre.X, S, C, 0, Centre.Y, 0, 0, 1, Centre.Z};
        Out.Push(T);
        Out.Box(Mt, HalfSize * -1.0, HalfSize);
        Out.Pop();
    }

    namespace
    {
        /** Ellipsoid from Theta0 (pi = bottom) to Theta1 (0 = top), rings upwards so the surface faces out. */
        void Ellipsoid(Mesh& Out, const Material& Mt, const V3& C, const V3& R, int Rings = 8, int Sides = 12, double Theta0 = Pi, double Theta1 = 0.0)
        {
            std::vector<Ring> List;
            for (int I = 0; I <= Rings; ++I)
            {
                const double T = Theta0 + (Theta1 - Theta0) * I / Rings;
                const double S = Sin(T);
                List.push_back(Ellipse(C.X, C.Y, C.Z + R.Z * Cos(T), R.X * S + 1e-4, R.Y * S + 1e-4, Sides));
            }
            Out.Loft(Mt, List, true);
        }

        void RingBand(Mesh& Out, const Material& Mt, double Z0, double Z1, double Rx, double Ry, double Cx = 0.0)
        {
            Out.Loft(Mt, {Ellipse(Cx, 0, Z0, Rx, Ry, 14), Ellipse(Cx, 0, Z1, Rx, Ry, 14)}, true);
        }

        void CapBottom(Mesh& Out, const Material& Mt, Ring R)
        {
            std::reverse(R.begin(), R.end());
            Out.Fan(Mt, R, {0, 0, -1});
        }

    }

    double SegmentDistance(const double Px, const double Py, const double Ax, const double Ay, const double Bx, const double By)
    {
        const double Dx = Bx - Ax, Dy = By - Ay;
        const double L2 = Dx * Dx + Dy * Dy;
        const double T = L2 > 0 ? Clamp(((Px - Ax) * Dx + (Py - Ay) * Dy) / L2, 0.0, 1.0) : 0.0;
        const double Ex = Ax + Dx * T - Px, Ey = Ay + Dy * T - Py;
        return std::sqrt(Ex * Ex + Ey * Ey);
    }

    void Rock(Mesh& Out, const Material& Mt, const V3& At, const double Radius, const double Height, const std::uint32_t Seed)
    {
        const int Rings = 7, Sides = 11;
        std::vector<double> Bump(static_cast<std::size_t>((Rings + 1) * Sides));
        Rng R(Seed);
        for (double& B : Bump) B = 0.78 + 0.34 * R.Unit();
        const double Lean = R.Range(-0.25, 0.25);
        std::vector<Ring> List;
        for (int I = 0; I <= Rings; ++I)
        {
            const double T = Pi - Pi * I / Rings;
            const double S = Sin(T);
            Ring Rg;
            for (int K = 0; K < Sides; ++K)
            {
                const double A = 2.0 * Pi * K / Sides;
                const double B = (I == 0 || I == Rings) ? 1.0 : Bump[static_cast<std::size_t>(I * Sides + K)];
                const double Z = Height * 0.5 * (1.0 + Cos(T));
                Rg.push_back({At.X + Radius * S * B * Cos(A) + Lean * Z, At.Y + Radius * 0.8 * S * B * Sin(A), At.Z - 0.12 * Height + Z * (0.85 + 0.15 * B)});
            }
            List.push_back(std::move(Rg));
        }
        Out.Loft(Mt, List, true);
    }

    void Barrel(Mesh& Out, const V3& At, const double Height, const double Radius)
    {
        std::vector<Ring> Staves;
        for (int I = 0; I <= 6; ++I)
        {
            const double T = I / 6.0;
            const double R = Radius * (0.84 + 0.16 * (1.0 - (2.0 * T - 1.0) * (2.0 * T - 1.0)));
            Staves.push_back(Ellipse(At.X, At.Y, At.Z + Height * T, R, R, 14));
        }
        Out.Loft(Mat(M::Timber), Staves, true);
        for (const double T : {0.1, 0.3, 0.7, 0.9})
        {
            const double R = Radius * (0.84 + 0.16 * (1.0 - (2.0 * T - 1.0) * (2.0 * T - 1.0))) + 0.008;
            Out.Loft(Mat(M::Iron), {Ellipse(At.X, At.Y, At.Z + Height * T - 0.025, R, R, 14), Ellipse(At.X, At.Y, At.Z + Height * T + 0.025, R, R, 14)}, true);
        }
        Out.Fan(Mat(M::TimberDark), Ellipse(At.X, At.Y, At.Z + Height - 0.01, Radius * 0.84, Radius * 0.84, 14), {0, 0, 1});
    }

    void Crate(Mesh& Out, const V3& Min, const V3& Max)
    {
        Out.Box(Mat(M::Timber), Min, Max);
        const Material& D = Mat(M::TimberDark);
        const double E = 0.045;
        for (const double Z : {Min.Z, Max.Z - E})
        {
            Out.Box(D, {Min.X - 0.01, Min.Y - 0.01, Z}, {Max.X + 0.01, Min.Y + E, Z + E});
            Out.Box(D, {Min.X - 0.01, Max.Y - E, Z}, {Max.X + 0.01, Max.Y + 0.01, Z + E});
            Out.Box(D, {Min.X - 0.01, Min.Y - 0.01, Z}, {Min.X + E, Max.Y + 0.01, Z + E});
            Out.Box(D, {Max.X - E, Min.Y - 0.01, Z}, {Max.X + 0.01, Max.Y + 0.01, Z + E});
        }
        for (const double X : {Min.X, Max.X - E})
        {
            for (const double Y : {Min.Y, Max.Y - E}) Out.Box(D, {X - 0.01, Y - 0.01, Min.Z}, {X + E + 0.01, Y + E + 0.01, Max.Z});
        }
    }

    // ------------------------------------------------------------------ people

    namespace
    {
        struct Look
        {
            M Shirt = M::Linen, Torso = M::WoolBrown, Legs = M::WoolGrey, Boots = M::Leather, Skin = M::Skin, Hair = M::Hair, Hat = M::WoolBlack;
            bool Coat = false, Dress = false, Tricorn = false, Bandana = false, Sash = false, Ponytail = false, Bun = false, Saber = false,
                 Bald = false, Beard = false;
            double Height = 1.78, Build = 1.0;
        };

        Look LookFor(const std::string_view Variant, const std::uint32_t Seed)
        {
            Look L;
            Rng R(Seed);
            if (Variant == "jake")
            {
                L.Legs = M::WoolBlue; L.Sash = true; L.Ponytail = true; L.Saber = true;
            }
            else if (Variant == "boarder")
            {
                L.Shirt = M::WoolGrey; L.Torso = M::WoolBlack; L.Legs = M::WoolBrown; L.Bandana = true; L.Saber = true;
                L.Skin = R.Unit() < 0.5 ? M::SkinDark : M::Skin; L.Beard = R.Unit() < 0.6; L.Build = 1.08;
            }
            else if (Variant == "merchant")
            {
                L.Torso = M::WoolBlue; L.Coat = true; L.Tricorn = true; L.Legs = M::WoolGrey; L.Hair = M::HairGrey; L.Build = 1.06;
            }
            else if (Variant == "officer")
            {
                L.Torso = M::WoolBlue; L.Coat = true; L.Tricorn = true; L.Legs = M::Linen; L.Height = 1.8;
            }
            else if (Variant == "draven")
            {
                L.Shirt = M::WoolBlack; L.Torso = M::WoolBlack; L.Coat = true; L.Tricorn = true; L.Legs = M::WoolBlack; L.Sash = true;
                L.Beard = true; L.Saber = true; L.Height = 1.86; L.Build = 1.08;
            }
            else if (Variant == "woman")
            {
                L.Torso = M::DressGreen; L.Dress = true; L.Legs = M::DressGreen; L.Bun = true; L.Height = 1.68; L.Build = 0.9;
            }
            else if (Variant == "youth")
            {
                L.Torso = M::Linen; L.Legs = M::WoolBrown; L.Height = 1.72; L.Build = 0.93;
            }
            else if (Variant == "sailor_f")
            {
                L.Torso = M::Ochre; L.Legs = M::WoolBrown; L.Bun = true; L.Sash = true; L.Height = 1.7; L.Build = 0.9;
            }
            else if (Variant == "bigman")
            {
                L.Torso = M::WoolGrey; L.Legs = M::WoolBrown; L.Skin = M::SkinDark; L.Bald = true; L.Beard = true; L.Height = 1.92; L.Build = 1.28;
            }
            else if (Variant == "elder")
            {
                L.Torso = M::Ochre; L.Dress = true; L.Legs = M::Ochre; L.Skin = M::SkinDark; L.Hair = M::HairGrey; L.Beard = true; L.Height = 1.74;
            }
            else if (Variant == "dockworker")
            {
                L.Torso = M::WoolBrown; L.Legs = M::WoolGrey; L.Bandana = R.Unit() < 0.5; L.Skin = R.Unit() < 0.4 ? M::SkinDark : M::Skin; L.Build = 1.1;
            }
            else  // sailor
            {
                L.Shirt = R.Unit() < 0.5 ? M::Linen : M::WoolGrey; L.Torso = R.Unit() < 0.5 ? M::WoolBlue : M::WoolBrown; L.Legs = M::Linen;
                L.Bandana = R.Unit() < 0.4; L.Skin = R.Unit() < 0.35 ? M::SkinDark : M::Skin;
            }
            L.Height *= 0.98 + 0.04 * R.Unit();
            return L;
        }
    }

    Mesh BuildFigure(const std::string_view Variant, const std::uint32_t Seed)
    {
        const Look L = LookFor(Variant, Seed);
        Mesh Out("SM_Art_Figure_" + std::string(Variant));
        const double K = L.Height / 1.78;
        Out.Push(Xf::Scale({K * (0.5 + 0.5 * L.Build), K * L.Build, K}));
        const Material& Skin = Mat(L.Skin);
        const Material& Top = Mat(L.Coat ? L.Torso : L.Shirt);
        const Material& Legs = Mat(L.Legs);
        const Material& Boots = Mat(L.Boots);

        // legs and boots
        for (const double S : {1.0, -1.0})
        {
            if (!L.Dress)
            {
                Out.Tube(Legs, {0.0, S * 0.095, 0.93}, {0.03, S * 0.105, 0.51}, 0.088, 0.062, 10);
                Out.Tube(Legs, {0.03, S * 0.105, 0.51}, {0.0, S * 0.11, 0.3}, 0.06, 0.05, 10);
                Out.Tube(Boots, {0.0, S * 0.11, 0.02}, {0.012, S * 0.11, 0.38}, 0.056, 0.06, 10);
            }
            Out.Box(Boots, {-0.07, S * 0.11 - 0.045, 0.0}, {0.18, S * 0.11 + 0.045, 0.075});
        }
        // pelvis and torso
        const Ring Hip0 = Ellipse(0, 0, 0.86, 0.115, 0.16, 14);
        Out.Loft(Legs, {Hip0, Ellipse(0, 0, 0.95, 0.12, 0.17, 14), Ellipse(0, 0, 1.05, 0.11, 0.155, 14)}, true);
        CapBottom(Out, Legs, Hip0);
        const Material& TorsoMat = Mat(L.Torso);
        Out.Loft(TorsoMat, {Ellipse(0, 0, 1.05, 0.11, 0.155, 14), Ellipse(0.005, 0, 1.15, 0.115, 0.165, 14), Ellipse(0.012, 0, 1.27, 0.125, 0.185, 14),
                               Ellipse(0.008, 0, 1.37, 0.12, 0.195, 14), Ellipse(0, 0, 1.44, 0.1, 0.185, 14), Ellipse(0, 0, 1.49, 0.07, 0.12, 14),
                               Ellipse(0.005, 0, 1.52, 0.05, 0.06, 14)},
            true);
        if (!L.Coat && L.Torso != L.Shirt)
        {
            // open vest over the shirt: a shirt panel down the chest
            Out.Box(Mat(L.Shirt), {0.1, -0.045, 1.12}, {0.132, 0.045, 1.46});
        }
        if (L.Coat)
        {
            Out.Loft(TorsoMat, {Ellipse(-0.005, 0, 0.47, 0.175, 0.225, 14), Ellipse(-0.005, 0, 0.6, 0.17, 0.22, 14), Ellipse(0, 0, 0.85, 0.15, 0.2, 14),
                                   Ellipse(0, 0, 1.06, 0.126, 0.166, 14)},
                true);
            Out.Box(Mat(M::Linen), {0.118, -0.04, 1.3}, {0.136, 0.04, 1.47});  // cravat
            for (int I = 0; I < 4; ++I) Out.Box(Mat(M::Brass), {0.126, 0.05, 1.12 + 0.08 * I}, {0.142, 0.07, 1.14 + 0.08 * I});
        }
        if (L.Dress)
        {
            const Ring Hem = Ellipse(0.01, 0, 0.03, 0.27, 0.31, 16);
            Out.Loft(TorsoMat, {Hem, Ellipse(0.01, 0, 0.4, 0.24, 0.28, 16), Ellipse(0.005, 0, 0.8, 0.17, 0.21, 16), Ellipse(0, 0, 1.06, 0.112, 0.157, 16)}, true);
            CapBottom(Out, TorsoMat, Hem);
            if (Variant == "woman") Out.Loft(Mat(M::Linen), {Ellipse(0.005, 0, 0.62, 0.205, 0.245, 16), Ellipse(0.002, 0, 1.0, 0.13, 0.172, 16)}, true);  // apron
        }
        if (L.Sash)
        {
            RingBand(Out, Mat(M::Sash), 0.99, 1.1, 0.126, 0.172);
            Out.Box(Mat(M::Sash), {-0.02, 0.15, 0.78}, {0.05, 0.185, 1.02});
        }
        else if (!L.Dress)
        {
            RingBand(Out, Mat(M::Leather), 1.01, 1.065, 0.118, 0.164);
            Out.Box(Mat(M::Brass), {0.112, -0.028, 1.012}, {0.126, 0.028, 1.063});
        }
        // arms
        for (const double S : {1.0, -1.0})
        {
            const V3 Shoulder{0.0, S * 0.195, 1.435}, Elbow{0.03, S * 0.235, 1.17}, Wrist{0.1, S * 0.235, 0.96};
            Ellipsoid(Out, Top, Shoulder, {0.055, 0.054, 0.052}, 5, 10);
            Out.Tube(Top, Shoulder, Elbow, 0.053, 0.043, 9);
            Out.Tube(Top, Elbow, Wrist, 0.043, 0.034, 9);
            Ellipsoid(Out, Skin, Wrist + V3{0.018, 0, -0.06}, {0.03, 0.022, 0.058}, 5, 8);
        }
        // neck and head
        Out.Tube(Skin, {0.005, 0, 1.49}, {0.015, 0, 1.6}, 0.052, 0.047, 10);
        Ellipsoid(Out, Skin, {0.015, 0, 1.685}, {0.093, 0.078, 0.112}, 9, 14);
        Out.Box(Skin, {0.098, -0.012, 1.655}, {0.124, 0.012, 1.705});
        for (const double S : {1.0, -1.0}) Out.Box(Skin, {-0.005, S * 0.074 - 0.012, 1.655}, {0.03, S * 0.074 + 0.012, 1.71});
        if (!L.Bald && !L.Bandana) Ellipsoid(Out, Mat(L.Hair), {-0.012, 0, 1.7}, {0.1, 0.086, 0.106}, 6, 14, Pi * 0.6, 0.0);
        if (L.Bandana)
        {
            Ellipsoid(Out, Mat(M::Sash), {-0.01, 0, 1.705}, {0.1, 0.087, 0.1}, 6, 14, Pi * 0.55, 0.0);
            Out.Tube(Mat(M::Sash), {-0.1, 0, 1.69}, {-0.16, 0.02, 1.58}, 0.02, 0.01, 5);
        }
        if (L.Beard) Ellipsoid(Out, Mat(L.Hair), {0.075, 0, 1.605}, {0.045, 0.062, 0.05}, 5, 10);
        if (L.Ponytail) Out.Tube(Mat(L.Hair), {-0.085, 0, 1.68}, {-0.125, 0, 1.49}, 0.03, 0.018, 6);
        if (L.Bun) Ellipsoid(Out, Mat(L.Hair), {-0.085, 0, 1.73}, {0.045, 0.045, 0.045}, 5, 10);
        if (L.Tricorn)
        {
            Out.Tube(Mat(L.Hat), {0.005, 0, 1.765}, {0.005, 0, 1.79}, 0.21, 0.21, 3);
            Out.Tube(Mat(L.Hat), {0.0, 0, 1.78}, {0.0, 0, 1.865}, 0.1, 0.092, 12);
        }
        if (L.Saber)
        {
            Out.Tube(Mat(M::Leather), {-0.03, 0.2, 0.98}, {-0.32, 0.24, 0.4}, 0.022, 0.017, 6);
            Out.Tube(Mat(M::Brass), {-0.03, 0.2, 0.98}, {0.05, 0.19, 1.1}, 0.018, 0.018, 6);
            Out.Box(Mat(M::Brass), {-0.045, 0.15, 0.97}, {-0.015, 0.25, 1.0});
        }
        Out.Pop();
        return Out;
    }

    // ------------------------------------------------------------------ nature

    Mesh BuildPalm(const double Height, const std::uint32_t Seed)
    {
        Mesh Out("SM_Art_Palm");
        Rng R(Seed);
        const double Yaw = R.Range(0.0, 2.0 * Pi);
        const double Lean = Height * R.Range(0.12, 0.3);
        const V3 Dir{Cos(Yaw), Sin(Yaw), 0};
        V3 Previous{0, 0, -0.3};
        const int Segments = 9;
        V3 TopPoint;
        for (int I = 1; I <= Segments; ++I)
        {
            const double T = static_cast<double>(I) / Segments;
            const V3 P = Dir * (Lean * T * T) + V3{0, 0, Height * T - 0.3 * (1.0 - T)};
            Out.Tube(Mat(M::Bark), Previous, P, 0.2 - 0.07 * (T - 1.0 / Segments), 0.2 - 0.07 * T, 9, I == Segments);
            Previous = P;
            TopPoint = P;
        }
        for (int I = 0; I < 3; ++I)
        {
            const double A = 2.0 * Pi * I / 3.0 + Yaw;
            Ellipsoid(Out, Mat(M::TimberDark), TopPoint + V3{0.16 * Cos(A), 0.16 * Sin(A), -0.22}, {0.1, 0.1, 0.11}, 4, 8);
        }
        const int Fronds = 11;
        for (int F = 0; F < Fronds; ++F)
        {
            const double A = 2.0 * Pi * F / Fronds + R.Range(-0.15, 0.15);
            const V3 Out2{Cos(A), Sin(A), 0};
            const V3 Side{-Sin(A), Cos(A), 0};
            const double Length = R.Range(2.6, 3.4);
            const double Rise = R.Range(0.4, 0.9);
            const int Steps = 5;
            std::vector<V3> Centre;
            for (int S = 0; S <= Steps; ++S)
            {
                const double T = static_cast<double>(S) / Steps;
                Centre.push_back(TopPoint + Out2 * (Length * T) + V3{0, 0, Rise * T - (Rise + 1.6) * T * T});
            }
            for (int S = 0; S < Steps; ++S)
            {
                const double T0 = static_cast<double>(S) / Steps, T1 = static_cast<double>(S + 1) / Steps;
                const double W0 = 0.62 * (0.35 + 0.65 * Sin(Pi * Clamp(T0 * 0.9 + 0.1, 0.0, 1.0))), W1 = 0.62 * (0.35 + 0.65 * Sin(Pi * Clamp(T1 * 0.9 + 0.1, 0.0, 1.0)));
                const V3 Droop0 = V3{0, 0, -0.18 * T0}, Droop1 = V3{0, 0, -0.18 * T1};
                const V3 A0 = Centre[static_cast<std::size_t>(S)] - Side * W0 + Droop0, B0 = Centre[static_cast<std::size_t>(S)] + Side * W0 + Droop0;
                const V3 A1 = Centre[static_cast<std::size_t>(S + 1)] - Side * W1 + Droop1, B1 = Centre[static_cast<std::size_t>(S + 1)] + Side * W1 + Droop1;
                const V3 N = Normalize(Cross(A1 - A0, B0 - A0));
                const double V0 = 1.0 - T0, V1 = 1.0 - T1;
                Out.Triangle(Mat(M::Frond), {A0, N, 0.0, V0}, {B1, N, 1.0, V1}, {B0, N, 1.0, V0});
                Out.Triangle(Mat(M::Frond), {A0, N, 0.0, V0}, {A1, N, 0.0, V1}, {B1, N, 1.0, V1});
            }
        }
        return Out;
    }

    Mesh BuildWreckage()
    {
        Mesh Out("SM_Art_HarlowWreckage");
        const Material& Hull = Mat(M::Hull);
        // A length of the Harlow's bottom lying keel-down in the sand: planking both sides, frames standing proud.
        const auto Section = [](double T, double Offset) { return V3{0, -0.55 + 1.1 * T, 0.08 + Offset + 0.72 * (2.0 * T - 1.0) * (2.0 * T - 1.0)}; };
        for (const double Offset : {0.0, 0.07})
        {
            std::vector<Ring> Rings;
            for (int I = 0; I <= 8; ++I)
            {
                const double X = -1.85 + 3.7 * I / 8.0;
                Ring Rg;
                for (int K = 0; K <= 8; ++K)
                {
                    const double T = K / 8.0;
                    const double Broken = (K == 8 && (I % 3) == 1) ? -0.18 : 0.0;
                    V3 P = Section(T, Offset);
                    P.X = X;
                    P.Z += Broken;
                    Rg.push_back(P);
                }
                Rings.push_back(std::move(Rg));
            }
            Out.Loft(Hull, Rings, false, Offset > 0.0);
        }
        for (int I = 0; I < 8; ++I)
        {
            const double X = -1.7 + 3.4 * I / 7.0;
            V3 Previous = Section(0.0, 0.08);
            Previous.X = X;
            for (int K = 1; K <= 6; ++K)
            {
                V3 P = Section(K / 6.0, 0.08);
                P.X = X;
                Out.Tube(Mat(M::TimberDark), Previous, P, 0.06, 0.06, 5, false);
                Previous = P;
            }
            if (I % 2 == 0) Out.Tube(Mat(M::TimberDark), Previous, Previous + V3{0.05, 0.1, 0.35 + 0.1 * I / 7.0}, 0.06, 0.04, 5);
        }
        // loose planks, a crate and a barrel from the hold
        OrientedBox(Out, Mat(M::Driftwood), {2.4, 0.8, 0.14}, 1.0, 0.35, {1.05, 0.12, 0.03});
        OrientedBox(Out, Mat(M::Driftwood), {2.2, 0.55, 0.2}, 1.0, -0.2, {0.9, 0.1, 0.03});
        OrientedBox(Out, Mat(M::Hull), {-2.2, -0.95, 0.12}, 1.0, 0.15, {1.1, 0.16, 0.035});
        Crate(Out, {1.7, 0.45, 0.1}, {2.35, 1.05, 0.62});
        Out.Push(Xf::Translate({-2.6, -0.95, 0.38}) * Xf::RotateX(Pi / 2.0) * Xf::RotateZ(0.4));
        Barrel(Out, {0, 0, -0.42}, 0.84, 0.28);
        Out.Pop();
        // the snapped mast still standing in the sand, with a coil of rope
        Out.Tube(Mat(M::TimberDark), {0.72, -1.2, -0.05}, {0.62, -1.24, 2.7}, 0.2, 0.17, 10);
        Out.Tube(Mat(M::TimberDark), {0.62, -1.24, 2.7}, {0.58, -1.22, 3.15}, 0.17, 0.03, 7);
        for (int I = 0; I < 10; ++I)
        {
            const double A0 = 2.0 * Pi * I / 10, A1 = 2.0 * Pi * (I + 1) / 10;
            Out.Tube(Mat(M::Rope), {1.25 + 0.3 * Cos(A0), -0.9 + 0.3 * Sin(A0), 0.1}, {1.25 + 0.3 * Cos(A1), -0.9 + 0.3 * Sin(A1), 0.1}, 0.05, 0.05, 5, false);
        }
        return Out;
    }

    Mesh BuildOuterReef()
    {
        Mesh Out("SM_Art_OuterReef");
        Rng R(4242u);
        struct Area { double X0, X1, Y0, Y1, Top; };
        for (const Area& A : {Area{-6.1, -2.2, -2.32, 1.99, 0.4}, Area{-2.7, 0.8, -2.22, 2.56, 0.26}})
        {
            for (int I = 0; I < 7; ++I)
            {
                const double X = R.Range(A.X0 + 0.5, A.X1 - 0.5), Y = R.Range(A.Y0 + 0.5, A.Y1 - 0.5);
                const double Radius = R.Range(0.5, 1.1);
                const double Height = 0.9 + A.Top - R.Range(0.0, 0.25);
                const std::uint32_t Seed = R.Next();
                Rock(Out, Mat(M::Rock), {X, Y, -0.9}, Radius, Height, Seed);
            }
            for (int I = 0; I < 5; ++I)
            {
                // branching coral heads, pale, low under the surface
                const double X = R.Range(A.X0, A.X1), Y = R.Range(A.Y0, A.Y1);
                const Material& C = Mat(I % 3 == 0 ? M::CoralRed : M::Coral);
                const V3 Base{X, Y, -0.5};
                for (int B = 0; B < 5; ++B)
                {
                    const double Ang = 2.0 * Pi * B / 5 + R.Range(0, 1);
                    const V3 Mid = Base + V3{0.12 * Cos(Ang), 0.12 * Sin(Ang), R.Range(0.12, 0.2)};
                    Out.Tube(C, Base, Mid, 0.035, 0.028, 5, false);
                    Out.Tube(C, Mid, Mid + V3{0.1 * Cos(Ang + 0.6), 0.1 * Sin(Ang + 0.6), R.Range(0.1, 0.18)}, 0.026, 0.012, 5);
                    Out.Tube(C, Mid, Mid + V3{0.1 * Cos(Ang - 0.6), 0.1 * Sin(Ang - 0.6), R.Range(0.1, 0.18)}, 0.026, 0.012, 5);
                }
            }
        }
        return Out;
    }

    Mesh BuildCoveJetty()
    {
        Mesh Out("SM_Art_GalleonCoveJetty");
        // jetty head: dressed stone pier with a timber deck (greybox platform -3..3 x -2..2, top 0.3)
        Out.Box(Mat(M::Stone), {-3.0, -2.0, -2.6}, {3.0, 2.0, 0.14});
        Out.Box(Mat(M::StoneDark), {-3.08, -2.08, -2.6}, {3.08, 2.08, -0.35});
        Out.Box(Mat(M::Deck), {-2.95, -1.95, 0.14}, {2.95, 1.95, 0.3});
        for (const double X : {-2.7, 2.7})
        {
            for (const double Y : {-1.75, 1.75})
            {
                Out.Tube(Mat(M::TimberDark), {X, Y, 0.3}, {X, Y, 0.72}, 0.13, 0.11, 10);
                Out.Tube(Mat(M::TimberDark), {X, Y, 0.72}, {X, Y, 0.78}, 0.15, 0.15, 10);
            }
        }
        // plank walk (-0.2..3.8 x 0.88..1.32) on its own piles
        Out.Box(Mat(M::DeckWorn), {-0.2, 0.88, 0.2}, {3.8, 1.32, 0.3});
        for (const double X : {3.4})
        {
            for (const double Y : {0.95, 1.25}) Out.Tube(Mat(M::Driftwood), {X, Y, -2.5}, {X, Y, 0.2}, 0.08, 0.08, 7);
        }
        // crate stack (0..1.6 x -1.6..-0.4) and barrels (-1.8..-0.6 x -1.4..-0.4)
        Crate(Out, {0.0, -1.6, 0.3}, {0.8, -0.95, 0.9});
        Crate(Out, {0.85, -1.55, 0.3}, {1.6, -0.9, 0.85});
        Crate(Out, {0.2, -1.5, 0.9}, {0.8, -1.0, 1.3});
        Barrel(Out, {-1.45, -1.05, 0.3}, 0.85, 0.3);
        Barrel(Out, {-0.9, -0.75, 0.3}, 0.8, 0.28);
        // lantern post
        Out.Tube(Mat(M::TimberDark), {-2.6, 1.6, 0.3}, {-2.6, 1.6, 2.6}, 0.07, 0.06, 8);
        Out.Box(Mat(M::TimberDark), {-2.6, 1.55, 2.5}, {-2.1, 1.65, 2.58});
        Out.Box(Mat(M::Brass), {-2.25, 1.48, 2.1}, {-2.05, 1.72, 2.45});
        return Out;
    }

    // ------------------------------------------------------------------ buildings

    namespace
    {
        /** One building volume: plastered walls on a stone plinth, windows, door and a tiled gable roof. */
        struct House
        {
            double X0 = 0, X1 = 0, Y0 = 0, Y1 = 0, Height = 0;
            M Wall = M::Plaster;
            bool DoorSouth = false;  // door on the -Y face, else +Y
            bool Roof = true;
            std::vector<std::pair<double, double>> OpeningsX0, OpeningsX1, OpeningsY0, OpeningsY1;  // gaps (along the face) per face
        };

        House MakeHouse(double X0, double X1, double Y0, double Y1, double Height, M Wall)
        {
            House H;
            H.X0 = X0;
            H.X1 = X1;
            H.Y0 = Y0;
            H.Y1 = Y1;
            H.Height = Height;
            H.Wall = Wall;
            return H;
        }

        /** Wall face with rectangular openings: split into columns between the gaps. */
        void WallFace(Mesh& Out, Mesh* Col, const Material& Mt, const V3& A, const V3& B, double Z0, double Z1, const V3& Normal,
            const std::vector<std::pair<double, double>>& Gaps, double Lintel)
        {
            const double Len = Length(B - A);
            const V3 Dir = (B - A) * (1.0 / Len);
            std::vector<std::pair<double, double>> Solid;
            double From = 0.0;
            std::vector<std::pair<double, double>> Sorted = Gaps;
            std::sort(Sorted.begin(), Sorted.end());
            for (const auto& [G0, G1] : Sorted)
            {
                if (G0 > From) Solid.push_back({From, G0});
                From = std::max(From, G1);
            }
            if (From < Len) Solid.push_back({From, Len});
            const auto Panel = [&](double S0, double S1, double Za, double Zb)
            {
                const V3 P0 = A + Dir * S0, P1 = A + Dir * S1;
                // Quad winding from the outward normal
                const V3 Q0{P0.X, P0.Y, Za}, Q1{P1.X, P1.Y, Za}, Q2{P1.X, P1.Y, Zb}, Q3{P0.X, P0.Y, Zb};
                if (Dot(Cross(Q1 - Q0, Q3 - Q0), Normal) > 0) Out.Quad(Mt, Q0, Q1, Q2, Q3);
                else Out.Quad(Mt, Q1, Q0, Q3, Q2);
                if (Col)
                {
                    Material C;
                    C.Name = "Collision";
                    const V3 Mid = (P0 + P1) * 0.5 - Normal * 0.15;
                    OrientedBox(*Col, C, {Mid.X, Mid.Y, (Za + Zb) / 2.0}, Dir.X, Dir.Y, {(S1 - S0) / 2.0, 0.15, (Zb - Za) / 2.0});
                }
            };
            for (const auto& [S0, S1] : Solid) Panel(S0, S1, Z0, Z1);
            for (const auto& [G0, G1] : Sorted)
            {
                if (Z0 + Lintel < Z1) Panel(G0, G1, Z0 + Lintel, Z1);
                // door frame
                const V3 P0 = A + Dir * G0, P1 = A + Dir * G1;
                Out.Tube(Mat(M::TimberDark), {P0.X, P0.Y, Z0}, {P0.X, P0.Y, Z0 + Lintel}, 0.09, 0.09, 4);
                Out.Tube(Mat(M::TimberDark), {P1.X, P1.Y, Z0}, {P1.X, P1.Y, Z0 + Lintel}, 0.09, 0.09, 4);
                Out.Tube(Mat(M::TimberDark), {P0.X, P0.Y, Z0 + Lintel}, {P1.X, P1.Y, Z0 + Lintel}, 0.1, 0.1, 4);
            }
        }

        double LengthOf(const V3& V) { return Length(V); }

        void Window(Mesh& Out, const V3& Centre, const V3& Normal, const V3& Along, double W, double H)
        {
            const V3 C = Centre + Normal * 0.015;
            const V3 U = Along * (W / 2.0);
            const V3 Up{0, 0, H / 2.0};
            const V3 A = C - U - Up, B = C + U - Up, D = C - U + Up, E = C + U + Up;
            if (Dot(Cross(B - A, D - A), Normal) > 0) Out.Quad(Mat(M::Glass), A, B, E, D);
            else Out.Quad(Mat(M::Glass), B, A, D, E);
            // sill and lintel
            const V3 Mid0 = (A + B) * 0.5 - Up * 0.06 + Normal * 0.04, Mid1 = (D + E) * 0.5 + Up * 0.05 + Normal * 0.03;
            OrientedBox(Out, Mat(M::TimberDark), Mid0, Along.X, Along.Y, {W / 2.0 + 0.12, 0.06, 0.05});
            OrientedBox(Out, Mat(M::TimberDark), Mid1, Along.X, Along.Y, {W / 2.0 + 0.06, 0.04, 0.05});
            for (const double S : {-1.0, 1.0})
            {
                const V3 Hinge = C + U * (S * 1.0) + Along * (S * W * 0.28) + Normal * 0.05;
                const V3 HalfW = Along * (W * 0.26), HalfH = Up * 1.05;
                const V3 P = Hinge - HalfW - HalfH, Q = Hinge + HalfW - HalfH, R = Hinge + HalfW + HalfH, T = Hinge - HalfW + HalfH;
                if (Dot(Cross(Q - P, T - P), Normal) > 0) Out.Quad(Mat(M::TimberDark), P, Q, R, T);
                else Out.Quad(Mat(M::TimberDark), Q, P, T, R);
            }
        }

        void GableRoof(Mesh& Out, double X0, double X1, double Y0, double Y1, double Eave, const Material& Gable)
        {
            const bool AlongX = (X1 - X0) >= (Y1 - Y0);
            const double O = 0.35;
            const Material& Tile = Mat(M::RoofTile);
            if (AlongX)
            {
                const double Mid = (Y0 + Y1) / 2.0, Ridge = Eave + 0.55 * (Y1 - Y0) / 2.0 + 0.3;
                Out.Quad(Tile, {X0 - O, Y0 - O, Eave - 0.12}, {X1 + O, Y0 - O, Eave - 0.12}, {X1 + O, Mid, Ridge}, {X0 - O, Mid, Ridge});
                Out.Quad(Tile, {X1 + O, Y1 + O, Eave - 0.12}, {X0 - O, Y1 + O, Eave - 0.12}, {X0 - O, Mid, Ridge}, {X1 + O, Mid, Ridge});
                Out.Tube(Mat(M::RoofTile), {X0 - O, Mid, Ridge + 0.02}, {X1 + O, Mid, Ridge + 0.02}, 0.09, 0.09, 6);
                Out.Fan(Gable, {{X0, Y0, Eave}, {X0, Mid, Ridge - 0.02}, {X0, Y1, Eave}}, {-1, 0, 0});
                Out.Fan(Gable, {{X1, Y1, Eave}, {X1, Mid, Ridge - 0.02}, {X1, Y0, Eave}}, {1, 0, 0});
                // underside of the eaves
                Out.Quad(Mat(M::TimberDark), {X1 + O, Y0 - O, Eave - 0.13}, {X0 - O, Y0 - O, Eave - 0.13}, {X0 - O, Y0, Eave - 0.02}, {X1 + O, Y0, Eave - 0.02});
                Out.Quad(Mat(M::TimberDark), {X0 - O, Y1 + O, Eave - 0.13}, {X1 + O, Y1 + O, Eave - 0.13}, {X1 + O, Y1, Eave - 0.02}, {X0 - O, Y1, Eave - 0.02});
            }
            else
            {
                const double Mid = (X0 + X1) / 2.0, Ridge = Eave + 0.55 * (X1 - X0) / 2.0 + 0.3;
                Out.Quad(Tile, {X0 - O, Y1 + O, Eave - 0.12}, {X0 - O, Y0 - O, Eave - 0.12}, {Mid, Y0 - O, Ridge}, {Mid, Y1 + O, Ridge});
                Out.Quad(Tile, {X1 + O, Y0 - O, Eave - 0.12}, {X1 + O, Y1 + O, Eave - 0.12}, {Mid, Y1 + O, Ridge}, {Mid, Y0 - O, Ridge});
                Out.Tube(Mat(M::RoofTile), {Mid, Y0 - O, Ridge + 0.02}, {Mid, Y1 + O, Ridge + 0.02}, 0.09, 0.09, 6);
                Out.Fan(Gable, {{X1, Y0, Eave}, {Mid, Y0, Ridge - 0.02}, {X0, Y0, Eave}}, {0, -1, 0});
                Out.Fan(Gable, {{X0, Y1, Eave}, {Mid, Y1, Ridge - 0.02}, {X1, Y1, Eave}}, {0, 1, 0});
            }
        }

        void BuildHouse(Mesh& Out, Mesh* Col, const House& H, std::uint32_t Seed)
        {
            Rng R(Seed);
            const Material& Wall = Mat(H.Wall);
            const double Plinth = 0.55;
            struct Face { V3 A, B, N; const std::vector<std::pair<double, double>>* Gaps; };
            const Face Faces[4] = {
                {{H.X0, H.Y0, 0}, {H.X1, H.Y0, 0}, {0, -1, 0}, &H.OpeningsY0},
                {{H.X1, H.Y0, 0}, {H.X1, H.Y1, 0}, {1, 0, 0}, &H.OpeningsX1},
                {{H.X1, H.Y1, 0}, {H.X0, H.Y1, 0}, {0, 1, 0}, &H.OpeningsY1},
                {{H.X0, H.Y1, 0}, {H.X0, H.Y0, 0}, {-1, 0, 0}, &H.OpeningsX0},
            };
            const double Lintel = std::min(3.0, H.Height - 0.4);
            for (const Face& F : Faces)
            {
                WallFace(Out, Col, Mat(M::Stone), F.A, F.B, 0.0, Plinth, F.N, *F.Gaps, Plinth);
                WallFace(Out, Col, Wall, F.A, F.B, Plinth, H.Height, F.N, *F.Gaps, Lintel - Plinth);
                // windows between the openings, upper floor always, ground floor where solid
                const V3 Along = Normalize(F.B - F.A);
                const double Len = LengthOf(F.B - F.A);
                const int Count = std::max(1, static_cast<int>(Len / 2.6));
                for (int I = 0; I < Count; ++I)
                {
                    const double S = Len * (I + 0.5) / Count;
                    bool InGap = false;
                    for (const auto& [G0, G1] : *F.Gaps) InGap = InGap || (S > G0 - 0.8 && S < G1 + 0.8);
                    const V3 Base = F.A + Along * S;
                    if (!InGap && H.Height > 2.6) Window(Out, {Base.X, Base.Y, 1.75}, F.N, Along, 0.8, 1.1);
                    if (H.Height > 4.4) Window(Out, {Base.X, Base.Y, H.Height - 1.25}, F.N, Along, 0.75, 1.0);
                }
            }
            // corner posts and a floor band
            for (const V3 C : {V3{H.X0, H.Y0, 0}, V3{H.X1, H.Y0, 0}, V3{H.X1, H.Y1, 0}, V3{H.X0, H.Y1, 0}})
            {
                Out.Box(Mat(M::StoneDark), {C.X - 0.18, C.Y - 0.18, 0}, {C.X + 0.18, C.Y + 0.18, H.Height});
            }
            if (H.Height > 4.4)
            {
                Out.Box(Mat(M::TimberDark), {H.X0 - 0.06, H.Y0 - 0.06, 2.9}, {H.X1 + 0.06, H.Y1 + 0.06, 3.05});
            }
            if (H.Roof) GableRoof(Out, H.X0, H.X1, H.Y0, H.Y1, H.Height, Wall);
            // chimney on bigger houses
            if (H.Roof && (H.X1 - H.X0) * (H.Y1 - H.Y0) > 30.0 && R.Unit() < 0.7)
            {
                const double Cx = Lerp(H.X0, H.X1, 0.25 + 0.5 * R.Unit()), Cy = (H.Y0 + H.Y1) / 2.0 + 0.6;
                Out.Box(Mat(M::StoneDark), {Cx - 0.35, Cy - 0.35, H.Height}, {Cx + 0.35, Cy + 0.35, H.Height + 0.55 * (std::min(H.X1 - H.X0, H.Y1 - H.Y0)) / 2.0 + 1.1});
            }
            if (Col && H.Roof)
            {
                Material C;
                C.Name = "Collision";
                Col->Box(C, {H.X0, H.Y0, H.Height - 0.2}, {H.X1, H.Y1, H.Height});
            }
        }
    }

    Mesh BuildRexaWarehouses(Mesh* Collision)
    {
        Mesh Out("SM_Art_RexaWarehouseQuarter");
        Material C;
        C.Name = "Collision";
        // B1: the long warehouse hall (x -9..9, y -4..4, eaves 3.6), open at both ends for the route.
        House Hall = MakeHouse(-9.0, 9.0, -4.0, 4.0, 3.6, M::PlasterOchre);
        Hall.OpeningsX0 = {{0.8, 4.2}};       // measured from y=4 towards -Y: doorway y -0.2..3.2 (route from the Broker)
        Hall.OpeningsX1 = {{0.25, 2.5}};      // doorway y -3.75..-1.5 (towards the Survivor)
        Hall.OpeningsY1 = {{7.5, 10.5}};      // loading door on the quay side
        BuildHouse(Out, Collision, Hall, 11u);
        Out.Box(Mat(M::Paving), {-8.9, -3.9, -0.02}, {8.9, 3.9, 0.0});
        for (const double X : {-5.0, -1.0, 3.0, 7.0})
        {
            for (const double Y : {-2.7, 3.3})
            {
                Out.Box(Mat(M::TimberDark), {X - 0.14, Y - 0.14, 0}, {X + 0.14, Y + 0.14, 3.6});
                if (Collision) Collision->Box(C, {X - 0.14, Y - 0.14, 0}, {X + 0.14, Y + 0.14, 3.6});
            }
            Out.Box(Mat(M::TimberDark), {X - 0.12, -4.0, 3.35}, {X + 0.12, 4.0, 3.6});
        }
        // stored cargo along the back wall, clear of the route
        Rng R(77u);
        for (int I = 0; I < 6; ++I)
        {
            const double X = -7.5 + 2.6 * I;
            Crate(Out, {X, -3.7, 0}, {X + 1.0, -3.0, 0.8});
            if (I % 2 == 0) Crate(Out, {X + 0.1, -3.65, 0.8}, {X + 0.9, -3.05, 1.4});
            if (Collision) Collision->Box(C, {X, -3.7, 0}, {X + 1.0, -3.0, 1.4});
        }
        // hoist beam over the loading door
        Out.Box(Mat(M::TimberDark), {0.2, 3.9, 3.2}, {0.45, 5.2, 3.45});
        Out.Tube(Mat(M::Rope), {0.32, 5.1, 3.2}, {0.32, 5.1, 1.2}, 0.02, 0.02, 4);
        Out.Tube(Mat(M::Iron), {0.32, 5.1, 3.2}, {0.32, 5.1, 3.05}, 0.08, 0.08, 8);
        // awning over the west doorway
        Out.Quad(Mat(M::Canvas), {-9.0, 0.2, 3.1}, {-9.0, 3.6, 3.1}, {-10.6, 3.6, 2.6}, {-10.6, 0.2, 2.6}, true);
        for (const double Y : {0.25, 3.55}) Out.Tube(Mat(M::TimberDark), {-10.55, Y, 0}, {-10.55, Y, 2.62}, 0.05, 0.05, 6);

        // B2: townhouse row behind the hall (x -6..6, y -9..-5, eaves 5.2)
        const M Walls[3] = {M::Plaster, M::PlasterBlue, M::PlasterOchre};
        for (int I = 0; I < 3; ++I)
        {
            House Townhouse = MakeHouse(-6.0 + 4.0 * I, -2.0 + 4.0 * I, -9.0, -5.0, 5.2 - 0.3 * (I % 2), Walls[I]);
            Townhouse.OpeningsY0 = {{1.4, 2.6}};
            BuildHouse(Out, nullptr, Townhouse, 20u + static_cast<std::uint32_t>(I));
        }
        if (Collision) Collision->Box(C, {-6.0, -9.0, 0}, {6.0, -5.0, 5.2});
        // B3: lean-to shed at the east end (x 9..13, y -1.5..3.5, eaves 2.8)
        House Shed = MakeHouse(9.0, 13.0, -1.5, 3.5, 2.8, M::Plaster);
        BuildHouse(Out, Collision, Shed, 30u);
        // B4: two small houses at the west end with the gate passage between them (y -0.9..3.4)
        House West0 = MakeHouse(-12.0, -8.2, -4.0, -0.9, 2.4, M::PlasterBlue);
        House West1 = MakeHouse(-12.0, -8.2, 3.4, 8.0, 2.4, M::Plaster);
        BuildHouse(Out, Collision, West0, 40u);
        BuildHouse(Out, Collision, West1, 41u);
        Out.Box(Mat(M::TimberDark), {-10.3, -0.9, 3.0}, {-9.9, 3.4, 3.3});
        Out.Box(Mat(M::Timber), {-10.35, 0.7, 2.4}, {-10.25, 1.8, 2.95});  // hanging sign
        // B5: low quay wall with cargo (x -4..12, y 6..8, 1.0 high)
        Out.Box(Mat(M::Stone), {-4.0, 6.0, 0.0}, {12.0, 8.0, 0.55});
        Out.Box(Mat(M::StoneDark), {-4.05, 5.95, 0.55}, {12.05, 8.05, 0.62});
        for (int I = 0; I < 5; ++I)
        {
            const double X = -3.2 + 3.1 * I;
            if (I % 2 == 0) Barrel(Out, {X, 7.0, 0.62}, 0.85, 0.3);
            else Crate(Out, {X - 0.45, 6.4, 0.62}, {X + 0.45, 7.5, 1.0});
        }
        if (Collision) Collision->Box(C, {-4.0, 6.0, 0.0}, {12.0, 8.0, 1.0});
        if (Collision) Collision->Box(C, {-9.0, -4.0, -0.3}, {9.0, 4.0, 0.0});
        return Out;
    }

    Mesh DressBoxes(const std::string& Name, const std::vector<std::pair<V3, V3>>& Boxes)
    {
        Mesh Out(Name);
        std::uint32_t Seed = HashString(Name);
        for (const auto& [Min, Max] : Boxes)
        {
            ++Seed;
            const double Sx = Max.X - Min.X, Sy = Max.Y - Min.Y, Sz = Max.Z - Min.Z;
            if (std::min(Sx, Sy) < 0.5 && Sz > 0.6)
            {
                // pole or post
                const V3 C{(Min.X + Max.X) / 2.0, (Min.Y + Max.Y) / 2.0, 0};
                const double R = std::min(Sx, Sy) / 2.0;
                Out.Tube(Mat(M::Driftwood), {C.X, C.Y, Min.Z}, {C.X, C.Y, Max.Z}, R, R * 0.85, 8);
            }
            else if (Sz >= 2.0 && std::min(Sx, Sy) >= 2.5)
            {
                // a building standing on the box footprint
                Out.Push(Xf::Translate({0, 0, Min.Z}));
                House H = MakeHouse(Min.X, Max.X, Min.Y, Max.Y, Sz, (Seed % 3 == 0) ? M::PlasterOchre : (Seed % 3 == 1) ? M::Plaster : M::PlasterBlue);
                const double Door = (Max.X - Min.X) / 2.0;
                H.OpeningsY0 = {{Door - 0.6, Door + 0.6}};
                BuildHouse(Out, nullptr, H, Seed);
                Out.Pop();
            }
            else if (Sz >= 2.0)
            {
                Out.Box(Mat(M::Stone), Min, {Max.X, Max.Y, Max.Z - 0.2});
                Out.Box(Mat(M::StoneDark), {Min.X - 0.06, Min.Y - 0.06, Max.Z - 0.2}, {Max.X + 0.06, Max.Y + 0.06, Max.Z});
            }
            else if (Sz <= 0.4)
            {
                Out.Box(Mat(M::Deck), {Min.X, Min.Y, Max.Z - 0.12}, Max);
                Out.Box(Mat(M::TimberDark), {Min.X + 0.1, Min.Y + 0.1, Min.Z - 0.4}, {Max.X - 0.1, Max.Y - 0.1, Max.Z - 0.12});
            }
            else
            {
                Out.Box(Mat(M::Stone), Min, {Max.X, Max.Y, Max.Z - 0.12});
                Out.Box(Mat(M::Paving), {Min.X - 0.04, Min.Y - 0.04, Max.Z - 0.12}, {Max.X + 0.04, Max.Y + 0.04, Max.Z});
            }
        }
        return Out;
    }
}
