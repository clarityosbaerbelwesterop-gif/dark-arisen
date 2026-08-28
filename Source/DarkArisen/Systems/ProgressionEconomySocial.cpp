// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Systems/ProgressionEconomyComponent.h"

bool UProgressionEconomyComponent::RecordWeaponDrawnHere(
    const FName SocialContextId,
    const int32 RecoveryChapters)
{
    if (SocialContextId.IsNone() || RecoveryChapters <= 0)
    {
        return false;
    }

    const ESocialGreetingState Current = GetGreetingState(SocialContextId);
    if (Current != ESocialGreetingState::Wary)
    {
        GreetingBeforeWary.FindOrAdd(SocialContextId) = Current;
    }

    GreetingStates.FindOrAdd(SocialContextId) = ESocialGreetingState::Wary;
    int32& Remaining = WaryChaptersRemaining.FindOrAdd(SocialContextId);
    Remaining = FMath::Max(Remaining, RecoveryChapters);

    // Drawing or otherwise escalating ends passive social presence immediately.
    InterruptListening();
    EndSitting();
    return true;
}

void UProgressionEconomyComponent::AdvanceSocialChapter()
{
    for (auto It = WaryChaptersRemaining.CreateIterator(); It; ++It)
    {
        int32& Remaining = It.Value();
        --Remaining;
        if (Remaining > 0)
        {
            continue;
        }

        const FName ContextId = It.Key();
        const ESocialGreetingState* Previous = GreetingBeforeWary.Find(ContextId);
        GreetingStates.FindOrAdd(ContextId) = Previous
            ? *Previous
            : ESocialGreetingState::Known;
        GreetingBeforeWary.Remove(ContextId);
        It.RemoveCurrent();
    }
}

bool UProgressionEconomyComponent::BeginSitting(const FName SeatId)
{
    if (SeatId.IsNone() || !ActiveSeatId.IsNone())
    {
        return false;
    }

    ActiveSeatId = SeatId;
    return true;
}

void UProgressionEconomyComponent::EndSitting()
{
    ActiveSeatId = NAME_None;
}
