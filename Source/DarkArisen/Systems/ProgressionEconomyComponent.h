// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProgressionEconomyComponent.generated.h"

UENUM(BlueprintType)
enum class EDarkArisenCurrency : uint8
{
    Doubloons,
    Pounds,
    SilverMarks
};

UENUM(BlueprintType)
enum class ESocialGreetingState : uint8
{
    Stranger,
    Known,
    Owed,
    Wary
};

USTRUCT(BlueprintType)
struct FSkillNodeDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName NodeId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName BranchId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1"))
    int32 MarkCost = 1;

    /** Any one of these teachers may satisfy the authored teaching gate. Empty means none. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FName> TeacherOptions;

    /** Optional hidden world/Standing gate. It is never surfaced as a numeric meter. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName RequiredWorldFlag;

    /** Optional mutually exclusive authored choice group. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ExclusiveGroup;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
    int32 MinimumPosture = 0;
};

USTRUCT(BlueprintType)
struct FCurrencyWallet
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadOnly)
    int64 Doubloons = 0;

    UPROPERTY(SaveGame, BlueprintReadOnly)
    int64 Pounds = 0;

    UPROPERTY(SaveGame, BlueprintReadOnly)
    int64 SilverMarks = 0;
};

/**
 * M4 source authority for the three non-convertible progression axes plus money/social state.
 *
 * Locked laws enforced here:
 * - BODY grows only from authored objects/acts;
 * - CRAFT uses Marks and authored teachers; money cannot unlock nodes;
 * - STANDING is hidden world state, never a numeric player-facing meter;
 * - 68-node / 23-teacher catalog sizes are acceptance invariants;
 * - no respec path exists;
 * - the three currencies have no generic conversion API;
 * - social state is four greeting states with no affinity number or relationship screen.
 *
 * The bible names only part of the 68 nodes individually. Missing nodes are intentionally not
 * invented: the catalog remains fail-closed until authored definitions bring it to exactly 68.
 */
