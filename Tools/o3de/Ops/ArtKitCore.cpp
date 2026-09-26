#include "ArtKitCore.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <cstring>
#include <map>

namespace DarkArisen::Tools::Art
{
    // ------------------------------------------------------------------ math

    double Sin(const double X)
    {
        const double TwoPi = 2.0 * Pi;
        double R = X - std::floor(X / TwoPi + 0.5) * TwoPi;  // [-pi, pi]
        if (R > Pi / 2.0) R = Pi - R;
        else if (R < -Pi / 2.0) R = -Pi - R;
        const double R2 = R * R;
        return R * (1.0 + R2 * (-1.0 / 6.0 + R2 * (1.0 / 120.0 + R2 * (-1.0 / 5040.0 + R2 * (1.0 / 362880.0 +
            R2 * (-1.0 / 39916800.0 + R2 * (1.0 / 6227020800.0 + R2 * (-1.0 / 1307674368000.0))))))));
    }

    double Cos(const double X) { return Sin(X + Pi / 2.0); }
    double Clamp(const double V, const double Lo, const double Hi) { return V < Lo ? Lo : V > Hi ? Hi : V; }
    double Lerp(const double A, const double B, const double T) { return A + (B - A) * T; }
    double SmoothStep(const double E0, const double E1, const double X)
    {
        const double T = Clamp((X - E0) / (E1 - E0), 0.0, 1.0);
        return T * T * (3.0 - 2.0 * T);
    }

    double Dot(const V3& A, const V3& B) { return A.X * B.X + A.Y * B.Y + A.Z * B.Z; }
    V3 Cross(const V3& A, const V3& B) { return {A.Y * B.Z - A.Z * B.Y, A.Z * B.X - A.X * B.Z, A.X * B.Y - A.Y * B.X}; }
    double Length(const V3& A) { return std::sqrt(Dot(A, A)); }
    V3 Normalize(const V3& A)
    {
        const double L = Length(A);
        return L > 1e-12 ? A * (1.0 / L) : V3{0, 1, 0};
    }

    Xf Xf::Translate(const V3& T) { Xf R; R.M[3] = T.X; R.M[7] = T.Y; R.M[11] = T.Z; return R; }
    Xf Xf::RotateX(const double A) { Xf R; const double C = Cos(A), S = Sin(A); R.M = {1, 0, 0, 0, 0, C, -S, 0, 0, S, C, 0}; return R; }
    Xf Xf::RotateY(const double A) { Xf R; const double C = Cos(A), S = Sin(A); R.M = {C, 0, S, 0, 0, 1, 0, 0, -S, 0, C, 0}; return R; }
    Xf Xf::RotateZ(const double A) { Xf R; const double C = Cos(A), S = Sin(A); R.M = {C, -S, 0, 0, S, C, 0, 0, 0, 0, 1, 0}; return R; }
    Xf Xf::Scale(const V3& S) { Xf R; R.M = {S.X, 0, 0, 0, 0, S.Y, 0, 0, 0, 0, S.Z, 0}; return R; }
    Xf Xf::operator*(const Xf& O) const
    {
        Xf R;
        for (int Row = 0; Row < 3; ++Row)
        {
            for (int Col = 0; Col < 4; ++Col)
            {
                double Sum = Col == 3 ? M[static_cast<std::size_t>(Row * 4 + 3)] : 0.0;
                for (int K = 0; K < 3; ++K) Sum += M[static_cast<std::size_t>(Row * 4 + K)] * O.M[static_cast<std::size_t>(K * 4 + Col)];
                R.M[static_cast<std::size_t>(Row * 4 + Col)] = Sum;
            }
        }
        return R;
    }
    V3 Xf::Point(const V3& P) const
    {
        return {M[0] * P.X + M[1] * P.Y + M[2] * P.Z + M[3], M[4] * P.X + M[5] * P.Y + M[6] * P.Z + M[7],
            M[8] * P.X + M[9] * P.Y + M[10] * P.Z + M[11]};
    }
    V3 Xf::Direction(const V3& D) const
    {
        return {M[0] * D.X + M[1] * D.Y + M[2] * D.Z, M[4] * D.X + M[5] * D.Y + M[6] * D.Z, M[8] * D.X + M[9] * D.Y + M[10] * D.Z};
    }

    std::uint32_t Rng::Next()
    {
        State = State * 747796405u + 2891336453u;
        std::uint32_t Word = ((State >> ((State >> 28u) + 4u)) ^ State) * 277803737u;
        return (Word >> 22u) ^ Word;
    }
    double Rng::Unit() { return static_cast<double>(Next() >> 8) / 16777216.0; }
    double Rng::Range(const double Lo, const double Hi) { return Lo + (Hi - Lo) * Unit(); }

