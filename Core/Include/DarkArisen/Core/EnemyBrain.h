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

        static EnemyProfile Boarder();
        static EnemyProfile HolderBoss(std::string_view BossId, std::string_view MissionId);
        static EnemyProfile DreamEthan();
        static EnemyProfile DravenVoss();
        bool Validate(std::string& OutError) const;
    };

    /** What the engine senses this frame about the current target. */
    struct EnemyPerception
    {
        std::string TargetId;
        bool TargetVisible = false;
        bool TargetDead = false;
        float DistanceMetres = 0.0f;
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

    private:
        EnemyProfile Data;
        EnemyState State = EnemyState::Idle;
        float StateTime = 0.0f;
        float CooldownRemaining = 0.0f;
        int PhaseIndex = 0;
        int AttackCounter = 0;
        std::optional<HitKind> PendingAttack;

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
}
