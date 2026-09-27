// Human pipeline, part 3: skinned actor and motion clips as glTF for EMotionFX. The MakeHuman
// skeleton keeps identity rest rotations (bind pose = translations only). Positions, directions
// and rotations are written in the frame the project's glTF import maps back to the O3DE world:
// MakeHuman (x, y, z) in decimetres -> glTF (-z, y, x) in metres.

#include "ArtKitHumanInternal.h"

#include <algorithm>
#include <cmath>
#include <map>

namespace DarkArisen::Tools::Art::Humans
{
    namespace
    {
        using namespace GltfBits;

        std::array<float, 3> G(const V3& P) { return {Quantize(-P.Z * 0.1, 4096.0), Quantize(P.Y * 0.1, 4096.0), Quantize(P.X * 0.1, 4096.0)}; }
        std::array<float, 3> GDir(const V3& N)
        {
            const V3 U = Normalize(N);
            return {Quantize(-U.Z, 1024.0), Quantize(U.Y, 1024.0), Quantize(U.X, 1024.0)};
        }
        /** glTF quaternion (x, y, z, w) of a MakeHuman-frame rotation. */
        std::array<float, 4> GQuat(const Quat& Q)
        {
            const Quat U = NormalizeQ(Q);
            return {Quantize(-U.Z, 1048576.0), Quantize(U.Y, 1048576.0), Quantize(U.X, 1048576.0), Quantize(U.W, 1048576.0)};
        }

        struct Writer
        {
            std::string Bin, Views, Accessors;
            int ViewCount = 0, AccessorCount = 0;
            int View(const std::size_t Offset, const int Target)
            {
                if (ViewCount) Views += ",";
                Views += "{\"buffer\":0,\"byteOffset\":" + std::to_string(Offset) + ",\"byteLength\":" + std::to_string(Bin.size() - Offset);
                if (Target) Views += ",\"target\":" + std::to_string(Target);
                Views += "}";
                while (Bin.size() % 4) Bin.push_back('\0');
                return ViewCount++;
            }
            int Accessor(const int ViewIndex, const int ComponentType, const std::size_t Count, const char* Type, const std::string& Extra)
            {
                if (AccessorCount) Accessors += ",";
                Accessors += "{\"bufferView\":" + std::to_string(ViewIndex) + ",\"componentType\":" + std::to_string(ComponentType) +
                    ",\"count\":" + std::to_string(Count) + ",\"type\":\"" + Type + "\"" + Extra + "}";
                return AccessorCount++;
            }
            std::string Tail() const
            {
                return ",\"accessors\":[" + Accessors + "],\"bufferViews\":[" + Views + "],\"buffers\":[{\"byteLength\":" + std::to_string(Bin.size()) +
                    ",\"uri\":\"data:application/octet-stream;base64," + Base64(Bin) + "\"}]}\n";
            }
        };

        /** Joint nodes (index 1 + bone, or 0 + bone when Offset is 0), bind translations relative to the parent. */
        std::string JointNodes(const Source& Data, const Body& B)
        {
            std::string Out;
            for (std::size_t I = 0; I < Data.Bones.size(); ++I)
            {
                const Bone& Bn = Data.Bones[I];
                const V3 Local = Bn.Parent < 0 ? B.Head[I] : B.Head[I] - B.Head[static_cast<std::size_t>(Bn.Parent)];
                const auto T = G(Local);
                std::string Children;
                for (std::size_t C = 0; C < Data.Bones.size(); ++C)
                {
                    if (Data.Bones[C].Parent == static_cast<int>(I)) Children += (Children.empty() ? "" : ",") + std::string("@") + std::to_string(C);
                }
                if (I) Out += ",";
                Out += "{\"name\":\"" + Escape(Bn.Name) + "\",\"translation\":[" + NumF(T[0]) + "," + NumF(T[1]) + "," + NumF(T[2]) + "]";
                if (!Children.empty()) Out += ",\"children\":[" + Children + "]";
                Out += "}";
            }
            return Out;
        }
        /** Replaces the @bone placeholders of JointNodes with node indices. */
        std::string Resolve(const std::string& Text, const int Offset)
        {
            std::string Out;
            for (std::size_t I = 0; I < Text.size(); ++I)
            {
                if (Text[I] != '@')
                {
                    Out.push_back(Text[I]);
                    continue;
                }
                std::size_t J = I + 1;
                int Value = 0;
                while (J < Text.size() && Text[J] >= '0' && Text[J] <= '9') Value = Value * 10 + (Text[J++] - '0');
                Out += std::to_string(Value + Offset);
                I = J - 1;
            }
            return Out;
        }
        int RootBone(const Source& Data)
        {
            for (std::size_t I = 0; I < Data.Bones.size(); ++I)
            {
                if (Data.Bones[I].Parent < 0) return static_cast<int>(I);
            }
            return 0;
        }
    }

