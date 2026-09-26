#pragma once

#include <DarkArisen/DarkArisenTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/RTTI/RTTI.h>

#include <DarkArisen/Core/FrontEnd.h>

namespace DarkArisen
{
    /**
     * The game's front end: main menu, pause menu, settings, subtitles, documents and the credits
     * roll. Keyboard and gamepad reach it through its input listener; the same requests are open to
     * tools and tests. One handler (FrontEndSystemComponent).
     */
    class FrontEndRequests : public AZ::EBusTraits
    {
    public:
        AZ_RTTI(FrontEndRequests, FrontEndRequestsTypeId);
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        virtual ~FrontEndRequests() = default;

        /** Called by the front-end level (MainMenuComponent). */
        virtual void OpenMainMenu() = 0;
        virtual void CloseMainMenu() = 0;
        virtual bool IsMenuOpen() const = 0;
        virtual bool IsPaused() const = 0;
        virtual bool IsDocumentOpen() const = 0;
        virtual bool IsCreditsRolling() const = 0;
        /** Up/down (keyboard arrows, D-pad). */
        virtual void Navigate(int delta) = 0;
        /** Enter / gamepad A. */
        virtual void Confirm() = 0;
        /** Escape / gamepad B or Start: close, go back, or pause during play. */
        virtual void Back() = 0;
        virtual Core::UserSettings GetUserSettings() const = 0;
    };

    using FrontEndRequestBus = AZ::EBus<FrontEndRequests>;
}
