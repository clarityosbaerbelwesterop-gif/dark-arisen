#pragma once

// ContentSource/Story mission contracts (chapters 3-10) and their ContentSource/Naval companions,
// read into the engine-neutral Core::StoryMissionContract. Parsing is strict: a missing or mistyped
// field is an error, never a default, and unknown actor fields are rejected so authoring typos
// cannot silently drop behaviour (the Unreal materialiser read "verb" while one contract wrote
// "actionType", which left Chapter 6 impossible to complete).

#include "Json.h"

#include <DarkArisen/Core/StoryWorld.h>

#include <string>
#include <vector>

namespace DarkArisen::Tools
{
    bool ParseStoryContract(const JsonValue& Root, const std::string& SourcePath, Core::StoryMissionContract& Out,
        std::vector<std::string>& OutErrors);

    /**
     * Folds a ContentSource/Naval contract into its mission: the player's ship, the hostile ships
     * and the completion gate become actors at synthetic "Naval.<name>" anchors.
     */
    bool MergeNavalContract(const JsonValue& Root, const std::string& SourcePath, Core::StoryMissionContract& InOut,
        std::vector<std::string>& OutErrors);

    /** UE ANavalMissionGateActor::CompletionRadius (1800 cm). */
    inline constexpr double NavalMissionGateRadiusMetres = 18.0;
}
