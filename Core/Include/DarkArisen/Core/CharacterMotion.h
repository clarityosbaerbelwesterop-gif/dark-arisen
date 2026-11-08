#pragma once

#include "DarkArisen/Core/Combat.h"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/**
 * Which motion a character plays, decided from gameplay state alone (engine-neutral).
 *
 * The clips are the gameplay motion library (Assets/Art/Characters/Motions/MotionLibrary.json): each has a
 * state name, a duration, whether it loops, the ground speed of its capture and, for attacks, the time of
 * contact. Each tick the selector picks the state the character is in, how fast to play it (locomotion is
 * scaled to the actual ground speed so the feet do not slide) and how long to blend into it, and reports the
 * contact exactly once per swing for CombatModel::ConsumeQueuedHit. The animation adapter only plays what it
 * is told; it never decides.
 */
namespace DarkArisen::Core
{
    struct MotionClip
    {
        std::string State;
        double Duration = 1.0;
        bool Loop = false;
        double GroundSpeed = 0.0;        // metres per second of the capture; 0 when it stays in place
        std::optional<double> Contact;   // seconds into the clip where the hit lands
    };

    class MotionClipTable
    {
    public:
        void Add(MotionClip Clip);
        const MotionClip* Find(std::string_view State) const;
        std::size_t Size() const { return Clips.size(); }

    private:
        std::vector<MotionClip> Clips;
    };

    enum class MoveDirection : std::uint8_t { Forward, Back, Left, Right };

    struct CharacterMotionInput
    {
        CombatState Combat = CombatState::Idle;
        int LightCombo = 0;              // 0 for the first light attack of a chain, 1 for the second
        int DodgeSide = 0;               // -1 left, 0 forward roll, +1 right (while Dodging)
        double PlanarSpeed = 0.0;        // metres per second over the ground (or through the water)
        bool Sprinting = false;
        bool Guarded = false;            // locked on or weapon raised: guarded combat locomotion
        MoveDirection Direction = MoveDirection::Forward;  // of travel relative to facing, while guarded
        bool Swimming = false;
        bool Struggling = false;         // exhausted or in a current: struggling swim
        bool HitTaken = false;           // a hit landed this tick (one shot)
        bool Dead = false;
    };

    struct CharacterMotionOutput
    {
        std::string State;               // empty only when the table has neither the state nor Idle
        double Time = 0.0;               // seconds into the clip
        double PlayRate = 1.0;
        double BlendSeconds = 0.0;       // blend into this clip when Started
        bool Loop = false;
        bool Started = false;            // a new clip began this tick
        bool Contact = false;            // the clip's contact time passed this tick (once per play)
    };

    class CharacterMotion
    {
    public:
        /** Speed under which a character stands rather than walks (m/s). */
        static constexpr double StandBelow = 0.3;
        /** Speed from which an unguarded character runs rather than walks (m/s). */
        static constexpr double RunFrom = 2.2;
        /** Locomotion play rate is the ground speed ratio, held inside this range. */
        static constexpr double MinRate = 0.5;
        static constexpr double MaxRate = 1.6;

        explicit CharacterMotion(const MotionClipTable& Clips) : Table(Clips) {}

        CharacterMotionOutput Tick(const CharacterMotionInput& In, double DeltaSeconds);

        const std::string& CurrentState() const { return Current; }

        /** The state the input asks for, before fallbacks (exposed for tests and tools). */
        static std::string Select(const CharacterMotionInput& In, bool HitReactionPlaying);

    private:
        const MotionClipTable& Table;
        std::string Current;
        double Time = 0.0;
        bool ContactDone = false;
        bool HitReaction = false;        // a hit reaction is playing out
        CombatState LastCombat = CombatState::Idle;
        int LastCombo = 0;
    };
}
