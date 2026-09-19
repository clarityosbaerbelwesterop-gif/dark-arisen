#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpeningWaterCurrentVolume.generated.h"
class UBoxComponent;
/** Physical current/swim adapter; reuses Jake movement + UWaterBreathComponent, owns no breath or story state. */
UCLASS() class DARKARISEN_API AOpeningWaterCurrentVolume : public AActor
{
    GENERATED_BODY()
public:
    AOpeningWaterCurrentVolume();
    virtual void Tick(float DeltaSeconds) override;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) TObjectPtr<UBoxComponent> Volume;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Opening|Water") FVector CurrentAcceleration=FVector(110.f,0.f,0.f);
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Opening|Water") float SurfaceOffsetCentimetres=40.f;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Opening|Water") bool bSignalsWaterEntry=false;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Opening|Water") bool bSignalsOuterReef=false;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Opening|Water") bool bShallowExit=false;
private:
    UFUNCTION() void HandleBegin(UPrimitiveComponent*,AActor*,UPrimitiveComponent*,int32,bool,const FHitResult&);
    UFUNCTION() void HandleEnd(UPrimitiveComponent*,AActor*,UPrimitiveComponent*,int32);
};
