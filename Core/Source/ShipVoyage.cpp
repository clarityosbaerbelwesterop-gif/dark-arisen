#include "DarkArisen/Core/ShipVoyage.h"

#include "DarkArisen/Core/SaveRules.h"

#include <algorithm>
#include <cmath>

namespace DarkArisen::Core
{
    ShipVoyage::ShipVoyage()
    {
        Snapshot.Valid = true;
        // crew_system.md canonical household, in UShipVoyageComponent::BuildCanonicalCrew order.
        for (const char* Id : {"crew.mira", "crew.big_tom", "crew.esteban", "crew.ines", "crew.father_salvio"})
        {
            Snapshot.NamedCrew.push_back({Id, true, true});
        }
        RecalculatePointOfSail();
    }

    float ShipVoyage::NormalizeHeading(const float Degrees)
    {
        float Result = std::fmod(Degrees, 360.0f);
        if (Result < 0.0f)
        {
            Result += 360.0f;
        }
        return Result;
    }

    float ShipVoyage::DeltaAngleDegrees(const float From, const float To)
    {
        float Delta = To - From;
        if (Delta > 180.0f) Delta -= 360.0f;
        else if (Delta < -180.0f) Delta += 360.0f;
        return Delta;
    }

    bool ShipVoyage::SetOwnedAndUnlocked()
    {
        Snapshot.OwnedAndUnlocked = true;
        return true;
    }

    bool ShipVoyage::Embark(const std::string_view CharacterId)
    {
        if (!Snapshot.OwnedAndUnlocked || CharacterId.empty())
        {
            return false;
        }
        if (!IsAboard(CharacterId))
        {
            Embarked.emplace_back(CharacterId);
        }
        return true;
    }

    bool ShipVoyage::Disembark(const std::string_view CharacterId)
    {
        // Jake must physically leave the wheel before stepping ashore.
        if (CharacterId.empty() || JakeAtHelm)
        {
            return false;
        }
        const auto Found = std::find(Embarked.begin(), Embarked.end(), CharacterId);
        if (Found == Embarked.end())
        {
            return false;
        }
        Embarked.erase(Found);
        return true;
    }

    bool ShipVoyage::IsAboard(const std::string_view CharacterId) const
    {
        return std::find(Embarked.begin(), Embarked.end(), CharacterId) != Embarked.end();
    }

    bool ShipVoyage::SetThrottle(const float NewThrottle)
    {
        if (!Snapshot.OwnedAndUnlocked || !JakeAtHelm || !std::isfinite(NewThrottle))
        {
            return false;
        }
        Snapshot.Throttle = std::clamp(NewThrottle, 0.0f, 1.0f);
        return true;
    }

    void ShipVoyage::SetHelmCommandDegrees(const float Degrees)
    {
        if (std::isfinite(Degrees))
        {
            Snapshot.CommandedHeadingDegrees = NormalizeHeading(Degrees);
        }
    }

    void ShipVoyage::SetWind(const float DirectionDegrees, const float StrengthMetresPerSecond)
    {
        if (!std::isfinite(DirectionDegrees) || !std::isfinite(StrengthMetresPerSecond))
        {
            return;
        }
        Snapshot.WindDirectionDegrees = NormalizeHeading(DirectionDegrees);
        Snapshot.WindStrengthMetresPerSecond = std::max(0.0f, StrengthMetresPerSecond);
        RecalculatePointOfSail();
    }

    void ShipVoyage::ApplyHullDamage(const float Damage)
    {
        if (Damage > 0.0f && std::isfinite(Damage))
        {
            Snapshot.HullIntegrity = std::max(0.0f, Snapshot.HullIntegrity - Damage);
        }
    }

    void ShipVoyage::SetActiveHands(const int Hands)
    {
        // Canon range is 40-90 once the household matures; zero stays legal for authored catastrophe.
        Snapshot.ActiveHands = std::clamp(Hands, 0, 90);
    }

    NamedCrewMember* ShipVoyage::FindCrew(const std::string_view StableId)
    {
        const auto Found = std::find_if(Snapshot.NamedCrew.begin(), Snapshot.NamedCrew.end(),
            [StableId](const NamedCrewMember& Member) { return Member.StableId == StableId; });
        return Found == Snapshot.NamedCrew.end() ? nullptr : &*Found;
    }

    void ShipVoyage::SetCrewMemberAboard(const std::string_view StableId, const bool Aboard)
    {
        if (NamedCrewMember* Member = FindCrew(StableId))
        {
            Member->Aboard = Aboard && Member->Alive;
        }
    }

    void ShipVoyage::SetCrewMemberAlive(const std::string_view StableId, const bool Alive)
    {
        if (NamedCrewMember* Member = FindCrew(StableId))
        {
            Member->Alive = Alive;
            Member->Aboard = Member->Aboard && Alive;
        }
    }

    bool ShipVoyage::AcquirePhysicalChart(const std::string_view ChartId)
    {
        if (ChartId.empty())
        {
            return false;
        }
        for (PhysicalChart& Chart : Snapshot.PhysicalCharts)
        {
            if (Chart.ChartId == ChartId)
            {
                Chart.Acquired = true;
                return true;
            }
        }
        Snapshot.PhysicalCharts.push_back({std::string(ChartId), true, false});
        return true;
    }

