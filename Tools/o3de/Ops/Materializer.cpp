#include "Materializer.h"

#include "Gltf.h"
#include "Hash.h"
#include "Json.h"
#include "O3deIds.h"
#include "Prefab.h"
#include "StoryContracts.h"

#include <DarkArisen/AuthoringEnums.h>
#include <DarkArisen/DarkArisenTypeIds.h>

#include <DarkArisen/Core/CampaignState.h>
#include <DarkArisen/Core/LevelSpawn.h>
#include <DarkArisen/Core/MissionCatalog.h>
#include <DarkArisen/Core/Ocean.h>
#include <DarkArisen/Core/Presentation.h>
#include <DarkArisen/Core/StoryWorld.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;

namespace DarkArisen::Tools
{
    namespace
    {
        const std::string ProjectDir = "Engine/O3DE/DarkArisen";
        const std::string GreyboxDir = "Assets/Greybox";  // relative to the project (the Asset Processor scan root)
        constexpr int MaterializerVersion = 2;

        // Unreal materialiser constants (DarkArisenMaterializeAlphaCommandlet.cpp, BuildHarlow).
        constexpr const char* HarlowEncounter = "Encounter.HarlowRaid.MainDeck";
        constexpr int HarlowRequiredBoarders = 3;
        constexpr const char* BoarderAnchors[] = {"Boarding.Port.A", "Boarding.Port.B", "Boarding.Starboard.A"};
        constexpr double DravenSilhouetteOffsetCentimetres = 9000.0;

        // Jake (AJakeCharacter: capsule radius 42 cm, half-height 96 cm; camera boom 360 cm).
        constexpr double JakeCapsuleHeight = 1.92;
        constexpr double JakeCapsuleRadius = 0.42;

        // PROVISIONAL authoring defaults: ContentSource gives these beats a point, not a size.
        constexpr Vec3d BeatVolumeSize{6.0, 6.0, 4.0};
        constexpr double FamilyFigureHeight = 1.8;
        constexpr double FamilyFigureRadius = 0.35;
        constexpr Vec3d ManifestCrateSize{0.8, 0.6, 0.5};

        struct ModelRef
        {
            std::string Guid;
            std::uint32_t SubId = 0;
            std::string Hint;
        };

        std::string Lower(std::string Text)
        {
            std::transform(Text.begin(), Text.end(), Text.begin(), [](const char Character)
            {
                return (Character >= 'A' && Character <= 'Z') ? static_cast<char>(Character + ('a' - 'A')) : Character;
            });
            return Text;
        }

        std::string Stem(const std::string& Path)
        {
            return fs::path(Path).stem().string();
        }

        std::string Directory(const std::string& Path)
        {
            return fs::path(Path).parent_path().generic_string();
        }

        class Session
        {
        public:
            Session(fs::path InRepo, MaterializeResult& InOut) : Repo(std::move(InRepo)), Out(InOut) {}

            bool Ok() const { return Out.Errors.empty(); }
            void Error(std::string Message) { Out.Errors.push_back(std::move(Message)); }

            const std::string* Text(const std::string& Relative)
            {
                if (const auto Found = Texts.find(Relative); Found != Texts.end())
                {
                    return &Found->second;
                }
                std::ifstream Stream(Repo / Relative, std::ios::binary);
                if (!Stream)
                {
                    Error("missing input " + Relative);
                    return nullptr;
                }
                std::string Content((std::istreambuf_iterator<char>(Stream)), std::istreambuf_iterator<char>());
                Inputs[Relative] = Sha256Hex(Content);
                return &Texts.emplace(Relative, std::move(Content)).first->second;
            }

            const JsonValue* Json(const std::string& Relative)
            {
                if (const auto Found = Parsed.find(Relative); Found != Parsed.end())
                {
                    return &Found->second;
                }
                const std::string* Content = Text(Relative);
                if (!Content)
                {
                    return nullptr;
                }
                JsonValue Value;
                std::string Problem;
                if (!JsonReader::Parse(*Content, Value, Problem))
                {
                    Error(Relative + ": " + Problem);
                    return nullptr;
                }
                return &Parsed.emplace(Relative, std::move(Value)).first->second;
            }

            /** Required anchor in centimetres (Unreal TryReadRequiredAnchor semantics). */
            bool Anchor(const std::string& Layout, const std::string& Key, Vec3d& OutCentimetres)
            {
                const JsonValue* Root = Json(Layout);
                const JsonValue* Anchors = Root ? Root->Find("anchors") : nullptr;
                const JsonValue* Value = Anchors ? Anchors->Find(Key) : nullptr;
                if (!Value || !Value->IsArray() || Value->Items.size() != 3)
                {
                    Error(Layout + ": required anchor " + Key + " missing or not [x,y,z]");
                    return false;
                }
                double Components[3] = {};
                for (int Index = 0; Index < 3; ++Index)
                {
                    const JsonValue& Item = Value->Items[static_cast<std::size_t>(Index)];
                    if (!Item.IsNumber() || !std::isfinite(Item.Number))
                    {
                        Error(Layout + ": anchor " + Key + " is not finite");
                        return false;
                    }
                    Components[Index] = Item.Number;
                }
                OutCentimetres = {Components[0], Components[1], Components[2]};
                return true;
            }

            Vec3d World(const std::string& Layout, const std::string& Key)
            {
                Vec3d Centimetres;
                return Anchor(Layout, Key, Centimetres) ? LayoutToWorld(Centimetres) : Vec3d{};
            }

            bool HasOutput(const std::string& Relative) const { return Outputs.count(Relative) != 0; }

            /** ContentSource glTF by asset name (an Unreal object path's last segment); exactly one must exist. */
            std::string FindContentGltf(const std::string& AssetName)
            {
                if (GltfIndex.empty())
                {
                    std::error_code Failure;
                    for (auto It = fs::recursive_directory_iterator(Repo / "ContentSource", Failure);
                         !Failure && It != fs::recursive_directory_iterator(); It.increment(Failure))
                    {
                        if (It->is_regular_file() && It->path().extension() == ".gltf")
                        {
                            const std::string Relative = fs::relative(It->path(), Repo).generic_string();
                            GltfIndex[It->path().stem().string()].push_back(Relative);
                        }
                    }
                }
                const auto Found = GltfIndex.find(AssetName);
                if (Found == GltfIndex.end() || Found->second.size() != 1)
                {
                    Error("geometry " + AssetName + ": expected exactly one ContentSource/**/" + AssetName + ".gltf");
                    return {};
                }
                return Found->second.front();
            }

            /** One row of the 34-mission physical coverage report. */
            void Cover(const std::string& MissionId, const std::string& Level, const std::string& Entry, const std::string& Completion,
                const std::string& Checkpoint, const std::string& Next)
            {
                JsonValue Row = JsonObject();
                Row.Members["level"] = JsonString(Level);
                Row.Members["entry"] = JsonString(Entry);
                Row.Members["completion"] = JsonString(Completion);
                Row.Members["checkpoint"] = JsonString(Checkpoint);
                Row.Members["next"] = JsonString(Next);
                if (!Coverage.emplace(MissionId, std::move(Row)).second) Error("mission " + MissionId + " covered twice");
            }

            const std::map<std::string, JsonValue>& CoverageRows() const { return Coverage; }

            void Emit(const std::string& Relative, std::string Content)
            {
                Outputs[Relative] = Sha256Hex(Content);
                Out.Files.push_back({Relative, std::move(Content)});
            }

            /** Project-relative greybox destination for a ContentSource mesh. */
            static std::string GreyboxPath(const std::string& SourceRelative, const std::string& StemOverride = {})
            {
                std::string Inside = SourceRelative.substr(std::string("ContentSource/").size());
                if (!StemOverride.empty())
                {
                    Inside = Directory(Inside) + "/" + StemOverride + ".gltf";
                }
                return GreyboxDir + "/" + Inside;
            }

            ModelRef Model(const std::string& ProjectRelative)
            {
                const std::string Stem_ = Stem(ProjectRelative);
                return {SourceAssetUuid(ProjectRelative), AtomModelSubId(Stem_), Lower(Directory(ProjectRelative) + "/" + Stem_ + ".azmodel")};
            }

            /** Converts a ContentSource greybox mesh for rendering. */
            ModelRef RenderModel(const std::string& SourceRelative, const bool BindPose = false, const std::string& StemOverride = {})
            {
                const std::string Destination = GreyboxPath(SourceRelative, StemOverride);
                if (!Converted.count(Destination))
                {
                    const std::string* Source = Text(SourceRelative);
                    if (!Source)
                    {
                        return {};
                    }
                    std::string Result;
                    std::string Problem;
                    GltfConversionStats Stats;
                    GltfConversionOptions Options;
                    Options.StripSkinToBindPose = BindPose;
                    if (!ConvertGreyboxGltf(*Source, Result, Stats, Problem, Options))
                    {
                        Error(SourceRelative + ": " + Problem);
                        return {};
                    }
                    Emit(ProjectDir + "/" + Destination, std::move(Result));
                    Converted.insert(Destination);
                }
                return Model(Destination);
            }

            ModelRef GeneratedModel(const std::string& Name, const std::string& SourceFrameGltf)
            {
                const std::string Destination = GreyboxDir + "/Placeholders/" + Name + ".gltf";
                if (!Converted.count(Destination))
                {
                    std::string Result;
                    std::string Problem;
                    GltfConversionStats Stats;
                    if (!ConvertGreyboxGltf(SourceFrameGltf, Result, Stats, Problem))
                    {
                        Error(Name + ": " + Problem);
                        return {};
                    }
                    Emit(ProjectDir + "/" + Destination, std::move(Result));
                    Converted.insert(Destination);
                }
                return Model(Destination);
            }

            /**
             * Collision copy of a greybox mesh with a PhysX-only scene manifest: the copy yields a
             * triangle-mesh .pxmesh and no render model. Sub-id: SceneBuilderWorker::BuildSubId of
             * AZ::Uuid::CreateName(group id string).
             */
            ModelRef CollisionMesh(const std::string& SourceRelative, const std::string& StemOverride = {})
            {
                const std::string BaseStem = StemOverride.empty() ? Stem(SourceRelative) : StemOverride;
                const std::string Destination = GreyboxPath(SourceRelative, BaseStem + "_Collision");
                const std::string GroupId = AzUuidFromName("DarkArisen.PhysXMeshGroup:" + Lower(Destination));
                if (!Converted.count(Destination))
                {
                    const std::string* Source = Text(SourceRelative);
                    if (!Source)
                    {
                        return {};
                    }
                    std::string Result;
                    std::string Problem;
                    GltfConversionStats Stats;
                    if (!ConvertGreyboxGltf(*Source, Result, Stats, Problem))
                    {
                        Error(SourceRelative + ": " + Problem);
                        return {};
                    }
                    Emit(ProjectDir + "/" + Destination, std::move(Result));
                    Emit(ProjectDir + "/" + Destination + ".assetinfo", PhysXManifest(GroupId, Stem(Destination)));
                    Converted.insert(Destination);
                }
                const std::array<std::uint8_t, 20> Digest = Sha1(GroupId);
                const std::uint32_t SubId = static_cast<std::uint32_t>(Digest[0]) | (static_cast<std::uint32_t>(Digest[1]) << 8) |
                    (static_cast<std::uint32_t>(Digest[2]) << 16) | (static_cast<std::uint32_t>(Digest[3]) << 24);
                return {SourceAssetUuid(Destination), SubId, Lower(Directory(Destination) + "/" + Stem(Destination) + ".pxmesh")};
            }

