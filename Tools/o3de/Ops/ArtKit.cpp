#include "ArtKit.h"
#include "ArtKitLibrary.h"
#include "Json.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <map>

namespace DarkArisen::Tools::Art
{
    namespace
    {
        const std::string Generator = "Dark Arisen art kit (procedural, hand-authored)";

        std::string Lower(std::string_view Text)
        {
            std::string Out(Text);
            for (char& C : Out) C = (C >= 'A' && C <= 'Z') ? static_cast<char>(C + ('a' - 'A')) : C;
            return Out;
        }

        std::string Decode64(std::string_view Text)
        {
            std::string Out;
            std::uint32_t Buffer = 0;
            int Bits = 0;
            for (const char C : Text)
            {
                int V = -1;
                if (C >= 'A' && C <= 'Z') V = C - 'A';
                else if (C >= 'a' && C <= 'z') V = C - 'a' + 26;
                else if (C >= '0' && C <= '9') V = C - '0' + 52;
                else if (C == '+') V = 62;
                else if (C == '/') V = 63;
                if (V < 0) continue;
                Buffer = (Buffer << 6) | static_cast<std::uint32_t>(V);
                Bits += 6;
                if (Bits >= 8)
                {
                    Bits -= 8;
                    Out.push_back(static_cast<char>((Buffer >> Bits) & 0xFFu));
                }
            }
            return Out;
        }

        float ReadFloat(const std::string& Bytes, std::size_t Offset)
        {
            std::uint32_t Bits = 0;
            for (int K = 0; K < 4; ++K) Bits |= static_cast<std::uint32_t>(static_cast<std::uint8_t>(Bytes[Offset + static_cast<std::size_t>(K)])) << (8 * K);
            float F;
            static_assert(sizeof(F) == sizeof(Bits));
            std::memcpy(&F, &Bits, sizeof(F));
            return F;
        }

        /** Positions of a greybox glTF (single embedded buffer), source frame mapped to world (mirror Y). */
        bool ReadPositions(std::string_view Gltf, std::vector<V3>& Out)
        {
            JsonValue Root;
            std::string Problem;
            if (!JsonReader::Parse(Gltf, Root, Problem)) return false;
            const JsonValue* Buffers = Root.Find("buffers");
            const JsonValue* Views = Root.Find("bufferViews");
            const JsonValue* Accessors = Root.Find("accessors");
            const JsonValue* Meshes = Root.Find("meshes");
            if (!Buffers || !Views || !Accessors || !Meshes || Buffers->Items.empty()) return false;
            const JsonValue* Uri = Buffers->Items[0].Find("uri");
            if (!Uri) return false;
            const std::string Bytes = Decode64(Uri->Text.substr(Uri->Text.find(',') + 1));
            for (const JsonValue& Mesh : Meshes->Items)
            {
                for (const JsonValue& Primitive : Mesh.Find("primitives")->Items)
                {
                    const auto Index = static_cast<std::size_t>(Primitive.Find("attributes")->Find("POSITION")->Number);
                    const JsonValue& Accessor = Accessors->Items[Index];
                    const JsonValue& View = Views->Items[static_cast<std::size_t>(Accessor.Find("bufferView")->Number)];
                    const std::size_t Base = static_cast<std::size_t>(View.Find("byteOffset") ? View.Find("byteOffset")->Number : 0.0) +
                        static_cast<std::size_t>(Accessor.Find("byteOffset") ? Accessor.Find("byteOffset")->Number : 0.0);
                    const std::size_t Stride = View.Find("byteStride") ? static_cast<std::size_t>(View.Find("byteStride")->Number) : 12u;
                    const auto Count = static_cast<std::size_t>(Accessor.Find("count")->Number);
                    for (std::size_t I = 0; I < Count; ++I)
                    {
                        const std::size_t At = Base + I * Stride;
                        if (At + 12 > Bytes.size()) return false;
                        Out.push_back({ReadFloat(Bytes, At), -static_cast<double>(ReadFloat(Bytes, At + 4)), ReadFloat(Bytes, At + 8)});
                    }
                }
            }
            return !Out.empty();
        }

