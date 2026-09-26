#pragma once

#include <DarkArisen/DarkArisenTypeIds.h>

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Component/EntityId.h>
#include <AzCore/RTTI/RTTI.h>

#include <DarkArisen/Core/NavalCombat.h>

namespace DarkArisen
{
    /** One ship's guns and hull (Core::NavalCombatant). Sides are Core::Broadside ordinals. */
    class NavalCombatRequests : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(NavalCombatRequests, NavalCombatRequestsTypeId);
        virtual ~NavalCombatRequests() = default;

        /** Fires at the nearest hostile ship on that side within range. Returns the damage dealt. */
        virtual float FireBroadside(int side) = 0;
        virtual void ApplyHullDamage(float damage) = 0;
        virtual bool IsSunk() const = 0;
        virtual bool IsPlayerAligned() const = 0;
        virtual float GetHullFraction() const = 0;
        virtual AZ::EntityId GetShipEntityId() const = 0;
        virtual Core::NavalCombatant& GetNavalCombatant() = 0;
    };
    using NavalCombatRequestBus = AZ::EBus<NavalCombatRequests>;

    /** Presentation hooks for one ship (cannon smoke, splinters, sinking). */
    class NavalCombatNotifications : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(NavalCombatNotifications, NavalCombatNotificationsTypeId);
        virtual ~NavalCombatNotifications() = default;
        virtual void OnBroadsideFired([[maybe_unused]] int side, [[maybe_unused]] float damage) {}
        virtual void OnSunk() {}
    };
    using NavalCombatNotificationBus = AZ::EBus<NavalCombatNotifications>;
}
