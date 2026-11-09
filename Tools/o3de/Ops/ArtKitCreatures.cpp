// Creature bosses of docs/design/bosses, built from their documents' visual descriptions. Static meshes in the bind
// pose; their rigs and motions are later work. Only + - * / and sqrt (and the kit's series sine and cosine) reach the
// output, so every compiler writes the same bytes.

#include "ArtKit.h"
#include "ArtKitCore.h"
#include "ArtKitLibrary.h"

#include <algorithm>
#include <cmath>

namespace DarkArisen::Tools::Art
{
    namespace
    {
        Material Flesh(const char* Name, const double R, const double G, const double B, const double Roughness)
        {
            Material M;
            M.Name = Name;
            M.BaseColor = {R, G, B, 1.0};
            M.Roughness = Roughness;
            return M;
        }

        /** A scanned CC0 surface (Poly Haven, Scanned.lock) tinted to the creature's colour, at Tile metres a repeat. */
        Material Hide(const char* Name, const char* Set, const double R, const double G, const double B, const double Tile, const double RoughScale)
        {
            Material M;
            const std::string Base = std::string("Scanned/") + Set + "/" + Set;
            M.Name = Name;
            M.BaseColorTexture = Base + "_diff_2k.jpg";
            M.NormalTexture = Base + "_nor_gl_2k.jpg";
            M.OrmTexture = Base + "_arm_2k.jpg";
            M.BaseColor = {R, G, B, 1.0};
            M.Roughness = RoughScale;
            M.MetresPerTile = Tile;
            return M;
        }

        /** Dome of radius R springing at Z (Rise tall) over (X, Y). */
        void Dome(Mesh& Out, const Material& M, const double X, const double Y, const double Z, const double R, const double Rise, const int Sides)
        {
            std::vector<Ring> Rings;
            for (int K = 0; K <= 6; ++K)
            {
                const double A = Pi / 2 * K / 6.0, Radius = std::max(R * Cos(A), 0.004);
                Rings.push_back(Ellipse(X, Y, Z + Rise * Sin(A), Radius, Radius, Sides));
            }
            Out.Loft(M, Rings, true);
        }

        /** One cross-section of a body along X: half width, half height, the centre's height, and how flat the underside is. */
        struct Section
        {
            double X, HalfWidth, HalfHeight, Z, Belly;
        };

        /**
         * A body lofted through sections along X, the back (Upper) and the underside (Lower) in their own materials. Each
         * ring runs from one flank over the back (or under the belly) to the other.
         */
        void LoftBody(Mesh& Out, const Material& Flank, const Material& Spine, const Material& Lower, const std::vector<Section>& Sections, const int Steps)
        {
            // the back in three bands (right flank, the dark spine, left flank), the underside in one
            struct Band { int From, To; bool Top; const Material* M; };
            const int S0 = Steps * 3 / 8, S1 = Steps - Steps * 3 / 8;
            const Band Bands[] = {{0, S0, true, &Flank}, {S0, S1, true, &Spine}, {S1, Steps, true, &Flank}, {0, Steps, false, &Lower}};
            for (const Band& B : Bands)
            {
                std::vector<std::vector<V3>> Rings;
                for (const Section& S : Sections)
                {
                    std::vector<V3> Ring;
                    for (int K = B.From; K <= B.To; ++K)
                    {
                        const double A = Pi * K / Steps;  // 0 at the right flank (-Y) to pi at the left (+Y)
                        const double Y = -S.HalfWidth * Cos(A), Up = S.HalfHeight * Sin(A);
                        Ring.push_back({S.X, B.Top ? Y : -Y, B.Top ? S.Z + Up : S.Z - Up * S.Belly});
                    }
                    Rings.push_back(Ring);
                }
                Out.Loft(*B.M, Rings, false, !B.Top);
            }
        }

        /** Where the body's back lies at X (for laying scutes and growths on it). */
        double BackAt(const std::vector<Section>& Sections, const double X, double& HalfWidth)
        {
            for (std::size_t K = 0; K + 1 < Sections.size(); ++K)
                if (X >= Sections[K].X && X <= Sections[K + 1].X)
                {
                    const double T = (X - Sections[K].X) / (Sections[K + 1].X - Sections[K].X);
                    HalfWidth = Sections[K].HalfWidth + (Sections[K + 1].HalfWidth - Sections[K].HalfWidth) * T;
                    return Sections[K].Z + Sections[K].HalfHeight + (Sections[K + 1].Z + Sections[K + 1].HalfHeight - Sections[K].Z - Sections[K].HalfHeight) * T;
                }
            HalfWidth = 0.0;
            return 0.0;
        }

        /** Point at T (0..1) along a polyline, by equal shares per segment. */
        V3 Along(const std::vector<V3>& Path, const double T)
        {
            const double Segments = static_cast<double>(Path.size() - 1), At = Clamp(T, 0.0, 1.0) * Segments;
            const std::size_t K = std::min(static_cast<std::size_t>(At), Path.size() - 2);
            const double F = At - static_cast<double>(K);
            return Path[K] + (Path[K + 1] - Path[K]) * F;
        }

        /**
         * A twisted bundle of Count strands round a polyline axis: roots grown into a limb. Spread is the strands' distance
         * from the axis (at the start, at the end), Radius each strand's thickness, Twist the turns along the whole length.
         */
        void Bundle(Mesh& Out, const Material& M, const std::vector<V3>& Path, const int Count, const double Spread0, const double Spread1,
                    const double Radius0, const double Radius1, const double Twist, const std::uint32_t Seed, const int Sides = 7)
        {
            Rng R(Seed);
            const int Steps = 6 * static_cast<int>(Path.size() - 1);
            for (int S = 0; S < Count; ++S)
            {
                const double Phase = 2.0 * Pi * S / Count + R.Range(-0.3, 0.3), Thick = R.Range(0.8, 1.15);
                V3 Prev{};
                for (int K = 0; K <= Steps; ++K)
                {
                    const double T = static_cast<double>(K) / Steps;
                    const V3 A = Along(Path, std::max(T - 0.02, 0.0)), B = Along(Path, std::min(T + 0.02, 1.0));
                    const V3 D = Normalize(B - A);
                    const V3 Ref = std::abs(D.Z) > 0.9 ? V3{1, 0, 0} : V3{0, 0, 1};
                    const V3 U = Normalize(Cross(D, Ref)), W = Cross(D, U);
                    const double Angle = Phase + Twist * 2.0 * Pi * T, Spread = Lerp(Spread0, Spread1, T);
                    const V3 P = Along(Path, T) + (U * Cos(Angle) + W * Sin(Angle)) * Spread;
                    if (K) Out.Tube(M, Prev, P, Lerp(Radius0, Radius1, T - 1.0 / Steps) * Thick, Lerp(Radius0, Radius1, T) * Thick, Sides);
                    Prev = P;
                }
            }
        }

        /**
         * A broken slab (a carved temple stone, an armour plate) of half sizes H, turned by Yaw, Pitch, Roll about its centre
         * C; each corner knocked out of true by up to Chip of the half size, so no two read as cut boxes.
         */
        void Slab(Mesh& Out, const Material& M, const V3& C, const V3& H, const double Yaw, const double Pitch, const double Roll,
                  const std::uint32_t Seed = 1u, const double Chip = 0.22)
        {
            Rng R(Seed);
            V3 P[8];
            for (int K = 0; K < 8; ++K)
            {
                const double Sx = (K == 1 || K == 2 || K == 5 || K == 6) ? 1.0 : -1.0, Sy = (K == 2 || K == 3 || K == 6 || K == 7) ? 1.0 : -1.0, Sz = K >= 4 ? 1.0 : -1.0;
                P[K] = {Sx * H.X * (1.0 - Chip * R.Unit()), Sy * H.Y * (1.0 - Chip * R.Unit()), Sz * H.Z * (1.0 - Chip * R.Unit())};
            }
            Out.Push(Xf::Translate(C) * Xf::RotateZ(Yaw) * Xf::RotateY(Pitch) * Xf::RotateX(Roll));
            Out.Quad(M, P[4], P[5], P[6], P[7]);
            Out.Quad(M, P[1], P[0], P[3], P[2]);
            Out.Quad(M, P[5], P[1], P[2], P[6]);
            Out.Quad(M, P[0], P[4], P[7], P[3]);
            Out.Quad(M, P[7], P[6], P[2], P[3]);
            Out.Quad(M, P[0], P[1], P[5], P[4]);
            Out.Pop();
        }