UCLASS(ClassGroup=(DarkArisen), meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UProgressionEconomyComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UProgressionEconomyComponent();
    virtual void BeginPlay() override;

    static constexpr int32 RequiredSkillNodeCount = 68;
    static constexpr int32 RequiredTeacherCount = 23;
    static constexpr int32 MaximumAvailableMarks = 94;
    static constexpr int32 FullTreeMarkCost = 141;

    UFUNCTION(BlueprintCallable, Category="Progression|Body")
    bool ApplyPhysiciansDraught(FName SourceId);

    UFUNCTION(BlueprintCallable, Category="Progression|Body")
    bool ApplyDeepWaterPearl(FName SourceId);

    UFUNCTION(BlueprintCallable, Category="Progression|Body")
    bool RecordNamedDeflectionSet(FName OpponentId);

    UFUNCTION(BlueprintCallable, Category="Progression|Body")
    bool RecordCarryMilestone(FName MilestoneId, float AuthoredCarryKg);

    UFUNCTION(BlueprintCallable, Category="Progression|Craft")
    bool AwardMarks(FName AuthoredSourceId, int32 Amount);

    UFUNCTION(BlueprintCallable, Category="Progression|Craft")
    void RecordTeacherMet(FName TeacherId);

    UFUNCTION(BlueprintCallable, Category="Progression|Standing")
    void SetWorldFlag(FName FlagId, bool bEnabled);

    UFUNCTION(BlueprintCallable, Category="Progression|Craft")
    bool RegisterAuthoredNode(const FSkillNodeDefinition& Definition);

    UFUNCTION(BlueprintCallable, Category="Progression|Craft")
    bool TryLearnNode(FName NodeId);

    UFUNCTION(BlueprintPure, Category="Progression|Craft")
    bool HasLearnedNode(FName NodeId) const { return LearnedNodes.Contains(NodeId); }

    UFUNCTION(BlueprintPure, Category="Progression|Craft")
    bool IsSkillCatalogComplete() const { return SkillNodeDefinitions.Num() == RequiredSkillNodeCount; }

    UFUNCTION(BlueprintPure, Category="Progression|Craft")
    int32 GetRegisteredSkillNodeCount() const { return SkillNodeDefinitions.Num(); }

    /** C++ validation/catalog authority only; intentionally not a Blueprint/UI list. */
    static const TArray<FName>& GetCanonicalTeacherIds();

    UFUNCTION(BlueprintCallable, Category="Economy")
    bool CreditCurrency(EDarkArisenCurrency Currency, int64 Amount);

    UFUNCTION(BlueprintCallable, Category="Economy")
    bool SpendCurrency(EDarkArisenCurrency Currency, int64 Amount);

    UFUNCTION(BlueprintPure, Category="Economy")
    FCurrencyWallet GetWallet() const { return Wallet; }

    UFUNCTION(BlueprintCallable, Category="Social")
    void SetGreetingState(FName SocialContextId, ESocialGreetingState NewState);

    UFUNCTION(BlueprintPure, Category="Social")
    ESocialGreetingState GetGreetingState(FName SocialContextId) const;

    UFUNCTION(BlueprintCallable, Category="Social|Listening")
    bool BeginListening(FName ConversationId);

    UFUNCTION(BlueprintCallable, Category="Social|Listening")
    void InterruptListening();

    UFUNCTION(BlueprintCallable, Category="Social|Listening")
    bool CompleteListening();

    UFUNCTION(BlueprintPure, Category="Social|Listening")
    bool HasOverheard(FName ConversationId) const { return OverheardConversations.Contains(ConversationId); }

    UFUNCTION(BlueprintPure, Category="Progression|Body")
    int32 GetMaximumHealth() const { return MaximumHealth; }

    UFUNCTION(BlueprintPure, Category="Progression|Body")
    int32 GetMaximumStamina() const { return MaximumStamina; }

    UFUNCTION(BlueprintPure, Category="Progression|Body")
    int32 GetMaximumPosture() const { return MaximumPosture; }

    UFUNCTION(BlueprintPure, Category="Progression|Body")
    float GetCarryKilograms() const { return CarryKilograms; }

private:
    void RegisterKnownCanonicalNodes();
    bool DefinitionCanBeLearned(const FSkillNodeDefinition& Definition) const;
    int64& ResolveCurrencyMutable(EDarkArisenCurrency Currency);

    UPROPERTY(SaveGame)
    int32 MaximumHealth = 200;

    UPROPERTY(SaveGame)
    int32 MaximumStamina = 120;

    UPROPERTY(SaveGame)
    int32 MaximumPosture = 100;

    UPROPERTY(SaveGame)
    float CarryKilograms = 80.0f;

    UPROPERTY(SaveGame)
    TSet<FName> PhysiciansDraughtSources;

    UPROPERTY(SaveGame)
    TSet<FName> DeepWaterPearlSources;

    UPROPERTY(SaveGame)
    TSet<FName> NamedDeflectionSources;

    UPROPERTY(SaveGame)
    TSet<FName> CarryMilestones;

    /** Internal by design: there is no Blueprint getter for Marks totals. */
    UPROPERTY(SaveGame)
    int32 MarksEarned = 0;

    UPROPERTY(SaveGame)
    int32 MarksSpent = 0;

    UPROPERTY(SaveGame)
    TSet<FName> AwardedMarkSources;

    UPROPERTY(SaveGame)
    TSet<FName> TeachersMet;

    UPROPERTY(SaveGame)
    TSet<FName> WorldFlags;

    UPROPERTY(SaveGame)
    TSet<FName> LearnedNodes;

    UPROPERTY(SaveGame)
    TSet<FName> ChosenExclusiveGroups;

    UPROPERTY(Transient)
    TMap<FName, FSkillNodeDefinition> SkillNodeDefinitions;

    UPROPERTY(SaveGame)
    FCurrencyWallet Wallet;

    UPROPERTY(SaveGame)
    TMap<FName, ESocialGreetingState> GreetingStates;

    UPROPERTY(SaveGame)
    TSet<FName> OverheardConversations;

    UPROPERTY(Transient)
    FName ActiveListeningConversation;
};
