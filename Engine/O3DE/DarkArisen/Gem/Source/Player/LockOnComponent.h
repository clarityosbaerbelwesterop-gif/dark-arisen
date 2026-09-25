#pragma once

#include <DarkArisen/CameraBus.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>

namespace DarkArisen
{
    /**
     * Lock-on for the player (Unreal ULockOnComponent): picks the best living, targetable combatant
     * in front and in sight (Core::SelectLockTarget; real Ethan and children never qualify), keeps it
     * while it stays within the retention range and in sight, and releases it otherwise.
     */
    class LockOnComponent
        : public AZ::Component
        , public LockOnRequestBus::Handler
        , protected AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(LockOnComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        // LockOnRequests
        bool ToggleLockOn() override;
        void ReleaseLockOn() override { m_target.SetInvalid(); }
        bool IsLockedOn() const override { return m_target.IsValid(); }
        AZ::EntityId GetLockTarget() const override { return m_target; }

    protected:
        void Activate() override;
        void Deactivate() override;
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

    private:
        AZ::EntityId m_target;

        bool InSight(const AZ::Vector3& from, const AZ::Vector3& to, AZ::EntityId target) const;
    };
}