        /** An ellipsoid cap (half-ellipsoid) facing +X at C: Tall, Wide, Deep half sizes. */
        void FrontCap(Mesh& Out, const Material& M, const V3& C, const double Tall, const double Wide, const double Deep, const int Sides)
        {
            Out.Push(Xf::Translate(C) * Xf::RotateY(Pi / 2) * Xf::Scale({Tall, Wide, Deep}));
            Dome(Out, M, 0, 0, 0, 1.0, 1.0, Sides);
            Out.Pop();
        }

        /** A sharpened feather: a long blade from Root along Dir, widest a third of the way, lying across Normal. */
        void Feather(Mesh& Out, const Material& M, const V3& Root, const V3& Dir, const V3& Normal, const double Length, const double Width)
        {
            const V3 Side = Normalize(Cross(Normal, Dir)) * (Width * 0.5), Lift = Normal * (0.02 * Length);
            Out.Quad(M, Root, Root + Dir * (Length * 0.32) + Side + Lift, Root + Dir * Length + Lift * 2.0, Root + Dir * (Length * 0.32) - Side + Lift, true);
        }

        /** A closed ellipsoid at C with half sizes Rx, Ry, Rz. */
        void Ellipsoid(Mesh& Out, const Material& M, const V3& C, const double Rx, const double Ry, const double Rz, const int Sides)
        {
            Out.Push(Xf::Translate(C) * Xf::Scale({Rx, Ry, Rz}));
            Dome(Out, M, 0, 0, 0, 1.0, 1.0, Sides);
            Out.Push(Xf::Scale({1.0, 1.0, -1.0}));
            Dome(Out, M, 0, 0, 0, 1.0, 1.0, Sides);
            Out.Pop();
            Out.Pop();
        }
    }

    /**
     * Jaw of the Mire, "the Swallowing King" (jaw_of_the_mire.md 2): an alligator fourteen metres from nose to tail tip, so
     * old he no longer looks like one; moss, mud, vines and coral-like growths over his back; the left eye clouded, 40% of
     * the left front tooth gone, a rusted harpoon head in the right shoulder from a hunt eighty years ago; dark olive skin
     * going near-black along the spine, a pale yellow belly, mottled brown-yellow teeth, blood-brown streaks round the
     * mouth; the good eye golden-amber, the blind one milky. Snout to +X, standing on z = 0.
     */
    Mesh BuildJawOfTheMire()
    {
        Mesh Out("SM_Art_Creature_JawOfTheMire");
        // the hide: a pitted, mossy scan reads as old scales; the belly's plates as a cobbled grid, pale yellow
        const Material Skin = Hide("M_Art_Jaw_Skin", "rock_pitted_mossy", 0.3, 0.36, 0.19, 0.7, 0.9);
        const Material Spine = Hide("M_Art_Jaw_Spine", "rock_pitted_mossy", 0.16, 0.18, 0.13, 0.7, 1.0);
        const Material Belly = Hide("M_Art_Jaw_Belly", "cobblestone_floor_08", 1.25, 1.12, 0.7, 0.55, 0.8);
        const Material Teeth = Flesh("M_Art_Jaw_Teeth", 0.58, 0.48, 0.28, 0.45);
        const Material Blood = Flesh("M_Art_Jaw_Blood", 0.26, 0.1, 0.06, 0.55);
        const Material Amber = Flesh("M_Art_Jaw_EyeAmber", 0.85, 0.58, 0.12, 0.1);
        const Material Milky = Flesh("M_Art_Jaw_EyeMilky", 0.84, 0.84, 0.8, 0.25);
        const Material Rust = Flesh("M_Art_Jaw_Rust", 0.36, 0.18, 0.09, 0.9);
        const Material Mouth = Flesh("M_Art_Jaw_Mouth", 0.08, 0.03, 0.03, 0.6);
        const Material Coral = Hide("M_Art_Jaw_Coral", "rock_pitted_mossy", 0.52, 0.5, 0.4, 2.0, 0.95);
        const Material Vine = Flesh("M_Art_Jaw_Vine", 0.09, 0.13, 0.05, 0.85);

        // the body from tail tip to nose: hips at -2.5, the widest belly at 0, shoulders at 2.6, the head from 4.2
        const std::vector<Section> Body = {
            {-7.0, 0.06, 0.06, 0.35, 0.6}, {-6.2, 0.18, 0.24, 0.4, 0.6}, {-5.0, 0.36, 0.42, 0.5, 0.6}, {-3.8, 0.55, 0.52, 0.58, 0.6},
            {-2.6, 0.82, 0.62, 0.66, 0.55}, {-1.4, 1.08, 0.7, 0.7, 0.5}, {0.0, 1.26, 0.76, 0.72, 0.5}, {1.4, 1.12, 0.72, 0.72, 0.5},
            {2.6, 0.88, 0.62, 0.75, 0.55}, {3.4, 0.62, 0.5, 0.78, 0.6}, {4.2, 0.64, 0.44, 0.8, 0.6}, {5.0, 0.52, 0.32, 0.74, 0.6},
            {5.8, 0.46, 0.24, 0.66, 0.6}, {6.5, 0.4, 0.18, 0.6, 0.6}, {7.0, 0.33, 0.14, 0.56, 0.6}, {7.14, 0.14, 0.06, 0.55, 0.6}};
        LoftBody(Out, Skin, Spine, Belly, Body, 16);

        // the mouth: a dark seam along each side of the head, the teeth above and below it, blood at the lips
        for (const double Side : {-1.0, 1.0})
        {
            for (double X = 4.3; X < 6.95; X += 0.21)
            {
                double Half = 0.0;
                BackAt(Body, X, Half);
                const double Y = Side * (Half - 0.02), Z = 0.6 - (X - 4.3) * 0.02;
                // 40% of the left front tooth row is gone (the left is +Y, the front the last metre)
                const bool Gone = Side > 0.0 && X > 6.2 && X < 6.75;
                if (!Gone)
                {
                    Out.Tube(Teeth, {X, Y, Z + 0.02}, {X, Y * 1.02, Z - 0.13}, 0.035, 0.002, 5, false);
                    Out.Tube(Teeth, {X + 0.1, Y, Z - 0.02}, {X + 0.1, Y * 1.02, Z + 0.1}, 0.03, 0.002, 5, false);
                }
                else
                    Out.Tube(Teeth, {X, Y, Z + 0.02}, {X, Y * 1.01, Z - 0.04}, 0.035, 0.03, 5, false);  // the broken stump
            }
            OrientedBox(Out, Mouth, {5.6, Side * 0.43, 0.6}, 1.0, -Side * 0.07, {1.35, 0.02, 0.03});
            OrientedBox(Out, Blood, {6.5, Side * 0.3, 0.57}, 1.0, -Side * 0.12, {0.45, 0.025, 0.05});
            OrientedBox(Out, Blood, {5.2, Side * 0.5, 0.63}, 1.0, -Side * 0.05, {0.3, 0.02, 0.04});
            // nostrils at the tip
            Dome(Out, Spine, 7.0, Side * 0.08, 0.66, 0.06, 0.05, 8);
        }
        // the eyes high on the head, each under a bony ridge: the left (+Y) clouded, the right golden
        for (const double Side : {-1.0, 1.0})
        {
            Rock(Out, Spine, {4.6, Side * 0.32, 1.1}, 0.18, 0.12, Side > 0.0 ? 17u : 23u);
            Dome(Out, Side > 0.0 ? Milky : Amber, 4.72, Side * 0.33, 1.12, 0.09, 0.06, 12);
        }

        // four legs splayed to the side, elbows out, broad clawed feet flat on the ground
        for (const double Fore : {2.3, -2.2})
            for (const double Side : {-1.0, 1.0})
            {
                const double Hip = Fore > 0.0 ? 0.85 : 0.95;
                const V3 Root{Fore, Side * Hip, 0.62}, Elbow{Fore + (Fore > 0.0 ? 0.25 : -0.3), Side * (Hip + 0.7), 0.42}, Foot{Fore + (Fore > 0.0 ? 0.45 : -0.1), Side * (Hip + 0.9), 0.06};
                Out.Tube(Skin, Root, Elbow, 0.32, 0.22, 12);
                Out.Tube(Skin, Elbow, Foot, 0.22, 0.16, 12);
                Rock(Out, Skin, {Root.X, Root.Y, Root.Z - 0.35}, 0.42, 0.62, Fore > 0.0 ? 31u : 37u);  // the shoulder or haunch
                Rock(Out, Skin, {Elbow.X, Elbow.Y, Elbow.Z - 0.22}, 0.24, 0.42, Side > 0.0 ? 41u : 43u);  // the elbow or knee
                OrientedBox(Out, Skin, {Foot.X + 0.2, Foot.Y, 0.06}, 1.0, 0.0, {0.32, 0.24, 0.07});
                for (int Toe = 0; Toe < 5; ++Toe)
                {
                    const double Spread = (Toe - 2) * 0.11;
                    Out.Tube(Teeth, {Foot.X + 0.48, Foot.Y + Spread, 0.05}, {Foot.X + 0.7, Foot.Y + Spread * 1.4, 0.0}, 0.035, 0.004, 5, false);
                }
            }

        Rng R0(0x5C17u);
        // the armour along the back: paired rows of ridged scutes, four rows over the body and two down the tail, in the
        // near-black of the spine
        for (double X = -6.6; X < 4.0; X += 0.42)
        {
            double Half = 0.0;
            const double Top = BackAt(Body, X, Half);
            const double Size = 0.05 + 0.1 * Clamp(Half / 1.26, 0.2, 1.0);
            const std::vector<double> Rows = X < -2.6 ? std::vector<double>{-0.25, 0.25} : std::vector<double>{-0.45, -0.17, 0.17, 0.45};
            for (const double Row : Rows)
            {
                // low keeled plates, each a little different, not spikes
                const double Y = Row * std::max(Half, 0.25) / 0.9, Jx = R0.Range(-0.05, 0.05), Grow = R0.Range(0.8, 1.2);
                Out.Tube(Spine, {X + Jx, Y, Top - 0.05}, {X + Jx, Y, Top + Size * 0.55 * Grow}, Size * 1.1 * Grow, Size * 0.15, 4);
            }
        }

        // a piece of the Mire that learned to eat: moss and mud mats, coral-like crusts, and vines trailing down the flanks
        Rng R(0x4A41u);
        for (int K = 0; K < 40; ++K)
        {
            const double X = R.Range(-4.5, 3.6);
            double Half = 0.0;
            const double Top = BackAt(Body, X, Half);
            const double Y = R.Range(-0.8, 0.8) * Half;
            const double Z = Top - 0.08 - (Y * Y) / std::max(Half, 0.1) * 0.5;
            if (K % 3 == 2)
            {
                // coral-like growth: a knot of short branching fingers, not a lump
                for (int F = 0; F < 4; ++F)
                {
                    const double A = F * 1.5708 + R.Range(-0.4, 0.4), Lean = R.Range(0.08, 0.2), Tall = R.Range(0.2, 0.38);
                    Out.Tube(Coral, {X, Y, Z - 0.02}, {X + Lean * Cos(A), Y + Lean * Sin(A), Z + Tall}, 0.06, 0.02, 6);
                }
            }
            else Rock(Out, K % 2 ? Mat(M::Moss) : Mat(M::Mud), {X, Y, Z - 0.02}, R.Range(0.2, 0.36), R.Range(0.12, 0.2), 200u + K);
        }
        for (int K = 0; K < 10; ++K)
        {
            const double X = R.Range(-3.5, 2.8), Side = K % 2 ? 1.0 : -1.0;
            double Half = 0.0;
            const double Top = BackAt(Body, X, Half);
            const V3 From{X, Side * Half * 0.4, Top - 0.05}, Mid{X + R.Range(-0.2, 0.2), Side * (Half + 0.03), Top - 0.35}, To{X + R.Range(-0.3, 0.3), Side * Half * 0.97, Top - 0.7};
            Out.Tube(Vine, From, Mid, 0.035, 0.03, 6, false);
            Out.Tube(Vine, Mid, To, 0.03, 0.015, 6, false);
        }

        // the harpoon head eighty years in the right shoulder (-Y): a rusted shaft stub and its barbs at the skin
        const V3 Wound{2.5, -0.82, 1.15}, Stub{2.2, -1.25, 1.75};
        Out.Tube(Rust, Wound, Stub, 0.045, 0.04, 8);
        for (const double Barb : {-1.0, 1.0})
            Out.Tube(Rust, {Wound.X + 0.05, Wound.Y - 0.05, Wound.Z + 0.06}, {Wound.X + 0.05 + Barb * 0.16, Wound.Y - 0.18, Wound.Z + 0.22}, 0.025, 0.004, 5);
        Dome(Out, Blood, Wound.X, Wound.Y + 0.03, Wound.Z - 0.04, 0.16, 0.04, 10);
        return Out;
    }

