#include "Gltf.h"
#include "Hash.h"
#include "Json.h"
#include "O3deIds.h"

#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using namespace DarkArisen::Tools;
namespace fs = std::filesystem;

namespace
{
    int Failures = 0;

    void Check(const bool Condition, const char* What)
    {
        if (!Condition)
        {
            ++Failures;
            std::cerr << "FAILED: " << What << "\n";
        }
    }

    std::string Read(const fs::path& Path)
    {
        std::ifstream Stream(Path, std::ios::binary);
        return std::string((std::istreambuf_iterator<char>(Stream)), std::istreambuf_iterator<char>());
    }

    std::string Base64(const std::vector<std::uint8_t>& Bytes)
    {
        static constexpr char Alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string Out;
        for (std::size_t Offset = 0; Offset < Bytes.size(); Offset += 3)
        {
            const std::size_t Remaining = Bytes.size() - Offset;
            std::uint32_t Triple = static_cast<std::uint32_t>(Bytes[Offset]) << 16;
            if (Remaining > 1) Triple |= static_cast<std::uint32_t>(Bytes[Offset + 1]) << 8;
            if (Remaining > 2) Triple |= Bytes[Offset + 2];
            Out += Alphabet[(Triple >> 18) & 63];
            Out += Alphabet[(Triple >> 12) & 63];
            Out += Remaining > 1 ? Alphabet[(Triple >> 6) & 63] : '=';
            Out += Remaining > 2 ? Alphabet[Triple & 63] : '=';
        }
        return Out;
    }

    std::vector<std::uint8_t> Unbase64(const std::string& Text)
    {
        std::vector<std::uint8_t> Out;
        std::uint32_t Accumulator = 0;
        int Bits = 0;
        for (const char Character : Text)
        {
            const char* Found = std::strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", Character);
            if (Character == '=' || !Found) break;
            Accumulator = (Accumulator << 6) | static_cast<std::uint32_t>(Found - "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
            Bits += 6;
            if (Bits >= 8)
            {
                Bits -= 8;
                Out.push_back(static_cast<std::uint8_t>((Accumulator >> Bits) & 0xFF));
            }
        }
        return Out;
    }

    /** One triangle, Z-up source frame: (0,0,0) (2,0,0) (0,1,3). */
    std::string TriangleGltf(const std::string& Extra = "")
    {
        std::vector<std::uint8_t> Bytes(3 * 12 + 6 + 2, 0);
        const float Points[9] = {0, 0, 0, 2, 0, 0, 0, 1, 3};
        std::memcpy(Bytes.data(), Points, sizeof(Points));
        const std::uint16_t Indices[3] = {0, 1, 2};
        std::memcpy(Bytes.data() + 36, Indices, sizeof(Indices));
        return R"({"asset":{"version":"2.0","generator":"test"},)" + Extra +
            R"("buffers":[{"byteLength":44,"uri":"data:application/octet-stream;base64,)" + Base64(Bytes) + R"("}],)"
            R"("bufferViews":[{"buffer":0,"byteOffset":0,"byteLength":36},{"buffer":0,"byteOffset":36,"byteLength":6}],)"
            R"("accessors":[{"bufferView":0,"componentType":5126,"count":3,"type":"VEC3","min":[0,0,0],"max":[2,1,3]},)"
            R"({"bufferView":1,"componentType":5123,"count":3,"type":"SCALAR"}],)"
            R"("meshes":[{"primitives":[{"attributes":{"POSITION":0},"indices":1}]}],"nodes":[{"mesh":0}],"scenes":[{"nodes":[0]}],"scene":0})";
    }

    /** O3DE's AssImp import with AssImpReadRootTransform: glTF g -> (-g.x, g.z, g.y). */
    Vec3d O3deImport(const Vec3d& G) { return {-G.X, G.Z, G.Y}; }
}

int main()
{
    // Hash primitives: published test vectors.
    Check(ToHex(Sha1("abc").data(), 20) == "a9993e364706816aba3e25717850c26c9cd0d89d", "SHA1(abc)");
    Check(ToHex(Sha1("").data(), 20) == "da39a3ee5e6b4b0d3255bfef95601890afd80709", "SHA1(empty)");
    Check(ToHex(Sha1(std::string(1000, 'a')).data(), 20) == "291e9a6c66994949b57ba5e650361e98fc36b1ba", "SHA1 multi-block");
    Check(Crc32("123456789") == 0xCBF43926u, "CRC32 check value");
    Check(Crc32("ABC", true) == Crc32("abc"), "CRC32 lowercase folding");

    // O3DE identifiers, checked against a prefab committed in the pinned O3DE 2605.0 engine
    // (TestDependenciesLevel.prefab: groundplane_512x512m.fbx model).
    Check(SourceAssetUuid("Objects/Groudplane/groundplane_512x512m.fbx") == "{0CD745C0-6AA8-569A-A68A-73A3270986C4}",
        "source UUID matches the O3DE Asset Processor");
    Check(AtomModelSubId("groundplane_512x512m") == 277889906u, "Atom model sub-id matches the O3DE model builder");

    // JSON writer: deterministic, round-trips numbers exactly.
    JsonValue Parsed;
    std::string Error;
    Check(JsonReader::Parse(R"({"b":[1,0.1,-2.5e-7,12345678901234],"a":"q\"\\\n"})", Parsed, Error), "JSON parse for writer");
    const std::string Written = JsonWriter::Write(Parsed);
    Check(Written == R"({"a":"q\"\\\n","b":[1,0.1,-2.5e-07,12345678901234]})", "JSON writer is compact, ordered and exact");
    JsonValue Again;
    Check(JsonReader::Parse(JsonWriter::Write(Parsed, 2), Again, Error) && JsonWriter::Write(Again) == Written, "pretty JSON round-trips");

