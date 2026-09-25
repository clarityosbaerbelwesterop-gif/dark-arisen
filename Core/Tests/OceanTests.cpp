#include "TestHarness.h"

#include "DarkArisen/Core/Ocean.h"

#include <cmath>

using namespace DarkArisen::Core;

namespace
{
    SeaState Wind(const double Speed, const double Exposure = 1.0)
    {
        SeaState State;
        State.WindSpeedMetresPerSecond = Speed;
        State.WindDirectionDegrees = 60.0;
        State.Exposure = Exposure;
        return State;
    }

    /** Statistical significant wave height (4 sigma) from a grid of samples over time. */
    double MeasuredHs(const OceanSurface& Ocean)
    {
        double Sum = 0.0;
        double SumSquares = 0.0;
        int Count = 0;
        for (int Step = 0; Step < 40; ++Step)
        {
            for (int GridX = 0; GridX < 12; ++GridX)
            {
                for (int GridY = 0; GridY < 12; ++GridY)
                {
                    const double Height = Ocean.Displace(GridX * 17.3, GridY * 23.9, Step * 1.37)[2];
                    Sum += Height;
                    SumSquares += Height * Height;
                    ++Count;
                }
            }
        }
        const double Mean = Sum / Count;
        return 4.0 * std::sqrt(SumSquares / Count - Mean * Mean);
    }
}

TEST_CASE("Ocean: crests never loop at any wind speed")
{
    for (const double Speed : {0.0, 3.0, 8.0, 15.0, 25.0, 40.0})
    {
        const OceanSurface Ocean(Wind(Speed));
        CHECK(Ocean.CombinedSteepness() <= 1.0);
        CHECK(Ocean.CombinedSteepness() > 0.0);
    }
}

TEST_CASE("Ocean: significant wave height follows Pierson-Moskowitz")
{
    const OceanSurface Fresh(Wind(10.0));
    CHECK_NEAR(Fresh.SignificantWaveHeight(), 0.21 * 100.0 / 9.81, 1e-9);
    const double Measured = MeasuredHs(Fresh);
    CHECK(Measured > 0.7 * Fresh.SignificantWaveHeight());
    CHECK(Measured < 1.3 * Fresh.SignificantWaveHeight());
    const OceanSurface Gale(Wind(20.0));
    CHECK(MeasuredHs(Gale) > 3.0 * Measured);
}

TEST_CASE("Ocean: sheltered water is nearly flat")
{
    const OceanSurface Harbour(Wind(15.0, 0.0));
    for (int Step = 0; Step < 50; ++Step)
    {
        CHECK(std::abs(Harbour.Sample(Step * 3.1, Step * 1.7, Step * 0.4).Height) < 0.06);
    }
}

TEST_CASE("Ocean: sampling inverts the horizontal drift of the surface")
{
    const OceanSurface Ocean(Wind(14.0));
    for (int Step = 0; Step < 30; ++Step)
    {
        const double X = Step * 7.7 - 90.0;
        const double Y = Step * 3.3 + 12.0;
        const double T = Step * 0.9;
        const auto Moved = Ocean.Displace(X, Y, T);
        CHECK_NEAR(Ocean.Sample(Moved[0], Moved[1], T).Height, Moved[2], 0.01);
    }
}

TEST_CASE("Ocean: normals are unit length and point up")
{
    const OceanSurface Ocean(Wind(18.0));
    for (int Step = 0; Step < 30; ++Step)
    {
        const OceanSample Sample = Ocean.Sample(Step * 5.0, -Step * 2.0, Step * 0.3);
        const auto& N = Sample.Normal;
        CHECK_NEAR(N[0] * N[0] + N[1] * N[1] + N[2] * N[2], 1.0, 1e-9);
        CHECK(N[2] > 0.2);
    }
}

TEST_CASE("Ocean: identical weather produces the identical sea; shader constants match")
{
    const OceanSurface A(Wind(9.0));
    const OceanSurface B(Wind(9.0));
    CHECK_NEAR(A.Sample(123.0, -45.0, 17.0).Height, B.Sample(123.0, -45.0, 17.0).Height, 0.0);
    const std::vector<float> Packed = A.PackShaderConstants();
    CHECK_EQ(Packed.size(), OceanSurface::WaveCount * 8);
    CHECK_NEAR(Packed[2], A.Waves()[0].Amplitude, 1e-6);
    CHECK_NEAR(Packed[3], A.Waves()[0].WaveNumber(), 1e-5);
}

TEST_CASE("Ocean: hull pose is quiet in calm water and works in a storm")
{
    const HullShape Hull;
    const OceanSurface Calm(Wind(2.0, 0.0));
    const HullPose Quiet = ComputeHullPose(Calm, Hull, 0.0, 0.0, 30.0, 5.0);
    CHECK(std::abs(Quiet.PitchRadians) < 0.01 && std::abs(Quiet.RollRadians) < 0.01);

    const OceanSurface Storm(Wind(22.0));
    double MaximumPitch = 0.0;
    double MaximumRoll = 0.0;
    for (int Step = 0; Step < 120; ++Step)
    {
        const HullPose Pose = ComputeHullPose(Storm, Hull, 0.0, 0.0, 30.0, Step * 0.25);
        MaximumPitch = std::max(MaximumPitch, std::abs(Pose.PitchRadians));
        MaximumRoll = std::max(MaximumRoll, std::abs(Pose.RollRadians));
        CHECK(std::abs(Pose.PitchRadians) <= Hull.MaximumPitchRadians + 1e-12);
        CHECK(std::abs(Pose.RollRadians) <= Hull.MaximumRollRadians + 1e-12);
    }
    CHECK(MaximumPitch > 0.02);
    CHECK(MaximumRoll > 0.02);
}

TEST_CASE("Ocean: a long hull rides out short chop better than a skiff")
{
    const OceanSurface Chop(Wind(12.0));
    HullShape Skiff;
    Skiff.LengthMetres = 5.0;
    Skiff.BeamMetres = 1.8;
    double ShipPitch = 0.0;
    double SkiffPitch = 0.0;
    for (int Step = 0; Step < 200; ++Step)
    {
        ShipPitch += std::abs(ComputeHullPose(Chop, HullShape{}, 0.0, 0.0, 0.0, Step * 0.21).PitchRadians);
        SkiffPitch += std::abs(ComputeHullPose(Chop, Skiff, 0.0, 0.0, 0.0, Step * 0.21).PitchRadians);
    }
    CHECK(ShipPitch < SkiffPitch);
}
