#include "TestHarness.h"

#include "DarkArisen/Core/Camera.h"

#include <cmath>

using namespace DarkArisen::Core;

TEST_CASE("Camera: spring arm sits behind and over the right shoulder")
{
    CameraRig Rig;
    Rig.SetRotation(0.0, 0.0);
    const CameraPose Pose = Rig.Update({0.0, 0.0, 0.0}, std::nullopt, 1.0 / 60.0);
    CHECK_NEAR(Pose.Position[0], 0.45, 1e-9);          // right shoulder (+X when facing +Y)
    CHECK_NEAR(Pose.Position[1], -3.6, 1e-9);          // 360 cm behind
    CHECK_NEAR(Pose.Position[2], 0.96 + 0.70, 1e-9);   // capsule centre + socket height
}

TEST_CASE("Camera: movement basis follows the camera yaw")
{
    CameraRig Rig;
    Rig.SetRotation(90.0, 0.0);
    CHECK_NEAR(Rig.Forward()[0], 1.0, 1e-9);
    CHECK_NEAR(Rig.Forward()[1], 0.0, 1e-9);
    CHECK_NEAR(Rig.Right()[0], 0.0, 1e-9);
    CHECK_NEAR(Rig.Right()[1], -1.0, 1e-9);
}

TEST_CASE("Camera: mouse and stick look use the Unreal rates and pitch limits")
{
    CameraRig Rig;
    Rig.SetRotation(0.0, 0.0);
    Rig.AddMouseLook(100.0, 0.0);
    CHECK_NEAR(Rig.GetYaw(), 17.5, 1e-9);            // 0.07 x 2.5 deg per pixel
    Rig.AddMouseLook(0.0, 40.0);
    CHECK_NEAR(Rig.GetPitch(), -7.0, 1e-9);          // mouse down looks down
    Rig.AddStickLook(1.0, 0.0, 1.0);
    CHECK_NEAR(Rig.GetYaw(), 17.5 + 112.5, 1e-9);    // 45 deg/s x 2.5
    Rig.AddMouseLook(0.0, -100000.0);
    CHECK_NEAR(Rig.GetPitch(), 89.9, 1e-9);
    Rig.AddStickLook(2.0, 0.0, 1.0);                 // stick input is clamped to [-1, 1]
    CHECK_NEAR(Rig.GetYaw(), -180.0 + (130.0 + 112.5 - 180.0), 1e-9);
}

TEST_CASE("Camera: lock-on eases towards the target and ignores look input")
{
    CameraRig Rig;
    Rig.SetRotation(0.0, 0.0);
    const Point3 Target{10.0, 0.0, 0.41};  // pivot height 0.96 = target 0.41 + aim 0.55: level aim
    for (int Frame = 0; Frame < 120; ++Frame)
    {
        Rig.Update({0.0, 0.0, 0.0}, Target, 1.0 / 60.0);
        Rig.AddMouseLook(500.0, 500.0);
    }
    CHECK_NEAR(Rig.GetYaw(), 90.0, 1e-3);
    CHECK_NEAR(Rig.GetPitch(), 0.0, 1e-3);
}

TEST_CASE("Camera: the arm shortens against geometry")
{
    CameraRig Rig;
    Rig.SetRotation(0.0, 0.0);
    const CameraPose Pose = Rig.Update({0.0, 0.0, 0.0}, std::nullopt, 1.0 / 60.0, 1.12);
    CHECK_NEAR(Pose.Position[1], -1.0, 1e-9);  // clearance 1.12 m minus 12 cm probe radius
}

TEST_CASE("Lock-on: nearest target in front wins, protected ids never do")
{
    const std::vector<LockOnCandidate> Candidates = {
        {"enemy.boarder.a", {0.0, 12.0, 0.0}},
        {"enemy.boarder.b", {0.0, 6.0, 0.0}},
        {"character.ethan", {0.0, 2.0, 0.0}},          // real Ethan: never a target
        {"child.harbor.1", {0.5, 3.0, 0.0}},           // children: never a target
        {"enemy.boarder.c", {0.0, -3.0, 0.0}},         // behind the player
        {"enemy.boarder.d", {0.0, 4.0, 0.0}, false},   // dead
        {"enemy.boarder.e", {1.0, 5.0, 0.0}, true, true, false},  // no line of sight
        {"enemy.boarder.f", {0.0, 21.0, 0.0}},         // out of acquisition range
    };
    CHECK_EQ(SelectLockTarget({0.0, 0.0, 0.0}, {0.0, 1.0}, Candidates), 1);
    CHECK_EQ(SelectLockTarget({0.0, 0.0, 0.0}, {0.0, -1.0}, Candidates), 4);
    CHECK_EQ(SelectLockTarget({0.0, 0.0, 0.0}, {0.0, 0.0}, Candidates), -1);
    CHECK(RetainLockTarget({0.0, 0.0, 0.0}, {"enemy.boarder.f", {0.0, 24.0, 0.0}}));   // within 25 m retention
    CHECK(!RetainLockTarget({0.0, 0.0, 0.0}, {"enemy.boarder.f", {0.0, 26.0, 0.0}}));
    CHECK(!RetainLockTarget({0.0, 0.0, 0.0}, {"character.ethan", {0.0, 1.0, 0.0}}));
}
