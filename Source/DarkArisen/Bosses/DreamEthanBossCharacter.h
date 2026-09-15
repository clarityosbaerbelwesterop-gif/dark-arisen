#pragma once

#include "CoreMinimal.h"
#include "Bosses/MainStoryHolderBossCharacter.h"
#include "DreamEthanBossCharacter.generated.h"

/**
 * Chapter 9 memory-image boss. This is deliberately a different actor identity from real Ethan.
 * It reuses the native dueling/posture combat stack and canonical story completion path.
 */
UCLASS()
class DARKARISEN_API ADreamEthanBossCharacter : public AMainStoryHolderBossCharacter
{
    GENERATED_BODY()

public:
    ADreamEthanBossCharacter();
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dream Ethan")
    float PhaseTwoHealthFraction = 0.66f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dream Ethan")
    float PhaseThreeHealthFraction = 0.33f;

    UPROPERTY(BlueprintReadOnly, Category="Dream Ethan")
    int32 DreamPhase = 1;

private:
    UFUNCTION()
    void HandleDreamHealthChanged(float NewHealth, float Delta);

    void ApplyDreamPhase(int32 NewPhase);
};
