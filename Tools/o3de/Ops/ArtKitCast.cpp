// The cast of Dark Arisen as the human pipeline builds them. Appearance follows the character
// documents (docs/characters/jake_harlow.md, docs/design/npcs/named_crew_deep_dives.md,
// docs/design/bosses/draven_voss.md, the Higgsfield catalogue subjects). Where a document is
// silent the choice is a period-plausible default for an 1830s Caribbean-like maritime setting,
// marked "default" below, and open to the owner.

#include "ArtKitHumanInternal.h"

#include <map>

namespace DarkArisen::Tools::Art::Humans
{
    namespace
    {
        // Palette (sRGB).
        constexpr Rgb Linen{0.86, 0.82, 0.72}, LinenDirty{0.76, 0.71, 0.6}, White{0.88, 0.87, 0.82}, Black{0.07, 0.07, 0.07};
        constexpr Rgb Navy{0.1, 0.13, 0.25}, Crimson{0.5, 0.08, 0.07}, Madder{0.55, 0.16, 0.12};
        constexpr Rgb Leather{0.34, 0.21, 0.12}, LeatherDark{0.17, 0.11, 0.08}, Canvas{0.6, 0.54, 0.42}, CanvasDark{0.4, 0.35, 0.27};
        constexpr Rgb Brass{0.74, 0.58, 0.3}, Gold{0.66, 0.52, 0.24}, Bone{0.86, 0.82, 0.72}, Indigo{0.2, 0.22, 0.36};
        constexpr Rgb Ochre{0.64, 0.49, 0.24}, Olive{0.34, 0.34, 0.22}, WoolBrown{0.3, 0.22, 0.15}, WoolGrey{0.4, 0.4, 0.4};

        // Skin (sRGB albedo).
        constexpr Rgb Pale{0.88, 0.73, 0.64}, Fair{0.83, 0.64, 0.52}, Tanned{0.76, 0.56, 0.42}, OliveSkin{0.72, 0.54, 0.4};
        constexpr Rgb Brown{0.52, 0.35, 0.24}, DarkBrown{0.36, 0.23, 0.16}, Mahogany{0.48, 0.3, 0.2}, Ruddy{0.82, 0.58, 0.46};

        // Eyes and hair.
        constexpr Rgb GreenEyes{0.3, 0.44, 0.22}, GreyBlue{0.5, 0.58, 0.66}, IceBlue{0.56, 0.7, 0.82}, BrownEyes{0.3, 0.18, 0.09};
        constexpr Rgb DarkEyes{0.18, 0.11, 0.06}, PaleBlue{0.52, 0.62, 0.74};
        constexpr Rgb HairDarkBrown{0.13, 0.085, 0.055}, HairBlack{0.05, 0.045, 0.04}, HairGreyBrown{0.36, 0.3, 0.25}, HairWhite{0.84, 0.82, 0.78};
        constexpr Rgb HairIron{0.46, 0.45, 0.43}, HairBrown{0.24, 0.16, 0.1};

        Garment Top(const Rgb& C, const Cloth F, const double Sleeve, const double Neck)
        {
            Garment G;
            G.On = true;
            G.Color = C;
            G.Fabric = F;
            G.Sleeve = Sleeve;
            G.NeckFront = Neck;
            G.Hem = -0.15;
            G.Thick = 0.09;
            return G;
        }
        Garment Vest(const Rgb& C, const Cloth F, const int Buttons)
        {
            Garment G;
            G.On = true;
            G.Color = C;
            G.Fabric = F;
            G.Sleeve = 0.0;
            G.NeckFront = 1.3;
            G.Hem = 0.14;
            G.Buttons = Buttons;
            G.Thick = 0.1;
            G.Wear = 0.3;
            return G;
        }
        Garment Coat(const Rgb& C, const Cloth F, const double Open, const int Buttons)
        {
            Garment G;
            G.On = true;
            G.Color = C;
            G.Fabric = F;
            G.Sleeve = 1.93;
            G.NeckFront = 1.1;
            G.Hem = -0.05;
            G.Open = Open;
            G.Buttons = Buttons;
            G.Thick = 0.12;
            G.Wear = 0.35;
            return G;
        }
        Garment Trousers(const Rgb& C, const Cloth F, const double LegEnd)
        {
            Garment G;
            G.On = true;
            G.Color = C;
            G.Fabric = F;
            G.LegEnd = LegEnd;
            G.Waist = 0.3;
            G.Thick = 0.13;
            return G;
        }
        Garment Boots(const Rgb& C, const double Top, const double Roughness = 0.5)
        {
            Garment G;
            G.On = true;
            G.Color = C;
            G.Fabric = Cloth::Leather;
            G.LegStart = Top;
            G.Thick = 0.1;
            G.Roughness = Roughness;
            G.Wear = 0.25;
            return G;
        }
        Garment Stockings(const Rgb& C)
        {
            Garment G;
            G.On = true;
            G.Color = C;
            G.Fabric = Cloth::Cotton;
            G.LegStart = 0.92;
            G.Thick = 0.03;
            G.Wear = 0.2;
            return G;
        }
        Skirt Tails(const Rgb& C, const Cloth F, const double Hem, const double Open)
        {
            Skirt S;
            S.On = true;
            S.Color = C;
            S.Fabric = F;
            S.From = 0.18;
            S.Hem = Hem;
            S.OpenFront = Open;
            S.Flare = 0.12;
            S.Thick = 0.12;
            return S;
        }
        Skirt LongSkirt(const Rgb& C, const Cloth F, const double Hem)
        {
            Skirt S;
            S.On = true;
            S.Color = C;
            S.Fabric = F;
            S.From = 0.3;
            S.Hem = Hem;
            S.Flare = 0.22;
            S.Thick = 0.1;
            return S;
        }
        Band Belt(const Rgb& C)
        {
            Band B;
            B.On = true;
            B.Color = C;
            B.At = 0.25;
            return B;
        }
        Band Sash(const Rgb& C, const bool Knot)
        {
            Band B;
            B.On = true;
            B.Color = C;
            B.Height = 0.8;
            B.At = 0.27;
            B.Buckle = false;
            B.Knot = Knot;
            return B;
        }

