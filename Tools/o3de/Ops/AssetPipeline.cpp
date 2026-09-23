#include "AssetPipeline.h"

#include "Hash.h"
#include "Json.h"

#include <cstdint>
#include <cstring>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace DarkArisen::Tools
{
    namespace
    {
        constexpr std::uint32_t GlbMagic = 0x46546C67; // "glTF"
        constexpr std::uint32_t JsonChunk = 0x4E4F534A;
        constexpr std::uint32_t BinChunk = 0x004E4942;
        constexpr const char* ImporterVersion = "DarkArisenO3DE-import-glb/1";

        std::uint32_t ReadU32(const std::string_view Bytes, const std::size_t Offset)
        {
            std::uint32_t Value = 0;
            for (int Index = 3; Index >= 0; --Index)
            {
                Value = (Value << 8) | static_cast<unsigned char>(Bytes[Offset + static_cast<std::size_t>(Index)]);
            }
            return Value;
        }

        bool ReadFile(const fs::path& Path, std::string& Out)
        {
            std::ifstream Stream(Path, std::ios::binary);
            if (!Stream)
            {
                return false;
            }
            std::ostringstream Buffer;
            Buffer << Stream.rdbuf();
            Out = Buffer.str();
            return true;
        }

        std::string JsonEscape(const std::string& Value)
        {
            std::string Out;
            for (const char Character : Value)
            {
                switch (Character)
                {
                case '"': Out += "\\\""; break;
                case '\\': Out += "\\\\"; break;
                case '\n': Out += "\\n"; break;
                case '\t': Out += "\\t"; break;
                default: Out += Character; break;
                }
            }
            return Out;
        }

        const std::string* StringMember(const JsonValue& Object, const std::string& Key)
        {
            const JsonValue* Value = Object.Find(Key);
            return Value && Value->IsString() && !Value->Text.empty() ? &Value->Text : nullptr;
        }

        bool IsSafeAssetId(const std::string& Id)
        {
            if (Id.empty() || Id.size() > 96 || Id.front() == '.')
            {
                return false;
            }
            for (const char Character : Id)
            {
                const bool Allowed = (Character >= 'a' && Character <= 'z') || (Character >= '0' && Character <= '9') ||
                    Character == '.' || Character == '_' || Character == '-';
                if (!Allowed)
                {
                    return false;
                }
            }
            return Id.find("..") == std::string::npos;
        }
    }

    bool ValidateGlb(const std::string_view Bytes, GlbStats& OutStats, std::string& OutError)
    {
        OutStats = {};
        OutStats.Bytes = Bytes.size();
        if (Bytes.size() < 20 || ReadU32(Bytes, 0) != GlbMagic || ReadU32(Bytes, 4) != 2 || ReadU32(Bytes, 8) != Bytes.size())
        {
            OutError = "not a complete glTF 2.0 binary (magic/version/length mismatch)";
            return false;
        }
        std::string_view JsonText;
        std::size_t BinaryLength = 0;
        bool HasBinary = false;
        std::size_t Offset = 12;
        while (Offset < Bytes.size())
        {
            if (Offset + 8 > Bytes.size())
            {
                OutError = "truncated GLB chunk header";
                return false;
            }
            const std::uint32_t Length = ReadU32(Bytes, Offset);
            const std::uint32_t Kind = ReadU32(Bytes, Offset + 4);
            Offset += 8;
            if (Length > Bytes.size() - Offset)
            {
                OutError = "truncated GLB chunk";
                return false;
            }
            if (Kind == JsonChunk && JsonText.empty()) JsonText = Bytes.substr(Offset, Length);
            else if (Kind == BinChunk && !HasBinary) { HasBinary = true; BinaryLength = Length; }
            Offset += Length;
        }
        if (JsonText.empty() || !HasBinary)
        {
            OutError = "GLB JSON or BIN chunk missing";
            return false;
        }
        while (!JsonText.empty() && (JsonText.back() == ' ' || JsonText.back() == '\0'))
        {
            JsonText.remove_suffix(1);
        }
        JsonValue Document;
        std::string ParseError;
        if (!JsonReader::Parse(JsonText, Document, ParseError) || !Document.IsObject())
        {
            OutError = "GLB JSON chunk invalid: " + ParseError;
            return false;
        }
        const JsonValue* Buffers = Document.Find("buffers");
        if (!Buffers || !Buffers->IsArray() || Buffers->Items.size() != 1)
        {
            OutError = "exactly one GLB binary buffer is required";
            return false;
        }
        const JsonValue* Declared = Buffers->Items.front().Find("byteLength");
        if (!Declared || !Declared->IsNumber() || Declared->Number < 0 || Declared->Number > static_cast<double>(BinaryLength))
        {
            OutError = "binary buffer shorter than declared";
            return false;
        }

        const auto Count = [&Document](const char* Key)
        {
            const JsonValue* Value = Document.Find(Key);
            return Value && Value->IsArray() ? static_cast<int>(Value->Items.size()) : 0;
        };
        OutStats.Meshes = Count("meshes");
        OutStats.Materials = Count("materials");
        OutStats.Images = Count("images");
        OutStats.Textures = Count("textures");
        OutStats.Animations = Count("animations");
        OutStats.Skinned = Count("skins") > 0;

        const JsonValue* Accessors = Document.Find("accessors");
        const JsonValue* Meshes = Document.Find("meshes");
        if (Meshes && Meshes->IsArray())
        {
            for (const JsonValue& Mesh : Meshes->Items)
            {
                const JsonValue* Primitives = Mesh.Find("primitives");
                if (!Primitives || !Primitives->IsArray()) continue;
                for (const JsonValue& Primitive : Primitives->Items)
                {
                    const JsonValue* Attributes = Primitive.Find("attributes");
                    if (!Attributes || !Attributes->IsObject()) continue;
                    OutStats.Normals = OutStats.Normals || Attributes->Find("NORMAL") != nullptr;
                    OutStats.Uv0 = OutStats.Uv0 || Attributes->Find("TEXCOORD_0") != nullptr;
                    const JsonValue* Position = Attributes->Find("POSITION");
                    if (Position && Position->IsNumber() && Accessors && Accessors->IsArray())
                    {
                        const auto Index = static_cast<std::size_t>(Position->Number);
                        if (Index < Accessors->Items.size())
                        {
                            const JsonValue* AccessorCount = Accessors->Items[Index].Find("count");
                            if (AccessorCount && AccessorCount->IsNumber())
                            {
                                OutStats.PositionVertices += static_cast<long long>(AccessorCount->Number);
                            }
                        }
                    }
                }
            }
        }
        return true;
    }

    bool IsReleaseGrade(const GlbStats& Stats, std::string& OutMissing)
    {
        OutMissing.clear();
        if (Stats.Meshes == 0) OutMissing += " meshes";
        if (Stats.Materials == 0) OutMissing += " materials";
        if (Stats.Images == 0 || Stats.Textures == 0) OutMissing += " textures";
        if (!Stats.Normals) OutMissing += " normals";
        if (!Stats.Uv0) OutMissing += " uv0";
        return OutMissing.empty();
    }

    bool ImportProviderGlb(const ImportRequest& Request, ImportResult& OutResult, std::string& OutError)
    {
        std::string ManifestText;
        JsonValue Manifest;
        std::string ParseError;
        if (!ReadFile(Request.ManifestPath, ManifestText) || !JsonReader::Parse(ManifestText, Manifest, ParseError) ||
            !Manifest.IsObject())
        {
            OutError = "cannot read source manifest " + Request.ManifestPath.string() + " " + ParseError;
            return false;
        }
        const std::string* AssetId = StringMember(Manifest, "assetId");
        const JsonValue* Glb = Manifest.Find("glb");
        const JsonValue* ExpectedSize = Glb ? Glb->Find("sizeBytes") : nullptr;
        const std::string* ExpectedEtag = Glb ? StringMember(*Glb, "etag") : nullptr;
        if (!AssetId || !IsSafeAssetId(*AssetId) || !ExpectedSize || !ExpectedSize->IsNumber() || !ExpectedEtag)
        {
            OutError = "manifest needs a safe assetId, glb.sizeBytes and glb.etag";
            return false;
        }
        if (ExpectedEtag->size() != 32 || ExpectedEtag->find_first_not_of("0123456789abcdef") != std::string::npos)
        {
            OutError = "glb.etag is not a single-part MD5; the exact revision cannot be proven";
            return false;
        }

        std::string Bytes;
        if (!ReadFile(Request.GlbPath, Bytes))
        {
            OutError = "cannot read " + Request.GlbPath.string();
            return false;
        }
        if (static_cast<double>(Bytes.size()) != ExpectedSize->Number)
        {
            OutError = "size mismatch: " + std::to_string(Bytes.size()) + " bytes, manifest records " +
                std::to_string(static_cast<long long>(ExpectedSize->Number));
            return false;
        }
        const std::string Md5 = Md5Hex(Bytes);
        if (Md5 != *ExpectedEtag)
        {
            OutError = "ETag mismatch: file MD5 " + Md5 + " != manifest " + *ExpectedEtag + " (wrong revision)";
            return false;
        }
        GlbStats Stats;
        if (!ValidateGlb(Bytes, Stats, OutError))
        {
            return false;
        }
        std::string Missing;
        if (!IsReleaseGrade(Stats, Missing))
        {
            OutError = "release source attributes incomplete:" + Missing;
            return false;
        }
        if (Stats.PositionVertices < Request.MinimumVertices)
        {
            OutError = "vertex floor failed: " + std::to_string(Stats.PositionVertices) + " < " +
                std::to_string(Request.MinimumVertices);
            return false;
        }

        const fs::path Folder = Request.RepoRoot / "Engine" / "O3DE" / "DarkArisen" / "Assets" / "Imported" / *AssetId;
        std::error_code Error;
        fs::create_directories(Folder, Error);
        OutResult.Destination = Folder / (*AssetId + ".glb");
        OutResult.ProvenancePath = Folder / (*AssetId + ".provenance.json");
        OutResult.Sha256 = Sha256Hex(Bytes);
        OutResult.Stats = Stats;
        {
            std::ofstream Stream(OutResult.Destination, std::ios::binary | std::ios::trunc);
            Stream.write(Bytes.data(), static_cast<std::streamsize>(Bytes.size()));
            if (!Stream)
            {
                OutError = "cannot write " + OutResult.Destination.string();
                return false;
            }
        }

        const auto Field = [&Manifest](const char* Key)
        {
            const std::string* Value = StringMember(Manifest, Key);
            return Value ? JsonEscape(*Value) : std::string();
        };
        const JsonValue* Revision = Manifest.Find("revision");
        const fs::path ManifestRelative = fs::relative(Request.ManifestPath, Request.RepoRoot, Error);
        std::ofstream Provenance(OutResult.ProvenancePath, std::ios::trunc);
        Provenance << "{\n"
                   << "  \"schemaVersion\": 1,\n"
                   << "  \"assetId\": \"" << JsonEscape(*AssetId) << "\",\n"
                   << "  \"provider\": \"Higgsfield\",\n"
                   << "  \"sourceTool\": \"" << Field("sourceTool") << "\",\n"
                   << "  \"projectId\": \"" << Field("projectId") << "\",\n"
                   << "  \"revision\": " << (Revision && Revision->IsNumber() ? static_cast<long long>(Revision->Number) : 0) << ",\n"
                   << "  \"operationId\": \"" << Field("operationId") << "\",\n"
                   << "  \"sourceManifest\": \"" << JsonEscape(ManifestRelative.generic_string()) << "\",\n"
                   << "  \"sizeBytes\": " << Bytes.size() << ",\n"
                   << "  \"md5\": \"" << Md5 << "\",\n"
                   << "  \"sha256\": \"" << OutResult.Sha256 << "\",\n"
                   << "  \"importer\": \"" << ImporterVersion << "\",\n"
                   << "  \"importedAtUtc\": \"" << JsonEscape(Request.ImportDateUtc) << "\",\n"
                   << "  \"destination\": \"Assets/Imported/" << JsonEscape(*AssetId) << "/" << JsonEscape(*AssetId) << ".glb\",\n"
                   << "  \"licenseStatus\": \"PROVIDER_OUTPUT_OWNER_REVIEW_REQUIRED\",\n"
                   << "  \"acceptanceStatus\": \"IMPORT_READY / O3DE_ASSET_PROCESSOR_AND_VISUAL_REVIEW_PENDING\",\n"
                   << "  \"stats\": {\"positionVertices\": " << Stats.PositionVertices << ", \"meshes\": " << Stats.Meshes
                   << ", \"materials\": " << Stats.Materials << ", \"textures\": " << Stats.Textures
                   << ", \"normals\": " << (Stats.Normals ? "true" : "false") << ", \"uv0\": " << (Stats.Uv0 ? "true" : "false")
                   << ", \"skinned\": " << (Stats.Skinned ? "true" : "false") << ", \"animations\": " << Stats.Animations << "}\n"
                   << "}\n";
        if (!Provenance)
        {
            OutError = "cannot write " + OutResult.ProvenancePath.string();
            return false;
        }
        return true;
    }
}
