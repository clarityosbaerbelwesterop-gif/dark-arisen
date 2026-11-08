#include "DarkArisen/Core/CharacterMotion.h"

#include <algorithm>

namespace DarkArisen::Core
{
    void MotionClipTable::Add(MotionClip Clip)
    {
        for (MotionClip& Existing : Clips)
        {
            if (Existing.State == Clip.State)
            {
                Existing = std::move(Clip);
                return;
            }
        }
        Clips.push_back(std::move(Clip));
    }

    const MotionClip* MotionClipTable::Find(const std::string_view State) const
    {
        for (const MotionClip& Clip : Clips)
        {
            if (Clip.State == State) return &Clip;
        }
        return nullptr;
    }

    namespace
    {
        bool IsAction(const CombatState State)
        {
            switch (State)
            {
            case CombatState::LightAttacking:
            case CombatState::HeavyAttacking:
            case CombatState::Parrying:
            case CombatState::Dodging:
            case CombatState::Backstepping:
            case CombatState::Staggered:
                return true;
            default:
                return false;
            }
        }

        /** Blend into a clip: actions snap in, the body settles between locomotion states. */
        double BlendFor(const std::string& State, const CharacterMotionInput& In)
        {
            if (State == "Death") return 0.15;
            if (State == "HitReactionFront" || State == "Stagger") return 0.06;
            if (IsAction(In.Combat)) return 0.08;
            if (In.Swimming) return 0.35;
            return 0.2;
        }
    }

    std::string CharacterMotion::Select(const CharacterMotionInput& In, const bool HitReactionPlaying)
    {
        if (In.Dead) return "Death";
        switch (In.Combat)
        {
        case CombatState::Staggered: return "Stagger";
        case CombatState::LightAttacking: return In.LightCombo % 2 == 0 ? "LightAttack1" : "LightAttack2";
        case CombatState::HeavyAttacking: return "HeavyAttack";
        case CombatState::Parrying: return "Deflect";
        case CombatState::Dodging: return In.DodgeSide < 0 ? "DodgeLeft" : In.DodgeSide > 0 ? "DodgeRight" : "DodgeForward";
        case CombatState::Backstepping: return "Backstep";
        case CombatState::Dead: return "Death";
        default: break;
        }
        if (In.HitTaken || HitReactionPlaying) return "HitReactionFront";
        if (In.Swimming) return In.Struggling ? "StrugglingSwim" : "Swim";
        if (In.Guarded)
        {
            if (In.PlanarSpeed < StandBelow) return "CombatIdle";
            switch (In.Direction)
            {
            case MoveDirection::Back: return "CombatBack";
            case MoveDirection::Left: return "CombatStrafeLeft";
            case MoveDirection::Right: return "CombatStrafeRight";
            case MoveDirection::Forward: default: return "CombatForward";
            }
        }
        if (In.PlanarSpeed < StandBelow) return "Idle";
        if (In.Sprinting && In.PlanarSpeed >= RunFrom) return "Sprint";
        return In.PlanarSpeed >= RunFrom ? "Run" : "Walk";
    }

    CharacterMotionOutput CharacterMotion::Tick(const CharacterMotionInput& In, const double DeltaSeconds)
    {
        // A hit reaction plays out unless an action, death or the water takes over.
        if (In.HitTaken) HitReaction = true;
        if (HitReaction && (In.Dead || IsAction(In.Combat) || In.Swimming)) HitReaction = false;

        std::string Wanted = Select(In, HitReaction);
        const MotionClip* Clip = Table.Find(Wanted);
        if (!Clip)
        {
            Wanted = In.Swimming ? "Swim" : "Idle";
            Clip = Table.Find(Wanted);
            if (!Clip)
            {
                Wanted = "Idle";
                Clip = Table.Find(Wanted);
            }
        }

        CharacterMotionOutput Out;
        if (!Clip)
        {
            Current.clear();
            return Out;
        }

        // A new swing restarts its clip even when the state name repeats (a fresh attack after Idle is the
        // ordinary case; a combo step changes the name).
        const bool NewAction = IsAction(In.Combat) && (In.Combat != LastCombat || In.LightCombo != LastCombo);
        const bool NewHit = In.HitTaken && Wanted == "HitReactionFront";
        Out.Started = Wanted != Current || NewAction || NewHit;
        LastCombat = In.Combat;
        LastCombo = In.LightCombo;

        double Rate = 1.0;
        const bool Locomotion = Clip->Loop && Clip->GroundSpeed > 0.2 && In.PlanarSpeed >= StandBelow;
        if (Locomotion) Rate = std::clamp(In.PlanarSpeed / Clip->GroundSpeed, MinRate, MaxRate);

        double Previous = Time;
        if (Out.Started)
        {
            Current = Wanted;
            Time = 0.0;
            Previous = -1.0;  // a contact at 0 still counts once
            ContactDone = false;
        }
        else
        {
            Time += std::max(DeltaSeconds, 0.0) * Rate;
        }

        if (Clip->Loop)
        {
            while (Time >= Clip->Duration && Clip->Duration > 0.0) Time -= Clip->Duration;
        }
        else if (Time >= Clip->Duration)
        {
            Time = Clip->Duration;  // hold the last frame (death, a finished swing waiting for the combat state)
            if (Wanted == "HitReactionFront") HitReaction = false;
        }

        if (Clip->Contact && !ContactDone && Previous < *Clip->Contact && Time >= *Clip->Contact)
        {
            Out.Contact = true;
            ContactDone = true;
        }

        Out.State = Current;
        Out.Time = Time;
        Out.PlayRate = Rate;
        Out.BlendSeconds = Out.Started ? BlendFor(Wanted, In) : 0.0;
        Out.Loop = Clip->Loop;
        return Out;
    }
}
