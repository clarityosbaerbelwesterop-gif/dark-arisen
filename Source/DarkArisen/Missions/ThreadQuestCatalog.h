#pragma once
#include "CoreMinimal.h"
#include "Components/QuestJournalComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ThreadQuestCatalog.generated.h"

/** Exact seventeen finite Thread identities. Step-level gaps remain source-gated; nothing radiant is generated. */
UCLASS()
class DARKARISEN_API UThreadQuestCatalog : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure,Category="Quest|Threads")
    static TArray<FQuestActivationDefinition> GetThreadDefinitions();
    UFUNCTION(BlueprintCallable,Category="Quest|Threads")
    static bool RegisterThreads(UQuestJournalComponent* Journal);
};
