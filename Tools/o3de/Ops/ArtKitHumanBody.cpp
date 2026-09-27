// Human pipeline, part 1: source data (MakeHuman mesh, rig, weights and targets; CMU BVH clips),
// body morphing, skeleton fitting, motion retargeting and linear blend skinning.

#include "ArtKitHumanInternal.h"
#include "Json.h"

#include <algorithm>
#include <charconv>
#include <cmath>

namespace DarkArisen::Tools::Art::Humans
{
    namespace
    {
        const std::string MakeHumanRoot = "ContentSource/ThirdParty/MakeHuman/";
        const std::string CmuRoot = "ContentSource/ThirdParty/CMU/";

        /** Line-oriented reader over a text file (accepts LF and CRLF). */
        class Lines
        {
        public:
            explicit Lines(const std::string& Text) : At(Text.data()), End(Text.data() + Text.size()) {}
            bool Next(std::string_view& Line)
            {
                if (At >= End) return false;
                const char* Start = At;
                while (At < End && *At != '\n') ++At;
                const char* Stop = At;
                if (At < End) ++At;
                if (Stop > Start && Stop[-1] == '\r') --Stop;
                Line = std::string_view(Start, static_cast<std::size_t>(Stop - Start));
                return true;
            }
        private:
            const char* At;
            const char* End;
        };

        /** Whitespace separated fields of a line. */
        class Fields
        {
        public:
            explicit Fields(std::string_view Line) : Text(Line) {}
            bool Word(std::string_view& Out)
            {
                while (Pos < Text.size() && (Text[Pos] == ' ' || Text[Pos] == '\t')) ++Pos;
                if (Pos >= Text.size()) return false;
                const std::size_t Start = Pos;
                while (Pos < Text.size() && Text[Pos] != ' ' && Text[Pos] != '\t') ++Pos;
                Out = Text.substr(Start, Pos - Start);
                return true;
            }
            bool Number(double& Out)
            {
                std::string_view W;
                if (!Word(W)) return false;
                if (!W.empty() && W.front() == '+') W.remove_prefix(1);
                const auto Result = std::from_chars(W.data(), W.data() + W.size(), Out);
                return Result.ec == std::errc() && Result.ptr == W.data() + W.size();
            }
            bool Integer(int& Out)
            {
                std::string_view W;
                if (!Word(W)) return false;
                const auto Result = std::from_chars(W.data(), W.data() + W.size(), Out);
                return Result.ec == std::errc();
            }
        private:
            std::string_view Text;
            std::size_t Pos = 0;
        };

        bool ParseObj(const std::string& Text, Source& Out, std::string& Problem)
        {
            Lines Reader(Text);
            std::string_view Line;
            int Group = -1;
            while (Reader.Next(Line))
            {
                Fields F(Line);
                std::string_view Key;
                if (!F.Word(Key)) continue;
                if (Key == "v")
                {
                    V3 P;
                    if (!F.Number(P.X) || !F.Number(P.Y) || !F.Number(P.Z)) { Problem = "base.obj: bad vertex"; return false; }
                    Out.Positions.push_back(P);
                }
                else if (Key == "vt")
                {
                    double U = 0, V = 0;
                    if (!F.Number(U) || !F.Number(V)) { Problem = "base.obj: bad texture coordinate"; return false; }
                    Out.Uvs.push_back({U, 1.0 - V});
                }
                else if (Key == "g")
                {
                    std::string_view Name;
                    F.Word(Name);
                    Group = Out.Group(Name);
                    if (Group < 0)
                    {
                        Out.Groups.emplace_back(Name);
                        Group = static_cast<int>(Out.Groups.size()) - 1;
                    }
                }
                else if (Key == "f")
                {
                    Face Fc;
                    Fc.Group = std::max(Group, 0);
                    std::string_view W;
                    while (F.Word(W) && Fc.Count < 4)
                    {
                        const std::size_t Slash = W.find('/');
                        int Vi = 0, Ti = 0;
                        std::from_chars(W.data(), W.data() + (Slash == std::string_view::npos ? W.size() : Slash), Vi);
                        if (Slash != std::string_view::npos) std::from_chars(W.data() + Slash + 1, W.data() + W.size(), Ti);
                        Fc.V[static_cast<std::size_t>(Fc.Count)] = Vi - 1;
                        Fc.T[static_cast<std::size_t>(Fc.Count)] = Ti - 1;
                        ++Fc.Count;
                    }
                    if (Fc.Count < 3) { Problem = "base.obj: bad face"; return false; }
                    if (Group < 0)
                    {
                        Out.Groups.emplace_back("default");
                        Group = 0;
                    }
                    Out.Faces.push_back(Fc);
                }
            }
            for (const Face& Fc : Out.Faces)
            {
                for (int K = 0; K < Fc.Count; ++K)
                {
                    const auto I = static_cast<std::size_t>(K);
                    if (Fc.V[I] < 0 || Fc.V[I] >= static_cast<int>(Out.Positions.size()) || Fc.T[I] < 0 || Fc.T[I] >= static_cast<int>(Out.Uvs.size()))
                    {
                        Problem = "base.obj: face index out of range";
                        return false;
                    }
                }
            }
            return !Out.Positions.empty();
        }

