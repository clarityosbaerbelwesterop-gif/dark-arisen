#pragma once

#include <DarkArisen/DarkArisenTypeIds.h>

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Component/EntityId.h>
#include <AzCore/RTTI/RTTI.h>
#include <AzCore/std/string/string.h>

namespace DarkArisen::Core
{
    struct Combatant;
}

namespace DarkArisen
{
    /** Per-entity combat requests. Addressed by the combatant's EntityId. */
    class CombatRequests : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(CombatRequests, CombatRequestsTypeId);
        virtual ~CombatRequests() = default;

        virtual bool LightAttack() = 0;
        virtual bool HeavyAttack() = 0;
        virtual bool Parry() = 0;
        /** A backstep when backstep is true, otherwise a directional dodge. */
        virtual bool Dodge(bool backstep) = 0;
        virtual void SetSprinting(bool sprinting) = 0;
        virtual bool StartRache() = 0;

        virtual float GetHealthFraction() const = 0;
        virtual float GetStaminaFraction() const = 0;
        virtual float GetPostureRemainingFraction() const = 0;
        virtual bool IsDead() const = 0;

        /** Stable canon id of this combatant, e.g. character.jake. */
        virtual AZStd::string GetCombatantId() const = 0;
        virtual AZ::EntityId GetCombatantEntityId() const = 0;
        /** Direct model access for hit resolution between two combatants in one process. */
        virtual Core::Combatant& GetCombatant() = 0;
        /** Restores full health/stamina and clears combat state at a checkpoint. */
        virtual void RecoverAtCheckpoint(float healthFraction, float staminaFraction) = 0;
    };

    using CombatRequestBus = AZ::EBus<CombatRequests>;

    /** Per-entity combat outcomes, addressed by the attacker's EntityId. */
    class CombatNotifications : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(CombatNotifications, CombatNotificationsTypeId);
        virtual ~CombatNotifications() = default;

        /** One of this combatant's melee contacts resolved against target. */
        virtual void OnMeleeResolved([[maybe_unused]] const AZ::EntityId& target, [[maybe_unused]] bool deflected) {}
    };

    using CombatNotificationBus = AZ::EBus<CombatNotifications>;
}
