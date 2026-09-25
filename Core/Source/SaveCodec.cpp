#include "DarkArisen/Core/SaveCodec.h"

#include <charconv>
#include <cstdio>
#include <functional>
#include <map>
#include <set>
#include <system_error>

namespace DarkArisen::Core
{
    namespace
    {
        std::string Escape(const std::string_view Value)
        {
            std::string Result;
            Result.reserve(Value.size());
            for (const char Character : Value)
            {
                switch (Character)
                {
                case '\\': Result += "\\\\"; break;
                case '\t': Result += "\\t"; break;
                case '\n': Result += "\\n"; break;
                case '\r': Result += "\\r"; break;
                default: Result += Character; break;
                }
            }
            return Result;
        }

        bool Unescape(const std::string_view Value, std::string& Out)
        {
            Out.clear();
            for (std::size_t Index = 0; Index < Value.size(); ++Index)
            {
                if (Value[Index] != '\\')
                {
                    Out += Value[Index];
                    continue;
                }
                if (++Index >= Value.size())
                {
                    return false;
                }
                switch (Value[Index])
                {
                case '\\': Out += '\\'; break;
                case 't': Out += '\t'; break;
                case 'n': Out += '\n'; break;
                case 'r': Out += '\r'; break;
                default: return false;
                }
            }
            return true;
        }

        template <typename NumberType>
        std::string Number(const NumberType Value)
        {
            char Buffer[64];
            const auto Result = std::to_chars(Buffer, Buffer + sizeof(Buffer), Value);
            return std::string(Buffer, Result.ptr);
        }

        template <typename NumberType>
        bool ParseNumber(const std::string& Text, NumberType& Out)
        {
            if (Text.empty())
            {
                return false;
            }
            const auto Result = std::from_chars(Text.data(), Text.data() + Text.size(), Out);
            return Result.ec == std::errc() && Result.ptr == Text.data() + Text.size();
        }

        bool ParseBool(const std::string& Text, bool& Out)
        {
            if (Text == "0" || Text == "1")
            {
                Out = Text == "1";
                return true;
            }
            return false;
        }

        class Writer
        {
        public:
            void Record(const std::string_view Key, std::initializer_list<std::string> Fields)
            {
                Body += Key;
                for (const std::string& Field : Fields)
                {
                    Body += '\t';
                    Body += Escape(Field);
                }
                Body += '\n';
            }

            void Transform(const std::string_view Key, const Core::Transform& Value)
            {
                Record(Key, {Number(Value.Translation.X), Number(Value.Translation.Y), Number(Value.Translation.Z),
                    Number(Value.Rotation.X), Number(Value.Rotation.Y), Number(Value.Rotation.Z), Number(Value.Rotation.W),
                    Number(Value.Scale.X), Number(Value.Scale.Y), Number(Value.Scale.Z)});
            }

            std::string Body;
        };

        std::string Flag(const bool Value) { return Value ? "1" : "0"; }
        std::string Ordinal(const auto Value) { return Number(static_cast<int>(Value)); }

        struct Reader
        {
            CampaignState& State;
            std::vector<std::string>& Errors;
            std::set<std::string> SeenScalars;
            int Line = 0;

            void Fail(const std::string& Message)
            {
                Errors.push_back("line " + std::to_string(Line) + ": " + Message);
            }

            bool Scalar(const std::string& Key)
            {
                if (!SeenScalars.insert(Key).second)
                {
                    Fail("duplicate record " + Key);
                    return false;
                }
                return true;
            }

            bool Arity(const std::vector<std::string>& Fields, const std::size_t Expected)
            {
                if (Fields.size() != Expected)
                {
                    Fail("record " + Fields.front() + " expects " + std::to_string(Expected - 1) + " values");
                    return false;
                }
                return true;
            }

            bool ReadTransform(const std::vector<std::string>& Fields, Core::Transform& Out)
            {
                if (!Arity(Fields, 11))
                {
                    return false;
                }
                double* Targets[] = {&Out.Translation.X, &Out.Translation.Y, &Out.Translation.Z, &Out.Rotation.X,
                    &Out.Rotation.Y, &Out.Rotation.Z, &Out.Rotation.W, &Out.Scale.X, &Out.Scale.Y, &Out.Scale.Z};
                for (std::size_t Index = 0; Index < 10; ++Index)
                {
                    if (!ParseNumber(Fields[Index + 1], *Targets[Index]))
                    {
                        Fail("invalid transform component");
                        return false;
                    }
                }
                return true;
            }
        };

