#pragma once

#include "DarkArisen/Core/CampaignState.h"

#include <string>
#include <string_view>
#include <vector>

namespace DarkArisen::Core
{
    /** Opening-route invariants shared by the runtime and save validation. */
    bool IsOpeningProgressValid(const OpeningProgress& Progress);

    /**
     * Fail-closed validation of a complete campaign state. Port of
     * UMainStorySubsystem::ValidateState including the PR #52 hardening:
     * canonical mission order/identity, no skipped prerequisites, a single legal
     * current mission/chapter, finite player vitals and transforms.
     */
    bool ValidateCampaignState(const CampaignState& Candidate, std::vector<std::string>& OutErrors);

    /**
     * Upgrades an older campaign state in place to CampaignState::CurrentVersion.
     * Unknown or future versions are rejected.
     */
    bool MigrateCampaignState(CampaignState& Candidate, std::vector<std::string>& OutErrors);

    /**
     * Map-local coordinates only apply inside the level that produced them.
     * An empty source (legacy save) never matches: the authored arrival spawn is used instead.
     */
    bool ShouldRestoreMapLocalTransform(std::string_view SnapshotSourceLevel, std::string_view CurrentLevel);
}
