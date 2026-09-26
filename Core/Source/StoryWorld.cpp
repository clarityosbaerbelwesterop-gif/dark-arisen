#include "DarkArisen/Core/StoryWorld.h"

#include "DarkArisen/Core/CampaignRuntime.h"
#include "DarkArisen/Core/EntityPolicy.h"
#include "DarkArisen/Core/MissionCatalog.h"

#include <algorithm>
#include <cmath>
#include <set>

namespace DarkArisen::Core
{
    namespace
    {
        constexpr std::array<std::string_view, 11> KindNames = {"LocationTrigger", "Contact", "Evidence", "RouteResolution",
            "WarAction", "HolderBoss", "DuelingEnemy", "EthanHarlow", "PlayerShip", "NavalEnemy", "NavalEncounterGate"};

        constexpr std::string_view FinalMission = "Main.C10.05.TheWakeAfter";
        /** Real Ethan is physically present only once Jake reaches him on the prison vessel. */
        constexpr std::string_view FirstMissionWithRealEthan = "Main.C08.02.BrothersAlive";
        constexpr std::string_view RealEthanContactPrefix = "ethan.harlow.real";

        bool IsCanonicalBoss(const std::string_view BossId)
        {
            const auto& Bosses = CampaignRuntime::CanonicalBossIds();
            return std::find(Bosses.begin(), Bosses.end(), BossId) != Bosses.end();
        }

        bool Completes(const StoryActorSpec& Actor)
        {
            switch (Actor.Kind)
            {
            case StoryActorKind::LocationTrigger: return Actor.Action == LocationAction::CompleteMission;
            case StoryActorKind::Contact:
            case StoryActorKind::Evidence: return Actor.CompletesMission;
            case StoryActorKind::RouteResolution: return true;
            case StoryActorKind::WarAction: return !Actor.IsHoldingClaim() || Actor.CompletesMission;
            case StoryActorKind::HolderBoss: return Actor.CompletesMissionOnDefeat;
            case StoryActorKind::NavalEncounterGate: return true;
            default: return false;
            }
        }

        bool IsActive(const CampaignRuntime& Campaign, const std::string_view MissionId)
        {
            return !MissionId.empty() && Campaign.GetMissionState(MissionId) == MissionState::Active;
        }

        void RecordCheckpoint(CampaignRuntime& Campaign, const StoryActorSpec& Actor)
        {
            if (!Actor.CheckpointId.empty() && !Actor.SpawnId.empty())
            {
                Campaign.SetCheckpoint(Actor.CheckpointId, Actor.SpawnId);
            }
        }

        /** Plain documents (no fact, no completion) are readable any time, like AExamineDocumentActor. */
        bool IsPlainDocument(const StoryActorSpec& Actor)
        {
            return Actor.Kind == StoryActorKind::Evidence && !Actor.CompletesMission && Actor.WorldFactOnRead.empty() &&
                Actor.RequiredBossId.empty();
        }

