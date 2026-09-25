#pragma once

// Frame conversion for Dark Arisen greybox glTF sources.
//
// ContentSource layouts and greybox meshes share one numeric frame: Unreal's left-handed axes
// (+X forward, +Y right, +Z up), layouts in centimetres, meshes in metres, written as Z-up even
// inside glTF (the glTF convention is right-handed Y-up). O3DE is right-handed Z-up, and this
// project imports glTF with the root transform (AssImpReadRootTransform), which maps a glTF point
// g to O3DE (-g.x, g.z, g.y).
//
// The O3DE world keeps forward (+X) and up (+Z) and mirrors Y, so port stays port and starboard
// stays starboard:  world = (s.x, -s.y, s.z). Meshes are rewritten as spec-compliant glTF with
// g = (-s.x, s.z, -s.y), which O3DE then imports exactly at world. The map is a reflection, so
// triangle winding is reversed to keep faces pointing outwards.

#include <cstddef>
#include <string>
#include <string_view>

namespace DarkArisen::Tools
{
    struct Vec3d
    {
        double X = 0.0;
        double Y = 0.0;
        double Z = 0.0;
    };

    /** Layout point or direction (Unreal centimetres, e.g. an anchor or a current) to O3DE world metres. */
    constexpr Vec3d LayoutToWorld(const Vec3d& Centimetres)
    {
        return {Centimetres.X / 100.0, -Centimetres.Y / 100.0, Centimetres.Z / 100.0};
    }

    /** Axis-aligned half extents (Unreal centimetres) to O3DE metres: a mirror leaves sizes unchanged. */
    constexpr Vec3d LayoutExtentToWorld(const Vec3d& Centimetres)
    {
        return {Centimetres.X / 100.0, Centimetres.Y / 100.0, Centimetres.Z / 100.0};
    }

    /** Greybox mesh point (source metres) to the glTF point O3DE imports at the world position. */
    constexpr Vec3d SourceMeshToGltf(const Vec3d& Metres)
    {
        return {-Metres.X, Metres.Z, -Metres.Y};
    }

    struct GltfConversionStats
    {
        std::size_t Vertices = 0;
        std::size_t Triangles = 0;
    };

    struct GltfConversionOptions
    {
        /**
         * Skinned greybox characters become static bind-pose meshes: skins, animations, joint nodes
         * and JOINTS/WEIGHTS attributes are dropped. The actor pipeline (EMotionFX) is separate.
         */
        bool StripSkinToBindPose = false;
    };

    /**
     * Rewrites a static greybox glTF (embedded base64 buffers, indexed triangles, POSITION and
     * optional NORMAL) into the O3DE frame. Fails closed on anything it cannot convert exactly:
     * skins, animations, node transforms, extensions, sparse accessors, external buffers,
     * non-triangle primitives or malformed accessors.
     */
    bool ConvertGreyboxGltf(std::string_view Source, std::string& OutGltf, GltfConversionStats& OutStats, std::string& OutError,
        const GltfConversionOptions& Options = {});

    /**
     * Placeholder figure for characters that have no greybox in ContentSource: an octagonal prism
     * with pointed caps, standing on the origin, in the source frame (metres, Z up). Feed it to
     * ConvertGreyboxGltf like any other greybox.
     */
    std::string MakePlaceholderFigureGltf(double Height, double Radius, std::string_view Name);

    /**
     * Flat still-water grid (source frame, Z up) centred on the origin: Size metres square with
     * Cells x Cells quads. The ocean shader displaces it; at most 254 cells (16-bit indices).
     */
    std::string MakeOceanGridGltf(double Size, int Cells, std::string_view Name);

    /** Placeholder prop: an axis-aligned box standing on the origin, source frame. */
    std::string MakePlaceholderBoxGltf(double SizeX, double SizeY, double SizeZ, std::string_view Name);
}
