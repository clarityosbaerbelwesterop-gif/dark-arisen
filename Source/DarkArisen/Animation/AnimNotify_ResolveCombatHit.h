// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Components/CombatComponent.h"
#include "AnimNotify_ResolveCombatHit.generated.h"

/** Authored montage contact point; the queued frame timer remains a greybox-only fallback. */
UCLASS(meta = (DisplayName = "Dark Arisen: Resolve Combat Hit"))
class DARKARISEN_API UAnimNotify_ResolveCombatHit : public UAnimNotify
{
    GENERATED_BODY()

public:
    virtual void Notify(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference) override;

    virtual FString GetNotifyName_Implementation() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    ECombatHitKind HitKind = ECombatHitKind::Light;
};