        void ValidateActor(const StoryMissionContract& Contract, const StoryActorSpec& Actor, std::vector<std::string>& Errors)
        {
            const std::string Where = Contract.MissionId + " actor " + (Actor.Name.empty() ? std::string("<unnamed>") : Actor.Name);
            const auto Require = [&](const std::string& Value, const char* Field)
            {
                if (Value.empty()) Errors.push_back(Where + " requires " + Field + ".");
            };
            Require(Actor.Name, "a name");
            if (Contract.FindAnchor(Actor.Anchor) == nullptr)
            {
                Errors.push_back(Where + " references missing anchor '" + Actor.Anchor + "'.");
            }
            if (Actor.MissionId != Contract.MissionId)
            {
                Errors.push_back(Where + " belongs to another mission.");
            }
            switch (Actor.Kind)
            {
            case StoryActorKind::Contact:
                Require(Actor.ContactId, "contactId");
                Require(Actor.DisplayName, "displayName");
                Require(Actor.Text, "text");
                break;
            case StoryActorKind::Evidence:
                Require(Actor.Title, "title");
                Require(Actor.Body, "body");
                if (!Actor.RequiredBossId.empty() && !IsCanonicalBoss(Actor.RequiredBossId))
                {
                    Errors.push_back(Where + " requires unknown boss " + Actor.RequiredBossId + ".");
                }
                break;
            case StoryActorKind::RouteResolution:
                Require(Actor.OutcomeKey, "outcomeKey");
                Require(Actor.OutcomeValue, "outcomeValue");
                Require(Actor.Label, "label");
                break;
            case StoryActorKind::WarAction:
                if (Actor.IsHoldingClaim())
                {
                    if (!Actor.CompletesMission) Errors.push_back(Where + " claims a holding without completing its mission.");
                    Require(Actor.CheckpointId, "checkpointId");
                }
                else
                {
                    Require(Actor.RegionId, "regionId");
                    Require(Actor.OutcomeKey, "outcomeKey");
                    Require(Actor.OutcomeValue, "outcomeValue");
                    Require(Actor.Label, "label");
                    if (Actor.ControlDelta < 0 || Actor.LiberationDelta < 0 || Actor.CrimsonDelta < 0)
                    {
                        Errors.push_back(Where + " has negative war deltas.");
                    }
                }
                break;
            case StoryActorKind::HolderBoss:
                if (!IsCanonicalBoss(Actor.BossId) || !EntityPolicy::CanBeBoss(Actor.BossId))
                {
                    Errors.push_back(Where + " names non-canonical boss '" + Actor.BossId + "'.");
                }
                if (Actor.BossId == EntityPolicy::DreamEthanBossId && Contract.MissionId != "Main.C09.03.DreamFight")
                {
                    Errors.push_back(Where + " places Dream Ethan outside the Dream Fight.");
                }
                if (Actor.BossId == "boss.draven_voss" && Contract.MissionId != "Main.C10.04.DravenVoss")
                {
                    Errors.push_back(Where + " places Draven's duel outside the Draven Voss mission.");
                }
                Require(Actor.OutcomeKey, "outcomeKey");
                Require(Actor.OutcomeValue, "outcomeValue");
                break;
            case StoryActorKind::NavalEncounterGate:
                if (Actor.RequiredDefeats < 1) Errors.push_back(Where + " requires at least one defeat.");
                break;
            default:
                break;
            }
            const bool RealEthan = Actor.Kind == StoryActorKind::EthanHarlow ||
                (Actor.Kind == StoryActorKind::Contact && Actor.ContactId.rfind(RealEthanContactPrefix, 0) == 0);
            if (RealEthan && MissionCatalog::FindIndex(Contract.MissionId) < MissionCatalog::FindIndex(FirstMissionWithRealEthan))
            {
                Errors.push_back(Where + " puts real Ethan in a scene before Jake reaches him (Chapter 8).");
            }
            if (Actor.Kind == StoryActorKind::EthanHarlow && Actor.Ethan == EthanPresence::Captive &&
                Contract.MissionId != FirstMissionWithRealEthan)
            {
                Errors.push_back(Where + " keeps Ethan captive after the rescue.");
            }
        }
    }

    std::optional<StoryActorKind> ParseStoryActorKind(const std::string_view Name)
    {
        for (std::size_t Index = 0; Index < KindNames.size(); ++Index)
        {
            if (KindNames[Index] == Name) return static_cast<StoryActorKind>(Index);
        }
        return std::nullopt;
    }

    std::string_view ToString(const StoryActorKind Kind)
    {
        const auto Index = static_cast<std::size_t>(Kind);
        return Index < KindNames.size() ? KindNames[Index] : std::string_view("Unknown");
    }

    const LayoutPoint* StoryMissionContract::FindAnchor(const std::string_view Name) const
    {
        for (const auto& [AnchorName, Point] : Anchors)
        {
            if (AnchorName == Name) return &Point;
        }
        return nullptr;
    }

    bool StoryMissionContract::IsNaval() const
    {
        return std::any_of(Actors.begin(), Actors.end(), [](const StoryActorSpec& Actor)
        {
            return Actor.Kind == StoryActorKind::PlayerShip || Actor.Kind == StoryActorKind::NavalEnemy ||
                Actor.Kind == StoryActorKind::NavalEncounterGate;
        });
    }

