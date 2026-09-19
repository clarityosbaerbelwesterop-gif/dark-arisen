#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Persistence/DarkArisenSaveGame.h"
#include "RacheStoryUnlockComponent.generated.h"
/** Bridges canonical story progression into Jake's native combat capability without creating parallel progression state. */
UCLASS(ClassGroup=(DarkArisen),meta=(BlueprintSpawnableComponent))
class DARKARISEN_API URacheStoryUnlockComponent:public UActorComponent
{
    GENERATED_BODY()
public: URacheStoryUnlockComponent();
protected: virtual void BeginPlay() override;
private:
    void RefreshUnlock();
    UFUNCTION() void HandleMissionChanged(FName MissionId,EMainMissionState State);
    UFUNCTION() void HandleWorldFactChanged(FName Fact,bool bEnabled);
};
