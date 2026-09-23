#include "JakeInputComponent.h"

#include "Story/StoryTriggerComponent.h"

#include <DarkArisen/CombatBus.h>

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Input/Devices/Gamepad/InputDeviceGamepad.h>
#include <AzFramework/Input/Devices/Keyboard/InputDeviceKeyboard.h>
#include <AzFramework/Input/Devices/Mouse/InputDeviceMouse.h>
#include <AzFramework/Physics/CharacterBus.h>
#include <AzFramework/Physics/Common/PhysicsSceneQueries.h>
#include <AzFramework/Physics/PhysicsScene.h>

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

        // Movement axes.
        if (id == Keyboard::AlphanumericW) { m_keyboardMove.SetY(value); return false; }
        if (id == Keyboard::AlphanumericS) { m_keyboardMove.SetY(-value); return false; }
        if (id == Keyboard::AlphanumericD) { m_keyboardMove.SetX(value); return false; }
        if (id == Keyboard::AlphanumericA) { m_keyboardMove.SetX(-value); return false; }
        if (id == Gamepad::ThumbStickAxis1D::LX) { m_stickMove.SetX(value); return false; }
        if (id == Gamepad::ThumbStickAxis1D::LY) { m_stickMove.SetY(value); return false; }
        if (id == Keyboard::ModifierShiftL || id == Gamepad::Button::L3)
        {
            m_sprintHeld = inputChannel.IsActive();
            CombatRequestBus::Event(self, &CombatRequests::SetSprinting, m_sprintHeld);
            return false;
        }
        if (!pressed)
        {
            return false;
        }

        // Committed actions: the combat model rejects anything that would cancel one.
        if (id == Mouse::Left || id == Gamepad::Button::R1) CombatRequestBus::Event(self, &CombatRequests::LightAttack);
        else if (id == Mouse::Right || id == Gamepad::Trigger::R2) CombatRequestBus::Event(self, &CombatRequests::HeavyAttack);
        else if (id == Keyboard::AlphanumericQ || id == Gamepad::Button::L1) CombatRequestBus::Event(self, &CombatRequests::Parry);
        else if (id == Keyboard::EditSpace || id == Gamepad::Button::B)
            CombatRequestBus::Event(self, &CombatRequests::Dodge, (m_keyboardMove + m_stickMove).GetLengthSq() < 0.01f);
        else if (id == Keyboard::AlphanumericR || id == Gamepad::Button::Y) CombatRequestBus::Event(self, &CombatRequests::StartRache);
        else if (id == Keyboard::AlphanumericE || id == Gamepad::Button::A) Interact();
        return false;
    }

    void JakeInputComponent::OnTick([[maybe_unused]] const float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        bool committedOrDead = true;
        CombatRequestBus::Event(GetEntityId(), [&committedOrDead](CombatRequests* handler)
        {
            const Core::Combatant& combatant = handler->GetCombatant();
            committedOrDead = combatant.Health.IsDead() || combatant.Combat.IsActionCommitted();
        });
        if (committedOrDead)
        {
            return;
        }
        AZ::Vector2 move = m_keyboardMove + m_stickMove;
        if (move.GetLengthSq() > 1.0f)
        {
            move.Normalize();
        }
        const float speed = m_sprintHeld ? SprintSpeedMetresPerSecond : RunSpeedMetresPerSecond;
        const AZ::Vector3 velocity(move.GetX() * speed, move.GetY() * speed, 0.0f);
        Physics::CharacterRequestBus::Event(GetEntityId(), &Physics::CharacterRequests::AddVelocityForTick, velocity);
        if (move.GetLengthSq() > 0.0001f)
        {
            AZ::TransformBus::Event(GetEntityId(), &AZ::TransformBus::Events::SetWorldRotationQuaternion,
                AZ::Quaternion::CreateRotationZ(AZ::Atan2(-move.GetX(), move.GetY())));
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
