#pragma once

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace DarkArisen::Core
{
    struct CharacterProgression;

    /** Ordinals match EDarkArisenCurrency and are persisted. No conversion between them exists. */
    enum class Currency : std::uint8_t { Doubloons, Pounds, SilverMarks };

    /** Ordinals match ESocialGreetingState. Four states; no affinity number, no relationship screen. */
    enum class GreetingState : std::uint8_t { Stranger, Known, Owed, Wary };

    enum class SkillGateKind : std::uint8_t { None, Standing, WorldState };

    /** One CRAFT node (FSkillNodeDefinition). Content, never persisted. */
    struct SkillNode
    {
        std::string NodeId;
        std::string BranchId;
        int MarkCost = 1;
        /** Any one of these teachers satisfies the authored teaching-scene gate. Empty: none. */
        std::vector<std::string> TeacherOptions;
        /** At most three authored prerequisites. */
        std::vector<std::string> PrerequisiteNodeIds;
        SkillGateKind GateKind = SkillGateKind::None;
        /** Hidden Standing/world-state gate; never shown as a meter. */
        std::string RequiredGateId;
        std::string ExclusiveGroup;
        int MinimumPosture = 0;
    };

    struct CurrencyWallet
    {
        std::int64_t Doubloons = 0;
        std::int64_t Pounds = 0;
        std::int64_t SilverMarks = 0;
        bool operator==(const CurrencyWallet&) const = default;
    };

    /** Doubloon ledger that resolves once per chapter (FChapterLedger). Income >= 0 or not; upkeep <= 0. */
    struct ChapterLedger
    {
        std::int64_t HoldingIncome = 0;
        std::int64_t ArmyUpkeep = 0;
        std::int64_t GarrisonUpkeep = 0;
        std::int64_t ConstructionDraw = 0;
        std::int64_t NetDoubloons() const { return HoldingIncome + ArmyUpkeep + GarrisonUpkeep + ConstructionDraw; }
        bool operator==(const ChapterLedger&) const = default;
    };

    /**
     * Persistent BODY/CRAFT/STANDING progression, money and social memory: port of
     * FProgressionEconomySnapshot plus the chapter ledger (UChapterEconomyComponent, which the Unreal
     * reference never saved).
     */
    struct ProgressionState
    {
        int MaximumHealth = 200;
        int MaximumStamina = 120;
        int MaximumPosture = 100;
        double CarryKilograms = 80.0;
        std::set<std::string, std::less<>> PhysiciansDraughtSources;
        std::set<std::string, std::less<>> DeepWaterPearlSources;
        std::set<std::string, std::less<>> NamedDeflectionSources;
        std::set<std::string, std::less<>> CarryMilestones;
        int MarksEarned = 0;
        int MarksSpent = 0;
        std::set<std::string, std::less<>> AwardedMarkSources;
        std::set<std::string, std::less<>> TeachersMet;
        std::set<std::string, std::less<>> CompletedTeachingScenes;
        std::set<std::string, std::less<>> WorldFlags;
        std::set<std::string, std::less<>> LearnedNodes;
        std::set<std::string, std::less<>> ChosenExclusiveGroups;
        CurrencyWallet Wallet;
        std::map<std::string, GreetingState, std::less<>> Greetings;
        std::map<std::string, GreetingState, std::less<>> GreetingBeforeWary;
        std::map<std::string, int, std::less<>> WaryChaptersRemaining;
        std::set<std::string, std::less<>> OverheardConversations;
        ChapterLedger PendingLedger;
        int LastResolvedLedgerChapter = 0;
        std::set<std::string, std::less<>> PaidLegendaryWork;
        std::set<std::string, std::less<>> PaidReconstructionProjects;
        std::set<int> PaidCrewShareChapters;
        std::set<std::string, std::less<>> PurchasedServices;
        bool operator==(const ProgressionState&) const = default;
    };

    /** The CRAFT catalog: the nodes the bible names, plus authored data, under structural invariants. */
    class SkillCatalog
    {
    public:
        static constexpr int RequiredSkillNodeCount = 68;
        static constexpr int RequiredTeacherCount = 23;
        static constexpr int RequiredTeacherGatedNodeCount = 23;
        static constexpr int RequiredStandingGatedNodeCount = 11;
        static constexpr int MaximumAvailableMarks = 94;
        static constexpr int FullTreeMarkCost = 141;

        /** The 19 nodes the design bible names (UProgressionEconomyComponent::RegisterKnownCanonicalNodes). */
        static SkillCatalog Canonical();
        static const std::vector<std::string_view>& TeacherIds();

        bool Register(const SkillNode& Node);
        const SkillNode* Find(std::string_view NodeId) const;
        std::size_t Size() const { return Nodes.size(); }
        /** 68 nodes, branch split 16/12/14/13/13, cost 141, 23 teacher-gated (all 23 teachers), 11 Standing-gated. */
        bool IsComplete() const;

    private:
        std::map<std::string, SkillNode, std::less<>> Nodes;
    };

    /** A priced service at a trader or workshop (repair, provisioning). Content, never persisted. */
    struct ServiceOffer
    {
        std::string ServiceId;
        Currency PriceCurrency = Currency::Doubloons;
        std::int64_t Price = 0;
        /** Items consumed from the inventory (repeat an id for quantity). */
        std::vector<std::string> ConsumedItems;
        std::vector<std::string> GrantedItems;
        /** One-time work (a repair, a commission) cannot be bought twice. */
        bool OneTime = false;
    };

    /**
     * Port of UProgressionEconomyComponent, UChapterEconomyComponent and the social rules. Locked laws:
     * BODY grows only from authored objects/acts; CRAFT uses Marks plus completed teaching scenes and
     * money never unlocks nodes; no respec; no currency conversion; four greeting states; drawing a
     * weapon is the only hostile social input; sitting never fast-forwards time.
     */
    namespace Progression
    {
        bool ApplyPhysiciansDraught(ProgressionState& State, std::string_view SourceId);
        bool ApplyDeepWaterPearl(ProgressionState& State, std::string_view SourceId);
        bool RecordNamedDeflectionSet(ProgressionState& State, std::string_view OpponentId);
        bool RecordCarryMilestone(ProgressionState& State, std::string_view MilestoneId, double AuthoredCarryKg);
        bool AwardMarks(ProgressionState& State, std::string_view AuthoredSourceId, int Amount);
        /** Meeting a teacher is not teaching; it only makes an authored teaching scene possible. */
        bool RecordTeacherMet(ProgressionState& State, std::string_view TeacherId);
        bool CompleteTeachingScene(ProgressionState& State, const SkillCatalog& Catalog, std::string_view TeacherId, std::string_view NodeId);
        bool SetWorldFlag(ProgressionState& State, std::string_view FlagId, bool Enabled);
        bool CanLearn(const ProgressionState& State, const SkillCatalog& Catalog, std::string_view NodeId);
        bool TryLearnNode(ProgressionState& State, const SkillCatalog& Catalog, std::string_view NodeId);

        bool Credit(ProgressionState& State, Currency Kind, std::int64_t Amount);
        bool Spend(ProgressionState& State, Currency Kind, std::int64_t Amount);
        std::int64_t Balance(const ProgressionState& State, Currency Kind);
        /** Pays and exchanges items atomically: nothing changes unless everything is available. */
        bool PurchaseService(ProgressionState& State, CharacterProgression& Character, const ServiceOffer& Offer);

        bool SetPendingLedger(ProgressionState& State, const ChapterLedger& Ledger);
        bool ResolveChapterLedger(ProgressionState& State, int Chapter);
        bool PayLegendaryTierCost(ProgressionState& State, Currency Kind, std::int64_t Amount, std::string_view WorkId);
        bool PayReconstructionSilver(ProgressionState& State, std::int64_t SilverMarks, std::string_view ProjectId);
        bool PayCrewShare(ProgressionState& State, std::int64_t Doubloons, int Chapter);

        void SetGreeting(ProgressionState& State, std::string_view SocialContextId, GreetingState NewState);
        GreetingState Greeting(const ProgressionState& State, std::string_view SocialContextId);
        /** Drawing a weapon here: Wary for the authored number of chapters, then the previous greeting. */
        bool RecordWeaponDrawnHere(ProgressionState& State, std::string_view SocialContextId, int RecoveryChapters);
        /** Chapter progression only. */
        void AdvanceSocialChapter(ProgressionState& State);
        bool RecordOverheard(ProgressionState& State, std::string_view ConversationId);

        bool Validate(const ProgressionState& State, std::vector<std::string>& OutErrors);
    }

    /**
     * Transient social presence (sitting at a seat, listening to a conversation). Never saved; a weapon
     * drawn ends both at once.
     */
    class SocialPresence
    {
    public:
        bool BeginSitting(std::string_view SeatId);
        void EndSitting() { Seat.clear(); }
        bool IsSitting() const { return !Seat.empty(); }
        bool BeginListening(std::string_view ConversationId);
        void InterruptListening() { Conversation.clear(); }
        /** Completing records the conversation as overheard. */
        bool CompleteListening(ProgressionState& State);
        void OnWeaponDrawn()
        {
            InterruptListening();
            EndSitting();
        }

    private:
        std::string Seat;
        std::string Conversation;
    };
}
