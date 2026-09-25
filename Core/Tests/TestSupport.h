#pragma once

#include "DarkArisen/Core/CampaignRuntime.h"
#include "DarkArisen/Core/MissionCatalog.h"

#include <string>
#include <string_view>

namespace DarkArisen::Test
{
    /** Recruits the opening crew the way the Moran route does (met, available, recruited). */
    inline bool RecruitOpeningCrew(Core::CampaignRuntime& Campaign)
    {
        const std::pair<std::string_view, std::string_view> Crew[] = {
            {"crew.mira", "FirstMate"}, {"crew.big_tom", "Smith"}, {"crew.esteban", "Navigator"}};
        for (const auto& [Id, Role] : Crew)
        {
            if (!Campaign.MeetCrew(Id) || !Campaign.SetCrewAvailable(Id, Role) || !Campaign.RecruitCrew(Id, Role))
            {
                return false;
            }
        }
        return true;
    }

    /** Activates and completes missions in catalog order until MissionId is reached (exclusive). */
    inline bool AdvanceUntil(Core::CampaignRuntime& Campaign, const std::string_view MissionId)
    {
        for (const Core::MissionDefinition& Mission : Core::MissionCatalog::Missions())
        {
            if (Mission.Id == MissionId)
            {
                return true;
            }
            if (Campaign.GetMissionState(Mission.Id) == Core::MissionState::Completed)
            {
                continue;
            }
            if (Mission.Id == "Main.C02.02.AShipToTake" && !Campaign.AreOpeningCrewRecruited() &&
                !RecruitOpeningCrew(Campaign))
            {
                return false;
            }
            if (!Campaign.ActivateMission(Mission.Id) || !Campaign.CompleteMission(Mission.Id))
            {
                return false;
            }
        }
        return MissionId.empty();
    }

    inline bool CompleteCampaign(Core::CampaignRuntime& Campaign)
    {
        return AdvanceUntil(Campaign, "");
    }
}