        /** Boxes of a greybox built from separate cuboids (24 or 8 vertices each), world frame. */
        std::vector<std::pair<V3, V3>> ReadBoxes(std::string_view Gltf)
        {
            std::vector<V3> P;
            std::vector<std::pair<V3, V3>> Out;
            if (!ReadPositions(Gltf, P)) return Out;
            const std::size_t Group = (P.size() % 24 == 0) ? 24 : 8;
            for (std::size_t I = 0; I + Group <= P.size(); I += Group)
            {
                V3 Lo{1e30, 1e30, 1e30}, Hi{-1e30, -1e30, -1e30};
                for (std::size_t K = I; K < I + Group; ++K)
                {
                    Lo = {std::min(Lo.X, P[K].X), std::min(Lo.Y, P[K].Y), std::min(Lo.Z, P[K].Z)};
                    Hi = {std::max(Hi.X, P[K].X), std::max(Hi.Y, P[K].Y), std::max(Hi.Z, P[K].Z)};
                }
                Out.push_back({Lo, Hi});
            }
            return Out;
        }

        std::string Mm(double Metres) { return std::to_string(std::lround(Metres * 100.0)); }
    }

    std::string Kit::Emit(const std::string& Name, const std::string& Gltf)
    {
        const std::string Path = "Assets/Art/Models/" + Name + ".gltf";
        Output.push_back({Path, Gltf});
        return Path;
    }

    Kit::Kit()
    {
        for (ArtFile& File : BuildTextures()) Output.push_back(std::move(File));

        // Harlow: merchant ship, three masts under full sail, flush main deck at 2.2 m (layout authority).
        ShipSpec Harlow;
        Harlow.Name = "SM_Art_HarlowMerchantShip";
        Harlow.Stern = -13.5;
        Harlow.Bow = 14.2;
        Harlow.HalfBeam = 3.95;
        Harlow.Deck = 2.2;
        Harlow.Keel = -2.3;
        Harlow.SternHalf = 0.64;
        Harlow.Sheer = 0.8;
        Harlow.Masts = {-7.0, -0.3, 6.5};
        Harlow.MastHeights = {15.0, 21.0, 18.5};
        Harlow.SpankerOnAftMast = true;
        Harlow.HelmX = 10.5;
        Harlow.Hatches = {{2.2, 4.2}, {-4.2, -2.9}};
        Harlow.CapstanX = -5.5;
        Harlow.Bowsprit = 8.0;
        Mesh HarlowCollision("SM_Art_HarlowMerchantShip_Collision");
        Replacements["ContentSource/Ships/Harlow/SM_HarlowMerchantShip_Alpha.gltf"] = Emit(Harlow.Name, WriteGltf(BuildShip(Harlow, &HarlowCollision), Generator));
        Collisions["ContentSource/Ships/Harlow/SM_HarlowShip_WalkableCollision_Alpha.gltf"] = WriteCollisionGltf(HarlowCollision);

        // La Liberacion: brig per LaLiberacion_Layout (weather deck 2.6 m, raised fore deck 3.0 m, helm hub 4.2 m), impounded: sails furled.
        ShipSpec Brig;
        Brig.Name = "SM_Art_LaLiberacion";
        Brig.Stern = -13.0;
        Brig.Bow = 15.5;
        Brig.HalfBeam = 4.3;
        Brig.Deck = 2.6;
        Brig.Keel = -2.6;
        Brig.SternHalf = 0.62;
        Brig.Sheer = 0.7;
        Brig.Topsides = M::HullBlack;
        Brig.Band = M::HullBandRed;
        Brig.Masts = {-5.5, 6.0};
        Brig.MastHeights = {22.0, 20.0};
        Brig.SailsSet = false;
        Brig.SpankerOnAftMast = true;
        Brig.GunX = {-10.0, -7.5, 1.0, 3.5};
        Brig.RaisedFrom = 8.0;
        Brig.RaisedDeck = 3.0;
        Brig.HelmX = 11.0;
        Brig.Hatches = {{-3.8, -1.8}, {3.0, 4.4}};
        Brig.CapstanX = -9.0;
        Brig.Bowsprit = 9.0;
        Mesh BrigCollision("SM_Art_LaLiberacion_Collision");
        Replacements["ContentSource/Ships/LaLiberacion/SM_LaLiberacion_Alpha.gltf"] = Emit(Brig.Name, WriteGltf(BuildShip(Brig, &BrigCollision), Generator));
        Collisions["ContentSource/Ships/LaLiberacion/SM_LaLiberacion_Alpha.gltf"] = WriteCollisionGltf(BrigCollision);

        // Draven's raider: black hull and sails, bow towards the Harlow (-X); its entity stands 3.3 m above the sea.
        {
            ShipSpec Raider;
            Raider.Name = "SM_Art_DravenRaider";
            Raider.Stern = -15.0;
            Raider.Bow = 16.0;
            Raider.HalfBeam = 4.6;
            Raider.Deck = 2.8;
            Raider.Keel = -2.8;
            Raider.Topsides = M::HullBlack;
            Raider.Band = M::HullBandRed;
            Raider.Bottom = M::HullBlack;
            Raider.Masts = {-8.0, 0.5, 8.5};
            Raider.MastHeights = {17.0, 24.0, 21.0};
            Raider.BlackSails = true;
            Raider.SpankerOnAftMast = true;
            Raider.GunX = {-11.0, -8.5, -4.5, -2.0, 3.0, 5.5, 11.0};
            Raider.Hatches = {{-5.8, -4.8}};
            Raider.Bowsprit = 9.5;
            Mesh Built = BuildShip(Raider, nullptr);
            Mesh Placed(Raider.Name);
            Placed.Push(Xf::Translate({0, 0, -3.3}) * Xf::RotateZ(Pi));
            Placed.Append(Built);
            Placed.Pop();
            Replacements["ContentSource/Ships/Draven/SM_DravenRaidSilhouette_Alpha.gltf"] = Emit(Raider.Name, WriteGltf(Placed, Generator));
        }

        // Moran set pieces
        Replacements["ContentSource/World/Moran/DriftwoodBeach/SM_HarlowWreckage_Alpha.gltf"] = Emit("SM_Art_HarlowWreckage", WriteGltf(BuildWreckage(), Generator));
        Replacements["ContentSource/World/Moran/OuterReef/SM_OuterReef_Alpha.gltf"] = Emit("SM_Art_OuterReef", WriteGltf(BuildOuterReef(), Generator));
        Replacements["ContentSource/World/Moran/GalleonCove/SM_GalleonCove_Alpha.gltf"] = Emit("SM_Art_GalleonCoveJetty", WriteGltf(BuildCoveJetty(), Generator));

        // Rexa warehouse quarter with walkable passages
        Mesh RexaCollision("SM_Art_RexaWarehouseQuarter_Collision");
        Replacements["ContentSource/World/Rexa/Story/SM_RexaWarehouseQuarter_Alpha.gltf"] =
            Emit("SM_Art_RexaWarehouseQuarter", WriteGltf(BuildRexaWarehouses(&RexaCollision), Generator));
        Collisions["ContentSource/World/Rexa/Story/SM_RexaWarehouseQuarter_Alpha.gltf"] = WriteCollisionGltf(RexaCollision);

        // Characters with a greybox in ContentSource
        Replacements["ContentSource/Characters/Jake/SK_Jake_Alpha.gltf"] = FigureFor("jake");
        Replacements["ContentSource/Characters/Boarders/SK_Boarder_Alpha.gltf"] = FigureFor("boarder");
    }

