#include "ArtKitLibrary.h"

#include <algorithm>
#include <cmath>

namespace DarkArisen::Tools::Art
{
    namespace
    {
        /** Hull lines of one ship: plan width, keel and sheer as functions of x, section as a function of z. */
        struct Lines
        {
            const ShipSpec& S;
            double U(double X) const { return Clamp((X - S.Stern) / (S.Bow - S.Stern), 0.0, 1.0); }

            /** Half width at the maximum section of station X. */
            double Plan(double X) const
            {
                const double T = U(X);
                double W = 1.0;
                if (T < 0.3)
                {
                    const double A = T / 0.3;
                    W = S.SternHalf + (1.0 - S.SternHalf) * (1.0 - (1.0 - A) * (1.0 - A));
                }
                else if (T > S.BowTaperFrom)
                {
                    const double V = (T - S.BowTaperFrom) / (1.0 - S.BowTaperFrom);
                    W = std::sqrt(std::max(0.0, 1.0 - V * V)) * (1.0 - 0.15 * V);
                }
                return S.HalfBeam * W;
            }

            double KeelZ(double X) const
            {
                const double T = U(X);
                double Z = S.Keel;
                if (T > 0.78)
                {
                    const double A = (T - 0.78) / 0.22;
                    Z += (S.Deck - 0.4 - S.Keel) * A * A;
                }
                if (T < 0.14)
                {
                    const double A = (0.14 - T) / 0.14;
                    Z += (0.2 - S.Keel) * 0.75 * A * A;
                }
                return Z;
            }

            double RailZ(double X) const
            {
                const double T = 2.0 * U(X) - 1.0;
                return S.Deck + S.Bulwark + S.Sheer * T * T * (T > 0.0 ? 1.0 : 0.75);
            }

            /** Half width of station X at height Z (round bilge, slight tumblehome above the wale). */
            double HalfWidth(double X, double Z) const
            {
                const double W = Plan(X);
                const double Kz = KeelZ(X);
                const double Top = RailZ(X);
                double Hm = S.Deck - 0.35;
                if (Hm < Kz + 0.25) Hm = Kz + 0.25;
                if (Z <= Hm)
                {
                    const double Tau = Clamp((Z - Kz) / (Hm - Kz), 0.0, 1.0);
                    const double R = 1.0 - Tau;
                    return W * (0.03 + 0.97 * (1.0 - R * R * std::sqrt(R)));
                }
                const double Q = Clamp((Z - Hm) / std::max(0.05, Top - Hm), 0.0, 1.0);
                return W * (1.0 - 0.07 * Q * Q);
            }

            double DeckAt(double X) const { return X >= S.RaisedFrom ? S.RaisedDeck : S.Deck; }
            /** Inside of the bulwark at deck height. */
            double DeckHalf(double X) const { return std::max(0.0, HalfWidth(X, DeckAt(X)) - 0.14); }
        };

        std::vector<double> Stations(double From, double To, int Count)
        {
            std::vector<double> Out;
            for (int I = 0; I <= Count; ++I) Out.push_back(From + (To - From) * I / Count);
            return Out;
        }

        /** Strip of hull between two heights on one side (+1 port, -1 starboard). */
        void HullBand(Mesh& Out, const Lines& L, const std::vector<double>& Xs, double ZLow, double ZHigh, int Samples, double Side,
            const Material& Mt, bool CrossKeel)
        {
            std::vector<Ring> Rings;
            for (const double X : Xs)
            {
                const double Kz = L.KeelZ(X);
                const double Top = L.RailZ(X);
                const double Lo = Clamp(ZLow, Kz, Top);
                const double Hi = Clamp(ZHigh, Kz, Top);
                Ring R;
                if (CrossKeel)
                {
                    // starboard top -> keel -> port top
                    for (int I = Samples; I >= 0; --I)
                    {
                        const double Z = Lo + (Hi - Lo) * I / Samples;
                        R.push_back({X, -L.HalfWidth(X, Z), Z});
                    }
                    for (int I = 1; I <= Samples; ++I)
                    {
                        const double Z = Lo + (Hi - Lo) * I / Samples;
                        R.push_back({X, L.HalfWidth(X, Z), Z});
                    }
                }
                else
                {
                    for (int I = 0; I <= Samples; ++I)
                    {
                        const int K = Side > 0 ? I : Samples - I;
                        const double Z = Lo + (Hi - Lo) * K / Samples;
                        R.push_back({X, Side * L.HalfWidth(X, Z), Z});
                    }
                }
                Rings.push_back(std::move(R));
            }
            Out.Loft(Mt, Rings, false);
        }

