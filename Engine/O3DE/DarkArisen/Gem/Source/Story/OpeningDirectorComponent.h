#pragma once

#include <DarkArisen/CampaignBus.h>
#include <DarkArisen/DarkArisenTypeIds.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/EntityId.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/RTTI/TypeInfo.h>
#include <AzCore/std/containers/vector.h>
#include <AzCore/std/optional.h>
#include <AzCore/std/string/string.h>

#include <DarkArisen/Core/Presentation.h>

namespace DarkArisen
{
    /** Serialized form of Core::PresentationBeat (written by the ContentSource materialiser). */
    struct PresentationBeatConfig
    {
        AZ_TYPE_INFO(PresentationBeatConfig, PresentationBeatConfigTypeId);
        static void Reflect(AZ::ReflectContext* context);

        float m_time = 0.0f;
        int m_kind = 0;  // Core::BeatKind
        AZStd::string m_speaker;
        AZStd::string m_text;
    };

    struct PresentationSequenceConfig
    {
        AZ_TYPE_INFO(PresentationSequenceConfig, PresentationSequenceConfigTypeId);
        static void Reflect(AZ::ReflectContext* context);

        AZStd::string m_id;
        float m_duration = 0.0f;
        AZStd::string m_playerControl;
        AZStd::string m_camera;
        AZStd::vector<PresentationBeatConfig> m_beats;
    };

    struct FamilyMemberConfig
    {
        AZ_TYPE_INFO(FamilyMemberConfig, FamilyMemberConfigTypeId);
        static void Reflect(AZ::ReflectContext* context);

        AZStd::string m_characterId;
        AZ::EntityId m_entity;
    };

    /**
     * Harlow raid director (level entity): plays the presentation the opening runtime requests,
     * activates the boarders (placed with Start Active off), removes Marc and Denise when they
     * fall and Ethan when he is seized alive, and puts Jake over the rail. Story rules stay in
     * Core::OpeningDirector and Core::OpeningRuntime.
     */
    class OpeningDirectorComponent
        : public AZ::Component
        , protected AZ::TickBus::Handler
        , protected CampaignNotificationBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(OpeningDirectorComponent);

        static void Reflect(AZ::ReflectContext* context);

        bool IsPresenting() const { return m_director.has_value() && m_director->IsPresenting(); }

    protected:
        void Activate() override;
        void Deactivate() override;
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        void OnCinematicRequested(const AZStd::string& cinematicId) override;

    private:
        AZStd::vector<PresentationSequenceConfig> m_sequences;
        AZStd::string m_encounterId = "Encounter.HarlowRaid.MainDeck";
        int m_requiredBoarders = 3;
        AZStd::vector<AZ::EntityId> m_boarders;
        AZStd::vector<FamilyMemberConfig> m_family;
        AZ::EntityId m_player;
        AZ::EntityId m_overboardAnchor;

        AZStd::optional<Core::OpeningDirector> m_director;
        const Core::OpeningRuntime* m_boundOpening = nullptr;
        size_t m_reportedErrors = 0;

        bool EnsureDirector();
        void ReportErrors();
        void ReleaseBoarders(int remaining);
        void ApplyFate(AZStd::string_view characterId, Core::OpeningDirector::Fate fate);
        void PutJakeOverboard();
    };
}
