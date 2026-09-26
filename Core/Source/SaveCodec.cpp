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

        for (const auto& [RegionId, Region] : State.ColonialWar.Regions)
        {
            Out.Record("war.region", {RegionId, Number(Region.ImperialControl), Number(Region.AlbionControl),
                Number(Region.LiberationStrength), Number(Region.CrimsonThreat), Ordinal(Region.Outcome),
                Flag(Region.FallAssaultCompleted), Number(Region.LastAutonomousTickChapter)});
        }
        for (const std::string& Id : State.ColonialWar.OwnedHoldings) Out.Record("war.holding", {Id});

        const QuestJournalState& Journal = State.Journal;
        for (const auto& [Id, Quest] : Journal.Quests)
        {
            Out.Record("journal.quest", {Id, Ordinal(Quest.Lifecycle), Ordinal(Quest.ActivationTrigger), Number(Quest.ActivationGameMinute),
                Number(Quest.ExpirationGameMinute), Number(Quest.ResolutionGameMinute), Quest.OutcomeId});
        }
        for (const JournalEntry& Entry : Journal.Entries)
        {
            Out.Record("journal.entry", {Entry.EntryId, Entry.QuestId, Entry.JakeText, Entry.Directions, Number(Entry.GameMinute),
                Number(Entry.Sequence), Flag(Entry.Distorted), Flag(Entry.Correction)});
        }
        Out.Record("journal.next", {Number(Journal.NextSequence)});
        for (const EvidenceRecord& Record : Journal.Evidence)
        {
            Out.Record("journal.evidence", {Record.EvidenceId, Record.MissionId, Record.Title, Record.Body, Number(Record.GameMinute)});
        }
        for (const std::string& Id : Journal.Observations) Out.Record("journal.observation", {Id});

        const ProgressionState& Progress = State.Progress;
        Out.Record("progress.body", {Number(Progress.MaximumHealth), Number(Progress.MaximumStamina), Number(Progress.MaximumPosture),
            Number(Progress.CarryKilograms)});
        Out.Record("progress.marks", {Number(Progress.MarksEarned), Number(Progress.MarksSpent)});
        Out.Record("progress.wallet", {Number(Progress.Wallet.Doubloons), Number(Progress.Wallet.Pounds), Number(Progress.Wallet.SilverMarks)});
        const std::pair<const char*, const std::set<std::string, std::less<>>*> ProgressSets[] = {
            {"draught", &Progress.PhysiciansDraughtSources}, {"pearl", &Progress.DeepWaterPearlSources},
            {"deflection", &Progress.NamedDeflectionSources}, {"carry", &Progress.CarryMilestones},
            {"mark_source", &Progress.AwardedMarkSources}, {"teacher", &Progress.TeachersMet},
            {"scene", &Progress.CompletedTeachingScenes}, {"flag", &Progress.WorldFlags}, {"node", &Progress.LearnedNodes},
            {"exclusive", &Progress.ChosenExclusiveGroups}, {"overheard", &Progress.OverheardConversations},
            {"legendary", &Progress.PaidLegendaryWork}, {"reconstruction", &Progress.PaidReconstructionProjects},
            {"service", &Progress.PurchasedServices}};
        for (const auto& [Name, Set] : ProgressSets)
        {
            for (const std::string& Id : *Set) Out.Record("progress.set", {Name, Id});
        }
        for (const auto& [Context, Greeting] : Progress.Greetings) Out.Record("progress.greeting", {Context, Ordinal(Greeting)});
        for (const auto& [Context, Greeting] : Progress.GreetingBeforeWary) Out.Record("progress.greeting_before_wary", {Context, Ordinal(Greeting)});
        for (const auto& [Context, Chapters] : Progress.WaryChaptersRemaining) Out.Record("progress.wary", {Context, Number(Chapters)});
        Out.Record("progress.ledger", {Number(Progress.PendingLedger.HoldingIncome), Number(Progress.PendingLedger.ArmyUpkeep),
            Number(Progress.PendingLedger.GarrisonUpkeep), Number(Progress.PendingLedger.ConstructionDraw), Number(Progress.LastResolvedLedgerChapter)});
        for (const int Chapter : Progress.PaidCrewShareChapters) Out.Record("progress.crew_share", {Number(Chapter)});

        Out.Record("npc.clock", {Number(State.LivingWorld.LastSimulatedGameMinute)});
        for (const auto& [Id, Npc] : State.LivingWorld.Records)
        {
            Out.Record("npc.record", {Id, Npc.CommunityId, Npc.ScheduleAnchorId, Number(Npc.PersonalReputation), Number(Npc.SpecificTrust),
                Ordinal(Npc.Mood), Flag(Npc.Alive), Flag(Npc.Available)});
        }
        for (const auto& [Id, Npc] : State.LivingWorld.Records)
        {
            for (const NpcMemory& Memory : Npc.Memories)
            {
                Out.Record("npc.memory", {Id, Memory.EventId, Ordinal(Memory.Kind), Ordinal(Memory.Emotion), Number(Memory.OriginalWeight),
                    Number(Memory.EffectiveWeight), Number(Memory.OccurredAtGameMinute), Number(Memory.DecayPerGameDay), Memory.SourceNpcId,
                    Flag(Memory.Permanent)});
            }
            for (const NpcConnection& Connection : Npc.Connections) Out.Record("npc.connection", {Id, Connection.OtherNpcId, Ordinal(Connection.Kind)});
            for (const std::string& Interaction : Npc.ResolvedInteractions) Out.Record("npc.resolved", {Id, Interaction});
        }

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
            {"war.region", [&](const std::vector<std::string>& F)
                {
                    if (!In.Arity(F, 9)) return;
                    ColonialRegionState Region;
                    bool Valid = ParseNumber(F[2], Region.ImperialControl) && ParseNumber(F[3], Region.AlbionControl) &&
                        ParseNumber(F[4], Region.LiberationStrength) && ParseNumber(F[5], Region.CrimsonThreat) &&
                        ParseOrdinal(F[6], Region.Outcome, RegionalWarOutcome::CrimsonOccupied) &&
                        ParseBool(F[7], Region.FallAssaultCompleted) && ParseNumber(F[8], Region.LastAutonomousTickChapter);
                    if (!Valid) In.Fail("invalid war region");
                    else if (!State.ColonialWar.Regions.emplace(F[1], Region).second) In.Fail("duplicate war region " + F[1]);
                }},
            {"war.holding", [&](const std::vector<std::string>& F)
                {
                    if (In.Arity(F, 2) && !State.ColonialWar.OwnedHoldings.insert(F[1]).second) In.Fail("duplicate holding " + F[1]);
                }},
            {"journal.quest", [&](const std::vector<std::string>& F)
                {
                    if (!In.Arity(F, 8)) return;
                    QuestRuntimeState Quest;
                    const bool Valid = ParseOrdinal(F[2], Quest.Lifecycle, QuestLifecycle::Unavailable) &&
                        ParseOrdinal(F[3], Quest.ActivationTrigger, QuestActivationTrigger::Absence) && ParseNumber(F[4], Quest.ActivationGameMinute) &&
                        ParseNumber(F[5], Quest.ExpirationGameMinute) && ParseNumber(F[6], Quest.ResolutionGameMinute);
                    Quest.OutcomeId = F[7];
                    if (!Valid) In.Fail("invalid journal quest");
                    else if (!State.Journal.Quests.emplace(F[1], Quest).second) In.Fail("duplicate journal quest " + F[1]);
                }},
            {"journal.entry", [&](const std::vector<std::string>& F)
                {
                    if (!In.Arity(F, 9)) return;
                    JournalEntry Entry{F[1], F[2], F[3], F[4]};
                    if (!ParseNumber(F[5], Entry.GameMinute) || !ParseNumber(F[6], Entry.Sequence) || !ParseBool(F[7], Entry.Distorted) ||
                        !ParseBool(F[8], Entry.Correction))
                    {
                        In.Fail("invalid journal entry");
                    }
                    else State.Journal.Entries.push_back(std::move(Entry));
                }},
            {"journal.next", NumberScalar(State.Journal.NextSequence)},
            {"journal.evidence", [&](const std::vector<std::string>& F)
                {
                    if (!In.Arity(F, 6)) return;
                    EvidenceRecord Record{F[1], F[2], F[3], F[4]};
                    if (!ParseNumber(F[5], Record.GameMinute)) In.Fail("invalid evidence record");
                    else State.Journal.Evidence.push_back(std::move(Record));
                }},
            {"journal.observation", ListItem(State.Journal.Observations)},
            {"progress.body", [&](const std::vector<std::string>& F)
                {
                    ProgressionState& P = State.Progress;
                    if (In.Scalar(F[0]) && In.Arity(F, 5) && !(ParseNumber(F[1], P.MaximumHealth) && ParseNumber(F[2], P.MaximumStamina) &&
                            ParseNumber(F[3], P.MaximumPosture) && ParseNumber(F[4], P.CarryKilograms)))
                    {
                        In.Fail("invalid progression body");
                    }
                }},
            {"progress.marks", [&](const std::vector<std::string>& F)
                {
                    if (In.Scalar(F[0]) && In.Arity(F, 3) && !(ParseNumber(F[1], State.Progress.MarksEarned) && ParseNumber(F[2], State.Progress.MarksSpent)))
                    {
                        In.Fail("invalid marks");
                    }
                }},
            {"progress.wallet", [&](const std::vector<std::string>& F)
                {
                    CurrencyWallet& W = State.Progress.Wallet;
                    if (In.Scalar(F[0]) && In.Arity(F, 4) && !(ParseNumber(F[1], W.Doubloons) && ParseNumber(F[2], W.Pounds) && ParseNumber(F[3], W.SilverMarks)))
                    {
                        In.Fail("invalid wallet");
                    }
                }},
            {"progress.set", [&](const std::vector<std::string>& F)
                {
                    if (!In.Arity(F, 3)) return;
                    ProgressionState& P = State.Progress;
                    const std::map<std::string, std::set<std::string, std::less<>>*, std::less<>> Sets = {{"draught", &P.PhysiciansDraughtSources},
                        {"pearl", &P.DeepWaterPearlSources}, {"deflection", &P.NamedDeflectionSources}, {"carry", &P.CarryMilestones},
                        {"mark_source", &P.AwardedMarkSources}, {"teacher", &P.TeachersMet}, {"scene", &P.CompletedTeachingScenes},
                        {"flag", &P.WorldFlags}, {"node", &P.LearnedNodes}, {"exclusive", &P.ChosenExclusiveGroups},
                        {"overheard", &P.OverheardConversations}, {"legendary", &P.PaidLegendaryWork},
                        {"reconstruction", &P.PaidReconstructionProjects}, {"service", &P.PurchasedServices}};
                    const auto Set = Sets.find(F[1]);
                    if (Set == Sets.end()) In.Fail("unknown progression set " + F[1]);
                    else if (!Set->second->insert(F[2]).second) In.Fail("duplicate progression entry " + F[2]);
                }},
            {"progress.greeting", [&](const std::vector<std::string>& F)
                {
                    GreetingState Greeting{};
                    if (In.Arity(F, 3) && (!ParseOrdinal(F[2], Greeting, GreetingState::Wary) || !State.Progress.Greetings.emplace(F[1], Greeting).second))
                    {
                        In.Fail("invalid greeting");
                    }
                }},
            {"progress.greeting_before_wary", [&](const std::vector<std::string>& F)
                {
                    GreetingState Greeting{};
                    if (In.Arity(F, 3) &&
                        (!ParseOrdinal(F[2], Greeting, GreetingState::Wary) || !State.Progress.GreetingBeforeWary.emplace(F[1], Greeting).second))
                    {
                        In.Fail("invalid greeting memory");
                    }
                }},
            {"progress.wary", [&](const std::vector<std::string>& F)
                {
                    int Chapters = 0;
                    if (In.Arity(F, 3) && (!ParseNumber(F[2], Chapters) || !State.Progress.WaryChaptersRemaining.emplace(F[1], Chapters).second))
                    {
                        In.Fail("invalid wary memory");
                    }
                }},
            {"progress.ledger", [&](const std::vector<std::string>& F)
                {
                    ChapterLedger& L = State.Progress.PendingLedger;
                    if (In.Scalar(F[0]) && In.Arity(F, 6) && !(ParseNumber(F[1], L.HoldingIncome) && ParseNumber(F[2], L.ArmyUpkeep) &&
                            ParseNumber(F[3], L.GarrisonUpkeep) && ParseNumber(F[4], L.ConstructionDraw) &&
                            ParseNumber(F[5], State.Progress.LastResolvedLedgerChapter)))
                    {
                        In.Fail("invalid ledger");
                    }
                }},
            {"progress.crew_share", [&](const std::vector<std::string>& F)
                {
                    int Chapter = 0;
                    if (In.Arity(F, 2) && (!ParseNumber(F[1], Chapter) || !State.Progress.PaidCrewShareChapters.insert(Chapter).second))
                    {
                        In.Fail("invalid crew share");
                    }
                }},
            {"npc.clock", NumberScalar(State.LivingWorld.LastSimulatedGameMinute)},
            {"npc.record", [&](const std::vector<std::string>& F)
                {
                    if (!In.Arity(F, 9)) return;
                    NpcRecord Npc;
                    Npc.CommunityId = F[2];
                    Npc.ScheduleAnchorId = F[3];
                    if (!(ParseNumber(F[4], Npc.PersonalReputation) && ParseNumber(F[5], Npc.SpecificTrust) && ParseOrdinal(F[6], Npc.Mood, NpcMood::Excited) &&
                            ParseBool(F[7], Npc.Alive) && ParseBool(F[8], Npc.Available)))
                    {
                        In.Fail("invalid NPC record");
                    }
                    else if (!State.LivingWorld.Records.emplace(F[1], std::move(Npc)).second) In.Fail("duplicate NPC " + F[1]);
                }},
            {"npc.memory", [&](const std::vector<std::string>& F)
                {
                    if (!In.Arity(F, 11)) return;
                    const auto Npc = State.LivingWorld.Records.find(F[1]);
                    NpcMemory Memory;
                    Memory.EventId = F[2];
                    Memory.SourceNpcId = F[9];
                    if (Npc == State.LivingWorld.Records.end() || !(ParseOrdinal(F[3], Memory.Kind, NpcMemoryKind::CulturalReputation) &&
                            ParseOrdinal(F[4], Memory.Emotion, NpcMemoryEmotion::Positive) && ParseNumber(F[5], Memory.OriginalWeight) &&
                            ParseNumber(F[6], Memory.EffectiveWeight) && ParseNumber(F[7], Memory.OccurredAtGameMinute) &&
                            ParseNumber(F[8], Memory.DecayPerGameDay) && ParseBool(F[10], Memory.Permanent)))
                    {
                        In.Fail("invalid NPC memory");
                    }
                    else Npc->second.Memories.push_back(std::move(Memory));
                }},
            {"npc.connection", [&](const std::vector<std::string>& F)
                {
                    if (!In.Arity(F, 4)) return;
                    const auto Npc = State.LivingWorld.Records.find(F[1]);
                    NpcConnection Connection{F[2]};
                    if (Npc == State.LivingWorld.Records.end() || !ParseOrdinal(F[3], Connection.Kind, NpcConnectionKind::Community))
                    {
                        In.Fail("invalid NPC connection");
                    }
                    else Npc->second.Connections.push_back(std::move(Connection));
                }},
            {"npc.resolved", [&](const std::vector<std::string>& F)
                {
                    if (!In.Arity(F, 3)) return;
                    const auto Npc = State.LivingWorld.Records.find(F[1]);
                    if (Npc == State.LivingWorld.Records.end() || !Npc->second.ResolvedInteractions.insert(F[2]).second) In.Fail("invalid NPC interaction");
                }},
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
