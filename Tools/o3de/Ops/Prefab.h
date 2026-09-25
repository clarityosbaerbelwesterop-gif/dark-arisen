#pragma once

// Deterministic O3DE prefab writer. Mirrors the layout the O3DE 2605.0 editor saves
// (ContainerEntity + Entities, "Component_[id]" keys, editor companion components),
// taken from level prefabs committed in the pinned engine.

#include "Gltf.h"
#include "Json.h"

#include <cstdint>
#include <map>
#include <string>
#include <string_view>

namespace DarkArisen::Tools
{
    JsonValue JsonObject();
    JsonValue JsonArray();
    JsonValue JsonNumber(double Number);
    JsonValue JsonString(std::string_view Text);
    JsonValue JsonBool(bool Value);
    JsonValue JsonVector(const Vec3d& Value);

    /** Stable 53-bit identifier (exact in any JSON reader) from a name. */
    std::uint64_t StableId(std::string_view Name);

    /** Asset<T> reference as the O3DE JSON serializer writes it. */
    JsonValue AssetReference(std::string_view Guid, std::uint32_t SubId, std::string_view Hint);

    class PrefabBuilder
    {
    public:
        explicit PrefabBuilder(std::string LevelName);

        /** Adds an entity under the container (or Parent) and returns its alias, "Entity_[n]". */
        std::string AddEntity(std::string_view Name, const Vec3d& Translate, const Vec3d& RotateDegrees = {},
            std::string_view Parent = {}, bool StartActive = true);

        /** Editor or engine component written as-is: Type is the "$type" string. */
        void AddComponent(const std::string& Alias, std::string_view Type, JsonValue Body = JsonObject());

        /** Runtime-only game component, wrapped as the editor does (GenericComponentWrapper). */
        void AddGameComponent(const std::string& Alias, std::string_view TypeId, std::string_view TypeName, JsonValue Fields);

        std::string Write() const;

    private:
        std::string Level;
        JsonValue Container;
        JsonValue Entities;
        std::map<std::string, std::string> Names;  // alias -> name, to reject duplicates

        JsonValue& ComponentsOf(const std::string& Alias);
        std::string ComponentKey(const std::string& Alias, std::string_view Type, std::uint64_t& OutId) const;
    };
}
