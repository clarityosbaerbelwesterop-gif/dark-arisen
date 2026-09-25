#include "Gltf.h"

#include "Json.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <limits>
#include <set>
#include <vector>

namespace DarkArisen::Tools
{
    namespace
    {
        constexpr int ComponentFloat = 5126;
        constexpr int ComponentUnsignedByte = 5121;
        constexpr int ComponentUnsignedShort = 5123;
        constexpr int ComponentUnsignedInt = 5125;
        constexpr int ModeTriangles = 4;

        constexpr std::string_view Base64Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        bool DecodeBase64(const std::string_view Text, std::vector<std::uint8_t>& Out)
        {
            std::array<int, 256> Lookup{};
            Lookup.fill(-1);
            for (std::size_t Index = 0; Index < Base64Alphabet.size(); ++Index)
            {
                Lookup[static_cast<unsigned char>(Base64Alphabet[Index])] = static_cast<int>(Index);
            }
            if (Text.size() % 4 != 0)
            {
                return false;
            }
            Out.clear();
            Out.reserve(Text.size() / 4 * 3);
            for (std::size_t Offset = 0; Offset < Text.size(); Offset += 4)
            {
                int Values[4];
                int Padding = 0;
                for (int Index = 0; Index < 4; ++Index)
                {
                    const char Character = Text[Offset + static_cast<std::size_t>(Index)];
                    if (Character == '=' && Offset + 4 == Text.size() && Index >= 2)
                    {
                        Values[Index] = 0;
                        ++Padding;
                        continue;
                    }
                    if (Padding > 0)
                    {
                        return false;
                    }
                    Values[Index] = Lookup[static_cast<unsigned char>(Character)];
                    if (Values[Index] < 0)
                    {
                        return false;
                    }
                }
                const std::uint32_t Triple = (static_cast<std::uint32_t>(Values[0]) << 18) | (static_cast<std::uint32_t>(Values[1]) << 12) |
                    (static_cast<std::uint32_t>(Values[2]) << 6) | static_cast<std::uint32_t>(Values[3]);
                Out.push_back(static_cast<std::uint8_t>(Triple >> 16));
                if (Padding < 2) Out.push_back(static_cast<std::uint8_t>((Triple >> 8) & 0xFF));
                if (Padding < 1) Out.push_back(static_cast<std::uint8_t>(Triple & 0xFF));
            }
            return true;
        }

        std::string EncodeBase64(const std::vector<std::uint8_t>& Bytes)
        {
            std::string Out;
            Out.reserve((Bytes.size() + 2) / 3 * 4);
            for (std::size_t Offset = 0; Offset < Bytes.size(); Offset += 3)
            {
                const std::size_t Remaining = Bytes.size() - Offset;
                std::uint32_t Triple = static_cast<std::uint32_t>(Bytes[Offset]) << 16;
                if (Remaining > 1) Triple |= static_cast<std::uint32_t>(Bytes[Offset + 1]) << 8;
                if (Remaining > 2) Triple |= static_cast<std::uint32_t>(Bytes[Offset + 2]);
                Out += Base64Alphabet[(Triple >> 18) & 0x3F];
                Out += Base64Alphabet[(Triple >> 12) & 0x3F];
                Out += Remaining > 1 ? Base64Alphabet[(Triple >> 6) & 0x3F] : '=';
                Out += Remaining > 2 ? Base64Alphabet[Triple & 0x3F] : '=';
            }
            return Out;
        }

        bool GetIndex(const JsonValue& Object, const char* Key, std::size_t& Out)
        {
            const JsonValue* Value = Object.Find(Key);
            if (!Value || !Value->IsNumber() || Value->Number < 0.0 || Value->Number != std::floor(Value->Number) ||
                Value->Number > 4294967295.0)
            {
                return false;
            }
            Out = static_cast<std::size_t>(Value->Number);
            return true;
        }

