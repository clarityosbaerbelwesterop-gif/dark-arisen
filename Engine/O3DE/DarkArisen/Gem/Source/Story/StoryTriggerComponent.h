#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/ComponentBus.h>
#include <AzCore/RTTI/TypeInfo.h>
#include <AzCore/std/string/string.h>
#include <AzFramework/Physics/Common/PhysicsSimulatedBodyEvents.h>

namespace DarkArisen
{
    /** Authored physical beats of the Chapter 1/2 opening (OpeningRuntime signals). */
    enum class StorySignal : int
    {
        FamilyInteraction,
        FleetDetected,
        BoardingStarted,
        BeginBoardingEncounter,
        DravenBoarded,
        TakingStarted,
        TakingCinematicComplete,
        EnteredWater,
        ReachedOuterReef,
        ReachedLocation,
        RecoveryComplete,
        CrewMet,
        CrewRecruitmentAvailable,
        CrewRecruited,
        GalleonCoveCleared,
        LaLiberacionBoarded,
        LaLiberacionHelmSecured,
        LaLiberacionHarborCleared,
        BeginFirstWake,
        ReachRexaHarbor
    };

    class StoryTriggerRequests : public AZ::ComponentBus
    {
    public:
        virtual ~StoryTriggerRequests() = default;
        /** Called by the player's interact input within the 1.4 m prompt range. */
        virtual bool Interact(const AZStd::string& actorId) = 0;
    };
    using StoryTriggerRequestBus = AZ::EBus<StoryTriggerRequests>;

    /**
     * Level-placed story beat. Either a PhysX trigger volume Jake walks into, or an
     * interactable object/person. No markers, no highlight: the level geometry carries it.
     * Rejected signals are logged loudly and never advance the story.
     */
    class StoryTriggerComponent
        : public AZ::Component
        , protected StoryTriggerRequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(StoryTriggerComponent);

        static void Reflect(AZ::ReflectContext* context);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        void Activate() override;
        void Deactivate() override;
        bool Interact(const AZStd::string& actorId) override;

    private:
        StorySignal m_signal = StorySignal::FamilyInteraction;
        AZStd::string m_argument;
        int m_integerArgument = 0;
        bool m_requiresInteract = false;
        bool m_oneShot = true;
        bool m_fired = false;

        AzPhysics::SimulatedBodyEvents::OnTriggerEnter::Handler m_triggerEnterHandler;

        bool Fire();
    };
}

namespace AZ
{
    AZ_TYPE_INFO_SPECIALIZE(DarkArisen::StorySignal, "{AD57BF9A-EC80-4E3B-8B74-3DA85B91D2BF}");
}