    std::uint32_t HashString(const std::string_view Text)
    {
        std::uint32_t H = 2166136261u;
        for (const char C : Text) H = (H ^ static_cast<std::uint8_t>(C)) * 16777619u;
        return H;
    }

    // ------------------------------------------------------------------ noise

    namespace
    {
        std::uint32_t Hash2(std::uint32_t X, std::uint32_t Y, std::uint32_t Seed)
        {
            std::uint32_t H = X * 0x8da6b343u ^ Y * 0xd8163841u ^ Seed * 0xcb1ab31fu;
            H ^= H >> 13;
            H *= 0x85ebca6bu;
            H ^= H >> 16;
            H *= 0xc2b2ae35u;
            H ^= H >> 16;
            return H;
        }

        std::int64_t Fade(const std::int64_t F)  // F in [0, 65536]; smoothstep in fixed point
        {
            return (F * F / 65536) * (3 * 65536 - 2 * F) / 65536;
        }
    }

    int Noise(const int X, const int Y, const int Size, const int Period, const std::uint32_t Seed)
    {
        const int Cell = std::max(1, Size / Period);
        const int IX = (X / Cell) % Period, IY = (Y / Cell) % Period;
        const int IX1 = (IX + 1) % Period, IY1 = (IY + 1) % Period;
        const std::int64_t FX = Fade(static_cast<std::int64_t>(X % Cell) * 65536 / Cell);
        const std::int64_t FY = Fade(static_cast<std::int64_t>(Y % Cell) * 65536 / Cell);
        const auto At = [&](int A, int B) { return static_cast<std::int64_t>(Hash2(static_cast<std::uint32_t>(A), static_cast<std::uint32_t>(B), Seed) >> 16); };
        const std::int64_t Top = At(IX, IY) + (At(IX1, IY) - At(IX, IY)) * FX / 65536;
        const std::int64_t Bottom = At(IX, IY1) + (At(IX1, IY1) - At(IX, IY1)) * FX / 65536;
        return static_cast<int>(Top + (Bottom - Top) * FY / 65536);
    }

    int Fbm(const int X, const int Y, const int Size, const int BasePeriod, const int Octaves, const std::uint32_t Seed)
    {
        std::int64_t Sum = 0, Weight = 0, Amplitude = 256;
        int Period = BasePeriod;
        for (int Octave = 0; Octave < Octaves && Period <= Size; ++Octave)
        {
            Sum += Noise(X, Y, Size, Period, Seed + static_cast<std::uint32_t>(Octave) * 1013u) * Amplitude;
            Weight += Amplitude;
            Amplitude /= 2;
            Period *= 2;
        }
        return static_cast<int>(Sum / std::max<std::int64_t>(1, Weight));
    }

    Image NormalFromHeight(const std::vector<int>& Height, const int Size, const int StrengthPercent)
    {
        Image Out;
        Out.Width = Out.Height = Size;
        Out.Channels = 3;
        Out.Pixels.resize(static_cast<std::size_t>(Size * Size * 3));
        const auto H = [&](int X, int Y) { return static_cast<double>(Height[static_cast<std::size_t>(((Y + Size) % Size) * Size + (X + Size) % Size)]) / 65535.0; };
        const double Strength = StrengthPercent / 100.0;
        for (int Y = 0; Y < Size; ++Y)
        {
            for (int X = 0; X < Size; ++X)
            {
                const double DX = (H(X + 1, Y - 1) + 2 * H(X + 1, Y) + H(X + 1, Y + 1)) - (H(X - 1, Y - 1) + 2 * H(X - 1, Y) + H(X - 1, Y + 1));
                const double DY = (H(X - 1, Y + 1) + 2 * H(X, Y + 1) + H(X + 1, Y + 1)) - (H(X - 1, Y - 1) + 2 * H(X, Y - 1) + H(X + 1, Y - 1));
                const V3 N = Normalize({-DX * Strength, DY * Strength, 1.0});
                std::uint8_t* P = &Out.Pixels[static_cast<std::size_t>((Y * Size + X) * 3)];
                P[0] = static_cast<std::uint8_t>(std::lround((N.X * 0.5 + 0.5) * 255.0));
                P[1] = static_cast<std::uint8_t>(std::lround((N.Y * 0.5 + 0.5) * 255.0));
                P[2] = static_cast<std::uint8_t>(std::lround((N.Z * 0.5 + 0.5) * 255.0));
            }
        }
        return Out;
    }

    // ------------------------------------------------------------------ PNG (zlib, fixed Huffman)

