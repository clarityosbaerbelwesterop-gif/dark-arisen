#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

namespace DarkArisen::Core
{
    using Point3 = std::array<double, 3>;  // world metres, Z up; yaw 0 faces +Y (O3DE forward)

    /**
     * Third-person camera tuning (Unreal AJakeCharacter + engine defaults): spring arm 360 cm with
     * socket offset (0, 45, 70) cm, controller-driven rotation, gamepad 45 deg/s x input scale 2.5,
     * mouse 0.07 x 2.5 deg per pixel, view pitch limits +-89.9 deg, lock-on interpolation speed 12.
     */
    struct CameraRigTuning
    {
        double ArmLength = 3.6;
        double ShoulderRight = 0.45;
        double ShoulderUp = 0.70;
        double PivotHeight = 0.96;  // capsule centre above the controller base (half-height 96 cm)
        double MouseDegreesPerPixel = 0.07 * 2.5;
        double StickDegreesPerSecond = 45.0 * 2.5;
        double PitchMin = -89.9;
        double PitchMax = 89.9;
        double LockOnInterpSpeed = 12.0;
        double LockOnAimHeight = 0.55;
        /** Clearance kept from geometry when the arm is shortened (Unreal probe size 12 cm). */
        double CollisionProbeRadius = 0.12;
    };

    struct CameraPose
    {
        Point3 Position{};
        double YawDegrees = 0.0;
        double PitchDegrees = 0.0;  // positive looks up
    };

    /**
     * Controller rotation plus a spring arm. Free look takes mouse and stick input; while locked on,
     * the rotation eases towards the target (RInterpTo semantics) and look input is ignored.
     */
    class CameraRig
    {
    public:
        CameraRigTuning Tuning;

        void AddMouseLook(double DeltaXPixels, double DeltaYPixels);
        void AddStickLook(double RightX, double RightY, double DeltaSeconds);
        void SetRotation(double YawDegrees, double PitchDegrees);

        /**
         * Updates the rotation (towards LockTarget when set) and returns the camera pose for a player
         * standing at PlayerBase. ArmClearance, when set, is the free distance along the arm (from a
         * sweep); the arm shortens to stay out of geometry.
         */
        CameraPose Update(const Point3& PlayerBase, const std::optional<Point3>& LockTarget, double DeltaSeconds,
            std::optional<double> ArmClearance = std::nullopt);

        double GetYaw() const { return Yaw; }
        double GetPitch() const { return Pitch; }
        /** Unit forward and right on the ground plane for camera-relative movement. */
        std::array<double, 2> Forward() const;
        std::array<double, 2> Right() const;

    private:
        double Yaw = 0.0;
        double Pitch = -10.0;
        bool LookLocked = false;
    };

    /** Lock-on tuning (Unreal ULockOnComponent). */
    struct LockOnTuning
    {
        double AcquisitionRange = 20.0;
        double RetentionRange = 25.0;
        double MinimumFacingDot = 0.35;
    };

    struct LockOnCandidate
    {
        std::string CombatantId;
        Point3 Position{};
        bool Alive = true;
        bool Targetable = true;
        bool LineOfSight = true;
    };

    /**
     * Best target in front of the player: distance squared plus a facing penalty, the Unreal score.
     * Canon-protected ids (real Ethan, children) are never selectable. Returns -1 when none qualifies.
     */
    int SelectLockTarget(const Point3& Player, const std::array<double, 2>& Facing, const std::vector<LockOnCandidate>& Candidates,
        const LockOnTuning& Tuning = {});

    /** Whether a held target stays locked: alive, targetable, in sight and within the retention range. */
    bool RetainLockTarget(const Point3& Player, const LockOnCandidate& Target, const LockOnTuning& Tuning = {});
}
