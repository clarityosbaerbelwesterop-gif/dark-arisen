#include "FrontEndSystemComponent.h"

#include "Persistence/SaveSlotStore.h"

#include <DarkArisen/CampaignBus.h>

#include <AzCore/Console/IConsole.h>
#include <AzCore/IO/FileIO.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Time/ITime.h>
#include <AzFramework/API/ApplicationAPI.h>
#include <AzFramework/Font/FontInterface.h>
#include <AzFramework/Input/Devices/Gamepad/InputDeviceGamepad.h>
#include <AzFramework/Input/Devices/Keyboard/InputDeviceKeyboard.h>
#include <AzFramework/Windowing/WindowBus.h>

#include <algorithm>
#include <string>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(FrontEndSystemComponent, "FrontEndSystemComponent", FrontEndSystemComponentTypeId);

    namespace
    {
        using Keyboard = AzFramework::InputDeviceKeyboard::Key;
        using Gamepad = AzFramework::InputDeviceGamepad;

        enum class Press { None, Up, Down, Confirm, Back, Close };

        Press Classify(const AzFramework::InputChannelId& id)
        {
            if (id == Keyboard::NavigationArrowUp || id == Gamepad::Button::DU) return Press::Up;
            if (id == Keyboard::NavigationArrowDown || id == Gamepad::Button::DD) return Press::Down;
            if (id == Keyboard::EditEnter || id == Keyboard::EditSpace || id == Gamepad::Button::A) return Press::Confirm;
            if (id == Keyboard::Escape || id == Gamepad::Button::B || id == Gamepad::Button::Start) return Press::Back;
            // Interact (E / Y) puts a document down again.
            if (id == Keyboard::AlphanumericE || id == Gamepad::Button::Y) return Press::Close;
            return Press::None;
        }

        AzFramework::WindowSize ScreenSize()
        {
            AzFramework::NativeWindowHandle window = nullptr;
            AzFramework::WindowSystemRequestBus::BroadcastResult(window, &AzFramework::WindowSystemRequests::GetDefaultWindowHandle);
            AzFramework::WindowSize size(1920, 1080);
            if (window)
            {
                AzFramework::WindowRequestBus::EventResult(size, window, &AzFramework::WindowRequests::GetClientAreaSize);
            }
            if (size.m_width == 0 || size.m_height == 0)
            {
                size = AzFramework::WindowSize(1920, 1080);
            }
            return size;
        }

        void DrawText(AzFramework::FontDrawInterface& font, const AZStd::string& text, const float x, const float y, const float size,
            const AZ::Color& color, const AzFramework::TextHorizontalAlignment align = AzFramework::TextHorizontalAlignment::Left)
        {
            AzFramework::TextDrawParameters params;
            params.m_drawViewportId = AzFramework::InvalidViewportId; // the game's default viewport
            params.m_position = AZ::Vector3(x, y, 1.0f);
            params.m_color = color;
            params.m_textSizeFactor = size;
            params.m_hAlign = align;
            params.m_multiline = true;
            font.DrawScreenAlignedText2d(params, text);
        }

        AZStd::string ToAz(const std::string& text) { return AZStd::string(text.c_str(), text.size()); }
        std::string ToStd(const AZStd::string& text) { return std::string(text.c_str(), text.size()); }
    }

    FrontEndSystemComponent::FrontEndSystemComponent()
        : AzFramework::InputChannelEventListener(AzFramework::InputChannelEventListener::GetPriorityUI())
    {
    }

    void FrontEndSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<FrontEndSystemComponent, AZ::Component>()->Version(1);
        }
    }

    void FrontEndSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("DarkArisenFrontEndService"));
    }

    void FrontEndSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("DarkArisenFrontEndService"));
    }

    void FrontEndSystemComponent::Activate()
    {
        LoadSettings();
        ApplySettings();
        FrontEndRequestBus::Handler::BusConnect();
        PresentationNotificationBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
        AzFramework::InputChannelEventListener::Connect();
        AzFramework::LevelSystemLifecycleNotificationBus::Handler::BusConnect();
    }

    void FrontEndSystemComponent::Deactivate()
    {
        AzFramework::LevelSystemLifecycleNotificationBus::Handler::BusDisconnect();
        AzFramework::InputChannelEventListener::Disconnect();
        AZ::TickBus::Handler::BusDisconnect();
        PresentationNotificationBus::Handler::BusDisconnect();
        FrontEndRequestBus::Handler::BusDisconnect();
        SetPaused(false);
        m_menu.reset();
    }

    bool FrontEndSystemComponent::HasSave(const char* slot)
    {
        return SaveSlotStore::Exists(slot);
    }

    AZStd::string FrontEndSystemComponent::NewestSave()
    {
        // Continue picks up whichever of the autosave and the manual save is newer.
        const AZ::u64 autosave = SaveSlotStore::ModifiedTime(SaveSlotStore::AutosaveSlot);
        const AZ::u64 manual = SaveSlotStore::ModifiedTime(SaveSlotStore::ManualSlot);
        if (autosave == 0 && manual == 0)
        {
            return {};
        }
        return autosave >= manual ? AZStd::string(SaveSlotStore::AutosaveSlot) : AZStd::string(SaveSlotStore::ManualSlot);
    }

    void FrontEndSystemComponent::OpenMainMenu()
    {
        SetPaused(false);
        m_mainMenuLevel = true;
        m_menu = Core::FrontEndMenu::MainMenu(!NewestSave().empty());
    }

    void FrontEndSystemComponent::CloseMainMenu()
    {
        m_mainMenuLevel = false;
        if (m_menu && !m_paused)
        {
            m_menu.reset();
        }
    }

    void FrontEndSystemComponent::Navigate(const int delta)
    {
        if (m_menu)
        {
            m_menu->MoveFocus(delta, m_settings, m_capabilities);
        }
    }

    void FrontEndSystemComponent::Confirm()
    {
        if (m_credits.IsRolling())
        {
            m_credits.Skip();
            return;
        }
        if (m_hud.HasDocument())
        {
            m_hud.CloseDocument();
            return;
        }
        if (m_menu)
        {
            Execute(m_menu->Activate(m_settings, m_capabilities));
        }
    }

    void FrontEndSystemComponent::Back()
    {
        if (m_credits.IsRolling())
        {
            m_credits.Skip();
            return;
        }
        if (m_hud.HasDocument())
        {
            m_hud.CloseDocument();
            return;
        }
        if (m_menu)
        {
            Execute(m_menu->Cancel());
            return;
        }
        if (!m_mainMenuLevel)
        {
            SetPaused(true);
        }
    }

    void FrontEndSystemComponent::Execute(const Core::MenuCommand command)
    {
        CampaignRequests* campaign = CampaignInterface::Get();
        switch (command)
        {
        case Core::MenuCommand::None:
            return;
        case Core::MenuCommand::StartNewGame:
            if (campaign)
            {
                m_menu.reset();
                m_mainMenuLevel = false;
                campaign->NewGame();
                LoadLevel(OpeningLevel);
            }
            return;
        case Core::MenuCommand::ContinueGame:
        {
            const AZStd::string slot = NewestSave();
            if (campaign && !slot.empty() && campaign->LoadFromSlot(slot))
            {
                m_menu.reset();
                m_mainMenuLevel = false;
            }
            else
            {
                m_hud.ShowSystemMessage("The save could not be loaded.");
            }
            return;
        }
        case Core::MenuCommand::Quit:
            AzFramework::ApplicationRequests::Bus::Broadcast(&AzFramework::ApplicationRequests::ExitMainLoop);
            return;
        case Core::MenuCommand::Resume:
            SetPaused(false);
            return;
        case Core::MenuCommand::SaveGame:
        {
            const bool saved = campaign && campaign->SaveToSlot(SaveSlotStore::ManualSlot);
            m_hud.ShowSystemMessage(saved ? "Game saved." : "Save failed.");
            if (m_menu)
            {
                m_menu->SetHasSave(HasSave(SaveSlotStore::ManualSlot));
            }
            return;
        }
        case Core::MenuCommand::LoadGame:
            SetPaused(false);
            if (!campaign || !campaign->LoadFromSlot(SaveSlotStore::ManualSlot))
            {
                m_hud.ShowSystemMessage("Load failed.");
            }
            return;
        case Core::MenuCommand::ReturnToMainMenu:
            SetPaused(false);
            LoadLevel(FrontEndLevel);
            return;
        case Core::MenuCommand::ApplySettings:
            ApplySettings();
            if (!SaveSettings())
            {
                m_hud.ShowSystemMessage("Settings could not be saved.");
            }
            return;
        }
    }

    void FrontEndSystemComponent::SetPaused(const bool paused)
    {
        if (paused == m_paused)
        {
            return;
        }
        auto* time = AZ::Interface<AZ::ITime>::Get();
        m_paused = paused;
        if (paused)
        {
            m_tickScaleBeforePause = time ? time->GetSimulationTickScale() : 1.0f;
            if (time)
            {
                time->SetSimulationTickScale(0.0f); // O3DE advances a paused world by 1 us per frame
            }
            m_menu = Core::FrontEndMenu::PauseMenu(HasSave(SaveSlotStore::ManualSlot));
            return;
        }
        if (time)
        {
            time->SetSimulationTickScale(m_tickScaleBeforePause);
        }
        if (!m_mainMenuLevel)
        {
            m_menu.reset();
        }
    }

    void FrontEndSystemComponent::OnSubtitle(const AZStd::string& speaker, const AZStd::string& text)
    {
        m_hud.ShowSubtitle(ToStd(speaker), ToStd(text));
    }

    void FrontEndSystemComponent::OnDocumentOpened(const AZStd::string& title, const AZStd::string& body)
    {
        m_hud.OpenDocument(ToStd(title), ToStd(body));
    }

    void FrontEndSystemComponent::OnCreditsStarted(const AZStd::string& rollText, const float allowSkipAfterSeconds)
    {
        m_credits.Start(ToStd(rollText), allowSkipAfterSeconds);
        m_returnAfterCredits = m_credits.IsRolling();
    }

    void FrontEndSystemComponent::OnTick(const float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        m_hud.Tick(deltaTime);
        m_credits.Tick(deltaTime);
        if (m_returnAfterCredits && m_credits.IsFinished())
        {
            m_returnAfterCredits = false;
            LoadLevel(FrontEndLevel); // ACreditsPresentationActor::FinishCredits
        }
        Draw();
    }

    void FrontEndSystemComponent::OnLoadingStart([[maybe_unused]] const char* levelName)
    {
        m_hud.ClearLevelPresentation();
    }

    bool FrontEndSystemComponent::OnInputChannelEventFiltered(const AzFramework::InputChannel& inputChannel)
    {
        const bool modal = m_menu.has_value() || m_hud.HasDocument() || m_credits.IsRolling();
        if (!inputChannel.IsStateBegan())
        {
            return false; // releases and axis updates always reach gameplay
        }
        switch (Classify(inputChannel.GetInputChannelId()))
        {
        case Press::Up:
            Navigate(-1);
            return modal;
        case Press::Down:
            Navigate(1);
            return modal;
        case Press::Confirm:
            if (!modal)
            {
                return false; // Space is also Jake's jump
            }
            Confirm();
            return true;
        case Press::Back:
            Back();
            return true;
        case Press::Close:
            if (m_hud.HasDocument())
            {
                m_hud.CloseDocument();
                return true;
            }
            return modal;
        case Press::None:
            break;
        }
        return modal;
    }

    void FrontEndSystemComponent::LoadSettings()
    {
        AZ::IO::FileIOBase* fileIO = AZ::IO::FileIOBase::GetInstance();
        AZ::IO::HandleType handle = AZ::IO::InvalidHandle;
        if (!fileIO || !fileIO->Exists(SettingsPath) || !fileIO->Open(SettingsPath, AZ::IO::OpenMode::ModeRead, handle))
        {
            return; // first launch: engine defaults
        }
        AZ::u64 size = 0;
        fileIO->Size(handle, size);
        std::string text(static_cast<std::size_t>(AZStd::min<AZ::u64>(size, 4096)), '\0');
        AZ::u64 read = 0;
        const bool ok = fileIO->Read(handle, text.data(), text.size(), true, &read);
        fileIO->Close(handle);
        std::vector<std::string> errors;
        Core::UserSettings loaded;
        if (!ok || !Core::Settings::Decode(text, loaded, errors))
        {
            AZ_Warning("DarkArisen", false, "Ignoring unreadable settings file %s.", SettingsPath);
            return;
        }
        // A setting only a measurement can grant is never restored without one.
        if (!Core::Settings::SetRayTracing(loaded, loaded.RayTracing, m_capabilities))
        {
            loaded.RayTracing = false;
        }
        m_settings = loaded;
    }

    bool FrontEndSystemComponent::SaveSettings() const
    {
        AZ::IO::FileIOBase* fileIO = AZ::IO::FileIOBase::GetInstance();
        if (!fileIO)
        {
            return false;
        }
        fileIO->CreatePath("@user@/DarkArisen");
        AZ::IO::HandleType handle = AZ::IO::InvalidHandle;
        if (!fileIO->Open(SettingsPath, AZ::IO::OpenMode::ModeWrite | AZ::IO::OpenMode::ModeBinary, handle))
        {
            return false;
        }
        const std::string text = Core::Settings::Encode(m_settings);
        const bool ok = fileIO->Write(handle, text.data(), text.size());
        fileIO->Close(handle);
        return ok;
    }

    void FrontEndSystemComponent::ApplySettings() const
    {
        if (auto* console = AZ::Interface<AZ::IConsole>::Get())
        {
            console->PerformCommand(AZStd::string::format("vsync_interval %d", m_settings.VSync ? 1 : 0).c_str());
            console->PerformCommand("r_resolutionMode 1");
            console->PerformCommand(AZStd::string::format("r_width %d", m_settings.Width).c_str());
            console->PerformCommand(AZStd::string::format("r_height %d", m_settings.Height).c_str());
        }
        AzFramework::NativeWindowHandle window = nullptr;
        AzFramework::WindowSystemRequestBus::BroadcastResult(window, &AzFramework::WindowSystemRequests::GetDefaultWindowHandle);
        if (window)
        {
            const bool fullscreen = m_settings.Window == Core::WindowMode::BorderlessFullscreen;
            AzFramework::WindowRequestBus::Event(window, &AzFramework::WindowRequests::SetFullScreenState, fullscreen);
            if (!fullscreen)
            {
                AzFramework::WindowRequestBus::Event(window, &AzFramework::WindowRequests::ResizeClientArea,
                    AzFramework::WindowSize(static_cast<uint32_t>(m_settings.Width), static_cast<uint32_t>(m_settings.Height)),
                    AzFramework::WindowPosOptions());
            }
        }
        // The graphics preset is stored and shown; no Atom feature consumes it until it is measured on a GPU.
    }

    bool FrontEndSystemComponent::LoadLevel(const char* levelName) const
    {
        auto* console = AZ::Interface<AZ::IConsole>::Get();
        if (!console)
        {
            return false;
        }
        const auto result = console->PerformCommand(AZStd::string::format("LoadLevel %s", levelName).c_str());
        AZ_Error("DarkArisen", result.IsSuccess(), "LoadLevel '%s' failed.", levelName);
        return result.IsSuccess();
    }

    void FrontEndSystemComponent::Draw() const
    {
        auto* fonts = AZ::Interface<AzFramework::FontQueryInterface>::Get();
        AzFramework::FontDrawInterface* font = fonts ? fonts->GetDefaultFontDrawInterface() : nullptr;
        if (!font)
        {
            return;
        }
        const AzFramework::WindowSize screen = ScreenSize();
        const float width = static_cast<float>(screen.m_width);
        const float height = static_cast<float>(screen.m_height);
        const AZ::Color parchment(0.95f, 0.90f, 0.78f, 1.0f);
        const AZ::Color white(1.0f, 1.0f, 1.0f, 1.0f);
        const AZ::Color grey(0.45f, 0.45f, 0.45f, 1.0f);
        using Align = AzFramework::TextHorizontalAlignment;

        if (m_credits.IsRolling())
        {
            const std::string& roll = m_credits.Text();
            const float lines = static_cast<float>(std::count(roll.begin(), roll.end(), '\n') + 1);
            const float lineHeight = 40.0f;
            const float y = height - m_credits.Progress() * (height + lines * lineHeight);
            DrawText(*font, ToAz(roll), width * 0.5f, y, 32.0f, parchment, Align::Center);
            return;
        }
        if (m_menu)
        {
            DrawText(*font, ToAz(std::string(m_menu->Title())), width * 0.08f, height * 0.10f, 56.0f, parchment);
            const std::vector<Core::MenuEntry> entries = m_menu->Entries(m_settings, m_capabilities);
            for (std::size_t index = 0; index < entries.size(); ++index)
            {
                const Core::MenuEntry& entry = entries[index];
                const bool focused = static_cast<int>(index) == m_menu->Focus();
                std::string label = (focused ? "> " : "  ") + entry.Label + (entry.Current ? "  *" : "");
                DrawText(*font, ToAz(label), width * 0.08f, height * (0.24f + 0.055f * static_cast<float>(index)), 30.0f,
                    !entry.Enabled ? grey : focused ? parchment : white);
            }
        }
        if (m_hud.HasDocument())
        {
            DrawText(*font, ToAz(m_hud.DocumentTitle()), width * 0.5f, height * 0.16f, 38.0f, parchment, Align::Center);
            DrawText(*font, ToAz(m_hud.DocumentBody()), width * 0.5f, height * 0.26f, 26.0f, white, Align::Center);
            DrawText(*font, "E / Enter: put it down", width * 0.5f, height * 0.9f, 20.0f, grey, Align::Center);
        }
        if (const Core::HudModel::Subtitle* line = m_hud.CurrentSubtitle())
        {
            const std::string text = line->Speaker.empty() ? line->Text : line->Speaker + ": " + line->Text;
            DrawText(*font, ToAz(text), width * 0.5f, height * 0.86f, 28.0f, white, Align::Center);
        }
        if (!m_hud.SystemMessage().empty())
        {
            DrawText(*font, ToAz(m_hud.SystemMessage()), width * 0.02f, height * 0.03f, 22.0f, parchment);
        }
    }
}
