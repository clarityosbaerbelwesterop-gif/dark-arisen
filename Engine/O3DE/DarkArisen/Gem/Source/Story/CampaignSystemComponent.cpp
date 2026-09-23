#include "CampaignSystemComponent.h"

#include "Persistence/SaveSlotStore.h"

#include <AzCore/Console/IConsole.h>
#include <AzCore/Serialization/SerializeContext.h>

#include <DarkArisen/Core/SaveCodec.h>

#include <string>
#include <string_view>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(CampaignSystemComponent, "CampaignSystemComponent", CampaignSystemComponentTypeId);

    namespace
    {
        AZStd::string ToAz(const std::string_view value)
        {
            return AZStd::string(value.data(), value.size());
        }

        std::string ToStd(const AZStd::string& value)
        {
            return std::string(value.data(), value.size());
        }

        void ReportErrors(const char* operation, const std::vector<std::string>& errors)
        {
            for (const std::string& error : errors)
            {
                AZ_Error("DarkArisen", false, "%s: %s", operation, error.c_str());
            }
        }
    }

    void CampaignSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<CampaignSystemComponent, AZ::Component>()->Version(1);
        }
    }

    void CampaignSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("DarkArisenCampaignService"));
    }

    void CampaignSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("DarkArisenCampaignService"));
    }

    CampaignSystemComponent::CampaignSystemComponent()
    {
        ResetCampaign();
        if (CampaignInterface::Get() == nullptr)
        {
            CampaignInterface::Register(this);
        }
    }

    CampaignSystemComponent::~CampaignSystemComponent()
    {
        if (CampaignInterface::Get() == this)
        {
            CampaignInterface::Unregister(this);
        }
    }

    void CampaignSystemComponent::Activate()
    {
        WireCampaign();
        CampaignRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
        AzFramework::LevelSystemLifecycleNotificationBus::Handler::BusConnect();
    }

    void CampaignSystemComponent::Deactivate()
    {
        AzFramework::LevelSystemLifecycleNotificationBus::Handler::BusDisconnect();
        AZ::TickBus::Handler::BusDisconnect();
        CampaignRequestBus::Handler::BusDisconnect();
    }

    void CampaignSystemComponent::CreateOpeningRuntime()
    {
        Core::OpeningRuntime::Events events;
        events.CinematicRequested = [](const std::string_view id)
        {
            CampaignNotificationBus::Broadcast(&CampaignNotifications::OnCinematicRequested, ToAz(id));
        };
        events.JournalObservation = [](const std::string_view id)
        {
            CampaignNotificationBus::Broadcast(&CampaignNotifications::OnJournalObservation, ToAz(id));
        };
        m_opening = AZStd::make_unique<Core::OpeningRuntime>(*m_campaign, events);
    }

    void CampaignSystemComponent::WireCampaign()
    {
        Core::CampaignWorldServices services;
        services.CaptureWorld = [this](Core::CampaignRuntime&) { return CaptureWorld(); };
        services.NotifyChapterBoundary = [](const int chapter)
        {
            CampaignNotificationBus::Broadcast(&CampaignNotifications::OnChapterBoundary, chapter);
        };
        services.IsAutosaveSuppressed = [this] { return m_autosaveSuppressed; };
        services.WriteAutosave = [this](const Core::CampaignState& state, Core::DesignLaws::AutosaveTrigger)
        {
            return WriteSlot(SaveSlotStore::AutosaveSlot, state);
        };
        m_campaign->SetWorldServices(AZStd::move(services));
    }

    bool CampaignSystemComponent::CaptureWorld()
    {
        Core::WorldRulesSnapshot rules;
        rules.Valid = true;
        rules.TotalWorldMinutes = static_cast<std::int64_t>(m_worldMinutes);
        rules.Chapter = m_campaign->State().CurrentChapter;
        rules.AutosaveSuppressed = m_autosaveSuppressed;
        if (!m_campaign->CaptureWorldRules(rules))
        {
            AZ_Error("DarkArisen", false, "World clock capture rejected by campaign validation.");
            return false;
        }
        // Player and ship components push their own snapshots synchronously.
        CampaignNotificationBus::Broadcast(&CampaignNotifications::OnCaptureWorldState);
        return true;
    }

    bool CampaignSystemComponent::WriteSlot(const AZStd::string& slotName, const Core::CampaignState& state)
    {
        AZStd::string error;
        if (!SaveSlotStore::Write(slotName, Core::SaveCodec::Encode(state), error))
        {
            AZ_Error("DarkArisen", false, "Save to slot '%s' failed: %s", slotName.c_str(), error.c_str());
            return false;
        }
        return true;
    }

    void CampaignSystemComponent::ResetCampaign()
    {
        m_campaign = AZStd::make_unique<Core::CampaignRuntime>();
        m_campaign->AddMissionListener([](const std::string_view id, const Core::MissionState state)
        {
            CampaignNotificationBus::Broadcast(&CampaignNotifications::OnMissionStateChanged, ToAz(id), static_cast<int>(state));
        });
        m_campaign->AddFactListener([](const std::string_view fact, const bool enabled)
        {
            CampaignNotificationBus::Broadcast(&CampaignNotifications::OnFactChanged, ToAz(fact), enabled);
        });
        CreateOpeningRuntime();
        m_worldMinutes = 0.0;
        m_autosaveSuppressed = false;
    }

    void CampaignSystemComponent::NewGame()
    {
        ResetCampaign();
        WireCampaign();
    }

    bool CampaignSystemComponent::SaveToSlot(const AZStd::string& slotName)
    {
        Core::CampaignState state;
        std::vector<std::string> errors;
        if (!m_campaign->PrepareSave(state, errors))
        {
            ReportErrors("Manual save rejected", errors);
            return false;
        }
        return WriteSlot(slotName, state);
    }

    bool CampaignSystemComponent::LoadFromSlot(const AZStd::string& slotName)
    {
        std::string text;
        AZStd::string error;
        if (!SaveSlotStore::Read(slotName, text, error))
        {
            AZ_Error("DarkArisen", false, "Load from slot '%s' failed: %s", slotName.c_str(), error.c_str());
            return false;
        }
        Core::CampaignState decoded;
        std::vector<std::string> errors;
        if (!Core::SaveCodec::Decode(text, decoded, errors) || !m_campaign->LoadState(AZStd::move(decoded), errors))
        {
            // The running campaign is untouched; corrupt saves never partially apply.
            ReportErrors("Save rejected", errors);
            return false;
        }
        m_opening->RestoreProgress();
        m_worldMinutes = static_cast<double>(m_campaign->State().WorldRules.TotalWorldMinutes);
        m_autosaveSuppressed = m_campaign->State().WorldRules.AutosaveSuppressed;

        const AZStd::string savedLevel = ToAz(m_campaign->State().PlayerRuntime.SourceLevel);
        if (!savedLevel.empty() && savedLevel != m_currentLevel)
        {
            return LoadLevel(savedLevel); // No capture: the loaded state must not be overwritten.
        }
        CampaignNotificationBus::Broadcast(&CampaignNotifications::OnRestoreWorldState, m_currentLevel);
        return true;
    }

    bool CampaignSystemComponent::TravelToLevel(const AZStd::string& levelName)
    {
        if (levelName.empty())
        {
            AZ_Error("DarkArisen", false, "Travel requested without a destination level.");
            return false;
        }
        if (!CaptureWorld())
        {
            return false;
        }
        return LoadLevel(levelName);
    }

    bool CampaignSystemComponent::LoadLevel(const AZStd::string& levelName)
    {
        auto* console = AZ::Interface<AZ::IConsole>::Get();
        if (!console)
        {
            AZ_Error("DarkArisen", false, "Console unavailable; cannot load level '%s'.", levelName.c_str());
            return false;
        }
        const AZStd::string command = AZStd::string::format("LoadLevel %s", levelName.c_str());
        const auto result = console->PerformCommand(command.c_str());
        if (!result.IsSuccess())
        {
            AZ_Error("DarkArisen", false, "LoadLevel '%s' failed: %s", levelName.c_str(), result.GetError().c_str());
            return false;
        }
        return true;
    }

    bool CampaignSystemComponent::ActivateMission(const AZStd::string& missionId)
    {
        return m_campaign->ActivateMission(ToStd(missionId));
    }

    bool CampaignSystemComponent::CompleteMission(const AZStd::string& missionId)
    {
        return m_campaign->CompleteMission(ToStd(missionId));
    }

    bool CampaignSystemComponent::HasFact(const AZStd::string& fact) const
    {
        return m_campaign->HasFact(ToStd(fact));
    }

    AZStd::string CampaignSystemComponent::GetCurrentMission() const
    {
        return ToAz(m_campaign->State().CurrentMission);
    }

    int CampaignSystemComponent::GetCurrentChapter() const
    {
        return m_campaign->State().CurrentChapter;
    }

    bool CampaignSystemComponent::CompleteRest()
    {
        const bool saved = m_campaign->CompleteRest();
        if (!saved && !m_campaign->LastAutosaveError().empty())
        {
            AZ_Error("DarkArisen", false, "Rest autosave failed: %s", m_campaign->LastAutosaveError().c_str());
        }
        return saved;
    }

    void CampaignSystemComponent::OnTick(const float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        m_worldMinutes += static_cast<double>(deltaTime) * WorldMinutesPerRealSecond;
    }

    void CampaignSystemComponent::OnLoadingStart(const char* levelName)
    {
        // Set before the level's entities activate, so they resolve map-local state correctly.
        m_currentLevel = levelName ? levelName : "";
    }

    void CampaignSystemComponent::OnLoadingComplete(const char* levelName)
    {
        m_currentLevel = levelName ? levelName : "";
        CampaignNotificationBus::Broadcast(&CampaignNotifications::OnRestoreWorldState, m_currentLevel);
    }
}
