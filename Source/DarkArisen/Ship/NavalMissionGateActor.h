#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavalMissionGateActor.generated.h"
/** Completes an authored naval mission only after fleet destruction and La Liberacion reaches the exit lane. */
UCLASS()
class DARKARISEN_API ANavalMissionGateActor : public AActor
{
    GENERATED_BODY()
public:
    ANavalMissionGateActor();virtual void Tick(float DeltaSeconds) override;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Story") FName MissionId;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Story") FName CheckpointId;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Story") FName SpawnId;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Naval",meta=(ClampMin="1")) int32 RequiredSunkShips=1;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Naval",meta=(ClampMin="100")) float CompletionRadius=1800.f;
private:
    bool bResolved=false;
};
