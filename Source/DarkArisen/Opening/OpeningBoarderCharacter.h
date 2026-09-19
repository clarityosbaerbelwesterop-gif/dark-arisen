#pragma once
#include "CoreMinimal.h"
#include "DuelingEnemyCharacter.h"
#include "OpeningBoarderCharacter.generated.h"
class UBoardingEnemyComponent;
UCLASS() class DARKARISEN_API AOpeningBoarderCharacter : public ADuelingEnemyCharacter
{
    GENERATED_BODY()
public:
    AOpeningBoarderCharacter();
    virtual void BeginPlay() override;
    void ConfigureEncounter(FName InEncounterId);
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Opening|Boarding") TObjectPtr<UBoardingEnemyComponent> BoardingEnemy;
private:
    UPROPERTY() FName EncounterId;
    UFUNCTION() void ReportOpeningDeath(AActor* DamageCauser);
};
