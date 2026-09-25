#include "WaterVolumeComponent.h"

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Physics/Collision/CollisionEvents.h>
#include <AzFramework/Physics/Common/PhysicsSimulatedBody.h>
#include <AzFramework/Physics/Components/SimulatedBodyComponentBus.h>
#include <AzFramework/Physics/PhysicsScene.h>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(WaterVolumeComponent, "WaterVolumeComponent", WaterVolumeComponentTypeId);

    void WaterVolumeComponent::Reflect(AZ::ReflectContext* context)
    {
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }
        serializeContext->Class<WaterVolumeComponent, AZ::Component>()
            ->Version(1)
            ->Field("CurrentAcceleration", &WaterVolumeComponent::m_currentAcceleration)
            ->Field("UseOceanSurface", &WaterVolumeComponent::m_useOceanSurface)
            ->Field("SurfaceOffset", &WaterVolumeComponent::m_surfaceOffset)
            ->Field("ShallowExit", &WaterVolumeComponent::m_shallowExit)
            ->Field("HalfExtents", &WaterVolumeComponent::m_halfExtents);
        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
        {
            editContext->Class<WaterVolumeComponent>("Dark Arisen Water Volume", "Swim water with an optional current.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                ->DataElement(AZ::Edit::UIHandlers::Default, &WaterVolumeComponent::m_currentAcceleration, "Current (m/s^2)",
                    "World-space pull; steady drift = current / water drag")
                ->DataElement(AZ::Edit::UIHandlers::Default, &WaterVolumeComponent::m_useOceanSurface, "Use Ocean Surface", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &WaterVolumeComponent::m_surfaceOffset, "Surface Offset (m)",
                    "Surface above this entity when there is no ocean")
                ->DataElement(AZ::Edit::UIHandlers::Default, &WaterVolumeComponent::m_shallowExit, "Shallow Exit",
                    "Leaving towards wadeable ground returns to walking")
                ->DataElement(AZ::Edit::UIHandlers::Default, &WaterVolumeComponent::m_halfExtents, "Half Extents (m)",
                    "Same box as the trigger collider; used when a swimmer is placed inside");
        }
    }

    void WaterVolumeComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC_CE("PhysicsWorldBodyService"));
    }

    WaterVolumeInfo WaterVolumeComponent::BuildInfo() const
    {
        WaterVolumeInfo info;
        info.m_volume = GetEntityId();
        info.m_currentAcceleration = m_currentAcceleration;
        info.m_useOceanSurface = m_useOceanSurface;
        AZ::Vector3 position = AZ::Vector3::CreateZero();
        AZ::TransformBus::EventResult(position, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
        info.m_surfaceHeight = position.GetZ() + m_surfaceOffset;
        info.m_shallowExit = m_shallowExit;
        return info;
    }

    void WaterVolumeComponent::NotifyEntered(const AZ::EntityId other) const
    {
        SwimRequestBus::Event(other, &SwimRequests::OnEnteredWaterVolume, BuildInfo());
    }

    void WaterVolumeComponent::NotifyExited(const AZ::EntityId other) const
    {
        SwimRequestBus::Event(other, &SwimRequests::OnExitedWaterVolume, GetEntityId());
    }

    void WaterVolumeComponent::AppendIfContains(const AZ::Vector3& point, AZStd::vector<WaterVolumeInfo>& volumes) const
    {
        AZ::Vector3 center = AZ::Vector3::CreateZero();
        AZ::TransformBus::EventResult(center, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
        const AZ::Vector3 offset = (point - center).GetAbs();
        if (offset.IsLessEqualThan(m_halfExtents) && !m_halfExtents.IsZero())
        {
            volumes.push_back(BuildInfo());
        }
    }

    void WaterVolumeComponent::Activate()
    {
        WaterVolumeQueryBus::Handler::BusConnect();
        m_enterHandler = AzPhysics::SimulatedBodyEvents::OnTriggerEnter::Handler(
            [this]([[maybe_unused]] AzPhysics::SimulatedBodyHandle bodyHandle, const AzPhysics::TriggerEvent& event)
            {
                if (event.m_otherBody)
                {
                    NotifyEntered(event.m_otherBody->GetEntityId());
                }
            });
        m_exitHandler = AzPhysics::SimulatedBodyEvents::OnTriggerExit::Handler(
            [this]([[maybe_unused]] AzPhysics::SimulatedBodyHandle bodyHandle, const AzPhysics::TriggerEvent& event)
            {
                if (event.m_otherBody)
                {
                    NotifyExited(event.m_otherBody->GetEntityId());
                }
            });

        AzPhysics::SimulatedBodyHandle bodyHandle = AzPhysics::InvalidSimulatedBodyHandle;
        AzPhysics::SimulatedBodyComponentRequestsBus::EventResult(
            bodyHandle, GetEntityId(), &AzPhysics::SimulatedBodyComponentRequests::GetSimulatedBodyHandle);
        auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
        if (bodyHandle == AzPhysics::InvalidSimulatedBodyHandle || !sceneInterface)
        {
            AZ_Error("DarkArisen", false, "Water volume on %s needs a PhysX trigger collider.", GetEntityId().ToString().c_str());
            return;
        }
        const AzPhysics::SceneHandle sceneHandle = sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
        AzPhysics::SimulatedBodyEvents::RegisterOnTriggerEnterHandler(sceneHandle, bodyHandle, m_enterHandler);
        AzPhysics::SimulatedBodyEvents::RegisterOnTriggerExitHandler(sceneHandle, bodyHandle, m_exitHandler);
    }

    void WaterVolumeComponent::Deactivate()
    {
        WaterVolumeQueryBus::Handler::BusDisconnect();
        m_exitHandler.Disconnect();
        m_enterHandler.Disconnect();
    }
}
