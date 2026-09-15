#include "Bosses/DreamEthanBossCharacter.h"
#include "Components/HealthComponent.h"

ADreamEthanBossCharacter::ADreamEthanBossCharacter()
{
    BossId = TEXT("boss.dream_ethan");
    MissionId = TEXT("Main.C09.03.DreamFight");
    OutcomeKey = TEXT("DreamEthan.Outcome");
    OutcomeValue = TEXT("Accepted");
    bCompleteMissionOnDefeat = true;

    AttackCooldown = 1.25f;
    AggroRange = 1850.f;
    AttackRange = 225.f;
    AttackDamage = 18.f;
    AttackStaminaDamage = 30.f;
}

void ADreamEthanBossCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (Health)
    {
        Health->SetMaxHealth(360.f, true);
        Health->OnHealthChanged.AddDynamic(this, &ADreamEthanBossCharacter::HandleDreamHealthChanged);
    }
    ApplyDreamPhase(1);
}

void ADreamEthanBossCharacter::HandleDreamHealthChanged(float NewHealth, float Delta)
{
    if (!Health || Delta >= 0.f) return;
    const float Max = FMath::Max(1.f, Health->GetMaxHealth());
    const float Fraction = NewHealth / Max;
    if (Fraction <= PhaseThreeHealthFraction) ApplyDreamPhase(3);
    else if (Fraction <= PhaseTwoHealthFraction) ApplyDreamPhase(2);
}

void ADreamEthanBossCharacter::ApplyDreamPhase(int32 NewPhase)
{
    if (NewPhase <= DreamPhase && NewPhase != 1) return;
    DreamPhase = FMath::Clamp(NewPhase, 1, 3);
    switch (DreamPhase)
    {
        case 1:
            AttackCooldown = 1.25f;
            AttackDamage = 18.f;
            AttackStaminaDamage = 30.f;
            break;
        case 2:
            AttackCooldown = 0.92f;
            AttackDamage = 21.f;
            AttackStaminaDamage = 38.f;
            break;
        default:
            AttackCooldown = 0.70f;
            AttackDamage = 24.f;
            AttackStaminaDamage = 46.f;
            AggroRange = 2200.f;
            break;
    }
}
