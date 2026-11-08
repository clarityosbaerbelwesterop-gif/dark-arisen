// Game-ready versions of heavy scanned models: see ArtKitDecimate.h.

#include "ArtKitDecimate.h"

#include "ArtKitCore.h"
#include "Json.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iterator>
#include <map>
#include <numeric>
#include <queue>
#include <vector>

namespace DarkArisen::Tools::Art
{
    namespace
    {
        using Matrix = std::array<double, 16>;  // column-major, as glTF writes it

        Matrix Identity() { return {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}; }

        Matrix Multiply(const Matrix& A, const Matrix& B)
        {
            Matrix C{};
            for (int Col = 0; Col < 4; ++Col)
                for (int Row = 0; Row < 4; ++Row)
                {
                    double Sum = 0.0;
                    for (int K = 0; K < 4; ++K) Sum += A[static_cast<std::size_t>(K * 4 + Row)] * B[static_cast<std::size_t>(Col * 4 + K)];
                    C[static_cast<std::size_t>(Col * 4 + Row)] = Sum;
                }
            return C;
        }

        double Number(const JsonValue* V, const double Fallback) { return V && V->IsNumber() ? V->Number : Fallback; }

        Matrix NodeMatrix(const JsonValue& Node)
        {
            if (const JsonValue* M = Node.Find("matrix"); M && M->IsArray() && M->Items.size() == 16)
            {
                Matrix Out{};
                for (std::size_t K = 0; K < 16; ++K) Out[K] = M->Items[K].Number;
                return Out;
            }
            std::array<double, 3> T{0, 0, 0}, S{1, 1, 1};
            std::array<double, 4> Q{0, 0, 0, 1};
            if (const JsonValue* V = Node.Find("translation"); V && V->Items.size() == 3)
                for (std::size_t K = 0; K < 3; ++K) T[K] = V->Items[K].Number;
            if (const JsonValue* V = Node.Find("scale"); V && V->Items.size() == 3)
                for (std::size_t K = 0; K < 3; ++K) S[K] = V->Items[K].Number;
            if (const JsonValue* V = Node.Find("rotation"); V && V->Items.size() == 4)
                for (std::size_t K = 0; K < 4; ++K) Q[K] = V->Items[K].Number;
            const double X = Q[0], Y = Q[1], Z = Q[2], W = Q[3];
            const double R[9] = {1 - 2 * (Y * Y + Z * Z), 2 * (X * Y + Z * W), 2 * (X * Z - Y * W),
                                 2 * (X * Y - Z * W), 1 - 2 * (X * X + Z * Z), 2 * (Y * Z + X * W),
                                 2 * (X * Z + Y * W), 2 * (Y * Z - X * W), 1 - 2 * (X * X + Y * Y)};  // columns
            return {R[0] * S[0], R[1] * S[0], R[2] * S[0], 0, R[3] * S[1], R[4] * S[1], R[5] * S[1], 0,
                    R[6] * S[2], R[7] * S[2], R[8] * S[2], 0, T[0], T[1], T[2], 1};
        }

        struct Source
        {
            JsonValue Doc;
            std::vector<std::string> Buffers;
        };

