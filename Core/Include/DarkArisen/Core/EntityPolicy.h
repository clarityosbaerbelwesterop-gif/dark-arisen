#pragma once

#include <string_view>

/**
 * Canon guards that must hold in every engine.
 *
 * Real Ethan ("character.ethan") is never hostile, never a boss and never a damage
 * target, before or after his Chapter-8 rescue. The Chapter-9 dream/memory figure is a
 * separate entity ("boss.dream_ethan") and is the only Ethan-shaped combatant.
 * Design law 10: no hostile input, targeting, damage, force or ragdoll path may
 * connect with a child.
 */
namespace DarkArisen::Core::EntityPolicy
{
    inline constexpr std::string_view RealEthanId = "character.ethan";
    inline constexpr std::string_view DreamEthanBossId = "boss.dream_ethan";
    inline constexpr std::string_view JakeId = "character.jake";

    bool IsRealEthan(std::string_view EntityId);
    bool IsChild(std::string_view EntityId);
    /** Whether any hostile path (targeting, damage, force) may reach this entity. */
    bool CanBeHostileTarget(std::string_view EntityId);
    /** Whether this entity may be registered as a boss encounter. */
    bool CanBeBoss(std::string_view EntityId);
}
