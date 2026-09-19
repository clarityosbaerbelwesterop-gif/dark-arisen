#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DarkArisenWorldDirector.generated.h"
class UMainStoryDirectorComponent;
class UOpeningRuntimeComponent;
/** Single world-facing orchestration actor. Durable story/save authority remains in UMainStorySubsystem. */
UCLASS(NotBlueprintable)
class DARKARISEN_API ADarkArisenWorldDirector : public AActor
{
    GENERATED_BODY()
public:
    ADarkArisenWorldDirector();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dark Arisen|Story")
    TObjectPtr<UMainStoryDirectorComponent> MainStoryDirector;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dark Arisen|Opening")
    TObjectPtr<UOpeningRuntimeComponent> OpeningRuntime;
    /** Returns the unique director; duplicate directors fail closed. */
    UFUNCTION(BlueprintPure, Category="Dark Arisen|World", meta=(WorldContext="WorldContextObject"))
    static ADarkArisenWorldDirector* Resolve(const UObject* WorldContextObject);
};