    namespace
    {
        class BitWriter
        {
        public:
            std::string Bytes;
            void Bits(std::uint32_t Value, int Count)
            {
                for (int I = 0; I < Count; ++I)
                {
                    Accumulator |= ((Value >> I) & 1u) << Filled;
                    if (++Filled == 8) Flush();
                }
            }
            /** Huffman codes are sent most significant bit first. */
            void Code(std::uint32_t Code, int Length)
            {
                for (int I = Length - 1; I >= 0; --I) Bits((Code >> I) & 1u, 1);
            }
            void Align() { if (Filled) Flush(); }
        private:
            std::uint32_t Accumulator = 0;
            int Filled = 0;
            void Flush()
            {
                Bytes.push_back(static_cast<char>(Accumulator & 0xFFu));
                Accumulator = 0;
                Filled = 0;
            }
        };

        void Literal(BitWriter& W, const int Symbol)
        {
            if (Symbol <= 143) W.Code(static_cast<std::uint32_t>(0x30 + Symbol), 8);
            else if (Symbol <= 255) W.Code(static_cast<std::uint32_t>(0x190 + Symbol - 144), 9);
            else if (Symbol <= 279) W.Code(static_cast<std::uint32_t>(Symbol - 256), 7);
            else W.Code(static_cast<std::uint32_t>(0xC0 + Symbol - 280), 8);
        }

        constexpr int LengthBase[29] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
        constexpr int LengthExtra[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
        constexpr int DistBase[30] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073,
            4097, 6145, 8193, 12289, 16385, 24577};
        constexpr int DistExtra[30] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

        std::string Deflate(const std::string& Data)
        {
            BitWriter W;
            W.Bits(1, 1);  // final block
            W.Bits(1, 2);  // fixed Huffman
            constexpr int Window = 32768, HashSize = 1 << 15, MaxChain = 48;
            std::vector<int> Head(HashSize, -1), Prev(Data.size(), -1);
            const auto HashAt = [&](std::size_t I)
            {
                return ((static_cast<std::uint8_t>(Data[I]) << 10) ^ (static_cast<std::uint8_t>(Data[I + 1]) << 5) ^
                           static_cast<std::uint8_t>(Data[I + 2])) & (HashSize - 1);
            };
            std::size_t I = 0;
            const std::size_t N = Data.size();
            const auto Insert = [&](std::size_t At)
            {
                if (At + 2 >= N) return;
                const int H = HashAt(At);
                Prev[At] = Head[static_cast<std::size_t>(H)];
                Head[static_cast<std::size_t>(H)] = static_cast<int>(At);
            };
            while (I < N)
            {
                int BestLength = 0, BestDistance = 0;
                if (I + 2 < N)
                {
                    int Candidate = Head[static_cast<std::size_t>(HashAt(I))];
                    for (int Chain = 0; Candidate >= 0 && Chain < MaxChain; ++Chain)
                    {
                        const int Distance = static_cast<int>(I) - Candidate;
                        if (Distance > Window) break;
                        int Length = 0;
                        while (Length < 258 && I + static_cast<std::size_t>(Length) < N &&
                            Data[static_cast<std::size_t>(Candidate + Length)] == Data[I + static_cast<std::size_t>(Length)])
                        {
                            ++Length;
                        }
                        if (Length > BestLength)
                        {
                            BestLength = Length;
                            BestDistance = Distance;
                            if (Length == 258) break;
                        }
                        Candidate = Prev[static_cast<std::size_t>(Candidate)];
                    }
                }
                if (BestLength >= 3)
                {
                    int Code = 28;
                    while (LengthBase[Code] > BestLength) --Code;
                    Literal(W, 257 + Code);
                    W.Bits(static_cast<std::uint32_t>(BestLength - LengthBase[Code]), LengthExtra[Code]);
                    int DCode = 29;
                    while (DistBase[DCode] > BestDistance) --DCode;
                    W.Code(static_cast<std::uint32_t>(DCode), 5);
                    W.Bits(static_cast<std::uint32_t>(BestDistance - DistBase[DCode]), DistExtra[DCode]);
                    for (int K = 0; K < BestLength; ++K) Insert(I + static_cast<std::size_t>(K));
                    I += static_cast<std::size_t>(BestLength);
                }
                else
                {
                    Literal(W, static_cast<std::uint8_t>(Data[I]));
                    Insert(I);
                    ++I;
                }
            }
            Literal(W, 256);
            W.Align();
            return W.Bytes;
        }

        std::uint32_t Crc32(const std::string& Bytes, std::size_t From)
        {
            static const std::array<std::uint32_t, 256> Table = []
            {
                std::array<std::uint32_t, 256> T{};
                for (std::uint32_t N = 0; N < 256; ++N)
                {
                    std::uint32_t C = N;
                    for (int K = 0; K < 8; ++K) C = (C & 1u) ? 0xEDB88320u ^ (C >> 1) : C >> 1;
                    T[N] = C;
                }
                return T;
            }();
            std::uint32_t C = 0xFFFFFFFFu;
            for (std::size_t I = From; I < Bytes.size(); ++I) C = Table[(C ^ static_cast<std::uint8_t>(Bytes[I])) & 0xFFu] ^ (C >> 8);
            return C ^ 0xFFFFFFFFu;
        }

