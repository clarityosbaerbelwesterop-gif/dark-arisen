// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UObject/SoftObjectPath.h"
#include "RuntimeIntegrationManifest.generated.h"

UENUM(BlueprintType)
enum class ERuntimeIntegrationKind : uint8
{
    Level,
    Navigation,
    InteractionAnchor,
    Animation,
    Audio,
    Population,
    Fauna,
    Dungeon,
    Boss,
    Ship,
    ColonialWar,
    Highmoore,
    Cutscene,
    ProtectedPlayableMoment
};

USTRUCT(BlueprintType)
struct FRuntimeIntegrationEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName StableId;

    /** Exact authored design/engineering source for this binding. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString GoverningSource;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ERuntimeIntegrationKind Kind = ERuntimeIntegrationKind::Level;

    /** Authored UE asset/level reference. Empty references fail closed. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FSoftObjectPath AssetPath;

    /** Must remain false: Blueprints/assets provide authored data and presentation, never gameplay authority. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bBlueprintOwnsGameplayLogic = false;

    /** True only for one of the canonical nineteen authored cutscenes. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bSequencerOwned = false;
};

/**
 * M9 authored-runtime integration contract.
 *
 * This C++ asset class validates authored UE references but does not claim that referenced content
 * exists, compiles, navigates, animates, performs, or has been observed in runtime. Those remain
 * external UE 5.5 acceptance facts.
 */
UCLASS(BlueprintType)
class DARKARISEN_API URuntimeIntegrationManifest : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="M9|Runtime")
    FName ManifestRevision;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="M9|Runtime")
    TArray<FRuntimeIntegrationEntry> Entries;

    bool ValidateDefinition(TArray<FString>& OutErrors) const;

    int32 CountKind(ERuntimeIntegrationKind Kind) const;

    static constexpr int32 RequiredCutsceneBindings = 19;
    static constexpr int32 RequiredProtectedPlayableMomentBindings = 22;
};