        bool ParseRig(const std::string& SkeletonText, const std::string& WeightsText, Source& Out, std::string& Problem)
        {
            JsonValue Skeleton, Weights;
            if (!JsonReader::Parse(SkeletonText, Skeleton, Problem) || !JsonReader::Parse(WeightsText, Weights, Problem)) return false;
            const JsonValue* Bones = Skeleton.Find("bones");
            const JsonValue* Joints = Skeleton.Find("joints");
            const JsonValue* BoneWeights = Weights.Find("weights");
            if (!Bones || !Joints || !BoneWeights || !Bones->IsObject() || !Joints->IsObject() || !BoneWeights->IsObject())
            {
                Problem = "default.mhskel/default_weights.mhw: unexpected layout";
                return false;
            }
            for (const std::string& Name : Joints->Order)
            {
                std::vector<int>& List = Out.Joints[Name];
                for (const JsonValue& Index : Joints->Find(Name)->Items) List.push_back(static_cast<int>(Index.Number));
            }
            // Parents before children, otherwise document order.
            std::vector<std::string> Pending = Bones->Order;
            while (!Pending.empty())
            {
                std::vector<std::string> Later;
                for (const std::string& Name : Pending)
                {
                    const JsonValue& B = *Bones->Find(Name);
                    const JsonValue* Parent = B.Find("parent");
                    const int ParentIndex = Parent && Parent->IsString() ? Out.FindBone(Parent->Text) : -1;
                    if (Parent && Parent->IsString() && ParentIndex < 0)
                    {
                        Later.push_back(Name);
                        continue;
                    }
                    Bone Entry;
                    Entry.Name = Name;
                    Entry.Parent = ParentIndex;
                    Entry.Head = B.Find("head") ? B.Find("head")->Text : std::string();
                    Entry.Tail = B.Find("tail") ? B.Find("tail")->Text : std::string();
                    if (!Out.Joints.count(Entry.Head) || !Out.Joints.count(Entry.Tail))
                    {
                        Problem = "default.mhskel: bone " + Name + " names an unknown joint";
                        return false;
                    }
                    Out.Bones.push_back(std::move(Entry));
                }
                if (Later.size() == Pending.size())
                {
                    Problem = "default.mhskel: bone hierarchy has a cycle or a missing parent";
                    return false;
                }
                Pending.swap(Later);
            }
            Out.Weights.assign(Out.Positions.size(), {});
            for (const std::string& Name : BoneWeights->Order)
            {
                const int BoneIndex = Out.FindBone(Name);
                if (BoneIndex < 0) continue;
                for (const JsonValue& Pair : BoneWeights->Find(Name)->Items)
                {
                    if (Pair.Items.size() != 2) continue;
                    const auto Vertex = static_cast<std::size_t>(Pair.Items[0].Number);
                    if (Vertex < Out.Weights.size()) Out.Weights[Vertex].push_back({BoneIndex, Pair.Items[1].Number});
                }
            }
            const int Root = Out.FindBone("root");
            for (auto& List : Out.Weights)
            {
                std::sort(List.begin(), List.end(), [](const auto& A, const auto& B) { return A.second != B.second ? A.second > B.second : A.first < B.first; });
                double Sum = 0;
                for (const auto& W : List) Sum += W.second;
                if (Sum <= 0.0)
                {
                    List.assign(1, {std::max(Root, 0), 1.0});
                    continue;
                }
                for (auto& W : List) W.second /= Sum;
            }
            return true;
        }

        bool ParseTarget(const std::string& Text, std::vector<std::pair<int, V3>>& Out)
        {
            Lines Reader(Text);
            std::string_view Line;
            while (Reader.Next(Line))
            {
                if (Line.empty() || Line.front() == '#') continue;
                Fields F(Line);
                int Index = 0;
                V3 D;
                if (!F.Integer(Index) || !F.Number(D.X) || !F.Number(D.Y) || !F.Number(D.Z)) return false;
                Out.push_back({Index, D});
            }
            return true;
        }

        bool ParseBvh(const std::string& Text, Motion& Out, std::string& Problem)
        {
            Lines Reader(Text);
            std::string_view Line;
            std::vector<int> Stack;
            int Channels = 0;
            bool InMotion = false;
            std::size_t FrameCount = 0;
            while (Reader.Next(Line))
            {
                Fields F(Line);
                std::string_view Key;
                if (!InMotion)
                {
                    if (!F.Word(Key)) continue;
                    if (Key == "ROOT" || Key == "JOINT" || Key == "End")
                    {
                        Motion::Joint J;
                        std::string_view Name;
                        F.Word(Name);
                        J.Parent = Stack.empty() ? -1 : Stack.back();
                        J.Name = Key == "End" ? Out.Joints[static_cast<std::size_t>(J.Parent)].Name + "_End" : std::string(Name);
                        J.First = Channels;
                        Out.Joints.push_back(std::move(J));
                    }
                    else if (Key == "{")
                    {
                        Stack.push_back(static_cast<int>(Out.Joints.size()) - 1);
                    }
                    else if (Key == "}")
                    {
                        if (!Stack.empty()) Stack.pop_back();
                    }
                    else if (Key == "OFFSET" && !Out.Joints.empty())
                    {
                        V3& O = Out.Joints[static_cast<std::size_t>(Stack.back())].Offset;
                        if (!F.Number(O.X) || !F.Number(O.Y) || !F.Number(O.Z)) { Problem = "bad OFFSET"; return false; }
                    }
                    else if (Key == "CHANNELS" && !Stack.empty())
                    {
                        Motion::Joint& J = Out.Joints[static_cast<std::size_t>(Stack.back())];
                        int Count = 0;
                        F.Integer(Count);
                        J.First = Channels;
                        std::string_view C;
                        for (int K = 0; K < Count && F.Word(C); ++K)
                        {
                            const int Axis = C.front() == 'X' ? 0 : C.front() == 'Y' ? 1 : 2;
                            J.Channels.push_back(C.find("rotation") != std::string_view::npos ? Axis + 3 : Axis);
                        }
                        Channels += Count;
                    }
                    else if (Key == "MOTION")
                    {
                        InMotion = true;
                    }
                    continue;
                }
                if (!F.Word(Key)) continue;
                if (Key == "Frames:")
                {
                    int Count = 0;
                    F.Integer(Count);
                    FrameCount = static_cast<std::size_t>(std::max(Count, 0));
                    continue;
                }
                if (Key == "Frame")
                {
                    std::string_view Time;
                    F.Word(Time);
                    F.Number(Out.FrameTime);
                    continue;
                }
                Fields Values(Line);
                std::vector<double> Frame;
                Frame.reserve(static_cast<std::size_t>(Channels));
                double V = 0;
                while (Values.Number(V)) Frame.push_back(V);
                if (static_cast<int>(Frame.size()) != Channels) { Problem = "frame with the wrong channel count"; return false; }
                Out.Frames.push_back(std::move(Frame));
            }
            if (Out.Frames.size() != FrameCount || Out.Frames.size() < 2) { Problem = "frame count mismatch"; return false; }
            return true;
        }