            std::string Manifest() const
            {
                JsonValue Root = JsonObject();
                Root.Members["tool"] = JsonString("DarkArisenO3DE materialize");
                Root.Members["version"] = JsonNumber(MaterializerVersion);
                JsonValue InputList = JsonObject();
                for (const auto& [Path, Hash] : Inputs) InputList.Members[Path] = JsonString(Hash);
                JsonValue OutputList = JsonObject();
                for (const auto& [Path, Hash] : Outputs) OutputList.Members[Path] = JsonString(Hash);
                Root.Members["inputs"] = std::move(InputList);
                Root.Members["outputs"] = std::move(OutputList);
                return JsonWriter::Write(Root, 2);
            }

        private:
            fs::path Repo;
            MaterializeResult& Out;
            std::map<std::string, std::string> Texts;
            std::map<std::string, JsonValue> Parsed;
            std::map<std::string, std::string> Inputs;
            std::map<std::string, std::string> Outputs;
            std::set<std::string> Converted;
            std::map<std::string, std::vector<std::string>> GltfIndex;
            std::map<std::string, JsonValue> Coverage;

            static std::string PhysXManifest(const std::string& GroupId, const std::string& Name)
            {
                // Layout of a PhysX mesh group as the O3DE scene settings write it. A non-empty
                // unselected list that names no node selects every mesh node (SceneGraphSelector).
                JsonValue Slot = JsonObject();
                Slot.Members["Name"] = JsonString("Entire object");
                JsonValue Slots = JsonArray();
                Slots.Items.push_back(std::move(Slot));
                JsonValue Materials = JsonObject();
                Materials.Members["Slots"] = std::move(Slots);
                JsonValue Unselected = JsonArray();
                Unselected.Items.push_back(JsonString("DarkArisen.NoNode"));
                JsonValue Selection = JsonObject();
                Selection.Members["unselectedNodes"] = std::move(Unselected);
                JsonValue Group = JsonObject();
                Group.Members["$type"] = JsonString("{5B03C8E6-8CEE-4DA0-A7FA-CD88689DD45B} MeshGroup");
                Group.Members["id"] = JsonString(GroupId);
                Group.Members["name"] = JsonString(Name);
                Group.Members["NodeSelectionList"] = std::move(Selection);
                Group.Members["export method"] = JsonNumber(0);  // TriMesh
                Group.Members["PhysicsMaterialSlots"] = std::move(Materials);
                JsonValue Values = JsonArray();
                Values.Items.push_back(std::move(Group));
                JsonValue Root = JsonObject();
                Root.Members["values"] = std::move(Values);
                return JsonWriter::Write(Root, 4);
            }
        };

        // ----------------------------------------------------------------------------- components

        JsonValue MeshComponent(const ModelRef& Model)
        {
            JsonValue Configuration = JsonObject();
            Configuration.Members["ModelAsset"] = AssetReference(Model.Guid, Model.SubId, Model.Hint);
            JsonValue Controller = JsonObject();
            Controller.Members["Configuration"] = std::move(Configuration);
            JsonValue Body = JsonObject();
            Body.Members["Controller"] = std::move(Controller);
            return Body;
        }

        JsonValue MaterialSlots()
        {
            JsonValue Slot = JsonObject();
            Slot.Members["Name"] = JsonString("Entire object");
            JsonValue Slots = JsonArray();
            Slots.Items.push_back(std::move(Slot));
            JsonValue Materials = JsonObject();
            Materials.Members["Slots"] = std::move(Slots);
            return Materials;
        }

        JsonValue ColliderConfiguration(const bool Trigger, const Vec3d& Offset = {})
        {
            JsonValue Configuration = JsonObject();
            Configuration.Members["MaterialSlots"] = MaterialSlots();
            if (Trigger)
            {
                Configuration.Members["Trigger"] = JsonBool(true);
                Configuration.Members["InSceneQueries"] = JsonBool(false);
            }
            if (Offset.X != 0.0 || Offset.Y != 0.0 || Offset.Z != 0.0)
            {
                Configuration.Members["Position"] = JsonVector(Offset);
            }
            return Configuration;
        }

        void AddBoxCollider(PrefabBuilder& Prefab, const std::string& Entity, const Vec3d& Size, const bool Trigger, const Vec3d& Offset = {})
        {
            JsonValue Box = JsonObject();
            Box.Members["Configuration"] = JsonVector(Size);
            JsonValue Shape = JsonObject();
            Shape.Members["ShapeType"] = JsonNumber(1);  // Physics::ShapeType::Box
            Shape.Members["Box"] = std::move(Box);
            JsonValue Body = JsonObject();
            Body.Members["ColliderConfiguration"] = ColliderConfiguration(Trigger, Offset);
            Body.Members["ShapeConfiguration"] = std::move(Shape);
            Prefab.AddComponent(Entity, "EditorStaticRigidBodyComponent");
            Prefab.AddComponent(Entity, "EditorColliderComponent", std::move(Body));
        }

        void AddCapsuleCollider(PrefabBuilder& Prefab, const std::string& Entity, const double Height, const double Radius)
        {
            JsonValue Capsule = JsonObject();
            Capsule.Members["Height"] = JsonNumber(Height);
            Capsule.Members["Radius"] = JsonNumber(Radius);
            JsonValue Shape = JsonObject();
            Shape.Members["ShapeType"] = JsonNumber(2);  // Physics::ShapeType::Capsule
            Shape.Members["Capsule"] = std::move(Capsule);
            JsonValue Body = JsonObject();
            Body.Members["ColliderConfiguration"] = ColliderConfiguration(false, {0.0, 0.0, Height / 2.0});
            Body.Members["ShapeConfiguration"] = std::move(Shape);
            Prefab.AddComponent(Entity, "EditorStaticRigidBodyComponent");
            Prefab.AddComponent(Entity, "EditorColliderComponent", std::move(Body));
        }

        void AddMeshCollider(PrefabBuilder& Prefab, const std::string& Entity, const ModelRef& Collision)
        {
            JsonValue Proxy = JsonObject();
            Proxy.Members["Asset"] = AssetReference(Collision.Guid, Collision.SubId, Collision.Hint);
            JsonValue Shape = JsonObject();
            Shape.Members["PhysicsAsset"] = std::move(Proxy);
            JsonValue Body = JsonObject();
            Body.Members["ColliderConfiguration"] = ColliderConfiguration(false);
            Body.Members["ShapeConfiguration"] = std::move(Shape);
            Prefab.AddComponent(Entity, "EditorStaticRigidBodyComponent");
            Prefab.AddComponent(Entity, "EditorMeshColliderComponent", std::move(Body));
        }

        void AddCharacterPhysics(PrefabBuilder& Prefab, const std::string& Entity)
        {
            JsonValue Capsule = JsonObject();
            Capsule.Members["Height"] = JsonNumber(JakeCapsuleHeight);
            Capsule.Members["Radius"] = JsonNumber(JakeCapsuleRadius);
            JsonValue Shape = JsonObject();
            Shape.Members["ShapeType"] = JsonNumber(2);
            Shape.Members["Capsule"] = std::move(Capsule);
            JsonValue Body = JsonObject();
            Body.Members["ShapeConfig"] = std::move(Shape);
            Prefab.AddComponent(Entity, "EditorCharacterControllerComponent", std::move(Body));
            Prefab.AddComponent(Entity, "{3BA7C3CB-C471-4230-8EC9-9EC4C529436F} EditorCharacterGameplayComponent");
        }

        JsonValue StoryTrigger(const StorySignal Signal, const std::string& Argument = {}, const int IntegerArgument = 0,
            const bool RequiresInteract = false)
        {
            JsonValue Fields = JsonObject();
            Fields.Members["Signal"] = JsonNumber(static_cast<int>(Signal));
            Fields.Members["Argument"] = JsonString(Argument);
            Fields.Members["IntegerArgument"] = JsonNumber(IntegerArgument);
            Fields.Members["RequiresInteract"] = JsonBool(RequiresInteract);
            Fields.Members["OneShot"] = JsonBool(true);
            return Fields;
        }

        void AddGame(PrefabBuilder& Prefab, const std::string& Entity, const char* TypeId, const char* Name, JsonValue Fields = JsonObject())
        {
            Prefab.AddGameComponent(Entity, TypeId, Name, std::move(Fields));
        }

        /** Sun and sky copied from the project's DefaultLevel (O3DE 2605.0 template). */
        void AddEnvironment(Session& Work, PrefabBuilder& Prefab)
        {
            const JsonValue* Default = Work.Json(ProjectDir + "/Levels/DefaultLevel/DefaultLevel.prefab");
            const JsonValue* Entities = Default ? Default->Find("Entities") : nullptr;
            if (!Entities)
            {
                Work.Error("DefaultLevel.prefab has no entities to take the sun and sky from");
                return;
            }
            static const std::set<std::string> Companions = {"EditorDisabledCompositionComponent", "EditorEntityIconComponent",
                "EditorEntitySortComponent", "EditorInspectorComponent", "EditorLockComponent", "EditorOnlyEntityComponent",
                "EditorPendingCompositionComponent", "EditorVisibilityComponent"};
            for (const char* Wanted : {"Sun", "Global Sky"})
            {
                const JsonValue* Source = nullptr;
                for (const auto& [Alias, Entity] : Entities->Members)
                {
                    const JsonValue* Name = Entity.Find("Name");
                    if (Name && Name->Text == Wanted) Source = &Entity;
                }
                if (!Source)
                {
                    Work.Error(std::string("DefaultLevel.prefab has no entity ") + Wanted);
                    continue;
                }
                Vec3d Translate;
                Vec3d Rotate;
                std::vector<const JsonValue*> Copied;
                for (const auto& [Key, Component] : Source->Find("Components")->Members)
                {
                    const std::string Type = Component.Find("$type") ? Component.Find("$type")->Text : "";
                    if (Companions.count(Type)) continue;
                    if (Type.find("TransformComponent") != std::string::npos)
                    {
                        const JsonValue* Data = Component.Find("Transform Data");
                        const auto Read = [](const JsonValue* Array, Vec3d& Target)
                        {
                            if (Array && Array->IsArray() && Array->Items.size() == 3)
                                Target = {Array->Items[0].Number, Array->Items[1].Number, Array->Items[2].Number};
                        };
                        if (Data)
                        {
                            Read(Data->Find("Translate"), Translate);
                            Read(Data->Find("Rotate"), Rotate);
                        }
                        continue;
                    }
                    Copied.push_back(&Component);
                }
                const std::string Entity = Prefab.AddEntity(Wanted, Translate, Rotate);
                for (const JsonValue* Component : Copied)
                {
                    JsonValue Body = *Component;
                    const std::string Type = Body.Find("$type")->Text;
                    Body.Members.erase("$type");
                    Body.Members.erase("Id");
                    Prefab.AddComponent(Entity, Type, std::move(Body));
                }
            }
        }

