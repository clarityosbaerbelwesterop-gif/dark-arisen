#include "Materializer.h"

#include "Gltf.h"
#include "Hash.h"
#include "Json.h"
#include "O3deIds.h"
#include "Prefab.h"

#include <DarkArisen/AuthoringEnums.h>
#include <DarkArisen/DarkArisenTypeIds.h>

#include <DarkArisen/Core/CampaignState.h>
#include <DarkArisen/Core/LevelSpawn.h>
#include <DarkArisen/Core/MissionCatalog.h>
#include <DarkArisen/Core/Presentation.h>

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
        constexpr int MaterializerVersion = 1;

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

        void AddSea(PrefabBuilder& Prefab)
        {
            const std::string Sea = Prefab.AddEntity("Sea", {});
            AddGame(Prefab, Sea, OceanComponentTypeId, "OceanComponent");  // component defaults
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
            // Follow camera placeholder: a child at the Unreal boom length. Camera system is NOT DONE.
            const std::string Camera = Prefab.AddEntity("Jake Camera", {0.0, -3.6, 1.9}, {-12.0, 0.0, 0.0}, Jake);
            JsonValue Configuration = JsonObject();
            Configuration.Members["Field of View"] = JsonNumber(60.0);
            JsonValue Controller = JsonObject();
            Controller.Members["Configuration"] = std::move(Configuration);
            JsonValue CameraBody = JsonObject();
            CameraBody.Members["Controller"] = std::move(Controller);
            Prefab.AddComponent(Camera, "{CA11DA46-29FF-4083-B5F6-E02C3A8C3A3D} EditorCameraComponent", std::move(CameraBody));
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
            AddSea(Prefab);

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
            AddSea(Prefab);

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
    }

    bool MaterializeVerticalSlice(const fs::path& RepoRoot, MaterializeResult& Out)
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
