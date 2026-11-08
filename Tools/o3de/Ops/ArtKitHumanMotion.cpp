// Gameplay motion library (ArtKitHumans.h, MotionSpec): cuts of the CMU clips, retargeted onto a
// person's skeleton and shaped for play. A cut can be time-warped so its hit lands on the authored
// combat timing, mirrored (left and right exchanged), looped (the end crossfades into the start,
// horizontally in place) and layered (the upper body held from another clip, e.g. a guard while
// strafing). Only + - * / and sqrt are used, so the output is the same on every compiler.

#include "ArtKitHumanInternal.h"

#include <algorithm>
#include <cmath>

namespace DarkArisen::Tools::Art::Humans
{
    namespace
    {
        Quat Nlerp(const Quat& A, const Quat& B, const double T)
        {
            const double Dot = A.W * B.W + A.X * B.X + A.Y * B.Y + A.Z * B.Z;
            const double S = Dot < 0.0 ? -1.0 : 1.0;  // shortest way
            return NormalizeQ(Quat{A.W + (S * B.W - A.W) * T, A.X + (S * B.X - A.X) * T, A.Y + (S * B.Y - A.Y) * T, A.Z + (S * B.Z - A.Z) * T});
        }

        /** Posed heads from the root head and the world rotations, parents first (MakeHuman bone order). */
        void RecomputeHeads(const Source& Data, const Body& B, Pose& P)
        {
            for (std::size_t I = 0; I < Data.Bones.size(); ++I)
            {
                const int Parent = Data.Bones[I].Parent;
                if (Parent < 0) continue;
                const auto Pi = static_cast<std::size_t>(Parent);
                P.Head[I] = P.Head[Pi] + Rotate(P.World[Pi], B.Head[I] - B.Head[Pi]);
            }
        }

        Pose Blend(const Source& Data, const Body& B, const Pose& A, const Pose& C, const double T)
        {
            Pose Out = A;
            for (std::size_t I = 0; I < A.World.size(); ++I) Out.World[I] = Nlerp(A.World[I], C.World[I], T);
            for (std::size_t I = 0; I < A.Head.size(); ++I)
            {
                if (Data.Bones[I].Parent < 0) Out.Head[I] = A.Head[I] + (C.Head[I] - A.Head[I]) * T;
            }
            RecomputeHeads(Data, B, Out);
            return Out;
        }

        /** Pose at a time in the clip (seconds), between frames by blending. */
        Pose Sample(const Source& Data, const Body& B, const Motion& Clip, const double Time, const int Reference, const MotionSpec& Spec)
        {
            const int Last = static_cast<int>(Clip.Frames.size()) - 1;
            double F = Time / Clip.FrameTime;
            F = std::min(std::max(F, 1.0), static_cast<double>(Last));
            const int F0 = std::min(static_cast<int>(F), Last);
            const int F1 = std::min(F0 + 1, Last);
            const double T = F - static_cast<double>(F0);
            const Pose A = PoseFromMotion(Data, B, Clip, F0, Reference, Spec.Upright, Spec.Curl);
            if (T <= 1e-9 || F1 == F0) return A;
            const Pose C = PoseFromMotion(Data, B, Clip, F1, Reference, Spec.Upright, Spec.Curl);
            return Blend(Data, B, A, C, T);
        }

        /** Left and right exchanged: the pose reflected through the body's mid plane (x = 0). */
        Pose Mirrored(const Source& Data, const Body& B, const Pose& P)
        {
            Pose Out = P;
            for (std::size_t I = 0; I < Data.Bones.size(); ++I)
            {
                std::string Name = Data.Bones[I].Name;
                const std::size_t Dot = Name.rfind('.');
                if (Dot != std::string::npos && Dot + 2 == Name.size() && (Name[Dot + 1] == 'L' || Name[Dot + 1] == 'R'))
                {
                    Name[Dot + 1] = Name[Dot + 1] == 'L' ? 'R' : 'L';
                }
                const int J = Data.FindBone(Name);
                const Quat& Q = P.World[static_cast<std::size_t>(J < 0 ? static_cast<int>(I) : J)];
                Out.World[I] = Quat{Q.W, Q.X, -Q.Y, -Q.Z};
                if (Data.Bones[I].Parent < 0)
                {
                    const V3 H = P.Head[I] - B.Head[I];
                    Out.Head[I] = B.Head[I] + V3{-H.X, H.Y, H.Z};
                }
            }
            RecomputeHeads(Data, B, Out);
            return Out;
        }

        /** Upper body (Pivot and everything below it in the hierarchy) of Top laid over Base. */
        Pose Layered(const Source& Data, const Body& B, const Pose& Base, const Pose& Top, const int Pivot)
        {
            std::vector<std::uint8_t> Upper(Data.Bones.size(), 0);
            for (std::size_t I = 0; I < Data.Bones.size(); ++I)
            {
                const int Parent = Data.Bones[I].Parent;
                Upper[I] = static_cast<int>(I) == Pivot || (Parent >= 0 && Upper[static_cast<std::size_t>(Parent)]) ? 1 : 0;
            }
            const auto LocalOf = [&](const Pose& P, const std::size_t I)
            {
                const int Parent = Data.Bones[I].Parent;
                return Parent < 0 ? P.World[I] : Mul(Conj(P.World[static_cast<std::size_t>(Parent)]), P.World[I]);
            };
            Pose Out = Base;
            for (std::size_t I = 0; I < Data.Bones.size(); ++I)
            {
                const int Parent = Data.Bones[I].Parent;
                if (Parent < 0 || !Upper[I]) continue;
                // The pivot keeps Top's orientation relative to the hips' heading (the body turns with the legs).
                Out.World[I] = NormalizeQ(Mul(Out.World[static_cast<std::size_t>(Parent)], LocalOf(Top, I)));
            }
            RecomputeHeads(Data, B, Out);
            return Out;
        }
    }

