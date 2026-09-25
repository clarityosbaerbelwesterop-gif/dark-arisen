#pragma once

// Probe-only stand-ins for engine components that need a renderer or a PhysX scene.
// Shared by the gameplay and level probes.

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/std/functional.h>
#include <AzFramework/Physics/CharacterBus.h>
#include <PhysX/CharacterGameplayBus.h>

namespace ProbeSupport
{
    /** Minimal transform so TransformBus calls have a real handler (probe only). */
    class ProbeTransformComponent : public AZ::Component, public AZ::TransformBus::Handler
    {
    public:
        AZ_COMPONENT(ProbeTransformComponent, "{5E0F7C11-2B7A-4C3D-9E8F-1A2B3C4D5E6F}");
        static void Reflect(AZ::ReflectContext* context)
        {
            if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
            {
                serialize->Class<ProbeTransformComponent, AZ::Component>()->Version(1);
            }
        }
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC_CE("TransformService"));
        }
        void Activate() override { AZ::TransformBus::Handler::BusConnect(GetEntityId()); }
        void Deactivate() override { AZ::TransformBus::Handler::BusDisconnect(); }

        void BindTransformChangedEventHandler(AZ::TransformChangedEvent::Handler&) override {}
        void BindParentChangedEventHandler(AZ::ParentChangedEvent::Handler&) override {}
        void BindChildChangedEventHandler(AZ::ChildChangedEvent::Handler&) override {}
        void NotifyChildChangedEvent(AZ::ChildChangeType, AZ::EntityId) override {}
        const AZ::Transform& GetLocalTM() override { return m_world; }
        const AZ::Transform& GetWorldTM() override { return m_world; }
        bool IsStaticTransform() override { return false; }
        void SetWorldTM(const AZ::Transform& tm) override { m_world = tm; }
        void SetWorldTranslation(const AZ::Vector3& position) override { m_world.SetTranslation(position); }
        AZ::Vector3 GetWorldTranslation() override { return m_world.GetTranslation(); }
        void SetWorldRotationQuaternion(const AZ::Quaternion& rotation) override { m_world.SetRotation(rotation); }

        AZ::Transform m_world = AZ::Transform::CreateIdentity();
    };

    /**
     * Stands in for the PhysX character controller and character gameplay components (probe only):
     * integrates the velocity requested each tick, and records the gravity multiplier. A locked
     * vertical axis simulates being pinned under wreckage or standing on the seabed.
     */
    class ProbeCharacterComponent
        : public AZ::Component
        , public Physics::CharacterRequestBus::Handler
        , public PhysX::CharacterGameplayRequestBus::Handler
        , public AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT(ProbeCharacterComponent, "{7C2E9A14-3B6D-4F80-A5C1-9E0D2B4F6A37}");
        static void Reflect(AZ::ReflectContext* context)
        {
            if (auto* serialize = azrtti_cast<AZ::SerializeContext*>(context))
            {
                serialize->Class<ProbeCharacterComponent, AZ::Component>()->Version(1);
            }
        }
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC_CE("PhysicsCharacterControllerService"));
            provided.push_back(AZ_CRC_CE("PhysicsCharacterGameplayService"));
        }
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
            required.push_back(AZ_CRC_CE("TransformService"));
        }
        void Activate() override
        {
            Physics::CharacterRequestBus::Handler::BusConnect(GetEntityId());
            PhysX::CharacterGameplayRequestBus::Handler::BusConnect(GetEntityId());
            AZ::TickBus::Handler::BusConnect();
        }
        void Deactivate() override
        {
            AZ::TickBus::Handler::BusDisconnect();
            PhysX::CharacterGameplayRequestBus::Handler::BusDisconnect();
            Physics::CharacterRequestBus::Handler::BusDisconnect();
        }
        int GetTickOrder() override { return AZ::TICK_LAST; }
        void OnTick(const float deltaTime, AZ::ScriptTimePoint) override
        {
            AZ::Vector3 velocity = m_pending;
            if (m_lockVertical)
            {
                velocity.SetZ(0.0f);
            }
            AZ::Vector3 position = AZ::Vector3::CreateZero();
            AZ::TransformBus::EventResult(position, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
            AZ::Vector3 next = position + velocity * deltaTime;
            if (m_ground)
            {
                next.SetZ(AZ::GetMax(next.GetZ(), m_ground(next.GetX(), next.GetY())));  // seabed / beach contact
            }
            AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetWorldTranslation, next);
            m_velocity = velocity;
            m_pending = AZ::Vector3::CreateZero();
        }

        // Physics::CharacterRequests
        AZ::Vector3 GetBasePosition() const override
        {
            AZ::Vector3 position = AZ::Vector3::CreateZero();
            AZ::TransformBus::EventResult(position, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
            return position;
        }
        void SetBasePosition(const AZ::Vector3& position) override
        {
            AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetWorldTranslation, position);
        }
        AZ::Vector3 GetCenterPosition() const override { return GetBasePosition() + AZ::Vector3(0.0f, 0.0f, 0.9f); }
        float GetStepHeight() const override { return 0.3f; }
        void SetStepHeight(float) override {}
        AZ::Vector3 GetUpDirection() const override { return AZ::Vector3::CreateAxisZ(); }
        void SetUpDirection(const AZ::Vector3&) override {}
        float GetSlopeLimitDegrees() const override { return 45.0f; }
        void SetSlopeLimitDegrees(float) override {}
        float GetMaximumSpeed() const override { return 0.0f; }
        void SetMaximumSpeed(float) override {}
        AZ::Vector3 GetVelocity() const override { return m_velocity; }
        void AddVelocityForTick(const AZ::Vector3& velocity) override { m_pending += velocity; }
        void AddVelocityForPhysicsTimestep(const AZ::Vector3& velocity) override { m_pending += velocity; }
        bool IsPresent() const override { return true; }
        Physics::Character* GetCharacter() override { return nullptr; }

        // PhysX::CharacterGameplayRequests
        bool IsOnGround() const override { return m_lockVertical; }
        float GetGravityMultiplier() const override { return m_gravityMultiplier; }
        void SetGravityMultiplier(const float multiplier) override { m_gravityMultiplier = multiplier; }
        float GetGroundDetectionBoxHeight() const override { return 0.05f; }
        void SetGroundDetectionBoxHeight(float) override {}
        AZ::Vector3 GetFallingVelocity() const override { return m_fallingVelocity; }
        void SetFallingVelocity(const AZ::Vector3& velocity) override { m_fallingVelocity = velocity; }

        float m_gravityMultiplier = 1.0f;
        AZ::Vector3 m_fallingVelocity = AZ::Vector3(0.0f, 0.0f, -3.0f);
        bool m_lockVertical = false;
        /** Optional ground height (world X, Y) the controller cannot sink below. */
        AZStd::function<float(float, float)> m_ground;

    private:
        AZ::Vector3 m_pending = AZ::Vector3::CreateZero();
        AZ::Vector3 m_velocity = AZ::Vector3::CreateZero();
    };
}