    std::string Kit::ReplacementFor(const std::string_view ContentSourceMesh) const
    {
        const auto Found = Replacements.find(ContentSourceMesh);
        return Found == Replacements.end() ? std::string() : Found->second;
    }

    const std::string* Kit::CollisionFor(const std::string_view ContentSourceMesh) const
    {
        const auto Found = Collisions.find(ContentSourceMesh);
        return Found == Collisions.end() ? nullptr : &Found->second;
    }

    std::string Kit::FigureFor(const std::string_view EntityName)
    {
        const std::string Name = Lower(EntityName);
        const auto Has = [&Name](const char* Part) { return Name.find(Part) != std::string::npos; };
        std::string Variant;
        if (Has("jake")) Variant = "jake";
        else if (Has("draven")) Variant = "draven";
        else if (Has("boarder") || Has("guard") || Has("duel") || Has("raider")) Variant = "boarder";
        else if (Has("marc")) Variant = "merchant";
        else if (Has("denise")) Variant = "woman";
        else if (Has("ethan")) Variant = "youth";
        else if (Has("mira")) Variant = "sailor_f";
        else if (Has("tom")) Variant = "bigman";
        else if (Has("koa")) Variant = "elder";
        else if (Has("esteban") || Has("holder") || Has("boss") || Has("captain") || Has("herrera")) Variant = "officer";
        else if (Has("broker") || Has("merchant") || Has("trader") || Has("clerk")) Variant = "merchant";
        else
        {
            static const char* Townsfolk[] = {"sailor", "dockworker", "woman", "sailor", "elder", "dockworker"};
            Variant = Townsfolk[HashString(Name) % 6u];
        }
        const std::string Model = "SM_Art_Figure_" + Variant;
        if (const auto Found = Generated.find(Model); Found != Generated.end()) return Found->second;
        return Generated[Model] = Emit(Model, WriteGltf(BuildFigure(Variant, HashString(Variant)), Generator));
    }