        /** Reads an accessor as doubles (Components per element); integers widen, normalised ones scale to 0..1. */
        bool ReadAccessor(const Source& Src, const std::size_t Index, const int Components, std::vector<double>& Out, std::string& Problem)
        {
            const JsonValue* Accessors = Src.Doc.Find("accessors");
            if (!Accessors || Index >= Accessors->Items.size()) return (Problem = "accessor out of range", false);
            const JsonValue& A = Accessors->Items[Index];
            const std::size_t Count = static_cast<std::size_t>(Number(A.Find("count"), 0));
            const int Type = static_cast<int>(Number(A.Find("componentType"), 0));
            const bool Normalized = A.Find("normalized") && A.Find("normalized")->Boolean;
            const JsonValue* ViewIndex = A.Find("bufferView");
            if (!ViewIndex) return (Problem = "sparse or empty accessor", false);
            const JsonValue& View = Src.Doc.Find("bufferViews")->Items[static_cast<std::size_t>(ViewIndex->Number)];
            const std::string& Bytes = Src.Buffers[static_cast<std::size_t>(Number(View.Find("buffer"), 0))];
            const std::size_t Size = Type == 5126 || Type == 5125 ? 4 : Type == 5123 || Type == 5122 ? 2 : 1;
            const std::size_t Stride = View.Find("byteStride") ? static_cast<std::size_t>(View.Find("byteStride")->Number) : Size * static_cast<std::size_t>(Components);
            const std::size_t Start = static_cast<std::size_t>(Number(View.Find("byteOffset"), 0) + Number(A.Find("byteOffset"), 0));
            if (Count && Start + Stride * (Count - 1) + Size * static_cast<std::size_t>(Components) > Bytes.size()) return (Problem = "accessor past its buffer", false);
            Out.resize(Count * static_cast<std::size_t>(Components));
            for (std::size_t E = 0; E < Count; ++E)
                for (int C = 0; C < Components; ++C)
                {
                    const char* P = Bytes.data() + Start + E * Stride + static_cast<std::size_t>(C) * Size;
                    double V = 0.0;
                    if (Type == 5126) { float F; std::memcpy(&F, P, 4); V = F; }
                    else if (Type == 5125) { std::uint32_t U; std::memcpy(&U, P, 4); V = U; }
                    else if (Type == 5123) { std::uint16_t U; std::memcpy(&U, P, 2); V = Normalized ? U / 65535.0 : U; }
                    else if (Type == 5121) { const auto U = static_cast<std::uint8_t>(*P); V = Normalized ? U / 255.0 : U; }
                    else return (Problem = "unsupported component type " + std::to_string(Type), false);
                    Out[E * static_cast<std::size_t>(Components) + static_cast<std::size_t>(C)] = V;
                }
            return true;
        }

        std::string ImageOf(const JsonValue& Doc, const JsonValue* TextureRef, const std::string& Prefix)
        {
            if (!TextureRef || !TextureRef->Find("index")) return {};
            const JsonValue& Texture = Doc.Find("textures")->Items[static_cast<std::size_t>(TextureRef->Find("index")->Number)];
            const JsonValue& Image = Doc.Find("images")->Items[static_cast<std::size_t>(Number(Texture.Find("source"), 0))];
            return Image.Find("uri") ? Prefix + Image.Find("uri")->Text : std::string();
        }

