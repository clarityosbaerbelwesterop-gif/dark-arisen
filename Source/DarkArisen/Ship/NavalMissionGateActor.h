#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavalMissionGateActor.generated.h"
class UBoxComponent;
/** Completes an authored naval mission only after the configured hostile fleet is sunk. */
UCLASS()
class DARKARISEN_API ANavalMissionGateActor : public AActor
{
    GENERATED_BODY()
public:
    ANavalMissionGateActor();
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) TObjectPtr<UBoxComponent> Trigger;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Story") FName MissionId;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Story") FName CheckpointId;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Story") FName SpawnId;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Naval",meta=(ClampMin="1")) int32 RequiredSunkShips=1;
protected:
    virtual void BeginPlay() override;
private:
    UFUNCTION() void HandleOverlap(UPrimitiveComponent* Overlapped,AActor* Other,UPrimitiveComponent* OtherComp,int32 BodyIndex,bool bFromSweep,const FHitResult& Sweep);
};