    /**
     * The Jungle Warden (jungle_warden.md 2): four and a half metres, vaguely humanoid, built from twisted vines, ancient
     * stone slabs and weathered armour fragments over a skeleton of ironwood roots grown through stone artefacts; a carved
     * wooden mask with no eyes, only two black hollows; moss and small flowers on the back and shoulders; the left arm much
     * larger than the right; a hollow chest where a green light pulses; roots running from the feet into the floor.
     * Facing +X, standing on z = 0.
     */
    Mesh BuildJungleWarden()
    {
        Mesh Out("SM_Art_Creature_JungleWarden");
        const Material Wood = Hide("M_Art_Warden_Ironwood", "bark_brown_02", 0.44, 0.37, 0.3, 0.7, 0.95);
        const Material Root = Hide("M_Art_Warden_Root", "bark_brown_02", 0.27, 0.22, 0.17, 0.5, 0.95);
        const Material Stone = Hide("M_Art_Warden_Stone", "rustic_stone_wall_02", 0.66, 0.66, 0.58, 1.1, 1.0);
        const Material Armour = Hide("M_Art_Warden_Armour", "rusty_metal_02", 0.72, 0.62, 0.52, 0.7, 0.9);
        const Material Mask = Hide("M_Art_Warden_Mask", "rough_wood", 0.64, 0.5, 0.34, 0.5, 0.85);
        const Material Carving = Hide("M_Art_Warden_MaskCarving", "rough_wood", 0.42, 0.32, 0.21, 0.5, 0.85);
        const Material Hollow = Flesh("M_Art_Warden_Hollow", 0.012, 0.01, 0.01, 0.95);
        const Material Vine = Flesh("M_Art_Warden_Vine", 0.1, 0.15, 0.05, 0.85);
        Material Heart = Flesh("M_Art_Warden_Heart", 0.3, 0.72, 0.28, 0.35);
        Heart.Emissive = {0.38, 1.0, 0.32};
        Heart.EmissiveStrength = 8.0;

        // legs: root bundles, a slight stride; the feet's roots run out into the floor
        const std::vector<V3> LegL{{0.0, 0.3, 2.15}, {0.26, 0.42, 1.2}, {0.06, 0.44, 0.2}}, LegR{{0.0, -0.3, 2.15}, {0.1, -0.42, 1.15}, {-0.12, -0.44, 0.2}};
        Bundle(Out, Wood, LegL, 5, 0.13, 0.09, 0.1, 0.07, 0.7, 11u);
        Bundle(Out, Wood, LegR, 5, 0.12, 0.09, 0.1, 0.07, -0.6, 12u);
        Rng R(0x57A2u);
        for (const V3& Ankle : {LegL.back(), LegR.back()})
            for (int K = 0; K < 7; ++K)
            {
                const double A = 2.0 * Pi * K / 7 + R.Range(-0.25, 0.25), Reach = R.Range(0.55, 0.95);
                const V3 Mid{Ankle.X + 0.45 * Reach * Cos(A), Ankle.Y + 0.45 * Reach * Sin(A), 0.14};
                const V3 End{Ankle.X + Reach * Cos(A), Ankle.Y + Reach * Sin(A), -0.06};
                Out.Tube(Root, Ankle, Mid, 0.075, 0.05, 7);
                Out.Tube(Root, Mid, End, 0.05, 0.012, 6);
            }
        // a rusted iron band round the right thigh, a cracked greave on the left shin
        Out.Push(Xf::Translate({0.05, -0.36, 1.75}) * Xf::RotateX(0.12));
        Out.Tube(Armour, {0, 0, -0.14}, {0, 0, 0.14}, 0.27, 0.25, 12);
        Out.Pop();
        Slab(Out, Armour, {0.3, 0.44, 0.7}, {0.03, 0.17, 0.32}, 0.0, -0.18, 0.0, 51u, 0.3);

        // pelvis: a carved temple block the roots have grown round
        Slab(Out, Stone, {-0.06, 0.12, 2.12}, {0.26, 0.3, 0.16}, 0.25, 0.1, 0.06, 52u, 0.35);
        Slab(Out, Stone, {0.02, -0.26, 2.2}, {0.2, 0.2, 0.14}, -0.4, -0.12, 0.2, 53u, 0.35);
        Bundle(Out, Root, {{0.0, -0.55, 2.3}, {0.34, 0.0, 2.2}, {0.0, 0.55, 2.3}}, 3, 0.03, 0.03, 0.05, 0.05, 1.0, 13u);

        // the trunk: a dark core at the back, ribs of root round a hollow chest; the green light pulses in the hollow
        std::vector<Ring> Core;
        for (int K = 0; K <= 8; ++K)
        {
            const double T = K / 8.0, Z = 2.25 + 1.6 * T, Lean = 0.22 * T * T;
            Core.push_back(Ellipse(-0.22 + Lean, 0.0, Z, 0.24 + 0.06 * Sin(Pi * T), 0.42 + 0.14 * T, 12));
        }
        Out.Loft(Root, Core, true);
        Dome(Out, Heart, 0.06, 0.0, 2.95, 0.24, 0.24, 14);
        Out.Push(Xf::Translate({0.06, 0.0, 2.95}) * Xf::Scale({1.0, 1.0, -1.0}));
        Dome(Out, Heart, 0.0, 0.0, 0.0, 0.24, 0.24, 14);
        Out.Pop();
        Dome(Out, Hollow, -0.04, 0.0, 2.6, 0.36, 0.7, 12);
        for (int Rib = 0; Rib < 5; ++Rib)
        {
            const double Z = 2.55 + 0.24 * Rib, Lean = 0.22 * ((Z - 2.25) / 1.6) * ((Z - 2.25) / 1.6), Depth = 0.48 - 0.03 * Rib, Wide = 0.56 + 0.04 * Rib;
            for (const double Side : {1.0, -1.0})
            {
                std::vector<V3> Arc;
                for (int K = 0; K <= 6; ++K)
                {
                    const double A = Pi - (Pi - 0.42) * K / 6.0;  // from the spine round the flank to the edge of the hollow
                    Arc.push_back({-0.05 + Lean + Depth * Cos(A), Side * Wide * Sin(A), Z - 0.22 * (K / 6.0) * (K / 6.0)});
                }
                Bundle(Out, Wood, Arc, 2, 0.03, 0.02, 0.055, 0.035, 0.5, 20u + static_cast<std::uint32_t>(Rib) * 2u + (Side > 0 ? 1u : 0u));
            }
        }
        // spine and neck
        Bundle(Out, Wood, {{-0.25, 0.0, 2.2}, {-0.3, 0.0, 2.9}, {-0.1, 0.0, 3.55}, {0.22, 0.0, 3.92}}, 4, 0.1, 0.07, 0.08, 0.06, 0.8, 30u);
        // a breastplate fragment across the left chest, a tilted temple slab grown into the back
        Slab(Out, Armour, {0.33, 0.36, 3.18}, {0.035, 0.24, 0.3}, 0.35, -0.3, 0.12, 54u, 0.3);
        Slab(Out, Stone, {-0.46, 0.18, 3.35}, {0.1, 0.32, 0.36}, 0.12, -0.32, 0.18, 55u, 0.35);
        Slab(Out, Stone, {-0.44, -0.24, 2.95}, {0.09, 0.24, 0.28}, -0.1, -0.22, -0.25, 56u, 0.35);

        // shoulders: the left a stone slab heaved up by the larger arm, the right a rusted pauldron
        const V3 ShL{0.12, 0.78, 3.62}, ShR{0.1, -0.62, 3.5};
        Slab(Out, Stone, {ShL.X - 0.05, ShL.Y + 0.02, ShL.Z + 0.2}, {0.32, 0.26, 0.1}, 0.3, 0.15, -0.45, 57u, 0.35);
        Out.Push(Xf::Translate({ShR.X, ShR.Y - 0.05, ShR.Z + 0.02}) * Xf::RotateX(0.5) * Xf::Scale({1.0, 0.85, 0.7}));
        Dome(Out, Armour, 0, 0, 0, 0.3, 0.3, 14);
        Out.Pop();
        Bundle(Out, Wood, {{0.0, 0.0, 3.6}, ShL}, 4, 0.08, 0.12, 0.07, 0.09, 0.4, 31u);
        Bundle(Out, Wood, {{0.0, 0.0, 3.55}, ShR}, 3, 0.06, 0.07, 0.06, 0.06, -0.4, 32u);

        // the left arm, grown far heavier over the centuries, hangs to the knee; a stone artefact is caught in its forearm
        const std::vector<V3> ArmL{ShL, {0.38, 1.08, 2.62}, {0.6, 1.02, 1.42}, {0.66, 0.98, 1.0}};
        Bundle(Out, Wood, ArmL, 6, 0.17, 0.13, 0.11, 0.09, 0.9, 33u);
        Slab(Out, Stone, {0.5, 1.07, 2.0}, {0.14, 0.2, 0.24}, 0.6, -0.35, 0.4, 58u, 0.35);
        for (int F = 0; F < 5; ++F)
        {
            const double A = -0.9 + 0.45 * F;
            const V3 Knuckle{0.66 + 0.16 * Cos(A), 0.98 + 0.16 * Sin(A), 0.95}, Tip{0.72 + 0.4 * Cos(A), 0.98 + 0.4 * Sin(A), 0.42 + (F == 0 ? 0.25 : 0.0)};
            Out.Tube(Wood, ArmL.back(), Knuckle, 0.1, 0.08, 7);
            Out.Tube(Root, Knuckle, {(Knuckle.X + Tip.X) / 2 + 0.06, (Knuckle.Y + Tip.Y) / 2, (Knuckle.Z + Tip.Z) / 2 + 0.08}, 0.08, 0.06, 7);
            Out.Tube(Root, {(Knuckle.X + Tip.X) / 2 + 0.06, (Knuckle.Y + Tip.Y) / 2, (Knuckle.Z + Tip.Z) / 2 + 0.08}, Tip, 0.06, 0.02, 6);
        }
        // the right arm, a thin thing by comparison
        const std::vector<V3> ArmR{ShR, {0.22, -0.82, 2.72}, {0.48, -0.74, 2.08}, {0.56, -0.7, 1.86}};
        Bundle(Out, Wood, ArmR, 3, 0.06, 0.05, 0.065, 0.05, -0.8, 34u);
        for (int F = 0; F < 4; ++F)
        {
            const double A = -0.6 + 0.4 * F;
            Out.Tube(Root, ArmR.back(), {0.66 + 0.18 * Cos(A), -0.7 + 0.18 * Sin(A), 1.62}, 0.04, 0.015, 6);
        }

        // head: a knot of root behind the mask, roots swept back like a crown
        Rock(Out, Root, {0.3, 0.0, 3.86}, 0.3, 0.62, 41u);
        for (int K = 0; K < 9; ++K)
        {
            const double Y = -0.26 + 0.065 * K + R.Range(-0.02, 0.02), Long = R.Range(0.7, 1.15);
            Bundle(Out, Root, {{0.4, Y * 0.7, 4.34}, {0.12, Y * 1.2, 4.5 - 0.3 * std::abs(Y)}, {-0.2 * Long, Y * 1.45, 4.3 - 0.25 * Long}, {-0.32 * Long, Y * 1.5, 3.95 - 0.3 * Long}},
                   1, 0.0, 0.0, 0.045, 0.012, 0.0, 60u + static_cast<std::uint32_t>(K));
        }
        // the mask: carved wood, no eyes, two black hollows; a brow band, a nose ridge, a mouth slit and cheek grooves
        const V3 Face{0.58, 0.0, 4.16};
        const double Tall = 0.34, Wide = 0.25, Deep = 0.13;
        FrontCap(Out, Mask, Face, Tall, Wide, Deep, 18);
        const auto Surface = [&](const double Y, const double Z)
        {
            const double Q = 1.0 - (Y / Wide) * (Y / Wide) - ((Z - Face.Z) / Tall) * ((Z - Face.Z) / Tall);
            return V3{Face.X + Deep * std::sqrt(std::max(Q, 0.0)), Y, Z};
        };
        for (const double Y : {-0.1, 0.1}) FrontCap(Out, Hollow, Surface(Y, 4.2) + V3{-0.015, 0, 0}, 0.09, 0.055, 0.02, 12);
        Out.Tube(Carving, Surface(-0.2, 4.31) + V3{0.005, 0, 0}, Surface(0.0, 4.33) + V3{0.012, 0, 0}, 0.024, 0.026, 6);
        Out.Tube(Carving, Surface(0.0, 4.33) + V3{0.012, 0, 0}, Surface(0.2, 4.31) + V3{0.005, 0, 0}, 0.026, 0.024, 6);
        Out.Tube(Carving, Surface(0.0, 4.28) + V3{0.01, 0, 0}, Surface(0.0, 4.07) + V3{0.025, 0, 0}, 0.024, 0.034, 6);
        Out.Tube(Hollow, Surface(-0.075, 3.97), Surface(0.075, 3.97), 0.012, 0.012, 6);
        for (const double Y : {-0.15, 0.15})
        {
            for (const double Z : {4.08, 4.02, 3.96})
                Out.Tube(Carving, Surface(Y * 1.08, Z) + V3{0.003, 0, 0}, Surface(Y * 0.82, Z - 0.02) + V3{0.003, 0, 0}, 0.01, 0.01, 5);
        }

        // moss and small flowers on the back and shoulders; vines draped over them
        const Material Petal[] = {Flesh("M_Art_Warden_FlowerPale", 0.92, 0.88, 0.62, 0.6), Flesh("M_Art_Warden_FlowerWhite", 0.9, 0.9, 0.86, 0.6),
                                  Flesh("M_Art_Warden_FlowerViolet", 0.62, 0.48, 0.78, 0.6)};
        const V3 Beds[] = {{-0.6, 0.0, 3.72}, {-0.62, 0.35, 3.62}, {-0.6, -0.35, 3.6}, {0.05, 0.85, 3.98}, {0.12, -0.62, 3.78}, {-0.35, 0.1, 3.9}};
        int Seed = 0;
        for (const V3& Bed : Beds)
        {
            Rock(Out, Mat(M::Moss), Bed + V3{0, 0, -0.08}, R.Range(0.22, 0.3), R.Range(0.1, 0.16), 300u + static_cast<std::uint32_t>(Seed));
            for (int F = 0; F < 5; ++F)
            {
                const V3 Base = Bed + V3{R.Range(-0.18, 0.18), R.Range(-0.18, 0.18), 0.02}, Head = Base + V3{R.Range(-0.03, 0.03), R.Range(-0.03, 0.03), R.Range(0.08, 0.16)};
                Out.Tube(Vine, Base, Head, 0.008, 0.006, 4);
                Dome(Out, Petal[(Seed + F) % 3], Head.X, Head.Y, Head.Z - 0.005, 0.045, 0.018, 8);
            }
            ++Seed;
        }
        for (int K = 0; K < 8; ++K)
        {
            const double Side = K % 2 ? 1.0 : -1.0, X = R.Range(-0.4, 0.1);
            const V3 From{X, Side * R.Range(0.2, 0.5), 3.75}, Mid{X + 0.1, Side * R.Range(0.55, 0.75), R.Range(3.2, 3.45)}, To{X + R.Range(0.0, 0.25), Side * R.Range(0.5, 0.65), R.Range(2.4, 2.9)};
            Out.Tube(Vine, From, Mid, 0.03, 0.025, 6, false);
            Out.Tube(Vine, Mid, To, 0.025, 0.012, 6, false);
        }
        return Out;
    }

