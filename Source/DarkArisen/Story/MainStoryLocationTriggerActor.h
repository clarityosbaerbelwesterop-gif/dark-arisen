#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainStoryLocationTriggerActor.generated.h"
class UBoxComponent;
UENUM(BlueprintType)
enum class EMainStoryLocationAction : uint8 { ActivateMission, CompleteMission };
/** Thin physical-world adapter into UMainStorySubsystem. Owns no durable story state. */
UCLASS()
class DARKARISEN_API AMainStoryLocationTriggerActor : public AActor
{
    GENERATED_BODY()
public:
    AMainStoryLocationTriggerActor();
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UBoxComponent> Trigger;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName MissionId;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) EMainStoryLocationAction Action = EMainStoryLocationAction::ActivateMission;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName CheckpointId;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName SpawnId;
protected:
    virtual void BeginPlay() override;
private:
    UFUNCTION() void HandleOverlap(UPrimitiveComponent*,AActor*,UPrimitiveComponent*,int32,bool,const FHitResult&);
};
