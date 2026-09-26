#pragma once

#include "Persistence/SaveSlotStore.h"

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

        /** UE AJakeCharacter: 330 cm/s run, 600 cm/s sprint. */
        static constexpr float RunSpeedMetresPerSecond = 3.3f;
        static constexpr float SprintSpeedMetresPerSecond = 6.0f;
        /** UE AJakeCharacter: JumpZVelocity 520 cm/s. */
        static constexpr float JumpSpeedMetresPerSecond = 5.2f;
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

        void Interact() const;
        void Jump() const;
        void QuickSave(bool save) const;
    };
}
