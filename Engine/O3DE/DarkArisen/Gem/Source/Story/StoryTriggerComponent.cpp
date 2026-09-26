#include "StoryTriggerComponent.h"

#include <DarkArisen/CampaignBus.h>
#include <DarkArisen/CombatBus.h>

#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Physics/Collision/CollisionEvents.h>
#include <AzFramework/Physics/Common/PhysicsSimulatedBody.h>
#include <AzFramework/Physics/Components/SimulatedBodyComponentBus.h>
#include <AzFramework/Physics/PhysicsScene.h>

#include <DarkArisen/Core/Combat.h>
#include <DarkArisen/Core/EntityPolicy.h>
#include <DarkArisen/Core/OpeningRuntime.h>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(StoryTriggerComponent, "StoryTriggerComponent", StoryTriggerComponentTypeId);

    void StoryTriggerComponent::Reflect(AZ::ReflectContext* context)
    {
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }
        serializeContext->Class<StoryTriggerComponent, AZ::Component>()
            ->Version(1)
            ->Field("Signal", &StoryTriggerComponent::m_signal)
            ->Field("Argument", &StoryTriggerComponent::m_argument)
            ->Field("IntegerArgument", &StoryTriggerComponent::m_integerArgument)
            ->Field("RequiresInteract", &StoryTriggerComponent::m_requiresInteract)
            ->Field("OneShot", &StoryTriggerComponent::m_oneShot);

        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
        {
            editContext->Enum<StorySignal>("Story Signal", "Opening beat reported to the campaign.")
                ->Value("Family interaction (Argument = character id)", StorySignal::FamilyInteraction)
                ->Value("Fleet detected", StorySignal::FleetDetected)
                ->Value("Boarding started", StorySignal::BoardingStarted)
                ->Value("Begin boarding encounter (Argument = id, Integer = boarders)", StorySignal::BeginBoardingEncounter)
                ->Value("Draven boarded (Argument = encounter id)", StorySignal::DravenBoarded)
                ->Value("Taking started", StorySignal::TakingStarted)
                ->Value("Taking cinematic complete", StorySignal::TakingCinematicComplete)
                ->Value("Entered water", StorySignal::EnteredWater)
                ->Value("Reached Outer Reef", StorySignal::ReachedOuterReef)
                ->Value("Reached location (Integer = OpeningLocation)", StorySignal::ReachedLocation)
                ->Value("Beach recovery complete", StorySignal::RecoveryComplete)
                ->Value("Crew met (Argument = crew id)", StorySignal::CrewMet)
                ->Value("Crew recruitment available (Argument = crew id)", StorySignal::CrewRecruitmentAvailable)
                ->Value("Crew recruited (Argument = crew id)", StorySignal::CrewRecruited)
                ->Value("Galleon Cove cleared", StorySignal::GalleonCoveCleared)
                ->Value("La Liberacion boarded", StorySignal::LaLiberacionBoarded)
                ->Value("La Liberacion helm secured", StorySignal::LaLiberacionHelmSecured)
                ->Value("La Liberacion cleared harbor", StorySignal::LaLiberacionHarborCleared)
                ->Value("Begin First Wake", StorySignal::BeginFirstWake)
                ->Value("Reach Rexa Harbor", StorySignal::ReachRexaHarbor)
                ->Value("Rest (safe rest and autosave)", StorySignal::Rest);

            editContext->Class<StoryTriggerComponent>("Dark Arisen Story Trigger", "Physical story beat. No markers, no highlight.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                ->DataElement(AZ::Edit::UIHandlers::ComboBox, &StoryTriggerComponent::m_signal, "Signal", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &StoryTriggerComponent::m_argument, "Argument", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &StoryTriggerComponent::m_integerArgument, "Integer Argument", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &StoryTriggerComponent::m_requiresInteract, "Requires Interact",
                    "Off: fires when Jake enters this entity's PhysX trigger. On: fires on Jake's interact input.")
                ->DataElement(AZ::Edit::UIHandlers::Default, &StoryTriggerComponent::m_oneShot, "One Shot", "");
        }
    }

    void StoryTriggerComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        // Activate after the PhysX body so its trigger handle exists.
        dependent.push_back(AZ_CRC_CE("PhysicsWorldBodyService"));
    }

    void StoryTriggerComponent::Activate()
    {
        m_fired = false;
        StoryTriggerRequestBus::Handler::BusConnect(GetEntityId());
        if (m_requiresInteract)
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

        AzPhysics::SimulatedBodyHandle bodyHandle = AzPhysics::InvalidSimulatedBodyHandle;
        AzPhysics::SimulatedBodyComponentRequestsBus::EventResult(
            bodyHandle, GetEntityId(), &AzPhysics::SimulatedBodyComponentRequests::GetSimulatedBodyHandle);
        auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
        if (bodyHandle == AzPhysics::InvalidSimulatedBodyHandle || !sceneInterface)
        {
            AZ_Error("DarkArisen", false, "Story trigger on %s needs a PhysX trigger collider.", GetEntityId().ToString().c_str());
            return;
        }
        AzPhysics::SimulatedBodyEvents::RegisterOnTriggerEnterHandler(
            sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName), bodyHandle, m_triggerEnterHandler);
    }

    void StoryTriggerComponent::Deactivate()
    {
        m_triggerEnterHandler.Disconnect();
        StoryTriggerRequestBus::Handler::BusDisconnect();
    }

    bool StoryTriggerComponent::Interact(const AZStd::string& actorId)
    {
        return m_requiresInteract && actorId == Core::EntityPolicy::JakeId.data() && Fire();
    }

    bool StoryTriggerComponent::NotifyBodyEntered(const AZ::EntityId other)
    {
        if (m_requiresInteract)
        {
            return false;
        }
        bool isJake = false;
        CombatRequestBus::Event(other, [&isJake](CombatRequests* handler)
        {
            isJake = handler->GetCombatant().EntityId == Core::EntityPolicy::JakeId;
        });
        return isJake && Fire();
    }

    bool StoryTriggerComponent::Fire()
    {
        if (m_oneShot && m_fired)
        {
            return false;
        }
        auto* campaign = CampaignInterface::Get();
        if (!campaign)
        {
            AZ_Error("DarkArisen", false, "Story trigger fired without the campaign system component.");
            return false;
        }
        Core::OpeningRuntime& opening = campaign->GetOpening();
        const std::string argument(m_argument.c_str());
        bool accepted = false;
        switch (m_signal)
        {
        case StorySignal::FamilyInteraction: accepted = opening.SignalFamilyInteraction(argument); break;
        case StorySignal::FleetDetected: accepted = opening.SignalFleetDetected(); break;
        case StorySignal::BoardingStarted: accepted = opening.SignalBoardingStarted(); break;
        case StorySignal::BeginBoardingEncounter: accepted = opening.BeginBoardingEncounter(argument, m_integerArgument); break;
        case StorySignal::DravenBoarded: accepted = opening.SignalDravenBoarded(argument); break;
        case StorySignal::TakingStarted: accepted = opening.SignalTakingStarted(); break;
        case StorySignal::TakingCinematicComplete: accepted = opening.SignalTakingCinematicComplete(); break;
        case StorySignal::EnteredWater: accepted = opening.SignalEnteredWater(); break;
        case StorySignal::ReachedOuterReef: accepted = opening.SignalReachedOuterReef(); break;
        case StorySignal::ReachedLocation:
            accepted = m_integerArgument >= 0 && m_integerArgument <= static_cast<int>(Core::OpeningLocation::RexaHarbor) &&
                opening.SignalReachedLocation(static_cast<Core::OpeningLocation>(m_integerArgument));
            break;
        case StorySignal::RecoveryComplete: accepted = opening.SignalRecoveryComplete(); break;
        case StorySignal::CrewMet: accepted = opening.SignalCrewMet(argument); break;
        case StorySignal::CrewRecruitmentAvailable: accepted = opening.SignalCrewRecruitmentAvailable(argument); break;
        case StorySignal::CrewRecruited: accepted = opening.SignalCrewRecruited(argument); break;
        case StorySignal::GalleonCoveCleared: accepted = opening.SignalGalleonCoveCleared(); break;
        case StorySignal::LaLiberacionBoarded: accepted = opening.SignalLaLiberacionBoarded(); break;
        case StorySignal::LaLiberacionHelmSecured: accepted = opening.SignalLaLiberacionHelmSecured(); break;
        case StorySignal::LaLiberacionHarborCleared: accepted = opening.SignalLaLiberacionHarborCleared(); break;
        case StorySignal::BeginFirstWake: accepted = opening.BeginFirstWake(); break;
        case StorySignal::ReachRexaHarbor: accepted = opening.ReachRexaHarbor(); break;
        case StorySignal::Rest: accepted = campaign->CompleteRest(); break;
        }
        if (!accepted)
        {
            // Out-of-order contact: logged, never advances the story. Re-entry may succeed later.
            AZ_Warning("DarkArisen", false, "Story signal %d on %s rejected in current campaign state.",
                static_cast<int>(m_signal), GetEntityId().ToString().c_str());
            return false;
        }
        m_fired = true;
        return true;
    }
}
