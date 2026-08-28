// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Interaction/InteractionPersistence.h"

#include "EngineUtils.h"
#include "Engine/World.h"
#include "Interaction/DarkArisenInteractable.h"

UInteractionStateSnapshot* UInteractionPersistenceLibrary::CaptureInteractionState(
    UObject* WorldContextObject)
{
    UInteractionStateSnapshot* Snapshot = NewObject<UInteractionStateSnapshot>();
    UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
    if (!World) return Snapshot;

    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* Actor = *It;
        if (!Actor->GetClass()->ImplementsInterface(UDarkArisenInteractable::StaticClass()))
            continue;
        const FName PersistentId =
            IDarkArisenInteractable::Execute_GetPersistentInteractionId(Actor);
        if (PersistentId.IsNone()) continue;
        Snapshot->StateByPersistentId.Add(
            PersistentId,
            IDarkArisenInteractable::Execute_CapturePersistentState(Actor));
    }
    return Snapshot;
}

void UInteractionPersistenceLibrary::RestoreInteractionState(
    UObject* WorldContextObject,
    const UInteractionStateSnapshot* Snapshot)
{
    UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
    if (!World || !Snapshot) return;

    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* Actor = *It;
        if (!Actor->GetClass()->ImplementsInterface(UDarkArisenInteractable::StaticClass()))
            continue;
        const FName PersistentId =
            IDarkArisenInteractable::Execute_GetPersistentInteractionId(Actor);
        const int32* StateBits = Snapshot->StateByPersistentId.Find(PersistentId);
        if (StateBits)
            IDarkArisenInteractable::Execute_RestorePersistentState(Actor, *StateBits);
    }
}
