#include "MapTransitionComponent.h"

#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>

#include <DarkArisen/Core/CampaignRuntime.h>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(MapTransitionComponent, "MapTransitionComponent", MapTransitionComponentTypeId);

    void MapTransitionComponent::Reflect(AZ::ReflectContext* context)
    {
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }
        serializeContext->Class<MapTransitionComponent, AZ::Component>()
            ->Version(1)
            ->Field("MissionId", &MapTransitionComponent::m_missionId)
            ->Field("Fact", &MapTransitionComponent::m_fact)
            ->Field("NextLevel", &MapTransitionComponent::m_nextLevel)
            ->Field("TravelDelay", &MapTransitionComponent::m_travelDelaySeconds);
        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
        {
            editContext->Class<MapTransitionComponent>("Dark Arisen Map Transition", "Travel when a mission completes.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Level"))
                ->DataElement(AZ::Edit::UIHandlers::Default, &MapTransitionComponent::m_missionId, "Mission", "Travel when completed")
                ->DataElement(AZ::Edit::UIHandlers::Default, &MapTransitionComponent::m_fact, "Fact", "Travel when first set")
                ->DataElement(AZ::Edit::UIHandlers::Default, &MapTransitionComponent::m_nextLevel, "Next Level", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &MapTransitionComponent::m_travelDelaySeconds, "Travel Delay (s)", "");
        }
    }

    void MapTransitionComponent::Activate()
    {
        m_queued = false;
        m_checkedOnEntry = false;
        m_remaining = 0.0f;
        CampaignNotificationBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
    }

    void MapTransitionComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        CampaignNotificationBus::Handler::BusDisconnect();
    }

    void MapTransitionComponent::OnMissionStateChanged(const AZStd::string& missionId, const int state)
    {
        if (m_queued || missionId != m_missionId || state != static_cast<int>(Core::MissionState::Completed) || m_nextLevel.empty())
        {
            return;
        }
        m_queued = true;
        m_remaining = AZ::GetMax(m_travelDelaySeconds, 0.0f);
    }

    void MapTransitionComponent::OnFactChanged(const AZStd::string& fact, const bool enabled)
    {
        if (m_queued || !enabled || m_fact.empty() || fact != m_fact || m_nextLevel.empty())
        {
            return;
        }
        m_queued = true;
        m_remaining = AZ::GetMax(m_travelDelaySeconds, 0.0f);
    }

    void MapTransitionComponent::OnTick(const float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        CampaignRequests* campaign = CampaignInterface::Get();
        if (!campaign)
        {
            return;
        }
        if (!m_checkedOnEntry)
        {
            // Entering a level whose exit mission is already done (e.g. a load between completion
            // and travel) still leaves through the authored transition.
            m_checkedOnEntry = true;
            if (!m_queued && !m_missionId.empty() &&
                campaign->GetCampaign().GetMissionState(m_missionId.c_str()) == Core::MissionState::Completed)
            {
                OnMissionStateChanged(m_missionId, static_cast<int>(Core::MissionState::Completed));
            }
        }
        if (!m_queued)
        {
            return;
        }
        m_remaining -= deltaTime;
        if (m_remaining > 0.0f)
        {
            return;
        }
        if (!campaign->TravelToLevel(m_nextLevel))
        {
            m_queued = false;
            AZ_Error("DarkArisen", false, "Map transition to %s refused; the live world could not be captured or loaded.",
                m_nextLevel.c_str());
            return;
        }
        m_queued = false;
        AZ::TickBus::Handler::BusDisconnect();
    }
}
