#pragma once

// Fail-closed ingest of provider-generated 3D sources (Higgsfield 3D Jutsu GLB) into the
// O3DE project. Native C++ successor of Tools/higgsfield/import_3d_jutsu_glb.py.

#include <filesystem>
#include <string>
#include <string_view>

namespace DarkArisen::Tools
{
    struct GlbStats
    {
        std::size_t Bytes = 0;
        long long PositionVertices = 0;
        int Meshes = 0;
        int Materials = 0;
        int Images = 0;
        int Textures = 0;
        bool Normals = false;
        bool Uv0 = false;
        bool Skinned = false;
        int Animations = 0;
    };

    /** Validates a complete glTF 2.0 binary container. Returns false with a reason. */
    bool ValidateGlb(std::string_view Bytes, GlbStats& OutStats, std::string& OutError);

    /** A release source needs PBR materials with textures, normals and UV0. */
    bool IsReleaseGrade(const GlbStats& Stats, std::string& OutMissing);

    struct ImportRequest
    {
        std::filesystem::path RepoRoot;
        std::filesystem::path ManifestPath;   // ContentSource/.../Higgsfield3DSource.json
        std::filesystem::path GlbPath;        // exact downloaded provider revision
        long long MinimumVertices = 0;
        std::string ImportDateUtc;            // injected for determinism in tests
    };

    struct ImportResult
    {
        std::filesystem::path Destination;
        std::filesystem::path ProvenancePath;
        std::string Sha256;
        GlbStats Stats;
    };

    /**
     * Verifies size and ETag (MD5) against the committed manifest, validates the GLB,
     * copies it to Engine/O3DE/DarkArisen/Assets/Imported/<assetId>/<assetId>.glb for the
     * O3DE Asset Processor and writes <assetId>.provenance.json next to it.
     */
    bool ImportProviderGlb(const ImportRequest& Request, ImportResult& OutResult, std::string& OutError);
}
