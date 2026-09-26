#include "TestHarness.h"

#include "DarkArisen/Core/FrontEnd.h"

#include <algorithm>

using namespace DarkArisen::Core;

namespace
{
    std::vector<std::string> Labels(const FrontEndMenu& Menu, const UserSettings& Settings, const RenderCapabilities& Caps)
    {
        std::vector<std::string> Out;
        for (const MenuEntry& Entry : Menu.Entries(Settings, Caps)) Out.push_back(Entry.Label);
        return Out;
    }

    void FocusOn(FrontEndMenu& Menu, const MenuItem Item, const UserSettings& Settings, const RenderCapabilities& Caps)
    {
        for (int Tries = 0; Tries < 20; ++Tries)
        {
            if (Menu.Entries(Settings, Caps)[static_cast<std::size_t>(Menu.Focus())].Item == Item) return;
            Menu.MoveFocus(1, Settings, Caps);
        }
    }
}

TEST_CASE("Front end: the Unreal main menu; Continue only with a save")
{
    UserSettings Settings;
    const RenderCapabilities Caps;
    FrontEndMenu Fresh = FrontEndMenu::MainMenu(false);
    CHECK(Fresh.Title() == "DARK ARISEN");
    CHECK(Labels(Fresh, Settings, Caps) == std::vector<std::string>({"NEW GAME", "CONTINUE", "OPTIONS / SETTINGS", "QUIT"}));
    Fresh.MoveFocus(1, Settings, Caps);
    CHECK_EQ(Fresh.Focus(), 2); // Continue is skipped without a save
    Fresh.MoveFocus(-2, Settings, Caps);
    CHECK_EQ(Fresh.Focus(), 3); // wraps past the disabled entry
    CHECK(Fresh.Activate(Settings, Caps) == MenuCommand::Quit);

    FrontEndMenu Saved = FrontEndMenu::MainMenu(true);
    CHECK(Saved.Activate(Settings, Caps) == MenuCommand::StartNewGame);
    Saved.MoveFocus(1, Settings, Caps);
    CHECK(Saved.Activate(Settings, Caps) == MenuCommand::ContinueGame);
    CHECK(Saved.Cancel() == MenuCommand::None);
}

TEST_CASE("Front end: settings change only what they name; ray tracing needs a measurement")
{
    UserSettings Settings;
    RenderCapabilities Caps;
    FrontEndMenu Menu = FrontEndMenu::MainMenu(false);
    FocusOn(Menu, MenuItem::OpenSettings, Settings, Caps);
    CHECK(Menu.Activate(Settings, Caps) == MenuCommand::None);
    CHECK(Menu.Page() == MenuPage::Settings);

    FocusOn(Menu, MenuItem::PresetEpic, Settings, Caps);
    CHECK(Menu.Activate(Settings, Caps) == MenuCommand::ApplySettings);
    CHECK(Settings.Preset == GraphicsPreset::Epic);
    FocusOn(Menu, MenuItem::Resolution2160, Settings, Caps);
    CHECK(Menu.Activate(Settings, Caps) == MenuCommand::ApplySettings);
    CHECK(Settings.Width == 3840 && Settings.Height == 2160 && Settings.Window == WindowMode::BorderlessFullscreen);
    FocusOn(Menu, MenuItem::VSync, Settings, Caps);
    CHECK(Menu.Activate(Settings, Caps) == MenuCommand::ApplySettings);
    CHECK(!Settings.VSync);

    const auto Entries = Menu.Entries(Settings, Caps);
    const auto Ray = std::find_if(Entries.begin(), Entries.end(), [](const MenuEntry& Entry) { return Entry.Item == MenuItem::RayTracing; });
    CHECK(Ray != Entries.end() && !Ray->Enabled && Ray->Label == "RAY TRACING: NOT MEASURED ON THIS MACHINE");
    CHECK(!Settings::SetRayTracing(Settings, true, Caps));
    Caps.HardwareRayTracing = true;
    Caps.MeasuredRayTracedFrameMs = 21.0; // measured, but over a 60 Hz budget
    CHECK(!Settings::SetRayTracing(Settings, true, Caps));
    Caps.MeasuredRayTracedFrameMs = 14.2;
    FocusOn(Menu, MenuItem::RayTracing, Settings, Caps);
    CHECK(Menu.Activate(Settings, Caps) == MenuCommand::ApplySettings);
    CHECK(Settings.RayTracing);

    CHECK(Menu.Cancel() == MenuCommand::None);
    CHECK(Menu.Page() == MenuPage::Main);
    CHECK(!Settings::SetResolution(Settings, 0, 1080));
    CHECK(Settings::SetResolution(Settings, 800, 600));
    CHECK(Settings.Width == 1280 && Settings.Height == 720); // the Unreal clamp
}