    // Frame: port (-Y in Unreal) stays on the left when facing +X in right-handed O3DE.
    const Vec3d Port = LayoutToWorld({100.0, -340.0, 220.0});
    Check(Port.X == 1.0 && Port.Y == 3.4 && Port.Z == 2.2, "layout anchors mirror Y into O3DE metres");

    // Greybox conversion of a synthetic triangle.
    std::string Converted;
    GltfConversionStats Stats;
    Check(ConvertGreyboxGltf(TriangleGltf(), Converted, Stats, Error), "triangle converts");
    Check(Stats.Vertices == 3 && Stats.Triangles == 1, "conversion stats");
    JsonValue Out;
    Check(JsonReader::Parse(Converted, Out, Error), "converted glTF is JSON");
    const std::string Uri = Out.Find("buffers")->Items[0].Find("uri")->Text;
    const std::vector<std::uint8_t> Bytes = Unbase64(Uri.substr(Uri.find(',') + 1));
    Check(Bytes.size() == 44, "buffer length preserved");
    float Points[9] = {};
    std::uint16_t Indices[3] = {};
    std::memcpy(Points, Bytes.data(), sizeof(Points));
    std::memcpy(Indices, Bytes.data() + 36, sizeof(Indices));
    const Vec3d Source[3] = {{0, 0, 0}, {2, 0, 0}, {0, 1, 3}};
    bool Placed = true;
    for (int Vertex = 0; Vertex < 3; ++Vertex)
    {
        const Vec3d World = O3deImport({Points[Vertex * 3], Points[Vertex * 3 + 1], Points[Vertex * 3 + 2]});
        Placed = Placed && World.X == Source[Vertex].X && World.Y == -Source[Vertex].Y && World.Z == Source[Vertex].Z;
    }
    Check(Placed, "O3DE import of the converted mesh lands on (x, -y, z), matching the layout anchors");
    Check(Indices[0] == 0 && Indices[1] == 2 && Indices[2] == 1, "winding reversed for the mirror");
    const JsonValue& Accessor = Out.Find("accessors")->Items[0];
    Check(Accessor.Find("min")->Items[0].Number == -2.0 && Accessor.Find("max")->Items[1].Number == 3.0 &&
              Accessor.Find("min")->Items[2].Number == -1.0,
        "POSITION min/max recomputed in glTF space");
    Check(Out.Find("asset")->Find("generator")->Text.find("frame conversion") != std::string::npos, "generator records the conversion");

    // Fail closed.
    Check(!ConvertGreyboxGltf(TriangleGltf(R"("skins":[{"joints":[0]}],)"), Converted, Stats, Error), "skinned glTF rejected");
    Check(!ConvertGreyboxGltf(TriangleGltf(R"("extensionsUsed":["KHR_draco_mesh_compression"],)"), Converted, Stats, Error),
        "extensions rejected");
    std::string Broken = TriangleGltf();
    Broken.replace(Broken.find("\"count\":3,\"type\":\"VEC3\""), 23, "\"count\":9,\"type\":\"VEC3\"");
    Check(!ConvertGreyboxGltf(Broken, Converted, Stats, Error), "accessor past its bufferView rejected");

    // The committed Outer Reef greybox converts and keeps its triangle count.
    const fs::path Reef = fs::path(DARKARISEN_REPO_ROOT) / "ContentSource/World/Moran/OuterReef/SM_OuterReef_Alpha.gltf";
    Check(ConvertGreyboxGltf(Read(Reef), Converted, Stats, Error) && Stats.Vertices == 48 && Stats.Triangles > 0,
        "ContentSource SM_OuterReef_Alpha converts");
    const fs::path Jake = fs::path(DARKARISEN_REPO_ROOT) / "ContentSource/Characters/Jake/SK_Jake_Alpha.gltf";
    Check(!ConvertGreyboxGltf(Read(Jake), Converted, Stats, Error), "skinned Jake greybox is left to the actor pipeline");

    std::cout << (Failures == 0 ? "conversion tests passed\n" : "conversion tests FAILED\n");
    return Failures == 0 ? 0 : 1;
}
