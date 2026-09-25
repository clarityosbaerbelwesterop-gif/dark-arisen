#include "DarkArisen/Core/SaveRules.h"

#include "DarkArisen/Core/Facts.h"

#include <cmath>

namespace DarkArisen::Core
{
    namespace
    {
        bool IsUnitInterval(const float Value)
        {
            return std::isfinite(Value) && Value >= 0.0f && Value <= 1.0f;
        }

        void ValidateMissionProgression(const CampaignState& Candidate, std::vector<std::string>& Errors)
        {
            const auto& Catalog = MissionCatalog::Missions();
            if (Candidate.Missions.size() != Catalog.size())
            {
                Errors.emplace_back("Save mission count invalid.");
            }

            int FirstIncomplete = -1;
            for (std::size_t Index = 0; Index < Candidate.Missions.size(); ++Index)
            {
                const MissionRuntime& Runtime = Candidate.Missions[Index];
                if (Index >= Catalog.size() || Runtime.MissionId != Catalog[Index].Id)
                {
                    Errors.emplace_back("Save missions must match the canonical order and identities.");
                }
                switch (Runtime.State)
                {
                case MissionState::Completed:
                    if (FirstIncomplete != -1)
                    {
                        Errors.emplace_back("Completed mission appears after an incomplete predecessor.");
                    }
                    break;
                case MissionState::Locked:
                case MissionState::Available:
                case MissionState::Active:
                case MissionState::Failed:
                    if (FirstIncomplete == -1)
                    {
                        FirstIncomplete = static_cast<int>(Index);
                        if (Runtime.State == MissionState::Locked)
                        {
                            Errors.emplace_back("The next mission cannot be locked in a saved progression.");
                        }
                    }
                    else if (Runtime.State != MissionState::Locked)
                    {
                        Errors.emplace_back("Only the first incomplete mission may be available, active or failed.");
                    }
                    break;
                default:
                    Errors.emplace_back("Unknown saved mission state.");
                    break;
                }
            }

            const int CurrentIndex = FirstIncomplete == -1 ? MissionCatalog::MissionCount - 1 : FirstIncomplete;
            const MissionDefinition& Current = Catalog[static_cast<std::size_t>(CurrentIndex)];
            if (Candidate.CurrentMission != Current.Id || Candidate.CurrentChapter != Current.Chapter)
            {
                Errors.emplace_back("Current mission/chapter does not match the canonical progression.");
            }
        }

        void ValidateCanon(const CampaignState& Candidate, std::vector<std::string>& Errors)
        {
            const auto Story = [&Candidate](const std::string_view Fact)
            { return Candidate.StoryFacts.count(std::string(Fact)) != 0; };
            const auto World = [&Candidate](const std::string_view Fact)
            { return Candidate.WorldFacts.count(std::string(Fact)) != 0; };
            const auto Completed = [&Candidate](const std::string_view Mission)
            { return Candidate.GetMissionState(Mission) == MissionState::Completed; };

            for (const std::string& Fact : Candidate.StoryFacts)
            {
                if (!Facts::IsStoryFact(Fact))
                {
                    Errors.emplace_back("Story fact without the Story. namespace: " + Fact);
                }
            }
            for (const std::string& Fact : Candidate.WorldFacts)
            {
                if (Fact.empty())
                {
                    Errors.emplace_back("World fact has no stable id.");
                }
            }

            if (Story(Facts::EthanRecovered) && !Story(Facts::EthanAbducted))
            {
                Errors.emplace_back("Ethan cannot be recovered before abduction.");
            }
            // MAIN_STORY_AUTHORITY 3.7: Ethan is recovered physically in Chapter 8, not earlier.
            if (Story(Facts::EthanRecovered) && !Completed("Main.C08.03.HomewardBearing"))
            {
                Errors.emplace_back("Ethan recovery requires the Chapter 8 rescue.");
            }
            // MAIN_STORY_AUTHORITY 3.8: Rache only after Ethan is recovered.
            if (World(Facts::RacheUnlocked) && !Story(Facts::EthanRecovered))
            {
                Errors.emplace_back("Rache cannot unlock before Ethan is recovered.");
            }
            if (Story(Facts::DravenKilled) && Story(Facts::DravenCaptured))
            {
                Errors.emplace_back("Draven outcomes are mutually exclusive.");
            }
            if (World(Facts::LaLiberacionOwned) && Candidate.CurrentChapter < 2)
            {
                Errors.emplace_back("La Liberacion cannot be owned before chapter 2.");
            }
            if (Story(Facts::MainComplete) && !Completed("Main.C10.05.TheWakeAfter"))
            {
                Errors.emplace_back("Main completion requires the finale.");
            }
        }

