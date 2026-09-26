#pragma once

// Shared stand-ins for the level probes: loads materialised level prefabs through O3DE's JSON
// serializer (unknown or mistyped fields fail), instantiates the entities on a real
// AZ::ComponentApplication and simulates what needs a GPU or a PhysX scene: transforms, the
// character controller, trigger overlap (point-in-box on the materialised trigger colliders), the
// game entity context and the level system ("LoadLevel" console command). Include once per probe
// executable.

#include <AzCore/Component/ComponentApplication.h>
#include <AzCore/Component/ComponentApplicationBus.h>
#include <AzCore/Component/Entity.h>
#include <AzCore/Component/EntityIdSerializer.h>
#include <AzCore/Console/IConsole.h>
#include <AzCore/IO/FileIO.h>
#include <AzCore/JSON/document.h>
#include <AzCore/Math/Aabb.h>
#include <AzCore/Math/Quaternion.h>
#include <AzCore/Serialization/Json/JsonSerialization.h>
#include <AzCore/Serialization/Json/JsonSerializationResult.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/API/ApplicationAPI.h>
#include <AzFramework/Entity/GameEntityContextBus.h>
#include <AzFramework/IO/LocalFileIO.h>

#include <DarkArisen/CameraBus.h>
#include <DarkArisen/CampaignBus.h>
#include <DarkArisen/CombatBus.h>
#include <DarkArisen/OceanBus.h>
#include <DarkArisen/SwimBus.h>
#include "Combat/CombatantComponent.h"
#include "Combat/EnemyBrainComponent.h"
#include "Player/CameraRigComponent.h"
#include "Player/LockOnComponent.h"
#include "Player/PlayerSpawnComponent.h"
#include "Player/SwimmerComponent.h"
#include "Story/CampaignSystemComponent.h"
#include "Story/MapTransitionComponent.h"
#include "Story/OpeningDirectorComponent.h"
#include "Story/StoryTriggerComponent.h"
#include "Story/StoryActorComponent.h"
#include "Story/CreditsComponent.h"
#include "FrontEnd/FrontEndSystemComponent.h"
#include "FrontEnd/MainMenuComponent.h"
#include "Ships/NavalCombatComponent.h"
#include "Ships/ShipVoyageComponent.h"
#include "World/OceanComponent.h"
#include "World/WaterVolumeComponent.h"

#include <DarkArisen/Core/CampaignRuntime.h>
#include <DarkArisen/Core/Combat.h>
#include <DarkArisen/Core/EntityPolicy.h>
#include <DarkArisen/Core/Facts.h>
#include <DarkArisen/Core/Ocean.h>
#include <DarkArisen/Core/OpeningRuntime.h>

#include "ProbeSupport.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

namespace
{
    int g_failures = 0;
    AZStd::string g_requestedLevel;

    void Check(const bool condition, const char* what)
    {
        std::printf("[%s] %s\n", condition ? " ok " : "FAIL", what);
        if (!condition)
        {
            ++g_failures;
        }
    }

    /** Level system stand-in: the campaign's TravelToLevel issues "LoadLevel <name>". */
    void LoadLevel(const AZ::ConsoleCommandContainer& arguments)
    {
        g_requestedLevel = arguments.empty() ? "" : AZStd::string(arguments.back());
    }
}
AZ_CONSOLEFREEFUNC(LoadLevel, AZ::ConsoleFunctorFlags::Null, "Probe level loader");

namespace
{
    /** Game entity context stand-in: Start Active off entities are activated on request. */
    class ProbeGameEntityContext : public AzFramework::GameEntityContextRequestBus::Handler
    {
    public:
        ProbeGameEntityContext() { BusConnect(); }
        ~ProbeGameEntityContext() override { BusDisconnect(); }

