#pragma once

#include <DarkArisen/DarkArisenTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/RTTI/RTTI.h>
#include <AzCore/std/string/string.h>

namespace DarkArisen
{
    /** Authored presentation playback, for subtitles and camera staging. Broadcast. */
    class PresentationNotifications : public AZ::EBusTraits
    {
    public:
        AZ_RTTI(PresentationNotifications, PresentationNotificationsTypeId);
        virtual ~PresentationNotifications() = default;

        virtual void OnPresentationStarted([[maybe_unused]] const AZStd::string& presentationId) {}
        virtual void OnSubtitle([[maybe_unused]] const AZStd::string& speaker, [[maybe_unused]] const AZStd::string& text) {}
    };

    using PresentationNotificationBus = AZ::EBus<PresentationNotifications>;
}