        void Be32(std::string& Out, std::uint32_t V)
        {
            for (int Shift = 24; Shift >= 0; Shift -= 8) Out.push_back(static_cast<char>((V >> Shift) & 0xFFu));
        }

        void Chunk(std::string& Png, const char* Type, const std::string& Data)
        {
            Be32(Png, static_cast<std::uint32_t>(Data.size()));
            const std::size_t Start = Png.size();
            Png.append(Type, 4);
            Png += Data;
            Be32(Png, Crc32(Png, Start));
        }
    }

    std::string EncodePng(const Image& Img)
    {
        const int Bpp = Img.Channels;
        std::string Raw;
        Raw.reserve(static_cast<std::size_t>((Img.Width * Bpp + 1) * Img.Height));
        for (int Y = 0; Y < Img.Height; ++Y)
        {
            Raw.push_back(1);  // Sub filter
            const std::uint8_t* Row = &Img.Pixels[static_cast<std::size_t>(Y * Img.Width * Bpp)];
            for (int X = 0; X < Img.Width * Bpp; ++X)
            {
                const int Left = X >= Bpp ? Row[X - Bpp] : 0;
                Raw.push_back(static_cast<char>((Row[X] - Left) & 0xFF));
            }
        }
        std::string Z;
        Z.push_back(static_cast<char>(0x78));
        Z.push_back(static_cast<char>(0x01));
        Z += Deflate(Raw);
        std::uint32_t A = 1, B = 0;
        for (const char C : Raw)
        {
            A = (A + static_cast<std::uint8_t>(C)) % 65521u;
            B = (B + A) % 65521u;
        }
        Be32(Z, (B << 16) | A);

        std::string Png("\x89PNG\r\n\x1a\n", 8);
        std::string Header;
        Be32(Header, static_cast<std::uint32_t>(Img.Width));
        Be32(Header, static_cast<std::uint32_t>(Img.Height));
        Header.push_back(8);
        Header.push_back(Bpp == 4 ? 6 : 2);
        Header.append(3, '\0');
        Chunk(Png, "IHDR", Header);
        Chunk(Png, "IDAT", Z);
        Chunk(Png, "IEND", {});
        return Png;
    }

    // ------------------------------------------------------------------ mesh builder

    void Mesh::Push(const Xf& T) { Stack.push_back(Stack.back() * T); }
    void Mesh::Pop() { if (Stack.size() > 1) Stack.pop_back(); }

    Mesh::Part& Mesh::PartFor(const Material& M)
    {
        for (Part& P : PartList)
        {
            if (P.Mat.Name == M.Name) return P;
        }
        PartList.push_back({M, {}, {}});
        return PartList.back();
    }

    void Mesh::Append(const Mesh& Other)
    {
        for (const Part& Source : Other.PartList)
        {
            Part& Target = PartFor(Source.Mat);
            const auto Base = static_cast<std::uint32_t>(Target.Vertices.size());
            for (Vertex V : Source.Vertices)
            {
                V.P = Top().Point(V.P);
                V.N = Normalize(Top().Direction(V.N));
                Target.Vertices.push_back(V);
            }
            for (const std::uint32_t Index : Source.Indices) Target.Indices.push_back(Base + Index);
        }
    }

    void Mesh::Triangle(const Material& M, const Vertex& A, const Vertex& B, const Vertex& C)
    {
        Part& P = PartFor(M);
        const auto Base = static_cast<std::uint32_t>(P.Vertices.size());
        for (Vertex V : {A, B, C})
        {
            V.P = Top().Point(V.P);
            V.N = Normalize(Top().Direction(V.N));
            P.Vertices.push_back(V);
        }
        P.Indices.insert(P.Indices.end(), {Base, Base + 1, Base + 2});
    }

    void Mesh::Indexed(const Material& M, const std::vector<Vertex>& Vertices, const std::vector<std::uint32_t>& Indices)
    {
        Part& P = PartFor(M);
        const auto Base = static_cast<std::uint32_t>(P.Vertices.size());
        for (Vertex V : Vertices)
        {
            V.P = Top().Point(V.P);
            V.N = Normalize(Top().Direction(V.N));
            P.Vertices.push_back(V);
        }
        for (const std::uint32_t I : Indices) P.Indices.push_back(Base + I);
    }

