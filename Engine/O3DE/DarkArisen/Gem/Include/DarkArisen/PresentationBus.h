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
        /** Jake reads a physical document (letter, ledger, chart): full-screen reading, no marker. */
        virtual void OnDocumentOpened([[maybe_unused]] const AZStd::string& title, [[maybe_unused]] const AZStd::string& body) {}
        /** The credits roll begins; only ever after The Wake After. */
        virtual void OnCreditsStarted([[maybe_unused]] const AZStd::string& rollText, [[maybe_unused]] float allowSkipAfterSeconds) {}
    };

    using PresentationNotificationBus = AZ::EBus<PresentationNotifications>;
}
