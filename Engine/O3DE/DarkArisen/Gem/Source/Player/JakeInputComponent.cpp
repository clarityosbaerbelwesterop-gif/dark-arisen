#include "JakeInputComponent.h"

#include "Story/StoryTriggerComponent.h"

#include <DarkArisen/CameraBus.h>
#include <DarkArisen/CampaignBus.h>
#include <DarkArisen/CombatBus.h>
#include <DarkArisen/SwimBus.h>

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Input/Devices/Gamepad/InputDeviceGamepad.h>
#include <AzFramework/Input/Devices/Keyboard/InputDeviceKeyboard.h>
#include <AzFramework/Input/Devices/Mouse/InputDeviceMouse.h>
#include <AzFramework/Physics/CharacterBus.h>
#include <AzFramework/Physics/Common/PhysicsSceneQueries.h>
#include <AzFramework/Physics/PhysicsScene.h>
#include <PhysX/CharacterGameplayBus.h>

#include <DarkArisen/Core/Combat.h>
#include <DarkArisen/Core/DesignLaws.h>
#include <DarkArisen/Core/EntityPolicy.h>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(JakeInputComponent, "JakeInputComponent", JakeInputComponentTypeId);

    namespace
    {
        using Keyboard = AzFramework::InputDeviceKeyboard::Key;
        using Mouse = AzFramework::InputDeviceMouse::Button;
        using MouseMovement = AzFramework::InputDeviceMouse::Movement;
        using Gamepad = AzFramework::InputDeviceGamepad;
    }

    void JakeInputComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<JakeInputComponent, AZ::Component>()->Version(1);
            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<JakeInputComponent>("Dark Arisen Jake Input", "Player control for Jake.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"));
            }
        }
    }

    void JakeInputComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("DarkArisenCombatantService"));
        required.push_back(AZ_CRC_CE("PhysicsCharacterControllerService"));
    }

    void JakeInputComponent::Activate()
    {
        AzFramework::InputChannelEventListener::Connect();
        AZ::TickBus::Handler::BusConnect();
    }

    void JakeInputComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        AzFramework::InputChannelEventListener::Disconnect();
    }

    bool JakeInputComponent::OnInputChannelEventFiltered(const AzFramework::InputChannel& inputChannel)
    {
        const AzFramework::InputChannelId& id = inputChannel.GetInputChannelId();
        const bool pressed = inputChannel.IsStateBegan();
        const float value = inputChannel.GetValue();
        const AZ::EntityId self = GetEntityId();

        // Look (Unreal: Turn/LookUp on the mouse, TurnRate/LookUpRate on the right stick).
        if (id == MouseMovement::X) { CameraRigRequestBus::Broadcast(&CameraRigRequests::AddMouseLook, value, 0.0f); return false; }
        if (id == MouseMovement::Y) { CameraRigRequestBus::Broadcast(&CameraRigRequests::AddMouseLook, 0.0f, value); return false; }
        if (id == Gamepad::ThumbStickAxis1D::RX || id == Gamepad::ThumbStickAxis1D::RY)
        {
            (id == Gamepad::ThumbStickAxis1D::RX) ? m_stickLook.SetX(value) : m_stickLook.SetY(value);
            CameraRigRequestBus::Broadcast(&CameraRigRequests::SetStickLook, m_stickLook);
            return false;
        }

        // Movement axes (camera relative in OnTick).
        if (id == Keyboard::AlphanumericW) { m_keyboardMove.SetY(value); return false; }
        if (id == Keyboard::AlphanumericS) { m_keyboardMove.SetY(-value); return false; }
        if (id == Keyboard::AlphanumericD) { m_keyboardMove.SetX(value); return false; }
        if (id == Keyboard::AlphanumericA) { m_keyboardMove.SetX(-value); return false; }
        if (id == Gamepad::ThumbStickAxis1D::LX) { m_stickMove.SetX(value); return false; }
        if (id == Gamepad::ThumbStickAxis1D::LY) { m_stickMove.SetY(value); return false; }
        if (id == Keyboard::ModifierShiftL || id == Gamepad::Button::L3)
        {
            m_sprintHeld = inputChannel.IsActive();
            CombatRequestBus::Event(self, &CombatRequests::SetSprinting, m_sprintHeld && !m_wasSwimming);
            return false;
        }
        if (!pressed)
        {
            return false;
        }

        // Actions, bound as Unreal's DefaultInput.ini. Committed actions: the combat model rejects
        // anything that would cancel one.
        if (id == Mouse::Left || id == Gamepad::Button::R1) CombatRequestBus::Event(self, &CombatRequests::LightAttack);
        else if (id == Mouse::Right || id == Gamepad::Trigger::R2) CombatRequestBus::Event(self, &CombatRequests::HeavyAttack);
        else if (id == Keyboard::AlphanumericQ || id == Gamepad::Trigger::L2 || id == Gamepad::Button::X)
            CombatRequestBus::Event(self, &CombatRequests::Parry);
        else if (id == Keyboard::ModifierAltL || id == Gamepad::Button::B)
            CombatRequestBus::Event(self, &CombatRequests::Dodge, (m_keyboardMove + m_stickMove).GetLengthSq() < 0.01f);
        else if (id == Keyboard::AlphanumericR) CombatRequestBus::Event(self, &CombatRequests::StartRache);
        else if (id == Keyboard::AlphanumericE || id == Gamepad::Button::Y) Interact();
        else if (id == Keyboard::EditTab || id == Gamepad::Button::R3) LockOnRequestBus::Event(self, &LockOnRequests::ToggleLockOn);
        else if (id == Keyboard::EditSpace || id == Gamepad::Button::A) Jump();
        else if (id == Keyboard::Function05) QuickSave(true);
        else if (id == Keyboard::Function09) QuickSave(false);
        return false;
    }

    void JakeInputComponent::Jump() const
    {
        bool committedOrDead = true;
        CombatRequestBus::Event(GetEntityId(), [&committedOrDead](CombatRequests* handler)
        {
            const Core::Combatant& combatant = handler->GetCombatant();
            committedOrDead = combatant.Health.IsDead() || combatant.Combat.IsActionCommitted();
        });
        bool grounded = false;
        PhysX::CharacterGameplayRequestBus::EventResult(grounded, GetEntityId(), &PhysX::CharacterGameplayRequests::IsOnGround);
        if (committedOrDead || m_wasSwimming || !grounded)
        {
            return;
        }
        PhysX::CharacterGameplayRequestBus::Event(GetEntityId(), &PhysX::CharacterGameplayRequests::SetFallingVelocity,
            AZ::Vector3(0.0f, 0.0f, JumpSpeedMetresPerSecond));
    }

    void JakeInputComponent::QuickSave(const bool save) const
    {
        CampaignRequests* campaign = CampaignInterface::Get();
        if (!campaign)
        {
            return;
        }
        const bool done = save ? campaign->SaveToSlot(QuickSlot) : campaign->LoadFromSlot(QuickSlot);
        AZ_Warning("DarkArisen", done, "Quick %s failed.", save ? "save" : "load");
    }

    void JakeInputComponent::OnTick([[maybe_unused]] const float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        bool committedOrDead = true;
        CombatRequestBus::Event(GetEntityId(), [&committedOrDead](CombatRequests* handler)
        {
            const Core::Combatant& combatant = handler->GetCombatant();
            committedOrDead = combatant.Health.IsDead() || combatant.Combat.IsActionCommitted();
        });
        AZ::Vector2 input = m_keyboardMove + m_stickMove;
        if (input.GetLengthSq() > 1.0f)
        {
            input.Normalize();
        }
        // Camera relative (Unreal: control-rotation yaw); world axes when no camera rig is present.
        AZ::Vector2 forward(0.0f, 1.0f);
        AZ::Vector2 right(1.0f, 0.0f);
        CameraRigRequestBus::BroadcastResult(forward, &CameraRigRequests::GetForward);
        CameraRigRequestBus::BroadcastResult(right, &CameraRigRequests::GetRight);
        AZ::Vector2 move = right * input.GetX() + forward * input.GetY();
        if (committedOrDead)
        {
            move = AZ::Vector2::CreateZero();
        }

        // In the water the swimmer owns movement: stroke direction and pace, no land sprint drain.
        bool swimming = false;
        SwimRequestBus::EventResult(swimming, GetEntityId(), &SwimRequests::IsSwimming);
        if (swimming != m_wasSwimming)
        {
            m_wasSwimming = swimming;
            CombatRequestBus::Event(GetEntityId(), &CombatRequests::SetSprinting, !swimming && m_sprintHeld);
        }
        if (swimming)
        {
            SwimRequestBus::Event(GetEntityId(), &SwimRequests::SetSwimIntent, move, m_sprintHeld && !committedOrDead);
            return;
        }
        if (committedOrDead)
        {
            return;
        }

        const float speed = m_sprintHeld ? SprintSpeedMetresPerSecond : RunSpeedMetresPerSecond;
        const AZ::Vector3 velocity(move.GetX() * speed, move.GetY() * speed, 0.0f);
        Physics::CharacterRequestBus::Event(GetEntityId(), &Physics::CharacterRequests::AddVelocityForTick, velocity);
        // Locked on, Jake faces the target (Unreal: controller yaw drives the pawn); otherwise he turns to movement.
        AZ::EntityId lockTarget;
        LockOnRequestBus::EventResult(lockTarget, GetEntityId(), &LockOnRequests::GetLockTarget);
        AZ::Vector2 facing = move;
        if (lockTarget.IsValid())
        {
            AZ::Vector3 self = AZ::Vector3::CreateZero();
            AZ::Vector3 target = AZ::Vector3::CreateZero();
            AZ::TransformBus::EventResult(self, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
            AZ::TransformBus::EventResult(target, lockTarget, &AZ::TransformBus::Events::GetWorldTranslation);
            facing = AZ::Vector2(target.GetX() - self.GetX(), target.GetY() - self.GetY());
        }
        if (facing.GetLengthSq() > 0.0001f)
        {
            AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetWorldRotationQuaternion,
                AZ::Quaternion::CreateRotationZ(AZ::Atan2(-facing.GetX(), facing.GetY())));
        }
    }

    void JakeInputComponent::Interact() const
    {
        auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
        if (!sceneInterface)
        {
            return;
        }
        AZ::Vector3 position = AZ::Vector3::CreateZero();
        AZ::TransformBus::EventResult(position, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
        // Design law 2: the prompt range is exactly 1.4 m; no highlight, no fade.
        AzPhysics::OverlapRequest request = AzPhysics::OverlapRequestHelpers::CreateSphereOverlapRequest(
            Core::DesignLaws::InteractionPromptRangeMetres, AZ::Transform::CreateTranslation(position));
        const AzPhysics::SceneQueryHits hits =
            sceneInterface->QueryScene(sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName), &request);
        for (const AzPhysics::SceneQueryHit& hit : hits.m_hits)
        {
            bool accepted = false;
            StoryTriggerRequestBus::EventResult(accepted, hit.m_entityId, &StoryTriggerRequests::Interact,
                AZStd::string(Core::EntityPolicy::JakeId.data()));
            if (accepted)
            {
                return;
            }
        }
    }
}
