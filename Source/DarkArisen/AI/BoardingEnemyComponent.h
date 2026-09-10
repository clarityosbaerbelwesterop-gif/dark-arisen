#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoardingEnemyComponent.generated.h"
UENUM(BlueprintType) enum class EBoardingEnemyState:uint8{Idle,Detect,Approach,Combat,Attack,Reposition,Recover,Search,Dead};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBoarderStateChanged,EBoardingEnemyState,OldState,EBoardingEnemyState,NewState);
/** Native decision authority for the first boarder; perception/animation remain replaceable adapters. */
UCLASS(ClassGroup=(DarkArisen),meta=(BlueprintSpawnableComponent)) class DARKARISEN_API UBoardingEnemyComponent:public UActorComponent
{
 GENERATED_BODY()
public:
 UBoardingEnemyComponent();virtual void BeginPlay()override;virtual void TickComponent(float DeltaTime,ELevelTick TickType,FActorComponentTickFunction* Fn)override;
 UFUNCTION(BlueprintCallable) void ReportTargetSeen(AActor* Target);
 UFUNCTION(BlueprintCallable) void ReportTargetLost();
 UFUNCTION(BlueprintCallable) void ReportAttackResolved();
 UFUNCTION(BlueprintPure) EBoardingEnemyState GetState()const{return State;}
 UPROPERTY(BlueprintAssignable) FBoarderStateChanged OnStateChanged;
 UPROPERTY(EditDefaultsOnly) float AttackRange=180.f;UPROPERTY(EditDefaultsOnly) float LoseTargetSeconds=4.f;UPROPERTY(EditDefaultsOnly) float RecoverySeconds=.8f;
private:
 UPROPERTY() TObjectPtr<AActor> CombatTarget;UPROPERTY() EBoardingEnemyState State=EBoardingEnemyState::Idle;float StateTime=0;void Transition(EBoardingEnemyState NewState);
 UFUNCTION() void HandleOwnerDeath(AActor* Causer);
};
