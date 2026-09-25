#pragma once

#include <array>
#include <cstddef>
#include <vector>

namespace DarkArisen::Core
{
    struct OceanSample
    {
        double Height = 0.0;                                 // metres above still water
        std::array<double, 3> Normal{0.0, 0.0, 1.0};        // unit surface normal
    };

    /** One Gerstner (trochoidal) wave. Directions are unit vectors in the XY plane, Z up. */
    struct GerstnerWave
    {
        double DirectionX = 1.0;
        double DirectionY = 0.0;
        double Amplitude = 0.0;     // metres
        double Wavelength = 1.0;    // metres
        double Steepness = 0.0;     // Q in [0, 1] before normalisation
        double Phase = 0.0;         // radians

        double WaveNumber() const;  // k = 2 pi / L
        double AngularSpeed() const; // deep-water dispersion: omega = sqrt(g k)
    };

    struct SeaState
    {
        double WindSpeedMetresPerSecond = 6.0;
        double WindDirectionDegrees = 90.0;  // direction the wind blows towards; 0 = +Y (north)
        /** 0 = calm harbour shelter, 1 = open sea. Coastlines and reefs reduce exposure. */
        double Exposure = 1.0;
    };

    /**
     * Deterministic ocean surface shared by gameplay (swimming, buoyancy, boarding) and the renderer.
     * The wave set is derived from wind with a Pierson-Moskowitz significant wave height
     * (Hs = 0.21 U^2 / g) and a fixed directional spread, so the same weather always produces the same
     * sea. The combined steepness is normalised to at most 1 so crests never loop over.
     * The Atom ocean shader must evaluate PackShaderConstants() with the same formula.
     */
    class OceanSurface
    {
    public:
        static constexpr double Gravity = 9.81;
        static constexpr std::size_t WaveCount = 8;

        explicit OceanSurface(const SeaState& State = {});
        void SetSeaState(const SeaState& State);
        const SeaState& GetSeaState() const { return Sea; }
        const std::vector<GerstnerWave>& Waves() const { return WaveSet; }

        /** Significant wave height the set was built for. */
        double SignificantWaveHeight() const { return TargetHs; }
        /** Sum of Q_i * k_i * A_i; <= 1 guarantees a single-valued, loop-free surface. */
        double CombinedSteepness() const;

        /** Displaced position of the still-water point (X, Y) at time T (x, y, z). */
        std::array<double, 3> Displace(double X, double Y, double T) const;
        /** Surface height and normal at the world position (X, Y), inverting the horizontal drift. */
        OceanSample Sample(double X, double Y, double T) const;

        /** 8 floats per wave: dirX, dirY, amplitude, k, omega, Q, phase, 0. */
        std::vector<float> PackShaderConstants() const;

    private:
        SeaState Sea;
        std::vector<GerstnerWave> WaveSet;
        double TargetHs = 0.0;
    };

    /** Rigid hull response to the ocean: heave, pitch and roll from sampled draught points. */
    struct HullPose
    {
        double HeaveMetres = 0.0;
        double PitchRadians = 0.0;  // bow up positive
        double RollRadians = 0.0;   // starboard down positive
    };

    struct HullShape
    {
        double LengthMetres = 38.0;   // La Liberacion placeholder hull (layout authority pending)
        double BeamMetres = 9.0;
        double MaximumPitchRadians = 0.35;
        double MaximumRollRadians = 0.45;
    };

    /**
     * Averages the surface over bow/stern/port/starboard sample points; a long hull filters short
     * waves naturally. Heading is degrees with 0 = +Y, matching ShipVoyage.
     */
    HullPose ComputeHullPose(const OceanSurface& Ocean, const HullShape& Hull, double CentreX, double CentreY,
        double HeadingDegrees, double T);
}
