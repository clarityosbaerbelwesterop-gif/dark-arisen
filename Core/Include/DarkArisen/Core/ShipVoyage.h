#pragma once

#include "DarkArisen/Core/CampaignState.h"

#include <cstdint>
#include <string_view>

namespace DarkArisen::Core
{
    enum class PointOfSail : std::uint8_t { InIrons, CloseHauled, CloseReach, BeamReach, BroadReach, Running };

    /**
     * La Liberacion's helm, wind and crew model. Port of UShipVoyageComponent.
     *
     * Canon: one continuously walkable four-deck ship; the ship keeps sailing when Jake
     * leaves the helm; navigation is coast/chart/sky based; there is no water fast travel.
     * The engine adapter moves the physical vessel by GetForwardSpeed() along
     * GetHeadingDegrees() every frame; this model never teleports or completes routes.
     */
    class ShipVoyage
    {
    public:
        static constexpr int RequiredDeckCount = 4;
        static constexpr float MaximumSpeedMetresPerSecond = 8.0f;
        static constexpr float HeadingResponseDegreesPerSecond = 8.0f;

        ShipVoyage();

        bool SetOwnedAndUnlocked();
        bool IsOwnedAndUnlocked() const { return Snapshot.OwnedAndUnlocked; }
        void SetJakeAtHelm(bool AtHelm) { JakeAtHelm = AtHelm && Snapshot.OwnedAndUnlocked; }
        bool IsJakeAtHelm() const { return JakeAtHelm; }
        bool Embark(std::string_view CharacterId);
        bool Disembark(std::string_view CharacterId);
        bool IsAboard(std::string_view CharacterId) const;

        bool SetThrottle(float NewThrottle);
        void SetHelmCommandDegrees(float Degrees);
        void SetWind(float DirectionDegrees, float StrengthMetresPerSecond);
        void ApplyHullDamage(float Damage);
        void SetActiveHands(int Hands);
        void SetCrewMemberAboard(std::string_view StableId, bool Aboard);
        void SetCrewMemberAlive(std::string_view StableId, bool Alive);
        bool AcquirePhysicalChart(std::string_view ChartId);
        bool AnnotatePhysicalChart(std::string_view ChartId);
        bool HasPhysicalChart(std::string_view ChartId) const;

        /** Advances heading and speed. Returns the metres travelled along the new heading. */
        float Tick(float DeltaSeconds);

        PointOfSail GetPointOfSail() const { return Sail; }
        float GetSailEfficiency() const;
        float GetCrewHandlingFactor() const;
        float GetHeadingDegrees() const { return Snapshot.HeadingDegrees; }
        float GetForwardSpeed() const { return Snapshot.ForwardSpeedMetresPerSecond; }
        float GetHullIntegrity() const { return Snapshot.HullIntegrity; }
        float GetThrottle() const { return Snapshot.Throttle; }

        /** Source level and transform are supplied by the adapter, which owns the actor. */
        ShipVoyageSnapshot Capture(std::string_view SourceLevel, const Transform& WorldTransform) const;
        /**
         * Restores voyage state. Returns true when the saved transform belongs to
         * CurrentLevel and must be applied to the vessel; otherwise the adapter keeps
         * the authored placement for this level.
         */
        bool Restore(const ShipVoyageSnapshot& Saved, std::string_view CurrentLevel);

        static float NormalizeHeading(float Degrees);
        static float DeltaAngleDegrees(float From, float To);

    private:
        ShipVoyageSnapshot Snapshot;
        PointOfSail Sail = PointOfSail::InIrons;
        bool JakeAtHelm = false;
        std::vector<std::string> Embarked;

        void RecalculatePointOfSail();
        NamedCrewMember* FindCrew(std::string_view StableId);
    };
}
