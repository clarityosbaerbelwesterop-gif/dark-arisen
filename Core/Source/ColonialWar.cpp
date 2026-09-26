#include "DarkArisen/Core/ColonialWar.h"

#include <algorithm>

namespace DarkArisen::Core::ColonialWar
{
    namespace
    {
        int ClampControl(const int Value)
        {
            return std::clamp(Value, MinimumControl, MaximumControl);
        }

        bool IsValidAutonomousDelta(const int Delta)
        {
            return Delta >= -MaximumControl && Delta <= MaximumControl;
        }

        int& FactionControl(ColonialRegionState& Region, const ColonialFaction Faction)
        {
            switch (Faction)
            {
            case ColonialFaction::Albion: return Region.AlbionControl;
            case ColonialFaction::Liberation: return Region.LiberationStrength;
            case ColonialFaction::Crimson: return Region.CrimsonThreat;
            case ColonialFaction::Imperial: break;
            }
            return Region.ImperialControl;
        }

        /** UColonialWarStateSubsystem::ReevaluateRegion. */
        RegionalWarOutcome Evaluate(const ColonialRegionState& Region)
        {
            const int ColonialControl = std::max(Region.ImperialControl, Region.AlbionControl);
            if (ColonialControl < 30)
            {
                if (!Region.FallAssaultCompleted) return RegionalWarOutcome::Crisis;
                if (Region.LiberationStrength >= 50) return RegionalWarOutcome::Liberated;
                if (Region.CrimsonThreat >= 50) return RegionalWarOutcome::CrimsonOccupied;
                return RegionalWarOutcome::Vacuum;
            }
            return Region.ImperialControl >= Region.AlbionControl ? RegionalWarOutcome::ImperialControlled
                                                                  : RegionalWarOutcome::AlbionControlled;
        }

        ColonialRegionState* Find(ColonialWarState& War, const std::string_view RegionId)
        {
            const auto It = War.Regions.find(RegionId);
            return It == War.Regions.end() ? nullptr : &It->second;
        }
    }

    bool RegisterRegion(ColonialWarState& War, const int CurrentChapter, const std::string_view RegionId,
        const ColonialFaction InitialController)
    {
        if (RegionId.empty() || War.Regions.find(RegionId) != War.Regions.end())
        {
            return false;
        }
        ColonialRegionState State;
        switch (InitialController)
        {
        case ColonialFaction::Imperial:
            State.ImperialControl = 70;
            State.AlbionControl = 20;
            break;
        case ColonialFaction::Albion:
            State.ImperialControl = 20;
            State.AlbionControl = 70;
            break;
        case ColonialFaction::Liberation:
            State.LiberationStrength = 60;
            break;
        case ColonialFaction::Crimson:
            State.CrimsonThreat = 60;
            break;
        }
        State.Outcome = Evaluate(State);
        State.LastAutonomousTickChapter = CurrentChapter - 1;
        War.Regions.emplace(std::string(RegionId), State);
        return true;
    }

    bool RecordResolvedWarAction(ColonialWarState& War, const std::string_view RegionId, const WarActionVerb Verb,
        const ColonialFaction TargetFaction, const int ControlDelta, const int LiberationDelta, const int CrimsonDelta)
    {
        ColonialRegionState* Region = Find(War, RegionId);
        if (Region == nullptr || ControlDelta < 0 || LiberationDelta < 0 || CrimsonDelta < 0)
        {
            return false;
        }
        int& Target = FactionControl(*Region, TargetFaction);
        switch (Verb)
        {
        case WarActionVerb::Support:
            Target = ClampControl(Target + ControlDelta);
            break;
        case WarActionVerb::Break:
            Target = ClampControl(Target - ControlDelta);
            break;
        case WarActionVerb::Liberate:
            Region->LiberationStrength = ClampControl(Region->LiberationStrength + LiberationDelta);
            break;
        }
        if (Verb != WarActionVerb::Liberate && LiberationDelta > 0)
        {
            Region->LiberationStrength = ClampControl(Region->LiberationStrength + LiberationDelta);
        }
        Region->CrimsonThreat = ClampControl(Region->CrimsonThreat + CrimsonDelta);
        Region->Outcome = Evaluate(*Region);
        return true;
    }

