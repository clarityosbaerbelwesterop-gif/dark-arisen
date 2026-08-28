// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Interaction/PhysicalJournalActor.h"

#include "Components/QuestJournalComponent.h"

APhysicalJournalActor::APhysicalJournalActor()
{
    DocumentTitle = NSLOCTEXT("DarkArisenJournal", "PhysicalTitle", "Jake's notebook");
    DocumentBody = NSLOCTEXT("DarkArisenJournal", "EmptyPages", "The pages are still blank.");
}

bool APhysicalJournalActor::CanInteract_Implementation(AActor* Interactor) const
{
    return Super::CanInteract_Implementation(Interactor) && IsValid(Interactor) &&
        Interactor->FindComponentByClass<UQuestJournalComponent>() != nullptr;
}

FText APhysicalJournalActor::GetInteractionLabel_Implementation() const
{
    return NSLOCTEXT("DarkArisenJournal", "OpenNotebook", "Open notebook");
}

FText APhysicalJournalActor::GetExamineBody_Implementation() const
{
    return BuildChronologicalPages();
}

void APhysicalJournalActor::BeginInteraction_Implementation(AActor* Interactor)
{
    JournalOwner = Interactor;
    Super::BeginInteraction_Implementation(Interactor);
}

void APhysicalJournalActor::CancelInteraction_Implementation(AActor* Interactor)
{
    Super::CancelInteraction_Implementation(Interactor);
    JournalOwner.Reset();
}

void APhysicalJournalActor::SetSearchQuery(const FString& Query)
{
    SearchQuery = Query.TrimStartAndEnd();
}

FText APhysicalJournalActor::BuildChronologicalPages() const
{
    const AActor* OwnerActor = JournalOwner.Get();
    const UQuestJournalComponent* Journal = OwnerActor
        ? OwnerActor->FindComponentByClass<UQuestJournalComponent>() : nullptr;
    if (!Journal) return DocumentBody;

    const TArray<FQuestJournalEntry> Entries = SearchQuery.IsEmpty()
        ? Journal->GetJournalEntries()
        : Journal->SearchJournal(SearchQuery);
    if (Entries.IsEmpty())
    {
        return SearchQuery.IsEmpty()
            ? NSLOCTEXT("DarkArisenJournal", "EmptyNotebook", "The pages are still blank.")
            : NSLOCTEXT("DarkArisenJournal", "NoSearchMatches", "Nothing in my notes matches those words.");
    }

    FString Pages;
    for (const FQuestJournalEntry& Entry : Entries)
    {
        const int64 Day = Entry.GameMinute / 1440 + 1;
        const int64 MinuteOfDay = Entry.GameMinute % 1440;
        const int64 Hour = MinuteOfDay / 60;
        const int64 Minute = MinuteOfDay % 60;
        if (!Pages.IsEmpty()) Pages.Append(TEXT("\n\n"));
        Pages.Append(FString::Printf(TEXT("Day %lld — %02lld:%02lld\n"), Day, Hour, Minute));
        Pages.Append(Entry.JakeText.ToString());
        if (!Entry.Directions.IsEmpty())
        {
            Pages.Append(TEXT("\n"));
            Pages.Append(Entry.Directions.ToString());
        }
    }
    return FText::FromString(Pages);
}