    bool ValidateStoryContract(const StoryMissionContract& Contract, std::vector<std::string>& OutErrors)
    {
        const std::size_t Before = OutErrors.size();
        const std::string& Id = Contract.MissionId;
        const int Index = MissionCatalog::FindIndex(Id);
        if (Index < 0)
        {
            OutErrors.push_back(Contract.SourcePath + ": unknown mission '" + Id + "'.");
            return false;
        }
        const std::string_view ExpectedMap = MissionCatalog::LevelFor(Id);
        if (Contract.MapId != ExpectedMap)
        {
            OutErrors.push_back(Id + " is authored in " + Contract.MapId + " but the campaign loads " + std::string(ExpectedMap) + ".");
        }
        if (Contract.CheckpointId.empty() || Contract.SpawnId.empty())
        {
            OutErrors.push_back(Id + " has no checkpoint/spawn.");
        }

        std::set<std::string, std::less<>> AnchorNames;
        for (const auto& [Name, Point] : Contract.Anchors)
        {
            if (Name.empty() || !AnchorNames.insert(Name).second)
            {
                OutErrors.push_back(Id + " has an empty or duplicate anchor '" + Name + "'.");
            }
            if (!std::isfinite(Point[0]) || !std::isfinite(Point[1]) || !std::isfinite(Point[2]))
            {
                OutErrors.push_back(Id + " anchor " + Name + " is not finite.");
            }
        }
        if (Contract.FindAnchor(Contract.EntryAnchor) == nullptr)
        {
            OutErrors.push_back(Id + " entry anchor '" + Contract.EntryAnchor + "' does not exist.");
        }
        for (const StoryGeometrySpec& Geometry : Contract.Geometry)
        {
            if (Geometry.Name.empty() || Contract.FindAnchor(Geometry.Anchor) == nullptr)
            {
                OutErrors.push_back(Id + " geometry '" + Geometry.Name + "' has no valid anchor.");
            }
            if (Geometry.IsBox() && !(Geometry.Size[0] > 0.0 && Geometry.Size[1] > 0.0 && Geometry.Size[2] > 0.0))
            {
                OutErrors.push_back(Id + " geometry '" + Geometry.Name + "' has a non-positive size.");
            }
        }

        // Next route: the campaign's own order decides; the final mission hands off to credits.
        const auto& Missions = MissionCatalog::Missions();
        const bool IsFinal = Id == FinalMission;
        if (IsFinal)
        {
            if (!Contract.NextMission.empty() || !Contract.CreditsHandoff)
            {
                OutErrors.push_back(Id + " must end the campaign and hand off to the credits.");
            }
        }
        else if (Index + 1 >= static_cast<int>(Missions.size()) || Contract.NextMission != Missions[static_cast<std::size_t>(Index + 1)].Id)
        {
            OutErrors.push_back(Id + " routes to '" + Contract.NextMission + "', not the campaign's next mission.");
        }

        const auto Authored = CampaignRuntime::AuthoredCompletionFacts(Id);
        for (const std::string& Fact : Contract.CompletionFacts)
        {
            if (std::find(Authored.begin(), Authored.end(), Fact) == Authored.end())
            {
                OutErrors.push_back(Id + " expects completion fact " + Fact + " that the campaign never sets.");
            }
        }

        std::set<std::string, std::less<>> ActorNames;
        bool ActivatesAtEntry = false;
        bool HasCompletion = false;
        bool HasNavalEnemy = false;
        bool HasNavalGate = false;
        bool HasPlayerShip = false;
        for (const StoryActorSpec& Actor : Contract.Actors)
        {
            if (!ActorNames.insert(Actor.Name).second)
            {
                OutErrors.push_back(Id + " has duplicate actor '" + Actor.Name + "'.");
            }
            ValidateActor(Contract, Actor, OutErrors);
            ActivatesAtEntry = ActivatesAtEntry || (Actor.Kind == StoryActorKind::LocationTrigger &&
                Actor.Action == LocationAction::ActivateMission && Actor.Anchor == Contract.EntryAnchor);
            HasCompletion = HasCompletion || Completes(Actor);
            HasNavalEnemy = HasNavalEnemy || Actor.Kind == StoryActorKind::NavalEnemy;
            HasNavalGate = HasNavalGate || Actor.Kind == StoryActorKind::NavalEncounterGate;
            HasPlayerShip = HasPlayerShip || Actor.Kind == StoryActorKind::PlayerShip;

            // A boss that does not complete its mission must unlock the evidence that does.
            if (Actor.Kind == StoryActorKind::HolderBoss && !Actor.CompletesMissionOnDefeat)
            {
                const bool Unlocks = std::any_of(Contract.Actors.begin(), Contract.Actors.end(), [&](const StoryActorSpec& Other)
                {
                    return Other.Kind == StoryActorKind::Evidence && Other.CompletesMission && Other.RequiredBossId == Actor.BossId;
                });
                if (!Unlocks) OutErrors.push_back(Id + " boss " + Actor.BossId + " neither completes the mission nor unlocks it.");
            }
        }
        if (!ActivatesAtEntry)
        {
            OutErrors.push_back(Id + " is not activated at its entry anchor.");
        }
        if (!HasCompletion)
        {
            OutErrors.push_back(Id + " has no physical way to complete.");
        }
        if (HasNavalEnemy && (!HasNavalGate || !HasPlayerShip))
        {
            OutErrors.push_back(Id + " has hostile ships without the player's ship and a naval gate.");
        }
        return OutErrors.size() == Before;
    }

