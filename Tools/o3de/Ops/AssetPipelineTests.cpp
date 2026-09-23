#include "AssetPipeline.h"
#include "Hash.h"
#include "Json.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

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

    void AppendU32(std::string& Out, const std::uint32_t Value)
    {
        for (int Shift = 0; Shift < 32; Shift += 8) Out += static_cast<char>((Value >> Shift) & 0xFF);
    }

    /** Builds a minimal but structurally complete GLB 2.0 container. */
    std::string MakeGlb(const std::string& Json, const std::uint32_t BinaryBytes)
    {
        std::string JsonChunk = Json;
        while (JsonChunk.size() % 4 != 0) JsonChunk += ' ';
        const std::uint32_t Total = 12 + 8 + static_cast<std::uint32_t>(JsonChunk.size()) + 8 + BinaryBytes;
        std::string Out;
        AppendU32(Out, 0x46546C67);
        AppendU32(Out, 2);
        AppendU32(Out, Total);
        AppendU32(Out, static_cast<std::uint32_t>(JsonChunk.size()));
        AppendU32(Out, 0x4E4F534A);
        Out += JsonChunk;
        AppendU32(Out, BinaryBytes);
        AppendU32(Out, 0x004E4942);
        Out += std::string(BinaryBytes, '\0');
        return Out;
    }

    const std::string ReleaseJson =
        R"({"asset":{"version":"2.0"},"buffers":[{"byteLength":64}],"accessors":[{"count":1200},{"count":1200}],)"
        R"("meshes":[{"primitives":[{"attributes":{"POSITION":0,"NORMAL":1,"TEXCOORD_0":1}}]}],)"
        R"("materials":[{"name":"M_Hull"}],"images":[{"uri":"x"}],"textures":[{"source":0}]})";

    void Write(const fs::path& Path, const std::string& Bytes)
    {
        fs::create_directories(Path.parent_path());
        std::ofstream(Path, std::ios::binary) << Bytes;
    }
}

int main()
{
    // Reference vectors (RFC 1321, FIPS 180-2).
    Check(Md5Hex("abc") == "900150983cd24fb0d6963f7d28e17f72", "MD5(abc)");
    Check(Md5Hex("") == "d41d8cd98f00b204e9800998ecf8427e", "MD5(empty)");
    Check(Sha256Hex("abc") == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad", "SHA256(abc)");
    Check(Sha256Hex(std::string(1000, 'a')).size() == 64, "SHA256 multi-block");

    JsonValue Parsed;
    std::string Error;
    Check(JsonReader::Parse(R"({"a":[1,2.5,-3e2],"b":"xé","c":null})", Parsed, Error), "JSON parse");
    Check(!JsonReader::Parse(R"({"a":1,"a":2})", Parsed, Error), "JSON duplicate keys rejected");
    Check(!JsonReader::Parse(R"({"a":1} x)", Parsed, Error), "JSON trailing garbage rejected");

    GlbStats Stats;
    const std::string Release = MakeGlb(ReleaseJson, 64);
    Check(ValidateGlb(Release, Stats, Error), "release GLB validates");
    Check(Stats.PositionVertices == 1200 && Stats.Normals && Stats.Uv0 && Stats.Materials == 1, "GLB stats");
    std::string Missing;
    Check(IsReleaseGrade(Stats, Missing), "release grade");

    const std::string Greybox = MakeGlb(R"({"buffers":[{"byteLength":8}],"accessors":[{"count":8}],)"
                                        R"("meshes":[{"primitives":[{"attributes":{"POSITION":0}}]}]})", 8);
    Check(ValidateGlb(Greybox, Stats, Error), "greybox GLB is structurally valid");
    Check(!IsReleaseGrade(Stats, Missing), "greybox GLB is not release grade");

    Check(!ValidateGlb(Release.substr(0, Release.size() - 4), Stats, Error), "truncated GLB rejected");
    Check(!ValidateGlb(MakeGlb(R"({"buffers":[{"byteLength":999}]})", 8), Stats, Error), "short buffer rejected");
    Check(!ValidateGlb("glTF", Stats, Error), "tiny file rejected");

    // End-to-end import with provenance into a scratch repository layout.
    const fs::path Root = fs::temp_directory_path() / "darkarisen-asset-pipeline-test";
    fs::remove_all(Root);
    const fs::path Glb = Root / "download" / "ship.glb";
    Write(Glb, Release);
    const std::string Manifest = std::string(R"({"assetId":"ship.test.alpha.higgsfield3d.r1","sourceTool":"Higgsfield 3D Jutsu",)") +
        R"("projectId":"p","revision":1,"operationId":"op","glb":{"sizeBytes":)" + std::to_string(Release.size()) +
        R"(,"etag":")" + Md5Hex(Release) + R"("}})";
    Write(Root / "ContentSource" / "Ships" / "Test" / "Higgsfield3DSource.json", Manifest);

    ImportRequest Request;
    Request.RepoRoot = Root;
    Request.ManifestPath = Root / "ContentSource" / "Ships" / "Test" / "Higgsfield3DSource.json";
    Request.GlbPath = Glb;
    Request.ImportDateUtc = "2026-09-23T00:00:00Z";
    ImportResult Result;
    Check(ImportProviderGlb(Request, Result, Error), "import succeeds for the exact revision");
    Check(fs::exists(Result.Destination) && fs::file_size(Result.Destination) == Release.size(), "GLB copied");
    Check(Result.Sha256 == Sha256Hex(Release), "provenance sha256");
    JsonValue Provenance;
    std::ifstream ProvenanceStream(Result.ProvenancePath);
    const std::string ProvenanceText((std::istreambuf_iterator<char>(ProvenanceStream)), std::istreambuf_iterator<char>());
    Check(JsonReader::Parse(ProvenanceText, Provenance, Error), "provenance is valid JSON");
    Check(Provenance.Find("provider") && Provenance.Find("provider")->Text == "Higgsfield", "provenance provider");

    Request.MinimumVertices = 5000;
    Check(!ImportProviderGlb(Request, Result, Error), "vertex floor enforced");
    Request.MinimumVertices = 0;

    std::string Tampered = Release;
    Tampered[Tampered.size() - 1] = 1;
    Write(Glb, Tampered);
    Check(!ImportProviderGlb(Request, Result, Error) && Error.find("ETag") != std::string::npos, "wrong revision rejected");

    Write(Glb, Release + "x");
    Check(!ImportProviderGlb(Request, Result, Error) && Error.find("size") != std::string::npos, "size mismatch rejected");

    fs::remove_all(Root);
    std::cout << (Failures == 0 ? "asset pipeline tests passed\n" : "asset pipeline tests FAILED\n");
    return Failures == 0 ? 0 : 1;
}
