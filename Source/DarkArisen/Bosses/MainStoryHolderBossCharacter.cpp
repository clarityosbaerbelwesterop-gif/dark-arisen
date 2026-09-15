#include "Bosses/MainStoryHolderBossCharacter.h"

#include "Components/HealthComponent.h"
#include "Story/MainStorySubsystem.h"

AMainStoryHolderBossCharacter::AMainStoryHolderBossCharacter()
{
    BossId = TEXT("boss.herrera");
    MissionId = TEXT("Main.C04.03.HerrerasFall");
    OutcomeKey = TEXT("BossResolution.boss.herrera");
}

void AMainStoryHolderBossCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (HealthComponent)
        HealthComponent->OnDied.AddDynamic(this, &AMainStoryHolderBossCharacter::HandleHolderDied);
}

void AMainStoryHolderBossCharacter::HandleHolderDied(AActor* /*DamageCauser*/)
{
    if (!GetGameInstance() || BossId.IsNone() || MissionId.IsNone()) return;
    UMainStorySubsystem* Story = GetGameInstance()->GetSubsystem<UMainStorySubsystem>();
    if (!Story || Story->GetMissionState(MissionId) != EMainMissionState::Active) return;
    if (!Story->MarkBossDefeated(BossId)) return;
    if (!OutcomeKey.IsNone() && !OutcomeValue.IsNone() && !Story->SetQuestOutcome(OutcomeKey, OutcomeValue)) return;
    if (bCompleteMissionOnDefeat) Story->CompleteAuthoredMission(MissionId);
}
