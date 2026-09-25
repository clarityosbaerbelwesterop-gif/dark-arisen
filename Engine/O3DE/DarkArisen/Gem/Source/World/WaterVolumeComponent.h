#pragma once

#include <DarkArisen/SwimBus.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Math/Vector3.h>
#include <AzFramework/Physics/Common/PhysicsSimulatedBodyEvents.h>

namespace DarkArisen
{
    /**
     * Authored swim water with an optional current (Unreal AOpeningWaterCurrentVolume, physical
     * half). Needs a PhysX trigger collider on the same entity. Story beats such as the water
     * entry or the Outer Reef go on a StoryTriggerComponent on the same entity.
     */
    class WaterVolumeComponent
        : public AZ::Component
        , protected WaterVolumeQueryBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(WaterVolumeComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        /** Relays a body entering/leaving the trigger to its SwimRequestBus handler, if any. */
        void NotifyEntered(AZ::EntityId other) const;
        void NotifyExited(AZ::EntityId other) const;
        WaterVolumeInfo BuildInfo() const;

    protected:
        void Activate() override;
        void Deactivate() override;
        void AppendIfContains(const AZ::Vector3& point, AZStd::vector<WaterVolumeInfo>& volumes) const override;

    private:
        /** m/s^2. ContentSource volumes author cm/s^2. */
        AZ::Vector3 m_currentAcceleration = AZ::Vector3::CreateZero();
        bool m_useOceanSurface = true;
        /** Surface height above this entity when there is no ocean (enclosed water). */
        float m_surfaceOffset = 0.0f;
        bool m_shallowExit = false;
        /** Axis-aligned half size matching the trigger collider, for containment queries. */
        AZ::Vector3 m_halfExtents = AZ::Vector3::CreateZero();

        AzPhysics::SimulatedBodyEvents::OnTriggerEnter::Handler m_enterHandler;
        AzPhysics::SimulatedBodyEvents::OnTriggerExit::Handler m_exitHandler;
    };
}
