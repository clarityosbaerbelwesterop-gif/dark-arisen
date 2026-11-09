#pragma once

#include <cstdint>
#include <functional>
#include <span>
#include <string_view>

/**
 * The cinematic register and its player, after `cutscene catalog.md` (Phase 10): the nineteen authored cutscenes
 * (section 3), the five two-second inserts (4), the six slow pushes (5) and the twenty-two moments that are not
 * cutscenes (6). The player keeps control by default (1); the catalog's three non-negotiables (9) hold here:
 * - section 6 is a prohibition list: none of its moments can be registered or started;
 * - there is no slow motion here (the only time scale is Rache, owned by Combat);
 * - music does not react: a cinematic may start a cue, it never stops, layers or changes one.
 *
 * Reconciliations (Docs/DesignAuthority.md canon order):
 * - #1 "The Brother" follows the 2026-09 canon lock, not the catalog's "Ethan's death": Marc and Denise die in
 *   Draven's attack and Ethan is taken alive.
 * - #15-#19 are "specified in" the endgame documents. Until those specifications are reconciled they are registered
 *   without content and cannot start (the omission law: nothing is invented for them).
 * - The catalog's per-entry animation tiers name #9 as P1 as well, ten P1 sequences against section 8.1's nine. The
 *   per-entry values are kept.
 */
namespace DarkArisen::Core
{
    enum class CinematicKind : std::uint8_t { Cutscene, Insert, SlowPush };

    /** camera_system.md section 3 vocabulary, as the catalog uses it. */
    enum class CinematicCamera : std::uint8_t
    {
        StaticHold,        // single position, no coverage
        SlowPush,          // at most 0.15 m/s onto one subject, then release
        AnchoredWide,      // a held wide, then control returns; never a flythrough
        OverShoulderTurn,  // authored over-shoulder for the turn only
        StaticTwoShot,     // two-shot, static, no coverage cuts
        Authored,          // an authored sequence of shots
        Unmoving,          // the camera does not move at all (#11)
    };

    /** What a cinematic starts. None is a specification ("silence"), not an omission. */
    enum class CinematicMusic : std::uint8_t
    {
        None,
        NoneUntilLastEightSeconds,  // #1
        MainThemeFirstStatement,    // #2
        EthanMotifUnresolved,       // #4
        SungDiegetic,               // #5, Mama Jacinta's, diegetic becoming non-diegetic
        BasinWarmMinor,             // #7
        ArionMusiciansDiegetic,     // #8
        RunningCueContinues,        // #11: the rain-dance cue already playing runs on unchanged
    };

    enum class Performance : std::uint8_t { None, P1, P2, P3 };

    struct Cinematic
    {
        std::string_view Id;
        int Number = 0;                   // 1-19 for cutscenes, 1-5 for inserts, 5-6 for standalone pushes
        CinematicKind Kind = CinematicKind::Cutscene;
        std::string_view Title;
        std::string_view Summary;
        std::string_view Source;          // the governing document and section
        CinematicCamera Camera = CinematicCamera::StaticHold;
        float PushMetresPerSecond = 0.0f; // non-zero for pushes; never above MaxPushMetresPerSecond
        CinematicMusic Music = CinematicMusic::None;
        Performance Animation = Performance::None;
        float LengthSeconds = 0.0f;
        float ControlTakenSeconds = 0.0f; // the player has control again after this, even if the shot runs on
        bool Specified = true;            // false: registered, awaiting its governing specification; never plays
        std::string_view RequiresFact;    // a story fact the cinematic needs (empty: none)
        bool NewGamePlusOnly = false;
    };

    inline constexpr float MaxPushMetresPerSecond = 0.15f;
    inline constexpr float InsertSeconds = 2.0f;

    /** Every registered cinematic: 19 cutscenes, 5 inserts and the 2 slow pushes that are not part of a cutscene. */
    std::span<const Cinematic> CinematicRegister();
    const Cinematic* FindCinematic(std::string_view Id);
    /** The twenty-two moments of section 6, by id. */
    std::span<const std::string_view> ProhibitedCinematicMoments();
    bool IsProhibitedCinematicMoment(std::string_view Id);

    enum class CinematicStart : std::uint8_t { Started, Unknown, Prohibited, NotSpecified, ConditionUnmet, AlreadyPlaying };

    struct CinematicFrame
    {
        bool Playing = false;
        bool PlayerHasControl = true;
        CinematicCamera Camera = CinematicCamera::StaticHold;
        float Elapsed = 0.0f;
        float PushDistance = 0.0f;  // metres the camera has pushed in
        bool Finished = false;      // true on the frame the cinematic ends
    };

    /**
     * Plays one cinematic in real time. Start reports the music cue to start (MusicToStart), once; nothing here
     * stops music. Tick takes real seconds; there is no time scale.
     */
    class CinematicPlayer
    {
    public:
        using FactQuery = std::function<bool(std::string_view)>;

        CinematicStart Start(std::string_view Id, const FactQuery& HasFact, bool NewGamePlus = false);
        CinematicFrame Tick(float RealDeltaSeconds);

        bool IsPlaying() const { return Current != nullptr; }
        const Cinematic* GetCurrent() const { return Current; }
        CinematicMusic MusicToStart() const { return StartedMusic; }

    private:
        const Cinematic* Current = nullptr;
        float Elapsed = 0.0f;
        CinematicMusic StartedMusic = CinematicMusic::None;
    };
}
