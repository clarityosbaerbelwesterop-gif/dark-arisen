#include "WorldColliderComponent.h"

#include <AzCore/Interface/Interface.h>
#include <AzCore/Math/MathUtils.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/std/smart_ptr/make_shared.h>
#include <AzFramework/Physics/Configuration/StaticRigidBodyConfiguration.h>
#include <AzFramework/Physics/HeightfieldProviderBus.h>
#include <AzFramework/Physics/PhysicsScene.h>
#include <AzFramework/Physics/PhysicsSystem.h>
#include <AzFramework/Physics/Shape.h>
#include <AzFramework/Physics/ShapeConfiguration.h>

#include <cmath>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(WorldColliderComponent, "WorldColliderComponent", WorldColliderComponentTypeId);

    void WorldColliderComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<WorldColliderComponent, AZ::Component>()->Version(1);
            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<WorldColliderComponent>("Dark Arisen World Collider", "Static heightfields for the streamed open world's ground.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Level"));
            }
        }
    }

    void WorldColliderComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("DarkArisenWorldColliderService"));
    }

    void WorldColliderComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("DarkArisenWorldStreamingService"));
    }

    void WorldColliderComponent::Activate()
    {
        if (auto* physics = AZ::Interface<AzPhysics::SystemInterface>::Get())
        {
            m_scene = physics->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
        }
        AZ_Error("WorldCollider", m_scene != AzPhysics::InvalidSceneHandle, "No default physics scene: the open world has no ground.");
        WorldStreamingNotificationBus::Handler::BusConnect();
    }

    void WorldColliderComponent::Deactivate()
    {
        WorldStreamingNotificationBus::Handler::BusDisconnect();
        if (auto* scenes = AZ::Interface<AzPhysics::SceneInterface>::Get())
        {
            for (auto& [tile, body] : m_bodies)
            {
                scenes->RemoveSimulatedBody(m_scene, body);
            }
            for (auto& [tile, boxes] : m_buildings)
            {
                for (AzPhysics::SimulatedBodyHandle& box : boxes)
                {
                    scenes->RemoveSimulatedBody(m_scene, box);
                }
            }
        }
        m_bodies.clear();
        m_buildings.clear();
    }

    void WorldColliderComponent::OnFoliageLoaded(const WorldFoliage& foliage)
    {
        auto* scenes = AZ::Interface<AzPhysics::SceneInterface>::Get();
        const WorldStreamingRequests* world = WorldStreamingInterface::Get();
        if (!scenes || !world || m_scene == AzPhysics::InvalidSceneHandle || foliage.Heights.empty() || m_bodies.find(foliage.Tile) != m_bodies.end())
        {
            return;
        }
        const size_t samples = static_cast<size_t>(std::lround(std::sqrt(static_cast<double>(foliage.Heights.size()))));
        if (samples < 2 || samples * samples != foliage.Heights.size())
        {
            return;
        }
        const float step = world->GetTileSize() / static_cast<float>(samples - 1);

        // PhysX stores heights as int16 within bounds centred on 0: the body sits at the tile's middle height
        float low = foliage.Heights.front(), high = foliage.Heights.front();
        for (const float h : foliage.Heights)
        {
            low = AZStd::min(low, h);
            high = AZStd::max(high, h);
        }
        const float middle = 0.5f * (low + high), half = AZStd::max(0.5f * (high - low), 1.0f);

        // rows run north from the tile's south edge, columns east: the order the heights are stored in. The tile
        // mesh splits each quad from its south-west to its north-east corner.
        AZStd::vector<Physics::HeightMaterialPoint> points;
        points.reserve(foliage.Heights.size());
        for (const float h : foliage.Heights)
        {
            points.emplace_back(h - middle, Physics::QuadMeshType::SubdivideBottomLeftToUpperRight, static_cast<uint8_t>(0));
        }
        auto shape = AZStd::make_shared<Physics::HeightfieldShapeConfiguration>();
        shape->SetGridResolution(AZ::Vector2(step, step));
        shape->SetNumColumnVertices(samples);
        shape->SetNumRowVertices(samples);
        shape->SetMinHeightBounds(-half);
        shape->SetMaxHeightBounds(half);
        shape->SetSamples(points);

        auto collider = AZStd::make_shared<Physics::ColliderConfiguration>();
        AzPhysics::StaticRigidBodyConfiguration body;
        // the heightfield is centred on the body (PhysX gem convention): the tile's centre at its middle height
        body.m_position = AZ::Vector3(foliage.X0 + 0.5f * world->GetTileSize(), foliage.Y0 + 0.5f * world->GetTileSize(), middle);
        body.m_entityId = GetEntityId();
        body.m_debugName = AZStd::string::format("World ground %d", foliage.Tile);
        body.m_colliderAndShapeData = AzPhysics::ShapeColliderPair(collider, shape);
        const AzPhysics::SimulatedBodyHandle handle = scenes->AddSimulatedBody(m_scene, &body);
        if (handle != AzPhysics::InvalidSimulatedBodyHandle)
        {
            m_bodies[foliage.Tile] = handle;
        }

        // the settlement buildings standing on the tile: a box each, base on the ground
        if (const WorldTile* tile = world->GetTile(foliage.Tile))
        {
            AZStd::vector<AzPhysics::SimulatedBodyHandle>& boxes = m_buildings[foliage.Tile];
            for (const WorldStructure& structure : tile->Structures)
            {
                const WorldStructureModel& model = world->GetStructureModels()[structure.Model];
                auto box = AZStd::make_shared<Physics::BoxShapeConfiguration>(AZ::Vector3(2.0f * model.HalfX, 2.0f * model.HalfY, model.Height));
                auto boxCollider = AZStd::make_shared<Physics::ColliderConfiguration>();
                boxCollider->m_position = AZ::Vector3(0.0f, 0.0f, 0.5f * model.Height);
                AzPhysics::StaticRigidBodyConfiguration building;
                building.m_position = AZ::Vector3(structure.X, structure.Y, structure.Z);
                building.m_orientation = AZ::Quaternion::CreateRotationZ(AZ::DegToRad(structure.Yaw));
                building.m_entityId = GetEntityId();
                building.m_colliderAndShapeData = AzPhysics::ShapeColliderPair(boxCollider, box);
                const AzPhysics::SimulatedBodyHandle boxHandle = scenes->AddSimulatedBody(m_scene, &building);
                if (boxHandle != AzPhysics::InvalidSimulatedBodyHandle)
                {
                    boxes.push_back(boxHandle);
                }
            }
        }
    }

    void WorldColliderComponent::OnFoliageUnloaded(const int tile)
    {
        if (const auto buildings = m_buildings.find(tile); buildings != m_buildings.end())
        {
            if (auto* scenes = AZ::Interface<AzPhysics::SceneInterface>::Get())
            {
                for (AzPhysics::SimulatedBodyHandle& box : buildings->second)
                {
                    scenes->RemoveSimulatedBody(m_scene, box);
                }
            }
            m_buildings.erase(buildings);
        }
        const auto found = m_bodies.find(tile);
        if (found == m_bodies.end())
        {
            return;
        }
        if (auto* scenes = AZ::Interface<AzPhysics::SceneInterface>::Get())
        {
            scenes->RemoveSimulatedBody(m_scene, found->second);
        }
        m_bodies.erase(found);
    }
}
