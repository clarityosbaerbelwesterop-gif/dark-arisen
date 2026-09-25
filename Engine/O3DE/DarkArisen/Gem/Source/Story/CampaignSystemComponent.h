#pragma once

#include <DarkArisen/CampaignBus.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/std/smart_ptr/unique_ptr.h>
#include <AzFramework/API/ApplicationAPI.h>

#include <DarkArisen/Core/CampaignRuntime.h>
#include <DarkArisen/Core/OpeningRuntime.h>

namespace DarkArisen
{
    /**
     * System component that owns the one CampaignRuntime for the whole game session.
     * It survives level changes, so world state captured before travel is restored in the
     * destination before its entities read it on activation (PR #52 ordering).
     */
    class CampaignSystemComponent
        : public AZ::Component
        , protected CampaignRequestBus::Handler
        , protected AZ::TickBus::Handler
        , protected AzFramework::LevelSystemLifecycleNotificationBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(CampaignSystemComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);

        /** One in-game hour is 150 real seconds (UDarkArisenWorldRulesSubsystem canon). */
        static constexpr double WorldMinutesPerRealSecond = 60.0 / 150.0;

        CampaignSystemComponent();
        ~CampaignSystemComponent() override;

    protected:
        void Activate() override;
        void Deactivate() override;

        // CampaignRequests
        Core::CampaignRuntime& GetCampaign() override { return *m_campaign; }
        Core::OpeningRuntime& GetOpening() override { return *m_opening; }
        void NewGame() override;
        bool SaveToSlot(const AZStd::string& slotName) override;
        bool LoadFromSlot(const AZStd::string& slotName) override;
        bool TravelToLevel(const AZStd::string& levelName) override;
        bool ActivateMission(const AZStd::string& missionId) override;
        bool CompleteMission(const AZStd::string& missionId) override;
        bool HasFact(const AZStd::string& fact) const override;
        AZStd::string GetCurrentMission() const override;
        int GetCurrentChapter() const override;
        AZStd::string GetCurrentLevelName() const override { return m_currentLevel; }
        void SetAutosaveSuppressed(bool suppressed) override { m_autosaveSuppressed = suppressed; }
        bool CompleteRest() override;

        // TickBus: the single world clock.
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

        // LevelSystemLifecycleNotificationBus
        void OnLoadingStart(const char* levelName) override;
        void OnLoadingComplete(const char* levelName) override;

    private:
        AZStd::unique_ptr<Core::CampaignRuntime> m_campaign;
        AZStd::unique_ptr<Core::OpeningRuntime> m_opening;
        AZStd::string m_currentLevel;
        bool m_autosaveSuppressed = false;
        double m_worldMinutes = 0.0;

        void ResetCampaign();
        void WireCampaign();
        void CreateOpeningRuntime();
        bool CaptureWorld();
        bool WriteSlot(const AZStd::string& slotName, const Core::CampaignState& state);
        bool LoadLevel(const AZStd::string& levelName);
    };
}
