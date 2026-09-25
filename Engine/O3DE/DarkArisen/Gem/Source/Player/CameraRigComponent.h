#pragma once

#include <DarkArisen/CameraBus.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>

#include <DarkArisen/Core/Camera.h>

namespace DarkArisen
{
    /**
     * Third-person follow camera on the camera entity (Unreal camera boom + controller rotation):
     * free look from mouse and stick, lock-on framing from the player's LockOnComponent, and an arm
     * that shortens against geometry (PhysX ray along the arm). Rules live in Core::CameraRig.
     */
    class CameraRigComponent
        : public AZ::Component
        , public CameraRigRequestBus::Handler
        , protected AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(CameraRigComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);

        // CameraRigRequests
        void AddMouseLook(float deltaXPixels, float deltaYPixels) override;
        void SetStickLook(const AZ::Vector2& stick) override { m_stick = stick; }
        float GetYawDegrees() const override { return static_cast<float>(m_rig.GetYaw()); }
        AZ::Vector2 GetForward() const override;
        AZ::Vector2 GetRight() const override;

    protected:
        void Activate() override;
        void Deactivate() override;
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        int GetTickOrder() override { return AZ::TICK_PRE_RENDER; }

    private:
        AZ::EntityId m_target;
        Core::CameraRig m_rig;
        AZ::Vector2 m_stick = AZ::Vector2::CreateZero();

        float ArmClearance(const AZ::Vector3& pivot, const AZ::Vector3& desired) const;
    };
}
