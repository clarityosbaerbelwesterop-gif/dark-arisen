#pragma once

#include "Persistence/SaveSlotStore.h"

#include <DarkArisen/Core/Combat.h>
#include <DarkArisen/Core/Traversal.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Math/Vector2.h>
#include <AzFramework/Input/Events/InputChannelEventListener.h>

namespace DarkArisen
{
    /**
     * Keyboard/mouse and gamepad control for Jake, bound as Unreal's DefaultInput.ini. Movement is
     * camera relative, look input drives the camera rig, and committed combat actions cannot be
     * cancelled by movement (Unreal parity).
     */
    class JakeInputComponent
        : public AZ::Component
        , public AzFramework::InputChannelEventListener
        , protected AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(JakeInputComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        /** Below this stick deflection Jake walks (traversal.md 2.1), above it he runs (3.1); sprint is held (4.1). */
        static constexpr float WalkBelowDeflection = 0.6f;
        static constexpr float GravityMetresPerSecondSquared = 9.81f;
        /** UE AAlphaMenuPlayerController save slot. */
        static constexpr const char* QuickSlot = SaveSlotStore::ManualSlot;

    protected:
        void Activate() override;
        void Deactivate() override;
        bool OnInputChannelEventFiltered(const AzFramework::InputChannel& inputChannel) override;
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

    private:
        AZ::Vector2 m_keyboardMove = AZ::Vector2::CreateZero();
        AZ::Vector2 m_stickMove = AZ::Vector2::CreateZero();
        bool m_sprintHeld = false;
        bool m_wasSwimming = false;
        AZ::Vector2 m_stickLook = AZ::Vector2::CreateZero();
        /** traversal.md: gait, speed ramps, exhaustion; it drains the combatant's stamina pool. */
        Core::TraversalModel m_traversal;
        /** The pool Jake moves on when the entity has no combatant (a level without combat). */
        Core::StaminaModel m_ownStamina;
        /** Last direction of travel, so Jake brakes along it when the stick is released. */
        AZ::Vector2 m_heading = AZ::Vector2(0.0f, 1.0f);

        void Interact() const;
        void Jump();
        void QuickSave(bool save) const;
    };
}
