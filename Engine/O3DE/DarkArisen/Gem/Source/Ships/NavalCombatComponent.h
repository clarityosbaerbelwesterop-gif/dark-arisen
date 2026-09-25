#pragma once

#include <DarkArisen/NavalBus.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>

#include <DarkArisen/Core/NavalCombat.h>

namespace DarkArisen
{
    /**
     * A ship's broadsides and hull (Core::NavalCombatant), port of UNavalCombatComponent and the
     * AHostileNavalShip fire loop. Greybox ships face +X (the Unreal forward axis the frame
     * conversion keeps), so starboard is world -Y at rest.
     */
    class NavalCombatComponent
        : public AZ::Component
        , protected NavalCombatRequestBus::Handler
        , protected AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(NavalCombatComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);

        float FireBroadside(int side) override;
        void ApplyHullDamage(float damage) override;
        bool IsSunk() const override { return m_combatant.IsSunk(); }
        bool IsPlayerAligned() const override { return m_playerAligned; }
        float GetHullFraction() const override { return m_combatant.GetHullFraction(); }
        AZ::EntityId GetShipEntityId() const override { return GetEntityId(); }
        Core::NavalCombatant& GetNavalCombatant() override { return m_combatant; }

    protected:
        void Activate() override;
        void Deactivate() override;
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

    private:
        bool m_playerAligned = false;
        /** Hostile ships try a broadside this often (UE AHostileNavalShip 2 s). */
        float m_fireDecisionSeconds = 2.0f;

        Core::NavalCombatant m_combatant;
        float m_fireDecisionRemaining = 0.0f;
        bool m_sunkReported = false;
    };
}