        std::size_t OptionalIndex(const JsonValue& Object, const char* Key, const std::size_t Default)
        {
            std::size_t Out = Default;
            return GetIndex(Object, Key, Out) ? Out : Default;
        }

        JsonValue MakeNumber(const double Number)
        {
            JsonValue Value;
            Value.Type = JsonValue::Kind::Number;
            Value.Number = Number;
            return Value;
        }

        /** Resolved, bounds-checked view of one accessor's elements inside a decoded buffer. */
        struct AccessorView
        {
            std::vector<std::uint8_t>* Buffer = nullptr;
            std::size_t Offset = 0;
            std::size_t Stride = 0;
            std::size_t Count = 0;
            int ComponentType = 0;
            std::size_t Components = 0;
        };

        std::size_t ComponentSize(const int ComponentType)
        {
            switch (ComponentType)
            {
            case ComponentUnsignedByte: return 1;
            case ComponentUnsignedShort: return 2;
            case ComponentUnsignedInt:
            case ComponentFloat: return 4;
            default: return 0;
            }
        }

        std::size_t ComponentCount(const std::string& Type)
        {
            if (Type == "SCALAR") return 1;
            if (Type == "VEC3") return 3;
            return 0;
        }

        class Converter
        {
        public:
            Converter(JsonValue& InRoot, std::string& InError) : Root(InRoot), Error(InError) {}

            bool Run(GltfConversionStats& Stats)
            {
                for (const char* Unsupported : {"skins", "animations", "extensionsUsed", "extensionsRequired", "cameras"})
                {
                    if (Root.Find(Unsupported))
                    {
                        return Fail(std::string("unsupported glTF feature for greybox conversion: ") + Unsupported);
                    }
                }
                const JsonValue* Asset = Root.Find("asset");
                const JsonValue* Version = Asset ? Asset->Find("version") : nullptr;
                if (!Version || !Version->IsString() || Version->Text != "2.0")
                {
                    return Fail("asset.version must be \"2.0\"");
                }
                if (!DecodeBuffers() || !CheckNodes())
                {
                    return false;
                }
                JsonValue* Meshes = Member("meshes");
                if (!Meshes || !Meshes->IsArray() || Meshes->Items.empty())
                {
                    return Fail("no meshes");
                }
                for (JsonValue& Mesh : Meshes->Items)
                {
                    JsonValue* Primitives = Mesh.IsObject() ? MemberOf(Mesh, "primitives") : nullptr;
                    if (!Primitives || !Primitives->IsArray() || Primitives->Items.empty())
                    {
                        return Fail("mesh without primitives");
                    }
                    for (JsonValue& Primitive : Primitives->Items)
                    {
                        if (!ConvertPrimitive(Primitive, Stats))
                        {
                            return false;
                        }
                    }
                }
                EncodeBuffers();
                if (JsonValue* Generator = MemberOf(*Member("asset"), "generator"); Generator && Generator->IsString())
                {
                    Generator->Text += " + DarkArisenO3DE frame conversion";
                }
                return true;
            }

        private:
            JsonValue& Root;
            std::string& Error;
            std::vector<std::vector<std::uint8_t>> Buffers;
            std::vector<std::string> BufferPrefixes;
            std::set<std::size_t> ConvertedPositions;
            std::set<std::size_t> ConvertedNormals;
            std::set<std::size_t> ConvertedIndices;

            bool Fail(std::string Message)
            {
                Error = std::move(Message);
                return false;
            }

            JsonValue* Member(const char* Key) { return MemberOf(Root, Key); }

            static JsonValue* MemberOf(JsonValue& Object, const char* Key)
            {
                const auto Found = Object.Members.find(Key);
                return Found == Object.Members.end() ? nullptr : &Found->second;
            }