        enum class Region { Head, Neck, Torso, Arm, Hand, Leg, Foot };
        Region RegionOf(const std::string& Bone)
        {
            const auto Starts = [&Bone](const char* Prefix) { return Bone.rfind(Prefix, 0) == 0; };
            if (Starts("neck")) return Region::Neck;
            if (Starts("root") || Starts("spine") || Starts("breast") || Starts("clavicle") || Starts("pelvis")) return Region::Torso;
            if (Starts("shoulder") || Starts("upperarm") || Starts("lowerarm")) return Region::Arm;
            if (Starts("wrist") || Starts("metacarpal") || Starts("finger")) return Region::Hand;
            if (Starts("upperleg") || Starts("lowerleg")) return Region::Leg;
            if (Starts("foot") || Starts("toe")) return Region::Foot;
            return Region::Head;
        }

        /** Parameter along a polyline A0..A3 (segment index plus fraction) of the closest point to P. */
        double ChainParameter(const V3& P, const V3 (&Chain)[4])
        {
            double Best = 1e30, Param = 0;
            for (int S = 0; S < 3; ++S)
            {
                const V3 A = Chain[S], D = Chain[S + 1] - Chain[S];
                const double L2 = Dot(D, D);
                const double T = L2 > 0 ? Clamp(Dot(P - A, D) / L2, 0.0, 1.0) : 0.0;
                const V3 Q = A + D * T;
                const double Dist = Dot(P - Q, P - Q);
                if (Dist < Best)
                {
                    Best = Dist;
                    Param = S + T;
                }
            }
            return Param;
        }

        V3 Mean(const std::vector<V3>& Points, const std::vector<int>& Indices)
        {
            V3 Sum;
            for (const int I : Indices) Sum = Sum + Points[static_cast<std::size_t>(I)];
            return Indices.empty() ? Sum : Sum * (1.0 / static_cast<double>(Indices.size()));
        }
    }

    // ------------------------------------------------------------------ rotation maths

    Quat Mul(const Quat& A, const Quat& B)
    {
        return {A.W * B.W - A.X * B.X - A.Y * B.Y - A.Z * B.Z, A.W * B.X + A.X * B.W + A.Y * B.Z - A.Z * B.Y,
            A.W * B.Y - A.X * B.Z + A.Y * B.W + A.Z * B.X, A.W * B.Z + A.X * B.Y - A.Y * B.X + A.Z * B.W};
    }
    Quat Conj(const Quat& Q) { return {Q.W, -Q.X, -Q.Y, -Q.Z}; }
    Quat NormalizeQ(const Quat& Q)
    {
        const double L = std::sqrt(Q.W * Q.W + Q.X * Q.X + Q.Y * Q.Y + Q.Z * Q.Z);
        if (L <= 0.0) return {};
        Quat R{Q.W / L, Q.X / L, Q.Y / L, Q.Z / L};
        if (R.W < 0.0) R = {-R.W, -R.X, -R.Y, -R.Z};
        return R;
    }
    V3 Rotate(const Quat& Q, const V3& V)
    {
        const V3 U{Q.X, Q.Y, Q.Z};
        const V3 T = Cross(U, V) * 2.0;
        return V + T * Q.W + Cross(U, T);
    }
    Quat AxisAngle(const V3& Axis, const double Radians)
    {
        const V3 A = Normalize(Axis);
        const double S = Sin(Radians * 0.5);
        return {Cos(Radians * 0.5), A.X * S, A.Y * S, A.Z * S};
    }
    Quat FromTo(const V3& From, const V3& To)
    {
        const double R = Dot(From, To) + 1.0;
        if (R < 1e-9)
        {
            const V3 Axis = std::abs(From.X) > std::abs(From.Z) ? V3{-From.Y, From.X, 0} : V3{0, -From.Z, From.Y};
            return NormalizeQ({0.0, Axis.X, Axis.Y, Axis.Z});
        }
        const V3 C = Cross(From, To);
        return NormalizeQ({R, C.X, C.Y, C.Z});
    }
    Quat FrameTo(const V3& FromDir, const V3& FromUp, const V3& ToDir, const V3& ToUp)
    {
        const V3 A1 = Normalize(FromDir), B1 = Normalize(ToDir);
        const V3 A2r = FromUp - A1 * Dot(FromUp, A1);
        const V3 B2r = ToUp - B1 * Dot(ToUp, B1);
        const double LA = Length(A2r), LB = Length(B2r);
        if (LA < 0.25 * Length(FromUp) || LB < 0.25 * Length(ToUp) || LA <= 0.0 || LB <= 0.0) return FromTo(A1, B1);
        const V3 A2 = A2r * (1.0 / LA), B2 = B2r * (1.0 / LB);
        const V3 A3 = Cross(A1, A2), B3 = Cross(B1, B2);
        // M = B * A^T with the frames as columns.
        double M[3][3];
        const V3 A[3] = {A1, A2, A3}, B[3] = {B1, B2, B3};
        const auto C = [](const V3& V, int I) { return I == 0 ? V.X : I == 1 ? V.Y : V.Z; };
        for (int R = 0; R < 3; ++R)
        {
            for (int K = 0; K < 3; ++K)
            {
                M[R][K] = C(B[0], R) * C(A[0], K) + C(B[1], R) * C(A[1], K) + C(B[2], R) * C(A[2], K);
            }
        }
        const double Trace = M[0][0] + M[1][1] + M[2][2];
        Quat Q;
        if (Trace > 0.0)
        {
            const double S = std::sqrt(Trace + 1.0) * 2.0;
            Q = {0.25 * S, (M[2][1] - M[1][2]) / S, (M[0][2] - M[2][0]) / S, (M[1][0] - M[0][1]) / S};
        }
        else if (M[0][0] > M[1][1] && M[0][0] > M[2][2])
        {
            const double S = std::sqrt(1.0 + M[0][0] - M[1][1] - M[2][2]) * 2.0;
            Q = {(M[2][1] - M[1][2]) / S, 0.25 * S, (M[0][1] + M[1][0]) / S, (M[0][2] + M[2][0]) / S};
        }
        else if (M[1][1] > M[2][2])
        {
            const double S = std::sqrt(1.0 + M[1][1] - M[0][0] - M[2][2]) * 2.0;
            Q = {(M[0][2] - M[2][0]) / S, (M[0][1] + M[1][0]) / S, 0.25 * S, (M[1][2] + M[2][1]) / S};
        }
        else
        {
            const double S = std::sqrt(1.0 + M[2][2] - M[0][0] - M[1][1]) * 2.0;
            Q = {(M[1][0] - M[0][1]) / S, (M[0][2] + M[2][0]) / S, (M[1][2] + M[2][1]) / S, 0.25 * S};
        }
        return NormalizeQ(Q);
    }