    std::string WriteActorGltf(const Source& Data, const Body& B, const std::string& Name, const std::vector<FigurePart>& Parts)
    {
        Writer W;
        std::string Primitives, Materials, Textures, Images;
        std::map<std::string, int> ImageIndex;
        int TextureCount = 0, MaterialCount = 0;
        const std::function<int(const std::string&)> TextureFor = [&](const std::string& File)
        {
            int Image;
            if (const auto Found = ImageIndex.find(File); Found != ImageIndex.end()) Image = Found->second;
            else
            {
                Image = static_cast<int>(ImageIndex.size());
                ImageIndex[File] = Image;
                Images += (Image ? "," : "") + std::string("{\"uri\":\"../Textures/") + File + "\"}";
            }
            Textures += (TextureCount ? "," : "") + std::string("{\"sampler\":0,\"source\":") + std::to_string(Image) + "}";
            return TextureCount++;
        };
        for (const FigurePart& P : Parts)
        {
            if (P.Indices.empty()) continue;
            std::size_t Offset = W.Bin.size();
            float Min[3] = {1e30f, 1e30f, 1e30f}, Max[3] = {-1e30f, -1e30f, -1e30f};
            for (const SkinVertex& V : P.Vertices)
            {
                const auto C = G(V.P);
                for (int K = 0; K < 3; ++K)
                {
                    PutF(W.Bin, C[static_cast<std::size_t>(K)]);
                    Min[K] = std::min(Min[K], C[static_cast<std::size_t>(K)]);
                    Max[K] = std::max(Max[K], C[static_cast<std::size_t>(K)]);
                }
            }
            const int Pos = W.Accessor(W.View(Offset, 34962), 5126, P.Vertices.size(), "VEC3",
                ",\"min\":[" + NumF(Min[0]) + "," + NumF(Min[1]) + "," + NumF(Min[2]) + "],\"max\":[" + NumF(Max[0]) + "," + NumF(Max[1]) + "," + NumF(Max[2]) + "]");
            Offset = W.Bin.size();
            for (const SkinVertex& V : P.Vertices)
            {
                for (const float F : GDir(V.N)) PutF(W.Bin, F);
            }
            const int Nrm = W.Accessor(W.View(Offset, 34962), 5126, P.Vertices.size(), "VEC3", "");
            Offset = W.Bin.size();
            for (const SkinVertex& V : P.Vertices)
            {
                PutF(W.Bin, Quantize(V.U, 1024.0));
                PutF(W.Bin, Quantize(V.V, 1024.0));
            }
            const int Uv = W.Accessor(W.View(Offset, 34962), 5126, P.Vertices.size(), "VEC2", "");
            Offset = W.Bin.size();
            for (const SkinVertex& V : P.Vertices)
            {
                for (const int J : V.Joint) PutU16(W.Bin, static_cast<std::uint16_t>(J));
            }
            const int Joints = W.Accessor(W.View(Offset, 34962), 5123, P.Vertices.size(), "VEC4", "");
            Offset = W.Bin.size();
            for (const SkinVertex& V : P.Vertices)
            {
                // Weights on a 1/4096 grid; the largest takes the remainder so the four sum to exactly 1.
                std::array<float, 4> Q{};
                float Sum = 0.0f;
                for (std::size_t K = 1; K < 4; ++K)
                {
                    Q[K] = Quantize(V.Weight[K], 4096.0);
                    Sum += Q[K];
                }
                Q[0] = 1.0f - Sum;
                for (const float F : Q) PutF(W.Bin, F);
            }
            const int Weights = W.Accessor(W.View(Offset, 34962), 5126, P.Vertices.size(), "VEC4", "");
            Offset = W.Bin.size();
            for (const std::uint32_t I : P.Indices) PutU(W.Bin, I);
            const int Idx = W.Accessor(W.View(Offset, 34963), 5125, P.Indices.size(), "SCALAR", "");
            Materials += (MaterialCount ? "," : "") + MaterialJson(P.Mat, TextureFor);
            Primitives += (MaterialCount ? "," : "") + std::string("{\"attributes\":{\"POSITION\":") + std::to_string(Pos) + ",\"NORMAL\":" + std::to_string(Nrm) +
                ",\"TEXCOORD_0\":" + std::to_string(Uv) + ",\"JOINTS_0\":" + std::to_string(Joints) + ",\"WEIGHTS_0\":" + std::to_string(Weights) +
                "},\"indices\":" + std::to_string(Idx) + ",\"material\":" + std::to_string(MaterialCount) + "}";
            ++MaterialCount;
        }
        // Inverse bind matrices: translation by minus the joint's bind position (column-major).
        const std::size_t IbmOffset = W.Bin.size();
        for (std::size_t I = 0; I < Data.Bones.size(); ++I)
        {
            const auto T = G(B.Head[I]);
            const float M[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -T[0] + 0.0f, -T[1] + 0.0f, -T[2] + 0.0f, 1};
            for (const float F : M) PutF(W.Bin, F);
        }
        const int Ibm = W.Accessor(W.View(IbmOffset, 0), 5126, Data.Bones.size(), "MAT4", "");

        std::string JointList;
        for (std::size_t I = 0; I < Data.Bones.size(); ++I) JointList += (I ? "," : "") + std::to_string(I + 1);
        const int Root = RootBone(Data);
        std::string Json = "{\"asset\":{\"version\":\"2.0\",\"generator\":\"Dark Arisen art kit (MakeHuman CC0 + CMU mocap)\"},\"scene\":0,\"scenes\":[{\"nodes\":[0," +
            std::to_string(Root + 1) + "]}],\"nodes\":[{\"name\":\"" + Escape(Name) + "\",\"mesh\":0,\"skin\":0}," + Resolve(JointNodes(Data, B), 1) +
            "],\"skins\":[{\"name\":\"" + Escape(Name) + "_Skeleton\",\"inverseBindMatrices\":" + std::to_string(Ibm) + ",\"skeleton\":" + std::to_string(Root + 1) +
            ",\"joints\":[" + JointList + "]}],\"meshes\":[{\"name\":\"" + Escape(Name) + "\",\"primitives\":[" + Primitives + "]}],\"materials\":[" + Materials + "]";
        if (TextureCount)
        {
            Json += ",\"samplers\":[{\"magFilter\":9729,\"minFilter\":9987,\"wrapS\":10497,\"wrapT\":10497}],\"textures\":[" + Textures + "],\"images\":[" + Images + "]";
        }
        return Json + W.Tail();
    }

