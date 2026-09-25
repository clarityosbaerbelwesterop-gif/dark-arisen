#include "OpeningDirectorComponent.h"

#include <DarkArisen/PresentationBus.h>

#include <AzCore/Component/ComponentApplicationBus.h>
#include <AzCore/Component/Entity.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Entity/GameEntityContextBus.h>

#include <DarkArisen/Core/OpeningRuntime.h>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(OpeningDirectorComponent, "OpeningDirectorComponent", OpeningDirectorComponentTypeId);

    namespace
    {
        AZ::Entity* FindEntity(const AZ::EntityId id)
        {
            AZ::Entity* entity = nullptr;
            AZ::ComponentApplicationBus::BroadcastResult(entity, &AZ::ComponentApplicationRequests::FindEntity, id);
            return entity;
        }
    }

    void PresentationBeatConfig::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<PresentationBeatConfig>()
                ->Version(1)
                ->Field("Time", &PresentationBeatConfig::m_time)
                ->Field("Kind", &PresentationBeatConfig::m_kind)
                ->Field("Speaker", &PresentationBeatConfig::m_speaker)
                ->Field("Text", &PresentationBeatConfig::m_text);
        }
    }

    void PresentationSequenceConfig::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<PresentationSequenceConfig>()
                ->Version(1)
                ->Field("Id", &PresentationSequenceConfig::m_id)
                ->Field("Duration", &PresentationSequenceConfig::m_duration)
                ->Field("PlayerControl", &PresentationSequenceConfig::m_playerControl)
                ->Field("Camera", &PresentationSequenceConfig::m_camera)
                ->Field("Beats", &PresentationSequenceConfig::m_beats);
        }
    }

    void FamilyMemberConfig::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<FamilyMemberConfig>()
                ->Version(1)
                ->Field("CharacterId", &FamilyMemberConfig::m_characterId)
                ->Field("Entity", &FamilyMemberConfig::m_entity);
        }
    }

    void OpeningDirectorComponent::Reflect(AZ::ReflectContext* context)
    {
        PresentationBeatConfig::Reflect(context);
        PresentationSequenceConfig::Reflect(context);
        FamilyMemberConfig::Reflect(context);
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }
        serializeContext->Class<OpeningDirectorComponent, AZ::Component>()
            ->Version(1)
            ->Field("Sequences", &OpeningDirectorComponent::m_sequences)
            ->Field("EncounterId", &OpeningDirectorComponent::m_encounterId)
            ->Field("RequiredBoarders", &OpeningDirectorComponent::m_requiredBoarders)
            ->Field("Boarders", &OpeningDirectorComponent::m_boarders)
            ->Field("Family", &OpeningDirectorComponent::m_family)
            ->Field("Player", &OpeningDirectorComponent::m_player)
            ->Field("OverboardAnchor", &OpeningDirectorComponent::m_overboardAnchor);
        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
        {
            editContext->Class<OpeningDirectorComponent>("Dark Arisen Opening Director", "Harlow raid presentation and staging.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Level"))
                ->DataElement(AZ::Edit::UIHandlers::Default, &OpeningDirectorComponent::m_encounterId, "Encounter", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &OpeningDirectorComponent::m_requiredBoarders, "Required Boarders", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &OpeningDirectorComponent::m_boarders, "Boarders", "Start Active off")
                ->DataElement(AZ::Edit::UIHandlers::Default, &OpeningDirectorComponent::m_family, "Family", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &OpeningDirectorComponent::m_player, "Player", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &OpeningDirectorComponent::m_overboardAnchor, "Overboard Anchor", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &OpeningDirectorComponent::m_sequences, "Sequences", "Materialised");
        }
    }

    void OpeningDirectorComponent::Activate()
    {
        m_director.reset();
        m_boundOpening = nullptr;
        m_reportedErrors = 0;
        CampaignNotificationBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
    }

    void OpeningDirectorComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        CampaignNotificationBus::Handler::BusDisconnect();
        m_director.reset();
        m_boundOpening = nullptr;
    }

    bool OpeningDirectorComponent::EnsureDirector()
    {
        CampaignRequests* campaign = CampaignInterface::Get();
        if (!campaign)
        {
            return false;
        }
        Core::OpeningRuntime& opening = campaign->GetOpening();
        if (m_director && m_boundOpening == &opening)
        {
            return true;
        }
        // First tick, New Game or Load: the campaign may have replaced its runtime.
        std::vector<Core::PresentationSequence> sequences;
        for (const PresentationSequenceConfig& config : m_sequences)
        {
            Core::PresentationSequence sequence;
            sequence.Id = config.m_id.c_str();
            sequence.Duration = static_cast<double>(config.m_duration);
            sequence.PlayerControl = config.m_playerControl.c_str();
            sequence.Camera = config.m_camera.c_str();
            for (const PresentationBeatConfig& beat : config.m_beats)
            {
                Core::PresentationBeat coreBeat;
                coreBeat.Time = static_cast<double>(beat.m_time);
                coreBeat.Kind = beat.m_kind >= 0 && beat.m_kind <= static_cast<int>(Core::BeatKind::Actors)
                    ? static_cast<Core::BeatKind>(beat.m_kind)
                    : Core::BeatKind::Shot;
                coreBeat.Speaker = beat.m_speaker.c_str();
                coreBeat.Text = beat.m_text.c_str();
                sequence.Beats.push_back(AZStd::move(coreBeat));
            }
            sequences.push_back(AZStd::move(sequence));
        }
        Core::OpeningDirector::Events events;
        events.ReleaseBoarders = [this](const int remaining) { ReleaseBoarders(remaining); };
        events.FamilyFate = [this](const std::string_view id, const Core::OpeningDirector::Fate fate)
        {
            ApplyFate(AZStd::string_view(id.data(), id.size()), fate);
        };
        events.JakeForcedOverboard = [this]() { PutJakeOverboard(); };
        events.Subtitle = [](const Core::PresentationBeat& beat)
        {
            PresentationNotificationBus::Broadcast(
                &PresentationNotifications::OnSubtitle, AZStd::string(beat.Speaker.c_str()), AZStd::string(beat.Text.c_str()));
        };
        Core::OpeningDirector::Encounter encounter;
        encounter.Id = m_encounterId.c_str();
        encounter.RequiredBoarders = m_requiredBoarders;
        m_director.emplace(opening, AZStd::move(sequences), AZStd::move(encounter), AZStd::move(events));
        m_boundOpening = &opening;
        m_reportedErrors = 0;
        m_director->Resume();
        ReportErrors();
        return true;
    }

    void OpeningDirectorComponent::ReportErrors()
    {
        const std::vector<std::string>& errors = m_director->Errors();
        for (; m_reportedErrors < errors.size(); ++m_reportedErrors)
        {
            AZ_Warning("DarkArisen", false, "Opening director: %s", errors[m_reportedErrors].c_str());
        }
    }

    void OpeningDirectorComponent::OnCinematicRequested(const AZStd::string& cinematicId)
    {
        if (!EnsureDirector())
        {
            return;
        }
        if (m_director->OnCinematicRequested(std::string_view(cinematicId.data(), cinematicId.size())))
        {
            PresentationNotificationBus::Broadcast(&PresentationNotifications::OnPresentationStarted, cinematicId);
        }
        ReportErrors();
    }

    void OpeningDirectorComponent::OnTick(const float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        if (!EnsureDirector())
        {
            return;
        }
        m_director->Tick(static_cast<double>(deltaTime));
        ReportErrors();
    }

    void OpeningDirectorComponent::ReleaseBoarders(const int remaining)
    {
        int released = 0;
        for (const AZ::EntityId& boarder : m_boarders)
        {
            if (released >= remaining)
            {
                break;
            }
            AZ::Entity* entity = FindEntity(boarder);
            if (!entity)
            {
                AZ_Error("DarkArisen", false, "Opening director: boarder %s is not in the level.", boarder.ToString().c_str());
                continue;
            }
            if (entity->GetState() != AZ::Entity::State::Active)
            {
                AzFramework::GameEntityContextRequestBus::Broadcast(&AzFramework::GameEntityContextRequests::ActivateGameEntity, boarder);
            }
            ++released;
        }
        AZ_Error("DarkArisen", released == remaining, "Opening director: %d boarders required, %d placed.", remaining, released);
    }

    void OpeningDirectorComponent::ApplyFate(const AZStd::string_view characterId, const Core::OpeningDirector::Fate fate)
    {
        for (const FamilyMemberConfig& member : m_family)
        {
            if (member.m_characterId != characterId)
            {
                continue;
            }
            // Killed or seized alive, the family member leaves the deck; how is the presentation's job.
            AZ_TracePrintf("DarkArisen", "%s %s.\n", member.m_characterId.c_str(),
                fate == Core::OpeningDirector::Fate::SeizedAlive ? "is seized alive" : "falls");
            AzFramework::GameEntityContextRequestBus::Broadcast(&AzFramework::GameEntityContextRequests::DeactivateGameEntity, member.m_entity);
            return;
        }
        AZ_Error("DarkArisen", false, "Opening director: no family entity for %.*s.", AZ_STRING_ARG(characterId));
    }

    void OpeningDirectorComponent::PutJakeOverboard()
    {
        AZ::Vector3 anchor = AZ::Vector3::CreateZero();
        if (!m_overboardAnchor.IsValid() || !m_player.IsValid())
        {
            AZ_Error("DarkArisen", false, "Opening director: player or overboard anchor missing.");
            return;
        }
        AZ::TransformBus::EventResult(anchor, m_overboardAnchor, &AZ::TransformBus::Events::GetWorldTranslation);
        AZ::TransformBus::Event(m_player, &AZ::TransformBus::Events::SetWorldTranslation, anchor);
    }
}
