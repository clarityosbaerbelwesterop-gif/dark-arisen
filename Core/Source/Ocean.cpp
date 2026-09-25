#include "DarkArisen/Core/Ocean.h"

#include <algorithm>
#include <cmath>

namespace DarkArisen::Core
{
    namespace
    {
        constexpr double Pi = 3.14159265358979323846;
        constexpr double DegreesToRadians = Pi / 180.0;
        // Relative wavelength, energy weight and direction offset (degrees) around the peak.
        constexpr double WavelengthFactor[OceanSurface::WaveCount] = {1.6, 1.3, 1.0, 0.8, 0.62, 0.48, 0.36, 0.26};
        constexpr double EnergyWeight[OceanSurface::WaveCount] = {0.60, 0.90, 1.00, 0.80, 0.55, 0.38, 0.25, 0.16};
        constexpr double DirectionOffset[OceanSurface::WaveCount] = {0.0, 15.0, -15.0, 30.0, -30.0, 45.0, -45.0, 20.0};
        constexpr double MinimumSwellHs = 0.05; // sheltered water still moves a little
    }

    double GerstnerWave::WaveNumber() const
    {
        return 2.0 * Pi / Wavelength;
    }

    double GerstnerWave::AngularSpeed() const
    {
        return std::sqrt(OceanSurface::Gravity * WaveNumber());
    }

    OceanSurface::OceanSurface(const SeaState& State)
    {
        SetSeaState(State);
    }

    void OceanSurface::SetSeaState(const SeaState& State)
    {
        Sea = State;
        const double Wind = std::clamp(std::isfinite(State.WindSpeedMetresPerSecond) ? State.WindSpeedMetresPerSecond : 0.0, 1.0, 40.0);
        const double Exposure = std::clamp(std::isfinite(State.Exposure) ? State.Exposure : 1.0, 0.0, 1.0);
        // Pierson-Moskowitz fully developed sea.
        TargetHs = std::max(MinimumSwellHs, 0.21 * Wind * Wind / Gravity * Exposure);
        const double PeakOmega = 0.877 * Gravity / Wind;
        const double PeakWavelength = 2.0 * Pi * Gravity / (PeakOmega * PeakOmega);

        // Hs = 4 sigma and sigma^2 = sum(A^2) / 2  =>  sqrt(sum(A^2)) = Hs / (2 sqrt 2).
        double WeightNorm = 0.0;
        for (const double Weight : EnergyWeight) WeightNorm += Weight * Weight;
        const double AmplitudeScale = TargetHs / (2.0 * std::sqrt(2.0)) / std::sqrt(WeightNorm);
        // Rougher water is choppier; the combined steepness stays below 1 (no looping crests).
        const double GlobalSteepness = 0.3 + 0.5 * std::clamp(Wind / 20.0, 0.0, 1.0);

        const double Heading = (std::isfinite(State.WindDirectionDegrees) ? State.WindDirectionDegrees : 0.0);
        WaveSet.assign(WaveCount, GerstnerWave{});
        for (std::size_t Index = 0; Index < WaveCount; ++Index)
        {
            GerstnerWave& Wave = WaveSet[Index];
            const double Angle = (Heading + DirectionOffset[Index]) * DegreesToRadians;
            Wave.DirectionX = std::sin(Angle);
            Wave.DirectionY = std::cos(Angle);
            Wave.Wavelength = std::max(0.5, PeakWavelength * WavelengthFactor[Index]);
            Wave.Amplitude = EnergyWeight[Index] * AmplitudeScale;
            Wave.Phase = static_cast<double>(Index) * 1.7;
            const double Slope = Wave.WaveNumber() * Wave.Amplitude;
            Wave.Steepness = Slope > 0.0 ? std::min(1.0, GlobalSteepness / (Slope * static_cast<double>(WaveCount))) : 0.0;
        }
    }

    double OceanSurface::CombinedSteepness() const
    {
        double Sum = 0.0;
        for (const GerstnerWave& Wave : WaveSet) Sum += Wave.Steepness * Wave.WaveNumber() * Wave.Amplitude;
        return Sum;
    }

    std::array<double, 3> OceanSurface::Displace(const double X, const double Y, const double T) const
    {
        std::array<double, 3> Result{X, Y, 0.0};
        for (const GerstnerWave& Wave : WaveSet)
        {
            const double Theta = Wave.WaveNumber() * (Wave.DirectionX * X + Wave.DirectionY * Y) - Wave.AngularSpeed() * T + Wave.Phase;
            const double Horizontal = Wave.Steepness * Wave.Amplitude * std::cos(Theta);
            Result[0] += Horizontal * Wave.DirectionX;
            Result[1] += Horizontal * Wave.DirectionY;
            Result[2] += Wave.Amplitude * std::sin(Theta);
        }
        return Result;
    }

