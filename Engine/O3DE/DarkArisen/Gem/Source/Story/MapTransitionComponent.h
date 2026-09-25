#pragma once

#include <DarkArisen/CampaignBus.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/std/string/string.h>

namespace DarkArisen
{
    /**
     * Travels to the next level when a mission completes (Unreal AMainStoryMapTransitionActor) or,
     * for walked routes inside a mission, when a world fact is first set (e.g. reaching a camp).
     * Travel captures the live world first; if capture or loading fails the transition stays
     * available and the failure is logged, never skipped.
     */
    class MapTransitionComponent
        : public AZ::Component
        , protected AZ::TickBus::Handler
        , protected CampaignNotificationBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(MapTransitionComponent);

        static void Reflect(AZ::ReflectContext* context);

        bool IsTravelQueued() const { return m_queued; }

    protected:
        void Activate() override;
        void Deactivate() override;
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        void OnMissionStateChanged(const AZStd::string& missionId, int state) override;
        void OnFactChanged(const AZStd::string& fact, bool enabled) override;

    private:
        AZStd::string m_missionId;
        AZStd::string m_fact;
        AZStd::string m_nextLevel;
        float m_travelDelaySeconds = 0.0f;

        bool m_queued = false;
        bool m_checkedOnEntry = false;
        float m_remaining = 0.0f;
    };
}
