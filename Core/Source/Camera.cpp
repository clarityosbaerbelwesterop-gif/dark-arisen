#include "DarkArisen/Core/Camera.h"

#include "DarkArisen/Core/EntityPolicy.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace DarkArisen::Core
{
    namespace
    {
        constexpr double Pi = 3.14159265358979323846;
        constexpr double Radians = Pi / 180.0;

        double NormalizeDegrees(double Angle)
        {
            Angle = std::fmod(Angle + 180.0, 360.0);
            return (Angle < 0.0 ? Angle + 360.0 : Angle) - 180.0;
        }

        /** Unreal FMath::RInterpTo per axis: move a share of the shortest delta, snapping when close. */
        double InterpAngle(const double Current, const double Target, const double DeltaSeconds, const double Speed)
        {
            if (Speed <= 0.0) return Target;
            const double Delta = NormalizeDegrees(Target - Current);
            if (std::fabs(Delta) < 1e-4) return Target;
            const double Step = Delta * std::clamp(DeltaSeconds * Speed, 0.0, 1.0);
            return NormalizeDegrees(Current + Step);
        }

        bool Finite(const Point3& Point)
        {
            return std::isfinite(Point[0]) && std::isfinite(Point[1]) && std::isfinite(Point[2]);
        }
    }

    void CameraRig::AddMouseLook(const double DeltaXPixels, const double DeltaYPixels)
    {
        if (LookLocked || !std::isfinite(DeltaXPixels) || !std::isfinite(DeltaYPixels)) return;
        // Screen Y grows downwards: moving the mouse up looks up.
        SetRotation(Yaw + DeltaXPixels * Tuning.MouseDegreesPerPixel, Pitch - DeltaYPixels * Tuning.MouseDegreesPerPixel);
    }

    void CameraRig::AddStickLook(const double RightX, const double RightY, const double DeltaSeconds)
    {
        if (LookLocked || !std::isfinite(RightX) || !std::isfinite(RightY) || !(DeltaSeconds > 0.0)) return;
        const double Rate = Tuning.StickDegreesPerSecond * DeltaSeconds;
        SetRotation(Yaw + std::clamp(RightX, -1.0, 1.0) * Rate, Pitch + std::clamp(RightY, -1.0, 1.0) * Rate);
    }

    void CameraRig::SetRotation(const double YawDegrees, const double PitchDegrees)
    {
        if (!std::isfinite(YawDegrees) || !std::isfinite(PitchDegrees)) return;
        Yaw = NormalizeDegrees(YawDegrees);
        Pitch = std::clamp(PitchDegrees, Tuning.PitchMin, Tuning.PitchMax);
    }

    std::array<double, 2> CameraRig::Forward() const
    {
        // Yaw 0 faces +Y; positive yaw turns to the right (clockwise seen from above).
        return {std::sin(Yaw * Radians), std::cos(Yaw * Radians)};
    }

    std::array<double, 2> CameraRig::Right() const
    {
        return {std::cos(Yaw * Radians), -std::sin(Yaw * Radians)};
    }

    CameraPose CameraRig::Update(const Point3& PlayerBase, const std::optional<Point3>& LockTarget, const double DeltaSeconds,
        const std::optional<double> ArmClearance)
    {
        const Point3 Pivot{PlayerBase[0], PlayerBase[1], PlayerBase[2] + Tuning.PivotHeight};
        LookLocked = LockTarget.has_value() && Finite(*LockTarget);
        if (LookLocked && DeltaSeconds > 0.0)
        {
            const Point3 Aim{(*LockTarget)[0], (*LockTarget)[1], (*LockTarget)[2] + Tuning.LockOnAimHeight};
            const double Dx = Aim[0] - Pivot[0];
            const double Dy = Aim[1] - Pivot[1];
            const double Dz = Aim[2] - Pivot[2];
            const double Flat = std::hypot(Dx, Dy);
            if (Flat > 1e-6 || std::fabs(Dz) > 1e-6)
            {
                const double TargetYaw = std::atan2(Dx, Dy) / Radians;
                const double TargetPitch = std::atan2(Dz, Flat) / Radians;
                Yaw = InterpAngle(Yaw, TargetYaw, DeltaSeconds, Tuning.LockOnInterpSpeed);
                Pitch = std::clamp(InterpAngle(Pitch, TargetPitch, DeltaSeconds, Tuning.LockOnInterpSpeed), Tuning.PitchMin, Tuning.PitchMax);
            }
        }
        const double CosPitch = std::cos(Pitch * Radians);
        const std::array<double, 3> View{std::sin(Yaw * Radians) * CosPitch, std::cos(Yaw * Radians) * CosPitch, std::sin(Pitch * Radians)};
        const std::array<double, 2> Side = Right();
        double Arm = Tuning.ArmLength;
        if (ArmClearance && std::isfinite(*ArmClearance))
        {
            Arm = std::clamp(*ArmClearance - Tuning.CollisionProbeRadius, 0.0, Tuning.ArmLength);
        }
        CameraPose Pose;
        Pose.YawDegrees = Yaw;
        Pose.PitchDegrees = Pitch;
        Pose.Position = {Pivot[0] - View[0] * Arm + Side[0] * Tuning.ShoulderRight, Pivot[1] - View[1] * Arm + Side[1] * Tuning.ShoulderRight,
            Pivot[2] - View[2] * Arm + Tuning.ShoulderUp};
        return Pose;
    }

    namespace
    {
        bool Eligible(const Point3& Player, const LockOnCandidate& Candidate, const double Range)
        {
            if (!Candidate.Alive || !Candidate.Targetable || !Candidate.LineOfSight || !Finite(Candidate.Position) ||
                !EntityPolicy::CanBeHostileTarget(Candidate.CombatantId))
            {
                return false;
            }
            const double Dx = Candidate.Position[0] - Player[0];
            const double Dy = Candidate.Position[1] - Player[1];
            const double Dz = Candidate.Position[2] - Player[2];
            return Dx * Dx + Dy * Dy + Dz * Dz <= Range * Range;
        }
    }

    int SelectLockTarget(const Point3& Player, const std::array<double, 2>& Facing, const std::vector<LockOnCandidate>& Candidates,
        const LockOnTuning& Tuning)
    {
        const double FacingLength = std::hypot(Facing[0], Facing[1]);
        if (!Finite(Player) || !(FacingLength > 1e-9))
        {
            return -1;
        }
        const double Fx = Facing[0] / FacingLength;
        const double Fy = Facing[1] / FacingLength;
        int Best = -1;
        double BestScore = std::numeric_limits<double>::max();
        for (std::size_t Index = 0; Index < Candidates.size(); ++Index)
        {
            const LockOnCandidate& Candidate = Candidates[Index];
            if (!Eligible(Player, Candidate, Tuning.AcquisitionRange))
            {
                continue;
            }
            const double Dx = Candidate.Position[0] - Player[0];
            const double Dy = Candidate.Position[1] - Player[1];
            const double Dz = Candidate.Position[2] - Player[2];
            const double Distance = std::sqrt(Dx * Dx + Dy * Dy + Dz * Dz);
            const double Dot = Distance > 1e-9 ? (Fx * Dx + Fy * Dy) / Distance : 1.0;
            if (Dot < Tuning.MinimumFacingDot)
            {
                continue;
            }
            const double Score = Distance * Distance + (1.0 - Dot) * Tuning.AcquisitionRange * Tuning.AcquisitionRange;
            if (Score < BestScore)
            {
                BestScore = Score;
                Best = static_cast<int>(Index);
            }
        }
        return Best;
    }

    bool RetainLockTarget(const Point3& Player, const LockOnCandidate& Target, const LockOnTuning& Tuning)
    {
        return Finite(Player) && Eligible(Player, Target, std::max(Tuning.AcquisitionRange, Tuning.RetentionRange));
    }
}