    // ------------------------------------------------------------------ source data

    int Source::Group(const std::string_view Name) const
    {
        for (std::size_t I = 0; I < Groups.size(); ++I)
        {
            if (Groups[I] == Name) return static_cast<int>(I);
        }
        return -1;
    }

    int Source::FindBone(const std::string_view Name) const
    {
        for (std::size_t I = 0; I < Bones.size(); ++I)
        {
            if (Bones[I].Name == Name) return static_cast<int>(I);
        }
        return -1;
    }

    int Motion::Find(const std::string_view Name) const
    {
        for (std::size_t I = 0; I < Joints.size(); ++I)
        {
            if (Joints[I].Name == Name) return static_cast<int>(I);
        }
        return -1;
    }

    bool LoadSource(const SourceReader& Reader, Source& Out, std::string& Problem)
    {
        const std::string* Obj = Reader(MakeHumanRoot + "3dobjs/base.obj");
        const std::string* Skeleton = Reader(MakeHumanRoot + "rigs/default.mhskel");
        const std::string* Weights = Reader(MakeHumanRoot + "rigs/default_weights.mhw");
        if (!Obj || !Skeleton || !Weights)
        {
            Problem = "MakeHuman source data missing under " + MakeHumanRoot;
            return false;
        }
        return ParseObj(*Obj, Out, Problem) && ParseRig(*Skeleton, *Weights, Out, Problem);
    }

    const std::vector<std::pair<int, V3>>* LoadTarget(const SourceReader& Reader, Source& Data, const std::string& Name, std::string& Problem)
    {
        if (const auto Found = Data.Targets.find(Name); Found != Data.Targets.end()) return &Found->second;
        const std::string Path = MakeHumanRoot + "targets/" + Name + ".target";
        const std::string* Text = Reader(Path);
        if (!Text)
        {
            Problem = "missing MakeHuman target " + Path;
            return nullptr;
        }
        std::vector<std::pair<int, V3>> Offsets;
        if (!ParseTarget(*Text, Offsets))
        {
            Problem = "unreadable MakeHuman target " + Path;
            return nullptr;
        }
        for (const auto& [Index, Offset] : Offsets)
        {
            if (Index < 0 || Index >= static_cast<int>(Data.Positions.size()))
            {
                Problem = Path + ": vertex index out of range";
                return nullptr;
            }
        }
        return &(Data.Targets[Name] = std::move(Offsets));
    }

    const Motion* LoadMotion(const SourceReader& Reader, Source& Data, const std::string& Clip, std::string& Problem)
    {
        if (const auto Found = Data.Motions.find(Clip); Found != Data.Motions.end()) return &Found->second;
        const std::string Path = CmuRoot + Clip + ".bvh";
        const std::string* Text = Reader(Path);
        if (!Text)
        {
            Problem = "missing motion clip " + Path;
            return nullptr;
        }
        Motion M;
        if (!ParseBvh(*Text, M, Problem))
        {
            Problem = Path + ": " + Problem;
            return nullptr;
        }
        return &(Data.Motions[Clip] = std::move(M));
    }

    // ------------------------------------------------------------------ bodies

