#pragma once

#include <DarkArisen/DarkArisenTypeIds.h>

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/RTTI/RTTI.h>
#include <AzCore/std/string/string.h>

namespace DarkArisen
{
    /**
     * Presentation hooks for one enemy entity (animation, audio, VFX). Gameplay never waits on
     * them; the brain has already decided. Kinds are DarkArisen::Core::HitKind / EnemyState ordinals.
     */
    class EnemyNotifications : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(EnemyNotifications, EnemyNotificationsTypeId);
        virtual ~EnemyNotifications() = default;

        virtual void OnTelegraph([[maybe_unused]] int hitKind) {}
        virtual void OnAttackCommitted([[maybe_unused]] int hitKind) {}
        virtual void OnEnemyStateChanged([[maybe_unused]] int state) {}
        virtual void OnBossPhaseChanged([[maybe_unused]] int phase) {}
        /** An authored boss move begins (animation, voice and tell selection). */
        virtual void OnMoveStarted([[maybe_unused]] const AZStd::string& moveId) {}
        /** DarkArisen::Core::BossStance ordinal; always announced before moves from it. */
        virtual void OnStanceChanged([[maybe_unused]] int stance) {}
        /** The telegraphed wind-up was a feint and is cancelled now. */
        virtual void OnFeint() {}
        virtual void OnPistolFired([[maybe_unused]] bool hit) {}
        /** The marked landing zone; it detonates after fuseSeconds. */
        virtual void OnPowderBombThrown([[maybe_unused]] const AZ::Vector3& target, [[maybe_unused]] float fuseSeconds) {}
        virtual void OnPowderBombDetonated([[maybe_unused]] const AZ::Vector3& position) {}
        /** Jake's sword was taken from his hand (Draven's disarm). */
        virtual void OnTargetDisarmed() {}
    };

    using EnemyNotificationBus = AZ::EBus<EnemyNotifications>;

    /** Every active enemy brain answers population queries. Broadcast. */
    class EnemyPopulationRequests : public AZ::EBusTraits
    {
    public:
        virtual ~EnemyPopulationRequests() = default;
        /** Adds one when this enemy is a living rank-and-file duelist (not a boss). */
        virtual void CountLivingRankAndFile(int& count) const = 0;
    };
    using EnemyPopulationRequestBus = AZ::EBus<EnemyPopulationRequests>;
}