        /**
         * The ocean material: DarkArisenOcean.materialtype with the wave constants of the sea state
         * OceanComponent starts with (Core::SeaState defaults), so drawn and simulated waves match.
         */
        ModelRef OceanMaterial(Session& Work)
        {
            const std::string Relative = "Assets/Materials/DarkArisenOcean.material";
            if (!Work.HasOutput(ProjectDir + "/" + Relative))
            {
                const Core::OceanSurface Surface{Core::SeaState{}};
                const std::vector<float> Constants = Surface.PackShaderConstants();
                JsonValue Values = JsonObject();
                for (std::size_t Wave = 0; Wave < Constants.size() / 4; ++Wave)
                {
                    JsonValue Vector = JsonArray();
                    for (std::size_t Component = 0; Component < 4; ++Component)
                    {
                        Vector.Items.push_back(JsonNumber(static_cast<double>(Constants[Wave * 4 + Component])));
                    }
                    Values.Members["waves.w" + std::to_string(Wave)] = std::move(Vector);
                }
                JsonValue Material = JsonObject();
                Material.Members["materialType"] = JsonString("Types/DarkArisenOcean.materialtype");
                Material.Members["materialTypeVersion"] = JsonNumber(1);
                Material.Members["propertyValues"] = std::move(Values);
                Work.Emit(ProjectDir + "/" + Relative, JsonWriter::Write(Material, 4));
            }
            return {SourceAssetUuid(Relative), 0, Lower("Assets/Materials/DarkArisenOcean.azmaterial")};
        }

        void AddSea(Session& Work, PrefabBuilder& Prefab)
        {
            const std::string Sea = Prefab.AddEntity("Sea", {});
            AddGame(Prefab, Sea, OceanComponentTypeId, "OceanComponent");  // component defaults = Core::SeaState{}

            // Visible water: 100 m tiles at 1 m around the play area, 400 m tiles at 8 m to the horizon.
            const ModelRef Material = OceanMaterial(Work);
            JsonValue Override = JsonObject();
            Override.Members["Key"] = JsonObject();
            JsonValue Assignment = JsonObject();
            Assignment.Members["MaterialAsset"] = AssetReference(Material.Guid, 0, Material.Hint);
            Override.Members["Value"] = std::move(Assignment);
            JsonValue Materials = JsonArray();
            Materials.Items.push_back(std::move(Override));
            const auto AddTile = [&Prefab, &Materials](const std::string& Name, const Vec3d& Where, const ModelRef& Tile)
            {
                const std::string Entity = Prefab.AddEntity(Name, Where, {}, "");
                Prefab.AddComponent(Entity, "AZ::Render::EditorMeshComponent", MeshComponent(Tile));
                JsonValue Configuration = JsonObject();
                Configuration.Members["materials"] = Materials;
                JsonValue Controller = JsonObject();
                Controller.Members["Configuration"] = std::move(Configuration);
                JsonValue Body = JsonObject();
                Body.Members["Controller"] = std::move(Controller);
                Prefab.AddComponent(Entity, "EditorMaterialComponent", std::move(Body));
            };
            const ModelRef Near = Work.GeneratedModel("SM_OceanTile_Near", MakeOceanGridGltf(100.0, 100, "SM_OceanTile_Near"));
            const ModelRef Far = Work.GeneratedModel("SM_OceanTile_Far", MakeOceanGridGltf(400.0, 50, "SM_OceanTile_Far"));
            for (int Row = 0; Row < 4; ++Row)
            {
                for (int Column = 0; Column < 4; ++Column)
                {
                    AddTile("Ocean Near " + std::to_string(Row) + std::to_string(Column),
                        {-150.0 + 100.0 * Column, -150.0 + 100.0 * Row, 0.0}, Near);
                }
            }
            for (int Row = -1; Row <= 1; ++Row)
            {
                for (int Column = -1; Column <= 1; ++Column)
                {
                    if (Row == 0 && Column == 0) continue;
                    AddTile("Ocean Far " + std::to_string(Row + 1) + std::to_string(Column + 1), {400.0 * Column, 400.0 * Row, 0.0}, Far);
                }
            }
        }

        void AddStaticMesh(PrefabBuilder& Prefab, const std::string& Name, const Vec3d& Where, const ModelRef& Model)
        {
            const std::string Entity = Prefab.AddEntity(Name, Where);
            Prefab.AddComponent(Entity, "AZ::Render::EditorMeshComponent", MeshComponent(Model));
        }

        std::string AddJake(Session& Work, PrefabBuilder& Prefab, const Vec3d& Where)
        {
            const std::string Jake = Prefab.AddEntity("Jake", Where);
            AddCharacterPhysics(Prefab, Jake);
            Prefab.AddComponent(Jake, "AZ::Render::EditorMeshComponent",
                MeshComponent(Work.RenderModel("ContentSource/Characters/Jake/SK_Jake_Alpha.gltf", true, "SK_Jake_Alpha_BindPose")));
            JsonValue Combatant = JsonObject();
            Combatant.Members["CombatantId"] = JsonString("character.jake");
            Combatant.Members["RallyEnabled"] = JsonBool(true);  // AJakeCharacter: HealthComponent->bRallyEnabled
            AddGame(Prefab, Jake, CombatantComponentTypeId, "CombatantComponent", std::move(Combatant));
            AddGame(Prefab, Jake, JakeInputComponentTypeId, "JakeInputComponent");
            AddGame(Prefab, Jake, SwimmerComponentTypeId, "SwimmerComponent");
            AddGame(Prefab, Jake, LockOnComponentTypeId, "LockOnComponent");
            // Follow camera: its own entity driven by the camera rig (Unreal boom 360 cm, shoulder offset).
            const std::string Camera = Prefab.AddEntity("Jake Camera", {Where.X + 0.45, Where.Y - 3.6, Where.Z + 1.66});
            JsonValue Configuration = JsonObject();
            Configuration.Members["Field of View"] = JsonNumber(60.0);
            JsonValue Controller = JsonObject();
            Controller.Members["Configuration"] = std::move(Configuration);
            JsonValue CameraBody = JsonObject();
            CameraBody.Members["Controller"] = std::move(Controller);
            Prefab.AddComponent(Camera, "{CA11DA46-29FF-4083-B5F6-E02C3A8C3A3D} EditorCameraComponent", std::move(CameraBody));
            JsonValue Rig = JsonObject();
            Rig.Members["Target"] = JsonString(Jake);
            AddGame(Prefab, Camera, CameraRigComponentTypeId, "CameraRigComponent", std::move(Rig));
            return Jake;
        }

        JsonValue SpawnPoint(const std::string& SpawnId, const std::string& Anchor, const Core::SpawnRule Rule)
        {
            JsonValue Point = JsonObject();
            Point.Members["SpawnId"] = JsonString(SpawnId);
            Point.Members["Anchor"] = JsonString(Anchor);
            Point.Members["Rule"] = JsonNumber(static_cast<int>(Rule));
            return Point;
        }

        std::string AnchorEntity(Session& Work, PrefabBuilder& Prefab, const std::string& Layout, const std::string& Key)
        {
            return Prefab.AddEntity("Anchor " + Key, Work.World(Layout, Key));
        }

        // ----------------------------------------------------------------------------- presentation

        bool ReadPresentation(Session& Work, JsonValue& OutSequences)
        {
            const std::string Relative = "ContentSource/Opening/HarlowOpeningPresentation.json";
            const JsonValue* Root = Work.Json(Relative);
            const JsonValue* Sequences = Root ? Root->Find("sequences") : nullptr;
            if (!Sequences || !Sequences->IsArray())
            {
                Work.Error(Relative + ": sequences missing");
                return false;
            }
            std::vector<Core::PresentationSequence> Core_;
            OutSequences = JsonArray();
            for (const JsonValue& Sequence : Sequences->Items)
            {
                Core::PresentationSequence Parsed;
                const JsonValue* Id = Sequence.Find("id");
                const JsonValue* Duration = Sequence.Find("duration");
                const JsonValue* Beats = Sequence.Find("beats");
                if (!Id || !Id->IsString() || !Duration || !Duration->IsNumber() || !Beats || !Beats->IsArray())
                {
                    Work.Error(Relative + ": sequence needs id, duration and beats");
                    return false;
                }
                Parsed.Id = Id->Text;
                Parsed.Duration = Duration->Number;
                if (const JsonValue* Control = Sequence.Find("playerControl")) Parsed.PlayerControl = Control->Text;
                if (const JsonValue* Camera = Sequence.Find("camera")) Parsed.Camera = Camera->Text;
                JsonValue BeatList = JsonArray();
                for (const JsonValue& Beat : Beats->Items)
                {
                    Core::PresentationBeat Item;
                    const JsonValue* Time = Beat.Find("t");
                    if (!Time || !Time->IsNumber())
                    {
                        Work.Error(Relative + ": beat without time in " + Parsed.Id);
                        return false;
                    }
                    Item.Time = Time->Number;
                    if (const JsonValue* Shot = Beat.Find("shot")) { Item.Kind = Core::BeatKind::Shot; Item.Text = Shot->Text; }
                    else if (const JsonValue* Subtitle = Beat.Find("subtitle"))
                    {
                        Item.Kind = Core::BeatKind::Subtitle;
                        const JsonValue* Speaker = Subtitle->Find("speaker");
                        const JsonValue* Line = Subtitle->Find("text");
                        Item.Speaker = Speaker ? Speaker->Text : "";
                        Item.Text = Line ? Line->Text : "";
                    }
                    else if (const JsonValue* Event = Beat.Find("event")) { Item.Kind = Core::BeatKind::Event; Item.Text = Event->Text; }
                    else if (const JsonValue* Callback = Beat.Find("callback")) { Item.Kind = Core::BeatKind::Callback; Item.Text = Callback->Text; }
                    else if (Beat.Find("actors")) { Item.Kind = Core::BeatKind::Actors; }
                    else
                    {
                        Work.Error(Relative + ": unknown beat in " + Parsed.Id);
                        return false;
                    }
                    JsonValue Written = JsonObject();
                    Written.Members["Time"] = JsonNumber(Item.Time);
                    Written.Members["Kind"] = JsonNumber(static_cast<int>(Item.Kind));
                    Written.Members["Speaker"] = JsonString(Item.Speaker);
                    Written.Members["Text"] = JsonString(Item.Text);
                    BeatList.Items.push_back(std::move(Written));
                    Parsed.Beats.push_back(std::move(Item));
                }
                JsonValue Written = JsonObject();
                Written.Members["Id"] = JsonString(Parsed.Id);
                Written.Members["Duration"] = JsonNumber(Parsed.Duration);
                Written.Members["PlayerControl"] = JsonString(Parsed.PlayerControl);
                Written.Members["Camera"] = JsonString(Parsed.Camera);
                Written.Members["Beats"] = std::move(BeatList);
                OutSequences.Items.push_back(std::move(Written));
                Core_.push_back(std::move(Parsed));
            }
            std::vector<std::string> Problems;
            if (!Core::ValidatePresentation(Core_, Problems))
            {
                for (const std::string& Problem : Problems) Work.Error(Relative + ": " + Problem);
                return false;
            }
            return true;
        }

