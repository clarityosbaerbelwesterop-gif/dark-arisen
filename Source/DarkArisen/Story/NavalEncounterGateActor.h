#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavalEncounterGateActor.generated.h"
UCLASS() class DARKARISEN_API ANavalEncounterGateActor:public AActor
{
    GENERATED_BODY()
public:ANavalEncounterGateActor();virtual void Tick(float DeltaSeconds) override;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Story") FName MissionId;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Story") int32 RequiredDefeats=1;
private:bool bCompleted=false;
};
