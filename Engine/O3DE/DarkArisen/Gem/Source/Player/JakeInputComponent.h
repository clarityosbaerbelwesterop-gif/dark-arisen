#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Math/Vector2.h>
#include <AzFramework/Input/Events/InputChannelEventListener.h>

namespace DarkArisen
{
    /**
     * Keyboard/mouse and gamepad control for Jake. Translates input channels into
     * CombatRequestBus actions and PhysX character velocity. Committed combat actions
     * cannot be cancelled by movement (Unreal parity).
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

        void Interact() const;
    };
}
