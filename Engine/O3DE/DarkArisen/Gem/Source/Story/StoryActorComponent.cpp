#include "StoryActorComponent.h"

#include <DarkArisen/CampaignBus.h>
#include <DarkArisen/CombatBus.h>
#include <DarkArisen/EnemyBus.h>
#include <DarkArisen/NavalBus.h>
#include <DarkArisen/PresentationBus.h>

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Physics/Common/PhysicsSimulatedBody.h>
#include <AzFramework/Physics/Components/SimulatedBodyComponentBus.h>
#include <AzFramework/Physics/PhysicsScene.h>

#include <DarkArisen/Core/CampaignRuntime.h>
#include <DarkArisen/Core/Combat.h>
#include <DarkArisen/Core/EntityPolicy.h>

#include <limits>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(StoryActorComponent, "StoryActorComponent", StoryActorComponentTypeId);

    void StoryActorComponent::Reflect(AZ::ReflectContext* context)
    {
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }
        serializeContext->Class<StoryActorComponent, AZ::Component>()
            ->Version(1)
            ->Field("Kind", &StoryActorComponent::m_kind)
            ->Field("MissionId", &StoryActorComponent::m_missionId)
            ->Field("CheckpointId", &StoryActorComponent::m_checkpointId)
            ->Field("SpawnId", &StoryActorComponent::m_spawnId)
            ->Field("Action", &StoryActorComponent::m_action)
            ->Field("ContactId", &StoryActorComponent::m_contactId)
            ->Field("DialogueId", &StoryActorComponent::m_dialogueId)
            ->Field("DisplayName", &StoryActorComponent::m_displayName)
            ->Field("Text", &StoryActorComponent::m_text)
            ->Field("Title", &StoryActorComponent::m_title)
            ->Field("Body", &StoryActorComponent::m_body)
            ->Field("WorldFactOnRead", &StoryActorComponent::m_worldFactOnRead)
            ->Field("RequiredBossId", &StoryActorComponent::m_requiredBossId)
            ->Field("CompletesMission", &StoryActorComponent::m_completesMission)
            ->Field("OutcomeKey", &StoryActorComponent::m_outcomeKey)
            ->Field("OutcomeValue", &StoryActorComponent::m_outcomeValue)
            ->Field("Label", &StoryActorComponent::m_label)
            ->Field("RegionId", &StoryActorComponent::m_regionId)
            ->Field("InitialController", &StoryActorComponent::m_initialController)
            ->Field("Verb", &StoryActorComponent::m_verb)
            ->Field("TargetFaction", &StoryActorComponent::m_targetFaction)
            ->Field("ControlDelta", &StoryActorComponent::m_controlDelta)
            ->Field("LiberationDelta", &StoryActorComponent::m_liberationDelta)
            ->Field("CrimsonDelta", &StoryActorComponent::m_crimsonDelta)
            ->Field("HoldingId", &StoryActorComponent::m_holdingId)
            ->Field("RequiredDefeats", &StoryActorComponent::m_requiredDefeats)
            ->Field("RequiredWorldFact", &StoryActorComponent::m_requiredWorldFact)
            ->Field("CompletionRadius", &StoryActorComponent::m_completionRadiusMetres);

        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
        {
            editContext->Enum<Core::StoryActorKind>("Story Actor Kind", "")
                ->Value("Location trigger", Core::StoryActorKind::LocationTrigger)
                ->Value("Contact", Core::StoryActorKind::Contact)
                ->Value("Evidence", Core::StoryActorKind::Evidence)
                ->Value("Route resolution", Core::StoryActorKind::RouteResolution)
                ->Value("War action", Core::StoryActorKind::WarAction)
                ->Value("Naval encounter gate", Core::StoryActorKind::NavalEncounterGate);
            editContext->Class<StoryActorComponent>("Dark Arisen Story Actor",
                    "Physical story actor from a mission contract. Materialised; edit ContentSource, not the level.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                ->DataElement(AZ::Edit::UIHandlers::ComboBox, &StoryActorComponent::m_kind, "Kind", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &StoryActorComponent::m_missionId, "Mission", "");
        }
    }

    void StoryActorComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC_CE("PhysicsWorldBodyService"));
    }

    Core::StoryActorSpec StoryActorComponent::BuildSpec() const
    {
        Core::StoryActorSpec Spec;
        Spec.Kind = m_kind;
        Spec.MissionId = m_missionId.c_str();
        Spec.CheckpointId = m_checkpointId.c_str();
        Spec.SpawnId = m_spawnId.c_str();
        Spec.Action = m_action;
        Spec.ContactId = m_contactId.c_str();
        Spec.DialogueId = m_dialogueId.c_str();
        Spec.DisplayName = m_displayName.c_str();
        Spec.Text = m_text.c_str();
        Spec.Title = m_title.c_str();
        Spec.Body = m_body.c_str();
        Spec.WorldFactOnRead = m_worldFactOnRead.c_str();
        Spec.RequiredBossId = m_requiredBossId.c_str();
        Spec.CompletesMission = m_completesMission;
        Spec.OutcomeKey = m_outcomeKey.c_str();
        Spec.OutcomeValue = m_outcomeValue.c_str();
        Spec.Label = m_label.c_str();
        Spec.RegionId = m_regionId.c_str();
        Spec.InitialController = m_initialController;
        Spec.Verb = m_verb;
        Spec.TargetFaction = m_targetFaction;
        Spec.ControlDelta = m_controlDelta;
        Spec.LiberationDelta = m_liberationDelta;
        Spec.CrimsonDelta = m_crimsonDelta;
        Spec.HoldingId = m_holdingId.c_str();
        Spec.RequiredDefeats = m_requiredDefeats;
        Spec.RequiredWorldFact = m_requiredWorldFact.c_str();
        Spec.CompletionRadiusMetres = m_completionRadiusMetres;
        return Spec;
    }

    void StoryActorComponent::Activate()
    {
        m_spec = BuildSpec();
        m_gateResolved = false;
        m_gateTimer = 0.0f;
        m_jakeWaiting = false;
        if (m_spec.Kind == Core::StoryActorKind::NavalEncounterGate)
        {
            AZ::TickBus::Handler::BusConnect();
            return;
        }
        if (m_spec.Kind == Core::StoryActorKind::LocationTrigger && m_spec.Action == Core::LocationAction::CompleteMission)
        {
            AZ::TickBus::Handler::BusConnect(); // re-checks while Jake waits inside for the fight to end
        }
        StoryTriggerRequestBus::Handler::BusConnect(GetEntityId());
        if (m_spec.Kind != Core::StoryActorKind::LocationTrigger)
        {
            return;
        }
        m_triggerEnterHandler = AzPhysics::SimulatedBodyEvents::OnTriggerEnter::Handler(
            [this]([[maybe_unused]] AzPhysics::SimulatedBodyHandle bodyHandle, const AzPhysics::TriggerEvent& event)
            {
                if (event.m_otherBody)
                {
                    NotifyBodyEntered(event.m_otherBody->GetEntityId());
                }
            });
        m_triggerExitHandler = AzPhysics::SimulatedBodyEvents::OnTriggerExit::Handler(
            [this]([[maybe_unused]] AzPhysics::SimulatedBodyHandle bodyHandle, const AzPhysics::TriggerEvent& event)
            {
                if (event.m_otherBody)
                {
                    NotifyBodyExited(event.m_otherBody->GetEntityId());
                }
            });
        AzPhysics::SimulatedBodyHandle bodyHandle = AzPhysics::InvalidSimulatedBodyHandle;
        AzPhysics::SimulatedBodyComponentRequestsBus::EventResult(
            bodyHandle, GetEntityId(), &AzPhysics::SimulatedBodyComponentRequests::GetSimulatedBodyHandle);
        auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
        if (bodyHandle == AzPhysics::InvalidSimulatedBodyHandle || !sceneInterface)
        {
            AZ_Error("DarkArisen", false, "Story location %s needs a PhysX trigger collider.", GetEntityId().ToString().c_str());
            return;
        }
        const AzPhysics::SceneHandle scene = sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
        AzPhysics::SimulatedBodyEvents::RegisterOnTriggerEnterHandler(scene, bodyHandle, m_triggerEnterHandler);
        AzPhysics::SimulatedBodyEvents::RegisterOnTriggerExitHandler(scene, bodyHandle, m_triggerExitHandler);
    }

    void StoryActorComponent::Deactivate()
    {
        m_triggerEnterHandler.Disconnect();
        m_triggerExitHandler.Disconnect();
        AZ::TickBus::Handler::BusDisconnect();
        StoryTriggerRequestBus::Handler::BusDisconnect();
    }

    bool StoryActorComponent::IsJake(const AZ::EntityId other) const
    {
        bool isJake = false;
        CombatRequestBus::Event(other, [&isJake](CombatRequests* handler)
        {
            isJake = handler->GetCombatant().EntityId == Core::EntityPolicy::JakeId;
        });
        return isJake;
    }

    bool StoryActorComponent::TryLocation()
    {
        auto* campaign = CampaignInterface::Get();
        if (!campaign)
        {
            return false;
        }
        int livingDuelists = 0;
        EnemyPopulationRequestBus::Broadcast(&EnemyPopulationRequests::CountLivingRankAndFile, livingDuelists);
        const bool changed = Core::StoryActions::OnLocationEntered(campaign->GetCampaign(), m_spec, livingDuelists > 0);
        // Standing in the exit while the fight goes on: complete as soon as the last duelist falls.
        m_jakeWaiting = !changed && livingDuelists > 0 && m_spec.Action == Core::LocationAction::CompleteMission &&
            campaign->GetCampaign().GetMissionState(m_spec.MissionId) == Core::MissionState::Active;
        return changed;
    }

    bool StoryActorComponent::NotifyBodyEntered(const AZ::EntityId other)
    {
        if (m_spec.Kind != Core::StoryActorKind::LocationTrigger || !IsJake(other))
        {
            return false;
        }
        return TryLocation();
    }

    void StoryActorComponent::NotifyBodyExited(const AZ::EntityId other)
    {
        if (IsJake(other))
        {
            m_jakeWaiting = false;
        }
    }

    bool StoryActorComponent::Interact(const AZStd::string& actorId)
    {
        auto* campaign = CampaignInterface::Get();
        if (actorId != Core::EntityPolicy::JakeId.data() || !campaign || m_spec.Kind == Core::StoryActorKind::LocationTrigger)
        {
            return false;
        }
        const Core::StoryInteraction result = Core::StoryActions::Interact(campaign->GetCampaign(), m_spec);
        if (!result.Accepted)
        {
            AZ_Warning("DarkArisen", false, "Story actor %s refused interaction in the current campaign state.",
                GetEntityId().ToString().c_str());
            return false;
        }
        if (!result.Line.empty())
        {
            PresentationNotificationBus::Broadcast(
                &PresentationNotifications::OnSubtitle, AZStd::string(result.Speaker.c_str()), AZStd::string(result.Line.c_str()));
        }
        if (!result.DocumentTitle.empty())
        {
            PresentationNotificationBus::Broadcast(&PresentationNotifications::OnDocumentOpened,
                AZStd::string(result.DocumentTitle.c_str()), AZStd::string(result.DocumentBody.c_str()));
        }
        return true;
    }

    void StoryActorComponent::OnTick(const float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        m_gateTimer += deltaTime;
        if (m_gateResolved || m_gateTimer < NavalGateIntervalSeconds)
        {
            return;
        }
        m_gateTimer = 0.0f;
        if (m_spec.Kind == Core::StoryActorKind::LocationTrigger)
        {
            if (m_jakeWaiting)
            {
                TryLocation();
            }
            return;
        }
        UpdateNavalGate();
    }

    void StoryActorComponent::UpdateNavalGate()
    {
        auto* campaign = CampaignInterface::Get();
        if (!campaign)
        {
            return;
        }
        AZ::Vector3 gate = AZ::Vector3::CreateZero();
        AZ::TransformBus::EventResult(gate, GetEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
        int hostiles = 0;
        int sunk = 0;
        double playerShipDistance = std::numeric_limits<double>::infinity();
        NavalCombatRequestBus::EnumerateHandlers([&](NavalCombatRequests* ship)
        {
            if (ship->IsPlayerAligned())
            {
                AZ::Vector3 where = AZ::Vector3::CreateZero();
                AZ::TransformBus::EventResult(where, ship->GetShipEntityId(), &AZ::TransformBus::Events::GetWorldTranslation);
                const AZ::Vector3 flat(where.GetX() - gate.GetX(), where.GetY() - gate.GetY(), 0.0f);
                playerShipDistance = AZStd::min(playerShipDistance, static_cast<double>(flat.GetLength()));
            }
            else
            {
                ++hostiles;
                sunk += ship->IsSunk() ? 1 : 0;
            }
            return true;
        });
        if (Core::StoryActions::UpdateNavalGate(campaign->GetCampaign(), m_spec, hostiles, sunk, playerShipDistance))
        {
            m_gateResolved = true;
            AZ::TickBus::Handler::BusDisconnect();
        }
    }
}
