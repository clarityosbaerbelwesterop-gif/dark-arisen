#include "LockOnComponent.h"

#include <DarkArisen/CombatBus.h>

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/std/containers/vector.h>
#include <AzFramework/Physics/Common/PhysicsSceneQueries.h>
#include <AzFramework/Physics/PhysicsScene.h>

#include <DarkArisen/Core/Camera.h>
#include <DarkArisen/Core/Combat.h>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(LockOnComponent, "LockOnComponent", LockOnComponentTypeId);

    namespace
    {
        struct Found
        {
            AZ::EntityId m_entity;
            Core::LockOnCandidate m_candidate;
        };

        constexpr float AimHeight = 0.55f;  // Unreal: actor location + 55 cm for sight and aim
    }

    void LockOnComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<LockOnComponent, AZ::Component>()->Version(1);
            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<LockOnComponent>("Dark Arisen Lock-On", "Target lock for the player.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"));
            }
        }
    }

    void LockOnComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("DarkArisenCombatantService"));
    }

    void LockOnComponent::Activate()
    {
        m_target.SetInvalid();
        LockOnRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TickBus::Handler::BusConnect();
    }

    void LockOnComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        LockOnRequestBus::Handler::BusDisconnect();
        m_target.SetInvalid();
    }

    bool LockOnComponent::InSight(const AZ::Vector3& from, const AZ::Vector3& to, const AZ::EntityId target) const
    {
        auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
        const AZ::Vector3 ray = to - from;
        if (!sceneInterface || ray.GetLength() < 1e-3f)
        {
            return true;  // no physics scene (tools, probes): sight is not the deciding rule
        }
        AzPhysics::RayCastRequest request;
        request.m_start = from;
        request.m_direction = ray.GetNormalized();
        request.m_distance = ray.GetLength();
        request.m_reportMultipleHits = true;
        const AzPhysics::SceneQueryHits hits =
            sceneInterface->QueryScene(sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName), &request);
        for (const AzPhysics::SceneQueryHit& hit : hits.m_hits)
        {
            if (hit.m_entityId != GetEntityId() && hit.m_entityId != target)
            {
                return false;
            }
        }
        return true;
    }

    bool LockOnComponent::ToggleLockOn()
    {
        if (m_target.IsValid())
        {
            m_target.SetInvalid();
            return false;
        }
        AZ::Transform self = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(self, GetEntityId(), &AZ::TransformBus::Events::GetWorldTM);
        const AZ::Vector3 eye = self.GetTranslation() + AZ::Vector3(0.0f, 0.0f, AimHeight);
        AZStd::vector<Found> found;
        CombatRequestBus::EnumerateHandlers([this, &found, &eye](CombatRequests* handler)
        {
            const AZ::EntityId entity = handler->GetCombatantEntityId();
            if (entity == GetEntityId())
            {
                return true;
            }
            AZ::Vector3 position = AZ::Vector3::CreateZero();
            AZ::TransformBus::EventResult(position, entity, &AZ::TransformBus::Events::GetWorldTranslation);
            Core::LockOnCandidate candidate;
            candidate.CombatantId = handler->GetCombatantId().c_str();
            candidate.Position = {position.GetX(), position.GetY(), position.GetZ()};
            candidate.Alive = !handler->IsDead();
            candidate.Targetable = handler->GetCombatant().Combat.IsTargetable();
            candidate.LineOfSight = InSight(eye, position + AZ::Vector3(0.0f, 0.0f, AimHeight), entity);
            found.push_back({entity, AZStd::move(candidate)});
            return true;
        });
        std::vector<Core::LockOnCandidate> candidates;
        for (const Found& entry : found)
        {
            candidates.push_back(entry.m_candidate);
        }
        const AZ::Vector3 forward = self.GetBasisY();
        const AZ::Vector3 origin = self.GetTranslation();
        const int chosen = Core::SelectLockTarget({origin.GetX(), origin.GetY(), origin.GetZ()}, {forward.GetX(), forward.GetY()}, candidates);
        if (chosen < 0)
        {
            return false;
        }
        m_target = found[static_cast<size_t>(chosen)].m_entity;
        return true;
    }

    void LockOnComponent::OnTick([[maybe_unused]] const float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        if (!m_target.IsValid())
        {
            return;
        }
        bool present = false;
        Core::LockOnCandidate held;
        CombatRequestBus::Event(m_target, [&present, &held](CombatRequests* handler)
        {
            present = true;
            held.CombatantId = handler->GetCombatantId().c_str();
            held.Alive = !handler->IsDead();
            held.Targetable = handler->GetCombatant().Combat.IsTargetable();
        });
        AZ::Vector3 self = AZ::Vector3::CreateZero();
        AZ::Vector3 target = AZ::Vector3::CreateZero();
        AZ::TransformBus::EventResult(self, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
        AZ::TransformBus::EventResult(target, m_target, &AZ::TransformBus::Events::GetWorldTranslation);
        held.Position = {target.GetX(), target.GetY(), target.GetZ()};
        held.LineOfSight = InSight(self + AZ::Vector3(0.0f, 0.0f, AimHeight), target + AZ::Vector3(0.0f, 0.0f, AimHeight), m_target);
        if (!present || !Core::RetainLockTarget({self.GetX(), self.GetY(), self.GetZ()}, held))
        {
            m_target.SetInvalid();
        }
    }
}
