#include "DarkArisen/Core/ProgressionEconomy.h"

#include "DarkArisen/Core/CampaignState.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace DarkArisen::Core
{
    namespace
    {
        constexpr std::int64_t LedgerMagnitudeLimit = 1000000000000LL;

        std::int64_t& Account(ProgressionState& State, const Currency Kind)
        {
            switch (Kind)
            {
            case Currency::Pounds: return State.Wallet.Pounds;
            case Currency::SilverMarks: return State.Wallet.SilverMarks;
            case Currency::Doubloons: break;
            }
            return State.Wallet.Doubloons;
        }

        std::string SceneKey(const std::string_view TeacherId, const std::string_view NodeId)
        {
            return std::string(TeacherId) + "::" + std::string(NodeId);
        }

        bool IsValidLedger(const ChapterLedger& Ledger)
        {
            return Ledger.HoldingIncome >= -LedgerMagnitudeLimit && Ledger.HoldingIncome <= LedgerMagnitudeLimit &&
                Ledger.ArmyUpkeep >= -LedgerMagnitudeLimit && Ledger.ArmyUpkeep <= 0 && Ledger.GarrisonUpkeep >= -LedgerMagnitudeLimit &&
                Ledger.GarrisonUpkeep <= 0 && Ledger.ConstructionDraw >= -LedgerMagnitudeLimit && Ledger.ConstructionDraw <= 0;
        }

        bool IsTeacher(const std::string_view Id)
        {
            const auto& Teachers = SkillCatalog::TeacherIds();
            return std::find(Teachers.begin(), Teachers.end(), Id) != Teachers.end();
        }
    }

    const std::vector<std::string_view>& SkillCatalog::TeacherIds()
    {
        static const std::vector<std::string_view> Teachers = {"teacher.mira", "teacher.big_tom", "teacher.ines", "teacher.father_salvio",
            "teacher.esteban", "teacher.mama_jacinta", "teacher.mbah_seruni", "teacher.mormor_astrid", "teacher.don_alejandro",
            "teacher.mateusz_voltari", "teacher.cassandra_wells", "teacher.dr_eleanor_whitcomb", "teacher.lt_marcus_sandford",
            "teacher.yara_sun_walker", "teacher.pak_setyo", "teacher.bu_wira", "teacher.isolde_thorn", "teacher.gruffydd",
            "teacher.maeve_donovan", "teacher.margarethe_fitzmueller", "teacher.admiral_sterling", "teacher.bram_kettle",
            "teacher.wooden_tooth_holloway"};
        return Teachers;
    }

    SkillCatalog SkillCatalog::Canonical()
    {
        SkillCatalog Catalog;
        const auto Add = [&Catalog](const char* NodeId, const char* BranchId, const int Cost, std::vector<std::string> Teachers,
                             const SkillGateKind Gate, const char* GateId, const int MinimumPosture)
        {
            SkillNode Node;
            Node.NodeId = NodeId;
            Node.BranchId = BranchId;
            Node.MarkCost = Cost;
            Node.TeacherOptions = std::move(Teachers);
            Node.GateKind = Gate;
            Node.RequiredGateId = GateId;
            Node.MinimumPosture = MinimumPosture;
            Catalog.Register(Node);
        };
        using G = SkillGateKind;
        // The bible names these nodes explicitly; the remaining authored nodes are not invented here.
        Add("node.blade.seamans_guard", "Blade", 1, {}, G::None, "", 0);
        Add("node.blade.mercy", "Blade", 2, {"teacher.father_salvio"}, G::None, "", 0);
        Add("node.blade.officers_line", "Blade", 3, {"teacher.don_alejandro"}, G::None, "", 0);
        Add("node.blade.big_toms_answer", "Blade", 1, {"teacher.big_tom"}, G::None, "", 0);
        Add("node.blade.long_night", "Blade", 4, {}, G::None, "", 150);
        Add("node.shadow.sound_discipline", "Shadow", 1, {}, G::None, "", 0);
        Add("node.shadow.counting_halls", "Shadow", 2, {"teacher.mateusz_voltari"}, G::None, "", 0);
        Add("node.shadow.fourth_rule", "Shadow", 2, {}, G::Standing, "standing.highmoore.fourth_rule", 0);
        Add("node.sea.estebans_sky", "Sea", 2, {"teacher.esteban"}, G::None, "", 0);
        Add("node.sea.weather_gauge", "Sea", 3, {"teacher.mira", "teacher.admiral_sterling"}, G::None, "", 0);
        Add("node.sea.hot_shot", "Sea", 2, {"teacher.maeve_donovan"}, G::None, "", 0);
        Add("node.land.hot_stone_walk", "Land", 2, {"teacher.mbah_seruni"}, G::Standing, "standing.hot_stone_walk", 0);
        Add("node.land.cold_is_a_teacher", "Land", 2, {"teacher.mormor_astrid"}, G::None, "", 0);
        Add("node.land.the_line", "Land", 3, {}, G::WorldState, "world.horse_bond_3", 0);
        Add("node.land.the_canopy", "Land", 2, {}, G::Standing, "standing.cultural_web_tier_3", 0);
        Add("node.word.the_register", "Word", 2, {"teacher.margarethe_fitzmueller", "teacher.don_alejandro"}, G::None, "", 0);
        Add("node.word.the_invoice", "Word", 2, {"teacher.bram_kettle"}, G::WorldState, "world.regulators_turned", 0);
        Add("node.word.whose_man", "Word", 1, {}, G::Standing, "standing.highmoore.whose_man", 0);
        Add("node.word.the_ledger", "Word", 3, {"teacher.ines"}, G::None, "", 0);
        return Catalog;
    }

    bool SkillCatalog::Register(const SkillNode& Node)
    {
        static const std::set<std::string, std::less<>> Branches = {"Blade", "Shadow", "Sea", "Land", "Word"};
        if (Node.NodeId.empty() || Node.MarkCost < 1 || Node.MarkCost > 4 || !Branches.count(Node.BranchId) || Nodes.count(Node.NodeId) ||
            static_cast<int>(Nodes.size()) >= RequiredSkillNodeCount || Node.PrerequisiteNodeIds.size() > 3)
        {
            return false;
        }
        std::set<std::string> Unique;
        for (const std::string& Prerequisite : Node.PrerequisiteNodeIds)
        {
            if (Prerequisite.empty() || Prerequisite == Node.NodeId || !Unique.insert(Prerequisite).second) return false;
        }
        if ((Node.GateKind != SkillGateKind::None) == Node.RequiredGateId.empty())
        {
            return false;
        }
        for (const std::string& Teacher : Node.TeacherOptions)
        {
            if (!IsTeacher(Teacher)) return false;
        }
        Nodes.emplace(Node.NodeId, Node);
        return true;
    }

    const SkillNode* SkillCatalog::Find(const std::string_view NodeId) const
    {
        const auto It = Nodes.find(NodeId);
        return It == Nodes.end() ? nullptr : &It->second;
    }

    bool SkillCatalog::IsComplete() const
    {
        if (static_cast<int>(Nodes.size()) != RequiredSkillNodeCount) return false;
        std::map<std::string, int> Branch;
        int Cost = 0;
        int TeacherGated = 0;
        int StandingGated = 0;
        std::set<std::string> Teachers;
        for (const auto& [Id, Node] : Nodes)
        {
            ++Branch[Node.BranchId];
            Cost += Node.MarkCost;
            if (!Node.TeacherOptions.empty())
            {
                ++TeacherGated;
                Teachers.insert(Node.TeacherOptions.begin(), Node.TeacherOptions.end());
            }
            StandingGated += Node.GateKind == SkillGateKind::Standing ? 1 : 0;
            for (const std::string& Prerequisite : Node.PrerequisiteNodeIds)
            {
                if (!Nodes.count(Prerequisite)) return false;
            }
        }
        return Branch["Blade"] == 16 && Branch["Shadow"] == 12 && Branch["Sea"] == 14 && Branch["Land"] == 13 && Branch["Word"] == 13 &&
            Cost == FullTreeMarkCost && TeacherGated == RequiredTeacherGatedNodeCount && StandingGated == RequiredStandingGatedNodeCount &&
            static_cast<int>(Teachers.size()) == RequiredTeacherCount;
    }

    namespace Progression
    {
        bool ApplyPhysiciansDraught(ProgressionState& State, const std::string_view SourceId)
        {
            if (SourceId.empty() || State.PhysiciansDraughtSources.count(SourceId) || State.PhysiciansDraughtSources.size() >= 12) return false;
            State.PhysiciansDraughtSources.emplace(SourceId);
            State.MaximumHealth = std::min(380, State.MaximumHealth + 15);
            return true;
        }

        bool ApplyDeepWaterPearl(ProgressionState& State, const std::string_view SourceId)
        {
            if (SourceId.empty() || State.DeepWaterPearlSources.count(SourceId) || State.DeepWaterPearlSources.size() >= 16) return false;
            State.DeepWaterPearlSources.emplace(SourceId);
            State.MaximumStamina = std::min(200, State.MaximumStamina + 5);
            return true;
        }

        bool RecordNamedDeflectionSet(ProgressionState& State, const std::string_view OpponentId)
        {
            if (OpponentId.empty() || State.NamedDeflectionSources.count(OpponentId) || State.NamedDeflectionSources.size() >= 15) return false;
            State.NamedDeflectionSources.emplace(OpponentId);
            State.MaximumPosture = std::min(175, State.MaximumPosture + 5);
            return true;
        }

        bool RecordCarryMilestone(ProgressionState& State, const std::string_view MilestoneId, const double AuthoredCarryKg)
        {
            if (MilestoneId.empty() || State.CarryMilestones.count(MilestoneId) || !(AuthoredCarryKg > State.CarryKilograms) ||
                AuthoredCarryKg > 130.0)
            {
                return false;
            }
            State.CarryMilestones.emplace(MilestoneId);
            State.CarryKilograms = AuthoredCarryKg;
            return true;
        }

        bool AwardMarks(ProgressionState& State, const std::string_view AuthoredSourceId, const int Amount)
        {
            if (AuthoredSourceId.empty() || Amount <= 0 || State.AwardedMarkSources.count(AuthoredSourceId) ||
                State.MarksEarned > SkillCatalog::MaximumAvailableMarks - Amount)
            {
                return false;
            }
            State.AwardedMarkSources.emplace(AuthoredSourceId);
            State.MarksEarned += Amount;
            return true;
        }

        bool RecordTeacherMet(ProgressionState& State, const std::string_view TeacherId)
        {
            return IsTeacher(TeacherId) && State.TeachersMet.emplace(TeacherId).second;
        }

        bool CompleteTeachingScene(ProgressionState& State, const SkillCatalog& Catalog, const std::string_view TeacherId,
            const std::string_view NodeId)
        {
            const SkillNode* Node = Catalog.Find(NodeId);
            if (!State.TeachersMet.count(TeacherId) || Node == nullptr ||
                std::find(Node->TeacherOptions.begin(), Node->TeacherOptions.end(), TeacherId) == Node->TeacherOptions.end())
            {
                return false;
            }
            State.CompletedTeachingScenes.insert(SceneKey(TeacherId, NodeId));
            return true;
        }

        bool SetWorldFlag(ProgressionState& State, const std::string_view FlagId, const bool Enabled)
        {
            if (FlagId.empty()) return false;
            if (Enabled) State.WorldFlags.emplace(FlagId);
            else if (const auto It = State.WorldFlags.find(FlagId); It != State.WorldFlags.end()) State.WorldFlags.erase(It);
            return true;
        }

        bool CanLearn(const ProgressionState& State, const SkillCatalog& Catalog, const std::string_view NodeId)
        {
            const SkillNode* Node = Catalog.Find(NodeId);
            if (Node == nullptr || State.LearnedNodes.count(NodeId) || State.MarksEarned - State.MarksSpent < Node->MarkCost ||
                State.MaximumPosture < Node->MinimumPosture)
            {
                return false;
            }
            for (const std::string& Prerequisite : Node->PrerequisiteNodeIds)
            {
                if (!State.LearnedNodes.count(Prerequisite)) return false;
            }
            if (Node->GateKind != SkillGateKind::None && !State.WorldFlags.count(Node->RequiredGateId)) return false;
            if (!Node->ExclusiveGroup.empty() && State.ChosenExclusiveGroups.count(Node->ExclusiveGroup)) return false;
            if (!Node->TeacherOptions.empty())
            {
                return std::any_of(Node->TeacherOptions.begin(), Node->TeacherOptions.end(),
                    [&](const std::string& Teacher) { return State.CompletedTeachingScenes.count(SceneKey(Teacher, NodeId)) != 0; });
            }
            return true;
        }

        bool TryLearnNode(ProgressionState& State, const SkillCatalog& Catalog, const std::string_view NodeId)
        {
            if (!CanLearn(State, Catalog, NodeId)) return false;
            const SkillNode& Node = *Catalog.Find(NodeId);
            State.MarksSpent += Node.MarkCost;
            State.LearnedNodes.emplace(NodeId);
            if (!Node.ExclusiveGroup.empty()) State.ChosenExclusiveGroups.insert(Node.ExclusiveGroup);
            return true;
        }

        bool Credit(ProgressionState& State, const Currency Kind, const std::int64_t Amount)
        {
            std::int64_t& Balance = Account(State, Kind);
            if (Amount <= 0 || Balance > std::numeric_limits<std::int64_t>::max() - Amount) return false;
            Balance += Amount;
            return true;
        }

        bool Spend(ProgressionState& State, const Currency Kind, const std::int64_t Amount)
        {
            std::int64_t& Balance = Account(State, Kind);
            if (Amount <= 0 || Balance < Amount) return false;
            Balance -= Amount;
            return true;
        }

        std::int64_t Balance(const ProgressionState& State, const Currency Kind)
        {
            switch (Kind)
            {
            case Currency::Pounds: return State.Wallet.Pounds;
            case Currency::SilverMarks: return State.Wallet.SilverMarks;
            case Currency::Doubloons: break;
            }
            return State.Wallet.Doubloons;
        }

        bool PurchaseService(ProgressionState& State, CharacterProgression& Character, const ServiceOffer& Offer)
        {
            if (Offer.ServiceId.empty() || Offer.Price < 0 || (Offer.OneTime && State.PurchasedServices.count(Offer.ServiceId)) ||
                (Offer.Price > 0 && Balance(State, Offer.PriceCurrency) < Offer.Price))
            {
                return false;
            }
            std::vector<std::string> Inventory = Character.Inventory;
            for (const std::string& Item : Offer.ConsumedItems)
            {
                const auto It = std::find(Inventory.begin(), Inventory.end(), Item);
                if (It == Inventory.end()) return false;
                Inventory.erase(It);
            }
            if (Offer.Price > 0 && !Spend(State, Offer.PriceCurrency, Offer.Price)) return false;
            Inventory.insert(Inventory.end(), Offer.GrantedItems.begin(), Offer.GrantedItems.end());
            Character.Inventory = std::move(Inventory);
            if (Offer.OneTime) State.PurchasedServices.insert(Offer.ServiceId);
            return true;
        }

        bool SetPendingLedger(ProgressionState& State, const ChapterLedger& Ledger)
        {
            if (!IsValidLedger(Ledger)) return false;
            State.PendingLedger = Ledger;
            return true;
        }

        bool ResolveChapterLedger(ProgressionState& State, const int Chapter)
        {
            if (Chapter <= State.LastResolvedLedgerChapter || !IsValidLedger(State.PendingLedger)) return false;
            const std::int64_t Net = State.PendingLedger.NetDoubloons();
            const bool Resolved = Net >= 0 ? (Net == 0 || Credit(State, Currency::Doubloons, Net)) : Spend(State, Currency::Doubloons, -Net);
            if (!Resolved) return false;
            State.LastResolvedLedgerChapter = Chapter;
            State.PendingLedger = ChapterLedger{};
            return true;
        }

        bool PayLegendaryTierCost(ProgressionState& State, const Currency Kind, const std::int64_t Amount, const std::string_view WorkId)
        {
            if (WorkId.empty() || Amount <= 0 || State.PaidLegendaryWork.count(WorkId) || !Spend(State, Kind, Amount)) return false;
            State.PaidLegendaryWork.emplace(WorkId);
            return true;
        }

        bool PayReconstructionSilver(ProgressionState& State, const std::int64_t SilverMarks, const std::string_view ProjectId)
        {
            if (ProjectId.empty() || SilverMarks <= 0 || State.PaidReconstructionProjects.count(ProjectId) ||
                !Spend(State, Currency::SilverMarks, SilverMarks))
            {
                return false;
            }
            State.PaidReconstructionProjects.emplace(ProjectId);
            return true;
        }

        bool PayCrewShare(ProgressionState& State, const std::int64_t Doubloons, const int Chapter)
        {
            if (Doubloons <= 0 || Chapter <= 0 || State.PaidCrewShareChapters.count(Chapter) || !Spend(State, Currency::Doubloons, Doubloons))
            {
                return false;
            }
            State.PaidCrewShareChapters.insert(Chapter);
            return true;
        }

        void SetGreeting(ProgressionState& State, const std::string_view SocialContextId, const GreetingState NewState)
        {
            if (!SocialContextId.empty()) State.Greetings[std::string(SocialContextId)] = NewState;
        }

        GreetingState Greeting(const ProgressionState& State, const std::string_view SocialContextId)
        {
            const auto It = State.Greetings.find(SocialContextId);
            return It == State.Greetings.end() ? GreetingState::Stranger : It->second;
        }

        bool RecordWeaponDrawnHere(ProgressionState& State, const std::string_view SocialContextId, const int RecoveryChapters)
        {
            if (SocialContextId.empty() || RecoveryChapters <= 0) return false;
            const std::string Context(SocialContextId);
            const GreetingState Current = Greeting(State, Context);
            if (Current != GreetingState::Wary) State.GreetingBeforeWary[Context] = Current;
            State.Greetings[Context] = GreetingState::Wary;
            int& Remaining = State.WaryChaptersRemaining[Context];
            Remaining = std::max(Remaining, RecoveryChapters);
            return true;
        }

        void AdvanceSocialChapter(ProgressionState& State)
        {
            for (auto It = State.WaryChaptersRemaining.begin(); It != State.WaryChaptersRemaining.end();)
            {
                if (--It->second > 0)
                {
                    ++It;
                    continue;
                }
                const auto Previous = State.GreetingBeforeWary.find(It->first);
                State.Greetings[It->first] = Previous != State.GreetingBeforeWary.end() ? Previous->second : GreetingState::Known;
                if (Previous != State.GreetingBeforeWary.end()) State.GreetingBeforeWary.erase(Previous);
                It = State.WaryChaptersRemaining.erase(It);
            }
        }

        bool RecordOverheard(ProgressionState& State, const std::string_view ConversationId)
        {
            return !ConversationId.empty() && State.OverheardConversations.emplace(ConversationId).second;
        }

        bool Validate(const ProgressionState& State, std::vector<std::string>& OutErrors)
        {
            const std::size_t Before = OutErrors.size();
            if (State.MaximumHealth <= 0 || State.MaximumHealth > 380 || State.MaximumStamina <= 0 || State.MaximumStamina > 200 ||
                State.MaximumPosture <= 0 || State.MaximumPosture > 175 || !(State.CarryKilograms > 0.0) || State.CarryKilograms > 130.0 ||
                !std::isfinite(State.CarryKilograms))
            {
                OutErrors.emplace_back("Progression body values are outside their authored bounds.");
            }
            if (State.MarksEarned < 0 || State.MarksSpent < 0 || State.MarksSpent > State.MarksEarned ||
                State.MarksEarned > SkillCatalog::MaximumAvailableMarks)
            {
                OutErrors.emplace_back("Progression marks are inconsistent.");
            }
            for (const std::string& Node : State.LearnedNodes)
            {
                if (Node.empty()) OutErrors.emplace_back("Learned skill node without an id.");
            }
            if (State.Wallet.Doubloons < 0 || State.Wallet.Pounds < 0 || State.Wallet.SilverMarks < 0)
            {
                OutErrors.emplace_back("Wallet balance is negative.");
            }
            if (!IsValidLedger(State.PendingLedger) || State.LastResolvedLedgerChapter < 0)
            {
                OutErrors.emplace_back("Chapter ledger is invalid.");
            }
            for (const auto& [Context, Remaining] : State.WaryChaptersRemaining)
            {
                const auto Greeting = State.Greetings.find(Context);
                if (Remaining <= 0 || Greeting == State.Greetings.end() || Greeting->second != GreetingState::Wary)
                {
                    OutErrors.push_back("Social context " + Context + " has an inconsistent Wary memory.");
                }
            }
            return OutErrors.size() == Before;
        }
    }

    bool SocialPresence::BeginSitting(const std::string_view SeatId)
    {
        if (SeatId.empty() || !Seat.empty()) return false;
        Seat = std::string(SeatId);
        return true;
    }

    bool SocialPresence::BeginListening(const std::string_view ConversationId)
    {
        if (ConversationId.empty() || !Conversation.empty()) return false;
        Conversation = std::string(ConversationId);
        return true;
    }

    bool SocialPresence::CompleteListening(ProgressionState& State)
    {
        if (Conversation.empty()) return false;
        Progression::RecordOverheard(State, Conversation);
        Conversation.clear();
        return true;
    }
}