        // ----------------------------------------------------------------------------- levels

        std::string BuildHarlow(Session& Work)
        {
            const std::string Layout = "ContentSource/Ships/Harlow/HarlowShip_GameplayLayout.json";
            PrefabBuilder Prefab("L_HarlowOpening");
            AddEnvironment(Work, Prefab);
            AddSea(Work, Prefab);

            AddStaticMesh(Prefab, "Harlow Merchant Ship", {}, Work.RenderModel("ContentSource/Ships/Harlow/SM_HarlowMerchantShip_Alpha.gltf"));
            const std::string Deck = Prefab.AddEntity("Harlow Walkable Collision", {});
            AddMeshCollider(Prefab, Deck, Work.CollisionMesh("ContentSource/Ships/Harlow/SM_HarlowShip_WalkableCollision_Alpha.gltf"));
            Vec3d Draven;
            if (Work.Anchor(Layout, "Draven.Arrival", Draven))
            {
                AddStaticMesh(Prefab, "Draven Raid Silhouette", LayoutToWorld({Draven.X + DravenSilhouetteOffsetCentimetres, Draven.Y, Draven.Z}),
                    Work.RenderModel("ContentSource/Ships/Draven/SM_DravenRaidSilhouette_Alpha.gltf"));
            }

            const std::string Spawn = AnchorEntity(Work, Prefab, Layout, "Spawn.Jake.HarlowOpening");
            const std::string Overboard = AnchorEntity(Work, Prefab, Layout, "Overboard");
            AnchorEntity(Work, Prefab, Layout, "Camera.BlackSails");
            AnchorEntity(Work, Prefab, Layout, "Helm");
            AnchorEntity(Work, Prefab, Layout, "Family.Gathering");
            const std::string Jake = AddJake(Work, Prefab, Work.World(Layout, "Spawn.Jake.HarlowOpening"));

            // Family: the three interactions gate the fleet (FamilyInteractionManifest "gate").
            const std::string ManifestPath = "ContentSource/Opening/FamilyInteractionManifest.json";
            const JsonValue* Family = Work.Json(ManifestPath);
            const JsonValue* Actors = Family ? Family->Find("actors") : nullptr;
            JsonValue FamilyRefs = JsonArray();
            const ModelRef Figure = Work.GeneratedModel("SM_Placeholder_Figure",
                MakePlaceholderFigureGltf(FamilyFigureHeight, FamilyFigureRadius, "SM_Placeholder_Figure"));
            std::set<std::string> Required = {"character.marc", "character.denise", "character.ethan"};
            if (!Actors || !Actors->IsArray())
            {
                Work.Error(ManifestPath + ": actors missing");
            }
            else
            {
                for (const JsonValue& Actor : Actors->Items)
                {
                    const JsonValue* Id = Actor.Find("id");
                    const JsonValue* Anchor = Actor.Find("anchor");
                    if (!Id || !Anchor || !Required.erase(Id->Text))
                    {
                        Work.Error(ManifestPath + ": unexpected or duplicate family actor");
                        continue;
                    }
                    const std::string Name = "Family " + Id->Text.substr(std::string("character.").size());
                    const std::string Entity = Prefab.AddEntity(Name, Work.World(Layout, Anchor->Text));
                    AddCapsuleCollider(Prefab, Entity, FamilyFigureHeight, FamilyFigureRadius);
                    Prefab.AddComponent(Entity, "AZ::Render::EditorMeshComponent", MeshComponent(Figure));
                    AddGame(Prefab, Entity, StoryTriggerComponentTypeId, "StoryTriggerComponent",
                        StoryTrigger(StorySignal::FamilyInteraction, Id->Text, 0, true));
                    JsonValue Ref = JsonObject();
                    Ref.Members["CharacterId"] = JsonString(Id->Text);
                    Ref.Members["Entity"] = JsonString(Entity);
                    FamilyRefs.Items.push_back(std::move(Ref));
                }
            }
            for (const std::string& Missing : Required) Work.Error(ManifestPath + ": missing " + Missing);

            // "Check the manifest with your brother when you're done." - the fleet is sighted there.
            const std::string Crate = Prefab.AddEntity("Cargo Manifest", Work.World(Layout, "Interaction.Cargo.Manifest"));
            AddBoxCollider(Prefab, Crate, ManifestCrateSize, false, {0.0, 0.0, ManifestCrateSize.Z / 2.0});
            Prefab.AddComponent(Crate, "AZ::Render::EditorMeshComponent",
                MeshComponent(Work.GeneratedModel("SM_Placeholder_Crate",
                    MakePlaceholderBoxGltf(ManifestCrateSize.X, ManifestCrateSize.Y, ManifestCrateSize.Z, "SM_Placeholder_Crate"))));
            AddGame(Prefab, Crate, StoryTriggerComponentTypeId, "StoryTriggerComponent", StoryTrigger(StorySignal::FleetDetected, {}, 0, true));

            // Boarders: placed inactive, released by the director when boarding begins.
            JsonValue Boarders = JsonArray();
            for (const char* Anchor : BoarderAnchors)
            {
                std::string Suffix = Lower(std::string(Anchor).substr(std::string("Boarding.").size()));
                std::replace(Suffix.begin(), Suffix.end(), '.', '_');
                const std::string Entity = Prefab.AddEntity(std::string("Boarder ") + Anchor, Work.World(Layout, Anchor), {}, {}, false);
                AddCharacterPhysics(Prefab, Entity);
                Prefab.AddComponent(Entity, "AZ::Render::EditorMeshComponent",
                    MeshComponent(Work.RenderModel("ContentSource/Characters/Boarders/SK_Boarder_Alpha.gltf", true, "SK_Boarder_Alpha_BindPose")));
                JsonValue Combatant = JsonObject();
                Combatant.Members["CombatantId"] = JsonString("enemy.boarder." + Suffix);
                Combatant.Members["DefeatEncounterId"] = JsonString(HarlowEncounter);
                AddGame(Prefab, Entity, CombatantComponentTypeId, "CombatantComponent", std::move(Combatant));
                JsonValue Brain = JsonObject();
                Brain.Members["Profile"] = JsonNumber(static_cast<int>(EnemyProfileKind::Boarder));
                AddGame(Prefab, Entity, EnemyBrainComponentTypeId, "EnemyBrainComponent", std::move(Brain));
                Boarders.Items.push_back(JsonString(Entity));
            }

            JsonValue Sequences;
            ReadPresentation(Work, Sequences);
            const std::string Director = Prefab.AddEntity("Opening Director", {});
            JsonValue DirectorFields = JsonObject();
            DirectorFields.Members["Sequences"] = std::move(Sequences);
            DirectorFields.Members["EncounterId"] = JsonString(HarlowEncounter);
            DirectorFields.Members["RequiredBoarders"] = JsonNumber(HarlowRequiredBoarders);
            DirectorFields.Members["Boarders"] = std::move(Boarders);
            DirectorFields.Members["Family"] = std::move(FamilyRefs);
            DirectorFields.Members["Player"] = JsonString(Jake);
            DirectorFields.Members["OverboardAnchor"] = JsonString(Overboard);
            AddGame(Prefab, Director, OpeningDirectorComponentTypeId, "OpeningDirectorComponent", std::move(DirectorFields));

            Work.Cover("Main.C01.01.HomeWater", "L_HarlowOpening", "Spawn.Jake.HarlowOpening (Arrival)",
                "StoryTrigger FamilyInteraction on Family marc, Family denise, Family ethan", "Spawn.Jake.HarlowOpening",
                "Main.C01.02.BlackSails in L_HarlowOpening");
            Work.Cover("Main.C01.02.BlackSails", "L_HarlowOpening", "StoryTrigger FleetDetected on Cargo Manifest",
                "Opening Director: three boarders defeated, Draven boards, TakingStarted", "Spawn.Jake.HarlowOpening",
                "Main.C01.03.TheTaking in L_HarlowOpening");
            Work.Cover("Main.C01.03.TheTaking", "L_HarlowOpening", "Opening Director TakingStarted",
                "Opening Director TakingPresentationComplete", "Spawn.Jake.HarlowOpening", "MapTransition -> L_DriftwoodBeach");
            JsonValue Transition = JsonObject();
            Transition.Members["MissionId"] = JsonString("Main.C01.03.TheTaking");
            Transition.Members["NextLevel"] = JsonString(std::string(Core::MissionCatalog::LevelFor("Main.C01.04.Undertow")));
            AddGame(Prefab, Director, MapTransitionComponentTypeId, "MapTransitionComponent", std::move(Transition));

            JsonValue Spawns = JsonArray();
            Spawns.Items.push_back(SpawnPoint("Spawn.Jake.HarlowOpening", Spawn, Core::SpawnRule::Arrival));
            JsonValue SpawnFields = JsonObject();
            SpawnFields.Members["Player"] = JsonString(Jake);
            SpawnFields.Members["Spawns"] = std::move(Spawns);
            AddGame(Prefab, Director, PlayerSpawnComponentTypeId, "PlayerSpawnComponent", std::move(SpawnFields));
            return Prefab.Write();
        }

