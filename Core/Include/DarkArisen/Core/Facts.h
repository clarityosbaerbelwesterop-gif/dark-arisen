#pragma once

#include <string_view>

/**
 * Stable fact identifiers. Values are persisted and shared with the Unreal
 * reference runtime; renaming one requires a save migration.
 *
 * Routing rule: IDs starting with "Story." live in StoryFacts, all others in WorldFacts.
 */
namespace DarkArisen::Core::Facts
{
    // Chapter 1 (Docs/MAIN_STORY_AUTHORITY_2026_09.md, section 4).
    inline constexpr std::string_view FamilyIntact = "Story.FamilyIntact";
    inline constexpr std::string_view EthanPresent = "Story.EthanPresent";
    inline constexpr std::string_view DravenIdentified = "Story.DravenIdentified";
    inline constexpr std::string_view MarcDead = "Story.MarcDead";
    inline constexpr std::string_view DeniseDead = "Story.DeniseDead";
    inline constexpr std::string_view EthanAbducted = "Story.EthanAbducted";
    inline constexpr std::string_view JakeOverboard = "Story.JakeOverboard";
    inline constexpr std::string_view JakeSurvived = "Story.JakeSurvived";
    inline constexpr std::string_view EthanAliveLead = "Story.EthanAliveLead";

    // Chapter 8/9: Ethan's recovery and the separate dream figure.
    inline constexpr std::string_view EthanSignalUnderstood = "Story.EthanSignalUnderstood";
    inline constexpr std::string_view EthanRecovered = "Story.EthanRecovered";
    inline constexpr std::string_view EthanNoLongerCaptive = "Story.EthanNoLongerCaptive";
    inline constexpr std::string_view DreamResolved = "Story.DreamResolved";

    // Chapter 10 and completion.
    inline constexpr std::string_view DravenDefeated = "Story.DravenDefeated";
    inline constexpr std::string_view DravenKilled = "Story.DravenKilled";
    inline constexpr std::string_view DravenCaptured = "Story.DravenCaptured";
    inline constexpr std::string_view MainComplete = "Story.MainComplete";
    inline constexpr std::string_view MainCampaignComplete = "Story.MainCampaignComplete";
    inline constexpr std::string_view CreditsReachable = "Story.CreditsReachable";

    // World facts.
    inline constexpr std::string_view LaLiberacionOwned = "Ship.LaLiberacionOwned";
    inline constexpr std::string_view RacheUnlocked = "Combat.RacheUnlocked";
    inline constexpr std::string_view StateTreasuresComplete = "Network.StateTreasuresComplete";

    constexpr bool IsStoryFact(const std::string_view Fact)
    {
        return Fact.substr(0, 6) == "Story.";
    }
}
