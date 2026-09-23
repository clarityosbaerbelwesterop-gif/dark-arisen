#include "DarkArisen/Core/EnemyBrain.h"

#include "DarkArisen/Core/CampaignRuntime.h"
#include "DarkArisen/Core/EntityPolicy.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace DarkArisen::Core
{
    EnemyProfile EnemyProfile::Boarder()
    {
        EnemyProfile Profile;
        Profile.ProfileId = "enemy.boarder";
        Profile.Phases = {{1.0f, 1.15f, 9.0f, 1.0f, 0.0f}}; // UE: cooldown 1.15 s, awareness 900 cm
        return Profile;
    }

    EnemyProfile EnemyProfile::HolderBoss(const std::string_view BossId, const std::string_view MissionId)
    {
        EnemyProfile Profile = Boarder();
        Profile.ProfileId = "enemy.holder";
        Profile.BossId = std::string(BossId);
        Profile.MissionId = std::string(MissionId);
        Profile.OutcomeKey = "BossResolution." + std::string(BossId);
        Profile.OutcomeValue = "Defeated";
        Profile.CompleteMissionOnDefeat = true; // UE default; the story contract may override it
        return Profile;
    }

    EnemyProfile EnemyProfile::DreamEthan()
    {
        EnemyProfile Profile;
        Profile.ProfileId = "enemy.dream_ethan";
        Profile.BossId = "boss.dream_ethan";
        Profile.MissionId = "Main.C09.03.DreamFight";
        Profile.OutcomeKey = "DreamEthan.Outcome";
        Profile.OutcomeValue = "Accepted";
        Profile.CompleteMissionOnDefeat = true;
        Profile.MaxHealth = 360.0f;
        Profile.MaxPosture = 150.0f;
        Profile.AttackRangeMetres = 2.25f;
        Profile.Phases = {{1.0f, 1.25f, 18.5f, 1.15f, 0.0f}, {0.66f, 0.92f, 20.5f, 1.30f, 0.25f},
            {0.33f, 0.70f, 22.0f, 1.45f, 0.35f}};
        return Profile;
    }

    EnemyProfile EnemyProfile::DravenVoss()
    {
        EnemyProfile Profile;
        Profile.ProfileId = "enemy.draven_voss";
        Profile.BossId = "boss.draven_voss";
        Profile.MissionId = "Main.C10.04.DravenVoss";
        Profile.OutcomeKey = "Draven.Outcome";
        Profile.OutcomeValue = "Defeated";
        Profile.CompleteMissionOnDefeat = true;
        Profile.MaxHealth = 520.0f;
        Profile.MaxPosture = 190.0f;
        Profile.AttackRangeMetres = 2.35f;
        Profile.TelegraphSeconds = 0.30f;
        Profile.Phases = {{1.0f, 1.05f, 23.0f, 1.35f, 0.25f}, {0.65f, 0.78f, 25.0f, 1.55f, 0.40f},
            {0.30f, 0.58f, 27.0f, 1.75f, 0.50f}};
        return Profile;
    }

    bool EnemyProfile::Validate(std::string& OutError) const
    {
        if (ProfileId.empty() || MaxHealth <= 0.0f || MaxPosture <= 0.0f || AttackRangeMetres <= 0.0f ||
            LoseTargetSeconds <= 0.0f || RecoverySeconds < 0.0f || RepositionSeconds < 0.0f || TelegraphSeconds < 0.0f)
        {
            OutError = "enemy profile needs an id and positive health, posture, range and timers";
            return false;
        }
        if (Phases.empty() || Phases.front().HealthFraction != 1.0f)
        {
            OutError = "the first phase must start at full health";
            return false;
        }
        for (std::size_t Index = 0; Index < Phases.size(); ++Index)
        {
            const EnemyPhase& Phase = Phases[Index];
            if (Phase.AttackCooldownSeconds <= 0.0f || Phase.AwarenessRangeMetres < AttackRangeMetres ||
                Phase.PoiseMultiplier < 0.0f || Phase.HeavyAttackShare < 0.0f || Phase.HeavyAttackShare > 1.0f ||
                (Index > 0 && Phase.HealthFraction >= Phases[Index - 1].HealthFraction))
            {
                OutError = "phases must be strictly descending with sane cooldown, awareness and shares";
                return false;
            }
        }
        if (!BossId.empty() && (!EntityPolicy::CanBeBoss(BossId) || MissionId.empty()))
        {
            OutError = "boss profiles need a legal boss id and an owning mission";
            return false;
        }
        return true;
    }

    EnemyBrain::EnemyBrain(EnemyProfile Profile)
        : Data(std::move(Profile))
    {
        if (Data.Phases.empty())
        {
            Data.Phases.push_back(EnemyPhase{});
        }
    }

    void EnemyBrain::Configure(Combatant& Self) const
    {
        Self.Health.MaxHealth = Data.MaxHealth;
        Self.Health.CurrentHealth = Data.MaxHealth;
        Self.Combat.MaxPosture = Data.MaxPosture;
        Self.Combat.PoiseMultiplier = Phase().PoiseMultiplier;
    }

    void EnemyBrain::Transition(const EnemyState Next)
    {
        if (State != Next)
        {
            State = Next;
            StateTime = 0.0f;
        }
    }

    std::optional<int> EnemyBrain::UpdatePhase(Combatant& Self)
    {
        const float Fraction = Self.Health.GetHealthFraction();
        int Next = PhaseIndex;
        while (Next + 1 < static_cast<int>(Data.Phases.size()) &&
            Fraction <= Data.Phases[static_cast<std::size_t>(Next + 1)].HealthFraction)
        {
            ++Next;
        }
        if (Next == PhaseIndex)
        {
            return std::nullopt;
        }
        PhaseIndex = Next; // Phases only escalate, never de-escalate (UE ApplyCombatPhase).
        Self.Combat.PoiseMultiplier = Phase().PoiseMultiplier;
        return GetPhase();
    }

    HitKind EnemyBrain::ChooseAttack()
    {
        // Deterministic pattern instead of dice: the player can learn it. A share of 0.5 means
        // every second attack is heavy, 0.25 every fourth, 0 never.
        ++AttackCounter;
        const float Share = Phase().HeavyAttackShare;
        if (Share <= 0.0f)
        {
            return HitKind::Light;
        }
        const int Period = std::max(1, static_cast<int>(std::lround(1.0f / Share)));
        return AttackCounter % Period == 0 ? HitKind::Heavy : HitKind::Light;
    }

    EnemyIntent EnemyBrain::Tick(const float DeltaSeconds, const EnemyPerception& Perception, Combatant& Self)
    {
        EnemyIntent Intent;
        const float Delta = std::isfinite(DeltaSeconds) ? std::max(0.0f, DeltaSeconds) : 0.0f;
        if (State == EnemyState::Dead || Self.Health.IsDead())
        {
            Transition(EnemyState::Dead);
            PendingAttack.reset();
            Intent.State = State;
            return Intent;
        }
        Intent.PhaseChanged = UpdatePhase(Self);
        StateTime += Delta;
        CooldownRemaining = std::max(0.0f, CooldownRemaining - Delta);

        // Canon: never pursue real Ethan or a child, whatever perception reports.
        const bool HasTarget = Perception.TargetVisible && !Perception.TargetDead &&
            EntityPolicy::CanBeHostileTarget(Perception.TargetId) &&
            Perception.DistanceMetres <= Phase().AwarenessRangeMetres;
        if (!HasTarget)
        {
            PendingAttack.reset();
            if (State != EnemyState::Idle && State != EnemyState::Search) Transition(EnemyState::Search);
            else if (State == EnemyState::Search && StateTime >= Data.LoseTargetSeconds) Transition(EnemyState::Idle);
            Intent.State = State;
            return Intent;
        }

        const bool InRange = Perception.DistanceMetres <= Data.AttackRangeMetres;
        const bool Staggered = Self.Combat.GetState() == CombatState::Staggered;
        switch (State)
        {
        case EnemyState::Idle:
        case EnemyState::Search:
            Transition(EnemyState::Detect);
            break;
        case EnemyState::Detect:
            Transition(EnemyState::Approach);
            break;
        case EnemyState::Approach:
            if (InRange) Transition(EnemyState::Combat);
            break;
        case EnemyState::Combat:
            if (!InRange) Transition(EnemyState::Approach);
            else if (CooldownRemaining <= 0.0f && !Staggered && !Self.Combat.IsActionCommitted())
            {
                PendingAttack = ChooseAttack();
                Intent.BeginTelegraph = PendingAttack;
                Transition(EnemyState::Attack);
            }
            break;
        case EnemyState::Attack:
            if (Staggered)
            {
                // A posture break interrupts the wind-up; the attack is lost.
                PendingAttack.reset();
                Transition(EnemyState::Recover);
            }
            else if (PendingAttack && StateTime >= Data.TelegraphSeconds)
            {
                Intent.CommitAttack = PendingAttack;
                PendingAttack.reset();
                CooldownRemaining = Phase().AttackCooldownSeconds;
            }
            else if (!PendingAttack && !Self.Combat.IsActionCommitted())
            {
                Transition(EnemyState::Recover);
            }
            break;
        case EnemyState::Recover:
            if (!Staggered && StateTime >= Data.RecoverySeconds) Transition(EnemyState::Reposition);
            break;
        case EnemyState::Reposition:
            if (StateTime >= Data.RepositionSeconds) Transition(EnemyState::Combat);
            break;
        case EnemyState::Dead:
            break;
        }

        Intent.State = State;
        Intent.FaceTarget = State != EnemyState::Idle && State != EnemyState::Search;
        Intent.MoveTowardTarget = State == EnemyState::Approach && !Self.Combat.IsActionCommitted();
        return Intent;
    }

    bool ResolveBossDefeat(CampaignRuntime& Campaign, const EnemyProfile& Profile)
    {
        if (Profile.BossId.empty() || Profile.MissionId.empty() ||
            Campaign.GetMissionState(Profile.MissionId) != MissionState::Active)
        {
            return false;
        }
        if (!Campaign.MarkBossDefeated(Profile.BossId))
        {
            return false;
        }
        if (!Profile.OutcomeKey.empty() && !Profile.OutcomeValue.empty() &&
            !Campaign.SetQuestOutcome(Profile.OutcomeKey, Profile.OutcomeValue))
        {
            return false;
        }
        return !Profile.CompleteMissionOnDefeat || Campaign.CompleteMission(Profile.MissionId);
    }
}
