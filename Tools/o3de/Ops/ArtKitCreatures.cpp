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

    void Kit::BuildCreatureModels()
    {
        Emit("SM_Art_Creature_JawOfTheMire", WriteGltf(BuildJawOfTheMire(), "Dark Arisen art kit (creatures)"));
    }
}