    bool ShipVoyage::AnnotatePhysicalChart(const std::string_view ChartId)
    {
        for (PhysicalChart& Chart : Snapshot.PhysicalCharts)
        {
            if (Chart.ChartId == ChartId && Chart.Acquired)
            {
                Chart.JakeHasAnnotated = true;
                return true;
            }
        }
        return false;
    }

    bool ShipVoyage::HasPhysicalChart(const std::string_view ChartId) const
    {
        return std::any_of(Snapshot.PhysicalCharts.begin(), Snapshot.PhysicalCharts.end(),
            [ChartId](const PhysicalChart& Chart) { return Chart.ChartId == ChartId && Chart.Acquired; });
    }

    void ShipVoyage::RecalculatePointOfSail()
    {
        const float Relative = std::abs(DeltaAngleDegrees(Snapshot.HeadingDegrees, Snapshot.WindDirectionDegrees));
        if (Relative < 35.0f) Sail = PointOfSail::InIrons;
        else if (Relative < 55.0f) Sail = PointOfSail::CloseHauled;
        else if (Relative < 80.0f) Sail = PointOfSail::CloseReach;
        else if (Relative < 110.0f) Sail = PointOfSail::BeamReach;
        else if (Relative < 150.0f) Sail = PointOfSail::BroadReach;
        else Sail = PointOfSail::Running;
    }

    float ShipVoyage::GetSailEfficiency() const
    {
        switch (Sail)
        {
        case PointOfSail::InIrons: return 0.0f;
        case PointOfSail::CloseHauled: return 0.35f;
        case PointOfSail::CloseReach: return 0.62f;
        case PointOfSail::BeamReach: return 0.88f;
        case PointOfSail::BroadReach: return 1.0f;
        case PointOfSail::Running: return 0.82f;
        }
        return 0.0f;
    }

    float ShipVoyage::GetCrewHandlingFactor() const
    {
        // First mate affects manoeuvre response; if Mira is lost a hand steps up badly.
        const auto Mira = std::find_if(Snapshot.NamedCrew.begin(), Snapshot.NamedCrew.end(),
            [](const NamedCrewMember& Member) { return Member.StableId == "crew.mira"; });
        const bool MiraPresent = Mira != Snapshot.NamedCrew.end() && Mira->Alive && Mira->Aboard;
        const float FirstMateFactor = MiraPresent ? 1.0f : 0.6f;
        const float HandsFactor = std::clamp(static_cast<float>(Snapshot.ActiveHands) / 40.0f, 0.15f, 1.0f);
        return std::clamp(FirstMateFactor * 0.35f + HandsFactor * 0.65f, 0.15f, 1.0f);
    }

    float ShipVoyage::Tick(const float DeltaSeconds)
    {
        if (DeltaSeconds <= 0.0f || !std::isfinite(DeltaSeconds))
        {
            return 0.0f;
        }
        // Leaving the wheel does not stop the vessel: the last commanded course is held.
        const float Handling = GetCrewHandlingFactor();
        const float HeadingDelta = DeltaAngleDegrees(Snapshot.HeadingDegrees, Snapshot.CommandedHeadingDegrees);
        const float MaximumStep = HeadingResponseDegreesPerSecond * Handling * DeltaSeconds;
        Snapshot.HeadingDegrees = NormalizeHeading(Snapshot.HeadingDegrees + std::clamp(HeadingDelta, -MaximumStep, MaximumStep));
        RecalculatePointOfSail();

        const float WindDrive = std::max(0.0f, Snapshot.WindStrengthMetresPerSecond) * GetSailEfficiency();
        const float Desired = std::min(MaximumSpeedMetresPerSecond, WindDrive) * Handling * Snapshot.Throttle;
        // FMath::FInterpTo with interp speed 0.75.
        const float Alpha = std::clamp(DeltaSeconds * 0.75f, 0.0f, 1.0f);
        Snapshot.ForwardSpeedMetresPerSecond += (Desired - Snapshot.ForwardSpeedMetresPerSecond) * Alpha;
        return Snapshot.ForwardSpeedMetresPerSecond * DeltaSeconds;
    }

    ShipVoyageSnapshot ShipVoyage::Capture(const std::string_view SourceLevel, const Transform& WorldTransform) const
    {
        ShipVoyageSnapshot Result = Snapshot;
        Result.Valid = true;
        Result.SourceLevel = std::string(SourceLevel);
        Result.WorldTransform = WorldTransform;
        return Result;
    }

    bool ShipVoyage::Restore(const ShipVoyageSnapshot& Saved, const std::string_view CurrentLevel)
    {
        if (!Saved.Valid)
        {
            return false;
        }
        Snapshot = Saved;
        Snapshot.HeadingDegrees = NormalizeHeading(Snapshot.HeadingDegrees);
        Snapshot.CommandedHeadingDegrees = NormalizeHeading(Snapshot.CommandedHeadingDegrees);
        Snapshot.WindDirectionDegrees = NormalizeHeading(Snapshot.WindDirectionDegrees);
        RecalculatePointOfSail();
        return ShouldRestoreMapLocalTransform(Saved.SourceLevel, CurrentLevel);
    }
}
