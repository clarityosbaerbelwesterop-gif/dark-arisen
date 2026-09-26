#include "StoryContracts.h"

#include <DarkArisen/Core/ColonialWar.h>

#include <cmath>
#include <set>

namespace DarkArisen::Tools
{
    namespace
    {
        class Reader
        {
        public:
            Reader(std::string InWhere, std::vector<std::string>& InErrors) : Where(std::move(InWhere)), Errors(InErrors) {}

            void Fail(const std::string& Message) { Errors.push_back(Where + ": " + Message); }

            std::string String(const JsonValue& Object, const char* Key, const bool Required = true)
            {
                const JsonValue* Value = Object.Find(Key);
                if (!Value)
                {
                    if (Required) Fail(std::string("missing \"") + Key + "\"");
                    return {};
                }
                if (!Value->IsString())
                {
                    Fail(std::string("\"") + Key + "\" is not a string");
                    return {};
                }
                if (Required && Value->Text.empty()) Fail(std::string("\"") + Key + "\" is empty");
                return Value->Text;
            }

            bool Bool(const JsonValue& Object, const char* Key, const bool Default)
            {
                const JsonValue* Value = Object.Find(Key);
                if (!Value) return Default;
                if (Value->Type != JsonValue::Kind::Bool)
                {
                    Fail(std::string("\"") + Key + "\" is not a boolean");
                    return Default;
                }
                return Value->Boolean;
            }

            int Integer(const JsonValue& Object, const char* Key, const bool Required, const int Default = 0)
            {
                const JsonValue* Value = Object.Find(Key);
                if (!Value)
                {
                    if (Required) Fail(std::string("missing \"") + Key + "\"");
                    return Default;
                }
                if (!Value->IsNumber() || !std::isfinite(Value->Number) || Value->Number != std::floor(Value->Number))
                {
                    Fail(std::string("\"") + Key + "\" is not an integer");
                    return Default;
                }
                return static_cast<int>(Value->Number);
            }

            bool Point(const JsonValue& Value, Core::LayoutPoint& Out, const std::string& What)
            {
                if (!Value.IsArray() || Value.Items.size() != 3)
                {
                    Fail(What + " is not [x, y, z]");
                    return false;
                }
                for (std::size_t Index = 0; Index < 3; ++Index)
                {
                    const JsonValue& Item = Value.Items[Index];
                    if (!Item.IsNumber() || !std::isfinite(Item.Number))
                    {
                        Fail(What + " has a non-finite component");
                        return false;
                    }
                    Out[Index] = Item.Number;
                }
                return true;
            }

            std::vector<std::string> Strings(const JsonValue& Object, const char* Key)
            {
                std::vector<std::string> Result;
                const JsonValue* Value = Object.Find(Key);
                if (!Value) return Result;
                if (!Value->IsArray())
                {
                    Fail(std::string("\"") + Key + "\" is not an array");
                    return Result;
                }
                for (const JsonValue& Item : Value->Items)
                {
                    if (!Item.IsString() || Item.Text.empty()) Fail(std::string("\"") + Key + "\" holds a non-string");
                    else Result.push_back(Item.Text);
                }
                return Result;
            }

            /** Every key the actor carries must be one its kind reads. */
            void OnlyKeys(const JsonValue& Object, const std::set<std::string>& Allowed, const std::string& What)
            {
                for (const auto& [Key, Value] : Object.Members)
                {
                    if (!Allowed.count(Key)) Fail(What + " has unknown field \"" + Key + "\"");
                }
            }

        private:
            std::string Where;
            std::vector<std::string>& Errors;
        };