    std::string Kit::Prop(const std::string_view PlaceholderName)
    {
        const std::string Key(PlaceholderName);
        if (const auto Found = Generated.find(Key); Found != Generated.end()) return Found->second;
        Mesh Out("SM_Art_" + Key.substr(Key.find_last_of('_') + 1));
        if (Key == "SM_Placeholder_Crate")
        {
            // manifest crate 0.8 x 0.6 x 0.5 with a ledger on top
            Crate(Out, {-0.4, -0.3, 0.0}, {0.4, 0.3, 0.5});
            Out.Box(Mat(M::Leather), {-0.15, -0.12, 0.5}, {0.15, 0.12, 0.54});
            Out.Box(Mat(M::Linen), {-0.14, -0.11, 0.54}, {0.14, 0.11, 0.545});
        }
        else if (Key == "SM_Placeholder_RouteMarker")
        {
            Out.Tube(Mat(M::Driftwood), {0, 0, -0.2}, {0, 0, 2.0}, 0.07, 0.06, 7);
            OrientedBox(Out, Mat(M::Timber), {0.3, 0.0, 1.7}, 1.0, 0.0, {0.4, 0.03, 0.1});
            OrientedBox(Out, Mat(M::Timber), {-0.15, 0.2, 1.45}, -0.6, 0.8, {0.35, 0.03, 0.09});
            Out.Tube(Mat(M::Sash), {0, 0, 1.95}, {0, 0, 2.08}, 0.075, 0.075, 7);
            Rock(Out, Mat(M::Rock), {0, 0, 0}, 0.3, 0.3, 91u);
        }
        else if (Key == "SM_Placeholder_WarMarker")
        {
            Rock(Out, Mat(M::Stone), {0, 0, 0}, 0.35, 0.5, 92u);
            Out.Tube(Mat(M::TimberDark), {0, 0, 0}, {0, 0, 3.2}, 0.05, 0.04, 7);
            std::vector<Ring> Flag;
            for (int I = 0; I <= 5; ++I)
            {
                const double T = I / 5.0;
                Flag.push_back({{0.03 + 1.2 * T, 0.12 * Sin(T * 5.0), 3.1}, {0.03 + 1.2 * T, 0.12 * Sin(T * 5.0), 2.35}});
            }
            Out.Loft(Mat(M::Sash), Flag, false);
            Out.Loft(Mat(M::Sash), Flag, false, true);
        }
        else if (Key == "SM_Placeholder_HostileHull")
        {
            ShipSpec Sloop;
            Sloop.Name = "SM_Art_HostileSloop";
            Sloop.Stern = -6.6;
            Sloop.Bow = 7.0;
            Sloop.HalfBeam = 1.95;
            Sloop.Deck = 1.6;
            Sloop.Keel = -1.4;
            Sloop.Bulwark = 0.7;
            Sloop.Sheer = 0.3;
            Sloop.Topsides = M::HullBlack;
            Sloop.Band = M::HullBandRed;
            Sloop.Masts = {0.8};
            Sloop.MastHeights = {13.0};
            Sloop.BlackSails = true;
            Sloop.Bowsprit = 4.5;
            Sloop.GunX = {-3.0, 3.0};
            Sloop.Cargo = false;
            Out = BuildShip(Sloop, nullptr);
        }
        return Generated[Key] = Emit(Out.Name(), WriteGltf(Out, Generator));
    }

    std::string Kit::Block(const double SizeX, const double SizeY, const double SizeZ, const double TopHeight, const bool Provisional)
    {
        const double Top = std::floor(TopHeight * 4.0 + 0.5) / 4.0;
        const std::string Name = std::string(Provisional ? "SM_Art_Pier_" : "SM_Art_Masonry_") + Mm(SizeX) + "x" + Mm(SizeY) + "x" + Mm(SizeZ) +
            (Provisional ? "_P" + Mm(Top + 2.5) : "");
        if (const auto Found = Generated.find(Name); Found != Generated.end()) return Found->second;
        Mesh Out(Name);
        const double Hx = SizeX / 2.0, Hy = SizeY / 2.0;
        if (Provisional)
        {
            // timber deck on stringers and piles down to the sea bed (-2.5 m), footprint centred, top at SizeZ
            Out.Box(Mat(M::DeckWorn), {-Hx, -Hy, SizeZ - 0.1}, {Hx, Hy, SizeZ});
            for (const double Y : {-Hy + 0.3, Hy - 0.3}) Out.Box(Mat(M::TimberDark), {-Hx, Y - 0.1, SizeZ - 0.32}, {Hx, Y + 0.1, SizeZ - 0.1});
            for (const double X : {-Hx + 0.15, Hx - 0.15}) Out.Box(Mat(M::TimberDark), {X - 0.1, -Hy + 0.1, SizeZ - 0.3}, {X + 0.1, Hy - 0.1, SizeZ - 0.1});
            const double Bottom = SizeZ - (Top + 2.5);
            for (const double X : {-Hx + 0.3, Hx - 0.3})
            {
                for (const double Y : {-Hy + 0.3, Hy - 0.3}) Out.Tube(Mat(M::Driftwood), {X, Y, Bottom}, {X, Y, SizeZ - 0.1}, 0.13, 0.12, 8);
            }
            // plank edge boards
            Out.Box(Mat(M::TimberDark), {-Hx - 0.02, -Hy - 0.02, SizeZ - 0.16}, {Hx + 0.02, -Hy + 0.04, SizeZ + 0.01});
            Out.Box(Mat(M::TimberDark), {-Hx - 0.02, Hy - 0.04, SizeZ - 0.16}, {Hx + 0.02, Hy + 0.02, SizeZ + 0.01});
        }
        else
        {
            Out.Box(Mat(M::Stone), {-Hx, -Hy, 0.0}, {Hx, Hy, SizeZ - 0.14});
            Out.Box(Mat(M::Paving), {-Hx - 0.05, -Hy - 0.05, SizeZ - 0.14}, {Hx + 0.05, Hy + 0.05, SizeZ});
        }
        return Generated[Name] = Emit(Name, WriteGltf(Out, Generator));
    }