        static AZ::Entity* Find(const AZ::EntityId& id)
        {
            AZ::Entity* entity = nullptr;
            AZ::ComponentApplicationBus::BroadcastResult(entity, &AZ::ComponentApplicationRequests::FindEntity, id);
            return entity;
        }
        void ActivateGameEntity(const AZ::EntityId& id) override
        {
            if (AZ::Entity* entity = Find(id); entity && entity->GetState() == AZ::Entity::State::Init)
            {
                entity->Activate();
            }
        }
        void DeactivateGameEntity(const AZ::EntityId& id) override
        {
            if (AZ::Entity* entity = Find(id); entity && entity->GetState() == AZ::Entity::State::Active)
            {
                entity->Deactivate();
            }
        }
        AzFramework::EntityContextId GetGameEntityContextId() override { return AzFramework::EntityContextId::CreateNull(); }
        AzFramework::EntityContext* GetGameEntityContextInstance() override { return nullptr; }
        AZ::Entity* CreateGameEntity(const char*) override { return nullptr; }
        AzFramework::BehaviorEntity CreateGameEntityForBehaviorContext(const char*) override { return {}; }
        void AddGameEntity(AZ::Entity*) override {}
        void DestroyGameEntity(const AZ::EntityId&) override {}
        void DestroyGameEntityAndDescendants(const AZ::EntityId&) override {}
        bool LoadFromStream(AZ::IO::GenericStream&, bool) override { return false; }
        void ResetGameContext() override {}
        AZStd::string GetEntityName(const AZ::EntityId& id) override
        {
            AZ::Entity* entity = Find(id);
            return entity ? entity->GetName() : AZStd::string();
        }
    };

    /** Prefab entity references ("Entity_[n]") resolved to the probe's runtime entity ids. */
    class AliasMapper : public AZ::JsonEntityIdSerializer::JsonEntityIdMapper
    {
    public:
        AZ_RTTI(AliasMapper, "{3D1F8B62-9E47-4C05-A2B8-6F0E1C9D7A43}", AZ::JsonEntityIdSerializer::JsonEntityIdMapper);
        AZStd::unordered_map<AZStd::string, AZ::EntityId> m_aliases;

        AZ::JsonSerializationResult::Result MapJsonToId(
            AZ::EntityId& outputValue, const rapidjson::Value& inputValue, AZ::JsonDeserializerContext& context) override
        {
            namespace JSR = AZ::JsonSerializationResult;
            if (!inputValue.IsString())
            {
                return context.Report(JSR::Tasks::ReadField, JSR::Outcomes::Invalid, "entity reference is not an alias string");
            }
            const AZStd::string alias(inputValue.GetString(), inputValue.GetStringLength());
            if (alias.empty())
            {
                outputValue.SetInvalid();
                return context.Report(JSR::Tasks::ReadField, JSR::Outcomes::Success, "empty reference");
            }
            const auto found = m_aliases.find(alias);
            if (found == m_aliases.end())
            {
                return context.Report(JSR::Tasks::ReadField, JSR::Outcomes::Missing, "reference to an entity not in the prefab");
            }
            outputValue = found->second;
            return context.Report(JSR::Tasks::ReadField, JSR::Outcomes::Success, "mapped");
        }
        AZ::JsonSerializationResult::Result MapIdToJson(rapidjson::Value&, const AZ::EntityId&, AZ::JsonSerializerContext& context) override
        {
            return context.Report(AZ::JsonSerializationResult::Tasks::WriteValue, AZ::JsonSerializationResult::Outcomes::Unsupported, "load only");
        }
    };

    struct TriggerVolume
    {
        AZ::EntityId m_entity;
        AZ::Aabb m_box = AZ::Aabb::CreateNull();
        bool m_inside = false;
    };

    /** A solid (non-trigger) box collider as materialised: world centre, half extents, yaw. */
    struct SolidBox
    {
        AZ::Vector3 m_center = AZ::Vector3::CreateZero();
        AZ::Vector3 m_halfExtents = AZ::Vector3::CreateZero();
        float m_yawRadians = 0.0f;
    };