        const std::set<std::string>& AllowedKeys(const Core::StoryActorKind Kind)
        {
            using K = Core::StoryActorKind;
            static const std::set<std::string> Common = {"type", "name", "anchor", "checkpointId", "spawnId"};
            static const auto With = [](std::initializer_list<const char*> Extra)
            {
                std::set<std::string> Keys = Common;
                for (const char* Key : Extra) Keys.insert(Key);
                return Keys;
            };
            static const std::set<std::string> Location = With({"action"});
            static const std::set<std::string> Contact = With({"contactId", "dialogueId", "displayName", "text", "completesMission"});
            static const std::set<std::string> Evidence = With({"title", "body", "completesMission", "worldFactOnRead", "requiredBossId"});
            static const std::set<std::string> Route = With({"outcomeKey", "outcomeValue", "label"});
            static const std::set<std::string> War = With({"regionId", "initialController", "verb", "targetFaction", "controlDelta",
                "liberationDelta", "crimsonDelta", "outcomeKey", "outcomeValue", "label", "actionId", "actionType", "faction", "locationId",
                "requiredMissionId", "completesMission"});
            static const std::set<std::string> Boss = With({"bossId", "outcomeKey", "outcomeValue", "completesMissionOnDefeat"});
            static const std::set<std::string> Ethan = With({"state"});
            static const std::set<std::string> Gate = With({"requiredDefeats", "requiredWorldFact"});
            switch (Kind)
            {
            case K::LocationTrigger: return Location;
            case K::Contact: return Contact;
            case K::Evidence: return Evidence;
            case K::RouteResolution: return Route;
            case K::WarAction: return War;
            case K::HolderBoss: return Boss;
            case K::EthanHarlow: return Ethan;
            case K::NavalEncounterGate: return Gate;
            default: return Common;
            }
        }

        void ParseWarAction(Reader& In, const JsonValue& Spec, Core::StoryActorSpec& Actor, const std::string& What,
            const std::string& MissionId)
        {
            if (Spec.Find("actionId"))
            {
                // Holding claim (ClaimHolding): the Chapter 6 variant of a war action.
                Actor.HoldingId = In.String(Spec, "locationId");
                const std::string Required = In.String(Spec, "requiredMissionId");
                if (Required != MissionId) In.Fail(What + " claims a holding for another mission");
                const std::string ActionType = In.String(Spec, "actionType");
                if (ActionType != "Support") In.Fail(What + " has holding action type " + ActionType + " (only Support claims)");
                Actor.CompletesMission = In.Bool(Spec, "completesMission", false);
                return;
            }
            Actor.RegionId = In.String(Spec, "regionId");
            const auto Controller = Core::ColonialWar::ParseFaction(In.String(Spec, "initialController"));
            const auto Verb = Core::ColonialWar::ParseVerb(In.String(Spec, "verb"));
            const auto Target = Core::ColonialWar::ParseFaction(In.String(Spec, "targetFaction"));
            if (!Controller || !Verb || !Target) In.Fail(What + " names an unknown faction or verb");
            Actor.InitialController = Controller.value_or(Core::ColonialFaction::Imperial);
            Actor.Verb = Verb.value_or(Core::WarActionVerb::Support);
            Actor.TargetFaction = Target.value_or(Core::ColonialFaction::Imperial);
            Actor.ControlDelta = In.Integer(Spec, "controlDelta", true);
            Actor.LiberationDelta = In.Integer(Spec, "liberationDelta", true);
            Actor.CrimsonDelta = In.Integer(Spec, "crimsonDelta", true);
            Actor.OutcomeKey = In.String(Spec, "outcomeKey");
            Actor.OutcomeValue = In.String(Spec, "outcomeValue");
            Actor.Label = In.String(Spec, "label");
        }
    }

