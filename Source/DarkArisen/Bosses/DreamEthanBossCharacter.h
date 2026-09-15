#pragma once
#include "CoreMinimal.h"
#include "Bosses/MainStoryHolderBossCharacter.h"
#include "DreamEthanBossCharacter.generated.h"
/** Separate Chapter 9 memory-image boss; never mutates real Ethan state. */
UCLASS()
class DARKARISEN_API ADreamEthanBossCharacter : public AMainStoryHolderBossCharacter
{
    GENERATED_BODY()
public:
    ADreamEthanBossCharacter();
    virtual void BeginPlay() override;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Dream Ethan") float PhaseTwoHealthFraction=0.66f;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Dream Ethan") float PhaseThreeHealthFraction=0.33f;
    UPROPERTY(BlueprintReadOnly,Category="Dream Ethan") int32 DreamPhase=1;
private:
    UFUNCTION() void HandleDreamHealthChanged(float NewHealth,float MaximumHealth,float Delta);
    void ApplyDreamPhase(int32 NewPhase);
};