    void Mesh::Quad(const Material& M, const V3& A, const V3& B, const V3& C, const V3& D, const bool TwoSided)
    {
        const V3 N = Normalize(Cross(B - A, D - A));
        const V3 AxisU = Normalize(B - A);
        const V3 AxisV = Normalize(Cross(N, AxisU));
        const double T = M.MetresPerTile;
        const auto Make = [&](const V3& P, const V3& Normal)
        {
            const V3 R = P - A;
            return Vertex{P, Normal, Dot(R, AxisU) / T, Dot(R, AxisV) / T};
        };
        Part& Pt = PartFor(M);
        const auto Emit = [&](const V3& Normal, bool Reverse)
        {
            const auto Base = static_cast<std::uint32_t>(Pt.Vertices.size());
            for (const V3& P : {A, B, C, D})
            {
                Vertex V = Make(P, Normal);
                V.P = Top().Point(V.P);
                V.N = Normalize(Top().Direction(V.N));
                Pt.Vertices.push_back(V);
            }
            if (Reverse) Pt.Indices.insert(Pt.Indices.end(), {Base, Base + 2, Base + 1, Base, Base + 3, Base + 2});
            else Pt.Indices.insert(Pt.Indices.end(), {Base, Base + 1, Base + 2, Base, Base + 2, Base + 3});
        };
        Emit(N, false);
        if (TwoSided) Emit(N * -1.0, true);
    }

    void Mesh::Card(const Material& M, const V3& A, const V3& B, const V3& C, const V3& D)
    {
        const V3 N = Normalize(Cross(B - A, D - A));
        Part& Pt = PartFor(M);
        const auto Base = static_cast<std::uint32_t>(Pt.Vertices.size());
        const double UV[4][2] = {{0, 1}, {1, 1}, {1, 0}, {0, 0}};
        int K = 0;
        for (const V3& P : {A, B, C, D})
        {
            Vertex V{Top().Point(P), Normalize(Top().Direction(N)), UV[K][0], UV[K][1]};
            Pt.Vertices.push_back(V);
            ++K;
        }
        Pt.Indices.insert(Pt.Indices.end(), {Base, Base + 1, Base + 2, Base, Base + 2, Base + 3});
    }

    void Mesh::Box(const Material& M, const V3& Min, const V3& Max)
    {
        const V3 C[8] = {{Min.X, Min.Y, Min.Z}, {Max.X, Min.Y, Min.Z}, {Max.X, Max.Y, Min.Z}, {Min.X, Max.Y, Min.Z},
            {Min.X, Min.Y, Max.Z}, {Max.X, Min.Y, Max.Z}, {Max.X, Max.Y, Max.Z}, {Min.X, Max.Y, Max.Z}};
        Quad(M, C[4], C[5], C[6], C[7]);  // +Z
        Quad(M, C[1], C[0], C[3], C[2]);  // -Z
        Quad(M, C[5], C[1], C[2], C[6]);  // +X
        Quad(M, C[0], C[4], C[7], C[3]);  // -X
        Quad(M, C[7], C[6], C[2], C[3]);  // +Y
        Quad(M, C[0], C[1], C[5], C[4]);  // -Y
    }

    void Mesh::Fan(const Material& M, const std::vector<V3>& Points, const V3& Normal)
    {
        if (Points.size() < 3) return;
        const V3 N = Normalize(Normal);
        const V3 Helper = std::fabs(N.Y) < 0.9 ? V3{0, 1, 0} : V3{1, 0, 0};
        const V3 AxisU = Normalize(Cross(Helper, N));
        const V3 AxisV = Cross(N, AxisU);
        V3 Centre;
        for (const V3& P : Points) Centre = Centre + P;
        Centre = Centre * (1.0 / static_cast<double>(Points.size()));
        const double T = M.MetresPerTile;
        const auto Make = [&](const V3& P) { return Vertex{P, N, Dot(P, AxisU) / T, Dot(P, AxisV) / T}; };
        for (std::size_t I = 0; I < Points.size(); ++I)
        {
            Triangle(M, Make(Centre), Make(Points[I]), Make(Points[(I + 1) % Points.size()]));
        }
    }

