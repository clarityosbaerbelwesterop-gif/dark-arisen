#include "DarkArisen/Core/FrontEnd.h"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <map>

namespace DarkArisen::Core
{
    namespace
    {
        constexpr std::string_view SettingsHeader = "DarkArisenSettings";

        bool ParseInt(const std::string_view Text, int& Out)
        {
            const auto Result = std::from_chars(Text.data(), Text.data() + Text.size(), Out);
            return Result.ec == std::errc() && Result.ptr == Text.data() + Text.size();
        }
    }

    namespace Settings
    {
        const std::vector<std::pair<int, int>>& ResolutionPresets()
        {
            static const std::vector<std::pair<int, int>> Presets = {{1920, 1080}, {2560, 1440}, {3840, 2160}};
            return Presets;
        }

        bool Validate(const UserSettings& Settings, std::vector<std::string>& OutErrors)
        {
            const std::size_t Before = OutErrors.size();
            if (Settings.Width < UserSettings::MinimumWidth || Settings.Width > UserSettings::MaximumWidth ||
                Settings.Height < UserSettings::MinimumHeight || Settings.Height > UserSettings::MaximumHeight)
            {
                OutErrors.emplace_back("Resolution is outside 1280 x 720 .. 7680 x 4320.");
            }
            if (Settings.Preset > GraphicsPreset::Epic || Settings.Window > WindowMode::BorderlessFullscreen)
            {
                OutErrors.emplace_back("Unknown graphics preset or window mode.");
            }
            return OutErrors.size() == Before;
        }

        bool RayTracingAllowed(const RenderCapabilities& Capabilities)
        {
            return Capabilities.HardwareRayTracing && std::isfinite(Capabilities.MeasuredRayTracedFrameMs) &&
                Capabilities.MeasuredRayTracedFrameMs > 0.0 && Capabilities.MeasuredRayTracedFrameMs <= Capabilities.FrameBudgetMs;
        }

        bool SetRayTracing(UserSettings& Settings, const bool Enabled, const RenderCapabilities& Capabilities)
        {
            if (Enabled && !RayTracingAllowed(Capabilities)) return false;
            Settings.RayTracing = Enabled;
            return true;
        }

        bool SetResolution(UserSettings& Settings, const int Width, const int Height)
        {
            if (Width <= 0 || Height <= 0 || Width > UserSettings::MaximumWidth || Height > UserSettings::MaximumHeight) return false;
            Settings.Width = std::max(UserSettings::MinimumWidth, Width);
            Settings.Height = std::max(UserSettings::MinimumHeight, Height);
            Settings.Window = WindowMode::BorderlessFullscreen;
            return true;
        }

        std::string_view PresetName(const GraphicsPreset Preset)
        {
            switch (Preset)
            {
            case GraphicsPreset::Low: return "low";
            case GraphicsPreset::Medium: return "medium";
            case GraphicsPreset::High: return "high";
            case GraphicsPreset::Epic: break;
            }
            return "epic";
        }

        std::string Encode(const UserSettings& Settings)
        {
            std::string Out = std::string(SettingsHeader) + " " + std::to_string(UserSettings::CurrentVersion) + "\n";
            Out += "vsync=" + std::string(Settings.VSync ? "1" : "0") + "\n";
            Out += "window=" + std::string(Settings.Window == WindowMode::Windowed ? "windowed" : "borderless") + "\n";
            Out += "width=" + std::to_string(Settings.Width) + "\n";
            Out += "height=" + std::to_string(Settings.Height) + "\n";
            Out += "preset=" + std::string(PresetName(Settings.Preset)) + "\n";
            Out += "raytracing=" + std::string(Settings.RayTracing ? "1" : "0") + "\n";
            return Out;
        }