        void ValidateSnapshots(const CampaignState& Candidate, std::vector<std::string>& Errors)
        {
            const WorldRulesSnapshot& Rules = Candidate.WorldRules;
            if (Rules.Valid && (Rules.TotalWorldMinutes < 0 || Rules.Chapter < 1 ||
                                   Rules.Chapter > MissionCatalog::ChapterCount))
            {
                Errors.emplace_back("Saved world clock/chapter is invalid.");
            }

            const PlayerRuntimeSnapshot& Player = Candidate.PlayerRuntime;
            if (Player.Valid &&
                (!Player.WorldTransform.IsFinite() || !std::isfinite(Player.HealthFraction) ||
                    Player.HealthFraction <= 0.0f || Player.HealthFraction > 1.0f ||
                    !IsUnitInterval(Player.StaminaFraction)))
            {
                Errors.emplace_back("Saved player transform or vitals are invalid.");
            }

            const ShipVoyageSnapshot& Ship = Candidate.LaLiberacionVoyage;
            if (Ship.Valid)
            {
                if (!Ship.WorldTransform.IsFinite() || !IsUnitInterval(Ship.Throttle) ||
                    !std::isfinite(Ship.HullIntegrity) || Ship.HullIntegrity < 0.0f ||
                    !std::isfinite(Ship.HeadingDegrees) || !std::isfinite(Ship.CommandedHeadingDegrees) ||
                    !std::isfinite(Ship.ForwardSpeedMetresPerSecond) || !std::isfinite(Ship.WindDirectionDegrees) ||
                    !std::isfinite(Ship.WindStrengthMetresPerSecond) || Ship.WindStrengthMetresPerSecond < 0.0f ||
                    Ship.ActiveHands < 0 || Ship.ActiveHands > 90)
                {
                    Errors.emplace_back("Saved ship voyage state is invalid.");
                }
                if (Ship.OwnedAndUnlocked && Candidate.WorldFacts.count(std::string(Facts::LaLiberacionOwned)) == 0)
                {
                    Errors.emplace_back("Saved ship is owned without the ownership fact.");
                }
                for (const NamedCrewMember& Member : Ship.NamedCrew)
                {
                    if (Member.StableId.empty() || (Member.Aboard && !Member.Alive))
                    {
                        Errors.emplace_back("Saved ship crew entry is invalid.");
                    }
                }
                for (const PhysicalChart& Chart : Ship.PhysicalCharts)
                {
                    if (Chart.ChartId.empty() || (Chart.JakeHasAnnotated && !Chart.Acquired))
                    {
                        Errors.emplace_back("Saved physical chart is invalid.");
                    }
                }
            }
        }

        void ValidateCollections(const CampaignState& Candidate, std::vector<std::string>& Errors)
        {
            for (const CrewRelationship& Crew : Candidate.Crew)
            {
                if (Crew.CharacterId.empty())
                {
                    Errors.emplace_back("Crew entry has no character id.");
                }
                if (Crew.Available && !Crew.Met)
                {
                    Errors.emplace_back("Crew cannot be available before being met.");
                }
                if (Crew.Recruited && (!Crew.Met || !Crew.Available || Crew.Role.empty()))
                {
                    Errors.emplace_back("Crew recruitment is missing an authored meeting, availability beat, or role.");
                }
                if (Crew.Aboard && !Crew.Recruited)
                {
                    Errors.emplace_back("Non-recruited crew cannot be aboard.");
                }
            }
            for (const std::string& Dungeon : Candidate.CompletedDungeons)
            {
                if (Candidate.DiscoveredDungeons.count(Dungeon) == 0)
                {
                    Errors.emplace_back("Completed dungeon was never discovered.");
                }
            }
            for (const std::string& Treasure : Candidate.RecoveredTreasures)
            {
                if (Treasure.empty())
                {
                    Errors.emplace_back("Recovered treasure has no stable id.");
                }
            }
            if (Candidate.RecoveredTreasures.size() > 9)
            {
                Errors.emplace_back("Recovered State Treasure count exceeds canonical nine.");
            }
            if (!IsOpeningProgressValid(Candidate.Opening))
            {
                Errors.emplace_back("Opening route state is invalid.");
            }
        }
    }

