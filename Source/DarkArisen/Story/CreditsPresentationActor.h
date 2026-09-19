#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CreditsPresentationActor.generated.h"

class UCameraComponent;
class USceneComponent;
class UTextRenderComponent;

/** Native, packageable end-roll presentation used by the Alpha credits map. */
UCLASS()
class DARKARISEN_API ACreditsPresentationActor : public AActor
{
    GENERATED_BODY()

public:
    ACreditsPresentationActor();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UTextRenderComponent> CreditsText;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Credits")
    FText RollText;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Credits", meta=(ClampMin="1.0"))
    float RollDurationSeconds = 75.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Credits", meta=(ClampMin="0.0"))
    float AllowSkipAfterSeconds = 8.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Credits")
    FName ReturnMap = TEXT("L_AlphaStartup");

private:
    void FinishCredits();
    float ElapsedSeconds = 0.f;
    float InitialTextZ = 0.f;
    bool bFinished = false;
};