    bool ParseStoryContract(const JsonValue& Root, const std::string& SourcePath, Core::StoryMissionContract& Out,
        std::vector<std::string>& OutErrors)
    {
        const std::size_t Before = OutErrors.size();
        Reader In(SourcePath, OutErrors);
        Out = {};
        Out.SourcePath = SourcePath;
        if (!Root.IsObject())
        {
            In.Fail("contract is not an object");
            return false;
        }
        if (In.Integer(Root, "schemaVersion", true) != 1) In.Fail("unsupported schemaVersion");
        Out.MissionId = In.String(Root, "missionId");
        Out.RegionId = In.String(Root, "regionId");
        Out.MapId = In.String(Root, "mapId");
        Out.EntryAnchor = In.String(Root, "entryAnchor");
        Out.CheckpointId = In.String(Root, "checkpointId");
        Out.SpawnId = In.String(Root, "spawnId");
        Out.NavigationLaw = In.String(Root, "navigationLaw");
        Out.NextMission = In.String(Root, "nextMission", false);
        Out.CompletionFacts = In.Strings(Root, "completionFacts");
        Out.EncounterIds = In.Strings(Root, "encounterIds");
        if (const JsonValue* Credits = Root.Find("creditsHandoff"))
        {
            Out.CreditsHandoff = Credits->IsObject() || (Credits->Type == JsonValue::Kind::Bool && Credits->Boolean) ||
                (Credits->IsString() && !Credits->Text.empty());
        }

        const JsonValue* Anchors = Root.Find("anchors");
        if (!Anchors || !Anchors->IsObject() || Anchors->Order.empty())
        {
            In.Fail("missing \"anchors\"");
        }
        else
        {
            for (const std::string& Name : Anchors->Order)
            {
                Core::LayoutPoint Point{};
                if (In.Point(*Anchors->Find(Name), Point, "anchor " + Name)) Out.Anchors.emplace_back(Name, Point);
            }
        }

        for (const char* Key : {"geometry", "physicalGeometry"})
        {
            const JsonValue* List = Root.Find(Key);
            if (!List) continue;
            if (!List->IsArray())
            {
                In.Fail(std::string("\"") + Key + "\" is not an array");
                continue;
            }
            for (const JsonValue& Spec : List->Items)
            {
                Core::StoryGeometrySpec Geometry;
                Geometry.Name = In.String(Spec, "name");
                Geometry.Anchor = In.String(Spec, "anchor");
                if (std::string(Key) == "geometry")
                {
                    Geometry.ObjectPath = In.String(Spec, "objectPath");
                }
                else if (const JsonValue* Size = Spec.Find("size"))
                {
                    In.Point(*Size, Geometry.Size, "geometry " + Geometry.Name + " size");
                }
                else
                {
                    In.Fail("geometry " + Geometry.Name + " has no size");
                }
                Out.Geometry.push_back(std::move(Geometry));
            }
        }

        const JsonValue* Actors = Root.Find("actors");
        if (!Actors || !Actors->IsArray())
        {
            In.Fail("missing \"actors\"");
        }
        else
        {
            for (const JsonValue& Spec : Actors->Items)
            {
                Core::StoryActorSpec Actor;
                const std::string Type = In.String(Spec, "type");
                Actor.Name = In.String(Spec, "name");
                Actor.Anchor = In.String(Spec, "anchor");
                const std::string What = "actor " + Actor.Name;
                const auto Kind = Core::ParseStoryActorKind(Type);
                if (!Kind)
                {
                    In.Fail(What + " has unknown type \"" + Type + "\"");
                    continue;
                }
                Actor.Kind = *Kind;
                In.OnlyKeys(Spec, AllowedKeys(Actor.Kind), What);
                Actor.MissionId = Out.MissionId;
                Actor.CheckpointId = In.String(Spec, "checkpointId", false);
                Actor.SpawnId = In.String(Spec, "spawnId", false);
                if (Actor.CheckpointId.empty()) Actor.CheckpointId = Out.CheckpointId;
                if (Actor.SpawnId.empty()) Actor.SpawnId = Out.SpawnId;
                switch (Actor.Kind)
                {
                case Core::StoryActorKind::LocationTrigger:
                {
                    const std::string Action = In.String(Spec, "action");
                    if (Action == "ActivateMission") Actor.Action = Core::LocationAction::ActivateMission;
                    else if (Action == "CompleteMission") Actor.Action = Core::LocationAction::CompleteMission;
                    else In.Fail(What + " has unknown action \"" + Action + "\"");
                    break;
                }
                case Core::StoryActorKind::Contact:
                    Actor.ContactId = In.String(Spec, "contactId");
                    Actor.DialogueId = In.String(Spec, "dialogueId");
                    Actor.DisplayName = In.String(Spec, "displayName");
                    Actor.Text = In.String(Spec, "text");
                    Actor.CompletesMission = In.Bool(Spec, "completesMission", false);
                    break;
                case Core::StoryActorKind::Evidence:
                    Actor.Title = In.String(Spec, "title");
                    Actor.Body = In.String(Spec, "body");
                    Actor.CompletesMission = In.Bool(Spec, "completesMission", false);
                    Actor.WorldFactOnRead = In.String(Spec, "worldFactOnRead", false);
                    Actor.RequiredBossId = In.String(Spec, "requiredBossId", false);
                    break;
                case Core::StoryActorKind::RouteResolution:
                    Actor.OutcomeKey = In.String(Spec, "outcomeKey");
                    Actor.OutcomeValue = In.String(Spec, "outcomeValue");
                    Actor.Label = In.String(Spec, "label");
                    break;
                case Core::StoryActorKind::WarAction:
                    ParseWarAction(In, Spec, Actor, What, Out.MissionId);
                    break;
                case Core::StoryActorKind::HolderBoss:
                    Actor.BossId = In.String(Spec, "bossId");
                    Actor.OutcomeKey = In.String(Spec, "outcomeKey");
                    Actor.OutcomeValue = In.String(Spec, "outcomeValue");
                    Actor.CompletesMissionOnDefeat = In.Bool(Spec, "completesMissionOnDefeat", true);
                    break;
                case Core::StoryActorKind::EthanHarlow:
                {
                    const std::string State = In.String(Spec, "state");
                    if (State == "Captive") Actor.Ethan = Core::EthanPresence::Captive;
                    else if (State == "Aboard") Actor.Ethan = Core::EthanPresence::Aboard;
                    else if (State == "Rescued") Actor.Ethan = Core::EthanPresence::Rescued;
                    else In.Fail(What + " has unknown Ethan state \"" + State + "\"");
                    break;
                }
                case Core::StoryActorKind::NavalEncounterGate:
                    Actor.RequiredDefeats = In.Integer(Spec, "requiredDefeats", true, 1);
                    Actor.RequiredWorldFact = In.String(Spec, "requiredWorldFact", false);
                    break;
                default:
                    break;
                }
                Out.Actors.push_back(std::move(Actor));
            }
        }
        return OutErrors.size() == Before;
    }