    bool IsOpeningProgressValid(const OpeningProgress& Progress)
    {
        if (Progress.Location > OpeningLocation::RexaHarbor || Progress.RaidState > OpeningRaidState::DravenAboard ||
            Progress.RecoveryState > WaterRecoveryState::Recovered)
        {
            return false;
        }
        if (Progress.BoardersRequired < 0 || Progress.BoardersDefeated < 0 ||
            Progress.BoardersDefeated > Progress.BoardersRequired)
        {
            return false;
        }
        if (Progress.BoardersRequired == 1)
        {
            return false;
        }
        if (Progress.BoardersRequired == 0 &&
            (!Progress.BoardingEncounterId.empty() || Progress.BoardersDefeated != 0 || Progress.DravenBoarded))
        {
            return false;
        }
        if (Progress.BoardersRequired >= 2 && Progress.BoardingEncounterId.empty())
        {
            return false;
        }
        if (Progress.DravenBoarded && Progress.BoardersDefeated < Progress.BoardersRequired)
        {
            return false;
        }
        if (Progress.LaLiberacionHelmSecured && !Progress.LaLiberacionBoarded)
        {
            return false;
        }
        if (Progress.LaLiberacionClearedHarbor && !Progress.LaLiberacionHelmSecured)
        {
            return false;
        }
        for (const std::string& Member : Progress.FamilyInteractions)
        {
            if (Member != "character.marc" && Member != "character.denise" && Member != "character.ethan")
            {
                return false;
            }
        }
        return true;
    }

    bool ValidateCampaignState(const CampaignState& Candidate, std::vector<std::string>& OutErrors)
    {
        OutErrors.clear();
        std::vector<std::string> CatalogErrors;
        MissionCatalog::Validate(CatalogErrors);
        OutErrors.insert(OutErrors.end(), CatalogErrors.begin(), CatalogErrors.end());

        if (Candidate.SaveVersion != CampaignState::CurrentVersion)
        {
            OutErrors.emplace_back("Save requires migration.");
        }
        ValidateMissionProgression(Candidate, OutErrors);
        ValidateCanon(Candidate, OutErrors);
        ValidateSnapshots(Candidate, OutErrors);
        ValidateCollections(Candidate, OutErrors);
        return OutErrors.empty();
    }

    bool MigrateCampaignState(CampaignState& Candidate, std::vector<std::string>& OutErrors)
    {
        if (Candidate.SaveVersion <= 0 || Candidate.SaveVersion > CampaignState::CurrentVersion)
        {
            OutErrors.emplace_back("Unsupported save version.");
            return false;
        }
        if (Candidate.SaveVersion == 1)
        {
            // v1 stored crew roles as quest outcomes and had no met/available beats.
            for (CrewRelationship& Crew : Candidate.Crew)
            {
                if (!Crew.Recruited)
                {
                    continue;
                }
                Crew.Met = true;
                Crew.Available = true;
                if (Crew.Role.empty())
                {
                    const auto Legacy = Candidate.QuestOutcomes.find("CrewRole." + Crew.CharacterId);
                    if (Legacy != Candidate.QuestOutcomes.end())
                    {
                        Crew.Role = Legacy->second;
                    }
                }
            }
            Candidate.SaveVersion = 2;
        }
        // v2..v7 added fields whose defaults are already correct.
        if (Candidate.SaveVersion >= 2 && Candidate.SaveVersion <= 7)
        {
            Candidate.SaveVersion = 8;
        }
        if (Candidate.SaveVersion == 8)
        {
            // v8 player snapshots had no source level; map-local coordinates are untrustworthy.
            Candidate.PlayerRuntime = PlayerRuntimeSnapshot{};
            Candidate.SaveVersion = 9;
        }
        return Candidate.SaveVersion == CampaignState::CurrentVersion;
    }

    bool ShouldRestoreMapLocalTransform(const std::string_view SnapshotSourceLevel, const std::string_view CurrentLevel)
    {
        return !SnapshotSourceLevel.empty() && SnapshotSourceLevel == CurrentLevel;
    }
}