    std::string Kit::GroundTile(const std::string_view Style)
    {
        const std::string Name = "SM_Art_GroundTile_" + std::string(Style);
        if (const auto Found = Generated.find(Name); Found != Generated.end()) return Found->second;
        Mesh Out(Name);
        const double H = GroundTileSize / 2.0;
        if (Style == "paving")
        {
            Out.Box(Mat(M::Paving), {-H, -H, -0.5}, {H, H, 0.0});
        }
        else
        {
            Out.Box(Mat(M::Grass), {-H, -H, -0.5}, {H, H, 0.0});
            Rng R(66u);
            for (int I = 0; I < 7; ++I)
            {
                // one draw per statement: the order of evaluation of call arguments is unspecified
                const double X = R.Range(-H + 3, H - 3), Y = R.Range(-H + 3, H - 3), Radius = R.Range(0.35, 1.0), Height = R.Range(0.25, 0.7);
                const std::uint32_t Seed = R.Next();
                Rock(Out, Mat(M::Rock), {X, Y, 0.0}, Radius, Height, Seed);
            }
        }
        return Generated[Name] = Emit(Name, WriteGltf(Out, Generator));
    }

    std::string Kit::HarborEdge(const double Length)
    {
        const std::string Name = "SM_Art_HarborEdge_" + Mm(Length);
        if (const auto Found = Generated.find(Name); Found != Generated.end()) return Found->second;
        Mesh Out(Name);
        const double H = Length / 2.0;
        // quay wall along the west edge (x = 0 is the quay face), harbour water beyond, bollards
        Out.Box(Mat(M::Stone), {-0.4, -H, -2.4}, {0.6, H, 0.02});
        Out.Box(Mat(M::StoneDark), {-0.45, -H, -0.06}, {0.65, H, 0.04});
        Out.Quad(Mat(M::HarborWater), {-160.0, -H - 80.0, -0.45}, {-0.4, -H - 80.0, -0.45}, {-0.4, H + 80.0, -0.45}, {-160.0, H + 80.0, -0.45});
        for (double Y = -H + 4.0; Y < H - 2.0; Y += 9.0)
        {
            Out.Tube(Mat(M::Iron), {0.3, Y, 0.0}, {0.3, Y, 0.55}, 0.16, 0.13, 10);
            Out.Tube(Mat(M::Iron), {0.3, Y, 0.55}, {0.3, Y, 0.62}, 0.2, 0.2, 10);
        }
        return Generated[Name] = Emit(Name, WriteGltf(Out, Generator));
    }

    std::string Kit::TownRow(const double Length, const bool FacingNorth)
    {
        const std::string Name = "SM_Art_TownRow_" + Mm(Length) + (FacingNorth ? "N" : "S");
        if (const auto Found = Generated.find(Name); Found != Generated.end()) return Found->second;
        Rng R(FacingNorth ? 55u : 56u);
        std::vector<std::pair<V3, V3>> Houses;
        double X = -Length / 2.0;
        while (X < Length / 2.0 - 4.0)
        {
            const double W = std::min(R.Range(5.0, 7.5), Length / 2.0 - X);
            Houses.push_back({{X, -3.0, 0.0}, {X + W, 3.0, R.Range(5.5, 9.0)}});
            X += W + 0.2;
        }
        Mesh Row = DressBoxes(Name, Houses);
        Mesh Out(Name);
        Out.Push(FacingNorth ? Xf::RotateZ(Pi) : Xf{});  // DressBoxes puts doors on -Y
        Out.Append(Row);
        Out.Pop();
        return Generated[Name] = Emit(Name, WriteGltf(Out, Generator));
    }

