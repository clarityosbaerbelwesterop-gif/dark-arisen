#include "Combat/DamagePipeline.h"
#include "Components/CombatComponent.h"
#include "Components/HealthComponent.h"
#include "GameFramework/Actor.h"

bool UDamagePipeline::ResolveDamage(const FDamageContext& C,FDamageResult& R)
{
 R=FDamageResult();if(!IsValid(C.Source)||!IsValid(C.Target)||C.Source==C.Target||C.BaseDamage<0||C.PostureDamage<0)return false;
 auto* H=C.Target->FindComponentByClass<UHealthComponent>();auto* Combat=C.Target->FindComponentByClass<UCombatComponent>();if(!H||H->IsDead()||!Combat||!Combat->IsCombatTargetable())return false;
 if(Combat->IsInvulnerable()){R.bInvulnerable=true;return true;}
 if(Combat->IsDeflectionWindowOpen()&&C.AttackType!=EAttackType::Critical){R.bDeflected=true;if(auto* Attacker=C.Source->FindComponentByClass<UCombatComponent>())Attacker->AddPostureDamage(C.PostureDamage*1.5f);return true;}
 const float Armor=FMath::Clamp(Combat->GetArmorFraction(),0.0f,0.9f),Resistance=FMath::Clamp(Combat->GetResistance(C.AttackType),0.0f,0.9f);
 R.HealthDamage=C.BaseDamage*(1.0f-Armor)*(1.0f-Resistance);R.PostureDamage=C.PostureDamage;
 H->ApplyDamageWithRally(R.HealthDamage,C.Source,ERallyDamageClass::StandardEnemy);Combat->AddPostureDamage(R.PostureDamage);R.bStaggered=Combat->CurrentState==ECombatState::Staggered;R.bKilled=H->IsDead();return true;
}
