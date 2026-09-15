#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NavalCombatComponent.generated.h"
UENUM(BlueprintType) enum class ENavalBroadside : uint8 { Port, Starboard };
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBroadsideFiredSignature,ENavalBroadside,Side,int32,CannonCount,float,Damage);
UCLASS(ClassGroup=(DarkArisen),meta=(BlueprintSpawnableComponent))
class DARKARISEN_API UNavalCombatComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UNavalCombatComponent();virtual void BeginPlay() override;virtual void TickComponent(float DeltaTime,ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
    UFUNCTION(BlueprintCallable,Category="Naval|Combat") bool FireBroadside(ENavalBroadside Side);
    UFUNCTION(BlueprintCallable,Category="Naval|Combat") void ApplyHullDamage(float Damage,AActor* DamageCauser);
    UFUNCTION(BlueprintPure,Category="Naval|Combat") bool IsSunk() const{return CurrentHull<=0.f;}
    UFUNCTION(BlueprintPure,Category="Naval|Combat") float GetHullFraction() const{return MaxHull>0.f?CurrentHull/MaxHull:0.f;}
    UFUNCTION(BlueprintPure,Category="Naval|Combat") float GetReloadRemaining(ENavalBroadside Side) const;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Naval|Combat") bool bPlayerAligned=false;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Naval|Combat",meta=(ClampMin="1")) int32 CannonsPerSide=8;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Naval|Combat",meta=(ClampMin="1.0")) float DamagePerCannon=24.f;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Naval|Combat",meta=(ClampMin="100.0")) float BroadsideRange=9000.f;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Naval|Combat",meta=(ClampMin="0.1")) float ReloadSeconds=7.5f;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Naval|Combat",meta=(ClampMin="1.0")) float MaxHull=1200.f;
    UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="Naval|Combat") float CurrentHull=1200.f;
    UPROPERTY(BlueprintAssignable,Category="Naval|Combat") FOnBroadsideFiredSignature OnBroadsideFired;
private:float PortReload=0.f;float StarboardReload=0.f;AActor* FindBroadsideTarget(ENavalBroadside Side) const;
};