        /**
         * Half-edge collapse by quadric error (Garland & Heckbert 1997) on position-welded topology: UV seams do
         * not tear, open borders are held by perpendicular planes, collapses that flip a triangle are refused, and
         * each corner keeps the surviving vertex's attribute wedge closest to its own UV. Attr: 8 floats per
         * vertex (position, normal, uv). Returns triangle corners as indices into the original vertices.
         */
        std::vector<std::uint32_t> Simplify(const std::vector<float>& Attr, const std::vector<std::uint32_t>& Indices, const std::size_t TargetTriangles)
        {
            const std::size_t N = Attr.size() / 8;
            // weld by exact position
            std::vector<std::uint32_t> Order(N);
            std::iota(Order.begin(), Order.end(), 0u);
            const auto Pos = [&Attr](std::uint32_t V, int K) { return Attr[static_cast<std::size_t>(V) * 8 + static_cast<std::size_t>(K)]; };
            std::sort(Order.begin(), Order.end(), [&](std::uint32_t A, std::uint32_t B)
                { return std::make_tuple(Pos(A, 0), Pos(A, 1), Pos(A, 2), A) < std::make_tuple(Pos(B, 0), Pos(B, 1), Pos(B, 2), B); });
            std::vector<std::uint32_t> Weld(N);
            std::vector<std::array<double, 3>> P;
            std::vector<std::vector<std::uint32_t>> Wedges;
            for (std::size_t K = 0; K < N; ++K)
            {
                const std::uint32_t V = Order[K];
                if (K == 0 || Pos(V, 0) != Pos(Order[K - 1], 0) || Pos(V, 1) != Pos(Order[K - 1], 1) || Pos(V, 2) != Pos(Order[K - 1], 2))
                {
                    P.push_back({Pos(V, 0), Pos(V, 1), Pos(V, 2)});
                    Wedges.emplace_back();
                }
                Weld[V] = static_cast<std::uint32_t>(P.size() - 1);
                Wedges.back().push_back(V);
            }
            const std::size_t W = P.size();

            std::vector<std::array<std::uint32_t, 3>> Tris;  // corners as wedges (original vertices)
            for (std::size_t T = 0; T + 2 < Indices.size(); T += 3)
            {
                const std::array<std::uint32_t, 3> C{Indices[T], Indices[T + 1], Indices[T + 2]};
                if (Weld[C[0]] != Weld[C[1]] && Weld[C[1]] != Weld[C[2]] && Weld[C[0]] != Weld[C[2]]) Tris.push_back(C);
            }
            std::vector<char> TriAlive(Tris.size(), 1);
            std::vector<std::vector<std::uint32_t>> Around(W);
            for (std::uint32_t T = 0; T < Tris.size(); ++T)
                for (const std::uint32_t C : Tris[T]) Around[Weld[C]].push_back(T);

            using Quadric = std::array<double, 10>;
            std::vector<Quadric> Q(W, Quadric{});
            const auto AddPlane = [](Quadric& Qd, double A, double B, double C, double D, double Wt)
            {
                const double V[10] = {A * A, A * B, A * C, A * D, B * B, B * C, B * D, C * C, C * D, D * D};
                for (int K = 0; K < 10; ++K) Qd[static_cast<std::size_t>(K)] += Wt * V[K];
            };
            const auto Normal = [&P](std::uint32_t A, std::uint32_t B, std::uint32_t C)
            {
                const auto& Pa = P[A]; const auto& Pb = P[B]; const auto& Pc = P[C];
                const double U[3] = {Pb[0] - Pa[0], Pb[1] - Pa[1], Pb[2] - Pa[2]}, V[3] = {Pc[0] - Pa[0], Pc[1] - Pa[1], Pc[2] - Pa[2]};
                return std::array<double, 3>{U[1] * V[2] - U[2] * V[1], U[2] * V[0] - U[0] * V[2], U[0] * V[1] - U[1] * V[0]};
            };
            std::map<std::pair<std::uint32_t, std::uint32_t>, int> EdgeUse;
            for (const auto& T : Tris)
            {
                const std::uint32_t A = Weld[T[0]], B = Weld[T[1]], C = Weld[T[2]];
                const auto Nn = Normal(A, B, C);
                const double L = std::sqrt(Nn[0] * Nn[0] + Nn[1] * Nn[1] + Nn[2] * Nn[2]);
                if (L <= 0.0) continue;
                const double Nx = Nn[0] / L, Ny = Nn[1] / L, Nz = Nn[2] / L, D = -(Nx * P[A][0] + Ny * P[A][1] + Nz * P[A][2]);
                for (const std::uint32_t V : {A, B, C}) AddPlane(Q[V], Nx, Ny, Nz, D, L * 0.5);
                for (const auto& E : {std::make_pair(A, B), std::make_pair(B, C), std::make_pair(C, A)})
                    ++EdgeUse[{std::min(E.first, E.second), std::max(E.first, E.second)}];
            }
            // open borders (leaf and blade outlines, scan edges) keep their shape
            for (const auto& T : Tris)
            {
                const std::uint32_t A = Weld[T[0]], B = Weld[T[1]], C = Weld[T[2]];
                const auto Nn = Normal(A, B, C);
                for (const auto& E : {std::make_pair(A, B), std::make_pair(B, C), std::make_pair(C, A)})
                {
                    if (EdgeUse[{std::min(E.first, E.second), std::max(E.first, E.second)}] != 1) continue;
                    const double Ex = P[E.second][0] - P[E.first][0], Ey = P[E.second][1] - P[E.first][1], Ez = P[E.second][2] - P[E.first][2];
                    double Bx = Ey * Nn[2] - Ez * Nn[1], By = Ez * Nn[0] - Ex * Nn[2], Bz = Ex * Nn[1] - Ey * Nn[0];
                    const double L = std::sqrt(Bx * Bx + By * By + Bz * Bz);
                    if (L <= 0.0) continue;
                    Bx /= L; By /= L; Bz /= L;
                    const double D = -(Bx * P[E.first][0] + By * P[E.first][1] + Bz * P[E.first][2]);
                    const double Len2 = Ex * Ex + Ey * Ey + Ez * Ez;
                    AddPlane(Q[E.first], Bx, By, Bz, D, 20.0 * Len2);
                    AddPlane(Q[E.second], Bx, By, Bz, D, 20.0 * Len2);
                }
            }
            const auto Cost = [&](std::uint32_t U, std::uint32_t V)
            {
                Quadric S = Q[U];
                for (std::size_t K = 0; K < 10; ++K) S[K] += Q[V][K];
                const double X = P[V][0], Y = P[V][1], Z = P[V][2];
                return S[0] * X * X + 2 * S[1] * X * Y + 2 * S[2] * X * Z + 2 * S[3] * X + S[4] * Y * Y + 2 * S[5] * Y * Z + 2 * S[6] * Y + S[7] * Z * Z +
                       2 * S[8] * Z + S[9];
            };
            struct Candidate
            {
                double Cost;
                std::uint32_t U, V, StampU, StampV;
                bool operator<(const Candidate& O) const { return Cost > O.Cost; }  // min-heap
            };
            std::vector<std::uint32_t> Stamp(W, 0);
            std::vector<char> Alive(W, 1);
            std::priority_queue<Candidate> Heap;
            const auto Push = [&](std::uint32_t A, std::uint32_t B)
            {
                const double Ab = Cost(A, B), Ba = Cost(B, A);
                if (Ab <= Ba) Heap.push({Ab, A, B, Stamp[A], Stamp[B]});
                else Heap.push({Ba, B, A, Stamp[B], Stamp[A]});
            };
            for (const auto& [E, Uses] : EdgeUse) Push(E.first, E.second);

            std::size_t Live = Tris.size();
            const auto Uv = [&Attr](std::uint32_t V) { return std::array<float, 2>{Attr[static_cast<std::size_t>(V) * 8 + 6], Attr[static_cast<std::size_t>(V) * 8 + 7]}; };
            while (Live > TargetTriangles && !Heap.empty())
            {
                const Candidate Cd = Heap.top();
                Heap.pop();
                const std::uint32_t U = Cd.U, V = Cd.V;
                if (!Alive[U] || !Alive[V] || Stamp[U] != Cd.StampU || Stamp[V] != Cd.StampV) continue;
                // refuse collapses that flip or crush a neighbouring triangle
                bool Ok = true;
                for (const std::uint32_t T : Around[U])
                {
                    if (!TriAlive[T]) continue;
                    std::array<std::uint32_t, 3> Wv{Weld[Tris[T][0]], Weld[Tris[T][1]], Weld[Tris[T][2]]};
                    if (Wv[0] == V || Wv[1] == V || Wv[2] == V) continue;
                    const auto Before = Normal(Wv[0], Wv[1], Wv[2]);
                    for (auto& X : Wv) if (X == U) X = V;
                    const auto After = Normal(Wv[0], Wv[1], Wv[2]);
                    const double Dot = Before[0] * After[0] + Before[1] * After[1] + Before[2] * After[2];
                    const double Lb = std::sqrt(Before[0] * Before[0] + Before[1] * Before[1] + Before[2] * Before[2]);
                    const double La = std::sqrt(After[0] * After[0] + After[1] * After[1] + After[2] * After[2]);
                    if (La <= 1e-12 || Dot < 0.3 * Lb * La) { Ok = false; break; }
                }
                if (!Ok) continue;
                for (const std::uint32_t T : Around[U])
                {
                    if (!TriAlive[T]) continue;
                    auto& C = Tris[T];
                    if (Weld[C[0]] == V || Weld[C[1]] == V || Weld[C[2]] == V)
                    {
                        TriAlive[T] = 0;
                        --Live;
                        continue;
                    }
                    for (auto& Corner : C)
                    {
                        if (Weld[Corner] != U) continue;
                        const auto Want = Uv(Corner);
                        std::uint32_t Best = Wedges[V].front();
                        float BestD = 1e30f;
                        for (const std::uint32_t Cand : Wedges[V])
                        {
                            const auto Have = Uv(Cand);
                            const float D = (Have[0] - Want[0]) * (Have[0] - Want[0]) + (Have[1] - Want[1]) * (Have[1] - Want[1]);
                            if (D < BestD) { BestD = D; Best = Cand; }
                        }
                        Corner = Best;
                    }
                    Around[V].push_back(T);
                }
                Alive[U] = 0;
                for (std::size_t K = 0; K < 10; ++K) Q[V][K] += Q[U][K];
                ++Stamp[V];
                std::vector<std::uint32_t> Next;
                for (const std::uint32_t T : Around[V])
                    if (TriAlive[T])
                        for (const std::uint32_t C : Tris[T]) if (Weld[C] != V) Next.push_back(Weld[C]);
                std::sort(Next.begin(), Next.end());
                Next.erase(std::unique(Next.begin(), Next.end()), Next.end());
                for (const std::uint32_t X : Next) Push(V, X);
            }
            std::vector<std::uint32_t> Out;
            for (std::size_t T = 0; T < Tris.size(); ++T)
                if (TriAlive[T]) Out.insert(Out.end(), Tris[T].begin(), Tris[T].end());
            return Out;
        }

