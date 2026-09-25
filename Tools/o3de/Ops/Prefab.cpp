#include "Prefab.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace DarkArisen::Tools
{
    JsonValue JsonObject()
    {
        JsonValue Value;
        Value.Type = JsonValue::Kind::Object;
        return Value;
    }

    JsonValue JsonArray()
    {
        JsonValue Value;
        Value.Type = JsonValue::Kind::Array;
        return Value;
    }

    JsonValue JsonNumber(const double Number)
    {
        JsonValue Value;
        Value.Type = JsonValue::Kind::Number;
        Value.Number = Number;
        return Value;
    }

    JsonValue JsonString(const std::string_view Text)
    {
        JsonValue Value;
        Value.Type = JsonValue::Kind::String;
        Value.Text = std::string(Text);
        return Value;
    }

    JsonValue JsonBool(const bool Flag)
    {
        JsonValue Value;
        Value.Type = JsonValue::Kind::Bool;
        Value.Boolean = Flag;
        return Value;
    }

    JsonValue JsonVector(const Vec3d& Vector)
    {
        JsonValue Value = JsonArray();
        for (const double Component : {Vector.X, Vector.Y, Vector.Z})
        {
            // Round to the micrometre so float noise in source data never churns generated files.
            Value.Items.push_back(JsonNumber(std::round(Component * 1e6) / 1e6 + 0.0));
        }
        return Value;
    }

    std::uint64_t StableId(const std::string_view Name)
    {
        std::uint64_t Hash = 1469598103934665603ull;
        for (const char Character : Name)
        {
            Hash ^= static_cast<unsigned char>(Character);
            Hash *= 1099511628211ull;
        }
        return (Hash & ((1ull << 53) - 1)) | (1ull << 40);  // never zero, always exact in doubles
    }

    JsonValue AssetReference(const std::string_view Guid, const std::uint32_t SubId, const std::string_view Hint)
    {
        JsonValue Id = JsonObject();
        Id.Members["guid"] = JsonString(Guid);
        Id.Members["subId"] = JsonNumber(SubId);
        JsonValue Reference = JsonObject();
        Reference.Members["assetId"] = std::move(Id);
        Reference.Members["assetHint"] = JsonString(Hint);
        return Reference;
    }

    namespace
    {
        constexpr std::string_view TransformType = "{27F1E1A1-8D9D-4C3B-BD3A-AFB9762449C0} TransformComponent";

        // Companion components the O3DE editor keeps on every entity.
        constexpr std::string_view EditorCompanions[] = {"EditorDisabledCompositionComponent", "EditorEntityIconComponent",
            "EditorEntitySortComponent", "EditorInspectorComponent", "EditorLockComponent", "EditorOnlyEntityComponent",
            "EditorPendingCompositionComponent", "EditorVisibilityComponent"};
    }

    PrefabBuilder::PrefabBuilder(std::string LevelName) : Level(std::move(LevelName))
    {
        Container = JsonObject();
        Container.Members["Id"] = JsonString("ContainerEntity");
        Container.Members["Name"] = JsonString("Level");
        Container.Members["Components"] = JsonObject();
        Entities = JsonObject();
        for (const std::string_view Companion : EditorCompanions)
        {
            AddComponent("ContainerEntity", Companion);
        }
        AddComponent("ContainerEntity", "EditorPrefabComponent");
        JsonValue Transform = JsonObject();
        Transform.Members["Parent Entity"] = JsonString("");
        AddComponent("ContainerEntity", TransformType, std::move(Transform));
    }

    JsonValue& PrefabBuilder::ComponentsOf(const std::string& Alias)
    {
        if (Alias == "ContainerEntity")
        {
            return Container.Members["Components"];
        }
        const auto Found = Entities.Members.find(Alias);
        if (Found == Entities.Members.end())
        {
            throw std::logic_error("unknown entity alias " + Alias);
        }
        return Found->second.Members["Components"];
    }

    std::string PrefabBuilder::ComponentKey(const std::string& Alias, const std::string_view Type, std::uint64_t& OutId) const
    {
        OutId = StableId(Level + "/" + Alias + "/" + std::string(Type));
        return "Component_[" + std::to_string(OutId) + "]";
    }

    std::string PrefabBuilder::AddEntity(const std::string_view Name, const Vec3d& Translate, const Vec3d& RotateDegrees,
        const std::string_view Parent, const bool StartActive)
    {
        const std::string Alias = "Entity_[" + std::to_string(StableId(Level + "/entity/" + std::string(Name))) + "]";
        if (!Names.emplace(Alias, std::string(Name)).second)
        {
            throw std::logic_error("duplicate entity name " + std::string(Name));
        }
        JsonValue Entity = JsonObject();
        Entity.Members["Id"] = JsonString(Alias);
        Entity.Members["Name"] = JsonString(Name);
        Entity.Members["Components"] = JsonObject();
        if (!StartActive)
        {
            Entity.Members["IsRuntimeActive"] = JsonBool(false);
        }
        Entities.Members[Alias] = std::move(Entity);
        for (const std::string_view Companion : EditorCompanions)
        {
            AddComponent(Alias, Companion);
        }
        JsonValue Data = JsonObject();
        Data.Members["Translate"] = JsonVector(Translate);
        if (RotateDegrees.X != 0.0 || RotateDegrees.Y != 0.0 || RotateDegrees.Z != 0.0)
        {
            Data.Members["Rotate"] = JsonVector(RotateDegrees);
        }
        JsonValue Transform = JsonObject();
        Transform.Members["Parent Entity"] = JsonString(Parent.empty() ? std::string_view("ContainerEntity") : Parent);
        Transform.Members["Transform Data"] = std::move(Data);
        AddComponent(Alias, TransformType, std::move(Transform));
        return Alias;
    }

    void PrefabBuilder::AddComponent(const std::string& Alias, const std::string_view Type, JsonValue Body)
    {
        std::uint64_t Id = 0;
        const std::string Key = ComponentKey(Alias, Type, Id);
        Body.Type = JsonValue::Kind::Object;
        Body.Members["$type"] = JsonString(Type);
        Body.Members["Id"] = JsonNumber(static_cast<double>(Id));
        JsonValue& Components = ComponentsOf(Alias);
        if (!Components.Members.emplace(Key, std::move(Body)).second)
        {
            throw std::logic_error("component added twice: " + std::string(Type) + " on " + Alias);
        }
    }

    void PrefabBuilder::AddGameComponent(const std::string& Alias, const std::string_view TypeId, const std::string_view TypeName,
        JsonValue Fields)
    {
        Fields.Type = JsonValue::Kind::Object;
        Fields.Members["$type"] = JsonString(std::string(TypeId) + " " + std::string(TypeName));
        JsonValue Wrapper = JsonObject();
        Wrapper.Members["m_template"] = std::move(Fields);
        std::uint64_t Id = 0;
        const std::string Key = ComponentKey(Alias, TypeName, Id);
        Wrapper.Members["$type"] = JsonString("GenericComponentWrapper");
        Wrapper.Members["Id"] = JsonNumber(static_cast<double>(Id));
        JsonValue& Components = ComponentsOf(Alias);
        if (!Components.Members.emplace(Key, std::move(Wrapper)).second)
        {
            throw std::logic_error("game component added twice: " + std::string(TypeName) + " on " + Alias);
        }
    }

    std::string PrefabBuilder::Write() const
    {
        JsonValue Root = JsonObject();
        Root.Members["ContainerEntity"] = Container;
        Root.Members["Entities"] = Entities;
        return JsonWriter::Write(Root, 4);
    }
}
