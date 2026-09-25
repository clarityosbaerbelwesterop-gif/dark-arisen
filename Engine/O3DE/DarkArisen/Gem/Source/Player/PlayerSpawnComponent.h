#pragma once

#include <DarkArisen/CampaignBus.h>
#include <DarkArisen/DarkArisenTypeIds.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Component/EntityId.h>
#include <AzCore/RTTI/TypeInfo.h>
#include <AzCore/std/containers/vector.h>
#include <AzCore/std/string/string.h>

namespace DarkArisen
{
    struct SpawnPointConfig
    {
        AZ_TYPE_INFO(SpawnPointConfig, SpawnPointConfigTypeId);
        static void Reflect(AZ::ReflectContext* context);

        AZStd::string m_spawnId;
        AZ::EntityId m_anchor;
        int m_rule = 0;  // Core::SpawnRule
    };

    /**
     * Places the player when a level finishes loading and no map-local transform of this level was
     * saved (Core::ResolveSpawn), and brings Jake back after death: after the death beat he returns
     * to the checkpoint (or the level's legal spawn) with full vitals and breath, no failure screen.
     * A level without a legal spawn logs an error and leaves the player where the level put them.
     */
    class PlayerSpawnComponent
        : public AZ::Component
        , protected CampaignNotificationBus::Handler
        , protected AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(PlayerSpawnComponent);

        static void Reflect(AZ::ReflectContext* context);

        /**
         * Resolves and applies the spawn; returns the chosen spawn id, empty on failure. A saved
         * map-local transform of this level wins unless the player is respawning.
         */
        AZStd::string PlacePlayer(const AZStd::string& levelName, bool respawning = false);
        int GetRespawnCount() const { return m_respawns; }

    protected:
        void Activate() override;
        void Deactivate() override;
        void OnRestoreWorldState(const AZStd::string& levelName) override;
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

    private:
        AZ::EntityId m_player;
        AZStd::vector<SpawnPointConfig> m_spawns;

        float m_deathTimer = -1.0f;
        int m_respawns = 0;
    };
}
