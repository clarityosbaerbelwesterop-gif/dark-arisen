#include "Bosses/DravenVossBossCharacter.h"

#include "Components/CombatComponent.h"
#include "Components/HealthComponent.h"

ADravenVossBossCharacter::ADravenVossBossCharacter()
{
    BossId = TEXT("boss.draven_voss");
    MissionId = TEXT("Main.C10.04.DravenVoss");
    OutcomeKey = TEXT("Draven.Outcome");
    OutcomeValue = TEXT("Defeated");
    bCompleteMissionOnDefeat = true;

    AttackCooldownSeconds = 1.05f;
    AwarenessRangeCentimetres = 2300.f;
    AttackRangeCentimetres = 235.f;
}

void ADravenVossBossCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (HealthComponent)
    {
        HealthComponent->MaxHealth = 520.f;
        HealthComponent->CurrentHealth = 520.f;
        HealthComponent->OnHealthChanged.AddDynamic(this, &ADravenVossBossCharacter::HandleDravenHealthChanged);
    }
    if (CombatComponent)
    {
        CombatComponent->MaxPosture = 190.f;
        CombatComponent->PoiseMultiplier = 1.35f;
    }
    ApplyCombatPhase(1);
}

void ADravenVossBossCharacter::HandleDravenHealthChanged(float NewHealth, float MaximumHealth, float Delta)
{
    if (Delta >= 0.f) return;
    const float Fraction = NewHealth / FMath::Max(1.f, MaximumHealth);
    if (Fraction <= 0.30f) ApplyCombatPhase(3);
    else if (Fraction <= 0.65f) ApplyCombatPhase(2);
}

void ADravenVossBossCharacter::ApplyCombatPhase(int32 NewPhase)
{
    if (NewPhase <= CombatPhase && NewPhase != 1) return;
    CombatPhase = FMath::Clamp(NewPhase, 1, 3);

    if (CombatPhase == 1)
    {
        AttackCooldownSeconds = 1.05f;
        AwarenessRangeCentimetres = 2300.f;
        if (CombatComponent) CombatComponent->PoiseMultiplier = 1.35f;
    }
    else if (CombatPhase == 2)
    {
        AttackCooldownSeconds = 0.78f;
        AwarenessRangeCentimetres = 2500.f;
        if (CombatComponent) CombatComponent->PoiseMultiplier = 1.55f;
    }
    else
    {
        AttackCooldownSeconds = 0.58f;
        AwarenessRangeCentimetres = 2700.f;
        if (CombatComponent) CombatComponent->PoiseMultiplier = 1.75f;
    }
}
