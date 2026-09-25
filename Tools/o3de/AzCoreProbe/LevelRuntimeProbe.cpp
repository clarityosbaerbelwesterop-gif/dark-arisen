// O3DE 2605.0 level runtime probe.
//
// Loads the level prefabs written by `DarkArisenO3DE materialize`, deserialises every Dark Arisen
// game component with O3DE's own JSON serializer (unknown or mistyped fields fail the probe),
// instantiates the entities on a real AZ::ComponentApplication and plays Chapter 1 through the
// materialised content: Harlow family, cargo manifest, black sails, boarders, Draven, The Taking,
// the level transition, the Outer Reef swim, Driftwood Beach and the walk to Driftwood Camp.
//
// What stands in for engine systems that need a GPU or a PhysX scene: transforms, the character
// controller (velocity integration plus a beach ground profile), trigger overlap (point-in-box on
// the materialised trigger colliders), the game entity context (activate/deactivate) and the
// level system ("LoadLevel" console command). Rendering components are counted, not run.

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

#include <DarkArisen/CampaignBus.h>
#include <DarkArisen/CombatBus.h>
#include <DarkArisen/OceanBus.h>
#include <DarkArisen/SwimBus.h>
#include "Combat/CombatantComponent.h"
#include "Combat/EnemyBrainComponent.h"
#include "Player/PlayerSpawnComponent.h"
#include "Player/SwimmerComponent.h"
#include "Story/CampaignSystemComponent.h"
#include "Story/MapTransitionComponent.h"
#include "Story/OpeningDirectorComponent.h"
#include "Story/StoryTriggerComponent.h"
#include "World/OceanComponent.h"
#include "World/WaterVolumeComponent.h"