        std::string BuildDriftwood(Session& Work)
        {
            const std::string Layout = "ContentSource/World/Moran/DriftwoodBeach/DriftwoodBeach_GameplayLayout.json";
            const std::string Reef = "ContentSource/World/Moran/OuterReef/OuterReef_GameplayLayout.json";
            PrefabBuilder Prefab("L_DriftwoodBeach");
            AddEnvironment(Work, Prefab);
            AddSea(Work, Prefab);

            const std::string Terrain = "ContentSource/World/Moran/DriftwoodBeach/SM_DriftwoodTerrain_Alpha.gltf";
            AddStaticMesh(Prefab, "Driftwood Terrain", {}, Work.RenderModel(Terrain));
            AddMeshCollider(Prefab, Prefab.AddEntity("Driftwood Terrain Collision", {}), Work.CollisionMesh(Terrain));
            AddStaticMesh(Prefab, "Harlow Wreckage", Work.World(Layout, "Harlow.Wreckage"),
                Work.RenderModel("ContentSource/World/Moran/DriftwoodBeach/SM_HarlowWreckage_Alpha.gltf"));
            AddStaticMesh(Prefab, "Outer Reef", {}, Work.RenderModel("ContentSource/World/Moran/OuterReef/SM_OuterReef_Alpha.gltf"));

            // Water volumes: the authored Outer Reef layout (Unreal hard-coded different values).
            const JsonValue* ReefLayout = Work.Json(Reef);
            const JsonValue* Volumes = ReefLayout ? ReefLayout->Find("volumes") : nullptr;
            if (!Volumes || !Volumes->IsArray() || Volumes->Items.empty())
            {
                Work.Error(Reef + ": volumes missing");
            }
            else
            {
                for (const JsonValue& Volume : Volumes->Items)
                {
                    const JsonValue* Id = Volume.Find("id");
                    const auto Vector = [&Work, &Reef, &Volume](const char* Key, Vec3d& Out)
                    {
                        const JsonValue* Value = Volume.Find(Key);
                        if (!Value || !Value->IsArray() || Value->Items.size() != 3 || !Value->Items[0].IsNumber() ||
                            !Value->Items[1].IsNumber() || !Value->Items[2].IsNumber())
                        {
                            Work.Error(Reef + ": volume " + Key + " must be [x,y,z]");
                            return false;
                        }
                        Out = {Value->Items[0].Number, Value->Items[1].Number, Value->Items[2].Number};
                        return true;
                    };
                    Vec3d Center, Extent, Current;
                    if (!Id || !Id->IsString() || !Vector("center", Center) || !Vector("extent", Extent) || !Vector("current", Current))
                    {
                        continue;
                    }
                    const Vec3d HalfSize = LayoutExtentToWorld(Extent);
                    const std::string Entity = Prefab.AddEntity("Water " + Id->Text, LayoutToWorld(Center));
                    AddBoxCollider(Prefab, Entity, {HalfSize.X * 2.0, HalfSize.Y * 2.0, HalfSize.Z * 2.0}, true);
                    JsonValue Water = JsonObject();
                    Water.Members["CurrentAcceleration"] = JsonVector(LayoutToWorld(Current));  // cm/s^2 -> m/s^2, mirrored
                    Water.Members["UseOceanSurface"] = JsonBool(true);
                    const JsonValue* Shallow = Volume.Find("shallowExit");
                    Water.Members["ShallowExit"] = JsonBool(Shallow && Shallow->Type == JsonValue::Kind::Bool && Shallow->Boolean);
                    Water.Members["HalfExtents"] = JsonVector(HalfSize);
                    AddGame(Prefab, Entity, WaterVolumeComponentTypeId, "WaterVolumeComponent", std::move(Water));
                    if (const JsonValue* Signal = Volume.Find("signal"))
                    {
                        if (Signal->Text == "EnteredWater")
                            AddGame(Prefab, Entity, StoryTriggerComponentTypeId, "StoryTriggerComponent", StoryTrigger(StorySignal::EnteredWater));
                        else if (Signal->Text == "ReachedOuterReef")
                            AddGame(Prefab, Entity, StoryTriggerComponentTypeId, "StoryTriggerComponent", StoryTrigger(StorySignal::ReachedOuterReef));
                        else
                            Work.Error(Reef + ": unknown volume signal " + Signal->Text);
                    }
                }
            }

            // Beach beats at the authored anchors (Unreal placed none, so Undertow could not finish).
            const std::string Recovery = AnchorEntity(Work, Prefab, Layout, "Spawn.Moran.DriftwoodBeach.Recovery");
            const std::string Beach = Prefab.AddEntity("Beat Driftwood Beach Reached", Work.World(Layout, "Spawn.Moran.DriftwoodBeach.Recovery"));
            AddBoxCollider(Prefab, Beach, BeatVolumeSize, true);
            AddGame(Prefab, Beach, StoryTriggerComponentTypeId, "StoryTriggerComponent",
                StoryTrigger(StorySignal::ReachedLocation, {}, static_cast<int>(Core::OpeningLocation::DriftwoodBeach)));
            const std::string Smoke = Prefab.AddEntity("Beat Camp Smoke Sighted", Work.World(Layout, "CampSmokeSightline"));
            AddBoxCollider(Prefab, Smoke, BeatVolumeSize, true);
            AddGame(Prefab, Smoke, StoryTriggerComponentTypeId, "StoryTriggerComponent", StoryTrigger(StorySignal::RecoveryComplete));
            const std::string Route = Prefab.AddEntity("Beat Route Driftwood Camp", Work.World(Layout, "Route.DriftwoodCamp"));
            AddBoxCollider(Prefab, Route, BeatVolumeSize, true);
            AddGame(Prefab, Route, StoryTriggerComponentTypeId, "StoryTriggerComponent",
                StoryTrigger(StorySignal::ReachedLocation, {}, static_cast<int>(Core::OpeningLocation::DriftwoodCamp)));
            for (const char* Key : {"OuterReef.WreckCue", "BrokenMast", "Journal.FamilyDebris"})
            {
                AnchorEntity(Work, Prefab, Layout, Key);
            }

            const std::string WaterEntry = AnchorEntity(Work, Prefab, Layout, "WaterEntry");
            const std::string Jake = AddJake(Work, Prefab, Work.World(Layout, "WaterEntry"));
            const std::string Director = Prefab.AddEntity("Moran Director", {});
            JsonValue Spawns = JsonArray();
            Spawns.Items.push_back(SpawnPoint("Spawn.Moran.DriftwoodBeach.Recovery", Recovery, Core::SpawnRule::Checkpoint));
            Spawns.Items.push_back(SpawnPoint("WaterEntry", WaterEntry, Core::SpawnRule::OpeningOverboard));
            Spawns.Items.push_back(SpawnPoint("Spawn.Moran.DriftwoodBeach.Recovery", Recovery, Core::SpawnRule::Arrival));
            JsonValue SpawnFields = JsonObject();
            SpawnFields.Members["Player"] = JsonString(Jake);
            SpawnFields.Members["Spawns"] = std::move(Spawns);
            AddGame(Prefab, Director, PlayerSpawnComponentTypeId, "PlayerSpawnComponent", std::move(SpawnFields));
            Work.Cover("Main.C01.04.Undertow", "L_DriftwoodBeach", "WaterEntry (OpeningOverboard)",
                "StoryTrigger RecoveryComplete (beach smoke)", "Spawn.Moran.DriftwoodBeach.Recovery",
                "Main.C02.01.ShatteredCoast; MapTransition -> L_DriftwoodCamp");
            JsonValue Transition = JsonObject();
            Transition.Members["Fact"] = JsonString("World.DriftwoodCampReached");
            Transition.Members["NextLevel"] = JsonString("L_DriftwoodCamp");
            AddGame(Prefab, Director, MapTransitionComponentTypeId, "MapTransitionComponent", std::move(Transition));
            return Prefab.Write();
        }

        std::string BuildCamp(Session& Work)
        {
            const std::string Layout = "ContentSource/World/Moran/DriftwoodCamp/DriftwoodCamp_Layout.json";
            PrefabBuilder Prefab("L_DriftwoodCamp");
            AddEnvironment(Work, Prefab);
            const std::string Mesh = "ContentSource/World/Moran/DriftwoodCamp/SM_DriftwoodCamp_Alpha.gltf";
            AddStaticMesh(Prefab, "Driftwood Camp", {}, Work.RenderModel(Mesh));
            AddMeshCollider(Prefab, Prefab.AddEntity("Driftwood Camp Collision", {}), Work.CollisionMesh(Mesh));
            const std::string Arrival = AnchorEntity(Work, Prefab, Layout, "Arrival");
            for (const char* Key : {"Shelter", "Fire", "SmokeColumn", "HarlowClue", "Route.MirasCove"})
            {
                AnchorEntity(Work, Prefab, Layout, Key);
            }
            const std::string Jake = AddJake(Work, Prefab, Work.World(Layout, "Arrival"));
            const std::string Director = Prefab.AddEntity("Camp Director", {});
            JsonValue Spawns = JsonArray();
            Spawns.Items.push_back(SpawnPoint("Arrival", Arrival, Core::SpawnRule::Arrival));
            JsonValue SpawnFields = JsonObject();
            SpawnFields.Members["Player"] = JsonString(Jake);
            SpawnFields.Members["Spawns"] = std::move(Spawns);
            AddGame(Prefab, Director, PlayerSpawnComponentTypeId, "PlayerSpawnComponent", std::move(SpawnFields));
            return Prefab.Write();
        }
        // ----------------------------------------------------------------------------- story (chapters 3-10)

        // PROVISIONAL greybox scaffolding sizes (ContentSource gives these a point, not a size).
        constexpr double ProvisionalPadSize = 4.0;         // walkable pad under an unsupported anchor
        constexpr double ProvisionalPathWidth = 3.0;       // walkable strip between consecutive anchors
        constexpr double ProvisionalPathDepth = 1.0;
        constexpr double ProvisionalStepRise = 0.25;       // stair rise when consecutive anchors differ in height
        constexpr double ProvisionalTileLength = 4.0;      // path tile length (one shared mesh)
        constexpr double MaxWalkLegMetres = 150.0;         // longer consecutive-anchor legs are sea crossings
        constexpr double SeaLevelTolerance = 0.5;          // anchors this close to z = 0 in seafaring missions are on the water
        constexpr double LocationTriggerHalfExtentCm[3] = {260.0, 260.0, 180.0};  // UE AMainStoryLocationTriggerActor
        constexpr double TriggerSinkMetres = 0.5;           // trigger boxes reach below the floor the anchor marks
        constexpr Vec3d EvidenceSize{0.8, 0.6, 0.5};
        constexpr Vec3d RouteMarkerSize{0.45, 0.45, 0.9};   // UE cylinder scale (0.45, 0.45, 0.9)
        constexpr Vec3d WarMarkerSize{0.6, 0.6, 0.9};       // UE cube scale (0.6, 0.6, 0.9)
        constexpr Vec3d HostileHullSize{12.0, 3.2, 1.8};    // UE AHostileNavalShip hull presentation
        constexpr double WeatherDeckHeight = 2.6;           // LaLiberacion_Layout decks.Weather (260 cm)

        struct Footprint
        {
            Vec3d Min;
            Vec3d Max;
            /** Walkable top surface height (metres, world). */
            double Top = 0.0;
        };

        std::string Slug(std::string Text)
        {
            for (char& Character : Text)
            {
                if (!std::isalnum(static_cast<unsigned char>(Character))) Character = '_';
            }
            return Text;
        }

        Vec3d World(const Core::LayoutPoint& Point)
        {
            return LayoutToWorld({Point[0], Point[1], Point[2]});
        }

        bool Supported(const std::vector<Footprint>& Ground, const Vec3d& Point)
        {
            for (const Footprint& Area : Ground)
            {
                if (Point.X >= Area.Min.X && Point.X <= Area.Max.X && Point.Y >= Area.Min.Y && Point.Y <= Area.Max.Y &&
                    Point.Z >= Area.Top - 0.5 && Point.Z <= Area.Top + 1.5)
                {
                    return true;
                }
            }
            return false;
        }

        /** World-space bounds of a greybox glTF (source frame, metres) placed at a world position. */
        bool GltfFootprint(Session& Work, const std::string& SourceRelative, const Vec3d& At, Footprint& Out)
        {
            const JsonValue* Root = Work.Json(SourceRelative);
            const JsonValue* Accessors = Root ? Root->Find("accessors") : nullptr;
            const JsonValue* Meshes = Root ? Root->Find("meshes") : nullptr;
            if (!Accessors || !Meshes)
            {
                return false;
            }
            Vec3d Low{1e30, 1e30, 1e30};
            Vec3d High{-1e30, -1e30, -1e30};
            for (const JsonValue& Mesh : Meshes->Items)
            {
                const JsonValue* Primitives = Mesh.Find("primitives");
                if (!Primitives) continue;
                for (const JsonValue& Primitive : Primitives->Items)
                {
                    const JsonValue* Attributes = Primitive.Find("attributes");
                    const JsonValue* Position = Attributes ? Attributes->Find("POSITION") : nullptr;
                    if (!Position || !Position->IsNumber()) continue;
                    const auto Index = static_cast<std::size_t>(Position->Number);
                    if (Index >= Accessors->Items.size()) return false;
                    const JsonValue* Min = Accessors->Items[Index].Find("min");
                    const JsonValue* Max = Accessors->Items[Index].Find("max");
                    if (!Min || !Max || Min->Items.size() != 3 || Max->Items.size() != 3) return false;
                    // Source frame (Unreal axes, metres) to world: mirror Y.
                    Low = {std::min(Low.X, Min->Items[0].Number), std::min(Low.Y, -Max->Items[1].Number), std::min(Low.Z, Min->Items[2].Number)};
                    High = {std::max(High.X, Max->Items[0].Number), std::max(High.Y, -Min->Items[1].Number), std::max(High.Z, Max->Items[2].Number)};
                }
            }
            if (Low.X > High.X)
            {
                return false;
            }
            Out.Min = {At.X + Low.X, At.Y + Low.Y, At.Z + Low.Z};
            Out.Max = {At.X + High.X, At.Y + High.Y, At.Z + High.Z};
            Out.Top = At.Z;  // greybox set pieces stand on their anchor
            return true;
        }