            bool DecodeBuffers()
            {
                JsonValue* List = Member("buffers");
                if (!List || !List->IsArray() || List->Items.empty())
                {
                    return Fail("no buffers");
                }
                for (const JsonValue& Buffer : List->Items)
                {
                    const JsonValue* Uri = Buffer.Find("uri");
                    std::size_t ByteLength = 0;
                    if (!Uri || !Uri->IsString() || !GetIndex(Buffer, "byteLength", ByteLength))
                    {
                        return Fail("buffer needs uri and byteLength");
                    }
                    const std::size_t Comma = Uri->Text.find(',');
                    const std::string Prefix = Comma == std::string::npos ? std::string() : Uri->Text.substr(0, Comma + 1);
                    if (Prefix != "data:application/octet-stream;base64," && Prefix != "data:application/gltf-buffer;base64,")
                    {
                        return Fail("only embedded base64 buffers are supported");
                    }
                    std::vector<std::uint8_t> Bytes;
                    if (!DecodeBase64(std::string_view(Uri->Text).substr(Comma + 1), Bytes) || Bytes.size() != ByteLength)
                    {
                        return Fail("buffer data does not decode to byteLength");
                    }
                    Buffers.push_back(std::move(Bytes));
                    BufferPrefixes.push_back(Prefix);
                }
                return true;
            }

            void EncodeBuffers()
            {
                JsonValue* List = Member("buffers");
                for (std::size_t Index = 0; Index < Buffers.size(); ++Index)
                {
                    List->Items[Index].Members["uri"].Text = BufferPrefixes[Index] + EncodeBase64(Buffers[Index]);
                }
            }

            bool CheckNodes()
            {
                const JsonValue* Nodes = Root.Find("nodes");
                if (!Nodes)
                {
                    return true;
                }
                for (const JsonValue& Node : Nodes->Items)
                {
                    for (const char* Transform : {"matrix", "rotation", "translation", "scale"})
                    {
                        if (Node.Find(Transform))
                        {
                            return Fail("node transforms are not supported for greybox conversion");
                        }
                    }
                }
                return true;
            }

            bool ResolveAccessor(const std::size_t AccessorIndex, const int ExpectedComponents, AccessorView& View)
            {
                JsonValue* Accessors = Member("accessors");
                JsonValue* Views = Member("bufferViews");
                if (!Accessors || AccessorIndex >= Accessors->Items.size() || !Views)
                {
                    return Fail("accessor index out of range");
                }
                const JsonValue& Accessor = Accessors->Items[AccessorIndex];
                if (Accessor.Find("sparse"))
                {
                    return Fail("sparse accessors are not supported");
                }
                std::size_t ViewIndex = 0;
                std::size_t Count = 0;
                std::size_t ComponentTypeIndex = 0;
                const JsonValue* Type = Accessor.Find("type");
                if (!GetIndex(Accessor, "bufferView", ViewIndex) || !GetIndex(Accessor, "count", Count) ||
                    !GetIndex(Accessor, "componentType", ComponentTypeIndex) || !Type || !Type->IsString() || ViewIndex >= Views->Items.size())
                {
                    return Fail("malformed accessor");
                }
                View.ComponentType = static_cast<int>(ComponentTypeIndex);
                View.Components = ComponentCount(Type->Text);
                if (View.Components == 0 || static_cast<int>(View.Components) != ExpectedComponents || ComponentSize(View.ComponentType) == 0)
                {
                    return Fail("unexpected accessor type");
                }
                const JsonValue& BufferView = Views->Items[ViewIndex];
                std::size_t BufferIndex = 0;
                std::size_t ViewLength = 0;
                if (!GetIndex(BufferView, "buffer", BufferIndex) || !GetIndex(BufferView, "byteLength", ViewLength) || BufferIndex >= Buffers.size())
                {
                    return Fail("malformed bufferView");
                }
                const std::size_t ElementSize = ComponentSize(View.ComponentType) * View.Components;
                View.Stride = OptionalIndex(BufferView, "byteStride", ElementSize);
                const std::size_t ViewOffset = OptionalIndex(BufferView, "byteOffset", 0);
                const std::size_t AccessorOffset = OptionalIndex(Accessor, "byteOffset", 0);
                View.Buffer = &Buffers[BufferIndex];
                View.Offset = ViewOffset + AccessorOffset;
                View.Count = Count;
                if (View.Stride < ElementSize || Count == 0 || ViewOffset + ViewLength > View.Buffer->size() ||
                    AccessorOffset + (Count - 1) * View.Stride + ElementSize > ViewLength)
                {
                    return Fail("accessor exceeds its bufferView");
                }
                return true;
            }

