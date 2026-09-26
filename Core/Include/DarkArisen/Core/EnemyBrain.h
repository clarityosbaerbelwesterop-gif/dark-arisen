#pragma once

#include "DarkArisen/Core/Combat.h"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace DarkArisen::Core
{
    class CampaignRuntime;

    /** Ordinals match EBoardingEnemyState in the Unreal reference. */
    enum class EnemyState : std::uint8_t
    {
        Idle,
        Detect,
        Approach,
        Combat,
        Attack,
        Reposition,
        Recover,
        Search,
        Dead
    };

    /** Boss escalation step, entered once health drops to or below HealthFraction. */
    struct EnemyPhase
    {
        float HealthFraction = 1.0f;
        float AttackCooldownSeconds = 1.15f;
        float AwarenessRangeMetres = 9.0f;
        float PoiseMultiplier = 1.0f;
        /** Share of committed attacks that are heavy (0 = light only, the Unreal behaviour). */
        float HeavyAttackShare = 0.0f;
    };

    /** A boss's readable fighting posture. Every authored move belongs to one; a change is always announced. */
    enum class BossStance : std::uint8_t { None, Officer, Pirate, Guarded, Unarmed };

    /** How a committed step reaches its target. Melee goes through the combat model, the rest through ResolveBossDelivery. */
    enum class MoveDelivery : std::uint8_t { Melee, PistolShot, PowderBomb, Grab, Disarm };

    /** What starts an authored move. Reactive triggers read Jake's play; none is random. */
    enum class MoveTrigger : std::uint8_t
    {
        Pattern,         // learnable rotation among the stance's moves in range
        AfterDeflected,  // Jake just deflected one of this boss's hits
        TargetCommitted, // Jake committed to an attack inside the move's reach
        ProximityHold,   // Jake stayed inside the move's reach for TriggerValue seconds
        HealthThreshold, // once, when health first drops to TriggerValue
        Interval         // every TriggerValue seconds of engagement
    };

    /** One beat of a move. Hit numbers always come from the combat laws for Weight. */
    struct MoveStep
    {
        HitKind Weight = HitKind::Light;
        MoveDelivery Delivery = MoveDelivery::Melee;
        float TelegraphSeconds = 0.35f;
        /** Wind-up shown, then cancelled into the next step: punishes an early deflect. */
        bool Feint = false;
        /** Busy time after a non-melee commit (melee waits for the combat model instead). */
        float RecoverySeconds = 0.0f;
    };

    /** An authored boss move: a chain of steps, each readable and answerable on its own. */
    struct BossMove
    {
        std::string MoveId;
        BossStance Stance = BossStance::None; // None: usable from any stance
        int MinPhase = 1;
        int MaxPhase = 1;
        float MinRangeMetres = 0.0f;
        float MaxRangeMetres = 2.0f;
        MoveTrigger Trigger = MoveTrigger::Pattern;
        float TriggerValue = 0.0f;
        float ReuseSeconds = 0.0f;
        std::vector<MoveStep> Steps;
    };

    /** The stances a phase fights in; more than one cycles every ShiftSeconds, never mid-move. */
    struct BossStanceRule
    {
        int Phase = 1;
        std::vector<BossStance> Cycle;
        float ShiftSeconds = 0.0f;
    };

    /**
     * Authored enemy data. Values marked "UE" are ported from the Unreal reference; telegraph and
     * heavy-share values are provisional tuning (the UE enemies only light-attack without wind-up).
     */
    struct EnemyProfile
    {
        std::string ProfileId;
        std::string BossId;          // empty for rank-and-file enemies
        std::string MissionId;       // mission whose active state the defeat resolves
        std::string OutcomeKey;
        std::string OutcomeValue;
        bool CompleteMissionOnDefeat = false;
        float MaxHealth = 180.0f;              // UE ADuelingEnemyCharacter
        float MaxPosture = 100.0f;             // UE
        float AttackRangeMetres = 2.10f;       // UE 210 cm
        float LoseTargetSeconds = 4.0f;        // UE UBoardingEnemyComponent
        float RecoverySeconds = 0.8f;          // UE
        float RepositionSeconds = 0.45f;       // UE
        float TelegraphSeconds = 0.35f;        // provisional: readable wind-up before every commit
        std::vector<EnemyPhase> Phases;        // ordered, first entry at HealthFraction 1.0
        /** Authored moveset; empty keeps the generic light/heavy pattern. */
        std::vector<BossMove> Moves;
        std::vector<BossStanceRule> StanceRules;

        /** No committed step may be faster to read than this. */
        static constexpr float MinimumTelegraphSeconds = 0.25f;
        static constexpr int MaximumChainSteps = 4;
        static constexpr float PistolRangeMetres = 15.0f;
        static constexpr float PowderBombFuseSeconds = 1.5f;
        static constexpr float PowderBombRadiusMetres = 4.0f;
        /** A deflect earns a punish only this long after it happened. */
        static constexpr float PunishWindowSeconds = 1.5f;

        static EnemyProfile Boarder();
        static EnemyProfile HolderBoss(std::string_view BossId, std::string_view MissionId);
        static EnemyProfile DreamEthan();
        static EnemyProfile DravenVoss();
        bool Validate(std::string& OutError) const;

    private:
        bool ValidateMoveset(std::string& OutError) const;
    };

    /** What the engine senses this frame about the current target. */
    struct EnemyPerception
    {
        std::string TargetId;
        bool TargetVisible = false;
        bool TargetDead = false;
        float DistanceMetres = 0.0f;
        /** The target has committed to an attack (light or heavy). */
        bool TargetAttacking = false;
        /** False while the target's weapon lies on the deck; the adapter default is armed. */
        bool TargetArmed = true;
    };

    /** What the engine must do this frame. The brain never moves or animates anything itself. */
    struct EnemyIntent
    {
        EnemyState State = EnemyState::Idle;
        bool MoveTowardTarget = false;
        bool FaceTarget = false;
        /** Start the wind-up presentation for this attack (sound, animation, weapon glint). */
        std::optional<HitKind> BeginTelegraph;
        /** Commit the attack through the combat model now. */
        std::optional<HitKind> CommitAttack;
        /** Set once when a boss enters a new phase (1-based). */
        std::optional<int> PhaseChanged;
        /** Delivery of this frame's telegraph or commit. */
        MoveDelivery Delivery = MoveDelivery::Melee;
        /** Set on the frame an authored move begins (animation, voice and tell selection). */
        std::string MoveId;
        std::optional<BossStance> StanceChanged;
        /** The telegraphed step was a feint and is cancelled now. */
        bool FeintCancelled = false;
    };

    /**
     * Engine-neutral enemy decision authority: port of UBoardingEnemyComponent (state machine),
     * ADuelingEnemyCharacter (awareness, range, cooldown) and the boss phase escalations
     * (AMainStoryHolderBossCharacter, ADreamEthanBossCharacter, ADravenVossBossCharacter),
     * plus a telegraph step so every committed attack is readable.
     */
    class EnemyBrain
    {
    public:
        explicit EnemyBrain(EnemyProfile Profile);

        /** Applies profile health/posture/poise to the combatant the brain drives. */
        void Configure(Combatant& Self) const;
        EnemyIntent Tick(float DeltaSeconds, const EnemyPerception& Perception, Combatant& Self);
        /** The attack the brain committed was accepted and has finished (combat model idle again). */
        EnemyState GetState() const { return State; }
        int GetPhase() const { return PhaseIndex + 1; }
        const EnemyProfile& Profile() const { return Data; }
        BossStance GetStance() const { return Stance; }
        /** The adapter reports how this enemy's hit landed (melee sweep or delivery). */
        void NotifyHitResolved(const DamageResult& Result);

    private:
        EnemyProfile Data;
        EnemyState State = EnemyState::Idle;
        float StateTime = 0.0f;
        float CooldownRemaining = 0.0f;
        int PhaseIndex = 0;
        int AttackCounter = 0;
        std::optional<HitKind> PendingAttack;

        // Authored moveset execution.
        BossStance Stance = BossStance::None;
        bool StanceAnnounced = false;
        bool StanceResetPending = false;
        std::size_t StanceIndex = 0;
        float StanceTime = 0.0f;
        std::optional<std::size_t> ActiveMove;
        std::size_t StepIndex = 0;
        float StepTime = 0.0f;
        bool StepCommitted = false;
        float StepBusyRemaining = 0.0f;
        float PunishWindowRemaining = 0.0f;
        std::size_t PatternCursor = 0;
        std::vector<float> MoveReuseRemaining;
        std::vector<float> MoveTriggerTime;
        std::vector<bool> MoveFired;

        const BossStanceRule* StanceRule() const;
        void ResetStanceForPhase(EnemyIntent& Intent);
        void TickStance(float DeltaSeconds, bool Engaged, EnemyIntent& Intent);
        void TickMoveTimers(float DeltaSeconds, const EnemyPerception& Perception);
        bool IsEligible(std::size_t Index, const EnemyPerception& Perception, const Combatant& Self) const;
        bool AnyPatternReaches(float DistanceMetres) const;
        std::optional<std::size_t> SelectMove(const EnemyPerception& Perception, const Combatant& Self, bool ReactiveOnly);
        void BeginMove(std::size_t Index, EnemyIntent& Intent);
        void TickMove(Combatant& Self, EnemyIntent& Intent);
        void EndMove(bool Interrupted);

        void Transition(EnemyState Next);
        const EnemyPhase& Phase() const { return Data.Phases[static_cast<std::size_t>(PhaseIndex)]; }
        std::optional<int> UpdatePhase(Combatant& Self);
        HitKind ChooseAttack();
    };

    /**
     * Port of AMainStoryHolderBossCharacter::HandleHolderDied: a physically defeated boss records
     * its defeat and outcome and, when authored, completes its mission. Only while that mission is
     * active; returns false otherwise (nothing changes).
     */
    bool ResolveBossDefeat(CampaignRuntime& Campaign, const EnemyProfile& Profile);

    /**
     * Resolves a non-melee authored delivery through the existing damage laws. Pistol shots are
     * projectiles (deflectable, like every non-critical hit); powder bombs are environmental (never
     * deflectable); a grab is a deflectable melee hit of the grab rally class; a disarm cannot be
     * deflected but dodge invulnerability avoids it. Melee forwards to ResolveMeleeHit.
     */
    DamageResult ResolveBossDelivery(Combatant& Boss, Combatant& Target, MoveDelivery Delivery, HitKind Weight);
}
