#include "Ship/NavalCombatComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"

UNavalCombatComponent::UNavalCombatComponent(){PrimaryComponentTick.bCanEverTick=true;}
void UNavalCombatComponent::BeginPlay(){Super::BeginPlay();CurrentHull=MaxHull;}
void UNavalCombatComponent::TickComponent(float DeltaTime,ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime,TickType,ThisTickFunction);
    PortReload=FMath::Max(0.f,PortReload-DeltaTime); StarboardReload=FMath::Max(0.f,StarboardReload-DeltaTime);
    SetComponentTickEnabled(PortReload>0.f||StarboardReload>0.f);
}
float UNavalCombatComponent::GetReloadRemaining(ENavalBroadside Side) const{return Side==ENavalBroadside::Port?PortReload:StarboardReload;}
AActor* UNavalCombatComponent::FindBroadsideTarget(ENavalBroadside Side) const
{
    const AActor* Owner=GetOwner(); UWorld* World=GetWorld(); if(!Owner||!World)return nullptr;
    const FVector Origin=Owner->GetActorLocation(); const FVector Right=Owner->GetActorRightVector();
    AActor* Best=nullptr; float BestDistSq=BroadsideRange*BroadsideRange;
    for(TActorIterator<AActor> It(World);It;++It)
    {
        AActor* Candidate=*It; if(!Candidate||Candidate==Owner)continue;
        UNavalCombatComponent* Combat=Candidate->FindComponentByClass<UNavalCombatComponent>();
        if(!Combat||Combat->IsSunk())continue;
        const FVector Delta=Candidate->GetActorLocation()-Origin; const float SideDot=FVector::DotProduct(Delta.GetSafeNormal2D(),Right);
        if((Side==ENavalBroadside::Port&&SideDot>-0.25f)||(Side==ENavalBroadside::Starboard&&SideDot<0.25f))continue;
        const float DistSq=Delta.SizeSquared(); if(DistSq<BestDistSq){BestDistSq=DistSq;Best=Candidate;}
    }
    return Best;
}
bool UNavalCombatComponent::FireBroadside(ENavalBroadside Side)
{
    if(IsSunk())return false; float& Reload=Side==ENavalBroadside::Port?PortReload:StarboardReload; if(Reload>0.f)return false;
    AActor* Target=FindBroadsideTarget(Side); if(!Target)return false;
    UNavalCombatComponent* TargetCombat=Target->FindComponentByClass<UNavalCombatComponent>(); if(!TargetCombat)return false;
    const float Damage=FMath::Max(1,CannonsPerSide)*FMath::Max(0.f,DamagePerCannon);
    TargetCombat->ApplyHullDamage(Damage,GetOwner()); Reload=ReloadSeconds; SetComponentTickEnabled(true);
    OnBroadsideFired.Broadcast(Side,CannonsPerSide,Damage); return true;
}
void UNavalCombatComponent::ApplyHullDamage(float Damage,AActor*)
{
    if(Damage<=0.f||IsSunk())return; CurrentHull=FMath::Clamp(CurrentHull-Damage,0.f,MaxHull);
    if(IsSunk()&&GetOwner())GetOwner()->SetActorEnableCollision(false);
}
