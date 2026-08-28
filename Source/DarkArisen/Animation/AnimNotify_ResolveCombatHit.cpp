// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#include "Animation/AnimNotify_ResolveCombatHit.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"

void UAnimNotify_ResolveCombatHit::Notify(
    USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);
    AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
    UCombatComponent* Combat = Owner
        ? Owner->FindComponentByClass<UCombatComponent>() : nullptr;
    if (Combat) Combat->ResolveQueuedMeleeHitFromAnimation(HitKind);
}

FString UAnimNotify_ResolveCombatHit::GetNotifyName_Implementation() const
{
    switch (HitKind)
    {
    case ECombatHitKind::Heavy: return TEXT("Resolve Heavy Hit");
    case ECombatHitKind::ParryStrike: return TEXT("Resolve Parry Strike");
    case ECombatHitKind::Critical: return TEXT("Resolve Critical Hit");
    default: return TEXT("Resolve Light Hit");
    }
}
