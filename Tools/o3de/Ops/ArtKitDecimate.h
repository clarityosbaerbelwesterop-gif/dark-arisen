#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

/**
 * Game-ready versions of heavy scanned models (DarkArisenO3DE decimate-art). The scans of trees, grass, shrubs
 * and cliffs carry hundreds of thousands to millions of triangles; real time needs tens of thousands. The
 * source is a glTF 2.0 file with external buffers (the Poly Haven layout). Its node tree is flattened and each
 * material's triangles are simplified by a native quadric-error edge collapse (Garland & Heckbert): positions
 * welded so UV seams hold, open borders such as leaf and blade outlines kept by perpendicular planes, flips
 * refused. The result is written with the art kit's glTF writer, its textures referenced in place.
 */
namespace DarkArisen::Tools::Art
{
    struct DecimateStats
    {
        std::size_t SourceTriangles = 0;
        std::size_t Triangles = 0;
    };

    /**
     * Source: the scanned glTF. TargetTriangles: the budget for the whole model, shared out by material.
     * ImagePrefix: prepended to the source's image URIs so they resolve from the output's folder
     * (for Assets/Art/Scanned/game/<id>_<n>.gltf and textures under Assets/Art/Scanned/models/<id>/, "models/<id>/").
     * One glTF per mesh node (a scan's variants), each centred on its node's origin.
     */
    bool DecimateGltf(const std::filesystem::path& Source, std::size_t TargetTriangles, const std::string& ImagePrefix, const std::string& Name,
        std::vector<std::string>& OutGltfs, DecimateStats& Stats, std::string& Problem);
}