        /** A solid PROVISIONAL or authored block whose top face is at TopZ (box glTF + box collider). */
        void AddBlock(Session& Work, PrefabBuilder& Prefab, const std::string& Name, const Vec3d& TopCenter, const Vec3d& Size,
            const double YawDegrees, const bool Solid)
        {
            const std::string Mesh = "SM_Block_" + std::to_string(std::lround(Size.X * 100.0)) + "x" +
                std::to_string(std::lround(Size.Y * 100.0)) + "x" + std::to_string(std::lround(Size.Z * 100.0));
            const std::string Entity = Prefab.AddEntity(Name, {TopCenter.X, TopCenter.Y, TopCenter.Z - Size.Z}, {0.0, 0.0, YawDegrees});
            // Placeholder boxes are authored in the source frame, where Y is mirrored: sizes are symmetric.
            Prefab.AddComponent(Entity, "AZ::Render::EditorMeshComponent",
                MeshComponent(Work.GeneratedModel(Mesh, MakePlaceholderBoxGltf(Size.X, Size.Y, Size.Z, Mesh))));
            if (Solid)
            {
                AddBoxCollider(Prefab, Entity, Size, false, {0.0, 0.0, Size.Z / 2.0});
            }
        }

        void AddStoryActorComponent(PrefabBuilder& Prefab, const std::string& Entity, const Core::StoryActorSpec& Actor)
        {
            JsonValue Fields = JsonObject();
            const auto Text = [&Fields](const char* Key, const std::string& Value)
            {
                if (!Value.empty()) Fields.Members[Key] = JsonString(Value);
            };
            Fields.Members["Kind"] = JsonNumber(static_cast<int>(Actor.Kind));
            Text("MissionId", Actor.MissionId);
            Text("CheckpointId", Actor.CheckpointId);
            Text("SpawnId", Actor.SpawnId);
            switch (Actor.Kind)
            {
            case Core::StoryActorKind::LocationTrigger:
                Fields.Members["Action"] = JsonNumber(static_cast<int>(Actor.Action));
                break;
            case Core::StoryActorKind::Contact:
                Text("ContactId", Actor.ContactId);
                Text("DialogueId", Actor.DialogueId);
                Text("DisplayName", Actor.DisplayName);
                Text("Text", Actor.Text);
                if (Actor.CompletesMission) Fields.Members["CompletesMission"] = JsonBool(true);
                break;
            case Core::StoryActorKind::Evidence:
                Text("Title", Actor.Title);
                Text("Body", Actor.Body);
                Text("WorldFactOnRead", Actor.WorldFactOnRead);
                Text("RequiredBossId", Actor.RequiredBossId);
                if (Actor.CompletesMission) Fields.Members["CompletesMission"] = JsonBool(true);
                break;
            case Core::StoryActorKind::RouteResolution:
                Text("OutcomeKey", Actor.OutcomeKey);
                Text("OutcomeValue", Actor.OutcomeValue);
                Text("Label", Actor.Label);
                break;
            case Core::StoryActorKind::WarAction:
                if (Actor.IsHoldingClaim())
                {
                    Text("HoldingId", Actor.HoldingId);
                    Fields.Members["CompletesMission"] = JsonBool(Actor.CompletesMission);
                    break;
                }
                Text("RegionId", Actor.RegionId);
                Fields.Members["InitialController"] = JsonNumber(static_cast<int>(Actor.InitialController));
                Fields.Members["Verb"] = JsonNumber(static_cast<int>(Actor.Verb));
                Fields.Members["TargetFaction"] = JsonNumber(static_cast<int>(Actor.TargetFaction));
                Fields.Members["ControlDelta"] = JsonNumber(Actor.ControlDelta);
                Fields.Members["LiberationDelta"] = JsonNumber(Actor.LiberationDelta);
                Fields.Members["CrimsonDelta"] = JsonNumber(Actor.CrimsonDelta);
                Text("OutcomeKey", Actor.OutcomeKey);
                Text("OutcomeValue", Actor.OutcomeValue);
                Text("Label", Actor.Label);
                break;
            case Core::StoryActorKind::NavalEncounterGate:
                Fields.Members["RequiredDefeats"] = JsonNumber(Actor.RequiredDefeats);
                Text("RequiredWorldFact", Actor.RequiredWorldFact);
                if (Actor.CompletionRadiusMetres > 0.0) Fields.Members["CompletionRadius"] = JsonNumber(Actor.CompletionRadiusMetres);
                break;
            default:
                break;
            }
            AddGame(Prefab, Entity, StoryActorComponentTypeId, "StoryActorComponent", std::move(Fields));
        }

        std::string Describe(const Core::StoryActorSpec& Actor)
        {
            std::string Text = std::string(Core::ToString(Actor.Kind)) + " " + Actor.Name;
            if (Actor.Kind == Core::StoryActorKind::HolderBoss) Text += " (" + Actor.BossId + " defeated)";
            if (Actor.Kind == Core::StoryActorKind::NavalEncounterGate)
                Text += " (" + std::to_string(Actor.RequiredDefeats) + " hostile ship(s) sunk)";
            return Text;
        }

