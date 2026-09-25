#include "DarkArisen/Core/LevelSpawn.h"

namespace DarkArisen::Core
{
    int ResolveSpawn(const CampaignState& State, const std::vector<SpawnPoint>& Points)
    {
        const auto Find = [&Points](const auto& Predicate)
        {
            for (std::size_t Index = 0; Index < Points.size(); ++Index)
            {
                if (!Points[Index].Id.empty() && Predicate(Points[Index])) return static_cast<int>(Index);
            }
            return -1;
        };
        if (!State.SpawnId.empty())
        {
            const int Checkpoint = Find([&State](const SpawnPoint& Point)
            {
                return Point.Rule == SpawnRule::Checkpoint && Point.Id == State.SpawnId;
            });
            if (Checkpoint >= 0) return Checkpoint;
        }
        const WaterRecoveryState Recovery = State.Opening.RecoveryState;
        // Until the beach is reached Jake is still in the sea: dying never shortcuts the swim.
        if (Recovery == WaterRecoveryState::Overboard || Recovery == WaterRecoveryState::Swimming ||
            Recovery == WaterRecoveryState::OuterReef)
        {
            const int Overboard = Find([](const SpawnPoint& Point) { return Point.Rule == SpawnRule::OpeningOverboard; });
            if (Overboard >= 0) return Overboard;
        }
        return Find([](const SpawnPoint& Point) { return Point.Rule == SpawnRule::Arrival; });
    }
}