    std::string Kit::BoardingStair(const double Distance, const double Rise, const double Drop, std::string& OutCollisionGltf)
    {
        const std::string Name = "SM_Art_BoardingStair_" + Mm(Distance) + "x" + Mm(Rise) + "x" + Mm(Drop);
        Mesh Out(Name);
        Mesh Col(Name + "_Collision");
        Material C;
        C.Name = "Collision";
        const double Half = 0.55, Landing = 0.25;
        const int Up = static_cast<int>(std::ceil(Rise / 0.26));
        const double RiseUp = Rise / Up, TreadUp = (Distance - Landing) / Up;
        for (int K = 0; K < Up; ++K)
        {
            const double Y0 = K * TreadUp, Y1 = (K + 1) * TreadUp, Top = (K + 1) * RiseUp;
            Out.Box(Mat(M::DeckWorn), {-Half, Y0, Top - 0.06}, {Half, Y1 + 0.04, Top});
            Col.Box(C, {-Half, Y0, 0.0}, {Half, Y1, Top});
        }
        Out.Box(Mat(M::DeckWorn), {-Half - 0.05, Distance - Landing, Rise - 0.08}, {Half + 0.05, Distance + Landing, Rise});
        Col.Box(C, {-Half, Distance - Landing, Rise - 0.3}, {Half, Distance + Landing, Rise});
        const int Down = std::max(1, static_cast<int>(std::ceil(Drop / 0.28)));
        const double RiseDown = Drop / Down, TreadDown = 0.35;
        for (int K = 0; K + 1 < Down; ++K)
        {
            const double Y0 = Distance + Landing + K * TreadDown, Top = Rise - (K + 1) * RiseDown;
            Out.Box(Mat(M::DeckWorn), {-Half, Y0 - 0.04, Top - 0.06}, {Half, Y0 + TreadDown, Top});
            Col.Box(C, {-Half, Y0, Rise - Drop}, {Half, Y0 + TreadDown, Top});
        }
        // stringers, posts and hand ropes
        for (const double X : {-Half - 0.04, Half + 0.04})
        {
            const double Len = std::sqrt(Distance * Distance + Rise * Rise);
            OrientedBox(Out, Mat(M::TimberDark), {X, 0.0, 0.0}, 1.0, 0.0, {0.0, 0.0, 0.0});
            Xf T;
            const double Cs = (Distance - Landing) / std::sqrt((Distance - Landing) * (Distance - Landing) + Rise * Rise);
            const double Sn = Rise / std::sqrt((Distance - Landing) * (Distance - Landing) + Rise * Rise);
            T.M = {1, 0, 0, X, 0, Cs, -Sn, 0, 0, Sn, Cs, 0};
            Out.Push(T);
            Out.Box(Mat(M::TimberDark), {-0.04, 0.0, -0.22}, {0.04, Len - 0.1, 0.0});
            Out.Pop();
            Out.Tube(Mat(M::TimberDark), {X, 0.05, 0.0}, {X, 0.05, 1.0}, 0.04, 0.04, 6);
            Out.Tube(Mat(M::TimberDark), {X, Distance, Rise}, {X, Distance, Rise + 1.0}, 0.04, 0.04, 6);
            Out.Tube(Mat(M::Rope), {X, 0.05, 0.95}, {X, Distance, Rise + 0.95}, 0.02, 0.02, 4);
        }
        OutCollisionGltf = WriteCollisionGltf(Col);
        if (const auto Found = Generated.find(Name); Found != Generated.end()) return Found->second;
        return Generated[Name] = Emit(Name, WriteGltf(Out, Generator));
    }

    std::string Kit::Dress(const std::string_view SourceRelative, const std::string_view Gltf)
    {
        std::string Stem(SourceRelative.substr(SourceRelative.find_last_of('/') + 1));
        Stem = Stem.substr(0, Stem.size() - 5);  // .gltf
        if (Stem.rfind("SM_", 0) == 0) Stem = Stem.substr(3);
        if (Stem.size() > 6 && Stem.compare(Stem.size() - 6, 6, "_Alpha") == 0) Stem = Stem.substr(0, Stem.size() - 6);
        const std::string Model = "SM_Art_" + Stem;
        if (const auto Found = Generated.find(Model); Found != Generated.end()) return Found->second;
        const std::vector<std::pair<V3, V3>> Boxes = ReadBoxes(Gltf);
        if (Boxes.empty()) return {};
        return Generated[Model] = Emit(Model, WriteGltf(DressBoxes(Model, Boxes), Generator));
    }

