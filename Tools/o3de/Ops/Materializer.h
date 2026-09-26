#pragma once

// ContentSource -> O3DE materialiser for the campaign: the Chapter 1 slice (Harlow opening ->
// Outer Reef -> Driftwood Beach -> Driftwood Camp), every ContentSource/Story mission contract of
// chapters 3-10 with its ContentSource/Naval companion, the credits, and a 34-mission physical
// coverage report (Levels/PhysicalCoverage.json).
//
// Port of UDarkArisenMaterializeAlphaCommandlet, extended where the Unreal maps could not be
// played through (see Docs/O3DE_MIGRATION.md): it reads the authored layouts, the family manifest
// and the opening presentation, converts the greybox meshes into the O3DE frame and writes level
// prefabs whose gameplay components are the Dark Arisen Gem's. Every anchor, file and value it
// needs is required: a missing one fails the run and nothing is written.

#include <filesystem>
#include <string>
#include <vector>

namespace DarkArisen::Tools
{
    struct MaterializedFile
    {
        std::string RelativePath;  // from the repository root, '/' separators
        std::string Content;
    };

    struct MaterializeResult
    {
        std::vector<MaterializedFile> Files;
        std::vector<std::string> Errors;
    };

    /** Builds every output in memory. Returns false (and fills Errors) on any missing input. */
    bool MaterializeCampaign(const std::filesystem::path& RepoRoot, MaterializeResult& Out);

    /**
     * Writes the result, or with Check compares it with the files on disk and reports each stale
     * or missing one in OutStale. Returns false on write failure or, with Check, on any difference.
     */
    bool ApplyMaterialization(const std::filesystem::path& RepoRoot, const MaterializeResult& Result, bool Check,
        std::vector<std::string>& OutStale);
}