    namespace StoryActions
    {
        bool OnLocationEntered(CampaignRuntime& Campaign, const StoryActorSpec& Actor, const bool LivingDuelistsRemain)
        {
            if (Actor.Kind != StoryActorKind::LocationTrigger || Actor.MissionId.empty())
            {
                return false;
            }
            bool Changed = false;
            if (Actor.Action == LocationAction::ActivateMission)
            {
                Changed = Campaign.GetMissionState(Actor.MissionId) == MissionState::Available && Campaign.ActivateMission(Actor.MissionId);
            }
            else
            {
                if (LivingDuelistsRemain)
                {
                    return false;
                }
                Changed = IsActive(Campaign, Actor.MissionId) && Campaign.CompleteMission(Actor.MissionId);
            }
            if (Changed)
            {
                RecordCheckpoint(Campaign, Actor);
            }
            return Changed;
        }

        bool CanInteract(const CampaignRuntime& Campaign, const StoryActorSpec& Actor)
        {
            switch (Actor.Kind)
            {
            case StoryActorKind::Contact:
            {
                if (Actor.MissionId.empty()) return true;
                const MissionState State = Campaign.GetMissionState(Actor.MissionId);
                return State == MissionState::Active || State == MissionState::Completed;
            }
            case StoryActorKind::Evidence:
                if (IsPlainDocument(Actor)) return true;
                return IsActive(Campaign, Actor.MissionId) &&
                    (Actor.RequiredBossId.empty() || Campaign.HasBossDefeated(Actor.RequiredBossId));
            case StoryActorKind::RouteResolution:
                return IsActive(Campaign, Actor.MissionId) && !Actor.OutcomeKey.empty() && !Actor.OutcomeValue.empty();
            case StoryActorKind::WarAction:
                return IsActive(Campaign, Actor.MissionId) &&
                    (Actor.IsHoldingClaim() || (!Actor.RegionId.empty() && !Actor.OutcomeKey.empty() && !Actor.OutcomeValue.empty()));
            default:
                return false;
            }
        }

