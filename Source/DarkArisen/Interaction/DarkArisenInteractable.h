// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DarkArisenInteractable.generated.h"

UENUM(BlueprintType)
enum class EInteractionClass : uint8
{
    Take,
    Use,
    Carry,
    Examine
};

UINTERFACE(BlueprintType)
class DARKARISEN_API UDarkArisenInteractable : public UInterface
{
    GENERATED_BODY()
};

/** Contract for physical, duration-bearing interaction. */
class DARKARISEN_API IDarkArisenInteractable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    bool CanInteract(AActor* Interactor) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    EInteractionClass GetInteractionClass() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    float GetInteractionDuration() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    FText GetInteractionLabel() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction|Examine")
    FText GetExamineTitle() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction|Examine")
    FText GetExamineBody() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction|Persistence")
    FName GetPersistentInteractionId() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction|Persistence")
    int32 CapturePersistentState() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction|Persistence")
    void RestorePersistentState(int32 StateBits);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void BeginInteraction(AActor* Interactor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void CompleteInteraction(AActor* Interactor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void CancelInteraction(AActor* Interactor);
};