        bool Decode(const std::string_view Text, UserSettings& OutSettings, std::vector<std::string>& OutErrors)
        {
            OutErrors.clear();
            std::map<std::string, std::string, std::less<>> Values;
            std::size_t Start = 0;
            bool HeaderSeen = false;
            while (Start < Text.size())
            {
                std::size_t End = Text.find('\n', Start);
                if (End == std::string_view::npos) End = Text.size();
                std::string_view Line = Text.substr(Start, End - Start);
                Start = End + 1;
                if (!Line.empty() && Line.back() == '\r') Line.remove_suffix(1);
                if (Line.empty()) continue;
                if (!HeaderSeen)
                {
                    HeaderSeen = true;
                    if (Line != std::string(SettingsHeader) + " " + std::to_string(UserSettings::CurrentVersion))
                    {
                        OutErrors.emplace_back("Settings file has an unknown header or version.");
                        return false;
                    }
                    continue;
                }
                const std::size_t Equals = Line.find('=');
                if (Equals == std::string_view::npos || !Values.emplace(std::string(Line.substr(0, Equals)), std::string(Line.substr(Equals + 1))).second)
                {
                    OutErrors.push_back("Malformed or repeated settings line: " + std::string(Line));
                    return false;
                }
            }
            UserSettings Candidate;
            const auto Take = [&Values](const std::string_view Key) -> const std::string*
            {
                const auto It = Values.find(Key);
                return It == Values.end() ? nullptr : &It->second;
            };
            const std::string* VSync = Take("vsync");
            const std::string* Window = Take("window");
            const std::string* Width = Take("width");
            const std::string* Height = Take("height");
            const std::string* Preset = Take("preset");
            const std::string* Ray = Take("raytracing");
            const auto Flag = [](const std::string* Value, bool& Out)
            {
                if (Value == nullptr || (*Value != "0" && *Value != "1")) return false;
                Out = *Value == "1";
                return true;
            };
            bool Valid = HeaderSeen && Values.size() == 6 && Flag(VSync, Candidate.VSync) && Flag(Ray, Candidate.RayTracing) &&
                Width != nullptr && ParseInt(*Width, Candidate.Width) && Height != nullptr && ParseInt(*Height, Candidate.Height) &&
                Window != nullptr && (*Window == "windowed" || *Window == "borderless");
            if (Valid) Candidate.Window = *Window == "windowed" ? WindowMode::Windowed : WindowMode::BorderlessFullscreen;
            bool PresetKnown = false;
            for (const GraphicsPreset Option : {GraphicsPreset::Low, GraphicsPreset::Medium, GraphicsPreset::High, GraphicsPreset::Epic})
            {
                if (Preset != nullptr && *Preset == PresetName(Option))
                {
                    Candidate.Preset = Option;
                    PresetKnown = true;
                }
            }
            Valid = Valid && PresetKnown;
            if (!Valid)
            {
                OutErrors.emplace_back("Settings file is missing, repeats or misspells a value.");
                return false;
            }
            if (!Validate(Candidate, OutErrors)) return false;
            OutSettings = Candidate;
            return true;
        }
    }

    FrontEndMenu FrontEndMenu::MainMenu(const bool HasContinueSave)
    {
        FrontEndMenu Menu;
        Menu.Pages = {MenuPage::Main};
        Menu.SaveExists = HasContinueSave;
        return Menu;
    }

    FrontEndMenu FrontEndMenu::PauseMenu(const bool HasSave)
    {
        FrontEndMenu Menu;
        Menu.Pages = {MenuPage::Pause};
        Menu.SaveExists = HasSave;
        return Menu;
    }

    std::string_view FrontEndMenu::Title() const
    {
        switch (Page())
        {
        case MenuPage::Main: return "DARK ARISEN";
        case MenuPage::Pause: return "PAUSED";
        case MenuPage::Settings: break;
        }
        return "SETTINGS";
    }