    std::string Kit::DressTerrain(const std::string_view Name, const std::string_view GreyboxGltf, const std::vector<std::pair<double, double>>& KeepClear)
    {
        const std::string Model = "SM_Art_" + std::string(Name);
        if (const auto Found = Generated.find(Model); Found != Generated.end()) return Found->second;
        std::vector<V3> P;
        if (!ReadPositions(GreyboxGltf, P)) return {};
        // regular grid of the greybox heightfield
        V3 Lo{1e30, 1e30, 0}, Hi{-1e30, -1e30, 0};
        for (const V3& Q : P)
        {
            Lo = {std::min(Lo.X, Q.X), std::min(Lo.Y, Q.Y), 0};
            Hi = {std::max(Hi.X, Q.X), std::max(Hi.Y, Q.Y), 0};
        }
        int N = 2;
        while (N * N < static_cast<int>(P.size())) ++N;
        std::vector<double> Grid(static_cast<std::size_t>(N * N), 0.0);
        const double Dx = (Hi.X - Lo.X) / (N - 1), Dy = (Hi.Y - Lo.Y) / (N - 1);
        for (const V3& Q : P)
        {
            const long I = std::lround((Q.X - Lo.X) / Dx), J = std::lround((Q.Y - Lo.Y) / Dy);
            if (I >= 0 && J >= 0 && I < N && J < N) Grid[static_cast<std::size_t>(J * N + I)] = Q.Z;
        }
        const auto Height = [&](double X, double Y)
        {
            const double Fx = Clamp((X - Lo.X) / Dx, 0.0, N - 1.000001), Fy = Clamp((Y - Lo.Y) / Dy, 0.0, N - 1.000001);
            const int I = static_cast<int>(std::floor(Fx)), J = static_cast<int>(std::floor(Fy));
            const double Tx = Fx - I, Ty = Fy - J;
            const auto G = [&](int A, int B) { return Grid[static_cast<std::size_t>(B * N + A)]; };
            return Lerp(Lerp(G(I, J), G(I + 1, J), Tx), Lerp(G(I, J + 1), G(I + 1, J + 1), Tx), Ty);
        };
        const auto Detail = [](double X, double Y, std::uint32_t Seed)
        {
            const int Xi = static_cast<int>(std::floor(X * 8.0)) & 1023, Yi = static_cast<int>(std::floor(Y * 8.0)) & 1023;
            return Noise(Xi, Yi, 1024, 64, Seed) / 65535.0;
        };

        Mesh Out(Model);
        const double Step = 1.0;
        const int Cx = static_cast<int>((Hi.X - Lo.X) / Step), Cy = static_cast<int>((Hi.Y - Lo.Y) / Step);
        const auto Vertex_ = [&](int I, int J, const Material& Mt)
        {
            const double X = Lo.X + I * Step, Y = Lo.Y + J * Step;
            const double Z = Height(X, Y) + 0.03 * (Detail(X, Y, 5u) - 0.5);
            const double Gx = Height(X + 0.5, Y) - Height(X - 0.5, Y), Gy = Height(X, Y + 0.5) - Height(X, Y - 0.5);
            return Vertex{{X, Y, Z}, Normalize({-Gx, -Gy, 1.0}), X / Mt.MetresPerTile, Y / Mt.MetresPerTile};
        };
        std::vector<std::vector<std::pair<int, int>>> Cells(static_cast<std::size_t>(M::Count));
        for (int J = 0; J < Cy; ++J)
        {
            for (int I = 0; I < Cx; ++I)
            {
                for (int Half = 0; Half < 2; ++Half)
                {
                    // lower-right triangle (A B C) then upper-left (A C D), each classified at its centroid
                    const double Xc = Lo.X + (I + (Half == 0 ? 0.667 : 0.333)) * Step, Yc = Lo.Y + (J + (Half == 0 ? 0.333 : 0.667)) * Step;
                    const double Zc = Height(Xc, Yc);
                    const double Slope = std::fabs(Height(Xc + 0.5, Yc) - Height(Xc - 0.5, Yc)) + std::fabs(Height(Xc, Yc + 0.5) - Height(Xc, Yc - 0.5));
                    const double Jitter = 0.6 * (Detail(Xc * 0.5, Yc * 0.5, 9u) - 0.5) + 0.25 * (Detail(Xc * 2.0, Yc * 2.0, 10u) - 0.5);
                    M Id = M::Sand;
                    if (Slope > 0.45) Id = M::Rock;
                    else if (Zc + Jitter < 0.3) Id = M::WetSand;
                    else if (Zc + Jitter > 1.9) Id = M::Grass;
                    Cells[static_cast<std::size_t>(Id)].push_back({I, J * 2 + Half});
                }
            }
        }
        // one indexed part per material, vertices shared between the cells of that material
        for (std::size_t Id = 0; Id < Cells.size(); ++Id)
        {
            if (Cells[Id].empty()) continue;
            const Material& Mt = Mat(static_cast<M>(Id));
            std::map<std::pair<int, int>, std::uint32_t> Index;
            std::vector<Vertex> Vertices;
            std::vector<std::uint32_t> Indices;
            const auto At = [&](int I, int J)
            {
                const auto Found = Index.find({I, J});
                if (Found != Index.end()) return Found->second;
                const auto New = static_cast<std::uint32_t>(Vertices.size());
                Vertices.push_back(Vertex_(I, J, Mt));
                Index[{I, J}] = New;
                return New;
            };
            for (const auto& [I, Code] : Cells[Id])
            {
                const int J = Code / 2;
                const std::uint32_t A = At(I, J), C = At(I + 1, J + 1);
                if (Code % 2 == 0) Indices.insert(Indices.end(), {A, At(I + 1, J), C});
                else Indices.insert(Indices.end(), {A, C, At(I, J + 1)});
            }
            Out.Indexed(Mt, Vertices, Indices);
        }

        // scatter, clear of the authored route (anchors and the legs between them)
        const auto Clear = [&](double X, double Y, double Radius)
        {
            for (std::size_t K = 0; K < KeepClear.size(); ++K)
            {
                const auto& A = KeepClear[K];
                const auto& B = KeepClear[K + 1 < KeepClear.size() ? K + 1 : K];
                if (SegmentDistance(X, Y, A.first, A.second, B.first, B.second) < Radius) return false;
            }
            return true;
        };
        Rng R(HashString(Name));
        int Palms = 0;
        for (int Try = 0; Try < 400 && Palms < 34; ++Try)
        {
            const double X = R.Range(Lo.X + 3, Hi.X - 3), Y = R.Range(Lo.Y + 3, Hi.Y - 3);
            const double Z = Height(X, Y);
            if (Z < 1.0 || !Clear(X, Y, 7.0)) continue;
            Out.Push(Xf::Translate({X, Y, Z}));
            const double PalmHeight = R.Range(6.5, 10.5);
            const std::uint32_t PalmSeed = R.Next();
            Out.Append(BuildPalm(PalmHeight, PalmSeed));
            Out.Pop();
            ++Palms;
        }
        for (int Try = 0; Try < 300; ++Try)
        {
            const double X = R.Range(Lo.X + 2, Hi.X - 2), Y = R.Range(Lo.Y + 2, Hi.Y - 2);
            const double Z = Height(X, Y);
            if (Z < -0.6 || !Clear(X, Y, 3.5)) continue;
            if (Try % 3 == 0)
            {
                const double Radius = R.Range(0.3, 1.3), Height = R.Range(0.25, 0.9);
                const std::uint32_t Seed = R.Next();
                Rock(Out, Mat(M::Rock), {X, Y, Z}, Radius, Height, Seed);
            }
            else if (Try % 3 == 1 && Z > 0.2 && Z < 1.6)
            {
                const double A = R.Range(0, Pi), L = R.Range(1.6, 3.8), Rad = R.Range(0.1, 0.2);
                const V3 D{Cos(A) * L / 2, Sin(A) * L / 2, 0};
                Out.Tube(Mat(M::Driftwood), V3{X, Y, Z + Rad * 0.6} - D, V3{X, Y, Z + Rad * 0.6} + D, Rad, Rad * 0.7, 7);
            }
            else if (Z > 1.6)
            {
                // low shrub: a ring of short fronds
                for (int F = 0; F < 7; ++F)
                {
                    const double A = 2.0 * Pi * F / 7 + R.Range(0, 0.5);
                    const V3 Dir{Cos(A), Sin(A), 0}, Side{-Sin(A), Cos(A), 0};
                    const V3 Base{X, Y, Z - 0.05};
                    const V3 Tip = Base + Dir * 0.9 + V3{0, 0, 0.55};
                    Out.Card(Mat(M::Frond), Base - Side * 0.18, Base + Side * 0.18, Tip + Side * 0.18, Tip - Side * 0.18);
                }
            }
        }
        return Generated[Model] = Emit(Model, WriteGltf(Out, Generator));
    }
}