#include <DarkArisen/Core/CampaignRuntime.h>
#include <DarkArisen/Core/Combat.h>
#include <DarkArisen/Core/Facts.h>
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

    struct LoadedLevel
    {
        AZStd::string m_name;
        AZStd::vector<AZ::Entity*> m_entities;
        AZStd::unordered_map<AZStd::string, AZ::EntityId> m_byName;
        AZStd::vector<TriggerVolume> m_triggers;
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
                    if (collider.HasMember("Trigger") && collider["Trigger"].GetBool())
                    {
                        trigger = true;
                        triggerSize = ReadVector(body["ShapeConfiguration"]["Box"], "Configuration");
                        triggerOffset = ReadVector(collider, "Position");
                    }
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

int main(int argc, char** argv)
{
    const std::filesystem::path repo = argc > 1 ? std::filesystem::path(argv[1]) : std::filesystem::path(DARKARISEN_REPO_ROOT);
    const std::filesystem::path levels = repo / "Engine/O3DE/DarkArisen/Levels";
    const std::filesystem::path userDir = std::filesystem::temp_directory_path() / "darkarisen_level_probe";
    std::filesystem::remove_all(userDir);
    std::filesystem::create_directories(userDir);

    AZ::ComponentApplication app(argc, argv);
    AZ::ComponentApplication::Descriptor descriptor;
    AZ::ComponentApplication::StartupParameters startup;
    startup.m_loadSettingsRegistry = false;
    startup.m_loadDynamicModules = false;
    startup.m_loadAssetCatalog = false;
    AZ::Entity* systemEntity = app.Create(descriptor, startup);

    AZ::IO::LocalFileIO fileIO;
    AZ::IO::FileIOBase* previousIO = AZ::IO::FileIOBase::GetInstance();
    AZ::IO::FileIOBase::SetInstance(nullptr);
    AZ::IO::FileIOBase::SetInstance(&fileIO);
    fileIO.SetAlias("@user@", userDir.string().c_str());

    for (const AZ::ComponentDescriptor* componentDescriptor :
         {DarkArisen::CampaignSystemComponent::CreateDescriptor(), DarkArisen::CombatantComponent::CreateDescriptor(),
             DarkArisen::EnemyBrainComponent::CreateDescriptor(), DarkArisen::StoryTriggerComponent::CreateDescriptor(),
             DarkArisen::OpeningDirectorComponent::CreateDescriptor(), DarkArisen::MapTransitionComponent::CreateDescriptor(),
             DarkArisen::PlayerSpawnComponent::CreateDescriptor(), DarkArisen::SwimmerComponent::CreateDescriptor(),
             DarkArisen::WaterVolumeComponent::CreateDescriptor(), DarkArisen::OceanComponent::CreateDescriptor(),
             ProbeSupport::ProbeTransformComponent::CreateDescriptor(), ProbeSupport::ProbeCharacterComponent::CreateDescriptor()})
    {
        app.RegisterComponentDescriptor(componentDescriptor);
    }
    systemEntity->CreateComponent<DarkArisen::CampaignSystemComponent>();
    systemEntity->Init();
    systemEntity->Activate();
    auto* campaign = DarkArisen::CampaignInterface::Get();
    Check(campaign != nullptr, "campaign system component active");
    ProbeGameEntityContext gameContext;
    AZ::SerializeContext* serialize = app.GetSerializeContext();

    const auto open = [&](const char* name, LoadedLevel& level)
    {
        AzFramework::LevelSystemLifecycleNotificationBus::Broadcast(&AzFramework::LevelSystemLifecycleNotifications::OnLoadingStart, name);
        const bool loaded = LoadPrefab(levels / name / (std::string(name) + ".prefab"), name, serialize, level);
        AzFramework::LevelSystemLifecycleNotificationBus::Broadcast(&AzFramework::LevelSystemLifecycleNotifications::OnLoadingComplete, name);
        return loaded;
    };

    // ---- L_HarlowOpening: Chapter 1, missions 1-3.
    LoadedLevel level;
    Check(open("L_HarlowOpening", level), "L_HarlowOpening.prefab parsed");
    std::printf("  %d game components loaded, %d rejected, %d engine components counted\n", level.m_gameComponents,
        level.m_rejectedComponents, level.m_engineComponents);
    Check(level.m_gameComponents == 16 && level.m_rejectedComponents == 0,
        "Harlow: every game component deserialised by O3DE's JSON serializer with no unknown fields");
    const AZ::EntityId jake = level.Id("Jake");
    Check(Position(jake).IsClose(AZ::Vector3(-9.0f, 0.0f, 2.2f)), "Harlow: player spawn resolved to Spawn.Jake.HarlowOpening");
    const AZ::EntityId boarders[] = {level.Id("Boarder Boarding.Port.A"), level.Id("Boarder Boarding.Port.B"), level.Id("Boarder Boarding.Starboard.A")};
    Check(!IsActive(boarders[0]) && !IsActive(boarders[1]) && !IsActive(boarders[2]), "Harlow: boarders placed inactive");

    bool accepted = false;
    DarkArisen::StoryTriggerRequestBus::EventResult(accepted, level.Id("Cargo Manifest"), &DarkArisen::StoryTriggerRequests::Interact,
        AZStd::string("character.jake"));
    Check(!accepted, "cargo manifest refused before the family is spoken to");
    for (const char* member : {"Family marc", "Family denise", "Family ethan"})
    {
        accepted = false;
        DarkArisen::StoryTriggerRequestBus::EventResult(accepted, level.Id(member), &DarkArisen::StoryTriggerRequests::Interact,
            AZStd::string("character.jake"));
        Check(accepted, member);
    }
    DarkArisen::Core::CampaignRuntime* runtime = &campaign->GetCampaign();
    Check(runtime->GetMissionState("Main.C01.01.HomeWater") == DarkArisen::Core::MissionState::Completed, "Home Water complete");
    DarkArisen::StoryTriggerRequestBus::EventResult(accepted, level.Id("Cargo Manifest"), &DarkArisen::StoryTriggerRequests::Interact,
        AZStd::string("character.jake"));
    Check(accepted, "cargo manifest: black sails sighted");
    Tick(level, jake, 7.0f);
    Check(IsActive(boarders[0]) && IsActive(boarders[1]) && IsActive(boarders[2]), "black sails presentation released three boarders");
    Check(campaign->GetOpening().Progress().RaidState == DarkArisen::Core::OpeningRaidState::FirstEncounter, "boarding encounter running");
    Check(Position(boarders[0]).GetY() > 0.0f && Position(boarders[2]).GetY() < 0.0f,
        "Harlow: port boarders on +Y (left when facing the bow), starboard on -Y");
    for (const AZ::EntityId& boarder : boarders)
    {
        if (DarkArisen::Core::Combatant* combatant = CombatantOf(boarder))
        {
            combatant->Health.ApplyDamage(10000.0f);
        }
    }
    Tick(level, jake, 0.5f);
    Check(campaign->GetOpening().Progress().RaidState == DarkArisen::Core::OpeningRaidState::DravenAboard, "deck cleared: Draven boards");
    Tick(level, jake, 4.0f);
    Check(campaign->GetOpening().Progress().RaidState == DarkArisen::Core::OpeningRaidState::Taking, "Draven presentation: The Taking starts");
    Tick(level, jake, 17.0f);
    Check(!IsActive(level.Id("Family marc")) && !IsActive(level.Id("Family denise")) && !IsActive(level.Id("Family ethan")),
        "The Taking: Marc and Denise fall, Ethan is seized alive");
    Check(Position(jake).IsClose(Position(level.Id("Anchor Overboard"))), "Jake forced over the rail at the Overboard anchor");
    Check(runtime->HasFact(DarkArisen::Core::Facts::EthanAbducted) && !runtime->HasFact(DarkArisen::Core::Facts::EthanRecovered),
        "canon: Ethan abducted alive, not recovered");
    Tick(level, jake, 2.0f);
    Check(runtime->GetMissionState("Main.C01.04.Undertow") == DarkArisen::Core::MissionState::Active, "Undertow active");
    Check(g_requestedLevel == "L_DriftwoodBeach", "map transition travels to L_DriftwoodBeach");
    Check(runtime->State().PlayerRuntime.SourceLevel == "L_HarlowOpening", "travel captured the player in its source level");

    // ---- L_DriftwoodBeach: Undertow.
    g_requestedLevel.clear();
    UnloadLevel(level);
    Check(open("L_DriftwoodBeach", level), "L_DriftwoodBeach.prefab parsed");
    std::printf("  %d game components loaded, %d rejected, %d engine components counted\n", level.m_gameComponents,
        level.m_rejectedComponents, level.m_engineComponents);
    Check(level.m_gameComponents == 13 && level.m_rejectedComponents == 0, "Driftwood: every game component deserialised cleanly");
    const AZ::EntityId swimmer = level.Id("Jake");
    Check(Position(swimmer).IsClose(Position(level.Id("Anchor WaterEntry"))), "overboard arrival spawns Jake in the water, not on the beach");
    if (auto* character = ProbeGameEntityContext::Find(swimmer)->FindComponent<ProbeSupport::ProbeCharacterComponent>())
    {
        // Beach profile from the layout: seabed about 2 m down offshore, rising to 0.55 m at the recovery
        // point; inland the walk follows the authored anchor heights.
        character->m_ground = [](const float x, float)
        {
            return x < -20.0f ? -2.0f : AZ::GetMin(0.55f, -2.0f + (x + 20.0f) * (2.55f / 11.0f));
        };
    }
    Tick(level, swimmer, 0.2f);
    bool swimming = false;
    DarkArisen::SwimRequestBus::EventResult(swimming, swimmer, &DarkArisen::SwimRequests::IsSwimming);
    Check(swimming, "Current.A volume: Jake swims");
    Check(campaign->GetOpening().Progress().Location == DarkArisen::Core::OpeningLocation::OpenWater, "entered water signalled");

    // Death in the water: after the two-second death beat Jake is back at the overboard arrival,
    // swimming again with full vitals (the trigger never re-fires for a body placed inside it).
    AZ::TransformBus::Event(swimmer, &AZ::TransformBus::Events::SetWorldTranslation, AZ::Vector3(-60.0f, -5.0f, -1.4f));
    if (DarkArisen::Core::Combatant* body = CombatantOf(swimmer))
    {
        body->Health.ApplyDamage(10000.0f, DarkArisen::Core::RallyDamageClass::Environmental);
    }
    Tick(level, swimmer, 1.5f);
    bool dead = false;
    DarkArisen::CombatRequestBus::EventResult(dead, swimmer, &DarkArisen::CombatRequests::IsDead);
    Check(dead, "death beat: still down 1.5 s after dying");
    Tick(level, swimmer, 1.0f);
    DarkArisen::CombatRequestBus::EventResult(dead, swimmer, &DarkArisen::CombatRequests::IsDead);
    DarkArisen::SwimRequestBus::EventResult(swimming, swimmer, &DarkArisen::SwimRequests::IsSwimming);
    float health = 0.0f;
    DarkArisen::CombatRequestBus::EventResult(health, swimmer, &DarkArisen::CombatRequests::GetHealthFraction);
    Check(!dead && health == 1.0f && swimming && Position(swimmer).GetDistance(Position(level.Id("Anchor WaterEntry"))) < 3.0f,
        "respawn at the overboard arrival, full health, swimming again (no failure screen)");
    const AZ::Vector3 waypoints[] = {Position(level.Id("Water Reef.SafeGap")), Position(level.Id("Water Shallows")), AZ::Vector3(-11.0f, 0.0f, 0.0f)};
    for (const AZ::Vector3& waypoint : waypoints)
    {
        for (int frame = 0; frame < 60 * 60 && g_requestedLevel.empty(); ++frame)
        {
            AZ::Vector3 delta = waypoint - Position(swimmer);
            delta.SetZ(0.0f);
            if (delta.GetLength() < 1.5f)
            {
                break;
            }
            DarkArisen::SwimRequestBus::Event(swimmer, &DarkArisen::SwimRequests::SetSwimIntent,
                AZ::Vector2(delta.GetX(), delta.GetY()).GetNormalized(), false);
            Tick(level, swimmer, 1.0f / 60.0f);
        }
    }
    Check(campaign->GetOpening().Progress().Location == DarkArisen::Core::OpeningLocation::OuterReef, "Outer Reef safe gap reached by swimming");
    DarkArisen::SwimRequestBus::EventResult(swimming, swimmer, &DarkArisen::SwimRequests::IsSwimming);
    Check(!swimming, "shallow exit at wading depth: Jake walks out");
    Walk(level, swimmer, Position(level.Id("Anchor Spawn.Moran.DriftwoodBeach.Recovery")));
    Check(campaign->GetOpening().Progress().Location == DarkArisen::Core::OpeningLocation::DriftwoodBeach, "Driftwood Beach reached");
    Walk(level, swimmer, Position(level.Id("Beat Camp Smoke Sighted")));
    Check(runtime->GetMissionState("Main.C01.04.Undertow") == DarkArisen::Core::MissionState::Completed &&
              runtime->GetMissionState("Main.C02.01.ShatteredCoast") == DarkArisen::Core::MissionState::Active,
        "camp smoke sighted: Undertow complete, Shattered Coast active");
    Check(runtime->State().SpawnId == "Spawn.Moran.DriftwoodBeach.Recovery", "beach checkpoint set");
    std::error_code ignored;
    bool autosave = false;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(userDir, ignored))
    {
        autosave = autosave || entry.path().extension() == ".dasave";
    }
    Check(autosave, "chapter 1 -> 2 boundary wrote an autosave through O3DE LocalFileIO");
    // Death after the recovery: back at the beach checkpoint.
    if (DarkArisen::Core::Combatant* body = CombatantOf(swimmer))
    {
        body->Health.ApplyDamage(10000.0f);
    }
    Tick(level, swimmer, 2.5f);
    DarkArisen::CombatRequestBus::EventResult(dead, swimmer, &DarkArisen::CombatRequests::IsDead);
    Check(!dead && Position(swimmer).GetDistance(Position(level.Id("Anchor Spawn.Moran.DriftwoodBeach.Recovery"))) < 0.5f,
        "respawn at the Driftwood Beach checkpoint");
    Walk(level, swimmer, Position(level.Id("Beat Route Driftwood Camp")));
    Tick(level, swimmer, 0.2f);
    Check(runtime->HasFact("World.DriftwoodCampReached") && g_requestedLevel == "L_DriftwoodCamp", "camp route: travel to L_DriftwoodCamp");

    // ---- L_DriftwoodCamp.
    g_requestedLevel.clear();
    UnloadLevel(level);
    Check(open("L_DriftwoodCamp", level), "L_DriftwoodCamp.prefab parsed");
    Check(level.m_gameComponents == 3 && level.m_rejectedComponents == 0, "Camp: every game component deserialised cleanly");
    Check(Position(level.Id("Jake")).IsClose(Position(level.Id("Anchor Arrival"))), "camp arrival spawn");
    Check(campaign->GetOpening().Progress().Location == DarkArisen::Core::OpeningLocation::DriftwoodCamp, "vertical slice ends at Driftwood Camp");

    UnloadLevel(level);
    systemEntity->Deactivate();
    AZ::IO::FileIOBase::SetInstance(nullptr);
    AZ::IO::FileIOBase::SetInstance(previousIO);
    app.Destroy();
    std::filesystem::remove_all(userDir);
    std::printf("%s (%d failed)\n", g_failures == 0 ? "O3DE level runtime probe PASSED" : "O3DE level runtime probe FAILED", g_failures);
    return g_failures == 0 ? 0 : 1;
}