        std::map<std::string, PersonSpec, std::less<>> BuildCast()
        {
            std::map<std::string, PersonSpec, std::less<>> Cast;
            const auto Add = [&Cast](PersonSpec P) { Cast[P.Id] = std::move(P); };

            // ---- Jake Harlow: 17, 1.75 m, slim but sinewy, dark brown hair falling into his face, green eyes (jake_harlow.md).
            {
                PersonSpec P;
                P.Id = "jake";
                P.Age = 25; P.Muscle = 0.62; P.Weight = 0.36; P.Proportions = 0.9; P.Height = 1.75;
                P.Detail = {{"head/head-age-decr", 0.7}, {"chin/chin-width-decr", 0.2}, {"head/head-oval", 0.4}, {"nose/nose-scale-horiz-decr", 0.2},
                    {"mouth/mouth-upperlip-volume-incr", 0.2}, {"torso/torso-vshape-incr", 0.2}};
                P.Skin = Fair; P.Weathered = 0.35; P.Eyes = GreenEyes;
                P.Hair = HairDarkBrown; P.Hairdo = HairStyle::Tousled; P.HairVolume = 1.15;
                P.Shirt = Top(Linen, Cloth::Linen, 1.55, 0.9);
                P.Shirt.Wear = 0.55;
                P.Vest = Vest(Rgb{0.24, 0.21, 0.15}, Cloth::Wool, 5);
                P.Vest.ButtonColor = Brass;
                P.Legs = Trousers(CanvasDark, Cloth::Canvas, 1.9);
                P.Feet = Boots(LeatherDark, 1.35);
                P.Belt = Belt(LeatherDark);
                P.Cutlass = true;
                P.Clip = "18_08"; P.Time = 14.4; P.Upright = 0.5;
                Add(P);
            }
            // ---- Ethan Harlow: young adult navigator, lean and exhausted, family resemblance; salt-stained linen, worn waistcoat (ethan_harlow.md).
            {
                PersonSpec P;
                P.Id = "ethan";
                P.Age = 25; P.Muscle = 0.55; P.Weight = 0.3; P.Proportions = 0.85; P.Height = 1.8;
                P.Detail = {{"head/head-age-decr", 0.3}, {"chin/chin-prominent-incr", 0.3}, {"nose/nose-hump-incr", 0.3}, {"head/head-square", 0.3}};
                P.Skin = Fair; P.Weathered = 0.5; P.Eyes = GreenEyes;
                P.Hair = HairDarkBrown; P.Hairdo = HairStyle::Short; P.Beard = BeardStyle::Stubble;
                P.Shirt = Top(LinenDirty, Cloth::Linen, 1.9, 0.7);
                P.Shirt.Wear = 0.85;
                P.Vest = Vest(Rgb{0.3, 0.33, 0.36}, Cloth::Wool, 5);
                P.Vest.Wear = 0.7;
                P.Legs = Trousers(Rgb{0.3, 0.27, 0.22}, Cloth::Wool, 1.9);
                P.Feet = Boots(Leather, 1.5);
                P.Scars = {{V3{0.1, -0.95, 0.2}, V3{0.25, -1.0, 0.1}, 0.02, true}};
                P.Clip = "26_10"; P.Time = 5.8; P.Upright = 0.4;
                Add(P);
                PersonSpec Dream = P;
                Dream.Id = "dream_ethan";
                Dream.Uncanny = true;
                Dream.Clip = "13_33"; Dream.Time = 12.4; Dream.Upright = 0.6;
                Add(Dream);
            }
            // ---- Mira: 32, 175 cm, lean, strong, weathered; long dark braid with salt-streaked grey; pale grey-blue eyes;
            // scar through the right eyebrow; practical layers, bone buttons, her brother's worn leather coat (named_crew_deep_dives.md 2.1).
            {
                PersonSpec P;
                P.Id = "mira";
                P.Male = 0.0; P.Age = 32; P.Muscle = 0.66; P.Weight = 0.4; P.Proportions = 0.85; P.Height = 1.75; P.BreastSize = 0.42;
                P.Detail = {{"head/head-oval", 0.4}, {"cheek/l-cheek-bones-incr", 0.5}, {"cheek/r-cheek-bones-incr", 0.5}, {"nose/nose-scale-vert-incr", 0.2},
                    {"chin/chin-prominent-incr", 0.2}, {"eyebrows/eyebrows-angle-up", 0.2}};
                P.Skin = Tanned; P.Weathered = 0.5; P.Eyes = GreyBlue;
                P.Hair = HairDarkBrown; P.Gray = 0.18; P.Hairdo = HairStyle::Braid; P.HairLength = 4.5;
                P.Scars = {{V3{-0.34, 0.16, 0.12}, V3{-0.27, 0.42, 0.12}, 0.022, true}};
                P.Shirt = Top(Rgb{0.8, 0.76, 0.66}, Cloth::Linen, 1.85, 0.7);
                P.Vest = Vest(WoolGrey, Cloth::Wool, 5);
                P.Vest.ButtonColor = Bone;
                P.Coat = Coat(Leather, Cloth::Leather, 0.4, 4);
                P.Coat.ButtonColor = Bone;
                P.Coat.Wear = 0.7;
                P.Coat.Roughness = 0.6;
                P.Tails = Tails(Leather, Cloth::Leather, 0.95, 0.5);
                P.Legs = Trousers(Rgb{0.2, 0.2, 0.22}, Cloth::Wool, 1.9);
                P.Feet = Boots(LeatherDark, 1.3);
                P.Belt = Belt(LeatherDark);
                P.Cutlass = true;
                P.Clip = "69_69"; P.Time = 4.0; P.Upright = 0.5;
                Add(P);
            }
            // ---- Big Tom: 47, 188 cm, massive; bald; full grey-brown beard; ruddy; leather apron, linen shirt with rolled sleeves,
            // heavy boots, ring on a chain (named_crew_deep_dives.md 3.1).
            {
                PersonSpec P;
                P.Id = "bigtom";
                P.Age = 47; P.Muscle = 1.0; P.Weight = 0.78; P.Proportions = 0.6; P.Height = 1.88;
                P.Detail = {{"head/head-square", 0.6}, {"head/head-fat-incr", 0.4}, {"nose/nose-scale-horiz-incr", 0.5}, {"neck/neck-scale-horiz-incr", 0.6},
                    {"chin/chin-width-incr", 0.4}, {"stomach/stomach-pregnant-incr", 0.25}};
                P.Skin = Ruddy; P.Ruddy = 0.3; P.Weathered = 0.45; P.Wrinkles = 0.35; P.Eyes = BrownEyes;
                P.Hair = HairGreyBrown; P.Gray = 0.35; P.Hairdo = HairStyle::Bald; P.Beard = BeardStyle::Full; P.BeardLength = 0.55;
                P.Shirt = Top(Linen, Cloth::Linen, 1.25, 0.75);
                P.Shirt.Wear = 0.6;
                P.Legs = Trousers(WoolBrown, Cloth::Wool, 1.9);
                P.Feet = Boots(LeatherDark, 1.6);
                P.Front.On = true; P.Front.Color = Rgb{0.3, 0.19, 0.11}; P.Front.Top = 0.78; P.Front.Hem = 1.1;
                P.Necklace = true;
                P.Clip = "70_03"; P.Time = 14.6; P.Upright = 0.5;
                Add(P);
            }
            // ---- Esteban: 78, 170 cm, slim and weathered; long white hair bound back; full white beard; mahogany skin;
            // mixed-cultural sailor's gear, cap, polished boots, ring on a chain (named_crew_deep_dives.md 6.1).
            {
                PersonSpec P;
                P.Id = "esteban";
                P.Age = 78; P.Muscle = 0.4; P.Weight = 0.3; P.Proportions = 0.6; P.Height = 1.7;
                P.African = 0.4; P.Asian = 0.1; P.Caucasian = 0.5;
                P.Detail = {{"head/head-age-incr", 0.8}, {"head/head-fat-decr", 0.4}, {"nose/nose-hump-incr", 0.4}, {"eyes/l-eye-bag-incr", 0.6}, {"eyes/r-eye-bag-incr", 0.6}};
                P.Skin = Mahogany; P.Weathered = 0.85; P.Wrinkles = 0.9; P.Eyes = BrownEyes;
                P.Hair = HairWhite; P.Gray = 1.0; P.Hairdo = HairStyle::TiedBack; P.HairLength = 3.2; P.Beard = BeardStyle::Full; P.BeardLength = 0.35;
                P.Hat = HatStyle::KnitCap; P.HatColor = Rgb{0.45, 0.2, 0.16};
                P.Shirt = Top(Rgb{0.74, 0.7, 0.6}, Cloth::Cotton, 1.9, 0.6);
                P.Vest = Vest(Indigo, Cloth::Wool, 6);
                P.Vest.ButtonColor = Brass;
                P.Legs = Trousers(Rgb{0.35, 0.3, 0.24}, Cloth::Canvas, 1.9);
                P.Feet = Boots(Black, 1.4, 0.3);
                P.Sash = Sash(Madder, true);
                P.Necklace = true;
                P.Clip = "18_08"; P.Time = 3.2; P.Upright = 0.4;
                Add(P);
            }
            // ---- Father Salvio: 58, 172 cm, slim; white tonsure, trimmed white beard, pale blue eyes; black cassock, sandals, wooden rosary (6.5).
            {
                PersonSpec P;
                P.Id = "salvio";
                P.Age = 58; P.Muscle = 0.4; P.Weight = 0.4; P.Height = 1.72;
                P.Detail = {{"head/head-oval", 0.5}, {"nose/nose-scale-vert-incr", 0.3}, {"head/head-age-incr", 0.3}};
                P.Skin = Pale; P.Weathered = 0.2; P.Wrinkles = 0.5; P.Eyes = PaleBlue;
                P.Hair = HairWhite; P.Gray = 1.0; P.Hairdo = HairStyle::Tonsure; P.Beard = BeardStyle::Trimmed; P.BeardLength = 0.15;
                P.Shirt = Top(Black, Cloth::Wool, 1.95, 0.02);
                P.Shirt.Hem = -0.3;
                P.Shirt.Buttons = 9;
                P.Shirt.ButtonColor = Rgb{0.1, 0.1, 0.1};
                P.Dress = LongSkirt(Black, Cloth::Wool, 1.92);
                P.Dress.Flare = 0.1;
                P.Feet = Boots(Leather, 1.97);
                P.Sash = Sash(Rgb{0.12, 0.12, 0.12}, false);
                P.Sash.Height = 0.3;
                P.Cross = true;
                P.Clip = "40_10"; P.Time = 0.6; P.Upright = 0.6;
                Add(P);
            }
            // ---- Koa: hidden Moran trader and repair worker; island-weathered practical clothing, tool belt (catalogue). Look: default.
            {
                PersonSpec P;
                P.Id = "koa";
                P.Age = 52; P.Muscle = 0.6; P.Weight = 0.55; P.Height = 1.74;
                P.African = 0.5; P.Asian = 0.3; P.Caucasian = 0.2;
                P.Detail = {{"head/head-round", 0.4}, {"nose/nose-scale-horiz-incr", 0.5}, {"nose/nose-nostrils-width-incr", 0.4}, {"mouth/mouth-lowerlip-volume-incr", 0.3}};
                P.Skin = Brown; P.Weathered = 0.6; P.Wrinkles = 0.3; P.Eyes = DarkEyes;
                P.Hair = HairBlack; P.Gray = 0.4; P.Hairdo = HairStyle::Cropped; P.Beard = BeardStyle::Stubble;
                P.Hat = HatStyle::Straw; P.HatColor = Rgb{0.74, 0.64, 0.44};
                P.Shirt = Top(Rgb{0.66, 0.54, 0.34}, Cloth::Cotton, 1.4, 0.9);
                P.Shirt.Pattern = Print::Check; P.Shirt.PatternColor = Rgb{0.5, 0.22, 0.14};
                P.Legs = Trousers(Canvas, Cloth::Canvas, 1.7);
                P.Feet = Boots(Leather, 1.98);
                P.Belt = Belt(Leather); P.Belt.Height = 0.6;
                P.Clip = "69_69"; P.Time = 3.8; P.Upright = 0.5;
                Add(P);
            }
            // ---- Draven Voss: 45, 1.90 m, wiry, military bearing; black hair streaked silver tied back; precise beard; ice-blue eyes;
            // old burn on the neck; deep imperial navy captain's coat, brass buttons, faded gold piping, saber (draven_voss.md).
            {
                PersonSpec P;
                P.Id = "draven";
                P.Age = 45; P.Muscle = 0.62; P.Weight = 0.32; P.Proportions = 0.95; P.Height = 1.9;
                P.Detail = {{"head/head-rectangular", 0.5}, {"chin/chin-prominent-incr", 0.5}, {"cheek/l-cheek-bones-incr", 0.6}, {"cheek/r-cheek-bones-incr", 0.6},
                    {"head/head-fat-decr", 0.5}, {"nose/nose-hump-incr", 0.4}, {"eyebrows/eyebrows-trans-down", 0.3}};
                P.Skin = Rgb{0.8, 0.65, 0.55}; P.Weathered = 0.45; P.Wrinkles = 0.35; P.Eyes = IceBlue;
                P.Hair = HairBlack; P.Gray = 0.3; P.Hairdo = HairStyle::TiedBack; P.HairLength = 1.6; P.Beard = BeardStyle::Trimmed; P.BeardLength = 0.12;
                P.Scars = {{V3{0.3, -1.55, -0.05}, V3{0.45, -1.2, -0.3}, 0.14, true}};
                P.Shirt = Top(White, Cloth::Linen, 1.95, 0.5);
                P.Coat = Coat(Navy, Cloth::Wool, 0.42, 7);
                P.Coat.Trimmed = true; P.Coat.Trim = Gold; P.Coat.ButtonColor = Brass; P.Coat.Wear = 0.45;
                P.Tails = Tails(Navy, Cloth::Wool, 1.0, 0.45);
                P.Tails.Trimmed = true; P.Tails.Trim = Gold;
                P.Legs = Trousers(Rgb{0.12, 0.12, 0.14}, Cloth::Wool, 1.9);
                P.Feet = Boots(Black, 1.2, 0.3);
                P.Belt = Belt(Black);
                P.Cutlass = true; P.Pistol = true;
                P.Clip = "40_10"; P.Time = 0.4; P.Upright = 0.9;
                Add(P);
            }
            // ---- General Herrera: imperial general, "absolute High" register. Look: default (imperial navy, gold, bicorne).
            {
                PersonSpec P;
                P.Id = "herrera";
                P.Age = 56; P.Muscle = 0.55; P.Weight = 0.58; P.Height = 1.8;
                P.Detail = {{"head/head-square", 0.4}, {"nose/nose-hump-incr", 0.5}, {"head/head-fat-incr", 0.2}, {"chin/chin-prominent-incr", 0.3}};
                P.Skin = Rgb{0.84, 0.66, 0.55}; P.Wrinkles = 0.5; P.Eyes = DarkEyes;
                P.Hair = HairIron; P.Gray = 0.7; P.Hairdo = HairStyle::Short; P.Beard = BeardStyle::Mustache;
                P.Shirt = Top(White, Cloth::Linen, 1.95, 0.1);
                P.Vest = Vest(White, Cloth::Wool, 6);
                P.Vest.NeckFront = 0.8;
                P.Coat = Coat(Navy, Cloth::Wool, 0.2, 8);
                P.Coat.Trimmed = true; P.Coat.Trim = Gold; P.Coat.ButtonColor = Gold; P.Coat.Wear = 0.1;
                P.Tails = Tails(Navy, Cloth::Wool, 1.05, 0.5);
                P.Tails.Trimmed = true; P.Tails.Trim = Gold;
                P.Legs = Trousers(White, Cloth::Wool, 1.15);
                P.Feet = Boots(Black, 1.12, 0.25);
                P.Sash = Sash(Crimson, true);
                P.Hat = HatStyle::Bicorne; P.HatColor = Black; P.HatTrimmed = true; P.HatTrim = Gold;
                P.Cutlass = true;
                P.Clip = "40_10"; P.Time = 0.8; P.Upright = 0.9;
                Add(P);
            }
            // ---- Marc and Denise: the Harlow crew of the opening (FamilyInteractionManifest.json). Looks: default.
            {
                PersonSpec P;
                P.Id = "marc";
                P.Age = 44; P.Muscle = 0.6; P.Weight = 0.62; P.Height = 1.78;
                P.African = 0.2; P.Caucasian = 0.8;
                P.Detail = {{"head/head-round", 0.3}, {"nose/nose-curve-convex", 0.4}, {"mouth/mouth-scale-horiz-incr", 0.3}};
                P.Skin = Tanned; P.Weathered = 0.55; P.Wrinkles = 0.2; P.Eyes = BrownEyes;
                P.Hair = HairBrown; P.Gray = 0.2; P.Hairdo = HairStyle::Short; P.Beard = BeardStyle::Trimmed; P.BeardLength = 0.18;
                P.Shirt = Top(White, Cloth::Cotton, 1.9, 0.5);
                P.Shirt.Pattern = Print::Stripes; P.Shirt.PatternColor = Navy;
                P.Coat = Coat(Navy, Cloth::Wool, 0.3, 6);
                P.Coat.Hem = 0.02; P.Coat.ButtonColor = Brass;
                P.Legs = Trousers(Rgb{0.72, 0.68, 0.58}, Cloth::Canvas, 1.85);
                P.Feet = Boots(Black, 1.95);
                P.Clip = "18_08"; P.Time = 9.8; P.Upright = 0.5;
                Add(P);
            }
            {
                PersonSpec P;
                P.Id = "denise";
                P.Male = 0.0; P.Age = 34; P.Muscle = 0.58; P.Weight = 0.45; P.Height = 1.68;
                P.African = 0.7; P.Caucasian = 0.3;
                P.Detail = {{"head/head-oval", 0.4}, {"mouth/mouth-upperlip-volume-incr", 0.3}, {"cheek/l-cheek-bones-incr", 0.3}, {"cheek/r-cheek-bones-incr", 0.3}};
                P.Skin = Brown; P.Weathered = 0.3; P.Eyes = DarkEyes;
                P.Hair = HairBlack; P.Hairdo = HairStyle::PinnedUp;
                P.Hat = HatStyle::Headscarf; P.HatColor = Rgb{0.66, 0.36, 0.12};
                P.Shirt = Top(Linen, Cloth::Linen, 1.5, 0.6);
                P.Vest = Vest(Rgb{0.4, 0.36, 0.26}, Cloth::Canvas, 4);
                P.Legs = Trousers(Rgb{0.25, 0.23, 0.2}, Cloth::Wool, 1.9);
                P.Feet = Boots(Leather, 1.6);
                P.Sash = Sash(Rgb{0.55, 0.3, 0.1}, true);
                P.Clip = "13_26"; P.Time = 9.4; P.Upright = 0.5;
                Add(P);
            }

            // ---- Crimson Armada boarders (catalogue: light, heavy, sharpshooter; no unique hero face).
            {
                PersonSpec P;
                P.Id = "boarder";
                P.Age = 30; P.Muscle = 0.66; P.Weight = 0.45; P.Height = 1.78;
                P.African = 0.2; P.Caucasian = 0.8;
                P.Detail = {{"nose/nose-hump-incr", 0.5}, {"chin/chin-width-incr", 0.3}};
                P.Skin = OliveSkin; P.Weathered = 0.6; P.Eyes = BrownEyes;
                P.Hair = HairBlack; P.Hairdo = HairStyle::Short; P.Beard = BeardStyle::Stubble;
                P.Hat = HatStyle::Headscarf; P.HatColor = Crimson;
                P.Shirt = Top(LinenDirty, Cloth::Linen, 1.4, 0.8);
                P.Shirt.Wear = 0.8;
                P.Vest = Vest(Leather, Cloth::Leather, 0);
                P.Vest.NeckFront = 1.0;
                P.Legs = Trousers(Rgb{0.3, 0.28, 0.25}, Cloth::Canvas, 1.8);
                P.Feet = Boots(LeatherDark, 1.5);
                P.Sash = Sash(Crimson, true);
                P.Cutlass = true;
                P.TextureSize = 512;
                P.Clip = "02_07"; P.Time = 13.2; P.Upright = 0.4;
                Add(P);
            }
            {
                PersonSpec P;
                P.Id = "boarder_heavy";
                P.Age = 38; P.Muscle = 0.92; P.Weight = 0.7; P.Height = 1.86;
                P.African = 0.5; P.Caucasian = 0.5;
                P.Detail = {{"head/head-square", 0.5}, {"nose/nose-scale-horiz-incr", 0.4}, {"neck/neck-scale-horiz-incr", 0.4}};
                P.Skin = Rgb{0.6, 0.42, 0.3}; P.Weathered = 0.6; P.Wrinkles = 0.2; P.Eyes = DarkEyes;
                P.Hair = HairBlack; P.Hairdo = HairStyle::Cropped; P.Beard = BeardStyle::Full; P.BeardLength = 0.3;
                P.Hat = HatStyle::Tricorne; P.HatColor = Black;
                P.Shirt = Top(Rgb{0.42, 0.1, 0.08}, Cloth::Wool, 1.9, 0.5);
                P.Coat = Coat(Rgb{0.44, 0.31, 0.18}, Cloth::Leather, 0.0, 0);
                P.Coat.NeckFront = 0.4; P.Coat.Hem = -0.25; P.Coat.Thick = 0.15; P.Coat.Roughness = 0.6;
                P.Legs = Trousers(Rgb{0.2, 0.18, 0.16}, Cloth::Wool, 1.9);
                P.Feet = Boots(LeatherDark, 1.25);
                P.Belt = Belt(LeatherDark); P.Belt.Height = 0.6;
                P.Cutlass = true; P.Pistol = true;
                P.TextureSize = 512;
                P.Clip = "15_13"; P.Time = 11.2; P.Upright = 0.4;
                Add(P);
            }
            {
                PersonSpec P;
                P.Id = "sharpshooter";
                P.Age = 34; P.Muscle = 0.5; P.Weight = 0.4; P.Height = 1.76;
                P.Asian = 0.3; P.Caucasian = 0.7;
                P.Detail = {{"head/head-triangular", 0.4}, {"nose/nose-point-width-decr", 0.4}};
                P.Skin = Fair; P.Weathered = 0.4; P.Eyes = GreyBlue;
                P.Hair = HairBrown; P.Hairdo = HairStyle::TiedBack; P.HairLength = 1.2; P.Beard = BeardStyle::Mustache;
                P.Hat = HatStyle::Tricorne; P.HatColor = Black;
                P.Shirt = Top(Linen, Cloth::Linen, 1.95, 0.4);
                P.Vest = Vest(Black, Cloth::Wool, 6);
                P.Coat = Coat(Rgb{0.3, 0.07, 0.07}, Cloth::Wool, 0.35, 6);
                P.Coat.ButtonColor = Rgb{0.55, 0.55, 0.55};
                P.Tails = Tails(Rgb{0.3, 0.07, 0.07}, Cloth::Wool, 1.05, 0.5);
                P.Legs = Trousers(Black, Cloth::Wool, 1.15);
                P.Stockings = Stockings(WoolGrey);
                P.Feet = Boots(Black, 1.95);
                P.Belt = Belt(Black);
                P.Pistol = true;
                P.TextureSize = 512;
                P.Clip = "77_01"; P.Time = 0.2; P.Upright = 0.6;
                Add(P);
            }
            {
                PersonSpec P = Cast["sharpshooter"];
                P.Id = "armada_officer";
                P.Age = 42; P.Weight = 0.55; P.Height = 1.8;
                P.Detail = {{"head/head-rectangular", 0.4}, {"chin/chin-prominent-incr", 0.4}};
                P.Hair = HairBlack; P.Gray = 0.15; P.Beard = BeardStyle::Trimmed; P.BeardLength = 0.15; P.Eyes = BrownEyes; P.Skin = OliveSkin;
                P.Coat.Color = Crimson; P.Coat.Trimmed = true; P.Coat.Trim = Gold; P.Coat.ButtonColor = Gold;
                P.Tails.Color = Crimson; P.Tails.Trimmed = true; P.Tails.Trim = Gold;
                P.Vest = Vest(Rgb{0.66, 0.6, 0.48}, Cloth::Silk, 7);
                P.HatTrimmed = true; P.HatTrim = Gold;
                P.Feet = Boots(Black, 1.2, 0.3);
                P.Stockings.On = false;
                P.Cutlass = true;
                P.Clip = "18_10"; P.Time = 10.8; P.Upright = 0.6;
                Add(P);
            }
            // ---- Soldiers of the colonial war (default looks: imperial navy blue, Albion red).
            {
                PersonSpec P;
                P.Id = "imperial_soldier";
                P.Age = 27; P.Muscle = 0.6; P.Weight = 0.45; P.Height = 1.77;
                P.Detail = {{"head/head-oval", 0.3}};
                P.Skin = Fair; P.Eyes = BrownEyes;
                P.Hair = HairBrown; P.Hairdo = HairStyle::Cropped; P.Beard = BeardStyle::None;
                P.Hat = HatStyle::Tricorne; P.HatColor = Black;
                P.Shirt = Top(White, Cloth::Linen, 1.95, 0.1);
                P.Vest = Vest(White, Cloth::Wool, 6);
                P.Coat = Coat(Rgb{0.12, 0.15, 0.3}, Cloth::Wool, 0.25, 8);
                P.Coat.Trimmed = true; P.Coat.Trim = Rgb{0.8, 0.78, 0.72}; P.Coat.ButtonColor = Rgb{0.8, 0.8, 0.8};
                P.Tails = Tails(Rgb{0.12, 0.15, 0.3}, Cloth::Wool, 0.9, 0.5);
                P.Legs = Trousers(White, Cloth::Wool, 1.15);
                P.Feet = Boots(Black, 1.1);
                P.Belt = Belt(White);
                P.TextureSize = 512;
                P.Clip = "40_10"; P.Time = 0.4; P.Upright = 0.85;
                Add(P);
                PersonSpec A = P;
                A.Id = "albion_soldier";
                A.Skin = Pale; A.Freckles = 0.4; A.Eyes = PaleBlue; A.Hair = Rgb{0.42, 0.24, 0.12};
                A.Coat.Color = Rgb{0.62, 0.1, 0.08}; A.Tails.Color = Rgb{0.62, 0.1, 0.08};
                A.Coat.Trim = White;
                A.Detail = {{"nose/nose-point-width-decr", 0.3}, {"head/head-triangular", 0.3}};
                A.Clip = "40_10"; A.Time = 0.6; A.Upright = 0.85;
                Add(A);
            }
            // ---- Liberation fighter (armed islander; default look).
            {
                PersonSpec P;
                P.Id = "liberation_fighter";
                P.Age = 29; P.Muscle = 0.7; P.Weight = 0.45; P.Height = 1.76;
                P.African = 0.8; P.Caucasian = 0.2;
                P.Skin = DarkBrown; P.Eyes = DarkEyes; P.Hair = HairBlack; P.Hairdo = HairStyle::Cropped; P.Beard = BeardStyle::Stubble;
                P.Hat = HatStyle::Headscarf; P.HatColor = Rgb{0.2, 0.36, 0.22};
                P.Shirt = Top(Linen, Cloth::Linen, 1.5, 0.9);
                P.Vest = Vest(Olive, Cloth::Canvas, 4);
                P.Legs = Trousers(Canvas, Cloth::Canvas, 1.75);
                P.Sash = Sash(Rgb{0.2, 0.36, 0.22}, true);
                P.Cutlass = true;
                P.TextureSize = 512;
                P.Clip = "02_07"; P.Time = 14.4; P.Upright = 0.4;
                Add(P);
            }

            // ---- Townsfolk (defaults).
            {
                PersonSpec P;
                P.Id = "dockworker";
                P.Age = 30; P.Muscle = 0.85; P.Weight = 0.5; P.Height = 1.8;
                P.African = 1.0; P.Caucasian = 0.0;
                P.Detail = {{"nose/nose-scale-horiz-incr", 0.3}};
                P.Skin = DarkBrown; P.Eyes = DarkEyes; P.Hair = HairBlack; P.Hairdo = HairStyle::Shaved;
                P.Shirt = Top(LinenDirty, Cloth::Linen, 0.9, 1.1);
                P.Shirt.Wear = 0.9;
                P.Legs = Trousers(Canvas, Cloth::Canvas, 1.6);
                P.Sash = Sash(Rgb{0.4, 0.3, 0.2}, false);
                P.TextureSize = 512;
                P.Clip = "70_03"; P.Time = 7.2; P.Upright = 0.5;
                Add(P);
            }
            {
                PersonSpec P;
                P.Id = "fisherman";
                P.Age = 61; P.Muscle = 0.5; P.Weight = 0.45; P.Height = 1.72;
                P.African = 0.4; P.Caucasian = 0.6;
                P.Detail = {{"head/head-age-incr", 0.4}};
                P.Skin = Rgb{0.62, 0.44, 0.3}; P.Weathered = 0.8; P.Wrinkles = 0.7; P.Eyes = BrownEyes;
                P.Hair = HairIron; P.Gray = 0.8; P.Hairdo = HairStyle::Short; P.Beard = BeardStyle::Trimmed; P.BeardLength = 0.2;
                P.Hat = HatStyle::Straw; P.HatColor = Rgb{0.7, 0.6, 0.4};
                P.Shirt = Top(Rgb{0.7, 0.6, 0.4}, Cloth::Cotton, 1.6, 0.8);
                P.Shirt.Pattern = Print::Check; P.Shirt.PatternColor = Indigo;
                P.Legs = Trousers(Canvas, Cloth::Canvas, 1.7);
                P.TextureSize = 512;
                P.Clip = "26_10"; P.Time = 5.6; P.Upright = 0.4;
                Add(P);
            }
            {
                PersonSpec P;
                P.Id = "market_woman";
                P.Male = 0.0; P.Age = 40; P.Muscle = 0.5; P.Weight = 0.6; P.Height = 1.64; P.BreastSize = 0.6;
                P.African = 0.6; P.Caucasian = 0.4;
                P.Detail = {{"head/head-round", 0.3}};
                P.Skin = Brown; P.Eyes = DarkEyes; P.Hair = HairBlack; P.Hairdo = HairStyle::PinnedUp;
                P.Hat = HatStyle::Headwrap; P.HatColor = Rgb{0.78, 0.56, 0.14};
                P.Shirt = Top(White, Cloth::Cotton, 1.2, 0.6);
                P.Vest = Vest(Madder, Cloth::Wool, 0);
                P.Vest.NeckFront = 1.2; P.Vest.Hem = 0.22;
                P.Dress = LongSkirt(Rgb{0.32, 0.3, 0.44}, Cloth::Cotton, 1.9);
                P.Front.On = true; P.Front.Color = Rgb{0.84, 0.82, 0.74}; P.Front.Fabric = Cloth::Cotton; P.Front.Top = 0.3; P.Front.Hem = 1.6; P.Front.HalfAngle = 1.0;
                P.Feet = Boots(Leather, 1.97);
                P.TextureSize = 512;
                P.Clip = "18_08"; P.Time = 3.2; P.Upright = 0.5;
                Add(P);
            }
            {
                PersonSpec P;
                P.Id = "townswoman";
                P.Male = 0.0; P.Age = 28; P.Muscle = 0.45; P.Weight = 0.42; P.Height = 1.66;
                P.Asian = 0.2; P.African = 0.2; P.Caucasian = 0.6;
                P.Detail = {{"head/head-oval", 0.4}, {"nose/nose-scale-horiz-decr", 0.3}};
                P.Skin = Rgb{0.8, 0.62, 0.5}; P.Eyes = BrownEyes; P.Hair = HairBrown; P.Hairdo = HairStyle::PinnedUp;
                P.Shirt = Top(White, Cloth::Cotton, 1.3, 0.7);
                P.Vest = Vest(Rgb{0.25, 0.35, 0.25}, Cloth::Wool, 0);
                P.Vest.NeckFront = 1.1; P.Vest.Hem = 0.22;
                P.Dress = LongSkirt(Ochre, Cloth::Cotton, 1.92);
                P.Feet = Boots(Leather, 1.97);
                P.TextureSize = 512;
                P.Clip = "69_69"; P.Time = 4.2; P.Upright = 0.5;
                Add(P);
            }
            {
                PersonSpec P;
                P.Id = "merchant";
                P.Age = 52; P.Muscle = 0.4; P.Weight = 0.72; P.Height = 1.74;
                P.Detail = {{"head/head-fat-incr", 0.4}, {"neck/neck-double-incr", 0.4}, {"stomach/stomach-pregnant-incr", 0.3}};
                P.Skin = Rgb{0.84, 0.66, 0.55}; P.Ruddy = 0.2; P.Wrinkles = 0.4; P.Eyes = BrownEyes;
                P.Hair = HairIron; P.Gray = 0.6; P.Hairdo = HairStyle::Short;
                P.Hat = HatStyle::Tricorne; P.HatColor = Rgb{0.12, 0.1, 0.08};
                P.Shirt = Top(White, Cloth::Linen, 1.95, 0.15);
                P.Vest = Vest(Rgb{0.52, 0.36, 0.12}, Cloth::Silk, 7);
                P.Coat = Coat(WoolBrown, Cloth::Wool, 0.5, 6);
                P.Coat.ButtonColor = Brass;
                P.Tails = Tails(WoolBrown, Cloth::Wool, 1.0, 0.55);
                P.Legs = Trousers(Rgb{0.3, 0.25, 0.18}, Cloth::Wool, 1.15);
                P.Stockings = Stockings(White);
                P.Feet = Boots(Black, 1.96, 0.35);
                P.TextureSize = 512;
                P.Clip = "18_10"; P.Time = 11.0; P.Upright = 0.6;
                Add(P);
            }
            {
                PersonSpec P;
                P.Id = "sailor";
                P.Age = 26; P.Muscle = 0.7; P.Weight = 0.4; P.Height = 1.74;
                P.African = 0.3; P.Asian = 0.2; P.Caucasian = 0.5;
                P.Skin = Rgb{0.66, 0.47, 0.34}; P.Weathered = 0.6; P.Eyes = BrownEyes;
                P.Hair = HairBlack; P.Hairdo = HairStyle::Short; P.Beard = BeardStyle::Stubble;
                P.Hat = HatStyle::KnitCap; P.HatColor = Rgb{0.22, 0.24, 0.34};
                P.Shirt = Top(White, Cloth::Cotton, 1.9, 0.5);
                P.Shirt.Pattern = Print::Stripes; P.Shirt.PatternColor = Crimson;
                P.Legs = Trousers(Rgb{0.74, 0.7, 0.6}, Cloth::Canvas, 1.8);
                P.TextureSize = 512;
                P.Clip = "02_01"; P.Time = 1.6; P.Upright = 0.5;
                Add(P);
            }
            {
                PersonSpec P;
                P.Id = "elder_woman";
                P.Male = 0.0; P.Age = 68; P.Muscle = 0.35; P.Weight = 0.5; P.Height = 1.6;
                P.African = 0.5; P.Caucasian = 0.5;
                P.Skin = Rgb{0.6, 0.42, 0.3}; P.Wrinkles = 0.8; P.Weathered = 0.6; P.Eyes = BrownEyes;
                P.Hair = HairWhite; P.Hairdo = HairStyle::PinnedUp; P.Gray = 1.0;
                P.Hat = HatStyle::Headwrap; P.HatColor = White;
                P.Shirt = Top(Rgb{0.7, 0.66, 0.56}, Cloth::Cotton, 1.7, 0.4);
                P.Dress = LongSkirt(Rgb{0.22, 0.2, 0.2}, Cloth::Wool, 1.92);
                P.Front.On = true; P.Front.Color = Rgb{0.6, 0.56, 0.46}; P.Front.Fabric = Cloth::Cotton; P.Front.Top = 0.3; P.Front.Hem = 1.7;
                P.Feet = Boots(Leather, 1.97);
                P.TextureSize = 512;
                P.Clip = "15_06"; P.Time = 7.2; P.Upright = 0.4;
                Add(P);
            }
            {
                PersonSpec P;
                P.Id = "youth";
                P.Age = 25; P.Muscle = 0.45; P.Weight = 0.3; P.Height = 1.68;
                P.African = 0.3; P.Caucasian = 0.7;
                P.Detail = {{"head/head-age-decr", 0.8}};
                P.Skin = OliveSkin; P.Eyes = BrownEyes; P.Hair = HairBlack; P.Hairdo = HairStyle::Tousled;
                P.Shirt = Top(LinenDirty, Cloth::Linen, 1.4, 0.9);
                P.Legs = Trousers(CanvasDark, Cloth::Canvas, 1.65);
                P.TextureSize = 512;
                P.Clip = "13_09"; P.Time = 3.6; P.Upright = 0.4;
                Add(P);
            }
            return Cast;
        }