    bool BuildMotionPoses(const Source& Data, const Body& B, const Motion& Clip, const Motion* Upper, const MotionSpec& Spec, std::vector<Pose>& Out, std::string& Problem,
        double* GroundSpeed)
    {
        const double Length = static_cast<double>(Clip.Frames.size() - 1) * Clip.FrameTime;
        if (!(Spec.From >= Clip.FrameTime && Spec.To > Spec.From && Spec.To <= Length + 1e-9))
        {
            Problem = Spec.Name + ": cut outside clip " + Spec.Clip;
            return false;
        }
        if (Spec.Contact >= 0.0 && !(Spec.Contact > Spec.From && Spec.Contact < Spec.To && Spec.ContactAt > 0.0 && Spec.End > Spec.ContactAt))
        {
            Problem = Spec.Name + ": contact outside the cut";
            return false;
        }
        if (Spec.Loop && Spec.From - Spec.Blend * Spec.Speed < Clip.FrameTime)
        {
            Problem = Spec.Name + ": loop crossfade needs " + std::to_string(Spec.Blend * Spec.Speed) + " s before the cut";
            return false;
        }
        const double Fps = 30.0;
        const double Duration = Spec.Duration();
        const int Frames = std::max(2, static_cast<int>(Duration * Fps + 0.5) + 1);
        const int Reference = std::max(1, static_cast<int>(Spec.From / Clip.FrameTime + 0.5));
        const int Pivot = Data.FindBone("spine02");

        Pose Guard;
        if (Upper)
        {
            const int At = std::max(1, static_cast<int>(Spec.UpperAt / Upper->FrameTime + 0.5));
            Guard = PoseFromMotion(Data, B, *Upper, At, At, Spec.Upright, Spec.Curl);
        }
        const auto SourceTime = [&](const double T)
        {
            if (Spec.Contact < 0.0) return Spec.From + T * Spec.Speed;
            if (T <= Spec.ContactAt) return Spec.From + (Spec.Contact - Spec.From) * (T / Spec.ContactAt);
            return Spec.Contact + (Spec.To - Spec.Contact) * ((T - Spec.ContactAt) / (Spec.End - Spec.ContactAt));
        };
        const int Root = [&]
        {
            for (std::size_t I = 0; I < Data.Bones.size(); ++I)
            {
                if (Data.Bones[I].Parent < 0) return static_cast<int>(I);
            }
            return 0;
        }();
        const auto R = static_cast<std::size_t>(Root);

        // Heading turn over the cut (about the vertical), unwound progressively when straightening.
        double Turn = 0.0;
        V3 Pivot0;
        if (Spec.Straighten)
        {
            const Pose A = Sample(Data, B, Clip, SourceTime(0.0), Reference, Spec);
            const Pose C = Sample(Data, B, Clip, SourceTime(Duration), Reference, Spec);
            const V3 Fa = Rotate(A.World[R], V3{0, 0, 1}), Fc = Rotate(C.World[R], V3{0, 0, 1});
            Turn = Atan2(Fc.X, Fc.Z) - Atan2(Fa.X, Fa.Z);
            if (Turn > Pi) Turn -= 2.0 * Pi;
            if (Turn < -Pi) Turn += 2.0 * Pi;
            Pivot0 = A.Head[R];
        }

        if (GroundSpeed)
        {
            // body units are decimetres (MakeHuman); the writer scales by 0.1
            const Pose A = Sample(Data, B, Clip, SourceTime(0.0), Reference, Spec);
            const Pose C = Sample(Data, B, Clip, SourceTime(Duration), Reference, Spec);
            const double Dx = C.Head[R].X - A.Head[R].X, Dz = C.Head[R].Z - A.Head[R].Z;
            *GroundSpeed = std::sqrt(Dx * Dx + Dz * Dz) * 0.1 / Duration;
        }

        Out.clear();
        for (int K = 0; K < Frames; ++K)
        {
            const double T = Duration * static_cast<double>(K) / static_cast<double>(Frames - 1);
            Pose P = Sample(Data, B, Clip, SourceTime(T), Reference, Spec);
            if (Spec.Loop && T > Duration - Spec.Blend)
            {
                // Crossfade into the frames just before the cut, which lead into its first frame.
                const double W = (T - (Duration - Spec.Blend)) / Spec.Blend;
                const double Smooth = W * W * (3.0 - 2.0 * W);
                const Pose Lead = Sample(Data, B, Clip, Spec.From - (Duration - T) * Spec.Speed, Reference, Spec);
                P = Blend(Data, B, P, Lead, Smooth);
            }
            if (Spec.Straighten)
            {
                const Quat Unwind = AxisAngle(V3{0, 1, 0}, -Turn * T / Duration);
                for (Quat& Q : P.World) Q = NormalizeQ(Mul(Unwind, Q));
                const V3 Off = P.Head[R] - V3{Pivot0.X, 0.0, Pivot0.Z};
                P.Head[R] = V3{Pivot0.X, 0.0, Pivot0.Z} + Rotate(Unwind, Off);
                RecomputeHeads(Data, B, P);
            }
            if (Upper) P = Layered(Data, B, P, Guard, Pivot);
            if (Spec.Mirror) P = Mirrored(Data, B, P);
            if (Spec.Loop || Spec.InPlace)
            {
                P.Head[R].X = B.Head[R].X;
                P.Head[R].Z = B.Head[R].Z;
                RecomputeHeads(Data, B, P);
            }
            Out.push_back(std::move(P));
        }
        return true;
    }
}