    void Mesh::Loft(const Material& M, const std::vector<std::vector<V3>>& Rings, const bool Closed, const bool FlipWinding)
    {
        if (Rings.size() < 2 || Rings.front().size() < 2) return;
        const std::size_t R = Rings.size(), C = Rings.front().size();
        const std::size_t Columns = Closed ? C + 1 : C;  // duplicate the seam for continuous UVs
        std::vector<V3> Normals(R * C);
        const auto Idx = [C](std::size_t Ring, std::size_t Col) { return Ring * C + Col % C; };
        const std::size_t Spans = Closed ? C : C - 1;
        for (std::size_t Ring = 0; Ring + 1 < R; ++Ring)
        {
            for (std::size_t Col = 0; Col < Spans; ++Col)
            {
                const V3& A = Rings[Ring][Col];
                const V3& B = Rings[Ring][(Col + 1) % C];
                const V3& D = Rings[Ring + 1][Col];
                V3 N = Cross(B - A, D - A);
                if (FlipWinding) N = N * -1.0;
                for (const std::size_t K : {Idx(Ring, Col), Idx(Ring, Col + 1), Idx(Ring + 1, Col), Idx(Ring + 1, Col + 1)}) Normals[K] = Normals[K] + N;
            }
        }
        // Along-section and along-length distances for world-scaled UVs.
        std::vector<double> U(Columns, 0.0), VAlong(R, 0.0);
        for (std::size_t Col = 1; Col < Columns; ++Col) U[Col] = U[Col - 1] + Length(Rings[R / 2][Col % C] - Rings[R / 2][Col - 1]);
        for (std::size_t Ring = 1; Ring < R; ++Ring) VAlong[Ring] = VAlong[Ring - 1] + Length(Rings[Ring][C / 2] - Rings[Ring - 1][C / 2]);
        Part& P = PartFor(M);
        const auto Base = static_cast<std::uint32_t>(P.Vertices.size());
        for (std::size_t Ring = 0; Ring < R; ++Ring)
        {
            for (std::size_t Col = 0; Col < Columns; ++Col)
            {
                Vertex V{Top().Point(Rings[Ring][Col % C]), Normalize(Top().Direction(Normalize(Normals[Idx(Ring, Col)]))),
                    U[Col] / M.MetresPerTile, VAlong[Ring] / M.MetresPerTile};
                P.Vertices.push_back(V);
            }
        }
        for (std::size_t Ring = 0; Ring + 1 < R; ++Ring)
        {
            for (std::size_t Col = 0; Col + 1 < Columns; ++Col)
            {
                const auto A = Base + static_cast<std::uint32_t>(Ring * Columns + Col);
                const auto B = A + 1;
                const auto D = Base + static_cast<std::uint32_t>((Ring + 1) * Columns + Col);
                const auto E = D + 1;
                if (FlipWinding) P.Indices.insert(P.Indices.end(), {A, D, B, B, D, E});
                else P.Indices.insert(P.Indices.end(), {A, B, D, B, E, D});
            }
        }
    }

    void Mesh::Tube(const Material& M, const V3& A, const V3& B, const double RadiusA, const double RadiusB, const int Sides, const bool Caps)
    {
        const V3 Axis = Normalize(B - A);
        const V3 Helper = std::fabs(Axis.Y) < 0.95 ? V3{0, 1, 0} : V3{1, 0, 0};
        const V3 E1 = Normalize(Cross(Helper, Axis));
        const V3 E2 = Cross(Axis, E1);
        std::vector<std::vector<V3>> Rings(2);
        for (int S = 0; S < Sides; ++S)
        {
            const double Angle = 2.0 * Pi * S / Sides;
            const V3 Dir = E1 * Cos(Angle) + E2 * Sin(Angle);
            Rings[0].push_back(A + Dir * RadiusA);
            Rings[1].push_back(B + Dir * RadiusB);
        }
        Loft(M, Rings, true, false);
        if (Caps)
        {
            std::vector<V3> Bottom(Rings[0].rbegin(), Rings[0].rend());
            Fan(M, Bottom, Axis * -1.0);
            Fan(M, Rings[1], Axis);
        }
    }

    // ------------------------------------------------------------------ glTF

    namespace
    {
        std::string Num(double V)
        {
            if (V == 0.0) return "0";
            char Buffer[32];
            const auto Result = std::to_chars(Buffer, Buffer + sizeof(Buffer), V);
            return std::string(Buffer, Result.ptr);
        }
        std::string NumF(float V)
        {
            if (V == 0.0f) return "0";
            char Buffer[32];
            const auto Result = std::to_chars(Buffer, Buffer + sizeof(Buffer), V);
            return std::string(Buffer, Result.ptr);
        }
        /** Grid-snapped float; + 0.0f turns -0 into +0 so every compiler writes the same bits. */
        float Quantize(double V, double Steps) { return static_cast<float>(std::round(V * Steps) / Steps) + 0.0f; }