            static float ReadFloat(const std::vector<std::uint8_t>& Buffer, const std::size_t Offset)
            {
                float Value = 0.0f;
                std::memcpy(&Value, Buffer.data() + Offset, sizeof(float));
                return Value;
            }

            static void WriteFloat(std::vector<std::uint8_t>& Buffer, const std::size_t Offset, const float Value)
            {
                std::memcpy(Buffer.data() + Offset, &Value, sizeof(float));
            }

            bool ConvertVectors(const std::size_t AccessorIndex, const bool UpdateBounds, std::size_t& OutCount)
            {
                AccessorView View;
                if (!ResolveAccessor(AccessorIndex, 3, View))
                {
                    return false;
                }
                if (View.ComponentType != ComponentFloat)
                {
                    return Fail("vector attributes must be float");
                }
                std::array<double, 3> Minimum{std::numeric_limits<double>::max(), std::numeric_limits<double>::max(),
                    std::numeric_limits<double>::max()};
                std::array<double, 3> Maximum{-Minimum[0], -Minimum[1], -Minimum[2]};
                for (std::size_t Element = 0; Element < View.Count; ++Element)
                {
                    const std::size_t Offset = View.Offset + Element * View.Stride;
                    const Vec3d Source{ReadFloat(*View.Buffer, Offset), ReadFloat(*View.Buffer, Offset + 4), ReadFloat(*View.Buffer, Offset + 8)};
                    if (!std::isfinite(Source.X) || !std::isfinite(Source.Y) || !std::isfinite(Source.Z))
                    {
                        return Fail("non-finite vertex data");
                    }
                    const Vec3d Target = SourceMeshToGltf(Source);
                    const float Components[3] = {static_cast<float>(Target.X), static_cast<float>(Target.Y), static_cast<float>(Target.Z)};
                    for (std::size_t Axis = 0; Axis < 3; ++Axis)
                    {
                        WriteFloat(*View.Buffer, Offset + Axis * 4, Components[Axis]);
                        Minimum[Axis] = std::min(Minimum[Axis], static_cast<double>(Components[Axis]));
                        Maximum[Axis] = std::max(Maximum[Axis], static_cast<double>(Components[Axis]));
                    }
                }
                JsonValue& Accessor = Member("accessors")->Items[AccessorIndex];
                if (UpdateBounds || Accessor.Find("min") || Accessor.Find("max"))
                {
                    JsonValue MinimumValue;
                    JsonValue MaximumValue;
                    MinimumValue.Type = MaximumValue.Type = JsonValue::Kind::Array;
                    for (std::size_t Axis = 0; Axis < 3; ++Axis)
                    {
                        MinimumValue.Items.push_back(MakeNumber(Minimum[Axis]));
                        MaximumValue.Items.push_back(MakeNumber(Maximum[Axis]));
                    }
                    Accessor.Members["min"] = std::move(MinimumValue);
                    Accessor.Members["max"] = std::move(MaximumValue);
                }
                OutCount = View.Count;
                return true;
            }

