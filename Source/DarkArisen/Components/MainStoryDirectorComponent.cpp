// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Components/MainStoryDirectorComponent.h"

#include "Missions/MainStoryMissionCatalog.h"

UMainStoryDirectorComponent::UMainStoryDirectorComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

FName UMainStoryDirectorComponent::GetCurrentMissionId() const
{
    const TArray<FMainStoryMissionDefinition> Missions = UMainStoryMissionCatalog::GetAuthoredMissions();
    if (!UMainStoryMissionCatalog::IsCatalogValid(Missions) ||
        CurrentMissionIndex < 0 || CurrentMissionIndex >= Missions.Num())
    {
        return NAME_None;
    }
    return Missions[CurrentMissionIndex].Activation.QuestId;
}

int32 UMainStoryDirectorComponent::GetCurrentChapter() const
{
    const TArray<FMainStoryMissionDefinition> Missions = UMainStoryMissionCatalog::GetAuthoredMissions();
    if (!UMainStoryMissionCatalog::IsCatalogValid(Missions)) return 0;
    if (CurrentMissionIndex == Missions.Num()) return 10;
    if (CurrentMissionIndex < 0 || CurrentMissionIndex >= Missions.Num()) return 0;
    return Missions[CurrentMissionIndex].Chapter;
}

bool UMainStoryDirectorComponent::IsStoryComplete() const
{
    const TArray<FMainStoryMissionDefinition> Missions = UMainStoryMissionCatalog::GetAuthoredMissions();
    return UMainStoryMissionCatalog::IsCatalogValid(Missions) &&
        CurrentMissionIndex == Missions.Num() && StoryFacts.Contains(TEXT("Story.MainComplete"));
}

bool UMainStoryDirectorComponent::IsStoryFactSet(const FName FactId) const
{
    return !FactId.IsNone() && StoryFacts.Contains(FactId);
}

bool UMainStoryDirectorComponent::HasValidStoryState() const
{
    const TArray<FMainStoryMissionDefinition> Missions = UMainStoryMissionCatalog::GetAuthoredMissions();
    if (!UMainStoryMissionCatalog::IsCatalogValid(Missions) ||
        CurrentMissionIndex < 0 || CurrentMissionIndex > Missions.Num())
    {
        return false;
    }

    const bool bEthanAbducted = StoryFacts.Contains(TEXT("Story.EthanAbducted"));
    const bool bEthanRecovered = StoryFacts.Contains(TEXT("Story.EthanRecovered"));
    const bool bMainComplete = StoryFacts.Contains(TEXT("Story.MainComplete"));

    // Milestone facts must exist iff the ordered critical path has passed their owning mission.
    if ((CurrentMissionIndex >= 3) != bEthanAbducted) return false;
    if ((CurrentMissionIndex >= 25) != bEthanRecovered) return false;
    if ((CurrentMissionIndex >= 34) != bMainComplete) return false;
    return true;
}

bool UMainStoryDirectorComponent::TryActivateCurrentMission(
    UQuestJournalComponent* Journal,
    const EQuestActivationTrigger Trigger,
    const int64 GameMinute,
    const bool bSpokenAgreementConfirmed)
{
    if (!Journal || !HasValidStoryState()) return false;
    const TArray<FMainStoryMissionDefinition> Missions = UMainStoryMissionCatalog::GetAuthoredMissions();
    if (CurrentMissionIndex >= Missions.Num()) return false;

    const FMainStoryMissionDefinition& Mission = Missions[CurrentMissionIndex];
    if (Mission.Activation.Trigger != Trigger) return false;
    return Journal->ActivateQuest(
        Mission.Activation.QuestId, Trigger, GameMinute, bSpokenAgreementConfirmed);
}

bool UMainStoryDirectorComponent::TryResolveCurrentMission(
    UQuestJournalComponent* Journal,
    const FName OutcomeId,
    const int64 GameMinute)
{
    if (!Journal || OutcomeId.IsNone() || !HasValidStoryState()) return false;
    const TArray<FMainStoryMissionDefinition> Missions = UMainStoryMissionCatalog::GetAuthoredMissions();
    if (CurrentMissionIndex >= Missions.Num()) return false;

    const FMainStoryMissionDefinition& Mission = Missions[CurrentMissionIndex];
    if (!Mission.AllowedOutcomeIds.Contains(OutcomeId) ||
        !Journal->ResolveQuest(Mission.Activation.QuestId, OutcomeId, GameMinute))
    {
        return false;
    }

    for (const FName FactId : Mission.PersistenceFlags)
    {
        if (!FactId.IsNone()) StoryFacts.Add(FactId);
    }
    StoryFacts.Add(OutcomeId);
    ++CurrentMissionIndex;
    return HasValidStoryState();
}