    std::vector<MenuEntry> FrontEndMenu::Entries(const UserSettings& Settings, const RenderCapabilities& Capabilities) const
    {
        switch (Page())
        {
        case MenuPage::Main:
            return {{MenuItem::NewGame, "NEW GAME", true, false}, {MenuItem::Continue, "CONTINUE", SaveExists, false},
                {MenuItem::OpenSettings, "OPTIONS / SETTINGS", true, false}, {MenuItem::Quit, "QUIT", true, false}};
        case MenuPage::Pause:
            return {{MenuItem::Resume, "RESUME", true, false}, {MenuItem::SaveGame, "SAVE GAME", true, false},
                {MenuItem::LoadGame, "LOAD GAME", SaveExists, false}, {MenuItem::OpenSettings, "SETTINGS", true, false},
                {MenuItem::ReturnToMainMenu, "MAIN MENU", true, false}};
        case MenuPage::Settings:
            break;
        }
        const auto IsResolution = [&Settings](const int Index)
        {
            const auto& [Width, Height] = Settings::ResolutionPresets()[static_cast<std::size_t>(Index)];
            return Settings.Width == Width && Settings.Height == Height;
        };
        const bool RayAllowed = Settings::RayTracingAllowed(Capabilities);
        std::string RayLabel = Settings.RayTracing ? "RAY TRACING: ON" : "RAY TRACING: OFF";
        if (!RayAllowed && !Settings.RayTracing) RayLabel = "RAY TRACING: NOT MEASURED ON THIS MACHINE";
        return {{MenuItem::VSync, Settings.VSync ? "VSYNC: ON" : "VSYNC: OFF", true, false},
            {MenuItem::PresetLow, "GRAPHICS LOW", true, Settings.Preset == GraphicsPreset::Low},
            {MenuItem::PresetMedium, "GRAPHICS MEDIUM", true, Settings.Preset == GraphicsPreset::Medium},
            {MenuItem::PresetHigh, "GRAPHICS HIGH", true, Settings.Preset == GraphicsPreset::High},
            {MenuItem::PresetEpic, "GRAPHICS EPIC", true, Settings.Preset == GraphicsPreset::Epic},
            {MenuItem::DisplayMode, Settings.Window == WindowMode::Windowed ? "DISPLAY: WINDOWED" : "DISPLAY: FULLSCREEN", true, false},
            {MenuItem::Resolution1080, "1920 x 1080", true, IsResolution(0)},
            {MenuItem::Resolution1440, "2560 x 1440", true, IsResolution(1)},
            {MenuItem::Resolution2160, "3840 x 2160 / 4K", true, IsResolution(2)},
            // Turning it off is always possible; turning it on needs a measurement.
            {MenuItem::RayTracing, RayLabel, RayAllowed || Settings.RayTracing, Settings.RayTracing},
            {MenuItem::Back, "BACK", true, false}};
    }

    void FrontEndMenu::MoveFocus(const int Delta, const UserSettings& Settings, const RenderCapabilities& Capabilities)
    {
        const std::vector<MenuEntry> Items = Entries(Settings, Capabilities);
        const int Count = static_cast<int>(Items.size());
        if (Count == 0 || Delta == 0) return;
        const int Step = Delta > 0 ? 1 : -1;
        for (int Moves = std::abs(Delta); Moves > 0; --Moves)
        {
            int Next = FocusIndex;
            for (int Tries = 0; Tries < Count; ++Tries)
            {
                Next = (Next + Step + Count) % Count;
                if (Items[static_cast<std::size_t>(Next)].Enabled) break;
            }
            FocusIndex = Next;
        }
    }

    void FrontEndMenu::FocusFirstEnabled(const UserSettings& Settings, const RenderCapabilities& Capabilities)
    {
        const std::vector<MenuEntry> Items = Entries(Settings, Capabilities);
        const auto It = std::find_if(Items.begin(), Items.end(), [](const MenuEntry& Entry) { return Entry.Enabled; });
        FocusIndex = It == Items.end() ? 0 : static_cast<int>(It - Items.begin());
    }

    void FrontEndMenu::Push(const MenuPage Page)
    {
        Pages.push_back(Page);
        FocusIndex = 0;
    }

    MenuCommand FrontEndMenu::Activate(UserSettings& Settings, const RenderCapabilities& Capabilities)
    {
        const std::vector<MenuEntry> Items = Entries(Settings, Capabilities);
        if (FocusIndex < 0 || FocusIndex >= static_cast<int>(Items.size()) || !Items[static_cast<std::size_t>(FocusIndex)].Enabled)
        {
            return MenuCommand::None;
        }
        const auto Preset = [&Settings](const GraphicsPreset Value)
        {
            Settings.Preset = Value;
            return MenuCommand::ApplySettings;
        };
        const auto Resolution = [&Settings](const std::size_t Index)
        {
            const auto& [Width, Height] = Settings::ResolutionPresets()[Index];
            return Settings::SetResolution(Settings, Width, Height) ? MenuCommand::ApplySettings : MenuCommand::None;
        };
        switch (Items[static_cast<std::size_t>(FocusIndex)].Item)
        {
        case MenuItem::NewGame: return MenuCommand::StartNewGame;
        case MenuItem::Continue: return MenuCommand::ContinueGame;
        case MenuItem::Quit: return MenuCommand::Quit;
        case MenuItem::Resume: return MenuCommand::Resume;
        case MenuItem::SaveGame: return MenuCommand::SaveGame;
        case MenuItem::LoadGame: return MenuCommand::LoadGame;
        case MenuItem::ReturnToMainMenu: return MenuCommand::ReturnToMainMenu;
        case MenuItem::OpenSettings:
            Push(MenuPage::Settings);
            FocusFirstEnabled(Settings, Capabilities);
            return MenuCommand::None;
        case MenuItem::VSync:
            Settings.VSync = !Settings.VSync;
            return MenuCommand::ApplySettings;
        case MenuItem::PresetLow: return Preset(GraphicsPreset::Low);
        case MenuItem::PresetMedium: return Preset(GraphicsPreset::Medium);
        case MenuItem::PresetHigh: return Preset(GraphicsPreset::High);
        case MenuItem::PresetEpic: return Preset(GraphicsPreset::Epic);
        case MenuItem::DisplayMode:
            Settings.Window = Settings.Window == WindowMode::Windowed ? WindowMode::BorderlessFullscreen : WindowMode::Windowed;
            return MenuCommand::ApplySettings;
        case MenuItem::Resolution1080: return Resolution(0);
        case MenuItem::Resolution1440: return Resolution(1);
        case MenuItem::Resolution2160: return Resolution(2);
        case MenuItem::RayTracing:
            return Settings::SetRayTracing(Settings, !Settings.RayTracing, Capabilities) ? MenuCommand::ApplySettings : MenuCommand::None;
        case MenuItem::Back: return Cancel();
        }
        return MenuCommand::None;
    }

