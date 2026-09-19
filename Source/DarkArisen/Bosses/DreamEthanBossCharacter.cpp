#include "Bosses/DreamEthanBossCharacter.h"

#include "Components/CombatComponent.h"
#include "Components/HealthComponent.h"

ADreamEthanBossCharacter::ADreamEthanBossCharacter()
{
    BossId = TEXT("boss.dream_ethan");
    MissionId = TEXT("Main.C09.03.DreamFight");
    OutcomeKey = TEXT("DreamEthan.Outcome");
    OutcomeValue = TEXT("Accepted");
    bCompleteMissionOnDefeat = true;

    AttackCooldownSeconds = 1.25f;
    AwarenessRangeCentimetres = 1850.f;
    AttackRangeCentimetres = 225.f;
}

void ADreamEthanBossCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (HealthComponent)
    {
        HealthComponent->MaxHealth = 360.f;
        HealthComponent->CurrentHealth = 360.f;
        HealthComponent->OnHealthChanged.AddDynamic(this, &ADreamEthanBossCharacter::HandleDreamHealthChanged);
    }
    if (CombatComponent)
    {
        CombatComponent->MaxPosture = 150.f;
        CombatComponent->PoiseMultiplier = 1.15f;
    }
    ApplyDreamPhase(1);
}

void ADreamEthanBossCharacter::HandleDreamHealthChanged(float NewHealth, float MaximumHealth, float Delta)
{
    if (Delta >= 0.f) return;
    const float Fraction = NewHealth / FMath::Max(1.f, MaximumHealth);
    if (Fraction <= PhaseThreeHealthFraction) ApplyDreamPhase(3);
    else if (Fraction <= PhaseTwoHealthFraction) ApplyDreamPhase(2);
}

void ADreamEthanBossCharacter::ApplyDreamPhase(int32 NewPhase)
{
    if (NewPhase <= DreamPhase && NewPhase != 1) return;
    DreamPhase = FMath::Clamp(NewPhase, 1, 3);

    if (DreamPhase == 1)
    {
        AttackCooldownSeconds = 1.25f;
        AwarenessRangeCentimetres = 1850.f;
        if (CombatComponent) CombatComponent->PoiseMultiplier = 1.15f;
    }
    else if (DreamPhase == 2)
    {
        AttackCooldownSeconds = 0.92f;
        AwarenessRangeCentimetres = 2050.f;
        if (CombatComponent) CombatComponent->PoiseMultiplier = 1.30f;
    }
    else
    {
        AttackCooldownSeconds = 0.70f;
        AwarenessRangeCentimetres = 2200.f;
        if (CombatComponent) CombatComponent->PoiseMultiplier = 1.45f;
    }
}