TEST_CASE("Front end: pause offers resume, save, load, settings and the main menu")
{
    UserSettings Settings;
    const RenderCapabilities Caps;
    FrontEndMenu Pause = FrontEndMenu::PauseMenu(false);
    CHECK(Pause.Title() == "PAUSED");
    CHECK(Labels(Pause, Settings, Caps) == std::vector<std::string>({"RESUME", "SAVE GAME", "LOAD GAME", "SETTINGS", "MAIN MENU"}));
    CHECK(Pause.Cancel() == MenuCommand::Resume);
    Pause.MoveFocus(1, Settings, Caps);
    CHECK(Pause.Activate(Settings, Caps) == MenuCommand::SaveGame);
    Pause.MoveFocus(1, Settings, Caps);
    CHECK(Pause.Entries(Settings, Caps)[static_cast<std::size_t>(Pause.Focus())].Item == MenuItem::OpenSettings);
    Pause.SetHasSave(true);
    Pause.MoveFocus(-1, Settings, Caps);
    CHECK(Pause.Activate(Settings, Caps) == MenuCommand::LoadGame);
    FocusOn(Pause, MenuItem::ReturnToMainMenu, Settings, Caps);
    CHECK(Pause.Activate(Settings, Caps) == MenuCommand::ReturnToMainMenu);
    for (const std::string& Label : Labels(Pause, Settings, Caps)) CHECK(Label.find("MAP") == std::string::npos); // no minimap, no map
}

TEST_CASE("Front end: settings file round trip fails closed")
{
    UserSettings Settings;
    Settings.VSync = false;
    Settings.Preset = GraphicsPreset::Low;
    CHECK(Settings::SetResolution(Settings, 2560, 1440));
    UserSettings Decoded;
    std::vector<std::string> Errors;
    CHECK(Settings::Decode(Settings::Encode(Settings), Decoded, Errors));
    CHECK(Decoded == Settings);

    UserSettings Untouched;
    CHECK(!Settings::Decode("DarkArisenSettings 2\nvsync=1\n", Untouched, Errors));
    CHECK(!Settings::Decode(Settings::Encode(Settings) + "upscaler=dlss5\n", Untouched, Errors));
    std::string Tiny = Settings::Encode(Settings);
    Tiny.replace(Tiny.find("width=2560"), 10, "width=320");
    CHECK(!Settings::Decode(Tiny, Untouched, Errors));
    CHECK(Untouched == UserSettings{});
}

TEST_CASE("Credits roll for 75 s, skippable after 8 s; subtitles stay long enough to read")
{
    CreditsRoll Credits;
    Credits.Start("");
    CHECK(!Credits.IsRolling()); // no text, no roll
    Credits.Start("DARK ARISEN\nO3DE 2605.0");
    CHECK(!Credits.Skip());
    Credits.Tick(8.0f);
    CHECK(Credits.Skip() && Credits.IsFinished());
    Credits.Start("DARK ARISEN");
    for (int Second = 0; Second < 74; ++Second) Credits.Tick(1.0f);
    CHECK(Credits.IsRolling() && Credits.Progress() > 0.98f);
    Credits.Tick(1.0f);
    CHECK(Credits.IsFinished());

    HudModel Hud;
    Hud.ShowSubtitle("Mira", "Hold the line.");
    CHECK(Hud.CurrentSubtitle() && Hud.CurrentSubtitle()->Speaker == "Mira");
    Hud.Tick(2.4f);
    CHECK(Hud.CurrentSubtitle() != nullptr);
    Hud.Tick(0.2f);
    CHECK(Hud.CurrentSubtitle() == nullptr);
    const std::string Long(100, 'a');
    CHECK_NEAR(HudModel::ReadingSeconds(Long), 6.0, 1e-4);
    Hud.OpenDocument("Surveyor's report", "The seam runs north.");
    Hud.Tick(60.0f);
    CHECK(Hud.HasDocument()); // a document waits for Jake
    CHECK(Hud.CloseDocument() && !Hud.CloseDocument());
    Hud.ShowSubtitle("Koa", "Mind the reef.");
    Hud.OpenDocument("Ledger", "Page one.");
    Hud.ClearLevelPresentation();
    CHECK(Hud.CurrentSubtitle() == nullptr && !Hud.HasDocument());
}