    MenuCommand FrontEndMenu::Cancel()
    {
        if (Page() == MenuPage::Settings)
        {
            Pages.pop_back();
            FocusIndex = 0;
            return MenuCommand::None;
        }
        return Page() == MenuPage::Pause ? MenuCommand::Resume : MenuCommand::None;
    }

    void CreditsRoll::Start(std::string RollText, const float AllowSkipAfterSeconds, const float RollSeconds)
    {
        Roll = std::move(RollText);
        Elapsed = 0.0f;
        SkipAfter = std::isfinite(AllowSkipAfterSeconds) ? std::max(0.0f, AllowSkipAfterSeconds) : DefaultAllowSkipAfterSeconds;
        Duration = std::isfinite(RollSeconds) ? std::max(1.0f, RollSeconds) : DefaultRollSeconds;
        Finished = false;
        Rolling = !Roll.empty(); // nothing to roll fails closed
    }

    void CreditsRoll::Tick(const float DeltaSeconds)
    {
        if (!Rolling) return;
        Elapsed += std::isfinite(DeltaSeconds) ? std::max(0.0f, DeltaSeconds) : 0.0f;
        if (Elapsed >= Duration)
        {
            Rolling = false;
            Finished = true;
        }
    }

    bool CreditsRoll::Skip()
    {
        if (!Rolling || Elapsed < SkipAfter) return false;
        Rolling = false;
        Finished = true;
        return true;
    }

    float CreditsRoll::Progress() const
    {
        return Finished ? 1.0f : std::clamp(Elapsed / Duration, 0.0f, 1.0f);
    }

    float HudModel::ReadingSeconds(const std::string_view Text)
    {
        return std::max(MinimumSubtitleSeconds, static_cast<float>(Text.size()) * SecondsPerCharacter);
    }

    void HudModel::ShowSubtitle(std::string Speaker, std::string Text)
    {
        if (Text.empty()) return;
        const float Seconds = ReadingSeconds(Text);
        Line = Subtitle{std::move(Speaker), std::move(Text), Seconds};
    }

    void HudModel::OpenDocument(std::string DocumentTitle, std::string DocumentBody)
    {
        Title = std::move(DocumentTitle);
        Body = std::move(DocumentBody);
        DocumentOpen = !Title.empty() || !Body.empty();
    }

    bool HudModel::CloseDocument()
    {
        if (!DocumentOpen) return false;
        DocumentOpen = false;
        Title.clear();
        Body.clear();
        return true;
    }

    void HudModel::ShowSystemMessage(std::string Text)
    {
        Message = std::move(Text);
        MessageRemaining = Message.empty() ? 0.0f : SystemMessageSeconds;
    }

    void HudModel::ClearLevelPresentation()
    {
        Line.reset();
        CloseDocument();
    }

    void HudModel::Tick(const float DeltaSeconds)
    {
        const float Delta = std::isfinite(DeltaSeconds) ? std::max(0.0f, DeltaSeconds) : 0.0f;
        if (Line && (Line->Remaining -= Delta) <= 0.0f) Line.reset();
        if (!Message.empty() && (MessageRemaining -= Delta) <= 0.0f) Message.clear();
    }
}