    OceanSample OceanSurface::Sample(const double X, const double Y, const double T) const
    {
        // Find the still-water point whose displaced position lands on (X, Y). The map is a
        // contraction because the combined steepness is below 1, so fixed-point iteration converges.
        double StillX = X;
        double StillY = Y;
        for (int Iteration = 0; Iteration < 6; ++Iteration)
        {
            const std::array<double, 3> Moved = Displace(StillX, StillY, T);
            StillX += X - Moved[0];
            StillY += Y - Moved[1];
        }
        OceanSample Result;
        Result.Height = Displace(StillX, StillY, T)[2];

        double NormalX = 0.0;
        double NormalY = 0.0;
        double NormalZ = 1.0;
        for (const GerstnerWave& Wave : WaveSet)
        {
            const double K = Wave.WaveNumber();
            const double Theta = K * (Wave.DirectionX * StillX + Wave.DirectionY * StillY) - Wave.AngularSpeed() * T + Wave.Phase;
            const double KA = K * Wave.Amplitude;
            NormalX -= Wave.DirectionX * KA * std::cos(Theta);
            NormalY -= Wave.DirectionY * KA * std::cos(Theta);
            NormalZ -= Wave.Steepness * KA * std::sin(Theta);
        }
        const double Length = std::sqrt(NormalX * NormalX + NormalY * NormalY + NormalZ * NormalZ);
        Result.Normal = {NormalX / Length, NormalY / Length, NormalZ / Length};
        return Result;
    }

    std::vector<float> OceanSurface::PackShaderConstants() const
    {
        std::vector<float> Packed;
        Packed.reserve(WaveSet.size() * 8);
        for (const GerstnerWave& Wave : WaveSet)
        {
            for (const double Value : {Wave.DirectionX, Wave.DirectionY, Wave.Amplitude, Wave.WaveNumber(), Wave.AngularSpeed(),
                     Wave.Steepness, Wave.Phase, 0.0})
            {
                Packed.push_back(static_cast<float>(Value));
            }
        }
        return Packed;
    }

    HullPose ComputeHullPose(const OceanSurface& Ocean, const HullShape& Hull, const double CentreX, const double CentreY,
        const double HeadingDegrees, const double T)
    {
        const double Heading = HeadingDegrees * DegreesToRadians;
        const double ForwardX = std::sin(Heading);
        const double ForwardY = std::cos(Heading);
        const double StarboardX = std::cos(Heading);
        const double StarboardY = -std::sin(Heading);
        const double HalfLength = Hull.LengthMetres * 0.5;
        const double HalfBeam = Hull.BeamMetres * 0.5;
        const auto HeightAt = [&](const double Along, const double Across)
        {
            return Ocean.Sample(CentreX + ForwardX * Along + StarboardX * Across, CentreY + ForwardY * Along + StarboardY * Across, T).Height;
        };
        // Two stations per end/side average out waves shorter than the hull.
        const double Bow = 0.5 * (HeightAt(HalfLength, 0.0) + HeightAt(HalfLength * 0.6, 0.0));
        const double Stern = 0.5 * (HeightAt(-HalfLength, 0.0) + HeightAt(-HalfLength * 0.6, 0.0));
        const double Port = 0.5 * (HeightAt(HalfLength * 0.3, -HalfBeam) + HeightAt(-HalfLength * 0.3, -HalfBeam));
        const double Starboard = 0.5 * (HeightAt(HalfLength * 0.3, HalfBeam) + HeightAt(-HalfLength * 0.3, HalfBeam));
        const double Centre = HeightAt(0.0, 0.0);

        HullPose Pose;
        Pose.HeaveMetres = (Bow + Stern + Port + Starboard + Centre) / 5.0;
        Pose.PitchRadians = std::clamp(std::atan2(Bow - Stern, Hull.LengthMetres * 0.8), -Hull.MaximumPitchRadians, Hull.MaximumPitchRadians);
        Pose.RollRadians = std::clamp(std::atan2(Port - Starboard, Hull.BeamMetres), -Hull.MaximumRollRadians, Hull.MaximumRollRadians);
        return Pose;
    }
}
