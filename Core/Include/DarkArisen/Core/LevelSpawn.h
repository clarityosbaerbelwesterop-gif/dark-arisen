#pragma once

#include "DarkArisen/Core/CampaignState.h"

#include <cstdint>
#include <string>
#include <vector>

namespace DarkArisen::Core
{
    /** How a level spawn point qualifies. */
    enum class SpawnRule : std::uint8_t
    {
        Arrival,           // default entry into the level
        OpeningOverboard,  // Jake arrives in the water after The Taking
        Checkpoint,        // matches CampaignState::SpawnId
    };

    struct SpawnPoint
    {
        std::string Id;
        SpawnRule Rule = SpawnRule::Arrival;
    };

    /**
     * Picks where the player enters a level when no map-local transform of this level was saved:
     * the campaign checkpoint's spawn, then the overboard arrival while Jake is still in the sea
     * (overboard, swimming or at the Outer Reef),
     * then the level's arrival point. Returns -1 when the level has no legal spawn (fail closed).
     */
    int ResolveSpawn(const CampaignState& State, const std::vector<SpawnPoint>& Points);
}
