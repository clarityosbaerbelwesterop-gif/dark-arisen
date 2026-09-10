#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DamagePipeline.generated.h"
class UCombatComponent;
UENUM(BlueprintType) enum class EAttackType:uint8{Light,Heavy,ParryStrike,Critical,Projectile,Environmental};
USTRUCT(BlueprintType) struct FDamageContext
{
 GENERATED_BODY()
 UPROPERTY(BlueprintReadWrite) TObjectPtr<AActor> Source=nullptr;
 UPROPERTY(BlueprintReadWrite) TObjectPtr<AController> Instigator=nullptr;
 UPROPERTY(BlueprintReadWrite) TObjectPtr<AActor> Target=nullptr;
 UPROPERTY(BlueprintReadWrite) FName WeaponId;
 UPROPERTY(BlueprintReadWrite) EAttackType AttackType=EAttackType::Light;
 UPROPERTY(BlueprintReadWrite) float BaseDamage=0;
 UPROPERTY(BlueprintReadWrite) float PostureDamage=0;
 UPROPERTY(BlueprintReadWrite) FVector HitLocation=FVector::ZeroVector;
 UPROPERTY(BlueprintReadWrite) bool bCritical=false;
};
USTRUCT(BlueprintType) struct FDamageResult
{
 GENERATED_BODY()
 UPROPERTY(BlueprintReadOnly) float HealthDamage=0;
 UPROPERTY(BlueprintReadOnly) float PostureDamage=0;
 UPROPERTY(BlueprintReadOnly) bool bBlocked=false;
 UPROPERTY(BlueprintReadOnly) bool bDeflected=false;
 UPROPERTY(BlueprintReadOnly) bool bInvulnerable=false;
 UPROPERTY(BlueprintReadOnly) bool bStaggered=false;
 UPROPERTY(BlueprintReadOnly) bool bKilled=false;
};
UCLASS() class DARKARISEN_API UDamagePipeline:public UBlueprintFunctionLibrary
{
 GENERATED_BODY()
public:
 UFUNCTION(BlueprintCallable,Category="Combat|Damage") static bool ResolveDamage(const FDamageContext& Context,FDamageResult& OutResult);
};