    bool MergeNavalContract(const JsonValue& Root, const std::string& SourcePath, Core::StoryMissionContract& InOut,
        std::vector<std::string>& OutErrors)
    {
        const std::size_t Before = OutErrors.size();
        Reader In(SourcePath, OutErrors);
        if (In.Integer(Root, "schemaVersion", true) != 1) In.Fail("unsupported schemaVersion");
        if (In.String(Root, "missionId") != InOut.MissionId || In.String(Root, "mapId") != InOut.MapId)
        {
            In.Fail("names a different mission or map than " + InOut.SourcePath);
        }
        const auto AddShip = [&](const JsonValue& Spec, const Core::StoryActorKind Kind) -> Core::StoryActorSpec*
        {
            Core::StoryActorSpec Actor;
            Actor.Kind = Kind;
            Actor.Name = In.String(Spec, "name");
            Actor.MissionId = InOut.MissionId;
            Actor.CheckpointId = InOut.CheckpointId;
            Actor.SpawnId = InOut.SpawnId;
            Actor.Anchor = "Naval." + Actor.Name;
            Core::LayoutPoint Point{};
            const JsonValue* Location = Spec.Find("location");
            if (!Location || !In.Point(*Location, Point, Actor.Name + " location")) return nullptr;
            if (InOut.FindAnchor(Actor.Anchor)) In.Fail("duplicate naval actor " + Actor.Name);
            InOut.Anchors.emplace_back(Actor.Anchor, Point);
            InOut.Actors.push_back(std::move(Actor));
            return &InOut.Actors.back();
        };
        const JsonValue* Player = Root.Find("playerShip");
        const JsonValue* Hostiles = Root.Find("hostiles");
        const JsonValue* Gate = Root.Find("completionGate");
        if (!Player || !Hostiles || !Hostiles->IsArray() || Hostiles->Items.empty() || !Gate)
        {
            In.Fail("needs playerShip, hostiles and completionGate");
            return false;
        }
        AddShip(*Player, Core::StoryActorKind::PlayerShip);
        for (const JsonValue& Hostile : Hostiles->Items) AddShip(Hostile, Core::StoryActorKind::NavalEnemy);
        if (Core::StoryActorSpec* Actor = AddShip(*Gate, Core::StoryActorKind::NavalEncounterGate))
        {
            Actor->RequiredDefeats = In.Integer(*Gate, "requiredSunkShips", true, 1);
            Actor->CheckpointId = In.String(*Gate, "checkpointId");
            Actor->SpawnId = In.String(*Gate, "spawnId");
            Actor->CompletionRadiusMetres = NavalMissionGateRadiusMetres;
            if (Actor->RequiredDefeats != static_cast<int>(Hostiles->Items.size()))
            {
                In.Fail("gate requires " + std::to_string(Actor->RequiredDefeats) + " sinkings but the screen has " +
                    std::to_string(Hostiles->Items.size()) + " ships");
            }
        }
        return OutErrors.size() == Before;
    }
}