            bool ReverseWinding(const std::size_t AccessorIndex, const std::size_t VertexCount, std::size_t& OutTriangles)
            {
                AccessorView View;
                if (!ResolveAccessor(AccessorIndex, 1, View))
                {
                    return false;
                }
                if (View.ComponentType == ComponentFloat || View.Count % 3 != 0)
                {
                    return Fail("indices must be unsigned integers in whole triangles");
                }
                const std::size_t Size = ComponentSize(View.ComponentType);
                auto Read = [&View, Size](const std::size_t Element)
                {
                    std::uint32_t Value = 0;
                    std::memcpy(&Value, View.Buffer->data() + View.Offset + Element * View.Stride, Size);
                    return Value;
                };
                for (std::size_t Element = 0; Element < View.Count; ++Element)
                {
                    if (Read(Element) >= VertexCount)
                    {
                        return Fail("index out of vertex range");
                    }
                }
                for (std::size_t Triangle = 0; Triangle < View.Count / 3; ++Triangle)
                {
                    std::uint8_t* Second = View.Buffer->data() + View.Offset + (Triangle * 3 + 1) * View.Stride;
                    std::uint8_t* Third = View.Buffer->data() + View.Offset + (Triangle * 3 + 2) * View.Stride;
                    std::uint8_t Swap[4];
                    std::memcpy(Swap, Second, Size);
                    std::memcpy(Second, Third, Size);
                    std::memcpy(Third, Swap, Size);
                }
                OutTriangles = View.Count / 3;
                return true;
            }

            bool ConvertPrimitive(JsonValue& Primitive, GltfConversionStats& Stats)
            {
                if (OptionalIndex(Primitive, "mode", ModeTriangles) != ModeTriangles)
                {
                    return Fail("only triangle primitives are supported");
                }
                if (Primitive.Find("targets"))
                {
                    return Fail("morph targets are not supported");
                }
                const JsonValue* Attributes = Primitive.Find("attributes");
                std::size_t Position = 0;
                if (!Attributes || !GetIndex(*Attributes, "POSITION", Position))
                {
                    return Fail("primitive without POSITION");
                }
                for (const auto& [Name, Value] : Attributes->Members)
                {
                    if (Name != "POSITION" && Name != "NORMAL")
                    {
                        return Fail("unsupported vertex attribute " + Name);
                    }
                }
                std::size_t Indices = 0;
                if (!GetIndex(Primitive, "indices", Indices))
                {
                    return Fail("non-indexed primitives are not supported");
                }
                std::size_t VertexCount = 0;
                AccessorView PositionView;
                if (!ResolveAccessor(Position, 3, PositionView))
                {
                    return false;
                }
                VertexCount = PositionView.Count;
                if (ConvertedPositions.insert(Position).second)
                {
                    std::size_t Converted = 0;
                    if (!ConvertVectors(Position, true, Converted))
                    {
                        return false;
                    }
                    Stats.Vertices += Converted;
                }
                if (std::size_t Normal = 0; GetIndex(*Attributes, "NORMAL", Normal) && ConvertedNormals.insert(Normal).second)
                {
                    std::size_t Converted = 0;
                    if (!ConvertVectors(Normal, false, Converted))
                    {
                        return false;
                    }
                }
                if (ConvertedIndices.insert(Indices).second)
                {
                    std::size_t Triangles = 0;
                    if (!ReverseWinding(Indices, VertexCount, Triangles))
                    {
                        return false;
                    }
                    Stats.Triangles += Triangles;
                }
                return true;
            }
        };
    }