    /** Scalar fields of one Dark Arisen game component as written in the prefab (for probe logic). */
    struct GameComponentRecord
    {
        AZStd::string m_type;
        AZStd::string m_entityName;
        AZ::EntityId m_entity;
        AZStd::unordered_map<AZStd::string, AZStd::string> m_fields;

        AZStd::string Field(const char* name) const
        {
            const auto found = m_fields.find(name);
            return found == m_fields.end() ? AZStd::string() : found->second;
        }
    };

    struct LoadedLevel
    {
        AZStd::string m_name;
        AZStd::vector<AZ::Entity*> m_entities;
        AZStd::unordered_map<AZStd::string, AZ::EntityId> m_byName;
        AZStd::vector<TriggerVolume> m_triggers;
        AZStd::vector<SolidBox> m_solidBoxes;
        AZStd::vector<AZ::Vector3> m_meshColliderOrigins;
        AZStd::vector<GameComponentRecord> m_records;
        int m_gameComponents = 0;
        int m_rejectedComponents = 0;
        int m_engineComponents = 0;

        AZ::EntityId Id(const char* name) const
        {
            const auto found = m_byName.find(name);
            return found == m_byName.end() ? AZ::EntityId() : found->second;
        }
    };

    AZ::Component* CreateGameComponent(const AZ::Uuid& type)
    {
        using namespace DarkArisen;
        if (type == azrtti_typeid<CombatantComponent>()) return aznew CombatantComponent();
        if (type == azrtti_typeid<EnemyBrainComponent>()) return aznew EnemyBrainComponent();
        if (type == azrtti_typeid<StoryTriggerComponent>()) return aznew StoryTriggerComponent();
        if (type == azrtti_typeid<OpeningDirectorComponent>()) return aznew OpeningDirectorComponent();
        if (type == azrtti_typeid<MapTransitionComponent>()) return aznew MapTransitionComponent();
        if (type == azrtti_typeid<PlayerSpawnComponent>()) return aznew PlayerSpawnComponent();
        if (type == azrtti_typeid<SwimmerComponent>()) return aznew SwimmerComponent();
        if (type == azrtti_typeid<WaterVolumeComponent>()) return aznew WaterVolumeComponent();
        if (type == azrtti_typeid<OceanComponent>()) return aznew OceanComponent();
        if (type == azrtti_typeid<CameraRigComponent>()) return aznew CameraRigComponent();
        if (type == azrtti_typeid<LockOnComponent>()) return aznew LockOnComponent();
        if (type == azrtti_typeid<StoryActorComponent>()) return aznew StoryActorComponent();
        if (type == azrtti_typeid<NavalCombatComponent>()) return aznew NavalCombatComponent();
        if (type == azrtti_typeid<CreditsComponent>()) return aznew CreditsComponent();
        if (type == azrtti_typeid<MainMenuComponent>()) return aznew MainMenuComponent();
        if (type == azrtti_typeid<ShipVoyageComponent>()) return aznew ShipVoyageComponent();
        return nullptr;  // JakeInputComponent needs the input system; it is compile-checked instead.
    }

    AZ::Vector3 ReadVector(const rapidjson::Value& object, const char* name, const AZ::Vector3& fallback = AZ::Vector3::CreateZero())
    {
        const auto member = object.FindMember(name);
        if (member == object.MemberEnd() || !member->value.IsArray() || member->value.Size() != 3)
        {
            return fallback;
        }
        const auto& array = member->value;
        return AZ::Vector3(array[0].GetFloat(), array[1].GetFloat(), array[2].GetFloat());
    }