    /**
     * The Caldera Titan, Ibu's Grief (caldera_titan.md 2): fifteen metres, humanoid and feminine, asymmetrical; the left arm
     * cooled black basalt, the right a flowing river of active magma; a torso of cracked stone over glowing veins of molten
     * rock; a smooth stone mask in the manner of the Anak Gunung funeral masks, no eyes, two black hollows where tears of
     * molten rock drip; hair of obsidian glass strands. She rises from the molten rock (1): a skirt of cooled lava flow
     * falls from the hips. Facing +X, standing on z = 0.
     */
    Mesh BuildCalderaTitan()
    {
        Mesh Out("SM_Art_Creature_CalderaTitan");
        const Material Stone = Hide("M_Art_Titan_Stone", "rock_face_03", 0.36, 0.33, 0.31, 2.5, 1.0);
        const Material Flow = Hide("M_Art_Titan_LavaRock", "rock_face_03", 0.2, 0.17, 0.16, 2.0, 1.0);
        const Material Basalt = Hide("M_Art_Titan_Basalt", "rock_face_03", 0.1, 0.1, 0.11, 1.4, 0.75);
        const Material Crust = Hide("M_Art_Titan_Crust", "rock_pitted_mossy", 0.09, 0.07, 0.06, 0.8, 1.0);
        const Material MaskStone = Hide("M_Art_Titan_Mask", "painted_plaster_wall", 0.5, 0.46, 0.42, 1.5, 0.7);
        const Material Hollow = Flesh("M_Art_Titan_Hollow", 0.01, 0.008, 0.008, 0.95);
        Material Obsidian = Flesh("M_Art_Titan_Obsidian", 0.025, 0.022, 0.03, 0.06);
        Obsidian.Metallic = 0.2;
        Material Magma = Flesh("M_Art_Titan_Magma", 0.9, 0.3, 0.05, 0.5);
        Magma.Emissive = {1.0, 0.27, 0.03};
        Magma.EmissiveStrength = 5.0;
        Material Vein = Flesh("M_Art_Titan_Vein", 0.95, 0.38, 0.06, 0.5);
        Vein.Emissive = {1.0, 0.33, 0.04};
        Vein.EmissiveStrength = 4.0;
        Rng R(0xCA1Du);

        // the trunk: hips, a narrow waist, the chest, broad shoulders (forward offset, height, half depth, half width)
        struct Band { double X, Z, Rx, Ry; };
        const std::vector<Band> Trunk{{0.0, 7.6, 1.2, 1.7}, {0.05, 8.4, 1.25, 1.78}, {0.1, 9.4, 0.92, 1.18}, {0.12, 10.4, 1.12, 1.55},
                                      {0.14, 11.2, 1.28, 1.88}, {0.08, 12.0, 1.08, 2.1}, {0.04, 12.6, 0.72, 1.42}, {0.08, 12.95, 0.42, 0.5}};
        std::vector<Ring> Rings;
        for (const Band& B : Trunk) Rings.push_back(Ellipse(B.X, 0.0, B.Z, B.Rx, B.Ry, 20));
        Out.Loft(Stone, Rings, true);
        const auto OnTrunk = [&](const double A, const double Z, const double Lift)
        {
            std::size_t K = 0;
            while (K + 2 < Trunk.size() && Z > Trunk[K + 1].Z) ++K;
            const double T = Clamp((Z - Trunk[K].Z) / (Trunk[K + 1].Z - Trunk[K].Z), 0.0, 1.0);
            const double X = Lerp(Trunk[K].X, Trunk[K + 1].X, T), Rx = Lerp(Trunk[K].Rx, Trunk[K + 1].Rx, T), Ry = Lerp(Trunk[K].Ry, Trunk[K + 1].Ry, T);
            return V3{X + (Rx + Lift) * Cos(A), (Ry + Lift) * Sin(A), Z};
        };
        // the breast, modest as a carved figure's
        for (const double Y : {-0.72, 0.72}) FrontCap(Out, Stone, {0.95, Y, 11.0}, 0.62, 0.6, 0.42, 14);

        // cracks with molten rock beneath: veins wandering down the stone, three wide fissures across the chest
        for (int K = 0; K < 34; ++K)
        {
            double A = R.Range(0.0, 2.0 * Pi), Z = R.Range(8.0, 12.4);
            const double Width = K < 3 ? 0.13 : R.Range(0.035, 0.07);
            if (K < 3) { A = R.Range(-0.5, 0.5); Z = R.Range(10.6, 11.8); }
            V3 Prev = OnTrunk(A, Z, 0.0);
            for (int Step = 0; Step < 9; ++Step)
            {
                A += R.Range(-0.14, 0.14);
                Z += R.Range(-0.42, 0.06);
                if (Z < 7.7) break;
                const V3 Next = OnTrunk(A, Z, 0.0);
                Out.Tube(Vein, Prev, Next, Width * (1.0 - Step * 0.08), Width * (1.0 - (Step + 1) * 0.08), 5);
                Prev = Next;
            }
        }

        // the skirt of cooled lava flow, its hem in heavy folds; molten seams run down it
        std::vector<Ring> Skirt;
        for (int K = 0; K <= 8; ++K)
        {
            const double T = K / 8.0, Z = Lerp(8.5, 0.4, T), Fold = 0.06 + 0.3 * T * T;
            Ring Rg;
            for (int S = 0; S < 32; ++S)
            {
                const double A = 2.0 * Pi * S / 32, Wave = 1.0 + Fold * Sin(5.0 * A + 0.6 * T);
                Rg.push_back({0.05 + Lerp(1.3, 2.7, T) * Wave * Cos(A), Lerp(1.82, 3.0, T) * Wave * Sin(A), Z});
            }
            Skirt.push_back(Rg);
        }
        Out.Loft(Flow, Skirt, true);
        for (int K = 0; K < 12; ++K)
        {
            double A = 2.0 * Pi * K / 12 + R.Range(-0.25, 0.25);
            const double Start = R.Range(0.0, 0.35), Stop = R.Range(0.55, 0.95), Width = R.Range(0.025, 0.06);
            V3 Prev{};
            for (int Step = 0; Step <= 12; ++Step)
            {
                const double T = Lerp(Start, Stop, Step / 12.0), Z = Lerp(8.5, 0.4, T), Fold = 0.06 + 0.3 * T * T;
                A += R.Range(-0.07, 0.07);
                const double Wave = 1.0 + Fold * Sin(5.0 * A + 0.6 * T);
                const V3 P{0.05 + Lerp(1.3, 2.7, T) * Wave * Cos(A) * 1.005, Lerp(1.82, 3.0, T) * Wave * Sin(A) * 1.005, Z};
                if (Step && R.Unit() > 0.18) Out.Tube(Vein, Prev, P, Width, Width * R.Range(0.6, 1.3), 5);
                Prev = P;
            }
        }
        for (const double Y : {-0.95, 0.95}) Rock(Out, Basalt, {2.6, Y, -0.1}, 0.75, 0.75, Y > 0 ? 71u : 72u);

        // the left arm: cooled basalt, a sheaf of hexagonal columns to a hand of columns
        const V3 ShL{0.15, 2.2, 12.2}, ShR{0.15, -2.2, 12.2};
        const std::vector<V3> ArmL{ShL, {0.7, 3.0, 9.4}, {1.7, 2.9, 7.3}, {2.1, 2.75, 6.6}};
        Bundle(Out, Basalt, ArmL, 7, 0.34, 0.26, 0.26, 0.2, 0.06, 73u, 6);
        Rock(Out, Basalt, ShL + V3{-0.1, 0.1, -0.45}, 0.85, 1.0, 74u);
        for (int F = 0; F < 5; ++F)
        {
            const double A = -0.8 + 0.4 * F;
            const V3 Knuckle{2.25 + 0.3 * Cos(A), 2.75 + 0.3 * Sin(A), 6.45}, Tip{2.5 + 0.75 * Cos(A), 2.75 + 0.75 * Sin(A), (F == 0 ? 6.2 : 5.55)};
            Out.Tube(Basalt, ArmL.back(), Knuckle, 0.2, 0.17, 6);
            Out.Tube(Basalt, Knuckle, Tip, 0.16, 0.1, 6);
        }
        // the right arm: a river of magma reshaping itself, crusts of cooling rock riding on it, dripping to the ground
        const std::vector<V3> ArmR{ShR, {1.0, -3.15, 9.6}, {2.4, -3.0, 8.0}, {2.9, -2.8, 7.3}};
        Bundle(Out, Magma, ArmR, 5, 0.3, 0.24, 0.28, 0.2, 1.6, 75u);
        for (int K = 0; K < 22; ++K)
        {
            const V3 P = Along(ArmR, R.Range(0.03, 0.95));
            Rock(Out, Crust, P + V3{R.Range(-0.32, 0.32), R.Range(-0.32, 0.32), -0.12}, R.Range(0.2, 0.36), R.Range(0.14, 0.26), 80u + static_cast<std::uint32_t>(K));
        }
        for (int F = 0; F < 4; ++F)
        {
            const double A = -0.7 + 0.45 * F;
            Out.Tube(Magma, ArmR.back(), {3.15 + 0.55 * Cos(A), -2.8 + 0.55 * Sin(A), 6.7 - 0.15 * F}, 0.17, 0.06, 7);
        }
        for (int D = 0; D < 6; ++D)
        {
            const double X = 3.0 + R.Range(-0.35, 0.35), Y = -2.85 + R.Range(-0.35, 0.35), Top = R.Range(4.5, 6.6);
            Out.Tube(Magma, {X, Y, Top}, {X, Y, Top - R.Range(0.5, 1.4)}, 0.05, 0.11, 6);
        }
        Rock(Out, Crust, {3.0, -2.85, -0.12}, 1.0, 0.22, 79u);
        Dome(Out, Magma, 3.0, -2.85, 0.02, 0.6, 0.1, 14);
        Rock(Out, Basalt, ShR + V3{-0.1, -0.1, -0.45}, 0.8, 0.9, 76u);

        // neck and head; the funeral mask, two black hollows, molten tears
        Out.Tube(Stone, {0.1, 0.0, 12.8}, {0.22, 0.0, 13.6}, 0.46, 0.42, 14);
        Rock(Out, Stone, {0.1, 0.0, 13.1}, 0.9, 1.9, 77u);
        const V3 Face{0.72, 0.0, 14.0};
        const double Tall = 0.98, Wide = 0.72, Deep = 0.42;
        FrontCap(Out, MaskStone, Face, Tall, Wide, Deep, 20);
        const auto Surface = [&](const double Y, const double Z)
        {
            const double Q = 1.0 - (Y / Wide) * (Y / Wide) - ((Z - Face.Z) / Tall) * ((Z - Face.Z) / Tall);
            return V3{Face.X + Deep * std::sqrt(std::max(Q, 0.0)), Y, Z};
        };
        for (const double Y : {-0.27, 0.27})
        {
            FrontCap(Out, Hollow, Surface(Y, 14.2) + V3{-0.035, 0, 0}, 0.24, 0.16, 0.045, 12);
            V3 Prev = Surface(Y * 1.02, 13.98) + V3{0.012, 0, 0};
            for (const double Z : {13.8, 13.62, 13.44})
            {
                const V3 Next = Surface(Y * (1.0 + (14.0 - Z) * 0.1), Z) + V3{0.012, 0, 0};
                Out.Tube(Magma, Prev, Next, 0.02, 0.016, 6);
                Prev = Next;
            }
            Dome(Out, Magma, Prev.X, Prev.Y, Prev.Z - 0.04, 0.03, 0.05, 8);
        }
        // the carving: a brow band, the ridge of the nose, the closed mouth
        Out.Tube(Stone, Surface(-0.55, 14.5) + V3{0.01, 0, 0}, Surface(0.0, 14.56) + V3{0.03, 0, 0}, 0.04, 0.05, 6);
        Out.Tube(Stone, Surface(0.0, 14.56) + V3{0.03, 0, 0}, Surface(0.55, 14.5) + V3{0.01, 0, 0}, 0.05, 0.04, 6);
        Out.Tube(Stone, Surface(0.0, 14.42) + V3{0.02, 0, 0}, Surface(0.0, 13.92) + V3{0.06, 0, 0}, 0.05, 0.07, 6);
        Out.Tube(Hollow, Surface(-0.2, 13.62), Surface(0.2, 13.62), 0.022, 0.022, 6);

        // hair: strands of obsidian glass from the crown down the back, shards broken along them
        for (int K = 0; K < 44; ++K)
        {
            const double Y = R.Range(-0.8, 0.8), Long = R.Range(0.7, 1.2), Lift = R.Range(0.0, 0.25);
            const std::vector<V3> Strand{{0.55, Y * 0.7, 14.75 + Lift * 0.4}, {-0.15, Y * 1.0, 15.0 + Lift}, {-0.95, Y * 1.3, 14.1}, {-1.25, Y * 1.55, 12.6 - 0.6 * Long}, {-1.15, Y * 1.7, 11.6 - 1.6 * Long}};
            Bundle(Out, Obsidian, Strand, 1, 0.0, 0.0, 0.07, 0.012, 0.0, 90u + static_cast<std::uint32_t>(K), 5);
            if (K % 4 == 0)
            {
                const V3 At = Along(Strand, R.Range(0.45, 0.85));
                Slab(Out, Obsidian, At + V3{-0.08, 0, 0}, {0.03, 0.09, 0.26}, R.Range(-0.6, 0.6), R.Range(-0.4, 0.4), R.Range(-0.4, 0.4), 140u + static_cast<std::uint32_t>(K), 0.5);
            }
        }
        return Out;
    }