    namespace
    {
        bool StripSkin(JsonValue& Root, std::string& Error)
        {
            Root.Members.erase("skins");
            Root.Members.erase("animations");
            auto NodesIt = Root.Members.find("nodes");
            if (NodesIt == Root.Members.end() || !NodesIt->second.IsArray())
            {
                Error = "skinned glTF without nodes";
                return false;
            }
            // Keep mesh nodes only, flattened to the scene root; joints carry no geometry.
            JsonValue MeshNodes;
            MeshNodes.Type = JsonValue::Kind::Array;
            for (const JsonValue& Node : NodesIt->second.Items)
            {
                if (!Node.Find("mesh"))
                {
                    continue;
                }
                JsonValue Kept;
                Kept.Type = JsonValue::Kind::Object;
                for (const char* Key : {"mesh", "name"})
                {
                    if (const JsonValue* Value = Node.Find(Key)) Kept.Members[Key] = *Value;
                }
                MeshNodes.Items.push_back(std::move(Kept));
            }
            if (MeshNodes.Items.empty())
            {
                Error = "skinned glTF has no mesh node";
                return false;
            }
            JsonValue SceneNodes;
            SceneNodes.Type = JsonValue::Kind::Array;
            for (std::size_t Index = 0; Index < MeshNodes.Items.size(); ++Index)
            {
                SceneNodes.Items.push_back(MakeNumber(static_cast<double>(Index)));
            }
            NodesIt->second = std::move(MeshNodes);
            JsonValue Scene;
            Scene.Type = JsonValue::Kind::Object;
            Scene.Members["nodes"] = std::move(SceneNodes);
            JsonValue Scenes;
            Scenes.Type = JsonValue::Kind::Array;
            Scenes.Items.push_back(std::move(Scene));
            Root.Members["scenes"] = std::move(Scenes);
            Root.Members["scene"] = MakeNumber(0.0);
            if (auto MeshesIt = Root.Members.find("meshes"); MeshesIt != Root.Members.end())
            {
                for (JsonValue& Mesh : MeshesIt->second.Items)
                {
                    auto PrimitivesIt = Mesh.Members.find("primitives");
                    if (PrimitivesIt == Mesh.Members.end()) continue;
                    for (JsonValue& Primitive : PrimitivesIt->second.Items)
                    {
                        auto AttributesIt = Primitive.Members.find("attributes");
                        if (AttributesIt == Primitive.Members.end()) continue;
                        for (auto Attribute = AttributesIt->second.Members.begin(); Attribute != AttributesIt->second.Members.end();)
                        {
                            const bool SkinAttribute = Attribute->first.rfind("JOINTS_", 0) == 0 || Attribute->first.rfind("WEIGHTS_", 0) == 0;
                            Attribute = SkinAttribute ? AttributesIt->second.Members.erase(Attribute) : std::next(Attribute);
                        }
                    }
                }
            }
            return true;
        }
    }

    bool ConvertGreyboxGltf(const std::string_view Source, std::string& OutGltf, GltfConversionStats& OutStats, std::string& OutError,
        const GltfConversionOptions& Options)
    {
        OutStats = {};
        JsonValue Root;
        if (!JsonReader::Parse(Source, Root, OutError))
        {
            OutError = "glTF JSON: " + OutError;
            return false;
        }
        if (!Root.IsObject())
        {
            OutError = "glTF root must be an object";
            return false;
        }
        if (Options.StripSkinToBindPose && Root.Find("skins") && !StripSkin(Root, OutError))
        {
            return false;
        }
        Converter Work(Root, OutError);
        if (!Work.Run(OutStats))
        {
            return false;
        }
        OutGltf = JsonWriter::Write(Root, 1);
        return true;
    }