    bool LoadPrefab(const std::filesystem::path& file, const char* levelName, AZ::SerializeContext* serialize, LoadedLevel& level)
    {
        level = {};
        level.m_name = levelName;
        std::ifstream stream(file, std::ios::binary);
        const std::string text((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        rapidjson::Document document;
        document.Parse(text.c_str());
        if (document.HasParseError() || !document.HasMember("Entities"))
        {
            std::printf("cannot parse %s\n", file.string().c_str());
            return false;
        }
        AliasMapper mapper;
        const rapidjson::Value& entities = document["Entities"];
        for (auto it = entities.MemberBegin(); it != entities.MemberEnd(); ++it)
        {
            mapper.m_aliases[it->name.GetString()] = AZ::Entity::MakeId();
        }
        AZ::JsonDeserializerSettings settings;
        settings.m_serializeContext = serialize;
        settings.m_metadata.Add(static_cast<AZ::JsonEntityIdSerializer::JsonEntityIdMapper*>(&mapper));
        AZStd::string problems;
        settings.m_reporting = [&problems](AZStd::string_view message, AZ::JsonSerializationResult::ResultCode result, AZStd::string_view path)
        {
            if (result.GetOutcome() > AZ::JsonSerializationResult::Outcomes::PartialDefaults ||
                result.GetOutcome() == AZ::JsonSerializationResult::Outcomes::Skipped ||
                result.GetOutcome() == AZ::JsonSerializationResult::Outcomes::PartialSkip)
            {
                problems += AZStd::string::format("  %.*s: %.*s\n", AZ_STRING_ARG(path), AZ_STRING_ARG(message));
            }
            return result;
        };

        AZStd::vector<AZ::Entity*> inactive;
        for (auto it = entities.MemberBegin(); it != entities.MemberEnd(); ++it)
        {
            const rapidjson::Value& source = it->value;
            const AZ::EntityId id = mapper.m_aliases[it->name.GetString()];
            auto* entity = aznew AZ::Entity(id, source["Name"].GetString());
            level.m_byName[source["Name"].GetString()] = id;
            AZ::Vector3 translate = AZ::Vector3::CreateZero();
            AZ::Vector3 rotate = AZ::Vector3::CreateZero();
            bool trigger = false;
            AZ::Vector3 triggerSize = AZ::Vector3::CreateZero();
            AZ::Vector3 triggerOffset = AZ::Vector3::CreateZero();
            bool solidBox = false;
            bool meshCollider = false;
            AZ::Vector3 solidSize = AZ::Vector3::CreateZero();
            AZ::Vector3 solidOffset = AZ::Vector3::CreateZero();
            const rapidjson::Value& components = source["Components"];
            for (auto comp = components.MemberBegin(); comp != components.MemberEnd(); ++comp)
            {
                const rapidjson::Value& body = comp->value;
                const AZStd::string type = body["$type"].GetString();
                if (type.ends_with("TransformComponent"))
                {
                    if (body.HasMember("Transform Data"))
                    {
                        translate = ReadVector(body["Transform Data"], "Translate");
                        rotate = ReadVector(body["Transform Data"], "Rotate");
                    }
                }
                else if (type == "EditorCharacterControllerComponent")
                {
                    entity->CreateComponent<ProbeSupport::ProbeCharacterComponent>();
                    ++level.m_engineComponents;
                }
                else if (type == "EditorColliderComponent")
                {
                    const rapidjson::Value& collider = body["ColliderConfiguration"];
                    const rapidjson::Value& shape = body["ShapeConfiguration"];
                    if (collider.HasMember("Trigger") && collider["Trigger"].GetBool())
                    {
                        trigger = true;
                        triggerSize = ReadVector(shape["Box"], "Configuration");
                        triggerOffset = ReadVector(collider, "Position");
                    }
                    else if (shape.HasMember("Box"))
                    {
                        solidBox = true;
                        solidSize = ReadVector(shape["Box"], "Configuration");
                        solidOffset = ReadVector(collider, "Position");
                    }
                    ++level.m_engineComponents;
                }
                else if (type == "EditorMeshColliderComponent")
                {
                    meshCollider = true;
                    ++level.m_engineComponents;
                }
                else if (type == "GenericComponentWrapper")
                {
                    const rapidjson::Value& templ = body["m_template"];
                    const AZStd::string templateType = templ["$type"].GetString();
                    const AZ::Uuid typeId(templateType.substr(0, 38).c_str());
                    AZ::Component* component = CreateGameComponent(typeId);
                    if (!component)
                    {
                        ++level.m_engineComponents;  // counted: compiled elsewhere, not run here
                        continue;
                    }
                    rapidjson::Document fields;
                    fields.CopyFrom(templ, fields.GetAllocator());
                    fields.RemoveMember("$type");
                    GameComponentRecord record;
                    record.m_type = templateType.substr(39).c_str();
                    record.m_entityName = source["Name"].GetString();
                    record.m_entity = id;
                    for (auto field = fields.MemberBegin(); field != fields.MemberEnd(); ++field)
                    {
                        const rapidjson::Value& value = field->value;
                        if (value.IsString()) record.m_fields[field->name.GetString()] = value.GetString();
                        else if (value.IsBool()) record.m_fields[field->name.GetString()] = value.GetBool() ? "true" : "false";
                        else if (value.IsNumber()) record.m_fields[field->name.GetString()] = AZStd::string::format("%g", value.GetDouble());
                    }
                    level.m_records.push_back(AZStd::move(record));
                    problems.clear();
                    const auto result = AZ::JsonSerialization::Load(component, typeId, fields, settings);
                    const bool clean = result.GetProcessing() == AZ::JsonSerializationResult::Processing::Completed && problems.empty() &&
                        result.GetOutcome() != AZ::JsonSerializationResult::Outcomes::Skipped &&
                        result.GetOutcome() != AZ::JsonSerializationResult::Outcomes::PartialSkip &&
                        result.GetOutcome() <= AZ::JsonSerializationResult::Outcomes::PartialDefaults;
                    if (!clean)
                    {
                        ++level.m_rejectedComponents;
                        std::printf("rejected %s on %s:\n%s", templateType.c_str(), entity->GetName().c_str(), problems.c_str());
                        delete component;
                        continue;
                    }
                    entity->AddComponent(component);
                    ++level.m_gameComponents;
                }
                else
                {
                    ++level.m_engineComponents;
                }
            }
            auto* transform = entity->CreateComponent<ProbeSupport::ProbeTransformComponent>();
            transform->m_world = AZ::Transform::CreateFromQuaternionAndTranslation(
                AZ::Quaternion::CreateFromEulerDegreesXYZ(rotate), translate);
            if (solidBox)
            {
                // Boxes only yaw (the materialiser never pitches or rolls them).
                const float yaw = AZ::DegToRad(rotate.GetZ());
                const AZ::Vector3 offset = AZ::Quaternion::CreateRotationZ(yaw).TransformVector(solidOffset);
                level.m_solidBoxes.push_back({translate + offset, solidSize * 0.5f, yaw});
            }
            if (meshCollider)
            {
                level.m_meshColliderOrigins.push_back(translate);
            }
            if (trigger)
            {
                const AZ::Vector3 center = translate + triggerOffset;
                level.m_triggers.push_back({id, AZ::Aabb::CreateCenterHalfExtents(center, triggerSize * 0.5f), false});
            }
            entity->Init();
            level.m_entities.push_back(entity);
            if (source.HasMember("IsRuntimeActive") && !source["IsRuntimeActive"].GetBool())
            {
                inactive.push_back(entity);
            }
        }
        for (AZ::Entity* entity : level.m_entities)
        {
            if (AZStd::find(inactive.begin(), inactive.end(), entity) == inactive.end())
            {
                entity->Activate();
            }
        }
        return true;
    }

    void UnloadLevel(LoadedLevel& level)
    {
        for (AZ::Entity* entity : level.m_entities)
        {
            if (entity->GetState() == AZ::Entity::State::Active)
            {
                entity->Deactivate();
            }
        }
        for (AZ::Entity* entity : level.m_entities)
        {
            delete entity;
        }
        level = {};
    }

    AZ::Vector3 Position(const AZ::EntityId& id)
    {
        AZ::Vector3 position = AZ::Vector3::CreateZero();
        AZ::TransformBus::EventResult(position, id, &AZ::TransformBus::Events::GetWorldTranslation);
        return position;
    }

    /** PhysX trigger stand-in: the player's base point entering or leaving a materialised trigger box. */
    void UpdateTriggers(LoadedLevel& level, const AZ::EntityId& player)
    {
        const AZ::Vector3 point = Position(player);
        for (TriggerVolume& trigger : level.m_triggers)
        {
            const bool inside = trigger.m_box.Contains(point);
            if (inside == trigger.m_inside)
            {
                continue;
            }
            trigger.m_inside = inside;
            AZ::Entity* entity = ProbeGameEntityContext::Find(trigger.m_entity);
            if (!entity || entity->GetState() != AZ::Entity::State::Active)
            {
                continue;
            }
            if (auto* water = entity->FindComponent<DarkArisen::WaterVolumeComponent>())
            {
                inside ? water->NotifyEntered(player) : water->NotifyExited(player);
            }
            if (auto* story = entity->FindComponent<DarkArisen::StoryTriggerComponent>(); story && inside)
            {
                story->NotifyBodyEntered(player);
            }
            if (auto* actor = entity->FindComponent<DarkArisen::StoryActorComponent>())
            {
                inside ? static_cast<void>(actor->NotifyBodyEntered(player)) : actor->NotifyBodyExited(player);
            }
        }
    }

    void Tick(LoadedLevel& level, const AZ::EntityId& player, const float seconds)
    {
        const int frames = static_cast<int>(seconds * 60.0f + 0.5f);
        for (int frame = 0; frame < frames && g_requestedLevel.empty(); ++frame)
        {
            AZ::TickBus::ExecuteQueuedEvents();
            AZ::TickBus::Broadcast(&AZ::TickEvents::OnTick, 1.0f / 60.0f, AZ::ScriptTimePoint());
            if (player.IsValid())
            {
                UpdateTriggers(level, player);
            }
        }
    }

    /** Walks the player in 0.25 m steps (land movement stand-in), updating triggers on the way. */
    void Walk(LoadedLevel& level, const AZ::EntityId& player, const AZ::Vector3& target)
    {
        for (int step = 0; step < 4000 && g_requestedLevel.empty(); ++step)
        {
            const AZ::Vector3 position = Position(player);
            AZ::Vector3 delta = target - position;
            delta.SetZ(0.0f);
            if (delta.GetLength() < 0.2f)
            {
                break;
            }
            AZ::Vector3 next = position + delta.GetNormalized() * AZ::GetMin(0.25f, delta.GetLength());
            next.SetZ(target.GetZ());
            AZ::TransformBus::Event(player, &AZ::TransformBus::Events::SetWorldTranslation, next);
            Tick(level, player, 1.0f / 60.0f);
        }
    }

    /** A point stands on a materialised solid box when it is over the box and within 0.5 m of its top. */
    bool StandsOnSolidBox(const LoadedLevel& level, const AZ::Vector3& point)
    {
        for (const SolidBox& box : level.m_solidBoxes)
        {
            const AZ::Vector3 local = AZ::Quaternion::CreateRotationZ(-box.m_yawRadians).TransformVector(point - box.m_center);
            const float top = box.m_halfExtents.GetZ();
            if (AZ::GetAbs(local.GetX()) <= box.m_halfExtents.GetX() + 0.01f && AZ::GetAbs(local.GetY()) <= box.m_halfExtents.GetY() + 0.01f &&
                local.GetZ() >= top - 0.5f && local.GetZ() <= top + 0.5f)
            {
                return true;
            }
        }
        return false;
    }

    DarkArisen::Core::Combatant* CombatantOf(const AZ::EntityId& id)
    {
        DarkArisen::Core::Combatant* combatant = nullptr;
        DarkArisen::CombatRequestBus::Event(id, [&combatant](DarkArisen::CombatRequests* handler) { combatant = &handler->GetCombatant(); });
        return combatant;
    }

    bool IsActive(const AZ::EntityId& id)
    {
        AZ::Entity* entity = ProbeGameEntityContext::Find(id);
        return entity && entity->GetState() == AZ::Entity::State::Active;
    }
}
