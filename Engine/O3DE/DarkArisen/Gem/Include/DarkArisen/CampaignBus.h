#pragma once

#include <DarkArisen/DarkArisenTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>
#include <AzCore/RTTI/RTTI.h>
#include <AzCore/std/string/string.h>

namespace DarkArisen::Core
{
    class CampaignRuntime;
    class OpeningRuntime;
}

namespace DarkArisen
{
    /**
     * Engine-facing access to the authoritative campaign. The engine-neutral
     * Core::CampaignRuntime owns every rule; this interface only adapts it to O3DE.
     */
    class CampaignRequests
    {
    public:
        AZ_RTTI(CampaignRequests, CampaignRequestsTypeId);
        virtual ~CampaignRequests() = default;

        virtual Core::CampaignRuntime& GetCampaign() = 0;
        virtual Core::OpeningRuntime& GetOpening() = 0;

        virtual void NewGame() = 0;
        /** Manual save. Autosaves go through the campaign's two legal triggers only. */
        virtual bool SaveToSlot(const AZStd::string& slotName) = 0;
        virtual bool LoadFromSlot(const AZStd::string& slotName) = 0;
        /** Captures live state, then loads the level; destination state restores before gameplay. */
        virtual bool TravelToLevel(const AZStd::string& levelName) = 0;

        virtual bool ActivateMission(const AZStd::string& missionId) = 0;
        virtual bool CompleteMission(const AZStd::string& missionId) = 0;
        virtual bool HasFact(const AZStd::string& fact) const = 0;
        virtual AZStd::string GetCurrentMission() const = 0;
        virtual int GetCurrentChapter() const = 0;
        virtual AZStd::string GetCurrentLevelName() const = 0;

        /** Lake-to-dock style windows suppress autosaves only; manual save stays available. */
        virtual void SetAutosaveSuppressed(bool suppressed) = 0;
        virtual bool CompleteRest() = 0;
    };

    class CampaignRequestBusTraits : public AZ::EBusTraits
    {
    public:
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
    };

    using CampaignRequestBus = AZ::EBus<CampaignRequests, CampaignRequestBusTraits>;
    using CampaignInterface = AZ::Interface<CampaignRequests>;

    /** Broadcast after the campaign has applied facts and legal saves. */
    class CampaignNotifications : public AZ::EBusTraits
    {
    public:
        AZ_RTTI(CampaignNotifications, CampaignNotificationsTypeId);
        virtual ~CampaignNotifications() = default;

        /** State is DarkArisen::Core::MissionState as an integer. */
        virtual void OnMissionStateChanged([[maybe_unused]] const AZStd::string& missionId, [[maybe_unused]] int state) {}
        virtual void OnFactChanged([[maybe_unused]] const AZStd::string& fact, [[maybe_unused]] bool enabled) {}
        virtual void OnChapterBoundary([[maybe_unused]] int newChapter) {}
        virtual void OnCinematicRequested([[maybe_unused]] const AZStd::string& cinematicId) {}
        virtual void OnJournalObservation([[maybe_unused]] const AZStd::string& journalId) {}
        /** Live components push their snapshots (player vitals, ship voyage) into the campaign. */
        virtual void OnCaptureWorldState() {}
        /** Components restore from the campaign; map-local transforms only when the level matches. */
        virtual void OnRestoreWorldState([[maybe_unused]] const AZStd::string& levelName) {}
    };

    using CampaignNotificationBus = AZ::EBus<CampaignNotifications>;
}
