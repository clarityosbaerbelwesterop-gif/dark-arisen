#pragma once

#include "StoryTriggerComponent.h"

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/RTTI/TypeInfo.h>
#include <AzCore/std/string/string.h>
#include <AzFramework/Physics/Common/PhysicsSimulatedBodyEvents.h>

#include <DarkArisen/Core/StoryWorld.h>

namespace DarkArisen
{
    /**
     * One physical story actor from a ContentSource/Story mission contract (chapters 3-10):
     * location triggers, contacts, evidence, route resolutions, war actions and naval gates.
     * A thin adapter: every rule lives in Core::StoryActions. Interaction uses the same 1.4 m
     * interact query as the opening beats (StoryTriggerRequestBus); no markers, no highlight.
     */
    class StoryActorComponent
        : public AZ::Component
        , protected StoryTriggerRequestBus::Handler
        , protected AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(StoryActorComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        bool Interact(const AZStd::string& actorId) override;
        bool NotifyBodyEntered(AZ::EntityId other) override;
        /** A body left the trigger volume (PhysX trigger exit). */
        void NotifyBodyExited(AZ::EntityId other);

        /** Naval gates poll four times a second (UE ANavalMissionGateActor tick interval). */
        static constexpr float NavalGateIntervalSeconds = 0.25f;

    protected:
        void Activate() override;
        void Deactivate() override;
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

    private:
        Core::StoryActorKind m_kind = Core::StoryActorKind::LocationTrigger;
        AZStd::string m_missionId;
        AZStd::string m_checkpointId;
        AZStd::string m_spawnId;
        Core::LocationAction m_action = Core::LocationAction::ActivateMission;
        AZStd::string m_contactId;
        AZStd::string m_dialogueId;
        AZStd::string m_displayName;
        AZStd::string m_text;
        AZStd::string m_title;
        AZStd::string m_body;
        AZStd::string m_worldFactOnRead;
        AZStd::string m_requiredBossId;
        bool m_completesMission = false;
        AZStd::string m_outcomeKey;
        AZStd::string m_outcomeValue;
        AZStd::string m_label;
        AZStd::string m_regionId;
        Core::ColonialFaction m_initialController = Core::ColonialFaction::Imperial;
        Core::WarActionVerb m_verb = Core::WarActionVerb::Support;
        Core::ColonialFaction m_targetFaction = Core::ColonialFaction::Imperial;
        int m_controlDelta = 0;
        int m_liberationDelta = 0;
        int m_crimsonDelta = 0;
        AZStd::string m_holdingId;
        int m_requiredDefeats = 1;
        AZStd::string m_requiredWorldFact;
        float m_completionRadiusMetres = 0.0f;

        Core::StoryActorSpec m_spec;
        AzPhysics::SimulatedBodyEvents::OnTriggerEnter::Handler m_triggerEnterHandler;
        AzPhysics::SimulatedBodyEvents::OnTriggerExit::Handler m_triggerExitHandler;
        float m_gateTimer = 0.0f;
        bool m_gateResolved = false;
        /** Jake is inside a completion volume that the fight has not released yet. */
        bool m_jakeWaiting = false;

        Core::StoryActorSpec BuildSpec() const;
        void UpdateNavalGate();
        bool IsJake(AZ::EntityId other) const;
        bool TryLocation();
    };
}

namespace AZ
{
    AZ_TYPE_INFO_SPECIALIZE(DarkArisen::Core::StoryActorKind, "{2462AADA-7FC0-4E53-B35F-F60BD3171A3B}");
    AZ_TYPE_INFO_SPECIALIZE(DarkArisen::Core::LocationAction, "{FCC4F7FA-2FF6-4023-A208-8A6BBA1EA0D6}");
    AZ_TYPE_INFO_SPECIALIZE(DarkArisen::Core::ColonialFaction, "{8EB434F4-BE46-4D88-AD1B-B0431797A5DB}");
    AZ_TYPE_INFO_SPECIALIZE(DarkArisen::Core::WarActionVerb, "{4654CF50-77C0-4A69-9793-6030AB638754}");
}