        template <typename EnumType>
        bool ParseOrdinal(const std::string& Text, EnumType& Out, const EnumType Maximum)
        {
            int Value = 0;
            if (!ParseNumber(Text, Value) || Value < 0 || Value > static_cast<int>(Maximum))
            {
                return false;
            }
            Out = static_cast<EnumType>(Value);
            return true;
        }
    }

    std::uint64_t SaveCodec::Checksum(const std::string_view Body)
    {
        std::uint64_t Hash = 14695981039346656037ull;
        for (const char Character : Body)
        {
            Hash ^= static_cast<unsigned char>(Character);
            Hash *= 1099511628211ull;
        }
        return Hash;
    }

    std::string SaveCodec::Encode(const CampaignState& State)
    {
        Writer Out;
        Out.Record("version", {Number(State.SaveVersion)});
        Out.Record("current_mission", {State.CurrentMission});
        Out.Record("current_chapter", {Number(State.CurrentChapter)});
        for (const MissionRuntime& Mission : State.Missions)
        {
            Out.Record("mission", {Mission.MissionId, Ordinal(Mission.State)});
        }
        for (const std::string& Fact : State.StoryFacts) Out.Record("story_fact", {Fact});
        for (const std::string& Fact : State.WorldFacts) Out.Record("world_fact", {Fact});

        Out.Record("progression.level", {Number(State.Progression.Level)});
        Out.Record("progression.experience", {Number(State.Progression.Experience)});
        for (const std::string& Item : State.Progression.Inventory) Out.Record("progression.inventory", {Item});
        for (const std::string& Item : State.Progression.Equipment) Out.Record("progression.equipment", {Item});

        const PlayerRuntimeSnapshot& Player = State.PlayerRuntime;
        Out.Record("player.valid", {Flag(Player.Valid)});
        Out.Record("player.mission", {Player.MissionId});
        Out.Record("player.source_level", {Player.SourceLevel});
        Out.Transform("player.transform", Player.WorldTransform);
        Out.Record("player.health", {Number(Player.HealthFraction)});
        Out.Record("player.stamina", {Number(Player.StaminaFraction)});

        for (const CrewRelationship& Crew : State.Crew)
        {
            Out.Record("crew", {Crew.CharacterId, Crew.Role, Number(Crew.Trust), Flag(Crew.Met), Flag(Crew.Available),
                Flag(Crew.Recruited), Flag(Crew.Aboard)});
        }
        Out.Record("current_ship", {State.CurrentShip});

        const ShipVoyageSnapshot& Ship = State.LaLiberacionVoyage;
        Out.Record("ship.valid", {Flag(Ship.Valid)});
        Out.Record("ship.source_level", {Ship.SourceLevel});
        Out.Transform("ship.transform", Ship.WorldTransform);
        Out.Record("ship.owned", {Flag(Ship.OwnedAndUnlocked)});
        Out.Record("ship.throttle", {Number(Ship.Throttle)});
        Out.Record("ship.hull", {Number(Ship.HullIntegrity)});
        Out.Record("ship.heading", {Number(Ship.HeadingDegrees)});
        Out.Record("ship.commanded_heading", {Number(Ship.CommandedHeadingDegrees)});
        Out.Record("ship.speed", {Number(Ship.ForwardSpeedMetresPerSecond)});
        Out.Record("ship.wind_direction", {Number(Ship.WindDirectionDegrees)});
        Out.Record("ship.wind_strength", {Number(Ship.WindStrengthMetresPerSecond)});
        Out.Record("ship.great_cabin", {Flag(Ship.JakeInGreatCabin)});
        Out.Record("ship.active_hands", {Number(Ship.ActiveHands)});
        for (const NamedCrewMember& Member : Ship.NamedCrew)
        {
            Out.Record("ship.crew", {Member.StableId, Flag(Member.Alive), Flag(Member.Aboard)});
        }
        for (const PhysicalChart& Chart : Ship.PhysicalCharts)
        {
            Out.Record("ship.chart", {Chart.ChartId, Flag(Chart.Acquired), Flag(Chart.JakeHasAnnotated)});
        }

        Out.Record("world.valid", {Flag(State.WorldRules.Valid)});
        Out.Record("world.minutes", {Number(State.WorldRules.TotalWorldMinutes)});
        Out.Record("world.chapter", {Number(State.WorldRules.Chapter)});
        Out.Record("world.autosave_suppressed", {Flag(State.WorldRules.AutosaveSuppressed)});

        for (const std::string& Id : State.DefeatedBosses) Out.Record("boss", {Id});
        for (const std::string& Id : State.ImportantShortcuts) Out.Record("shortcut", {Id});
        for (const std::string& Id : State.DiscoveredDungeons) Out.Record("dungeon.discovered", {Id});
        for (const std::string& Id : State.CompletedDungeons) Out.Record("dungeon.completed", {Id});
        for (const std::string& Id : State.RecoveredTreasures) Out.Record("treasure", {Id});
        for (const auto& [Key, Value] : State.QuestOutcomes) Out.Record("outcome", {Key, Value});
        Out.Record("checkpoint", {State.CheckpointId});
        Out.Record("spawn", {State.SpawnId});

        const OpeningProgress& Opening = State.Opening;
        Out.Record("opening.location", {Ordinal(Opening.Location)});
        Out.Record("opening.raid", {Ordinal(Opening.RaidState)});
        Out.Record("opening.recovery", {Ordinal(Opening.RecoveryState)});
        for (const std::string& Id : Opening.FamilyInteractions) Out.Record("opening.family", {Id});
        Out.Record("opening.encounter", {Opening.BoardingEncounterId});
        Out.Record("opening.boarders_required", {Number(Opening.BoardersRequired)});
        Out.Record("opening.boarders_defeated", {Number(Opening.BoardersDefeated)});
        Out.Record("opening.draven_boarded", {Flag(Opening.DravenBoarded)});
        Out.Record("opening.ship_boarded", {Flag(Opening.LaLiberacionBoarded)});
        Out.Record("opening.helm_secured", {Flag(Opening.LaLiberacionHelmSecured)});
        Out.Record("opening.harbor_cleared", {Flag(Opening.LaLiberacionClearedHarbor)});

        char ChecksumText[17];
        std::snprintf(ChecksumText, sizeof(ChecksumText), "%016llx",
            static_cast<unsigned long long>(Checksum(Out.Body)));
        return std::string(Magic) + " " + std::to_string(FormatVersion) + "\nchecksum " + ChecksumText + "\n" + Out.Body;
    }

