// Copyright (c) 2026 Dark Arisen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RexaSettlementAnchor.generated.h"

class USceneComponent;

/** Explicit level-authored destination for one Las Raíces schedule purpose. */
UCLASS()
class DARKARISEN_API ARexaSettlementAnchor : public AActor
{
    GENERATED_BODY()

public:
    ARexaSettlementAnchor();

    bool InitializeAuthoredAnchor(
        FName InSettlementId,
        FName InAnchorId,
        bool bInShelteredFromMiddayHeat,
        bool bInChildSafetyDestination);

    bool IsAuthoredAnchorValid() const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Anchor")
    FName SettlementId = TEXT("Rexa.LasRaices");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Anchor")
    FName AnchorId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Anchor")
    bool bShelteredFromMiddayHeat = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rexa|Anchor")
    bool bChildSafetyDestination = false;

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USceneComponent> SceneRoot;
};