    bool BuildBody(const SourceReader& Reader, Source& Data, const PersonSpec& Spec, Body& Out, std::string& Problem)
    {
        // MakeHuman macro modifier weights (apps/humanmodifier.py semantics, adults from 25 to 90).
        const double Male = Clamp(Spec.Male, 0.0, 1.0);
        const double Old = Clamp((Spec.Age - 25.0) / 65.0, 0.0, 1.0);
        const double RaceSum = Spec.African + Spec.Asian + Spec.Caucasian;
        const double Races[3] = {Spec.African / RaceSum, Spec.Asian / RaceSum, Spec.Caucasian / RaceSum};
        const char* RaceNames[3] = {"african", "asian", "caucasian"};
        const auto Tri = [](double V, double& Min, double& Avg, double& Max)
        {
            V = Clamp(V, 0.0, 1.0);
            Min = V < 0.5 ? 1.0 - 2.0 * V : 0.0;
            Max = V > 0.5 ? 2.0 * V - 1.0 : 0.0;
            Avg = 1.0 - Min - Max;
        };
        double Muscles[3], Weights[3];
        Tri(Spec.Muscle, Muscles[0], Muscles[1], Muscles[2]);
        Tri(Spec.Weight, Weights[0], Weights[1], Weights[2]);
        const char* MuscleNames[3] = {"minmuscle", "averagemuscle", "maxmuscle"};
        const char* WeightNames[3] = {"minweight", "averageweight", "maxweight"};

        std::vector<std::pair<std::string, double>> Targets;
        for (int G = 0; G < 2; ++G)
        {
            const double Gw = G == 0 ? 1.0 - Male : Male;
            const char* Gender = G == 0 ? "female" : "male";
            for (int A = 0; A < 2; ++A)
            {
                const double Aw = A == 0 ? 1.0 - Old : Old;
                const char* Age = A == 0 ? "young" : "old";
                if (Gw * Aw <= 0.0) continue;
                for (int R = 0; R < 3; ++R)
                {
                    if (Races[R] > 0.0) Targets.push_back({std::string("macrodetails/") + RaceNames[R] + "-" + Gender + "-" + Age, Races[R] * Gw * Aw});
                }
                for (int Mi = 0; Mi < 3; ++Mi)
                {
                    for (int Wi = 0; Wi < 3; ++Wi)
                    {
                        const double W = Gw * Aw * Muscles[Mi] * Weights[Wi];
                        if (W <= 0.0 || (Mi == 1 && Wi == 1)) continue;
                        Targets.push_back({std::string("macrodetails/universal-") + Gender + "-" + Age + "-" + MuscleNames[Mi] + "-" + WeightNames[Wi], W});
                    }
                }
                const double Ideal = Clamp((Spec.Proportions - 0.5) * 2.0, 0.0, 1.0);
                if (Ideal > 0.0)
                {
                    Targets.push_back({std::string("macrodetails/proportions/") + Gender + "-" + Age + "-averagemuscle-averageweight-idealproportions", Gw * Aw * Ideal});
                }
                if (G == 0)
                {
                    const double Cup = (Spec.BreastSize - 0.5) * 2.0;
                    if (Cup != 0.0)
                    {
                        Targets.push_back({std::string("breast/female-") + Age + "-averagemuscle-averageweight-" + (Cup > 0 ? "maxcup" : "mincup") + "-averagefirmness",
                            Gw * Aw * (Cup > 0 ? Cup : -Cup)});
                    }
                }
            }
        }
        for (const auto& Detail : Spec.Detail) Targets.push_back(Detail);

        Out.Rest = Data.Positions;
        for (const auto& [Name, W] : Targets)
        {
            const auto* Offsets = LoadTarget(Reader, Data, Name, Problem);
            if (!Offsets) return false;
            for (const auto& [Index, D] : *Offsets) Out.Rest[static_cast<std::size_t>(Index)] = Out.Rest[static_cast<std::size_t>(Index)] + D * W;
        }

        // Stand on y = 0 and scale to the authored height.
        const int BodyGroup = Data.Group("body");
        Out.IsBody.assign(Out.Rest.size(), 0);
        for (const Face& F : Data.Faces)
        {
            if (F.Group != BodyGroup) continue;
            for (int K = 0; K < F.Count; ++K) Out.IsBody[static_cast<std::size_t>(F.V[static_cast<std::size_t>(K)])] = 1;
        }
        double MinY = 1e30, MaxY = -1e30;
        for (std::size_t I = 0; I < Out.Rest.size(); ++I)
        {
            if (!Out.IsBody[I]) continue;
            MinY = std::min(MinY, Out.Rest[I].Y);
            MaxY = std::max(MaxY, Out.Rest[I].Y);
        }
        const double Scale = Spec.Height * 10.0 / (MaxY - MinY);
        for (V3& P : Out.Rest) P = V3{P.X * Scale, (P.Y - MinY) * Scale, P.Z * Scale};
        Out.Stature = Spec.Height * 10.0;

        // Skeleton from the joint vertex groups.
        const auto Joint = [&](const std::string& Name) { return Mean(Out.Rest, Data.Joints.find(Name)->second); };
        Out.Head.clear();
        Out.Tail.clear();
        for (const Bone& B : Data.Bones)
        {
            Out.Head.push_back(Joint(B.Head));
            Out.Tail.push_back(Joint(B.Tail));
        }
        const auto HeadOf = [&](const char* Name) { return Out.Head[static_cast<std::size_t>(Data.FindBone(Name))]; };
        const auto TailOf = [&](const char* Name) { return Out.Tail[static_cast<std::size_t>(Data.FindBone(Name))]; };
        const char* Sides[2] = {"L", "R"};
        for (int S = 0; S < 2; ++S)
        {
            const std::string X = std::string(".") + Sides[S];
            Out.Shoulder[S] = HeadOf(("upperarm01" + X).c_str());
            Out.Elbow[S] = HeadOf(("lowerarm01" + X).c_str());
            Out.Wrist[S] = HeadOf(("wrist" + X).c_str());
            Out.FingerTip[S] = TailOf(("finger3-3" + X).c_str());
            Out.Hip[S] = HeadOf(("upperleg01" + X).c_str());
            Out.Knee[S] = HeadOf(("lowerleg01" + X).c_str());
            Out.Ankle[S] = HeadOf(("foot" + X).c_str());
            Out.ToeTip[S] = TailOf(("toe3-3" + X).c_str());
        }
        Out.HipY = (Out.Hip[0].Y + Out.Hip[1].Y) * 0.5;
        Out.NeckY = HeadOf("neck01").Y;

        // Rest normals and cavity over the body topology.
        Out.Normals.assign(Out.Rest.size(), V3{});
        std::vector<std::vector<int>> Neighbours(Out.Rest.size());
        for (const Face& F : Data.Faces)
        {
            const bool BodyFace = F.Group == BodyGroup;
            if (Data.Groups[static_cast<std::size_t>(F.Group)].rfind("joint-", 0) == 0) continue;
            for (int K = 1; K + 1 < F.Count; ++K)
            {
                const V3& A = Out.Rest[static_cast<std::size_t>(F.V[0])];
                const V3& Bv = Out.Rest[static_cast<std::size_t>(F.V[static_cast<std::size_t>(K)])];
                const V3& C = Out.Rest[static_cast<std::size_t>(F.V[static_cast<std::size_t>(K + 1)])];
                const V3 N = Cross(Bv - A, C - A);
                for (const int I : {F.V[0], F.V[static_cast<std::size_t>(K)], F.V[static_cast<std::size_t>(K + 1)]}) Out.Normals[static_cast<std::size_t>(I)] = Out.Normals[static_cast<std::size_t>(I)] + N;
            }
            if (!BodyFace) continue;
            for (int K = 0; K < F.Count; ++K)
            {
                const int A = F.V[static_cast<std::size_t>(K)], B = F.V[static_cast<std::size_t>((K + 1) % F.Count)];
                Neighbours[static_cast<std::size_t>(A)].push_back(B);
                Neighbours[static_cast<std::size_t>(B)].push_back(A);
            }
        }
        for (V3& N : Out.Normals) N = Normalize(N);
        for (auto& List : Neighbours)
        {
            std::sort(List.begin(), List.end());
            List.erase(std::unique(List.begin(), List.end()), List.end());
        }

        // Landmarks.
        const auto GroupMean = [&](const char* Name)
        {
            const int G = Data.Group(Name);
            V3 Sum;
            int Count = 0;
            std::vector<std::uint8_t> Seen(Out.Rest.size(), 0);
            for (const Face& F : Data.Faces)
            {
                if (F.Group != G) continue;
                for (int K = 0; K < F.Count; ++K)
                {
                    const auto I = static_cast<std::size_t>(F.V[static_cast<std::size_t>(K)]);
                    if (Seen[I]) continue;
                    Seen[I] = 1;
                    Sum = Sum + Out.Rest[I];
                    ++Count;
                }
            }
            return Count ? Sum * (1.0 / Count) : Sum;
        };
        Out.EyeL = GroupMean("helper-l-eye");
        Out.EyeR = GroupMean("helper-r-eye");
        Out.EyeMid = (Out.EyeL + Out.EyeR) * 0.5;
        Out.Mouth = (HeadOf("oris01") + HeadOf("oris05")) * 0.5;
        const V3 CornerL = HeadOf("levator05.L"), CornerR = HeadOf("levator05.R");
        Out.MouthL = V3{Out.Mouth.X + (CornerL.X - Out.Mouth.X) * 0.82, Out.Mouth.Y, CornerL.Z};
        Out.MouthR = V3{Out.Mouth.X + (CornerR.X - Out.Mouth.X) * 0.82, Out.Mouth.Y, CornerR.Z};
        Out.Chin = TailOf("jaw");
        Out.HeadTop = V3{0, -1e30, 0};
        Out.NoseTip = V3{0, 0, -1e30};
        double MinZ = 1e30;
        for (std::size_t I = 0; I < Out.Rest.size(); ++I)
        {
            if (!Out.IsBody[I]) continue;
            const V3& P = Out.Rest[I];
            if (P.Y > Out.HeadTop.Y) Out.HeadTop = P;
            if (P.Y > Out.Mouth.Y && P.Y < Out.EyeMid.Y && std::abs(P.X - Out.EyeMid.X) < 0.3 && P.Z > Out.NoseTip.Z) Out.NoseTip = P;
            if (P.Y > Out.EyeMid.Y && P.Y < Out.HeadTop.Y + 1.0 && P.Z < MinZ && std::abs(P.X) < 0.3) MinZ = P.Z;
        }
        Out.HeadCentre = V3{Out.EyeMid.X, (Out.EyeMid.Y + Out.HeadTop.Y) * 0.5 - 0.15, (MinZ + Out.NoseTip.Z) * 0.5 - 0.15};
        // Ears: the most lateral head points level with the eyes, moved a little inwards.
        for (int S = 0; S < 2; ++S)
        {
            const double Side = S == 0 ? 1.0 : -1.0;
            V3 Best = Out.HeadCentre;
            for (std::size_t I = 0; I < Out.Rest.size(); ++I)
            {
                const V3& P = Out.Rest[I];
                if (!Out.IsBody[I] || P.Y < Out.EyeMid.Y - 0.45 || P.Y > Out.EyeMid.Y + 0.15 || P.Z > Out.EyeMid.Z) continue;
                if ((P.X - Out.HeadCentre.X) * Side > (Best.X - Out.HeadCentre.X) * Side) Best = P;
            }
            Out.Ear[S] = Best - V3{Side * 0.12, 0.05, 0.0};
        }

        // Anatomy per body vertex.
        Out.Parts.assign(Out.Rest.size(), Anatomy{});
        for (std::size_t I = 0; I < Out.Rest.size(); ++I)
        {
            if (!Out.IsBody[I]) continue;
            Anatomy& A = Out.Parts[I];
            for (const auto& [BoneIndex, W] : Data.Weights[I])
            {
                switch (RegionOf(Data.Bones[static_cast<std::size_t>(BoneIndex)].Name))
                {
                case Region::Head: A.Head += W; break;
                case Region::Neck: A.Neck += W; break;
                case Region::Torso: A.Torso += W; break;
                case Region::Arm: A.Arm += W; break;
                case Region::Hand: A.Hand += W; break;
                case Region::Leg: A.Leg += W; break;
                case Region::Foot: A.Foot += W; break;
                }
            }
            const V3& P = Out.Rest[I];
            const int S = P.X >= 0.0 ? 0 : 1;
            A.Side = S == 0 ? 1.0 : -1.0;
            const V3 ArmChain[4] = {Out.Shoulder[S], Out.Elbow[S], Out.Wrist[S], Out.FingerTip[S]};
            const V3 LegChain[4] = {Out.Hip[S], Out.Knee[S], Out.Ankle[S], Out.ToeTip[S]};
            A.ArmT = ChainParameter(P, ArmChain);
            A.LegT = ChainParameter(P, LegChain);
            A.TorsoF = Out.TorsoF(P.Y);
            const auto& Near = Neighbours[I];
            if (!Near.empty())
            {
                V3 Avg;
                double Edge = 0;
                for (const int J : Near)
                {
                    Avg = Avg + Out.Rest[static_cast<std::size_t>(J)];
                    Edge += Length(Out.Rest[static_cast<std::size_t>(J)] - P);
                }
                Avg = Avg * (1.0 / static_cast<double>(Near.size()));
                Edge /= static_cast<double>(Near.size());
                A.Cavity = Edge > 0 ? Dot(Avg - P, Out.Normals[I]) / Edge : 0.0;
            }
        }
        Out.Neighbours = std::move(Neighbours);
        return true;
    }