        std::string Base64(const std::string& Bytes)
        {
            static constexpr char Alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
            std::string Out;
            Out.reserve((Bytes.size() + 2) / 3 * 4);
            for (std::size_t I = 0; I < Bytes.size(); I += 3)
            {
                const std::uint32_t B0 = static_cast<std::uint8_t>(Bytes[I]);
                const std::uint32_t B1 = I + 1 < Bytes.size() ? static_cast<std::uint8_t>(Bytes[I + 1]) : 0u;
                const std::uint32_t B2 = I + 2 < Bytes.size() ? static_cast<std::uint8_t>(Bytes[I + 2]) : 0u;
                const std::uint32_t W = (B0 << 16) | (B1 << 8) | B2;
                Out.push_back(Alphabet[(W >> 18) & 63u]);
                Out.push_back(Alphabet[(W >> 12) & 63u]);
                Out.push_back(I + 1 < Bytes.size() ? Alphabet[(W >> 6) & 63u] : '=');
                Out.push_back(I + 2 < Bytes.size() ? Alphabet[W & 63u] : '=');
            }
            return Out;
        }

        void PutF(std::string& Bin, float F)
        {
            std::uint32_t Bits;
            static_assert(sizeof(Bits) == sizeof(F));
            std::memcpy(&Bits, &F, sizeof(F));
            for (int Shift = 0; Shift < 32; Shift += 8) Bin.push_back(static_cast<char>((Bits >> Shift) & 0xFFu));
        }
        void PutU(std::string& Bin, std::uint32_t U)
        {
            for (int Shift = 0; Shift < 32; Shift += 8) Bin.push_back(static_cast<char>((U >> Shift) & 0xFFu));
        }

        std::string Escape(const std::string& Text)
        {
            std::string Out;
            for (const char C : Text)
            {
                if (C == '"' || C == '\\') Out.push_back('\\');
                Out.push_back(C);
            }
            return Out;
        }
    }