    namespace
    {
        std::string MeshGltf(const std::vector<float>& Positions, const std::vector<std::uint16_t>& Indices, const std::string_view Name,
            const std::string_view Generator)
        {
            std::vector<std::uint8_t> Bytes(Positions.size() * 4 + Indices.size() * 2);
            std::memcpy(Bytes.data(), Positions.data(), Positions.size() * 4);
            std::memcpy(Bytes.data() + Positions.size() * 4, Indices.data(), Indices.size() * 2);
            std::string Escaped;
            JsonWriter::AppendString(Escaped, Name);
            std::string GeneratorText;
            JsonWriter::AppendString(GeneratorText, Generator);
            std::string Json = "{\"asset\":{\"version\":\"2.0\",\"generator\":" + GeneratorText + "},";
            Json += "\"buffers\":[{\"byteLength\":" + std::to_string(Bytes.size()) +
                ",\"uri\":\"data:application/octet-stream;base64," + EncodeBase64(Bytes) + "\"}],";
            Json += "\"bufferViews\":[{\"buffer\":0,\"byteOffset\":0,\"byteLength\":" + std::to_string(Positions.size() * 4) +
                "},{\"buffer\":0,\"byteOffset\":" + std::to_string(Positions.size() * 4) + ",\"byteLength\":" +
                std::to_string(Indices.size() * 2) + "}],";
            Json += "\"accessors\":[{\"bufferView\":0,\"componentType\":5126,\"count\":" + std::to_string(Positions.size() / 3) +
                ",\"type\":\"VEC3\"},{\"bufferView\":1,\"componentType\":5123,\"count\":" + std::to_string(Indices.size()) +
                ",\"type\":\"SCALAR\"}],";
            Json += "\"meshes\":[{\"name\":" + Escaped + ",\"primitives\":[{\"attributes\":{\"POSITION\":0},\"indices\":1}]}],";
            Json += "\"nodes\":[{\"name\":" + Escaped + ",\"mesh\":0}],\"scenes\":[{\"nodes\":[0]}],\"scene\":0}";
            return Json;
        }
    }

    std::string MakePlaceholderFigureGltf(const double Height, const double Radius, const std::string_view Name)
    {
        constexpr int Sides = 8;
        const double Pi = std::acos(-1.0);
        std::vector<float> Positions;
        const double Shoulder = Height - Radius;
        const double Hip = Radius;
        for (const double Z : {Hip, Shoulder})
        {
            for (int Side = 0; Side < Sides; ++Side)
            {
                const double Angle = 2.0 * Pi * Side / Sides;
                Positions.push_back(static_cast<float>(Radius * std::cos(Angle)));
                Positions.push_back(static_cast<float>(Radius * std::sin(Angle)));
                Positions.push_back(static_cast<float>(Z));
            }
        }
        const std::uint16_t Bottom = static_cast<std::uint16_t>(Positions.size() / 3);
        Positions.insert(Positions.end(), {0.0f, 0.0f, 0.0f});
        const std::uint16_t Top = static_cast<std::uint16_t>(Positions.size() / 3);
        Positions.insert(Positions.end(), {0.0f, 0.0f, static_cast<float>(Height)});
        std::vector<std::uint16_t> Indices;
        for (int Side = 0; Side < Sides; ++Side)
        {
            const auto Low = static_cast<std::uint16_t>(Side);
            const auto LowNext = static_cast<std::uint16_t>((Side + 1) % Sides);
            const auto High = static_cast<std::uint16_t>(Side + Sides);
            const auto HighNext = static_cast<std::uint16_t>((Side + 1) % Sides + Sides);
            Indices.insert(Indices.end(), {Low, LowNext, HighNext, Low, HighNext, High});  // outward, counter-clockwise
            Indices.insert(Indices.end(), {Bottom, LowNext, Low});
            Indices.insert(Indices.end(), {Top, High, HighNext});
        }
        return MeshGltf(Positions, Indices, Name, "DarkArisenO3DE placeholder figure");
    }

    std::string MakePlaceholderBoxGltf(const double SizeX, const double SizeY, const double SizeZ, const std::string_view Name)
    {
        const float X = static_cast<float>(SizeX / 2.0);
        const float Y = static_cast<float>(SizeY / 2.0);
        const float Z = static_cast<float>(SizeZ);
        const std::vector<float> Positions = {-X, -Y, 0, X, -Y, 0, X, Y, 0, -X, Y, 0, -X, -Y, Z, X, -Y, Z, X, Y, Z, -X, Y, Z};
        // Counter-clockwise seen from outside, in the source (Z-up) frame.
        const std::vector<std::uint16_t> Indices = {0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7, 0, 1, 5, 0, 5, 4,
            1, 2, 6, 1, 6, 5, 2, 3, 7, 2, 7, 6, 3, 0, 4, 3, 4, 7};
        return MeshGltf(Positions, Indices, Name, "DarkArisenO3DE placeholder prop");
    }
}