    /**
     * Sturmkralle, the Sky Terror (sturmkralle_sky_terror.md 2): a massive raptor, part eagle, part condor, part something
     * older; eight metres tall standing, twenty metres across the wings; blue-black feathers streaked with silver, every
     * one sharpened; talons the length of a man's forearm, curved and golden; amber eyes. The storm clouds at her wing tips
     * and the lightning between her primaries are effects of the fight, not of the model. Facing +X, wings spread, standing
     * on z = 0.
     */
    Mesh BuildSturmkralle()
    {
        Mesh Out("SM_Art_Creature_Sturmkralle");
        const Material Down = Flesh("M_Art_Sturm_Down", 0.05, 0.06, 0.09, 0.7);
        const Material Black = Flesh("M_Art_Sturm_Feather", 0.07, 0.08, 0.12, 0.38);
        const Material Silver = Flesh("M_Art_Sturm_FeatherSilver", 0.5, 0.54, 0.6, 0.3);
        const Material Streak = Flesh("M_Art_Sturm_FeatherStreak", 0.2, 0.23, 0.3, 0.34);
        const Material Scale = Hide("M_Art_Sturm_Scale", "rock_pitted_mossy", 0.45, 0.42, 0.3, 0.25, 0.8);
        const Material Horn = Flesh("M_Art_Sturm_Beak", 0.16, 0.16, 0.18, 0.32);
        Material Gold = Flesh("M_Art_Sturm_Talon", 0.86, 0.66, 0.28, 0.28);
        Gold.Metallic = 0.85;
        Material Eye = Flesh("M_Art_Sturm_Eye", 1.0, 0.62, 0.12, 0.15);
        Eye.Emissive = {1.0, 0.58, 0.1};
        Eye.EmissiveStrength = 2.5;
        Rng R(0x5778u);
        const auto Plume = [&]() -> const Material& { const double U = R.Unit(); return U < 0.14 ? Silver : U < 0.36 ? Streak : Black; };

        // the body, tilted upright from the hips; laid over with sharpened feathers pointing to the tail
        const V3 Hip{-0.3, 0.0, 3.6};
        const double Tilt = -0.75;
        const std::vector<Section> Body{{-2.7, 0.35, 0.3, 0, 1}, {-2.0, 0.9, 0.8, 0, 1}, {-1.0, 1.25, 1.1, 0, 1}, {0.0, 1.35, 1.2, 0, 1},
                                        {1.0, 1.3, 1.15, 0.05, 1}, {2.0, 1.0, 0.95, 0.1, 1}, {2.7, 0.72, 0.72, 0.1, 1}};
        Out.Push(Xf::Translate(Hip) * Xf::RotateY(Tilt));
        LoftBody(Out, Down, Down, Down, Body, 16);
        int Row = 0;
        for (double X = -2.5; X <= 2.55; X += 0.3, ++Row)
        {
            double Half = 0.0;
            BackAt(Body, X, Half);
            std::size_t K = 0;
            while (K + 2 < Body.size() && X > Body[K + 1].X) ++K;
            const double T = Clamp((X - Body[K].X) / (Body[K + 1].X - Body[K].X), 0.0, 1.0);
            const double Hh = Lerp(Body[K].HalfHeight, Body[K + 1].HalfHeight, T), Zc = Lerp(Body[K].Z, Body[K + 1].Z, T);
            const int Count = std::max(8, static_cast<int>(Pi * (Half + Hh) / 0.3));
            for (int F = 0; F < Count; ++F)
            {
                const double A = 2.0 * Pi * (F + 0.5 * (Row % 2)) / Count;
                const V3 N = Normalize({0.0, -Cos(A) / Half, Sin(A) / Hh});
                const V3 P{X, -Half * Cos(A), Zc + Hh * Sin(A)};
                Feather(Out, Plume(), P + N * 0.02, Normalize(V3{-1.0, 0, 0} + N * 0.12), N, 0.95, 0.4);
            }
        }
        // the tail: a fan of long blades
        for (int F = 0; F < 11; ++F)
        {
            const double Spread = -0.5 + F / 10.0;
            Feather(Out, F % 4 == 1 ? Silver : Black, {-2.6, Spread * 0.5, 0.05 * (F % 2)}, Normalize({-1.0, Spread * 0.7, -0.15}), {0, 0, 1}, 3.0 - 0.6 * std::abs(Spread), 0.55);
        }
        Out.Pop();

        // neck and its ruff, the condor in her
        const V3 Neck0{1.45, 0.0, 5.2}, Neck1{1.78, 0.0, 6.9};
        Out.Tube(Down, Neck0, Neck1, 0.78, 0.5, 14);
        for (int Ring = 0; Ring < 3; ++Ring)
            for (int F = 0; F < 22; ++F)
            {
                const double A = 2.0 * Pi * (F + 0.5 * Ring) / 22, Z = 5.6 + 0.32 * Ring;
                const V3 Out1{Cos(A), Sin(A), 0.0};
                Feather(Out, Ring == 2 ? Silver : (F % 3 ? Streak : Silver), V3{1.5 + 0.06 * Ring, 0.0, Z} + Out1 * (0.72 - 0.08 * Ring), Normalize(Out1 + V3{0, 0, -0.9}), Out1, 1.1 - 0.15 * Ring, 0.36);
            }

        for (int Ring = 0; Ring < 3; ++Ring)
            for (int F = 0; F < 18; ++F)
            {
                const double A = 2.0 * Pi * (F + 0.5 * Ring) / 18, T = 0.45 + 0.2 * Ring;
                const V3 Axis = Neck0 + (Neck1 - Neck0) * T, N{Cos(A), Sin(A), 0.0};
                Feather(Out, Plume(), Axis + N * Lerp(0.78, 0.5, T), Normalize(N * 0.35 + V3{0, 0, -1}), N, 0.7, 0.32);
            }
        // the head: an eagle's skull, a hooked beak, a hard brow over amber eyes, a crest of blades swept back
        const V3 Head{1.9, 0.0, 7.35};
        Ellipsoid(Out, Down, Head, 0.72, 0.5, 0.55, 16);
        Out.Tube(Horn, {2.45, 0.0, 7.36}, {2.95, 0.0, 7.3}, 0.24, 0.17, 12);
        Out.Tube(Horn, {2.95, 0.0, 7.3}, {3.2, 0.0, 7.12}, 0.17, 0.1, 12);
        Out.Tube(Horn, {3.2, 0.0, 7.12}, {3.2, 0.0, 6.86}, 0.1, 0.015, 10);
        Out.Tube(Horn, {2.45, 0.0, 7.18}, {2.92, 0.0, 7.1}, 0.16, 0.07, 10);
        for (const double S : {-1.0, 1.0})
        {
            Ellipsoid(Out, Eye, {2.26, S * 0.37, 7.47}, 0.07, 0.05, 0.08, 10);
            Out.Tube(Black, {2.02, S * 0.45, 7.62}, {2.44, S * 0.3, 7.6}, 0.09, 0.05, 8);
        }
        for (int F = 0; F < 12; ++F)
        {
            const double Y = -0.36 + 0.065 * F, Up = 1.0 - std::abs(Y) * 1.6;
            Feather(Out, Plume(), {2.0, Y, 7.68 + 0.12 * Up}, Normalize({-1.0, Y * 0.6, 0.25 * Up}), {0, 0, 1}, 0.9 + 0.3 * Up, 0.22);
        }
        for (const double Dx : {0.35, 0.1, -0.2, -0.45})
            for (int F = 0; F < 18; ++F)
            {
                const double A = 2.0 * Pi * (F + (Dx > 0.0 ? 0.0 : 0.5)) / 18, Shrink = std::sqrt(std::max(1.0 - (Dx / 0.72) * (Dx / 0.72), 0.1));
                const V3 N{0.0, Cos(A), Sin(A)};
                if (Dx > 0.2 && N.Z < -0.3) continue;  // the throat under the beak stays clear
                Feather(Out, Plume(), Head + V3{Dx, N.Y * 0.5 * Shrink, N.Z * 0.55 * Shrink}, Normalize({-1.0, N.Y * 0.25, N.Z * 0.25}), N, 0.5, 0.22);
            }

        // the wings, spread: a feathered leading edge; primaries off the hand, secondaries off the forearm, coverts over them
        for (const double S : {-1.0, 1.0})
        {
            const V3 Sh{0.75, S * 1.05, 5.25}, El{0.1, S * 3.45, 6.15}, Wr{0.5, S * 5.9, 6.55}, Tp{0.25, S * 7.4, 6.85};  // span 20 m with the primaries
            Out.Tube(Down, Sh, El, 0.28, 0.17, 10);
            Out.Tube(Down, El, Wr, 0.17, 0.11, 10);
            Out.Tube(Down, Wr, Tp, 0.11, 0.05, 8);
            const V3 Up{0, 0, 1};
            for (int F = 0; F < 11; ++F)
            {
                const double T = F / 10.0;
                const V3 Root = Wr + (Tp - Wr) * (0.08 + 0.86 * T);
                Feather(Out, F % 5 == 2 ? Silver : Plume(), Root + V3{0, 0, -0.02 * F}, Normalize({-Lerp(1.0, 0.3, T), S * Lerp(0.25, 1.0, T), -0.03}), Up, Lerp(2.5, 2.9, T), 0.74);
            }
            const std::vector<V3> Arm{Sh, El, Wr};
            for (int F = 0; F < 16; ++F)
            {
                const double T = F / 15.0;
                const V3 Root = Along(Arm, T);
                Feather(Out, Plume(), Root + V3{0, 0, -0.03}, Normalize({-1.0, S * 0.12 * T, -0.04}), Up, Lerp(2.3, 2.8, T), 0.8);
                Feather(Out, Plume(), Root + V3{0.12, 0, 0.06}, Normalize({-1.0, S * 0.1, -0.02}), Up, 1.35, 0.58);
                Feather(Out, Plume(), Root + V3{0.25, 0, 0.12}, Normalize({-1.0, S * 0.08, 0.0}), Up, 0.75, 0.46);
            }
            // the leading edge feathered over: marginal coverts laid along the top of the arm
            for (int F = 0; F < 30; ++F)
            {
                const double T = F / 29.0;
                const V3 Root = Along({Sh, El, Wr, Tp}, T);
                const double Radius = T < 1.0 / 3 ? Lerp(0.28, 0.17, T * 3) : T < 2.0 / 3 ? Lerp(0.17, 0.11, T * 3 - 1) : Lerp(0.11, 0.05, T * 3 - 2);
                Feather(Out, Plume(), Root + V3{Radius * 1.1, 0, Radius * 0.6}, Normalize({-1.0, S * 0.15, -0.12}), Normalize({0.5, 0, 1}), 0.75 + Radius, 0.46);
            }
            for (int F = 0; F < 6; ++F)
            {
                const V3 Root = Wr + (Tp - Wr) * (F / 6.0);
                Feather(Out, Plume(), Root + V3{0.12, 0, 0.06}, Normalize({-0.9, S * 0.45, -0.02}), Up, 1.2, 0.36);
            }
        }

        // legs: feathered thighs, scaled shanks, four toes each with a golden talon a forearm long
        for (const double S : {-1.0, 1.0})
        {
            const V3 Thigh0{-0.35, S * 0.78, 3.35}, Knee{0.0, S * 0.82, 2.0}, Foot{0.22, S * 0.82, 0.42};
            Out.Tube(Down, Thigh0, Knee, 0.62, 0.32, 12);
            for (int F = 0; F < 16; ++F)
            {
                const double A = 2.0 * Pi * F / 16, Z = 3.0 - 0.35 * (F % 3);
                const V3 N{Cos(A), Sin(A), 0.0};
                Feather(Out, Plume(), V3{-0.25, S * 0.8, Z} + N * 0.55, Normalize(V3{0, 0, -1} + N * 0.25), N, 1.0, 0.34);
            }
            Out.Tube(Scale, Knee, Foot, 0.22, 0.17, 10);
            const V3 Toes[] = {{0.95, S * 0.42, -0.24}, {1.0, 0.0, -0.24}, {0.9, -S * 0.36, -0.24}, {-0.75, 0.0, -0.24}};
            for (const V3& Toe : Toes)
            {
                const V3 Tip = Foot + Toe, Dir = Normalize({Toe.X, Toe.Y, 0.0});
                Out.Tube(Scale, Foot, Tip, 0.15, 0.1, 8);
                const V3 C1 = Tip + Dir * 0.2 + V3{0, 0, 0.06}, C2 = C1 + Dir * 0.16 + V3{0, 0, -0.16}, C3 = C2 + Dir * 0.03 + V3{0, 0, -0.17};
                Out.Tube(Gold, Tip, C1, 0.085, 0.07, 8);
                Out.Tube(Gold, C1, C2, 0.07, 0.04, 8);
                Out.Tube(Gold, C2, C3, 0.04, 0.006, 8);
            }
        }
        return Out;
    }

    void Kit::BuildCreatureModels()
    {
        Emit("SM_Art_Creature_JawOfTheMire", WriteGltf(BuildJawOfTheMire(), "Dark Arisen art kit (creatures)"));
        Emit("SM_Art_Creature_JungleWarden", WriteGltf(BuildJungleWarden(), "Dark Arisen art kit (creatures)"));
        Emit("SM_Art_Creature_CalderaTitan", WriteGltf(BuildCalderaTitan(), "Dark Arisen art kit (creatures)"));
        Emit("SM_Art_Creature_Sturmkralle", WriteGltf(BuildSturmkralle(), "Dark Arisen art kit (creatures)"));
    }
}