        std::string BuildStoryMission(Session& Work, const Core::StoryMissionContract& Contract)
        {
            PrefabBuilder Prefab(Contract.MapId);
            AddEnvironment(Work, Prefab);
            const bool Naval = Contract.IsNaval();
            // A land contract whose route legs cross open water is sailed, not walked (no water fast travel).
            bool SeaCrossing = false;
            for (std::size_t Index = 1; Index < Contract.Anchors.size(); ++Index)
            {
                const Vec3d From = World(Contract.Anchors[Index - 1].second);
                const Vec3d To = World(Contract.Anchors[Index].second);
                SeaCrossing = SeaCrossing || std::hypot(To.X - From.X, To.Y - From.Y) > MaxWalkLegMetres;
            }
            const bool Seafaring = Naval || SeaCrossing;
            const bool HasPlayerShip = std::any_of(Contract.Actors.begin(), Contract.Actors.end(),
                [](const Core::StoryActorSpec& Actor) { return Actor.Kind == Core::StoryActorKind::PlayerShip; });
            if (Seafaring)
            {
                AddSea(Work, Prefab);
            }
            std::string ProvisionalNote;

            // Authored geometry. Boxes are read with their top face on the anchor (the floor the
            // anchor marks); in naval missions they are sea zones and stay non-solid.
            std::vector<Footprint> Ground;
            for (const Core::StoryGeometrySpec& Geometry : Contract.Geometry)
            {
                const Core::LayoutPoint* Anchor = Contract.FindAnchor(Geometry.Anchor);
                if (!Anchor) continue;
                const Vec3d At = World(*Anchor);
                if (Geometry.IsBox())
                {
                    const Vec3d Size = LayoutExtentToWorld({Geometry.Size[0], Geometry.Size[1], Geometry.Size[2]});
                    if (Seafaring)
                    {
                        const std::string Zone = Prefab.AddEntity("Sea Zone " + Geometry.Name, At);
                        AddBoxCollider(Prefab, Zone, Size, true);
                        continue;
                    }
                    AddBlock(Work, Prefab, "Geometry " + Geometry.Name, At, Size, 0.0, true);
                    Ground.push_back({{At.X - Size.X / 2.0, At.Y - Size.Y / 2.0, At.Z - Size.Z}, {At.X + Size.X / 2.0, At.Y + Size.Y / 2.0, At.Z}, At.Z});
                    continue;
                }
                const std::string AssetName = Geometry.ObjectPath.substr(Geometry.ObjectPath.find_last_of('.') + 1);
                const std::string Source = Work.FindContentGltf(AssetName);
                if (Source.empty()) continue;
                AddStaticMesh(Prefab, "Geometry " + Geometry.Name, At, Work.RenderModel(Source));
                AddMeshCollider(Prefab, Prefab.AddEntity("Geometry " + Geometry.Name + " Collision", At), Work.CollisionMesh(Source));
                Footprint Area;
                if (GltfFootprint(Work, Source, At, Area)) Ground.push_back(Area);
            }

            // PROVISIONAL ground: ContentSource leaves many anchors floating. Pads under every
            // unsupported land anchor and stepped strips between consecutive anchors (authored route
            // order) keep the space physically walkable until production geometry replaces them.
            std::set<std::string> ShipAnchors;
            for (const Core::StoryActorSpec& Actor : Contract.Actors)
            {
                if (Actor.Kind == Core::StoryActorKind::PlayerShip || Actor.Kind == Core::StoryActorKind::NavalEnemy ||
                    Actor.Kind == Core::StoryActorKind::NavalEncounterGate)
                {
                    ShipAnchors.insert(Actor.Anchor);
                }
            }
            int Provisional = 0;
            std::vector<std::pair<std::string, Vec3d>> Land;
            std::set<std::string> PeopleAndObjects;
            for (const Core::StoryActorSpec& Actor : Contract.Actors)
            {
                if (Actor.Kind == Core::StoryActorKind::Contact || Actor.Kind == Core::StoryActorKind::Evidence ||
                    Actor.Kind == Core::StoryActorKind::EthanHarlow)
                {
                    PeopleAndObjects.insert(Actor.Anchor);
                }
            }
            for (const auto& [Name, Point] : Contract.Anchors)
            {
                const bool SeaLevel = Seafaring && (ShipAnchors.count(Name) || std::abs(Point[2]) / 100.0 < SeaLevelTolerance);
                if (!SeaLevel)
                {
                    Land.emplace_back(Name, World(Point));
                }
                else if (PeopleAndObjects.count(Name) && !ShipAnchors.count(Name))
                {
                    // Someone or something authored on open water (a skiff, a bearing hail): a raft to reach.
                    AddBlock(Work, Prefab, "PROVISIONAL Raft " + Name, World(Point), {ProvisionalPadSize, ProvisionalPadSize, ProvisionalPathDepth}, 0.0, true);
                    ++Provisional;
                }
            }
            for (const auto& [Name, At] : Land)
            {
                if (!Supported(Ground, At))
                {
                    AddBlock(Work, Prefab, "PROVISIONAL Pad " + Name, At, {ProvisionalPadSize, ProvisionalPadSize, ProvisionalPathDepth}, 0.0, true);
                    ++Provisional;
                }
            }
            {
                // Fixed 4 m tiles (one shared mesh); more, overlapping tiles where the rise needs steps.
                for (std::size_t Index = 1; Index < Land.size(); ++Index)
                {
                    const Vec3d& From = Land[Index - 1].second;
                    const Vec3d& To = Land[Index].second;
                    const double Dx = To.X - From.X;
                    const double Dy = To.Y - From.Y;
                    const double Length = std::hypot(Dx, Dy);
                    if (Length < ProvisionalPadSize || Length > MaxWalkLegMetres) continue;
                    const int Tiles = std::max(static_cast<int>(std::ceil(Length / ProvisionalTileLength)),
                        static_cast<int>(std::ceil(std::abs(To.Z - From.Z) / ProvisionalStepRise)));
                    const double Yaw = std::atan2(Dy, Dx) * 180.0 / 3.14159265358979323846;
                    const double Usable = std::max(0.0, Length - ProvisionalTileLength);
                    for (int Tile = 0; Tile < Tiles; ++Tile)
                    {
                        const double Along = ProvisionalTileLength / 2.0 + (Tiles > 1 ? Usable * Tile / (Tiles - 1) : Usable / 2.0);
                        const double T = Along / Length;
                        const Vec3d Center{From.X + Dx * T, From.Y + Dy * T, From.Z + (To.Z - From.Z) * (Tile + 0.5) / Tiles};
                        AddBlock(Work, Prefab, "PROVISIONAL Path " + Land[Index - 1].first + " to " + Land[Index].first + " " + std::to_string(Tile),
                            Center, {ProvisionalTileLength + 0.05, ProvisionalPathWidth, ProvisionalPathDepth}, Yaw, true);
                        ++Provisional;
                    }
                }
            }
            (void)Provisional;

            // Anchors as entities: spawn points and the travel exit use them.
            std::map<std::string, std::string> AnchorEntities;
            for (const auto& [Name, Point] : Contract.Anchors)
            {
                Vec3d At = World(Point);
                if (Seafaring && Name == Contract.EntryAnchor) At.Z += WeatherDeckHeight;  // Jake starts on the weather deck
                AnchorEntities[Name] = Prefab.AddEntity("Anchor " + Name, At);
            }

            const Core::LayoutPoint* Entry = Contract.FindAnchor(Contract.EntryAnchor);
            Vec3d JakeAt = Entry ? World(*Entry) : Vec3d{};
            if (Seafaring) JakeAt.Z += WeatherDeckHeight;
            const std::string Jake = AddJake(Work, Prefab, JakeAt);
            if (SeaCrossing && !HasPlayerShip)
            {
                // PROVISIONAL: the contract routes Jake across open water but authors no ship.
                const std::string Mesh = "ContentSource/Ships/LaLiberacion/SM_LaLiberacion_Alpha.gltf";
                const std::string Ship = Prefab.AddEntity("PROVISIONAL La Liberacion", Entry ? World(*Entry) : Vec3d{});
                Prefab.AddComponent(Ship, "AZ::Render::EditorMeshComponent", MeshComponent(Work.RenderModel(Mesh)));
                AddMeshCollider(Prefab, Ship, Work.CollisionMesh(Mesh));
                AddGame(Prefab, Ship, ShipVoyageComponentTypeId, "ShipVoyageComponent");
                JsonValue Guns = JsonObject();
                Guns.Members["PlayerAligned"] = JsonBool(true);
                AddGame(Prefab, Ship, NavalCombatComponentTypeId, "NavalCombatComponent", std::move(Guns));
                ProvisionalNote = "PROVISIONAL ship: contract crosses open water without a PlayerShip";
            }

            const ModelRef Figure = Work.GeneratedModel("SM_Placeholder_Figure",
                MakePlaceholderFigureGltf(FamilyFigureHeight, FamilyFigureRadius, "SM_Placeholder_Figure"));
            std::vector<std::string> Completions;
            JsonValue Spawns = JsonArray();
            Spawns.Items.push_back(SpawnPoint(Contract.SpawnId, AnchorEntities[Contract.EntryAnchor], Core::SpawnRule::Arrival));
            Spawns.Items.push_back(SpawnPoint(Contract.SpawnId, AnchorEntities[Contract.EntryAnchor], Core::SpawnRule::Checkpoint));
            std::set<std::string> SpawnIds{Contract.SpawnId};

            for (const Core::StoryActorSpec& Actor : Contract.Actors)
            {
                const Core::LayoutPoint* Point = Contract.FindAnchor(Actor.Anchor);
                if (!Point) continue;
                const Vec3d At = World(*Point);
                if (SpawnIds.insert(Actor.SpawnId).second)
                {
                    Spawns.Items.push_back(SpawnPoint(Actor.SpawnId, AnchorEntities[Actor.Anchor], Core::SpawnRule::Checkpoint));
                }
                const std::string Name = Actor.Name;
                switch (Actor.Kind)
                {
                case Core::StoryActorKind::LocationTrigger:
                {
                    const std::string Entity = Prefab.AddEntity(Name, At);
                    AddBoxCollider(Prefab, Entity,
                        LayoutExtentToWorld({LocationTriggerHalfExtentCm[0] * 2.0, LocationTriggerHalfExtentCm[1] * 2.0, LocationTriggerHalfExtentCm[2] * 2.0}),
                        true, {0.0, 0.0, LocationTriggerHalfExtentCm[2] / 100.0 - TriggerSinkMetres});
                    AddStoryActorComponent(Prefab, Entity, Actor);
                    break;
                }
                case Core::StoryActorKind::Contact:
                {
                    const std::string Entity = Prefab.AddEntity(Name, At);
                    Prefab.AddComponent(Entity, "AZ::Render::EditorMeshComponent", MeshComponent(Figure));
                    AddCapsuleCollider(Prefab, Entity, FamilyFigureHeight, FamilyFigureRadius);
                    AddStoryActorComponent(Prefab, Entity, Actor);
                    break;
                }
                case Core::StoryActorKind::Evidence:
                case Core::StoryActorKind::RouteResolution:
                case Core::StoryActorKind::WarAction:
                {
                    const Vec3d Size = Actor.Kind == Core::StoryActorKind::Evidence ? EvidenceSize
                        : Actor.Kind == Core::StoryActorKind::RouteResolution    ? RouteMarkerSize
                                                                                  : WarMarkerSize;
                    const std::string Mesh = Actor.Kind == Core::StoryActorKind::Evidence ? "SM_Placeholder_Crate"
                        : Actor.Kind == Core::StoryActorKind::RouteResolution            ? "SM_Placeholder_RouteMarker"
                                                                                          : "SM_Placeholder_WarMarker";
                    const std::string Entity = Prefab.AddEntity(Name, At);
                    Prefab.AddComponent(Entity, "AZ::Render::EditorMeshComponent",
                        MeshComponent(Work.GeneratedModel(Mesh, MakePlaceholderBoxGltf(Size.X, Size.Y, Size.Z, Mesh))));
                    AddBoxCollider(Prefab, Entity, Size, false, {0.0, 0.0, Size.Z / 2.0});
                    AddStoryActorComponent(Prefab, Entity, Actor);
                    break;
                }
                case Core::StoryActorKind::DuelingEnemy:
                case Core::StoryActorKind::HolderBoss:
                {
                    const std::string Entity = Prefab.AddEntity(Name, At);
                    AddCharacterPhysics(Prefab, Entity);
                    const bool Boss = Actor.Kind == Core::StoryActorKind::HolderBoss;
                    Prefab.AddComponent(Entity, "AZ::Render::EditorMeshComponent",
                        MeshComponent(Boss ? Figure
                                           : Work.RenderModel("ContentSource/Characters/Boarders/SK_Boarder_Alpha.gltf", true, "SK_Boarder_Alpha_BindPose")));
                    JsonValue Combatant = JsonObject();
                    Combatant.Members["CombatantId"] = JsonString(Boss ? Actor.BossId : "enemy.duelist." + Lower(Slug(Name)));
                    AddGame(Prefab, Entity, CombatantComponentTypeId, "CombatantComponent", std::move(Combatant));
                    JsonValue Brain = JsonObject();
                    EnemyProfileKind Profile = EnemyProfileKind::Boarder;
                    if (Boss)
                    {
                        Profile = Actor.BossId == "boss.dream_ethan"   ? EnemyProfileKind::DreamEthan
                            : Actor.BossId == "boss.draven_voss" ? EnemyProfileKind::DravenVoss
                                                                 : EnemyProfileKind::Holder;
                        if (Profile == EnemyProfileKind::Holder)
                        {
                            Brain.Members["HolderBossId"] = JsonString(Actor.BossId);
                            Brain.Members["HolderMissionId"] = JsonString(Actor.MissionId);
                        }
                        Brain.Members["OverrideResolution"] = JsonBool(true);
                        Brain.Members["OutcomeKey"] = JsonString(Actor.OutcomeKey);
                        Brain.Members["OutcomeValue"] = JsonString(Actor.OutcomeValue);
                        Brain.Members["CompleteMissionOnDefeat"] = JsonBool(Actor.CompletesMissionOnDefeat);
                    }
                    Brain.Members["Profile"] = JsonNumber(static_cast<int>(Profile));
                    AddGame(Prefab, Entity, EnemyBrainComponentTypeId, "EnemyBrainComponent", std::move(Brain));
                    break;
                }
                case Core::StoryActorKind::EthanHarlow:
                {
                    // Real Ethan: present and interactable through his Contact, never a combatant.
                    const std::string Entity = Prefab.AddEntity(Name, At);
                    Prefab.AddComponent(Entity, "AZ::Render::EditorMeshComponent", MeshComponent(Figure));
                    AddCapsuleCollider(Prefab, Entity, FamilyFigureHeight, FamilyFigureRadius);
                    break;
                }
                case Core::StoryActorKind::PlayerShip:
                {
                    const std::string Mesh = "ContentSource/Ships/LaLiberacion/SM_LaLiberacion_Alpha.gltf";
                    const std::string Entity = Prefab.AddEntity(Name, At);
                    Prefab.AddComponent(Entity, "AZ::Render::EditorMeshComponent", MeshComponent(Work.RenderModel(Mesh)));
                    AddMeshCollider(Prefab, Entity, Work.CollisionMesh(Mesh));
                    AddGame(Prefab, Entity, ShipVoyageComponentTypeId, "ShipVoyageComponent");
                    JsonValue Guns = JsonObject();
                    Guns.Members["PlayerAligned"] = JsonBool(true);
                    AddGame(Prefab, Entity, NavalCombatComponentTypeId, "NavalCombatComponent", std::move(Guns));
                    break;
                }
                case Core::StoryActorKind::NavalEnemy:
                {
                    const std::string Entity = Prefab.AddEntity(Name, At);
                    Prefab.AddComponent(Entity, "AZ::Render::EditorMeshComponent",
                        MeshComponent(Work.GeneratedModel("SM_Placeholder_HostileHull",
                            MakePlaceholderBoxGltf(HostileHullSize.X, HostileHullSize.Y, HostileHullSize.Z, "SM_Placeholder_HostileHull"))));
                    AddBoxCollider(Prefab, Entity, HostileHullSize, false, {0.0, 0.0, HostileHullSize.Z / 2.0});
                    AddGame(Prefab, Entity, NavalCombatComponentTypeId, "NavalCombatComponent");
                    break;
                }
                case Core::StoryActorKind::NavalEncounterGate:
                {
                    AddStoryActorComponent(Prefab, Prefab.AddEntity(Name, At), Actor);
                    break;
                }
                }
                const bool CompletesHere = (Actor.Kind == Core::StoryActorKind::LocationTrigger && Actor.Action == Core::LocationAction::CompleteMission) ||
                    ((Actor.Kind == Core::StoryActorKind::Evidence || Actor.Kind == Core::StoryActorKind::Contact) && Actor.CompletesMission) ||
                    Actor.Kind == Core::StoryActorKind::RouteResolution ||
                    (Actor.Kind == Core::StoryActorKind::WarAction && (!Actor.IsHoldingClaim() || Actor.CompletesMission)) ||
                    (Actor.Kind == Core::StoryActorKind::HolderBoss && Actor.CompletesMissionOnDefeat) ||
                    Actor.Kind == Core::StoryActorKind::NavalEncounterGate;
                if (CompletesHere) Completions.push_back(Describe(Actor));
            }

            const bool Final = Contract.NextMission.empty();
            const std::string NextLevel = Final ? "L_Credits" : std::string(Core::MissionCatalog::LevelFor(Contract.NextMission));
            const std::string Director = Prefab.AddEntity("Story Director", {});
            JsonValue SpawnFields = JsonObject();
            SpawnFields.Members["Player"] = JsonString(Jake);
            SpawnFields.Members["Spawns"] = std::move(Spawns);
            AddGame(Prefab, Director, PlayerSpawnComponentTypeId, "PlayerSpawnComponent", std::move(SpawnFields));
            JsonValue Transition = JsonObject();
            Transition.Members["MissionId"] = JsonString(Contract.MissionId);
            Transition.Members["NextLevel"] = JsonString(NextLevel);
            AddGame(Prefab, Director, MapTransitionComponentTypeId, "MapTransitionComponent", std::move(Transition));

            std::string Completion;
            for (const std::string& Item : Completions) Completion += (Completion.empty() ? "" : " | ") + Item;
            if (!ProvisionalNote.empty()) Completion += " [" + ProvisionalNote + "]";
            Work.Cover(Contract.MissionId, Contract.MapId, Contract.EntryAnchor + " (" + Contract.SpawnId + ")", Completion,
                Contract.CheckpointId, Final ? "Credits (L_Credits)" : Contract.NextMission + " in " + NextLevel);
            return Prefab.Write();
        }

