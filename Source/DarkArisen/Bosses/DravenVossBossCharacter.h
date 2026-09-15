#pragma once

#include "CoreMinimal.h"
#include "Bosses/MainStoryHolderBossCharacter.h"
#include "DravenVossBossCharacter.generated.h"

/** Final physical Draven encounter on the Black Deck. Reuses native dueling/posture combat. */
UCLASS()
class DARKARISEN_API ADravenVossBossCharacter : public AMainStoryHolderBossCharacter
{
    GENERATED_BODY()

public:
    ADravenVossBossCharacter();
    virtual void BeginPlay() override;

    UPROPERTY(BlueprintReadOnly, Category="Draven")
    int32 CombatPhase = 1;

private:
    UFUNCTION()
    void HandleDravenHealthChanged(float NewHealth, float Delta);
    void ApplyCombatPhase(int32 NewPhase);
};
