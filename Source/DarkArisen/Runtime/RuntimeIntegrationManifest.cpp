// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Runtime/RuntimeIntegrationManifest.h"

bool URuntimeIntegrationManifest::ValidateDefinition(TArray<FString>& OutErrors) const
{
    if (ManifestRevision.IsNone())
    {
        OutErrors.Add(TEXT("M9 runtime integration manifest requires a stable revision."));
    }

    TSet<FName> SeenIds;
    for (const FRuntimeIntegrationEntry& Entry : Entries)
    {
        if (Entry.StableId.IsNone())
        {
            OutErrors.Add(TEXT("M9 runtime integration entry has no stable ID."));
            continue;
        }
        if (SeenIds.Contains(Entry.StableId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate M9 runtime integration ID: %s"), *Entry.StableId.ToString()));
        }
        SeenIds.Add(Entry.StableId);

        if (Entry.GoverningSource.TrimStartAndEnd().IsEmpty())
        {
            OutErrors.Add(FString::Printf(TEXT("M9 runtime integration entry %s has no governing source."), *Entry.StableId.ToString()));
        }
        if (!Entry.AssetPath.IsValid())
        {
            OutErrors.Add(FString::Printf(TEXT("M9 runtime integration entry %s has no valid authored asset path."), *Entry.StableId.ToString()));
        }
        if (Entry.bBlueprintOwnsGameplayLogic)
        {
            OutErrors.Add(FString::Printf(TEXT("M9 runtime integration entry %s attempts to move gameplay authority into Blueprint/content."), *Entry.StableId.ToString()));
        }

        const bool bIsCutscene = Entry.Kind == ERuntimeIntegrationKind::Cutscene;
        const bool bIsProtectedPlayable = Entry.Kind == ERuntimeIntegrationKind::ProtectedPlayableMoment;
        if (Entry.bSequencerOwned && !bIsCutscene)
        {
            OutErrors.Add(FString::Printf(TEXT("Only canonical cutscenes may be Sequencer-owned: %s"), *Entry.StableId.ToString()));
        }
        if (bIsProtectedPlayable && Entry.bSequencerOwned)
        {
            OutErrors.Add(FString::Printf(TEXT("Protected playable moment may not become a cutscene: %s"), *Entry.StableId.ToString()));
        }
    }

    const TArray<ERuntimeIntegrationKind> RequiredRuntimeKinds = {
        ERuntimeIntegrationKind::Level,
        ERuntimeIntegrationKind::Navigation,
        ERuntimeIntegrationKind::InteractionAnchor,
        ERuntimeIntegrationKind::Animation,
        ERuntimeIntegrationKind::Audio,
        ERuntimeIntegrationKind::Population,
        ERuntimeIntegrationKind::Fauna,
        ERuntimeIntegrationKind::Dungeon,
        ERuntimeIntegrationKind::Boss,
        ERuntimeIntegrationKind::Ship,
        ERuntimeIntegrationKind::ColonialWar,
        ERuntimeIntegrationKind::Highmoore
    };
    for (const ERuntimeIntegrationKind Kind : RequiredRuntimeKinds)
    {
        if (CountKind(Kind) <= 0)
        {
            OutErrors.Add(FString::Printf(
                TEXT("M9 authored runtime category has no integration binding: %d"),
                static_cast<int32>(Kind)));
        }
    }

    if (CountKind(ERuntimeIntegrationKind::Cutscene) != RequiredCutsceneBindings)
    {
        OutErrors.Add(FString::Printf(TEXT("M9 requires exactly %d authored cutscene bindings."), RequiredCutsceneBindings));
    }
    if (CountKind(ERuntimeIntegrationKind::ProtectedPlayableMoment) != RequiredProtectedPlayableMomentBindings)
    {
        OutErrors.Add(FString::Printf(TEXT("M9 requires exactly %d protected playable-moment bindings."), RequiredProtectedPlayableMomentBindings));
    }

    return OutErrors.IsEmpty();
}

int32 URuntimeIntegrationManifest::CountKind(const ERuntimeIntegrationKind Kind) const
{
    int32 Count = 0;
    for (const FRuntimeIntegrationEntry& Entry : Entries)
    {
        if (Entry.Kind == Kind)
        {
            ++Count;
        }
    }
    return Count;
}
