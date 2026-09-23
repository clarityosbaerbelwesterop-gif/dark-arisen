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
}
