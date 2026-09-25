#include "DarkArisen/Core/EntityPolicy.h"

namespace DarkArisen::Core::EntityPolicy
{
    bool IsRealEthan(const std::string_view EntityId)
    {
        return EntityId == RealEthanId;
    }

    bool IsChild(const std::string_view EntityId)
    {
        // Authored children use the "child." namespace (e.g. "child.rexa.dock_runner").
        return EntityId.substr(0, 6) == "child.";
    }

    bool CanBeHostileTarget(const std::string_view EntityId)
    {
        return !EntityId.empty() && !IsRealEthan(EntityId) && !IsChild(EntityId);
    }

    bool CanBeBoss(const std::string_view EntityId)
    {
        if (!CanBeHostileTarget(EntityId))
        {
            return false;
        }
        // Any "ethan" boss other than the dream figure is the forbidden legacy betrayal branch.
        const bool MentionsEthan = EntityId.find("ethan") != std::string_view::npos;
        return !MentionsEthan || EntityId == DreamEthanBossId;
    }
}