        void Wheel(Mesh& Out, const V3& Hub, double Radius)
        {
            const Material& Wood = Mat(M::TimberDark);
            const int Segments = 16;
            for (int I = 0; I < Segments; ++I)
            {
                const double A0 = 2.0 * Pi * I / Segments, A1 = 2.0 * Pi * (I + 1) / Segments;
                Out.Tube(Wood, {Hub.X, Hub.Y + Radius * Cos(A0), Hub.Z + Radius * Sin(A0)},
                    {Hub.X, Hub.Y + Radius * Cos(A1), Hub.Z + Radius * Sin(A1)}, 0.035, 0.035, 5, false);
            }
            for (int I = 0; I < 8; ++I)
            {
                const double A = 2.0 * Pi * I / 8 + Pi / 8;
                Out.Tube(Wood, Hub, {Hub.X, Hub.Y + (Radius + 0.16) * Cos(A), Hub.Z + (Radius + 0.16) * Sin(A)}, 0.025, 0.02, 5, false);
            }
            Out.Tube(Mat(M::Brass), {Hub.X - 0.1, Hub.Y, Hub.Z}, {Hub.X + 0.12, Hub.Y, Hub.Z}, 0.08, 0.08, 10);
            // pedestal
            Out.Box(Wood, {Hub.X - 0.2, Hub.Y - 0.12, Hub.Z - Radius - 0.62}, {Hub.X - 0.02, Hub.Y + 0.12, Hub.Z + 0.1});
        }

        /** Square sail hanging from a yard at Top (z), Width across, Drop deep, bellied forward (+X) when set. */
        void SquareSail(Mesh& Out, const Material& Canvas, double X, double Top, double WidthTop, double WidthFoot, double Drop, double Belly)
        {
            std::vector<Ring> Rings;
            const int Rows = 6, Cols = 8;
            for (int R = 0; R <= Rows; ++R)
            {
                const double V = static_cast<double>(R) / Rows;
                const double Width = Lerp(WidthTop, WidthFoot, V);
                Ring Row;
                for (int C = 0; C <= Cols; ++C)
                {
                    const double Uc = static_cast<double>(C) / Cols;
                    const double Bulge = Belly * 4.0 * Uc * (1.0 - Uc) * (0.35 + 0.65 * 4.0 * V * (1.0 - V) + 0.2 * V);
                    Row.push_back({X + 0.25 + Bulge, (Uc - 0.5) * Width, Top - 0.25 - V * Drop});
                }
                Rings.push_back(std::move(Row));
            }
            Out.Loft(Canvas, Rings, false);
        }

        void Line(Mesh& Out, const V3& A, const V3& B, double Radius = 0.025)
        {
            Out.Tube(Mat(M::Rope), A, B, Radius, Radius, 4, false);
        }
    }

    void Cannon(Mesh& Out, const V3& At, const double Side)
    {
        const Material& Wood = Mat(M::TimberDark);
        const double Y = At.Y;
        // carriage cheeks and axles
        Out.Box(Wood, {At.X - 0.42, Y - 0.55 * Side - 0.3, At.Z}, {At.X - 0.30, Y - 0.55 * Side + 0.3, At.Z + 0.42});
        Out.Box(Wood, {At.X + 0.30, Y - 0.55 * Side - 0.3, At.Z}, {At.X + 0.42, Y - 0.55 * Side + 0.3, At.Z + 0.42});
        for (const double Dy : {-0.25, 0.25})
        {
            Out.Tube(Wood, {At.X - 0.5, Y - 0.55 * Side + Dy, At.Z + 0.12}, {At.X + 0.5, Y - 0.55 * Side + Dy, At.Z + 0.12}, 0.13, 0.13, 10);
        }
        // barrel pointing outboard
        Out.Tube(Mat(M::Iron), {At.X, Y - 1.25 * Side, At.Z + 0.5}, {At.X, Y + 0.75 * Side, At.Z + 0.52}, 0.2, 0.13, 12);
        Out.Tube(Mat(M::Iron), {At.X, Y + 0.66 * Side, At.Z + 0.52}, {At.X, Y + 0.78 * Side, At.Z + 0.52}, 0.16, 0.16, 12);
    }