    std::string WriteGltf(const Mesh& Model, const std::string& Generator)
    {
        std::string Bin, BufferViews, Accessors, Primitives, Materials, Textures, Images;
        int ViewCount = 0, AccessorCount = 0, TextureCount = 0;
        std::map<std::string, int> ImageIndex;
        const auto View = [&](std::size_t Offset, std::size_t Length, int Target)
        {
            if (ViewCount) BufferViews += ",";
            BufferViews += "{\"buffer\":0,\"byteOffset\":" + std::to_string(Offset) + ",\"byteLength\":" + std::to_string(Length) +
                ",\"target\":" + std::to_string(Target) + "}";
            return ViewCount++;
        };
        const auto Accessor = [&](int ViewIndex, int ComponentType, std::size_t Count, const char* Type, const std::string& Extra)
        {
            if (AccessorCount) Accessors += ",";
            Accessors += "{\"bufferView\":" + std::to_string(ViewIndex) + ",\"componentType\":" + std::to_string(ComponentType) +
                ",\"count\":" + std::to_string(Count) + ",\"type\":\"" + Type + "\"" + Extra + "}";
            return AccessorCount++;
        };
        const auto TextureFor = [&](const std::string& File)
        {
            int Image;
            const auto Found = ImageIndex.find(File);
            if (Found == ImageIndex.end())
            {
                Image = static_cast<int>(ImageIndex.size());
                ImageIndex[File] = Image;
                if (Image) Images += ",";
                Images += "{\"uri\":\"../Textures/" + File + "\"}";
            }
            else Image = Found->second;
            if (TextureCount) Textures += ",";
            Textures += "{\"sampler\":0,\"source\":" + std::to_string(Image) + "}";
            return TextureCount++;
        };
        int MaterialIndex = 0;
        for (const Mesh::Part& P : Model.Parts())
        {
            if (P.Indices.empty()) continue;
            // Positions
            const std::size_t PosOffset = Bin.size();
            float Min[3] = {1e30f, 1e30f, 1e30f}, Max[3] = {-1e30f, -1e30f, -1e30f};
            for (const Vertex& V : P.Vertices)
            {
                // World frame (Z up, X forward) to glTF, as O3DE imports it back: g = (-x, z, y).
                const float C[3] = {Quantize(-V.P.X, 4096.0), Quantize(V.P.Z, 4096.0), Quantize(V.P.Y, 4096.0)};
                for (int K = 0; K < 3; ++K)
                {
                    PutF(Bin, C[K]);
                    Min[K] = std::min(Min[K], C[K]);
                    Max[K] = std::max(Max[K], C[K]);
                }
            }
            const int PosView = View(PosOffset, Bin.size() - PosOffset, 34962);
            const int PosAcc = Accessor(PosView, 5126, P.Vertices.size(), "VEC3",
                ",\"min\":[" + NumF(Min[0]) + "," + NumF(Min[1]) + "," + NumF(Min[2]) + "],\"max\":[" + NumF(Max[0]) + "," + NumF(Max[1]) + "," +
                    NumF(Max[2]) + "]");
            const std::size_t NormOffset = Bin.size();
            for (const Vertex& V : P.Vertices)
            {
                const V3 N = Normalize(V.N);
                PutF(Bin, Quantize(-N.X, 1024.0));
                PutF(Bin, Quantize(N.Z, 1024.0));
                PutF(Bin, Quantize(N.Y, 1024.0));
            }
            const int NormAcc = Accessor(View(NormOffset, Bin.size() - NormOffset, 34962), 5126, P.Vertices.size(), "VEC3", "");
            const std::size_t UvOffset = Bin.size();
            for (const Vertex& V : P.Vertices)
            {
                PutF(Bin, Quantize(V.U, 1024.0));
                PutF(Bin, Quantize(V.V, 1024.0));
            }
            const int UvAcc = Accessor(View(UvOffset, Bin.size() - UvOffset, 34962), 5126, P.Vertices.size(), "VEC2", "");
            const std::size_t IdxOffset = Bin.size();
            for (const std::uint32_t I : P.Indices) PutU(Bin, I);
            const int IdxAcc = Accessor(View(IdxOffset, Bin.size() - IdxOffset, 34963), 5125, P.Indices.size(), "SCALAR", "");

            const Material& M = P.Mat;
            // Colours are authored in sRGB; glTF factors are linear (decode with gamma 2.25 = c^2 * c^(1/4)).
            const auto Linear = [](double C) { return static_cast<double>(Quantize(C * C * std::sqrt(std::sqrt(C)), 1024.0)); };
            std::string Pbr = "\"baseColorFactor\":[" + Num(Linear(M.BaseColor[0])) + "," + Num(Linear(M.BaseColor[1])) + "," + Num(Linear(M.BaseColor[2])) + "," +
                Num(M.BaseColor[3]) + "],\"metallicFactor\":" + Num(M.Metallic) + ",\"roughnessFactor\":" + Num(M.Roughness);
            if (!M.BaseColorTexture.empty()) Pbr += ",\"baseColorTexture\":{\"index\":" + std::to_string(TextureFor(M.BaseColorTexture)) + "}";
            std::string Mat = "{\"name\":\"" + Escape(M.Name) + "\",\"pbrMetallicRoughness\":{" + Pbr + "}";
            if (!M.NormalTexture.empty()) Mat += ",\"normalTexture\":{\"index\":" + std::to_string(TextureFor(M.NormalTexture)) + "}";
            if (M.Mask) Mat += ",\"alphaMode\":\"MASK\",\"alphaCutoff\":0.5";
            if (M.DoubleSided) Mat += ",\"doubleSided\":true";
            Mat += "}";
            if (MaterialIndex) Materials += ",";
            Materials += Mat;
            if (!Primitives.empty()) Primitives += ",";
            Primitives += "{\"attributes\":{\"POSITION\":" + std::to_string(PosAcc) + ",\"NORMAL\":" + std::to_string(NormAcc) +
                ",\"TEXCOORD_0\":" + std::to_string(UvAcc) + "},\"indices\":" + std::to_string(IdxAcc) + ",\"material\":" +
                std::to_string(MaterialIndex) + "}";
            ++MaterialIndex;
            while (Bin.size() % 4) Bin.push_back('\0');
        }
        std::string Json = "{\"asset\":{\"version\":\"2.0\",\"generator\":\"" + Escape(Generator) + "\"},\"scene\":0,\"scenes\":[{\"nodes\":[0]}]," +
            "\"nodes\":[{\"mesh\":0,\"name\":\"" + Escape(Model.Name()) + "\"}],\"meshes\":[{\"name\":\"" + Escape(Model.Name()) +
            "\",\"primitives\":[" + Primitives + "]}],\"materials\":[" + Materials + "]";
        if (TextureCount)
        {
            Json += ",\"samplers\":[{\"magFilter\":9729,\"minFilter\":9987,\"wrapS\":10497,\"wrapT\":10497}],\"textures\":[" + Textures +
                "],\"images\":[" + Images + "]";
        }
        Json += ",\"accessors\":[" + Accessors + "],\"bufferViews\":[" + BufferViews + "],\"buffers\":[{\"byteLength\":" +
            std::to_string(Bin.size()) + ",\"uri\":\"data:application/octet-stream;base64," + Base64(Bin) + "\"}]}\n";
        return Json;
    }

    std::string WriteCollisionGltf(const Mesh& Model)
    {
        Mesh Flat(Model.Name());
        Material Collision;
        Collision.Name = "Collision";
        for (const Mesh::Part& P : Model.Parts())
        {
            for (std::size_t I = 0; I + 2 < P.Indices.size(); I += 3)
            {
                Flat.Triangle(Collision, P.Vertices[P.Indices[I]], P.Vertices[P.Indices[I + 1]], P.Vertices[P.Indices[I + 2]]);
            }
        }
        return WriteGltf(Flat, "Dark Arisen art kit collision");
    }
}