    bool RecordAutonomousChapterTick(ColonialWarState& War, const int CurrentChapter, const std::string_view RegionId,
        const AutonomousWarTick& Tick)
    {
        ColonialRegionState* Region = Find(War, RegionId);
        if (Region == nullptr || Region->LastAutonomousTickChapter >= CurrentChapter ||
            !IsValidAutonomousDelta(Tick.ImperialDelta) || !IsValidAutonomousDelta(Tick.AlbionDelta) ||
            !IsValidAutonomousDelta(Tick.LiberationDelta) || !IsValidAutonomousDelta(Tick.CrimsonDelta))
        {
            return false;
        }
        Region->ImperialControl = ClampControl(Region->ImperialControl + Tick.ImperialDelta);
        Region->AlbionControl = ClampControl(Region->AlbionControl + Tick.AlbionDelta);
        Region->LiberationStrength = ClampControl(Region->LiberationStrength + Tick.LiberationDelta);
        Region->CrimsonThreat = ClampControl(Region->CrimsonThreat + Tick.CrimsonDelta);
        Region->LastAutonomousTickChapter = CurrentChapter;
        Region->Outcome = Evaluate(*Region);
        return true;
    }

    bool RecordFallAssaultCompleted(ColonialWarState& War, const std::string_view RegionId)
    {
        ColonialRegionState* Region = Find(War, RegionId);
        if (Region == nullptr)
        {
            return false;
        }
        Region->FallAssaultCompleted = true;
        Region->Outcome = Evaluate(*Region);
        return true;
    }

    bool ClaimHolding(ColonialWarState& War, const std::string_view HoldingId)
    {
        return !HoldingId.empty() && War.OwnedHoldings.emplace(HoldingId).second;
    }

    WarMomentumPhase MomentumPhase(const int Chapter)
    {
        if (Chapter <= 6) return WarMomentumPhase::Alpha;
        if (Chapter <= 8) return WarMomentumPhase::Beta;
        return WarMomentumPhase::Gamma;
    }

    RegionalWarSnapshot Snapshot(const ColonialWarState& War, const std::string_view RegionId)
    {
        RegionalWarSnapshot Result;
        const auto It = War.Regions.find(RegionId);
        if (It == War.Regions.end())
        {
            return Result;
        }
        const ColonialRegionState& Region = It->second;
        Result.RegionId = It->first;
        Result.Outcome = Region.Outcome;
        Result.Crisis = Region.Outcome == RegionalWarOutcome::Crisis;
        Result.FallAssaultCompleted = Region.FallAssaultCompleted;
        const int MaxControl = std::max({Region.ImperialControl, Region.AlbionControl, Region.LiberationStrength, Region.CrimsonThreat});
        if (MaxControl == Region.LiberationStrength) Result.DominantFaction = ColonialFaction::Liberation;
        else if (MaxControl == Region.CrimsonThreat) Result.DominantFaction = ColonialFaction::Crimson;
        else if (MaxControl == Region.AlbionControl) Result.DominantFaction = ColonialFaction::Albion;
        else Result.DominantFaction = ColonialFaction::Imperial;
        return Result;
    }

    bool IsRegionLiberated(const ColonialWarState& War, const std::string_view RegionId)
    {
        const auto It = War.Regions.find(RegionId);
        return It != War.Regions.end() && It->second.Outcome == RegionalWarOutcome::Liberated;
    }

    bool Validate(const ColonialWarState& War, std::vector<std::string>& OutErrors)
    {
        const std::size_t Before = OutErrors.size();
        const auto InRange = [](const int Value) { return Value >= MinimumControl && Value <= MaximumControl; };
        for (const auto& [RegionId, Region] : War.Regions)
        {
            if (RegionId.empty())
            {
                OutErrors.emplace_back("Colonial war region without an id.");
                continue;
            }
            if (!InRange(Region.ImperialControl) || !InRange(Region.AlbionControl) || !InRange(Region.LiberationStrength) ||
                !InRange(Region.CrimsonThreat))
            {
                OutErrors.push_back("Colonial war region " + RegionId + " has control outside 0..100.");
            }
            if (Region.Outcome != Evaluate(Region))
            {
                OutErrors.push_back("Colonial war region " + RegionId + " outcome contradicts its control state.");
            }
        }
        for (const std::string& Holding : War.OwnedHoldings)
        {
            if (Holding.empty()) OutErrors.emplace_back("Owned holding without an id.");
        }
        return OutErrors.size() == Before;
    }

    std::optional<ColonialFaction> ParseFaction(const std::string_view Name)
    {
        if (Name == "Imperial") return ColonialFaction::Imperial;
        if (Name == "Albion") return ColonialFaction::Albion;
        if (Name == "Liberation") return ColonialFaction::Liberation;
        if (Name == "Crimson" || Name == "CrimsonArmada") return ColonialFaction::Crimson;
        return std::nullopt;
    }

    std::optional<WarActionVerb> ParseVerb(const std::string_view Name)
    {
        if (Name == "Support") return WarActionVerb::Support;
        if (Name == "Break") return WarActionVerb::Break;
        if (Name == "Liberate") return WarActionVerb::Liberate;
        return std::nullopt;
    }
}