    // ------------------------------------------------------------------ poses

    Pose RestPose(const Source& Data, const Body& B)
    {
        Pose P;
        P.World.assign(Data.Bones.size(), Quat{});
        P.Head = B.Head;
        return P;
    }

    namespace
    {
        struct Retarget
        {
            const char* Bone;
            const char* From;   // BVH joints giving the bone direction
            const char* To;
            const char* UpFrom; // BVH joints giving the secondary axis (nullptr: swing only)
            const char* UpTo;
            const char* RestUpFrom;  // MakeHuman bones whose heads give the rest secondary axis
            const char* RestUpTo;
        };

        // "#" is replaced by L/Left or R/Right.
        constexpr Retarget RetargetMap[] = {
            {"spine05", "LowerBack", "Spine", "RightUpLeg", "LeftUpLeg", "upperleg01.R", "upperleg01.L"},
            {"spine04", "LowerBack", "Spine", "RightUpLeg", "LeftUpLeg", "upperleg01.R", "upperleg01.L"},
            {"spine03", "Spine", "Spine1", "RightArm", "LeftArm", "upperarm01.R", "upperarm01.L"},
            {"spine02", "Spine", "Spine1", "RightArm", "LeftArm", "upperarm01.R", "upperarm01.L"},
            {"spine01", "Spine1", "Neck1", "RightArm", "LeftArm", "upperarm01.R", "upperarm01.L"},  // Neck sits on Spine1
            {"neck01", "Neck", "Neck1", "RightArm", "LeftArm", "upperarm01.R", "upperarm01.L"},
            {"neck02", "Neck1", "Head", "RightArm", "LeftArm", "upperarm01.R", "upperarm01.L"},
            {"neck03", "Neck1", "Head", "RightArm", "LeftArm", "upperarm01.R", "upperarm01.L"},
            {"head", "Head", "Head_End", "RightArm", "LeftArm", "upperarm01.R", "upperarm01.L"},
            {"clavicle.#", "#Shoulder", "#Arm", nullptr, nullptr, nullptr, nullptr},
            {"upperarm01.#", "#Arm", "#ForeArm", "#ForeArm", "#Hand", "lowerarm01.#", "wrist.#"},
            {"upperarm02.#", "#Arm", "#ForeArm", "#ForeArm", "#Hand", "lowerarm01.#", "wrist.#"},
            {"lowerarm01.#", "#ForeArm", "#Hand", "#Hand", "#Thumb_End", "wrist.#", "finger1-2.#"},  // #Thumb sits on #Hand
            {"lowerarm02.#", "#ForeArm", "#Hand", "#Hand", "#Thumb_End", "wrist.#", "finger1-2.#"},
            {"wrist.#", "#Hand", "#HandIndex1", "#Hand", "#Thumb_End", "wrist.#", "finger1-2.#"},
            {"upperleg01.#", "#UpLeg", "#Leg", "#Foot", "#ToeBase", "foot.#", "toe3-1.#"},
            {"upperleg02.#", "#UpLeg", "#Leg", "#Foot", "#ToeBase", "foot.#", "toe3-1.#"},
            {"lowerleg01.#", "#Leg", "#Foot", "#Foot", "#ToeBase", "foot.#", "toe3-1.#"},
            {"lowerleg02.#", "#Leg", "#Foot", "#Foot", "#ToeBase", "foot.#", "toe3-1.#"},
            {"foot.#", "#Foot", "#ToeBase", "#Leg", "#Foot", "lowerleg01.#", "foot.#"},
        };

