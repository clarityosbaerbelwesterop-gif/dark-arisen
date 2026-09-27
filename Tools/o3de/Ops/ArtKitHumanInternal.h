#pragma once

// Internal structures of the human pipeline (ArtKitHumans.h). Frame: the MakeHuman frame
// throughout (decimetres, Y up, the person faces +Z, their left is +X); the CMU BVH clips use
// the same axes. Conversion to the O3DE world frame (metres, X forward, Y left, Z up) happens
// only when a figure is written: world = (z, x, y) / 10.

#include "ArtKitHumans.h"

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace DarkArisen::Tools::Art::Humans
{
    // ------------------------------------------------------------------ rotation maths

    struct Quat
    {
        double W = 1, X = 0, Y = 0, Z = 0;
    };
    Quat Mul(const Quat& A, const Quat& B);
    Quat Conj(const Quat& Q);
    Quat NormalizeQ(const Quat& Q);
    V3 Rotate(const Quat& Q, const V3& V);
    Quat AxisAngle(const V3& Axis, double Radians);
    /** Shortest rotation taking unit vector From onto unit vector To. */
    Quat FromTo(const V3& From, const V3& To);
    /** Rotation taking the frame (FromDir, FromUp) onto (ToDir, ToUp); swing only when an up vector is degenerate. */
    Quat FrameTo(const V3& FromDir, const V3& FromUp, const V3& ToDir, const V3& ToUp);

    // ------------------------------------------------------------------ source data

    struct Face
    {
        std::array<int, 4> V{};  // position indices
        std::array<int, 4> T{};  // texture coordinate indices
        int Count = 0;           // 3 or 4
        int Group = 0;
    };

    struct Bone
    {
        std::string Name;
        int Parent = -1;
        std::string Head, Tail;  // joint names
    };

    struct Motion
    {
        struct Joint
        {
            std::string Name;
            int Parent = -1;
            V3 Offset;
            std::vector<int> Channels;  // 0..2 position x y z, 3..5 rotation x y z
            int First = 0;              // index of the first channel in a frame
        };
        std::vector<Joint> Joints;      // parents first; end sites are joints named Parent + "_End"
        std::vector<std::vector<double>> Frames;
        double FrameTime = 1.0 / 30.0;
        int Find(std::string_view Name) const;
    };

    struct Source
    {
        std::vector<V3> Positions;                    // base mesh
        std::vector<std::array<double, 2>> Uvs;       // glTF convention (v down)
        std::vector<Face> Faces;
        std::vector<std::string> Groups;
        std::vector<Bone> Bones;                      // parents before children
        std::map<std::string, std::vector<int>, std::less<>> Joints;
        std::vector<std::vector<std::pair<int, double>>> Weights;  // per vertex (bone, weight), sums to 1
        std::map<std::string, std::vector<std::pair<int, V3>>, std::less<>> Targets;
        std::map<std::string, Motion, std::less<>> Motions;
        int Group(std::string_view Name) const;
        int FindBone(std::string_view Name) const;
    };

    bool LoadSource(const SourceReader& Reader, Source& Out, std::string& Problem);
    /** Loads a MakeHuman target once (name relative to targets/, without extension); nullptr when missing. */
    const std::vector<std::pair<int, V3>>* LoadTarget(const SourceReader& Reader, Source& Data, const std::string& Name, std::string& Problem);
    const Motion* LoadMotion(const SourceReader& Reader, Source& Data, const std::string& Clip, std::string& Problem);

    // ------------------------------------------------------------------ people

    struct Rgb
    {
        double R = 0, G = 0, B = 0;
    };

    enum class HairStyle { Bald, Shaved, Cropped, Short, Tousled, SlickedBack, TiedBack, Braid, LongLoose, PinnedUp, Tonsure };
    enum class BeardStyle { None, Stubble, Mustache, Trimmed, Full };
    enum class HatStyle { None, Tricorne, Bicorne, KnitCap, Headscarf, Straw, Headwrap };
    enum class Cloth { Linen, Wool, Cotton, Leather, Canvas, Silk };
    enum class Print { Plain, Stripes, Check, Pinstripe };

    /**
     * One garment cut from the body surface. Upper garments (shirt, vest, coat, bodice) use the
     * neck, sleeve and hem cuts; lower garments (legs) use the waist and leg end; stockings and
     * footwear run from LegStart to the toes.
     */
    struct Garment
    {
        bool On = false;
        Rgb Color{0.8, 0.8, 0.8};
        Cloth Fabric = Cloth::Linen;
        Print Pattern = Print::Plain;
        Rgb PatternColor{0.2, 0.2, 0.3};
        Rgb Trim{0, 0, 0};
        bool Trimmed = false;       // piping along the edges
        double Thick = 0.08;        // decimetres off the skin
        double NeckFront = 0.6;     // V depth below the neck base at the front
        double Sleeve = 1.9;        // arm parameter where the sleeve ends (0 shoulder, 1 elbow, 2 wrist)
        double Hem = 0.0;           // torso fraction of the hem (0 hip joints, 1 neck base; negative below the hips)
        double Open = 0.0;          // half width of an open front (decimetres)
        int Buttons = 0;
        Rgb ButtonColor{0.72, 0.6, 0.35};
        double Waist = 0.3;         // torso fraction of the waistband
        double LegEnd = 1.95;       // leg parameter where a leg ends (0 hip, 1 knee, 2 ankle, 3 toes)
        double LegStart = 0.0;      // stockings and footwear start here
        double Roughness = 0.85;
        double Wear = 0.4;          // dirt, salt and fading
    };

    struct Skirt
    {
        bool On = false;
        Rgb Color{0.3, 0.3, 0.3};
        Cloth Fabric = Cloth::Wool;
        Print Pattern = Print::Plain;
        Rgb PatternColor{0.2, 0.2, 0.2};
        double From = 0.3;          // torso fraction where it starts (waist)
        double Hem = 1.9;           // leg parameter of the hem
        double OpenFront = 0.0;     // half angle (radians) of the gap at the front; coat skirts
        double Flare = 0.2;         // outward spread per decimetre of drop
        double Thick = 0.1;
        Rgb Trim{0, 0, 0};
        bool Trimmed = false;
    };

    struct Band
    {
        bool On = false;
        Rgb Color{0.2, 0.12, 0.08};
        double Height = 0.45;       // decimetres
        double At = 0.3;            // torso fraction
        bool Buckle = true;
        Rgb BuckleColor{0.75, 0.62, 0.35};
        bool Knot = false;          // sash knot with hanging ends on the left hip
    };

    struct Apron
    {
        bool On = false;
        Rgb Color{0.3, 0.2, 0.12};
        Cloth Fabric = Cloth::Leather;
        double Top = 0.75;          // torso fraction of the bib top
        double Hem = 1.05;          // leg parameter of the hem
        double HalfAngle = 1.0;     // radians around the front
    };

    /** Scar as a segment in the face frame (decimetres, origin between the eyes) or the body frame. */
    struct Scar
    {
        V3 A, B;
        double Width = 0.03;
        bool Face = true;
    };

    struct PersonSpec
    {
        std::string Id;
        // body (MakeHuman macro parameters)
        double Male = 1.0;
        double Age = 30.0;          // years (25..90 drive the targets; younger looks come from face detail)
        double Muscle = 0.5, Weight = 0.5, Proportions = 0.8, BreastSize = 0.5;
        double African = 0.0, Asian = 0.0, Caucasian = 1.0;
        double Height = 1.75;       // metres
        std::vector<std::pair<std::string, double>> Detail;  // face and body detail targets
        // skin and eyes
        Rgb Skin{0.78, 0.6, 0.48};
        double Weathered = 0.3, Freckles = 0.0, Ruddy = 0.0, Wrinkles = 0.0;
        Rgb Eyes{0.35, 0.25, 0.15};
        // grooming
        Rgb Hair{0.12, 0.08, 0.05};
        double Gray = 0.0;          // share of grey strands
        HairStyle Hairdo = HairStyle::Short;
        double HairVolume = 1.0;
        double HairLength = 3.0;    // decimetres of hanging hair (braids, tails, loose hair)
        BeardStyle Beard = BeardStyle::None;
        double BeardLength = 0.2;   // decimetres at the chin
        // clothes (inner to outer)
        Garment Shirt, Vest, Coat, Legs, Stockings, Feet;
        Skirt Dress, Tails;
        Band Belt, Sash;
        Apron Front;
        HatStyle Hat = HatStyle::None;
        Rgb HatColor{0.1, 0.09, 0.08};
        Rgb HatTrim{0.7, 0.58, 0.3};
        bool HatTrimmed = false;
        // gear
        bool Cutlass = false, Pistol = false, Necklace = false, Cross = false, Satchel = false;
        std::vector<Scar> Scars;
        // pose from the motion library
        std::string Clip = "77_02";
        double Time = 1.0;          // seconds into the clip
        double Upright = 0.5;       // posture correction of the pose (0 as captured, 1 the body's own)
        int TextureSize = 1024;
        bool Uncanny = false;       // memory figure: drained, cold
    };

    /** The authored cast and crowd (ArtKitCast.cpp); nullptr for an unknown id. */
    const PersonSpec* FindPerson(std::string_view Id);

    // ------------------------------------------------------------------ bodies

    struct Anatomy
    {
        // category weights from the skin weights
        double Head = 0, Neck = 0, Torso = 0, Arm = 0, Hand = 0, Leg = 0, Foot = 0;
        double ArmT = 0;   // 0 shoulder, 1 elbow, 2 wrist, 3 finger tips
        double LegT = 0;   // 0 hip, 1 knee, 2 ankle, 3 toe tips
        double TorsoF = 0; // 0 hip joints, 1 neck base
        double Side = 0;   // +1 left, -1 right
        double Cavity = 0; // concavity (positive in creases)
    };

    struct Body
    {
        std::vector<V3> Rest;                        // every base mesh vertex, feet on y = 0
        std::vector<V3> Normals;                     // rest normals (body topology)
        std::vector<V3> Head, Tail;                  // per bone
        std::vector<Anatomy> Parts;                  // per vertex (body group only; helpers stay default)
        std::vector<std::uint8_t> IsBody;            // per vertex
        std::vector<std::vector<int>> Neighbours;    // per vertex, over the body topology (sorted, unique)
        // landmarks (rest)
        V3 EyeL, EyeR, EyeMid, Mouth, MouthL, MouthR, Chin, NoseTip, HeadTop, HeadCentre;
        V3 Ear[2];                                   // [0] left, [1] right: centre of the outer ear
        double HipY = 0, NeckY = 0, Stature = 0;
        V3 Shoulder[2], Elbow[2], Wrist[2], FingerTip[2], Hip[2], Knee[2], Ankle[2], ToeTip[2];  // [0] left, [1] right
        double TorsoF(double Y) const { return (Y - HipY) / (NeckY - HipY); }
        double TorsoY(double F) const { return HipY + F * (NeckY - HipY); }
    };

    bool BuildBody(const SourceReader& Reader, Source& Data, const PersonSpec& Spec, Body& Out, std::string& Problem);

    /** World rotation (rest = identity) and posed head position of every bone. */
    struct Pose
    {
        std::vector<Quat> World;
        std::vector<V3> Head;
    };
    /** Rest pose (identity rotations). */
    Pose RestPose(const Source& Data, const Body& B);
    /**
     * Retargets frame Frame of Clip onto the body. The heading is normalised so the person faces +Z
     * at frame Reference, with the hips over the origin there. Upright (0..1) eases the spine, neck
     * and head back towards the body's own posture (mocap performers often look down); Curl (0..1)
     * relaxes the fingers, which the clips do not capture.
     */
    Pose PoseFromMotion(const Source& Data, const Body& B, const Motion& Clip, int Frame, int Reference, double Upright = 0.0, double Curl = 0.35);

    /** Skinned vertex (rest frame) of a figure part. */
    struct SkinVertex
    {
        V3 P, N;
        double U = 0, V = 0;
        std::array<int, 4> Joint{0, 0, 0, 0};
        std::array<double, 4> Weight{1, 0, 0, 0};
    };

    /** Top four influences of base mesh vertex Index, renormalised. */
    void InfluencesOf(const Source& Data, int Index, std::array<int, 4>& Joint, std::array<double, 4>& Weight);
    V3 SkinPoint(const Body& B, const Pose& P, const SkinVertex& V);
    V3 SkinNormal(const Pose& P, const SkinVertex& V);

    /** One material's worth of a dressed person in the rest frame. */
    struct FigurePart
    {
        Material Mat;
        std::vector<SkinVertex> Vertices;
        std::vector<std::uint32_t> Indices;
        bool Ground = false;  // counts for standing a posed figure on z = 0
    };

    /** Skinned actor (bind pose) glTF: the MakeHuman skeleton, one primitive per part (ArtKitHumanActor.cpp). */
    std::string WriteActorGltf(const Source& Data, const Body& B, const std::string& Name, const std::vector<FigurePart>& Parts);
    /** Skeleton plus one animation retargeted from Clip frames First..Last (inclusive). */
    std::string WriteMotionGltf(const Source& Data, const Body& B, const std::string& Name, const Motion& Clip, int First, int Last);
}
