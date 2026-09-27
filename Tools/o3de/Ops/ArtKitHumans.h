#pragma once

// People for Dark Arisen, built natively from free source data under ContentSource/ThirdParty:
// MakeHuman (CC0) supplies the base mesh, morph targets, skeleton and skin weights, and the CMU
// Graphics Lab Motion Capture Database supplies motion.
//
// For each person the kit:
// - morphs the body (sex, age, ethnicity, muscle, weight, proportions and face detail);
// - cuts clothes from the body surface and grows hair and beards as layered shells;
// - bakes skin, cloth, hair and eye shading into the MakeHuman UV layout;
// - poses the body with retargeted motion capture.
//
// Levels get static posed meshes. A skinned actor with motion clips is also written for
// EMotionFX (runtime verification pending). Every step uses basic IEEE arithmetic, so the
// output has the same bytes on every compiler.

#include "ArtKit.h"
#include "ArtKitCore.h"

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace DarkArisen::Tools::Art
{
    /** Cast member or crowd archetype for an entity name ("crew.bigtom.c07" -> "bigtom", strangers -> a townsfolk id). */
    std::string PersonFor(std::string_view EntityName);

    class HumanFactory
    {
    public:
        explicit HumanFactory(SourceReader Reader);
        ~HumanFactory();
        HumanFactory(const HumanFactory&) = delete;
        HumanFactory& operator=(const HumanFactory&) = delete;

        /** Static figure in its authored pose: world frame, feet on z = 0, facing +X. New textures are appended to Files. */
        bool BuildFigure(const std::string& PersonId, Mesh& Out, std::vector<ArtFile>& Files, std::string& Problem);
        /** Skinned actor in bind pose (glTF with skin); new textures are appended to Files. */
        bool BuildActor(const std::string& PersonId, std::string& OutGltf, std::vector<ArtFile>& Files, std::string& Problem);
        /** Motion clip retargeted onto the person's skeleton (glTF: skeleton plus one animation). */
        bool BuildMotion(const std::string& PersonId, const std::string& ClipId, std::string& OutGltf, std::string& Problem);
        /** Clip ids of the motion library, in a fixed order. */
        static std::vector<std::string> MotionClips();

    private:
        struct Impl;
        std::unique_ptr<Impl> P;
    };
}