    bool SaveCodec::Decode(const std::string_view Text, CampaignState& OutState, std::vector<std::string>& OutErrors)
    {
        OutErrors.clear();
        const std::string ExpectedHeader = std::string(Magic) + " " + std::to_string(FormatVersion) + "\n";
        if (Text.substr(0, ExpectedHeader.size()) != ExpectedHeader)
        {
            OutErrors.emplace_back("Not a Dark Arisen save or unsupported save format.");
            return false;
        }
        std::string_view Rest = Text.substr(ExpectedHeader.size());
        const std::size_t ChecksumEnd = Rest.find('\n');
        if (Rest.substr(0, 9) != "checksum " || ChecksumEnd != 25)
        {
            OutErrors.emplace_back("Missing or malformed checksum.");
            return false;
        }
        std::uint64_t Expected = 0;
        const std::string_view ChecksumText = Rest.substr(9, 16);
        const auto Parsed = std::from_chars(ChecksumText.data(), ChecksumText.data() + ChecksumText.size(), Expected, 16);
        const std::string_view Body = Rest.substr(ChecksumEnd + 1);
        if (Parsed.ec != std::errc() || Parsed.ptr != ChecksumText.data() + ChecksumText.size() ||
            Expected != Checksum(Body))
        {
            OutErrors.emplace_back("Save checksum mismatch: file is corrupt or truncated.");
            return false;
        }
        if (Body.empty() || Body.back() != '\n')
        {
            OutErrors.emplace_back("Save body is truncated.");
            return false;
        }

        CampaignState State;
        State.SaveVersion = 0;
        Reader In{State, OutErrors, {}, 0};

        using Handler = std::function<void(const std::vector<std::string>&)>;
        const auto StringScalar = [&In](std::string& Target)
        {
            return [&In, &Target](const std::vector<std::string>& F)
            {
                if (In.Scalar(F[0]) && In.Arity(F, 2)) Target = F[1];
            };
        };
        const auto NumberScalar = [&In](auto& Target)
        {
            return [&In, &Target](const std::vector<std::string>& F)
            {
                if (In.Scalar(F[0]) && In.Arity(F, 2) && !ParseNumber(F[1], Target)) In.Fail("invalid number");
            };
        };
        const auto FlagScalar = [&In](bool& Target)
        {
            return [&In, &Target](const std::vector<std::string>& F)
            {
                if (In.Scalar(F[0]) && In.Arity(F, 2) && !ParseBool(F[1], Target)) In.Fail("invalid flag");
            };
        };
        const auto SetItem = [&In](std::set<std::string>& Target)
        {
            return [&In, &Target](const std::vector<std::string>& F)
            {
                if (In.Arity(F, 2) && !Target.insert(F[1]).second) In.Fail("duplicate entry " + F[1]);
            };
        };
        const auto ListItem = [&In](std::vector<std::string>& Target)
        {
            return [&In, &Target](const std::vector<std::string>& F)
            {
                if (In.Arity(F, 2)) Target.push_back(F[1]);
            };
        };
        const auto TransformScalar = [&In](Transform& Target)
        {
            return [&In, &Target](const std::vector<std::string>& F)
            {
                if (In.Scalar(F[0])) In.ReadTransform(F, Target);
            };
        };
        const auto OrdinalScalar = [&In](auto& Target, const auto Maximum)
        {
            return [&In, &Target, Maximum](const std::vector<std::string>& F)
            {
                if (In.Scalar(F[0]) && In.Arity(F, 2) && !ParseOrdinal(F[1], Target, Maximum)) In.Fail("invalid ordinal");
            };
        };

        PlayerRuntimeSnapshot& Player = State.PlayerRuntime;
        ShipVoyageSnapshot& Ship = State.LaLiberacionVoyage;
        OpeningProgress& Opening = State.Opening;
        const std::map<std::string, Handler, std::less<>> Handlers = {
            {"version", NumberScalar(State.SaveVersion)},
            {"current_mission", StringScalar(State.CurrentMission)},
            {"current_chapter", NumberScalar(State.CurrentChapter)},
            {"mission", [&](const std::vector<std::string>& F)
                {
                    MissionRuntime Runtime;
                    if (In.Arity(F, 3) && ParseOrdinal(F[2], Runtime.State, MissionState::Failed))
                    {
                        Runtime.MissionId = F[1];
                        State.Missions.push_back(std::move(Runtime));
                    }
                    else if (F.size() == 3) In.Fail("invalid mission state");
                }},
            {"story_fact", SetItem(State.StoryFacts)},
            {"world_fact", SetItem(State.WorldFacts)},
            {"progression.level", NumberScalar(State.Progression.Level)},
            {"progression.experience", NumberScalar(State.Progression.Experience)},
            {"progression.inventory", ListItem(State.Progression.Inventory)},
            {"progression.equipment", ListItem(State.Progression.Equipment)},
            {"player.valid", FlagScalar(Player.Valid)},
            {"player.mission", StringScalar(Player.MissionId)},
            {"player.source_level", StringScalar(Player.SourceLevel)},
            {"player.transform", TransformScalar(Player.WorldTransform)},
            {"player.health", NumberScalar(Player.HealthFraction)},
            {"player.stamina", NumberScalar(Player.StaminaFraction)},
            {"crew", [&](const std::vector<std::string>& F)
                {
                    CrewRelationship Crew;
                    if (!In.Arity(F, 8)) return;
                    Crew.CharacterId = F[1];
                    Crew.Role = F[2];
                    if (!ParseNumber(F[3], Crew.Trust) || !ParseBool(F[4], Crew.Met) || !ParseBool(F[5], Crew.Available) ||
                        !ParseBool(F[6], Crew.Recruited) || !ParseBool(F[7], Crew.Aboard))
                    {
                        In.Fail("invalid crew record");
                        return;
                    }
                    State.Crew.push_back(std::move(Crew));
                }},
            {"current_ship", StringScalar(State.CurrentShip)},
            {"ship.valid", FlagScalar(Ship.Valid)},
            {"ship.source_level", StringScalar(Ship.SourceLevel)},
            {"ship.transform", TransformScalar(Ship.WorldTransform)},
            {"ship.owned", FlagScalar(Ship.OwnedAndUnlocked)},
            {"ship.throttle", NumberScalar(Ship.Throttle)},
            {"ship.hull", NumberScalar(Ship.HullIntegrity)},
            {"ship.heading", NumberScalar(Ship.HeadingDegrees)},
            {"ship.commanded_heading", NumberScalar(Ship.CommandedHeadingDegrees)},
            {"ship.speed", NumberScalar(Ship.ForwardSpeedMetresPerSecond)},
            {"ship.wind_direction", NumberScalar(Ship.WindDirectionDegrees)},
            {"ship.wind_strength", NumberScalar(Ship.WindStrengthMetresPerSecond)},
            {"ship.great_cabin", FlagScalar(Ship.JakeInGreatCabin)},
            {"ship.active_hands", NumberScalar(Ship.ActiveHands)},
            {"ship.crew", [&](const std::vector<std::string>& F)
                {
                    NamedCrewMember Member;
                    if (!In.Arity(F, 4)) return;
                    Member.StableId = F[1];
                    if (!ParseBool(F[2], Member.Alive) || !ParseBool(F[3], Member.Aboard)) In.Fail("invalid ship crew");
                    else Ship.NamedCrew.push_back(std::move(Member));
                }},
            {"ship.chart", [&](const std::vector<std::string>& F)
                {
                    PhysicalChart Chart;
                    if (!In.Arity(F, 4)) return;
                    Chart.ChartId = F[1];
                    if (!ParseBool(F[2], Chart.Acquired) || !ParseBool(F[3], Chart.JakeHasAnnotated)) In.Fail("invalid chart");
                    else Ship.PhysicalCharts.push_back(std::move(Chart));
                }},
            {"world.valid", FlagScalar(State.WorldRules.Valid)},
            {"world.minutes", NumberScalar(State.WorldRules.TotalWorldMinutes)},
            {"world.chapter", NumberScalar(State.WorldRules.Chapter)},
            {"world.autosave_suppressed", FlagScalar(State.WorldRules.AutosaveSuppressed)},
            {"boss", SetItem(State.DefeatedBosses)},
            {"shortcut", SetItem(State.ImportantShortcuts)},
            {"dungeon.discovered", SetItem(State.DiscoveredDungeons)},
            {"dungeon.completed", SetItem(State.CompletedDungeons)},
            {"treasure", SetItem(State.RecoveredTreasures)},
            {"outcome", [&](const std::vector<std::string>& F)
                {
                    if (In.Arity(F, 3) && !State.QuestOutcomes.emplace(F[1], F[2]).second) In.Fail("duplicate outcome");
                }},
            {"checkpoint", StringScalar(State.CheckpointId)},
            {"spawn", StringScalar(State.SpawnId)},
            {"opening.location", OrdinalScalar(Opening.Location, OpeningLocation::RexaHarbor)},
            {"opening.raid", OrdinalScalar(Opening.RaidState, OpeningRaidState::DravenAboard)},
            {"opening.recovery", OrdinalScalar(Opening.RecoveryState, WaterRecoveryState::Recovered)},
            {"opening.family", SetItem(Opening.FamilyInteractions)},
            {"opening.encounter", StringScalar(Opening.BoardingEncounterId)},
            {"opening.boarders_required", NumberScalar(Opening.BoardersRequired)},
            {"opening.boarders_defeated", NumberScalar(Opening.BoardersDefeated)},
            {"opening.draven_boarded", FlagScalar(Opening.DravenBoarded)},
            {"opening.ship_boarded", FlagScalar(Opening.LaLiberacionBoarded)},
            {"opening.helm_secured", FlagScalar(Opening.LaLiberacionHelmSecured)},
            {"opening.harbor_cleared", FlagScalar(Opening.LaLiberacionClearedHarbor)},
        };

        std::size_t Start = 0;
        while (Start < Body.size())
        {
            const std::size_t End = Body.find('\n', Start);
            const std::string_view LineText = Body.substr(Start, End - Start);
            Start = End + 1;
            ++In.Line;

            std::vector<std::string> Fields;
            std::size_t FieldStart = 0;
            bool Valid = true;
            while (true)
            {
                const std::size_t Tab = LineText.find('\t', FieldStart);
                std::string Field;
                if (!Unescape(LineText.substr(FieldStart, Tab - FieldStart), Field))
                {
                    Valid = false;
                    break;
                }
                Fields.push_back(std::move(Field));
                if (Tab == std::string_view::npos) break;
                FieldStart = Tab + 1;
            }
            if (!Valid || Fields.size() < 2)
            {
                In.Fail("malformed record");
                continue;
            }
            const auto HandlerIt = Handlers.find(Fields.front());
            if (HandlerIt == Handlers.end())
            {
                In.Fail("unknown record " + Fields.front());
                continue;
            }
            HandlerIt->second(Fields);
        }

        for (const char* Required : {"version", "current_mission", "current_chapter"})
        {
            if (In.SeenScalars.count(Required) == 0)
            {
                OutErrors.emplace_back(std::string("Missing required record ") + Required + ".");
            }
        }
        if (!OutErrors.empty())
        {
            return false;
        }
        OutState = std::move(State);
        return true;
    }
}
