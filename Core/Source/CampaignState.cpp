#include "DarkArisen/Core/CampaignState.h"

#include "DarkArisen/Core/Facts.h"

#include <cmath>

namespace DarkArisen::Core
{
    bool Transform::IsFinite() const
    {
        const double Values[] = {Translation.X, Translation.Y, Translation.Z, Rotation.X, Rotation.Y,
            Rotation.Z, Rotation.W, Scale.X, Scale.Y, Scale.Z};
        for (const double Value : Values)
        {
            if (!std::isfinite(Value))
            {
                return false;
            }
        }
        return true;
    }

    CampaignState CampaignState::NewGame()
    {
        CampaignState State;
        State.Missions.reserve(MissionCatalog::MissionCount);
        for (const MissionDefinition& Definition : MissionCatalog::Missions())
        {
            State.Missions.push_back({std::string(Definition.Id), MissionState::Locked});
        }
        State.Missions.front().State = MissionState::Available;
        State.CurrentMission = State.Missions.front().MissionId;
        State.CurrentChapter = 1;
        return State;
    }

    MissionState CampaignState::GetMissionState(const std::string_view MissionId) const
    {
        for (const MissionRuntime& Runtime : Missions)
        {
            if (Runtime.MissionId == MissionId)
            {
                return Runtime.State;
            }
        }
        return MissionState::Locked;
    }

    bool CampaignState::HasFact(const std::string_view Fact) const
    {
        const auto& Facts = Facts::IsStoryFact(Fact) ? StoryFacts : WorldFacts;
        return Facts.count(std::string(Fact)) != 0;
    }
}
