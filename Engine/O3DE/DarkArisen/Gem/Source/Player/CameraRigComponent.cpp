#include "CameraRigComponent.h"

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Math/Quaternion.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Physics/Common/PhysicsSceneQueries.h>
#include <AzFramework/Physics/PhysicsScene.h>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(CameraRigComponent, "CameraRigComponent", CameraRigComponentTypeId);

    namespace
    {
        Core::Point3 ToPoint(const AZ::Vector3& value)
        {
            return {value.GetX(), value.GetY(), value.GetZ()};
        }

        AZ::Vector3 ToVector(const Core::Point3& value)
        {
            return AZ::Vector3(static_cast<float>(value[0]), static_cast<float>(value[1]), static_cast<float>(value[2]));
        }
    }

    void CameraRigComponent::Reflect(AZ::ReflectContext* context)
    {
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }
        serializeContext->Class<CameraRigComponent, AZ::Component>()->Version(1)->Field("Target", &CameraRigComponent::m_target);
        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
        {
            editContext->Class<CameraRigComponent>("Dark Arisen Camera Rig", "Third-person follow camera with lock-on.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                ->DataElement(AZ::Edit::UIHandlers::Default, &CameraRigComponent::m_target, "Target", "The player");
        }
    }

    void CameraRigComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("DarkArisenCameraRigService"));
    }

    void CameraRigComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("DarkArisenCameraRigService"));
    }

    void CameraRigComponent::Activate()
    {
        // Start looking the way the player faces.
        AZ::Transform player = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(player, m_target, &AZ::TransformBus::Events::GetWorldTM);
        const AZ::Vector3 forward = player.GetBasisY();
        m_rig.SetRotation(AZ::RadToDeg(AZ::Atan2(forward.GetX(), forward.GetY())), -10.0);
        CameraRigRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
    }

    void CameraRigComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        CameraRigRequestBus::Handler::BusDisconnect();
    }

    void CameraRigComponent::AddMouseLook(const float deltaXPixels, const float deltaYPixels)
    {
        m_rig.AddMouseLook(deltaXPixels, deltaYPixels);
    }

    AZ::Vector2 CameraRigComponent::GetForward() const
    {
        const auto forward = m_rig.Forward();
        return AZ::Vector2(static_cast<float>(forward[0]), static_cast<float>(forward[1]));
    }

    AZ::Vector2 CameraRigComponent::GetRight() const
    {
        const auto right = m_rig.Right();
        return AZ::Vector2(static_cast<float>(right[0]), static_cast<float>(right[1]));
    }

    float CameraRigComponent::ArmClearance(const AZ::Vector3& pivot, const AZ::Vector3& desired) const
    {
        auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
        const AZ::Vector3 arm = desired - pivot;
        const float length = arm.GetLength();
        if (!sceneInterface || length < 1e-3f)
        {
            return length;
        }
        AzPhysics::RayCastRequest request;
        request.m_start = pivot;
        request.m_direction = arm / length;
        request.m_distance = length;
        request.m_reportMultipleHits = true;
        const AzPhysics::SceneQueryHits hits =
            sceneInterface->QueryScene(sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName), &request);
        float clearance = length;
        for (const AzPhysics::SceneQueryHit& hit : hits.m_hits)
        {
            if (hit.m_entityId != m_target)
            {
                clearance = AZ::GetMin(clearance, hit.m_distance);
            }
        }
        return clearance;
    }

    void CameraRigComponent::OnTick(const float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        AZ::Vector3 base = AZ::Vector3::CreateZero();
        AZ::TransformBus::EventResult(base, m_target, &AZ::TransformBus::Events::GetWorldTranslation);
        m_rig.AddStickLook(m_stick.GetX(), m_stick.GetY(), deltaTime);

        std::optional<Core::Point3> lockTarget;
        AZ::EntityId target;
        LockOnRequestBus::EventResult(target, m_target, &LockOnRequests::GetLockTarget);
        if (target.IsValid())
        {
            AZ::Vector3 position = AZ::Vector3::CreateZero();
            AZ::TransformBus::EventResult(position, target, &AZ::TransformBus::Events::GetWorldTranslation);
            lockTarget = ToPoint(position);
        }
        // Pose without collision first, then shorten the arm to what the ray finds free.
        Core::CameraRig probe = m_rig;
        const Core::CameraPose unobstructed = probe.Update(ToPoint(base), lockTarget, deltaTime);
        const AZ::Vector3 pivot = base + AZ::Vector3(0.0f, 0.0f, static_cast<float>(m_rig.Tuning.PivotHeight));
        const float clearance = ArmClearance(pivot, ToVector(unobstructed.Position));
        const float armLength = static_cast<float>(m_rig.Tuning.ArmLength);
        const Core::CameraPose pose = m_rig.Update(ToPoint(base), lockTarget, deltaTime,
            clearance < (ToVector(unobstructed.Position) - pivot).GetLength() - 1e-3f ? std::optional<double>(clearance * armLength /
                AZ::GetMax((ToVector(unobstructed.Position) - pivot).GetLength(), 1e-3f)) : std::nullopt);
        const AZ::Quaternion rotation = AZ::Quaternion::CreateRotationZ(AZ::DegToRad(static_cast<float>(-pose.YawDegrees))) *
            AZ::Quaternion::CreateRotationX(AZ::DegToRad(static_cast<float>(pose.PitchDegrees)));
        AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetWorldTM,
            AZ::Transform::CreateFromQuaternionAndTranslation(rotation, ToVector(pose.Position)));
    }
}