        StoryInteraction Interact(CampaignRuntime& Campaign, const StoryActorSpec& Actor)
        {
            StoryInteraction Result;
            if (!CanInteract(Campaign, Actor))
            {
                return Result;
            }
            switch (Actor.Kind)
            {
            case StoryActorKind::Contact:
                Result.Accepted = true;
                Result.Speaker = Actor.DisplayName.empty() ? Actor.ContactId : Actor.DisplayName;
                Result.Line = Actor.Text;
                if (Actor.CompletesMission && IsActive(Campaign, Actor.MissionId) && Campaign.CompleteMission(Actor.MissionId))
                {
                    Result.MissionCompleted = true;
                    RecordCheckpoint(Campaign, Actor);
                }
                break;
            case StoryActorKind::Evidence:
                Result.Accepted = true;
                Result.DocumentTitle = Actor.Title;
                Result.DocumentBody = Actor.Body;
                // Jake keeps what he read, verbatim and once; the notebook is the only record.
                Campaign.UpdateJournal([&](QuestJournalState& State, const QuestCatalog&)
                {
                    return QuestJournal::RecordEvidence(State, {Actor.MissionId + "/" + Actor.Name, Actor.MissionId, Actor.Title, Actor.Body,
                        Campaign.State().WorldRules.TotalWorldMinutes});
                });
                if (!Actor.WorldFactOnRead.empty())
                {
                    Campaign.SetWorldFact(Actor.WorldFactOnRead, true);
                }
                if (Actor.CompletesMission && Campaign.CompleteMission(Actor.MissionId))
                {
                    Result.MissionCompleted = true;
                    RecordCheckpoint(Campaign, Actor);
                }
                break;
            case StoryActorKind::RouteResolution:
                if (Campaign.SetQuestOutcome(Actor.OutcomeKey, Actor.OutcomeValue))
                {
                    Result.Accepted = true;
                    Result.MissionCompleted = Campaign.CompleteMission(Actor.MissionId);
                }
                break;
            case StoryActorKind::WarAction:
                if (Actor.IsHoldingClaim())
                {
                    if (Campaign.ClaimHolding(Actor.HoldingId))
                    {
                        Result.Accepted = true;
                        if (Actor.CompletesMission && Campaign.CompleteMission(Actor.MissionId))
                        {
                            Result.MissionCompleted = true;
                            RecordCheckpoint(Campaign, Actor);
                        }
                    }
                    break;
                }
                {
                    // Outcomes are final: refuse before touching the war if another choice was recorded.
                    const auto& Outcomes = Campaign.State().QuestOutcomes;
                    const auto Existing = Outcomes.find(Actor.OutcomeKey);
                    if (Existing != Outcomes.end() && Existing->second != Actor.OutcomeValue)
                    {
                        break;
                    }
                    if (Campaign.GetColonialWar().Regions.count(Actor.RegionId) == 0 &&
                        !Campaign.RegisterWarRegion(Actor.RegionId, Actor.InitialController))
                    {
                        break;
                    }
                    if (!Campaign.RecordWarAction(Actor.RegionId, Actor.Verb, Actor.TargetFaction, Actor.ControlDelta,
                            Actor.LiberationDelta, Actor.CrimsonDelta) ||
                        !Campaign.SetQuestOutcome(Actor.OutcomeKey, Actor.OutcomeValue))
                    {
                        break;
                    }
                    Result.Accepted = true;
                    Result.MissionCompleted = Campaign.CompleteMission(Actor.MissionId);
                }
                break;
            default:
                break;
            }
            return Result;
        }

        bool UpdateNavalGate(CampaignRuntime& Campaign, const StoryActorSpec& Gate, const int HostileShips, const int SunkShips,
            const double PlayerShipDistanceMetres)
        {
            if (Gate.Kind != StoryActorKind::NavalEncounterGate || !IsActive(Campaign, Gate.MissionId) ||
                HostileShips < Gate.RequiredDefeats || SunkShips < HostileShips)
            {
                return false;
            }
            if (!Gate.RequiredWorldFact.empty() && !Campaign.HasFact(Gate.RequiredWorldFact))
            {
                return false;
            }
            if (Gate.CompletionRadiusMetres > 0.0 &&
                !(std::isfinite(PlayerShipDistanceMetres) && PlayerShipDistanceMetres <= Gate.CompletionRadiusMetres))
            {
                return false;
            }
            if (!Campaign.CompleteMission(Gate.MissionId))
            {
                return false;
            }
            RecordCheckpoint(Campaign, Gate);
            return true;
        }
    }
}
