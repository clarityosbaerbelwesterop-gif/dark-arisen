#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpeningBoardingEncounterActor.generated.h"
class AOpeningBoarderCharacter;
UCLASS() class DARKARISEN_API AOpeningBoardingEncounterActor : public AActor
{
    GENERATED_BODY()
public:
    AOpeningBoardingEncounterActor();
    virtual void BeginPlay() override;
    UFUNCTION(BlueprintCallable,Category="Opening|Boarding") bool StartEncounter();
    UFUNCTION(BlueprintCallable,Category="Opening|Boarding") bool SignalDravenArrived();
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Opening|Boarding") FName EncounterId=TEXT("Encounter.HarlowRaid.MainDeck");
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Opening|Boarding",meta=(ClampMin="2")) int32 RequiredBoarders=3;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Opening|Boarding") TSubclassOf<AOpeningBoarderCharacter> BoarderClass;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Opening|Boarding") TArray<FTransform> SpawnTransforms;
private:
    bool SpawnRemainingBoarders();
};
