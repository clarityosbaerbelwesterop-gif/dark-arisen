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

        // The officer, the pirate, the man: the combat content of the Draven boss specification
        // (docs/design/bosses/draven_voss.md sections 6-8), which conflicts with no current canon.
        // Hit numbers stay the combat laws' Light/Heavy profiles, the phase thresholds stay the
        // Unreal 0.65/0.30, frame wind-ups are converted at 60 fps and floored at the readable
        // minimum. The arena beats (falling mast, tilting helm) belong to the level, not to him.
        using D = MoveDelivery;
        using S = BossStance;
        using T = MoveTrigger;
        const auto Step = [](const HitKind Weight, const float Telegraph, const D Delivery = D::Melee, const bool Feint = false,
                              const float Recovery = 0.0f) { return MoveStep{Weight, Delivery, Telegraph, Feint, Recovery}; };
        constexpr HitKind L = HitKind::Light;
        constexpr HitKind H = HitKind::Heavy;
        constexpr float Reach = 2.35f;
        constexpr float Pistol = PistolRangeMetres;
        Profile.Moves = {
            // Phase 1, the officer: textbook academy sabre, nothing wasted; he tests Jake.
            {"draven.lunge_thrust", S::Officer, 1, 2, 1.2f, Reach, T::Pattern, 0.0f, 0.0f, {Step(H, 0.42f)}},
            {"draven.three_form_slash", S::Officer, 1, 2, 0.0f, 2.0f, T::Pattern, 0.0f, 0.0f, {Step(L, 0.50f), Step(L, 0.30f), Step(H, 0.30f)}},
            // A deflect is answered at once: committing to a parry is rewarded, mashing it is not.
            {"draven.saber_disengage", S::Officer, 1, 2, 0.0f, Reach, T::AfterDeflected, 0.0f, 0.0f, {Step(L, 0.25f)}},
            // Phase 2, the pirate: sabre and flintlock, powder and feints.
            {"draven.saber_and_pistol", S::Pirate, 2, 2, 0.0f, Reach, T::Pattern, 0.0f, 0.0f,
                {Step(L, 0.42f), Step(H, 0.25f, D::PistolShot, false, 0.67f)}},
            {"draven.false_lunge", S::Pirate, 2, 2, 1.0f, Reach, T::Pattern, 0.0f, 0.0f, {Step(H, 0.42f, D::Melee, true), Step(L, 0.25f)}},
            {"draven.pistol_shot", S::Pirate, 2, 2, 3.0f, Pistol, T::Pattern, 0.0f, 6.0f, {Step(H, 0.60f, D::PistolShot, false, 0.67f)}},
            {"draven.powder_bomb", S::Pirate, 2, 2, 0.0f, Pistol, T::Interval, 20.0f, 0.0f, {Step(H, 0.50f, D::PowderBomb, false, 0.50f)}},
            // The low guard he has not shown before: he waits and answers a committed attack.
            {"draven.guard_counter", S::Guarded, 2, 2, 0.0f, Reach, T::TargetCommitted, 0.0f, 0.0f, {Step(L, 0.25f)}},
            {"draven.low_cut", S::Guarded, 2, 2, 0.0f, 2.0f, T::Pattern, 0.0f, 4.0f, {Step(L, 0.50f)}},
            // Phase 3, the man: sabre and pistol dropped, elbows, knees and the clinch.
            {"draven.close_quarters", S::Unarmed, 3, 3, 0.0f, 2.0f, T::Pattern, 0.0f, 0.0f, {Step(L, 0.25f), Step(L, 0.25f), Step(L, 0.25f), Step(L, 0.25f)}},
            {"draven.embrace", S::Unarmed, 3, 3, 0.0f, 2.0f, T::ProximityHold, 3.0f, 0.0f, {Step(H, 0.33f, D::Grab)}},
            // Once: he takes Jake's sword and waits for him to pick it up.
            {"draven.disarm", S::None, 3, 3, 0.0f, Reach, T::HealthThreshold, 0.25f, 0.0f, {Step(L, 0.30f, D::Disarm, false, 0.50f)}},
        };
        Profile.StanceRules = {{1, {S::Officer}, 0.0f}, {2, {S::Pirate, S::Guarded, S::Officer}, 30.0f}, {3, {S::Unarmed}, 0.0f}};
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
        return ValidateMoveset(OutError);
    }

    bool EnemyProfile::ValidateMoveset(std::string& OutError) const
    {
        if (Moves.empty())
        {
            if (!StanceRules.empty()) OutError = "stance rules without an authored moveset";
            return StanceRules.empty();
        }
        const int PhaseCount = static_cast<int>(Phases.size());
        std::vector<const BossStanceRule*> RuleByPhase(Phases.size(), nullptr);
        for (const BossStanceRule& Rule : StanceRules)
        {
            if (Rule.Phase < 1 || Rule.Phase > PhaseCount || RuleByPhase[static_cast<std::size_t>(Rule.Phase - 1)] != nullptr ||
                Rule.Cycle.empty() || (Rule.Cycle.size() > 1 && !(Rule.ShiftSeconds > 0.0f)) ||
                std::find(Rule.Cycle.begin(), Rule.Cycle.end(), BossStance::None) != Rule.Cycle.end())
            {
                OutError = "every phase needs exactly one stance rule with real stances and a shift time when it cycles";
                return false;
            }
            RuleByPhase[static_cast<std::size_t>(Rule.Phase - 1)] = &Rule;
        }
        if (std::find(RuleByPhase.begin(), RuleByPhase.end(), nullptr) != RuleByPhase.end())
        {
            OutError = "a phase has no stance rule";
            return false;
        }
        const auto InPhase = [&RuleByPhase](const BossStance Stance, const int Phase)
        {
            const std::vector<BossStance>& Cycle = RuleByPhase[static_cast<std::size_t>(Phase - 1)]->Cycle;
            return Stance == BossStance::None || std::find(Cycle.begin(), Cycle.end(), Stance) != Cycle.end();
        };
        std::vector<std::string> Ids;
        for (const BossMove& Move : Moves)
        {
            const std::string Name = Move.MoveId.empty() ? std::string("<unnamed>") : Move.MoveId;
            if (Move.MoveId.empty() || std::find(Ids.begin(), Ids.end(), Move.MoveId) != Ids.end())
            {
                OutError = "move ids must be present and unique: " + Name;
                return false;
            }
            Ids.push_back(Move.MoveId);
            bool RangeOk = Move.MinPhase >= 1 && Move.MinPhase <= Move.MaxPhase && Move.MaxPhase <= PhaseCount && Move.MinRangeMetres >= 0.0f &&
                Move.MinRangeMetres < Move.MaxRangeMetres && Move.MaxRangeMetres <= PistolRangeMetres && Move.ReuseSeconds >= 0.0f &&
                !Move.Steps.empty() && static_cast<int>(Move.Steps.size()) <= MaximumChainSteps;
            for (int Phase = Move.MinPhase; RangeOk && Phase <= Move.MaxPhase; ++Phase) RangeOk = InPhase(Move.Stance, Phase);
            if (!RangeOk)
            {
                OutError = Name + ": phases, stance, range, reuse or chain length are invalid";
                return false;
            }
            const bool TriggerOk = Move.Trigger == MoveTrigger::HealthThreshold ? Move.TriggerValue > 0.0f && Move.TriggerValue < 1.0f
                : Move.Trigger == MoveTrigger::ProximityHold || Move.Trigger == MoveTrigger::Interval ? Move.TriggerValue > 0.0f
                                                                                                           : Move.TriggerValue == 0.0f;
            if (!TriggerOk)
            {
                OutError = Name + ": trigger value does not fit its trigger";
                return false;
            }
            bool NeedsReach = false;
            for (std::size_t Index = 0; Index < Move.Steps.size(); ++Index)
            {
                const MoveStep& Step = Move.Steps[Index];
                const bool Last = Index + 1 == Move.Steps.size();
                if (!std::isfinite(Step.TelegraphSeconds) || Step.TelegraphSeconds < MinimumTelegraphSeconds || !(Step.RecoverySeconds >= 0.0f) ||
                    (Step.Weight != HitKind::Light && Step.Weight != HitKind::Heavy))
                {
                    OutError = Name + ": every step needs a readable telegraph and a light or heavy weight";
                    return false;
                }
                if (Step.Feint && (Last || Step.Delivery != MoveDelivery::Melee || Move.Steps[Index + 1].Feint))
                {
                    OutError = Name + ": a feint must be a melee wind-up followed by a real step";
                    return false;
                }
                if ((Step.Delivery == MoveDelivery::PowderBomb || Step.Delivery == MoveDelivery::Disarm) && Move.Steps.size() != 1)
                {
                    OutError = Name + ": powder bombs and disarms are single-step moves";
                    return false;
                }
                if (Step.Delivery == MoveDelivery::Disarm && Move.Trigger != MoveTrigger::HealthThreshold)
                {
                    OutError = Name + ": a disarm is a one-time authored beat";
                    return false;
                }
                NeedsReach = NeedsReach || Step.Delivery == MoveDelivery::Melee || Step.Delivery == MoveDelivery::Grab ||
                    Step.Delivery == MoveDelivery::Disarm;
            }
            if (NeedsReach && Move.MaxRangeMetres > AttackRangeMetres)
            {
                OutError = Name + ": a melee, grab or disarm step cannot land beyond the attack range";
                return false;
            }
        }
        for (const BossStanceRule& Rule : StanceRules)
        {
            for (const BossStance Stance : Rule.Cycle)
            {
                const bool HasPattern = std::any_of(Moves.begin(), Moves.end(), [&](const BossMove& Move)
                {
                    return Move.Trigger == MoveTrigger::Pattern && Move.MinPhase <= Rule.Phase && Move.MaxPhase >= Rule.Phase &&
                        (Move.Stance == Stance || Move.Stance == BossStance::None);
                });
                if (!HasPattern)
                {
                    OutError = "a stance in phase " + std::to_string(Rule.Phase) + " has no pattern move and would stall";
                    return false;
                }
            }
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
        MoveReuseRemaining.assign(Data.Moves.size(), 0.0f);
        MoveTriggerTime.assign(Data.Moves.size(), 0.0f);
        MoveFired.assign(Data.Moves.size(), false);
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
        StanceResetPending = !Data.StanceRules.empty();
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
            if (ActiveMove) EndMove(true);
            if (State != EnemyState::Idle && State != EnemyState::Search) Transition(EnemyState::Search);
            else if (State == EnemyState::Search && StateTime >= Data.LoseTargetSeconds) Transition(EnemyState::Idle);
            Intent.State = State;
            return Intent;
        }

        const bool InRange = Perception.DistanceMetres <= Data.AttackRangeMetres;
        const bool Staggered = Self.Combat.GetState() == CombatState::Staggered;
        if (!Data.Moves.empty())
        {
            TickStance(Delta, true, Intent);
            TickMoveTimers(Delta, Perception);
            if (ActiveMove) StepTime += Delta;
            StepBusyRemaining = std::max(0.0f, StepBusyRemaining - Delta);
            // Nothing new starts while Jake's sword lies on the deck: he waits for him to pick it up.
            const bool CanStart = Perception.TargetArmed && !Staggered && !Self.Combat.IsActionCommitted();
            bool Closing = false;
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
                else if (CanStart && CooldownRemaining <= 0.0f)
                {
                    // Out of reach: only ranged moves are eligible here (a shot at a kiting Jake).
                    if (const auto Move = SelectMove(Perception, Self, false)) BeginMove(*Move, Intent);
                }
                Closing = State == EnemyState::Approach && Perception.TargetArmed;
                break;
            case EnemyState::Combat:
                if (!InRange) Transition(EnemyState::Approach);
                else if (CanStart)
                {
                    if (const auto Move = SelectMove(Perception, Self, CooldownRemaining > 0.0f)) BeginMove(*Move, Intent);
                    else Closing = Perception.TargetArmed && CooldownRemaining <= 0.0f && !AnyPatternReaches(Perception.DistanceMetres);
                }
                break;
            case EnemyState::Attack:
                TickMove(Self, Intent);
                break;
            case EnemyState::Recover:
            case EnemyState::Reposition:
                if (CanStart && InRange)
                {
                    if (const auto Move = SelectMove(Perception, Self, true))
                    {
                        BeginMove(*Move, Intent);
                        break;
                    }
                }
                if (State == EnemyState::Recover && !Staggered && StateTime >= Data.RecoverySeconds) Transition(EnemyState::Reposition);
                else if (State == EnemyState::Reposition && StateTime >= Data.RepositionSeconds) Transition(EnemyState::Combat);
                break;
            case EnemyState::Dead:
                break;
            }
            Intent.State = State;
            Intent.FaceTarget = State != EnemyState::Idle && State != EnemyState::Search;
            Intent.MoveTowardTarget = Closing && !Self.Combat.IsActionCommitted();
            return Intent;
        }
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

    void EnemyBrain::NotifyHitResolved(const DamageResult& Result)
    {
        if (Result.Resolved && Result.Deflected) PunishWindowRemaining = EnemyProfile::PunishWindowSeconds;
    }

    const BossStanceRule* EnemyBrain::StanceRule() const
    {
        for (const BossStanceRule& Rule : Data.StanceRules)
        {
            if (Rule.Phase == GetPhase()) return &Rule;
        }
        return nullptr;
    }

    void EnemyBrain::ResetStanceForPhase(EnemyIntent& Intent)
    {
        const BossStanceRule* Rule = StanceRule();
        const BossStance Next = Rule != nullptr && !Rule->Cycle.empty() ? Rule->Cycle.front() : BossStance::None;
        StanceIndex = 0;
        StanceTime = 0.0f;
        if (Next != Stance || !StanceAnnounced) Intent.StanceChanged = Next;
        Stance = Next;
        StanceAnnounced = true;
    }

    void EnemyBrain::TickStance(const float DeltaSeconds, const bool Engaged, EnemyIntent& Intent)
    {
        if (!Engaged) return;
        if (!StanceAnnounced || (StanceResetPending && !ActiveMove))
        {
            StanceResetPending = false;
            ResetStanceForPhase(Intent);
            return;
        }
        const BossStanceRule* Rule = StanceRule();
        if (Rule == nullptr || Rule->Cycle.size() < 2 || StanceResetPending) return;
        StanceTime += DeltaSeconds;
        if (StanceTime >= Rule->ShiftSeconds && !ActiveMove)
        {
            StanceIndex = (StanceIndex + 1) % Rule->Cycle.size();
            Stance = Rule->Cycle[StanceIndex];
            StanceTime = 0.0f;
            Intent.StanceChanged = Stance;
        }
    }

    void EnemyBrain::TickMoveTimers(const float DeltaSeconds, const EnemyPerception& Perception)
    {
        for (std::size_t Index = 0; Index < Data.Moves.size(); ++Index)
        {
            const BossMove& Move = Data.Moves[Index];
            MoveReuseRemaining[Index] = std::max(0.0f, MoveReuseRemaining[Index] - DeltaSeconds);
            if (Move.Trigger == MoveTrigger::Interval) MoveTriggerTime[Index] += DeltaSeconds;
            else if (Move.Trigger == MoveTrigger::ProximityHold)
            {
                MoveTriggerTime[Index] = Perception.DistanceMetres <= Move.MaxRangeMetres ? MoveTriggerTime[Index] + DeltaSeconds : 0.0f;
            }
        }
        // A punish answers a deflect after the chain it interrupted, so the window waits for it.
        if (!ActiveMove) PunishWindowRemaining = std::max(0.0f, PunishWindowRemaining - DeltaSeconds);
    }

    bool EnemyBrain::IsEligible(const std::size_t Index, const EnemyPerception& Perception, const Combatant& Self) const
    {
        const BossMove& Move = Data.Moves[Index];
        if (GetPhase() < Move.MinPhase || GetPhase() > Move.MaxPhase || (Move.Stance != BossStance::None && Move.Stance != Stance) ||
            Perception.DistanceMetres < Move.MinRangeMetres || Perception.DistanceMetres > Move.MaxRangeMetres || MoveReuseRemaining[Index] > 0.0f)
        {
            return false;
        }
        switch (Move.Trigger)
        {
        case MoveTrigger::Pattern: return true;
        case MoveTrigger::AfterDeflected: return PunishWindowRemaining > 0.0f;
        case MoveTrigger::TargetCommitted: return Perception.TargetAttacking;
        case MoveTrigger::ProximityHold:
        case MoveTrigger::Interval: return MoveTriggerTime[Index] >= Move.TriggerValue;
        case MoveTrigger::HealthThreshold: return !MoveFired[Index] && Self.Health.GetHealthFraction() <= Move.TriggerValue;
        }
        return false;
    }

    std::optional<std::size_t> EnemyBrain::SelectMove(const EnemyPerception& Perception, const Combatant& Self, const bool ReactiveOnly)
    {
        // Fixed priority, never dice: the authored beat, the answer to Jake's play, then the rhythm.
        static constexpr MoveTrigger Priority[] = {MoveTrigger::HealthThreshold, MoveTrigger::AfterDeflected, MoveTrigger::TargetCommitted,
            MoveTrigger::ProximityHold, MoveTrigger::Interval};
        for (const MoveTrigger Trigger : Priority)
        {
            if (ReactiveOnly && Trigger != MoveTrigger::AfterDeflected && Trigger != MoveTrigger::TargetCommitted) continue;
            for (std::size_t Index = 0; Index < Data.Moves.size(); ++Index)
            {
                if (Data.Moves[Index].Trigger == Trigger && IsEligible(Index, Perception, Self)) return Index;
            }
        }
        if (ReactiveOnly) return std::nullopt;
        std::vector<std::size_t> Pattern;
        for (std::size_t Index = 0; Index < Data.Moves.size(); ++Index)
        {
            if (Data.Moves[Index].Trigger == MoveTrigger::Pattern && IsEligible(Index, Perception, Self)) Pattern.push_back(Index);
        }
        if (Pattern.empty()) return std::nullopt;
        return Pattern[PatternCursor++ % Pattern.size()];
    }

    bool EnemyBrain::AnyPatternReaches(const float DistanceMetres) const
    {
        return std::any_of(Data.Moves.begin(), Data.Moves.end(), [&](const BossMove& Move)
        {
            return Move.Trigger == MoveTrigger::Pattern && GetPhase() >= Move.MinPhase && GetPhase() <= Move.MaxPhase &&
                (Move.Stance == BossStance::None || Move.Stance == Stance) && DistanceMetres >= Move.MinRangeMetres &&
                DistanceMetres <= Move.MaxRangeMetres;
        });
    }

    void EnemyBrain::BeginMove(const std::size_t Index, EnemyIntent& Intent)
    {
        const BossMove& Move = Data.Moves[Index];
        ActiveMove = Index;
        StepIndex = 0;
        StepTime = 0.0f;
        StepCommitted = false;
        StepBusyRemaining = 0.0f;
        if (Move.Trigger == MoveTrigger::AfterDeflected) PunishWindowRemaining = 0.0f;
        if (Move.Trigger == MoveTrigger::Interval || Move.Trigger == MoveTrigger::ProximityHold) MoveTriggerTime[Index] = 0.0f;
        if (Move.Trigger == MoveTrigger::HealthThreshold) MoveFired[Index] = true;
        Intent.MoveId = Move.MoveId;
        Intent.BeginTelegraph = Move.Steps.front().Weight;
        Intent.Delivery = Move.Steps.front().Delivery;
        Transition(EnemyState::Attack);
    }

    void EnemyBrain::TickMove(Combatant& Self, EnemyIntent& Intent)
    {
        if (!ActiveMove)
        {
            Transition(EnemyState::Recover);
            return;
        }
        if (Self.Combat.GetState() == CombatState::Staggered)
        {
            // A posture break ends the chain; the rest of the move is lost.
            EndMove(true);
            Transition(EnemyState::Recover);
            return;
        }
        const BossMove& Move = Data.Moves[*ActiveMove];
        const MoveStep& Step = Move.Steps[StepIndex];
        if (!StepCommitted)
        {
            if (StepTime < Step.TelegraphSeconds) return;
            if (!Step.Feint)
            {
                Intent.CommitAttack = Step.Weight;
                Intent.Delivery = Step.Delivery;
                StepCommitted = true;
                StepBusyRemaining = Step.Delivery == MoveDelivery::Melee ? 0.0f : Step.RecoverySeconds;
                return;
            }
            Intent.FeintCancelled = true;
        }
        else if (Step.Delivery == MoveDelivery::Melee ? Self.Combat.IsActionCommitted() : StepBusyRemaining > 0.0f)
        {
            return;
        }
        if (++StepIndex >= Move.Steps.size())
        {
            EndMove(false);
            Transition(EnemyState::Recover);
            return;
        }
        StepTime = 0.0f;
        StepCommitted = false;
        Intent.BeginTelegraph = Move.Steps[StepIndex].Weight;
        Intent.Delivery = Move.Steps[StepIndex].Delivery;
    }

    void EnemyBrain::EndMove([[maybe_unused]] const bool Interrupted)
    {
        if (ActiveMove) MoveReuseRemaining[*ActiveMove] = Data.Moves[*ActiveMove].ReuseSeconds;
        CooldownRemaining = Phase().AttackCooldownSeconds;
        ActiveMove.reset();
        StepCommitted = false;
        StepBusyRemaining = 0.0f;
    }

    DamageResult ResolveBossDelivery(Combatant& Boss, Combatant& Target, const MoveDelivery Delivery, const HitKind Weight)
    {
        if (Delivery == MoveDelivery::Melee)
        {
            return ResolveMeleeHit(Boss, Target, Weight);
        }
        if (!Boss.Combat.IsTargetable() || (Weight != HitKind::Light && Weight != HitKind::Heavy))
        {
            return {};
        }
        const HitProfile Profile = CombatModel::GetHitProfile(Weight);
        DamageContext Context;
        Context.BaseDamage = Profile.HealthDamage;
        Context.PostureDamage = Profile.PostureDamage;
        Context.DamageClass = RallyDamageClass::Boss;
        switch (Delivery)
        {
        case MoveDelivery::PistolShot:
            Context.Type = AttackType::Projectile;
            break;
        case MoveDelivery::PowderBomb:
            Context.Type = AttackType::Environmental;
            Context.DamageClass = RallyDamageClass::Environmental;
            break;
        case MoveDelivery::Grab:
            Context.Type = Weight == HitKind::Heavy ? AttackType::Heavy : AttackType::Light;
            Context.DamageClass = RallyDamageClass::Grab;
            break;
        case MoveDelivery::Disarm:
            Context.Type = AttackType::Critical; // cannot be deflected; a dodge still avoids it
            Context.PostureDamage = 0.0f;
            Context.DamageClass = RallyDamageClass::Grab;
            break;
        case MoveDelivery::Melee:
            break;
        }
        const DamageResult Result = DamagePipeline::Resolve(Boss, Target, Context);
        if (Result.Deflected && Delivery == MoveDelivery::Grab)
        {
            Boss.Combat.AddPostureDamage(Profile.DeflectedPostureDamage);
        }
        return Result;
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
