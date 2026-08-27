// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InteractionPersistence.generated.h"

/** Serializable interaction state; disk writes remain owned by the chapter/rest save system. */
UCLASS()
class DARKARISEN_API UInteractionStateSnapshot : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, SaveGame, Category = "Persistence")
    TMap<FName, int32> StateByPersistentId;
};

UCLASS()
class DARKARISEN_API UInteractionPersistenceLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Interaction|Persistence", meta = (WorldContext = "WorldContextObject"))
    static UInteractionStateSnapshot* CaptureInteractionState(UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable, Category = "Interaction|Persistence", meta = (WorldContext = "WorldContextObject"))
    static void RestoreInteractionState(
        UObject* WorldContextObject,
        const UInteractionStateSnapshot* Snapshot);
};