        struct Group
        {
            Material Mat;
            std::vector<float> Attributes;  // position xyz, normal xyz, uv: 8 floats per vertex (glTF frame)
            std::vector<std::uint32_t> Indices;
        };
    }

    bool DecimateGltf(const std::filesystem::path& Path, const std::size_t TargetTriangles, const std::string& ImagePrefix, const std::string& Name,
        std::vector<std::string>& OutGltfs, DecimateStats& Stats, std::string& Problem)
    {
        const auto ReadFile = [](const std::filesystem::path& File, std::string& Out)
        {
            std::ifstream In(File, std::ios::binary);
            if (!In) return false;
            Out.assign(std::istreambuf_iterator<char>(In), std::istreambuf_iterator<char>());
            return true;
        };
        std::string Text;
        Source Src;
        if (!ReadFile(Path, Text) || !JsonReader::Parse(Text, Src.Doc, Problem)) return (Problem = Path.string() + ": " + (Problem.empty() ? "unreadable" : Problem), false);
        if (const JsonValue* Buffers = Src.Doc.Find("buffers"))
            for (const JsonValue& B : Buffers->Items)
            {
                std::string Bytes;
                if (!B.Find("uri") || !ReadFile(Path.parent_path() / B.Find("uri")->Text, Bytes)) return (Problem = Path.string() + ": missing buffer", false);
                Src.Buffers.push_back(std::move(Bytes));
            }

        // materials
        std::vector<Group> Groups;
        if (const JsonValue* Materials = Src.Doc.Find("materials"))
            for (const JsonValue& M : Materials->Items)
            {
                Group G;
                G.Mat.Name = M.Find("name") ? M.Find("name")->Text : "M_Scanned";
                const JsonValue* Pbr = M.Find("pbrMetallicRoughness");
                if (Pbr)
                {
                    G.Mat.BaseColorTexture = ImageOf(Src.Doc, Pbr->Find("baseColorTexture"), ImagePrefix);
                    G.Mat.OrmTexture = ImageOf(Src.Doc, Pbr->Find("metallicRoughnessTexture"), ImagePrefix);
                    G.Mat.Roughness = Number(Pbr->Find("roughnessFactor"), 1.0);
                    G.Mat.Metallic = Number(Pbr->Find("metallicFactor"), 1.0);
                    if (const JsonValue* F = Pbr->Find("baseColorFactor"); F && F->Items.size() == 4)
                        for (std::size_t K = 0; K < 4; ++K) G.Mat.BaseColor[K] = F->Items[K].Number;
                }
                G.Mat.NormalTexture = ImageOf(Src.Doc, M.Find("normalTexture"), ImagePrefix);
                G.Mat.Mask = M.Find("alphaMode") && M.Find("alphaMode")->Text == "MASK";
                G.Mat.AlphaCutoff = Number(M.Find("alphaCutoff"), 0.5);
                G.Mat.DoubleSided = M.Find("doubleSided") && M.Find("doubleSided")->Boolean;
                Groups.push_back(std::move(G));
            }
        if (Groups.empty()) Groups.push_back(Group{});
        std::vector<std::vector<Group>> Variants;

        // flatten the node tree into the groups
        const JsonValue* Nodes = Src.Doc.Find("nodes");
        const JsonValue* Meshes = Src.Doc.Find("meshes");
        if (!Nodes || !Meshes) return (Problem = Path.string() + ": no nodes or meshes", false);
        std::vector<std::pair<std::size_t, Matrix>> Stack;
        const JsonValue* Scenes = Src.Doc.Find("scenes");
        const std::size_t SceneIndex = static_cast<std::size_t>(Number(Src.Doc.Find("scene"), 0));
        if (Scenes && SceneIndex < Scenes->Items.size() && Scenes->Items[SceneIndex].Find("nodes"))
            for (const JsonValue& N : Scenes->Items[SceneIndex].Find("nodes")->Items) Stack.push_back({static_cast<std::size_t>(N.Number), Identity()});
        while (!Stack.empty())
        {
            const auto [NodeIndex, Parent] = Stack.back();
            Stack.pop_back();
            const JsonValue& Node = Nodes->Items[NodeIndex];
            const Matrix World = Multiply(Parent, NodeMatrix(Node));
            if (const JsonValue* Children = Node.Find("children"))
                for (const JsonValue& C : Children->Items) Stack.push_back({static_cast<std::size_t>(C.Number), World});
            const JsonValue* MeshIndex = Node.Find("mesh");
            if (!MeshIndex) continue;
            // every mesh node is a variant of its own (scans ship several sizes side by side), centred on its origin
            Variants.push_back(Groups);
            const double Origin[3] = {World[12], World[13], World[14]};
            for (const JsonValue& Prim : Meshes->Items[static_cast<std::size_t>(MeshIndex->Number)].Find("primitives")->Items)
            {
                if (Number(Prim.Find("mode"), 4) != 4) continue;  // triangles only
                const JsonValue* Attr = Prim.Find("attributes");
                std::vector<double> Pos, Nrm, Uv, Idx;
                if (!Attr->Find("POSITION") || !ReadAccessor(Src, static_cast<std::size_t>(Attr->Find("POSITION")->Number), 3, Pos, Problem)) return false;
                const std::size_t Count = Pos.size() / 3;
                if (Attr->Find("NORMAL") && !ReadAccessor(Src, static_cast<std::size_t>(Attr->Find("NORMAL")->Number), 3, Nrm, Problem)) return false;
                if (Attr->Find("TEXCOORD_0") && !ReadAccessor(Src, static_cast<std::size_t>(Attr->Find("TEXCOORD_0")->Number), 2, Uv, Problem)) return false;
                if (Prim.Find("indices"))
                {
                    if (!ReadAccessor(Src, static_cast<std::size_t>(Prim.Find("indices")->Number), 1, Idx, Problem)) return false;
                }
                else
                    for (std::size_t K = 0; K < Count; ++K) Idx.push_back(static_cast<double>(K));
                Group& G = Variants.back()[std::min(Groups.size() - 1, static_cast<std::size_t>(Number(Prim.Find("material"), 0)))];
                const auto Base = static_cast<std::uint32_t>(G.Attributes.size() / 8);
                for (std::size_t V = 0; V < Count; ++V)
                {
                    const double* P = &Pos[V * 3];
                    double Out[3], N[3] = {0, 1, 0};
                    for (int R = 0; R < 3; ++R)
                        Out[R] = World[static_cast<std::size_t>(R)] * P[0] + World[static_cast<std::size_t>(4 + R)] * P[1] + World[static_cast<std::size_t>(8 + R)] * P[2] +
                                 World[static_cast<std::size_t>(12 + R)] - Origin[R];
                    if (!Nrm.empty())
                    {
                        // rotation and uniform scale only in practice; renormalised
                        for (int R = 0; R < 3; ++R)
                            N[R] = World[static_cast<std::size_t>(R)] * Nrm[V * 3] + World[static_cast<std::size_t>(4 + R)] * Nrm[V * 3 + 1] +
                                   World[static_cast<std::size_t>(8 + R)] * Nrm[V * 3 + 2];
                        const double L = std::sqrt(N[0] * N[0] + N[1] * N[1] + N[2] * N[2]);
                        for (double& C : N) C = L > 0 ? C / L : C;
                    }
                    for (double C : Out) G.Attributes.push_back(static_cast<float>(C));
                    for (double C : N) G.Attributes.push_back(static_cast<float>(C));
                    G.Attributes.push_back(Uv.empty() ? 0.0f : static_cast<float>(Uv[V * 2]));
                    G.Attributes.push_back(Uv.empty() ? 0.0f : static_cast<float>(Uv[V * 2 + 1]));
                }
                for (double I : Idx) G.Indices.push_back(Base + static_cast<std::uint32_t>(I));
            }
        }

        std::size_t Total = 0;
        for (const auto& Variant : Variants)
            for (const Group& G : Variant) Total += G.Indices.size() / 3;
        Stats.SourceTriangles = Total;
        if (Total == 0) return (Problem = Path.string() + ": no triangles", false);

        Stats.Triangles = 0;
        OutGltfs.clear();
        for (auto& Variant : Variants)
        {
            Mesh Out(Name + "_" + std::to_string(OutGltfs.size()));
            for (Group& G : Variant)
            {
                if (G.Indices.empty()) continue;
                const double Share = static_cast<double>(G.Indices.size() / 3) / static_cast<double>(Total);
                const std::size_t Target = std::max<std::size_t>(12, static_cast<std::size_t>(static_cast<double>(TargetTriangles) * Share)) * 3;
                const std::vector<std::uint32_t> Simplified = Target < G.Indices.size() ? Simplify(G.Attributes, G.Indices, Target / 3) : G.Indices;

                // compact the vertices the kept triangles use, in the kit's frame (glTF (x, y, z) -> kit (-x, z, y))
                std::map<std::uint32_t, std::uint32_t> Remap;
                std::vector<Vertex> Verts;
                std::vector<std::uint32_t> Indices;
                Indices.reserve(Simplified.size());
                for (const std::uint32_t I : Simplified)
                {
                    auto [It, Inserted] = Remap.try_emplace(I, static_cast<std::uint32_t>(Verts.size()));
                    if (Inserted)
                    {
                        const float* A = &G.Attributes[static_cast<std::size_t>(I) * 8];
                        Verts.push_back(Vertex{V3{-A[0], A[2], A[1]}, V3{-A[3], A[5], A[4]}, A[6], A[7]});
                    }
                    Indices.push_back(It->second);
                }
                Out.Indexed(G.Mat, Verts, Indices);
                Stats.Triangles += Indices.size() / 3;
            }
            if (!Out.Parts().empty()) OutGltfs.push_back(WriteGltf(Out, "Dark Arisen decimate-art (quadric edge collapse)"));
        }
        return true;
    }
}
