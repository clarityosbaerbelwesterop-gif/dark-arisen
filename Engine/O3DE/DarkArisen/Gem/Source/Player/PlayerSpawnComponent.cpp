#include "PlayerSpawnComponent.h"

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>

#include <DarkArisen/Core/CampaignRuntime.h>
#include <DarkArisen/Core/LevelSpawn.h>

namespace DarkArisen
{
    AZ_COMPONENT_IMPL(PlayerSpawnComponent, "PlayerSpawnComponent", PlayerSpawnComponentTypeId);

    void SpawnPointConfig::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<SpawnPointConfig>()
                ->Version(1)
                ->Field("SpawnId", &SpawnPointConfig::m_spawnId)
                ->Field("Anchor", &SpawnPointConfig::m_anchor)
                ->Field("Rule", &SpawnPointConfig::m_rule);
        }
    }

    void PlayerSpawnComponent::Reflect(AZ::ReflectContext* context)
    {
        SpawnPointConfig::Reflect(context);
        auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }
        serializeContext->Class<PlayerSpawnComponent, AZ::Component>()
            ->Version(1)
            ->Field("Player", &PlayerSpawnComponent::m_player)
            ->Field("Spawns", &PlayerSpawnComponent::m_spawns);
        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
        {
            editContext->Class<PlayerSpawnComponent>("Dark Arisen Player Spawn", "Checkpoint, overboard or arrival spawn.")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "Dark Arisen")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Level"))
                ->DataElement(AZ::Edit::UIHandlers::Default, &PlayerSpawnComponent::m_player, "Player", "")
                ->DataElement(AZ::Edit::UIHandlers::Default, &PlayerSpawnComponent::m_spawns, "Spawns", "");
        }
    }

    void PlayerSpawnComponent::Activate()
    {
        CampaignNotificationBus::Handler::BusConnect();
    }

    void PlayerSpawnComponent::Deactivate()
    {
        CampaignNotificationBus::Handler::BusDisconnect();
    }

    void PlayerSpawnComponent::OnRestoreWorldState(const AZStd::string& levelName)
    {
        PlacePlayer(levelName);
    }

    AZStd::string PlayerSpawnComponent::PlacePlayer(const AZStd::string& levelName)
    {
        CampaignRequests* campaign = CampaignInterface::Get();
        if (!campaign)
        {
            return {};
        }
        const Core::CampaignState& state = campaign->GetCampaign().State();
        const Core::PlayerRuntimeSnapshot& saved = state.PlayerRuntime;
        if (saved.Valid && !saved.SourceLevel.empty() && saved.SourceLevel == levelName.c_str())
        {
            return {};  // the combatant restores the saved map-local transform
        }
        std::vector<Core::SpawnPoint> points;
        for (const SpawnPointConfig& spawn : m_spawns)
        {
            const bool known = spawn.m_rule >= 0 && spawn.m_rule <= static_cast<int>(Core::SpawnRule::Checkpoint);
            points.push_back({known && spawn.m_anchor.IsValid() ? spawn.m_spawnId.c_str() : "",
                known ? static_cast<Core::SpawnRule>(spawn.m_rule) : Core::SpawnRule::Arrival});
        }
        const int chosen = Core::ResolveSpawn(state, points);
        if (chosen < 0 || !m_player.IsValid())
        {
            AZ_Error("DarkArisen", false, "Level %s has no legal player spawn for the campaign state.", levelName.c_str());
            return {};
        }
        const SpawnPointConfig& spawn = m_spawns[static_cast<size_t>(chosen)];
        AZ::Transform anchor = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(anchor, spawn.m_anchor, &AZ::TransformBus::Events::GetWorldTM);
        AZ::TransformBus::Event(m_player, &AZ::TransformBus::Events::SetWorldTM, anchor);
        return spawn.m_spawnId;
    }
}