        std::string Sided(const char* Pattern, const bool Left, const bool Bvh)
        {
            std::string Out;
            for (const char* C = Pattern; *C; ++C)
            {
                if (*C != '#')
                {
                    Out.push_back(*C);
                    continue;
                }
                if (!Bvh) Out += Left ? "L" : "R";
                else if (std::string_view(C + 1).rfind("Thumb", 0) == 0) Out += Left ? "L" : "R";
                else Out += Left ? "Left" : "Right";
            }
            return Out;
        }

        /** World positions and rotations of every BVH joint at a frame. */
        void Forward(const Motion& M, const int Frame, std::vector<V3>& Position, std::vector<Quat>& Rotation)
        {
            const std::vector<double>& Values = M.Frames[static_cast<std::size_t>(Frame)];
            Position.assign(M.Joints.size(), V3{});
            Rotation.assign(M.Joints.size(), Quat{});
            for (std::size_t J = 0; J < M.Joints.size(); ++J)
            {
                const Motion::Joint& Jt = M.Joints[J];
                V3 Local = Jt.Offset;
                Quat R;
                for (std::size_t C = 0; C < Jt.Channels.size(); ++C)
                {
                    const double V = Values[static_cast<std::size_t>(Jt.First) + C];
                    const int Channel = Jt.Channels[C];
                    if (Channel < 3)
                    {
                        if (Channel == 0) Local.X += V;
                        else if (Channel == 1) Local.Y += V;
                        else Local.Z += V;
                    }
                    else
                    {
                        const V3 Axis = Channel == 3 ? V3{1, 0, 0} : Channel == 4 ? V3{0, 1, 0} : V3{0, 0, 1};
                        R = Mul(R, AxisAngle(Axis, V * Pi / 180.0));
                    }
                }
                if (Jt.Parent < 0)
                {
                    Position[J] = Local;
                    Rotation[J] = NormalizeQ(R);
                }
                else
                {
                    const auto Parent = static_cast<std::size_t>(Jt.Parent);
                    Position[J] = Position[Parent] + Rotate(Rotation[Parent], Local);
                    Rotation[J] = NormalizeQ(Mul(Rotation[Parent], R));
                }
            }
        }
    }