        const std::map<std::string, PersonSpec, std::less<>>& Cast()
        {
            static const std::map<std::string, PersonSpec, std::less<>> Table = BuildCast();
            return Table;
        }
    }

    const PersonSpec* FindPerson(const std::string_view Id)
    {
        const auto Found = Cast().find(Id);
        return Found == Cast().end() ? nullptr : &Found->second;
    }
}

namespace DarkArisen::Tools::Art
{
    std::string PersonFor(const std::string_view EntityName)
    {
        std::string Name(EntityName);
        for (char& C : Name) C = (C >= 'A' && C <= 'Z') ? static_cast<char>(C + ('a' - 'A')) : C;
        const auto Has = [&Name](const char* Part) { return Name.find(Part) != std::string::npos; };
        if (Has("jake")) return "jake";
        if (Has("dream")) return "dream_ethan";
        if (Has("ethan")) return "ethan";
        if (Has("broker") || Has("merchant") || Has("trader") || Has("clerk")) return "merchant";
        if (Has("draven")) return "draven";
        if (Has("herrera")) return "herrera";
        if (Has("mira")) return "mira";
        if (Has("bigtom") || Has("big tom") || Has("big_tom") || Has("tom")) return "bigtom";
        if (Has("esteban")) return "esteban";
        if (Has("salvio")) return "salvio";
        if (Has("koa")) return "koa";
        if (Has("marc")) return "marc";
        if (Has("denise")) return "denise";
        if (Has("imperial")) return "imperial_soldier";
        if (Has("albion")) return "albion_soldier";
        if (Has("liberation")) return "liberation_fighter";
        if (Has("quartermaster") || Has("captain") || Has("officer") || Has("holder") || Has("boss")) return "armada_officer";
        if (Has("assassin") || Has("watcher") || Has("observer")) return "sharpshooter";
        if (Has("boarder") || Has("raider") || Has("duel") || Has("guard") || Has("armada") || Has("captive"))
        {
            static const char* Hostiles[] = {"boarder", "boarder_heavy", "sharpshooter"};
            return Has("boarder") && !Has(".") ? "boarder" : Hostiles[HashString(Name) % 3u];
        }
        if (Has("crew")) return "sailor";
        static const char* Townsfolk[] = {"dockworker", "fisherman", "market_woman", "townswoman", "sailor", "elder_woman", "youth"};
        return Townsfolk[HashString(Name) % 7u];
    }
}