    Mesh BuildShip(const ShipSpec& S, Mesh* Collision)
    {
        Mesh Out(S.Name);
        const Lines L{S};
        const std::vector<double> Xs = Stations(S.Stern, S.Bow, 44);
        const double Waterline = 0.08;
        const double WaleLo = S.Deck - 1.15, WaleHi = S.Deck - 0.62;

        // ---- hull planking in painted bands
        HullBand(Out, L, Xs, -10.0, Waterline, 6, 0, Mat(S.Bottom), true);
        for (const double Side : {1.0, -1.0})
        {
            HullBand(Out, L, Xs, Waterline, WaleLo, 4, Side, Mat(S.Topsides), false);
            HullBand(Out, L, Xs, WaleLo, WaleHi, 1, Side, Mat(S.Band), false);
            HullBand(Out, L, Xs, WaleHi, 100.0, 3, Side, Mat(S.Topsides), false);
        }

        // ---- transom (flat stern) with the great cabin windows
        {
            const double X = S.Stern;
            const double Kz = L.KeelZ(X);
            const double Top = L.RailZ(X);
            std::vector<V3> Face;
            const int N = 8;
            for (int I = 0; I <= N; ++I)
            {
                const double Z = Kz + (Top - Kz) * I / N;
                Face.push_back({X, -L.HalfWidth(X, Z), Z});
            }
            for (int I = N; I >= 0; --I)
            {
                const double Z = Kz + (Top - Kz) * I / N;
                Face.push_back({X, L.HalfWidth(X, Z), Z});
            }
            Out.Fan(Mat(S.Topsides), Face, {-1, 0, 0});
            const double WinLo = S.Deck - 1.25, WinHi = S.Deck - 0.55;
            const double Half = L.HalfWidth(X, WinHi) * 0.72;
            const int Windows = 4;
            for (int I = 0; I < Windows; ++I)
            {
                const double Y0 = -Half + 2.0 * Half * I / Windows + 0.12, Y1 = -Half + 2.0 * Half * (I + 1) / Windows - 0.12;
                Out.Quad(Mat(M::Glass), {X - 0.02, Y1, WinLo}, {X - 0.02, Y0, WinLo}, {X - 0.02, Y0, WinHi}, {X - 0.02, Y1, WinHi});
                Out.Box(Mat(M::TimberDark), {X - 0.06, (Y0 + Y1) / 2 - 0.025, WinLo}, {X, (Y0 + Y1) / 2 + 0.025, WinHi});
                Out.Box(Mat(M::TimberDark), {X - 0.06, Y0 - 0.06, WinLo}, {X, Y0, WinHi});
                Out.Box(Mat(M::TimberDark), {X - 0.06, Y1, WinLo}, {X, Y1 + 0.06, WinHi});
            }
            Out.Box(Mat(S.Band), {X - 0.08, -Half - 0.1, WinHi}, {X, Half + 0.1, WinHi + 0.14});
            Out.Box(Mat(S.Band), {X - 0.08, -Half - 0.1, WinLo - 0.14}, {X, Half + 0.1, WinLo});
            // stern lanterns
            for (const double Y : {-Half * 0.9, Half * 0.9})
            {
                Out.Tube(Mat(M::Iron), {X - 0.25, Y, Top}, {X - 0.25, Y, Top + 0.6}, 0.02, 0.02, 4, false);
                Out.Box(Mat(M::Brass), {X - 0.38, Y - 0.12, Top + 0.6}, {X - 0.12, Y + 0.12, Top + 0.95});
            }
        }

        // ---- keel, stem, sternpost and rudder
        {
            const Material& Wood = Mat(M::TimberDark);
            std::vector<V3> Stem;
            for (int I = 0; I <= 10; ++I)
            {
                const double X = S.Stern + (S.Bow - S.Stern) * (0.74 + 0.26 * I / 10.0);
                Stem.push_back({X + 0.04, 0, L.KeelZ(X) - 0.08});
            }
            Stem.push_back({S.Bow + 0.15, 0, L.RailZ(S.Bow) + 0.1});
            for (std::size_t I = 1; I < Stem.size(); ++I) Out.Tube(Wood, Stem[I - 1], Stem[I], 0.13, 0.13, 6, false);
            Out.Box(Wood, {S.Stern + 0.8, -0.13, S.Keel - 0.2}, {S.Stern + (S.Bow - S.Stern) * 0.76, 0.13, S.Keel + 0.05});
            Out.Box(Wood, {S.Stern - 0.14, -0.14, S.Keel - 0.2}, {S.Stern + 0.8, 0.14, L.KeelZ(S.Stern) + 0.1});
            Out.Box(Wood, {S.Stern - 0.65, -0.08, S.Keel + 0.2}, {S.Stern - 0.12, 0.08, S.Deck - 0.9});
            Out.Box(Wood, {S.Stern - 0.4, -0.07, S.Deck - 0.9}, {S.Stern - 0.12, 0.07, S.Deck - 0.2});
        }

        // ---- decks
        const double RaisedFrom = std::min(S.RaisedFrom, S.Bow);
        const auto DeckStrip = [&](double From, double To, double Z, int Count)
        {
            std::vector<Ring> Rings;
            for (const double X : Stations(From, To, Count))
            {
                const double W = std::max(0.02, L.HalfWidth(X, Z) - 0.14);
                Rings.push_back({{X, -W, Z}, {X, W, Z}});
            }
            Out.Loft(Mat(M::Deck), Rings, false, true);
        };
        const double DeckEnd = S.Bow - 0.35;
        DeckStrip(S.Stern + 0.12, std::min(RaisedFrom, DeckEnd), S.Deck, 36);
        if (RaisedFrom < DeckEnd)
        {
            DeckStrip(RaisedFrom, DeckEnd, S.RaisedDeck, 10);
            // stair: two treads across the deck, a face and a rail on the raised deck
            const double W = L.HalfWidth(RaisedFrom, S.Deck) - 0.14;
            const double Rise = S.RaisedDeck - S.Deck;
            Out.Box(Mat(M::DeckWorn), {RaisedFrom - 0.9, -W * 0.45, S.Deck}, {RaisedFrom - 0.45, W * 0.45, S.Deck + Rise / 3.0});
            Out.Box(Mat(M::DeckWorn), {RaisedFrom - 0.45, -W * 0.45, S.Deck}, {RaisedFrom, W * 0.45, S.Deck + 2.0 * Rise / 3.0});
            Out.Quad(Mat(M::Timber), {RaisedFrom, W, S.Deck}, {RaisedFrom, -W, S.Deck}, {RaisedFrom, -W, S.RaisedDeck}, {RaisedFrom, W, S.RaisedDeck});
            for (const double Side : {-1.0, 1.0})
            {
                const double Y = Side * W * 0.72;
                Out.Tube(Mat(M::Timber), {RaisedFrom + 0.05, Y, S.RaisedDeck}, {RaisedFrom + 0.05, Y, S.RaisedDeck + 0.95}, 0.05, 0.05, 6);
            }
            Out.Box(Mat(M::Timber), {RaisedFrom, -W * 0.72 - 0.05, S.RaisedDeck + 0.92}, {RaisedFrom + 0.1, -W * 0.45, S.RaisedDeck + 1.0});
            Out.Box(Mat(M::Timber), {RaisedFrom, W * 0.45, S.RaisedDeck + 0.92}, {RaisedFrom + 0.1, W * 0.72 + 0.05, S.RaisedDeck + 1.0});
        }

        // ---- bulwarks: inner face, cap rail and stanchions
        for (const double Side : {1.0, -1.0})
        {
            std::vector<Ring> Inner, Cap;
            for (const double X : Stations(S.Stern + 0.12, S.Bow - 0.2, 40))
            {
                const double Z = L.DeckAt(X);
                const double Top = L.RailZ(X);
                const double WIn = std::max(0.02, L.HalfWidth(X, Z) - 0.14);
                const double WOut = L.HalfWidth(X, Top);
                if (Side > 0)
                {
                    Inner.push_back({{X, WIn, Z}, {X, WIn, Top}});
                    Cap.push_back({{X, WIn - 0.03, Top}, {X, WOut + 0.04, Top}});
                }
                else
                {
                    Inner.push_back({{X, -WIn, Z}, {X, -WIn, Top}});
                    Cap.push_back({{X, -WOut - 0.04, Top}, {X, -WIn + 0.03, Top}});
                }
            }
            Out.Loft(Mat(M::Timber), Inner, false, Side > 0);
            Out.Loft(Mat(M::TimberDark), Cap, false, true);
            for (const double X : Stations(S.Stern + 0.9, S.Bow - 2.0, 18))
            {
                const double Z = L.DeckAt(X);
                const double W = L.HalfWidth(X, Z) - 0.2;
                Out.Box(Mat(M::TimberDark), {X - 0.06, Side * W - 0.06, Z}, {X + 0.06, Side * W + 0.06, L.RailZ(X) - 0.02});
            }
            // gun ports: dark openings on the outside of the bulwark
            for (const double X : S.GunX)
            {
                const double Z0 = S.Deck + 0.25, Z1 = S.Deck + 0.8;
                const double W = L.HalfWidth(X, (Z0 + Z1) / 2.0) + 0.02;
                if (Side > 0)
                    Out.Quad(Mat(M::Glass), {X + 0.35, W, Z0}, {X - 0.35, W, Z0}, {X - 0.35, W, Z1}, {X + 0.35, W, Z1});
                else
                    Out.Quad(Mat(M::Glass), {X - 0.35, -W, Z0}, {X + 0.35, -W, Z0}, {X + 0.35, -W, Z1}, {X - 0.35, -W, Z1});
            }
        }

        // ---- deck fittings
        for (const auto& [From, To] : S.Hatches)
        {
            const double Z = L.DeckAt(From);
            Out.Box(Mat(M::Timber), {From, -1.05, Z}, {To, 1.05, Z + 0.34});
            Out.Box(Mat(M::TimberDark), {From + 0.1, -0.95, Z + 0.34}, {To - 0.1, 0.95, Z + 0.4});
            for (int I = 1; I < 6; ++I)
            {
                const double X = From + 0.1 + (To - From - 0.2) * I / 6.0;
                Out.Box(Mat(M::Timber), {X - 0.03, -0.95, Z + 0.4}, {X + 0.03, 0.95, Z + 0.43});
            }
        }
        if (S.CapstanX < 1e8)
        {
            const double Z = L.DeckAt(S.CapstanX);
            Out.Tube(Mat(M::TimberDark), {S.CapstanX, 0, Z}, {S.CapstanX, 0, Z + 0.95}, 0.42, 0.3, 12);
            Out.Tube(Mat(M::Timber), {S.CapstanX, 0, Z + 0.8}, {S.CapstanX, 0, Z + 0.98}, 0.48, 0.48, 12);
        }
        if (S.HelmX < 1e8)
        {
            Wheel(Out, {S.HelmX, 0, L.DeckAt(S.HelmX) + 1.2}, 0.55);
        }
        for (std::size_t I = 0; I < S.GunX.size(); ++I)
        {
            for (const double Side : {1.0, -1.0})
            {
                const double X = S.GunX[I];
                const double W = L.HalfWidth(X, S.Deck) - 0.14;
                Cannon(Out, {X, Side * (W - 0.55), S.Deck}, Side);
            }
        }
        if (S.Cargo)
        {
            Rng R(HashString(S.Name));
            for (const double Side : {1.0, -1.0})
            {
                for (const double X : {S.Stern + 2.2, S.Stern + 3.0})
                {
                    const double W = L.HalfWidth(X, S.Deck) - 0.14;
                    Barrel(Out, {X, Side * (W - 0.4), S.Deck}, 0.9, 0.3 + 0.02 * R.Unit());
                }
            }
            const double X = S.Stern + 4.6;
            const double W = L.HalfWidth(X, S.Deck) - 0.14;
            Crate(Out, {X - 0.45, W - 1.2, S.Deck}, {X + 0.45, W - 0.5, S.Deck + 0.6});
            Crate(Out, {X - 0.35, W - 1.1, S.Deck + 0.6}, {X + 0.3, W - 0.55, S.Deck + 1.05});
        }

        // ---- rig
        const Material& Canvas = Mat(S.BlackSails ? M::CanvasBlack : M::Canvas);
        const Material& Spar = Mat(M::TimberDark);
        std::vector<V3> Mastheads;
        for (std::size_t I = 0; I < S.Masts.size(); ++I)
        {
            const double X = S.Masts[I];
            const double H = I < S.MastHeights.size() ? S.MastHeights[I] : 18.0;
            const double Z = L.DeckAt(X);
            const double Lower = Z + H * 0.52, TopM = Z + H * 0.84, Head = Z + H;
            Out.Tube(Spar, {X, 0, Z - 0.4}, {X, 0, Lower + 0.9}, 0.3, 0.23, 12);
            Out.Tube(Spar, {X, 0, Lower - 1.2}, {X, 0, TopM + 0.5}, 0.17, 0.12, 10);
            Out.Tube(Spar, {X, 0, TopM - 0.8}, {X, 0, Head}, 0.1, 0.05, 8);
            Out.Box(Spar, {X - 0.9, -1.3, Lower}, {X + 0.9, 1.3, Lower + 0.12});
            Out.Box(Spar, {X - 0.45, -0.7, TopM}, {X + 0.45, 0.7, TopM + 0.08});
            Out.Box(Mat(M::Iron), {X - 0.12, -0.12, Z}, {X + 0.12, 0.12, Z + 0.25});
            Mastheads.push_back({X, 0, Head});

            const double Beam = S.HalfBeam * 2.0;
            const bool Spanker = S.SpankerOnAftMast && I == 0 && S.Masts.size() > 1;
            const bool Single = S.Masts.size() == 1;
            struct Yard { double Z, Length, Drop; };
            std::vector<Yard> Yards;
            if (!Spanker && !Single) Yards.push_back({Lower - 1.3, Beam * 1.9, Lower - 1.3 - (L.RailZ(X) + 2.3)});
            Yards.push_back({TopM - 0.9, Beam * 1.45, TopM - 0.9 - (Lower + 0.6)});
            Yards.push_back({Head - 0.9, Beam * 1.0, Head - 0.9 - (TopM + 0.3)});
            if (Single) Yards.resize(1);
            for (std::size_t K = 0; K < Yards.size(); ++K)
            {
                const Yard& Y = Yards[K];
                const double Half = Y.Length / 2.0;
                Out.Tube(Spar, {X + 0.25, 0, Y.Z}, {X + 0.25, Half, Y.Z}, 0.13, 0.06, 8, false);
                Out.Tube(Spar, {X + 0.25, 0, Y.Z}, {X + 0.25, -Half, Y.Z}, 0.13, 0.06, 8, false);
                if (S.SailsSet)
                {
                    SquareSail(Out, Canvas, X, Y.Z, Y.Length - 0.6, Y.Length + (K == 0 ? 0.4 : 0.8), Y.Drop, 0.9 + 0.25 * K);
                }
                else
                {
                    Out.Tube(Canvas, {X + 0.35, -Half + 0.4, Y.Z + 0.12}, {X + 0.35, Half - 0.4, Y.Z + 0.12}, 0.26, 0.26, 8);
                }
            }
            if (Spanker || Single)
            {
                // fore-and-aft sail: boom and gaff towards the stern (-X)
                const double BoomZ = Z + 2.2;
                const double BoomLen = std::min(8.0, X - S.Stern + 2.5);
                const double GaffZ = Lower - 0.2;
                Out.Tube(Spar, {X - 0.3, 0, BoomZ}, {X - BoomLen, 0, BoomZ + 0.3}, 0.11, 0.08, 8);
                Out.Tube(Spar, {X - 0.3, 0, GaffZ}, {X - BoomLen * 0.72, 0, GaffZ + 2.6}, 0.09, 0.06, 8);
                if (S.SailsSet)
                {
                    std::vector<Ring> Rings;
                    for (int R = 0; R <= 5; ++R)
                    {
                        const double V = R / 5.0;
                        Ring Row;
                        for (int C = 0; C <= 6; ++C)
                        {
                            const double Uc = C / 6.0;
                            const V3 Top = {Lerp(X - 0.35, X - BoomLen * 0.72, Uc), 0.0, Lerp(GaffZ - 0.1, GaffZ + 2.5, Uc)};
                            const V3 Bottom = {Lerp(X - 0.35, X - BoomLen + 0.2, Uc), 0.0, Lerp(BoomZ + 0.15, BoomZ + 0.42, Uc)};
                            V3 P = Top + (Bottom - Top) * V;
                            P.Y = 0.55 * 4.0 * Uc * (1.0 - Uc) * (0.4 + 0.6 * V);
                            Row.push_back(P);
                        }
                        Rings.push_back(std::move(Row));
                    }
                    Out.Loft(Canvas, Rings, false);
                }
                else
                {
                    Out.Tube(Canvas, {X - 0.6, 0, BoomZ + 0.25}, {X - BoomLen + 0.4, 0, BoomZ + 0.5}, 0.22, 0.22, 8);
                }
            }

            // shrouds with ratlines, both sides, lower and top
            for (const double Side : {1.0, -1.0})
            {
                std::vector<std::pair<V3, V3>> Shrouds;
                for (int K = 0; K < 4; ++K)
                {
                    const double Cx = X - 0.35 - 0.55 * K;
                    const double W = L.HalfWidth(Cx, L.RailZ(Cx)) + 0.18;
                    Shrouds.push_back({{Cx, Side * W, L.RailZ(Cx) - 0.35}, {X - 0.1, Side * 0.32, Lower - 0.1}});
                }
                for (const auto& [A, B] : Shrouds) Line(Out, A, B);
                for (int K = 1; K < 18; ++K)
                {
                    const double T = K / 18.0;
                    const V3 A = Shrouds.front().first + (Shrouds.front().second - Shrouds.front().first) * T;
                    const V3 B = Shrouds.back().first + (Shrouds.back().second - Shrouds.back().first) * T;
                    if (T > 0.92) break;
                    Line(Out, A, B, 0.012);
                }
                // chain wale plank
                const double Cx = X - 0.9;
                const double W = L.HalfWidth(Cx, L.RailZ(Cx) - 0.35);
                Out.Box(Mat(M::TimberDark), {X - 2.1, Side > 0 ? W - 0.02 : -W - 0.2, L.RailZ(Cx) - 0.45}, {X + 0.1, Side > 0 ? W + 0.2 : -W + 0.02, L.RailZ(Cx) - 0.33});
                for (int K = 0; K < 3; ++K)
                {
                    Line(Out, {X - 0.3 - 0.4 * K, Side * 1.25, Lower + 0.1}, {X - 0.05, Side * 0.18, TopM - 0.4}, 0.018);
                }
                // backstay
                const double Bx = X - 3.2;
                Line(Out, {Bx, Side * (L.HalfWidth(Bx, L.RailZ(Bx)) + 0.1), L.RailZ(Bx) - 0.2}, {X, Side * 0.12, TopM + 0.2}, 0.02);
            }
        }

        // ---- bowsprit, jib boom, stays and head sails
        {
            const double Z0 = L.RailZ(S.Bow) - 0.3;
            const V3 Heel{S.Bow - 1.2, 0, Z0 - 0.3};
            const double Cs = 0.94, Sn = 0.34;  // about 20 degrees of steeve
            const V3 Tip{S.Bow + S.Bowsprit * Cs, 0, Z0 + S.Bowsprit * Sn};
            const V3 Boom{S.Bow + S.Bowsprit * 1.55 * Cs, 0, Z0 + S.Bowsprit * 1.55 * Sn};
            Out.Tube(Spar, Heel, Tip, 0.26, 0.16, 10);
            Out.Tube(Spar, Tip + V3{-1.8 * Cs, 0, -1.8 * Sn + 0.14}, Boom, 0.1, 0.06, 8);
            if (!Mastheads.empty())
            {
                const V3 Fore = Mastheads.back();
                const double ForeZ = L.DeckAt(Fore.X);
                const double ForeH = Fore.Z - ForeZ;
                Line(Out, {Fore.X, 0, ForeZ + ForeH * 0.52}, Tip, 0.04);
                Line(Out, {Fore.X, 0, ForeZ + ForeH * 0.84}, Boom, 0.03);
                Line(Out, Tip, {Tip.X - 0.5, 0, S.Keel + 1.2}, 0.03);  // bobstay
                if (S.SailsSet)
                {
                    const auto Jib = [&](const V3& Head, const V3& Tack, const V3& Clew)
                    {
                        std::vector<Ring> Rings;
                        for (int R = 0; R <= 4; ++R)
                        {
                            const double V = R / 4.0;
                            const V3 A = Head + (Tack - Head) * V;
                            const V3 B = Head + (Clew - Head) * V;
                            Ring Row;
                            for (int C = 0; C <= 4; ++C)
                            {
                                const double Uc = C / 4.0;
                                V3 P = A + (B - A) * Uc;
                                P.Y += 0.5 * V * 4.0 * Uc * (1.0 - Uc);
                                Row.push_back(P);
                            }
                            Rings.push_back(std::move(Row));
                        }
                        Out.Loft(Canvas, Rings, false);
                    };
                    const V3 HeadA{Fore.X + 0.4, 0, ForeZ + ForeH * 0.5};
                    Jib(HeadA, Tip + V3{-0.3, 0, -0.1}, {S.Bow - 0.8, -0.6, L.RailZ(S.Bow) + 1.2});
                    const V3 HeadB{Fore.X + 0.3, 0, ForeZ + ForeH * 0.8};
                    Jib(HeadB, Boom + V3{-0.4, 0, -0.1}, {Tip.X - 1.0, -0.9, Tip.Z + 1.4});
                }
            }
            for (std::size_t I = 1; I < Mastheads.size(); ++I)
            {
                const V3 A = Mastheads[I - 1];
                const V3 B = Mastheads[I];
                Line(Out, {A.X, 0, L.DeckAt(A.X) + (A.Z - L.DeckAt(A.X)) * 0.52}, {B.X, 0, L.DeckAt(B.X) + 1.4}, 0.04);
                Line(Out, {A.X, 0, A.Z - 0.3}, {B.X, 0, L.DeckAt(B.X) + (B.Z - L.DeckAt(B.X)) * 0.56}, 0.025);
            }
        }

        // ---- walkable collision: deck slab, bulwark walls, masts, fittings and the hull shell
        if (Collision)
        {
            Material C;
            C.Name = "Collision";
            const auto Slab = [&](double From, double To, double Z)
            {
                const std::vector<double> Seg = Stations(From, To, 14);
                for (std::size_t I = 1; I < Seg.size(); ++I)
                {
                    const double W = std::min(L.HalfWidth(Seg[I - 1], Z), L.HalfWidth(Seg[I], Z)) - 0.1;
                    if (W < 0.2) continue;
                    Collision->Box(C, {Seg[I - 1], -W, Z - 0.3}, {Seg[I], W, Z});
                }
            };
            Slab(S.Stern + 0.05, std::min(RaisedFrom, DeckEnd), S.Deck);
            if (RaisedFrom < DeckEnd)
            {
                Slab(RaisedFrom, DeckEnd, S.RaisedDeck);
                // ramp across the full width up the step
                const double W = L.HalfWidth(RaisedFrom, S.Deck) - 0.14;
                const double Rise = S.RaisedDeck - S.Deck;
                const V3 A{RaisedFrom - 1.3, -W, S.Deck}, B{RaisedFrom - 1.3, W, S.Deck}, D{RaisedFrom, -W, S.RaisedDeck}, E{RaisedFrom, W, S.RaisedDeck};
                Collision->Quad(C, A, D, E, B);
                Collision->Box(C, {RaisedFrom - 0.01, -W, S.Deck}, {RaisedFrom + 0.3, W, S.Deck + Rise});
            }
            for (const double Side : {1.0, -1.0})
            {
                const std::vector<double> Seg = Stations(S.Stern + 0.05, S.Bow - 0.2, 24);
                for (std::size_t I = 1; I < Seg.size(); ++I)
                {
                    const double X0 = Seg[I - 1], X1 = Seg[I];
                    const double Z = L.DeckAt((X0 + X1) / 2.0);
                    const double Y0 = Side * (L.HalfWidth(X0, Z) - 0.08), Y1 = Side * (L.HalfWidth(X1, Z) - 0.08);
                    const double Top = std::max(L.RailZ(X0), L.RailZ(X1));
                    OrientedBox(*Collision, C, {(X0 + X1) / 2.0, (Y0 + Y1) / 2.0, (Z - 0.3 + Top) / 2.0}, X1 - X0, Y1 - Y0,
                        {std::sqrt((X1 - X0) * (X1 - X0) + (Y1 - Y0) * (Y1 - Y0)) / 2.0 + 0.05, 0.09, (Top - Z + 0.3) / 2.0});
                }
            }
            Collision->Box(C, {S.Stern - 0.1, -L.HalfWidth(S.Stern, S.Deck), S.Deck - 0.3}, {S.Stern + 0.12, L.HalfWidth(S.Stern, S.Deck), L.RailZ(S.Stern)});
            for (const double X : S.Masts)
            {
                const double Z = L.DeckAt(X);
                Collision->Box(C, {X - 0.3, -0.3, Z}, {X + 0.3, 0.3, Z + 4.0});
            }
            for (const auto& [From, To] : S.Hatches)
            {
                const double Z = L.DeckAt(From);
                Collision->Box(C, {From, -1.05, Z}, {To, 1.05, Z + 0.43});
            }
            if (S.CapstanX < 1e8)
            {
                const double Z = L.DeckAt(S.CapstanX);
                Collision->Box(C, {S.CapstanX - 0.45, -0.45, Z}, {S.CapstanX + 0.45, 0.45, Z + 1.0});
            }
            HullBand(*Collision, L, Stations(S.Stern, S.Bow, 16), -10.0, S.Deck - 0.3, 4, 0, C, true);
        }
        return Out;
    }
}
