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

    /**
     * One clip of the gameplay motion library (ContentSource/Animations/HumanMotionLibrary.json): a cut of a
     * CMU clip retargeted onto a person's skeleton. Times are seconds in the source clip unless noted.
     */
    struct MotionSpec
    {
        std::string Name;            // animation name, e.g. AN_Jake_LightAttack1
        std::string Person = "jake"; // whose proportions the skeleton has
        std::string Clip;            // CMU clip id
        double From = 0.0, To = 0.0; // cut
        bool Loop = false;           // cycle: the end crossfades into the start; horizontally in place
        double Blend = 0.3;          // loop crossfade (output seconds)
        bool InPlace = false;        // one-shot without horizontal root motion
        bool Mirror = false;         // left and right exchanged
        double Speed = 1.0;          // playback rate when not timed
        double Contact = -1.0;       // hit moment in the clip; with ContactAt and End the cut is time-warped
        double ContactAt = 0.0;      // output seconds of the hit
        double End = 0.0;            // output seconds of the end
        std::string UpperClip;       // optional upper body (spine02 and above) held from this clip...
        double UpperAt = 0.0;        // ...at this time
        double Upright = 0.35;       // posture correction (PoseFromMotion)
        double Curl = 0.35;          // finger curl (0.35 relaxed, 1 a closed grip)
        bool Straighten = false;     // the turn over the cut is unwound: the person ends facing where they started
        /** Output duration in seconds. */
        double Duration() const { return Contact >= 0.0 ? End : (To - From) / Speed; }
    };

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
        /** Gameplay motion library clip (glTF: skeleton plus one animation named Spec.Name). */
        bool BuildLibraryMotion(const MotionSpec& Spec, std::string& OutGltf, std::string& Problem);
        /** Every cast and crowd id, sorted. */
        static std::vector<std::string> People();
        /** Clip ids of the motion library, in a fixed order. */
        static std::vector<std::string> MotionClips();

    private:
        struct Impl;
        std::unique_ptr<Impl> P;
    };
}
