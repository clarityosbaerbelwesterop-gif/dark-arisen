#pragma once

#include <DarkArisen/DarkArisenTypeIds.h>

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/RTTI/RTTI.h>

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