    std::string WriteMotionGltf(const Source& Data, const Body& B, const std::string& Name, const Motion& Clip, const int First, const int Last)
    {
        std::vector<Pose> Poses;
        for (int F = First; F <= Last; ++F) Poses.push_back(PoseFromMotion(Data, B, Clip, F, First));
        const std::size_t Frames = Poses.size();
        const int Root = RootBone(Data);

        Writer W;
        std::size_t Offset = W.Bin.size();
        float EndTime = 0.0f;
        for (std::size_t F = 0; F < Frames; ++F)
        {
            EndTime = Quantize(static_cast<double>(F) * Clip.FrameTime, 1048576.0);
            PutF(W.Bin, EndTime);
        }
        const int Time = W.Accessor(W.View(Offset, 0), 5126, Frames, "SCALAR", ",\"min\":[0],\"max\":[" + NumF(EndTime) + "]");

        std::string Samplers, Channels;
        int SamplerCount = 0;
        for (std::size_t I = 0; I < Data.Bones.size(); ++I)
        {
            const int Parent = Data.Bones[I].Parent;
            std::vector<Quat> Local(Frames);
            bool Moves = static_cast<int>(I) == Root;
            for (std::size_t F = 0; F < Frames; ++F)
            {
                Local[F] = Parent < 0 ? Poses[F].World[I] : NormalizeQ(Mul(Conj(Poses[F].World[static_cast<std::size_t>(Parent)]), Poses[F].World[I]));
                Moves = Moves || std::abs(Local[F].W) < 1.0 - 1e-9;
            }
            if (!Moves) continue;
            Offset = W.Bin.size();
            for (const Quat& Q : Local)
            {
                for (const float C : GQuat(Q)) PutF(W.Bin, C);
            }
            const int Out = W.Accessor(W.View(Offset, 0), 5126, Frames, "VEC4", "");
            Samplers += (SamplerCount ? "," : "") + std::string("{\"input\":") + std::to_string(Time) + ",\"output\":" + std::to_string(Out) + ",\"interpolation\":\"LINEAR\"}";
            Channels += (SamplerCount ? "," : "") + std::string("{\"sampler\":") + std::to_string(SamplerCount) + ",\"target\":{\"node\":" + std::to_string(I) + ",\"path\":\"rotation\"}}";
            ++SamplerCount;
        }
        // Root motion.
        Offset = W.Bin.size();
        for (const Pose& P : Poses)
        {
            for (const float C : G(P.Head[static_cast<std::size_t>(Root)])) PutF(W.Bin, C);
        }
        const int Move = W.Accessor(W.View(Offset, 0), 5126, Frames, "VEC3", "");
        Samplers += (SamplerCount ? "," : "") + std::string("{\"input\":") + std::to_string(Time) + ",\"output\":" + std::to_string(Move) + ",\"interpolation\":\"LINEAR\"}";
        Channels += (SamplerCount ? "," : "") + std::string("{\"sampler\":") + std::to_string(SamplerCount) + ",\"target\":{\"node\":" + std::to_string(Root) + ",\"path\":\"translation\"}}";

        const std::string Json = "{\"asset\":{\"version\":\"2.0\",\"generator\":\"Dark Arisen art kit (MakeHuman CC0 + CMU mocap)\"},\"scene\":0,\"scenes\":[{\"nodes\":[" +
            std::to_string(Root) + "]}],\"nodes\":[" + Resolve(JointNodes(Data, B), 0) + "],\"animations\":[{\"name\":\"" + Escape(Name) + "\",\"samplers\":[" + Samplers +
            "],\"channels\":[" + Channels + "]}]";
        return Json + W.Tail();
    }
}
