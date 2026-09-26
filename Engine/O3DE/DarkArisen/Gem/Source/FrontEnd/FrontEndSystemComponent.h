#pragma once

#include <DarkArisen/FrontEndBus.h>
#include <DarkArisen/PresentationBus.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/std/optional.h>
#include <AzCore/std/string/string.h>
#include <AzFramework/API/ApplicationAPI.h>
#include <AzFramework/Input/Events/InputChannelEventListener.h>

#include <DarkArisen/Core/FrontEnd.h>

namespace DarkArisen
{
    /**
     * Port of the Unreal Alpha front end (AAlphaMenuPlayerController/AAlphaMenuHUD, the pause
     * overlay of AAlphaGameplayPlayerController/APostureOnlyHUD and ACreditsPresentationActor).
     * Rules live in Core::FrontEndMenu, Core::HudModel and Core::CreditsRoll; this component applies
     * settings through O3DE console variables and the window bus, persists them under @user@, and
     * draws text through AzFramework::FontDrawInterface. While a menu, document or the credits hold
     * the screen, presses are consumed before gameplay input sees them; releases pass through so
     * Jake never keeps walking on a key let go during a pause.
     */
    class FrontEndSystemComponent
        : public AZ::Component
        , protected FrontEndRequestBus::Handler
        , protected PresentationNotificationBus::Handler
        , protected AZ::TickBus::Handler
        , protected AzFramework::InputChannelEventListener
        , protected AzFramework::LevelSystemLifecycleNotificationBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(FrontEndSystemComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);

        static constexpr const char* FrontEndLevel = "L_FrontEnd";
        static constexpr const char* OpeningLevel = "L_HarlowOpening";
        static constexpr const char* SettingsPath = "@user@/DarkArisen/settings.cfg";

        FrontEndSystemComponent();

    protected:
        void Activate() override;
        void Deactivate() override;

        // FrontEndRequests
        void OpenMainMenu() override;
        void CloseMainMenu() override;
        bool IsMenuOpen() const override { return m_menu.has_value(); }
        bool IsPaused() const override { return m_paused; }
        bool IsDocumentOpen() const override { return m_hud.HasDocument(); }
        bool IsCreditsRolling() const override { return m_credits.IsRolling(); }
        void Navigate(int delta) override;
        void Confirm() override;
        void Back() override;
        Core::UserSettings GetUserSettings() const override { return m_settings; }

        // PresentationNotifications
        void OnSubtitle(const AZStd::string& speaker, const AZStd::string& text) override;
        void OnDocumentOpened(const AZStd::string& title, const AZStd::string& body) override;
        void OnCreditsStarted(const AZStd::string& rollText, float allowSkipAfterSeconds) override;

        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        bool OnInputChannelEventFiltered(const AzFramework::InputChannel& inputChannel) override;

        /** A document or line belongs to the level it was read in. */
        void OnLoadingStart(const char* levelName) override;

    private:
        Core::UserSettings m_settings;
        Core::RenderCapabilities m_capabilities;
        AZStd::optional<Core::FrontEndMenu> m_menu;
        Core::HudModel m_hud;
        Core::CreditsRoll m_credits;
        bool m_mainMenuLevel = false;
        bool m_paused = false;
        bool m_returnAfterCredits = false;
        float m_tickScaleBeforePause = 1.0f;

        static bool HasSave(const char* slot);
        static AZStd::string NewestSave();
        void Execute(Core::MenuCommand command);
        void SetPaused(bool paused);
        void LoadSettings();
        bool SaveSettings() const;
        void ApplySettings() const;
        bool LoadLevel(const char* levelName) const;
        void Draw() const;
    };
}