        /** L_Credits: factual roll from CreditsAuthority.json (O3DE technology list), return to the front end. */
        std::string BuildCredits(Session& Work)
        {
            const std::string ContractPath = "ContentSource/Presentation/Credits/L_Credits.contract.json";
            const std::string AuthorityPath = "ContentSource/Story/Credits/CreditsAuthority.json";
            const JsonValue* Contract = Work.Json(ContractPath);
            const JsonValue* Authority = Work.Json(AuthorityPath);
            if (!Contract || !Authority) return {};
            const JsonValue* Title = Authority->Find("title");
            const JsonValue* ByEngine = Authority->Find("verifiedTechnologyByEngine");
            const JsonValue* Technology = ByEngine ? ByEngine->Find("O3DE") : nullptr;
            const JsonValue* Provenance = Authority->Find("generatedSourceProvenance");
            const JsonValue* Input = Contract->Find("input");
            const JsonValue* Skip = Input ? Input->Find("allowSkipAfterSeconds") : nullptr;
            const JsonValue* Map = Contract->Find("mapId");
            if (!Title || !Title->IsString() || !Technology || !Technology->IsArray() || Technology->Items.empty() || !Provenance ||
                !Provenance->IsArray() || !Skip || !Skip->IsNumber() || !Map || Map->Text != "L_Credits")
            {
                Work.Error(AuthorityPath + " / " + ContractPath + ": title, verifiedTechnologyByEngine.O3DE, generatedSourceProvenance, "
                    "input.allowSkipAfterSeconds and mapId L_Credits are required");
                return {};
            }
            std::string Roll = Title->Text + "\n\nVERIFIED TECHNOLOGY\n";
            for (const JsonValue& Item : Technology->Items) Roll += Item.Text + "\n";
            Roll += "\nSOURCE PROVENANCE\n";
            for (const JsonValue& Item : Provenance->Items) Roll += Item.Text + "\n";
            Roll += "\nThank you for playing.\n";

            PrefabBuilder Prefab("L_Credits");
            AddEnvironment(Work, Prefab);
            AddSea(Work, Prefab);  // "quiet open sea after dawn"
            const std::string Credits = Prefab.AddEntity("Credits Roll", {});
            JsonValue Fields = JsonObject();
            Fields.Members["RollText"] = JsonString(Roll);
            Fields.Members["AllowSkipAfterSeconds"] = JsonNumber(Skip->Number);
            AddGame(Prefab, Credits, CreditsComponentTypeId, "CreditsComponent", std::move(Fields));
            return Prefab.Write();
        }

        /** Every ContentSource/Story contract, validated against the campaign (fails closed). */
        bool LoadStoryContracts(Session& Work, const fs::path& Repo, std::vector<Core::StoryMissionContract>& Out)
        {
            std::vector<std::string> Paths;
            std::error_code Failure;
            for (auto It = fs::recursive_directory_iterator(Repo / "ContentSource/Story", Failure);
                 !Failure && It != fs::recursive_directory_iterator(); It.increment(Failure))
            {
                const std::string File = It->path().filename().string();
                if (It->is_regular_file() && File.size() > 5 && File[0] == 'C' && std::isdigit(static_cast<unsigned char>(File[1])) &&
                    It->path().extension() == ".json")
                {
                    Paths.push_back(fs::relative(It->path(), Repo).generic_string());
                }
            }
            std::sort(Paths.begin(), Paths.end());
            if (Paths.empty())
            {
                Work.Error("no ContentSource/Story mission contracts");
                return false;
            }
            std::vector<std::string> Errors;
            for (const std::string& Path : Paths)
            {
                const JsonValue* Root = Work.Json(Path);
                if (!Root) continue;
                Core::StoryMissionContract Contract;
                if (!ParseStoryContract(*Root, Path, Contract, Errors)) continue;
                const std::string Naval = "ContentSource/Naval/" + fs::path(Path).stem().string() + ".naval.json";
                if (fs::exists(Repo / Naval))
                {
                    if (const JsonValue* NavalRoot = Work.Json(Naval)) MergeNavalContract(*NavalRoot, Naval, Contract, Errors);
                }
                Core::ValidateStoryContract(Contract, Errors);
                Out.push_back(std::move(Contract));
            }
            for (std::string& Error : Errors) Work.Error(std::move(Error));
            return Work.Ok();
        }
    }

    bool MaterializeCampaign(const fs::path& RepoRoot, MaterializeResult& Out)
    {
        Out = {};
        Session Work(RepoRoot, Out);
        try
        {
            const std::pair<const char*, std::string (*)(Session&)> Levels[] = {
                {"L_HarlowOpening", &BuildHarlow}, {"L_DriftwoodBeach", &BuildDriftwood}, {"L_DriftwoodCamp", &BuildCamp}};
            for (const auto& [Name, Build] : Levels)
            {
                std::string Prefab = Build(Work);
                Work.Emit(ProjectDir + "/Levels/" + Name + "/" + Name + ".prefab", std::move(Prefab));
            }

            std::vector<Core::StoryMissionContract> Story;
            if (LoadStoryContracts(Work, RepoRoot, Story))
            {
                for (const Core::StoryMissionContract& Contract : Story)
                {
                    std::string Prefab = BuildStoryMission(Work, Contract);
                    Work.Emit(ProjectDir + "/Levels/" + Contract.MapId + "/" + Contract.MapId + ".prefab", std::move(Prefab));
                }
            }
            std::string Credits = BuildCredits(Work);
            Work.Emit(ProjectDir + "/Levels/L_Credits/L_Credits.prefab", std::move(Credits));

            // Physical coverage: every catalogue mission with a materialised level contract.
            JsonValue Missions = JsonObject();
            int Covered = 0;
            for (const Core::MissionDefinition& Mission : Core::MissionCatalog::Missions())
            {
                const auto Row = Work.CoverageRows().find(std::string(Mission.Id));
                if (Row == Work.CoverageRows().end())
                {
                    JsonValue Missing = JsonObject();
                    Missing.Members["level"] = JsonString(Core::MissionCatalog::LevelFor(Mission.Id));
                    Missing.Members["status"] = JsonString("NOT MATERIALISED");
                    Missions.Members[std::string(Mission.Id)] = std::move(Missing);
                    continue;
                }
                JsonValue Entry = Row->second;
                if (Entry.Find("level")->Text != Core::MissionCatalog::LevelFor(Mission.Id))
                {
                    Work.Error(std::string(Mission.Id) + " is materialised in " + Entry.Find("level")->Text + ", not its campaign level");
                }
                if (Entry.Find("completion")->Text.empty())
                {
                    Work.Error(std::string(Mission.Id) + " has no materialised completion");
                }
                Entry.Members["status"] = JsonString("MATERIALISED");
                Missions.Members[std::string(Mission.Id)] = std::move(Entry);
                ++Covered;
            }
            JsonValue Coverage = JsonObject();
            Coverage.Members["covered"] = JsonNumber(Covered);
            Coverage.Members["total"] = JsonNumber(Core::MissionCatalog::MissionCount);
            Coverage.Members["missions"] = std::move(Missions);
            Work.Emit(ProjectDir + "/Levels/PhysicalCoverage.json", JsonWriter::Write(Coverage, 2));
        }
        catch (const std::exception& Failure)
        {
            Work.Error(Failure.what());
        }
        if (!Work.Ok())
        {
            Out.Files.clear();
            return false;
        }
        Out.Files.push_back({ProjectDir + "/Levels/Materialized.json", Work.Manifest()});
        return true;
    }

    bool ApplyMaterialization(const fs::path& RepoRoot, const MaterializeResult& Result, const bool Check, std::vector<std::string>& OutStale)
    {
        OutStale.clear();
        bool Ok = true;
        for (const MaterializedFile& File : Result.Files)
        {
            const fs::path Target = RepoRoot / File.RelativePath;
            bool Exists = false;
            std::string Current;
            {
                // Closed before any write: Windows does not replace files that are still open.
                std::ifstream Existing(Target, std::ios::binary);
                Exists = static_cast<bool>(Existing);
                if (Exists) Current.assign((std::istreambuf_iterator<char>(Existing)), std::istreambuf_iterator<char>());
            }
            const bool Same = Exists && Current == File.Content;
            if (Same)
            {
                continue;
            }
            OutStale.push_back(File.RelativePath);
            if (Check)
            {
                Ok = false;
                continue;
            }
            std::error_code Failure;
            fs::create_directories(Target.parent_path(), Failure);
            std::ofstream Stream(Target, std::ios::binary | std::ios::trunc);
            Stream << File.Content;
            if (!Stream)
            {
                Ok = false;
            }
        }
        return Ok;
    }
}
