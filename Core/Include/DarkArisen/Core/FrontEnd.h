#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace DarkArisen::Core
{
    /** Ordinals match the Unreal overall scalability levels 0-3 and are persisted. */
    enum class GraphicsPreset : std::uint8_t { Low, Medium, High, Epic };

    /** The Unreal menu toggled between a window and windowed fullscreen. */
    enum class WindowMode : std::uint8_t { Windowed, BorderlessFullscreen };

    /** Per-machine user settings (never part of the campaign save). Defaults are the engine's. */
    struct UserSettings
    {
        static constexpr int CurrentVersion = 1;
        static constexpr int MinimumWidth = 1280;  // UE SetResolutionPreset clamp
        static constexpr int MinimumHeight = 720;
        static constexpr int MaximumWidth = 7680;
        static constexpr int MaximumHeight = 4320;

        bool VSync = true;
        WindowMode Window = WindowMode::Windowed;
        int Width = 1920;
        int Height = 1080;
        GraphicsPreset Preset = GraphicsPreset::High;
        /** Only ever on after a measurement on this machine proved it fits the frame budget. */
        bool RayTracing = false;
        bool operator==(const UserSettings&) const = default;
    };

    /**
     * What this machine is known to do. No upscaler exists in O3DE 2605.0 and none is offered; ray
     * tracing is offered only once a measured frame time with it enabled fits the budget.
     */
    struct RenderCapabilities
    {
        bool HardwareRayTracing = false;
        /** Frame time measured here with ray tracing on; 0 means never measured. */
        double MeasuredRayTracedFrameMs = 0.0;
        double FrameBudgetMs = 1000.0 / 60.0;
    };

    namespace Settings
    {
        /** The Unreal menu's resolution buttons: 1080p, 1440p and 4K. */
        const std::vector<std::pair<int, int>>& ResolutionPresets();
        bool Validate(const UserSettings& Settings, std::vector<std::string>& OutErrors);
        bool RayTracingAllowed(const RenderCapabilities& Capabilities);
        /** Enabling fails (nothing changes) unless RayTracingAllowed; disabling always succeeds. */
        bool SetRayTracing(UserSettings& Settings, bool Enabled, const RenderCapabilities& Capabilities);
        /** UE rule: clamp to at least 1280 x 720 and switch to borderless fullscreen. */
        bool SetResolution(UserSettings& Settings, int Width, int Height);
        std::string_view PresetName(GraphicsPreset Preset);

        /** Line-based "key=value" file with a version line; unknown keys and bad values fail. */
        std::string Encode(const UserSettings& Settings);
        bool Decode(std::string_view Text, UserSettings& OutSettings, std::vector<std::string>& OutErrors);
    }

    enum class MenuPage : std::uint8_t { Main, Pause, Settings };

    enum class MenuItem : std::uint8_t
    {
        NewGame, Continue, OpenSettings, Quit,              // main menu
        Resume, SaveGame, LoadGame, ReturnToMainMenu,       // pause menu (plus OpenSettings)
        VSync, PresetLow, PresetMedium, PresetHigh, PresetEpic, DisplayMode,
        Resolution1080, Resolution1440, Resolution2160, RayTracing, Back
    };

    struct MenuEntry
    {
        MenuItem Item = MenuItem::Back;
        std::string Label;
        bool Enabled = true;
        /** The current choice among alternatives (the active preset or resolution). */
        bool Current = false;
    };

    enum class MenuCommand : std::uint8_t
    {
        None, StartNewGame, ContinueGame, Quit, Resume, SaveGame, LoadGame, ReturnToMainMenu, ApplySettings
    };

    /**
     * Port of AAlphaMenuPlayerController/AAlphaMenuHUD (main menu) and the pause overlay of
     * AAlphaGameplayPlayerController/APostureOnlyHUD. Keyboard and gamepad navigate by focus; a
     * disabled entry (Continue without a save, unmeasured ray tracing) can never be activated.
     * No minimap and no world map are offered (owner authority pending).
     */
    class FrontEndMenu
    {
    public:
        static FrontEndMenu MainMenu(bool HasContinueSave);
        static FrontEndMenu PauseMenu(bool HasSave);

        MenuPage Page() const { return Pages.back(); }
        std::string_view Title() const;
        std::vector<MenuEntry> Entries(const UserSettings& Settings, const RenderCapabilities& Capabilities) const;
        int Focus() const { return FocusIndex; }
        /** Moves to the next enabled entry, wrapping. */
        void MoveFocus(int Delta, const UserSettings& Settings, const RenderCapabilities& Capabilities);
        /** Activates the focused entry; settings entries change Settings and return ApplySettings. */
        MenuCommand Activate(UserSettings& Settings, const RenderCapabilities& Capabilities);
        /** Escape / gamepad B: leaves Settings, resumes from Pause, does nothing on the main page. */
        MenuCommand Cancel();
        void SetHasSave(bool HasSave) { SaveExists = HasSave; }

    private:
        std::vector<MenuPage> Pages;
        int FocusIndex = 0;
        bool SaveExists = false;

        void Push(MenuPage Page);
        void FocusFirstEnabled(const UserSettings& Settings, const RenderCapabilities& Capabilities);
    };

    /** Port of ACreditsPresentationActor: a 75 s roll, skippable after 8 s, then the front end. */
    class CreditsRoll
    {
    public:
        static constexpr float DefaultRollSeconds = 75.0f;
        static constexpr float DefaultAllowSkipAfterSeconds = 8.0f;

        void Start(std::string RollText, float AllowSkipAfterSeconds = DefaultAllowSkipAfterSeconds, float RollSeconds = DefaultRollSeconds);
        void Tick(float DeltaSeconds);
        /** Accepted only once the skip delay has passed. */
        bool Skip();
        bool IsRolling() const { return Rolling; }
        bool IsFinished() const { return Finished; }
        /** 0 at the start, 1 when the last line has scrolled past. */
        float Progress() const;
        const std::string& Text() const { return Roll; }

    private:
        std::string Roll;
        float Elapsed = 0.0f;
        float SkipAfter = DefaultAllowSkipAfterSeconds;
        float Duration = DefaultRollSeconds;
        bool Rolling = false;
        bool Finished = false;
    };

    /**
     * What the screen shows over gameplay: the current subtitle, a document Jake is reading and a
     * one-line system message. A new line replaces the current one (presentations own their timing);
     * each stays long enough to read (minimum plus per-character time). A document stays until Jake
     * closes it.
     */
    class HudModel
    {
    public:
        static constexpr float MinimumSubtitleSeconds = 2.5f;
        static constexpr float SecondsPerCharacter = 0.06f;
        static constexpr float SystemMessageSeconds = 3.0f;

        struct Subtitle
        {
            std::string Speaker;
            std::string Text;
            float Remaining = 0.0f;
        };

        static float ReadingSeconds(std::string_view Text);
        void ShowSubtitle(std::string Speaker, std::string Text);
        void OpenDocument(std::string Title, std::string Body);
        /** Returns false when no document was open. */
        bool CloseDocument();
        void ShowSystemMessage(std::string Message);
        /** A level change ends the line being spoken and puts the document down. */
        void ClearLevelPresentation();
        void Tick(float DeltaSeconds);

        const Subtitle* CurrentSubtitle() const { return Line ? &*Line : nullptr; }
        bool HasDocument() const { return DocumentOpen; }
        const std::string& DocumentTitle() const { return Title; }
        const std::string& DocumentBody() const { return Body; }
        const std::string& SystemMessage() const { return Message; }

    private:
        std::optional<Subtitle> Line;
        bool DocumentOpen = false;
        std::string Title;
        std::string Body;
        std::string Message;
        float MessageRemaining = 0.0f;
    };
}