    Pose PoseFromMotion(const Source& Data, const Body& B, const Motion& Clip, const int Frame, const int Reference, const double Upright, const double Curl)
    {
        std::vector<V3> T0, Now, RefP;
        std::vector<Quat> R0, RNow, RRef;
        Forward(Clip, 0, T0, R0);
        Forward(Clip, Frame, Now, RNow);
        Forward(Clip, Reference, RefP, RRef);
        const int Hips = Clip.Find("Hips");
        const auto Hi = static_cast<std::size_t>(std::max(Hips, 0));
        const Quat Hips0 = R0[Hi];

        // Heading: the person faces +Z at the reference frame, hips over the origin.
        const V3 Facing = Rotate(Mul(RRef[Hi], Conj(Hips0)), V3{0, 0, 1});
        const Quat Yaw = AxisAngle(V3{0, 1, 0}, -Atan2(Facing.X, Facing.Z));
        const V3 Anchor{RefP[Hi].X, 0.0, RefP[Hi].Z};
        for (V3& P : Now) P = Rotate(Yaw, P - Anchor);

        // Scale from the T-pose hip height to the body's.
        double Ground = 1e30;
        for (const V3& P : T0) Ground = std::min(Ground, P.Y);
        const double Scale = B.HipY / std::max(T0[Hi].Y - Ground, 1e-6);

        Pose Out;
        Out.World.assign(Data.Bones.size(), Quat{});
        Out.Head.assign(Data.Bones.size(), V3{});
        const Quat HipsNow = NormalizeQ(Mul(Yaw, Mul(RNow[Hi], Conj(Hips0))));

        struct Target
        {
            int From = -1, To = -1, UpFrom = -1, UpTo = -1;
            V3 RestUp;
            bool HasUp = false;
            bool Relative = false;  // spine, neck, head: motion relative to the clip's T-pose, on the body's own posture
        };
        std::vector<Target> Targets(Data.Bones.size());
        for (const Retarget& R : RetargetMap)
        {
            for (const bool Left : {true, false})
            {
                const std::string Bone = Sided(R.Bone, Left, false);
                const int Index = Data.FindBone(Bone);
                if (Index < 0) continue;
                Target& T = Targets[static_cast<std::size_t>(Index)];
                T.From = Clip.Find(Sided(R.From, Left, true));
                T.To = Clip.Find(Sided(R.To, Left, true));
                if (R.UpFrom)
                {
                    T.UpFrom = Clip.Find(Sided(R.UpFrom, Left, true));
                    T.UpTo = Clip.Find(Sided(R.UpTo, Left, true));
                    const int A = Data.FindBone(Sided(R.RestUpFrom, Left, false));
                    const int C = Data.FindBone(Sided(R.RestUpTo, Left, false));
                    if (A >= 0 && C >= 0 && T.UpFrom >= 0 && T.UpTo >= 0)
                    {
                        T.RestUp = B.Head[static_cast<std::size_t>(C)] - B.Head[static_cast<std::size_t>(A)];
                        T.HasUp = true;
                    }
                }
                if (Bone.find('.') == std::string::npos)
                {
                    T.Relative = true;
                    break;  // unsided bone: once
                }
            }
        }

        for (std::size_t I = 0; I < Data.Bones.size(); ++I)
        {
            const Bone& Bn = Data.Bones[I];
            if (Bn.Parent < 0)
            {
                Out.World[I] = HipsNow;
                const V3 Hip = Now[Hi];
                Out.Head[I] = B.Head[I] + V3{Hip.X * Scale, (Hip.Y - T0[Hi].Y) * Scale, Hip.Z * Scale};
                continue;
            }
            const auto Parent = static_cast<std::size_t>(Bn.Parent);
            const Quat ParentQ = Out.World[Parent];
            Out.Head[I] = Out.Head[Parent] + Rotate(ParentQ, B.Head[I] - B.Head[Parent]);
            const Target& T = Targets[I];
            Quat Q = ParentQ;
            if (T.From >= 0 && T.To >= 0 && Length(Now[static_cast<std::size_t>(T.To)] - Now[static_cast<std::size_t>(T.From)]) > 1e-6)
            {
                const auto F = static_cast<std::size_t>(T.From), To = static_cast<std::size_t>(T.To);
                const V3 RestDir = Normalize(B.Tail[I] - B.Head[I]);
                const V3 Have = Rotate(ParentQ, RestDir);
                V3 Want = Normalize(Now[To] - Now[F]);
                Quat Delta = FromTo(Have, Want);
                if (T.HasUp)
                {
                    V3 WantUp = Now[static_cast<std::size_t>(T.UpTo)] - Now[static_cast<std::size_t>(T.UpFrom)];
                    if (T.Relative)
                    {
                        // Rotation of the clip's segment since its T-pose, applied to the body's rest posture.
                        const V3 PoseUp = T0[static_cast<std::size_t>(T.UpTo)] - T0[static_cast<std::size_t>(T.UpFrom)];
                        Quat Since = FrameTo(Normalize(T0[To] - T0[F]), PoseUp, Want, WantUp);
                        const double Keep = 1.0 - Clamp(Upright, 0.0, 1.0);
                        Since = NormalizeQ({1.0 - Keep + Since.W * Keep, Since.X * Keep, Since.Y * Keep, Since.Z * Keep});
                        Want = Rotate(Since, RestDir);
                        WantUp = Rotate(Since, T.RestUp);
                    }
                    Delta = FrameTo(Have, Rotate(ParentQ, T.RestUp), Want, WantUp);
                }
                Q = NormalizeQ(Mul(Delta, ParentQ));
            }
            if (Curl > 0.0 && Bn.Name.rfind("finger", 0) == 0 && Bn.Name.size() > 9)
            {
                // Relaxed hand: each finger segment bends about the knuckle line, towards the thumb.
                const bool Left = Bn.Name.back() == 'L';
                const int Segment = Bn.Name[8] - '0';
                const bool Thumb = Bn.Name[6] == '1';
                const std::string X = Left ? ".L" : ".R";
                const V3 Knuckles = Normalize(B.Head[static_cast<std::size_t>(Data.FindBone("finger2-1" + X))] - B.Head[static_cast<std::size_t>(Data.FindBone("finger5-1" + X))]);
                const V3 Pivot = B.Head[static_cast<std::size_t>(Data.FindBone("finger3-1" + X))];
                const V3 Tip = B.Tail[static_cast<std::size_t>(Data.FindBone("finger3-3" + X))];
                const V3 ThumbTip = B.Tail[static_cast<std::size_t>(Data.FindBone("finger1-3" + X))];
                const V3 Bent = Pivot + Rotate(AxisAngle(Knuckles, 0.5), Tip - Pivot);
                const double Sign = Length(Bent - ThumbTip) < Length(Tip - ThumbTip) ? 1.0 : -1.0;
                const double Degrees[4] = {0.0, 30.0, 45.0, 30.0};
                const double Angle = Curl * (Thumb ? 0.35 : 1.0) * Degrees[std::clamp(Segment, 0, 3)] * Pi / 180.0;
                Q = NormalizeQ(Mul(Q, AxisAngle(Knuckles, Sign * Angle)));
            }
            Out.World[I] = Q;
        }
        return Out;
    }

    // ------------------------------------------------------------------ skinning

    void InfluencesOf(const Source& Data, const int Index, std::array<int, 4>& Joint, std::array<double, 4>& Weight)
    {
        const auto& List = Data.Weights[static_cast<std::size_t>(Index)];
        Joint = {0, 0, 0, 0};
        Weight = {0, 0, 0, 0};
        double Sum = 0;
        for (std::size_t K = 0; K < 4 && K < List.size(); ++K)
        {
            Joint[K] = List[K].first;
            Weight[K] = List[K].second;
            Sum += List[K].second;
        }
        for (double& W : Weight) W /= Sum;
    }

    V3 SkinPoint(const Body& B, const Pose& P, const SkinVertex& V)
    {
        V3 Out;
        for (std::size_t K = 0; K < 4; ++K)
        {
            if (V.Weight[K] <= 0.0) continue;
            const auto J = static_cast<std::size_t>(V.Joint[K]);
            Out = Out + (P.Head[J] + Rotate(P.World[J], V.P - B.Head[J])) * V.Weight[K];
        }
        return Out;
    }

    V3 SkinNormal(const Pose& P, const SkinVertex& V)
    {
        V3 Out;
        for (std::size_t K = 0; K < 4; ++K)
        {
            if (V.Weight[K] > 0.0) Out = Out + Rotate(P.World[static_cast<std::size_t>(V.Joint[K])], V.N) * V.Weight[K];
        }
        return Normalize(Out);
    }
}
